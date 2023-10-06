/*!
	created:	2019/05/08
	created: 	11:45  08 ��� 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrLock.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrLock
	file ext:	cpp
	author:		 Dremin
	
	purpose: ����� �� ������ ���� ���������� �� �������� � ����	
  
  */
 
 #include "stdafx.h"
 #include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
 
using namespace std;


void TC_AThread::testBIDIR_AUTOBLOCK_WITH_LOCOSEMAS()
{//������������� �������������� � ��������� �� �������� ������������ ����������
	TRACE("\r\n =========== testBIDIR_AUTOBLOCK_WITH_LOCOSEMAS ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� t �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) ����� t �� ��������� 2,5 ������.

	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 10, 900 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
	tWnd.sR= make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS);
	tWnd.sW=make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS);
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
	//--------------------------------
	TestAAdmin adm(p);
	int crosses=4;
	adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������

	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,2));
	
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
	CPPUNIT_ASSERT(m11[2].diap.getOrig()-m12[2].diap.getEnd()==m11[4].diap.getOrig()-m12[4].diap.getEnd());
	CPPUNIT_ASSERT(m11[2].diap.getOrig()-m11[4].diap.getOrig()==m12[2].diap.getOrig()-m12[4].diap.getOrig());
	CPPUNIT_ASSERT(m11[2].diap.getOrig()-m12[2].diap.getEnd()==crosses*TestAAdmin::crossTmPass + tpl.getPVX(nullptr,SttMove::Type::TIME_PASS).full);
}

void TC_AThread::testELECTRIC_TOKEN()
{ //��������������� ����������
	TRACE("\r\n =========== testELECTRIC_TOKEN ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� t �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) ����� t �� ��������� 2,5 ������.
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 10, 900 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
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

	TestAAdmin::StopBefAftLockPath(30,*tWnd.sR,TestAAdmin::LockSem,true); /* ������� ����� ������� �� ����.����*/
	//--------------------------------
	TestAAdmin adm(p);
	int crosses=4;
	adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������
	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,2));
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
	ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d4(m12[4].diap.getEnd(),m11[4].diap.getOrig());

	CPPUNIT_ASSERT(!d2.intersect(d4) && " �� ������������ �� ��������");
	CPPUNIT_ASSERT(d2.duration()==d4.duration());
	auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
	CPPUNIT_ASSERT(d2.duration()>=(crosses*TestAAdmin::crossTmPass + pvx.full+ pvx.start+ pvx.stop +AAdmin::DelayLockSema(*tWnd.sR)));
	CPPUNIT_ASSERT(d2.duration()-(crosses*TestAAdmin::crossTmPass + pvx.full+ pvx.start+AAdmin::DelayLockSema(*tWnd.sR))<AAdmin::deltaThrough());
	CPPUNIT_ASSERT(m12[4].diap.duration()>=AAdmin::StopBefAftLockPath(*tWnd.sR,true,true));
	CPPUNIT_ASSERT(m12[4].diap.duration()-AAdmin::StopBefAftLockPath(*tWnd.sR,true,true)< AAdmin::GapStop());
	CPPUNIT_ASSERT(m11[4].diap.duration()==AAdmin::deltaThrough()+  AAdmin::StopBefAftLockPath(*tWnd.sR,false,true));
	CPPUNIT_ASSERT(m11[4].diap.duration()==m11[2].diap.duration());
}

void TC_AThread::testTELEPHONE_CONNECTION()
{ //���������� �������� �����
	TRACE("\r\n =========== testTELEPHONE_CONNECTION ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� t �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) ����� t �� ��������� 2,5 ������.
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 10, 900 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
	tWnd.sR=make_shared<SpanLockType>(SpanLockType::TELEPHONE_CONNECTION);
	tWnd.sW=make_shared<SpanLockType>(SpanLockType::TELEPHONE_CONNECTION);
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

	TestAAdmin::StopBefAftLockPath(30,*tWnd.sR,TestAAdmin::LockSem,true); /* ������� ����� ������� �� ����.����*/
	//--------------------------------
	TestAAdmin adm(p);
	int crosses=4;
	adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������
	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,2));
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
	ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d4(m12[4].diap.getEnd(),m11[4].diap.getOrig());

	CPPUNIT_ASSERT(!d2.intersect(d4) && " �� ������������ �� ��������");
	CPPUNIT_ASSERT(d2.duration()==d4.duration());
	auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
	CPPUNIT_ASSERT(d2.duration()>=(crosses*TestAAdmin::crossTmPass + pvx.full+ pvx.start+AAdmin::DelayLockSema(*tWnd.sR)));
	CPPUNIT_ASSERT(d2.duration()-(crosses*TestAAdmin::crossTmPass + pvx.full+ pvx.start+AAdmin::DelayLockSema(*tWnd.sR))<AAdmin::deltaThrough());
	CPPUNIT_ASSERT(m12[4].diap.duration()>=AAdmin::StopBefAftLockPath(*tWnd.sR,true,true));
	CPPUNIT_ASSERT(m12[2].diap.duration()>m12[4].diap.duration());
}
 
 
// ��� ���������
void TC_AThread::testCounterBIDIR_AUTOBLOCK_WITH_LOCOSEMAS()
{//������������� �������������� � ��������� �� �������� ������������ ����������
	TRACE("\r\n =========== testCounterBIDIR_AUTOBLOCK_WITH_LOCOSEMAS ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� t �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) ����� t �� ��������� 2,5 ������.
	TestAAdmin::tskWrng[SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS]=15; /*���������� ��������� ��������� �� ������������ ������� �� ���� � �������� ����������*/

	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 10, 900 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
	tWnd.sR=make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS);
	tWnd.sW=make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS);
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv1(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		62, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		0, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, Sevent( NO_FIXED ));

	TestPossessions p( TestAAdmin::getNsi() );
	//----- ��������� ���� ------------
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	//--------------------------------
	TestAAdmin adm(p);
	int crosses=4;
	adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������

	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn1,2));

	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
	auto st_int=tpl.getInterval();
	M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
	ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d1(m11[1].diap.getEnd(),m12[1].diap.getOrig());
	CPPUNIT_ASSERT(!d2.intersect(d1) && " �� ������������ �� ��������");
	CPPUNIT_ASSERT(d1.duration()== pvx.full  + pvx.start );
	CPPUNIT_ASSERT(d2.duration()>=(crosses*TestAAdmin::crossTmPass +AAdmin::DelayLockSema(*tWnd.sR)));
	CPPUNIT_ASSERT(d2.getEnd()<d1.getOrig());
	CPPUNIT_ASSERT(abs(d1.getOrig()-d2.getEnd()-(st_int.tsk.count()+ TestAAdmin::tskWrng[SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS]))< AAdmin::GapStop());
}

void TC_AThread::testCounterELECTRIC_TOKEN()
{ //��������������� ����������
	TRACE("\r\n =========== testCounterELECTRIC_TOKEN ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� t �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) ����� t �� ��������� 2,5 ������.
	TestAAdmin::tskWrng[SpanLockType::ELECTRIC_TOKEN]=40; /* �������� ��������� �� ������������ ������� �� ���� � �������� ����������*/
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 10, 900 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
	tWnd.sR=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
	tWnd.sW=make_shared<SpanLockType>(SpanLockType::ELECTRIC_TOKEN);
	TestAAdmin::delayLockSema[*tWnd.sR]=25;
	TestAAdmin::delayAsymmetricSema=30;
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv1(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		62, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		0, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, Sevent( NO_FIXED ));

	TestPossessions p( TestAAdmin::getNsi() );
	//----- ��������� ���� ------------
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	//--------------------------------
	{
		TestAAdmin adm(p);
		int crosses=4;
		adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������

		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn1,2));

		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
		auto st_int=tpl.getInterval();
		M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
		ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d1(m11[1].diap.getEnd(),m12[1].diap.getOrig());
		CPPUNIT_ASSERT(!d2.intersect(d1) && " �� ������������ �� ��������");
		CPPUNIT_ASSERT(d1.duration()== pvx.full  + pvx.start+ pvx.stop + AAdmin::DelayLockSema(*tWnd.sR));
		CPPUNIT_ASSERT(d2.duration()== d1.duration()   + crosses*TestAAdmin::crossTmPass);
		CPPUNIT_ASSERT(d2.getEnd()<d1.getOrig());
		CPPUNIT_ASSERT((d1.getOrig()-d2.getEnd())>=AAdmin::tskReset(*tWnd.sW));
		CPPUNIT_ASSERT((d1.getOrig()-d2.getEnd())<=AAdmin::tskReset(*tWnd.sW)+ AAdmin::GapStop());
		CPPUNIT_ASSERT(TestAAdmin::tskWrng[SpanLockType::ELECTRIC_TOKEN]==AAdmin::tskReset(*tWnd.sW));
	}
	{
		TestAAdmin adm(p);
		int crosses=4;
		adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������
		adm.setAsymmetricSema(EsrKit(tWnd.e1,tWnd.e2),true);
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn1,2));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
		auto st_int=tpl.getInterval();
		M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
		ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d1(m11[1].diap.getEnd(),m12[1].diap.getOrig());
		CPPUNIT_ASSERT(!d2.intersect(d1) && " �� ������������ �� ��������");
		CPPUNIT_ASSERT(d1.duration()== pvx.full  + pvx.start+ pvx.stop + AAdmin::DelayLockSema(*tWnd.sR));
		CPPUNIT_ASSERT(d2.duration()== d1.duration()   + crosses*TestAAdmin::crossTmPass +AAdmin::DelayAsymmetricSema());
		CPPUNIT_ASSERT(d2.getEnd()<d1.getOrig());
		CPPUNIT_ASSERT((d1.getOrig()-d2.getEnd())==AAdmin::tskReset(*tWnd.sW));
		CPPUNIT_ASSERT(TestAAdmin::tskWrng[SpanLockType::ELECTRIC_TOKEN]==AAdmin::tskReset(*tWnd.sW));
	}
}

void TC_AThread::testCounterTELEPHONE_CONNECTION()
{ //���������� �������� �����
	TRACE("\r\n =========== testCounterTELEPHONE_CONNECTION ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� t �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) ����� t �� ��������� 2,5 ������.
	TestAAdmin::tskWrng[SpanLockType::TELEPHONE_CONNECTION]=30; /* �������� ��������� �� ������������ ������� �� ���� � �������� ����������*/

	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 10, 900 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
	tWnd.sR=make_shared<SpanLockType>(SpanLockType::TELEPHONE_CONNECTION);
	tWnd.sW=make_shared<SpanLockType>(SpanLockType::TELEPHONE_CONNECTION);
	TestAAdmin::delayLockSema[*tWnd.sR]=25;
	TestAAdmin::delayAsymmetricSema=30;
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv1(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		62, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		0, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, Sevent( NO_FIXED ));

	TestPossessions p( TestAAdmin::getNsi() );
	//----- ��������� ���� ------------
	WayInfoCPtr w=tpl.fnd(tWnd);
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( w, vR );
	//--------------------------------
	{
		TestAAdmin adm(p);
		adm.setAsymmetricSema(EsrKit(tWnd.e1,tWnd.e2),true);
		int crosses=4;
		adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������
		adm.setAsymmetricSema(EsrKit(tWnd.e1,tWnd.e2),true);// �����  ��������� �� ��������
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn1,2));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
		auto st_int=tpl.getInterval();
		M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
		ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d1(m11[1].diap.getEnd(),m12[1].diap.getOrig());
		CPPUNIT_ASSERT(!d2.intersect(d1) && " �� ������������ �� ��������");
		CPPUNIT_ASSERT(d1.duration()== pvx.full  + pvx.start+ AAdmin::DelayLockSema(*tWnd.sR));
		CPPUNIT_ASSERT(d2.duration()== d1.duration()   + crosses*TestAAdmin::crossTmPass+TestAAdmin::delayAsymmetricSema);
		CPPUNIT_ASSERT(d2.getEnd()<d1.getOrig());
		CPPUNIT_ASSERT((d1.getOrig()-d2.getEnd())==AAdmin::tskReset(*tWnd.sW));
		CPPUNIT_ASSERT(TestAAdmin::tskWrng[SpanLockType::TELEPHONE_CONNECTION]==AAdmin::tskReset(*tWnd.sW));
	}
	{
		TestAAdmin adm(p);
		int crosses=4;
		adm.setCrossesNum(EsrKit(tWnd.e1,tWnd.e2),crosses);// �����  ��������� �� ��������
		adm.setAsymmetricSema(EsrKit(tWnd.e1,tWnd.e2),false);
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn1,2));
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
		auto st_int=tpl.getInterval();
		M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
		ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d1(m11[1].diap.getEnd(),m12[1].diap.getOrig());
		CPPUNIT_ASSERT(!d2.intersect(d1) && " �� ������������ �� ��������");
		CPPUNIT_ASSERT(d1.duration()== pvx.full  + pvx.start+ AAdmin::DelayLockSema(*tWnd.sR));
		CPPUNIT_ASSERT(d2.duration()== d1.duration()   + crosses*TestAAdmin::crossTmPass);
		CPPUNIT_ASSERT(d2.getEnd()<d1.getOrig());
		CPPUNIT_ASSERT((d1.getOrig()-d2.getEnd())==AAdmin::tskReset(*tWnd.sW));
		CPPUNIT_ASSERT(TestAAdmin::tskWrng[SpanLockType::TELEPHONE_CONNECTION]==AAdmin::tskReset(*tWnd.sW));
	}

}

void TC_AThread::testELECTRIC_TOKENbndWnd()
{ //��������������� ���������� � �� �� ������� ����
	TRACE("\r\n =========== testELECTRIC_TOKENbndWnd ============");
	TestAAdmin::crossTmPass=15;//	��� ������������ ������� (1-998; 5001-7998) ����� ������� �������� �� ��������� 3 ������.
	TestAAdmin::crossTm=10;//	��� ��������� ������� (1001-4998; 8001-9798) �����  ������� �������� �� ��������� 2,5 ������.
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Stech tWnd( 10, 100 /*������������*/, 2, 1/*park*/, 2/*e1*/, 1/*e2*/, nullptr/*limit*/ );
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
	M_INT_EV m11=fltrStatEv(11,trFrc),m12=fltrStatEv(12,trFrc);
	ADiap d2(m12[2].diap.getEnd(),m11[2].diap.getOrig()),d4(m12[4].diap.getEnd(),m11[4].diap.getOrig());
	auto pvx=tpl.getPVX(nullptr,SttMove::Type::TIME_PASS);
	auto stint=tpl.getInterval();

	CPPUNIT_ASSERT(d2.intersect(d4) && " ������������ �� ��������");
	CPPUNIT_ASSERT(d2.enclose(d4.getEnd()));
	CPPUNIT_ASSERT(d2.getEnd()-d4.getEnd()-stint.Ipk.count()< AAdmin::GapStop());
	CPPUNIT_ASSERT(d2.getEnd()>=d4.getEnd()+stint.Ipk.count());
	CPPUNIT_ASSERT(d2.getOrig()-d4.getOrig()-stint.Iot.count()< AAdmin::GapStop());
	CPPUNIT_ASSERT(d2.getOrig()>=d4.getOrig()+stint.Iot.count());
	CPPUNIT_ASSERT(d4.getOrig()>tWnd.or+tWnd.count);
	CPPUNIT_ASSERT(d4.duration()==pvx.full+pvx.start);
	CPPUNIT_ASSERT(d4.getOrig()-tWnd.or-tWnd.count< AAdmin::GapStop());
	CPPUNIT_ASSERT(m11[4].getType()==AEvent::MOVE && m11[2].getType()==AEvent::MOVE);
}
 
