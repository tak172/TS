/*!
	created:	2018/05/24
	created: 	12:54  24 Май 2018
	filename: 	F:\potapTrunk\Charm\UT\TC_LimWarn.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_LimWarn
	file ext:	cpp
	author:		 Dremin
	
	purpose:	тесты для ограничений при прогнозе
  
  */
 
 #include "stdafx.h"
 #include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"

void TC_AThread::testLimWarnSpan()
{// тест предупреждений на перегоне (огран.времени)
	TRACE("\r\n =========== testLimWarnSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60);
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	120/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		5/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv1, Sevent( NO_FIXED ));

		//нулевой указатель означает окно
	Stech tWnd( 1, 310 /*длительность*/, 1, 1/*park*/, 2/*e1*/,1/*e2*/,std::make_shared<unsigned int> (20));
	tWnd.kmLen=.5;
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
	TestAAdmin adm(p);
	adm.add(tpl.add(trn1));//ПЕ реальное
	auto nsiPtr = adm.getNsi();
	auto sRegPtr = nsiPtr->spansRegPtr;
	EsrKit esr(tWnd.e1,tWnd.e2);
	auto pt=	sRegPtr->getWay(esr,tWnd.path);
	CPPUNIT_ASSERT(pt );
	auto vlim= AAdmin::intersectsLim(pt,ADiap(tWnd.or,tWnd.or+2),trn1.tdescr.GetNumber().getNumber());
	CPPUNIT_ASSERT(vlim.size()==1);
	auto lnW=   vlim.front()->getKmLength();
	CPPUNIT_ASSERT(lnW && *lnW==tWnd.kmLen);
	auto ln= nsiPtr->stationsRegPtr->kmBetween(EsrKit(esr.getSpan().first),EsrKit(esr.getSpan().second));
	auto trFrc0= adm.forecastGlobal();
	tstTRACE(adm);
	auto lnEv=trFrc0.getSpotLines();
	CPPUNIT_ASSERT(lnEv.size()==1 );
	EsrKit eLm(tWnd.e1,tWnd.e2);
	auto train =adm.fnd(trn1);
	STT_Cat_Val pmt = train->getPMT(eLm);
	std::vector<AEvent> v=  lnEv.front().second;
	int eM =max(esr.getSpan().first,esr.getSpan().second);
	EsrKit eP(eM,eM+1);
	auto itL=std::find_if(v.begin(),v.end(),[&eLm](const AEvent& ev){return ev.Esr()==eLm;});
	auto it=std::find_if(v.begin(),v.end(),[&eP](const AEvent& ev){return ev.Esr()==eP;});
	CPPUNIT_ASSERT(it!=v.end() && itL!=v.end() );
	CPPUNIT_ASSERT(it->diap.duration()==pmt.full);
	CPPUNIT_ASSERT(itL->diap.duration()>2*pmt.full);
	CPPUNIT_ASSERT(itL->diap.duration()<2*pmt.full+ pmt.start+ pmt.stop);
}

void TC_AThread::testLimNormSpeedupSpan()
{// тест предупреждений на перегоне (огран.времени) при попытке нагнать нормативку
	TRACE("\r\n =========== testLimNormSpeedupSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60);
	Sevent sEv1(AEvent::REAL ,100/*начало нити*/,	120/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		5/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv1, Sevent( NO_FIXED ));


	Sevent tNrm(AEvent::NONE,// тип событий
		20,// начало нити
		0,// время на станции
		60,// время на перегоне
		0,// время на последнем перегоне
		6, // нач.станция
		7// число станций
		);
	auto norm=tpl.norma(tNrm,2);
	Stech tWnd( 1, 510 /*длительность*/, 1, 1/*park*/, 2/*e1*/,1/*e2*/,std::make_shared<unsigned int> (20));//огр.скорости на перегоне = предупреждение
	tWnd.kmLen=.5;
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
	TestAAdmin adm(p);
	adm.addNorma( trn1.tdescr, norm );
	adm.add(tpl.add(trn1));//ПЕ реальное
	auto trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto lnEv=trFrc.getSpotLines();
	CPPUNIT_ASSERT(lnEv.size()==1 );
	CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
	EsrKit esr(tWnd.e1,tWnd.e2);
	auto train =adm.fnd(trn1);
	STT_Cat_Val pmt = train->getPMT(esr);
	std::vector<AEvent> v=  lnEv.front().second;
	int eM = max(esr.getSpan().first,esr.getSpan().second);
	EsrKit eP(eM,eM+1);
	auto itL=std::find_if(v.begin(),v.end(),[&esr](const AEvent& ev){return ev.Esr()==esr;});
	auto it=std::find_if(v.begin(),v.end(),[&eP](const AEvent& ev){return ev.Esr()==eP;});
	auto vN=train-> getIdeal();// нормативка для ПЕ
	CPPUNIT_ASSERT(vN.size()>2 && vN[1].onSpan());
	ADiap dN =vN[1].diap;
	int tSpeedNorm=static_cast<int>(dN.duration()*(1.0 -SPEEDUP_NORM));
	CPPUNIT_ASSERT(it!=v.end() && itL!=v.end() );
	CPPUNIT_ASSERT(tSpeedNorm<it->diap.duration()+AAdmin::deltaThrough() && tSpeedNorm>it->diap.duration()) ;
	CPPUNIT_ASSERT(itL->diap.duration()>2*pmt.full);
	CPPUNIT_ASSERT(itL->diap.duration()<2*pmt.full+ pmt.start+ pmt.stop);
}
 
 
