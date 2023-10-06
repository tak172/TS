/*!
	created:	2019/04/26
	created: 	14:36  26 ������ 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrConflicts.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrConflicts
	file ext:	cpp
	author:		 Dremin
	
	purpose:	������������� ��������� � ��������������
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "TC_statPathProp.h"
using namespace std;

void TC_AThread::testInflConfl()
{//��������� � �������������� (�����������  �������� ����� ��� ������� ���������� ���������)
	TRACE("\r\n =========== testMorphFrcReal ============");
	Sevent sEv1(AEvent::REAL ,10/*������ ����*/,	1/*����� �� �������*/,2/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,50/*������ ����*/,1/*����� �� �������*/,2/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		2/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv3(AEvent::REAL ,30/*������ ����*/,1/*����� �� �������*/,2/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		2/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Sevent sEv4(AEvent::REAL ,60/*������ ����*/,1/*����� �� �������*/,2/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	trn1.tdescr.SetBrutto(AAdmin::Advantage_brutto()+1);
	trn4.tdescr.SetBrutto(AAdmin::Advantage_brutto()+1);

	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,5, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	defTimes.Iot=chrono::seconds(7);
	defTimes.Ipr=chrono::seconds(5);
	defTimes.Ipk=chrono::seconds(12);
	TC_Atopology tpl( tplParams );
	STT_Val pvx=generateSTT(1,50,1,1);
	STT_Val pvx3_4=generateSTT(1,50,1,20);
	TC_Atopology::MAP_SPANTIME m;
	m[make_pair(12,13)]=pvx3_4;
	tpl.setPvx(pvx,m);
	tpl.setStInt(defTimes);
	{// ��������� �������� � ��������� � ��������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� ������� ���������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� ������� ��������
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
		TestAAdmin::maxWaitCounterPack=50;
		ACounterInt::limit=10;

		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn4));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT( m13[3].diap.getEnd()<m13[1].diap.getOrig() && "����� �����");
		CPPUNIT_ASSERT(m13[3].diap.enclose(m13[4].diap.getOrig()) && m13[4].diap.enclose(m13[1].diap.getOrig()));
		CPPUNIT_ASSERT(m12[2].diap.duration()>10 && m12[2].diap.enclose(m12[1].diap.getOrig()));
		CPPUNIT_ASSERT(m14[4].diap.getEnd()<m14[3].diap.getOrig());
		CPPUNIT_ASSERT( m14[3].diap.getEnd()<m14[1].diap.getOrig() );
	}
 	{// �� ��������� ���������
  		TestAAdmin adm;
  		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
  		TestAAdmin::fnFltr.clear();
   		adm.add(tpl.add(trn3));//�� ��������
  		adm.add(tpl.add(trn4));//�� ��������
 		adm.add(tpl.add(trn2));//�� ��������
  		adm.add(tpl.add(trn1));//�� ��������
  		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
  		tstTRACE(adm);
  		M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc)
  			,m13=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
  		CPPUNIT_ASSERT(m12.size()==4 && m13.size()==3 && m14.size()==3);
  		CPPUNIT_ASSERT(m14[4].diap.duration()>50 && m14[4].diap.enclose(m14[3].diap.getOrig()));
  		CPPUNIT_ASSERT(m13[4].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT( m12[1].diap.enclose(m12[4].diap.getOrig()) && m12[1].diap.duration()>50);
  		CPPUNIT_ASSERT(m11[1].diap.duration()>50 && m11[1].diap.enclose(m11[2].diap.getOrig()));
 	}
	{// ��������� �������� � ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� ������� ���������
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
		ACounterInt::limit=10;
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn4));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT( m13[3].diap.enclose(m13[1].diap) && "������ �����");
		CPPUNIT_ASSERT(m13[3].diap.enclose(m13[4].diap.getOrig()));
		CPPUNIT_ASSERT(m12[2].diap.duration()>10 && m12[2].diap.enclose(m12[1].diap.getOrig()));
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::MOVE  && m13[1].getType()==AEvent::MOVE);
	}
}

void TC_AThread::testPackConflNextStation()
{//�������� � ������� �������� �� ��.������� � ��������� �������� �������
	TRACE("\r\n =========== testPackConflNextStation ============");
	Sevent sEv1(AEvent::REAL ,20/*������ ����*/,	1,2,0,1,1);
	Sevent sEv3(AEvent::REAL ,30/*������ ����*/,	1,2,0,1,1);
	Sevent sEv5(AEvent::REAL ,50/*������ ����*/,	1,2,0,0,1);
	Sevent sEv7(AEvent::REAL ,85/*������ ����*/,	1,2,0,0,1);
	Sevent sEv9(AEvent::REAL ,95/*������ ����*/,	1,2,0,0,1);
	Sevent sEv2(AEvent::REAL ,10/*������ ����*/,1,2,0,5/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	
	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn5( TrainDescr( L"5", L"1-1-2"),sEv5, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn7( TrainDescr( L"7", L"1-1-2"),sEv7, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn9( TrainDescr( L"9", L"1-1-2"),sEv9, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	trn5.tdescr.SetRelLength(100);

	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,5, 1, SpanBreadth::ONE_WAY_SPAN ); 

	//------------ �� 18 ��� ������� �� ��� 2 -------------------
	for (int i = 1; i < 6; i++)
		tplParams.add(12,i,TC_StatPath(passMaj ,99));//����+ �� �� 99 �������

	st_intervals_t defTimes(1.f);
	//defTimes.Iot=defTimes.Ipk=defTimes.Ipr=chrono::seconds(10);
	defTimes.Ipk=chrono::seconds(10);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(30);
	tpl.setStInt(defTimes);

	{// ��������� ��������� �� ������
		TestAAdmin adm;
		adm.numStepFrc.limit=20;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::deltaWait=10;//����� ��� ������� ��� ���������� ������ 
		TestAAdmin::fnFltr.clear();
		TestAAdmin::useBackAdjust=true;
		adm.maxWaitCounterPack=40 ;// ����� ������������� �������� ��� �������� ���������� ������ ��

		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn5));//�� ��������
		adm.add(tpl.add(trn7));//�� ��������
		adm.add(tpl.add(trn9));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12.size()==6 && m13.size()==6 );
		CPPUNIT_ASSERT(count_if(m12.begin(),m12.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==5);
		CPPUNIT_ASSERT(m11[5].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m11[5].diap.getEnd()<m11[7].diap.getEnd() );
		CPPUNIT_ASSERT(m11[7].diap.getEnd()<m11[9].diap.getEnd() );
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m12[2].diap.enclose(m12[5].diap));
		CPPUNIT_ASSERT( m12[2].diap.enclose(m12[9].diap.getOrig()));
		CPPUNIT_ASSERT(count_if(m13.begin(),m13.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==5);
		CPPUNIT_ASSERT(m13[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[1].diap));
		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[3].diap.getOrig()));
	}
	{// �� ��������� ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn5));//�� ��������
		adm.add(tpl.add(trn7));//�� ��������
		adm.add(tpl.add(trn9));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12.size()==6 && m13.size()==6 );
		CPPUNIT_ASSERT(count_if(m12.begin(),m12.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==6);
		CPPUNIT_ASSERT(count_if(m13.begin(),m13.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==5);
		CPPUNIT_ASSERT(m13[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[1].diap));
		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[9].diap.getOrig()));
	}	
// 	{//  ��������� ���������
// 		TestAAdmin adm;
// 		adm.numStepFrc.limit=20;
// 		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
// 		TestAAdmin::deltaWait=10;//����� ��� ������� ��� ���������� ������ 
// 		TestAAdmin::fnFltr.clear();
// 		TestAAdmin::useBackAdjust=true;
// 		TestAAdmin::backAjustNew=false;
// 		adm.maxWaitCounterPack=40 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
// 
// 		adm.add(tpl.add(trn1));//�� ��������
// 		adm.add(tpl.add(trn3));//�� ��������
// 		adm.add(tpl.add(trn5));//�� ��������
// 		adm.add(tpl.add(trn7));//�� ��������
// 		adm.add(tpl.add(trn9));//�� ��������
// 		adm.add(tpl.add(trn2));//�� ��������
// 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
// 		tstTRACE(adm);
// 		M_INT_EV m12=fltrStatEv(12,trFrc),m13=fltrStatEv(13,trFrc);
// 		CPPUNIT_ASSERT(m12.size()==6 && m13.size()==6 );
// 		CPPUNIT_ASSERT(count_if(m12.begin(),m12.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==6);
// 		CPPUNIT_ASSERT(count_if(m13.begin(),m13.end(),[](const INT_EV&t ){return t.second.getType()==AEvent::MOVE;})==5);
// 		CPPUNIT_ASSERT(m13[2].getType()==AEvent::STOP_START);
// 		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[1].diap));
// 		CPPUNIT_ASSERT( m13[2].diap.enclose(m13[9].diap.getOrig()));
// 	}

}

