/*!
	created:	2019/06/04
	created: 	08:55  04 Июнь 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrBackAjust.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrBackAjust
	file ext:	cpp
	author:		 Dremin
	
	purpose:	обратное согласование после выполнения задания
  
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Hem/RouteIntrusion.h"
using namespace std;

void TC_AThread::testManyBackAdjust()
{ // множественное  согласование ПЕ с другой и Третьей Пе с первой
	TRACE("\r\n =========== testManyBackAdjust ============");
	Sevent sEv1(AEvent::REAL ,10+50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv3(AEvent::REAL ,190/*начало нити*/,	1/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv2(AEvent::REAL ,75/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(40,&defTimes);
    tpl.addSpanInterval( EsrKit(15,16), 80);
	{// строим конфликты
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m5=fltrStatEv(15,trFrc),m4=fltrStatEv(14,trFrc),m6=fltrStatEv(16,trFrc);
		CPPUNIT_ASSERT(m5[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m5[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m6[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m6[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m6[2].diap.duration()>100);
		CPPUNIT_ASSERT(m6[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m4[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m4[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m4[3].diap.duration()>50);
	}
	{// решаем конфликты
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t1= adm.fnd(trn1);
		auto t2= adm.fnd(trn2);
		auto t3= adm.fnd(trn3);
		auto r=t1->Reason(15);
		CPPUNIT_ASSERT(r);
		CPPUNIT_ASSERT(r->banTrn.empty());
		CPPUNIT_ASSERT(r->first==AClaim::ShiftCnfl);
		CPPUNIT_ASSERT(r->second==t2);
		r=t2->Reason(15);
		CPPUNIT_ASSERT(r && "задание создано");
		CPPUNIT_ASSERT(r->banTrn.empty());
		CPPUNIT_ASSERT(r->first==AClaim::ShiftCnfl);
		CPPUNIT_ASSERT(r->second==t3);
		M_INT_EV m5=fltrStatEv(15,trFrc),m4=fltrStatEv(14,trFrc),m3=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m5[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m5[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m5[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m5[1].diap.getEnd()>=m5[2].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m4[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m4[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m4[2].diap.duration()>15);
		CPPUNIT_ASSERT(m3[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m3[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m3[3].diap.getEnd()<m3[2].diap.getOrig()+100);
	}
	{// решаем конфликты и согласовываем
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::useBackAdjust=true;
 		TestAAdmin::fnFltr.clear();
 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
 		adm.add(tpl.add(trn1));//ПЕ реальное
 		adm.add(tpl.add(trn2));//ПЕ реальное
 		adm.add(tpl.add(trn3));//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		M_INT_EV m5=fltrStatEv(15,trFrc),m4=fltrStatEv(14,trFrc),m3=fltrStatEv(13,trFrc);
		auto t1= adm.fnd(trn1);
		auto t2= adm.fnd(trn2);
		auto r=t1->Reason(15);
		CPPUNIT_ASSERT(r);
		CPPUNIT_ASSERT(r->banTrn.empty());
		CPPUNIT_ASSERT(r->first==AClaim::ShiftCnfl);
		CPPUNIT_ASSERT(r->second==t2);
		CPPUNIT_ASSERT(!t2->Reason(15) && "задание снято");

		CPPUNIT_ASSERT(t1->banTrain(14).empty());
		CPPUNIT_ASSERT(m5[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m5[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m5[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m5[1].diap.getEnd()>=m5[2].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m5[1].diap.getEnd()<=m5[2].diap.getOrig()+defTimes.tsk.count()+AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m4[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m4[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m3[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m3[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m3[3].diap.getEnd()>=m3[2].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m3[3].diap.getEnd()<=m3[2].diap.getOrig()+defTimes.tsk.count()+AAdmin::deltaThrough());
	}
}

void TC_AThread::testBackAdjust()
{// тест обратной коррекции после конфликта
	TRACE("\r\n =========== testBackAdjust ============");
	Sevent sEv1(AEvent::REAL ,10+50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv2(AEvent::REAL ,75/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	{// построен конфликт
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(16,trFrc);
		CPPUNIT_ASSERT(m[1].diap.duration()==AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m[2].diap.duration()>100 && " есть конфликт ");
		auto t1=adm.fnd(trn1.tdescr);
		st_intervals_t s=t1->getStInterval(16,false);// ст.интервалы для попутного(true)/встречного (false)
		CPPUNIT_ASSERT(m[2].diap.getEnd()-m[1].diap.getOrig()==s.tsk.count());
	}
	{//  показывать конфликты + решение конфликта
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		ACounterInt::limit=2; 
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам встречных
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(15,trFrc);
		auto t1=adm.fnd(trn1.tdescr);
		st_intervals_t s=t1->getStInterval(15,false);// ст.интервалы для попутного(true)/встречного (false)
 		CPPUNIT_ASSERT(m[2].diap.duration()==AAdmin::deltaThrough());
 		CPPUNIT_ASSERT(m[1].diap.duration()>20 && "конфликт решен и нужна коррекция ");
		CPPUNIT_ASSERT(m[2].diap.getOrig()-m[1].diap.getOrig()>s.tsk.count());
		CPPUNIT_ASSERT(m[2].diap.getOrig()-m[1].diap.getOrig()<=s.tsk.count()+ AAdmin::GapStop());
	}
	V_EVNT v1,v2;
	{//  показывать конфликты + решение конфликта + обратная коррекция
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		ACounterInt::limit=3; 
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам встречных
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(15,trFrc);
		auto t1=adm.fnd(trn1.tdescr);
		auto t2=adm.fnd(trn2.tdescr);
		st_intervals_t s=t1->getStInterval(16,false);// ст.интервалы для попутного(true)/встречного (false)
		CPPUNIT_ASSERT(m[2].diap.duration()==AAdmin::deltaThrough());
 		CPPUNIT_ASSERT(m[1].diap.duration()<20);
 		CPPUNIT_ASSERT(m[2].diap.getOrig()-m[1].diap.getOrig()>s.tsk.count());
		CPPUNIT_ASSERT(m[2].diap.getOrig()-m[1].diap.getOrig()<=s.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m[1].diap.getEnd()-m[2].diap.getEnd()<=s.tsk.count()+ AAdmin::GapStop() && " коррекция прошла и поменяла прогноз ");
		v1=t1->get(AEvent::FRC);
		v2=t2->get(AEvent::FRC);
	}
	{//  показывать конфликты + решение конфликта + обратная коррекция+ проверка + тест на отсутствие коррекции
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		ACounterInt::limit=5; 
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам встречных
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t1=adm.fnd(trn1.tdescr);
		auto t2=adm.fnd(trn2.tdescr);
		V_EVNT z1=t1->get(AEvent::FRC);
		V_EVNT z2=t2->get(AEvent::FRC);
		CPPUNIT_ASSERT(z1==v1 && "шаги после коррекции не меняют прогноз ");
		CPPUNIT_ASSERT(z2==v2 && "после коррекции не меняется прогноз ");
	}
}

void TC_AThread::testBackAdjustIntrusion()
{// тест // обратное согласование интрузии 
	TRACE("\r\n =========== testBackAdjustIntrusion ============");
	Sevent sEv4(AEvent::REAL ,10+50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		5/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv2(AEvent::REAL ,75/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,10/*время на последнем перегоне*/,
		4/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent tNrm2(AEvent::NONE,// тип событий
		20,// начало нити
		0,// время на станции
		60,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция
		6// число станций
		);

	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	defTimes.Iot= chrono::seconds(5);
	defTimes.Ipk=chrono::seconds(15);

	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	auto norm2=tpl.norma(tNrm2,2);
	map<EsrKit, RouteIntrusion::StationIntrusion> mIntrusions;
	RouteIntrusion::PassingOrderPtr passingOrderPtr( new RouteIntrusion::PassingOrder( TrainDescrPtr( new TrainDescr(trn2.tdescr) ) ) );
	mIntrusions[ EsrKit( 13 )]=RouteIntrusion::StationIntrusion( passingOrderPtr);
	RouteIntrusion rIntr(trn4.tdescr, vector<EsrKit>(), mIntrusions, map<EsrKit, RouteIntrusion::SpanIntrusion>() );
	TestAAdmin::VRouteIntrusionPtr v(new std::vector<RouteIntrusion>());
	v->push_back(rIntr);
	{// построен без интрузии
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn4));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[2].diap.enclose(m13[4].diap));
		CPPUNIT_ASSERT(m13[2].diap.getEnd()-m13[4].diap.getEnd()==defTimes.Iot.count());
		CPPUNIT_ASSERT(m13[2].diap.duration()>40 && m13[2].diap.duration()<50);
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE);
	}
	{// построен c интрузией
		TestAAdmin adm;
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn4),v);//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[4].diap.duration()>15);
		CPPUNIT_ASSERT(m13[4].diap.getOrig()>m13[2].diap.getEnd());
		CPPUNIT_ASSERT(m13[2].diap.duration()>=30 && m13[2].diap.duration()<=30+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE);
	}
	{// построен c интрузией и обратным согласованием
		TestAAdmin adm;
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		TestAAdmin::deltaIntrusion=1;
		adm.add(tpl.add(trn4),v);//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[4].diap.duration()>AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m13[4].diap.duration()<=AAdmin::deltaThrough()+  AAdmin::GapStop()+1);
		CPPUNIT_ASSERT(m13[4].diap.getOrig()>m13[2].diap.getEnd());
		CPPUNIT_ASSERT(m13[2].diap.duration()>=30 && m13[2].diap.duration()<=30+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE);
	}
}

void TC_AThread::testBackAdjustSpan()
{// обратное  согласование при морфинге на перегоне
	// однопутка, 3 пути на станции
	TC_AtopologyParams tplParams( ForkTopology::ONE_FORK, stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN);
	st_intervals_t defTimes(1.f);
	defTimes.Ipk=chrono::seconds(30);
	defTimes.Iot=chrono::seconds(10);
	defTimes.Ipr=chrono::seconds(17);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	auto path10_17 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit( 17 ) ) );// получаем один маршрут
	auto path10_24 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit( 24 ) ) );// получаем один маршрут
	auto path10_180 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit(180 ) ) );// получаем один маршрут
	auto path20_180 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(20 ), EsrKit(180 ) ) );// получаем один маршрут
	CPPUNIT_ASSERT(path20_180.size()==21 && path10_180.size()==29 &&path10_24.size()==29 && path10_17.size()==15   );
/*        
          /  18-24
	10-17
	     \   25-30-180
*/
	//реальная нить
	Sevent sEv1(AEvent::REAL,370/*начало нити*/,5/*на станции*/,20/*на перегоне*/,0,7/*нач.станция*/,1/*число станций*/) ;
	Sevent sEv4(AEvent::REAL,380/*начало нити*/,15/*на станции*/,20/*на перегоне*/,0,9/*нач.станция*/,1/*число станций*/) ;
	Sevent sEv2(AEvent::REAL,230/*начало нити*/,5/*на станции*/,20/*на перегоне*/,0,16/*нач.станция*/,1/*число станций*/) ;

	Strain trn2( TrainDescr( L"2", L"1-100-1" ),	sEv2, Sevent( NO_FIXED )	);
	Strain trn4( TrainDescr( L"4", L"1-100-1" ),	sEv4, Sevent( NO_FIXED )	);
	Strain trn1( TrainDescr( L"1", L"1-100-3" ),	sEv1, Sevent( NO_FIXED )	);
 	{// без морфинга
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;
		ATrainPtr thr(tpl.add(trn1));
		adm.add(thr);//ПЕ реальное
		STT_Cat_Val pvx=thr->getPMT(27,28);

		adm.add(tpl.add(trn2));//ПЕ реальное
 		adm.add(tpl.add(trn4));//ПЕ реальное
 		auto v=adm.forecastGlobal();
 		tstTRACE(adm);
		M_INT_EV m17= fltrStatEv(17,v),m16= fltrStatEv(16,v),m18=fltrStatEv(18,v);
 		CPPUNIT_ASSERT( m17[1].typeDiap()==AEvent::MOVE);
 		CPPUNIT_ASSERT(  m17[2].getType()==AEvent::MOVE&& m17[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m17[4].diap.getOrig()-m18[4].diap.getEnd()==pvx.full+pvx.stop && "торможение по ПВХ");
		CPPUNIT_ASSERT( m17[2].diap.getEnd()<m17[4].diap.getOrig());
 		CPPUNIT_ASSERT( m16[2].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT( m16[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m17[4].diap.getEnd()-m17[2].diap.getEnd()<defTimes.Ipk.count()+ AAdmin::GapStop());
		//CPPUNIT_ASSERT(m17[4].diap.getEnd()-m17[2].diap.getEnd()>defTimes.Iot.count());
 	}
	{// c морфингом
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;
 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);// ф-ция  отбора заданий по обгонам
		ATrainPtr thr(tpl.add(trn1));
		adm.add(thr);//ПЕ реальное
		STT_Cat_Val pvx=thr->getPMT(27,28);
 		adm.add(tpl.add(trn2));//ПЕ реальное
 		adm.add(tpl.add(trn4));//ПЕ реальное
 		auto v=adm.forecastGlobal();
 		tstTRACE(adm);
		M_INT_EV m17= fltrStatEv(17,v),m16= fltrStatEv(16,v),m18=fltrStatEv(18,v);
 		CPPUNIT_ASSERT( m17[1].typeDiap()==AEvent::MOVE);
 		CPPUNIT_ASSERT(  m17[2].getType()==AEvent::MOVE&& m17[4].getType()==AEvent::STOP_START);
// 		CPPUNIT_ASSERT(m17[4].diap.getOrig()-m18[4].diap.getEnd()>pvx.full+pvx.stop+ AAdmin::GapStop() && " проследование не по ПВХ");
 		CPPUNIT_ASSERT(abs(m17[4].diap.getEnd()-m17[2].diap.getEnd()-defTimes.Ipk.count())< AAdmin::GapStop()+AAdmin::deltaThrough());
 		CPPUNIT_ASSERT( m16[2].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT( m16[4].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(abs(m16[4].diap.getOrig()-m16[2].diap.getEnd()-defTimes.Ipk.count())< AAdmin::GapStop()+AAdmin::deltaThrough());	
	}
	{// c морфингом и конфликтом встречных
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);// ф-ция  отбора заданий по обгонам
		ATrainPtr thr(tpl.add(trn1));
		adm.add(thr);//ПЕ реальное
		STT_Cat_Val pvx=thr->getPMT(27,28);
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m17= fltrStatEv(17,v),m16= fltrStatEv(16,v),m18=fltrStatEv(18,v);
		CPPUNIT_ASSERT( m17[1].typeDiap()==AEvent::STOP_START);
		CPPUNIT_ASSERT(  m17[2].getType()==AEvent::MOVE&& m17[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m17[4].diap.getOrig()-m18[4].diap.getEnd()==pvx.full && " проследование по ПВХ");
		CPPUNIT_ASSERT(m17[4].diap.getEnd()-m17[2].diap.getEnd()>50);
		CPPUNIT_ASSERT( m16[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m16[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m16[4].diap.getEnd()-m16[2].diap.getEnd()==m17[4].diap.getEnd()-m17[2].diap.getEnd());	
	}
	{// c морфингом и конфликтом встречных и обратное согласование
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);// ф-ция  отбора заданий по обгонам
		TestAAdmin::useBackAdjust=true;
		ACounterInt::limit=10;
		ATrainPtr thr(tpl.add(trn1));
		adm.add(thr);//ПЕ реальное
		STT_Cat_Val pvx=thr->getPMT(27,28);
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		// 		adm.add(tpl.add(trn6));//ПЕ реальное
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m17= fltrStatEv(17,v),m16= fltrStatEv(16,v),m18=fltrStatEv(18,v);
		CPPUNIT_ASSERT( m17[1].typeDiap()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m17[1].diap.duration()<AAdmin::deltaThrough()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(  m17[2].getType()==AEvent::MOVE&& m17[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m17[4].diap.getOrig()-m18[4].diap.getEnd()==pvx.full && "по ПВХ");
		CPPUNIT_ASSERT( m16[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m16[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m16[4].diap.getEnd()-m16[2].diap.getEnd()==m17[4].diap.getEnd()-m17[2].diap.getEnd());	
	}
}
