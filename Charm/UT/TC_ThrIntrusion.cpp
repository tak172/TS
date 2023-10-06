/*!
	created:	2018/12/05
	created: 	19:56  05 ������� 2018
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrIntrusion.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrIntrusion
	file ext:	cpp
	author:		 Dremin
	
	purpose:	
  
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Hem/RouteIntrusion.h"
using namespace std;

void TC_AThread::testUpOrderIntrusion()
{// ���� ������������� �� �������
	TRACE("\r\n =========== testUpOrderIntrusion ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60);
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Sevent sEv3(AEvent::REAL ,15/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn3( TrainDescr( L"3", L"2-100-1"),sEv3, Sevent( NO_FIXED ));

	//������� ��������� �������� ����
	Stech tWnd( 1, 310 /*������������*/, 1, 1/*park*/, 2/*e1*/,1/*e2*/,nullptr);
	tWnd.kmLen=.5;
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
	TestAAdmin adm(p);
	adm.add(tpl.add(trn1));//�� ��������
	adm.add(tpl.add(trn3));//�� ��������
	auto trFrc0= adm.forecastGlobal();
	tstTRACE(adm);

}

void TC_AThread::testFiltrPassEquPr()
{// ������ ������� �������� ����������������

	TRACE("\r\n =========== testFiltrPassEquPr ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60);
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn1( TrainDescr( L"1001", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Sevent sEv3(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn3( TrainDescr( L"1003", L"2-100-1"),sEv3, Sevent( NO_FIXED ));

	//������� ��������� �������� ����
	Stech tWnd( 1, 600 /*������������*/, 1, 1/*park*/, 2/*e1*/,1/*e2*/,nullptr);
	tWnd.kmLen=.5;
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
	{
		TestAAdmin adm(p);
		TestAAdmin::noSort=true;// ��������� ���������� �����
		TestAAdmin::fnFltr.clear();// �-���  ������ ������� �� �������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto v=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(v.size()==2);
		auto it1=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
		auto it3=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn3.tdescr;});
		CPPUNIT_ASSERT(it1!=v.end() && it3!=v.end());
		auto ev1= std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(12);});
		auto ev3= std::find_if(it3->second.begin(),it3->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(12);});
		CPPUNIT_ASSERT(ev1!=it1->second.end() && ev3!=it3->second.end());
		CPPUNIT_ASSERT(ev3->diap.getOrig()< ev1->diap.getOrig() && ev3->diap.getEnd()< ev1->diap.getEnd() && "����� ������������");
	}
	{
		TestAAdmin adm(p);
		TestAAdmin::noSort=true;// ��������� ���������� �����
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto v=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(v.size()==2);
		auto it1=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
		auto it3=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn3.tdescr;});
		CPPUNIT_ASSERT(it1!=v.end() && it3!=v.end());
		auto ev1= std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(12);});
		auto ev3= std::find_if(it3->second.begin(),it3->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(12);});
		CPPUNIT_ASSERT(ev1!=it1->second.end() && ev3!=it3->second.end());
		CPPUNIT_ASSERT(ev3->diap.getOrig()< ev1->diap.getOrig() && ev1->diap.getEnd()< ev3->diap.getEnd() && "����� ��������� �����������������");
	}
}

static const string intrusTestStr1_3 = 
"<ArtificalRouteIntrusions>"\
	"<RouteIntrusion>"\
		"<TrainDescr index=\"2-100-1\" num=\"1003\" >"\
		"</TrainDescr>"\
		"<Station esrCode=\"14\">"\
		"<OrderPriority>"\
		"<YieldFor>"\
		"<TrainDescr index=\"2-100-1\" num=\"1001\" >"\
		"</TrainDescr>"\
		"</YieldFor>"\
		"</OrderPriority>"\
	"</Station>"\
	"</RouteIntrusion>"
		"<RouteIntrusion>"\
		"<TrainDescr index=\"2-100-1\" num=\"1001\" >"\
		"</TrainDescr>"\
		"<Station esrCode=\"12\">"\
		"<OrderPriority>"\
		"<YieldFor>"\
		"<TrainDescr index=\"2-100-1\" num=\"1003\" >"\
		"</TrainDescr>"\
		"</YieldFor>"\
		"</OrderPriority>"\
		"</Station>"\
	"</RouteIntrusion>"
"</ArtificalRouteIntrusions>";

static const string intrusTestStr3 = 
	"<ArtificalRouteIntrusions>"\
	"<RouteIntrusion>"\
	"<TrainDescr index=\"2-100-1\" num=\"1003\" >"\
	"</TrainDescr>"\
	"<Station esrCode=\"14\">"\
	"<OrderPriority>"\
	"<YieldFor>"\
	"<TrainDescr index=\"2-100-1\" num=\"1001\" >"\
	"</TrainDescr>"\
	"</YieldFor>"\
	"</OrderPriority>"\
	"</Station>"\
	"</RouteIntrusion>"
	"</ArtificalRouteIntrusions>";

void TC_AThread::testIntrusionShouldYieldForOtherPass()
{// ������� �� ����������� ����� ������� ��
	TRACE("\r\n =========== testIntrusionShouldYieldForOther ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60);
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn1( TrainDescr( L"1001", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Sevent sEv3(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn3( TrainDescr( L"1003", L"2-100-1"),sEv3, Sevent( NO_FIXED ));
	{
		auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>( intrusTestStr1_3 );
		TestAAdmin adm;
		//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
		adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
		adm.add(tpl.add(trn3),routeIntensionsPtr);//�� ��������
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto v=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(v.size()==2);
		auto it1=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
		auto it3=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn3.tdescr;});
		CPPUNIT_ASSERT(it1!=v.end() && it3!=v.end());
		auto ev1= std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
		auto ev3= std::find_if(it3->second.begin(),it3->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
		CPPUNIT_ASSERT(ev1!=it1->second.end() && ev3!=it3->second.end());
		CPPUNIT_ASSERT(ev3->diap.getOrig()< ev1->diap.getOrig() && ev1->diap.getEnd()< ev3->diap.getEnd() && "����� ��������� �����������������");
		auto ev1_12= std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(12);});
		auto ev3_12= std::find_if(it3->second.begin(),it3->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(12);});
		CPPUNIT_ASSERT(ev1_12!=it1->second.end() && ev3_12!=it3->second.end());
		CPPUNIT_ASSERT(ev1_12->diap.getOrig()< ev3_12->diap.getOrig() && ev1_12->diap.getEnd()> ev3_12->diap.getEnd() && "����� ��������� �����������������");
		CPPUNIT_ASSERT(std::count_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==1);
		CPPUNIT_ASSERT(std::count_if(it3->second.begin(),it3->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==1);
	}
	{
		auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>( intrusTestStr3 );
		TestAAdmin adm;
		//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
		adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
		adm.add(tpl.add(trn3),routeIntensionsPtr);//�� ��������
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto v=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(v.size()==2);
		auto it1=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
		auto it3=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn3.tdescr;});
		CPPUNIT_ASSERT(it1!=v.end() && it3!=v.end());
		auto ev1= std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
		auto ev3= std::find_if(it3->second.begin(),it3->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
		CPPUNIT_ASSERT(ev1!=it1->second.end() && ev3!=it3->second.end());
		CPPUNIT_ASSERT(ev3->diap.getOrig()< ev1->diap.getOrig() && ev1->diap.getEnd()< ev3->diap.getEnd() && "����� ��������� �����������������");
		CPPUNIT_ASSERT(std::count_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==0);
		CPPUNIT_ASSERT(std::count_if(it3->second.begin(),it3->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==1);
	}
}

void TC_AThread::testIntrusionShouldYieldForOtherCounter()
{// ������� �� ����������� ����� ������� ��
	TRACE("\r\n =========== testIntrusionShouldYieldForOtherCounter ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60);
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn1( TrainDescr( L"1001", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Sevent sEv2(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn2( TrainDescr( L"1002", L"2-100-2"),sEv2, Sevent( NO_FIXED ));
	static const string intrusTestStr = 
		"<ArtificalRouteIntrusions>"\
		"<RouteIntrusion>"\
		"<TrainDescr index=\"2-100-1\" num=\"1001\" >"\
		"</TrainDescr>"\
		"<Station esrCode=\"14\">"\
		"<OrderPriority>"\
		"<YieldFor>"\
		"<TrainDescr index=\"2-100-2\" num=\"1002\" >"\
		"</TrainDescr>"\
		"</YieldFor>"\
		"</OrderPriority>"\
		"</Station>"\
		"</RouteIntrusion>"
		"</ArtificalRouteIntrusions>";
	auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>( intrusTestStr );
	{
		TestAAdmin adm;
		//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
		TestAAdmin::deltaIntrusion=10;
		adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
		adm.add(tpl.add(trn2),routeIntensionsPtr);//�� ��������
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto v=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(v.size()==2);
		auto it1=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
		auto it2=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn2.tdescr;});
		CPPUNIT_ASSERT(it1!=v.end() && it2!=v.end());
		auto ev1= std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
		auto ev2= std::find_if(it2->second.begin(),it2->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
		CPPUNIT_ASSERT(ev1!=it1->second.end() && ev2!=it2->second.end());
		CPPUNIT_ASSERT(ev1->diap.getOrig()< ev2->diap.getOrig() && ev2->diap.getEnd()< ev1->diap.getEnd() && "����� ���������� �����������������");
		st_intervals_t st= getInterval(StationEsr(16),StationEsr(17),adm.getNsi()->stIntPtr);
		CPPUNIT_ASSERT(ev2->diap.getOrig()+TestAAdmin::DeltaWait()+ st.tsk.count()> ev2->diap.getEnd() && " ������������� ����� ");
		CPPUNIT_ASSERT(std::count_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==1);
		CPPUNIT_ASSERT(std::count_if(it2->second.begin(),it2->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==0);
		CPPUNIT_ASSERT(adm.numStepFrc.get()==2 && " ��������� � ����� c �������� �������������");
	}
	{
		static const string intrusTestStr = 
			"<ArtificalRouteIntrusions>"\
			"<RouteIntrusion>"\
			"<TrainDescr index=\"2-100-1\" num=\"1001\" >"\
			"</TrainDescr>"\
			"<Station esrCode=\"13\">"\
			"<OrderPriority>"\
			"<YieldFor>"\
			"<TrainDescr index=\"2-100-2\" num=\"1002\" >"\
			"</TrainDescr>"\
			"</YieldFor>"\
			"</OrderPriority>"\
			"</Station>"\
			"</RouteIntrusion>"
			"</ArtificalRouteIntrusions>";

		auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>( intrusTestStr );
		{
			TestAAdmin adm;
			ACounterInt::limit=4; 
			//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
			adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
			adm.add(tpl.add(trn2),routeIntensionsPtr);//�� ��������
			TestAAdmin::deltaWait=10;
			auto trFrc0= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
			auto v=trFrc0.getSpotLines();
			CPPUNIT_ASSERT(v.size()==2);
			M_INT_EV m13= fltrStatEv(13,trFrc0);
			CPPUNIT_ASSERT(m13[1001].diap.getEnd()> m13[1002].diap.getOrig()  && "����� ���������� �����������������");
			CPPUNIT_ASSERT(m13[1001].diap.getEnd()< m13[1002].diap.getEnd()+TestAAdmin::DeltaWait()  && "����� ���������� �����������������");
		}
		{
			TestAAdmin adm;
			ACounterInt::limit=4; 
			//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
			adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
			adm.add(tpl.add(trn2),routeIntensionsPtr);//�� ��������
			TestAAdmin::deltaWait=10;
			auto trFrc0= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
			auto v=trFrc0.getSpotLines();
			CPPUNIT_ASSERT(v.size()==2);
			M_INT_EV m13= fltrStatEv(13,trFrc0);
			CPPUNIT_ASSERT(m13[1001].diap.getOrig()< m13[1002].diap.getEnd()  && "����� ���������� �����������������");
			CPPUNIT_ASSERT(m13[1001].diap.getEnd()< m13[1002].diap.getEnd()+TestAAdmin::DeltaWait()  && "����� ���������� �����������������");
		}
	}
	{
		TestAAdmin adm;
		TestAAdmin::deltaIntrusion=180;
		//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
		adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
		adm.add(tpl.add(trn2),routeIntensionsPtr);//�� ��������
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto v=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(v.size()==2);
		auto it1=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
		auto it2=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn2.tdescr;});
		CPPUNIT_ASSERT(it1!=v.end() && it2!=v.end());
		auto ev1= std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
		auto ev2= std::find_if(it2->second.begin(),it2->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
		CPPUNIT_ASSERT(ev1!=it1->second.end() && ev2!=it2->second.end());
		CPPUNIT_ASSERT(ev1->diap.getOrig()< ev2->diap.getOrig() && ev2->diap.getEnd()< ev1->diap.getEnd() && "����� ���������� �����������������");
		CPPUNIT_ASSERT(std::count_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==1);
		CPPUNIT_ASSERT(std::count_if(it2->second.begin(),it2->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==0);
		CPPUNIT_ASSERT(adm.numStepFrc.get()==1 && " ��������� � ����� ��� ��������� ������������");
	}
}

void TC_AThread::testIntrusionTransitStations()
{// ������� �� ���������� ��������
	TRACE("\r\n =========== testIntrusionTransitStations ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,3/*����� �������*/);
	Strain trn1( TrainDescr( L"1001", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	vector<EsrKit> visitingChain;
	visitingChain.emplace_back( EsrKit( 20 ) );
	visitingChain.emplace_back( EsrKit( 17 ) );
	visitingChain.emplace_back( EsrKit( 21 ) );
	map<EsrKit, RouteIntrusion::StationIntrusion> m;
	TestAAdmin::VRouteIntrusionPtr routeIntrusions(new std::vector<RouteIntrusion>());
	routeIntrusions->emplace_back( RouteIntrusion( trn1.tdescr, visitingChain, m, map<EsrKit, RouteIntrusion::SpanIntrusion>() ) );
	TestPossessions p( TestAAdmin::getNsi() );
	TestAAdmin adm(p);
	adm.add(tpl.add(trn1),routeIntrusions);//�� ��������
	auto trFrc0= adm.forecastGlobal();
	tstTRACE(adm);
	long tbl[]={16,17,18,19,20,19,18,17,18,19,20,21,20,19,18,17,16,15,14,13,12,11,10};
	LONG_VECTOR smpl(ARRAY_IT(tbl));
	auto v=trFrc0.getSpotLines();
	CPPUNIT_ASSERT(v.size()==1);
	 auto& r= v.front().second;
	 LONG_VECTOR s;
	 std::for_each(r.begin(),r.end(),[&s](const AEvent&t){if(!t.onSpan()) s.push_back(t.Esr().getTerm()) ;});
	 CPPUNIT_ASSERT(s==smpl);
}

void TC_AThread::testIntrusionCounterCorrection()
{// ������� �� ����������� ����� ������� �� c ��������������

	TRACE("\r\n =========== testIntrusionShouldYieldForOtherCounter ============");
	static int tbl[]={ 20,180};// ���������� ���������� ��� �������
	for (const auto& dtW: tbl)
	{
		TestAAdmin::deltaWait=dtW;
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN); // ���������, 3 ���� �� �������
		TC_Atopology tpl( tplParams );
		tpl.setPvx(60);
		Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
			6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
		Strain trn1( TrainDescr( L"1001", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
		Sevent sEv2(AEvent::REAL ,5/*������ ����*/,	120/*����� �� �������*/,60/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
			1/*���.������� (+10 � ������������)*/,1/*����� �������*/);
		Strain trn2( TrainDescr( L"1002", L"2-100-2"),sEv2, Sevent( NO_FIXED ));
			static const string intrusTestStr = 
				"<ArtificalRouteIntrusions>"\
				"<RouteIntrusion>"\
				"<TrainDescr index=\"2-100-1\" num=\"1001\" >"\
				"</TrainDescr>"\
				"<Station esrCode=\"14\">"\
				"<OrderPriority>"\
				"<YieldFor>"\
				"<TrainDescr index=\"2-100-2\" num=\"1002\" >"\
				"</TrainDescr>"\
				"</YieldFor>"\
				"</OrderPriority>"\
				"</Station>"\
				"</RouteIntrusion>"
				"</ArtificalRouteIntrusions>";

			//auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>("" );
			auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>( intrusTestStr );
			TestAAdmin adm;
			//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
			adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
			adm.add(tpl.add(trn2),routeIntensionsPtr);//�� ��������
			auto trFrc0= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
			if(TestAAdmin::DeltaWait()<100)
				CPPUNIT_ASSERT(adm.numStepFrc.get()==2 && "2 �������������� ���� - ������� +�������������");// ����.����� ����� �������� ��� ���������� ������� �� ���������)
			else
				CPPUNIT_ASSERT(adm.numStepFrc.get()==1 && "1 �������������� ��� - �������  ��� �������������");// ����.����� ����� �������� ��� ���������� ������� �� ���������)
			auto v=trFrc0.getSpotLines();
			CPPUNIT_ASSERT(v.size()==2);
			auto it1=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn1.tdescr;});
			auto it2=std::find_if(v.begin(),v.end(),[&](const std::pair<TrainDescr, V_EVNT>&t){return t.first==trn2.tdescr;});
			CPPUNIT_ASSERT(it1!=v.end() && it2!=v.end());
			auto ev1= std::find_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
			auto ev2= std::find_if(it2->second.begin(),it2->second.end(),[](const AEvent& t){return t.Esr()==EsrKit(14);});
			CPPUNIT_ASSERT(ev1!=it1->second.end() && ev2!=it2->second.end());
			CPPUNIT_ASSERT(ev1->diap.getOrig()< ev2->diap.getOrig() && ev2->diap.getEnd()< ev1->diap.getEnd() && "����� ���������� �����������������");
			CPPUNIT_ASSERT(std::count_if(it1->second.begin(),it1->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==1);
			CPPUNIT_ASSERT(std::count_if(it2->second.begin(),it2->second.end(),[](const AEvent& t){ return !t.onSpan() && t.diap.duration()>AAdmin::deltaThrough();})==0);
			CPPUNIT_ASSERT(  ev1->diap.getEnd()-ev2->diap.getEnd()<TestAAdmin::deltaWait && "����������� ����� �� ����������");
	}
}