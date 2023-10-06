/*!
	created:	2017/08/07
	created: 	13:49  07 ������ 2017
	filename: 	D:\potapTrunk\Charm\UT\TC_varForecast.cpp
	file path:	D:\potapTrunk\Charm\UT
	file base:	TC_varForecast
	file ext:	cpp
	author:		 Dremin
	
	purpose:	���������� ����������� ������� 
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h"
#include "TC_data.h"

void TC_AThread::testVarForecastCreate()
{// ���� ���������� ����������� ������� 
	TRACE("\r\n   ----  testVarForecastCreate  -----");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)20.0);
	tpl.setPvx(20,&defTimes);
	Sevent tNrm1(AEvent::NONE,// ��� �������
		10,// ������ ����
		10,// ����� �� �������
		10,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		4// ����� �������
		);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		15,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		12, // ���.������� (+10 � ������������)
		4// ����� �������
		);
	int tbl[]={1,3,1001,1003,2,4,1002,1004};// ������������ ����������
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
 	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
 	struct P{
 		int nEsr;
 		int ntr;
 	}tP[]={/* {14,1001},{15,1003},*/{13,1}};// ������ ��� �������� �� ���� �� ������� �� ����������
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
 			CPPUNIT_ASSERT(it!=vOutrun.end() && "��� ���������� ������ ���� ������� � �������" );
 			if(it!=vOutrun.end())
 				nOutr++;
 		}
 	}
 	CPPUNIT_ASSERT(nOutr==vOutrun.size() && "��������� ��� ����������" );
}

void TC_AThread::testVarForecast()
{// ���� ���������� ����������� �������  � ��������� ����
	TRACE("\r\n   ----  testVarForecast  -----");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)1.0);
	tpl.setPvx(20,&defTimes);
	Sevent tNrm1(AEvent::NONE,// ��� �������
		70,// ������ ����
		10,// ����� �� �������
		50,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		3// ����� �������
		);
	Sevent tNrm1002(AEvent::NONE,// ��� �������
		15,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		3// ����� �������
		);
	Sevent sEv1(//�������� ����
		AEvent::REAL,// ��� �������
		70,// ������ ����
		10, // ����� �� �������
		50/*30*/,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		3// ����� �������
		);
	Sevent sEv1002(//�������� ����
		AEvent::REAL,// ��� �������
		82,// ������ ����
		5,// ����� �� �������
		30/*60*/,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		3// ����� �������
		);

	int tbl[]={1,1002};// ������������ ����������
	Stech tWnd( 115, 100 /*������������*/, 1, 1/*park*/, 2/*e1*/,3,nullptr);
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
	CPPUNIT_ASSERT(lns.size()==2 && "������� 2-� �����");
	for (const auto& t: tbl)
	{
		auto it=std::find_if(lns.begin(),lns.end(),[t](const  std::pair<TrainDescr, V_EVNT>& tv){return t==tv.first.GetNumber().getNumber();});
		CPPUNIT_ASSERT(it!=lns.end() && "������ ���� ������� ���� ����");
		auto ev=std::find_if(it->second.begin(),it->second.end(),[&tWnd](const AEvent& et){return et.Esr()==EsrKit(tWnd.e1,tWnd.e2);});
		CPPUNIT_ASSERT(ev!=it->second.end() && "������ ���� �������");
		if(t==1)
			CPPUNIT_ASSERT(ev->Wrongway() && ev->diap.getEnd()>tWnd.or && "�������� ���� �� ����.����");
		else
			CPPUNIT_ASSERT(ev->diap.getEnd()<tWnd.or && "�������� �� ����");
	}
}

 
 
 
