/*!
	created:	2019/08/29
	created: 	11:41  
 	filename: 	F:\potapTrunk\Charm\UT\T�_ThrIntrusionAjust.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	T�_ThrIntrusionAjust
	file ext:	cpp
	author:		 Dremin
	
	purpose: ������������ ��� ���������� ��������
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Hem/RouteIntrusion.h"
using namespace std;

void TC_AThread::testAdjustIntrusion()
{// ������������ ��� ���������� �������
	
	TRACE("\r\n ===========testAdjustIntrusion  ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Sevent sEv3(AEvent::REAL ,5/*������ ����*/,	10/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn3( TrainDescr( L"3", L"2-100-1"),sEv3, Sevent( NO_FIXED ));

	//������� ��������� �������� ����
	Stech tWnd( 0, 600 /*������������*/, 1, 1/*park*/, 2/*e1*/,1/*e2*/,nullptr);
	Stech tWnd2( 0, 160 /*������������*/, 1, 1/*park*/, 4/*e1*/,5/*e2*/,nullptr);
	TestPossessions p( TestAAdmin::getNsi() );
	st_intervals_t defTimes(1.f);
	defTimes.Ipk=chrono::seconds(20);
	defTimes.Iot=chrono::seconds(10);
	defTimes.Ipr=chrono::seconds(15);
	tpl.setPvx(60,&defTimes);
	p.setLimitation( tWnd, tpl );
	{// ��� ��������
 		TestAAdmin adm(p,1);
 		TestAAdmin::noSort=true;// ��������� ���������� �����
 		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������
 		adm.add(tpl.add(trn1));//�� ��������
 		adm.add(tpl.add(trn3,2));//�� ��������
 		auto trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
 		M_INT_EV m12= fltrStatEv(12,trFrc),m11= fltrStatEv(11,trFrc),m14=fltrStatEv(14,trFrc);
 		CPPUNIT_ASSERT(m14[1].getType()==AEvent::MOVE && m14[3].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(m14[1].diap.getOrig()> m14[3].diap.getEnd());
 		CPPUNIT_ASSERT(m12[1].getType()==AEvent::STOP_START && m12[3].getType()==AEvent::STOP_START);
 		CPPUNIT_ASSERT(m12[1].diap.getEnd()>= m12[3].diap.getEnd()+defTimes.Iot.count());
 		CPPUNIT_ASSERT(m12[1].diap.getEnd()< m12[3].diap.getEnd()+defTimes.Iot.count()+ AAdmin::GapStop());
 		CPPUNIT_ASSERT(m12[3].diap.getEnd()> tWnd.count);
 		CPPUNIT_ASSERT(m12[3].diap.getEnd()< tWnd.count+ AAdmin::GapStop());
 		CPPUNIT_ASSERT(m11[1].getType()==AEvent::MOVE && m11[3].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(m11[1].diap.getEnd()>= m11[3].diap.getEnd()+defTimes.Ipk.count());
 		CPPUNIT_ASSERT(m11[1].diap.getEnd()< m11[3].diap.getEnd()+defTimes.Ipk.count()+ AAdmin::GapStop());
	}
	{// � ��������� � ��� ������.�������
		map<EsrKit, RouteIntrusion::StationIntrusion> mIntrusions;
		RouteIntrusion::PassingOrderPtr passingOrderPtr( new RouteIntrusion::PassingOrder( TrainDescrPtr( new TrainDescr(trn1.tdescr) ) ) );
		mIntrusions[ EsrKit( 14 )]=RouteIntrusion::StationIntrusion( passingOrderPtr);
		RouteIntrusion rIntr(trn3.tdescr, vector<EsrKit>(), mIntrusions, map<EsrKit, RouteIntrusion::SpanIntrusion>() );
		TestAAdmin::VRouteIntrusionPtr v(new std::vector<RouteIntrusion>());
		v->push_back(rIntr);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;// ��������� ���������� �����
		adm.add(tpl.add(trn3,2),v);//�� ��������
		adm.add(tpl.add(trn1));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		M_INT_EV m12= fltrStatEv(12,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14[1].getType()==AEvent::MOVE && m14[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m14[3].diap.enclose(m14[1].diap));
		CPPUNIT_ASSERT(m12[1].getType()==AEvent::STOP_START && m12[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[1].diap.getEnd()>= m12[3].diap.getEnd()+defTimes.Iot.count());
		CPPUNIT_ASSERT(m12[1].diap.getEnd()< m12[3].diap.getEnd()+defTimes.Iot.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[3].diap.getEnd()> tWnd.count);
		CPPUNIT_ASSERT(m12[3].diap.getEnd()< tWnd.count+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[1].diap.enclose(m12[3].diap) && " �� ������.�����");
	}
	{// � ��������� � ������.�������
		p.setLimitation( tWnd2, tpl );
		map<EsrKit, RouteIntrusion::StationIntrusion> mIntrusions;
		RouteIntrusion::PassingOrderPtr passingOrderPtr( new RouteIntrusion::PassingOrder( TrainDescrPtr( new TrainDescr(trn1.tdescr) ) ) );
		mIntrusions[ EsrKit( 14 )]=RouteIntrusion::StationIntrusion( passingOrderPtr);
		RouteIntrusion rIntr(trn3.tdescr, vector<EsrKit>(), mIntrusions, map<EsrKit, RouteIntrusion::SpanIntrusion>() );
		TestAAdmin::VRouteIntrusionPtr v(new std::vector<RouteIntrusion>());
		v->push_back(rIntr);
		TestAAdmin adm(p,1);
		ACounterInt::limit=20; 
		TestAAdmin::noSort=true;// ��������� ���������� �����
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������
		adm.add(tpl.add(trn3,2),v);//�� ��������
		adm.add(tpl.add(trn1));//�� ��������
		auto trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		M_INT_EV m12= fltrStatEv(12,trFrc),m14=fltrStatEv(14,trFrc),m15=fltrStatEv(15,trFrc);

		CPPUNIT_ASSERT(m15[1].diap.getEnd()>=m15[3].diap.getEnd()+ defTimes.Iot.count());
		CPPUNIT_ASSERT(m15[3].diap.getEnd()>tWnd2.count);
		CPPUNIT_ASSERT(m15[3].diap.getEnd()<tWnd2.count + AAdmin::GapStop());

		CPPUNIT_ASSERT(m14[3].diap.enclose(m14[1].diap));
		CPPUNIT_ASSERT(m14[3].diap.getEnd()-m14[1].diap.getEnd()>=defTimes.Iot.count());
		CPPUNIT_ASSERT(m14[3].diap.getEnd()-m14[1].diap.getEnd()<=defTimes.Ipk.count()+ AAdmin::GapStop());

		CPPUNIT_ASSERT(m12[1].getType()==AEvent::STOP_START && m12[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[3].diap.getEnd()>= m12[1].diap.getEnd()+defTimes.Iot.count());
		CPPUNIT_ASSERT(m12[3].diap.getEnd()<= m12[1].diap.getEnd()+defTimes.Ipk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[1].diap.getEnd()> tWnd.count);
		CPPUNIT_ASSERT(m12[1].diap.getEnd()< tWnd.count+ AAdmin::GapStop());
	}
}
