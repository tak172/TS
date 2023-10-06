/*!
	created:	2018/06/04
	created: 	15:45  04 ���� 2018
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrOutrun.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrOutrun
	file ext:	cpp
	author:		 Dremin
	
	purpose:	����� ������� ��
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Augur/ALogWrn.h"
#include "../helpful/TrainsPriorityOffsets.h"
#include "../helpful/TrainsPriorityDefiner.h"
#include "../Augur/AOutrun.h"
#include "../Augur/TxtComment.h"
#include "../Augur/AOutrunTask.h"


struct TOutrun{
	long esr;
	int n1;  // ����������
	int n2;  // ����������
}; 

static 	bool equ(std::shared_ptr<AOutrun>& outr,std::vector<TOutrun>&v )// ���������� �� ������� ������� �� ��������
{
	MTR_F_B mall=outr->get();
	MTR_F_B m;
	std::for_each(mall.begin(),mall.end(),[&m](const PTR_F_B& p){
		VTR_F_B v;
		for (const auto& t: p.second)
			if(t.first)
				v.push_back(t);
		if(!v.empty())
			m[p.first]=v;
	});
	int sz=0,fnd=0;
	for (const auto& t: m)
	{
		sz+=static_cast<int>( t.second.size() );
		for (const auto& tr:t.second )
		{
			for (const auto& tb: v)
				if(tb.esr==t.first && tb.n1==tr.first->getNum() && tb.n2==tr.second->getNum())
					fnd++;
		}
	}
	return sz==v.size() && sz==fnd;
}

static  std::string deviation( TestAAdmin& adm,TrainRoutes<AEvent> result)
{
	std::stringstream r;
	r<<std::endl<<" ��������� ���������� �� ���������� �������� ="<< adm.messDeviatFromIdeal();

	typedef std::pair<std::chrono::seconds ,ATrainPtr> DEV_TRN;
	std::vector<DEV_TRN> devV;
	for (const auto& t: result.getSpotLines())
	{
		ATrainPtr trn=adm.fnd(t.first);
		devV.push_back(make_pair(trn->deviatFromIdeal(),trn));
	}

	if(!devV.empty())
	{
		std::sort(devV.begin(),devV.end(),[](const DEV_TRN&t1,const DEV_TRN&t2 ){return t1.first>t2.first;});
		r<<std::endl<< " ������ ���������� �� ���������� ��������  ��� �������";
		for (const auto& t: devV)
		{
			if(!t.first.count())
				break;
			r<<std::endl<<t.first.count()<<" ��� | "<<ALogWrn::convWstring(t.second->txtDescr());
		}
	}
	return r.str();
}

void testVectClash()
{
	AVectClash tcl;
	const wchar_t* txt=L"111 222";
	AEvent ev1(1);
	ev1.setObj(1,7);
	AEvent ev2=ev1;
	ev2.diap.set(22,33);
	AClash* c1=new AClash(nullptr ,nullptr, &ev1,txt ,PrognosisErrorCode::COMMON);
	AClash* c2=new AClash(nullptr ,nullptr, &ev2,txt ,PrognosisErrorCode::COMMON);
	tcl.add(P_CLSH(c1));
	tcl.add(P_CLSH(c2));
	CPPUNIT_ASSERT(tcl.size()==1);
}

void TC_AThread::testOutrunFltrTask()
{// ���� ������� ��� ������� �� �������
	testVectClash();
	TRACE("\r\n =========== testOutrunFltrTask ============");
	Sevent sEv1(AEvent::REAL ,10/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,75/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		7/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn2( TrainDescr( L"6002", L""),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"6001", L""),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998

	Strain trn2p( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1p( TrainDescr( L"1", L""),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	
	Sevent tNrm1(AEvent::NONE/*��� �������*/	,40/* ������ ����*/,20/* ����� �� �������*/,40/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
		,3/*���.�������*/,7/* ����� �������*/);
	Sevent tNrm2(AEvent::NONE/*��� �������*/,45/*������ ����*/,10/* ����� �� �������*/,50/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
		,7/*���.�������*/,7/* ����� �������*/);
	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	auto norm2=tpl.norma(tNrm2,2);
	auto norm1=tpl.norma(tNrm1,1);
 	{
 		Sevent tN_1(AEvent::NONE/*��� �������*/	,40/* ������ ����*/,40/* ����� �� �������*/,140/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
 			,3/*���.�������*/,7/* ����� �������*/);
 		Sevent tN_2(AEvent::NONE/*��� �������*/,45/*������ ����*/,90/* ����� �� �������*/,150/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
 			,7/*���.�������*/,7/* ����� �������*/);
 		auto norm2=tpl.norma(tN_2,2);
 		auto norm1=tpl.norma(tN_1,1);
 		TestAAdmin adm;
 		adm.add(tpl.add(trn1));//�� ��������
 		adm.add(tpl.add(trn2));//�� ��������
 		adm.addNorma(trn1.tdescr,norm1);
 		adm.addNorma(trn2.tdescr,norm2);
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		//��  16  �� 6002 ������ ������� � 182, �� � 180  �� 6001 ������� � 16 , �.�. �� 6002 ���� ������������ ��������
 		tstTRACE(adm);
 		TRACE(deviation(adm,trFrc).c_str());
 		auto tFrst=adm.fnd(trn1.tdescr);
 		auto tScnd=adm.fnd(trn2.tdescr);
 		AEvent ev=tScnd->fndEv(EsrKit(16));
 		AEvent evF=tFrst->fndEv(EsrKit(15));
 		auto pmt=tFrst->getPMT(ev);
 		CPPUNIT_ASSERT(!ev.empty() && ev.diap.duration()>pmt.full && "�� 16 ��2 �� ����� ������� ��-�� ��1, ������� ������� � 16, �.�. ��2 ���� ������������ ��������");
 		CPPUNIT_ASSERT(!evF.empty() && evF.diap.duration()==tN_1.tSt && "��1 ������� ������ �� �������");
 		auto rsn=tScnd->outrunInfo();// ����� �������
 		CPPUNIT_ASSERT(rsn.size()==1 && rsn.find(16)!=rsn.end());
 		CPPUNIT_ASSERT(adm.numStepFrc.get()==0 && "1 ��� �������");
 		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
 	}
 	{
 	Sevent tN_1(AEvent::NONE/*��� �������*/	,40/* ������ ����*/,40/* ����� �� �������*/,140/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
 		,3/*���.�������*/,7/* ����� �������*/);
 	Sevent tN_2(AEvent::NONE/*��� �������*/,45/*������ ����*/,90/* ����� �� �������*/,150/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
 		,7/*���.�������*/,7/* ����� �������*/);
 		auto norm2=tpl.norma(tN_2,2);
 		auto norm1=tpl.norma(tN_1,1);
 		TestAAdmin adm;
 		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
 		adm.add(tpl.add(trn1));//�� ��������
 		adm.add(tpl.add(trn2));//�� ��������
 		adm.addNorma(trn1.tdescr,norm1);
 		adm.addNorma(trn2.tdescr,norm2);
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		//��  16  �� 6002 ������ ������� � 182, �� � 180  �� 6001 ������� � 16 , �.�. �� 6002 ���� ������������ ��������
 		tstTRACE(adm);
 		TRACE(deviation(adm,trFrc).c_str());
 		auto tFrst=adm.fnd(trn1.tdescr);
 		auto tScnd=adm.fnd(trn2.tdescr);
 		AEvent ev=tScnd->fndEv(EsrKit(16));
 		AEvent evF=tFrst->fndEv(EsrKit(15));
 		auto pmt=tFrst->getPMT(ev);
 		auto rsn=tScnd->outrunInfo();// ����� �������
  		CPPUNIT_ASSERT(rsn.empty()&& " ������ �������������");
 		CPPUNIT_ASSERT(adm.numStepFrc.get()==1 && "2 ���� �������");
 		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
 	}
	{
 		TestAAdmin adm;
 		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
 		adm.add(tpl.add(trn1));//�� ��������
 		adm.add(tpl.add(trn2));//�� ��������
 		adm.addNorma(trn1.tdescr,norm1);
 		adm.addNorma(trn2.tdescr,norm2);
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		//��  16  �� 6002 ������ ������� � 182, �� � 180  �� 6001 ������� � 16 , �.�. �� 6002 ���� ������������ ��������
 		tstTRACE(adm);
 		auto tFrst=adm.fnd(trn1.tdescr);
 		auto tScnd=adm.fnd(trn2.tdescr);
 		M_INT_EV m15= fltrStatEv(15,trFrc);
 		CPPUNIT_ASSERT(m15[6002].diap.duration()==tNrm2.tSt && " ��2 �� �������� �� 16");
 		CPPUNIT_ASSERT(m15[6001].diap.duration()>tNrm1.tSt && "��1  �������������� �� 15");
		CPPUNIT_ASSERT(m15[6002].diap.enclose(m15[6001].diap.getEnd()));
		CPPUNIT_ASSERT(m15[6002].diap.getEnd()<=m15[6001].diap.getEnd()+defTimes.tsk.count()+ AAdmin::GapStop());
 		auto rsn=tScnd->outrunInfo();// ����� �������
 		CPPUNIT_ASSERT(rsn.empty());
 		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
	}
}
 
void TC_AThread::testOutrunInfo()
{// ���� ����� ���������� �� �������
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	auto ipp= tpl.getInterval();
	ipp.tn=std::chrono::seconds(1);
	ipp.tnp=std::chrono::seconds(1);
	tpl.setStInt(ipp);
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.������� (+10 � ������������)
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
	Sevent sEv1001(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		20, // ����� �� �������
		20,// ����� �� ��������
		10,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent sEv1003(//�������� ����
		AEvent::REAL,// ��� �������
		22,// ������ ����
		5, // ����� �� �������
		0,// ����� �� ��������
		0,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		1// ����� �������
		);

 	Sevent tNrm(AEvent::NONE,// ��� �������
 		20,// ������ ����
 		20,// ����� �� �������
 		20,// ����� �� ��������
 		0,// ����� �� ��������� ��������
 		6, // ���.�������
 		7// ����� �������
 		);
 	auto norm1001=tpl.norma(tNrm,6001);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv4, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"6001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
	Strain trn1003( TrainDescr( L"6003", L"1-100-2"),sEv1003, Sevent( NO_FIXED ));
	TRACE("\r\n =========== testOutrunInfo ============");
	{
		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
		TestAAdmin adm;

		TestAAdmin::dispNormElSec=0;//  ���������� ���������� �� ���������� ��� ��.������
		TestAAdmin::dispNorm=0;//  ���������� ���������� �� ���������� �� ��� ��.������
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn4,1));
		adm.add(tpl.add(trn1001,2));//�� ��������
		adm.add(tpl.add(trn1003,1));//�� ��������
		adm.addNorma( trn1001.tdescr, norm1001 );
		adm.addNorma( trn1003.tdescr, norm1001 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");

		auto t2=adm.fnd(trn2);
		auto t4=adm.fnd(trn4);
		auto t1001=adm.fnd(trn1001);
		auto t1003=adm.fnd(trn1003);
		
		TrainDescr tbl[]={trn2.tdescr,trn1001.tdescr,trn4.tdescr,trn1003.tdescr};
		std::vector<TrainDescr>vT(ARRAY_IT(tbl));
		std::vector<ConstTrainDescrPtr> vTd;
		for (const auto& t: vT)
		{
			auto trainDescr = ConstTrainDescrPtr( new TrainDescr( t ) );
			vTd.emplace_back( trainDescr );
		}
		const auto & sortedVec = TrainsPriorityDefiner().Sort( vTd, TrainsPriorityOffsets() );

		CPPUNIT_ASSERT(sortedVec.size()==2 && sortedVec.front().size()==2&& sortedVec.back().size()==2);
		for (const auto& t: sortedVec.front())
			CPPUNIT_ASSERT(t->GetNumber().getNumber()==2 ||t->GetNumber().getNumber()==4);
		for (const auto& t: sortedVec.back())
			CPPUNIT_ASSERT(t->GetNumber().getNumber()==6001 ||t->GetNumber().getNumber()==6003);
		
		auto outr=adm.outrunInfo(false);
		std::wstring outRun= outr->text();
		outTRACE(outRun);
		{
			TOutrun tbl[]={{16,6001,6003},{16,2,6003},
			/*{17,6001,6003},*/{17,6001,2}/*,{17,2,6003}*/,{17,4,6003},/*{17,2,4},*/
			               /*{18,4,6001},{18,2,6001},*/{18,6001,4}/*,{18,6001,2},{18,2,4}*/
			};
			CPPUNIT_ASSERT(equ(outr,std::vector<TOutrun>(ARRAY_IT(tbl))) && "���������� � �������� � �������");
		}
	}
}

void TC_AThread::testExcepToOutrun()
{// ������� ������������� ������� ���� � ������� ��� ������
	TRACE("\r\n =========== testExcepToOutrun ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 2 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(30);
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		5, // ����� �� �������
		30,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv2001(//�������� ����
		AEvent::REAL,// ��� �������
		2,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		10,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv2004(//�������� ����
		AEvent::REAL,// ��� �������
		22,// ������ ����
		5, // ����� �� �������
		0,// ����� �� ��������
		0,// ����� �� ��������� ��������
		7, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm(AEvent::NONE,// ��� �������
		20,// ������ ����
		1,// ����� �� �������
		35,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.�������
		4// ����� �������
		);
	TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
	auto norm2=tpl.norma(tNrm,2);
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	Strain trn2001( TrainDescr( L"2001", L"1-100-2"),sEv2001, Sevent( NO_FIXED ));
	Strain trn2004( TrainDescr( L"2004", L"1-100-1"),sEv2004, Sevent( NO_FIXED ));
	AEvent ev18;
	{
		TestAAdmin::fnFltr.push_back(nullptr);// �-���  ������ ������� �� ������� �� �����������
		TestAAdmin adm;
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
		TestAAdmin::deltaWait=6;//����� ��� ������� ��� ���������� ������
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
		adm.add(tpl.add(trn2));
		adm.add(tpl.add(trn2001));//�� ��������
		adm.add(tpl.add(trn2004,1));//�� ��������
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.empty());
		M_INT_EV m17= fltrStatEv(17,trFrc),m18= fltrStatEv(18,trFrc);

		CPPUNIT_ASSERT(m18[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m18[2].diap.enclose(m18[2001].diap));
		CPPUNIT_ASSERT(m18[2].diap.getEnd()-m18[2001].diap.getEnd()<AAdmin::deltaThrough()+ AAdmin::GapStop() && "������ ������� ������������");

		CPPUNIT_ASSERT(m17[2004].diap.enclose(m17[2001].diap));
		CPPUNIT_ASSERT(m17[2004].diap.getEnd()-m17[2001].diap.getEnd()<AAdmin::deltaThrough()+ AAdmin::GapStop());
		ATrainPtr t1=adm.fnd(trn2);
		ATrainPtr t2=adm.fnd(trn2001);
		CPPUNIT_ASSERT( t2->outrun(t1).empty() && " ����� �� 17 ������� ������������") ;
	}
	{
		TestAAdmin::fnFltr.push_back(nullptr);// �-���  ������ ������� �� ������� �� �����������
		TestAAdmin adm;
		adm.add(tpl.add(trn2));
		adm.add(tpl.add(trn2001));//�� ��������
		adm.add(tpl.add(trn2004,1));//�� ��������
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.empty());
		ATrainPtr t1=adm.fnd(trn2);
		ev18= t1->fndEv(EsrKit(18));
		CPPUNIT_ASSERT(!ev18.empty() && ev18.getType()==AEvent::STOP_START);
		ATrainPtr t2=adm.fnd(trn2001);
		CPPUNIT_ASSERT( t2->outrun(t1).empty() && " ����� �� 17 ������� ������������") ;
	}
	{
 		TestAAdmin adm;
		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
		TestAAdmin::noSort=true;
 		adm.add(tpl.add(trn2));
 		adm.add(tpl.add(trn2001));//�� ��������
 		adm.add(tpl.add(trn2004,1));//�� ��������
 		adm.addNorma( trn2.tdescr, norm2 );
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		auto clash=adm.getClash();
 		CPPUNIT_ASSERT(clash.size()==1 && clash.front()->Comment()==INVALID_ACT_OBJ);
 		ATrainPtr t1=adm.fnd(trn2);
 		ATrainPtr t2=adm.fnd(trn2001);
		long tx[]={17};
		CPPUNIT_ASSERT( t2->outrun(t1)==LONG_SET(ARRAY_IT(tx))  && " ����� �� 17 �������") ;
// 		CPPUNIT_ASSERT(AOutrunTaskLst::filtrExcp(17,t1, t2,AVectClash(clash)) && " ����������� ������ �� ����������");// t1- ���������� , t2 -����������
 		/* ������� ������ ���������� ��� ����������� ��, ������ �� ��� ��� �������� �� ������� � �������� ����
 		� ������� ��� �� ���� ������� ��2, ��� ��������� ������� �� �������� */
 	}
	TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
}


void TC_AThread::testPassOutrun()
{// ������� ��� ������ ��� �������� ������
	TRACE("\r\n =========== testPassOutrun ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 2 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(30);
	Sevent sEv1(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		5, // ����� �� �������
		30,// ����� �� ��������
		0,// ����� �� ��������� ��������
		5, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv2001(//�������� ����
		AEvent::REAL,// ��� �������
		22,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		10,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv2004(//�������� ����
		AEvent::REAL,// ��� �������
		42,// ������ ����
		5, // ����� �� �������
		0,// ����� �� ��������
		0,// ����� �� ��������� ��������
		7, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm(AEvent::NONE,// ��� �������
		20,// ������ ����
		1,// ����� �� �������
		30,// ����� �� ��������
		0,// ����� �� ��������� ��������
		5, // ���.�������
		4// ����� �������
		);
	TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
	auto norm1=tpl.norma(tNrm,1);
	Strain trn1( TrainDescr( L"1", L""),sEv1, Sevent( NO_FIXED ));
	Strain trn2001( TrainDescr( L"2001", L"1-100-2"),sEv2001, Sevent( NO_FIXED ));
	Strain trn2004( TrainDescr( L"2004", L"1-100-1"),sEv2004, Sevent( NO_FIXED ));
	{
		TestAAdmin adm;
		TestAAdmin::fnFltr.push_back(nullptr);// �-���  ������ ������� �� �����������
		TestAAdmin::noSort=true;
		adm.numStepFrc.limit=10;
		adm.add(tpl.add(trn1));
		adm.add(tpl.add(trn2001));//�� ��������
		adm.add(tpl.add(trn2004,1));//�� ��������
		adm.addNorma( trn1.tdescr, norm1 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.empty() && "���������� �������������");
		ATrainPtr t1=adm.fnd(trn1);
		ATrainPtr t2=adm.fnd(trn2001);
		CPPUNIT_ASSERT( t2->outrun(t1).empty() && " ����� �� 17 ������� ������������") ;
	}
	TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
		adm.add(tpl.add(trn1));
		adm.add(tpl.add(trn2001));//�� ��������
		adm.add(tpl.add(trn2004,1));//�� ��������
		adm.addNorma( trn1.tdescr, norm1 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		ATrainPtr t1=adm.fnd(trn1);
		ATrainPtr t2=adm.fnd(trn2001);
		ATrainPtr t4=adm.fnd(trn2004);
		long tx[]={17};
		CPPUNIT_ASSERT( t2->outrun(t1)==LONG_SET(ARRAY_IT(tx)) && " ����� �� 17 �������") ;
		CPPUNIT_ASSERT(clash.size()==1 && clash.front()->event()->Esr()==EsrKit(17));
		CPPUNIT_ASSERT(clash.front()->Trn()==t4 && clash.front()->ConflTrn() && " ����������� ������ �� ����������");// t1- ���������� , t2 -����������
	}
}

void TC_AThread::testWaitCounterPack()
{// ������� �� ����.�������� ���������� ������
	TRACE("\r\n =========== testWaitCounterPack ============");
	Sevent sEv1(AEvent::REAL ,10/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,10/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv4(AEvent::REAL ,65/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv6(AEvent::REAL ,85/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv8(AEvent::REAL ,155/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	TrainDescr t2( L"8002", L"1-1-1");
	t2.SetBrutto( AAdmin::Advantage_brutto()+1); 
	TrainDescr t4( L"8004", L"1-1-1");
	t4.SetBrutto( AAdmin::Advantage_brutto()+1); 
	TrainDescr t6( L"8006", L"1-1-1");
	t6.SetBrutto( AAdmin::Advantage_brutto()+1); 
	TrainDescr t8( L"8008", L"1-1-1");
	t8.SetBrutto( AAdmin::Advantage_brutto()+1); 
	Strain trn1( TrainDescr(L"8001", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//��������
	Strain trn2( t2,sEv2, Sevent( NO_FIXED )); 
	Strain trn4( t4,sEv4, Sevent( NO_FIXED )); 
	Strain trn6( t6,sEv6, Sevent( NO_FIXED )); 
	Strain trn8( t8,sEv8, Sevent( NO_FIXED )); 

	// �� 1-� ����� ����������� ����
//	TC_AtopologyParams tplParams( 30,8, 1, SpanBreadth::DOUBLE_WAY_SPAN ); 
	TC_AtopologyParams tplParams( 30,8, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20,&defTimes);

	{
		TestAAdmin adm;
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		adm.deltaWait=2;// ����� ��� ������� ��� ���������� ������ 
		adm.rgSortCargo=true; // ���������� �� ����
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2,1));//�� ��������
		adm.add(tpl.add(trn4,2));//�� ��������
		adm.add(tpl.add(trn6,3));//�� ��������
		adm.add(tpl.add(trn8,4));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.empty() && "���������� ���");
		std::map<int,std::vector<long>> mStop;
		for (const auto& t: trFrc.getSpotLines())
		{
			auto n=t.first.GetNumber().getNumber();
			std::vector<long>  v;
			std::for_each(t.second.begin(),t.second.end(),[&v](const AEvent& ev){if(!ev.onSpan() && ev.diap.duration()>AAdmin:: deltaThrough()) v.push_back(ev.Esr().getTerm());});
			mStop[n]=v;
		}
		long tbl8001[]={13,14};
		long tbl[]={14};
		CPPUNIT_ASSERT(mStop[8001]==std::vector<long>(ARRAY_IT(tbl8001)) && " ���������  �� 13 � 14 ��������");
		CPPUNIT_ASSERT(mStop[8004]==std::vector<long>(ARRAY_IT(tbl)) && " ���������  �� 14 �������");
		CPPUNIT_ASSERT(mStop[8006]==std::vector<long>(ARRAY_IT(tbl)) && " ���������  �� 14 �������");
		CPPUNIT_ASSERT(mStop[8008]==std::vector<long>() && " ��������� ���");
		ATrainPtr t1=adm.fnd(trn1);
		CPPUNIT_ASSERT(t1->fndEv(13,0).diap.duration()<AAdmin::MaxWaitCounterPack() && t1->fndEv(14,0).diap.duration()<AAdmin::MaxWaitCounterPack() && " ���������� ������� ���");
	}
	{
		TestAAdmin adm;
		adm.maxWaitCounterPack=0 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		adm.rgSortCargo=true; // ���������� �� ����
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2,1));//�� ��������
		adm.add(tpl.add(trn4,2));//�� ��������
		adm.add(tpl.add(trn6,3));//�� ��������
		adm.add(tpl.add(trn8,4));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.empty() && "���������� ���");
		auto outr=adm.outrunInfo(false);
		std::wstring outRun= outr->text();
		outTRACE(outRun);
		TOutrun tbl[]={{13,8004,8001},{13,8006,8001}, // ��� �������� 8001
		{13,8008,8001},
		{13,8002,8001} 
		};
		CPPUNIT_ASSERT(equ(outr,std::vector<TOutrun>(ARRAY_IT(tbl))) && "��� �������� 8001");
		for (const auto& t: trFrc.getSpotLines())
			for (const auto& ev:t.second )
				if(!ev.onSpan())
				{
					if(t.first.GetNumber().getNumber()==8001 && ev.Esr().getTerm()==13)
						CPPUNIT_ASSERT(ev.diap.duration()>AAdmin::MaxWaitCounterPack() && " ���������� �������");
					else
						CPPUNIT_ASSERT(ev.diap.duration()<AAdmin::deltaThrough()+2 && "��� ������� �� ������ �������� � ��� ������ ��");
				}
	}

}