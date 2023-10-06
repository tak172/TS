/*!
	created:	2019/08/19
	created: 	15:10  
 	filename: 	F:\potapTrunk\Charm\UT\TC_ThrWndLock.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrWndLock
	file ext:	cpp
	author:		 Dremin
	
	purpose: окна и средства связи
  
  */

#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"

using namespace std;

void TC_AThread::testCrossBndWnd()
{// пересечение границы окна( измененние средств связи)
	TRACE("\r\n =========== testCrossBndWnd ============");
	TestAAdmin::crossTmPass=15;//	Для пассажирских поездов (1-998; 5001-7998) время проезда переезда по умолчанию 3 минуты.
	TestAAdmin::crossTm=10;//	Для остальных поездов (1001-4998; 8001-9798) время  проезда переезда по умолчанию 2,5 минуты.
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 45, 200 /*длительность*/, 1, 1/*park*/, 2/*e1*/,3/*e2*/, nullptr/*limit*/ );
	st_intervals_t defTimes;
	Sevent sEv2(//реальная нить
		AEvent::REAL,// тип событий
		5,// начало нити
		2, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		4, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
   	{// окно с однотипной сигнализацией
   		tWnd.sR=make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS);
   		TestPossessions p( TestAAdmin::getNsi() );
   		//----- установка окна ------------
   		WayInfoCPtr w=tpl.fnd(tWnd);
   		RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
   		std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
   		p.setLimitation( w, vR );
   		TestAAdmin::StopBefAftLockPath(50,*tWnd.sR,TestAAdmin::LockSem,true); /*увеличение перегонного времени на стоянку перед семафором на непр.пути*/
   		//--------------------------------
   		TestAAdmin adm(p);
   		int crosses=4;
   		adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// число  переездов на перегоне
   		adm.add(tpl.add(trn2,2));
   		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
   		tstTRACE(adm);
   		V_EVNT v=trFrc.getSpotLines().front().second;
   		auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
   		CPPUNIT_ASSERT(v.size()>7);
   		CPPUNIT_ASSERT(v[2].Esr()==EsrKit(13));
   		CPPUNIT_ASSERT(v[2].typeDiap()==AEvent::MOVE);
   		CPPUNIT_ASSERT(v[4].Esr()==EsrKit(12));
   		CPPUNIT_ASSERT(v[4].typeDiap()==AEvent::MOVE);
   		CPPUNIT_ASSERT(v[3].Esr()==EsrKit(13,12));
   		CPPUNIT_ASSERT(v[3].diap.duration()==pvx.full);
   		CPPUNIT_ASSERT(!v[3].Lock());
  	}
 	{// пересекаем окончание окна
 		Stech tWnd( 0, 160 /*длительность*/, 1, 1/*park*/, 2/*e1*/,3/*e2*/, nullptr/*limit*/ );
 		tWnd.sR=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
 		tWnd.sW=nullptr;
 		TestPossessions p( TestAAdmin::getNsi() );
 		//----- установка окна ------------
 		WayInfoCPtr w=tpl.fnd(tWnd);
 		RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
 		std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
 		p.setLimitation( w, vR );
 		TestAAdmin::StopBefAftLockPath(50,*tWnd.sR,TestAAdmin::LockSem,true); /*увеличение перегонного времени на стоянку перед семафором на непр.пути*/
 		//--------------------------------
 		TestAAdmin adm(p);
 		int crosses=4;
 		adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// число  переездов на перегоне
 		adm.add(tpl.add(trn2,2));
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		V_EVNT v=trFrc.getSpotLines().front().second;
 		auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
 		CPPUNIT_ASSERT(v.size()>7);
 		CPPUNIT_ASSERT(v[2].Esr()==EsrKit(13));
 		CPPUNIT_ASSERT(v[2].diap.getOrig()<50);
 		CPPUNIT_ASSERT(v[2].diap.getEnd()<tWnd.count+ AAdmin::GapStop());
 		CPPUNIT_ASSERT(v[2].diap.getEnd()>tWnd.count);
 		CPPUNIT_ASSERT(v[4].Esr()==EsrKit(12));
 		CPPUNIT_ASSERT(v[4].diap.duration()==AAdmin::deltaThrough());
 		CPPUNIT_ASSERT(v[3].Esr()==EsrKit(13,12));
 		CPPUNIT_ASSERT(v[3].diap.duration()==pvx.full+pvx.start);
 		CPPUNIT_ASSERT(!v[3].Lock());
 	}
	{// пересекаем начало окна
		tWnd.sR=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
		TestPossessions p( TestAAdmin::getNsi() );
		//----- установка окна ------------
		WayInfoCPtr w=tpl.fnd(tWnd);
		RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
		std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
		p.setLimitation( w, vR );
		TestAAdmin::StopBefAftLockPath(50,*tWnd.sR,TestAAdmin::LockSem,true); /*увеличение перегонного времени на стоянку перед семафором на непр.пути*/
		//--------------------------------
		TestAAdmin adm(p);
		int crosses=4;
		adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// число  переездов на перегоне
		adm.add(tpl.add(trn2,2));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		V_EVNT v=trFrc.getSpotLines().front().second;
		auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
		CPPUNIT_ASSERT(v.size()>7);
		CPPUNIT_ASSERT(v[2].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(v[2].diap.duration()>=TestAAdmin::stopBeforeLockPath[SpanLockType::ELECTRIC_TOKEN]);
		CPPUNIT_ASSERT(v[2].diap.duration()<TestAAdmin::stopBeforeLockPath[SpanLockType::ELECTRIC_TOKEN]+ AAdmin::GapStop());
		CPPUNIT_ASSERT(v[4].Esr()==EsrKit(12));
		CPPUNIT_ASSERT(v[4].diap.duration()>=TestAAdmin::stopAfterLockPath[SpanLockType::ELECTRIC_TOKEN]);
		CPPUNIT_ASSERT(v[4].diap.duration()<=TestAAdmin::stopAfterLockPath[SpanLockType::ELECTRIC_TOKEN]+AAdmin::deltaThrough());
		CPPUNIT_ASSERT(v[3].Esr()==EsrKit(13,12));
		CPPUNIT_ASSERT(v[3].diap.duration()==pvx.full+pvx.start+pvx.stop+ TestAAdmin::DelayLockSema(SpanLockType::ELECTRIC_TOKEN));
		CPPUNIT_ASSERT(v[3].Lock());
		CPPUNIT_ASSERT(*v[3].Lock()==SpanLockType::ELECTRIC_TOKEN);
	}
	{// пересекаем окончание окна
		Stech tWnd( 0, 160 /*длительность*/, 1, 1/*park*/, 2/*e1*/,3/*e2*/, nullptr/*limit*/ );
		tWnd.sR=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
		TestPossessions p( TestAAdmin::getNsi() );
		//----- установка окна ------------
		WayInfoCPtr w=tpl.fnd(tWnd);
		RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
		std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
		p.setLimitation( w, vR );
		TestAAdmin::StopBefAftLockPath(50,*tWnd.sR,TestAAdmin::LockSem,true); /*увеличение перегонного времени на стоянку перед семафором на непр.пути*/
		//--------------------------------
		TestAAdmin adm(p);
		int crosses=4;
		adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// число  переездов на перегоне
		adm.add(tpl.add(trn2,2));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		V_EVNT v=trFrc.getSpotLines().front().second;
		auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
		CPPUNIT_ASSERT(v.size()>7);
		CPPUNIT_ASSERT(v[2].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(v[2].diap.getEnd()>tWnd.count && "ждем концв окна");
		CPPUNIT_ASSERT(v[2].diap.getEnd()<tWnd.count+ AAdmin::GapStop());
		CPPUNIT_ASSERT(v[4].Esr()==EsrKit(12));
		CPPUNIT_ASSERT(v[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(v[3].Esr()==EsrKit(13,12));
		CPPUNIT_ASSERT(v[3].diap.duration()==pvx.full+pvx.start);
		CPPUNIT_ASSERT(!v[3].Lock());
	}
}

 
