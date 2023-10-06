/*!
	created:	2018/06/25
	created: 	11:32  25 ���� 2018
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrNorm.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrNorm
	file ext:	cpp
	author:		 Dremin
	
	purpose:	
  
  */
 
 #include "stdafx.h"
 #include "TC_Thread.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/AInterval.h"
#include "../Augur/ProNorm.h"
#include "../Augur/TxtComment.h"

using namespace std;
typedef TechnicalTime TT;

void TC_AThread::testFndNormOccupy()
{// ����� ��������� ���������� ��� ����������� �� ������� c �������� ��� ��
	RoadKit  emptyRoad;
	std::map<EsrKit, TechnicalTime> techTimes;
	TT::Specification specification( Oddness::UNDEF, TT::Fullness::Any, emptyRoad, emptyRoad, TTF::Refrigerator );
	techTimes[EsrKit(15)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 3 ) ) );
	// ���������, 2 ���� �� �������
	TC_AtopologyParams tplParams( ForkTopology::NO_FORK, stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes );
	tplParams.departureByHemPath.insert(EsrKit(15));// ��� ������ �������� � ����������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20) ;
	// ����������
	Sevent sN3(AEvent::NONE,280/*������ ����*/,100/*�� �������*/,20/*�� ��������*/,0,5/*���.�������*/,8/*����� �������*/) ;
	Sevent sN4(AEvent::NONE,380/*������ ����*/,100/*�� �������*/,20/*�� ��������*/,0,5/*���.�������*/,7/*����� �������*/) ;
	auto norm3=tpl.norma(sN3,1);
	auto norm4=tpl.norma(sN4,1);
	//�������� ����
	Sevent sEv1(AEvent::REAL,20/*������ ����*/,15/*�� �������*/,20/*�� ��������*/,10,4/*���.�������*/,1/*����� �������*/) ;
	Sevent sEv2(AEvent::REAL,30/*������ ����*/,15/*�� �������*/,20/*�� ��������*/,10,4/*���.�������*/,1/*����� �������*/) ;
	Strain trn1( TrainDescr( L"1005", L"1-100-2" ),	sEv1, Sevent( NO_FIXED )	);
	Strain trn2( TrainDescr( L"1007", L"1-100-2" ),	sEv2, Sevent( NO_FIXED )	);
	Sevent sEv3(AEvent::REAL,430/*������ ����*/,100/*�� �������*/,20/*�� ��������*/,0,6/*���.�������*/,1/*����� �������*/) ;
	Strain trn3( TrainDescr( L"1009", L"" ),	sEv3, Sevent( NO_FIXED )	);
	Strain trn4( TrainDescr( L"1019", L"" ),	sEv3, Sevent( NO_FIXED )	);
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.add(tpl.add(trn3,1));//�� ��������
		adm.add(tpl.add(trn4,2));//�� ��������
		adm.addNorma(TrainDescr( L"2309", L""), norm3 );
		adm.addNorma(TrainDescr( L"2311", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto vl= v.getSpotLines();
		CPPUNIT_ASSERT(vl.size()==4);
		auto tr=vl.front();
		CPPUNIT_ASSERT(tr.first==trn1.tdescr);
		auto m16=fltrStatEv(16,v),m15=fltrStatEv(15,v);
		st_intervals_t st= getInterval(StationEsr(16),StationEsr(17),adm.getNsi()->stIntPtr);
		auto itN=std::find_if(norm3.begin(),norm3.end(),[](const AEvent& t){return t.Esr().getTerm()==15;});
		auto trPtr= adm.fnd(trn1);//1005
		auto trPtr9= adm.fnd(trn3);//1009
		AEvent ev= trPtr9->fndEv(16,0);
		CPPUNIT_ASSERT(itN!=norm3.end() &&  itN->diap.getEnd()<m15[1005].diap.getEnd() && "����������� �� ���������� 2309");
		CPPUNIT_ASSERT( itN->diap.getEnd()==TestProNorm::fndPreset(trPtr,AEvent(15)) && "���������� 2309 ������ ��� 1005");
		CPPUNIT_ASSERT(m16[1005].diap.getOrig()>ev.diap.getOrig());
		CPPUNIT_ASSERT(m16[1005].diap.getOrig()<(ev.diap.getEnd()+st.tsk.count()+  AAdmin::GapStop()) && "����� ���� �� 16 ������� � ��� ����� ���� �� ���������� 2309");
		itN=std::find_if(norm4.begin(),norm4.end(),[](const AEvent& t){return t.Esr().getTerm()==15;});
		CPPUNIT_ASSERT(itN!=norm4.end() && itN->diap.getEnd()==m15[1007].diap.getEnd() && "����������� �� ���������� 2311");
	}
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		adm.addNorma(TrainDescr( L"2309", L""), norm3 );
		adm.addNorma(TrainDescr( L"2311", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto vl= v.getSpotLines();
		CPPUNIT_ASSERT(vl.size()==2);
		auto tr=vl.front();
		CPPUNIT_ASSERT(tr.first==trn1.tdescr);
		EsrKit esr(15);
		auto it=std::find_if(tr.second.begin(),tr.second.end(),[&esr](const AEvent& t){return t.Esr()==esr;});
		auto itN=std::find_if(norm3.begin(),norm3.end(),[&esr](const AEvent& t){return t.Esr()==esr;});
		CPPUNIT_ASSERT(itN!=norm3.end() && it!=tr.second.end() && itN->diap.getEnd()==it->diap.getEnd() && "����������� �� ���������� 2309");
		tr=vl.back();
		it=std::find_if(tr.second.begin(),tr.second.end(),[&esr](const AEvent& t){return t.Esr()==esr;});
		itN=std::find_if(norm4.begin(),norm4.end(),[&esr](const AEvent& t){return t.Esr()==esr;});
		CPPUNIT_ASSERT(itN!=norm4.end() && it!=tr.second.end() && itN->diap.getEnd()==it->diap.getEnd() && "����������� �� ���������� 2311");
	}
}

void TC_AThread::testFndProNorm()
{// ����� ��������� ���������� ��� ����������� �� ������� � ����������
	{
		INT_INT tx[]={INT_INT(1,698),INT_INT(901,998),INT_INT(5001,5998)};
		//--- ��������� ������� ������� ������������ ��� ��������������� ---
		INTERVALS  n(tx, tx+size_array(tx));// ���������� ���������� ���� �� ������ ������������ �������
		INTERVALS n1(ARRAY_IT(tx));
		CPPUNIT_ASSERT(n==n1);
		CPPUNIT_ASSERT(n.size()==3 && n.front()==AInterval(INT_INT(1,698)) && n[1]==AInterval(INT_INT(901,998)) && n[2]==AInterval(INT_INT(5001,5998)));
	}
	const TrainCharacteristics & trnChr=  TrainCharacteristics::instanceCRef();
	RoadKit  emptyRoad;
	std::map<EsrKit, TechnicalTime> techTimes;
	TT::Specification specification( Oddness::UNDEF, TT::Fullness::Any, emptyRoad, emptyRoad, TTF::Refrigerator );
	techTimes[EsrKit(15)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 3 ) ) );
	// ���������, 3 ���� �� �������
	TC_AtopologyParams tplParams( ForkTopology::ONE_FORK, stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes );
	tplParams.departureByHemPath.insert(EsrKit(15));// ��� ������ �������� � ����������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20) ;

	auto path10_17 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit( 17 ) ) );// �������� ���� �������

	auto path10_24 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit( 24 ) ) );// �������� ���� �������
	auto path10_180 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(10 ), EsrKit(180 ) ) );// �������� ���� �������
	auto path20_180 = AAdmin::getNsi()->topologyPtr->get(std::make_pair( EsrKit(20 ), EsrKit(180 ) ) );// �������� ���� �������
	CPPUNIT_ASSERT(path20_180.size()==21 && path10_180.size()==29 &&path10_24.size()==29 && path10_17.size()==15   );
	// ����������
	Sevent sN1(AEvent::NONE,70/*������ ����*/,100/*�� �������*/,20/*�� ��������*/,0,4/*+10 ���.�������*/,4/*����� �������*/) ;
	Sevent sN2(AEvent::NONE,260/*������ ����*/,100/*�� �������*/,20/*�� ��������*/,0,5/*���.�������*/,4/*����� �������*/) ;
	Sevent sN3(AEvent::NONE,280/*������ ����*/,100/*�� �������*/,20/*�� ��������*/,0,5/*���.�������*/,8/*����� �������*/) ;
	Sevent sN4(AEvent::NONE,300/*������ ����*/,100/*�� �������*/,20/*�� ��������*/,0,5/*���.�������*/,7/*����� �������*/) ;
	auto norm1=tpl.norma(sN1,1);
	auto norm2=tpl.norma(sN2,1);
	auto norm3=tpl.norma(sN3,1);
	auto norm4=tpl.norma(sN4,1);
	for ( auto& t: norm4)
		if(t.Esr().getTerm()>17)
			t.resetEsr(EsrKit(t.Esr().getTerm()-18 +25));// ������� �� ������ �����
	//�������� ����
	Sevent sEv(AEvent::REAL,20/*������ ����*/,15/*�� �������*/,20/*�� ��������*/,0,4/*���.�������*/,1/*����� �������*/) ;
	Strain trn( TrainDescr( L"1005", L"1-100-2" ),	sEv, Sevent( NO_FIXED )	);

	TestAAdmin adm;
	adm.add(tpl.add(trn));//�� ��������
	adm.addNorma(TrainDescr( L"3601", L""), norm1 );
	adm.addNorma(TrainDescr( L"9703", L""), norm2 );
	adm.addNorma(TrainDescr( L"2309", L""), norm3 );
	adm.addNorma(TrainDescr( L"2311", L""), norm4 );
	ProNorm::SET_SYNON s;
	s.insert(StationEsr(16));
	ProNorm::addSynonim(s);// ��� ������ ���������
	{
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto vl= v.getSpotLines();
		CPPUNIT_ASSERT(vl.size()==1 );
		auto vEv=vl.front().second;
		time_t dep15=0;
		for (const auto& t: vEv)
			if(t.Esr()==EsrKit(15))
				dep15=t.diap.getEnd();
		CPPUNIT_ASSERT(dep15 && " �������� 15 �������");
		for (const auto& t: norm1)
			if(t.Esr()==EsrKit(15))
				CPPUNIT_ASSERT(t.diap.getEnd()==dep15 && " ���� �� ��������� ��������� �����.���������� c ������ �������-�����������");
	}

	ProNorm::clearSynonim();// ��� ������ ���������
	auto v=adm.forecastGlobal();
	auto vl= v.getSpotLines();
	CPPUNIT_ASSERT(vl.size()==1 );
	auto vEv=vl.front().second;
	time_t dep15=0;
	for (const auto& t: vEv)
		if(t.Esr()==EsrKit(15))
			dep15=t.diap.getEnd();
	CPPUNIT_ASSERT(dep15 && " �������� 15 �������");
	for (const auto& t: norm3)
		if(t.Esr()==EsrKit(15))
			CPPUNIT_ASSERT(t.diap.getEnd()==dep15 && " ���� �� ����.����������� ����������");
}

void TC_AThread::testOutrunTechStation()
{// ����� �������� �� ���.������� ��� ������ ���������� 
	TRACE("\r\n =========== testOutrunTechStation ============");
	Sevent sEv1001(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		10, // ����� �� �������
		0,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv1003(//�������� ����
		AEvent::REAL,// ��� �������
		100,// ������ ����
		10, // ����� �� �������
		0,// ����� �� ��������
		0,// ����� �� ��������� ��������
		3, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm1(AEvent::NONE,// ��� �������
		290,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		5, // ���.������� (+10 � ������������)
		5// ����� �������
		);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		320,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		5, // ���.������� (+10 � ������������)
		5// ����� �������
		);
	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
	Strain trn1003( TrainDescr( L"1003", L"1-100-2"),sEv1003, Sevent( NO_FIXED ));
	TechnicalTime::Specification spec= TechnicalTime::MakeSpecification(trn1001.tdescr);
	long tblSt[]={15};
	map<EsrKit, TechnicalTime> techTimes;
	techTimes[EsrKit(15)].AddTime( TechnicalTime::SpecifiedTime( spec, TechnicalTime::Minutes( 1 ) ) );
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)1.0);
	tpl.setPvx(20,&defTimes);
	auto norm1=tpl.norma(tNrm1,1);
	auto norm2=tpl.norma(tNrm2,1);

	{// ��� ������� �������� �������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::checkOutrunTechStation=false;// ��������� ������ �� ���.��������
		adm.add(tpl.add(trn1003));//�� ��������
		adm.add(tpl.add(trn1001));//�� ��������
		adm.addNorma(TrainDescr( L"1001", L""), norm1 );
		adm.addNorma(TrainDescr( L"1003", L""), norm2 );
		// ����������  ���.�������
		for (const auto& t: tblSt)
		{
			LONG_VECTOR lv(1,t);
			lv.push_back(t+1);
			ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
			CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(1) && "����������� ���.�������");
		}

		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto allNrm=adm.getNorm();
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		M_INT_EV n15=fltrStatEv(15,allNrm);
		CPPUNIT_ASSERT( m15.size()==2 && n15.size()==2);
		CPPUNIT_ASSERT(m15[1001].diap.duration()>60 && m15[1003].diap.duration()>60);
		CPPUNIT_ASSERT(m15[1001].diap.enclose(m15[1003].diap));
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()== n15[1003].diap.getEnd());
		CPPUNIT_ASSERT(m15[1003].diap.getEnd()== n15[1001].diap.getEnd());
	}
	{// ��� ������� �������� �������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::checkOutrunTechStation=true;// ��������� ������ �� ���.��������
		adm.add(tpl.add(trn1003));//�� ��������
		adm.add(tpl.add(trn1001));//�� ��������
		adm.addNorma(TrainDescr( L"1001", L""), norm1 );
		adm.addNorma(TrainDescr( L"1003", L""), norm2 );
		// ����������  ���.�������
		for (const auto& t: tblSt)
		{
			LONG_VECTOR lv(1,t);
			lv.push_back(t+1);
			ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
			CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(1) && "����������� ���.�������");
		}

		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto allNrm=adm.getNorm();
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		M_INT_EV n15=fltrStatEv(15,allNrm);
		CPPUNIT_ASSERT( m15.size()==2 && n15.size()==2);
		CPPUNIT_ASSERT(m15[1001].diap.duration()>60 && m15[1003].diap.duration()>60);
		CPPUNIT_ASSERT(m15[1001].diap.enclose(m15[1003].diap.getOrig()));
		CPPUNIT_ASSERT(m15[1003].diap.enclose(m15[1001].diap.getEnd()));
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()== n15[1001].diap.getEnd());
		CPPUNIT_ASSERT(m15[1003].diap.getEnd()== n15[1003].diap.getEnd());
	}
	{// � �������� �������� �������
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// �-���  ������ ������� �� ������� ��������
		adm.add(tpl.add(trn1003));//�� ��������
		adm.add(tpl.add(trn1001));//�� ��������
		adm.addNorma(TrainDescr( L"1001", L""), norm1 );
		adm.addNorma(TrainDescr( L"1003", L""), norm2 );
		// ����������  ���.�������
		for (const auto& t: tblSt)
		{
			LONG_VECTOR lv(1,t);
			lv.push_back(t+1);
			ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
			CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(1) && "����������� ���.�������");
		}

		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		auto allNrm=adm.getNorm();
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==2);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		M_INT_EV n15=fltrStatEv(15,allNrm);
		CPPUNIT_ASSERT( m15.size()==2 && n15.size()==2);
		CPPUNIT_ASSERT(m15[1001].diap.duration()>60 && m15[1003].diap.duration()>60);
		CPPUNIT_ASSERT(m15[1001].diap.getOrig()<m15[1003].diap.getOrig());
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()<m15[1003].diap.getEnd());
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()== n15[1001].diap.getEnd());
		CPPUNIT_ASSERT(m15[1003].diap.getEnd()== n15[1003].diap.getEnd());
	}
}

void TC_AThread::testFixTechSt()
{// �������� �������� �� ���.�������
	TRACE("\r\n =========== testFixTechSt ============");
	Sevent sEv1(AEvent::REAL,20,10, /* ����� �� �������*/0,	0,2 /* ���.������� (+10 � ������������)*/,	1);// ����� �������
	Sevent sEv2(AEvent::REAL,200,10, /* ����� �� �������*/0,	0,5 /* ���.������� (+10 � ������������)*/,	1);// ����� �������

	Sevent fEv1(AEvent::FIXED,215/*������ ����*/,5/*����� �� �������*/,	20/*����� �� ��������*/,1/*������� ������� �� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Strain trn( TrainDescr( L"1001", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trnF( TrainDescr( L"1001", L"1-100-2"),sEv1, fEv1);
	Strain trn2( TrainDescr( L"1001", L"1-100-2"),sEv2, fEv1);
	TechnicalTime::Specification spec= TechnicalTime::MakeSpecification(trn.tdescr);
	long tblSt[]={15};
	map<EsrKit, TechnicalTime> techTimes;
	techTimes[EsrKit(15)].AddTime( TechnicalTime::SpecifiedTime( spec, TechnicalTime::Minutes( 1 ) ) );
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
 	{// ��� ��������
 		TestAAdmin adm;
 		adm.add(tpl.add(trn));//�� ��������
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
 		M_INT_EV m15= fltrStatEv(15,trFrc);
 		CPPUNIT_ASSERT(m15[1001].diap.duration()==techTimes[EsrKit(15)].GetTime(spec)->count()*60);
 	}
	{// � ���������
		TestAAdmin adm;
		adm.add(tpl.add(trnF,1,TC_Atopology::END));//�� ��������
		auto trn= adm.fnd(trnF.tdescr);
		AEvent evFix=trn->fndEv(EsrKit(15));
		CPPUNIT_ASSERT(!evFix.empty());
		CPPUNIT_ASSERT(evFix.getSourceExt()==AEvent::FIXED);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==1 && "���������� ������ ����.");
		CPPUNIT_ASSERT(adm.getClash().front()->Comment()==	ERROR_LNK_FIX_FRC);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		auto evId= trn->getIdeal(EsrKit(15));
		CPPUNIT_ASSERT(m15[1001].diap.getOrig()==evId->diap.getOrig());
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()==evFix.diap.getEnd());
		CPPUNIT_ASSERT(evId->diap.getEnd()!=evFix.diap.getEnd());
		CPPUNIT_ASSERT(evId->diap.getOrig()!=evFix.diap.getOrig());
	}
	{// � ��������� � ������
		TestAAdmin adm;
		adm.add(tpl.add(trn2,1,TC_Atopology::END));//�� ��������
		auto trn= adm.fnd(trn2.tdescr);
		AEvent evFix=trn->fndEv(EsrKit(15));
		CPPUNIT_ASSERT(!evFix.empty());
		CPPUNIT_ASSERT(evFix.getSourceExt()==AEvent::FIXED);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().size()==1 && "���������� ������ ����.");
		CPPUNIT_ASSERT(adm.getClash().front()->Comment()==	ERROR_LNK_FIX_FRC);
		M_INT_EV m15= fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m15[1001].diap.getEnd()==evFix.diap.getEnd());
		CPPUNIT_ASSERT(m15[1001].diap.getOrig()==trn->origExactFrc());
		CPPUNIT_ASSERT(m15[1001].diap.getOrig()!=evFix.diap.getOrig());
	}
}