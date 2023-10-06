/*!
	created:	2019/08/05
	created: 	19:54  
 	filename: 	F:\potapTrunk\Charm\UT\TC_ThrFixBackLnk.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrFixBackLnk
	file ext:	cpp
	author:		 Dremin
	
	purpose: обратное  согласование
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_data.h"
#include "../Augur/AClash.h"
#include "../Augur/TxtComment.h"
using namespace std; 
 
void TC_AThread::testAdjstFixStopViaStart()
{//согласование фикс на перегоне и фикс. прибытия/отправления
	TRACE("\r\n =========== testAdjstFixStopViaStart ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	{
		V_EVNT v;
		AEvent ev(11);
		ev.diap.set(0,1);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		AEvent ev12F(EsrKit(12));
		ev12F.diap.set(50,50);
		ev12F.setSource(AEvent::FIXED);
		ev12F.setType(AEvent::STOP);
		v.push_back(ev12F);
		ev.resetEsr(EsrKit(13,12));
		ev.diap.set(89,89);
		ev.setSource(AEvent::FIXED);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		AEvent ev13F(EsrKit(13));
		ev13F.diap.set(100,100);
		ev13F.setSource(AEvent::FIXED);
		ev13F.setType(AEvent::MOVE);
		v.push_back(ev13F);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto evSpan3=t->fndEv(EsrKit(13,14));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()- pvx.full-pvx.stop)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-(pvx.full +pvx.start))<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan3.diap.duration()-pvx.full )<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(abs(ev13.diap.getEnd()-ev13F.diap.getOrig())<=AAdmin::GapStop()/2);
		CPPUNIT_ASSERT(ev12.diap.enclose(ev12F.diap));
		CPPUNIT_ASSERT(ev12.diap.getOrig()+10 <ev12F.diap.getOrig());
		CPPUNIT_ASSERT(ev12.diap.getEnd()>ev12F.diap.getEnd()+10);
	}
	{
		V_EVNT v;
		AEvent ev(11);
		ev.diap.set(0,1);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		AEvent ev12F(EsrKit(12));
		ev12F.diap.set(50,50);
		ev12F.setSource(AEvent::FIXED);
		ev12F.setType(AEvent::START);
		v.push_back(ev12F);
		ev.resetEsr(EsrKit(13,12));
		ev.diap.set(59,59);
		ev.setSource(AEvent::FIXED);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		AEvent ev13F(EsrKit(13));
		ev13F.diap.set(100,100);
		ev13F.setSource(AEvent::FIXED);
		ev13F.setType(AEvent::START);
		v.push_back(ev13F);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()- pvx.full-pvx.stop)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-(pvx.full +pvx.stop +pvx.start))<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(abs(ev13.diap.getEnd()-ev13F.diap.getOrig())<=AAdmin::GapStop()/2);
		CPPUNIT_ASSERT(ev12.diap.getEnd()>=ev12F.diap.getEnd());
		CPPUNIT_ASSERT(ev12.diap.getEnd()<=ev12F.diap.getEnd()+AAdmin::GapStop()/2);
	}
	{
		V_EVNT v;
		AEvent ev(11);
		ev.diap.set(0,1);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		AEvent ev12F(EsrKit(12));
		ev12F.diap.set(50,50);
		ev12F.setSource(AEvent::FIXED);
		ev12F.setType(AEvent::START);
		v.push_back(ev12F);
		ev.resetEsr(EsrKit(13,12));
		ev.diap.set(59,59);
		ev.setSource(AEvent::FIXED);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		AEvent ev13F(EsrKit(13));
		ev13F.diap.set(100,100);
		ev13F.setSource(AEvent::FIXED);
		ev13F.setType(AEvent::STOP);
		v.push_back(ev13F);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()- pvx.full-pvx.stop)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(evSpan2.diap.duration()>pvx.full +pvx.stop +pvx.start+ (int) AAdmin::GapStop() && "Фиксация с нарушением ПВХ");
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(abs(ev13.diap.getOrig()-ev13F.diap.getOrig())<=AAdmin::GapStop()/2);
		CPPUNIT_ASSERT(ev13.diap.duration()>AAdmin::deltaThrough());
		CPPUNIT_ASSERT(ev13.diap.duration()<AAdmin::deltaThrough()+AAdmin::GapStop());
	}
}

void TC_AThread::testBackAdjustFixMoveStop()
{// обратное согласование при изменении фикс проследования на стоянку
	TRACE("\r\n =========== testBackAdjstFixMoveStop ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	Stech tWnd( 0, 1000 /*длительность*/, 1, 0/*park*/, 4/*e1*/,3/*e1*/,nullptr);
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
  	{//  без фиксации
  		V_EVNT v;
  		v.push_back(AEvent(11));
  		v.back().diap.set(0,1);
  		v.back().setSource(AEvent::REAL);
  		v.back().setType(AEvent::MOVE);
  		AThread tmp(v);
  		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
  		TestAAdmin adm(p);
  		adm.add(tr1);//ПЕ реальное
  		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
  		tstTRACE(adm);
  		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
  		auto t=adm.get("1");
  		auto evSpan1=t->fndEv(EsrKit(11,12));
  		auto evSpan2=t->fndEv(EsrKit(13,12));
  		auto ev12=t->fndEv(EsrKit(12));
  		auto ev13=t->fndEv(EsrKit(13));
  		auto pvx= t->getPMT(evSpan1);
  		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()- pvx.full)<(int) AAdmin::GapStop());
  		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-pvx.full -pvx.stop)<(int) AAdmin::GapStop());
  		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START);
  		CPPUNIT_ASSERT(ev12.getType()==AEvent::MOVE);
  		CPPUNIT_ASSERT(abs(ev13.diap.getOrig()-73)<(int) AAdmin::GapStop());
  	}
  	{//  с фиксацией до
  		V_EVNT v;
  		v.push_back(AEvent(11));
  		v.back().diap.set(0,1);
  		v.back().setSource(AEvent::REAL);
  		v.back().setType(AEvent::MOVE);
  		v.push_back(AEvent(13,12));
  		v.back().diap.set(59,59);
  		v.back().setSource(AEvent::FIXED);
  		v.back().setType(AEvent::MOVE);
  		v.push_back(AEvent(13));
  		v.back().diap.set(60,60);
  		v.back().setSource(AEvent::FIXED);
  		v.back().setType(AEvent::MOVE);
  		AThread tmp(v);
  		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
  		TestAAdmin adm(p);
 		TestAAdmin::dispNorm=1;// допустимое отклонение от нормативки и фикс.прогноза не для эл.секций
 		adm.add(tr1);//ПЕ реальное
  		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
  		tstTRACE(adm);
		auto cv=adm.getClash();
		CPPUNIT_ASSERT(cv.size()==1 && "Исключение должно быть.");
		CPPUNIT_ASSERT(cv.front()->Comment()==ERROR_LNK_FIX_FRC );
		CPPUNIT_ASSERT(cv.front()->event()->Esr()==EsrKit(12,13));
  		auto t=adm.get("1");
  		auto evSpan1=t->fndEv(EsrKit(11,12));
  		auto evSpan2=t->fndEv(EsrKit(13,12));
  		auto ev12=t->fndEv(EsrKit(12));
  		auto ev13=t->fndEv(EsrKit(13));
  		auto pvx= t->getPMT(evSpan1);
  		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.full)<(int) AAdmin::GapStop());
  		CPPUNIT_ASSERT(evSpan2.diap.duration()+(int) AAdmin::GapStop()<pvx.full);
  		CPPUNIT_ASSERT(ev12.getType()==AEvent::MOVE);
  		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START);
  		CPPUNIT_ASSERT(abs(ev13.diap.getOrig()-v.back().diap.getOrig())<(int) AAdmin::GapStop());
  	}
 	{//  с фиксацией после
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::MOVE);
		v.push_back(AEvent(13));
		v.back().diap.set(100,100);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		v.push_back(AEvent(13,12));
		v.back().diap.set(99,99);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		TestAAdmin::dispNorm=1;// допустимое отклонение от нормативки и фикс.прогноза не для эл.секций
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.full-pvx.stop)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-pvx.full-pvx.stop-pvx.start)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev13.diap.getOrig()==v.back().diap.getOrig());
	}
}

void TC_AThread::testAdjstStatFixWarning()
{// согласование при фиксации на станции около перегона с предупреждением
	TRACE("\r\n =========== testAdjstStatFixWarning ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );

	Stech tWnd( 0, 1000 /*длительность*/, 1, 0/*park*/, 2/*e1*/,3/*e1*/,std::make_shared<unsigned int> (10));
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
	{// c фиксацией неточной (дальше)
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(12));
		v.back().diap.set(60,61);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		v.push_back(AEvent(13,12));
		v.back().diap.set(61,61);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.start- pvx.full-pvx.stop)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-360)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev12.diap.getEnd()==v.back().diap.getEnd());
		CPPUNIT_ASSERT(abs(evSpan2.diap.getEnd()-420)<(int) AAdmin::GapStop());
	}
	{// c фиксацией неточной (ближе) и выходом на перегон с предупреждением
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(12));
		v.back().diap.set(30,31);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		v.push_back(AEvent(13,12));
		v.back().diap.set(31,31);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==1);
		CPPUNIT_ASSERT(adm.getClash().front()->Comment()==ERROR_LNK_FIX_FRC);
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.start- pvx.full)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-360)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(abs(evSpan2.diap.getOrig()-31)<(int) AAdmin::GapStop());
	}

	{// c фиксацией неточной (ближе)
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(13));
		v.back().diap.set(351,351);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==1);
		CPPUNIT_ASSERT(adm.getClash().front()->Comment()==ERROR_LNK_FIX_FRC);
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.start- pvx.full)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-360)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(evSpan2.diap.getEnd()>ev13.diap.getOrig()+50);
		CPPUNIT_ASSERT(abs(evSpan2.diap.getOrig()-44)<(int) AAdmin::GapStop());
	}
	{// c фиксацией неточной (дальше)
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(13));
		v.back().diap.set(504,504);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.start- pvx.full)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-360)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev13.diap.getEnd()-v.back().diap.getEnd()<=AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.getEnd()-404)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.getOrig()-44)<(int) AAdmin::GapStop());
	}
}


void TC_AThread::testAdjstFixWarning()
{//обратное согласование при фиксации на перегоне с предупреждением
	TRACE("\r\n =========== testAdjstFixWarning ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );

	Stech tWnd( 0, 1000 /*длительность*/, 1, 0/*park*/, 2/*e1*/,3/*e1*/,std::make_shared<unsigned int> (20));
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
	{// c фиксацией неточной (ближе)
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(12,13));
		v.back().diap.set(100,100);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		v.push_back(AEvent(12));
		v.back().diap.set(100,100);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==0);
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev=t->fndEv(EsrKit(12));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-(pvx.start + pvx.full+pvx.stop))<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-pvx.full*(120/20))<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(abs(evSpan2.diap.getOrig()-100)<(int) AAdmin::GapStop());
	}
	{// c фиксацией неточной (ближе)
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);

		v.push_back(AEvent(12,13));
		v.back().diap.set(350,350);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		v.push_back(AEvent(13));
		v.back().diap.set(351,351);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==0);
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev=t->fndEv(EsrKit(12));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-(pvx.start + pvx.full+pvx.stop))<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-pvx.full*(120/20))<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(abs(evSpan2.diap.getEnd()-350)<(int) AAdmin::GapStop());
	}
	{// без фиксации
		V_EVNT v;
		v.push_back(AEvent(11));
		v.back().diap.set(0,1);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto ev=t->fndEv(EsrKit(12));
		auto pvx= t->getPMT(evSpan1);
		CPPUNIT_ASSERT(ev.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.start- pvx.full)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-6*pvx.full)<(int) AAdmin::GapStop());
	}
}

void TC_AThread::testAdjstFixStopViaSpan()
{// согласование фикс. прибытия и фикс на перегоне +прибытие
	TRACE("\r\n =========== testAdjstFixStopViaSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );

	Stech tWnd( 0, 100 /*длительность*/, 1, 0/*park*/, 2/*e1*/,3/*e1*/,nullptr);
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
	{// c точной фиксацией
		V_EVNT v;
		AEvent ev(11);
		ev.diap.set(0,1);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		ev.resetEsr(EsrKit(12));
		ev.diap.set(39,39);
		ev.setType(AEvent::STOP);
		ev.setSource(AEvent::FIXED);
		v.push_back(ev);
		ev.resetEsr(EsrKit(12,13));
		ev.diap.set(137,137);
		ev.setType(AEvent::MOVE);
		ev.setSource(AEvent::FIXED);
		v.push_back(ev);
		ev.resetEsr(EsrKit(13));
		ev.diap.set(145,145);
		ev.setType(AEvent::STOP);
		ev.setSource(AEvent::FIXED);
		AEvent ev13Fix=ev;
		v.push_back(ev);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		TestAAdmin::dispNorm=1;// допустимое отклонение от нормативки и фикс.прогноза не для эл.секций
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==0);
		auto t=adm.get("1");
		auto evSpan=t->fndEv(EsrKit(12,13));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan);
		CPPUNIT_ASSERT(evSpan.diap.duration()>=pvx.start+pvx.full+pvx.stop);
		CPPUNIT_ASSERT(evSpan.diap.duration()<=pvx.start+pvx.full+pvx.stop+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT(ev12.diap.getEnd()-tWnd.count<AAdmin::GapStop());
		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT(abs(ev13.diap.getOrig()-ev13Fix.diap.getOrig())<= TestAAdmin::dispNorm );
		CPPUNIT_ASSERT(ev13.diap.duration()>AAdmin::deltaThrough());
		CPPUNIT_ASSERT(ev13.diap.duration()<AAdmin::deltaThrough()+ AAdmin::GapStop());
	}
	{// c фиксацией до
		V_EVNT v;
		AEvent ev(11);
		ev.diap.set(0,1);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		ev.resetEsr(EsrKit(12));
		ev.diap.set(39,39);
		ev.setType(AEvent::STOP);
		ev.setSource(AEvent::FIXED);
		v.push_back(ev);
		ev.resetEsr(EsrKit(12,13));
		ev.diap.set(137,137);
		ev.setType(AEvent::MOVE);
		ev.setSource(AEvent::FIXED);
		v.push_back(ev);
		ev.resetEsr(EsrKit(13));
		ev.diap.set(139,139);
		ev.setType(AEvent::STOP);
		ev.setSource(AEvent::FIXED);
		AEvent ev13Fix=ev;
		v.push_back(ev);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		TestAAdmin::dispNorm=3;// допустимое отклонение от нормативки и фикс.прогноза не для эл.секций
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto vCl=adm.getClash();
		CPPUNIT_ASSERT(vCl.size()==1);
		CPPUNIT_ASSERT(vCl.front()->Comment()==INTO_SPAN_WND);
		auto t=adm.get("1");
		auto evSpan=t->fndEv(EsrKit(12,13));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan);
		CPPUNIT_ASSERT(evSpan.diap.duration()>=pvx.start+pvx.full+pvx.stop);
		CPPUNIT_ASSERT(evSpan.diap.duration()<=pvx.start+pvx.full+pvx.stop+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT(ev12.diap.getEnd()<tWnd.count);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT(abs(ev13.diap.getOrig()-ev13Fix.diap.getOrig())<= TestAAdmin::dispNorm );
		CPPUNIT_ASSERT(ev13.diap.duration()>AAdmin::deltaThrough());
		CPPUNIT_ASSERT(ev13.diap.duration()<AAdmin::deltaThrough()+ AAdmin::GapStop());
	}
	{// c фиксацией после
		V_EVNT v;
		AEvent ev(11);
		ev.diap.set(0,1);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		ev.resetEsr(EsrKit(12));
		ev.diap.set(59,59);
		ev.setType(AEvent::STOP);
		ev.setSource(AEvent::FIXED);
		AEvent ev12Fix=ev;
		v.push_back(ev);
		ev.resetEsr(EsrKit(12,13));
		ev.diap.set(137,137);
		ev.setType(AEvent::MOVE);
		ev.setSource(AEvent::FIXED);
		v.push_back(ev);
		ev.resetEsr(EsrKit(13));
		ev.diap.set(159,159);
		ev.setType(AEvent::STOP);
		ev.setSource(AEvent::FIXED);
		AEvent ev13Fix=ev;
		v.push_back(ev);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		TestAAdmin::dispNorm=3;// допустимое отклонение от нормативки и фикс.прогноза не для эл.секций
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto vCl=adm.getClash();
		CPPUNIT_ASSERT(vCl.empty());
		auto t=adm.get("1");
		auto evSpan=t->fndEv(EsrKit(12,13));
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto pvx= t->getPMT(evSpan);
		CPPUNIT_ASSERT(evSpan.diap.duration()>=pvx.start+pvx.full+pvx.stop);
		CPPUNIT_ASSERT(evSpan.diap.duration()<=pvx.start+pvx.full+pvx.stop+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT(ev12.diap.enclose(ev12Fix.diap));
		CPPUNIT_ASSERT(ev12.diap.getOrig()<ev12Fix.diap.getOrig()+10);
		CPPUNIT_ASSERT(ev12.diap.getEnd()>ev12Fix.diap.getEnd()+50);
		CPPUNIT_ASSERT(ev12.diap.getEnd()>tWnd.count +10);
		CPPUNIT_ASSERT(ev13.getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT(abs(ev13.diap.getOrig()-ev13Fix.diap.getOrig())<= TestAAdmin::dispNorm );
		CPPUNIT_ASSERT(ev13.diap.duration()>AAdmin::deltaThrough());
		CPPUNIT_ASSERT(ev13.diap.duration()<AAdmin::deltaThrough()+ AAdmin::GapStop());
	}

	{// без фиксации
		V_EVNT v;
		AEvent ev(11);
		ev.diap.set(0,1);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		AThread tmp(v);
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==0);
		auto t=adm.get("1");
		auto evSpan=t->fndEv(EsrKit(12,13));
		auto ev12=t->fndEv(EsrKit(12));
		auto pvx= t->getPMT(evSpan);
		CPPUNIT_ASSERT(evSpan.diap.duration()==pvx.start+pvx.full);
		CPPUNIT_ASSERT(ev12.getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT(ev12.diap.getEnd()-tWnd.count<AAdmin::GapStop());
	}
}