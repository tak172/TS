/*!
	created:	2018/12/27
	created: 	13:50  27 Декабрь 2018
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrAjust.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrAjust
	file ext:	cpp
	author:		 Dremin
	
	purpose:	коррекция прогноза для согласования после решения конфликтов
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Augur/AArchive.h"
#include "../helpful/Attic.h"
using namespace std;


void TC_AThread::testAdjustFirstStep()
{// тест согласования. коррекция дотягивания реальной нити до станции с перегона по полученным результатам прогноза.
	//учет возникшего торможения.
	TRACE("\r\n =========== testAdjustFirstStep ============");
	Sevent sEv1(AEvent::REAL ,10+50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv2(AEvent::REAL ,75/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent tNrm2(AEvent::NONE/*тип событий*/,75/*начало нити*/,10/* время на станции*/,50/* время на перегоне*/,0/*время на последнем перегоне*/
		,7/*нач.станция*/,7/* число станций*/);


	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1001", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998

	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	TC_Atopology tpl( tplParams );
	auto norm2=tpl.norma(tNrm2,2);
	tpl.setPvx(60,&defTimes);
	TestAAdmin adm;
	adm.addNorma(trn2.tdescr,norm2);
	adm.add(tpl.add(trn1));//ПЕ реальное
	adm.add(tpl.add(trn2));//ПЕ реальное
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto t1=adm.fnd(trn1.tdescr);
	AEvent ev= t1->origUnionFrc();
	CPPUNIT_ASSERT(ev.Esr()==EsrKit(13,14));
	auto pmt= t1->getPMT(ev);
	CPPUNIT_ASSERT(ev.diap.duration()==pmt.full+pmt.start+pmt.stop);
	auto v=t1->getFrc();
	auto it=std::find_if(v.begin(),v.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
	CPPUNIT_ASSERT(it!=v.end() && it->getType()==AEvent::STOP_START);
}

void TC_AThread::testResort()
{//  тест пересортировки списка ПЕ
	Sevent sEv(AEvent::REAL ,10+50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	TC_Atopology tpl( tplParams );
	TestAAdmin adm;
	TestAAdmin::noSort=true;
	int tbl[]={2,4,6,8,10,12};
	std::vector<int>vOr(ARRAY_IT(tbl));
	for (const auto& t: vOr)
	{
		Strain trn( TrainDescr( std::to_wstring(t).c_str(), L"1-1-1"),sEv, Sevent( NO_FIXED ));
		adm.add(tpl.add(trn));//ПЕ реальное
	}
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	std::vector<int> v;
	std::for_each(adm.allTrn.begin(),adm.allTrn.end(),[&v](const ATrainPtr&t){ v.push_back(t->getNum());});
	CPPUNIT_ASSERT(v==vOr);
	auto t2=AAdmin::fndTrn(TrainDescrPtr(new TrainDescr(L"2", L"1-1-1"))).front();// найти ПЕ по дескриптору
	auto t6=AAdmin::fndTrn(TrainDescrPtr(new TrainDescr(L"6", L"1-1-1"))).front();// найти ПЕ по дескриптору
	auto t8=AAdmin::fndTrn(TrainDescrPtr(new TrainDescr(L"8", L"1-1-1"))).front();// найти ПЕ по дескриптору
	auto t10=AAdmin::fndTrn(TrainDescrPtr(new TrainDescr(L"10", L"1-1-1"))).front();// найти ПЕ по дескриптору
	CPPUNIT_ASSERT( AAdmin::resort(t6,t8));
	int t1[]={2,4,8,6,10,12};
	std::vector<int>v1(ARRAY_IT(t1));
	v.clear();
	std::for_each(adm.allTrn.begin(),adm.allTrn.end(),[&v](const ATrainPtr&t){ v.push_back(t->getNum());});
	CPPUNIT_ASSERT(v==v1);
	CPPUNIT_ASSERT( !AAdmin::resort(t6,t8));
	v.clear();
	std::for_each(adm.allTrn.begin(),adm.allTrn.end(),[&v](const ATrainPtr&t){ v.push_back(t->getNum());});
	CPPUNIT_ASSERT(v==v1);
	{
		CPPUNIT_ASSERT( AAdmin::resort(t2,t10));
		int t1[]={10,2,4,8,6,12};
		std::vector<int>v2(ARRAY_IT(t1));
		v.clear();
		std::for_each(adm.allTrn.begin(),adm.allTrn.end(),[&v](const ATrainPtr&t){ v.push_back(t->getNum());});
		CPPUNIT_ASSERT(v==v2);
		CPPUNIT_ASSERT( !AAdmin::resort(t2,t10));
		v.clear();
		std::for_each(adm.allTrn.begin(),adm.allTrn.end(),[&v](const ATrainPtr&t){ v.push_back(t->getNum());});
		CPPUNIT_ASSERT(v==v2);
	}
}

void TC_AThread::testPassOutrunResort()
{// тест решени попутных обгонов путем переупорядочения попутных ПЕ
	TRACE("\r\n =========== testPassOutrunResort ============");
	Sevent sEv1(AEvent::REAL ,10+50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv3(AEvent::REAL ,20+50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv2(AEvent::REAL ,75/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		7/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1001", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	Strain trn3( TrainDescr( L"1003", L"1-1-2"),sEv3, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	// на 1-о путке скрещивание есть
	TC_AtopologyParams tplParams( 30,4, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=1; 
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==3);
		M_INT_EV m14= fltrStatEv(14,trFrc),m15= fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m14.size()==3 && m15.size()==3);
		CPPUNIT_ASSERT( m14[2].getType()==AEvent::MOVE && m14[1001].getType()==AEvent::STOP_START&& m14[1003].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m14[1001].diap.enclose(m14[1003].diap));
		CPPUNIT_ASSERT( std::count_if( m15.begin(),m15.end(),[](const INT_EV& t){ return t.second.getType()==AEvent::MOVE;})==3);
	}
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=2; 
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по обгонам
		adm.add(tpl.add(trn2));//ПЕ реальное
		adm.add(tpl.add(trn3));//ПЕ реальное
		adm.add(tpl.add(trn1));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==3);
		M_INT_EV m14= fltrStatEv(14,trFrc),m15= fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m14.size()==3 && m15.size()==3);
		CPPUNIT_ASSERT( m14[2].getType()==AEvent::MOVE && m14[1001].getType()==AEvent::STOP_START&& m14[1003].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m14[1001].diap.getOrig()<m14[1003].diap.getOrig());
		CPPUNIT_ASSERT( m14[1003].diap.getEnd()>m14[1001].diap.getEnd());
		CPPUNIT_ASSERT( std::count_if( m15.begin(),m15.end(),[](const INT_EV& t){ return t.second.getType()==AEvent::MOVE;})==3);
	}
}

void TC_AThread::testConflCounterViaDiapBreakService()
{// проверка разрешения конфликта встречных с проверкой времени проверки тормозов
	TRACE("\r\n =========== testConflCounterViaDiapBreakService ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Iot=chrono::seconds(10);
	tpl.setPvx(60,&defTimes);

	Sevent sEv3(	AEvent::REAL,85/*начало нити*/,4/*время на станции*/,	1/*время на перегоне*/,1/*время на последнем перегоне*/,
		1 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent sEv5(	AEvent::REAL,65/*начало нити*/,10/*время на станции*/,	1/*время на перегоне*/,0/*время на последнем перегоне*/,
		2 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent sEv4(	AEvent::REAL,11/*начало нити*/,4/*время на станции*/,	1/*время на перегоне*/,10/*время на последнем перегоне*/,
		4 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);

	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3,Sevent(NO_FIXED));
	ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::BOTH);
	Strain trn5( TrainDescr( L"5", L"1-100-2"),sEv5,Sevent(NO_FIXED));
	ATrainPtr tr5=tpl.add(trn5,1,TC_Atopology::BOTH);
	Strain trn4( TrainDescr( L"4", L"2-200-1"),sEv4,Sevent(NO_FIXED));
	ATrainPtr tr4=tpl.add(trn4,2,TC_Atopology::BOTH);
	TestPossessions p( TestAAdmin::getNsi() );
	{// не решаем конфликты
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useBackAdjust=true;
		ACounterInt::limit=8;//построение конфликтов

		adm.add(tr4);//ПЕ реальное
		adm.add(tr3);//ПЕ реальное
		adm.add(tr5);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::MOVE && m12[3].getType()==AEvent::MOVE );
		CPPUNIT_ASSERT(m12[5].diap.getOrig()<m12[4].diap.getOrig() && m12[3].diap.getOrig()<m12[5].diap.getEnd());
		CPPUNIT_ASSERT( m12[4].diap.getEnd()<m12[5].diap.getEnd());
		CPPUNIT_ASSERT(m13[3].getType()==AEvent::MOVE  && m13[4].getType()==AEvent::MOVE && m13[5].getType()==AEvent::MOVE);
	}
	{// без контроля времени на обслуживание (проверку) тормозов
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по обгонам
		ACounterInt::limit=8;//построение конфликтов
		adm.add(tr4);//ПЕ реальное
		adm.add(tr3);//ПЕ реальное
		adm.add(tr5);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[4].getType()==AEvent::STOP_START && m13[3].getType()==AEvent::MOVE && m13[5].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::MOVE && m12[3].getType()==AEvent::MOVE );
		CPPUNIT_ASSERT(m13[4].diap.enclose(m13[5].diap));
		CPPUNIT_ASSERT(m13[4].diap.enclose(m13[3].diap));
		CPPUNIT_ASSERT(m13[4].diap.getEnd()-m13[3].diap.getOrig()<=defTimes.tsk.count()+ 2*AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[4].diap.getEnd()-m13[3].diap.getOrig()>=defTimes.tsk.count());
	}
	{// с контролем времени на обслуживание (проверку) тормозов
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- для тестовой отладки--------------
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::maxWaitCounterPack=50;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// ф-ция  отбора заданий по обгонам
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по обгонам
		adm.add(tr4);//ПЕ реальное
		adm.add(tr3);//ПЕ реальное
		adm.add(tr5);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[4].getType()==AEvent::MOVE && m13[3].getType()==AEvent::MOVE && m13[5].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[5].diap.enclose(m12[4].diap.getOrig()));
		CPPUNIT_ASSERT(m12[5].diap.getOrig()<m12[3].diap.getOrig());
		CPPUNIT_ASSERT(m12[3].diap.getEnd()>m12[5].diap.getEnd());

		CPPUNIT_ASSERT(m12[5].diap.getEnd()-m12[4].diap.getOrig()<=defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[5].diap.getEnd()-m12[4].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m12[3].diap.getEnd()-m12[5].diap.getEnd()<=defTimes.Iot.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[3].diap.getEnd()-m12[5].diap.getEnd()>=defTimes.Iot.count());
	}
}