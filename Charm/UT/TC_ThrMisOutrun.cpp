/*!
	created:	2019/07/10
	created: 	10:43  10 Июль 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrMisOutrun.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrMisOutrun
	file ext:	cpp
	author:		 Dremin
	
	purpose:	потерянные обгоны
  
  */
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "TC_statPathProp.h"

using namespace std;

 void TC_AThread::testResortTrn()
 {// поиск обгонов попутных ПЕ
 	TRACE("\r\n =========== testResortTrn ============");
 	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 2 пути на станции
	//------------ на 12 стоянка запрещена -------------------
 	tplParams.add(12,1,TC_StatPath(noStopPr));
 	tplParams.add(12,2,TC_StatPath(noStopPr));
 	TC_Atopology tpl( tplParams );
 	Stech tWnd( 0, 240 /*длительность*/, 1, 1/*park*/, 4/*e1*/, 3/*e2*/, nullptr/*limit*/ );
 	st_intervals_t defTimes(4.f);
 	defTimes.tnp=chrono::seconds(20);
	defTimes.tn=chrono::seconds(20);
// 	defTimes.Iot=chrono::seconds(10);
// 	defTimes.Ipk=chrono::seconds(30);
 	tpl.setPvx(40,&defTimes);
 	V_EVNT v;
 	AEvent ev(14);
 	ev.setObj(1,1);
 	ev.diap.set(1);
 	ev.setSource(AEvent::REAL);
 	ev.setType(AEvent::MOVE);
 	v.push_back(ev);
 	ev.resetEsr(EsrKit(13,14));
 	ev.diap+=ADiap(1,90);
 	v.push_back(ev);
 	ev.resetEsr(EsrKit(13));
 	ev.diap.set(100,310);
 	ev.setType(AEvent::STOP_START);
 	v.push_back(ev);
 	AThread tmp(v);
 	ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmp);
 
 	Sevent sEv3(AEvent::REAL ,10,1,0,0,0,1);/* начало ,время на станции+перегоне +на последнем перегоне,нач.станция, число станций*/
 	Sevent sEv5(AEvent::REAL ,70,1,2,1,0,1);
 	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3, Sevent( NO_FIXED ));
 	Strain trn5( TrainDescr( L"5", L"1-100-2"),sEv5, Sevent( NO_FIXED ));
 	TestPossessions p( TestAAdmin::getNsi() );
 	//----- установка окна ------------
 	WayInfoCPtr w=tpl.fnd(tWnd);
 	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
 	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
 	p.setLimitation( w, vR );
 	//--------------------------------
 	{// без пересортировки
 		TestAAdmin adm(p,1);
 		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
 		TestAAdmin::deltaWait=10;//зазор для расчета при ликвидации обгона 
 		//------------------------------
 		adm.add(tr2);
 		adm.add(tpl.add(trn5,1));
 		adm.add(tpl.add(trn3,1));
 
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc),m11=fltrStatEv(11,trFrc);
	 	CPPUNIT_ASSERT(  m13[3].getType()==AEvent::STOP_START&& m13[5].getType()==AEvent::MOVE);
	 	CPPUNIT_ASSERT(  m12[5].getType()==AEvent::MOVE && m12[3].getType()==AEvent::MOVE && " нет путей для стоянки");
		CPPUNIT_ASSERT(m11[5].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m13[3].diap.getEnd()>tWnd.count);
		CPPUNIT_ASSERT(m13[3].diap.getEnd()<tWnd.count+  AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[3].diap.getEnd()<m13[5].diap.getOrig());
		CPPUNIT_ASSERT(m13[3].diap.getEnd()+  AAdmin::GapStop()>=m13[5].diap.getOrig());
		CPPUNIT_ASSERT(m13[3].diap.getEnd()+defTimes.Iot.count()<m13[5].diap.getEnd());
	}
 }

void TC_AThread::testMisOutrun()
{// возможная потеря обгонов из-за нехватки путей на станции
	TRACE("\r\n =========== testMisOutrun ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 2 пути на станции
	TC_Atopology tpl( tplParams );
	Stech tWnd( 10, 900 /*длительность*/, 1, 1/*park*/, 4/*e1*/, 5/*e2*/, nullptr/*limit*/ );
	st_intervals_t defTimes(5.f);
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Iot=chrono::seconds(10);
	tpl.setPvx(20,&defTimes);
	Sevent sEv1(AEvent::REAL ,20,1,2,1,1,1); /* начало ,время на станции+перегоне +на последнем перегоне,нач.станция, число станций*/
	Sevent sEv3(AEvent::REAL ,30,1,2,1,1,1);
	Sevent sEv5(AEvent::REAL ,50,1,2,1,1,1);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3, Sevent( NO_FIXED ));
	Strain trn5( TrainDescr( L"5", L"1-100-2"),sEv5, Sevent( NO_FIXED ));
	TestPossessions p( TestAAdmin::getNsi() );
	//----- установка окна ------------
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	//--------------------------------
	TestAAdmin adm(p);
	TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
	TestAAdmin::deltaWait=10;//зазор для расчета при ликвидации обгона 
	TestAAdmin::fnFltr.clear();
	TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по попутным обгонам
	TestAAdmin::useBackAdjust=true;
	//------------------------------
	adm.add(tpl.add(trn3,1));
	adm.add(tpl.add(trn5,1));
	adm.add(tpl.add(trn1,1));

	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc),m15=fltrStatEv(15,trFrc);
	CPPUNIT_ASSERT(  m14[1].getType()==AEvent::STOP_START&& m14[3].getType()==AEvent::STOP_START&&m14[5].getType()==AEvent::STOP_START);
	CPPUNIT_ASSERT(  m13[5].getType()==AEvent::STOP_START&& m13[1].getType()==AEvent::MOVE&&m13[3].getType()==AEvent::MOVE);
	CPPUNIT_ASSERT(m14[3].diap.getEnd()-m14[1].diap.getEnd()-defTimes.Iot.count()<= AAdmin::GapStop());
	CPPUNIT_ASSERT(m14[5].diap.getEnd()-m14[3].diap.getEnd()-defTimes.Iot.count()<= AAdmin::GapStop());
	CPPUNIT_ASSERT(m14[3].diap.getEnd()>=m14[1].diap.getEnd()+defTimes.Iot.count());
	CPPUNIT_ASSERT(m14[5].diap.getEnd()>=m14[3].diap.getEnd()+defTimes.Iot.count());
	CPPUNIT_ASSERT(m14[5].diap.getOrig()>m14[1].diap.getEnd());
	CPPUNIT_ASSERT(  m15[5].getType()==AEvent::MOVE&& m15[1].getType()==AEvent::MOVE&&m15[3].getType()==AEvent::MOVE);
	CPPUNIT_ASSERT(m15[3].diap.getEnd()-m15[1].diap.getEnd()-defTimes.Ipk.count()<= AAdmin::GapStop());
	CPPUNIT_ASSERT(m15[5].diap.getEnd()-m15[3].diap.getEnd()-defTimes.Ipk.count()<= AAdmin::GapStop());
	CPPUNIT_ASSERT(m15[3].diap.getEnd()>=m15[1].diap.getEnd()+defTimes.Ipk.count());
	CPPUNIT_ASSERT(m15[5].diap.getEnd()>=m15[3].diap.getEnd()+defTimes.Ipk.count());
}
