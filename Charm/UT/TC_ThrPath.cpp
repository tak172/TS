/*!
	created:	2018/05/31
	created: 	20:17  31 Май 2018
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrPath.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrPath
	file ext:	cpp
	author:		 Dremin
	
	purpose:	подбор путей для ПЕ
  
  */
 
 #include "stdafx.h"
 #include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Augur/AStatWayLocVec.h"
#include "TC_statPathProp.h"

void TC_AThread::testEmptyStationPathStop()
{//тест  c отсутствием  путей для стоянки на станции 
	TRACE("\r\n =========== testEmptyStationPathStop ============");
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	120/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1001(AEvent::REAL ,25/*начало нити*/,	120/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		4/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1003(AEvent::REAL ,10/*начало нити*/,999/*время на станции*/,100/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent tNrm(AEvent::NONE,// тип событий
		40+185,// начало нити
		10,// время на станции
		40,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция
		6// число станций
		);
	Strain trn1( TrainDescr( L"2", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"2002", L"2-100-1"),sEv1001, Sevent( NO_FIXED ));// грузовое ПЕ
	Strain trn1003( TrainDescr( L"2004", L""),sEv1003, Sevent( NO_FIXED ));// грузовое ПЕ
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 2 пути на станции
		//------------ на 12 стоянка запрещена -------------------
		tplParams.add(12,1,TC_StatPath(noStopPr));
		tplParams.add(12,2,TC_StatPath(noStopPr));
		TC_Atopology tpl( tplParams );
		auto norm=tpl.norma(tNrm,2);
		tpl.setPvx(60);
		TestAAdmin adm;
		adm.addNorma( trn1.tdescr, norm );
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.add(tpl.add(trn1003,1));//ПЕ реальное
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tf=trFrc.getSpotLines();
		auto it=std::find_if(tf.begin(),tf.end(),[&trn1001](const std::pair<TrainDescr, V_EVNT>& t){return t.first==trn1001.tdescr;});
		CPPUNIT_ASSERT(it!=tf.end());
		V_EVNT startStop;
		std::for_each(it->second.begin(),it->second.end(),[&startStop](const AEvent& ev){
			if(!ev.onSpan() && ev.getType()==AEvent::STOP_START)
				startStop.push_back(ev);});
			CPPUNIT_ASSERT(startStop.size()==2 && startStop.back().Esr()==EsrKit(13) && "ожидание на 13 (на 12 нет путей для стоянки)");
	}
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 2 пути на станции
		TC_Atopology tpl( tplParams );
		auto norm=tpl.norma(tNrm,2);
		tpl.setPvx(60);
		TestAAdmin adm;
		adm.addNorma( trn1.tdescr, norm );
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.add(tpl.add(trn1003,1));//ПЕ реальное
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tf=trFrc.getSpotLines();
		auto it=std::find_if(tf.begin(),tf.end(),[&trn1001](const std::pair<TrainDescr, V_EVNT>& t){return t.first==trn1001.tdescr;});
		CPPUNIT_ASSERT(it!=tf.end());
		V_EVNT startStop;
		std::for_each(it->second.begin(),it->second.end(),[&startStop](const AEvent& ev){
			if(!ev.onSpan() && ev.getType()==AEvent::STOP_START)
				startStop.push_back(ev);});
			CPPUNIT_ASSERT(startStop.size()==2 && startStop.back().Esr()==EsrKit(12) && "ожидание на 12");
	}
}

void TC_AThread::testStationPath()
{//тест подбора путей на станции  для событий с ПЕ
	TRACE("\r\n =========== testStationPath ============");
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	120/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		5/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1001(AEvent::REAL ,25/*начало нити*/,	120/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1003(AEvent::REAL ,310/*начало нити*/,	5/*время на станции*/,100/*время на перегоне*/,0/*время на последнем перегоне*/,
		2/*нач.станция (+10 в конструкторе)*/,2/*число станций*/);
	Sevent tNrm(AEvent::NONE,// тип событий
		20,// начало нити
		10,// время на станции
		60,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция
		4// число станций
		);
	Strain trn1( TrainDescr( L"2", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"2002", L"2-100-1"),sEv1001, Sevent( NO_FIXED ));// грузовое ПЕ
	Strain trn1003( TrainDescr( L"2004", L""),sEv1003, Sevent( NO_FIXED ));// грузовое ПЕ
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 2 пути на станции
		//------------ на 14 выбор для стоянки с выгр/погр пассажиров ПЕ -------------------
		tplParams.add(14,6,TC_StatPath(passStopInOutPr));
		tplParams.add(14,5,TC_StatPath(allPr));
		tplParams.add(14,2,TC_StatPath(passStopPr));
		tplParams.add(14,1,TC_StatPath(noStopPr));
		//------------ на 12 стоянка выбор для пассажирского ПЕ -------------------
		tplParams.add(12,8,TC_StatPath(passStopInOutPr));// пасс+ посадка/высадка
		tplParams.add(12,7,TC_StatPath(passStopPr));//пасс
		tplParams.add(12,2,TC_StatPath(passMovePr));//безостановочный пропуск
		tplParams.add(12,5,TC_StatPath(allPr));// нет атр
		tplParams.add(12,6,TC_StatPath(noStopPr));// гл + безост
		TC_Atopology tpl( tplParams );
		auto norm=tpl.norma(tNrm,2);
		tpl.setPvx(60);
		TestAAdmin adm;
		adm.addNorma( trn1.tdescr, norm );
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn1003));//ПЕ реальное
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tPass= adm.fnd(trn1);
		CPPUNIT_ASSERT(tPass);
		AEvent evStop= tPass->fndEv(EsrKit(12));
		AEvent evStopInOut= tPass->fndEv(EsrKit(14));
		CPPUNIT_ASSERT(evStop.getType()==AEvent::MOVE && evStop.Num()==6 && "гл + безост");
		CPPUNIT_ASSERT(evStopInOut.getType()==AEvent::STOP_START && evStopInOut.Num()==6 && "стоянка с погр/выгр пассажиров  ПЕ");
	}
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 2 пути на станции
		//------------ на 11 ходом выбор для грузового ПЕ -------------------
		tplParams.add(11,5,TC_StatPath(allPr));
		tplParams.add(11,2,TC_StatPath(passStopPr));
		tplParams.add(11,1,TC_StatPath(noStopPr));
		//------------ на 12 стоянка выбор для грузового ПЕ -------------------
		tplParams.add(12,1,TC_StatPath(passStopPr));
		tplParams.add(12,2,TC_StatPath(passMovePr));
		tplParams.add(12,5,TC_StatPath(allPr));
		tplParams.add(12,6,TC_StatPath(noStopPr));
		TC_Atopology tpl( tplParams );
		tpl.setPvx(60);
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn1001));//ПЕ реальное
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tGr= adm.fnd(trn1001);
		auto tPass= adm.fnd(trn1);
		auto pvx= tGr->getPMT(11,12);
		CPPUNIT_ASSERT(tGr && tPass);
		AEvent ev12= tGr->fndEv(EsrKit(12));
		AEvent ev11= tGr->fndEv(EsrKit(11));
		AEvent evMoveP= tPass->fndEv(EsrKit(11));
		CPPUNIT_ASSERT(ev12.getType()==AEvent::MOVE && ev12.Num()==6 && "безост.проезд");
		CPPUNIT_ASSERT(tGr->fndEv(EsrKit(13,12)).diap.duration()==pvx.full+pvx.start );
		CPPUNIT_ASSERT(ev11.getType()==AEvent::MOVE && ev11.Num()==1 && "проход грузового ПЕ");
		CPPUNIT_ASSERT(evMoveP.getType()==AEvent::MOVE && evMoveP.Num()==1 && "проход пасс ПЕ");
	}
}

void TC_AThread::testStationPathEndNorm()
{//тест  c подбором пути на станции в конце маршрута по нормативке
	Sevent sEv2(AEvent::REAL ,5/*начало нити*/,	120/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		5/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent tNrm(AEvent::NONE,// тип событий
		20,// начало нити
		10,// время на станции
		60,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция
		5// число станций
		);
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 2 пути на станции
	//------------ на 12 ходом выбор для грузового ПЕ -------------------
	tplParams.add(12,1,TC_StatPath( pMaj));//гл 
	tplParams.add(12,2,TC_StatPath(allPr));
//		tplParams.add(12,4,TC_StatPath(passMaj));//пасс+ гл 
	{
		TC_Atopology tpl( tplParams );
		auto norm=tpl.norma(tNrm,2);
		norm.back().setType(AEvent::STOP);
		norm.back().diap.setEnd(norm.back().diap.getOrig());
		TestAAdmin adm;
		adm.addNorma( trn2.tdescr, norm );
		adm.add(tpl.add(trn2));//ПЕ реальное
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v=trFrc.getSpotLines();
		CPPUNIT_ASSERT(v.size()==1);
		auto ev=v.back().second.back();
		CPPUNIT_ASSERT(ev.Esr()==EsrKit(12) && ev.getType()==AEvent::STOP);
		CPPUNIT_ASSERT(ev.Esr()==EsrKit(12) && ev.Num()==2 && " Путь 2 без ограничений ");
	}
	{
		tplParams.add(12,5,TC_StatPath(passStopInOutPr));// пасс+ не гл.+ платформа
		TC_Atopology tpl( tplParams );
		auto norm=tpl.norma(tNrm,2);
		norm.back().setType(AEvent::STOP);
		norm.back().diap.setEnd(norm.back().diap.getOrig());
		TestAAdmin adm;
		adm.addNorma( trn2.tdescr, norm );
		adm.add(tpl.add(trn2));//ПЕ реальное
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v=trFrc.getSpotLines();
		CPPUNIT_ASSERT(v.size()==1);
		auto ev=v.back().second.back();
		CPPUNIT_ASSERT(ev.Esr()==EsrKit(12) && ev.Num()==5 && " Путь для пасс.ПЕ");
	}
}
 
typedef ADProperties::Attrubute ATTR;

static std::set<ATTR> bldAttr(const std::string& attr)
{
	struct Lnk{	char c;
		ATTR a;};
	static Lnk tbl[]={
		'P',ATTR::PASSENGER,
		'M',ATTR::MAJOR,
		'E',ATTR::EMBARKATION,
		'N',ATTR::NON_STOP_PASSING,
		'B',ATTR::BID_CARGO};
	static std::vector<Lnk> v(ARRAY_IT(tbl));
	std::set<ATTR> res;
	for (const auto& t: attr)
	{
		auto it=std::find_if(v.begin(),v.end(),[t](const Lnk&l){return l.c==t;});
		if(it!=v.end())
			res.insert(it->a);
	}
	return res;
}

static ADProperties bld(const std::string& attr,Oddness d)
{
	ADProperties res;
	auto s=	bldAttr(attr);
	for (const auto& t: s)
		res.SetAttribute(t);
	res.SetPassedTrainOddness(d);
	return res;
}

struct SSec{
	std::string pr;
	Oddness d;
	unsigned int even;
	unsigned int odd;
	SSec(const char* p, int n=0,unsigned int ev=0,unsigned int od=0 ):pr(p?p:""),d(Oddness::UNDEF)
		,even(ev),odd(od)
	{
		if(n==2)
			d=Oddness::EVEN;
		if(n==1)
			d=Oddness::ODD;
	}
};

static std::vector<StatWayInfo::Section> bldS(const std::vector<SSec>& v)
{
	std::vector<StatWayInfo::Section> res;
	for (const auto& t: v)
	{
		ADProperties p=bld(t.pr,t.d);
		res.emplace_back(L"",t.even, t.odd, p);
	}
	return res;
}

void TC_AThread::testSortSecPath()
{// тест сортировки  путей посекционно 
		static SSec tbl1[]={
			SSec("PMENB"),SSec("MN",2,1,1),SSec("PME",1,1,1),SSec("MN",0,1,1) 
			,SSec("PME",1,1,2),SSec("PME",1,1,1),SSec("PME",1,0,1)
		};
		static SSec tblUniq1[]={
			/*SSec("PMENB"),*/SSec("MN",2,1,1),SSec("PME",1,1,1),SSec("MN",0,1,1) 
			,SSec("PME",1,1,2)/*,SSec("PME",1,1,1)*/,SSec("PME",1,0,1)
		};
		static SSec tbl2[]={
			SSec("PMENB"),SSec("MN",2),SSec("PME",1),SSec("MN",0) 
			,SSec("PME",0),SSec("PME",1),SSec("PME",0)
		};
		static SSec tblUniq2[]={
			SSec("PMENB"),SSec("MN",2),SSec("PME",1),SSec("MN",0) 
			,SSec("PME",0)/*,SSec("PME",1),SSec("PME",0)*/
		};
		
	struct TMP{
		std::vector<SSec> or;
		std::vector<SSec> un;
	}tblT[]=
	{ std::vector<SSec>(ARRAY_IT(tbl1)),std::vector<SSec>(ARRAY_IT(tblUniq1)),
	  std::vector<SSec>(ARRAY_IT(tbl2)),std::vector<SSec>(ARRAY_IT(tblUniq2))
	};
	std::vector<TMP>vTMP(ARRAY_IT(tblT));
	for (const auto& t: vTMP)
	{
		StatWayInfoCPtr way(new StatWayInfo(1,1,bldS(t.or)));
		V_SEC v= getSignSections(way,0);// получить значимые уникальные описания секций пути с длиной не меньше sz
		CPPUNIT_ASSERT(!v.empty() && v.size()==t.un.size());
		auto vS=bldS(t.un);
		for (const auto& t: vS)
		{
			auto it=std::find_if(v.begin(),v.end(),[&t](const StatWayInfo::Section& s)
			{
				return t.Capacity(Oddness::EVEN)==s.Capacity(Oddness::EVEN)&& 
					t.Capacity(Oddness::ODD)==s.Capacity(Oddness::ODD)&& 
					t.GetADProperties()==s.GetADProperties();
			});
			CPPUNIT_ASSERT(it!=v.end());
		}
	}
}

void TC_AThread::testStopStationToOutrun()
{// тест подбора станции торможения при задании на обгон
	TRACE("\r\n =========== testStopStationToOutrun ============");

	ADProperties::Attrubute tbl[]={	ADProperties::Attrubute::NON_STOP_PASSING, //безостановочный пропуск
		ADProperties::Attrubute::PASSENGER};
	ADProperties passNoStop( ARRAY_IT( tbl ),Oddness::UNDEF,WeightNorms());// для просл. не гл , не пасс и  безост

	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 2 пути на станции

	//------------ на 18 нет стоянки ПЕ для 2 -------------------
	Sevent sEv2(//реальная нить
		AEvent::REAL,// тип событий
		5,// начало нити
		5, // время на станции
		30,// время на перегоне
		0,// время на последнем перегоне
		9, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent sEv2001(//реальная нить
		AEvent::REAL,// тип событий
		2,// начало нити
		2, // время на станции
		20,// время на перегоне
		10,// время на последнем перегоне
		6, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent sEv2004(//реальная нить
		AEvent::REAL,// тип событий
		22,// начало нити
		5, // время на станции
		0,// время на перегоне
		0,// время на последнем перегоне
		7, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent tNrm(AEvent::NONE,// тип событий
		20,// начало нити
		1,// время на станции
		35,// время на перегоне
		0,// время на последнем перегоне
		9, // нач.станция
		4// число станций
		);
	TestAAdmin::fnFltr.clear();// ф-ция  отбора заданий по обгонам
	TrainDescr d2( L"2", L"");
	d2.SetRelLength(5);//длина ПЕ
	Strain trn2( d2,sEv2, Sevent( NO_FIXED ));
	Strain trn2001( TrainDescr( L"2001", L"1-100-2"),sEv2001, Sevent( NO_FIXED ));
	Strain trn2004( TrainDescr( L"2004", L"1-100-1"),sEv2004, Sevent( NO_FIXED ));
	st_intervals_t defTimes(1.f);
	defTimes.tsk=std::chrono::seconds(10) ;
	{
 		tplParams.add(18,1,TC_StatPath(passNoStop,6));
 		TC_Atopology tpl( tplParams );
 		tpl.setPvx(30);
 		tpl.setStInt(defTimes);
 		auto norm2=tpl.norma(tNrm,2);
 		TestAAdmin adm;
 		TestAAdmin::fnFltr.push_back(nullptr);// ф-ция  отбора заданий по обгонам по исключениям
 		adm.useBackAdjust=true;
 		adm.add(tpl.add(trn2));
 		adm.add(tpl.add(trn2001));//ПЕ реальное
 		adm.add(tpl.add(trn2004,1));//ПЕ реальное
 		adm.addNorma( trn2.tdescr, norm2 );
 		ACounterInt::limit=12;
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		auto clash=adm.getClash();
 		CPPUNIT_ASSERT(clash.empty());
 		M_INT_EV m=fltrStatEv(19,trFrc),m18=fltrStatEv(18,trFrc),m17=fltrStatEv(17,trFrc);
 		CPPUNIT_ASSERT( m18[2].diap.duration()>20 && " конфликт на 17 станции ликвидирован на 18") ;
 		CPPUNIT_ASSERT(m[2].getType()==AEvent::STOP_START && m18[2].getType()==AEvent::STOP_START && "На 18 станции можно останавливаться ПЕ2");
 		CPPUNIT_ASSERT( !m17[2].diap.intersect(m17[2004].diap)) ;
	}
	{
		tplParams.add(18,1,TC_StatPath(passNoStop,3));
		tplParams.add(18,2,TC_StatPath(allPr,1));
		TC_Atopology tpl( tplParams );
		tpl.setPvx(30);
		tpl.setStInt(defTimes);
		auto norm2=tpl.norma(tNrm,2);
		TestAAdmin adm;
		TestAAdmin::fnFltr.push_back(nullptr);// ф-ция  отбора заданий по обгонам по исключениям
		adm.add(tpl.add(trn2));
		adm.add(tpl.add(trn2001));//ПЕ реальное
		adm.add(tpl.add(trn2004,1));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.empty());
		M_INT_EV m19=fltrStatEv(19,trFrc),m18=fltrStatEv(18,trFrc),m17=fltrStatEv(17,trFrc);
		CPPUNIT_ASSERT( m18[2].getType()==AEvent::MOVE && "На 18 станции нельзя останавливаться ПЕ2");
		CPPUNIT_ASSERT( m19[2].getType()==AEvent::STOP_START && " конфликт на 17 станции ликвидирован стоянкой на 19");
		CPPUNIT_ASSERT( m18[2].diap.getEnd()>m18[2001].diap.getOrig()+ defTimes.tsk.count()+ AAdmin::GapStop()+20) ;	
		CPPUNIT_ASSERT( m18[2].diap.getOrig()+ defTimes.tsk.count()<=m18[2001].diap.getEnd());		
		CPPUNIT_ASSERT( m18[2].diap.getOrig()+ defTimes.tsk.count()+ AAdmin::GapStop()>=m18[2001].diap.getEnd());
	}
	{// обратное  согласование
		tplParams.add(18,1,TC_StatPath(passNoStop,3));
		tplParams.add(18,2,TC_StatPath(allPr,1));
		TC_Atopology tpl( tplParams );
		tpl.setPvx(30);
		tpl.setStInt(defTimes);
		auto norm2=tpl.norma(tNrm,2);
		TestAAdmin adm;
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
		TestAAdmin::fnFltr.push_back(nullptr);// ф-ция  отбора заданий по обгонам по исключениям
		adm.add(tpl.add(trn2));
		adm.add(tpl.add(trn2001));//ПЕ реальное
		adm.add(tpl.add(trn2004,1));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.empty());
		M_INT_EV m19=fltrStatEv(19,trFrc),m18=fltrStatEv(18,trFrc),m17=fltrStatEv(17,trFrc);
		CPPUNIT_ASSERT( m18[2].getType()==AEvent::MOVE && "На 18 станции нельзя останавливаться ПЕ2");
		CPPUNIT_ASSERT( m19[2].getType()==AEvent::STOP_START && " конфликт на 17 станции ликвидирован стоянкой на 19");
		CPPUNIT_ASSERT( m18[2].diap.getEnd()>=m18[2001].diap.getOrig()+ defTimes.tsk.count()) ;	
		CPPUNIT_ASSERT( m18[2].diap.getEnd()<=m18[2001].diap.getOrig()+ defTimes.tsk.count()+ AAdmin::GapStop()) ;	
		CPPUNIT_ASSERT( m18[2].diap.getOrig()+ defTimes.tsk.count()<=m18[2001].diap.getEnd());		
		CPPUNIT_ASSERT( m18[2].diap.getOrig()+ defTimes.tsk.count()+ AAdmin::GapStop()>=m18[2001].diap.getEnd());
	}
}
