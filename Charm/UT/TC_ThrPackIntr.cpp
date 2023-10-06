/*!
	created:	2019/02/08
	created: 	20:53  08 ������� 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrPackIntr.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrPackIntr
	file ext:	cpp
	author:		 Dremin
	
	purpose:	��������  ��������  
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Hem/RouteIntrusion.h"
#include "../Augur/TxtComment.h"
using namespace std;

void TC_AThread::testBanIntrusion()
{//������ ��� ������� �� ������
	TRACE("\r\n =========== testBanIntrusion ============");
	const std::string intrusTestStr = 
		"<ArtificalRouteIntrusions>"\
		"<RouteIntrusion>"\
		"<TrainDescr index=\"1-1-1\" num=\"1114\" >"\
		"</TrainDescr>"\
		"<Station esrCode=\"15\">"\
		"<OrderPriority>"\
		"<YieldFor>"\
		"<TrainDescr index=\"1-1-2\" num=\"1109\" >"\
		"</TrainDescr>"\
		"</YieldFor>"\
		"</OrderPriority>"\
		"</Station>"\
		"</RouteIntrusion>"
		"</ArtificalRouteIntrusions>";
	auto routeIntensionsPtr = deserialize<std::vector<RouteIntrusion>>( intrusTestStr );

	Sevent sEv1(AEvent::REAL ,70/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv9(AEvent::REAL ,15/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv44(AEvent::REAL ,170/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv46(AEvent::REAL ,200/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv14(AEvent::REAL ,10/*������ ����*/,40/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn9( TrainDescr( L"1109", L"1-1-2"),sEv9, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn44( TrainDescr( L"1044", L"1-1-1"),sEv44, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn46( TrainDescr( L"1046", L"1-1-1"),sEv46, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn14( TrainDescr( L"1114", L"1-1-1"),sEv14, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,3, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(5.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(10,&defTimes);
	{// � ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=10; 
		TestAAdmin::deltaWait=10;//����� ��� ������� ��� ���������� ������ 
		TestAAdmin::deltaIntrusion=10;
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		auto t14=tpl.add(trn14);
		auto t44=tpl.add(trn44);
		auto t9=tpl.add(trn9);
		auto t46=tpl.add(trn46);
		CPPUNIT_ASSERT(t14->equPrior(t44->descr())==ATrain::EQU_PR && t14->equPrior(t9->descr())==ATrain::EQU_PR && t14->equPrior(t46->descr())==ATrain::EQU_PR);
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(t46);//�� ��������
		adm.add(t44);//�� ��������
		adm.add(t14,routeIntensionsPtr);//�� ��������
		adm.add(t9);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
//		CPPUNIT_ASSERT(v.size()==1 && (v[0]->Comment()==ERASE_INTRUSION || v[0]->Comment()==NO_REALIZED_INTRUSION));
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==5);
		M_INT_EV m15= fltrStatEv(15,trFrc),m14= fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14.size()==5 && m15.size()==5);
		CPPUNIT_ASSERT(  m14[1109].diap.enclose(m14[1].diap.getOrig()) )  ;
		CPPUNIT_ASSERT(   m14[1109].diap.getEnd()< m14[1114].diap.getOrig()+10)   ;
		CPPUNIT_ASSERT(  m15[1114].diap.getEnd()>m15[1].diap.getOrig());
		CPPUNIT_ASSERT(  m15[1114].diap.getEnd()<=m15[1109].diap.getOrig()+10+ AAdmin::GapStop());
		CPPUNIT_ASSERT(  m15[1109].diap.getOrig()<m15[1044].diap.getEnd());
		CPPUNIT_ASSERT(  m15[1109].diap.getEnd()>m15[1046].diap.getOrig());
		CPPUNIT_ASSERT(  m15[1109].diap.getEnd()<m15[1046].diap.getOrig()+10);
	}
}

void TC_AThread::testIntrusionPackNoBan()
{//������ ��� ������� �� ������
	TRACE("\r\n =========== testIntrusionPackNoBan ============");
	const std::string intrusTestStr = 
		"<ArtificalRouteIntrusions>"\
		"<RouteIntrusion>"\
		"<TrainDescr index=\"1-1-1\" num=\"1114\" >"\
		"</TrainDescr>"\
		"<Station esrCode=\"15\">"\
		"<OrderPriority>"\
		"<YieldFor>"\
		"<TrainDescr index=\"1-1-2\" num=\"1109\" >"\
		"</TrainDescr>"\
		"</YieldFor>"\
		"</OrderPriority>"\
		"</Station>"\
		"</RouteIntrusion>"
		"</ArtificalRouteIntrusions>";
	auto routeIntensionsPtr = deserialize<std::vector<RouteIntrusion>>( intrusTestStr );

	Sevent sEv1(AEvent::REAL ,70/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv9(AEvent::REAL ,15/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv44(AEvent::REAL ,170/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv46(AEvent::REAL ,200/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv14(AEvent::REAL ,10/*������ ����*/,40/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn9( TrainDescr( L"1109", L"1-1-2"),sEv9, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn44( TrainDescr( L"1044", L"1-1-1"),sEv44, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn46( TrainDescr( L"1046", L"1-1-1"),sEv46, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn14( TrainDescr( L"1114", L"1-1-1"),sEv14, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,3+1, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(5.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(10,&defTimes);
	{// � ��������� � ������������� ��������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=20; 
		TestAAdmin::deltaWait=10;//����� ��� ������� ��� ���������� ������ 
		TestAAdmin::deltaIntrusion=100;
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::useBackAdjust=true;
		auto t14=tpl.add(trn14);
		auto t44=tpl.add(trn44);
		auto t9=tpl.add(trn9);
		auto t46=tpl.add(trn46);
		CPPUNIT_ASSERT(t14->equPrior(t44->descr())==ATrain::EQU_PR && t14->equPrior(t9->descr())==ATrain::EQU_PR && t14->equPrior(t46->descr())==ATrain::EQU_PR);
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(t46);//�� ��������
		adm.add(t44);//�� ��������
		adm.add(t14,routeIntensionsPtr);//�� ��������
		adm.add(t9);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==5);
		M_INT_EV m15= fltrStatEv(15,trFrc),m14= fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14.size()==5 && m15.size()==5);
		CPPUNIT_ASSERT(  m14[1109].diap.enclose(m14[1].diap.getOrig()) )  ;
		CPPUNIT_ASSERT(   m14[1109].diap.getEnd()< m14[1114].diap.getOrig()+10)   ;
		CPPUNIT_ASSERT(  m15[1114].diap.getEnd()>m15[1].diap.getOrig());
		CPPUNIT_ASSERT(  m15[1109].diap.getOrig()<m15[1044].diap.getEnd());
		CPPUNIT_ASSERT(  m15[1109].diap.getEnd()>m15[1046].diap.getOrig());
		CPPUNIT_ASSERT(  m15[1109].diap.getEnd()<m15[1046].diap.getOrig()+10);
	}
	{// ��� ��������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=1; 
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn46));//�� ��������
		adm.add(tpl.add(trn44));//�� ��������
		adm.add(tpl.add(trn14));//�� ��������
		adm.add(tpl.add(trn9));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==5);
		M_INT_EV m15= fltrStatEv(15,trFrc),m14= fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14.size()==5 && m15.size()==5);
		CPPUNIT_ASSERT(  m15[1109].getType()==AEvent::STOP_START&& m15[1044].getType()==AEvent::MOVE && m15[1044].getType()==AEvent::MOVE && m15[1114].diap.duration()>70);
		CPPUNIT_ASSERT(  m15[1114].diap.getEnd()>m15[1].diap.getOrig() && m15[1109].diap.getOrig()<m15[1044].diap.getOrig() && m15[1109].diap.getEnd()>m15[1046].diap.getOrig())   ;
		CPPUNIT_ASSERT(  m14[1109].diap.getOrig()<m14[1].diap.getOrig() && m14[1109].diap.getEnd()>m14[1114].diap.getOrig())   ;
	}
	{// � ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=10; 
		TestAAdmin::deltaIntrusion=10;//����� ��� ������� ��� ���������� ������ 
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		auto t14=tpl.add(trn14);
		auto t44=tpl.add(trn44);
		auto t9=tpl.add(trn9);
		auto t46=tpl.add(trn46);
		CPPUNIT_ASSERT(t14->equPrior(t44->descr())==ATrain::EQU_PR && t14->equPrior(t9->descr())==ATrain::EQU_PR && t14->equPrior(t46->descr())==ATrain::EQU_PR);
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(t46);//�� ��������
		adm.add(t44);//�� ��������
		adm.add(t14,routeIntensionsPtr);//�� ��������
		adm.add(t9);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==5);
		M_INT_EV m15= fltrStatEv(15,trFrc),m14= fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14.size()==5 && m15.size()==5);
		CPPUNIT_ASSERT(  m14[1109].diap.enclose(m14[1].diap))   ;
		CPPUNIT_ASSERT(  m15[1109].diap.getOrig()<m15[1114].diap.getEnd());
		CPPUNIT_ASSERT(  m15[1109].diap.getOrig()+TestAAdmin::deltaWait+ defTimes.tsk.count() >m15[1114].diap.getEnd());
		CPPUNIT_ASSERT(  m15[1114].diap.getEnd()<m15[1044].diap.getEnd());
// 		CPPUNIT_ASSERT(  m15[1044].diap.getEnd()<m15[1046].diap.getEnd());
	}
}


void TC_AThread::testIntrusionPack()
{// ������� �� ������ ������
	TRACE("\r\n =========== testIntrusionPack ============");
	Sevent sEv1(AEvent::REAL ,10/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv3(AEvent::REAL ,20/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv5(AEvent::REAL ,50/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,5/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn2( TrainDescr( L"1002", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1001", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn3( TrainDescr( L"1003", L"1-1-2"),sEv3, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn5( TrainDescr( L"1005", L"1-1-2"),sEv5, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,4, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(5.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(10,&defTimes);
	const std::string intrusTestStr = 
		"<ArtificalRouteIntrusions>"\
		"<RouteIntrusion>"\
		"<TrainDescr index=\"1-1-2\" num=\"1001\" >"\
		"</TrainDescr>"\
		"<Station esrCode=\"14\">"\
		"<OrderPriority>"\
		"<YieldFor>"\
		"<TrainDescr index=\"1-1-1\" num=\"1002\" >"\
		"</TrainDescr>"\
		"</YieldFor>"\
		"</OrderPriority>"\
		"</Station>"\
		"</RouteIntrusion>"
		"</ArtificalRouteIntrusions>";
	auto routeIntensionsPtr = deserialize<std::vector<RouteIntrusion>>( intrusTestStr );
	{// ��� ��������
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;
 		ACounterInt::limit=2; 
 		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
 		adm.add(tpl.add(trn3));//�� ��������
 		adm.add(tpl.add(trn1));//�� ��������
 		adm.add(tpl.add(trn5));//�� ��������
 		adm.add(tpl.add(trn2));//�� ��������
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		auto v= adm.getClash();
 		CPPUNIT_ASSERT(v.empty());
 		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==4);
 		M_INT_EV m15= fltrStatEv(15,trFrc),m14= fltrStatEv(14,trFrc);
 		CPPUNIT_ASSERT(m14.size()==4 && m15.size()==4);
 		CPPUNIT_ASSERT(  m15[1002].getType()==AEvent::STOP_START&& m15[1001].getType()==AEvent::MOVE && m15[1003].getType()==AEvent::MOVE&& m15[1005].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT( m15[1002].diap.getOrig()<m15[1001].diap.getOrig());
 		CPPUNIT_ASSERT( m15[1002].diap.getEnd()>m15[1005].diap.getOrig());
 		CPPUNIT_ASSERT( std::count_if( m14.begin(),m14.end(),[](const INT_EV& t){ return t.second.getType()==AEvent::MOVE;})==4);
	}
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=15; 
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
 		TestAAdmin:: deltaIntrusion=10;// 3 ������ ����� ��� ������� ��� ���������� ������ 
		adm.regimLog=AAdmin::LOG_DATA_OUT;// ����� ������ ����
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������
		//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
		adm.add(tpl.add(trn5));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==4);
		M_INT_EV m15= fltrStatEv(15,trFrc),m14= fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14.size()==4 && m15.size()==4);
		CPPUNIT_ASSERT(  m14[1002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m14[1001].getType()==AEvent::STOP_START && m14[1003].getType()==AEvent::STOP_START&& m14[1005].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m14[1002].diap.getOrig()<m14[1001].diap.getEnd());
		CPPUNIT_ASSERT( m14[1001].diap.getEnd()<m14[1003].diap.getEnd() && m14[1003].diap.getEnd()<m14[1005].diap.getEnd());
		CPPUNIT_ASSERT( std::count_if( m15.begin(),m15.end(),[](const INT_EV& t){ return t.second.getType()==AEvent::MOVE;})==4);
		CPPUNIT_ASSERT( m14[1001].diap.getEnd()>m14[1002].diap.getEnd() && m14[1001].diap.getEnd()<AAdmin::DeltaWait()+m14[1002].diap.getEnd());
		CPPUNIT_ASSERT(adm.numStepFrc.get()<10);
	}
	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=15; 
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		TestAAdmin:: deltaIntrusion=10;// 3 ������ ����� ��� ������� ��� ���������� ������ 
		adm.regimLog=AAdmin::LOG_DATA_OUT;// ����� ������ ����
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� �������
		//TestAAdmin::plusPrefPath=std::chrono::seconds(3); 
 /*	attic::a_document configDoc;
 	std::stringstream arCfg;
 	arCfg<<"<Augur>"
 		<<"<Archive  path=\"e:\\hlam\\XXX\\\" name=\"xch\" depth=\"5\"   />"
 		<<"</Augur>";
 	auto r=configDoc.load(arCfg);
 	attic::a_node root=configDoc.child("Augur");
 	auto error=AArchive::parseConf(root);
 		 
 	::DeleteFile(L"e:\\hlam\\XXX\\log.txt");*/
		adm.add(tpl.add(trn5));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn1),routeIntensionsPtr);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		st_intervals_t st= getInterval(StationEsr(16),StationEsr(17),adm.getNsi()->stIntPtr);

		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==4);
		M_INT_EV m15= fltrStatEv(15,trFrc),m14= fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14.size()==4 && m15.size()==4);
		CPPUNIT_ASSERT(  m14[1002].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m14[1001].getType()==AEvent::STOP_START && m14[1003].getType()==AEvent::STOP_START&& m14[1005].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m14[1002].diap.getOrig()<m14[1001].diap.getEnd());
		CPPUNIT_ASSERT( m14[1001].diap.getEnd()<m14[1003].diap.getEnd() && m14[1003].diap.getEnd()<m14[1005].diap.getEnd());
		CPPUNIT_ASSERT( std::count_if( m15.begin(),m15.end(),[](const INT_EV& t){ return t.second.getType()==AEvent::MOVE;})==4);
		CPPUNIT_ASSERT( m14[1001].diap.getEnd()<AAdmin::DeltaWait()+m14[1002].diap.getOrig()+st.tsk.count());
		CPPUNIT_ASSERT(adm.numStepFrc.get()<10);
	}
}
 
void TC_AThread::testIntrusionNoRoot()
{// ���� �������� � ����������� �� �� �������� �������
	TRACE("\r\n =========== testIntrusionNoRoot ============");
	Sevent sEv1(AEvent::REAL ,10/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		2/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,5/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		7/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	TrainDescrPtr d2(new TrainDescr(L"1002", L"1-1-1"));
	d2->SetRelLength(7);
	Strain trn2(*d2.get(),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 

	TrainDescrPtr d1(new TrainDescr( L"1001", L"1-1-2"));
	 d1->SetRelLength( 10 );
	Strain trn1(*d1.get(),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,4, 1, SpanBreadth::ONE_WAY_SPAN ); 
	tplParams.setCapacity(11);

	tplParams.setCapacity(15,1);
	tplParams.setCapacity(14,1);
		
	TC_Atopology tpl( tplParams );

	st_intervals_t defTimes(5.f);
	defTimes.tsk=chrono::seconds(6);
	tpl.setStInt(defTimes);

	STT_Val v;
	v.set_gruz(10,40,15,15);
	tpl.setPvx(v);

	{// ��� ��������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=2; 
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
 		M_INT_EV m15= fltrStatEv(15,trFrc),m16= fltrStatEv(16,trFrc);
 		CPPUNIT_ASSERT(  m15[1002].getType()==AEvent::MOVE && m15[1001].getType()==AEvent::MOVE );
		CPPUNIT_ASSERT( m16[1002].diap.getOrig()<m16[1001].diap.getOrig());
		CPPUNIT_ASSERT( m16[1002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m16[1002].diap.duration()>120);
		CPPUNIT_ASSERT( abs(m16[1002].diap.getEnd()-m16[1001].diap.getEnd())< AAdmin::GapStop());
	}
	RouteIntrusion::PassingOrderPtr p(new RouteIntrusion::PassingOrder(d2));
	RouteIntrusion::StationIntrusion rs(p);
	map<EsrKit, RouteIntrusion::StationIntrusion> m;
	m.insert(make_pair(EsrKit(14),rs));// ����� �� 14 ������� (��� ������� �����������, �.�. ����� ����� ����)
	TestAAdmin::VRouteIntrusionPtr r(new std::vector<RouteIntrusion>(1,RouteIntrusion(*d1.get(), std::vector<EsrKit>(), m, map<EsrKit, RouteIntrusion::SpanIntrusion>() )));
	{// c ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::deltaIntrusion=15;
		adm.add(tpl.add(trn1),r);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		M_INT_EV m14= fltrStatEv(14,trFrc),m13= fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(  m14[1002].getType()==AEvent::MOVE && m14[1001].getType()==AEvent::MOVE );
		CPPUNIT_ASSERT( m13[1001].diap.getOrig()<m13[1002].diap.getOrig());
		CPPUNIT_ASSERT( m13[1001].diap.duration()>100);
		CPPUNIT_ASSERT( m13[1001].diap.enclose(m13[1002].diap));
		CPPUNIT_ASSERT( m13[1001].diap.getEnd()<m13[1002].diap.getOrig()+TestAAdmin::deltaWait);
	}
	{// ��� ��������
		defTimes.tsk=chrono::seconds(5);
		tpl.setStInt(defTimes);
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		ACounterInt::limit=2; 
		adm.maxWaitCounterPack=100 ;// ����� ������������� �������� ��� �������� ���������� ������ ��
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		M_INT_EV m15= fltrStatEv(15,trFrc),m16= fltrStatEv(16,trFrc);
		CPPUNIT_ASSERT(  m15[1002].getType()==AEvent::MOVE && m15[1001].getType()==AEvent::MOVE );
		CPPUNIT_ASSERT( m16[1002].diap.getOrig()<m16[1001].diap.getOrig());
		CPPUNIT_ASSERT( m16[1002].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT( abs(m16[1002].diap.getEnd()-m16[1001].diap.getOrig())<defTimes.tsk.count()+  AAdmin::GapStop());
//		CPPUNIT_ASSERT( abs(m16[1002].diap.getOrig()-m16[1001].diap.getEnd())<defTimes.tsk.count()+  AAdmin::GapStop());
// 		CPPUNIT_ASSERT( abs(m15[1002].diap.getEnd()-m15[1001].diap.getOrig())>=defTimes.tsk.count());
// 		CPPUNIT_ASSERT( abs(m15[1002].diap.getOrig()-m15[1001].diap.getEnd())>=defTimes.tsk.count());
	}

}