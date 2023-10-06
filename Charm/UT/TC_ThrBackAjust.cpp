/*!
	created:	2019/06/04
	created: 	08:55  04 ���� 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrBackAjust.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrBackAjust
	file ext:	cpp
	author:		 Dremin
	
	purpose:	�������� ������������ ����� ���������� �������
  
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Hem/RouteIntrusion.h"
using namespace std;

void TC_AThread::testManyBackAdjust()
{ // �������������  ������������ �� � ������ � ������� �� � ������
	TRACE("\r\n =========== testManyBackAdjust ============");
	Sevent sEv1(AEvent::REAL ,10+50/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Sevent sEv3(AEvent::REAL ,190/*������ ����*/,	1/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Sevent sEv2(AEvent::REAL ,75/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		7/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	Strain trn3( TrainDescr( L"3", L"1-1-2"),sEv3, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998
	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(40,&defTimes);
    tpl.addSpanInterval( EsrKit(15,16), 80);
	{// ������ ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m5=fltrStatEv(15,trFrc),m4=fltrStatEv(14,trFrc),m6=fltrStatEv(16,trFrc);
		CPPUNIT_ASSERT(m5[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m5[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m6[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m6[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m6[2].diap.duration()>100);
		CPPUNIT_ASSERT(m6[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m4[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m4[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m4[3].diap.duration()>50);
	}
	{// ������ ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t1= adm.fnd(trn1);
		auto t2= adm.fnd(trn2);
		auto t3= adm.fnd(trn3);
		auto r=t1->Reason(15);
		CPPUNIT_ASSERT(r);
		CPPUNIT_ASSERT(r->banTrn.empty());
		CPPUNIT_ASSERT(r->first==AClaim::ShiftCnfl);
		CPPUNIT_ASSERT(r->second==t2);
		r=t2->Reason(15);
		CPPUNIT_ASSERT(r && "������� �������");
		CPPUNIT_ASSERT(r->banTrn.empty());
		CPPUNIT_ASSERT(r->first==AClaim::ShiftCnfl);
		CPPUNIT_ASSERT(r->second==t3);
		M_INT_EV m5=fltrStatEv(15,trFrc),m4=fltrStatEv(14,trFrc),m3=fltrStatEv(13,trFrc);
		CPPUNIT_ASSERT(m5[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m5[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m5[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m5[1].diap.getEnd()>=m5[2].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m4[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m4[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m4[2].diap.duration()>15);
		CPPUNIT_ASSERT(m3[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m3[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m3[3].diap.getEnd()<m3[2].diap.getOrig()+100);
	}
	{// ������ ��������� � �������������
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::useBackAdjust=true;
 		TestAAdmin::fnFltr.clear();
 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
 		adm.add(tpl.add(trn1));//�� ��������
 		adm.add(tpl.add(trn2));//�� ��������
 		adm.add(tpl.add(trn3));//�� ��������
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		M_INT_EV m5=fltrStatEv(15,trFrc),m4=fltrStatEv(14,trFrc),m3=fltrStatEv(13,trFrc);
		auto t1= adm.fnd(trn1);
		auto t2= adm.fnd(trn2);
		auto r=t1->Reason(15);
		CPPUNIT_ASSERT(r);
		CPPUNIT_ASSERT(r->banTrn.empty());
		CPPUNIT_ASSERT(r->first==AClaim::ShiftCnfl);
		CPPUNIT_ASSERT(r->second==t2);
		CPPUNIT_ASSERT(!t2->Reason(15) && "������� �����");

		CPPUNIT_ASSERT(t1->banTrain(14).empty());
		CPPUNIT_ASSERT(m5[1].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m5[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m5[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m5[1].diap.getEnd()>=m5[2].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m5[1].diap.getEnd()<=m5[2].diap.getOrig()+defTimes.tsk.count()+AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m4[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m4[3].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m3[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m3[3].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m3[3].diap.getEnd()>=m3[2].diap.getOrig()+defTimes.tsk.count());
		CPPUNIT_ASSERT(m3[3].diap.getEnd()<=m3[2].diap.getOrig()+defTimes.tsk.count()+AAdmin::deltaThrough());
	}
}

void TC_AThread::testBackAdjust()
{// ���� �������� ��������� ����� ���������
	TRACE("\r\n =========== testBackAdjust ============");
	Sevent sEv1(AEvent::REAL ,10+50/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,75/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		7/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn1( TrainDescr( L"1", L"1-1-2"),sEv1, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998

	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	{// �������� ��������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(16,trFrc);
		CPPUNIT_ASSERT(m[1].diap.duration()==AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m[2].diap.duration()>100 && " ���� �������� ");
		auto t1=adm.fnd(trn1.tdescr);
		st_intervals_t s=t1->getStInterval(16,false);// ��.��������� ��� ���������(true)/���������� (false)
		CPPUNIT_ASSERT(m[2].diap.getEnd()-m[1].diap.getOrig()==s.tsk.count());
	}
	{//  ���������� ��������� + ������� ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		ACounterInt::limit=2; 
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� ������� ���������
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(15,trFrc);
		auto t1=adm.fnd(trn1.tdescr);
		st_intervals_t s=t1->getStInterval(15,false);// ��.��������� ��� ���������(true)/���������� (false)
 		CPPUNIT_ASSERT(m[2].diap.duration()==AAdmin::deltaThrough());
 		CPPUNIT_ASSERT(m[1].diap.duration()>20 && "�������� ����� � ����� ��������� ");
		CPPUNIT_ASSERT(m[2].diap.getOrig()-m[1].diap.getOrig()>s.tsk.count());
		CPPUNIT_ASSERT(m[2].diap.getOrig()-m[1].diap.getOrig()<=s.tsk.count()+ AAdmin::GapStop());
	}
	V_EVNT v1,v2;
	{//  ���������� ��������� + ������� ��������� + �������� ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		ACounterInt::limit=3; 
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� ������� ���������
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m=fltrStatEv(15,trFrc);
		auto t1=adm.fnd(trn1.tdescr);
		auto t2=adm.fnd(trn2.tdescr);
		st_intervals_t s=t1->getStInterval(16,false);// ��.��������� ��� ���������(true)/���������� (false)
		CPPUNIT_ASSERT(m[2].diap.duration()==AAdmin::deltaThrough());
 		CPPUNIT_ASSERT(m[1].diap.duration()<20);
 		CPPUNIT_ASSERT(m[2].diap.getOrig()-m[1].diap.getOrig()>s.tsk.count());
		CPPUNIT_ASSERT(m[2].diap.getOrig()-m[1].diap.getOrig()<=s.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m[1].diap.getEnd()-m[2].diap.getEnd()<=s.tsk.count()+ AAdmin::GapStop() && " ��������� ������ � �������� ������� ");
		v1=t1->get(AEvent::FRC);
		v2=t2->get(AEvent::FRC);
	}
	{//  ���������� ��������� + ������� ��������� + �������� ���������+ �������� + ���� �� ���������� ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		ACounterInt::limit=5; 
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� ������� ���������
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto t1=adm.fnd(trn1.tdescr);
		auto t2=adm.fnd(trn2.tdescr);
		V_EVNT z1=t1->get(AEvent::FRC);
		V_EVNT z2=t2->get(AEvent::FRC);
		CPPUNIT_ASSERT(z1==v1 && "���� ����� ��������� �� ������ ������� ");
		CPPUNIT_ASSERT(z2==v2 && "����� ��������� �� �������� ������� ");
	}
}

void TC_AThread::testBackAdjustIntrusion()
{// ���� // �������� ������������ �������� 
	TRACE("\r\n =========== testBackAdjustIntrusion ============");
	Sevent sEv4(AEvent::REAL ,10+50/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,75/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,10/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		20,// ������ ����
		0,// ����� �� �������
		60,// ����� �� ��������
		0,// ����� �� ��������� ��������
		6, // ���.�������
		6// ����� �������
		);

	Strain trn2( TrainDescr( L"2", L"1-1-1"),sEv2, Sevent( NO_FIXED )); // FastAllYear ( 1, 150 ) 
	Strain trn4( TrainDescr( L"4", L"1-1-1"),sEv4, Sevent( NO_FIXED ));//�����������(������������)  6001, 6998

	// �� 1-� ����� ����������� ����
	TC_AtopologyParams tplParams( 30,2, 1, SpanBreadth::ONE_WAY_SPAN ); 
	st_intervals_t defTimes(1.f);
	defTimes.Iot= chrono::seconds(5);
	defTimes.Ipk=chrono::seconds(15);

	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	auto norm2=tpl.norma(tNrm2,2);
	map<EsrKit, RouteIntrusion::StationIntrusion> mIntrusions;
	RouteIntrusion::PassingOrderPtr passingOrderPtr( new RouteIntrusion::PassingOrder( TrainDescrPtr( new TrainDescr(trn2.tdescr) ) ) );
	mIntrusions[ EsrKit( 13 )]=RouteIntrusion::StationIntrusion( passingOrderPtr);
	RouteIntrusion rIntr(trn4.tdescr, vector<EsrKit>(), mIntrusions, map<EsrKit, RouteIntrusion::SpanIntrusion>() );
	TestAAdmin::VRouteIntrusionPtr v(new std::vector<RouteIntrusion>());
	v->push_back(rIntr);
	{// �������� ��� ��������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn4));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[2].diap.enclose(m13[4].diap));
		CPPUNIT_ASSERT(m13[2].diap.getEnd()-m13[4].diap.getEnd()==defTimes.Iot.count());
		CPPUNIT_ASSERT(m13[2].diap.duration()>40 && m13[2].diap.duration()<50);
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE);
	}
	{// �������� c ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.clear();
		adm.add(tpl.add(trn4),v);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[4].diap.duration()>15);
		CPPUNIT_ASSERT(m13[4].diap.getOrig()>m13[2].diap.getEnd());
		CPPUNIT_ASSERT(m13[2].diap.duration()>=30 && m13[2].diap.duration()<=30+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE);
	}
	{// �������� c ��������� � �������� �������������
		TestAAdmin adm;
		TestAAdmin::useBackAdjust=true;// ������������ �������� ��������� ����� ���������� ���������
		TestAAdmin::noSort=true;// �� ��������� �� ��� ������  ����������� ����������
		TestAAdmin::fnFltr.clear();
		TestAAdmin::deltaIntrusion=1;
		adm.add(tpl.add(trn4),v);//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.addNorma( trn2.tdescr, norm2 );
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m13=fltrStatEv(13,trFrc),m12=fltrStatEv(12,trFrc);
		CPPUNIT_ASSERT(m13[4].diap.duration()>AAdmin::deltaThrough());
		CPPUNIT_ASSERT(m13[4].diap.duration()<=AAdmin::deltaThrough()+  AAdmin::GapStop()+1);
		CPPUNIT_ASSERT(m13[4].diap.getOrig()>m13[2].diap.getEnd());
		CPPUNIT_ASSERT(m13[2].diap.duration()>=30 && m13[2].diap.duration()<=30+(int) AAdmin::GapStop());
		CPPUNIT_ASSERT(m12[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m12[2].getType()==AEvent::MOVE);
	}
}

void TC_AThread::testBackAdjustSpan()
{// ��������  ������������ ��� �������� �� ��������
	// ���������, 3 ���� �� �������
	TC_AtopologyParams tplParams( ForkTopology::ONE_FORK, stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN);
	st_intervals_t defTimes(1.f);
	defTimes.Ipk=chrono::seconds(30);
	defTimes.Iot=chrono::seconds(10);
	defTimes.Ipr=chrono::seconds(17);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	auto path10_17 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit( 17 ) ) );// �������� ���� �������
	auto path10_24 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit( 24 ) ) );// �������� ���� �������
	auto path10_180 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit(180 ) ) );// �������� ���� �������
	auto path20_180 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(20 ), EsrKit(180 ) ) );// �������� ���� �������
	CPPUNIT_ASSERT(path20_180.size()==21 && path10_180.size()==29 &&path10_24.size()==29 && path10_17.size()==15   );
/*        
          /  18-24
	10-17
	     \   25-30-180
*/
	//�������� ����
	Sevent sEv1(AEvent::REAL,370/*������ ����*/,5/*�� �������*/,20/*�� ��������*/,0,7/*���.�������*/,1/*����� �������*/) ;
	Sevent sEv4(AEvent::REAL,380/*������ ����*/,15/*�� �������*/,20/*�� ��������*/,0,9/*���.�������*/,1/*����� �������*/) ;
	Sevent sEv2(AEvent::REAL,230/*������ ����*/,5/*�� �������*/,20/*�� ��������*/,0,16/*���.�������*/,1/*����� �������*/) ;

	Strain trn2( TrainDescr( L"2", L"1-100-1" ),	sEv2, Sevent( NO_FIXED )	);
	Strain trn4( TrainDescr( L"4", L"1-100-1" ),	sEv4, Sevent( NO_FIXED )	);
	Strain trn1( TrainDescr( L"1", L"1-100-3" ),	sEv1, Sevent( NO_FIXED )	);
 	{// ��� ��������
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;
		ATrainPtr thr(tpl.add(trn1));
		adm.add(thr);//�� ��������
		STT_Cat_Val pvx=thr->getPMT(27,28);

		adm.add(tpl.add(trn2));//�� ��������
 		adm.add(tpl.add(trn4));//�� ��������
 		auto v=adm.forecastGlobal();
 		tstTRACE(adm);
		M_INT_EV m17= fltrStatEv(17,v),m16= fltrStatEv(16,v),m18=fltrStatEv(18,v);
 		CPPUNIT_ASSERT( m17[1].typeDiap()==AEvent::MOVE);
 		CPPUNIT_ASSERT(  m17[2].getType()==AEvent::MOVE&& m17[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m17[4].diap.getOrig()-m18[4].diap.getEnd()==pvx.full+pvx.stop && "���������� �� ���");
		CPPUNIT_ASSERT( m17[2].diap.getEnd()<m17[4].diap.getOrig());
 		CPPUNIT_ASSERT( m16[2].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT( m16[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m17[4].diap.getEnd()-m17[2].diap.getEnd()<defTimes.Ipk.count()+ AAdmin::GapStop());
		//CPPUNIT_ASSERT(m17[4].diap.getEnd()-m17[2].diap.getEnd()>defTimes.Iot.count());
 	}
	{// c ���������
 		TestAAdmin adm;
 		TestAAdmin::noSort=true;
 		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);// �-���  ������ ������� �� �������
		ATrainPtr thr(tpl.add(trn1));
		adm.add(thr);//�� ��������
		STT_Cat_Val pvx=thr->getPMT(27,28);
 		adm.add(tpl.add(trn2));//�� ��������
 		adm.add(tpl.add(trn4));//�� ��������
 		auto v=adm.forecastGlobal();
 		tstTRACE(adm);
		M_INT_EV m17= fltrStatEv(17,v),m16= fltrStatEv(16,v),m18=fltrStatEv(18,v);
 		CPPUNIT_ASSERT( m17[1].typeDiap()==AEvent::MOVE);
 		CPPUNIT_ASSERT(  m17[2].getType()==AEvent::MOVE&& m17[4].getType()==AEvent::STOP_START);
// 		CPPUNIT_ASSERT(m17[4].diap.getOrig()-m18[4].diap.getEnd()>pvx.full+pvx.stop+ AAdmin::GapStop() && " ������������� �� �� ���");
 		CPPUNIT_ASSERT(abs(m17[4].diap.getEnd()-m17[2].diap.getEnd()-defTimes.Ipk.count())< AAdmin::GapStop()+AAdmin::deltaThrough());
 		CPPUNIT_ASSERT( m16[2].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT( m16[4].getType()==AEvent::MOVE);
 		CPPUNIT_ASSERT(abs(m16[4].diap.getOrig()-m16[2].diap.getEnd()-defTimes.Ipk.count())< AAdmin::GapStop()+AAdmin::deltaThrough());	
	}
	{// c ��������� � ���������� ���������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);// �-���  ������ ������� �� �������
		ATrainPtr thr(tpl.add(trn1));
		adm.add(thr);//�� ��������
		STT_Cat_Val pvx=thr->getPMT(27,28);
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn4));//�� ��������
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m17= fltrStatEv(17,v),m16= fltrStatEv(16,v),m18=fltrStatEv(18,v);
		CPPUNIT_ASSERT( m17[1].typeDiap()==AEvent::STOP_START);
		CPPUNIT_ASSERT(  m17[2].getType()==AEvent::MOVE&& m17[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m17[4].diap.getOrig()-m18[4].diap.getEnd()==pvx.full && " ������������� �� ���");
		CPPUNIT_ASSERT(m17[4].diap.getEnd()-m17[2].diap.getEnd()>50);
		CPPUNIT_ASSERT( m16[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m16[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m16[4].diap.getEnd()-m16[2].diap.getEnd()==m17[4].diap.getEnd()-m17[2].diap.getEnd());	
	}
	{// c ��������� � ���������� ��������� � �������� ������������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrCounterEquPr);// �-���  ������ ������� �� �������
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrMorphReal);// �-���  ������ ������� �� �������
		TestAAdmin::useBackAdjust=true;
		ACounterInt::limit=10;
		ATrainPtr thr(tpl.add(trn1));
		adm.add(thr);//�� ��������
		STT_Cat_Val pvx=thr->getPMT(27,28);
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn4));//�� ��������
		// 		adm.add(tpl.add(trn6));//�� ��������
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		M_INT_EV m17= fltrStatEv(17,v),m16= fltrStatEv(16,v),m18=fltrStatEv(18,v);
		CPPUNIT_ASSERT( m17[1].typeDiap()==AEvent::STOP_START);
		CPPUNIT_ASSERT( m17[1].diap.duration()<AAdmin::deltaThrough()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(  m17[2].getType()==AEvent::MOVE&& m17[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m17[4].diap.getOrig()-m18[4].diap.getEnd()==pvx.full && "�� ���");
		CPPUNIT_ASSERT( m16[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT( m16[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m16[4].diap.getEnd()-m16[2].diap.getEnd()==m17[4].diap.getEnd()-m17[2].diap.getEnd());	
	}
}
