/*!
	created:	2017/10/31
	created: 	14:05  31 Октябрь 2017
	filename: 	D:\potapTrunk\Charm\UT\TC_ThrTrn.cpp
	file path:	D:\potapTrunk\Charm\UT
	file base:	TC_ThrTrn
	file ext:	cpp
	author:		 Dremin
	
	purpose:	расширение тестов для прогноза
  
  */
 
#include "stdafx.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/AClash.h"

using namespace std; 

void TC_AThread::testTrainStation()// тест ПЕ на станции без маршрута
{ 
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );// однопутка, 3 пути на станции
	tpl.setPvx(20);
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Strain tr1( TrainDescr( L"1", L"2-100-72"),sEv1, Sevent( NO_FIXED ));
	TestAAdmin adm;
	V_EVNT vEv=tpl.bld(sEv1,1);
	AEvent ev1=vEv.back();
	CPPUNIT_ASSERT(vEv.size()==1 && !ev1.onSpan() && ev1.getType()==AEvent::STOP_START);
	adm.add(tpl.add(tr1));//ПЕ реальное
	auto v=adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().size()==1 && !adm.getClash().front()->event() && "Исключение должно быть при построении маршрута.");
	CPPUNIT_ASSERT(v.getSpotLines().size()==1 && v.getSpotLines().front().first==tr1.tdescr && v.getSpotLines().front().second.empty() );
	auto vtr=adm.getTrn();
	CPPUNIT_ASSERT(vtr.size()==1 && vtr.front()->getNum()==1);
	CPPUNIT_ASSERT(vtr.front()-> getIdeal().empty());
}

typedef std::vector<int> V_T;
V_T sortCargoTrn(V_T vTrn)// сортировка для грузовых ПЕ по весу ПЕ
{
	V_T r=vTrn;
	auto ind=r.size();
	while(ind>1)
	{
		auto t=r[ind -1];
		int repl=-1;
		int i = static_cast<int>( ind-2 );
		for (; i >=0; i--)
		{
			auto it=r[i];
			if(it<t)/*превышение бруттто для груз.ПЕ, дающее преимущество  для встречных*/
				repl=i;
		}
		if(repl<0)
			ind--;
		else
		{
			V_T rt(r);
			rt.insert(rt.begin()+repl,t);
			rt.erase(rt.begin()+ind);
			r=rt;
		}
	}
	return r;
}


void TC_AThread::testIdealForTrGoods()// построение идеального прогноза для Грузового ПЕ
{
	TRACE("\r\n   ----  testIdealForTrGoods  -----");

	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );// однопутка, 3 пути на станции
	tpl.setPvx(20);
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Strain tr1( TrainDescr( L"1", L"2-100-72"),sEv1, Sevent( NO_FIXED ));
	Sevent sEv1001(AEvent::REAL ,50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Strain tr1001( TrainDescr( L"1001", L"2-100-72"),sEv1001, Sevent( NO_FIXED ));
	Strain tr1003( TrainDescr( L"1003", L""),sEv1001, Sevent( NO_FIXED ));
	Sevent sN(/* нормативка*/AEvent::NONE,30/*начало нити*/,20/*время на станции*/,20/*время на перегоне*/,
		0/*время на последнем перегоне*/,5/*нач.станция(+10 в конструкторе)*/,6/*число станций*/) ;
	auto norm1=tpl.norma(sN,1);
	auto norm1001=tpl.norma(sN,1001);
	TestAAdmin adm;
	adm.addNorma( tr1.tdescr, norm1 );
	adm.addNorma( tr1001.tdescr, norm1001 );
	adm.addNorma(tr1003.tdescr,norm1001);
	 adm.add(tpl.add(tr1));
	 adm.add(tpl.add(tr1001));
	 adm.add(tpl.add(tr1003));
	 adm.fndAllNorm();// для всех ПЕ ищется нормативка
	 ATrainPtr t1= adm.fnd(tr1);
	 ATrainPtr t1001= adm.fnd(tr1001);
	 ATrainPtr t1003= adm.fnd(tr1003);
	 tstTRACE(adm);
	 CPPUNIT_ASSERT(t1 && t1->getNorm() && "Для пассажирского ПЕ нормативка остается.");
	 CPPUNIT_ASSERT(t1001 && !t1001->getNorm()  &&  t1001->buildTarget() && "Для грузового ПЕ с индексом нормативка убирается.");
	 CPPUNIT_ASSERT(t1003 && t1003->getNorm()  &&  !t1003->buildTarget() && "Для грузового ПЕ без индекса нормативка остается.");
}


void TC_AThread::testSortCargoTr()
{// алгоритм сортировки Грузовых ПЕ

	int tb[]={ 1,2,3,4,5};
	V_T v(tb,tb+size_array(tb));
	V_T vt=sortCargoTrn(v);
	int tbr[]={5,4,3,2,1};
	V_T vtt(tbr,tbr+size_array(tbr));
	ASSERT(vt==vtt && "Сортировка ");

	TRACE("\r\n   ----  testSortCargoTr  -----");

	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	TestAAdmin adm;
	adm.rgSortCargo=true;
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1001(AEvent::REAL ,52/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1003(AEvent::REAL ,60/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1002(AEvent::REAL ,50/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		9/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv1004(AEvent::REAL ,62/*начало нити*/,	10/*время на станции*/,20/*время на перегоне*/,0/*время на последнем перегоне*/,
		9/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Strain tr1( TrainDescr( L"1", L"42-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain tr1001( TrainDescr( L"2001", L"62-100-2"),sEv1001, Sevent( NO_FIXED ));
	Strain tr1003( TrainDescr( L"2003", L"72-101-2"),sEv1003, Sevent( NO_FIXED ));
	Strain tr1002( TrainDescr( L"2002", L"22-100-1"),sEv1002, Sevent( NO_FIXED ));
	Strain tr1004( TrainDescr( L"2004", L"32-101-1"),sEv1004, Sevent( NO_FIXED ));

	adm.add(tpl.add(tr1002));
	adm.add(tpl.add(tr1004));
	adm.add(tpl.add(tr1));
	adm.add(tpl.add(tr1001));
	adm.add(tpl.add(tr1003));
	V_TRN_PTR vTr= adm.prepareTrains();
	for ( auto& t:adm.allTrn )
		t->firstStepForecast();
	{
		auto v= adm.sort();
		CPPUNIT_ASSERT( v[0]->getNum()==1  && " Пассажирский ПЕ приоритетнее" );
		auto n2001= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2001==t->getNum();}));
		auto n2002= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2002==t->getNum();}));
		auto n2003= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2003==t->getNum();}));
		auto n2004= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2004==t->getNum();}));
		CPPUNIT_ASSERT(n2001<n2003 && n2002<n2004);

	}

 	ATrainPtr t1003= adm.fnd(tr1003);
	ATrainPtr t1002= adm.fnd(tr1002);
	t1003->tdescr.SetBrutto(AAdmin::Advantage_brutto()+1);
	{
		auto v= adm.sort();
		CPPUNIT_ASSERT( v[0]->getNum()==1);
		auto n2001= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2001==t->getNum();}));
		auto n2002= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2002==t->getNum();}));
		auto n2003= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2003==t->getNum();}));
		auto n2004= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2004==t->getNum();}));
		CPPUNIT_ASSERT(n2003<n2001 && n2002<n2004 && " Тяжелый грузовой" );
	}
	t1002->tdescr.SetBrutto(2*AAdmin::Advantage_brutto());
	{
		auto v= adm.sort();
		CPPUNIT_ASSERT( v[0]->getNum()==1);
		auto n2001= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2001==t->getNum();}));
		auto n2002= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2002==t->getNum();}));
		auto n2003= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2003==t->getNum();}));
		auto n2004= std::distance(v.begin(),std::find_if(v.begin(),v.end(),[](const ATrainPtr& t){return 2004==t->getNum();}));
		CPPUNIT_ASSERT(n2003<n2001 && n2002<n2004 && " Нет критического преимущества веса грузового над встречными" );
	}
	t1003->tdescr.SetBrutto(3*(AAdmin::Advantage_brutto()+1));
	{
		auto v= adm.sort();
		CPPUNIT_ASSERT( v[0]->getNum()==1 && v[1]->getNum()==2003 && v[2]->getNum()==2002 /*&& v[3]->getNum()==2001*/	
			&& "Критическое преимущество веса грузового над встречными" );
	}
	//adm.rgSortCargo=false;
}

static map<EsrKit, TechnicalTime> makeTimeForTestTrnEmptyInd()
{
	typedef TechnicalTime TT;
	RoadKit belorussiaRoad( RoadCode::belorussia,L"Белорусь" ), latvianRoad( RoadCode::latvianNationalCode ), emptyRoad;
	map<EsrKit, TechnicalTime> techTimes;

	techTimes.insert( make_pair( EsrKit(12), TechnicalTime() ) );
	TT::Specification specification( Oddness::ODD, TT::Fullness::Any, emptyRoad, emptyRoad );
	techTimes[EsrKit(13)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 3 ) ) );
	specification = TT::Specification( Oddness::EVEN, TT::Fullness::Any, belorussiaRoad, latvianRoad );
	techTimes[EsrKit(13)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 4 ) ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, belorussiaRoad, latvianRoad );
	techTimes[EsrKit(13)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 5 ) ) );

	techTimes.insert( make_pair( EsrKit(14), TechnicalTime() ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, emptyRoad, emptyRoad );
	techTimes[EsrKit(15)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 2 ) ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Full, emptyRoad, emptyRoad );
	techTimes[EsrKit(16)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 2 ) ) );

	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, latvianRoad, latvianRoad );
	techTimes[EsrKit(17)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 7 ) ) );

	specification = TT::Specification( Oddness::EVEN, TT::Fullness::Any, latvianRoad, latvianRoad );
	techTimes[EsrKit(18)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 8 ) ) );
	specification = TT::Specification( Oddness::EVEN, TT::Fullness::Empty, emptyRoad, belorussiaRoad );
	techTimes[EsrKit(19)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 2 ) ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, belorussiaRoad, emptyRoad );
	techTimes[EsrKit(19)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 2 ) ) );
	specification = TT::Specification( Oddness::EVEN, TT::Fullness::Any, emptyRoad, emptyRoad );
	techTimes[EsrKit(20)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 2 ) ) );

	return techTimes;
}

void TC_AThread::testTrnEmptyInd()// отбор маршрута для ПЕ без индекса
{
	/*Поездам только с номером (без индекса) строить прогноз только до ближайшей технической станции,
	за исключением случаев когда техническая стоянка задана для любых полигонов формирования и назначения.*/

	// однопутка, 3 пути на станции
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, makeTimeForTestTrnEmptyInd() );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20) ;

	Sevent sN(// нормативка
		AEvent::NONE,// тип событий
		30,// начало нити
		100,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		1, // нач.станция(+10 в конструкторе)
		16 // число станций
		) ;
	Sevent sN2(// нормативка
		AEvent::NONE,// тип событий
		30,// начало нити
		100,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		14, // нач.станция(+10 в конструкторе)
		16 // число станций
		) ;
	auto norm3=tpl.norma(sN,3);
	auto norm2=tpl.norma(sN2,2);
	Sevent sEv(//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		15,// время на станции
		25,// время на перегоне
		5,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		1// число станций
		);

	Sevent sEv2(//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		15,// время на станции
		25,// время на перегоне
		5,// время на последнем перегоне
		12, // нач.станция (+10 в конструкторе)
		1// число станций
		);

	Strain trn5( TrainDescr( L"1005", L"1-100-2" ),	sEv, Sevent( NO_FIXED )	);
	Strain trn3( TrainDescr( L"3", L"" ),	sEv, Sevent( NO_FIXED )	);
	Strain trn9003( TrainDescr( L"9003", L"" ),	sEv, Sevent( NO_FIXED )	);

	Strain trn2( TrainDescr( L"3002", L"" ),	sEv2, Sevent( NO_FIXED )	);

	TestAAdmin adm;

	adm.add(tpl.add(trn3));//ПЕ реальное
	adm.add(tpl.add(trn9003));//ПЕ реальное
	adm.add(tpl.add(trn5));//ПЕ реальное
	adm.add(tpl.add(trn2));//ПЕ реальное
	adm.addNorma(TrainDescr( L"3", L""), norm3 );
	adm.addNorma(TrainDescr( L"9003", L""), norm3 );
	adm.addNorma(TrainDescr( L"3002", L""), norm2 );
	adm.fndAllNorm();
	{
		ATrainPtr t=adm.fnd(trn5);
		V_EVNT vC= t->prepare();// возвращает нормализ.реальную нить
		V_EVNT vId=t->buildIdeal().first;// построение по ПВХ
		CPPUNIT_ASSERT(vId.front().Esr()==EsrKit(13) && vId.back().Esr()==EsrKit(20) && " При наличии индекса строим прогноз до конечной станции по индексу. ");
	}
	{
		ATrainPtr t=adm.fnd(trn9003);
		V_EVNT vC= t->prepare();// возвращает нормализ.реальную нить
		V_EVNT vId=t->buildIdeal().first;// построение по ПВХ
		CPPUNIT_ASSERT(vId.front().Esr()==EsrKit(13) && vId.back().Esr()==EsrKit(16) && " На ЕСР=16 техническая стоянка задана не для любого нетто. ");
	}
	{
		ATrainPtr t=adm.fnd(trn3);
		V_EVNT vC= t->prepare();// возвращает нормализ.реальную нить
		V_EVNT vId=t->buildIdeal().first;// построение по ПВХ
		CPPUNIT_ASSERT(vId.front().Esr()==EsrKit(13) && vId.back().Esr()==EsrKit(26) && " Для пасс.ПЕ техническая стоянка не учитывается. ");
	}
	{
		ATrainPtr t=adm.fnd(trn2);
		V_EVNT vC= t->prepare();// возвращает нормализ.реальную нить
		V_EVNT vId=t->buildIdeal().first;// построение по ПВХ
		CPPUNIT_ASSERT(vId.front().Esr()==EsrKit(21) && vId.back().Esr()==EsrKit(19) && "техническая стоянка задана  не для любых полигонов формирования и назначения. ");
	}
}

static map<EsrKit, TechnicalTime> makeTimeForTestNoInfoTrn()
{
	typedef TechnicalTime TT;
	RoadKit belorussiaRoad( RoadCode::belorussia,L"Белорусь" ), latvianRoad( RoadCode::latvianNationalCode ), emptyRoad;
	map<EsrKit, TechnicalTime> techTimes;

	techTimes.insert( make_pair( EsrKit(12), TechnicalTime() ) );
	TT::Specification specification( Oddness::UNDEF, TT::Fullness::Any, emptyRoad, emptyRoad, TrainCharacteristics::TrainFeature::FastAllYear );
	techTimes[EsrKit(13)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 3 ) ) );
	techTimes.insert( make_pair( EsrKit(14), TechnicalTime() ) );
	specification = TT::Specification( Oddness::UNDEF, TT::Fullness::Any, emptyRoad, emptyRoad );
	techTimes[EsrKit(15)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 12 ) ) );

	return techTimes;
}

void TC_AThread::testNoInfoTrn()
{ //техн.время для типа поезда "Неопределенные" относится ко всем поездам за исключением поездов прогнозируемых по нормативному графику.

	// однопутка, 3 пути на станции
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, makeTimeForTestNoInfoTrn() );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20) ;

	Sevent sN(// нормативка
		AEvent::NONE,// тип событий
		30,// начало нити
		100,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		1, // нач.станция(+10 в конструкторе)
		7 // число станций
		) ;
	auto norm3=tpl.norma(sN,3);
	Sevent sEv(//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		15,// время на станции
		25,// время на перегоне
		5,// время на последнем перегоне
		1, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Strain trn5( TrainDescr( L"5", L"1-100-2" ),	sEv, Sevent( NO_FIXED )	);
	Strain trn3( TrainDescr( L"3", L"" ),	sEv, Sevent( NO_FIXED )	);

	TestAAdmin adm;

	adm.add(tpl.add(trn3));//ПЕ реальное
	adm.add(tpl.add(trn5));//ПЕ реальное
	adm.addNorma(TrainDescr( L"3", L""), norm3 );
	adm.fndAllNorm();
	{
		ATrainPtr t=adm.fnd(trn3);
		V_EVNT vC= t->prepare();// возвращает нормализ.реальную нить
		V_EVNT vId=t->buildIdeal().first;// построение по ПВХ
		auto it15= std::find_if(vId.begin(),vId.end(),[](const AEvent& ev){return ev.Esr()==EsrKit(15);});
		CPPUNIT_ASSERT(it15!=vId.end() && it15->diap.duration()==100 && " На ЕСР=15 стоянка для ПЕ по нормативке.");
	}
	{
		ATrainPtr t=adm.fnd(trn5);
		V_EVNT vC= t->prepare();// возвращает нормализ.реальную нить
		V_EVNT vId=t->buildIdeal().first;// построение по ПВХ
		auto it15= std::find_if(vId.begin(),vId.end(),[](const AEvent& ev){return ev.Esr()==EsrKit(15);});
		CPPUNIT_ASSERT(it15!=vId.end() && it15->diap.duration()==12*60 && " На ЕСР=15 техническая стоянка задана для ПЕ без нормативки.");
	}
}

void TC_AThread::testProperNorm()
{// подбор требуемой нормативкии для отправления со станции
	typedef TechnicalTime TT;
	const TrainCharacteristics & trnChr=  TrainCharacteristics::instanceCRef();

	for (const auto& t: ATrain::properTypeNorm)
	{
		auto v= trnChr.GetPurposeNumRanges( t ) ;
		auto info= trnChr.GetTrainFeatureInfo(t);
		stringstream s;
		s<<std::endl <<info.xml_name<<" = ";
		for (const auto& it:v )
			s<<it.start<<":"<<it.end<<"  ";
		TRACE(s.str().c_str());
	}

	RoadKit  emptyRoad;
	map<EsrKit, TechnicalTime> techTimes;

	techTimes.insert( make_pair( EsrKit(12), TechnicalTime() ) );
	TT::Specification specification( Oddness::UNDEF, TT::Fullness::Any, emptyRoad, emptyRoad, TTF::Refrigerator );
	techTimes[EsrKit(15)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 1 ) ) );

	// однопутка, 3 пути на станции
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes );
	tplParams.departureByHemPath.insert(EsrKit(15));// где искать привязку к нормативке
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20) ;

	Sevent sN(// нормативка
		AEvent::NONE,// тип событий
		30,// начало нити
		100,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		2, // нач.станция(+10 в конструкторе)
		7 // число станций
		) ;
	auto norm3=tpl.norma(sN,1003);
	AEvent evN;
	for (const auto& t: norm3)
		if(t.Esr()==EsrKit(15))
			evN=t;
	Sevent sEv(//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		15,// время на станции
		25,// время на перегоне
		5,// время на последнем перегоне
		1, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Strain trn( TrainDescr( L"1005", L"1-100-2" ),	sEv, Sevent( NO_FIXED )	);

	const wchar_t*tb[]={L"1003", L"1401", L"1899", L"3111", L"9703"};
	vector<wstring>vWs(ARRAY_IT(tb));
	for (const auto& l:vWs )
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn));//ПЕ реальное
		adm.addNorma(TrainDescr( l.c_str(), L""), norm3 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(v.getSpotLines().size()==1);
		auto vT=v.getSpotLines().front();
		AEvent ev;
		for (const auto& t: vT.second)
			if(t.Esr()==EsrKit(15))
				ev=t;
		CPPUNIT_ASSERT(evN.Esr()==ev.Esr() && evN.diap.getEnd()==ev.diap.getEnd() && " уход должен быть по нормативке");
	}
	TestAAdmin adm;
	adm.add(tpl.add(trn));//ПЕ реальное
	adm.addNorma(TrainDescr( L"4001", L""), norm3 );
	auto v=adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(v.getSpotLines().size()==1);
	auto vT=v.getSpotLines().front();
	AEvent ev;
	for (const auto& t: vT.second)
		if(t.Esr()==EsrKit(15))
			ev=t;
	CPPUNIT_ASSERT(evN.Esr()==ev.Esr() && evN.diap.getEnd()>ev.diap.getEnd() && " уход должен быть не по нормативке");
}
