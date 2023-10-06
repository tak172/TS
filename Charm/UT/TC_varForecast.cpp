/*!
	created:	2017/08/07
	created: 	13:49  07 Август 2017
	filename: 	D:\potapTrunk\Charm\UT\TC_varForecast.cpp
	file path:	D:\potapTrunk\Charm\UT
	file base:	TC_varForecast
	file ext:	cpp
	author:		 Dremin
	
	purpose:	построение вариантного графика 
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h"
#include "TC_data.h"

void TC_AThread::testVarForecastCreate()
{// тест построения вариантного графика 
	TRACE("\r\n   ----  testVarForecastCreate  -----");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)20.0);
	tpl.setPvx(20,&defTimes);
	Sevent tNrm1(AEvent::NONE,// тип событий
		10,// начало нити
		10,// время на станции
		10,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		4// число станций
		);
	Sevent tNrm2(AEvent::NONE,// тип событий
		15,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		12, // нач.станция (+10 в конструкторе)
		4// число станций
		);
	int tbl[]={1,3,1001,1003,2,4,1002,1004};// генерируемые нормативки
 	TestAAdmin adm;
 	for (const auto& t: tbl)
 	{
 		Sevent& tNrm=t&1?tNrm1:tNrm2;
 		auto nrm=tpl.norma(tNrm,t);
 		auto wNum=std::to_wstring(t);
 		adm.addNorma(TrainDescr( wNum.c_str(), L""), nrm );
 		Strain trn( TrainDescr(wNum.c_str(), L""),Sevent(NO_REAL),Sevent( NO_FIXED ));
 		adm.add(tpl.add(trn));
 		tNrm.e1++;
 		tNrm.or+=5;
 	}
 	adm.setVariant(true);
 	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 	tstTRACE(adm);
 	CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
 	struct P{
 		int nEsr;
 		int ntr;
 	}tP[]={/* {14,1001},{15,1003},*/{13,1}};// теперь для грузовых ПЕ цель не берется из нормативки
 	std::vector<P> vOutrun(tP,tP+ size_array(tP));
 	auto frLn=trFrc.getSpotLines();
 	int nOutr=0;
 	for (const auto& tr:frLn )
 	{
 		auto trn= adm.get(std::to_string(tr.first.GetNumber().getNumber()));
 		const MAP_ESR_VTRN& rsn=trn->outrunInfo();
 		for (const auto& est: rsn)
 		{
 			auto it=std::find_if(vOutrun.begin(),vOutrun.end(),[&](const P& pt)
 			{return pt.nEsr==est.first && pt.ntr==tr.first.GetNumber().getNumber();});
 			CPPUNIT_ASSERT(it!=vOutrun.end() && "Все торможения должны быть указаны в таблице" );
 			if(it!=vOutrun.end())
 				nOutr++;
 		}
 	}
 	CPPUNIT_ASSERT(nOutr==vOutrun.size() && "Встречены все торможения" );
}

void TC_AThread::testVarForecast()
{// тест построения вариантного графика  с вариацией окон
	TRACE("\r\n   ----  testVarForecast  -----");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)1.0);
	tpl.setPvx(20,&defTimes);
	Sevent tNrm1(AEvent::NONE,// тип событий
		70,// начало нити
		10,// время на станции
		50,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		3// число станций
		);
	Sevent tNrm1002(AEvent::NONE,// тип событий
		15,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		3, // нач.станция (+10 в конструкторе)
		3// число станций
		);
	Sevent sEv1(//реальная нить
		AEvent::REAL,// тип событий
		70,// начало нити
		10, // время на станции
		50/*30*/,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		3// число станций
		);
	Sevent sEv1002(//реальная нить
		AEvent::REAL,// тип событий
		82,// начало нити
		5,// время на станции
		30/*60*/,// время на перегоне
		0,// время на последнем перегоне
		3, // нач.станция (+10 в конструкторе)
		3// число станций
		);

	int tbl[]={1,1002};// генерируемые нормативки
	Stech tWnd( 115, 100 /*длительность*/, 1, 1/*park*/, 2/*e1*/,3,nullptr);
	TestPossessions p( AAdmin::getNsi() );
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech(tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	TestAAdmin adm(p);
	adm.StopBefAftLockPath(30, SpanLockType::BIDIR_AUTOBLOCK,TestAAdmin::Before,false);
	for (const auto& t: tbl)
	{
		Sevent& tNrm=t&1?tNrm1:tNrm1002;
		auto nrm=tpl.norma(tNrm,t);
		auto wNum=std::to_wstring(t);
		adm.addNorma(TrainDescr( wNum.c_str(), L""), nrm );
		Strain trn( TrainDescr(wNum.c_str(), L""),t&1?sEv1:sEv1002,Sevent( NO_FIXED ));
		adm.add(tpl.add(trn));
	}
// 	VAR_POSS  tmp( new TestPossessions( AAdmin::getNsi() ) );
// 	tmp->setLimitation( w, vR );
	adm.setVariant(true);
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto lns=trFrc.getSpotLines();
	CPPUNIT_ASSERT(lns.size()==2 && "Прогноз 2-х нитей");
	for (const auto& t: tbl)
	{
		auto it=std::find_if(lns.begin(),lns.end(),[t](const  std::pair<TrainDescr, V_EVNT>& tv){return t==tv.first.GetNumber().getNumber();});
		CPPUNIT_ASSERT(it!=lns.end() && "Должен быть прогноз этой нити");
		auto ev=std::find_if(it->second.begin(),it->second.end(),[&tWnd](const AEvent& et){return et.Esr()==EsrKit(tWnd.e1,tWnd.e2);});
		CPPUNIT_ASSERT(ev!=it->second.end() && "Должно быть событие");
		if(t==1)
			CPPUNIT_ASSERT(ev->Wrongway() && ev->diap.getEnd()>tWnd.or && "Проходим окно по непр.пути");
		else
			CPPUNIT_ASSERT(ev->diap.getEnd()<tWnd.or && "Проходим до окна");
	}
}

 
 
 
