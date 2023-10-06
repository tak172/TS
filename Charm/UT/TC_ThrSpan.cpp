/*!
	created:	2019/07/03
	created: 	16:02  03 ���� 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrSpan.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrSpan
	file ext:	cpp
	author:		 Dremin
	
	purpose:	��������  �� ��������
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"

using namespace std;

void TC_AThread::testAutoBlockPack()
{// ����������� ������ ��� ������������� 

	TRACE("\r\n =========== testAutoBlockPack ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Ipr=chrono::seconds(10);
	defTimes.Iot=chrono::seconds(10);

	tpl.setPvx(40,&defTimes);
	Stech tWnd( 10, 190 /*������������*/, 1, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
	Sevent sEv2(AEvent::REAL,5,/* ������ ����*/2, /* ����� �� �������*/20,0,4, /* ���.������� (+10 � ������������)*/1/* ����� �������*/	);
	Sevent sEv4(AEvent::REAL,55,/* ������ ����*/2, /* ����� �� �������*/20,0,4, /* ���.������� (+10 � ������������)*/1/* ����� �������*/	);
	Sevent sEv6(AEvent::REAL,105,/* ������ ����*/2, /* ����� �� �������*/20,0,4, /* ���.������� (+10 � ������������)*/1/* ����� �������*/	);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv4, Sevent( NO_FIXED ));
	Strain trn6( TrainDescr( L"6", L"1-100-1"),sEv6, Sevent( NO_FIXED ));

	TestPossessions p( TestAAdmin::getNsi() );
	//----- ��������� ���� ------------
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	//--------------------------------
	TestAAdmin adm(p,1);
	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,2));
	adm.add(tpl.add(trn6,2));
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);

	CPPUNIT_ASSERT(m12[2].diap.getEnd()>tWnd.or+tWnd.count);
	CPPUNIT_ASSERT(m12[2].diap.getEnd()<tWnd.or+tWnd.count+ AAdmin::GapStop());
	CPPUNIT_ASSERT(m12[4].diap.getEnd()>m12[2].diap.getEnd());
	CPPUNIT_ASSERT(abs(m12[4].diap.getEnd()-m12[2].diap.getEnd()-defTimes.Iot.count())<= AAdmin::GapStop());
	CPPUNIT_ASSERT(m12[6].diap.getEnd()>m12[4].diap.getEnd());
	CPPUNIT_ASSERT(abs(m12[6].diap.getEnd()-m12[4].diap.getEnd()-defTimes.Iot.count())<= AAdmin::GapStop());
	CPPUNIT_ASSERT(m11[2].getType()==AEvent::MOVE && m11[4].getType()==AEvent::MOVE &&m11[6].getType()==AEvent::MOVE);
	CPPUNIT_ASSERT(abs(m11[4].diap.getEnd()-m11[2].diap.getEnd()-defTimes.Ipk.count())<= AAdmin::GapStop());
	CPPUNIT_ASSERT(abs(m11[6].diap.getEnd()-m11[4].diap.getEnd()-defTimes.Ipk.count())<= AAdmin::GapStop());
}

void TC_AThread::testAutoBlockDiffSpeed()
{// ����������� �� � ������ ��������� ��� ������������� 
	TRACE("\r\n =========== testAutoBlockDiffSpeed ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Ipr=chrono::seconds(10);
	defTimes.Iot=chrono::seconds(10);

	tpl.setPvx(40,&defTimes);
	Sevent sEv2(AEvent::REAL,55,/* ������ ����*/2, /* ����� �� �������*/20,0,5, /* ���.������� (+10 � ������������)*/1/* ����� �������*/	);
	Sevent sEv4(AEvent::REAL,15,/* ������ ����*/2, /* ����� �� �������*/20,0,4, /* ���.������� (+10 � ������������)*/1/* ����� �������*/	);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"1004", L"1-100-1"),sEv4, Sevent( NO_FIXED ));
	TestPossessions p( TestAAdmin::getNsi() );
	//--------------------------------
	TestAAdmin adm(p,1);
	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,2));
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);

	CPPUNIT_ASSERT(m13[2].getType()==AEvent::MOVE && m13[1004].getType()==AEvent::STOP_START);
	CPPUNIT_ASSERT(m13[1004].diap.enclose(m13[2].diap));
	CPPUNIT_ASSERT(m13[1004].diap.getEnd()==m13[2].diap.getEnd()+defTimes.Iot.count());
	CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE && m12[1004].getType()==AEvent::MOVE);
	CPPUNIT_ASSERT(m12[1004].diap.getOrig()>m12[2].diap.getOrig()+defTimes.Ipk.count());
}

void TC_AThread::testConflInWindow()
{//�������� � ���� � �������� �� ����.����
	TRACE("\r\n =========== testConflInWindow ============");
	st_intervals_t defTimes(1.f);
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Ipr=chrono::seconds(10);
	defTimes.Iot=chrono::seconds(10);
	defTimes.tsk=chrono::seconds(5);
	Sevent sEv2(AEvent::REAL,85,/* ������ ����*/2, /* ����� �� �������*/20,0,5, /* ���.������� (+10 � ������������)*/1/* ����� �������*/	);
	Sevent sEv3(AEvent::REAL,15,/* ������ ����*/2, /* ����� �� �������*/20,0,1, /* ���.������� (+10 � ������������)*/1/* ����� �������*/	);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"1-100-2"),sEv3, Sevent( NO_FIXED ));

	Stech tWnd( 10, 900 /*������������*/, 1, 0/*park*/, 4/*e1*/, 3/*e2*/, nullptr/*limit*/ );
	tWnd.sR=make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK);
	tWnd.sW=make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS);
	{
		TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // 1-� �����, 3 ���� �� �������
		TC_Atopology tpl( tplParams );
		tpl.setPvx(40,&defTimes);
		TestPossessions p( TestAAdmin::getNsi() );
		//--------------------------------
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� ������� ���������
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn3,1));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m13=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m13[2].getType()==AEvent::MOVE && m13[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[3].getType()==AEvent::MOVE && m14[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[2].diap.enclose(m14[3].diap.getOrig()));
		CPPUNIT_ASSERT(abs(m14[2].diap.getEnd()-m14[3].diap.getOrig()-defTimes.tsk.count())<= AAdmin::GapStop());
	}
	{
		TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // 2-� �����, 3 ���� �� �������
		TC_Atopology tpl( tplParams );
		tpl.setPvx(40,&defTimes);
		TestPossessions p( TestAAdmin::getNsi() );
		//----- ��������� ���� ------------
		WayInfoCPtr w=tpl.fnd(tWnd);
		RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
		std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
		p.setLimitation( w, vR );
		//--------------------------------
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� ������� ���������
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
		TestAAdmin::tskWrng[SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS]=15; /*���������� ��������� ��������� �� ������������ ������� �� ���� � �������� ����������*/

		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn3,1));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m13=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE && m14[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2].getType()==AEvent::MOVE && m13[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m13[3].diap.enclose(m13[2].diap));
		CPPUNIT_ASSERT(m13[3].diap.getEnd()-m13[2].diap.getOrig()-(defTimes.tsk.count()+TestAAdmin::tskWrng[SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS])<= AAdmin::GapStop());
		CPPUNIT_ASSERT(m13[3].diap.getEnd()>=m13[2].diap.getOrig()+(defTimes.tsk.count()+TestAAdmin::tskWrng[SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS]));
	}
}

void TC_AThread::testPartWindow()
{ //����������� ������� ���� ��������������� ���������� � �� �� ������� ����
	TRACE("\r\n =========== testPartWindow ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� ������� �������� �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) �����  ������� �������� �� ��������� 2,5 ������.
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 90, 500 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
	tWnd.sR=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
	tWnd.sW=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv4(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		62, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv4, Sevent( NO_FIXED ));

	TestPossessions p( TestAAdmin::getNsi() );
	//----- ��������� ���� ------------
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );

	TestAAdmin::StopBefAftLockPath(50,*tWnd.sR,TestAAdmin::LockSem,true); /*���������� ����������� ������� �� ������� ����� ��������� �� ����.����*/
	//--------------------------------
	TestAAdmin adm(p);
	int crosses=4;
	adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������
	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,2));
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto t2= adm.get("2");
	auto t4= adm.get("4");
	auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
	auto stint=tpl.getInterval();
	auto ev2=t2->fndEv(EsrKit(11,12));
	auto ev4=t4->fndEv(EsrKit(11,12));
 	M_INT_EV m12=fltrStatEv(12,trFrc),m11=fltrStatEv(11,trFrc);
	CPPUNIT_ASSERT(m12[4].diap.duration()>=AAdmin::StopBefAftLockPath(*tWnd.sW, true,true));
	CPPUNIT_ASSERT(m12[4].diap.duration()<=AAdmin::StopBefAftLockPath(*tWnd.sW, true,true)+ AAdmin::GapStop());
	CPPUNIT_ASSERT(m12[2].diap.duration()>=AAdmin::StopBefAftLockPath(*tWnd.sW, true,true));
	CPPUNIT_ASSERT(m12[2].diap.getEnd()> ev4.diap.getEnd());
	CPPUNIT_ASSERT(m12[2].diap.getEnd()< ev4.diap.getEnd()+AAdmin::deltaThrough() + AAdmin::GapStop());
	CPPUNIT_ASSERT(ev2.Wrongway() && ev4.Wrongway() );
	CPPUNIT_ASSERT(*ev2.Lock()==SpanLockType::ELECTRIC_TOKEN);
	CPPUNIT_ASSERT(*ev4.Lock()==SpanLockType::ELECTRIC_TOKEN);
	CPPUNIT_ASSERT(ev4.diap.duration()==ev2.diap.duration());
 	CPPUNIT_ASSERT(ev2.diap.duration()==pvx.start+ pvx.full  + pvx.stop + AAdmin::DelayLockSema(*tWnd.sR)+ crosses*TestAAdmin::crossTmPass);

	CPPUNIT_ASSERT(m11[2].diap.duration()==m11[4].diap.duration());
	CPPUNIT_ASSERT(m11[2].diap.duration()>=AAdmin::StopBefAftLockPath(*tWnd.sW, false,true));
	CPPUNIT_ASSERT(m11[2].diap.duration()<=AAdmin::StopBefAftLockPath(*tWnd.sW, false,true)+ AAdmin::deltaThrough());
}


void TC_AThread::testELECTRIC_TOKENbndWndNorm()
{ //��������������� ���������� � �� �� ������� ���� c ������.��������
	TRACE("\r\n =========== testELECTRIC_TOKENbndWndNorm ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� ������� �������� �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) �����  ������� �������� �� ��������� 2,5 ������.
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	Stech tWnd( 10, 100 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
	tWnd.sR=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
	tWnd.sW=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv4(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm4(AEvent::NONE,// ��� �������
		35,// ������ ����
		10,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.�������
		3// ����� �������
		);

	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv4, Sevent( NO_FIXED ));
	st_intervals_t defTimes;
	defTimes.Iot=chrono::seconds(7);
	defTimes.Ipr=chrono::seconds(14);
	defTimes.Ipk=chrono::seconds(22);
	tpl.setPvx(20,&defTimes);

	TestPossessions p( TestAAdmin::getNsi() );
	//----- ��������� ���� ------------
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	auto norm4=tpl.norma(tNrm4,2);
	//--------------------------------
	TestAAdmin adm(p);
	int crosses=4;
	TestAAdmin::StopBefAftLockPath(50,*tWnd.sR,TestAAdmin::LockSem,true); /*���������� ����������� ������� �� ������� ����� ��������� �� ����.����*/

	adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������
	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,2));
	adm.addNorma( trn4.tdescr, norm4 );

	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
	ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d4(m12[4].diap.getEnd(),m11[4].diap.getOrig());
	auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
	auto stint=tpl.getInterval();
	CPPUNIT_ASSERT(d2.intersect(d4) && " ������������ �� ��������");
	CPPUNIT_ASSERT(d2.enclose(d4.getEnd()));
	CPPUNIT_ASSERT(d2.getEnd()-d4.getEnd()-stint.Ipr.count()< AAdmin::GapStop());
	CPPUNIT_ASSERT(d2.getEnd()>=d4.getEnd()+stint.Ipr.count());
	CPPUNIT_ASSERT(d2.getOrig()-d4.getOrig()-stint.Iot.count()< AAdmin::GapStop());
	CPPUNIT_ASSERT(d2.getOrig()>=d4.getOrig()+stint.Iot.count());
	CPPUNIT_ASSERT(d4.getOrig()>tWnd.or+tWnd.count);
	CPPUNIT_ASSERT(d4.getOrig()-tWnd.or-tWnd.count< AAdmin::GapStop());
	CPPUNIT_ASSERT(m11[4].getType()==AEvent::STOP_START && m11[2].getType()==AEvent::MOVE);
}
