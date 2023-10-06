/*!
	created:	2019/02/19
	created: 	17:52  19 Февраль 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrMorphing.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrMorphing
	file ext:	cpp
	author:		 Dremin
	
	purpose:	тесты превращения событий в прогнозной нити в зависимости от окружения (соседние нити)
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Augur/AArchive.h"
#include "../helpful/Attic.h"
using namespace std;
 
void TC_AThread::testMorphPack()
{//превращение событий в прогнозе в зависимости от соседней прогнозной нити 
	TRACE("\r\n =========== testMorphPack ============");
	Sevent sEv1(AEvent::REAL ,24/*начало нити*/,	1/*время на станции*/,10/*время на перегоне*/,0/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv3(AEvent::REAL ,17/*начало нити*/,1/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv5(AEvent::REAL ,40/*начало нити*/,1/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent fEv1(AEvent::FIXED,60/*начало нити*/,1/*время на станции*/,	80/*время на перегоне*/,1/*признак событий на перегоне*/,
		8 /*нач.станция (+10 в конструкторе)*/,	2/*число станций*/);

	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED )); 
	Strain trn5( TrainDescr( L"5", L"1-1-2"),sEv5, Sevent( NO_FIXED )); 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, fEv1);

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,4, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(5.f);
	defTimes.Iot=chrono::seconds(10) ;
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Ipr=chrono::seconds(12);
	TC_Atopology tpl( tplParams );
	STT_Val pvx=generateSTT(2,30,1,3);
	tpl.setPvx(pvx);
	tpl.setStInt(defTimes);

	{// изменяем из-за предшествующей нити
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn5));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto m18=fltrStatEv(18,trFrc),m19=fltrStatEv(19,trFrc);
		CPPUNIT_ASSERT(m18[1].typeDiap()==AEvent::MOVE);
		CPPUNIT_ASSERT(m18[3].typeDiap()==AEvent::MOVE);
		CPPUNIT_ASSERT(m18[5].typeDiap()==AEvent::MOVE);
		CPPUNIT_ASSERT(m19[3].diap.getOrig()-m19[1].diap.getOrig()>defTimes.Ipk.count() );
		CPPUNIT_ASSERT(m19[5].diap.getOrig()-m19[3].diap.getOrig()>defTimes.Ipk.count() );
		CPPUNIT_ASSERT(m19[3].diap.getOrig()-m19[1].diap.getOrig()<=defTimes.Ipk.count() + AAdmin::GapStop());
		CPPUNIT_ASSERT(m19[5].diap.getOrig()-m19[3].diap.getOrig()<=defTimes.Ipk.count() + AAdmin::GapStop());
	}
}

void TC_AThread::testMorphFrcFrc()
{//превращение событий в прогнозе в зависимости от соседней прогнозной нити 
	TRACE("\r\n =========== testMorphFrcFrc ============");
	Sevent sEv1(AEvent::REAL ,24/*начало нити*/,	1/*время на станции*/,140/*время на перегоне*/,70/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv3(AEvent::REAL ,40/*начало нити*/,1/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv5(AEvent::REAL ,60/*начало нити*/,1/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn5( TrainDescr( L"5", L"1-1-2"),sEv5, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(10.f);
	defTimes.Iot=defTimes.Ipk=defTimes.Ipr=chrono::seconds(20);
	TC_Atopology tpl( tplParams );
	STT_Val pvx=generateSTT(2,30,1,3);
	tpl.setPvx(pvx);
	tpl.setStInt(defTimes);
	M_INT_EV m18,m17;
	{// изменения из-за реальной нити
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
 		TestAAdmin::fnFltr.clear();
 		adm.add(tpl.add(trn1));//ПЕ реальное
 		adm.add(tpl.add(trn3));//ПЕ реальное
 		adm.add(tpl.add(trn5));//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
		m18=fltrStatEv(18,trFrc),m17=fltrStatEv(17,trFrc);
		CPPUNIT_ASSERT(m17.size()==2);
		CPPUNIT_ASSERT(m17[3].typeDiap()==AEvent::MOVE && m17[5].typeDiap()==AEvent::MOVE );
		CPPUNIT_ASSERT(m18[3].typeDiap()==AEvent::MOVE && m18[5].typeDiap()==AEvent::MOVE );
		CPPUNIT_ASSERT(abs(m18[1].diap.getOrig()+defTimes.Ipk.count()-m18[3].diap.getOrig())< AAdmin::GapStop() );
		CPPUNIT_ASSERT(abs(m18[3].diap.getOrig()+defTimes.Ipk.count()-m18[5].diap.getOrig())< AAdmin::GapStop() );
	}
	{//  проверяем изменения из-за реальной нити
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn5));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV n18=fltrStatEv(18,trFrc),n17=fltrStatEv(17,trFrc);
		CPPUNIT_ASSERT(n17.size()==2);
		CPPUNIT_ASSERT(n17[3].typeDiap()==AEvent::MOVE && n17[5].typeDiap()==AEvent::MOVE );
		CPPUNIT_ASSERT(n18[3].typeDiap()==AEvent::MOVE && n18[5].typeDiap()==AEvent::MOVE );
		CPPUNIT_ASSERT(abs(m18[1].diap.getOrig()+defTimes.Ipk.count()-m18[3].diap.getOrig())< AAdmin::GapStop() );
		CPPUNIT_ASSERT(abs(m18[3].diap.getOrig()+defTimes.Ipk.count()-m18[5].diap.getOrig())< AAdmin::GapStop() );
	}
}

void TC_AThread::testMorphFrcReal()
{//превращение событий в прогнозе в зависимости от соседней реальной нити 
	TRACE("\r\n =========== testMorphFrcReal ============");
	Sevent sEv1(AEvent::REAL ,10/*начало нити*/,	1/*время на станции*/,55/*время на перегоне*/,5/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,2/*число станций*/);
	Sevent sEv3(AEvent::REAL ,30/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv2(AEvent::REAL ,40/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		10/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(10.f);
	defTimes.Iot=defTimes.Ipk=defTimes.Ipr=chrono::seconds(20);
	TC_Atopology tpl( tplParams );
	STT_Val pvx=generateSTT(2,30,1,3);
	tpl.setPvx(pvx);
	tpl.setStInt(defTimes);
 	{// не проверяем изменения из-за реальной нити
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
 		TestAAdmin::fnFltr.clear();
 		adm.add(tpl.add(trn1));//ПЕ реальное
 		adm.add(tpl.add(trn2));//ПЕ реальное
 		adm.add(tpl.add(trn3));//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		M_INT_EV m18=fltrStatEv(18,trFrc),m19=fltrStatEv(19,trFrc);
 		CPPUNIT_ASSERT(m18.size()==2 && m19.size()==3);
 		CPPUNIT_ASSERT(m18[3].getType()==AEvent::STOP_START && m18[2].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(m18[3].diap.enclose(m18[2].diap));
 		CPPUNIT_ASSERT(m19[2].getType()==AEvent::STOP_START && m19[1].getType()==AEvent::MOVE&& m19[3].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(m19[2].diap.enclose(m19[1].diap));
 	}
 	{// решаем  конфликт с встречным, но не проверяем изменения из-за реальной нити
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// фильтровать конфликты со встречными
 		adm.add(tpl.add(trn1));//ПЕ реальное
 		adm.add(tpl.add(trn2));//ПЕ реальное
 		adm.add(tpl.add(trn3));//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		M_INT_EV m18=fltrStatEv(18,trFrc),m19=fltrStatEv(19,trFrc);
 		CPPUNIT_ASSERT(m18.size()==2 && m19.size()==3);
 		CPPUNIT_ASSERT(m18[3].getType()==AEvent::MOVE && m18[2].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(m19[2].getType()==AEvent::STOP_START && m19[1].getType()==AEvent::MOVE&& m19[3].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(m19[2].diap.enclose(m19[1].diap) && m19[2].diap.enclose(m19[3].diap));
 	}
	{// превращаем изменения из-за реальной нити в изменения прохода по перегону
		TestAAdmin adm;
		ACounterInt::limit=10;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// фильтровать конфликты со встречными
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m18=fltrStatEv(18,trFrc),m19=fltrStatEv(19,trFrc);
		CPPUNIT_ASSERT(m18.size()==2 && m19.size()==3);
		CPPUNIT_ASSERT(m18[3].getType()==AEvent::MOVE && m18[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m18[3].diap.duration()<=AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m19[2].getType()==AEvent::STOP_START && m19[1].getType()==AEvent::MOVE&& m19[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m19[2].diap.enclose(m19[1].diap) && m19[2].diap.enclose(m19[3].diap));
	}
}

void TC_AThread::testReductionStInterval()
{// смягчение (уменьшение ) ст.интервалов при проверке их соблюдения 
	TRACE("\r\n =========== testReductionStInterval ============");
	Sevent sEv1(AEvent::REAL ,10/*начало нити*/,	1/*время на станции*/,55/*время на перегоне*/,0/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv2(AEvent::REAL ,15/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		9/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv4(AEvent::REAL ,10/*начало нити*/,120/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		8/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent tNrm1(AEvent::NONE,// тип событий
		10,// начало нити
		20,// время на станции
		40,// время на перегоне
		0,// время на последнем перегоне
		7, // нач.станция
		7// число станций
		);
	Sevent tNrm2(AEvent::NONE,// тип событий
		15,// начало нити
		20,// время на станции
		40,// время на перегоне
		0,// время на последнем перегоне
		9, // нач.станция
		7// число станций
		);
	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,3, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(15.f);
	defTimes.Iot=defTimes.Ipk=defTimes.Ipr=chrono::seconds(20);
	defTimes.tsk=chrono::seconds(25);
	defTimes.tn=chrono::seconds(6);
	TC_Atopology tpl( tplParams );
	STT_Val pvx=generateSTT(2,30,1,3);
	tpl.setPvx(pvx);
	tpl.setStInt(defTimes);
	auto norm2=tpl.norma(tNrm2,2);
	auto norm1=tpl.norma(tNrm1,1);
	{// не редуцируем интервалы . ПЕ 1 стоит на 19 из-за конфликта на 18
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm2 );
		adm.addNorma( trn1.tdescr, norm1 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m18=fltrStatEv(18,trFrc),m19=fltrStatEv(19,trFrc);
		CPPUNIT_ASSERT(m19[2].diap.getEnd()>m19[1].diap.getOrig());
		CPPUNIT_ASSERT(m18[1].diap.duration()==tNrm1.tSt && m18[2].diap.duration()==tNrm2.tSt);
		CPPUNIT_ASSERT(m18[4].diap.getEnd()==m18[1].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m19[1].diap.duration()==tNrm1.tSt && m19[2].diap.duration()>tNrm2.tSt);
		CPPUNIT_ASSERT(m19[1].diap.getOrig()+ defTimes.tsk.count()==m19[2].diap.getEnd());
	}
	{// не редуцируем интервалы . ПЕ 1 стоит на 19 из-за конфликта на 18
		TestReduction::defaultReduction=60;// ослабляем  требования по интервалам на 40%
		TestAAdmin::reductSt=AReduction();// понижение ст.интервалов при проверке конфликтов
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm2 );
		adm.addNorma( trn1.tdescr, norm1 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m18=fltrStatEv(18,trFrc),m19=fltrStatEv(19,trFrc);
		CPPUNIT_ASSERT(m19[2].diap.getEnd()==tNrm2.or+tNrm2.tSt);
		CPPUNIT_ASSERT(m18[1].diap.duration()==tNrm1.tSt);
		CPPUNIT_ASSERT( m18[2].diap.duration()>tNrm2.tSt );
		CPPUNIT_ASSERT( m18[4].diap.getEnd()>m18[1].diap.getOrig());
		int d=int((defTimes.tsk.count()*(int)(100-TestReduction::defaultReduction))/100);
		CPPUNIT_ASSERT( m18[4].diap.getEnd()==m18[1].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m18[1].diap.enclose(m18[2].diap.getOrig()));
		CPPUNIT_ASSERT(m18[2].diap.getOrig()<m18[1].diap.getOrig()+defTimes.tn.count());
		CPPUNIT_ASSERT( m18[2].diap.getEnd()==m18[4].diap.getEnd()+defTimes.Iot.count());
	}
	TestReduction::defaultReduction=0;// ослабляем  требования по интервалам на 40%
	TestAAdmin::reductSt=AReduction();// понижение ст.интервалов при проверке конфликтов
}

void TC_AThread::testModifyFrstStep()
{// тест изменения дотягивания с перегона 
	TRACE("\r\n =========== testModifyFrstStep ============");
	Sevent sEv1(AEvent::REAL ,30/*начало нити*/,	10/*время на станции*/,5/*время на перегоне*/,5/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv2(AEvent::REAL ,75/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		9/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(10.f);
	defTimes.Iot=defTimes.Ipk=defTimes.Ipr=chrono::seconds(20);
	TC_Atopology tpl( tplParams );
	STT_Val pvx=generateSTT(6,30,1,13);
	tpl.setPvx(pvx);
	tpl.setStInt(defTimes);
	{// меняем дотягивание из-за стоянки
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(17,trFrc), m18=fltrStatEv(18,trFrc);
		CPPUNIT_ASSERT(m[2].getType()==AEvent::MOVE  && m[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m18[2].getType()==AEvent::MOVE  && m18[1].getType()==AEvent::MOVE);
		auto t1=adm.get("1");
		auto ev=t1->origUnionFrc();
		CPPUNIT_ASSERT(ev.Esr()==EsrKit(16,17));
		CPPUNIT_ASSERT(ev.diap.duration()==pvx.pPass->start +pvx.pPass->full+ pvx.pPass->stop);
	}
	{//корректируем  изменение дотягивания из-за стоянки
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// фильтровать конфликты со встречными
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m17=fltrStatEv(17,trFrc), m18=fltrStatEv(18,trFrc);
		CPPUNIT_ASSERT(m17[1].getType()==AEvent::STOP_START  && m18[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m17[2].getType()==AEvent::MOVE  && m18[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m18[2].diap.enclose(m18[1].diap));
		CPPUNIT_ASSERT(m18[2].diap.getEnd()-m18[1].diap.getOrig()>defTimes.tsk.count());
		auto t1=adm.get("1");
		auto ev=t1->origUnionFrc();
		CPPUNIT_ASSERT(ev.Esr()==EsrKit(16,17));
		CPPUNIT_ASSERT(ev.diap.duration()==pvx.pPass->start +pvx.pPass->full+pvx.pPass->stop);
	}
	{//корректируем  изменение дотягивания из-за стоянки + обратное согласование
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// фильтровать конфликты со встречными
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m17=fltrStatEv(17,trFrc), m18=fltrStatEv(18,trFrc);
		CPPUNIT_ASSERT(m17[1].getType()==AEvent::STOP_START  && m18[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m17[2].getType()==AEvent::MOVE  && m18[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m18[2].diap.enclose(m18[1].diap));
		CPPUNIT_ASSERT(m18[2].diap.getEnd()-m18[1].diap.getOrig()<defTimes.tsk.count()+ 2*AAdmin::GapStop());
		auto t1=adm.get("1");
		auto ev=t1->origUnionFrc();
		CPPUNIT_ASSERT(ev.Esr()==EsrKit(16,17));
		CPPUNIT_ASSERT(ev.diap.duration()==pvx.pPass->start +pvx.pPass->full+pvx.pPass->stop);
	}
}

void TC_AThread::testMorphConflReal()
{//превращение событий в прогнозе в зависимости от соседней реальной нити при конфликте
	TRACE("\r\n =========== testMorphConflReal ============");
	Sevent sEv1(AEvent::REAL ,10/*начало нити*/,	1/*время на станции*/,55/*время на перегоне*/,0/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,2/*число станций*/);
	Sevent sEv3(AEvent::REAL ,40/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv2(AEvent::REAL ,10/*начало нити*/,1/*время на станции*/,20/*время на перегоне*/,10/*время на последнем перегоне*/,
		10/*нач.станция (+10 в конструкторе)*/,2/*число станций*/);

	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(10.f);
	defTimes.Iot=defTimes.Ipk=defTimes.Ipr=chrono::seconds(20);
	TC_Atopology tpl( tplParams );
	STT_Val pvx=generateSTT(2,30,1,3);
	tpl.setPvx(pvx);
	tpl.setStInt(defTimes);
	{// не проверяем изменения из-за реальной нити
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(18,trFrc);
		CPPUNIT_ASSERT(m.size()==3);
		CPPUNIT_ASSERT( std::count_if(m.begin(),m.end(),[](const  INT_EV& t){return t.second.getType()==AEvent::STOP_START ;})==3);
		CPPUNIT_ASSERT(m[2].diap.enclose(m[1].diap.getEnd()));
		CPPUNIT_ASSERT(m[2].diap.enclose(m[3].diap.getOrig()));
		CPPUNIT_ASSERT(m[3].diap.enclose(m[2].diap.getEnd()));
		CPPUNIT_ASSERT(m[3].diap.getEnd()-m[1].diap.getEnd()>defTimes.Iot.count()- AAdmin::GapStop());
		CPPUNIT_ASSERT(m[3].diap.getEnd()-m[1].diap.getEnd()<defTimes.Iot.count() + AAdmin::GapStop());
	}
	{// превращаем изменения из-за реальной нити в изменения прохода по перегону
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// фильтровать конфликты со встречными
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(18,trFrc);
		CPPUNIT_ASSERT( std::count_if(m.begin(),m.end(),[](const  INT_EV& t){return t.second.getType()==AEvent::STOP_START ;})==3);
		CPPUNIT_ASSERT(m[2].diap.enclose(m[1].diap.getEnd()));
		CPPUNIT_ASSERT(m[2].diap.enclose(m[3].diap.getOrig()));
		CPPUNIT_ASSERT(m[3].diap.getEnd()-m[1].diap.getEnd()>defTimes.Iot.count()- AAdmin::GapStop());
		CPPUNIT_ASSERT(m[3].diap.getEnd()-m[1].diap.getEnd()<defTimes.Iot.count() + AAdmin::GapStop());
	}
}


