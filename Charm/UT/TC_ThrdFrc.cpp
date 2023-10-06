/*!
	created:	2017/01/26
	created: 	18:36  26 Январь 2017
	filename: 	D:\potap\Charm\UT\TC_ThrdFrc.cpp
	file path:	D:\potap\Charm\UT
	file base:	TC_ThrdFrc
	file ext:	cpp
	author:		 Dremin
	
	purpose:	
  
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../helpful/StationsRegistry.h"
#include "../helpful/SpansRegistry.h"
#include "../Augur/Occupancy.h"
#include "../Augur/RailLimitation.h"
#include "../Augur/TrainRoutes.h"
#include "TestAAdmin.h"
#include "TestPossessions.h"
#include "../Augur/ALogWrn.h"
#include "../helpful/TrainsPriorityOffsets.h"
#include "../helpful/TrainsPriorityDefiner.h"
#include "../Augur/TxtComment.h"

void TC_AThread::tstTRACE(const NsiBasis& bs,bool full)
{
	TRACE("\r\n ---- TRACE NsiBasis ----");
	auto sp= bs.spanTravTimePtr;
	if (sp)
	{
		TRACE("\r\n ---- TRACE ПВХ ----");
		std::vector <SpanTravelTimeInfo> vS = sp->GetSpans();
		for (const auto& t: vS)
		{
			TRACE("\r\n    ==  %s ==" ,t.esr_string().c_str());
			STT_Val sv = sp->GetSpanTime(t);
            if ( sv.pGruz )
                TRACE("\r\n Грузовой     start=%d, stop=%d,reserv=%d,full=%d ", sv.pGruz->start, sv.pGruz->stop, sv.pGruz->reserv, sv.pGruz->full);
            if ( sv.pPass )
                TRACE("\r\n Пассажирский start=%d, stop=%d,reserv=%d,full=%d ", sv.pPass->start, sv.pPass->stop, sv.pPass->reserv, sv.pPass->full);
			if( !full )
				break;
		}
	}
	auto cl= bs.stIntPtr;
	if (cl)
	{
		TRACE("\r\n ---- TRACE станц.интервалы ----");
		std::vector<long>st=cl->getStations();
		for (const auto& t: st)
		{
			st_intervals_t s1=cl->getInterval(StationEsr(t),StationEsr(t+1));
			st_intervals_t s2=cl->getInterval(StationEsr(t),StationEsr(t-1));
			{
				TRACE("\r\n    == станция %d ==" ,t);
				TRACE("\r\n %s \r\n %s ",s1.text().c_str(),s2.text().c_str());
				if(!full)
					break;
			}
		}
	}
}


void TC_AThread::tstTRACE(const TestAAdmin& adm,bool full)
{
	int esr=10;
	auto nsiPtr = adm.getNsi();
	while (true)
	{
		if(	nsiPtr->stationsRegPtr->getWays(EsrKit(++esr)).empty())
			break;
	}
	for(int n=10;n<(full?esr:11);n++) // перегоны +станции
	{
		StationEsr esr1(n); // перегоны +станции
		StationEsr esr2(n+1); // перегоны +станции
		st_intervals_t t1=nsiPtr->stIntPtr->getInterval(esr1,esr2);
		st_intervals_t t2=nsiPtr->stIntPtr->getInterval(esr2,esr1);
		std::stringstream ss;
		ss<<"\r\n ----Intervals >>PACK="<<t1.tp.count()  <<" PACK="<<t2.tp.count();
		TRACE(ss.str().c_str());
	}
	for (const auto& t:adm.getNorm())// все нормативки
	{
		TRACE("\r\n ---- TRACE Norm %d ----",t.first);
		AEvent prev;
		for (const auto& v: t.second)
			for (const auto& ev:v )
			{
				std::stringstream ss;
				ss<<"\r\n esr="<<ev.Esr().getTerm();
				if(!prev.empty() && prev.onSpan())
					ss<<"  на перегоне "<< ev.diap.getOrig()-prev.diap.getEnd();
				ss<<"  на станции "<<ev.diap.getOrig() <<"-"<<ev.diap.getEnd() <<" {"<<ev.diap.duration() <<"} ";
				TRACE(ss.str().c_str());
				prev=ev;
			}
	}
	TRACE("\r\n ---- TRACE tech limit ----");
	for(int n=10;n<esr+10;n++) // перегоны
	{
		long esr1=n; // перегоны 
		long esr2=esr1+1; // перегоны 
		auto pthSt=nsiPtr->stationsRegPtr->getWays(EsrKit(esr1));
		auto pthSp=nsiPtr->spansRegPtr->getWays(EsrKit(esr1,esr2));
		for ( auto& pt:pthSp)
		{
			BUSYLIM bl=AAdmin::intersectsLim(pt,ADiap(),0);
			for (auto& tsb:bl )
			{
				const RailLimitation* tch=tsb.get();
				std::stringstream ss;
				ss<<"\r\n перегон="<<esr1<<"-"<<esr2<<" путь "<<pt->Num()<<" ограничение "<<tch->to_string()
					<<"  время "<<tch->getDiap().getOrig()<<"-"<<tch->getDiap().getEnd();
				TRACE(ss.str().c_str());
			}
		}
		for (const auto& pt:pthSt)
		{
			BUSYLIM bl=AAdmin::intersectsLim(pt,ADiap(),0);
			for (const auto& tsb:bl )
			{
				const RailLimitation* tch=tsb.get();
				std::stringstream ss;
				ss<<"\r\n станция="<<esr1<<"путь "<<pt->Num()<<" ограничение "<<tch->to_string()
					<<"  время "<<tch->getDiap().getOrig()<<"-"<<tch->getDiap().getEnd();
				TRACE(ss.str().c_str());
			}
		}
	}
	tstTRACEtrn(adm);
}

void TC_AThread::tstTRACEtrn(const TestAAdmin& adm)
{
	std::stringstream r;
	r<<std::endl<<" ---- TRACE all trains ----"<<std::endl<<" выполнено число шагов прогноза ="<<adm.numStepFrc.get()+1
		<<std::endl<<" суммарное отклонение от идеального прогноза ="<<std::endl<< adm.messDeviatFromIdeal();
	TRACE(r.str().c_str());
	for (const auto& t: adm.getTrn())
	{
		TRACE("\r\n ---- train N %d (цель %d) ----",t->getNum(),t->descr().GetIndex().destination().getTerm());
			//ALogWrn::convWstring(t->getTrainDescr().GetIndex().str()).c_str());
		TRACE("\r\n ....   real   ....");
		for (const auto& ev:*(t->real.get()))
			if(ev.getSource()==AEvent::REAL)
			{
			std::stringstream ss;
			ss<<std::endl<<ALogWrn::convWstring(ev.text(nullptr))<<" {"<<ev.diap.getOrig() <<"-"<<ev.diap.getEnd() <<" = "<<ev.diap.duration() <<"} ";
			TRACE(ss.str().c_str());
			}
		TRACE("\r\n ....   прогноз   ....\n");
		EsrKit esr;
		if(!t->real->empty())
			esr=t->real->back().Esr();
		long e1=t->getRoute().firstInRoute(esr);
		auto vStat=t->real->fnd(EsrKit(e1));
		AEvent prev;
		if(!vStat.empty())
			prev=vStat.front();
		if(!t->frc.empty())
		{
			time_t prevId=prev.diap.getEnd();
			int ind=0;
			std::string errPath;
			for (const auto& ev:t->frc )
			{
				const AEvent* evPtrId=t->ideal.fnd(ev.Esr(),AIdeal::SAMPLE);
				AEvent evId;
				if(evPtrId)
					evId=*evPtrId;
				if(ev.onSpan())
				{
					auto s= adm.getNsi()->spansRegPtr->getWays(ev.Esr());
					errPath= ev.Num()==t->needPath(ev.Esr()) || s.size()<2 ?"":" НЕПРАВ ПУТЬ ";
				}
				else
				{
					std::stringstream ss;
					if(t->nrm)
					{
						int dis=0;
						V_EVNT::const_iterator itNr=std::find_if(t->nrm->begin(),t->nrm->end(),[&ev](const AEvent& evN){return evN.Esr()==ev.Esr();});
						if(itNr!=t->nrm->end())
							dis=static_cast<int>(ev.diap.getOrig()-itNr->diap.getOrig()) ;
						ss/*<<std::endl*/<<"esr="<< ev.Esr().getTerm()<<"  отставание от норм. "<<dis <<" на перегоне "<<
							errPath.c_str()<<" "<<ev.diap.getOrig()-prev.diap.getEnd() <<" {"<<evId.diap.getOrig()-prevId <<"} на станции "<<
							ev.diap.duration()<<" {"<<evId.diap.duration() <<"} ("<<ev.diap.getOrig() <<"-"<<ev.diap.getEnd() <<
							" {"<<evId.diap.getOrig() <<"-"<<evId.diap.getEnd() <<"}) "<<"/ путь="<<ev.Num()<<"/";
					}
					else
						ss/*<<std::endl*/<<"esr="<< ev.Esr().getTerm()<<"  на перегоне "<<errPath.c_str()<<" "<<ev.diap.getOrig()-prev.diap.getEnd() <<
						" {"<<evId.diap.getOrig()-prevId <<"} на станции "<<ev.diap.duration() <<" {"<<evId.diap.duration() <<"} ("<<ev.diap.getOrig()
						<<"-"<<ev.diap.getEnd() <<" {"<<evId.diap.getOrig() <<"-"<<evId.diap.getEnd() <<"}) "<<"/ путь="<<ev.Num()<<"/";
					ss<<(ev.getSource()==AEvent::FIXED ?" FIX":(ev.getSource()!=AEvent::FRC ?" NO_SRC":""))<<std::endl
						<<ALogWrn::convWstring(t->txtOutrun(ev.Esr().getTerm()));
					TRACE(ss.str().c_str());
					prev=ev; 
					prevId=evId.diap.getEnd();
				}
				ind++;
			}
		}
		else
			for (const auto& ev:t->ideal.get(AIdeal::SAMPLE))
				if(!ev.onSpan())
				{
					std::stringstream ss;
					ss/*<<std::endl*/<<"идеал esr="<<ev.Esr().getTerm() <<"  на перегоне "<<ev.diap.getOrig()-prev.diap.getEnd() <<" на станции  "<<ev.diap.getOrig() <<"-"
						<<ev.diap.getEnd() <<" ("<<ev.diap.duration() <<") "<<std::endl;
					TRACE(ss.str().c_str());
					prev=ev;
				}
	}
}

void TC_AThread::testCatchNorm()
{// догоняем нормативку
	TRACE("\r\n =========== testCatchNorm ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 2, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	TestAAdmin adm;
	ATrainPtr atrainPtr = tpl.add(sd.tblTrCtchNrm); //ПЕ
	adm.add( atrainPtr );

	auto norm=tpl.norma( sd.tblCtchNrm, sd.tblTrCtchNrm.tdescr.GetNumber().getNumber() );// нормативка для ПЕ
	adm.addNorma( sd.tblTrCtchNrm.tdescr, norm );
	
	ATrain* trn=adm.getTrn().front().get();
	trn->setNorm(&norm);
	V_EVNT vC= trn->prepare();
	trn->firstStepForecast();
	trn->buildFrc();
	V_EVNT frc=trn->mergeFrcIdealReal();
	tstTRACE(adm);
	int dFp=0;
	for (const auto& t: norm)
	{
		V_EVNT::iterator itR=std::find_if(vC.begin(),vC.end(),[&t](const AEvent& ev){return t.Esr()==ev.Esr();});
		if(itR!=vC.end())
		{
			int dis=static_cast<int>( t.diap.getEnd())-static_cast<int>(itR->diap.getEnd());
			dFp=dis;
		}
		else
		{
			itR=std::find_if(frc.begin(),frc.end(),[&t](const AEvent& ev){return t.Esr()==ev.Esr();});
			int dis=static_cast<int>( t.diap.getOrig())-static_cast<int>(itR->diap.getOrig());
			CPPUNIT_ASSERT((dis>dFp ||(!dis && !dFp))  && " сокращение отставания от нормативки");
			dFp=dis;
		}
	}
	ADiap dN(norm[0].diap.getEnd(),norm[1].diap.getOrig());
	AEvent ev=frc.front();
	CPPUNIT_ASSERT(ev.onSpan() && ev.getSource()==AEvent::FRC && ev.diap.duration()<dN.duration() && "начало прогнозной нити на перегоне");
	AEvent evr= trn->fndEv(ev.Esr());
	CPPUNIT_ASSERT(ev.Esr()==evr.Esr() && evr.diap.getOrig()<ev.diap.getOrig() && evr.diap.duration()<dN.duration() && "конец реальной нити на перегоне");
	AEvent ev1=trn->fndEv(EsrKit(ev.Esr().getSpan().first));// найти событие на соотв.месте 
	AEvent ev2=trn->fndEv(EsrKit(ev.Esr().getSpan().second));// найти событие на соотв.месте 
	ADiap d(ev1.diap.getEnd(),ev2.diap.getOrig());// время на перегоне на первом шаге (реальное +прогноз)
	CPPUNIT_ASSERT( (1.-SPEEDUP_NORM)*dN.duration()==d.duration() && " догоняем нормативку и на первом шаге прогноза");
}

void TC_AThread::testDeadlock()
{// тест клинча
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Strain tblTrDdl2( TrainDescr( L"2", L"18-100-1" ),
		Sevent(
		//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		10,// время на станции
		25,// время на перегоне
		0,// время на последнем перегоне
		11, // нач.станция
		1// число станций
		),
		Sevent( NO_FIXED )) ;

	Strain tblTrDdl4( TrainDescr( L"4", L"18-100-1" ),
		Sevent(
		//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		10,// время на станции
		25,// время на перегоне
		0,// время на последнем перегоне
		11, // нач.станция
		1// число станций
		),	Sevent( NO_FIXED )) ;
	 Strain tblTrDdl1003( TrainDescr( L"1003", L"1-100-18" ),
		Sevent(
		//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		115,// время на станции
		25,// время на перегоне
		0,// время на последнем перегоне
		10, // нач.станция
		1// число станций
		),	Sevent( NO_FIXED ));
	  Strain tblTrDdl1001(  TrainDescr( L"1001", L"1-100-18" ),
		 Sevent(
		 //реальная нить
		 AEvent::REAL,// тип событий
		 20,// начало нити
		 115,// время на станции
		 25,// время на перегоне
		 0,// время на последнем перегоне
		 10, // нач.станция
		 1// число станций
		 ), Sevent( NO_FIXED ) ) ;

	TestAAdmin adm;
	ACounterInt::limit=1; 

	adm.add(tpl.add(tblTrDdl2));//ПЕ конфликтующее
	adm.add(tpl.add(tblTrDdl4));//ПЕ конфликтующее
	adm.add(tpl.add(tblTrDdl1003));//ПЕ конфликтующее
	adm.add(tpl.add(tblTrDdl1001));//ПЕ конфликтующее
	V_EVNT eps;
	for (auto& t: adm.getTrn())
	{
		AEvent ev=t->origUnionFrc();
		for (const auto& et:eps )
			if(ev.equObj(et)) 
			{
				ev.setObj( ev.Num()==1?2:1,ev.Park());
				for (auto& rt:*(t->real.get()) )
				{
					if(rt.Esr()== ev.Esr())
						rt.setObj(ev.Num(),ev.Park());
				}
				break;
			}
		eps.push_back(ev);
	}
	TRACE("\r\n =========== testDeadlock ============");
// 	tstTRACE(*adm.getNsi());
//	tstTRACE(adm);
	adm.forecastGlobal();
	tstTRACE(adm);
	VCLASH vCl=adm.getClash();
	CPPUNIT_ASSERT(vCl.size()==2 && "В тесте была  взаимная блокировка");
	auto n=std::count_if(vCl.begin(),vCl.end(),[](const P_CLSH&t){return t->Comment()==INVALID_ACT_OBJ;});
	CPPUNIT_ASSERT(n==2 && "На станции 2 пути и два ПЕ с одной станции и 2 с другой");
}

void TC_AThread::testWaitlock()
{//  ожидание занятых путей на станции
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Strain tblTrDdl2( TrainDescr( L"2", L"18-100-1" ),
		Sevent(
		//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		10,// время на станции
		25,// время на перегоне
		0,// время на последнем перегоне
		11, // нач.станция
		1// число станций
		),
		Sevent( NO_FIXED )) ;
	Strain tblTrDdl4( TrainDescr( L"4", L"18-100-1" ),
		Sevent(
		//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		10,// время на станции
		25,// время на перегоне
		0,// время на последнем перегоне
		11, // нач.станция
		1// число станций
		),	Sevent( NO_FIXED )) ;
	Strain tblTrDdl1003( TrainDescr( L"6003", L"1-100-18" ),
		Sevent(
		//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		115,// время на станции
		25,// время на перегоне
		0,// время на последнем перегоне
		10, // нач.станция
		1// число станций
		),	Sevent( NO_FIXED ));
	Strain tblTrDdl1001(  TrainDescr( L"6001", L"1-100-18" ),
		Sevent(
		//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		115,// время на станции
		25,// время на перегоне
		0,// время на последнем перегоне
		10, // нач.станция
		1// число станций
		), Sevent( NO_FIXED ) ) ;
	Sevent tblNrm1001( 	AEvent::NONE,// тип событий
		30,// начало нити
		100,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		10, // нач.станция
		3// число станций
		) ;
	Sevent tblNrm1003( 	AEvent::NONE,// тип событий
		60,// начало нити
		100,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		10, // нач.станция
		3// число станций
		);
	TestAAdmin adm;
	ACounterInt::limit=10; 
	adm.add(tpl.add(tblTrDdl2,1));//ПЕ конфликтующее
	adm.add(tpl.add(tblTrDdl4));//ПЕ конфликтующее
	adm.add(tpl.add(tblTrDdl1003,2));//ПЕ конфликтующее
	adm.add(tpl.add(tblTrDdl1001));//ПЕ конфликтующее
	auto norm1001=tpl.norma( tblNrm1001, tblTrDdl1001.tdescr.GetNumber().getNumber() );// нормативка для ПЕ
	adm.addNorma( tblTrDdl1001.tdescr, norm1001 );
	auto norm1003=tpl.norma( tblNrm1003, tblTrDdl1003.tdescr.GetNumber().getNumber() );// нормативка для ПЕ
	adm.addNorma( tblTrDdl1003.tdescr, norm1003 );
	TRACE("\r\n =========== testWaitlock ============");
	adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "В тесте не должно быть взаимных блокировок");
}

void TC_AThread::testConflStat()
{// задержка из-за занятости путей на станции (вынужденная стоянка на пред.станции)
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Strain tblTrCnfl( TrainDescr( L"1005", L"1-100-18" ),Sevent(AEvent::REAL,20,// начало нити
		15,// время на станции
		25,// время на перегоне
		5,// время на последнем перегоне
		1, // нач.станция
		1// число станций
		),Sevent( NO_FIXED ) );
	Strain tblTrCnfl1( TrainDescr( L"1" ),Sevent(AEvent::REAL,10,// начало нити
		15,// время на станции
		25,// время на перегоне
		5,// время на последнем перегоне
		1, // нач.станция
		2// число станций
		),	Sevent( NO_FIXED ) );
	Strain tblTrCnfl3( TrainDescr( L"3" ),Sevent(AEvent::REAL,5,// начало нити
		15,// время на станции
		25,// время на перегоне
		5,// время на последнем перегоне
		1, // нач.станция
		2// число станций
		),Sevent( NO_FIXED ) );
	Sevent tblCnflNrm(AEvent::NONE,30,// начало нити
		50,// время на станции
		100,// время на перегоне
		0,// время на последнем перегоне
		1, // нач.станция
		10);// число станций
	TestAAdmin adm;
	adm.add(tpl.add(tblTrCnfl));//ПЕ конфликтующее
	adm.add(tpl.add(tblTrCnfl1));//ПЕ конфликтующее
	adm.add(tpl.add(tblTrCnfl3));//ПЕ конфликтующее
	auto norm=tpl.norma( tblCnflNrm, tblTrCnfl1.tdescr.GetNumber().getNumber() );// нормативка для ПЕ
	adm.addNorma( tblTrCnfl1.tdescr, norm );
	// нормативка для ПЕ
	adm.addNorma(tblTrCnfl3.tdescr, tpl.norma( tblCnflNrm, tblTrCnfl3.tdescr.GetNumber().getNumber()));
	/* подготовка - очищаем  нити, выделяем фикс прогноз, определяем маршрут и по нему сонаправленность нитей
	 возвращает упорядоченный список нитей поездов для первого шага прогноза (дотягивание с перегона до станции) */
	TRACE("\r\n =========== testConflStat ============");
	adm.forecastGlobal();
	tstTRACE(*adm.getNsi());
	tstTRACEtrn(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && " Исключений не должно быть");
	for (const auto& t: adm.getTrn())
	{
		AEvent prev;
		for (const auto& ev:t->frc )
		{
			if(!prev.empty())
			{
				CPPUNIT_ASSERT((prev.onSpan()^ev.onSpan()) && "В прогнозе должны чередоваться события на станции и на перегоне");
				CPPUNIT_ASSERT(prev.diap.getEnd()==ev.diap.getOrig() && "В прогнозе разрыв времени событий");
				CPPUNIT_ASSERT(((prev.onSpan() && prev.Esr().occured(ev.Esr().getTerm()))|| (ev.onSpan() && ev.Esr().occured(prev.Esr().getTerm())))
					&& "Последовательность станций в прогнозе нарушена");
			}
			else
				CPPUNIT_ASSERT(ev.diap.getOrig()==t->origExactFrc() && "В  начале прогноза разрыв времени событий");

			prev=ev;
		}
	}

	int cnfl=0;
	auto pvx= getPmt(17,18).pGruz;
	auto  stt=TestAAdmin::getNsi()->stIntPtr->getInterval(StationEsr(17),StationEsr(18));
	auto trn3=adm.fnd(tblTrCnfl3.tdescr);
	auto trn1=adm.fnd(tblTrCnfl1.tdescr);
	auto trn1005=adm.fnd(tblTrCnfl.tdescr);

	for (const auto& nt:norm )
	{
		AEvent ev1=trn1->fndEv(nt.Esr());
		AEvent ev3=trn3->fndEv(nt.Esr());
		AEvent ev=trn1005->fndEv(nt.Esr());
		switch (nt.Esr().getTerm())
		{
		case 11:
			break;
		case 12:
			CPPUNIT_ASSERT(ev.diap.duration()>150);
			break;
		case 13:
			CPPUNIT_ASSERT(ev3.diap.getEnd()==nt.diap.getEnd());
			CPPUNIT_ASSERT(ev3.diap.getOrig()+100<nt.diap.getOrig());
			CPPUNIT_ASSERT(abs(ev3.diap.getEnd()+stt.Iot.count()-ev1.diap.getEnd())< AAdmin::GapStop());
			CPPUNIT_ASSERT(abs(ev3.diap.getOrig()+stt.Ipr.count()-ev1.diap.getOrig())< AAdmin::GapStop());
			CPPUNIT_ASSERT(ev.diap.getOrig()>ev3.diap.getEnd());
			CPPUNIT_ASSERT(abs(ev.diap.duration()-60)<=(int) AAdmin::GapStop());
			break;
		case  19:
			CPPUNIT_ASSERT(ev.diap.duration()>20);
			CPPUNIT_ASSERT(ev.diap.getOrig()>ev3.diap.getEnd());
// 			CPPUNIT_ASSERT(ev.diap.getOrig()<ev1.diap.getEnd());
			break;
		case 20:
			CPPUNIT_ASSERT(ev.getType()==AEvent::MOVE);
		break;
		default:
			CPPUNIT_ASSERT(ev3.diap==nt.diap);
			CPPUNIT_ASSERT(ev1.diap.duration()==nt.diap.duration());
			CPPUNIT_ASSERT(abs(ev3.diap.getEnd()+stt.Iot.count()-ev1.diap.getEnd())<= AAdmin::GapStop());
			CPPUNIT_ASSERT(abs(ev3.diap.getOrig()+stt.Ipr.count()-ev1.diap.getOrig())<= AAdmin::GapStop());
			CPPUNIT_ASSERT(ev.diap.getOrig()>ev3.diap.getEnd());
			break;
		}
	}
}

void TC_AThread::testOnePath()
{// проезд по однопутке (окно на перегоне)
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	WayInfoCPtr w=tpl.fnd(sd.tWnd);

	RailLimitationCPtr railLimitCPtr = tpl.tech( sd.tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	TestAAdmin adm(p);
	adm.add(tpl.add(sd.tblTrCnfl));//ПЕ конфликтующее
	adm.add(tpl.add(sd.tblTrCnfl2));//ПЕ
	TRACE("\r\n =========== testOnePath ============");
	EsrKit eWnd;
	for(unsigned int i=startStatCode;i<=finishStatCode;i++) // перегоны +станции
	{
		EsrKit span(i,i+1);
		auto ws= adm.getNsi()->spansRegPtr->getWays(span);
		for ( auto& pt:ws )
		{
			BUSYLIM bl=AAdmin::intersectsLim(pt,ADiap(),0);
			CPPUNIT_ASSERT(bl.size()==(pt==w?1:0) && "недопустимое занятие объекта");
			if(pt==w)
				eWnd=span;
		}
	}
	adm.forecastGlobal();
	tstTRACE(adm);
	for (const auto& t: adm.getTrn())
	{// вывод результатов расчета в трассу
	 	V_EVNT ideal=t->buildIdeal().first;
		long esrW=t->passRoute(eWnd,true);
 		for (const auto& evt:t->frc )
			if(evt.onSpan())
				for (const auto& evd:ideal )
					if(evt.Esr()==evd.Esr())
					{
						if(evt.Esr().occured(StationEsr(esrW)) )// станция по маршруту "+" вперед "-" назад(evt.Esr().is(eWnd.first)|| evt.Esr().is(eWnd.second))
							CPPUNIT_ASSERT(evd.diap.duration()<evt.diap.duration() && "Нет учета разгона/торможения при окне");
						else
							CPPUNIT_ASSERT(evd.diap.duration()==evt.diap.duration() && "Лишний разгон/торможения без окна");
					}
	}
}

void TC_AThread::testWrongPath()
{// проезд по неправильному пути
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	//----- установка окна ------------
	WayInfoCPtr w=tpl.fnd(sd.tWnd);
	RailLimitationCPtr tt = tpl.tech( sd.tWnd );
	std::vector<RailLimitationCPtr> vR(1,tt);
	p.setLimitation( w, vR );
	//--------------------------------------------
	TestAAdmin adm(p);
	adm.StopBefAftLockPath(0, SpanLockType::BIDIR_AUTOBLOCK,TestAAdmin::Before,true);  /* стоянка перед выходом на непр.путь*/
	adm.add(tpl.add(sd.tblTrCnfl));//ПЕ конфликтующее
	adm.add(tpl.add(sd.tblTrCnfl2));//ПЕ
	TRACE("\r\n =========== testWrongPath ============");
	adm.forecastGlobal();
	tstTRACE(adm);
	for (const auto& t: adm.getTrn())
		for (unsigned int i=0;i<t->frc.size();i++ )
		{
			AEvent ev=t->frc[i];
			const AEvent* evPtrId=t->ideal.fnd(ev.Esr(),AIdeal::CURR);
			AEvent evId;
			if(evPtrId)
				evId=*evPtrId;
			if(ev.onSpan())
			{
				auto sV=adm.getNsi()->spansRegPtr->getWays(ev.Esr());
				if(ev.Num()!=(t->getNum()&1?1:2) && sV.size()>1)//" НЕПРАВ ПУТЬ ";
				{
					CPPUNIT_ASSERT(EsrKit(sd.tWnd.e1,sd.tWnd.e2)==ev.Esr() && "Лишний проезд по неправильному пути");
					CPPUNIT_ASSERT(ev.Wrongway() && "Признак проезда по неправильному пути должен быть установлен.");

					AEvent prEv=t->frc[i-1];
					CPPUNIT_ASSERT(prEv.diap.duration()>=AAdmin::StopBefAftLockPath(SpanLockType::BIDIR_AUTOBLOCK, true,false)
						&& "Невыдержка времени перед  отправлением по непр.пути");
					STT_Cat_Val pmt=t->getPMT(ev);
					CPPUNIT_ASSERT(ev.diap.duration()==pmt.full && "Время прохождения по непр.пути должно сохраняться");
				}
				else
					CPPUNIT_ASSERT(!ev.Wrongway() && "Признака проезда по неправильному пути не должно быть.");
			}
			else
				CPPUNIT_ASSERT(!ev.Wrongway() && "На станции не может быть движения по неправ.пути.");
		}
}

void TC_AThread::testFrstStep()
{// первый шаг - дотягивание до станции с перегона, выдержка стоянки по нормативке на станции
	TC_AtopologyParams tplParams( stationsCount, 3, 2, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	TestAAdmin adm;
	adm.add(tpl.add(sd.tblTr1));
	adm.add(tpl.add(sd.tblTr2));
	adm.add(tpl.add(sd.tblTr3));
	adm.add(tpl.add(sd.tblTr4));
	adm.add(tpl.add(sd.tblTr5));
	adm.add(tpl.add(sd.tblTr6));

	adm.add(tpl.add(sd.tblTrDdl1003));//ПЕ стоит на станции по нормативке
	auto norm1003=tpl.norma( sd.tblNrm1003, sd.tblTrDdl1003.tdescr.GetNumber().getNumber() );// нормативка для ПЕ
	adm.addNorma( sd.tblTrDdl1003.tdescr, norm1003 );

	// выделение  нитей для первого шага прогноза(дотягивание с перегона до станции)
	V_TRN_PTR vTr= adm.prepareTrains();
	TRACE("\r\n ==========  testFrstStep ==========");
	V_EVNT vEnd;
	for (const auto& t: vTr)
	{ 
		V_EVNT vEv=	t->prepare();
		AEvent ev=vEv.back();
		size_t  n= std::count_if(vEnd.begin(),vEnd.end(),[&ev](AEvent& t)
		{return ev.equObj( t );});
		if(!vEnd.empty())
		{
			AEvent& evB=vEnd.back();
			CPPUNIT_ASSERT( ((ev.equObj( evB ) &&  ev.diap.getOrig()>evB.diap.getOrig())
				|| !ev.equObj( evB ) )
				&& " Нарушен порядок нитей для первого шага прогноза ");
			CPPUNIT_ASSERT( ( (!ev.equObj(vEnd.back()) && !n )|| ev.equObj( vEnd.back() )) 
				&& " Порядок объекта для первого шага прогноза");
		}
		vEnd.push_back(ev);
	}
	for (const auto& t: vTr)
		t->firstStepForecast();
	tstTRACE(adm);
	AEvent prevOdd,prevEven ;
	for (const auto& t: vTr)
	{
		AEvent& prev=t->getNum()%2? prevOdd:prevEven ;
		AEvent& bck=t->real->back();
		AEvent& itEv=t->real->at(t->real->size()-2);
		CPPUNIT_ASSERT((itEv.onSpan() || (!itEv.onSpan() && t->nrm )) && bck.getSource()==AEvent::FRC &&  "Недостроен первый шаг прогноза");
		if(prev.Esr()==itEv.Esr())
			CPPUNIT_ASSERT(prev.diap.getOrig()<itEv.diap.getOrig() && prev.diap.getEnd()<bck.diap.getEnd() &&
			"Порядок нарушен после первого шага прогноза");
		prev=itEv;
		prev.diap.set(itEv.diap.getOrig(),bck.diap.getEnd());
	}
}

void TC_AThread::testNormWnd()
{// обход окна при движении по нормативке
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	st_intervals_t defTimes;
	Sevent sEv2(//реальная нить
		AEvent::REAL,// тип событий
		5,// начало нити
		2, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		8, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent sEv4(//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		62, // время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		9, // нач.станция (+10 в конструкторе)
		1// число станций
		);
	Sevent tNrm2(AEvent::NONE,// тип событий
		20,// начало нити
		0,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		9, // нач.станция
		7// число станций
		);
	Sevent tNrm4(AEvent::NONE,// тип событий
		20,// начало нити
		20,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		9, // нач.станция
		7// число станций
		);
	auto norm2=tpl.norma(tNrm2,2);
	auto norm4=tpl.norma(tNrm4,4);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv4, Sevent( NO_FIXED ));
	Stech tWnd( 10, 9989 /*длительность*/, 1, 1/*park*/, 6/*e1*/, 5/*e2*/, nullptr/*limit*/ );

	TRACE("\r\n =========== testNormWnd ============");
	TestPossessions p( TestAAdmin::getNsi() );
	WayInfoCPtr w=tpl.fnd(tWnd);

	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	TestAAdmin adm(p);
	adm.add(tpl.add(trn2,2));
//	adm.add(tpl.add(trn4,1));
	adm.addNorma( trn2.tdescr, norm2 );
//	adm.addNorma( trn4.tdescr, norm4 );
	EsrKit eWnd;
	for(unsigned int i=startStatCode;i<=finishStatCode;i++) // перегоны +станции
	{
		EsrKit span(i,i+1);
		auto ws= adm.getNsi()->spansRegPtr->getWays(span);
		for ( auto& pt:ws )
		{
			BUSYLIM bl=AAdmin::intersectsLim(pt,ADiap(),0);
			CPPUNIT_ASSERT(bl.size()==(pt==w?1:0) && "недопустимое занятие объекта");
			if(pt==w)
				eWnd=span;
		}
	}
	adm.forecastGlobal();
	tstTRACE(*adm.getNsi());
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть");
	for (const auto& t: adm.getTrn())
	{// вывод результатов расчета в трассу
		V_EVNT ideal=t->buildIdeal().first;
		long esrW=t->passRoute(eWnd,true);
		bool isW=false;
		for (const auto& evt:t->frc )
		{
			if(evt.onSpan())
				for (const auto& evd:ideal )
					if(evt.Esr()==evd.Esr())
					{
						if(evt.Esr().occured(StationEsr(esrW)) )// станция по маршруту "+" вперед "-" назад(evt.Esr().is(eWnd.first)|| evt.Esr().is(eWnd.second))
							CPPUNIT_ASSERT(evd.diap.duration()<evt.diap.duration() && "Нет учета разгона/торможения при окне");
						else
							if(isW)
								CPPUNIT_ASSERT(evd.diap.duration()>evt.diap.duration() && "ускорение для нагона нормативки");
							else
								CPPUNIT_ASSERT(evd.diap.duration()==evt.diap.duration() && "идем  по нормативке");
					}
			isW|=evt.Esr().occured(StationEsr(esrW));
		}
	}
}

void TC_AThread::testFirstStepWrongWay()
{//тест первый шаг прогноза по неправ.пути
	TRACE("\r\n =========== testFirstStepWrongWay ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // однопутка, 3 пути на станции
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	st_intervals_t defTimes;
	Sevent sEv2(//реальная нить
		AEvent::REAL,// тип событий
		5,// начало нити
		10, // время на станции
		20,// время на перегоне
		5,// время на последнем перегоне
		2, // нач.станция (+10 в конструкторе)
		2// число станций
		);
	Sevent sEv1001(//реальная нить
		AEvent::REAL,// тип событий
		20,// начало нити
		12, // время на станции
		25,// время на перегоне
		18,// время на последнем перегоне
		4, // нач.станция (+10 в конструкторе)
		2// число станций
		);
	Sevent tNrm2(AEvent::NONE,// тип событий
		10,// начало нити
		10,// время на станции
		20,// время на перегоне
		0,// время на последнем перегоне
		3, // нач.станция
		7// число станций
		);	auto norm2=tpl.norma(tNrm2,2);
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));


	TestAAdmin adm;
	adm.add(tpl.add(trn1001));//ПЕ реальное
	adm.add(tpl.add(trn2));//ПЕ реальное
	adm.addNorma(TrainDescr( L"2", L""), norm2 );
	{
		auto t=adm.fnd(trn2.tdescr);
		t->real->back().Wrongway(true);
		V_EVNT::reverse_iterator it=t->real->rbegin()+1;
		CPPUNIT_ASSERT(t->real->back().onSpan() && it->Esr()==t->real->back().Esr() && (t->real->back().Wrongway()^it->Wrongway()));
		t=adm.fnd(trn1001.tdescr);
		it=t->real->rbegin()+1;
		it->Wrongway(true);
		CPPUNIT_ASSERT(t->real->back().onSpan() && it->Esr()==t->real->back().Esr() && (t->real->back().Wrongway()^it->Wrongway()));
	}
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть");
	CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2 && "Прогноз по обеим ПЕ");
	for (const auto& t: trFrc.getSpotLines())
	{
		CPPUNIT_ASSERT((t.first.GetNumber().getNumberString()==L"2" && t.second.size()==2 ) || (t.first.GetNumber().getNumberString()==L"1001" && t.second.size()==10 ));
		CPPUNIT_ASSERT( t.second.front().onSpan() && t.second.front().Wrongway() && "Прогноз на перегоне по неправ.пути" );
		auto ptrn=adm.fnd(t.first);
		long esr=t.second.front().Esr().getSpan().first==t.second[1].Esr().getTerm()?t.second.front().Esr().getSpan().second
		 	:t.second.front().Esr().getSpan().first;
		auto evt=ptrn->fndEv(EsrKit(esr));
		auto evtN=ptrn->fndEv(EsrKit(ptrn->fndStation(esr,1)));
		ADiap d(evt.diap.getEnd(),evtN.diap.getOrig());
		STT_Cat_Val s=ptrn->getPMT(t.second.front());// ПВХ для перегона/ перегона к станции 
		CPPUNIT_ASSERT((d.duration()==(ptrn->getNorm()?0:s.start)+(1+PVX_WRONG_PATH)*s.full) && "Расчет по неправильному пути");
	}
}
