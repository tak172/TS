/*!
	created:	2019/10/14
	created: 	11:51  
 	filename: 	F:\potapTrunk\Charm\UT\TC_ThrStatInterval.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrStatInterval
	file ext:	cpp
	author:		 Dremin
	
	purpose: учет  станц. интервалов
  
  */
 
#include "stdafx.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/TrainRoutes.h"
#include "../Augur/AClash.h"
#include "../Augur/AAdmin.h"
#include "../Augur/AConflData.h"

using namespace std;

void TC_AThread::testTnTsk()
{// учет станц.интервалов
	TRACE("\r\n =========== testTnTsk ============");
	TC_AtopologyParams tplParams( stationsCount,2, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)4.0);
	tpl.setPvx(40,&defTimes);

	Sevent sEv1(	AEvent::REAL,20/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Sevent sEv81(	AEvent::REAL,0/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Sevent sEv2(	AEvent::REAL,5/* начало*/,1/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,6/* нач.станция*/,1/* число станций*/);
	Sevent sEv82(	AEvent::REAL,22/* начало*/,1/* на станции*/,0/*время на перегоне*/,10/*на последнем перегоне*/,5/* нач.станция*/,1/* число станций*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn81( TrainDescr( L"1", L"1-100-2"),sEv81, Sevent( NO_FIXED ));
	Strain trn82( TrainDescr( L"8002", L"1-100-1"),sEv82, Sevent( NO_FIXED ));

	{// большой tn
		defTimes.tsk=chrono::seconds(6);
		defTimes.tn=chrono::seconds(15);
		defTimes.tnp=chrono::seconds(15);
		tpl.setStInt(defTimes);
		TestAAdmin adm;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrSoftTnInterval);// ф-ция  отбора заданий по обгонам
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useBackAdjust=true;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=  adm.get("1");
		auto pvx=t->getPMT(13,14);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc),m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m[1].diap.duration()<=AAdmin::deltaThrough()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m[2].diap.enclose(m[1].diap.getOrig()));
		CPPUNIT_ASSERT(m[1].diap.getEnd()>m[2].diap.getOrig()+defTimes.tn.count());
		CPPUNIT_ASSERT(m[2].diap.getEnd()-m[1].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m[2].diap.getEnd()-m[1].diap.getOrig()<defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m[1].diap.getOrig()-m13[1].diap.getEnd()==pvx.full +pvx.stop  && " для выполнения Tn");
		CPPUNIT_ASSERT(m15[1].diap.getOrig()-m[1].diap.getEnd()>=pvx.full +pvx.start  && " для выполнения Tn");
		CPPUNIT_ASSERT(m15[1].diap.getOrig()-m[1].diap.getEnd()<=pvx.full +pvx.start );
	}
	{// большой tn
		defTimes.tsk=chrono::seconds(6);
		defTimes.tn=chrono::seconds(15);
		defTimes.tnp=chrono::seconds(15);
		tpl.setStInt(defTimes);
		TestAAdmin adm;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrSoftTnInterval);// ф-ция  отбора заданий по обгонам
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t=  adm.get("1");
		auto pvx=t->getPMT(13,14);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc),m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m[1].diap.duration()<=AAdmin::deltaThrough()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m[2].diap.enclose(m[1].diap.getOrig()));
		CPPUNIT_ASSERT(m[1].diap.getEnd()>m[2].diap.getOrig()+defTimes.tn.count());
		CPPUNIT_ASSERT(m[2].diap.getEnd()-m[1].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m[2].diap.getEnd()-m[1].diap.getOrig()<defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m[1].diap.getOrig()-m13[1].diap.getEnd()==pvx.full +pvx.stop  && " для выполнения Tn");
		CPPUNIT_ASSERT(m15[1].diap.getOrig()-m[1].diap.getEnd()>=pvx.full +pvx.start  && " для выполнения Tn");
		CPPUNIT_ASSERT(m15[1].diap.getOrig()-m[1].diap.getEnd()<=pvx.full +pvx.start );
	}
	{// большой tn
		TestAAdmin adm;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrSoftTnInterval);// ф-ция  отбора заданий по обгонам
		defTimes.tsk=chrono::seconds(6);
		defTimes.tn=chrono::seconds(15);
		defTimes.tnp=chrono::seconds(15);
		tpl.setStInt(defTimes);
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useAdjustFirstStepForecast=true;// использовать коррекцию дотягивания на первом шаге после расчета прогноза
		TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию 
		adm.add(tpl.add(trn81));//ПЕ реальное
		adm.add(tpl.add(trn82));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m13[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[8002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[8002].diap.enclose(m14[1].diap.getOrig()));
		CPPUNIT_ASSERT(m14[1].diap.getOrig()>=m14[8002].diap.getOrig()+defTimes.tn.count());
		CPPUNIT_ASSERT(m14[1].diap.getOrig()<=m14[8002].diap.getOrig()+defTimes.tn.count()+AAdmin::deltaThrough()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[8002].diap.getEnd()-m14[1].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m14[8002].diap.getEnd()-m14[1].diap.getOrig()<defTimes.tsk.count()+ AAdmin::GapStop());
	}
	{// небольшой tn
		TestAAdmin adm;
		defTimes.tn=chrono::seconds(1);
		defTimes.tnp=chrono::seconds(1);
		tpl.setStInt(defTimes);
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn82));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m[8002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m[8002].diap.enclose(m[1].diap.getOrig()));
		CPPUNIT_ASSERT(m[1].diap.getEnd()>m[8002].diap.getOrig()+defTimes.tn.count());
		CPPUNIT_ASSERT(m[8002].diap.getEnd()-m[1].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m[8002].diap.getEnd()-m[1].diap.getOrig()<defTimes.tsk.count()+ AAdmin::GapStop());
	}

	{// небольшой tn
		TestAAdmin adm;
		defTimes.tn=chrono::seconds(1);
		defTimes.tnp=chrono::seconds(1);
		tpl.setStInt(defTimes);
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m[2].diap.enclose(m[1].diap.getOrig()));
		CPPUNIT_ASSERT(m[1].diap.getEnd()>m[2].diap.getOrig()+defTimes.tn.count());
		CPPUNIT_ASSERT(m[2].diap.getEnd()-m[1].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m[2].diap.getEnd()-m[1].diap.getOrig()<defTimes.tsk.count()+ AAdmin::GapStop());
	}
}

static void tstStIntervals(TrainRoutes<AEvent>& trFrc, TestAAdmin& adm)
{
	auto evTr=trFrc.getSpotLines();
	std::vector<TrainRoutes<AEvent>::EventVec> vEvOdd, vEvEven;
	auto evLn=trFrc.getSpotLines();
	for (const auto& t: evLn)
	{
		ATrainPtr  trn= adm.get(std::to_string(t.first.GetNumber().getNumber()));// получить поезд(всю инфу )
		V_EVNT real= trn->get(AEvent::REAL);// все события указанного происхождения
		real.insert(real.end(),t.second.begin(),t.second.end());
		if(trn->getNum()%2)
			vEvOdd.push_back(real);
		else
			vEvEven.push_back(real);
	}
	for (const auto& t:vEvOdd )
		for (const auto& ev: t)
			if(!ev.onSpan())
			{
				st_intervals_t stInt= getInterval(ev.Esr().getTerm(),StationEsr(ev.Esr().getTerm()+1), adm.getNsi()->stIntPtr);
				for (const auto& tC:vEvEven )
					for (const auto& evt:tC )
						if(evt.Esr()==ev.Esr())
							switch (ev.getType())
						{
							case AEvent::MOVE:
								if(evt.getType()==AEvent::STOP_START)
									CPPUNIT_ASSERT(chrono::seconds( abs(ev.diap.getOrig()-evt.diap.getOrig()) )>=stInt.tnp);
								break;
							case AEvent::STOP_START:
								if(evt.getType()==AEvent::STOP_START)
									CPPUNIT_ASSERT(chrono::seconds( abs(ev.diap.getOrig()-evt.diap.getOrig()) )>=stInt.tn);
								if(evt.getType()==AEvent::MOVE)
									CPPUNIT_ASSERT(chrono::seconds( abs(ev.diap.getOrig()-evt.diap.getOrig()) )>=stInt.tnp);
						}
			}
}


void TC_AThread::testSkrech()
{// тест учета интервалов скрещения 
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(30);

	st_intervals_t defTimes;

	Sevent sEv2(//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		//60,// время на станции
		( int ) defTimes.tnp.count(), // время на станции
		40,// время на перегоне
		0,// время на последнем перегоне
		8, // нач.станция (+10 в конструкторе)
		6// число станций
		);
	Sevent sEv4(//реальная нить
		AEvent::REAL,// тип событий
		110,// начало нити
		//60,// время на станции
		( int ) defTimes.tnp.count(), // время на станции
		40,// время на перегоне
		0,// время на последнем перегоне
		9, // нач.станция (+10 в конструкторе)
		6// число станций
		);
	Sevent sEv1001(//реальная нить
		AEvent::REAL,// тип событий
		30,// начало нити
		//10,// время на станции
		( int ) defTimes.tnp.count(), // время на станции
		0,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция (+10 в конструкторе)
		1// число станций
		);

	Sevent tNrm(AEvent::NONE,// тип событий
		30,// начало нити
		40,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция
		7// число станций
		);
	auto norm1001=tpl.norma(tNrm,1001);
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L""),sEv4, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
	TRACE("\r\n =========== testSkrech ============");
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn2));
		adm.add(tpl.add(trn4));
		adm.add(tpl.add(trn1001));//ПЕ реальное
		tstTRACE(*adm.getNsi());
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		tstStIntervals(trFrc,adm);
	}
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn2));
		adm.add(tpl.add(trn4));
		adm.add(tpl.add(trn1001));//ПЕ реальное

		adm.addNorma( trn1001.tdescr, norm1001 );

		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений c нормативкой не должно быть.");
		tstStIntervals(trFrc,adm);
	}
}
 

void TC_AThread::testTnpRegim()
{// тест режима учета интервала Тн в случае, если оба поезда прибывают с остановкой, то  можно не соблюдать.
	TRACE("\r\n =========== testTnpRegim ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.tn=chrono::seconds(9);
	defTimes.tnp=chrono::seconds(12);
	tpl.setPvx(PVX_STANDART,&defTimes);
	Sevent sEv2(	AEvent::REAL,20/*начало нити*/,1/*время на станции*/,	1/*время на перегоне*/,0/*время на последнем перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);

	Sevent sEv1(	AEvent::REAL,1/*начало нити*/,1/*время на станции*/,	1/*время на перегоне*/,0/*время на последнем перегоне*/,
		0 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent tNrm1(AEvent::NONE,// тип событий
		95,// начало нити
		30,// время на станции
		30,// время на перегоне
		0,// время на последнем перегоне
		0, // нач.станция (+10 в конструкторе)
		10// число станций
		);
	Sevent tNrm2(AEvent::NONE,// тип событий
		10,// начало нити
		10,// время на станции
		60,// время на перегоне
		0,// время на последнем перегоне
		3, // нач.станция (+10 в конструкторе)
		10// число станций
		);

	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,Sevent(NO_FIXED));
	ATrainPtr tr1=tpl.add(trn1,1,TC_Atopology::BOTH);
	Strain trn2( TrainDescr( L"2", L"2-200-1"),sEv2,Sevent(NO_FIXED));
	ATrainPtr tr2=tpl.add(trn2,2,TC_Atopology::BOTH);
	auto norm1=tpl.norma(tNrm1,1);
	auto norm2=tpl.norma(tNrm2,2);
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		ACounterInt::limit=1;//построение конфликтов
		adm.addNorma(TrainDescr( L"1", L""), norm1 );
		adm.addNorma(TrainDescr( L"2", L""), norm2 );
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
		CPPUNIT_ASSERT(m12[2].diap.duration()>norm2.front().diap.duration());
		CPPUNIT_ASSERT(m11[2].diap.duration()==norm2.front().diap.duration());
		CPPUNIT_ASSERT(m11[1].diap.duration()==norm1.front().diap.duration() && m12[1].diap.duration()==norm1.front().diap.duration());
		CPPUNIT_ASSERT(m11[2].diap.getOrig()>m11[1].diap.getOrig());
		CPPUNIT_ASSERT(abs(m11[2].diap.getOrig()-m11[1].diap.getOrig()-defTimes.tn.count())< AAdmin::GapStop());
	}

	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		ACounterInt::limit=1;//построение конфликтов
		AConflData::strongTnpRgm(false);// установка(true)/снятие(false) учета интервала Тн в случае, если оба поезда прибывают с остановкой, то  можно не соблюдать.
		adm.addNorma(TrainDescr( L"1", L""), norm1 );
		adm.addNorma(TrainDescr( L"2", L""), norm2 );
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
		CPPUNIT_ASSERT(m12[2].diap.duration()==norm2.front().diap.duration());
		CPPUNIT_ASSERT(m11[2].diap.duration()==norm2.front().diap.duration());
		CPPUNIT_ASSERT(m11[1].diap.duration()==norm1.front().diap.duration() && m12[1].diap.duration()==norm1.front().diap.duration());
		CPPUNIT_ASSERT(m11[2].diap.getOrig()<m11[1].diap.getOrig());
		CPPUNIT_ASSERT(abs(m11[2].diap.getOrig()-m11[1].diap.getOrig())==defTimes.tn.count()- AAdmin::GapStop());
	}
}

