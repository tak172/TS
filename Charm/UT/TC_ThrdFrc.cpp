/*!
	created:	2017/01/26
	created: 	18:36  26 ������ 2017
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
		TRACE("\r\n ---- TRACE ��� ----");
		std::vector <SpanTravelTimeInfo> vS = sp->GetSpans();
		for (const auto& t: vS)
		{
			TRACE("\r\n    ==  %s ==" ,t.esr_string().c_str());
			STT_Val sv = sp->GetSpanTime(t);
            if ( sv.pGruz )
                TRACE("\r\n ��������     start=%d, stop=%d,reserv=%d,full=%d ", sv.pGruz->start, sv.pGruz->stop, sv.pGruz->reserv, sv.pGruz->full);
            if ( sv.pPass )
                TRACE("\r\n ������������ start=%d, stop=%d,reserv=%d,full=%d ", sv.pPass->start, sv.pPass->stop, sv.pPass->reserv, sv.pPass->full);
			if( !full )
				break;
		}
	}
	auto cl= bs.stIntPtr;
	if (cl)
	{
		TRACE("\r\n ---- TRACE �����.��������� ----");
		std::vector<long>st=cl->getStations();
		for (const auto& t: st)
		{
			st_intervals_t s1=cl->getInterval(StationEsr(t),StationEsr(t+1));
			st_intervals_t s2=cl->getInterval(StationEsr(t),StationEsr(t-1));
			{
				TRACE("\r\n    == ������� %d ==" ,t);
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
	for(int n=10;n<(full?esr:11);n++) // �������� +�������
	{
		StationEsr esr1(n); // �������� +�������
		StationEsr esr2(n+1); // �������� +�������
		st_intervals_t t1=nsiPtr->stIntPtr->getInterval(esr1,esr2);
		st_intervals_t t2=nsiPtr->stIntPtr->getInterval(esr2,esr1);
		std::stringstream ss;
		ss<<"\r\n ----Intervals >>PACK="<<t1.tp.count()  <<" PACK="<<t2.tp.count();
		TRACE(ss.str().c_str());
	}
	for (const auto& t:adm.getNorm())// ��� ����������
	{
		TRACE("\r\n ---- TRACE Norm %d ----",t.first);
		AEvent prev;
		for (const auto& v: t.second)
			for (const auto& ev:v )
			{
				std::stringstream ss;
				ss<<"\r\n esr="<<ev.Esr().getTerm();
				if(!prev.empty() && prev.onSpan())
					ss<<"  �� �������� "<< ev.diap.getOrig()-prev.diap.getEnd();
				ss<<"  �� ������� "<<ev.diap.getOrig() <<"-"<<ev.diap.getEnd() <<" {"<<ev.diap.duration() <<"} ";
				TRACE(ss.str().c_str());
				prev=ev;
			}
	}
	TRACE("\r\n ---- TRACE tech limit ----");
	for(int n=10;n<esr+10;n++) // ��������
	{
		long esr1=n; // �������� 
		long esr2=esr1+1; // �������� 
		auto pthSt=nsiPtr->stationsRegPtr->getWays(EsrKit(esr1));
		auto pthSp=nsiPtr->spansRegPtr->getWays(EsrKit(esr1,esr2));
		for ( auto& pt:pthSp)
		{
			BUSYLIM bl=AAdmin::intersectsLim(pt,ADiap(),0);
			for (auto& tsb:bl )
			{
				const RailLimitation* tch=tsb.get();
				std::stringstream ss;
				ss<<"\r\n �������="<<esr1<<"-"<<esr2<<" ���� "<<pt->Num()<<" ����������� "<<tch->to_string()
					<<"  ����� "<<tch->getDiap().getOrig()<<"-"<<tch->getDiap().getEnd();
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
				ss<<"\r\n �������="<<esr1<<"���� "<<pt->Num()<<" ����������� "<<tch->to_string()
					<<"  ����� "<<tch->getDiap().getOrig()<<"-"<<tch->getDiap().getEnd();
				TRACE(ss.str().c_str());
			}
		}
	}
	tstTRACEtrn(adm);
}

void TC_AThread::tstTRACEtrn(const TestAAdmin& adm)
{
	std::stringstream r;
	r<<std::endl<<" ---- TRACE all trains ----"<<std::endl<<" ��������� ����� ����� �������� ="<<adm.numStepFrc.get()+1
		<<std::endl<<" ��������� ���������� �� ���������� �������� ="<<std::endl<< adm.messDeviatFromIdeal();
	TRACE(r.str().c_str());
	for (const auto& t: adm.getTrn())
	{
		TRACE("\r\n ---- train N %d (���� %d) ----",t->getNum(),t->descr().GetIndex().destination().getTerm());
			//ALogWrn::convWstring(t->getTrainDescr().GetIndex().str()).c_str());
		TRACE("\r\n ....   real   ....");
		for (const auto& ev:*(t->real.get()))
			if(ev.getSource()==AEvent::REAL)
			{
			std::stringstream ss;
			ss<<std::endl<<ALogWrn::convWstring(ev.text(nullptr))<<" {"<<ev.diap.getOrig() <<"-"<<ev.diap.getEnd() <<" = "<<ev.diap.duration() <<"} ";
			TRACE(ss.str().c_str());
			}
		TRACE("\r\n ....   �������   ....\n");
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
					errPath= ev.Num()==t->needPath(ev.Esr()) || s.size()<2 ?"":" ������ ���� ";
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
						ss/*<<std::endl*/<<"esr="<< ev.Esr().getTerm()<<"  ���������� �� ����. "<<dis <<" �� �������� "<<
							errPath.c_str()<<" "<<ev.diap.getOrig()-prev.diap.getEnd() <<" {"<<evId.diap.getOrig()-prevId <<"} �� ������� "<<
							ev.diap.duration()<<" {"<<evId.diap.duration() <<"} ("<<ev.diap.getOrig() <<"-"<<ev.diap.getEnd() <<
							" {"<<evId.diap.getOrig() <<"-"<<evId.diap.getEnd() <<"}) "<<"/ ����="<<ev.Num()<<"/";
					}
					else
						ss/*<<std::endl*/<<"esr="<< ev.Esr().getTerm()<<"  �� �������� "<<errPath.c_str()<<" "<<ev.diap.getOrig()-prev.diap.getEnd() <<
						" {"<<evId.diap.getOrig()-prevId <<"} �� ������� "<<ev.diap.duration() <<" {"<<evId.diap.duration() <<"} ("<<ev.diap.getOrig()
						<<"-"<<ev.diap.getEnd() <<" {"<<evId.diap.getOrig() <<"-"<<evId.diap.getEnd() <<"}) "<<"/ ����="<<ev.Num()<<"/";
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
					ss/*<<std::endl*/<<"����� esr="<<ev.Esr().getTerm() <<"  �� �������� "<<ev.diap.getOrig()-prev.diap.getEnd() <<" �� �������  "<<ev.diap.getOrig() <<"-"
						<<ev.diap.getEnd() <<" ("<<ev.diap.duration() <<") "<<std::endl;
					TRACE(ss.str().c_str());
					prev=ev;
				}
	}
}

void TC_AThread::testCatchNorm()
{// �������� ����������
	TRACE("\r\n =========== testCatchNorm ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 2, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	TestAAdmin adm;
	ATrainPtr atrainPtr = tpl.add(sd.tblTrCtchNrm); //��
	adm.add( atrainPtr );

	auto norm=tpl.norma( sd.tblCtchNrm, sd.tblTrCtchNrm.tdescr.GetNumber().getNumber() );// ���������� ��� ��
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
			CPPUNIT_ASSERT((dis>dFp ||(!dis && !dFp))  && " ���������� ���������� �� ����������");
			dFp=dis;
		}
	}
	ADiap dN(norm[0].diap.getEnd(),norm[1].diap.getOrig());
	AEvent ev=frc.front();
	CPPUNIT_ASSERT(ev.onSpan() && ev.getSource()==AEvent::FRC && ev.diap.duration()<dN.duration() && "������ ���������� ���� �� ��������");
	AEvent evr= trn->fndEv(ev.Esr());
	CPPUNIT_ASSERT(ev.Esr()==evr.Esr() && evr.diap.getOrig()<ev.diap.getOrig() && evr.diap.duration()<dN.duration() && "����� �������� ���� �� ��������");
	AEvent ev1=trn->fndEv(EsrKit(ev.Esr().getSpan().first));// ����� ������� �� �����.����� 
	AEvent ev2=trn->fndEv(EsrKit(ev.Esr().getSpan().second));// ����� ������� �� �����.����� 
	ADiap d(ev1.diap.getEnd(),ev2.diap.getOrig());// ����� �� �������� �� ������ ���� (�������� +�������)
	CPPUNIT_ASSERT( (1.-SPEEDUP_NORM)*dN.duration()==d.duration() && " �������� ���������� � �� ������ ���� ��������");
}

void TC_AThread::testDeadlock()
{// ���� ������
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Strain tblTrDdl2( TrainDescr( L"2", L"18-100-1" ),
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		10,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		11, // ���.�������
		1// ����� �������
		),
		Sevent( NO_FIXED )) ;

	Strain tblTrDdl4( TrainDescr( L"4", L"18-100-1" ),
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		10,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		11, // ���.�������
		1// ����� �������
		),	Sevent( NO_FIXED )) ;
	 Strain tblTrDdl1003( TrainDescr( L"1003", L"1-100-18" ),
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		115,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		10, // ���.�������
		1// ����� �������
		),	Sevent( NO_FIXED ));
	  Strain tblTrDdl1001(  TrainDescr( L"1001", L"1-100-18" ),
		 Sevent(
		 //�������� ����
		 AEvent::REAL,// ��� �������
		 20,// ������ ����
		 115,// ����� �� �������
		 25,// ����� �� ��������
		 0,// ����� �� ��������� ��������
		 10, // ���.�������
		 1// ����� �������
		 ), Sevent( NO_FIXED ) ) ;

	TestAAdmin adm;
	ACounterInt::limit=1; 

	adm.add(tpl.add(tblTrDdl2));//�� �������������
	adm.add(tpl.add(tblTrDdl4));//�� �������������
	adm.add(tpl.add(tblTrDdl1003));//�� �������������
	adm.add(tpl.add(tblTrDdl1001));//�� �������������
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
	CPPUNIT_ASSERT(vCl.size()==2 && "� ����� ����  �������� ����������");
	auto n=std::count_if(vCl.begin(),vCl.end(),[](const P_CLSH&t){return t->Comment()==INVALID_ACT_OBJ;});
	CPPUNIT_ASSERT(n==2 && "�� ������� 2 ���� � ��� �� � ����� ������� � 2 � ������");
}

void TC_AThread::testWaitlock()
{//  �������� ������� ����� �� �������
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Strain tblTrDdl2( TrainDescr( L"2", L"18-100-1" ),
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		10,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		11, // ���.�������
		1// ����� �������
		),
		Sevent( NO_FIXED )) ;
	Strain tblTrDdl4( TrainDescr( L"4", L"18-100-1" ),
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		10,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		11, // ���.�������
		1// ����� �������
		),	Sevent( NO_FIXED )) ;
	Strain tblTrDdl1003( TrainDescr( L"6003", L"1-100-18" ),
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		115,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		10, // ���.�������
		1// ����� �������
		),	Sevent( NO_FIXED ));
	Strain tblTrDdl1001(  TrainDescr( L"6001", L"1-100-18" ),
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		115,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		10, // ���.�������
		1// ����� �������
		), Sevent( NO_FIXED ) ) ;
	Sevent tblNrm1001( 	AEvent::NONE,// ��� �������
		30,// ������ ����
		100,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		10, // ���.�������
		3// ����� �������
		) ;
	Sevent tblNrm1003( 	AEvent::NONE,// ��� �������
		60,// ������ ����
		100,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		10, // ���.�������
		3// ����� �������
		);
	TestAAdmin adm;
	ACounterInt::limit=10; 
	adm.add(tpl.add(tblTrDdl2,1));//�� �������������
	adm.add(tpl.add(tblTrDdl4));//�� �������������
	adm.add(tpl.add(tblTrDdl1003,2));//�� �������������
	adm.add(tpl.add(tblTrDdl1001));//�� �������������
	auto norm1001=tpl.norma( tblNrm1001, tblTrDdl1001.tdescr.GetNumber().getNumber() );// ���������� ��� ��
	adm.addNorma( tblTrDdl1001.tdescr, norm1001 );
	auto norm1003=tpl.norma( tblNrm1003, tblTrDdl1003.tdescr.GetNumber().getNumber() );// ���������� ��� ��
	adm.addNorma( tblTrDdl1003.tdescr, norm1003 );
	TRACE("\r\n =========== testWaitlock ============");
	adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "� ����� �� ������ ���� �������� ����������");
}

void TC_AThread::testConflStat()
{// �������� ��-�� ��������� ����� �� ������� (����������� ������� �� ����.�������)
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Strain tblTrCnfl( TrainDescr( L"1005", L"1-100-18" ),Sevent(AEvent::REAL,20,// ������ ����
		15,// ����� �� �������
		25,// ����� �� ��������
		5,// ����� �� ��������� ��������
		1, // ���.�������
		1// ����� �������
		),Sevent( NO_FIXED ) );
	Strain tblTrCnfl1( TrainDescr( L"1" ),Sevent(AEvent::REAL,10,// ������ ����
		15,// ����� �� �������
		25,// ����� �� ��������
		5,// ����� �� ��������� ��������
		1, // ���.�������
		2// ����� �������
		),	Sevent( NO_FIXED ) );
	Strain tblTrCnfl3( TrainDescr( L"3" ),Sevent(AEvent::REAL,5,// ������ ����
		15,// ����� �� �������
		25,// ����� �� ��������
		5,// ����� �� ��������� ��������
		1, // ���.�������
		2// ����� �������
		),Sevent( NO_FIXED ) );
	Sevent tblCnflNrm(AEvent::NONE,30,// ������ ����
		50,// ����� �� �������
		100,// ����� �� ��������
		0,// ����� �� ��������� ��������
		1, // ���.�������
		10);// ����� �������
	TestAAdmin adm;
	adm.add(tpl.add(tblTrCnfl));//�� �������������
	adm.add(tpl.add(tblTrCnfl1));//�� �������������
	adm.add(tpl.add(tblTrCnfl3));//�� �������������
	auto norm=tpl.norma( tblCnflNrm, tblTrCnfl1.tdescr.GetNumber().getNumber() );// ���������� ��� ��
	adm.addNorma( tblTrCnfl1.tdescr, norm );
	// ���������� ��� ��
	adm.addNorma(tblTrCnfl3.tdescr, tpl.norma( tblCnflNrm, tblTrCnfl3.tdescr.GetNumber().getNumber()));
	/* ���������� - �������  ����, �������� ���� �������, ���������� ������� � �� ���� ���������������� �����
	 ���������� ������������� ������ ����� ������� ��� ������� ���� �������� (����������� � �������� �� �������) */
	TRACE("\r\n =========== testConflStat ============");
	adm.forecastGlobal();
	tstTRACE(*adm.getNsi());
	tstTRACEtrn(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && " ���������� �� ������ ����");
	for (const auto& t: adm.getTrn())
	{
		AEvent prev;
		for (const auto& ev:t->frc )
		{
			if(!prev.empty())
			{
				CPPUNIT_ASSERT((prev.onSpan()^ev.onSpan()) && "� �������� ������ ������������ ������� �� ������� � �� ��������");
				CPPUNIT_ASSERT(prev.diap.getEnd()==ev.diap.getOrig() && "� �������� ������ ������� �������");
				CPPUNIT_ASSERT(((prev.onSpan() && prev.Esr().occured(ev.Esr().getTerm()))|| (ev.onSpan() && ev.Esr().occured(prev.Esr().getTerm())))
					&& "������������������ ������� � �������� ��������");
			}
			else
				CPPUNIT_ASSERT(ev.diap.getOrig()==t->origExactFrc() && "�  ������ �������� ������ ������� �������");

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
{// ������ �� ��������� (���� �� ��������)
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	WayInfoCPtr w=tpl.fnd(sd.tWnd);

	RailLimitationCPtr railLimitCPtr = tpl.tech( sd.tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	TestAAdmin adm(p);
	adm.add(tpl.add(sd.tblTrCnfl));//�� �������������
	adm.add(tpl.add(sd.tblTrCnfl2));//��
	TRACE("\r\n =========== testOnePath ============");
	EsrKit eWnd;
	for(unsigned int i=startStatCode;i<=finishStatCode;i++) // �������� +�������
	{
		EsrKit span(i,i+1);
		auto ws= adm.getNsi()->spansRegPtr->getWays(span);
		for ( auto& pt:ws )
		{
			BUSYLIM bl=AAdmin::intersectsLim(pt,ADiap(),0);
			CPPUNIT_ASSERT(bl.size()==(pt==w?1:0) && "������������ ������� �������");
			if(pt==w)
				eWnd=span;
		}
	}
	adm.forecastGlobal();
	tstTRACE(adm);
	for (const auto& t: adm.getTrn())
	{// ����� ����������� ������� � ������
	 	V_EVNT ideal=t->buildIdeal().first;
		long esrW=t->passRoute(eWnd,true);
 		for (const auto& evt:t->frc )
			if(evt.onSpan())
				for (const auto& evd:ideal )
					if(evt.Esr()==evd.Esr())
					{
						if(evt.Esr().occured(StationEsr(esrW)) )// ������� �� �������� "+" ������ "-" �����(evt.Esr().is(eWnd.first)|| evt.Esr().is(eWnd.second))
							CPPUNIT_ASSERT(evd.diap.duration()<evt.diap.duration() && "��� ����� �������/���������� ��� ����");
						else
							CPPUNIT_ASSERT(evd.diap.duration()==evt.diap.duration() && "������ ������/���������� ��� ����");
					}
	}
}

void TC_AThread::testWrongPath()
{// ������ �� ������������� ����
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	TestPossessions p( TestAAdmin::getNsi() );
	//----- ��������� ���� ------------
	WayInfoCPtr w=tpl.fnd(sd.tWnd);
	RailLimitationCPtr tt = tpl.tech( sd.tWnd );
	std::vector<RailLimitationCPtr> vR(1,tt);
	p.setLimitation( w, vR );
	//--------------------------------------------
	TestAAdmin adm(p);
	adm.StopBefAftLockPath(0, SpanLockType::BIDIR_AUTOBLOCK,TestAAdmin::Before,true);  /* ������� ����� ������� �� ����.����*/
	adm.add(tpl.add(sd.tblTrCnfl));//�� �������������
	adm.add(tpl.add(sd.tblTrCnfl2));//��
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
				if(ev.Num()!=(t->getNum()&1?1:2) && sV.size()>1)//" ������ ���� ";
				{
					CPPUNIT_ASSERT(EsrKit(sd.tWnd.e1,sd.tWnd.e2)==ev.Esr() && "������ ������ �� ������������� ����");
					CPPUNIT_ASSERT(ev.Wrongway() && "������� ������� �� ������������� ���� ������ ���� ����������.");

					AEvent prEv=t->frc[i-1];
					CPPUNIT_ASSERT(prEv.diap.duration()>=AAdmin::StopBefAftLockPath(SpanLockType::BIDIR_AUTOBLOCK, true,false)
						&& "���������� ������� �����  ������������ �� ����.����");
					STT_Cat_Val pmt=t->getPMT(ev);
					CPPUNIT_ASSERT(ev.diap.duration()==pmt.full && "����� ����������� �� ����.���� ������ �����������");
				}
				else
					CPPUNIT_ASSERT(!ev.Wrongway() && "�������� ������� �� ������������� ���� �� ������ ����.");
			}
			else
				CPPUNIT_ASSERT(!ev.Wrongway() && "�� ������� �� ����� ���� �������� �� ������.����.");
		}
}

void TC_AThread::testFrstStep()
{// ������ ��� - ����������� �� ������� � ��������, �������� ������� �� ���������� �� �������
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

	adm.add(tpl.add(sd.tblTrDdl1003));//�� ����� �� ������� �� ����������
	auto norm1003=tpl.norma( sd.tblNrm1003, sd.tblTrDdl1003.tdescr.GetNumber().getNumber() );// ���������� ��� ��
	adm.addNorma( sd.tblTrDdl1003.tdescr, norm1003 );

	// ���������  ����� ��� ������� ���� ��������(����������� � �������� �� �������)
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
				&& " ������� ������� ����� ��� ������� ���� �������� ");
			CPPUNIT_ASSERT( ( (!ev.equObj(vEnd.back()) && !n )|| ev.equObj( vEnd.back() )) 
				&& " ������� ������� ��� ������� ���� ��������");
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
		CPPUNIT_ASSERT((itEv.onSpan() || (!itEv.onSpan() && t->nrm )) && bck.getSource()==AEvent::FRC &&  "���������� ������ ��� ��������");
		if(prev.Esr()==itEv.Esr())
			CPPUNIT_ASSERT(prev.diap.getOrig()<itEv.diap.getOrig() && prev.diap.getEnd()<bck.diap.getEnd() &&
			"������� ������� ����� ������� ���� ��������");
		prev=itEv;
		prev.diap.set(itEv.diap.getOrig(),bck.diap.getEnd());
	}
}

void TC_AThread::testNormWnd()
{// ����� ���� ��� �������� �� ����������
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		8, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv4(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		62, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		20,// ������ ����
		0,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.�������
		7// ����� �������
		);
	Sevent tNrm4(AEvent::NONE,// ��� �������
		20,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.�������
		7// ����� �������
		);
	auto norm2=tpl.norma(tNrm2,2);
	auto norm4=tpl.norma(tNrm4,4);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv4, Sevent( NO_FIXED ));
	Stech tWnd( 10, 9989 /*������������*/, 1, 1/*park*/, 6/*e1*/, 5/*e2*/, nullptr/*limit*/ );

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
	for(unsigned int i=startStatCode;i<=finishStatCode;i++) // �������� +�������
	{
		EsrKit span(i,i+1);
		auto ws= adm.getNsi()->spansRegPtr->getWays(span);
		for ( auto& pt:ws )
		{
			BUSYLIM bl=AAdmin::intersectsLim(pt,ADiap(),0);
			CPPUNIT_ASSERT(bl.size()==(pt==w?1:0) && "������������ ������� �������");
			if(pt==w)
				eWnd=span;
		}
	}
	adm.forecastGlobal();
	tstTRACE(*adm.getNsi());
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����");
	for (const auto& t: adm.getTrn())
	{// ����� ����������� ������� � ������
		V_EVNT ideal=t->buildIdeal().first;
		long esrW=t->passRoute(eWnd,true);
		bool isW=false;
		for (const auto& evt:t->frc )
		{
			if(evt.onSpan())
				for (const auto& evd:ideal )
					if(evt.Esr()==evd.Esr())
					{
						if(evt.Esr().occured(StationEsr(esrW)) )// ������� �� �������� "+" ������ "-" �����(evt.Esr().is(eWnd.first)|| evt.Esr().is(eWnd.second))
							CPPUNIT_ASSERT(evd.diap.duration()<evt.diap.duration() && "��� ����� �������/���������� ��� ����");
						else
							if(isW)
								CPPUNIT_ASSERT(evd.diap.duration()>evt.diap.duration() && "��������� ��� ������ ����������");
							else
								CPPUNIT_ASSERT(evd.diap.duration()==evt.diap.duration() && "����  �� ����������");
					}
			isW|=evt.Esr().occured(StationEsr(esrW));
		}
	}
}

void TC_AThread::testFirstStepWrongWay()
{//���� ������ ��� �������� �� ������.����
	TRACE("\r\n =========== testFirstStepWrongWay ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		5,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent sEv1001(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		12, // ����� �� �������
		25,// ����� �� ��������
		18,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		10,// ������ ����
		10,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.�������
		7// ����� �������
		);	auto norm2=tpl.norma(tNrm2,2);
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));


	TestAAdmin adm;
	adm.add(tpl.add(trn1001));//�� ��������
	adm.add(tpl.add(trn2));//�� ��������
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
	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����");
	CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2 && "������� �� ����� ��");
	for (const auto& t: trFrc.getSpotLines())
	{
		CPPUNIT_ASSERT((t.first.GetNumber().getNumberString()==L"2" && t.second.size()==2 ) || (t.first.GetNumber().getNumberString()==L"1001" && t.second.size()==10 ));
		CPPUNIT_ASSERT( t.second.front().onSpan() && t.second.front().Wrongway() && "������� �� �������� �� ������.����" );
		auto ptrn=adm.fnd(t.first);
		long esr=t.second.front().Esr().getSpan().first==t.second[1].Esr().getTerm()?t.second.front().Esr().getSpan().second
		 	:t.second.front().Esr().getSpan().first;
		auto evt=ptrn->fndEv(EsrKit(esr));
		auto evtN=ptrn->fndEv(EsrKit(ptrn->fndStation(esr,1)));
		ADiap d(evt.diap.getEnd(),evtN.diap.getOrig());
		STT_Cat_Val s=ptrn->getPMT(t.second.front());// ��� ��� ��������/ �������� � ������� 
		CPPUNIT_ASSERT((d.duration()==(ptrn->getNorm()?0:s.start)+(1+PVX_WRONG_PATH)*s.full) && "������ �� ������������� ����");
	}
}
