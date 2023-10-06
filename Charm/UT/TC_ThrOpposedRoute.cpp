/*!
	created:	2019/07/27
	created: 	15:21 
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrOpposedRoute.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrOpposedRoute
	file ext:	cpp
	author:		 Dremin
	
	purpose:	���� ����������� ��������
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_data.h"
#include "TC_statPathProp.h"
#include "TestAAdmin.h"

using namespace std;

void TC_AThread::testWndViaOpposed()
{// ���� ���� �� �������� ��� �������� ������������� ���������
	TRACE("\r\n =========== testWndViaOpposed ============");
	Sevent sEv1(AEvent::REAL ,15/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv3(AEvent::REAL ,10/*������ ����*/,	500/*����� �� �������*/,0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn2( TrainDescr( L"2", L"2-100-2"),sEv1, Sevent( NO_FIXED ));// �������� ��
	Strain trn( TrainDescr( L"", L""),sEv3, Sevent( NO_FIXED ));// �������� ��
	st_intervals_t def(float(1.));
	def.Iot=chrono::seconds(15);
	def.tsk=chrono::seconds(1);
	def.Tvp=chrono::seconds(15);
	def.Tvo=chrono::seconds(5);
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 2 ���� �� �������
	//------------ �� 14 ����� ��� ������� � ����/���� ���������� �� -------------------
	tplParams.add(14,1,TC_StatPath(oddMajor));// ������ + ��
	tplParams.add(14,3,TC_StatPath(oddPr));// ������
	tplParams.add(14,2,TC_StatPath( evenMajor));//���� + ��
	tplParams.add(14,4,TC_StatPath( evenPr));//����
	tplParams.add(14,5,TC_StatPath( allPr));//� ����� �����������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&def);
	AStIntervals stI;
	stI.setInterval(nullptr,def);
	V_EVNT v1;
	AEvent ev(15);
	ev.setSource(AEvent::REAL);
	ev.setObj(1,1);
	ev.diap.set(70,110);
	ev.setType(AEvent::STOP_START);
	v1.push_back(ev);
	ev.resetEsr(EsrKit(15,14));
	ev.diap.set(110);
	ev.setType(AEvent::MOVE);
	v1.push_back(ev);
	ev.resetEsr(EsrKit(13,14));
	ev.diap.set(170);
	ev.setType(AEvent::MOVE);
	ev.setObj(1,1);
	v1.push_back(ev);
	ev.resetEsr(EsrKit(13));
	ev.diap.set(230,240);
	ev.setType(AEvent::STOP_START);
	v1.push_back(ev);
	{// �� ���������� ��������, �.�. � ����� �����
		Stech tWnd( 90, 500 /*������������*/, 1, 1/*park*/, 4/*e1*/, 3/*e2*/, nullptr/*limit*/ );
	//----- ��������� ���� ------------
		WayInfoCPtr w=tpl.fnd(tWnd);
		RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
		std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( w, vR );
		TestAAdmin adm(p,1);
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
		V_EVNT v;
		AEvent ev(15);
		ev.setSource(AEvent::REAL);
		ev.setObj(1,1);
		ev.diap.set(70,110);
		ev.setType(AEvent::STOP_START);
		v.push_back(ev);
		ev.resetEsr(EsrKit(15,14));
		ev.setObj(2,1);
		ev.Wrongway(true);
		ev.diap.set(110);
		ev.setType(AEvent::MOVE);
		v.push_back(ev);
		AEvent ev14(14);
		ev14.setSource(AEvent::REAL);
		ev14.diap.set(156,165);
		ev14.setType(AEvent::STOP_START);
		ev14.setObj(4,1);
		v.push_back(ev14);
		AThread tmp(v);
		ATrainPtr trn1=tpl.add(TrainDescr( L"1", L"2-2-1"),&tmp);
		adm.add(trn1);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,2));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,5));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tr=adm.fnd(trn1->descr());
		auto pvx=  adm.fnd(trn2)->getPMT(AEvent(13,14));
		auto evS=tr->fndEv(EsrKit(14,13));
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()<ev14.diap.getEnd());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()+stI.getUndepInterval().Tvo.count()>ev14.diap.getEnd() && "����������� ���" );
		CPPUNIT_ASSERT(evS.Wrongway() && " �������� �� ����.����" );
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()==pvx.start+pvx.full+pvx.stop);
	}
	{// ������ c ���������� ��������� 1+4 ��-�� ������������ ����� ����������������� ����
		V_EVNT v(v1);
		AEvent ev14(14);
		ev14.setSource(AEvent::REAL);
		ev14.diap.set(150,153);
		ev14.setType(AEvent::MOVE);
		ev14.setObj(4,1);
		v.push_back(ev14);
		AThread tmp(v);
		ATrainPtr trn1=tpl.add(TrainDescr( L"1", L""),&tmp);
		TestAAdmin adm(1);
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
		adm.add(trn1);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,2));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,5));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tr=adm.fnd(trn1->descr());
		auto pvx=  adm.fnd(trn2)->getPMT(AEvent(13,14));
		auto evId=tr->getIdeal(EsrKit(14));
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2].diap.duration()>AAdmin::deltaThrough()+AAdmin::GapStop() && "������� � �������");
		CPPUNIT_ASSERT(m14[2].diap.getOrig()>=  ev14.diap.getEnd()+stI.getUndepInterval().Tvp.count());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()<=  ev14.diap.getEnd()+stI.getUndepInterval().Tvp.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()<pvx.start+pvx.full+pvx.stop);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()>pvx.start+pvx.full);
	}
	{// ������ c ���������� ��������� 1+4 ��-�� ������������ ����� ����������������� ����
		V_EVNT v(v1);
		AEvent ev14(14);
		ev14.setSource(AEvent::REAL);
		ev14.diap.set(140,153);
		ev14.setType(AEvent::STOP_START);
		ev14.setObj(4,1);
		v.push_back(ev14);
		AThread tmp(v);
		ATrainPtr trn1=tpl.add(TrainDescr( L"1", L""),&tmp);
		TestAAdmin adm(1);
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
		adm.add(trn1);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,2));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,5));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tr=adm.fnd(trn1->descr());
		auto pvx=  adm.fnd(trn2)->getPMT(AEvent(13,14));
		auto evId=tr->getIdeal(EsrKit(14));
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2].diap.duration()>AAdmin::deltaThrough()+AAdmin::GapStop() && "������� � �������");
		CPPUNIT_ASSERT(m14[2].diap.getOrig()>=  ev14.diap.getEnd()+stI.getUndepInterval().Tvp.count());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()<=  ev14.diap.getEnd()+stI.getUndepInterval().Tvp.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()<pvx.start+pvx.full+pvx.stop);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()>pvx.start+pvx.full);
	}
	{// ������ c ���������� ��������� 1+4 ��-�� ������������ ����� ����������������� ����
		V_EVNT v(v1);
		AEvent ev14(14);
		ev14.setSource(AEvent::REAL);
		ev14.diap.set(160,170);
		ev14.setType(AEvent::STOP_START);
		ev14.setObj(4,1);
		v.push_back(ev14);
		AThread tmp(v);
		ATrainPtr trn1=tpl.add(TrainDescr( L"1", L""),&tmp);
		TestAAdmin adm(1);
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
		adm.add(trn1);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,2));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,5));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tr=adm.fnd(trn1->descr());
		auto pvx=  adm.fnd(trn2)->getPMT(AEvent(13,14));
		auto evId=tr->getIdeal(EsrKit(14));
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(abs(m14[2].diap.getOrig()- m14[1].diap.getEnd())>=stI.getUndepInterval().Tvo.count());
		CPPUNIT_ASSERT(m14[2].diap.getEnd()- m14[1].diap.getOrig()>stI.getUndepInterval().Tvo.count());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()<=pvx.start+pvx.full+pvx.stop);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()>pvx.start+pvx.full);
	}
}

void TC_AThread::testOpposedStopRoute()
{// �� ���������� �������� �� ��������
	TRACE("\r\n =========== testOpposedStopRoute ============");
	Sevent sEv2(AEvent::REAL ,5/*������ ����*/,	10/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv1(AEvent::REAL ,25/*������ ����*/,	10/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv3(AEvent::REAL ,10/*������ ����*/,	500/*����� �� �������*/,0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent tNrm2(AEvent::NONE,0 /*������ ����*/,10 /*����� �� �������*/,60/*����� �� ��������*/,0,5/*���.�������*/,4/*����� �������*/);
	Sevent tNrm1(AEvent::NONE,0 /*������ ����*/,10 /*����� �� �������*/,60/*����� �� ��������*/,0,3/*���.�������*/,4/*����� �������*/);

	Strain trn1( TrainDescr( L"1", L"2-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"2-100-1"),sEv2, Sevent( NO_FIXED ));// �������� ��
	Strain trn( TrainDescr( L"", L""),sEv3, Sevent( NO_FIXED ));// �������� ��
	st_intervals_t def(float(1.));
	def.tn=chrono::seconds(15);
	def.tsk=chrono::seconds(10);
	def.adjustVpVo();
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 2 ���� �� �������
	//------------ �� 14 ����� ��� ������� � ����/���� ���������� �� -------------------
	tplParams.add(14,1,TC_StatPath(oddMajor));// ������ + ��
	tplParams.add(14,3,TC_StatPath(oddPr));// ������
	tplParams.add(14,2,TC_StatPath( evenMajor));//���� + ��
	tplParams.add(14,4,TC_StatPath( evenPr));//����
	tplParams.add(14,5,TC_StatPath( allPr));//� ����� �����������
	TC_Atopology tpl( tplParams );
	auto norm1=tpl.norma(tNrm1,1);
	auto norm2=tpl.norma(tNrm2,2);
	CPPUNIT_ASSERT(norm1[1].Esr()==EsrKit(14));
	CPPUNIT_ASSERT(norm2[1].Esr()==EsrKit(14));
	tpl.setPvx(60,&def);
	AStIntervals stI;
	stI.setInterval(nullptr,def);
 	{// ������ c ���������� ��������� 2+4
 		TestAAdmin adm;
		adm.dispNorm=1;//  ���������� ���������� �� ���������� 
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
 		adm.addNorma( trn1.tdescr, norm1 );
 		adm.addNorma( trn2.tdescr, norm2 );
 		adm.add(tpl.add(trn1));//�� ��������
 		adm.add(tpl.add(trn2));//�� ��������
 		adm.add(tpl.add(trn,1));//�� ��������
 		adm.add(tpl.add(trn,3));//�� ��������
 		adm.add(tpl.add(trn,5));//�� ��������
 		auto trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
		auto pvx=  adm.fnd(trn2)->getPMT(AEvent(13,14));
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].Num()==m14[1].Num());
 		CPPUNIT_ASSERT(m14[1].diap.duration()== norm1[1].diap.duration());
 		CPPUNIT_ASSERT(m14[2].diap== norm2[1].diap);
 		CPPUNIT_ASSERT(m14[1].diap.getOrig()>=m14[2].diap.getEnd()+stI.getUndepInterval().Tvp.count());
 		CPPUNIT_ASSERT(m14[1].diap.getOrig()<=m14[2].diap.getEnd()+stI.getUndepInterval().Tvp.count()+ AAdmin::GapStop());
 	}
	def.tn=chrono::seconds(5);
	def.adjustVpVo();
	tpl.setPvx(60,&def);
	stI.setInterval(nullptr,def);
	tNrm1.or+=20;
	auto norm1_=tpl.norma(tNrm1,1);
	{// ������ c ���������� ��������� 2+4 �������� ��� ��������
		TestAAdmin adm;
		//TestAAdmin::plusPrefPath=std::chrono::seconds(3); //������������ ����� ����������������� ���� ��� ������
		adm.addNorma( trn1.tdescr, norm1_ );
		adm.addNorma( trn2.tdescr, norm2 );
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,1));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,5));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14[1].diap.duration()== norm1[1].diap.duration());
		CPPUNIT_ASSERT(m14[2].diap== norm2[1].diap);
		CPPUNIT_ASSERT(m14[1].diap.getOrig()>norm1[1].diap.getOrig()+10);
		CPPUNIT_ASSERT(m14[1].diap.getOrig()>=m14[2].diap.getEnd()+stI.getUndepInterval().Tvo.count());
		CPPUNIT_ASSERT(m14[1].diap.getOrig()<=m14[2].diap.getEnd()+stI.getUndepInterval().Tvo.count()+ AAdmin::GapStop());
	}
	{// ������ ��� ���������� ��������� 
		TestAAdmin adm;
		adm.addNorma( trn1.tdescr, norm1 );
		adm.addNorma( trn2.tdescr, norm2 );
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc) ;
		CPPUNIT_ASSERT(m14[2].Num()!=m14[1].Num());
		CPPUNIT_ASSERT(m14[1].diap.distance(norm1[1].diap)<m13[1].diap.distance(norm1[0].diap));
		CPPUNIT_ASSERT(m14[2].diap== norm2[1].diap);
		CPPUNIT_ASSERT(m14[1].diap.duration()== norm1[1].diap.duration());
	}
}

void TC_AThread::testOpposedRoute()
{//�� ���������� ���� � ��� �� ����
	TRACE("\r\n =========== testOpposedRoute ============");
	Sevent sEv2(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv1(AEvent::REAL ,25/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv3(AEvent::REAL ,10/*������ ����*/,	500/*����� �� �������*/,0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent tNrm(AEvent::NONE,// ��� �������
		20,// ������ ����
		10,// ����� �� �������
		60,// ����� �� ��������
		0,// ����� �� ��������� ��������
		6, // ���.�������
		4// ����� �������
		);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		85,// ������ ����
		10,// ����� �� �������
		60,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.�������
		4// ����� �������
		);

	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"2-100-2"),sEv1, Sevent( NO_FIXED ));// �������� ��
	Strain trn( TrainDescr( L"", L""),sEv3, Sevent( NO_FIXED ));// �������� ��
	st_intervals_t def(float(1.));
	def.tn=chrono::seconds(15);
	def.tsk=chrono::seconds(10);
	def.adjustVpVo();
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 2 ���� �� �������
	//------------ �� 14 ����� ��� ������� � ����/���� ���������� �� -------------------
	tplParams.add(14,1,TC_StatPath(oddMajor));// ������ + ��
	tplParams.add(14,3,TC_StatPath(oddPr));// ������
	tplParams.add(14,2,TC_StatPath( evenMajor));//���� + ��
	tplParams.add(14,4,TC_StatPath( oddPr));//������
	tplParams.add(14,5,TC_StatPath( allPr));//� ����� �����������
	TC_Atopology tpl( tplParams );
	auto norm=tpl.norma(tNrm,2);
	tpl.setPvx(60,&def);
	AStIntervals stI;
	stI.setInterval(nullptr,def);
	{// ������� c ���������� ��������� 1+4
		// �������� ����������
		V_EVNT v;
		AEvent ev(15);
		ev.diap.set(1,120);
		ev.setSource(AEvent::REAL);
		ev.setType(AEvent::STOP_START);
		v.push_back(ev);
		AThread tmpx(v);
		ATrainPtr tr1=tpl.add(trn1.tdescr,&tmpx);
		TestAAdmin adm;
		auto norm2=tpl.norma(tNrm2,1);
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
		adm.addNorma( trn1.tdescr, norm );
		adm.addNorma( trn2.tdescr, norm2 );
		TestAAdmin::dispNorm=1;// ���������� ���������� �� ����������
		adm.add(tr1);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,2));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,5));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc);
		int tbl[]={m14[1].Num(),m14[2].Num()}, tbl1[]={1,4};
		set<int> s(ARRAY_IT(tbl)),s1(ARRAY_IT(tbl1));
		CPPUNIT_ASSERT(s ==s1);
		CPPUNIT_ASSERT(m14[2].diap.duration()>tNrm2.tSt+(int) AAdmin::GapStop() && "���������� ������� ");
		CPPUNIT_ASSERT(norm2[1].Esr().getTerm()==14);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()==norm2[1].diap.getOrig() && "�������� �� ���������� ");
		CPPUNIT_ASSERT(norm2[1].diap.getEnd()+stI.getUndepInterval().Tvp.count()> m14[1].diap.getOrig() && " �� ���������� �������� �� �������������");
		CPPUNIT_ASSERT(m14[2].diap.getEnd()>= m14[1].diap.getOrig()+ stI.getUndepInterval().Tvo.count());
		CPPUNIT_ASSERT(m14[2].diap.getEnd()<= m14[1].diap.getOrig()+stI.getUndepInterval().Tvo.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()+stI.getUndepInterval().Tvp.count()< m14[1].diap.getEnd());
	}
	{// ������ ��� ���������� ��������� 2+5
		TestAAdmin adm;
		adm.addNorma( trn1.tdescr, norm );
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,4));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,1));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tr=adm.fnd(trn1);
		auto evId=tr->getIdeal(EsrKit(14));
		M_INT_EV m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14[2].Num()==2 && m14[1].Num()==5);
		CPPUNIT_ASSERT(m14[1].diap==evId->diap);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m14[1].diap.getEnd()<stI.getUndepInterval().Tvp.count());
	}
	{// ������ c ���������� ��������� 1+4
		TestAAdmin adm;
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
		//TestAAdmin::plusPrefPath=std::chrono::seconds(3); //������������ ����� ����������������� ���� ��� ������
		adm.addNorma( trn1.tdescr, norm );
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,2));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,5));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tr=adm.fnd(trn1);
		auto pvx=  adm.fnd(trn2)->getPMT(AEvent(13,14));
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT( m14[1].Num()==4);
		CPPUNIT_ASSERT(m14[2].Num()==4 || m14[2].Num()==1);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m14[1].diap.getEnd()>=stI.getUndepInterval().Tvp.count());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m14[1].diap.getEnd()<=stI.getUndepInterval().Tvp.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()<pvx.start+pvx.full+pvx.stop);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()>pvx.start+pvx.full);
	}
	{// �� ����� ����
		TestAAdmin adm;
		adm.addNorma( trn1.tdescr, norm );
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn,1));//�� ��������
		adm.add(tpl.add(trn,2));//�� ��������
		adm.add(tpl.add(trn,3));//�� ��������
		adm.add(tpl.add(trn,4));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto tr=adm.fnd(trn1);
		auto pvx=  adm.fnd(trn2)->getPMT(AEvent(13,14));
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].Num()==5 && m14[1].Num()==5);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m14[1].diap.getEnd()>=stI.getUndepInterval().Tvp.count());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m14[1].diap.getEnd()<=stI.getUndepInterval().Tvp.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()<pvx.start+pvx.full+pvx.stop);
		CPPUNIT_ASSERT(m14[2].diap.getOrig()- m13[2].diap.getEnd()>pvx.start+pvx.full);
	}
}


void TC_AThread::testOpposedDisblPath()
{// ������ ���� ��� ����� �����.���������
	TRACE("\r\n =========== testOpposedDisblPath ============");
	V_EVNT v2;
	{
		AEvent ev(15,14);
		ev.setSource(AEvent::REAL);
		ev.setObj(1,1);
		ev.diap.set(1,9);
		ev.setType(AEvent::MOVE);
		v2.push_back(ev);
		ev.resetEsr(EsrKit(14));
		ev.diap.set(9,10);
		ev.setType(AEvent::STOP_START);
		v2.push_back(ev);
	}
	V_EVNT v1;
	{
		AEvent ev(12);
		ev.setSource(AEvent::REAL);
		ev.setObj(1,1);
		ev.diap.set(1,10);
		ev.setType(AEvent::STOP_START);
		v1.push_back(ev);
	}
	Sevent tNrm(AEvent::NONE,// ��� �������
		1,// ������ ����
		1,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.�������
		5// ����� �������
		);
	st_intervals_t def(float(1.));
	def.Iot=chrono::seconds(10);
	def.tsk=chrono::seconds(5);
	def.Tvp=chrono::seconds(25);
	def.Tvo=chrono::seconds(15);
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 2 ���� �� �������
	//------------ �� 14 ����� ��� ������� � ����/���� ���������� �� -------------------
	tplParams.add(14,1,TC_StatPath(pMaj));// ������ + ��
	tplParams.add(14,2,TC_StatPath( allPr));//� ����� �����������
	TC_Atopology tpl( tplParams );
	auto norm=tpl.norma(tNrm,1);
	tpl.setPvx(60,&def);
	{// ������� c ���������� ��������� 1+1
		ATrainPtr tr1=tpl.add(TrainDescr( L"1", L"2-100-2"),&AThread(v1));
		ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"2-100-1"),&AThread(v2));
		TestAAdmin adm;
		TestAAdmin::dThrough[0]=2;// ����� ������������� � ��������
		TestAAdmin::dThrough[1]=2;// ����� ������������� � ��������
		TestAAdmin::noSort=true;
		auto norm=tpl.norma(tNrm,1);
		adm.MinSmallStop(6);// ����� ��������� ������� ���  ����������� � �������������
		adm.addNorma( tr1->descr(), norm );
		TestAAdmin::dispNorm=1;// ���������� ���������� �� ����������
		adm.add(tr1);//�� ��������
		adm.add(tr2);//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14[2].diap.enclose(m14[1].diap));
		CPPUNIT_ASSERT(m14[2].diap.getEnd()-m14[1].diap.getOrig()>=def.tsk.count());
		CPPUNIT_ASSERT(m14[2].diap.getEnd()-m14[1].diap.getOrig()<=def.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[2].Num()==1 );
		CPPUNIT_ASSERT(m14[1].Num()==2 );
		auto t1=adm.fnd(tr1->descr());
		auto rsn= t1->Reason(14);
		CPPUNIT_ASSERT(rsn);
		CPPUNIT_ASSERT(rsn->disablePath.size()==1);
		CPPUNIT_ASSERT(rsn->disablePath.front().second->descr()==tr2->descr());
		CPPUNIT_ASSERT(rsn->disablePath.front().first->Num()==1);
	}
}