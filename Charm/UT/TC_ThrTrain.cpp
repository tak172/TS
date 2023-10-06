/*!
	created:	2018/02/22
	created: 	18:23  22 ������� 2018
	filename: 	F:\potap\Charm\UT\TC_ThrTrain.cpp
	file path:	F:\potap\Charm\UT
	file base:	TC_ThrTrain
	file ext:	cpp
	author:		 Dremin
	
	purpose:	����� ����� ��� ��������  
  */
 
 #include "stdafx.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/AInterval.h"
#include "../Augur/AClash.h"
#include "../Augur/TxtComment.h"
#include "../Augur/AugurRequest.h"
#include "../helpful/Serialization.h"
 
void TC_AThread::testSpanFrstStp()
{// ���� ����������� �� � ��������
	TRACE("\r\n =========== testSpanFrstStp ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	st_intervals_t defTimes;
	Sevent sEv(//�������� ����
		AEvent::REAL,// ��� �������
		15,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		5,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Strain trn( TrainDescr( L"", L""),sEv, Sevent( NO_FIXED ));
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn));//�� ��������
		auto t=adm.fnd(trn.tdescr);
		t->real->clear();
		AEvent ev(13,12);
		ev.diap.set(10,11);
		ev.setSource(AEvent::REAL);
		t->real->insert(t->real->begin(),ev);
		ev.diap.set(13,13);
		t->real->insert(t->real->begin(),ev);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(t->real->size()==2 );
		auto ev1=t->real->front();
		auto ev2=t->real->back();
		CPPUNIT_ASSERT(ev1.Esr()==EsrKit(12,13) && ev1.diap==ADiap(10,13) && ev2.getSource()==AEvent::FRC && ev1.getSource()==AEvent::REAL && "�� ����������� ������� �������� ");
		CPPUNIT_ASSERT(t->route.empty());
		CPPUNIT_ASSERT(trFrc.getSpotLines().front().second.empty() && "������� �� ��������");
	}
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn));//�� ��������
		auto t=adm.fnd(trn.tdescr);
		AEvent ev(13,12);
		ev.diap.set(10,12);
		t->real->insert(t->real->begin(),ev);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(t->real->front().Esr()==EsrKit(12,13) && t->real->front().diap.getOrig()==10 && t->real->front().diap.getEnd()==t->real->at(1).diap.getOrig() && "����������� ������� ���������� �������� ");
		CPPUNIT_ASSERT(t->route[0]==13 && t->route[1]==12);
	}
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn));//�� ��������
		auto t=adm.fnd(trn.tdescr);
		AEvent ev(13,12);
		ev.diap.set(10,11);
		t->real->insert(t->real->begin(),ev);
		ev.diap.set(13,13);
		t->real->insert(t->real->begin(),ev);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(t->real->front().Esr()==EsrKit(12,13) && t->real->front().diap.getOrig()==10 && t->real->front().diap.getEnd()==t->real->at(1).diap.getOrig() && "����������� ������� ���������� �������� ");
		CPPUNIT_ASSERT(t->route[0]==13 && t->route[1]==12);
	}
}

void TC_AThread::testFewEvPrepare()
{// ��������� ������� �� ��������
	Sevent sEv1(//�������� ����
		AEvent::REAL,// ��� �������
		15,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		5,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent sEv1stop(//�������� ����
		AEvent::REAL,// ��� �������
		15,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		5,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		2,// ����� �������
		7// ���������� stop � ����� �������
		);
		Sevent sEv3(//�������� ����
		AEvent::REAL,// ��� �������
		15+1,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		5,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Strain trn1( TrainDescr( L"1", L""),sEv1, Sevent( NO_FIXED ));
	Strain trn1stop( TrainDescr( L"1", L""),sEv1stop, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L""),sEv3, Sevent( NO_FIXED ));
	TC_AtopologyParams tplParams( stationsCount, 3, 2, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn3));
		// ���������  ����� ��� ������� ���� ��������(����������� � �������� �� �������)
		V_TRN_PTR vTr= adm.prepareTrains();
		CPPUNIT_ASSERT(vTr.size()==2);
		CPPUNIT_ASSERT(vTr.at(0)->getNum()==1 && vTr.at(1)->getNum()==3);
	}
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1stop));//�� ��������
		adm.add(tpl.add(trn3));
		// ���������  ����� ��� ������� ���� ��������(����������� � �������� �� �������)
		V_TRN_PTR vTr= adm.prepareTrains();
		auto t1=adm.fnd(trn1stop);
		auto t3=adm.fnd(trn3);
		CPPUNIT_ASSERT(t1 && t1->getNum()==1);
		auto evStop=t1->evFrc(-1,false);
		auto evM=t1->evFrc(-2,false);
		auto ev3=t3->evFrc(-1,false);
		CPPUNIT_ASSERT(evStop && ev3 && evM && evStop->Esr()==evM->Esr() && evStop->Esr()==ev3->Esr());
		CPPUNIT_ASSERT(evM->getType()==AEvent::MOVE && evStop->getType()==AEvent::STOP);
		CPPUNIT_ASSERT(evStop->diap.getOrig()==evM->diap.getEnd() && evStop->diap.duration()==7);
		CPPUNIT_ASSERT(evStop->diap.getOrig()>ev3->diap.getOrig() && evM->diap.getOrig()<ev3->diap.getOrig());
		CPPUNIT_ASSERT(vTr.size()==2);
		CPPUNIT_ASSERT(vTr.at(0)->getNum()==1 && vTr.at(1)->getNum()==3);
	}
}

void TC_AThread::testFirstStepConfl()
{//���� ������ ��� �������� c ���������� �� ��������
	TRACE("\r\n =========== testFirstStepConfl ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(float(1.));
	defTimes.Top=std::chrono::seconds(100);/*����������� �������� ���������������� ����������� � ��������� ��������, ��� �������, ��� ������� �� �������� ���� �������������� ��� ���������*/
	defTimes.Ipr/*�������� ����� �������� �� �������� �� ������� ��� �� */=std::chrono::seconds(12);
	defTimes.Ipk=std::chrono::seconds(20);// ����� � ������

	tpl.setPvx(20,&defTimes);
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		7,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent sEv4(//�������� ����
		AEvent::REAL,// ��� �������
		7,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		3,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Strain trn2( TrainDescr( L"1002", L"1-1-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv4, Sevent( NO_FIXED ));
	
	{
		TestAAdmin::useAdjustFirstStepForecast=false;// ������������ ��������� ����������� �� ������ ���� ����� ������� ��������
		TestAAdmin adm;
		adm.add(tpl.add(trn4));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����");
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2 && "������� �� ����� ��");
		auto ptrn4=adm.fnd(trn4.tdescr);
		auto ptrn1002=adm.fnd(trn2.tdescr);
		auto ev4=ptrn4->fndEv(EsrKit(12));
		auto ev1002=ptrn1002->fndEv(EsrKit(12));
		CPPUNIT_ASSERT(defTimes.Ipk.count()+  AAdmin::GapStop()==(ev4.diap.getOrig()-ev1002.diap.getOrig()) && "����� � ������" );
		CPPUNIT_ASSERT(ev4.diap.getEnd()<ev1002.diap.getEnd() && " ��4 �������� 1002" );
	}
	{
		TestAAdmin::useAdjustFirstStepForecast=true;// ������������ ��������� ����������� �� ������ ���� ����� ������� ��������
		TestAAdmin adm;
		adm.add(tpl.add(trn4));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����");
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2 && "������� �� ����� ��");
		auto ptrn4=adm.fnd(trn4.tdescr);
		auto ptrn1002=adm.fnd(trn2.tdescr);
		auto ev4=ptrn4->fndEv(EsrKit(12));
		auto ev1002=ptrn1002->fndEv(EsrKit(12));
		CPPUNIT_ASSERT(defTimes.Ipk.count()>(ev4.diap.getOrig()-ev1002.diap.getOrig()) && "����� � ������" );
		CPPUNIT_ASSERT(ev4.diap.getEnd()<ev1002.diap.getEnd() && " ��4 �������� 1002" );
	}
}

void TC_AThread::testTrOnSt()
{//  ������ ��� �������� ��� �� �� ������� � ���������� ��������� � �������� ����
	TRACE("\r\n =========== testFirstStepConfl ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	{
	Sevent sEv(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		10,// ����� �� ��������� ��������
		7, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent sEv1(//�������� ����
		AEvent::REAL,// ��� �������
		25,// ������ ����
		30, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		2// ����� �������
		);

		Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));
		Strain trn( TrainDescr( L"1002", L""),sEv, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn1,1));//�� ��������
		adm.add(tpl.add(trn,1));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto cls=adm.getClash();
		 const wchar_t* tbl[]={ ATTEMPT_MODIFY_PREV,NO_PATH_ON_STATION/*,INVALID_ACT_OBJ*/};
		size_t s=0,n=0;
		for (const auto& t: tbl)
		{
			auto k=std::count_if(cls.begin(),cls.end(),[&t](P_CLSH& c){ return c->Comment().find(t)==0;});
			s+=k;
			if(k)
				n++;
		}
		CPPUNIT_ASSERT(s==n && n==size_array(tbl) && "��������� ��� ����������");
	}
	{
		Sevent sEv(//�������� ����
			AEvent::REAL,// ��� �������
			5,// ������ ����
			10, // ����� �� �������
			20,// ����� �� ��������
			10,// ����� �� ��������� ��������
			0, // ���.������� (+10 � ������������)
			2// ����� �������
			);
		Sevent sEv1(//�������� ����
			AEvent::REAL,// ��� �������
			95,// ������ ����
			10, // ����� �� �������
			20,// ����� �� ��������
			0,// ����� �� ��������� ��������
			6, // ���.������� (+10 � ������������)
			2// ����� �������
			);
		Strain trn1( TrainDescr( L"1", L""),sEv1, Sevent( NO_FIXED ));
		Strain trn( TrainDescr( L"1002", L""),sEv, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
// 		tstTRACE(adm);
		auto cls=adm.getClash();
		CPPUNIT_ASSERT(cls.size()==1 && cls.front()->Comment()==NO_DESCR_PATH_ON_SPAN );
		auto ptrn=adm.fnd(trn.tdescr);
		auto ev=ptrn->fndEv(EsrKit(99999));
		auto ev1=ptrn->fndEv(EsrKit(10,99999));
		CPPUNIT_ASSERT(ev.empty() && !ev1.empty() && "������� �� ��������");
	}
}

void TC_AThread::testDrawThread()
{// #4809 	��������������� ������������ �����
	typedef AugurRequest<AEvent, AEvent> AugurRequestOfAEvents;
//		"<!-- �������� �������� ���  ���������  Death  -->"\

	static std::string requestContent="<AugurRequest>"\
		"<LiveMoment value=\"19700101T010000Z\" />"\
		"<HappenLayer>"\
		"<HemPath>"\
		"<SpotEvent create_time=\"19700101T005000\" name=\"Info_changing\">"\
		"<TrainDescr index=\"1-052-2\" num=\"2252\" length=\"1\" weight=\"5436\" net_weight=\"3790\" through=\"Y\">"\
		"<feat_texts typeinfo=\"�\" />"\
		"<Locomotive Series=\"2��116\" NumLoc=\"559\" Consec=\"1\" CarrierCode=\"3\">"\
		"<Crew EngineDriver=\"VOROBJOVS\" Tim_Beg=\"2018-06-27 10:25\" />"\
		"</Locomotive>"\
		"</TrainDescr>"\
		"</SpotEvent>"\
		"<SpotEvent create_time=\"19700101T003000\" name=\"ExplicitForm\" Bdg=\"ASOUP 1042[11]\" waynum=\"0\" parknum=\"0\" />"\
		"<SpotEvent create_time=\"19700101T003300\" name=\"Departure\" Bdg=\"6-8SP:6+[11]\" waynum=\"0\" parknum=\"0\" />"\
		"<SpotEvent create_time=\"19700101T003500\" name=\"Transition\" Bdg=\"701SP[12]\" waynum=\"0\" parknum=\"0\" />"\
		"<SpotEvent create_time=\"19700101T003900\" name=\"Transition\" Bdg=\"1C[13]\" waynum=\"1\" parknum=\"1\" />"\
		"<SpotEvent  create_time=\"19700101T004400\" name=\"Death\" Bdg=\"2-4SP:2+,4+[14]\" waynum=\"0\" parknum=\"0\" />"\
		"<!-- ���������  Death  -->"\
		"</HemPath>"\
		"</HappenLayer>"\
		"<ScheduledLayer>"\
		"</ScheduledLayer>"\
		"</AugurRequest>";
	std::shared_ptr<AugurRequestOfAEvents>requestPtr = deserialize<AugurRequestOfAEvents>( requestContent );
	CPPUNIT_ASSERT(requestPtr->inputPathsPtr->getSpotLines().size()==1);
	auto p_d_v=requestPtr->inputPathsPtr->getSpotLines().front();
	V_EVNT v=p_d_v.second;
	CPPUNIT_ASSERT(v.size()==4);
	CPPUNIT_ASSERT(v[0].Esr()==EsrKit(11) && v[1].Esr()==EsrKit(12) &&v[2].Esr()==EsrKit(13) &&v[3].Esr()==EsrKit(14)  );
	CPPUNIT_ASSERT(v[0].getSource()==AEvent::REAL && v[1].getSource()==AEvent::REAL &&v[2].getSource()==AEvent::REAL &&v[3].getSource()==AEvent::REAL  );
	CPPUNIT_ASSERT(v[0].getType()==AEvent::STOP_START && v[1].getType()==AEvent::MOVE && v[2].getType()==AEvent::MOVE &&v[3].getType()==AEvent::DEATH  );
	CPPUNIT_ASSERT(requestPtr->liveMoment==1*3600);
	CPPUNIT_ASSERT(v[0].diap==ADiap(30*60,33*60) &&v[3].diap==ADiap(44*60) );

	TRACE("\r\n =========== testDrawThread ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	AThread thr(p_d_v.second);
	ATrainPtr trn(new ATrain(p_d_v.first,&thr,nullptr) );//�� ��� 
	TestAAdmin adm;
	adm.add(trn);//�� ��������
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����");
	CPPUNIT_ASSERT(trFrc.getSpotLines().size()==1 );
	auto vF=trFrc.getSpotLines().front().second;
	CPPUNIT_ASSERT(vF.size()==13);
	CPPUNIT_ASSERT(vF.front().Esr()==EsrKit(14) && vF.front().diap==ADiap(44*60,44*60+1));
	CPPUNIT_ASSERT( vF.front().diap.getEnd()< requestPtr->liveMoment);
	CPPUNIT_ASSERT(vF.front().getType()==AEvent::MOVE);
}


void TC_AThread::testAdvantageBrutto()
{// ���� �� ������������ �������� ��������
	TRACE("\r\n =========== testAdvantageBrutto ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.tsk=std::chrono::seconds(10);
	tpl.setPvx(40,&defTimes);
	Sevent sEv3(	AEvent::REAL,5/*������ ����*/,4/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv4(	AEvent::REAL,5/*������ ����*/,4/*����� �� �������*/,	1/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Strain trn3( TrainDescr( L"2003", L"1-200-2"),sEv3,Sevent(NO_FIXED));
	ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::BOTH);
	Strain trn4( TrainDescr( L"2004", L"2-200-1"),sEv4,Sevent(NO_FIXED));
	ATrainPtr tr4=tpl.add(trn4,2,TC_Atopology::BOTH);
	TestPossessions p( TestAAdmin::getNsi() );
	{// �� ������ ���������
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::useBackAdjust=true;
		ACounterInt::limit=8;//���������� ����������
		adm.add(tr4);//�� ��������
		adm.add(tr3);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[2004].getType()==AEvent::MOVE && m12[2004].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2003].getType()==AEvent::MOVE && m12[2003].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[2003].diap.getEnd()> m12[2004].diap.getOrig());
		CPPUNIT_ASSERT(abs(m12[2003].diap.getEnd()- m12[2004].diap.getOrig()-defTimes.tsk.count())< AAdmin::GapStop());
	}
	{// ������ ��������
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		ACounterInt::limit=8;//���������� ����������
		adm.add(tr4);//�� ��������
		adm.add(tr3);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[2004].getType()==AEvent::STOP_START && m12[2004].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2003].getType()==AEvent::MOVE && m12[2003].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2004].diap.enclose(m13[2003].diap));
		CPPUNIT_ASSERT(m13[2004].diap.getEnd()-m13[2003].diap.getEnd()<25);
	}
	{// ����� ��� � ��������  �� ��������
		trn4.tdescr.SetBrutto( AAdmin::Advantage_brutto()+1); 
		ATrainPtr tr4=tpl.add(trn4,1,TC_Atopology::BOTH);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		ACounterInt::limit=8;//���������� ����������
		adm.add(tr4);//�� ��������
		adm.add(tr3);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[2004].getType()==AEvent::MOVE && m12[2004].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2003].getType()==AEvent::MOVE && m12[2003].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[2003].diap.getEnd()> m12[2004].diap.getOrig());
		CPPUNIT_ASSERT(abs(m12[2003].diap.getEnd()- m12[2004].diap.getOrig()-defTimes.tsk.count())< AAdmin::GapStop());
	}
	{// ��� ��������� ������� �� ���������� ��������
		sEv3.or=130;
		Strain trn3( TrainDescr( L"2003", L"1-200-2"),sEv3,Sevent(NO_FIXED));
		//trn3.tdescr.SetBrutto( AAdmin::Advantage_brutto()+1); 
		ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::BOTH);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		ACounterInt::limit=8;//���������� ����������
		adm.add(tr4);//�� ��������
		adm.add(tr3);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[2004].getType()==AEvent::MOVE && m12[2004].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2003].getType()==AEvent::MOVE && m12[2003].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[2003].diap.getEnd()> m12[2004].diap.getOrig());
		CPPUNIT_ASSERT(abs(m12[2003].diap.getEnd()- m12[2004].diap.getOrig()-defTimes.tsk.count())< AAdmin::GapStop());
	}
	{// ��� ��������� ������� � �������� ���������� � �������� ��������
		sEv3.or=130;
		Strain trn3( TrainDescr( L"2003", L"1-200-2"),sEv3,Sevent(NO_FIXED));
		trn3.tdescr.SetBrutto( AAdmin::Advantage_brutto()+1); 
		ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::BOTH);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::maxWaitCounterPack=300;
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		ACounterInt::limit=8;//���������� ����������
		adm.add(tr4);//�� ��������
		adm.add(tr3);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[2004].getType()==AEvent::STOP_START && m12[2004].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2003].getType()==AEvent::MOVE && m12[2003].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2004].diap.enclose(m13[2003].diap));
		CPPUNIT_ASSERT(m13[2004].diap.getEnd()-m13[2003].diap.getEnd()<25);
	}
	{// �������� ����� ������� ����� �������� ��������� � ��������� �������
		// ������ ��� ��������� ������� � �������� �� ���������� ��������
		sEv3.or=130;
		Strain trn3( TrainDescr( L"2003", L"1-200-2"),sEv3,Sevent(NO_FIXED));
		trn3.tdescr.SetBrutto( AAdmin::Advantage_brutto()+1); 
		ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::BOTH);
		TestAAdmin adm(p,1);
		TestAAdmin::noSort=true;//----------- ��� �������� �������--------------
		TestAAdmin::useBackAdjust=true;
		TestAAdmin::diffAdvBrutto=40;// �������� ����� ������� ����� �������� ��������� � ��������� �������
		TestAAdmin::maxWaitCounterPack=300;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		ACounterInt::limit=8;//���������� ����������
		adm.add(tr4);//�� ��������
		adm.add(tr3);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[2004].getType()==AEvent::MOVE && m12[2004].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m13[2003].getType()==AEvent::MOVE && m12[2003].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m12[2003].diap.getEnd()> m12[2004].diap.getOrig());
		CPPUNIT_ASSERT(abs(m12[2003].diap.getEnd()- m12[2004].diap.getOrig()-defTimes.tsk.count())< AAdmin::GapStop());
	}
}