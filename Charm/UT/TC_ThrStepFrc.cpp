/*!
	created:	2019/03/13
	created: 	12:48  13 ���� 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrStepFrc.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrStepFrc
	file ext:	cpp
	author:		 Dremin
	
	purpose:	��� �������� � ��������� ��� �����������
  
  */
 
#include "stdafx.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/TrainRoutes.h"
#include "../Augur/AOutrun.h"
#include "../helpful/TechnicalTime.h"
#include "../helpful/RoadCode.h"
#include "../Augur/AClash.h"
#include "../Augur/AAdmin.h"
#include "../Augur/AConflData.h"

using namespace std;

void TC_AThread::testMorthStopMove()
{// ���� ����������� ������� � ������������� (������ � �����) 
	TRACE("\r\n =========== testMorthStopMove ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.tn=chrono::seconds(9);
	defTimes.tnp=chrono::seconds(12);
	tpl.setPvx(PVX_STANDART,&defTimes);
	Sevent sEv2(	AEvent::REAL,77/*������ ����*/,1/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	Sevent sEv1(	AEvent::REAL,1/*������ ����*/,1/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		0 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent tNrm1(AEvent::NONE,// ��� �������
		95,// ������ ����
		30,// ����� �� �������
		30,// ����� �� ��������
		0,// ����� �� ��������� ��������
		0, // ���.������� (+10 � ������������)
		10// ����� �������
		);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,Sevent(NO_FIXED));
	ATrainPtr tr1=tpl.add(trn1,1,TC_Atopology::BOTH);
	Strain trn2( TrainDescr( L"2", L"2-200-1"),sEv2,Sevent(NO_FIXED));
	ATrainPtr tr2=tpl.add(trn2,2,TC_Atopology::BOTH);
	auto norm1=tpl.norma(tNrm1,1);
	{// ��� �������� ����� ��������� �� 
		TestAAdmin adm;
		adm.MinSmallStop(14);// ����� ��������� ������� ���  ����������� � ������������� � ���
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		adm.addNorma(TrainDescr( L"1", L""), norm1 );
		adm.add(tr1);//�� ��������
		adm.add(tr2);//�� ��������
		AConflData::strongTnpRgm(false);// ���������(true)/������(false) ����� ��������� �� � ������, ���� ��� ������ ��������� � ����������, ��  ����� �� ���������.
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m11[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m11[1].diap.enclose(m11[2].diap.getEnd()));
		CPPUNIT_ASSERT(m11[2].diap.enclose(m11[1].diap.getOrig()));
		CPPUNIT_ASSERT(m11[2].diap.getEnd()>=m11[1].diap.getOrig()+ defTimes.tsk.count());
		CPPUNIT_ASSERT(m11[2].diap.getEnd()<=m11[1].diap.getOrig()+ defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(abs(m11[2].diap.getOrig()-m11[1].diap.getOrig())>=defTimes.tn.count());
		CPPUNIT_ASSERT(abs(m11[2].diap.getOrig()-m11[1].diap.getOrig())<=defTimes.tn.count()+ AAdmin::GapStop());
	}
	{// ��� ������� ����� ��������� �� 
		TestAAdmin adm;
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		ACounterInt::limit=1;//���������� ����������
		adm.addNorma(TrainDescr( L"1", L""), norm1 );
		adm.add(tr1);//�� ��������
		adm.add(tr2);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m11[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m11[1].diap.enclose(m11[2].diap));
		CPPUNIT_ASSERT(m11[2].diap.getOrig()-m11[1].diap.getOrig()>defTimes.tn.count());
		CPPUNIT_ASSERT(m11[2].diap.getOrig()-m11[1].diap.getOrig()<defTimes.tn.count()+9);
	}
}
