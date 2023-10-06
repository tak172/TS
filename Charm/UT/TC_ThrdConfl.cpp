/*!
	created:	2017/08/29
	created: 	16:56  29 ������ 2017
	filename: 	D:\potapTrunk\Charm\UT\TC_ThrdConfl.cpp
	file path:	D:\potapTrunk\Charm\UT
	file base:	TC_ThrdConfl
	file ext:	cpp
	author:		 Dremin
	
	purpose:	�������� ���������� ����������  
  */
 
 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Augur/Occupancy.h"
#include "../Augur/RailLimitation.h"
#include "../Augur/TrainRoutes.h"
#include "TestAAdmin.h"
#include "TestPossessions.h"
#include "../Augur/ALogWrn.h"

void TC_AThread::testTopInterval()
{	// ���� ����� Top (����������� �������� ���������������� ����������� � ��������� ��������, ��� �������, ��� ������� �� �������� ���� �������������� ��� ���������)
	TRACE("\r\n =========== testTopInterval ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes;
	defTimes.Top = std::chrono::minutes(3);
	defTimes.tpr = std::chrono::seconds(4);
	defTimes.tot = std::chrono::seconds(4);
	defTimes.tp = std::chrono::seconds(4);
	defTimes.tbsk = std::chrono::seconds(4);
	defTimes.tnp = std::chrono::seconds(14);
	defTimes.tn = std::chrono::seconds(31);
	defTimes.tsk = std::chrono::seconds(3);
	defTimes.tpbo = std::chrono::seconds(11);
	defTimes.Ipr = std::chrono::seconds(13);
	defTimes.Iot = std::chrono::seconds(18);
	defTimes.Ipk = std::chrono::seconds(23);
	tpl.setPvx(20,&defTimes);
	
// 	tpl.setPvx(20);
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv5003(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		5, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	Strain trn5003( TrainDescr( L"5003", L""),sEv5003, Sevent( NO_FIXED ));
 
	Sevent tNrm2(AEvent::NONE/*��� �������*/,20/*������ ����*/,1000/* ����� �� �������*/,20/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
		,6/*���.�������*/,2/* ����� �������*/);
	Sevent tNrm5003(AEvent::NONE/*��� �������*/	,40/* ������ ����*/,20/* ����� �� �������*/,20/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
		,5/*���.�������*/,3/* ����� �������*/);
	auto norm2=tpl.norma(tNrm2,2);
	auto norm5003=tpl.norma(tNrm5003,1);
	{
		Sevent sEv1(//�������� ����
			AEvent::REAL,// ��� �������
			5,// ������ ����
			10, // ����� �� �������
			20,// ����� �� ��������
			0,// ����� �� ��������� ��������
			6, // ���.������� (+10 � ������������)
			1// ����� �������
			);
		Strain trn1( TrainDescr( L"1", L""),sEv1, Sevent( NO_FIXED ));
		Sevent tNrm1(AEvent::NONE/*��� �������*/	,28/* ������ ����*/,50/* ����� �� �������*/,20/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
			,6/*���.�������*/,3/* ����� �������*/);
		auto norm1=tpl.norma(tNrm1,1);

		TestAAdmin adm;
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn5003));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������

		adm.addNorma( trn1.tdescr, norm1 );
		adm.addNorma( trn5003.tdescr, norm5003 );
		adm.addNorma( trn2.tdescr, norm2 );

		st_intervals_t st= getInterval(StationEsr(16),StationEsr(17),adm.getNsi()->stIntPtr);
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		AEvent ev1,ev3;
		for (const auto& t: trFrc0.getSpotLines())
			switch (t.first.GetNumber().getNumber())
		{
			case 1:
				ev1=*std::find_if(t.second.begin(),t.second.end(),[](const AEvent& ev){return ev.Esr()==EsrKit(16);});
				break;
			case 5003:
				ev3=*std::find_if(t.second.begin(),t.second.end(),[](const AEvent& ev){return ev.Esr()==EsrKit(16);});
				break;
		}

		CPPUNIT_ASSERT(!ev1.empty() && !ev3.empty());
		CPPUNIT_ASSERT(abs(ev1.diap.getEnd()+ st.Top.count()-ev3.diap.getOrig())<5 && "����������� �������� ���������������� ����������� � ��������� ��������");
	}
}
 
void TC_AThread::testBlockPost()
{// ���� �� ���������� ���������� �������������� ������ ��������������� ������� (�������������).
	TRACE("\r\n =========== testBlockPost ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	tplParams.setDetalization( StationEsr(12), TC_AtopologyParams::StationDetalization( NsiBasis::StationKind::Blockpost ) );
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(4.f);
	defTimes.Ipk=std::chrono::seconds(20);
	defTimes.Iot=std::chrono::seconds(10);
	defTimes.Ipr=std::chrono::seconds(10);
	tpl.setPvx(20,&defTimes);
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	1000/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv3(AEvent::REAL ,15/*������ ����*/,600/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		1/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv1001(AEvent::REAL ,5/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		4/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"2-100-1"),sEv3, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"1001", L"2-100-1"),sEv1001, Sevent( NO_FIXED ));
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1,1));//�� ��������
		adm.add(tpl.add(trn3,2));//�� ��������
		adm.add(tpl.add(trn1001));//�� ��������
		auto v= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		M_INT_EV m11= fltrStatEv(11,v),m12= fltrStatEv(12,v),m13=fltrStatEv(13,v);
		CPPUNIT_ASSERT(m13[1001].getType()==AEvent::STOP_START&& "�������� ����� ����������.");
		CPPUNIT_ASSERT(m12[1001].getType()==AEvent::MOVE && "�� ��������� �� �������.");
		CPPUNIT_ASSERT(m11[1001].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m11[3].diap.getEnd()+defTimes.Iot.count()<m11[1001].diap.getEnd()&&	"�� 1001 ������ ����� ������������ ���� �� ���=11");
		CPPUNIT_ASSERT(m11[3].diap.getEnd()+defTimes.Iot.count()+ AAdmin::GapStop()+AAdmin::deltaThrough() >=m11[1001].diap.getEnd());
	}
	{
		TestPossessions p( TestAAdmin::getNsi() );
		//----- ��������� ���� ------------
		Stech tWnd( 1, 390 /*������������*/, 1, 1/*park*/, 2/*e1*/,1,nullptr);
		WayInfoCPtr w=tpl.fnd(tWnd);
		RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
		std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
		p.setLimitation( w, vR );
		//--------------------------------------------
		TestAAdmin adm(p);
		adm.add(tpl.add(trn1001));//�� ��������
		adm.StopBefAftLockPath(30, SpanLockType::BIDIR_AUTOBLOCK,TestAAdmin::Before,true);  /* ������� ����� ������� �� ����.����*/
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto frc=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(frc.size()==1);
		for (const auto& ev: frc.front().second)
			if(!ev.onSpan())
				switch (ev.Esr().getTerm())
			{
				case 13:
					CPPUNIT_ASSERT( ev.diap.duration()==AAdmin::deltaThrough() && "�������� �� ������� ��������.");
					break;
				case 12:
					CPPUNIT_ASSERT( ev.diap.duration()>=AAdmin::StopBefAftLockPath(SpanLockType::BIDIR_AUTOBLOCK, true,false) && "���� �� ��������� ������ �� ����.����.");
					break;
			}
			else
				if(ev.Esr()==EsrKit(11,12))
					CPPUNIT_ASSERT(ev.Wrongway() && "������ �� ������.����");
	}
	{
		Stech tWnd( 1, 310 /*������������*/, 0, 0/*park*/, 2/*e1*/,1,nullptr);
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		TestAAdmin adm(p);
		adm.add(tpl.add(trn1001));//�� ��������
		auto trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto frc=trFrc0.getSpotLines();
		CPPUNIT_ASSERT(frc.size()==1);
		for (const auto& ev: frc.front().second)
			if(!ev.onSpan())
				switch (ev.Esr().getTerm())
			{
				case 13:
					CPPUNIT_ASSERT( ev.diap.duration()>AAdmin::deltaThrough() && "�������� �� ������� ��������.");
					break;
				case 12:
					CPPUNIT_ASSERT( ev.diap.duration()==AAdmin::deltaThrough() && "�� ���� �� ���������.");
					break;
				case 11:
					;
			}
	}
}

void TC_AThread::testSortTrn()
{// ���� ���������� �������
	TRACE("\r\n =========== testSortTrn ============");
	{
		long s[]={ 1,2,3,4,0,5,6,7,8,9};
		long w1[]={11,12,13,0,5,6};
		long w2[]={11,0,5,6,7,12,13};
		LONG_VECTOR sV(s,s+size_array(s));
		LONG_VECTOR wV1(w1,w1+size_array(w1));
		LONG_VECTOR wV2(w2,w2+size_array(w2));

		int r=level(sV,0,sV,0);
		int r1=level(sV,0,sV,5);
		int r2=level(sV,0,sV,2);

		int r11=level(sV,0,wV1,11);
		int r0=level(sV,0,wV1,0);
		int r5=level(sV,0,wV1,5);
		int r6=level(sV,0,wV1,6);
		CPPUNIT_ASSERT(r==0 && r1==1&& r2==-3 && r11==-3 && r0==0 && r5==1 && r6==2);
	}

	TC_AtopologyParams tplParams( 60, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		16/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv3(AEvent::REAL ,10/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,5/*����� �� ��������� ��������*/,
		18/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv5(AEvent::REAL ,15/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		11/*���.������� (+10 � ������������)*/,2/*����� �������*/);
	Sevent sEv2(AEvent::REAL ,4/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		26/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Sevent sEv4(AEvent::REAL ,9/*������ ����*/,10/*����� �� �������*/,20/*����� �� ��������*/,5/*����� �� ��������� ��������*/,
		25/*���.������� (+10 � ������������)*/,3/*����� �������*/);
	Sevent sEv6(AEvent::REAL ,14/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		22/*���.������� (+10 � ������������)*/,1/*����� �������*/);

	Sevent tNrm1(AEvent::NONE/*��� �������*/	,10/* ������ ����*/,50/* ����� �� �������*/,20/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
		,10/*���.�������*/,11/* ����� �������*/);
	Sevent tNrm2(AEvent::NONE/*��� �������*/	,10/* ������ ����*/,50/* ����� �� �������*/,20/* ����� �� ��������*/,0/*����� �� ��������� ��������*/
		,30/*���.�������*/,10/* ����� �������*/);
	auto norm1=tpl.norma(tNrm1,1);
	auto norm2=tpl.norma(tNrm2,2);
	Strain trn1( TrainDescr( L"1", L""),sEv1, Sevent( NO_FIXED ));
	Strain trn3( TrainDescr( L"3", L"2-100-4"),sEv3, Sevent( NO_FIXED ));
	Strain trn5( TrainDescr( L"5", L"2-100-5"),sEv5, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L""),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"2-100-2"),sEv4, Sevent( NO_FIXED ));
	Strain trn6( TrainDescr( L"6", L"2-100-1"),sEv6, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn3));//�� ��������
		adm.add(tpl.add(trn4));//�� ��������
		adm.add(tpl.add(trn5));//�� ��������
		adm.add(tpl.add(trn6));//�� ��������

		adm.addNorma( trn1.tdescr, norm1 );
		adm.addNorma( trn2.tdescr, norm2 );
		// ���������� ����������.������ �����+ ���������  ����� ��� ������� ���� ��������
		adm.fndAllNorm();// ��� ���� �� ������ ����������
		V_TRN_PTR vTr= adm.prepareTrains();
		const V_TRN & all=adm.getTrn();

		V_TRN oddV,evenV;
		for (const auto& t:all )
		{
			CPPUNIT_ASSERT(t->level(t->sharedPtr())==(t->origUnionFrc().onSpan()?-1:0) && "�� ����� ��������� �� ������� �� ������ ��������");
			if(t->getNum()&1)
				oddV.push_back(t);
			else
				evenV.push_back(t);
		}
		{
			auto oddS=adm.sort(oddV);
			auto evenS=adm.sort(evenV);
			CPPUNIT_ASSERT(oddS!=oddV && evenS!=evenV && " ���������� �������� �������");
			int nOdd[]={3,1,5};
			int nEven[]={6,4,2};
			for (int i=0;i<size_array(nOdd);i++ )
			{
				CPPUNIT_ASSERT(nOdd[i]==oddS.at(i)->getNum() && "������� ����� ����������");
				CPPUNIT_ASSERT(nEven[i]==evenS.at(i)->getNum() && "������� ����� ����������");
			}
			auto allS=adm.sort(all);
			auto allS1=adm.sort(allS);
			CPPUNIT_ASSERT(allS==allS1 && "���������� ���������������� �� ������ �������");
		}
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
}

void TC_AThread::testStopOnSpan()
{ // ���� ������� �� ��������
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl(tplParams );
	tpl.setPvx(60);
	V_EVNT v;
	v.push_back(AEvent(16));
	v.back().diap.set(0,5);
	v.back().setSource(AEvent::REAL);
	v.back().setType(AEvent::MOVE);

	AEvent evStop(16,17);
	evStop.setObj(1);
	evStop.diap.set(20,20);
	evStop.setSource(AEvent::REAL);
	evStop.setType(AEvent::STOP);
	v.push_back(evStop);
	AThread tStop(v);

	AEvent evStart(16,17);
	evStart.setObj(1);
	evStart.diap.set(60,60);
	evStart.setSource(AEvent::REAL);
	evStart.setType(AEvent::START);
	v.push_back(evStart);
	AThread tmp(v);
 	{// �������� ������� ������ (��������������� MOVE) �� ��������
 		ATrainPtr tr=tpl.add(TrainDescr( L"1", L"1-100-2"),&tStop);
 		TestAAdmin adm(70);
 		adm.add(tr);//�� ��������
 		auto v=adm.forecastGlobal();
 		tstTRACE(adm);
 		auto t= adm.fnd(tr->descr());
 		auto pvx= t->getPMT(16,17);
 		AEvent ev16= t->fndEv(16,0), ev17= t->fndEv(17,0);
 		ADiap d(ev16.diap.getEnd(),ev17.diap.getOrig());
 		ADiap dStop(evStop.diap.getOrig(),AAdmin::LiveMoment());
 		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
 		CPPUNIT_ASSERT(d.duration()==pvx.full+pvx.stop+pvx.start+ TM_STOPSPAN_PASS*60 + dStop.duration() && "����� ����� �� ���������");
 	}
	{// ������ ������� ������ (��������������� MOVE) �� ��������
		ATrainPtr tr=tpl.add(TrainDescr( L"1", L"1-100-2"),&tmp);
		TestAAdmin adm(70);
		adm.add(tr);//�� ��������
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto t= adm.fnd(tr->descr());
		auto pvx= t->getPMT(16,17);
		AEvent ev16= t->fndEv(16,0), ev17= t->fndEv(17,0);
		ADiap d(ev16.diap.getEnd(),ev17.diap.getOrig());
		ADiap dStop(evStop.diap.getOrig(),evStart.diap.getEnd());
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		CPPUNIT_ASSERT(d.duration()==pvx.full+pvx.stop+pvx.start + dStop.duration()&& "����� �� ��������� ���");
	}
}

void TC_AThread::testFndPVX()
{// ������ ��� ��� ������ ����� ��
	int pvhDies=20,pvhGr=40,pvhPass=10,pvhEl=30;
	Sevent sEv(	AEvent::REAL,20,/*������ ����*/	15,/*����� �� �������*/25,/*����� �� ��������*/0,/*����� �� ��������� ��������*/
		2, /* ���.������� (+10 � ������������)*/1/* ����� �������*/	);
	Strain trPass( TrainDescr( L"301", L"2-100-2"),sEv, Sevent( NO_FIXED ));
	Strain trEl( TrainDescr( L"7601", L"2-100-2"),sEv, Sevent( NO_FIXED ));
	Strain trDies( TrainDescr( L"7801", L"2-100-2"),sEv, Sevent( NO_FIXED ));
	Strain trGr( TrainDescr( L"3111", L"2-100-2"),sEv, Sevent( NO_FIXED ));
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
		TC_Atopology tpl(tplParams );
		tpl.setPvx(pvhDies,SttMove::Type::TIME_DIESEL);
		tpl.setPvx(pvhGr,SttMove::Type::TIME_GOODS);
		tpl.setPvx(pvhPass,SttMove::Type::TIME_PASS);
		tpl.setPvx(pvhEl,SttMove::Type::TIME_ELSECTION);
		ATrainPtr thrPass(tpl.add(trPass)),thrEl(tpl.add(trEl)),thrGr(tpl.add(trGr)),thrDis(tpl.add(trDies));
		V_EVNT vC= thrPass->prepare();// ���������� ��������.�������� ����
		thrGr->prepare();
		thrEl->prepare();
		thrDis->prepare();
		V_EVNT vIdGr=thrGr->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdPass=thrPass->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdEl=thrEl->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdDis=thrDis->buildIdeal().first;// ���������� �� ���
		int pvh=pvhDies;
		auto pD=std::count_if(vIdDis.begin(),vIdDis.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhGr;
		auto pG=std::count_if(vIdGr.begin(),vIdGr.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhPass;
		auto pP=std::count_if(vIdPass.begin(),vIdPass.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhEl;
		auto pE=std::count_if(vIdEl.begin(),vIdEl.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		CPPUNIT_ASSERT(pD==6 &&pG==6 &&pE==6 &&pP==6 && "8 ���������. � ������ ������ , � ����� ����������");
	}
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
		TC_Atopology tpl(tplParams );
		tpl.setPvx(pvhPass,SttMove::Type::TIME_PASS);
		tpl.setPvx(pvhEl,SttMove::Type::TIME_ELSECTION);
		ATrainPtr thrPass(tpl.add(trPass)),thrEl(tpl.add(trEl)),thrGr(tpl.add(trGr)),thrDis(tpl.add(trDies));
		V_EVNT vC= thrPass->prepare();// ���������� ��������.�������� ����
		thrGr->prepare();
		thrEl->prepare();
		thrDis->prepare();
		V_EVNT vIdGr=thrGr->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdPass=thrPass->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdEl=thrEl->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdDis=thrDis->buildIdeal().first;// ���������� �� ���
		int pvh=pvhPass;
		auto pD=std::count_if(vIdDis.begin(),vIdDis.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		auto pG=std::count_if(vIdGr.begin(),vIdGr.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		auto pP=std::count_if(vIdPass.begin(),vIdPass.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhEl;
		auto pE=std::count_if(vIdEl.begin(),vIdEl.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		CPPUNIT_ASSERT(pD==6 &&pG==6 &&pE==6 &&pP==6 && "8 ���������. � ������ ������, � ����� ����������. ��� �� ����.��");
	}
	{
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
		TC_Atopology tpl(tplParams );
		tpl.setPvx(pvhDies,SttMove::Type::TIME_DIESEL);
		tpl.setPvx(pvhGr,SttMove::Type::TIME_GOODS);
		ATrainPtr thrPass(tpl.add(trPass)),thrEl(tpl.add(trEl)),thrGr(tpl.add(trGr)),thrDis(tpl.add(trDies));
		V_EVNT vC= thrPass->prepare();// ���������� ��������.�������� ����
		thrGr->prepare();
		thrEl->prepare();
		thrDis->prepare();
		V_EVNT vIdGr=thrGr->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdPass=thrPass->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdEl=thrEl->buildIdeal().first;// ���������� �� ���
		V_EVNT vIdDis=thrDis->buildIdeal().first;// ���������� �� ���
		int pvh=pvhDies;
		auto pD=std::count_if(vIdDis.begin(),vIdDis.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		pvh=pvhGr;
		auto pG=std::count_if(vIdGr.begin(),vIdGr.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		auto pP=std::count_if(vIdPass.begin(),vIdPass.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		auto pE=std::count_if(vIdEl.begin(),vIdEl.end(),[pvh](const AEvent& ev){ return ev.onSpan() && ev.diap.duration()==pvh;});
		CPPUNIT_ASSERT(pD==6 &&pG==6 &&pE==6 &&pP==6 && "8 ���������. � ������ ������, � ����� ����������. ��� �� ����.��");
	}

}

void TC_AThread::testSkrechArr()
{	// ���� �� ����� ���������� ��������� ��� �� �� �������
	Sevent sEv4(//�������� ����
		AEvent::REAL,// ��� �������
		110,// ������ ����
		//60,// ����� �� �������
		180/*( int ) defTimes.tnp.count()*/, // ����� �� �������
		40,// ����� �� ��������
		0,// ����� �� ��������� ��������
		7, // ���.������� (+10 � ������������)
		6// ����� �������
		);
	Strain trn4( TrainDescr( L"4", L""),sEv4, Sevent( NO_FIXED ));
	TRACE("\r\n =========== testSkrech ============");
	{

		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
		Sevent sEv1001(//�������� ����
			AEvent::REAL,// ��� �������
			240,// ������ ����
			//10,// ����� �� �������
			180/*( int ) defTimes.tnp.count()*/, // ����� �� �������
			0,// ����� �� ��������
			0,// ����� �� ��������� ��������
			5, // ���.������� (+10 � ������������)
			1// ����� �������
			);
		Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
		{// ��������� ��������� ������ ����� ������� 4 �� 16 �������
			st_intervals_t defTimes;
			defTimes.tsk=std::chrono::seconds(45);
			defTimes.tn=std::chrono::seconds(140);
			defTimes.tnp=std::chrono::seconds(140);
			TC_Atopology tpl( tplParams );

			tpl.setPvx(30,&defTimes);
			TestAAdmin adm;
			adm.add(tpl.add(trn4));
			adm.add(tpl.add(trn1001));//�� ��������
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� c ����������� �� ������ ����.");
			auto t4=adm.fnd(trn4.tdescr);
			auto ev4= t4->fndEv(EsrKit(15));
			auto t1001=adm.fnd(trn1001.tdescr);
			auto ev16= t1001->fndEv(EsrKit(16));
			CPPUNIT_ASSERT(ev16.diap.duration()==AAdmin::deltaThrough() );
			auto ev15= t1001->fndEv(EsrKit(15));
			CPPUNIT_ASSERT(ev15.diap.getEnd()==defTimes.tsk.count()+ev4.diap.getOrig()  && "���� ��������� ��� �������� �������� ") ;
		}
		{// ��������� �� ��������� ������ ����� ������� 4 �� 16 �������. ���� �� 15 �������
			st_intervals_t defTimes;
			defTimes.tsk=std::chrono::seconds(30);
			defTimes.tn=std::chrono::seconds(140);
			defTimes.tnp=std::chrono::seconds(140);
			TC_Atopology tpl( tplParams );

			tpl.setPvx(30,&defTimes);
			TestAAdmin adm;
			adm.add(tpl.add(trn4));
			adm.add(tpl.add(trn1001));//�� ��������
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� c ����������� �� ������ ����.");
			auto t4=adm.fnd(trn4.tdescr);
			auto ev4_15= t4->fndEv(EsrKit(15));
			auto ev4_16= t4->fndEv(EsrKit(16));
			auto t1001=adm.fnd(trn1001.tdescr);
			auto ev16= t1001->fndEv(EsrKit(16));
			CPPUNIT_ASSERT(ev16.diap.duration()==AAdmin::deltaThrough() );
			CPPUNIT_ASSERT(ev16.diap.getEnd()<=ev4_16.diap.getEnd()-defTimes.tsk.count()  && "���� ��������� ��� �������� �������� ") ;
			CPPUNIT_ASSERT(ev16.diap.getOrig()>=ev4_16.diap.getOrig()+defTimes.tn.count()  && "���� ���������������� �������� ") ;
		}
		{
			st_intervals_t defTimes;
			TC_Atopology tpl( tplParams );
			tpl.setPvx(30,&defTimes);
			TestAAdmin adm;
			adm.add(tpl.add(trn4));
			adm.add(tpl.add(trn1001));//�� ��������
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� c ����������� �� ������ ����.");
			auto t4=adm.fnd(trn4.tdescr);
			auto ev4= t4->fndEv(EsrKit(15));
			auto t1001=adm.fnd(trn1001.tdescr);
			auto ev= t1001->fndEv(EsrKit(15));
			CPPUNIT_ASSERT(abs (ev4.diap.getOrig()+ defTimes.tsk.count()-ev.diap.getEnd())<3  && "���� ��������� ") ;
			auto ev16= t1001->fndEv(EsrKit(16));
			CPPUNIT_ASSERT(ev16.diap.duration()==AAdmin::deltaThrough() );
	}
	}
	{
		st_intervals_t defTimes;
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN );
		TC_Atopology tpl( tplParams );
		tpl.setPvx(30,&defTimes);

		Sevent sEv1001(//�������� ����
			AEvent::REAL,// ��� �������
			240,// ������ ����
			//10,// ����� �� �������
			( int ) defTimes.tnp.count(), // ����� �� �������
			0,// ����� �� ��������
			0,// ����� �� ��������� ��������
			5, // ���.������� (+10 � ������������)
			1// ����� �������
			);
		Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn4));
		adm.add(tpl.add(trn1001));//�� ��������


		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
// 		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� c ����������� �� ������ ����.");
		auto t1001=adm.fnd(trn1001.tdescr);
		auto ev= t1001->fndEv(EsrKit(16));
		CPPUNIT_ASSERT( ev.diap.duration()==AAdmin::deltaThrough()  && " ��� 2-� ����� ��������� �� ��������� ") ;
	}
	{
		st_intervals_t defTimes;
		TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
		TC_Atopology tpl( tplParams );
		tpl.setPvx(30,&defTimes);
		Sevent sEv1001(//�������� ����
		AEvent::REAL,// ��� �������
		410,// ������ ����
		//10,// ����� �� �������
		( int ) defTimes.tnp.count(), // ����� �� �������
		0,// ����� �� ��������
		0,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		1// ����� �������
		);
		Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
		TestAAdmin adm;
		adm.add(tpl.add(trn4));
		adm.add(tpl.add(trn1001));//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto vl=trFrc.getSpotLines();
		CPPUNIT_ASSERT(vl.size()==2 && vl.back().first==trn1001.tdescr);
		auto ev= vl.back().second.front();
		CPPUNIT_ASSERT(ev.Esr().getTerm()==16 && ev.diap.duration()==1  && "�������� ���� � ���� ��������� ��� �������� �� ��������") ;
	}
}
