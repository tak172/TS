/*!
	created:	2019/07/16
	created: 	12:11  16 Июль 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrFixLnk2.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrFixLnk2
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

void TC_AThread::testLnkFixArrival()
{// линковка фикс.прибытия 
	TRACE("\r\n =========== testLnkFixArrival ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
	defTimes.tsk=chrono::seconds(10);
	tpl.setPvx(PVX_STANDART,&defTimes);
	TestPossessions p( TestAAdmin::getNsi() );
	V_EVNT v;
	v.push_back(AEvent(11));
	v.back().diap.set(0,1);
	v.back().setSource(AEvent::REAL);
	v.back().setType(AEvent::STOP_START);
	AThread tmp(v);
	ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
	{// без фиксации
		V_EVNT v2;
		v2.push_back(AEvent(15));
		v2.back().diap.set(0,10);
		v2.back().setSource(AEvent::REAL);
		v2.back().setType(AEvent::STOP_START);
		AThread tmpx(v2);
		ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmpx);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=adm.get("2");
		auto t1=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(14,15));
		auto evSpan2=t->fndEv(EsrKit(13,14));
		auto pvx= t->getPMT(evSpan1);
		auto ev14=t->fndEv(EsrKit(14));
		auto ev14x=t1->fndEv(EsrKit(14));
		auto v=adm.getClash();
		CPPUNIT_ASSERT(v.empty()&& "Исключений не должно быть.");
		CPPUNIT_ASSERT(ev14.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev14.diap.getEnd()>= ev14x.diap.getOrig()+ defTimes.tsk.count());
		CPPUNIT_ASSERT(ev14.diap.getEnd()< ev14x.diap.getOrig()+ defTimes.tsk.count()+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-(pvx.start+ pvx.full+pvx.stop))<2);
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-(pvx.start+ pvx.full))<2);
	}
	{// с фиксацией
		V_EVNT v2;
		v2.push_back(AEvent(15));
		v2.back().diap.set(0,10);
		v2.back().setSource(AEvent::REAL);
		v2.back().setType(AEvent::STOP_START);
		v2.push_back(AEvent(14));
		v2.back().diap.set(45,45);
		v2.back().setSource(AEvent::FIXED);
		v2.back().setType(AEvent::STOP);
		AThread tmpx(v2);
		ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmpx);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=adm.get("2");
		auto t1=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(14,15));
		auto evSpan2=t->fndEv(EsrKit(13,14));
		auto pvx= t->getPMT(evSpan1);
		auto ev14=t->fndEv(EsrKit(14));
		auto ev14x=t1->fndEv(EsrKit(14));
		auto v=adm.getClash();
		CPPUNIT_ASSERT(v.size()==1&& "Исключение должно быть.");
		CPPUNIT_ASSERT(v.front()->Comment()==ERROR_LNK_FIX_FRC);
		CPPUNIT_ASSERT(v.front()->event()->Esr()==EsrKit(14));
		CPPUNIT_ASSERT(ev14.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(ev14.getSource()==AEvent::FIXED);
		CPPUNIT_ASSERT(ev14.diap.getOrig()+2<evSpan1.diap.getEnd());
		CPPUNIT_ASSERT(ev14.diap.getOrig()==44);
		CPPUNIT_ASSERT(ev14.diap.getEnd()>= ev14x.diap.getOrig()+ defTimes.tsk.count());
		CPPUNIT_ASSERT(ev14.diap.getEnd()< ev14x.diap.getOrig()+ defTimes.tsk.count()+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(evSpan1.diap.duration()>=pvx.start+ pvx.full);
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-(pvx.start+ pvx.full))<2);
	}
}

void TC_AThread::testAdjstFixSpanOnWnd()
{// согласование фикс.события на перегоне при окне на пути
	TRACE("\r\n =========== testAdjstFixSpanOnWnd ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	Stech tWnd( 0, 300 /*длительность*/, 1, 1/*park*/, 3/*e1*/,2/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	tpl.setPvx(PVX_STANDART);
	V_EVNT v;
	v.push_back(AEvent(11));
	v.back().diap.set(0,1);
	v.back().setSource(AEvent::REAL);
	v.back().setType(AEvent::STOP_START);
	AEvent evF1(12);
	evF1.diap.set(40,40);
	evF1.setSource(AEvent::FIXED);
	v.push_back(evF1);
	AEvent evF2=evF1;
	evF2.resetEsr(EsrKit(12,13));
	v.push_back(evF2);
	AThread tmp(v);
	ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
	{
		TestPossessions p( TestAAdmin::getNsi() );
		TestAAdmin adm(p,1);
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto tr=adm.fnd(tr1->descr());
		auto evSp= tr->fndEv(12,1);
		CPPUNIT_ASSERT(!evSp.Wrongway());
		CPPUNIT_ASSERT(evSp.Esr()==EsrKit(12,13));
		CPPUNIT_ASSERT(evSp.Num()==1);
	}
	{
		TestPossessions p( TestAAdmin::getNsi() );
		// с окном на 12-13 
		p.setLimitation( tpl.fnd(tWnd), vR );
		TestAAdmin adm(p,1);
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto tr=adm.fnd(tr1->descr());
		auto evSp= tr->fndEv(12,1);
		CPPUNIT_ASSERT(evSp.Wrongway());
		CPPUNIT_ASSERT(evSp.Esr()==EsrKit(12,13));
		CPPUNIT_ASSERT(evSp.Num()==2);
	}
}

void TC_AThread::testFixDepartMove()
{// фиксация отправления в конфликте со встречным
	TRACE("\r\n =========== testFixDepartMove ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	V_EVNT v;
	v.push_back(AEvent(11));
	v.back().diap.set(0,1);
	v.back().setSource(AEvent::REAL);
	v.back().setType(AEvent::STOP_START);
	AThread tmp(v);
	ATrainPtr tr1=tpl.add(TrainDescr( L"1001", L"1-100-2"),&tmp);

 	v.push_back(AEvent(12));
	v.back().diap.set(140,140);
	v.back().setObj(1,1);
 	v.back().setSource(AEvent::FIXED);
 	v.back().setType(AEvent::START);
	AThread tmpF(v);
	ATrainPtr trF1=tpl.add(TrainDescr( L"1001", L"1-100-2"),&tmpF);
	v.clear();
	v.push_back(AEvent(15));
	v.back().diap.set(10,12);
	v.back().setSource(AEvent::REAL);
	v.back().setType(AEvent::STOP_START);
	AThread tmp2(v);
	ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmp2);
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	{
		TestAAdmin adm(p,1);
		adm.add(tr2);//ПЕ реальное
		adm.add(trF1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		M_INT_EV m=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m[1001].getSource()==AEvent::FIXED);
		CPPUNIT_ASSERT(m[2].Num()==2);
		CPPUNIT_ASSERT(m[1001].Num()==1);
		CPPUNIT_ASSERT(m[1001].diap.enclose(m[2].diap.getOrig()));
	}
	{
		TestAAdmin adm(p,1);
		adm.add(tr2);//ПЕ реальное
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		M_INT_EV m=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m[1001].getSource()==AEvent::FRC);
		CPPUNIT_ASSERT(m[2].Num()==1);
		CPPUNIT_ASSERT(m[1001].Num()==2);
		CPPUNIT_ASSERT(m[1001].diap.enclose(m[2].diap));
	}
}

void TC_AThread::testLnkFixDepart()
{// фиксация на перегоне перед ст. и фикс.отправления с нее
	TRACE("\r\n =========== testLnkFixDepart ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	V_EVNT v;
	v.push_back(AEvent(11));
	v.back().diap.set(10,20);
	v.back().setSource(AEvent::REAL);
	v.back().setType(AEvent::STOP_START);

	v.push_back(AEvent(12,11));
	v.back().diap.set(69,69);
	v.back().setSource(AEvent::FIXED);
	v.back().setType(AEvent::MOVE);

	v.push_back(AEvent(12));
	v.back().diap.set(70,70);
	v.back().setSource(AEvent::FIXED);
	v.back().setType(AEvent::MOVE);

	v.push_back(AEvent(12,13));
	v.back().diap.set(71,71);
	v.back().setSource(AEvent::FIXED);
	v.back().setType(AEvent::MOVE);

	v.push_back(AEvent(13));
	v.back().diap.set(370,370);
	v.back().setSource(AEvent::FIXED);
	v.back().setType(AEvent::START);

	AThread tmp(v);
	ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	{
		TestAAdmin adm(p,1);
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto pvx= t->getPMT(evSpan1);
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.start- pvx.full)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(evSpan2.diap.duration()-pvx.start- pvx.full)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(ev13.diap.getEnd()-370)<2);
		CPPUNIT_ASSERT(ev13.diap.duration()>50);
		CPPUNIT_ASSERT(ev12.diap.duration()==2);
		CPPUNIT_ASSERT(ev12.diap.getEnd()>=70);
		CPPUNIT_ASSERT(ev12.diap.getEnd()<=71);
	}
	Stech tWnd( 0, 300 /*длительность*/, 1, 1/*park*/, 3/*e1*/,-10/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	{// с окном на 13 => изменяем время на перегоне(продление фикс.перегона)
		p.setLimitation( tpl.fnd(tWnd), vR );
		TestAAdmin adm(p,1);
		TestAAdmin::dispNorm=1;
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=adm.get("1");
		auto evSpan1=t->fndEv(EsrKit(11,12));
		auto evSpan2=t->fndEv(EsrKit(13,12));
		auto pvx= t->getPMT(evSpan1);
		auto ev12=t->fndEv(EsrKit(12));
		auto ev13=t->fndEv(EsrKit(13));
		auto v=adm.getClash();
		CPPUNIT_ASSERT(v.size()==1);
		CPPUNIT_ASSERT(v.front()->Comment()==INTO_STATION_WND);
		CPPUNIT_ASSERT(v.front()->event()->Esr()==EsrKit(tWnd.e1));
		CPPUNIT_ASSERT(abs(evSpan1.diap.duration()-pvx.start- pvx.full)<(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(ev13.diap.getEnd()-370)<2);
		CPPUNIT_ASSERT(ev12.diap.duration()==2);
		CPPUNIT_ASSERT(ev12.diap.getEnd()>=70);
		CPPUNIT_ASSERT(ev12.diap.getEnd()<=71);
	}
}

void TC_AThread::testFixLnk()
{// фиксация и путь после нее
	TRACE("\r\n =========== testFixLnk ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	Sevent sEv1(	AEvent::REAL,30/*начало нити*/,10/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		1 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent fEv1(AEvent::FIXED,128/*начало нити*/,1/*время на станции*/,	1/*время на перегоне*/,1/*признак событий на перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,fEv1);
	ATrainPtr tr1=tpl.add(trn1,1,TC_Atopology::END);

	Sevent sEv3(	AEvent::REAL,100/*начало нити*/,4/*время на станции*/,	20/*время на перегоне*/,0/*время на последнем перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent fEv3(AEvent::FIXED,100/*начало нити*/,1/*время на станции*/,	80/*время на перегоне*/,1/*признак событий на перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	2/*число станций*/);
	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3,fEv3);
	ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::END);

	st_intervals_t defTimes(1.f);
	defTimes.tsk=chrono::seconds(5);
	defTimes.Iot=chrono::seconds(12);
	defTimes.Ipk=chrono::seconds(22);
	tpl.setPvx(PVX_STANDART,&defTimes);

	TestPossessions p( TestAAdmin::getNsi() );
 	{
 		TestAAdmin adm(p,1);
 		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::dispNorm=4;//  допустимое отклонение от нормативки не для эл.секций
 		adm.add(tr3);//ПЕ реальное
 		adm.add(tr1);//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==0 && "Исключений не должно быть.");
 		auto t=adm.get("1");
 		auto t3=adm.get("3");
 		auto evSpan=t->fndEv(EsrKit(13,14));
 		auto pvx= t->getPMT(evSpan);
 		auto evOr=t->fndEv(EsrKit(13));
 		auto evEnd=t->fndEv(EsrKit(14));
 		auto ev2=t3->fndEv(EsrKit(14));
 		CPPUNIT_ASSERT(evOr.getType()==AEvent::STOP_START);
 		CPPUNIT_ASSERT(evOr.getSource()==AEvent::FIXED);
 		CPPUNIT_ASSERT(evOr.diap.duration()>10);
 		CPPUNIT_ASSERT(evEnd.getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(evEnd.getSource()==AEvent::FRC);
 		CPPUNIT_ASSERT(evSpan.diap.duration()>pvx.full+pvx.start+pvx.stop+(int) AAdmin::GapStop());
 		CPPUNIT_ASSERT(evEnd.diap.getOrig()>ev2.diap.getEnd());
 		CPPUNIT_ASSERT( evEnd.diap.getOrig()-ev2.diap.getOrig()>=defTimes.Ipk.count());
 		CPPUNIT_ASSERT( evEnd.diap.getOrig()-ev2.diap.getOrig()<=defTimes.Ipk.count()+(int) AAdmin::GapStop());
 	}
 	{
 		TestAAdmin adm(p);
 		adm.add(tr1);//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		auto t=adm.get("1");
 		auto evSpan=t->fndEv(EsrKit(13,14));
 		auto pvx= t->getPMT(evSpan);
 		auto evOr=t->fndEv(EsrKit(13));
 		CPPUNIT_ASSERT(evOr.getType()==AEvent::STOP_START);
 		CPPUNIT_ASSERT(evOr.getSource()==AEvent::FIXED);
 		CPPUNIT_ASSERT(evOr.diap.duration()>10);
 		CPPUNIT_ASSERT(evSpan.diap.duration()==pvx.full+pvx.start);
 	}
	Sevent sEv2(	AEvent::REAL,30/*начало нити*/,10/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		9 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2,Sevent( NO_FIXED ));
	ATrainPtr tr2=tpl.add(trn2,2);
	{
		TestAAdmin adm(p);
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		adm.add(tr2);//ПЕ реальное
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v=adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto t=adm.get("1");
		auto t2=adm.get("2");
		auto evSpan2=t->fndEv(EsrKit(13,14));
		auto evSpan1=t->fndEv(EsrKit(13,12));
		auto pvx= t->getPMT(evSpan1);
		auto ev12=t->fndEv(EsrKit(12));
		auto evOr=t->fndEv(EsrKit(13));
		auto evEnd=t->fndEv(EsrKit(14));
		auto ev2=t2->fndEv(EsrKit(14));
		CPPUNIT_ASSERT(ev12.getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(evOr.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(evOr.getSource()==AEvent::FIXED);
		CPPUNIT_ASSERT(evOr.diap.duration()>10);
		CPPUNIT_ASSERT(evEnd.getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(evEnd.getSource()==AEvent::FRC);
		CPPUNIT_ASSERT(evEnd.diap.duration()>10);
		CPPUNIT_ASSERT(evSpan1.diap.duration()>=pvx.full+pvx.stop);
		CPPUNIT_ASSERT(evSpan1.diap.duration()<pvx.full+pvx.stop+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(evSpan2.diap.duration()>=pvx.full+pvx.start+pvx.stop);
		CPPUNIT_ASSERT(evSpan2.diap.duration()<pvx.full+pvx.start+pvx.stop+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(evEnd.diap.enclose(ev2.diap.getOrig()));
		CPPUNIT_ASSERT( evEnd.diap.getEnd()-ev2.diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT( evEnd.diap.getEnd()-ev2.diap.getOrig()<defTimes.tsk.count()+ AAdmin::GapStop());
	}
}


void TC_AThread::testFixFrcLine()
{// построение прогноза с ниткой фиксированного прогноза
	TRACE("\r\n =========== testFixFrc ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,40/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,0/*время на последнем перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent fEv1(	AEvent::FIXED,140/*начало нити*/,30/*время на станции*/,	4/*время на перегоне*/,1/*признак непрерывности =время на последнем перегоне*/,
		5 /*нач.станция (+10 в конструкторе)*/,	2/*число станций*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, fEv1);
	ATrainPtr trF=tpl.add(trn1,1);
	V_EVNT fxEv;
	fxEv.push_back(trF->fndEv(EsrKit(15)));
	fxEv.push_back(trF->fndEv(EsrKit(16)));

	TestPossessions p( TestAAdmin::getNsi() );
	{// без окна на 17
		TestAAdmin adm(p);
		TestAAdmin::dispNorm= AAdmin::GapStop();
		adm.add(trF);//ПЕ реальное
		ATrainPtr  t1=adm.fnd(trn1.tdescr);
		V_EVNT vFix=t1->get(AEvent::FIXED);
		CPPUNIT_ASSERT(vFix.size()==3 && vFix[0].Esr()==EsrKit(15)&& vFix[1].Esr()==EsrKit(15,16)&& vFix[2].Esr()==EsrKit(16));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto evTr=trFrc.getSpotLines();
		auto clFix=adm.getClash();// неустранимые конфликты при линковке с фикс.прогнозом
		CPPUNIT_ASSERT(clFix.size()==1 && "Исключение должно быть.");
		CPPUNIT_ASSERT(clFix.front()->Comment()==ERROR_LNK_FIX_FRC);
		CPPUNIT_ASSERT(clFix.front()->event()->Esr()==EsrKit(15));
		CPPUNIT_ASSERT(evTr.size()==1 );
		V_EVNT evF=fnd(evTr.front().second,EsrKit(),AEvent::FIXED);
		CPPUNIT_ASSERT( evF.size()==3  && evF.front().Esr()==EsrKit(15) && evF.back().Esr()==EsrKit(16) && evF[1].Esr()==EsrKit(15,16) &&" 3 фикс.события");
		auto pvx=t1->getPMT(16,17);
		for (const auto& t: evF)
		{
			V_EVNT r=fnd(evF,t.Esr(),AEvent::FIXED);
			CPPUNIT_ASSERT(r.size()==1 && r.front().diap==t.diap && " Фикс.события выделены правильно");
		}
		V_EVNT v=fnd(evTr.front().second,EsrKit(),AEvent::FRC);
		V_EVNT r=fnd(v,EsrKit(16),AEvent::FRC);
		CPPUNIT_ASSERT(r.empty());
		r=fnd(v,EsrKit(16,17),AEvent::FRC);
		CPPUNIT_ASSERT(r.size()==1);
		CPPUNIT_ASSERT(r.front().diap.getOrig()>=evF.back().diap.getEnd());
		CPPUNIT_ASSERT(r.front().diap.getOrig()<=evF.back().diap.getEnd()+1);
		CPPUNIT_ASSERT(r.front().diap.duration()==pvx.full+pvx.start);
	}
	Stech tWnd( 1, 9999 /*длительность*/, 1, 1/*park*/, 7/*e1*/,-10/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	{// с окном на 17 => доп.стоянка на 16 (продление фикс.стоянки)
		p.setLimitation( tpl.fnd(tWnd), vR );
		TestAAdmin adm(p);
		TestAAdmin::dispNorm= AAdmin::GapStop();
		adm.add(trF);//ПЕ реальное

		ATrainPtr  t1=adm.fnd(trn1.tdescr);
		V_EVNT vFix=t1->get(AEvent::FIXED);
		CPPUNIT_ASSERT(vFix.size()==3 && vFix[0].Esr()==EsrKit(15)&& vFix[1].Esr()==EsrKit(15,16)&& vFix[2].Esr()==EsrKit(16));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto evTr=trFrc.getSpotLines();
		auto clFix=adm.getClash();// неустранимые конфликты при линковке с фикс.прогнозом
		CPPUNIT_ASSERT(clFix.size()==1 && "Исключение должно быть.");
		CPPUNIT_ASSERT(clFix.front()->Comment()==ERROR_LNK_FIX_FRC);
		CPPUNIT_ASSERT(clFix.front()->event()->Esr()==EsrKit(15));
		CPPUNIT_ASSERT(evTr.size()==1 );
		V_EVNT evF=fnd(evTr.front().second,EsrKit(),AEvent::FIXED);
		CPPUNIT_ASSERT( evF.size()==3  && evF.front().Esr()==EsrKit(15) && evF.back().Esr()==EsrKit(16) && evF[1].Esr()==EsrKit(15,16) &&" 3 фикс.события");
		for (const auto& t: evF)
		{
			V_EVNT r=fnd(evF,t.Esr(),AEvent::FIXED);
			CPPUNIT_ASSERT(r.size()==1 && r.front().diap==t.diap && " Фикс.события выделены правильно");
		}
		evF=fnd(evTr.front().second,EsrKit(15,16),AEvent::FRC);
		CPPUNIT_ASSERT(evF.empty() && "События внутри фикс.линии идут без изменения");
		evF=fnd(evTr.front().second,EsrKit(15),AEvent::FRC);
		V_EVNT evN=fnd(evTr.front().second,EsrKit(15),AEvent::FIXED);
		auto evP=fnd(evTr.front().second,EsrKit(14,15),AEvent::NONE);
		CPPUNIT_ASSERT(evF.empty() && evN.size()==1 && evN.front().diap.getOrig()<evP.front().diap.getEnd() && " Прибытие ранее фикс.прогноза");
		evF=fnd(evTr.front().second,EsrKit(16),AEvent::FRC);
		evN=fnd(evTr.front().second,EsrKit(16),AEvent::FIXED);
		CPPUNIT_ASSERT(evF.size()==1 && evN.size()==1 && evF.front().diap.getOrig()>=evN.front().diap.getEnd() && " Прибытие по фикс.прогнозу");

		evF=fnd(evTr.front().second,EsrKit(16,17),AEvent::NONE);
		CPPUNIT_ASSERT(evF.size()==1 &&  evF.front().diap.duration()>30 && evF.front().diap.duration()<50 && evF.front().diap.getEnd()>10000 && 
			evF.front().diap.getEnd()<10009 && "Построен обход окна на  17 ");
	}
}
