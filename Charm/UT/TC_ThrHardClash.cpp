/*!
	created:	2019/10/09
	created: 	11:48  
 	filename: 	F:\potapTrunk\Charm\UT\TC_ThrHardClash.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrHardClash
	file ext:	cpp
	author:		 Dremin
	
	purpose: решение тяжелых исключений  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "TC_statPathProp.h"
using namespace std;

void TC_AThread::testManyHardClashWnd()
{// решение тяжелого исключения при множественности конфликтов и окне на перегоне
	TRACE("\r\n =========== testManyHardClashWnd ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 2 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)5.0);
	tpl.setPvx(20,&defTimes);
	TestPossessions p( TestAAdmin::getNsi() );
	//----- установка окна ------------
	Stech tWnd( 0, 200 /*длительность*/, 1, 0/*park*/, 4/*e1*/, 3/*e2*/, nullptr/*limit*/ );
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	Sevent sEv5(	AEvent::REAL,50/* начало*/,1/* на станции*/,1/*время на перегоне*/,1/*на последнем перегоне*/,0/* нач.станция*/,1/* число станций*/);
	Sevent sEv3(	AEvent::REAL,0/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,1/* нач.станция*/,1/* число станций*/);
	Sevent sEv1(	AEvent::REAL,0/* начало*/,1/* на станции*/,1/*время на перегоне*/,1/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Sevent sEv2(	AEvent::REAL,0/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,6/* нач.станция*/,1/* число станций*/);
	Strain trn1( TrainDescr( L"8001", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn5( TrainDescr( L"8005", L"1-100-2"),sEv5, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));

	{// обгон создан и не решен 
		TestAAdmin adm(p,1);
		ACounterInt::limit= 16; // по умолчанию
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useBackAdjust=true;
		adm.add(tpl.add(trn3,2));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn5,1));//ПЕ реальное
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].diap.enclose(m14[3].diap));
		CPPUNIT_ASSERT(m14[2].diap.getEnd()>=m14[3].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m14[2].diap.getEnd()<=m14[3].diap.getOrig()+defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[3].diap.getEnd()>tWnd.or+ tWnd.count);
		CPPUNIT_ASSERT(m13[3].diap.getEnd()<tWnd.or+ tWnd.count+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[8001].diap.enclose(m13[2].diap));
		CPPUNIT_ASSERT(m13[8001].diap.getEnd()>=m13[2].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m13[8001].diap.getEnd()<=m13[2].diap.getOrig()+defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[3].getType()==AEvent::MOVE);
	}
}

void TC_AThread::testHardClashWnd()
{// решение тяжелых исключений  при окне на перегоне  
	TRACE("\r\n =========== testHardClashWnd ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)5.0);
	tpl.setPvx(40,&defTimes);
	TestPossessions p( TestAAdmin::getNsi() );
	//----- установка окна ------------
	Stech tWnd( 0, 200 /*длительность*/, 1, 0/*park*/, 4/*e1*/, 3/*e2*/, nullptr/*limit*/ );
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	Sevent sEv1(	AEvent::REAL,0/* начало*/,1/* на станции*/,1/*время на перегоне*/,1/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Sevent sEv3(	AEvent::REAL,0/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,0/* нач.станция*/,1/* число станций*/);
	Sevent sEv2(	AEvent::REAL,0/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,6/* нач.станция*/,1/* число станций*/);
	Strain trn1( TrainDescr( L"8001", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));

	{// обгон создан и не решен 
		TestAAdmin adm(p,1);
		ACounterInt::limit= 16; // по умолчанию
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useBackAdjust=true;
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3,2));//ПЕ реальное
		adm.add(tpl.add(trn1,1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].diap.enclose(m14[3].diap));
		CPPUNIT_ASSERT(m14[2].diap.getEnd()>=m14[3].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m14[2].diap.getEnd()>=m14[8001].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m14[2].diap.getEnd()<=m14[8001].diap.getOrig()+defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[3].diap.getEnd()>tWnd.or+ tWnd.count);
		CPPUNIT_ASSERT(m13[3].diap.getEnd()<tWnd.or+ tWnd.count+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[3].getType()==AEvent::MOVE);
	}
}

void TC_AThread::testHardClash()
{// решение тяжелых исключений  c менеджером путей
	TRACE("\r\n =========== testHardClash ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)5.0);
	tpl.setPvx(40,&defTimes);

	Sevent sEv1(	AEvent::REAL,0/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,5/* нач.станция*/,1/* число станций*/);
	Sevent sEv3(	AEvent::REAL,15/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,5/* нач.станция*/,1/* число станций*/);
	Sevent sEv5(	AEvent::REAL,30/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,5/* нач.станция*/,1/* число станций*/);
	Sevent sEv2(	AEvent::REAL,5/* начало*/,10/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,7/* нач.станция*/,1/* число станций*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3, Sevent( NO_FIXED ));
	Strain trn5( TrainDescr( L"5", L"1-100-2"),sEv5, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	map<EsrKit, RouteIntrusion::StationIntrusion> mIntrusions;
	RouteIntrusion::PassingOrderPtr passingOrderPtr( new RouteIntrusion::PassingOrder( TrainDescrPtr( new TrainDescr(trn5.tdescr) ) ) );
	mIntrusions[ EsrKit( 15 )]=RouteIntrusion::StationIntrusion( passingOrderPtr);
	RouteIntrusion rIntr(trn1.tdescr, vector<EsrKit>(), mIntrusions, map<EsrKit, RouteIntrusion::SpanIntrusion>() );
	TestAAdmin::VRouteIntrusionPtr v(new std::vector<RouteIntrusion>());
	v->push_back(rIntr);

	{// обгон создан и не решен 
		TestAAdmin adm;
		ACounterInt::limit= 16; // по умолчанию
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useBackAdjust=true;

		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn1,1),v);//ПЕ реальное
		adm.add(tpl.add(trn3,2));//ПЕ реальное
		adm.add(tpl.add(trn5,3));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(16,trFrc),m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m[2].diap.enclose(m[3].diap));
		CPPUNIT_ASSERT(m15[1].diap.enclose(m15[5].diap));
		CPPUNIT_ASSERT(m15[5].diap.getEnd()-m15[3].diap.getOrig()>=defTimes.Iot.count());
		CPPUNIT_ASSERT(m[5].diap.getEnd()-m[3].diap.getOrig()>=defTimes.Ipk.count());
	}
}

void TC_AThread::testHardClashPassWnd()
{// решение тяжелых исключений c попутными  при окне на перегоне  
	TRACE("\r\n =========== testHardClashPassWnd ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)5.0);
	tpl.setPvx(40,&defTimes);
	TestPossessions p( TestAAdmin::getNsi() );
	//----- установка окна ------------
	Stech tWnd( 0, 200 /*длительность*/, 1, 0/*park*/, 4/*e1*/, 3/*e2*/, nullptr/*limit*/ );
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	Sevent sEv1(	AEvent::REAL,0/* начало*/,1/* на станции*/,1/*время на перегоне*/,0/*на последнем перегоне*/,0/* нач.станция*/,1/* число станций*/);
	Sevent sEv3(	AEvent::REAL,0/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,1/* нач.станция*/,1/* число станций*/);
	Sevent sEv5(	AEvent::REAL,0/* начало*/,1/* на станции*/,0/*время на перегоне*/,1/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3, Sevent( NO_FIXED ));
	Strain trn5( TrainDescr( L"8005", L"1-100-2"),sEv5, Sevent( NO_FIXED ));

	{// обгон создан и решен 
		TestAAdmin adm(p,1);
		ACounterInt::limit= 16; // по умолчанию
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useBackAdjust=true;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn5));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[8005].diap.enclose(m13[3].diap));
		CPPUNIT_ASSERT(m13[8005].diap.enclose(m13[1].diap));
		CPPUNIT_ASSERT(m13[3].diap.getEnd()<m13[1].diap.getOrig());
		CPPUNIT_ASSERT(abs(m13[1].diap.getEnd()-m13[8005].diap.getEnd())>=defTimes.Iot.count());
		CPPUNIT_ASSERT(abs(m13[1].diap.getEnd()-m13[8005].diap.getEnd())<=defTimes.Iot.count() + AAdmin::GapStop());
	}
	{// обгон создан и решен 
		TestAAdmin adm(p,1);
		ACounterInt::limit= 16; // по умолчанию
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		//		TestAAdmin::useBackAdjust=true;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn5));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[8005].diap.enclose(m13[3].diap));
		CPPUNIT_ASSERT(m13[8005].diap.enclose(m13[1].diap));
		CPPUNIT_ASSERT(m13[3].diap.getEnd()<m13[1].diap.getOrig());
		CPPUNIT_ASSERT(abs(m13[1].diap.getEnd()-m13[8005].diap.getEnd())>=defTimes.Iot.count());
		CPPUNIT_ASSERT(abs(m13[1].diap.getEnd()-m13[8005].diap.getEnd())<=defTimes.Iot.count() + AAdmin::GapStop());
	}
}
