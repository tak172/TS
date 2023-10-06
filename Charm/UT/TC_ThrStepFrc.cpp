/*!
	created:	2019/03/13
	created: 	12:48  13 ћарт 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrStepFrc.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrStepFrc
	file ext:	cpp
	author:		 Dremin
	
	purpose:	шаг прогноза и обработка его результатов
  
  */
 
#include "stdafx.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/TrainRoutes.h"
#include "../Augur/AOutrun.h"
#include "../helpful/TechnicalTime.h"
#include "../helpful/RoadCode.h"
#include "../Augur/AClash.h"
#include "../Augur/AAdmin.h"
#include "../Augur/AConflData.h"

using namespace std;

void TC_AThread::testMorthStopMove()
{// тест превращени€ сто€нки в проследование (борьба с жуком) 
	TRACE("\r\n =========== testMorthStopMove ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.tn=chrono::seconds(9);
	defTimes.tnp=chrono::seconds(12);
	tpl.setPvx(PVX_STANDART,&defTimes);
	Sevent sEv2(	AEvent::REAL,77/*начало нити*/,1/*врем€ на станции*/,	1/*врем€ на перегоне*/,0/*врем€ на последнем перегоне*/,
		3 /*нач.станци€ (+10 в конструкторе)*/,	1/*число станций*/);

	Sevent sEv1(	AEvent::REAL,1/*начало нити*/,1/*врем€ на станции*/,	1/*врем€ на перегоне*/,0/*врем€ на последнем перегоне*/,
		0 /*нач.станци€ (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent tNrm1(AEvent::NONE,// тип событий
		95,// начало нити
		30,// врем€ на станции
		30,// врем€ на перегоне
		0,// врем€ на последнем перегоне
		0, // нач.станци€ (+10 в конструкторе)
		10// число станций
		);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,Sevent(NO_FIXED));
	ATrainPtr tr1=tpl.add(trn1,1,TC_Atopology::BOTH);
	Strain trn2( TrainDescr( L"2", L"2-200-1"),sEv2,Sevent(NO_FIXED));
	ATrainPtr tr2=tpl.add(trn2,2,TC_Atopology::BOTH);
	auto norm1=tpl.norma(tNrm1,1);
	{// нет строгого учета интервала “н 
		TestAAdmin adm;
		adm.MinSmallStop(14);// врем€ маленькой сто€нки дл€  превращени€ в проследование в сек
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::noSort=true;//----------- дл€ тестовой отладки--------------
		adm.addNorma(TrainDescr( L"1", L""), norm1 );
		adm.add(tr1);//ѕ≈ реальное
		adm.add(tr2);//ѕ≈ реальное
		AConflData::strongTnpRgm(false);// установка(true)/сн€тие(false) учета интервала “н в случае, если оба поезда прибывают с остановкой, то  можно не соблюдать.
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m11[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m11[1].diap.enclose(m11[2].diap.getEnd()));
		CPPUNIT_ASSERT(m11[2].diap.enclose(m11[1].diap.getOrig()));
		CPPUNIT_ASSERT(m11[2].diap.getEnd()>=m11[1].diap.getOrig()+ defTimes.tsk.count());
		CPPUNIT_ASSERT(m11[2].diap.getEnd()<=m11[1].diap.getOrig()+ defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(m11[2].diap.getOrig()-m11[1].diap.getOrig())>=defTimes.tn.count());
		CPPUNIT_ASSERT(abs(m11[2].diap.getOrig()-m11[1].diap.getOrig())<=defTimes.tn.count()+ AAdmin::GapStop());
	}
	{// при строгом учете интервала “н 
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- дл€ тестовой отладки--------------
		ACounterInt::limit=1;//построение конфликтов
		adm.addNorma(TrainDescr( L"1", L""), norm1 );
		adm.add(tr1);//ѕ≈ реальное
		adm.add(tr2);//ѕ≈ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m11[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m11[1].diap.enclose(m11[2].diap));
		CPPUNIT_ASSERT(m11[2].diap.getOrig()-m11[1].diap.getOrig()>defTimes.tn.count());
		CPPUNIT_ASSERT(m11[2].diap.getOrig()-m11[1].diap.getOrig()<defTimes.tn.count()+9);
	}
}
