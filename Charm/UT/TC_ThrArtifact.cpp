/*!
	created:	2019/01/15
	created: 	15:17  15 ������ 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrArtifact.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrArtifact
	file ext:	cpp
	author:		 Dremin
	
	purpose:	������ ���������� ���������� 
  
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_data.h"
#include "../Augur/AClash.h"
// #include "../Augur/ADiap.h"
// #include "../helpful/PrognosisErrorCode.h"
using namespace std;

void TC_AThread::testRejectArtifact()
{// ���� ������ ���������� ����������
	TRACE("\r\n =========== testRejectArtifact ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	//	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	tpl.setPvx(PVX_STANDART,&defTimes);
	Sevent sEv2(	AEvent::REAL,20/*������ ����*/,1/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	Sevent sEv4(	AEvent::REAL,40/*������ ����*/,1/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		4 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	Sevent sEv1(	AEvent::REAL,1/*������ ����*/,1/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		0 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	Sevent sEv3(	AEvent::REAL,170/*������ ����*/,1/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,Sevent(NO_FIXED));
	ATrainPtr tr1=tpl.add(trn1,1,TC_Atopology::BOTH);
	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3,Sevent(NO_FIXED));
	ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::BOTH);

	Strain trn2( TrainDescr( L"2", L"2-200-1"),sEv2,Sevent(NO_FIXED));
	ATrainPtr tr2=tpl.add(trn2,2,TC_Atopology::BOTH);
	Strain trn4( TrainDescr( L"4", L"2-200-1"),sEv4,Sevent(NO_FIXED));
	ATrainPtr tr4=tpl.add(trn4,2,TC_Atopology::BOTH);
	TestPossessions p( TestAAdmin::getNsi() );
	ADiap d4_12;
	{
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::useBackAdjust=true;
		ACounterInt::limit=6; // �������� ������������

		adm.add(tr2);//�� ��������
		adm.add(tr3);//�� ��������
		adm.add(tr1);//�� ��������
		adm.add(tr4);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==4);
		std::map<int,AEvent> m12= fltrStatEv(12,trFrc),m11= fltrStatEv(11,trFrc),m13=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m12.size()==4 && m11.size()==4);
		CPPUNIT_ASSERT( m11[1].getType()==AEvent::MOVE && m11[2].getType()==AEvent::MOVE );
		CPPUNIT_ASSERT( m11[4].getType()==AEvent::MOVE && m12[4].getType()==AEvent::MOVE );
		CPPUNIT_ASSERT( m13[4].getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT( m12[2].getType()==AEvent::STOP_START );
		CPPUNIT_ASSERT( m11[3].getType()==AEvent::STOP_START );

		CPPUNIT_ASSERT( m11[3].diap.enclose(m11[4].diap));
		CPPUNIT_ASSERT( m11[3].diap.getEnd()-m11[4].diap.getOrig()<=defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT( m11[3].diap.getEnd()-m11[4].diap.getOrig()>=defTimes.tsk.count());

		CPPUNIT_ASSERT( m13[4].diap.enclose(m13[1].diap.getOrig()));
		CPPUNIT_ASSERT( m13[4].diap.getEnd()-m13[1].diap.getOrig()<=defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT( m13[4].diap.getEnd()-m13[1].diap.getOrig()>=defTimes.tsk.count());

		CPPUNIT_ASSERT( m12[2].diap.enclose(m12[1].diap));
		CPPUNIT_ASSERT( m12[2].diap.getEnd()-m12[1].diap.getOrig()<=defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT( m12[2].diap.getEnd()-m12[1].diap.getOrig()>=defTimes.tsk.count());
	}	
	{
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::useBackAdjust=true;
		ACounterInt::limit=1;//���������� ����������
		adm.add(tr2);//�� ��������
		adm.add(tr3);//�� ��������
		adm.add(tr1);//�� ��������
		adm.add(tr4);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		std::map<int,V_EVNT> m;
		for (const auto& t:trFrc.getSpotLines())
			m[t.first.GetNumber().getNumber()]=t.second;
		CPPUNIT_ASSERT(m.size()==4);
		std::map<int,AEvent> m12,m11;
		for (const auto& t: m)
			for (const auto& ev:t.second )
			{
				if(ev.Esr()==EsrKit(11))
					m11[t.first]=ev;
				if(ev.Esr()==EsrKit(12))
					m12[t.first]=ev;
			}
		CPPUNIT_ASSERT(m11.size()==4 && m12.size()==4);
		CPPUNIT_ASSERT( m11[1].getType()==AEvent::STOP_START && m11[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m12[1].getType()==AEvent::MOVE && m12[3].getType()==AEvent::MOVE && m12[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m11[1].diap.enclose(m11[2].diap.getOrig()));
		CPPUNIT_ASSERT(m12[4].diap.enclose( m12[1].diap.getOrig()) && m12[4].diap.enclose(m12[3].diap.getOrig()));
		d4_12=m12[4].diap;
	}
	{
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		ACounterInt::limit=2; // ������� ����������
		TestAAdmin::useBackAdjust=true;
		adm.add(tr2);//�� ��������
		adm.add(tr3);//�� ��������
		adm.add(tr1);//�� ��������
		adm.add(tr4);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		std::map<int,V_EVNT> m;
		for (const auto& t:trFrc.getSpotLines())
			m[t.first.GetNumber().getNumber()]=t.second;
		CPPUNIT_ASSERT(m.size()==4);
		std::map<int,AEvent> m12,m13;
		for (const auto& t: m)
			for (const auto& ev:t.second )
			{
				if(ev.Esr()==EsrKit(13))
					m13[t.first]=ev;
				if(ev.Esr()==EsrKit(12))
					m12[t.first]=ev;
			}
		CPPUNIT_ASSERT(m13.size()==4 && m12.size()==4);
		CPPUNIT_ASSERT(  m13[4].getType()==AEvent::STOP_START && m13[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m13[4].diap.enclose(m13[1].diap.getOrig()));
		CPPUNIT_ASSERT( m12[1].getType()==AEvent::MOVE && m12[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m12[2].getType()==AEvent::STOP_START && m12[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[2].diap.enclose( m12[1].diap) && m12[4].diap.enclose(m12[3].diap.getOrig()));
		CPPUNIT_ASSERT(m12[4].diap.duration()<d4_12.duration());
	}

}

void TC_AThread::testAllowPackWait()
{// ���� ���������� �� ������� �������
	TRACE("\r\n =========== testAllowPackWait ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.Ipk=chrono::seconds(10);
	tpl.setPvx(PVX_STANDART,&defTimes);

	Sevent sEv2733(	AEvent::REAL,21/*������ ����*/,4/*����� �� �������*/,	1/*����� �� ��������*/,1/*����� �� ��������� ��������*/,
		1 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv2735(	AEvent::REAL,25/*������ ����*/,10/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		2 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv1444(	AEvent::REAL,11/*������ ����*/,4/*����� �� �������*/,	1/*����� �� ��������*/,10/*����� �� ��������� ��������*/,
		4 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv2740(	AEvent::REAL,25/*������ ����*/,10/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);



	Strain trn2733( TrainDescr( L"2733", L"1-100-2"),sEv2733,Sevent(NO_FIXED));
	ATrainPtr tr2733=tpl.add(trn2733,1,TC_Atopology::BOTH);
	Strain trn2735( TrainDescr( L"2735", L"1-100-2"),sEv2735,Sevent(NO_FIXED));
	ATrainPtr tr2735=tpl.add(trn2735,1,TC_Atopology::BOTH);

	Strain trn2740( TrainDescr( L"2740", L"2-200-1"),sEv2740,Sevent(NO_FIXED));
	ATrainPtr tr2740=tpl.add(trn2740,2,TC_Atopology::BOTH);
	Strain trn1444( TrainDescr( L"1444", L"2-200-1"),sEv1444,Sevent(NO_FIXED));
	ATrainPtr tr1444=tpl.add(trn1444,2,TC_Atopology::BOTH);

	TestPossessions p( TestAAdmin::getNsi() );

	{
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������
		ACounterInt::limit=8;//���������� ����������

		adm.add(tr2733);//�� ��������
		adm.add(tr2735);//�� ��������
		adm.add(tr1444);//�� ��������
		adm.add(tr2740);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(adm.numStepFrc.get()<ACounterInt::limit);
		M_INT_VEV m;
		for (const auto& t:trFrc.getSpotLines())
			m[t.first.GetNumber().getNumber()]=t.second;
		CPPUNIT_ASSERT(m.size()==4);
		M_INT_EV m12,m13;
		for (const auto& t: m)
			for (const auto& ev:t.second )
			{
				if(ev.Esr()==EsrKit(13))
					m13[t.first]=ev;
				if(ev.Esr()==EsrKit(12))
					m12[t.first]=ev;
			}
		CPPUNIT_ASSERT(m13.size()==4 && m12.size()==4);
		CPPUNIT_ASSERT(m12[2733].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(std::count_if(m12.begin(),m12.end(),[](const INT_EV& p ){ return p.second.diap.duration()>AAdmin::deltaThrough();})==0 && " �� 12 ��� �������");
		CPPUNIT_ASSERT(  m13[1444].getType()==AEvent::STOP_START && m13[2740].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(  m13[2733].getType()==AEvent::MOVE && m13[2735].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(  m13[2733].diap.getOrig()<m13[2740].diap.getEnd() && m13[2740].diap.getEnd()<m13[1444].diap.getEnd());
		CPPUNIT_ASSERT(  m13[2740].diap.getEnd()-m13[2733].diap.getOrig()<defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(  m13[1444].diap.getEnd()-m13[2740].diap.getEnd()<=defTimes.Ipk.count()+ AAdmin::GapStop());
	}

	{
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		// 		TestAAdmin::useBackAdjust=true;
		// 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		ACounterInt::limit=12;//���������� ����������
		adm.add(tr2733);//�� ��������
		adm.add(tr2735);//�� ��������
		adm.add(tr1444);//�� ��������
		adm.add(tr2740);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		CPPUNIT_ASSERT(adm.numStepFrc.get()<ACounterInt::limit);
		std::map<int,AEvent> m12= fltrStatEv(12,trFrc),m13= fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m13.size()==4 && m12.size()==4);
		CPPUNIT_ASSERT(m12[2733].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(std::count_if(m12.begin(),m12.end(),[](const INT_EV& p ){ return p.second.diap.duration()>AAdmin::deltaThrough();})==0 && " �� 12 ��� �������");
		CPPUNIT_ASSERT(  m13[1444].getType()==AEvent::STOP_START && m13[2740].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(  m13[2733].getType()==AEvent::MOVE && m13[2735].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(  m13[2733].diap.getOrig()<m13[1444].diap.getEnd() && m13[2740].diap.getEnd()>m13[1444].diap.getEnd());
		CPPUNIT_ASSERT(  m13[1444].diap.getEnd()-m13[2733].diap.getOrig()<defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(  m13[2740].diap.getEnd()-m13[1444].diap.getEnd()<=defTimes.Ipk.count()+ AAdmin::GapStop());
	}
}

void TC_AThread::testPackViaPack()
{// ���� ���������� �� ������� ������ �������
	TRACE("\r\n =========== testPackViaPack ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.Ipk=chrono::seconds(15);
	defTimes.Iot=chrono::seconds(12);
	tpl.setPvx(PVX_STANDART,&defTimes);

	Sevent sEv3(	AEvent::REAL,46/*������ ����*/,4/*����� �� �������*/,	1/*����� �� ��������*/,1/*����� �� ��������� ��������*/,
		1 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv5(	AEvent::REAL,25/*������ ����*/,10/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		2 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv4(	AEvent::REAL,11/*������ ����*/,4/*����� �� �������*/,	1/*����� �� ��������*/,10/*����� �� ��������� ��������*/,
		4 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv2(	AEvent::REAL,1/*������ ����*/,1/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	AEvent ev(13);
	ev.diap.set(2,52);
	V_EVNT nrm(1,ev);

	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3,Sevent(NO_FIXED));
	ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::BOTH);
	Strain trn5( TrainDescr( L"5", L"1-100-2"),sEv5,Sevent(NO_FIXED));
	ATrainPtr tr5=tpl.add(trn5,1,TC_Atopology::BOTH);

	Strain trn2( TrainDescr( L"2", L"2-200-1"),sEv2,Sevent(NO_FIXED));
	ATrainPtr tr2=tpl.add(trn2,2,TC_Atopology::BOTH);
	Strain trn4( TrainDescr( L"4", L"2-200-1"),sEv4,Sevent(NO_FIXED));
	ATrainPtr tr4=tpl.add(trn4,2,TC_Atopology::BOTH);

	TestPossessions p( TestAAdmin::getNsi() );
	{// �� ��������� ����� �������
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::useBackAdjust=true;
		//	TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������� �������
		adm.maxWaitCounterPack=0 ;// ����� ������������� �������� ��� �������� ���������� ������ ��

		ACounterInt::limit=8;//���������� ����������

		adm.add(tr3);//�� ��������
		adm.add(tr5);//�� ��������
		adm.add(tr4);//�� ��������
		adm.add(tr2);//�� ��������
		adm.addNorma( tr2->descr(),nrm);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m[4].diap.getOrig()<m[5].diap.getOrig());
		CPPUNIT_ASSERT(m[2].diap.getEnd()>m[3].diap.getOrig());
		CPPUNIT_ASSERT(m[2].diap.getEnd()<m[4].diap.getEnd());
		CPPUNIT_ASSERT(abs(m[2].diap.getEnd()+ defTimes.Ipk.count()-m[4].diap.getEnd())<15);
		CPPUNIT_ASSERT(m[5].getType()==AEvent::MOVE && m[3].getType()==AEvent::MOVE);
	}
	{// ��������� ����� �������
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::useBackAdjust=true;
		//	TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������� �������
		adm.maxWaitCounterPack=60 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		TestAAdmin::deltaWait=10;
		ACounterInt::limit=20;//���������� ����������

		adm.add(tr3);//�� ��������
		adm.add(tr5);//�� ��������
		adm.add(tr2);//�� ��������
		adm.add(tr4);//�� ��������
		adm.addNorma( tr2->descr(),nrm);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m13=fltrStatEv(13,trFrc), m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[4].diap.getOrig()<m13[5].diap.getOrig());
		CPPUNIT_ASSERT(m13[2].diap.getEnd()<m13[4].diap.getEnd());
		CPPUNIT_ASSERT(abs(m13[2].diap.getEnd()+ defTimes.Ipk.count()-m13[4].diap.getEnd())< AAdmin::GapStop()+1);

		CPPUNIT_ASSERT(m12[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[3].diap.getEnd()>m12[4].diap.getEnd());
		CPPUNIT_ASSERT(m12[3].diap.getOrig()<m12[2].diap.getEnd());
		CPPUNIT_ASSERT(m12[2].diap.getEnd()<m12[4].diap.getEnd());
	}
}