/*!
	created:	2019/07/05
	created: 	18:57  05 ���� 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrIntrusYield.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrIntrusYield
	file ext:	cpp
	author:		 Dremin
	
	purpose:	������� �� �� ��������
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Hem/RouteIntrusion.h"
using namespace std;
 
static const string intrusTestStr = 
	"<ArtificalRouteIntrusions>"\
	"<RouteIntrusion>"\
	"<TrainDescr index=\"2-100-1\" num=\"1\" >"\
	"</TrainDescr>"\
	"<Station esrCode=\"15\">"\
	"<OrderPriority>"\
	"<YieldFor>"\
	"<TrainDescr index=\"2-100-2\" num=\"2\" >"\
	"</TrainDescr>"\
	"</YieldFor>"\
	"</OrderPriority>"\
	"</Station>"\
	"<Station esrCode=\"14\">"\
	"<OrderPriority>"\
	"<YieldFor>"\
	"<TrainDescr index=\"2-100-2\" num=\"4\" >"\
	"</TrainDescr>"\
	"</YieldFor>"\
	"</OrderPriority>"\
	"</Station>"\
	"</RouteIntrusion>"
	"</ArtificalRouteIntrusions>";
 
void TC_AThread::testIntrusionYieldMany()
{//����� ������� �� ����������� ����� ������� ��
	TRACE("\r\n =========== testIntrusionYieldMany ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 3 ���� �� �������
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	10/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,30/*������ ����*/,	10/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv4(AEvent::REAL ,70/*������ ����*/,	10/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		2/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"2-100-2"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"2-100-2"),sEv4, Sevent( NO_FIXED ));
	st_intervals_t defTimes(1.f);
	defTimes.Iot= chrono::seconds(5);
	defTimes.tsk= chrono::seconds(10);
	defTimes.Ipk=chrono::seconds(15);
	auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>( intrusTestStr);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	TestAAdmin adm;
	TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
	TestAAdmin::deltaIntrusion= AAdmin::GapStop();// ����� ��� ������� ��� ���������� ������ 
	TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
	adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
	adm.add(tpl.add(trn2));//�� ��������
	adm.add(tpl.add(trn4));//�� ��������
	auto trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
	auto v=trFrc.getSpotLines();
	CPPUNIT_ASSERT(v.size()==3);
	M_INT_EV m15=fltrStatEv(15,trFrc),m14=fltrStatEv(14,trFrc);
	CPPUNIT_ASSERT(m15[4].getType()==AEvent::MOVE && m15[2].getType()==AEvent::MOVE);
	CPPUNIT_ASSERT(m14[4].getType()==AEvent::MOVE );
	CPPUNIT_ASSERT(m15[1].getType()==AEvent::STOP_START && m14[1].getType()==AEvent::STOP_START);
	CPPUNIT_ASSERT(m15[1].diap.enclose(m15[2].diap.getOrig()));
	CPPUNIT_ASSERT(abs(m15[2].diap.getEnd()+defTimes.tsk.count() -m15[1].diap.getEnd())< AAdmin::GapStop());
	CPPUNIT_ASSERT(m14[1].diap.enclose(m14[4].diap.getOrig()));
	CPPUNIT_ASSERT(abs(m14[4].diap.getEnd()+defTimes.tsk.count() -m14[1].diap.getEnd())< AAdmin::GapStop());
}

void TC_AThread::testGenerateIntrusion()
{// ��������� ������� �� ������� ��� ������������ ��������� � ������ �� �������
	TRACE("\r\n =========== testGenerateIntrusion ============");
	// ���������, 2 ���� �� �������
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
	defTimes.tsk=chrono::seconds(5);
	// 	defTimes.Iot=chrono::seconds(12);
	tpl.setPvx(60,&defTimes);
	// ����������
// �� �������
	Sevent sN1(AEvent::NONE,0/*������ ����*/,2/*�� �������*/,30/*�� ��������*/,0,3/*���.�������*/,8/*����� �������*/) ;
	Sevent sN2(AEvent::NONE,0/*������ ����*/,2/*�� �������*/,30/*�� ��������*/,0,6/*���.�������*/,5/*����� �������*/) ;
	auto norm1=tpl.norma(sN1,1);
	auto norm2=tpl.norma(sN2,2);
	//�������� ����
	Sevent sEv1(AEvent::REAL,0/*������ ����*/,1/*�� �������*/,20/*�� ��������*/,0,3/*���.�������*/,1/*����� �������*/) ;
	Sevent sEv2(AEvent::REAL,0/*������ ����*/,1/*�� �������*/,20/*�� ��������*/,0,6/*���.�������*/,1/*����� �������*/) ;
	Strain trn1( TrainDescr( L"1", L"1-1-2" ),	sEv1, Sevent( NO_FIXED));
	Strain trn2( TrainDescr( L"2", L"1-1-1" ),	sEv2, Sevent( NO_FIXED));
// �� �������
	Sevent sN3(AEvent::NONE,0/*������ ����*/,2/*�� �������*/,70/*�� ��������*/,0,5/*���.�������*/,3/*����� �������*/) ;
	Sevent sN4(AEvent::NONE,0/*������ ����*/,2/*�� �������*/,60/*�� ��������*/,0,5/*���.�������*/,3/*����� �������*/) ;
	auto norm3=tpl.norma(sN3,1);
	auto norm4=tpl.norma(sN4,2);
	//�������� ����
	Sevent sEv3(AEvent::REAL,0/*������ ����*/,10/*�� �������*/,20/*�� ��������*/,0,5/*���.�������*/,1/*����� �������*/) ;
	Sevent sEv4(AEvent::REAL,0/*������ ����*/,10/*�� �������*/,20/*�� ��������*/,0,5/*���.�������*/,1/*����� �������*/) ;
	Strain trn3( TrainDescr( L"3", L"" ),	sEv3, Sevent( NO_FIXED));
	Strain trn4( TrainDescr( L"4", L"" ),	sEv4, Sevent( NO_FIXED));

	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;//  ���������� ���������� �� ���������� �� ��� ��.������
		TestAAdmin::deltaIntrusion=10;
		adm.add(tpl.add(trn1,1));//�� ��������
		adm.add(tpl.add(trn2,2));//�� ��������
		adm.add(tpl.add(trn3,1));//�� ��������
		adm.add(tpl.add(trn4,2));//�� ��������

		adm.addNorma(TrainDescr( L"1", L""), norm1 );
		adm.addNorma(TrainDescr( L"2", L""), norm2 );
		adm.addNorma(TrainDescr( L"3", L""), norm3 );
		adm.addNorma(TrainDescr( L"4", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto m14=fltrStatEv(14,v),m15=fltrStatEv(15,v),m16=fltrStatEv(16,v);
		CPPUNIT_ASSERT(m14[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m15[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m15[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m15[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m15[4].diap.enclose(m15[1].diap.getOrig()));
		CPPUNIT_ASSERT(m15[4].diap.getEnd()<=m15[1].diap.getOrig()+defTimes.tsk.count()+AAdmin::GapStop());
		CPPUNIT_ASSERT(m15[4].diap.getEnd()>=m15[1].diap.getOrig()+defTimes.tsk.count());

		CPPUNIT_ASSERT(m16[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m16[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m16[3].getType()==AEvent::STOP_START);
		
		CPPUNIT_ASSERT(m16[2].diap.enclose(m16[1].diap.getOrig()));
		CPPUNIT_ASSERT(m16[2].diap.getEnd()<=m16[1].diap.getOrig()+defTimes.tsk.count()+AAdmin::GapStop());
		CPPUNIT_ASSERT(m16[2].diap.getEnd()>=m16[1].diap.getOrig()+defTimes.tsk.count());

		CPPUNIT_ASSERT(m16[3].diap.enclose(m16[1].diap));
		CPPUNIT_ASSERT(m16[3].diap.enclose(m16[2].diap.getEnd()));
		CPPUNIT_ASSERT(m16[3].diap.getEnd()<=m16[2].diap.getEnd()+AAdmin::GapStop());
	}
}