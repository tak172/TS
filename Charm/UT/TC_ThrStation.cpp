/*!
	created:	2019/05/21
	created: 	11:48  21 ��� 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrStation.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrStation
	file ext:	cpp
	author:		 Dremin
	
	purpose:	����� ��� �������� ����������� �������
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "TC_statPathProp.h"
using namespace std;

void TC_AThread::testAbsenseStationPath()
{// ���������� ���� ��� �������
	TRACE("\r\n =========== testAbsenseStationPath  ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 1 ���� �� �������
	//------------ �� 12 ������� ��������� -------------------
	tplParams.add(12,1,TC_StatPath(noStopPr));
	TC_Atopology tpl( tplParams );
	tpl.setPvx(40);
	Sevent sEv1(AEvent::REAL ,10/*������ ����*/,	1/*����� �� �������*/,0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		0/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv(AEvent::REAL ,0/*������ ����*/,	400/*����� �� �������*/,0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn( TrainDescr(),sEv, Sevent( NO_FIXED ));
	TestAAdmin adm;
	adm.add(tpl.add(trn1));//�� ��������
	adm.add(tpl.add(trn,1));//�� ��������
	auto trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto t0=adm.fnd(trn.tdescr);
	CPPUNIT_ASSERT(adm.getClash().empty());
	M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
	CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE && m12[1].getType()==AEvent::MOVE);
	CPPUNIT_ASSERT(m11[1].getType()==AEvent::STOP_START && "������� ����������");
	auto ev=t0->fndEv(EsrKit(13));
	CPPUNIT_ASSERT(ev.diap==ADiap(0,200));
	CPPUNIT_ASSERT(m13[1].diap.getOrig()>ev.diap.getEnd());
	CPPUNIT_ASSERT(m13[1].diap.getOrig()<=ev.diap.getEnd()+ AAdmin::GapStop());
}

void TC_AThread::testManagePath()
{//�������� ������������� ����� �� ������� (�����.���������)
	TRACE("\r\n =========== testTopStart ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	tplParams.add(13,2,TC_StatPath(noStopPr));
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(float(1.0));
	STT_Val s;
	s.set_gruz(4,30,0,5);
	tpl.setPvx(s);
	tpl.setStInt(defTimes);
	Sevent sEv1(AEvent::REAL ,30/*������ ����*/,	1/*����� �� �������*/,2/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv3(AEvent::REAL ,50/*������ ����*/,	1/*����� �� �������*/,2/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Sevent sEv13(AEvent::REAL ,10/*������ ����*/,	10/*����� �� �������*/,2/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Sevent sEv2(AEvent::REAL ,50/*������ ����*/,	1/*����� �� �������*/,40/*����� �� ��������*/,10/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv4(AEvent::REAL ,1/*������ ����*/,	1/*����� �� �������*/,40/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn13( TrainDescr( L"8013", L"1-1-2"),sEv13, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn2( TrainDescr( L"6002", L"1-1-1"),sEv2, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn4( TrainDescr( L"6004", L"1-1-1"),sEv4, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	{// ��� ������� �������
		TestAAdmin adm;
		adm.noSort=true;
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn4));//�� ��������
		adm.add(tpl.add(trn13));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v=adm.getClash();
		CPPUNIT_ASSERT(v.size()<2);
		CPPUNIT_ASSERT(all_of(v.begin(),v.end(),[](const P_CLSH& t){return t->significance()==PrognosisErrorCode::SINGLE;})) ;
		M_INT_EV m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[6004].diap.enclose(m13[6002].diap.getOrig()));
		CPPUNIT_ASSERT(m13[8013].diap.enclose(m13[6002].diap.getOrig()));
		CPPUNIT_ASSERT(m13[8013].diap.getEnd()>=defTimes.tsk.count()+ m13[6002].diap.getOrig());
		CPPUNIT_ASSERT(m13[8013].diap.getEnd()<=defTimes.tsk.count()+ m13[6002].diap.getOrig()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[8013].equ(m13[1]));
		CPPUNIT_ASSERT(m13[8013].equ(m13[3]));
		CPPUNIT_ASSERT(!m13[8013].equ(m13[6002]));
		CPPUNIT_ASSERT(!m13[8013].equ(m13[6004]));
		CPPUNIT_ASSERT(!m13[6004].equ(m13[6002]));
	}
	{// � �������� �������
		TestAAdmin adm;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������� �������
		adm.noSort=true;
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn4));//�� ��������
		adm.add(tpl.add(trn13));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v=adm.getClash();
		CPPUNIT_ASSERT(v.size()<2);
		CPPUNIT_ASSERT(all_of(v.begin(),v.end(),[](const P_CLSH& t){return t->significance()==PrognosisErrorCode::SINGLE;})) ;
		M_INT_EV m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m13[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[6004].diap.enclose(m13[6002].diap.getOrig()));
		CPPUNIT_ASSERT(m13[6002].diap.enclose(m13[6004].diap.getEnd()));
		CPPUNIT_ASSERT(m13[8013].diap.enclose(m13[6002].diap.getOrig()));
		CPPUNIT_ASSERT(m13[8013].diap.getEnd()>=defTimes.tsk.count()+ m13[6002].diap.getOrig());
		CPPUNIT_ASSERT(m13[8013].diap.getEnd()<=defTimes.tsk.count()+ m13[6002].diap.getOrig()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[8013].equ(m13[1]));
		CPPUNIT_ASSERT(m13[8013].equ(m13[3]));
		CPPUNIT_ASSERT(!m13[8013].equ(m13[6002]));
		CPPUNIT_ASSERT(!m13[8013].equ(m13[6004]));
		CPPUNIT_ASSERT(!m13[6004].equ(m13[6002]));
	}
}

void TC_AThread::testTopMove()
{	// ���� ��� ��� ������������� ������� �� �������
	TRACE("\r\n =========== testTopMove ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
// 	tplParams.add(12,1,TC_StatPath(noStopPr));
	tplParams.setCapacity(12,3);// ��������� ������� ����� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(float(1.0));

	Sevent sEv4(//�������� ����
		AEvent::REAL,// ��� �������
		35,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sv(//�������� ����
		AEvent::REAL,// ��� �������
		1,// ������ ����
		1000, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		1// ����� �������
		);


	Sevent sEv2002(//�������� ����
		AEvent::REAL,// ��� �������
		150,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		5, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	
	Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
	trn2002.tdescr.SetRelLength(5);
	Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED ));
	Strain trnx( TrainDescr( L"", L""),sv, Sevent( NO_FIXED ));

	Sevent tNrm4(AEvent::NONE/*��� �������*/,44/*������ ����*/,30/* ����� �� �������*/,20/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
		,5/*���.�������*/,6/* ����� �������*/);
	auto norm4=tpl.norma(tNrm4,2);
	{// ��������� ��� =>  �������� ������� �������
		defTimes.Top = std::chrono::seconds(40);
		tpl.setPvx(10,&defTimes);
		Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED ));
		Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
		trn2002.tdescr.SetRelLength(5);
		TestAAdmin adm;
		adm.add(tpl.add(trn4));//�� ��������
		adm.add(tpl.add(trn2002));//�� ��������
		adm.add(tpl.add(trnx));//�� ��������
		adm.addNorma( trn4.tdescr, norm4 );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc), m11=fltrStatEv(11,trFrc);
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[6002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[6002].diap.getOrig()>m12[4].diap.getEnd()+defTimes.Top.count());
		CPPUNIT_ASSERT(m12[6002].diap.getOrig()<=m12[4].diap.getEnd()+defTimes.Top.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m11[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m11[6002].diap.duration()<=AAdmin::GapStop()+ AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m11[6002].diap.getOrig()>m11[4].diap.getEnd());
		CPPUNIT_ASSERT(m11[6002].diap.getOrig()-m11[4].diap.getEnd()<defTimes.Top.count());
	}
	{// ��������� ��� =>  �������� ������� �������
		defTimes.Top = std::chrono::seconds(10);
		tpl.setPvx(10,&defTimes);
		TestAAdmin adm;
		adm.add(tpl.add(trn4));//�� ��������
		adm.add(tpl.add(trn2002));//�� ��������
		adm.addNorma( trn4.tdescr, norm4 );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[6002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[6002].diap.getOrig()>m12[4].diap.getEnd()+defTimes.Top.count());
		CPPUNIT_ASSERT(m12[6002].diap.getOrig()<=m12[4].diap.getEnd()+defTimes.Top.count()+ AAdmin::GapStop());
	}
}

void TC_AThread::testTopStart()
{	// ���� ��� ��� ����������� (����������� �������� ���������������� ����������� � ��������� ��������, ��� �������, ��� ������� �� �������� ���� �������������� ��� ���������)
	TRACE("\r\n =========== testTopStart ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(float(1.0));
 
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv2002(//�������� ����
		AEvent::REAL,// ��� �������
		1,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		5, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sv(//�������� ����
		AEvent::REAL,// ��� �������
		1,// ������ ����
		300, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		1// ����� �������
		);


	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
	Strain trnx( TrainDescr( L"", L""),sv, Sevent( NO_FIXED ));

	Sevent tNrm(AEvent::NONE/*��� �������*/,4/*������ ����*/,20/* ����� �� �������*/,20/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
		,5/*���.�������*/,6/* ����� �������*/);
	auto norm=tpl.norma(tNrm,2);
	{// ��������� ��� => ������ �������
		defTimes.Top = std::chrono::seconds(10);
		tpl.setPvx(10,&defTimes);
		Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
		Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn2002));//�� ��������
		adm.addNorma( trn2.tdescr, norm );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc),m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[2].diap==norm[1].diap);
		CPPUNIT_ASSERT(norm[1].Esr()==EsrKit(14));
		CPPUNIT_ASSERT(m14[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[6002].Num()!=m14[2].Num());
		CPPUNIT_ASSERT(m15[6002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()>= m14[2].diap.getEnd());
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()<= m14[2].diap.getEnd()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m14[6002].diap.duration()>AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m14[6002].diap.duration()<=AAdmin::deltaThrough()+ AAdmin::GapStop()*2);
	}
	{// ��������� ��� => ������ ������� �� ����� ����
		defTimes.Top = std::chrono::seconds(10);
		tpl.setPvx(10,&defTimes);
		Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2002, Sevent( NO_FIXED ));
		Strain trn2002( TrainDescr( L"6002", L"1-1-1"),sEv2, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn2002));//�� ��������
		adm.add(tpl.add(trnx));//�� ��������
		adm.addNorma( trn2.tdescr, norm );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc),m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[2].diap==norm[1].diap);
		CPPUNIT_ASSERT(norm[1].Esr()==EsrKit(14));
		CPPUNIT_ASSERT(m14[6002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[6002].Num()==m14[2].Num());
//		CPPUNIT_ASSERT(m15[6002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()>=defTimes.Top.count()+ m14[2].diap.getEnd());
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()<=defTimes.Top.count()+ m14[2].diap.getEnd()+ AAdmin::GapStop());
	}
	{// ��������� ��� => ���������� �������
		defTimes.Top = std::chrono::seconds(10);
		tpl.setPvx(20,&defTimes);

		TestAAdmin adm;
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn2002));//�� ��������
		TestAAdmin::gapStop[0]=1;
		TestAAdmin::gapStop[1]=1;

		adm.addNorma( trn2002.tdescr, norm );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[6002].diap.getOrig()==norm[1].diap.getOrig());
		CPPUNIT_ASSERT(m14[6002].diap.getEnd()>norm[1].diap.getEnd());
		CPPUNIT_ASSERT(norm[1].Esr()==EsrKit(14));
		CPPUNIT_ASSERT(m14[6002].diap.enclose(m14[2].diap));
		CPPUNIT_ASSERT(m14[6002].diap.getEnd()>=defTimes.Top.count()+ m14[2].diap.getOrig());
		CPPUNIT_ASSERT(m13[6002].diap.getOrig()+ AAdmin::GapStop()>defTimes.Top.count()+ m13[2].diap.getOrig()- AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[6002].diap.getOrig()<defTimes.Top.count()+ m13[2].diap.getOrig()+ AAdmin::GapStop());
	}
	{// �� ��������� ���
		defTimes.Top = std::chrono::seconds(2);
		tpl.setPvx(20,&defTimes);
		TestAAdmin adm;
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn2002));//�� ��������
		adm.addNorma( trn2002.tdescr, norm );
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[6002].diap==norm[1].diap);
		CPPUNIT_ASSERT(norm[1].Esr()==EsrKit(14));
		CPPUNIT_ASSERT(m14[6002].diap.enclose(m14[2].diap.getOrig()));
		CPPUNIT_ASSERT(m14[6002].diap.getEnd()>=defTimes.Top.count()+ m14[2].diap.getOrig());
		CPPUNIT_ASSERT(m14[6002].diap.getEnd()<6+m14[2].diap.getOrig());
	}
}

void TC_AThread::testLimStatPath()
{// ����� ���� ��� ������������� ��� �������������� (����������� ��������) �� �����.����
	TRACE("\r\n =========== testLimStatPath ============");
	Sevent sEv1(AEvent::REAL ,10/*������ ����*/,	1/*����� �� �������*/,2/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	TC_AtopologyParams tplParams( 30,5, 1, SpanBreadth::ONE_WAY_SPAN ); 

	//------------ �� 14 ����� ��� ������� � ����/���� ���������� �� -------------------
	tplParams.add(14,6,TC_StatPath(passStopInOutPr));
	tplParams.add(14,5,TC_StatPath(allPr));
	tplParams.add(14,2,TC_StatPath(passStopPr));
	tplParams.add(14,1,TC_StatPath(passStopPr));
	tplParams.add(14,5,TC_StatPath(noStopPr));
	//------------ �� 12 ������� ����� ��� ������������� �� -------------------
	tplParams.add(12,8,TC_StatPath(passStopInOutPr));// ����+ �������/�������
	tplParams.add(12,7,TC_StatPath(passStopPr));//����
	tplParams.add(12,2,TC_StatPath(passMovePr));//��������������� �������
	tplParams.add(12,5,TC_StatPath(allPr));// ��� ���
	tplParams.add(12,6,TC_StatPath(noStopPr));// �� + ������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60);
	{// �������������  ��� �������������� �� �����
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m14=fltrStatEv(14,trFrc), m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::MOVE && m14[1].getType()==AEvent::MOVE && m15[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[1].Num()==6);
		CPPUNIT_ASSERT(m14[1].Num()==5);
		CPPUNIT_ASSERT(m15[1].Num()==1);
	}
	{//�������������  c ��������������� �� ����
		//������� ��������� �������� ����
		Stech tWnd( 1, 310 /*������������*/, 6, 1/*park*/, 12/*e1*/,std::make_shared<unsigned int> (39));
		Stech tWnd14_5( 1, 310 /*������������*/, 5, 1/*park*/, 14/*e1*/,std::make_shared<unsigned int> (40));
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		p.setLimitation( tWnd14_5, tpl );
		TestAAdmin adm(p);
		adm.add(tpl.add(trn1));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m14=fltrStatEv(14,trFrc), m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::MOVE && m14[1].getType()==AEvent::MOVE && m15[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[1].Num()==2);
		CPPUNIT_ASSERT(m14[1].Num()==5);
		CPPUNIT_ASSERT(m15[1].Num()==1);
	}
	{//�������������  c ���������������� �� �����
		//������� ��������� �������� ����
		Stech tWnd12_6( 1, 310 /*������������*/, 6, 1/*park*/, 12/*e1*/,std::make_shared<unsigned int> (20));
		Stech tWnd12_2( 1, 310 /*������������*/, 2, 1/*park*/, 12/*e1*/,std::make_shared<unsigned int> (20));
		Stech tWnd14_5( 1, 310 /*������������*/, 5, 1/*park*/, 14/*e1*/,std::make_shared<unsigned int> (20));
		Stech tWnd15_1( 1, 310 /*������������*/, 1, 1/*park*/, 15/*e1*/,std::make_shared<unsigned int> (20));
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd12_2, tpl );
		p.setLimitation( tWnd12_6, tpl );
		p.setLimitation( tWnd14_5, tpl );
		p.setLimitation( tWnd15_1, tpl );
		TestAAdmin adm(p);
		adm.add(tpl.add(trn1));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m14=fltrStatEv(14,trFrc), m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::MOVE && m14[1].getType()==AEvent::MOVE && m15[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[1].Num()==1);
		CPPUNIT_ASSERT(m14[1].Num()==1);
		CPPUNIT_ASSERT(m15[1].Num()==2);
	}
}
