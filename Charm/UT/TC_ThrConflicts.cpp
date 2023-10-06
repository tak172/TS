/*!
	created:	2019/04/26
	created: 	14:36  26 Апрель 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrConflicts.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrConflicts
	file ext:	cpp
	author:		 Dremin
	
	purpose:	множественные конфликты с взаимовлиянием
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "TC_statPathProp.h"
using namespace std;

void TC_AThread::testInflConfl()
{//конфликты с взаимовлиянием (ликвидируем  попутный обгон при решении встречного конфликта)
	TRACE("\r\n =========== testMorphFrcReal ============");
	Sevent sEv1(AEvent::REAL ,10/*начало нити*/,	1/*время на станции*/,2/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv2(AEvent::REAL ,50/*начало нити*/,1/*время на станции*/,2/*время на перегоне*/,0/*время на последнем перегоне*/,
		2/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv3(AEvent::REAL ,30/*начало нити*/,1/*время на станции*/,2/*время на перегоне*/,0/*время на последнем перегоне*/,
		2/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv4(AEvent::REAL ,60/*начало нити*/,1/*время на станции*/,2/*время на перегоне*/,0/*время на последнем перегоне*/,
		4/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	trn1.tdescr.SetBrutto(AAdmin::Advantage_brutto()+1);
	trn4.tdescr.SetBrutto(AAdmin::Advantage_brutto()+1);

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,5, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	defTimes.Iot=chrono::seconds(7);
	defTimes.Ipr=chrono::seconds(5);
	defTimes.Ipk=chrono::seconds(12);
	TC_Atopology tpl( tplParams );
	STT_Val pvx=generateSTT(1,50,1,1);
	STT_Val pvx3_4=generateSTT(1,50,1,20);
	TC_Atopology::MAP_SPANTIME m;
	m[make_pair(12,13)]=pvx3_4;
	tpl.setPvx(pvx,m);
	tpl.setStInt(defTimes);
	{// проверяем конфликт с встречным и попутным
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам встречных
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по обгонам попутных
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
		TestAAdmin::maxWaitCounterPack=50;
		ACounterInt::limit=10;

		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT( m13[3].diap.getEnd()<m13[1].diap.getOrig() && "убран обгон");
		CPPUNIT_ASSERT(m13[3].diap.enclose(m13[4].diap.getOrig()) && m13[4].diap.enclose(m13[1].diap.getOrig()));
		CPPUNIT_ASSERT(m12[2].diap.duration()>10 && m12[2].diap.enclose(m12[1].diap.getOrig()));
		CPPUNIT_ASSERT(m14[4].diap.getEnd()<m14[3].diap.getOrig());
		CPPUNIT_ASSERT( m14[3].diap.getEnd()<m14[1].diap.getOrig() );
	}
 	{// не проверяем конфликты
  		TestAAdmin adm;
  		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
  		TestAAdmin::fnFltr.clear();
   		adm.add(tpl.add(trn3));//ПЕ реальное
  		adm.add(tpl.add(trn4));//ПЕ реальное
 		adm.add(tpl.add(trn2));//ПЕ реальное
  		adm.add(tpl.add(trn1));//ПЕ реальное
  		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
  		tstTRACE(adm);
  		M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc)
  			,m13=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
  		CPPUNIT_ASSERT(m12.size()==4 && m13.size()==3 && m14.size()==3);
  		CPPUNIT_ASSERT(m14[4].diap.duration()>50 && m14[4].diap.enclose(m14[3].diap.getOrig()));
  		CPPUNIT_ASSERT(m13[4].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT( m12[1].diap.enclose(m12[4].diap.getOrig()) && m12[1].diap.duration()>50);
  		CPPUNIT_ASSERT(m11[1].diap.duration()>50 && m11[1].diap.enclose(m11[2].diap.getOrig()));
 	}
	{// проверяем конфликт с встречным
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам встречных
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
		ACounterInt::limit=10;
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT( m13[3].diap.enclose(m13[1].diap) && "прошел обгон");
		CPPUNIT_ASSERT(m13[3].diap.enclose(m13[4].diap.getOrig()));
		CPPUNIT_ASSERT(m12[2].diap.duration()>10 && m12[2].diap.enclose(m12[1].diap.getOrig()));
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::MOVE  && m13[1].getType()==AEvent::MOVE);
	}
}

void TC_AThread::testPackConflNextStation()
{//конфликт с пакетом решается на др.станции и проверкой попутных обгонов
	TRACE("\r\n =========== testPackConflNextStation ============");
	Sevent sEv1(AEvent::REAL ,20/*начало нити*/,	1,2,0,1,1);
	Sevent sEv3(AEvent::REAL ,30/*начало нити*/,	1,2,0,1,1);
	Sevent sEv5(AEvent::REAL ,50/*начало нити*/,	1,2,0,0,1);
	Sevent sEv7(AEvent::REAL ,85/*начало нити*/,	1,2,0,0,1);
	Sevent sEv9(AEvent::REAL ,95/*начало нити*/,	1,2,0,0,1);
	Sevent sEv2(AEvent::REAL ,10/*начало нити*/,1,2,0,5/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	
	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn5( TrainDescr( L"5", L"1-1-2"),sEv5, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn7( TrainDescr( L"7", L"1-1-2"),sEv7, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn9( TrainDescr( L"9", L"1-1-2"),sEv9, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	trn5.tdescr.SetRelLength(100);

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,5, 1, SpanBreadth::ONE_WAY_SPAN ); 

	//------------ на 18 нет стоянки ПЕ для 2 -------------------
	for (int i = 1; i < 6; i++)
		tplParams.add(12,i,TC_StatPath(passMaj ,99));//пасс+ гл до 99 вагонов

	st_intervals_t defTimes(1.f);
	//defTimes.Iot=defTimes.Ipk=defTimes.Ipr=chrono::seconds(10);
	defTimes.Ipk=chrono::seconds(10);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(30);
	tpl.setStInt(defTimes);

	{// проверяем конфликты по новому
		TestAAdmin adm;
		adm.numStepFrc.limit=20;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::deltaWait=10;//зазор для расчета при ликвидации обгона 
		TestAAdmin::fnFltr.clear();
		TestAAdmin::useBackAdjust=true;
		adm.maxWaitCounterPack=40 ;// время максимального ожидания при пропуске встречного пакета ПЕ

		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn5));//ПЕ реальное
		adm.add(tpl.add(trn7));//ПЕ реальное
		adm.add(tpl.add(trn9));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12.size()==6 && m13.size()==6 );
		CPPUNIT_ASSERT(count_if(m12.begin(),m12.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==5);
		CPPUNIT_ASSERT(m11[5].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m11[5].diap.getEnd()<m11[7].diap.getEnd() );
		CPPUNIT_ASSERT(m11[7].diap.getEnd()<m11[9].diap.getEnd() );
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m12[2].diap.enclose(m12[5].diap));
		CPPUNIT_ASSERT( m12[2].diap.enclose(m12[9].diap.getOrig()));
		CPPUNIT_ASSERT(count_if(m13.begin(),m13.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==5);
		CPPUNIT_ASSERT(m13[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[1].diap));
		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[3].diap.getOrig()));
	}
	{// не проверяем конфликты
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn5));//ПЕ реальное
		adm.add(tpl.add(trn7));//ПЕ реальное
		adm.add(tpl.add(trn9));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12.size()==6 && m13.size()==6 );
		CPPUNIT_ASSERT(count_if(m12.begin(),m12.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==6);
		CPPUNIT_ASSERT(count_if(m13.begin(),m13.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==5);
		CPPUNIT_ASSERT(m13[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[1].diap));
		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[9].diap.getOrig()));
	}	
// 	{//  проверяем конфликты
// 		TestAAdmin adm;
// 		adm.numStepFrc.limit=20;
// 		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
// 		TestAAdmin::deltaWait=10;//зазор для расчета при ликвидации обгона 
// 		TestAAdmin::fnFltr.clear();
// 		TestAAdmin::useBackAdjust=true;
// 		TestAAdmin::backAjustNew=false;
// 		adm.maxWaitCounterPack=40 ;// время максимального ожидания при пропуске встречного пакета ПЕ
// 
// 		adm.add(tpl.add(trn1));//ПЕ реальное
// 		adm.add(tpl.add(trn3));//ПЕ реальное
// 		adm.add(tpl.add(trn5));//ПЕ реальное
// 		adm.add(tpl.add(trn7));//ПЕ реальное
// 		adm.add(tpl.add(trn9));//ПЕ реальное
// 		adm.add(tpl.add(trn2));//ПЕ реальное
// 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
// 		tstTRACE(adm);
// 		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
// 		CPPUNIT_ASSERT(m12.size()==6 && m13.size()==6 );
// 		CPPUNIT_ASSERT(count_if(m12.begin(),m12.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==6);
// 		CPPUNIT_ASSERT(count_if(m13.begin(),m13.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==5);
// 		CPPUNIT_ASSERT(m13[2].getType()==AEvent::STOP_START);
// 		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[1].diap));
// 		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[9].diap.getOrig()));
// 	}

}

