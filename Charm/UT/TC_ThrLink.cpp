/*!
	created:	2019/12/18
	created: 	12:59  
 	filename: 	F:\potapTrunk\Charm\UT\TC_ThrLink.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrLink
	file ext:	cpp
	author:		 Dremin
	
	purpose: линковка с фикс и реальной нитями  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_data.h"
#include "../Augur/AClash.h"
#include "../Augur/TxtComment.h"
using namespace std;

 void TC_AThread::testAdvEvent()
 {//линковка c разными типами подходов 
	 TRACE("\r\n =========== testAdvEvent ============");
	 TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	 TC_Atopology tpl( tplParams );
	 tpl.setPvx(PVX_STANDART);
	 Stech tWnd( 0, 100 /*длительность*/, 1, 0/*park*/, 5/*e1*/,4/*e1*/,nullptr);
	 TestPossessions p( TestAAdmin::getNsi() );
	 p.setLimitation( tWnd, tpl );
	 AEvent::Type tbl[]={AEvent::START,AEvent::STOP,AEvent::FORM};
	 vector<AEvent::Type>vx(ARRAY_IT(tbl));
	 for (const auto& tp:vx )
	 {
		 V_EVNT v;
		 AEvent ev(15);
		 ev.diap.set(1,1);
		 ev.setSource(AEvent::ADV);
		 ev.setType(tp);
		 v.push_back(ev);
		 AThread tmpx(v);
		 ATrainPtr tr=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmpx);
		 TestAAdmin adm(p,1);
		 adm.add(tr);//ПЕ реальное
		 TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		 tstTRACE(adm);
		 auto vt= trFrc.getSpotLines().front().second;
		 CPPUNIT_ASSERT(vt[0].Esr()==EsrKit(15));
		 CPPUNIT_ASSERT(vt[0].getSourceExt()==AEvent::ADV);
		 CPPUNIT_ASSERT(vt[0].getType()==tp);
		 CPPUNIT_ASSERT(vt[0].diap==ADiap(1,1));
		 CPPUNIT_ASSERT(vt[1].Esr()==EsrKit(15));
		 CPPUNIT_ASSERT(vt[1].getSource()==AEvent::FRC);
		 CPPUNIT_ASSERT(vt[1].getType()==AEvent::START);
		 CPPUNIT_ASSERT(vt[1].diap==ADiap(101,101));
	 }
 }

void TC_AThread::testLnkRealErrorPvx()
{//линковка при влиянии реальной нити с нарушением ПВХ
	TRACE("\r\n =========== testReLinkFixSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Ipr=chrono::seconds(5);
	tpl.setPvx(PVX_STANDART,&defTimes);
	TestPossessions p( TestAAdmin::getNsi() );
	V_EVNT v2;
	{
		AEvent ev(15);
		ev.diap.set(0,60);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::STOP_START);
		v2.push_back(ev);
		ev.resetEsr(EsrKit(14));
		ev.diap.set(100,100);
		ev.setType(AEvent::MOVE);
		v2.push_back(ev);
		ev.resetEsr( EsrKit(14,13));
		ev.diap.set(101,101);
		ev.setType(AEvent::MOVE);
		v2.push_back(ev);
	}
	AThread tmpx(v2);
	ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmpx);
	{
		V_EVNT v;
		{
			AEvent ev(15);
			ev.diap.set(0,70);
			ev.setSource(AEvent::REAL);
			ev.setType(AEvent::STOP_START);
			v.push_back(ev);
			ev.resetEsr(EsrKit(14,15));
			ev.diap.set(90,90);
			ev.setType(AEvent::MOVE);
			v.push_back(ev);
		}
		AThread tmp(v);
		ATrainPtr tr4=tpl.add(TrainDescr( L"4", L"1-100-1"),&tmp);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;
		adm.add(tr2);//ПЕ реальное
		adm.add(tr4);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t2=adm.get("2");
		auto t4=adm.get("4");
		auto pvx=t4->getPMT(13,14);
		auto ev14=t4->fndEv(EsrKit(14));
		auto ev14x=t2->fndEv(EsrKit(14));
		auto evSp1=t4->fndEv(EsrKit(14,15));
		auto evSp2=t4->fndEv(EsrKit(14,13));
		auto ev13=t4->fndEv(EsrKit(13));
		auto ev13x=t2->fndEv(EsrKit(13));
		CPPUNIT_ASSERT(ev14.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev14x.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev13x.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev14.diap.getOrig()>= ev14x.diap.getOrig()+ defTimes.Ipk.count());
		CPPUNIT_ASSERT(ev14.diap.getOrig()<= ev14x.diap.getOrig()+ defTimes.Ipk.count()+AAdmin::GapStop());
		CPPUNIT_ASSERT(ev13.diap.getOrig()>= ev13x.diap.getOrig()+ defTimes.Ipk.count());
		CPPUNIT_ASSERT(ev13.diap.getOrig()< ev13x.diap.getOrig()+ defTimes.Ipk.count()+AAdmin::GapStop());
		CPPUNIT_ASSERT(evSp1.diap.duration()>pvx.start+pvx.full+ pvx.stop+(int) AAdmin::GapStop() && "Нарушение ПВХ");
		CPPUNIT_ASSERT(evSp2.diap.duration()==pvx.full);
	}
}

void TC_AThread::testReLinkFixSpan()
{//перелинковка  обратного дотягивания с перегона 
	TRACE("\r\n =========== testReLinkFixSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
	//	defTimes.tsk=chrono::seconds(10);
	tpl.setPvx(PVX_STANDART,&defTimes);
	TestPossessions p( TestAAdmin::getNsi() );
	V_EVNT v2;
	v2.push_back(AEvent(15));
	v2.back().diap.set(0,60);
	v2.back().setSource(AEvent::REAL);
	v2.back().setType(AEvent::STOP_START);
	AThread tmpx(v2);
	ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmpx);
	{// без фиксации
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=adm.get("2");
		auto t1=adm.get("1");
		auto ev14=t->fndEv(EsrKit(14));
		auto ev14x=t1->fndEv(EsrKit(14));
		auto vс=adm.getClash();
		CPPUNIT_ASSERT(vс.empty()&& "Исключений не должно быть.");
		CPPUNIT_ASSERT(ev14.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev14x.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev14.diap.getEnd()>= ev14x.diap.getOrig()+ defTimes.tsk.count());
		CPPUNIT_ASSERT(ev14.diap.getEnd()< ev14x.diap.getOrig()+ defTimes.tsk.count()+(int) AAdmin::GapStop());
	}
	{// с фиксацией точно
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		AEvent evS(14,15);
		evS.diap.set(145,145);
		evS.setSource(AEvent::FIXED);
		evS.setType(AEvent::MOVE);
		v.push_back(evS);
		AEvent evSt(15);
		evSt.diap.set(145,145);
		evSt.setSource(AEvent::FIXED);
		evSt.setType(AEvent::MOVE);
		v.push_back(evSt);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=adm.get("2");
		auto t1=adm.get("1");
		auto ev14=t->fndEv(EsrKit(14));
		auto ev14x=t1->fndEv(EsrKit(14));
		auto evSp=t1->fndEv(EsrKit(14,15));
		auto pvx=t1->getPMT(14,15);
		auto vс=adm.getClash();
		CPPUNIT_ASSERT(vс.empty()&& "Исключений не должно быть.");
		CPPUNIT_ASSERT(ev14.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev14x.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev14.diap.getEnd()>= ev14x.diap.getOrig()+ defTimes.tsk.count());
		CPPUNIT_ASSERT(ev14.diap.getEnd()< ev14x.diap.getOrig()+ defTimes.tsk.count()+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(evSp.diap.duration()==pvx.full && "Нет нарушения ПВХ из-за фиксации");
	}
	{// с фиксацией до
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		AEvent evS(14,15);
		evS.diap.set(125,125);
		evS.setSource(AEvent::FIXED);
		evS.setType(AEvent::MOVE);
		v.push_back(evS);
		AEvent evSt(15);
		evSt.diap.set(125,125);
		evSt.setSource(AEvent::FIXED);
		evSt.setType(AEvent::MOVE);
		v.push_back(evSt);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=adm.get("2");
		auto t1=adm.get("1");
		auto ev14=t->fndEv(EsrKit(14));
		auto ev14x=t1->fndEv(EsrKit(14));
		auto evSp=t1->fndEv(EsrKit(14,15));
		auto pvx=t1->getPMT(14,15);
		auto vс=adm.getClash();
		CPPUNIT_ASSERT(vс.size()==1 && "Исключение должно быть.");
		CPPUNIT_ASSERT(vс.front()->Comment()==ERROR_LNK_FIX_FRC);
		CPPUNIT_ASSERT(vс.front()->event()->Esr()==EsrKit(15,14));
		CPPUNIT_ASSERT(ev14.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev14x.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev14.diap.getEnd()>= ev14x.diap.getOrig()+ defTimes.tsk.count());
		CPPUNIT_ASSERT(ev14.diap.getEnd()< ev14x.diap.getOrig()+ defTimes.tsk.count()+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(evSp.diap.duration()+(int)AAdmin::GapStop()<pvx.full && "Нарушение ПВХ из-за фиксации");
	}
	{// с фиксацией после
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		AEvent evS(14,15);
		evS.diap.set(175,175);
		evS.setSource(AEvent::FIXED);
		evS.setType(AEvent::MOVE);
		v.push_back(evS);
		AEvent evSt(15);
		evSt.diap.set(175,175);
		evSt.setSource(AEvent::FIXED);
		evSt.setType(AEvent::MOVE);
		v.push_back(evSt);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=adm.get("2");
		auto t1=adm.get("1");
		auto ev14=t->fndEv(EsrKit(14));
		auto ev14x=t1->fndEv(EsrKit(14));
		auto evSp=t1->fndEv(EsrKit(14,15));
		auto pvx=t1->getPMT(14,15);
		auto vс=adm.getClash();
		CPPUNIT_ASSERT(vс.empty()&& "Исключений не должно быть.");
		CPPUNIT_ASSERT(ev14.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev14x.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev14.diap.getEnd()>= ev14x.diap.getOrig()+ defTimes.tsk.count());
		CPPUNIT_ASSERT(ev14.diap.getEnd()< ev14x.diap.getOrig()+ defTimes.tsk.count()+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSp.diap.duration()-(pvx.full+pvx.start) )<2);
	}
}

