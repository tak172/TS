/*!
	created:	2019/05/21
	created: 	11:48  21 Май 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrStation.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrStation
	file ext:	cpp
	author:		 Dremin
	
	purpose:	тесты для прогноза станционных событий
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "TC_statPathProp.h"
using namespace std;

void TC_AThread::testAbsenseStationPath()
{// отсутствие пути для стоянки
	TRACE("\r\n =========== testAbsenseStationPath  ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 1 путь на станции
	//------------ на 12 стоянка запрещена -------------------
	tplParams.add(12,1,TC_StatPath(noStopPr));
	TC_Atopology tpl( tplParams );
	tpl.setPvx(40);
	Sevent sEv1(AEvent::REAL ,10/*начало нити*/,	1/*время на станции*/,0/*время на перегоне*/,0/*время на последнем перегоне*/,
		0/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv(AEvent::REAL ,0/*начало нити*/,	400/*время на станции*/,0/*время на перегоне*/,0/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn( TrainDescr(),sEv, Sevent( NO_FIXED ));
	TestAAdmin adm;
	adm.add(tpl.add(trn1));//ПЕ реальное
	adm.add(tpl.add(trn,1));//ПЕ реальное
	auto trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto t0=adm.fnd(trn.tdescr);
	CPPUNIT_ASSERT(adm.getClash().empty());
	M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
	CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE && m12[1].getType()==AEvent::MOVE);
	CPPUNIT_ASSERT(m11[1].getType()==AEvent::STOP_START && "стоянка перенесена");
	auto ev=t0->fndEv(EsrKit(13));
	CPPUNIT_ASSERT(ev.diap==ADiap(0,200));
	CPPUNIT_ASSERT(m13[1].diap.getOrig()>ev.diap.getEnd());
	CPPUNIT_ASSERT(m13[1].diap.getOrig()<=ev.diap.getEnd()+ AAdmin::GapStop());
}

void TC_AThread::testManagePath()
{//менеджер распределения путей на станции (станц.диспетчер)
	TRACE("\r\n =========== testTopStart ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 3 пути на станции
	tplParams.add(13,2,TC_StatPath(noStopPr));
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(float(1.0));
	STT_Val s;
	s.set_gruz(4,30,0,5);
	tpl.setPvx(s);
	tpl.setStInt(defTimes);
	Sevent sEv1(AEvent::REAL ,30/*начало нити*/,	1/*время на станции*/,2/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv3(AEvent::REAL ,50/*начало нити*/,	1/*время на станции*/,2/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv13(AEvent::REAL ,10/*начало нити*/,	10/*время на станции*/,2/*время на перегоне*/,0/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Sevent sEv2(AEvent::REAL ,50/*начало нити*/,	1/*время на станции*/,40/*время на перегоне*/,10/*время на последнем перегоне*/,
		4/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv4(AEvent::REAL ,1/*начало нити*/,	1/*время на станции*/,40/*время на перегоне*/,0/*время на последнем перегоне*/,
		5/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn13( TrainDescr( L"8013", L"1-1-2"),sEv13, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn2( TrainDescr( L"6002", L"1-1-1"),sEv2, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn4( TrainDescr( L"6004", L"1-1-1"),sEv4, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	{// без решения обгонов
		TestAAdmin adm;
		adm.noSort=true;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		adm.add(tpl.add(trn13));//ПЕ реальное
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v=adm.getClash();
		CPPUNIT_ASSERT(v.size()<2);
		CPPUNIT_ASSERT(all_of(v.begin(),v.end(),[](const P_CLSH& t){return t->significance()==PrognosisErrorCode::SINGLE;})) ;
		M_INT_EV m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[6004].diap.enclose(m13[6002].diap.getOrig()));
		CPPUNIT_ASSERT(m13[8013].diap.enclose(m13[6002].diap.getOrig()));
		CPPUNIT_ASSERT(m13[8013].diap.getEnd()>=defTimes.tsk.count()+ m13[6002].diap.getOrig());
		CPPUNIT_ASSERT(m13[8013].diap.getEnd()<=defTimes.tsk.count()+ m13[6002].diap.getOrig()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[8013].equ(m13[1]));
		CPPUNIT_ASSERT(m13[8013].equ(m13[3]));
		CPPUNIT_ASSERT(!m13[8013].equ(m13[6002]));
		CPPUNIT_ASSERT(!m13[8013].equ(m13[6004]));
		CPPUNIT_ASSERT(!m13[6004].equ(m13[6002]));
	}
	{// с решением обгонов
		TestAAdmin adm;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по попутным обгонам
		adm.noSort=true;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn4));//ПЕ реальное
		adm.add(tpl.add(trn13));//ПЕ реальное
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v=adm.getClash();
		CPPUNIT_ASSERT(v.size()<2);
		CPPUNIT_ASSERT(all_of(v.begin(),v.end(),[](const P_CLSH& t){return t->significance()==PrognosisErrorCode::SINGLE;})) ;
		M_INT_EV m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[6004].diap.enclose(m13[6002].diap.getOrig()));
		CPPUNIT_ASSERT(m13[6002].diap.enclose(m13[6004].diap.getEnd()));
		CPPUNIT_ASSERT(m13[8013].diap.enclose(m13[6002].diap.getOrig()));
		CPPUNIT_ASSERT(m13[8013].diap.getEnd()>=defTimes.tsk.count()+ m13[6002].diap.getOrig());
		CPPUNIT_ASSERT(m13[8013].diap.getEnd()<=defTimes.tsk.count()+ m13[6002].diap.getOrig()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[8013].equ(m13[1]));
		CPPUNIT_ASSERT(m13[8013].equ(m13[3]));
		CPPUNIT_ASSERT(!m13[8013].equ(m13[6002]));
		CPPUNIT_ASSERT(!m13[8013].equ(m13[6004]));
		CPPUNIT_ASSERT(!m13[6004].equ(m13[6002]));
	}
}

void TC_AThread::testTopMove()
{	// учет Тор при невозможности стоянки на станции
	TRACE("\r\n =========== testTopMove ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 3 пути на станции
// 	tplParams.add(12,1,TC_StatPath(noStopPr));
	tplParams.setCapacity(12,3);// установка емкости путей на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(float(1.0));

	Sevent sEv4(//реальная нить
		AEvent::REAL,// тип событий
		35,// начало нити
		10, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent sv(//реальная нить
		AEvent::REAL,// тип событий
		1,// начало нити
		1000, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		1// число станций
		);


	Sevent sEv2002(//реальная нить
		AEvent::REAL,// тип событий
		150,// начало нити
		10, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		5, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	
	Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
	trn2002.tdescr.SetRelLength(5);
	Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED ));
	Strain trnx( TrainDescr( L"", L""),sv, Sevent( NO_FIXED ));

	Sevent tNrm4(AEvent::NONE/*тип событий*/,44/*начало нити*/,30/* время на станции*/,20/* время на перегоне*/,0/*время на последнем перегоне*/
		,5/*нач.станция*/,6/* число станций*/);
	auto norm4=tpl.norma(tNrm4,2);
	{// превышаем Тор =>  пытаемся сделать стоянку
		defTimes.Top = std::chrono::seconds(40);
		tpl.setPvx(10,&defTimes);
		Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED ));
		Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
		trn2002.tdescr.SetRelLength(5);
		TestAAdmin adm;
		adm.add(tpl.add(trn4));//ПЕ реальное
		adm.add(tpl.add(trn2002));//ПЕ реальное
		adm.add(tpl.add(trnx));//ПЕ реальное
		adm.addNorma( trn4.tdescr, norm4 );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc), m11=fltrStatEv(11,trFrc);
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[6002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[6002].diap.getOrig()>m12[4].diap.getEnd()+defTimes.Top.count());
		CPPUNIT_ASSERT(m12[6002].diap.getOrig()<=m12[4].diap.getEnd()+defTimes.Top.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m11[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m11[6002].diap.duration()<=AAdmin::GapStop()+ AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m11[6002].diap.getOrig()>m11[4].diap.getEnd());
		CPPUNIT_ASSERT(m11[6002].diap.getOrig()-m11[4].diap.getEnd()<defTimes.Top.count());
	}
	{// превышаем Тор =>  пытаемся сделать стоянку
		defTimes.Top = std::chrono::seconds(10);
		tpl.setPvx(10,&defTimes);
		TestAAdmin adm;
		adm.add(tpl.add(trn4));//ПЕ реальное
		adm.add(tpl.add(trn2002));//ПЕ реальное
		adm.addNorma( trn4.tdescr, norm4 );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[6002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[6002].diap.getOrig()>m12[4].diap.getEnd()+defTimes.Top.count());
		CPPUNIT_ASSERT(m12[6002].diap.getOrig()<=m12[4].diap.getEnd()+defTimes.Top.count()+ AAdmin::GapStop());
	}
}

void TC_AThread::testTopStart()
{	// учет Тор при отправлении (Станционный интервал неодновременного отправления и попутного прибытия, для станций, где пропуск по главному пути осуществляется без остановки)
	TRACE("\r\n =========== testTopStart ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(float(1.0));
 
	Sevent sEv2(//реальная нить
		AEvent::REAL,// тип событий
		5,// начало нити
		10, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent sEv2002(//реальная нить
		AEvent::REAL,// тип событий
		1,// начало нити
		10, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		5, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent sv(//реальная нить
		AEvent::REAL,// тип событий
		1,// начало нити
		300, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		4, // нач.станция (+10 в конструкторе)
		1// число станций
		);


	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
	Strain trnx( TrainDescr( L"", L""),sv, Sevent( NO_FIXED ));

	Sevent tNrm(AEvent::NONE/*тип событий*/,4/*начало нити*/,20/* время на станции*/,20/* время на перегоне*/,0/*время на последнем перегоне*/
		,5/*нач.станция*/,6/* число станций*/);
	auto norm=tpl.norma(tNrm,2);
	{// превышаем Тор => делаем стоянку
		defTimes.Top = std::chrono::seconds(10);
		tpl.setPvx(10,&defTimes);
		Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
		Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn2002));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc),m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[2].diap==norm[1].diap);
		CPPUNIT_ASSERT(norm[1].Esr()==EsrKit(14));
		CPPUNIT_ASSERT(m14[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[6002].Num()!=m14[2].Num());
		CPPUNIT_ASSERT(m15[6002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()>= m14[2].diap.getEnd());
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()<= m14[2].diap.getEnd()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[6002].diap.duration()>AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m14[6002].diap.duration()<=AAdmin::deltaThrough()+ AAdmin::GapStop()*2);
	}
	{// превышаем Тор => делаем стоянку на одном пути
		defTimes.Top = std::chrono::seconds(10);
		tpl.setPvx(10,&defTimes);
		Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
		Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn2002));//ПЕ реальное
		adm.add(tpl.add(trnx));//ПЕ реальное
		adm.addNorma( trn2.tdescr, norm );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc),m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[2].diap==norm[1].diap);
		CPPUNIT_ASSERT(norm[1].Esr()==EsrKit(14));
		CPPUNIT_ASSERT(m14[6002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[6002].Num()==m14[2].Num());
//		CPPUNIT_ASSERT(m15[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()>=defTimes.Top.count()+ m14[2].diap.getEnd());
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()<=defTimes.Top.count()+ m14[2].diap.getEnd()+ AAdmin::GapStop());
	}
	{// превышаем Тор => продлеваем стоянку
		defTimes.Top = std::chrono::seconds(10);
		tpl.setPvx(20,&defTimes);

		TestAAdmin adm;
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn2002));//ПЕ реальное
		TestAAdmin::gapStop[0]=1;
		TestAAdmin::gapStop[1]=1;

		adm.addNorma( trn2002.tdescr, norm );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()==norm[1].diap.getOrig());
		CPPUNIT_ASSERT(m14[6002].diap.getEnd()>norm[1].diap.getEnd());
		CPPUNIT_ASSERT(norm[1].Esr()==EsrKit(14));
		CPPUNIT_ASSERT(m14[6002].diap.enclose(m14[2].diap));
		CPPUNIT_ASSERT(m14[6002].diap.getEnd()>=defTimes.Top.count()+ m14[2].diap.getOrig());
		CPPUNIT_ASSERT(m13[6002].diap.getOrig()+ AAdmin::GapStop()>defTimes.Top.count()+ m13[2].diap.getOrig()- AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[6002].diap.getOrig()<defTimes.Top.count()+ m13[2].diap.getOrig()+ AAdmin::GapStop());
	}
	{// не превышаем Тор
		defTimes.Top = std::chrono::seconds(2);
		tpl.setPvx(20,&defTimes);
		TestAAdmin adm;
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn2002));//ПЕ реальное
		adm.addNorma( trn2002.tdescr, norm );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[6002].diap==norm[1].diap);
		CPPUNIT_ASSERT(norm[1].Esr()==EsrKit(14));
		CPPUNIT_ASSERT(m14[6002].diap.enclose(m14[2].diap.getOrig()));
		CPPUNIT_ASSERT(m14[6002].diap.getEnd()>=defTimes.Top.count()+ m14[2].diap.getOrig());
		CPPUNIT_ASSERT(m14[6002].diap.getEnd()<6+m14[2].diap.getOrig());
	}
}

void TC_AThread::testLimStatPath()
{// выбор пути для проследования при предупреждении (ограничение скорости) на станц.пути
	TRACE("\r\n =========== testLimStatPath ============");
	Sevent sEv1(AEvent::REAL ,10/*начало нити*/,	1/*время на станции*/,2/*время на перегоне*/,0/*время на последнем перегоне*/,
		1/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	TC_AtopologyParams tplParams( 30,5, 1, SpanBreadth::ONE_WAY_SPAN ); 

	//------------ на 14 выбор для стоянки с выгр/погр пассажиров ПЕ -------------------
	tplParams.add(14,6,TC_StatPath(passStopInOutPr));
	tplParams.add(14,5,TC_StatPath(allPr));
	tplParams.add(14,2,TC_StatPath(passStopPr));
	tplParams.add(14,1,TC_StatPath(passStopPr));
	tplParams.add(14,5,TC_StatPath(noStopPr));
	//------------ на 12 стоянка выбор для пассажирского ПЕ -------------------
	tplParams.add(12,8,TC_StatPath(passStopInOutPr));// пасс+ посадка/высадка
	tplParams.add(12,7,TC_StatPath(passStopPr));//пасс
	tplParams.add(12,2,TC_StatPath(passMovePr));//безостановочный пропуск
	tplParams.add(12,5,TC_StatPath(allPr));// нет атр
	tplParams.add(12,6,TC_StatPath(noStopPr));// гл + безост
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60);
	{// проследование  без предупреждений на путях
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m14=fltrStatEv(14,trFrc), m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::MOVE && m14[1].getType()==AEvent::MOVE && m15[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[1].Num()==6);
		CPPUNIT_ASSERT(m14[1].Num()==5);
		CPPUNIT_ASSERT(m15[1].Num()==1);
	}
	{//проследование  c предупреждением на пути
		//нулевой указатель означает окно
		Stech tWnd( 1, 310 /*длительность*/, 6, 1/*park*/, 12/*e1*/,std::make_shared<unsigned int> (39));
		Stech tWnd14_5( 1, 310 /*длительность*/, 5, 1/*park*/, 14/*e1*/,std::make_shared<unsigned int> (40));
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		p.setLimitation( tWnd14_5, tpl );
		TestAAdmin adm(p);
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m14=fltrStatEv(14,trFrc), m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::MOVE && m14[1].getType()==AEvent::MOVE && m15[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[1].Num()==2);
		CPPUNIT_ASSERT(m14[1].Num()==5);
		CPPUNIT_ASSERT(m15[1].Num()==1);
	}
	{//проследование  c предупреждениями на путях
		//нулевой указатель означает окно
		Stech tWnd12_6( 1, 310 /*длительность*/, 6, 1/*park*/, 12/*e1*/,std::make_shared<unsigned int> (20));
		Stech tWnd12_2( 1, 310 /*длительность*/, 2, 1/*park*/, 12/*e1*/,std::make_shared<unsigned int> (20));
		Stech tWnd14_5( 1, 310 /*длительность*/, 5, 1/*park*/, 14/*e1*/,std::make_shared<unsigned int> (20));
		Stech tWnd15_1( 1, 310 /*длительность*/, 1, 1/*park*/, 15/*e1*/,std::make_shared<unsigned int> (20));
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd12_2, tpl );
		p.setLimitation( tWnd12_6, tpl );
		p.setLimitation( tWnd14_5, tpl );
		p.setLimitation( tWnd15_1, tpl );
		TestAAdmin adm(p);
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m14=fltrStatEv(14,trFrc), m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::MOVE && m14[1].getType()==AEvent::MOVE && m15[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[1].Num()==1);
		CPPUNIT_ASSERT(m14[1].Num()==1);
		CPPUNIT_ASSERT(m15[1].Num()==2);
	}
}
