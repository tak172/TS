/*!
	created:	2018/06/25
	created: 	11:32  25 Июнь 2018
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrNorm.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrNorm
	file ext:	cpp
	author:		 Dremin
	
	purpose:	
  
  */
 
 #include "stdafx.h"
 #include "TC_Thread.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/AInterval.h"
#include "../Augur/ProNorm.h"
#include "../Augur/TxtComment.h"

using namespace std;
typedef TechnicalTime TT;

void TC_AThread::testFndNormOccupy()
{// поиск требуемой нормативки для отправления со станции c захватом для ПЕ
	RoadKit  emptyRoad;
	std::map<EsrKit, TechnicalTime> techTimes;
	TT::Specification specification( Oddness::UNDEF, TT::Fullness::Any, emptyRoad, emptyRoad, TTF::Refrigerator );
	techTimes[EsrKit(15)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 3 ) ) );
	// однопутка, 2 пути на станции
	TC_AtopologyParams tplParams( ForkTopology::NO_FORK, stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes );
	tplParams.departureByHemPath.insert(EsrKit(15));// где искать привязку к нормативке
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20) ;
	// нормативки
	Sevent sN3(AEvent::NONE,280/*начало нити*/,100/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,8/*число станций*/) ;
	Sevent sN4(AEvent::NONE,380/*начало нити*/,100/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,7/*число станций*/) ;
	auto norm3=tpl.norma(sN3,1);
	auto norm4=tpl.norma(sN4,1);
	//реальная нить
	Sevent sEv1(AEvent::REAL,20/*начало нити*/,15/*на станции*/,20/*на перегоне*/,10,4/*нач.станция*/,1/*число станций*/) ;
	Sevent sEv2(AEvent::REAL,30/*начало нити*/,15/*на станции*/,20/*на перегоне*/,10,4/*нач.станция*/,1/*число станций*/) ;
	Strain trn1( TrainDescr( L"1005", L"1-100-2" ),	sEv1, Sevent( NO_FIXED )	);
	Strain trn2( TrainDescr( L"1007", L"1-100-2" ),	sEv2, Sevent( NO_FIXED )	);
	Sevent sEv3(AEvent::REAL,430/*начало нити*/,100/*на станции*/,20/*на перегоне*/,0,6/*нач.станция*/,1/*число станций*/) ;
	Strain trn3( TrainDescr( L"1009", L"" ),	sEv3, Sevent( NO_FIXED )	);
	Strain trn4( TrainDescr( L"1019", L"" ),	sEv3, Sevent( NO_FIXED )	);
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3,1));//ПЕ реальное
		adm.add(tpl.add(trn4,2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"2309", L""), norm3 );
		adm.addNorma(TrainDescr( L"2311", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto vl= v.getSpotLines();
		CPPUNIT_ASSERT(vl.size()==4);
		auto tr=vl.front();
		CPPUNIT_ASSERT(tr.first==trn1.tdescr);
		auto m16=fltrStatEv(16,v),m15=fltrStatEv(15,v);
		st_intervals_t st= getInterval(StationEsr(16),StationEsr(17),adm.getNsi()->stIntPtr);
		auto itN=std::find_if(norm3.begin(),norm3.end(),[](const AEvent& t){return t.Esr().getTerm()==15;});
		auto trPtr= adm.fnd(trn1);//1005
		auto trPtr9= adm.fnd(trn3);//1009
		AEvent ev= trPtr9->fndEv(16,0);
		CPPUNIT_ASSERT(itN!=norm3.end() &&  itN->diap.getEnd()<m15[1005].diap.getEnd() && "Отправление по нормативке 2309");
		CPPUNIT_ASSERT( itN->diap.getEnd()==TestProNorm::fndPreset(trPtr,AEvent(15)) && "нормативка 2309 занята под 1005");
		CPPUNIT_ASSERT(m16[1005].diap.getOrig()>ev.diap.getOrig());
		CPPUNIT_ASSERT(m16[1005].diap.getOrig()<(ev.diap.getEnd()+st.tsk.count()+  AAdmin::GapStop()) && "ждали пути на 16 станции и все равно ушли по нормативке 2309");
		itN=std::find_if(norm4.begin(),norm4.end(),[](const AEvent& t){return t.Esr().getTerm()==15;});
		CPPUNIT_ASSERT(itN!=norm4.end() && itN->diap.getEnd()==m15[1007].diap.getEnd() && "Отправление по нормативке 2311");
	}
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"2309", L""), norm3 );
		adm.addNorma(TrainDescr( L"2311", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto vl= v.getSpotLines();
		CPPUNIT_ASSERT(vl.size()==2);
		auto tr=vl.front();
		CPPUNIT_ASSERT(tr.first==trn1.tdescr);
		EsrKit esr(15);
		auto it=std::find_if(tr.second.begin(),tr.second.end(),[&esr](const AEvent& t){return t.Esr()==esr;});
		auto itN=std::find_if(norm3.begin(),norm3.end(),[&esr](const AEvent& t){return t.Esr()==esr;});
		CPPUNIT_ASSERT(itN!=norm3.end() && it!=tr.second.end() && itN->diap.getEnd()==it->diap.getEnd() && "Отправление по нормативке 2309");
		tr=vl.back();
		it=std::find_if(tr.second.begin(),tr.second.end(),[&esr](const AEvent& t){return t.Esr()==esr;});
		itN=std::find_if(norm4.begin(),norm4.end(),[&esr](const AEvent& t){return t.Esr()==esr;});
		CPPUNIT_ASSERT(itN!=norm4.end() && it!=tr.second.end() && itN->diap.getEnd()==it->diap.getEnd() && "Отправление по нормативке 2311");
	}
}

void TC_AThread::testFndProNorm()
{// поиск требуемой нормативки для отправления со станции с синонимами
	{
		INT_INT tx[]={INT_INT(1,698),INT_INT(901,998),INT_INT(5001,5998)};
		//--- Диапазоны номеров поездов используемые при прогнозировании ---
		INTERVALS  n(tx, tx+size_array(tx));// построения прогнозной нити по данным нормативного графика
		INTERVALS n1(ARRAY_IT(tx));
		CPPUNIT_ASSERT(n==n1);
		CPPUNIT_ASSERT(n.size()==3 && n.front()==AInterval(INT_INT(1,698)) && n[1]==AInterval(INT_INT(901,998)) && n[2]==AInterval(INT_INT(5001,5998)));
	}
	const TrainCharacteristics & trnChr=  TrainCharacteristics::instanceCRef();
	RoadKit  emptyRoad;
	std::map<EsrKit, TechnicalTime> techTimes;
	TT::Specification specification( Oddness::UNDEF, TT::Fullness::Any, emptyRoad, emptyRoad, TTF::Refrigerator );
	techTimes[EsrKit(15)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 3 ) ) );
	// однопутка, 3 пути на станции
	TC_AtopologyParams tplParams( ForkTopology::ONE_FORK, stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes );
	tplParams.departureByHemPath.insert(EsrKit(15));// где искать привязку к нормативке
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20) ;

	auto path10_17 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit( 17 ) ) );// получаем один маршрут

	auto path10_24 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit( 24 ) ) );// получаем один маршрут
	auto path10_180 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit(180 ) ) );// получаем один маршрут
	auto path20_180 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(20 ), EsrKit(180 ) ) );// получаем один маршрут
	CPPUNIT_ASSERT(path20_180.size()==21 && path10_180.size()==29 &&path10_24.size()==29 && path10_17.size()==15   );
	// нормативки
	Sevent sN1(AEvent::NONE,70/*начало нити*/,100/*на станции*/,20/*на перегоне*/,0,4/*+10 нач.станция*/,4/*число станций*/) ;
	Sevent sN2(AEvent::NONE,260/*начало нити*/,100/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,4/*число станций*/) ;
	Sevent sN3(AEvent::NONE,280/*начало нити*/,100/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,8/*число станций*/) ;
	Sevent sN4(AEvent::NONE,300/*начало нити*/,100/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,7/*число станций*/) ;
	auto norm1=tpl.norma(sN1,1);
	auto norm2=tpl.norma(sN2,1);
	auto norm3=tpl.norma(sN3,1);
	auto norm4=tpl.norma(sN4,1);
	for ( auto& t: norm4)
		if(t.Esr().getTerm()>17)
			t.resetEsr(EsrKit(t.Esr().getTerm()-18 +25));// перевод на другую ветвь
	//реальная нить
	Sevent sEv(AEvent::REAL,20/*начало нити*/,15/*на станции*/,20/*на перегоне*/,0,4/*нач.станция*/,1/*число станций*/) ;
	Strain trn( TrainDescr( L"1005", L"1-100-2" ),	sEv, Sevent( NO_FIXED )	);

	TestAAdmin adm;
	adm.add(tpl.add(trn));//ПЕ реальное
	adm.addNorma(TrainDescr( L"3601", L""), norm1 );
	adm.addNorma(TrainDescr( L"9703", L""), norm2 );
	adm.addNorma(TrainDescr( L"2309", L""), norm3 );
	adm.addNorma(TrainDescr( L"2311", L""), norm4 );
	ProNorm::SET_SYNON s;
	s.insert(StationEsr(16));
	ProNorm::addSynonim(s);// все наборы синонимов
	{
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto vl= v.getSpotLines();
		CPPUNIT_ASSERT(vl.size()==1 );
		auto vEv=vl.front().second;
		time_t dep15=0;
		for (const auto& t: vEv)
			if(t.Esr()==EsrKit(15))
				dep15=t.diap.getEnd();
		CPPUNIT_ASSERT(dep15 && " проходим 15 станцию");
		for (const auto& t: norm1)
			if(t.Esr()==EsrKit(15))
				CPPUNIT_ASSERT(t.diap.getEnd()==dep15 && " идем по ближайшей полностью эквив.нормативке c учетом станций-направлений");
	}

	ProNorm::clearSynonim();// все наборы синонимов
	auto v=adm.forecastGlobal();
	auto vl= v.getSpotLines();
	CPPUNIT_ASSERT(vl.size()==1 );
	auto vEv=vl.front().second;
	time_t dep15=0;
	for (const auto& t: vEv)
		if(t.Esr()==EsrKit(15))
			dep15=t.diap.getEnd();
	CPPUNIT_ASSERT(dep15 && " проходим 15 станцию");
	for (const auto& t: norm3)
		if(t.Esr()==EsrKit(15))
			CPPUNIT_ASSERT(t.diap.getEnd()==dep15 && " идем по макс.совпадающей нормативке");
}

void TC_AThread::testOutrunTechStation()
{// обгон попутных на тех.станции при отборе нормативки 
	TRACE("\r\n =========== testOutrunTechStation ============");
	Sevent sEv1001(//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		10, // время на станции
		0,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent sEv1003(//реальная нить
		AEvent::REAL,// тип событий
		100,// начало нити
		10, // время на станции
		0,// время на перегоне
		0,// время на последнем перегоне
		3, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent tNrm1(AEvent::NONE,// тип событий
		290,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		5, // нач.станция (+10 в конструкторе)
		5// число станций
		);
	Sevent tNrm2(AEvent::NONE,// тип событий
		320,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		5, // нач.станция (+10 в конструкторе)
		5// число станций
		);
	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
	Strain trn1003( TrainDescr( L"1003", L"1-100-2"),sEv1003, Sevent( NO_FIXED ));
	TechnicalTime::Specification spec= TechnicalTime::MakeSpecification(trn1001.tdescr);
	long tblSt[]={15};
	map<EsrKit, TechnicalTime> techTimes;
	techTimes[EsrKit(15)].AddTime( TechnicalTime::SpecifiedTime( spec, TechnicalTime::Minutes( 1 ) ) );
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)1.0);
	tpl.setPvx(20,&defTimes);
	auto norm1=tpl.norma(tNrm1,1);
	auto norm2=tpl.norma(tNrm2,1);

	{// без решения попутных обгонов
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::checkOutrunTechStation=false;// проверять обгоны на тех.станциии
		adm.add(tpl.add(trn1003));//ПЕ реальное
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1001", L""), norm1 );
		adm.addNorma(TrainDescr( L"1003", L""), norm2 );
		// добавление  тех.станций
		for (const auto& t: tblSt)
		{
			LONG_VECTOR lv(1,t);
			lv.push_back(t+1);
			ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
			CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(1) && "Установлена тех.станция");
		}

		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto allNrm=adm.getNorm();
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		M_INT_EV n15=fltrStatEv(15,allNrm);
		CPPUNIT_ASSERT( m15.size()==2 && n15.size()==2);
		CPPUNIT_ASSERT(m15[1001].diap.duration()>60 && m15[1003].diap.duration()>60);
		CPPUNIT_ASSERT(m15[1001].diap.enclose(m15[1003].diap));
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()== n15[1003].diap.getEnd());
		CPPUNIT_ASSERT(m15[1003].diap.getEnd()== n15[1001].diap.getEnd());
	}
	{// без решения попутных обгонов
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::checkOutrunTechStation=true;// проверять обгоны на тех.станциии
		adm.add(tpl.add(trn1003));//ПЕ реальное
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1001", L""), norm1 );
		adm.addNorma(TrainDescr( L"1003", L""), norm2 );
		// добавление  тех.станций
		for (const auto& t: tblSt)
		{
			LONG_VECTOR lv(1,t);
			lv.push_back(t+1);
			ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
			CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(1) && "Установлена тех.станция");
		}

		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto allNrm=adm.getNorm();
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		M_INT_EV n15=fltrStatEv(15,allNrm);
		CPPUNIT_ASSERT( m15.size()==2 && n15.size()==2);
		CPPUNIT_ASSERT(m15[1001].diap.duration()>60 && m15[1003].diap.duration()>60);
		CPPUNIT_ASSERT(m15[1001].diap.enclose(m15[1003].diap.getOrig()));
		CPPUNIT_ASSERT(m15[1003].diap.enclose(m15[1001].diap.getEnd()));
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()== n15[1001].diap.getEnd());
		CPPUNIT_ASSERT(m15[1003].diap.getEnd()== n15[1003].diap.getEnd());
	}
	{// с решением попутных обгонов
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по обгонам попутных
		adm.add(tpl.add(trn1003));//ПЕ реальное
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1001", L""), norm1 );
		adm.addNorma(TrainDescr( L"1003", L""), norm2 );
		// добавление  тех.станций
		for (const auto& t: tblSt)
		{
			LONG_VECTOR lv(1,t);
			lv.push_back(t+1);
			ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
			CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(1) && "Установлена тех.станция");
		}

		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto allNrm=adm.getNorm();
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		M_INT_EV n15=fltrStatEv(15,allNrm);
		CPPUNIT_ASSERT( m15.size()==2 && n15.size()==2);
		CPPUNIT_ASSERT(m15[1001].diap.duration()>60 && m15[1003].diap.duration()>60);
		CPPUNIT_ASSERT(m15[1001].diap.getOrig()<m15[1003].diap.getOrig());
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()<m15[1003].diap.getEnd());
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()== n15[1001].diap.getEnd());
		CPPUNIT_ASSERT(m15[1003].diap.getEnd()== n15[1003].diap.getEnd());
	}
}

void TC_AThread::testFixTechSt()
{// фиксация прогноза на тех.станции
	TRACE("\r\n =========== testFixTechSt ============");
	Sevent sEv1(AEvent::REAL,20,10, /* время на станции*/0,	0,2 /* нач.станция (+10 в конструкторе)*/,	1);// число станций
	Sevent sEv2(AEvent::REAL,200,10, /* время на станции*/0,	0,5 /* нач.станция (+10 в конструкторе)*/,	1);// число станций

	Sevent fEv1(AEvent::FIXED,215/*начало нити*/,5/*время на станции*/,	20/*время на перегоне*/,1/*признак событий на перегоне*/,
		5 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Strain trn( TrainDescr( L"1001", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trnF( TrainDescr( L"1001", L"1-100-2"),sEv1, fEv1);
	Strain trn2( TrainDescr( L"1001", L"1-100-2"),sEv2, fEv1);
	TechnicalTime::Specification spec= TechnicalTime::MakeSpecification(trn.tdescr);
	long tblSt[]={15};
	map<EsrKit, TechnicalTime> techTimes;
	techTimes[EsrKit(15)].AddTime( TechnicalTime::SpecifiedTime( spec, TechnicalTime::Minutes( 1 ) ) );
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
 	{// без фиксации
 		TestAAdmin adm;
 		adm.add(tpl.add(trn));//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
 		M_INT_EV m15= fltrStatEv(15,trFrc);
 		CPPUNIT_ASSERT(m15[1001].diap.duration()==techTimes[EsrKit(15)].GetTime(spec)->count()*60);
 	}
	{// с фиксацией
		TestAAdmin adm;
		adm.add(tpl.add(trnF,1,TC_Atopology::END));//ПЕ реальное
		auto trn= adm.fnd(trnF.tdescr);
		AEvent evFix=trn->fndEv(EsrKit(15));
		CPPUNIT_ASSERT(!evFix.empty());
		CPPUNIT_ASSERT(evFix.getSourceExt()==AEvent::FIXED);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==1 && "Исключение должно быть.");
		CPPUNIT_ASSERT(adm.getClash().front()->Comment()==	ERROR_LNK_FIX_FRC);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		auto evId= trn->getIdeal(EsrKit(15));
		CPPUNIT_ASSERT(m15[1001].diap.getOrig()==evId->diap.getOrig());
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()==evFix.diap.getEnd());
		CPPUNIT_ASSERT(evId->diap.getEnd()!=evFix.diap.getEnd());
		CPPUNIT_ASSERT(evId->diap.getOrig()!=evFix.diap.getOrig());
	}
	{// с фиксацией в начале
		TestAAdmin adm;
		adm.add(tpl.add(trn2,1,TC_Atopology::END));//ПЕ реальное
		auto trn= adm.fnd(trn2.tdescr);
		AEvent evFix=trn->fndEv(EsrKit(15));
		CPPUNIT_ASSERT(!evFix.empty());
		CPPUNIT_ASSERT(evFix.getSourceExt()==AEvent::FIXED);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==1 && "Исключение должно быть.");
		CPPUNIT_ASSERT(adm.getClash().front()->Comment()==	ERROR_LNK_FIX_FRC);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()==evFix.diap.getEnd());
		CPPUNIT_ASSERT(m15[1001].diap.getOrig()==trn->origExactFrc());
		CPPUNIT_ASSERT(m15[1001].diap.getOrig()!=evFix.diap.getOrig());
	}
}