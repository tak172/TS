/*!
	created:	2019/07/23
	created: 	12:54  23 Июль 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrNormNorm.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrNormNorm
	file ext:	cpp
	author:		 Dremin
	
	purpose:	особенности решения конфликтов между ПЕ, идущими по нормативке
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/ProNorm.h"
#include "../Augur/ADiap.h"

using namespace std;
 
void TC_AThread::testNormNormInterval()
{// учет интервалов между ПЕ. Обе по нормативке
	TRACE("\r\n =========== testNormNormInterval ============");
	// однопутка, 2 пути на станции
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
 	defTimes.tsk=chrono::seconds(15);
// 	defTimes.Iot=chrono::seconds(12);
	tpl.setPvx(60,&defTimes);
	// нормативки
	Sevent sN3(AEvent::NONE,0/*начало нити*/,20/*на станции*/,50/*на перегоне*/,0,5/*нач.станция*/,8/*число станций*/) ;
	Sevent sN4(AEvent::NONE,0/*начало нити*/,10/*на станции*/,30/*на перегоне*/,0,8/*нач.станция*/,10/*число станций*/) ;
	auto norm3=tpl.norma(sN3,1);
	auto norm4=tpl.norma(sN4,2);
	//реальная нить
	Sevent sEv3(AEvent::REAL,0/*начало нити*/,1/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,1/*число станций*/) ;
	Sevent sEv4(AEvent::REAL,22/*начало нити*/,1/*на станции*/,20/*на перегоне*/,0,8/*нач.станция*/,1/*число станций*/) ;
	Strain trn3( TrainDescr( L"3", L"1-1-2" ),	sEv3, Sevent( NO_FIXED));
	Strain trn4( TrainDescr( L"4", L"1-1-1" ),	sEv4, Sevent( NO_FIXED));
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::dispNormElSec=4;//  допустимое отклонение от нормативки для эл.секций
		TestAAdmin::dispNorm=10;//  допустимое отклонение от нормативки не для эл.секций

		adm.add(tpl.add(trn3,1));//ПЕ реальное
		adm.add(tpl.add(trn4,2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"3", L""), norm3 );
		adm.addNorma(TrainDescr( L"4", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto m17=fltrStatEv(17,v),m16=fltrStatEv(16,v);
		CPPUNIT_ASSERT(norm4[2].Esr()==EsrKit(16));
		CPPUNIT_ASSERT(m16[4].diap.duration()==norm4[2].diap.duration());
		CPPUNIT_ASSERT(abs(m16[4].diap.getOrig()-norm4[2].diap.getOrig())<TestAAdmin::dispNorm && "допустимое отклонение от нормативки");
		CPPUNIT_ASSERT(norm4[1].Esr()==EsrKit(17));
		CPPUNIT_ASSERT(m17[4].diap.duration()==norm4[1].diap.duration());
		CPPUNIT_ASSERT(abs(m17[4].diap.getOrig()-norm4[1].diap.getOrig())>TestAAdmin::dispNorm);
		CPPUNIT_ASSERT(m16[4].diap.getOrig()-m17[4].diap.getEnd()== (norm4[2].diap.getOrig()-norm4[1].diap.getEnd())*9/10);
		CPPUNIT_ASSERT(abs(m16[4].diap.getOrig()-m16[3].diap.getEnd())<defTimes.tsk.count() && " не учет иннтервалов");
	}
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::dispNormElSec=4;//  допустимое отклонение от нормативки для эл.секций
		TestAAdmin::dispNorm=14;//  допустимое отклонение от нормативки не для эл.секций
		adm.add(tpl.add(trn3,1));//ПЕ реальное
		adm.add(tpl.add(trn4,2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"3", L""), norm3 );
		adm.addNorma(TrainDescr( L"4", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto m17=fltrStatEv(17,v),m16=fltrStatEv(16,v);
		CPPUNIT_ASSERT(m17[4].diap.duration()==10);
		CPPUNIT_ASSERT(m17[3].diap.duration()==20);
		CPPUNIT_ASSERT(m16[4].diap.enclose(m16[3].diap.getEnd()));
		CPPUNIT_ASSERT(m16[3].diap.enclose(m16[4].diap.getOrig()));
		CPPUNIT_ASSERT(m16[4].diap.duration()==10);
		CPPUNIT_ASSERT(m16[3].diap.duration()==20);
		CPPUNIT_ASSERT(m16[3].diap.getEnd()-m16[4].diap.getOrig()<defTimes.tsk.count()- AAdmin::GapStop());
	}
}
 
void TC_AThread::testNormNormStrong()
{// учет интервалов между ПЕ. Обе строго по нормативке 
	TRACE("\r\n =========== testNormNormStrong ============");
	// однопутка, 2 пути на станции
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
	defTimes.tsk=chrono::seconds(15);
	// 	defTimes.Iot=chrono::seconds(12);
	tpl.setPvx(60,&defTimes);
	// нормативки
	Sevent sN3(AEvent::NONE,0/*начало нити*/,20/*на станции*/,50/*на перегоне*/,0,5/*нач.станция*/,8/*число станций*/) ;
	Sevent sN4(AEvent::NONE,115/*начало нити*/,10/*на станции*/,30/*на перегоне*/,0,8/*нач.станция*/,10/*число станций*/) ;
	auto norm3=tpl.norma(sN3,1);
	auto norm4=tpl.norma(sN4,2);
	//реальная нить
	Sevent sEv3(AEvent::REAL,0/*начало нити*/,1/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,1/*число станций*/) ;
	Sevent sEv4(AEvent::REAL,131/*начало нити*/,1/*на станции*/,20/*на перегоне*/,0,8/*нач.станция*/,1/*число станций*/) ;
	Strain trn3( TrainDescr( L"3", L"1-1-2" ),	sEv3, Sevent( NO_FIXED));
	Strain trn4( TrainDescr( L"4", L"1-1-1" ),	sEv4, Sevent( NO_FIXED));
	{
		/*если по прогнозу поезд не отстает от расписания (прибывает и отправляется в соответствии 
		с нормативным расписанием +15 секунд для поездов состоящих из электросекций (6001-7798), +30 секунд для 
		остальных поездов) станционные интервалы, при конфликтах с другими поездами прогнозируемыми по 
		нормативным нитям без опоздания, проверять не надо.	*/
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::dispNormElSec=4;//  допустимое отклонение от нормативки для эл.секций
		TestAAdmin::dispNorm=10;//  допустимое отклонение от нормативки не для эл.секций

		adm.add(tpl.add(trn3,1));//ПЕ реальное
		adm.add(tpl.add(trn4,2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"3", L""), norm3 );
		adm.addNorma(TrainDescr( L"4", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto m17=fltrStatEv(17,v);
		CPPUNIT_ASSERT(norm3[2].Esr()==EsrKit(17));
		CPPUNIT_ASSERT(norm4[1].Esr()==EsrKit(17));
		CPPUNIT_ASSERT(m17[3].diap==norm3[2].diap);
		CPPUNIT_ASSERT(abs(m17[4].diap.getOrig()-m17[3].diap.getEnd())<defTimes.tsk.count()&& "не учитываем интервалы");
		CPPUNIT_ASSERT(abs(m17[4].diap.getOrig()-norm4[1].diap.getOrig())<TestAAdmin::dispNorm);
		CPPUNIT_ASSERT(abs(m17[3].diap.getOrig()-norm3[2].diap.getOrig())<TestAAdmin::dispNorm);
	}
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=20;//  допустимое отклонение от нормативки не для эл.секций

		adm.add(tpl.add(trn3,1));//ПЕ реальное
		adm.add(tpl.add(trn4,2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"3", L""), norm3 );
		adm.addNorma(TrainDescr( L"4", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto m17=fltrStatEv(17,v);
		CPPUNIT_ASSERT(norm4[1].Esr()==EsrKit(17));
		CPPUNIT_ASSERT(norm3[2].Esr()==EsrKit(17));
		CPPUNIT_ASSERT(m17[3].diap==norm3[2].diap);
		CPPUNIT_ASSERT( abs(m17[4].diap.getOrig()-norm4[1].diap.getOrig() )<TestAAdmin::dispNorm);
		CPPUNIT_ASSERT( abs(m17[4].diap.getEnd()-norm4[1].diap.getEnd() )<TestAAdmin::dispNorm);
		CPPUNIT_ASSERT(m17[4].diap.getOrig()==m17[3].diap.getEnd());
	}
}