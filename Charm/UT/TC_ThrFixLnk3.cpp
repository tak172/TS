/*!
	created:	2020/02/20
	created: 	18:35  
 	filename: 	F:\potapTrunk\Charm\UT\TC_ThrFixLnk3.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrFixLnk3
	file ext:	cpp
	author:		 Dremin
	
	purpose: 
  
  */
 

#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_data.h"
#include "../Augur/AClash.h"
#include "../Augur/TxtComment.h"
using namespace std;

void TC_AThread::testAdjstFixSpanPassTrn()
{ // согласование фикс.события на перегоне при окне на пути и попутном ПЕ
	TRACE("\r\n =========== testAdjstFixSpanPassTrn ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	Stech tWnd( 0, 100 /*длительность*/, 1, 1/*park*/, 3/*e1*/,2/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	TestPossessions p( TestAAdmin::getNsi() );
	// с окном на 12-13 
	p.setLimitation( tpl.fnd(tWnd), vR );
	st_intervals_t defTimes(1.f);
	defTimes.tsk=chrono::seconds(3);
	defTimes.Iot=chrono::seconds(5);
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Ipr=chrono::seconds(7);
	tpl.setPvx(PVX_STANDART,&defTimes);

	AEvent ev1(11);
	ev1.diap.set(0,1);
	ev1.setSource(AEvent::REAL);
	ev1.setType(AEvent::STOP_START);
	ev1.setObj(1,1);
	V_EVNT v1(1,ev1);
	ev1.diap+=30;
	V_EVNT v2(1,ev1);
	ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&AThread(v1));
 	{
 		ATrainPtr tr3=tpl.add(TrainDescr( L"3", L"1-100-2"),&AThread(v2));
 		TestAAdmin adm(p,1);
 		adm.add(tr1);//ПЕ реальное
 		adm.add(tr3);//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
 		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
 		CPPUNIT_ASSERT(m12[1].diap.getEnd()>tWnd.count);
 		CPPUNIT_ASSERT(m12[3].diap.getEnd()>=m12[1].diap.getEnd()+ defTimes.Iot.count());
 		CPPUNIT_ASSERT(m12[3].diap.getEnd()<=m12[1].diap.getEnd()+ defTimes.Iot.count()+AAdmin::GapStop());
 		CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(m13[3].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(m13[3].diap.getOrig()>=m13[1].diap.getOrig()+ defTimes.Ipk.count());
 		CPPUNIT_ASSERT(m13[3].diap.getOrig()<=m13[1].diap.getOrig()+ defTimes.Ipk.count()+AAdmin::GapStop());
 	}
	AEvent evF(12);
	evF.diap.set(106,106);
	evF.setObj(2,1);
	evF.setType(AEvent::START);
	evF.setSource(AEvent::FIXED);
	v2.push_back(evF);
	evF.resetEsr(EsrKit(12,13));
	evF.setObj(1,1);
	evF.setType(AEvent::MOVE);
	v2.push_back(evF);
	{
		ATrainPtr tr3=tpl.add(TrainDescr( L"3", L"1-100-2"),&AThread(v2));
		TestAAdmin adm(p,1);
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);// ф-ция  отбора заданий по обгонам
		adm.add(tr1);//ПЕ реальное
		adm.add(tr3);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12[1].diap.getEnd()>tWnd.count);
		CPPUNIT_ASSERT(m12[3].diap.getEnd()>=m12[1].diap.getEnd()+ defTimes.Iot.count());
		CPPUNIT_ASSERT(m12[3].diap.getEnd()<=m12[1].diap.getEnd()+ defTimes.Iot.count()+AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[3].diap.getOrig()>=m13[1].diap.getOrig()+ defTimes.Ipk.count());
		CPPUNIT_ASSERT(m13[3].diap.getOrig()<=m13[1].diap.getOrig()+ defTimes.Ipk.count()+AAdmin::GapStop());
	}
}

 
