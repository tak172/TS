/*!
	created:	2019/10/07
	created: 	19:13  
 	filename: 	F:\potapTrunk\Charm\UT\TC_ThrTechStat.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrTechStat
	file ext:	cpp
	author:		 Dremin
	
	purpose: техн.станция  и вопросы с ней
  
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

void TC_AThread::testConflCounterTechSt()
{// конфликт с встречным от тех.станции
	TRACE("\r\n =========== testConflCounterTechSt ============");
	Sevent sEv1(	AEvent::REAL,10/* начало*/,10/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Sevent sEv3(	AEvent::REAL,5/* начало*/,10/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Sevent sEv2(	AEvent::REAL,40/* начало*/,10/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,5/* нач.станция*/,1/* число станций*/);

	Sevent tNrm1(AEvent::NONE,// тип событий
		70,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		10// число станций
		);
	Sevent tNrm3(AEvent::NONE,// тип событий
		170,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		10// число станций
		);

	Strain trn1( TrainDescr( L"1001", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	trn1.tdescr.SetBrutto( AAdmin::Advantage_brutto()+1); 
	Strain trn3( TrainDescr( L"1003", L"1-100-2"),sEv3, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"1002", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	TechnicalTime::Specification spec= TechnicalTime::MakeSpecification(trn1.tdescr);
	long tblSt[]={13};
	map<EsrKit, TechnicalTime> techTimes;
	for (const auto& t: tblSt)
		techTimes[EsrKit(t)].AddTime( TechnicalTime::SpecifiedTime( spec, TechnicalTime::Minutes( 1) ) );
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)1.0);
	tpl.setPvx(10,&defTimes);
	auto norm1=tpl.norma(tNrm1,1);
	auto norm3=tpl.norma(tNrm3,1);

	{// обгон создан и не решен 
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1101", L""), norm1 );
		adm.addNorma(TrainDescr( L"1103", L""), norm3 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1001].diap.enclose(m14[1002].diap.getOrig()));
		CPPUNIT_ASSERT(m14[1002].diap.enclose(m14[1001].diap.getOrig()));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()>=norm1[1].diap.getEnd() && norm1[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()<norm1[1].diap.getEnd()+10);
		CPPUNIT_ASSERT(m14[1002].diap.getEnd()-m14[1001].diap.getOrig()==defTimes.tsk.count());
	}
	{// обгон создан и решен 
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		// 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по попутным обгонам
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1101", L""), norm1 );
		adm.addNorma(TrainDescr( L"1103", L""), norm3 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1001].diap.enclose(m14[1002].diap.getEnd()));
		CPPUNIT_ASSERT(m14[1002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m[1001].diap.getEnd()>=m[1002].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m[1001].diap.getEnd()<m[1002].diap.getOrig()+defTimes.tsk.count()+20);
		CPPUNIT_ASSERT(m[1001].diap.getEnd()>norm1[1].diap.getEnd() && norm1[1].Esr()==EsrKit(13));
	}
	{// обгон создан ,решен и согласован
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::useBackAdjust=true;// согласование
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1101", L""), norm1 );
		adm.addNorma(TrainDescr( L"1103", L""), norm3 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1001].diap.enclose(m14[1002].diap.getEnd()));
		CPPUNIT_ASSERT(m14[1002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m[1001].diap.getEnd()>m[1002].diap.getOrig());
		CPPUNIT_ASSERT(m[1001].diap.getEnd()<=m[1002].diap.getOrig()+AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m[1001].diap.getEnd()>norm1[1].diap.getEnd() && norm1[1].Esr()==EsrKit(13));
	}
}

void TC_AThread::testRelinkNormTech()
{// тест пересвязывания нормативки на тех.станции с разрешением конфликта
	TRACE("\r\n =========== testRelinkNormTech ============");
	Sevent sEv1001(	AEvent::REAL,10/* начало*/,10/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Sevent sEv1003(	AEvent::REAL,5/* начало*/,10/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,2/* нач.станция*/,1/* число станций*/);
	Sevent sEv1002(	AEvent::REAL,40/* начало*/,10/* на станции*/,0/*время на перегоне*/,0/*на последнем перегоне*/,5/* нач.станция*/,1/* число станций*/);

	Sevent tNrm1(AEvent::NONE,// тип событий
		70,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		10// число станций
		);
	Sevent tNrm3(AEvent::NONE,// тип событий
		170,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		10// число станций
		);

	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
	Strain trn1003( TrainDescr( L"1003", L"1-100-2"),sEv1003, Sevent( NO_FIXED ));
	Strain trn1002( TrainDescr( L"1002", L"1-100-1"),sEv1002, Sevent( NO_FIXED ));
	TechnicalTime::Specification spec= TechnicalTime::MakeSpecification(trn1001.tdescr);
	long tblSt[]={13};
	map<EsrKit, TechnicalTime> techTimes;
	for (const auto& t: tblSt)
		techTimes[EsrKit(t)].AddTime( TechnicalTime::SpecifiedTime( spec, TechnicalTime::Minutes( 1) ) );
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)1.0);
	tpl.setPvx(6,&defTimes);
	auto norm1=tpl.norma(tNrm1,1);
	auto norm3=tpl.norma(tNrm3,1);

	{// обгоны созданы и решены (встречные и попутные) + перепривязаны нормативки
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по попутным обгонам
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.add(tpl.add(trn1003,2));//ПЕ реальное
		adm.add(tpl.add(trn1002));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1101", L""), norm1 );
		adm.addNorma(TrainDescr( L"1103", L""), norm3 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1001].diap.enclose(m[1003].diap.getEnd()));
		CPPUNIT_ASSERT(m[1001].diap.enclose(m[1002].diap));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()>norm1[1].diap.getEnd() && norm1[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()==norm3[1].diap.getEnd() && norm3[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m14[1002].getType()==AEvent::MOVE&& m[1002].getType()==AEvent::MOVE);
	}
	{// обгоны созданы, но не решены
		TestAAdmin adm;
		TestAAdmin::checkOutrunTechStation=true;// проверять обгоны на тех.станциии
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.add(tpl.add(trn1003,2));//ПЕ реальное
		adm.add(tpl.add(trn1002));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1101", L""), norm1 );
		adm.addNorma(TrainDescr( L"1103", L""), norm3 );
		// добавление  тех.станций
		for (const auto& t: tblSt)
		{
			LONG_VECTOR lv(1,t);
			lv.push_back(t+1);
			ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
			CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(1) && "Установлена тех.станция");
		}
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto v=trFrc.getSpotLines();
		CPPUNIT_ASSERT(v.size()==3);
		auto m=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m[1003].diap.enclose(m[1001].diap.getOrig()));
		CPPUNIT_ASSERT(m[1001].diap.enclose(m[1003].diap.getEnd()));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()==norm3[1].diap.getEnd() && norm3[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m[1003].diap.getEnd()==norm1[1].diap.getEnd() && norm1[1].Esr()==EsrKit(13));
	}

	{// обгоны созданы, но не решены
		TestAAdmin::checkOutrunTechStation=false;// проверять обгоны на тех.станциии
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.add(tpl.add(trn1003,2));//ПЕ реальное
		adm.add(tpl.add(trn1002));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1101", L""), norm1 );
		adm.addNorma(TrainDescr( L"1103", L""), norm3 );
		// добавление  тех.станций
		for (const auto& t: tblSt)
		{
			LONG_VECTOR lv(1,t);
			lv.push_back(t+1);
			ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
			CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(1) && "Установлена тех.станция");
		}
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto v=trFrc.getSpotLines();
		CPPUNIT_ASSERT(v.size()==3);
		CPPUNIT_ASSERT(adm.numStepFrc.get()==0);
		auto m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1003].diap.enclose(m[1001].diap));
		CPPUNIT_ASSERT(m[1003].diap.getEnd()==norm3[1].diap.getEnd() && norm3[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()==norm1[1].diap.getEnd() && norm1[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()<m[1002].diap.getOrig() && m[1002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[1002].getType()==AEvent::STOP_START && m14[1002].diap.duration()>50);
	}
	{// обгоны созданы, но решены только встречные
		TestAAdmin adm;
		TestAAdmin::checkOutrunTechStation=false;// проверять обгоны на тех.станциии
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.add(tpl.add(trn1003,2));//ПЕ реальное
		adm.add(tpl.add(trn1002));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1101", L""), norm1 );
		adm.addNorma(TrainDescr( L"1103", L""), norm3 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		CPPUNIT_ASSERT(adm.numStepFrc.get()==1);
		auto m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1003].diap.enclose(m[1001].diap));
		CPPUNIT_ASSERT(m[1003].diap.enclose(m[1002].diap));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()>norm1[1].diap.getEnd() && norm1[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m[1003].diap.getEnd()==norm3[1].diap.getEnd() && norm3[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m14[1002].getType()==AEvent::MOVE&& m[1002].getType()==AEvent::MOVE);
	}
	{// обгоны созданы, но решены только встречные
		TestAAdmin adm;
		TestAAdmin::checkOutrunTechStation=true;// проверять обгоны на тех.станциии
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		adm.add(tpl.add(trn1001));//ПЕ реальное
		adm.add(tpl.add(trn1003,2));//ПЕ реальное
		adm.add(tpl.add(trn1002));//ПЕ реальное
		adm.addNorma(TrainDescr( L"1101", L""), norm1 );
		adm.addNorma(TrainDescr( L"1103", L""), norm3 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
		auto m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1003].diap.enclose(m[1001].diap.getOrig()));
		CPPUNIT_ASSERT(m[1001].diap.enclose(m[1003].diap.getEnd()));
		CPPUNIT_ASSERT(m[1003].diap.enclose(m[1002].diap));
		CPPUNIT_ASSERT(m[1003].diap.getEnd()>norm1[1].diap.getEnd() && norm1[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m[1001].diap.getEnd()==norm3[1].diap.getEnd() && norm3[1].Esr()==EsrKit(13));
		CPPUNIT_ASSERT(m14[1002].getType()==AEvent::MOVE&& m[1002].getType()==AEvent::MOVE);
	}
}

 
 
 
