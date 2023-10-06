/*!
	created:	2017/10/04
	created: 	14:30  04 Октябрь 2017
	filename: 	D:\potap\Charm\UT\TC_outrunUser.cpp
	file path:	D:\potap\Charm\UT
	file base:	TC_outrunUser
	file ext:	cpp
	author:		 Dremin
	
	purpose:	 тест запросов пользователя на обгоны
  
  */
 
 #include "stdafx.h"
 #include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "TestAAdmin.h"
#include "../Augur/AOutrun.h"
#include "../Augur/AOutrunTask.h"

void TC_AThread::testOutrunUser()
{// тест запросов пользователя на обгоны
	TC_AtopologyParams tplParams( 60,2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); 
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		16/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv3(AEvent::REAL ,10/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		18/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv5(AEvent::REAL ,15/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		11/*нач.станция (+10 в конструкторе)*/,2/*число станций*/);
	Sevent sEv2(AEvent::REAL ,4/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		26/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv4(AEvent::REAL ,9/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,5/*время на последнем перегоне*/,
		25/*нач.станция (+10 в конструкторе)*/,3/*число станций*/);
	Sevent sEv6(AEvent::REAL ,14/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		22/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn1( TrainDescr( L"1", L"2-100-3"),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"2-100-4"),sEv3, Sevent( NO_FIXED ));
	Strain trn5( TrainDescr( L"5", L"2-100-5"),sEv5, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"2-100-2"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"2-100-2"),sEv4, Sevent( NO_FIXED ));
	Strain trn6( TrainDescr( L"6", L"2-100-1"),sEv6, Sevent( NO_FIXED ));
TestAAdmin adm;
	adm.add(tpl.add(trn1));//ПЕ реальное
	adm.add(tpl.add(trn2));//ПЕ реальное
	adm.add(tpl.add(trn3));//ПЕ реальное
	adm.add(tpl.add(trn4));//ПЕ реальное
	adm.add(tpl.add(trn5));//ПЕ реальное
	adm.add(tpl.add(trn6));//ПЕ реальное
	auto v=adm.forecastGlobal();
	auto pTr1=adm.fnd(trn1);
	auto pTr2=adm.fnd(trn2);
	auto pTr3=adm.fnd(trn3);
	auto pTr4=adm.fnd(trn4);
	auto pTr5=adm.fnd(trn5);
	auto pTr6=adm.fnd(trn6);
	AOutrun outR;
	outR.add(28,pTr3,pTr2);
	outR.add(30,pTr2,pTr5);
	outR.add(29,pTr5,pTr1);
	outR.add(29,pTr1,pTr4);
	outR.add(30,pTr2,pTr4);
	{
		std::array<int,3> r={28,30,29};
		CPPUNIT_ASSERT(outR.get().size()==r.size());
		for (unsigned i=0;i<r.size();i++)
		{
			auto m=outR.get();
			CPPUNIT_ASSERT(m.find(r[i])!=m.end());
		}
		{
			auto m=outR.get();
			CPPUNIT_ASSERT(m.find(28)!=m.end() && m.find(28)->second.size()==1);
		}
		{
			outR.erase(28,pTr2);
			CPPUNIT_ASSERT(outR.get().size()==r.size());
			auto m=outR.get();
			CPPUNIT_ASSERT(m.find(28)!=m.end() && m.find(28)->second.empty());
		}
		{
			outR.erase(28,pTr3);
			CPPUNIT_ASSERT(outR.get().size()==r.size());
			auto m=outR.get();
			CPPUNIT_ASSERT(m.find(28)!=m.end() && m.find(28)->second.empty());
		}
	}
}

void TC_AThread::testOutrunTask()
{// тест задания на обгоны
	st_intervals_t defTimes((float)20.0);
	Sevent sEv1(AEvent::REAL ,10/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		5/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv2(AEvent::REAL ,10+12/*начало нити*/,10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		3/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn2( TrainDescr( L"2", L"2-100-2"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"6001", L"2-100-1"),sEv1, Sevent( NO_FIXED ));//Пригородные(региональные)  6001, 6998
	TRACE("\r\n =========== testOutrunTask ============");
	{// на 1-о путке скрещивание есть
		TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
		TC_Atopology tpl( tplParams );
		tpl.setPvx(20,&defTimes);
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		CPPUNIT_ASSERT( adm.fnd(trn1.tdescr) && adm.fnd(trn2.tdescr));
		adm.tskOutrun->add(14,adm.fnd(trn1.tdescr),adm.fnd(trn2.tdescr));// задание по обгонам
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
	}
	{// на 2-х путке скрещивания нет
		TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); 
		TC_Atopology tpl( tplParams );
		tpl.setPvx(20,&defTimes);
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		auto vl=trFrc.getSpotLines();
		auto it2=std::find_if(vl.begin(),vl.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn2.tdescr;});
		auto it1=std::find_if(vl.begin(),vl.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
		CPPUNIT_ASSERT(it1!=vl.end() && it2!=vl.end());
		auto evIt14=std::find_if(it2->second.begin(),it2->second.end(),[](const AEvent&t){return t.Esr()==EsrKit(14);});
		auto evIt15=std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent&t){return t.Esr()==EsrKit(15);});
		CPPUNIT_ASSERT(evIt14!=it2->second.end() && evIt15!= it1->second.end())	;
		CPPUNIT_ASSERT(evIt14->diap.duration()==AAdmin::deltaThrough()  && evIt15->diap.duration()<AAdmin::deltaThrough());
	}
	{// на 1-о путке скрещивание есть
		TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
		TC_Atopology tpl( tplParams );
		tpl.setPvx(20,&defTimes);
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//ПЕ реальное
		adm.add(tpl.add(trn2));//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto vl=trFrc.getSpotLines();
		auto it2=std::find_if(vl.begin(),vl.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn2.tdescr;});
		auto it1=std::find_if(vl.begin(),vl.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
		CPPUNIT_ASSERT(it1!=vl.end() && it2!=vl.end());
		auto evIt14=std::find_if(it2->second.begin(),it2->second.end(),[](const AEvent&t){return t.Esr()==EsrKit(14);});
		auto evIt15_2=std::find_if(it2->second.begin(),it2->second.end(),[](const AEvent&t){return t.Esr()==EsrKit(15);});
		auto evIt15=std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent&t){return t.Esr()==EsrKit(15);});
		CPPUNIT_ASSERT(evIt14!=it2->second.end() && evIt15!= it1->second.end() && evIt15_2!= it2->second.end())	;
		CPPUNIT_ASSERT(evIt14->diap.duration()==AAdmin::deltaThrough() && evIt15->diap.getEnd()==evIt15_2->diap.getOrig()+defTimes.tsk.count() );
	}
}



 
 
 
