/*!
	created:	2017/08/29
	created: 	16:56  29 Август 2017
	filename: 	D:\potapTrunk\Charm\UT\TC_ThrdConfl.cpp
	file path:	D:\potapTrunk\Charm\UT
	file base:	TC_ThrdConfl
	file ext:	cpp
	author:		 Dremin
	
	purpose:	проверка разрешения конфликтов  
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Augur/Occupancy.h"
#include "../Augur/RailLimitation.h"
#include "../Augur/TrainRoutes.h"
#include "TestAAdmin.h"
#include "TestPossessions.h"
#include "../Augur/ALogWrn.h"

void TC_AThread::testTopInterval()
{	// тест учета Top (Станционный интервал неодновременного отправления и попутного прибытия, для станций, где пропуск по главному пути осуществляется без остановки)
	TRACE("\r\n =========== testTopInterval ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes;
	defTimes.Top = std::chrono::minutes(3);
	defTimes.tpr = std::chrono::seconds(4);
	defTimes.tot = std::chrono::seconds(4);
	defTimes.tp = std::chrono::seconds(4);
	defTimes.tbsk = std::chrono::seconds(4);
	defTimes.tnp = std::chrono::seconds(14);
	defTimes.tn = std::chrono::seconds(31);
	defTimes.tsk = std::chrono::seconds(3);
	defTimes.tpbo = std::chrono::seconds(11);
	defTimes.Ipr = std::chrono::seconds(13);
	defTimes.Iot = std::chrono::seconds(18);
	defTimes.Ipk = std::chrono::seconds(23);
	tpl.setPvx(20,&defTimes);
	
// 	tpl.setPvx(20);
	Sevent sEv2(//реальная нить
		AEvent::REAL,// тип событий
		5,// начало нити
		10, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent sEv5003(//реальная нить
		AEvent::REAL,// тип событий
		5,// начало нити
		10, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		5, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	Strain trn5003( TrainDescr( L"5003", L""),sEv5003, Sevent( NO_FIXED ));
 
	Sevent tNrm2(AEvent::NONE/*тип событий*/,20/*начало нити*/,1000/* время на станции*/,20/* время на перегоне*/,0/*время на последнем перегоне*/
		,6/*нач.станция*/,2/* число станций*/);
	Sevent tNrm5003(AEvent::NONE/*тип событий*/	,40/* начало нити*/,20/* время на станции*/,20/* время на перегоне*/,0/*время на последнем перегоне*/
		,5/*нач.станция*/,3/* число станций*/);
	auto norm2=tpl.norma(tNrm2,2);
	auto norm5003=tpl.norma(tNrm5003,1);
	{
		Sevent sEv1(//реальная нить
			AEvent::REAL,// тип событий
			5,// начало нити
			10, // время на станции
			20,// время на перегоне
			0,// время на последнем перегоне
			6, // нач.станция (+10 в конструкторе)
			1// число станций
			);
		Strain trn1( TrainDescr( L"1", L""),sEv1, Sevent( NO_FIXED ));
		Sevent tNrm1(AEvent::NONE/*тип событий*/	,28/* начало нити*/,50/* время на станции*/,20/* время на перегоне*/,0/*время на последнем перегоне*/
			,6/*нач.станция*/,3/* число станций*/);
		auto norm1=tpl.norma(tNrm1,1);

		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn5003));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное

		adm.addNorma( trn1.tdescr, norm1 );
		adm.addNorma( trn5003.tdescr, norm5003 );
		adm.addNorma( trn2.tdescr, norm2 );

		st_intervals_t st= getInterval(StationEsr(16),StationEsr(17),adm.getNsi()->stIntPtr);
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		AEvent ev1,ev3;
		for (const auto& t: trFrc0.getSpotLines())
			switch (t.first.GetNumber().getNumber())
		{
			case 1:
				ev1=*std::find_if(t.second.begin(),t.second.end(),[](const AEvent& ev){return ev.Esr()==EsrKit(16);});
				break;
			case 5003:
				ev3=*std::find_if(t.second.begin(),t.second.end(),[](const AEvent& ev){return ev.Esr()==EsrKit(16);});
				break;
		}

		CPPUNIT_ASSERT(!ev1.empty() && !ev3.empty());
		CPPUNIT_ASSERT(abs(ev1.diap.getEnd()+ st.Top.count()-ev3.diap.getOrig())<5 && "Станционный интервал неодновременного отправления и попутного прибытия");
	}
}
 
void TC_AThread::testBlockPost()
{// тест На блокпостах необходимо прогнозировать только безостановочный пропуск (проследование).
	TRACE("\r\n =========== testBlockPost ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	tplParams.setDetalization( StationEsr(12), TC_AtopologyParams::StationDetalization( NsiBasis::StationKind::Blockpost ) );
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.Ipk=std::chrono::seconds(20);
	defTimes.Iot=std::chrono::seconds(10);
	defTimes.Ipr=std::chrono::seconds(10);
	tpl.setPvx(20,&defTimes);
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	1000/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv3(AEvent::REAL ,15/*начало нити*/,600/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1001(AEvent::REAL ,5/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		4/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"2-100-1"),sEv3, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"1001", L"2-100-1"),sEv1001, Sevent( NO_FIXED ));
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1,1));//ПЕ реальное
		adm.add(tpl.add(trn3,2));//ПЕ реальное
		adm.add(tpl.add(trn1001));//ПЕ реальное
		auto v= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		M_INT_EV m11= fltrStatEv(11,v),m12= fltrStatEv(12,v),m13=fltrStatEv(13,v);
		CPPUNIT_ASSERT(m13[1001].getType()==AEvent::STOP_START&& "Ожидание перед блокпостом.");
		CPPUNIT_ASSERT(m12[1001].getType()==AEvent::MOVE && "На блокпосте не ожидаем.");
		CPPUNIT_ASSERT(m11[1001].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m11[3].diap.getEnd()+defTimes.Iot.count()<m11[1001].diap.getEnd()&&	"ПЕ 1001 должен ждать освобождения пути на ЕСР=11");
		CPPUNIT_ASSERT(m11[3].diap.getEnd()+defTimes.Iot.count()+ AAdmin::GapStop()+AAdmin::deltaThrough() >=m11[1001].diap.getEnd());
	}
	{
		TestPossessions p( TestAAdmin::getNsi() );
		//----- установка окна ------------
		Stech tWnd( 1, 390 /*длительность*/, 1, 1/*park*/, 2/*e1*/,1,nullptr);
		WayInfoCPtr w=tpl.fnd(tWnd);
		RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
		std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
		p.setLimitation( w, vR );
		//--------------------------------------------
		TestAAdmin adm(p);
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.StopBefAftLockPath(30, SpanLockType::BIDIR_AUTOBLOCK,TestAAdmin::Before,true);  /* стоянка перед выходом на непр.путь*/
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto frc=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(frc.size()==1);
		for (const auto& ev: frc.front().second)
			if(!ev.onSpan())
				switch (ev.Esr().getTerm())
			{
				case 13:
					CPPUNIT_ASSERT( ev.diap.duration()==AAdmin::deltaThrough() && "Ожидание на обычных станциях.");
					break;
				case 12:
					CPPUNIT_ASSERT( ev.diap.duration()>=AAdmin::StopBefAftLockPath(SpanLockType::BIDIR_AUTOBLOCK, true,false) && "Ждем на блокпосте выезда по непр.пути.");
					break;
			}
			else
				if(ev.Esr()==EsrKit(11,12))
					CPPUNIT_ASSERT(ev.Wrongway() && "Проход по неправ.пути");
	}
	{
		Stech tWnd( 1, 310 /*длительность*/, 0, 0/*park*/, 2/*e1*/,1,nullptr);
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		TestAAdmin adm(p);
		adm.add(tpl.add(trn1001));//ПЕ реальное
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto frc=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(frc.size()==1);
		for (const auto& ev: frc.front().second)
			if(!ev.onSpan())
				switch (ev.Esr().getTerm())
			{
				case 13:
					CPPUNIT_ASSERT( ev.diap.duration()>AAdmin::deltaThrough() && "Ожидание на обычных станциях.");
					break;
				case 12:
					CPPUNIT_ASSERT( ev.diap.duration()==AAdmin::deltaThrough() && "Не ждем на блокпосте.");
					break;
				case 11:
					;
			}
	}
}

void TC_AThread::testSortTrn()
{// тест сортировки поездов
	TRACE("\r\n =========== testSortTrn ============");
	{
		long s[]={ 1,2,3,4,0,5,6,7,8,9};
		long w1[]={11,12,13,0,5,6};
		long w2[]={11,0,5,6,7,12,13};
		LONG_VECTOR sV(s,s+size_array(s));
		LONG_VECTOR wV1(w1,w1+size_array(w1));
		LONG_VECTOR wV2(w2,w2+size_array(w2));

		int r=level(sV,0,sV,0);
		int r1=level(sV,0,sV,5);
		int r2=level(sV,0,sV,2);

		int r11=level(sV,0,wV1,11);
		int r0=level(sV,0,wV1,0);
		int r5=level(sV,0,wV1,5);
		int r6=level(sV,0,wV1,6);
		CPPUNIT_ASSERT(r==0 && r1==1&& r2==-3 && r11==-3 && r0==0 && r5==1 && r6==2);
	}

	TC_AtopologyParams tplParams( 60, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		16/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv3(AEvent::REAL ,10/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		18/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv5(AEvent::REAL ,15/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		11/*нач.станция (+10 в конструкторе)*/,2/*число станций*/);
	Sevent sEv2(AEvent::REAL ,4/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		26/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv4(AEvent::REAL ,9/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		25/*нач.станция (+10 в конструкторе)*/,3/*число станций*/);
	Sevent sEv6(AEvent::REAL ,14/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		22/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent tNrm1(AEvent::NONE/*тип событий*/	,10/* начало нити*/,50/* время на станции*/,20/* время на перегоне*/,0/*время на последнем перегоне*/
		,10/*нач.станция*/,11/* число станций*/);
	Sevent tNrm2(AEvent::NONE/*тип событий*/	,10/* начало нити*/,50/* время на станции*/,20/* время на перегоне*/,0/*время на последнем перегоне*/
		,30/*нач.станция*/,10/* число станций*/);
	auto norm1=tpl.norma(tNrm1,1);
	auto norm2=tpl.norma(tNrm2,2);
	Strain trn1( TrainDescr( L"1", L""),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"2-100-4"),sEv3, Sevent( NO_FIXED ));
	Strain trn5( TrainDescr( L"5", L"2-100-5"),sEv5, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"2-100-2"),sEv4, Sevent( NO_FIXED ));
	Strain trn6( TrainDescr( L"6", L"2-100-1"),sEv6, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		adm.add(tpl.add(trn5));//ПЕ реальное
		adm.add(tpl.add(trn6));//ПЕ реальное

		adm.addNorma( trn1.tdescr, norm1 );
		adm.addNorma( trn2.tdescr, norm2 );
		// фильтрация недействит.кусков нитей+ выделение  нитей для первого шага прогноза
		adm.fndAllNorm();// для всех ПЕ ищется нормативка
		V_TRN_PTR vTr= adm.prepareTrains();
		const V_TRN & all=adm.getTrn();

		V_TRN oddV,evenV;
		for (const auto& t:all )
		{
			CPPUNIT_ASSERT(t->level(t->sharedPtr())==(t->origUnionFrc().onSpan()?-1:0) && "ПЕ может отставать на перегон от своего маршрута");
			if(t->getNum()&1)
				oddV.push_back(t);
			else
				evenV.push_back(t);
		}
		{
			auto oddS=adm.sort(oddV);
			auto evenS=adm.sort(evenV);
			CPPUNIT_ASSERT(oddS!=oddV && evenS!=evenV && " Сортировка изменила порядок");
			int nOdd[]={3,1,5};
			int nEven[]={6,4,2};
			for (int i=0;i<size_array(nOdd);i++ )
			{
				CPPUNIT_ASSERT(nOdd[i]==oddS.at(i)->getNum() && "Порядок после сортировки");
				CPPUNIT_ASSERT(nEven[i]==evenS.at(i)->getNum() && "Порядок после сортировки");
			}
			auto allS=adm.sort(all);
			auto allS1=adm.sort(allS);
			CPPUNIT_ASSERT(allS==allS1 && "Сортировка отсортированного не меняет порядок");
		}
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
}

void TC_AThread::testStopOnSpan()
{ // тест стоянки на перегоне
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl(tplParams );
	tpl.setPvx(60);
	V_EVNT v;
	v.push_back(AEvent(16));
	v.back().diap.set(0,5);
	v.back().setSource(AEvent::REAL);
	v.back().setType(AEvent::MOVE);

	AEvent evStop(16,17);
	evStop.setObj(1);
	evStop.diap.set(20,20);
	evStop.setSource(AEvent::REAL);
	evStop.setType(AEvent::STOP);
	v.push_back(evStop);
	AThread tStop(v);

	AEvent evStart(16,17);
	evStart.setObj(1);
	evStart.diap.set(60,60);
	evStart.setSource(AEvent::REAL);
	evStart.setType(AEvent::START);
	v.push_back(evStart);
	AThread tmp(v);
 	{// неполные входные данные (восстанавливаем MOVE) на перегоне
 		ATrainPtr tr=tpl.add(TrainDescr( L"1", L"1-100-2"),&tStop);
 		TestAAdmin adm(70);
 		adm.add(tr);//ПЕ реальное
 		auto v=adm.forecastGlobal();
 		tstTRACE(adm);
 		auto t= adm.fnd(tr->descr());
 		auto pvx= t->getPMT(16,17);
 		AEvent ev16= t->fndEv(16,0), ev17= t->fndEv(17,0);
 		ADiap d(ev16.diap.getEnd(),ev17.diap.getOrig());
 		ADiap dStop(evStop.diap.getOrig(),AAdmin::LiveMoment());
 		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
 		CPPUNIT_ASSERT(d.duration()==pvx.full+pvx.stop+pvx.start+ TM_STOPSPAN_PASS*60 + dStop.duration() && "нужен старт со светофора");
 	}
	{// полные входные данные (восстанавливаем MOVE) на перегоне
		ATrainPtr tr=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(70);
		adm.add(tr);//ПЕ реальное
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto t= adm.fnd(tr->descr());
		auto pvx= t->getPMT(16,17);
		AEvent ev16= t->fndEv(16,0), ev17= t->fndEv(17,0);
		ADiap d(ev16.diap.getEnd(),ev17.diap.getOrig());
		ADiap dStop(evStop.diap.getOrig(),evStart.diap.getEnd());
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		CPPUNIT_ASSERT(d.duration()==pvx.full+pvx.stop+pvx.start + dStop.duration()&& "старт со светофора был");
	}
}

void TC_AThread::testFndPVX()
{// подбор ПВХ для разных типов ПЕ
	int pvhDies=20,pvhGr=40,pvhPass=10,pvhEl=30;
	Sevent sEv(	AEvent::REAL,20,/*начало нити*/	15,/*время на станции*/25,/*время на перегоне*/0,/*время на последнем перегоне*/
		2, /* нач.станция (+10 в конструкторе)*/1/* число станций*/	);
	Strain trPass( TrainDescr( L"301", L"2-100-2"),sEv, Sevent( NO_FIXED ));
	Strain trEl( TrainDescr( L"7601", L"2-100-2"),sEv, Sevent( NO_FIXED ));
	Strain trDies( TrainDescr( L"7801", L"2-100-2"),sEv, Sevent( NO_FIXED ));
	Strain trGr( TrainDescr( L"3111", L"2-100-2"),sEv, Sevent( NO_FIXED ));
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
		TC_Atopology tpl(tplParams );
		tpl.setPvx(pvhDies,SttMove::Type::TIME_DIESEL);
		tpl.setPvx(pvhGr,SttMove::Type::TIME_GOODS);
		tpl.setPvx(pvhPass,SttMove::Type::TIME_PASS);
		tpl.setPvx(pvhEl,SttMove::Type::TIME_ELSECTION);
		ATrainPtr thrPass(tpl.add(trPass)),thrEl(tpl.add(trEl)),thrGr(tpl.add(trGr)),thrDis(tpl.add(trDies));
		V_EVNT vC= thrPass->prepare();// возвращает нормализ.реальную нить
		thrGr->prepare();
		thrEl->prepare();
		thrDis->prepare();
		V_EVNT vIdGr=thrGr->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdPass=thrPass->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdEl=thrEl->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdDis=thrDis->buildIdeal().first;// построение по ПВХ
		int pvh=pvhDies;
		auto pD=std::count_if(vIdDis.begin(),vIdDis.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhGr;
		auto pG=std::count_if(vIdGr.begin(),vIdGr.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhPass;
		auto pP=std::count_if(vIdPass.begin(),vIdPass.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhEl;
		auto pE=std::count_if(vIdEl.begin(),vIdEl.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		CPPUNIT_ASSERT(pD==6 &&pG==6 &&pE==6 &&pP==6 && "8 перегонов. В начале разгон , в конце торможение");
	}
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
		TC_Atopology tpl(tplParams );
		tpl.setPvx(pvhPass,SttMove::Type::TIME_PASS);
		tpl.setPvx(pvhEl,SttMove::Type::TIME_ELSECTION);
		ATrainPtr thrPass(tpl.add(trPass)),thrEl(tpl.add(trEl)),thrGr(tpl.add(trGr)),thrDis(tpl.add(trDies));
		V_EVNT vC= thrPass->prepare();// возвращает нормализ.реальную нить
		thrGr->prepare();
		thrEl->prepare();
		thrDis->prepare();
		V_EVNT vIdGr=thrGr->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdPass=thrPass->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdEl=thrEl->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdDis=thrDis->buildIdeal().first;// построение по ПВХ
		int pvh=pvhPass;
		auto pD=std::count_if(vIdDis.begin(),vIdDis.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		auto pG=std::count_if(vIdGr.begin(),vIdGr.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		auto pP=std::count_if(vIdPass.begin(),vIdPass.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhEl;
		auto pE=std::count_if(vIdEl.begin(),vIdEl.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		CPPUNIT_ASSERT(pD==6 &&pG==6 &&pE==6 &&pP==6 && "8 перегонов. В начале разгон, в конце торможение. ПВХ от пасс.ПЕ");
	}
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
		TC_Atopology tpl(tplParams );
		tpl.setPvx(pvhDies,SttMove::Type::TIME_DIESEL);
		tpl.setPvx(pvhGr,SttMove::Type::TIME_GOODS);
		ATrainPtr thrPass(tpl.add(trPass)),thrEl(tpl.add(trEl)),thrGr(tpl.add(trGr)),thrDis(tpl.add(trDies));
		V_EVNT vC= thrPass->prepare();// возвращает нормализ.реальную нить
		thrGr->prepare();
		thrEl->prepare();
		thrDis->prepare();
		V_EVNT vIdGr=thrGr->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdPass=thrPass->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdEl=thrEl->buildIdeal().first;// построение по ПВХ
		V_EVNT vIdDis=thrDis->buildIdeal().first;// построение по ПВХ
		int pvh=pvhDies;
		auto pD=std::count_if(vIdDis.begin(),vIdDis.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhGr;
		auto pG=std::count_if(vIdGr.begin(),vIdGr.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		auto pP=std::count_if(vIdPass.begin(),vIdPass.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		auto pE=std::count_if(vIdEl.begin(),vIdEl.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		CPPUNIT_ASSERT(pD==6 &&pG==6 &&pE==6 &&pP==6 && "8 перегонов. В начале разгон, в конце торможение. ПВХ от груз.ПЕ");
	}

}

void TC_AThread::testSkrechArr()
{	// тест не учета интервалов скрещения для ПЕ на станции
	Sevent sEv4(//реальная нить
		AEvent::REAL,// тип событий
		110,// начало нити
		//60,// время на станции
		180/*( int ) defTimes.tnp.count()*/, // время на станции
		40,// время на перегоне
		0,// время на последнем перегоне
		7, // нач.станция (+10 в конструкторе)
		6// число станций
		);
	Strain trn4( TrainDescr( L"4", L""),sEv4, Sevent( NO_FIXED ));
	TRACE("\r\n =========== testSkrech ============");
	{

		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
		Sevent sEv1001(//реальная нить
			AEvent::REAL,// тип событий
			240,// начало нити
			//10,// время на станции
			180/*( int ) defTimes.tnp.count()*/, // время на станции
			0,// время на перегоне
			0,// время на последнем перегоне
			5, // нач.станция (+10 в конструкторе)
			1// число станций
			);
		Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
		{// интервалы позволяют пройти через стоянку 4 на 16 станции
			st_intervals_t defTimes;
			defTimes.tsk=std::chrono::seconds(45);
			defTimes.tn=std::chrono::seconds(140);
			defTimes.tnp=std::chrono::seconds(140);
			TC_Atopology tpl( tplParams );

			tpl.setPvx(30,&defTimes);
			TestAAdmin adm;
			adm.add(tpl.add(trn4));
			adm.add(tpl.add(trn1001));//ПЕ реальное
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений c нормативкой не должно быть.");
			auto t4=adm.fnd(trn4.tdescr);
			auto ev4= t4->fndEv(EsrKit(15));
			auto t1001=adm.fnd(trn1001.tdescr);
			auto ev16= t1001->fndEv(EsrKit(16));
			CPPUNIT_ASSERT(ev16.diap.duration()==AAdmin::deltaThrough() );
			auto ev15= t1001->fndEv(EsrKit(15));
			CPPUNIT_ASSERT(ev15.diap.getEnd()==defTimes.tsk.count()+ev4.diap.getOrig()  && "учет скрещения при прогнозе прибытия ") ;
		}
		{// интервалы не позволяют пройти через стоянку 4 на 16 станции. ждем на 15 станции
			st_intervals_t defTimes;
			defTimes.tsk=std::chrono::seconds(30);
			defTimes.tn=std::chrono::seconds(140);
			defTimes.tnp=std::chrono::seconds(140);
			TC_Atopology tpl( tplParams );

			tpl.setPvx(30,&defTimes);
			TestAAdmin adm;
			adm.add(tpl.add(trn4));
			adm.add(tpl.add(trn1001));//ПЕ реальное
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений c нормативкой не должно быть.");
			auto t4=adm.fnd(trn4.tdescr);
			auto ev4_15= t4->fndEv(EsrKit(15));
			auto ev4_16= t4->fndEv(EsrKit(16));
			auto t1001=adm.fnd(trn1001.tdescr);
			auto ev16= t1001->fndEv(EsrKit(16));
			CPPUNIT_ASSERT(ev16.diap.duration()==AAdmin::deltaThrough() );
			CPPUNIT_ASSERT(ev16.diap.getEnd()<=ev4_16.diap.getEnd()-defTimes.tsk.count()  && "учет скрещения при прогнозе прибытия ") ;
			CPPUNIT_ASSERT(ev16.diap.getOrig()>=ev4_16.diap.getOrig()+defTimes.tn.count()  && "учет неодновременного прибытия ") ;
		}
		{
			st_intervals_t defTimes;
			TC_Atopology tpl( tplParams );
			tpl.setPvx(30,&defTimes);
			TestAAdmin adm;
			adm.add(tpl.add(trn4));
			adm.add(tpl.add(trn1001));//ПЕ реальное
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений c нормативкой не должно быть.");
			auto t4=adm.fnd(trn4.tdescr);
			auto ev4= t4->fndEv(EsrKit(15));
			auto t1001=adm.fnd(trn1001.tdescr);
			auto ev= t1001->fndEv(EsrKit(15));
			CPPUNIT_ASSERT(abs (ev4.diap.getOrig()+ defTimes.tsk.count()-ev.diap.getEnd())<3  && "учет скрещения ") ;
			auto ev16= t1001->fndEv(EsrKit(16));
			CPPUNIT_ASSERT(ev16.diap.duration()==AAdmin::deltaThrough() );
	}
	}
	{
		st_intervals_t defTimes;
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN );
		TC_Atopology tpl( tplParams );
		tpl.setPvx(30,&defTimes);

		Sevent sEv1001(//реальная нить
			AEvent::REAL,// тип событий
			240,// начало нити
			//10,// время на станции
			( int ) defTimes.tnp.count(), // время на станции
			0,// время на перегоне
			0,// время на последнем перегоне
			5, // нач.станция (+10 в конструкторе)
			1// число станций
			);
		Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn4));
		adm.add(tpl.add(trn1001));//ПЕ реальное


		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
// 		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений c нормативкой не должно быть.");
		auto t1001=adm.fnd(trn1001.tdescr);
		auto ev= t1001->fndEv(EsrKit(16));
		CPPUNIT_ASSERT( ev.diap.duration()==AAdmin::deltaThrough()  && " при 2-х путке скрещения не учитывают ") ;
	}
	{
		st_intervals_t defTimes;
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
		TC_Atopology tpl( tplParams );
		tpl.setPvx(30,&defTimes);
		Sevent sEv1001(//реальная нить
		AEvent::REAL,// тип событий
		410,// начало нити
		//10,// время на станции
		( int ) defTimes.tnp.count(), // время на станции
		0,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция (+10 в конструкторе)
		1// число станций
		);
		Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn4));
		adm.add(tpl.add(trn1001));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto vl=trFrc.getSpotLines();
		CPPUNIT_ASSERT(vl.size()==2 && vl.back().first==trn1001.tdescr);
		auto ev= vl.back().second.front();
		CPPUNIT_ASSERT(ev.Esr().getTerm()==16 && ev.diap.duration()==1  && "прибытие было и учет скрещения при прогнозе не делается") ;
	}
}
