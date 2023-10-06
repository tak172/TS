/*!
	created:	2017/07/05
	created: 	21:19  05 ���� 2017
	filename: 	D:\potapTrunk\Charm\UT\TC_nsi_ext.cpp
	file path:	D:\potapTrunk\Charm\UT
	file base:	TC_nsi_ext
	file ext:	cpp
	author:		 Dremin
	
	purpose:	����� ���������� ��� ��� ��������
  
  */
 
#include "stdafx.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/TrainRoutes.h"
#include "../Augur/AOutrun.h"
#include "../helpful/TechnicalTime.h"
#include "../helpful/RoadCode.h"
#include "../Augur/AClash.h"


using namespace std;

void TC_AThread::testProxyStation()
{// ���� ������ �������
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	tplParams.setDetalization( StationEsr(17), TC_AtopologyParams::StationDetalization( NsiBasis::StationKind::Blockpost ) );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		2, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv1(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		20, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		7, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		2,// ������ ����
		50,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.�������
		5// ����� �������
		);
	Sevent tNrm1(AEvent::NONE,// ��� �������
		2,// ������ ����
		50,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		7, // ���.�������
		5// ����� �������
		);
	auto norm1=tpl.norma(tNrm1,1);
	auto norm2=tpl.norma(tNrm2,2);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	Strain trn1003( TrainDescr( L"1003", L"1-100-2"),sEv1, Sevent( NO_FIXED ));
	TRACE("\r\n =========== testProxyStation no proxy ============");
	TestAAdmin adm;
	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,1));
	adm.add(tpl.add(trn1001,2));//�� ��������
	adm.add(tpl.add(trn1003,1));//�� ��������
	adm.addNorma( trn1001.tdescr, norm1 );
	adm.addNorma( trn1003.tdescr, norm1 );
	adm.addNorma( trn2.tdescr, norm2 );
	adm.addNorma( trn4.tdescr, norm2 );
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	adm.clearOccupy();
	//auto clash=adm.getClash();
	//CPPUNIT_ASSERT(clash.size()==2 && "���������� ������ ���� 2.");
	//Strain& trn=trn1001;
	//EsrKit eProxy(trn.evn[0].e1/*18*/);
	//auto fun=[&trn](const P_CLSH& t){return t->Trn()->tdescr==trn.tdescr && t->event()->Esr().getTerm()==trn.evn[0].e1 && t->event()->Esr().terminal();};
	//CPPUNIT_ASSERT(std::find_if(clash.begin(),clash.end(),fun)!=clash.end() && "���������� ��� �� 1001 ������ ����. ");
	//trn=trn1003;
	//CPPUNIT_ASSERT(std::find_if(clash.begin(),clash.end(),fun)!=clash.end() && "���������� ��� �� 1003 ������ ����. ");
	TRACE("\r\n =========== testProxyStation ============");

	//auto v1=adm.getNsi()->stationsRegPtr->getWays(eProxy);
	//adm.erase(eProxy);
	
	//auto v=adm.getNsi()->stationsRegPtr->getWays(eProxy);
	//CPPUNIT_ASSERT(!v1.empty() && v.empty() && "������� ������ ����� proxy");

	trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto clash=adm.getClash();
	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
	auto outr=adm.outrunInfo(false);
	std::wstring outRun= outr->text();
	outTRACE(outRun);

}

STT_Val getPmt(long eFrom,long eTo) 
{//   �������� ���������� ����� ����
	auto sp=TestAAdmin::getSpanTravelTime();
	auto STT = sp->GetSpanTime(SpanTravelTimeInfo( EsrKit(eFrom),EsrKit(eTo)));
	return STT;
}

void TC_AThread::testProxySpan()
{// ���� ������ ��������
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 2 ���� �� �������
	tplParams.spansWOWays.insert( EsrKit( 17, 18 ) );
	tplParams.spansWOWays.insert( EsrKit( 18, 19 ) );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		1,// ������ ����
		5, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv1001(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		30, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		8, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv1003(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		100, // ����� �� �������
		20,// ����� �� ��������
		10,// ����� �� ��������� ��������
		7, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		0,// ������ ����
		50,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.�������
		4// ����� �������
		);
	
	auto norm2=tpl.norma(tNrm2,2);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
	Strain trn1003( TrainDescr( L"1003", L"1-100-2"),sEv1003, Sevent( NO_FIXED ));
	TestAAdmin adm;
	adm.dispNormElSec=adm.dispNorm=0;//  ���������� ���������� �� ���������� ��� ��.������+ ���������� ���������� �� ���������� �� ��� ��.������
	adm.add(tpl.add(trn2,2));
	adm.add(tpl.add(trn4,1));
	adm.add(tpl.add(trn1001,2));//�� ��������
	adm.add(tpl.add(trn1003,1));//�� ��������
	adm.addNorma( trn2.tdescr, norm2 );
	adm.addNorma( trn4.tdescr, norm2 );
	TRACE("\r\n =========== testProxySpan ============");
	long tbl[]={17,18,19};
	for (int i = 0; i < size_array(tbl)-1; i++)
	{
		EsrKit t(tbl[i],tbl[i+1]);
	//	auto vA1= AAdmin::allPath<SpanWayInfo>(t);
	//	auto v1=adm.getNsi()->spansRegPtr->getWays(t);
	//	//adm.erase(t);
		auto v=adm.getNsi()->spansRegPtr->getWays(t);
	//	auto vA= AAdmin::allPath<SpanWayInfo>(t);
	//	CPPUNIT_ASSERT(!v1.empty() && v.empty() && vA.size()==1 && vA1.size()==1
	//		&& vA1.front()->Num()>0 && vA.front()->Num()==0
	//		&& "������� ����e� ����� proxy");
	}
	auto trFrc= adm.forecastGlobal();
	tstTRACE(adm); 
	auto clash=adm.getClash();
	CPPUNIT_ASSERT(clash.empty() && "���������� �� ������ ����.");
// 	CPPUNIT_ASSERT(!clash.front()->event() && clash.front()->Trn()->descr().GetNumber().getNumber()==1003 &&
// 		" ������� ������� �������� �� �������� � �������������� �����");
	auto train1001=adm.get(std::to_string(1001));
 	V_EVNT v1001=train1001->buildIdeal().first; 
	for (const auto& t: tbl)
	{
		auto evSt4=fndFrcEv(trFrc,4,t);
		auto evSt2=fndFrcEv(trFrc,2,t);
		if(t!=17)
			CPPUNIT_ASSERT(evSt2.diap==evSt4.diap);
		else
			CPPUNIT_ASSERT(evSt2.diap.getOrig()==evSt4.diap.getOrig() && abs(evSt2.diap.getEnd()-evSt4.diap.getEnd())> AAdmin::GapStop());
	}
	auto pvx= getPmt(17,18).pGruz;
	for (const auto& t: adm.getTrn())
		if(t->getNum()==1003)
		{
			auto ev=t->origUnionFrc();
			time_t tR=t->origExactFrc();
			CPPUNIT_ASSERT( ev.onSpan() && ev.diap.enclose(tR) && "����� �������� ���� �� ��������");
			CPPUNIT_ASSERT(ev.diap.duration()==(pvx->full+ pvx->start) && "����������� �� ������");
			break;
		}
	auto evSp=fndFrcEv(trFrc,1001,18,19);
	CPPUNIT_ASSERT(evSp.diap.duration()==(pvx->full+ pvx->start) && "����������� �� ������");
}

AEvent fndFrcEv(const TrainRoutes<AEvent>& f,int nTr,int esr, int e)
{
	AEvent res;
	auto lines=f.getSpotLines();
	auto frc=std::find_if(lines.begin(),lines.end(),Srch(nTr));
	if(frc!=lines.end())
		for (const auto& t: frc->second)
			if(t.Esr()==EsrKit(esr,e))
			{
				res=t;
				break;
			}
	return res;
}

void TC_AThread::testAccordPath()
{// ���� ������� ���� �� �������
	TRACE("\r\n =========== testAccordPath ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(5.f);
	tpl.setPvx(20,&defTimes);
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		1,// ������ ����
		100, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		9, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv6001(//�������� ����
		AEvent::REAL,// ��� �������
		1,// ������ ����
		20, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		8, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent sEv6003(//�������� ����
		AEvent::REAL,// ��� �������
		2,// ������ ����
		20, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		8, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm1(AEvent::NONE,// ��� �������
		0,// ������ ����
		50,// ����� �� �������
		30,// ����� �� ��������
		0,// ����� �� ��������� ��������
		8, // ���.�������
		4// ����� �������
		);
	Sevent sEv6005(//�������� ����
		AEvent::REAL,// ��� �������
		3,// ������ ����
		20, // ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		8, // ���.������� (+10 � ������������)
		1// ����� �������
		);

	auto norm1=tpl.norma(tNrm1,1);
	Strain trn2( TrainDescr( L"2", L"1-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn6001( TrainDescr( L"6001", L"1-100-2"),sEv6001, Sevent( NO_FIXED ));
	Strain trn6003( TrainDescr( L"6003", L"1-100-2"),sEv6003, Sevent( NO_FIXED ));
	Strain trn6005( TrainDescr( L"6005", L"1-100-2"),sEv6005, Sevent( NO_FIXED ));
	{
		TestAAdmin adm;
		TestAAdmin::dispNormElSec=0;//  ���������� ���������� �� ���������� ��� ��.������
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn6001,1));//�� ��������
		adm.add(tpl.add(trn6003,2));//�� ��������
		adm.add(tpl.add(trn6005,3));//�� ��������
		adm.addNorma( trn6001.tdescr, norm1 );
		adm.addNorma( trn6003.tdescr, norm1 );
		adm.addNorma( trn6005.tdescr, norm1 );
		auto frc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.size()==0 && "���������� �� ������ ����.");
		M_INT_EV m18=fltrStatEv(18,frc),m19=fltrStatEv(19,frc);
		CPPUNIT_ASSERT(m18[6001].diap.getEnd()<m18[2].diap.getOrig() && "������� ��� ������������ ���� �� 18");
		CPPUNIT_ASSERT(m19[2].diap.enclose(m19[6001].diap.getOrig()));
		CPPUNIT_ASSERT(m19[2].diap.getEnd()-m19[6001].diap.getOrig()>=defTimes.tsk.count());
	}
	{
		TestAAdmin adm;
		TestAAdmin::dispNormElSec=0;//  ���������� ���������� �� ���������� ��� ��.������
		TestAAdmin::useBackAdjust=true;
		ACounterInt::limit= 12;/* ����.����� ����� �������� ��� ���������� ������� �� ���������*/
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn6001,3));//�� ��������
		adm.add(tpl.add(trn6003,2));//�� ��������
		adm.add(tpl.add(trn6005,1));//�� ��������
		adm.addNorma( trn6001.tdescr, norm1 );
		adm.addNorma( trn6003.tdescr, norm1 );
		adm.addNorma( trn6005.tdescr, norm1 );
		auto frc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.size()==0 && "���������� �� ������ ����.");
		M_INT_EV m18=fltrStatEv(18,frc),m19=fltrStatEv(19,frc);
		CPPUNIT_ASSERT(m18[6001].diap.getEnd()<m18[2].diap.getOrig() && "������� ��� ������������ ���� �� 18");
		CPPUNIT_ASSERT(m19[2].diap.enclose(m19[6001].diap.getOrig()));
		CPPUNIT_ASSERT(m19[2].diap.getEnd()-m19[6001].diap.getOrig()<=defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m19[2].diap.getEnd()-m19[6001].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m18[6003].diap.enclose(m18[2].diap.getOrig()));
		CPPUNIT_ASSERT(m18[6003].diap.getEnd()-m18[2].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m18[6003].diap.getEnd()-m18[2].diap.getOrig()<=defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m18[6005].diap.getEnd()-m18[6003].diap.getEnd()>=defTimes.Iot.count());
		CPPUNIT_ASSERT(m18[6005].diap.getEnd()-m18[6003].diap.getEnd()<=defTimes.Iot.count()+ AAdmin::GapStop());
	}
	{
		TestAAdmin adm;
		TestAAdmin::dispNormElSec=0;//  ���������� ���������� �� ���������� ��� ��.������
		TestAAdmin::useBackAdjust=true;
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn6001,1));//�� ��������
		adm.add(tpl.add(trn6003,2));//�� ��������
		adm.add(tpl.add(trn6005,3));//�� ��������
		adm.addNorma( trn6001.tdescr, norm1 );
		adm.addNorma( trn6003.tdescr, norm1 );
		adm.addNorma( trn6005.tdescr, norm1 );
		auto frc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.size()==0 && "���������� �� ������ ����.");
		M_INT_EV m18=fltrStatEv(18,frc),m19=fltrStatEv(19,frc);
		CPPUNIT_ASSERT(m18[6001].diap.getEnd()<m18[2].diap.getOrig() && "������� ��� ������������ ���� �� 18");
		CPPUNIT_ASSERT(m19[2].diap.enclose(m19[6001].diap.getOrig()));
		CPPUNIT_ASSERT(m19[2].diap.getEnd()-m19[6001].diap.getOrig()<=defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m19[2].diap.getEnd()-m19[6001].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m18[6003].diap.enclose(m18[2].diap.getOrig()));
		CPPUNIT_ASSERT(m18[6003].diap.getEnd()-m18[2].diap.getOrig()>=defTimes.tsk.count());
		CPPUNIT_ASSERT(m18[6003].diap.getEnd()-m18[2].diap.getOrig()<=defTimes.tsk.count()+ AAdmin::GapStop());
		CPPUNIT_ASSERT(m18[6005].diap.getEnd()-m18[6003].diap.getEnd()>=defTimes.Iot.count());
		CPPUNIT_ASSERT(m18[6005].diap.getEnd()-m18[6003].diap.getEnd()<=defTimes.Iot.count()+ AAdmin::GapStop());
	}
	{
		TestAAdmin adm;
		TestAAdmin::dispNormElSec=0;//  ���������� ���������� �� ���������� ��� ��.������
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn6001,3));//�� ��������
		adm.add(tpl.add(trn6003,2));//�� ��������
		adm.add(tpl.add(trn6005,1));//�� ��������
		adm.addNorma( trn6001.tdescr, norm1 );
		adm.addNorma( trn6003.tdescr, norm1 );
		adm.addNorma( trn6005.tdescr, norm1 );
		auto frc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.size()==0 && "���������� �� ������ ����.");
		M_INT_EV m18=fltrStatEv(18,frc),m19=fltrStatEv(19,frc);
		CPPUNIT_ASSERT(m18[6001].diap.getEnd()<m18[2].diap.getOrig() && "������� ��� ������������ ���� �� 18");
		CPPUNIT_ASSERT(m19[2].diap.enclose(m19[6001].diap.getOrig()));
		CPPUNIT_ASSERT(m19[2].diap.getEnd()-m19[6001].diap.getOrig()>=defTimes.tsk.count());
	}
	struct P{
		int n1;
		int n2;
	}tP[]={ {1,3},{2,3},{1,2},{3,1},{3,2},{2,1}};
	for (const auto& tp:tP )
	{
		TestAAdmin adm;
		TestAAdmin::dispNormElSec=100;//  ���������� ���������� �� ���������� ��� ��.������
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn6001,tp.n2));//�� ��������
		adm.add(tpl.add(trn6003,tp.n1));//�� ��������
		adm.addNorma( trn6001.tdescr, norm1 );
		adm.addNorma( trn6003.tdescr, norm1 );
		auto frc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.size()==0 && "���������� �� ������ ����.");
		M_INT_EV m=fltrStatEv(18,frc);
		CPPUNIT_ASSERT(m[2].Num()!= m[6001].Num());
		CPPUNIT_ASSERT(m[2].Num()!= m[6003].Num());
		CPPUNIT_ASSERT(m[6003].Num()!= m[6001].Num());
		CPPUNIT_ASSERT(m[6003].diap.getEnd()==m[6001].diap.getEnd());
		int tblSt[]={19,20};
		for (const auto& t:tblSt )
		{
			m=fltrStatEv(t,frc);
			CPPUNIT_ASSERT(m[6001].diap==m[6003].diap && "������ ������ ���� �� ����������") ;
		}
	}
	for (const auto& tp:tP )
	{
		TestAAdmin adm;
		TestAAdmin::dispNormElSec=0;//  ���������� ���������� �� ���������� ��� ��.������
		adm.add(tpl.add(trn2,2));
		adm.add(tpl.add(trn6001,tp.n2));//�� ��������
		adm.add(tpl.add(trn6003,tp.n1));//�� ��������
		adm.addNorma( trn6001.tdescr, norm1 );
		adm.addNorma( trn6003.tdescr, norm1 );
		auto frc= adm.forecastGlobal();
		tstTRACE(adm);
		auto clash=adm.getClash();
		CPPUNIT_ASSERT(clash.size()==0 && "���������� �� ������ ����.");
		M_INT_EV m=fltrStatEv(18,frc);
		CPPUNIT_ASSERT(m[2].Num()!= m[6001].Num());
		CPPUNIT_ASSERT(m[2].Num()!= m[6003].Num());
		CPPUNIT_ASSERT(m[6003].Num()!= m[6001].Num());
		CPPUNIT_ASSERT(m[6003].diap.getEnd()- m[6001].diap.getEnd()==defTimes.Iot.count());
		int tblSt[]={19,20};
		for (const auto& t:tblSt )
		{
			m=fltrStatEv(t,frc);
			CPPUNIT_ASSERT(m[6003].diap.getEnd()-m[6001].diap.getEnd()>=defTimes.Iot.count() && "����� 6003 �� ���� �� ����������") ;
			CPPUNIT_ASSERT(m[6003].diap.getEnd()-m[6001].diap.getEnd()<=defTimes.Iot.count()+ AAdmin::GapStop()) ;
		}
	}

}

static map<EsrKit, TechnicalTime> makeTimeForTestTechStation()
{
	typedef TechnicalTime TT;
	RoadKit belorussiaRoad( RoadCode::belorussia,L"��������" );
	RoadKit latvianRoad( RoadCode::latvianNationalCode );
	map<EsrKit, TechnicalTime> techTimes;

	TT::Specification specification( Oddness::EVEN, TT::Fullness::Any, belorussiaRoad, latvianRoad );
	techTimes[EsrKit(12)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 2 ) ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, belorussiaRoad, latvianRoad );
	techTimes[EsrKit(13)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 3 ) ) );
	specification = TT::Specification( Oddness::EVEN, TT::Fullness::Any, belorussiaRoad, latvianRoad );
	techTimes[EsrKit(14)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 4 ) ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, belorussiaRoad, latvianRoad );
	techTimes[EsrKit(15)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 5 ) ) );

	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, latvianRoad, latvianRoad );
	techTimes[EsrKit(17)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 7 ) ) );
	specification = TT::Specification( Oddness::EVEN, TT::Fullness::Any, latvianRoad, latvianRoad );
	techTimes[EsrKit(18)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 8 ) ) );

	specification = TT::Specification( Oddness::EVEN, TT::Fullness::Any, latvianRoad, belorussiaRoad );
	techTimes[EsrKit(20)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 10 ) ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, latvianRoad, belorussiaRoad );
	techTimes[EsrKit(21)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 11 ) ) );
	specification = TT::Specification( Oddness::EVEN, TT::Fullness::Any, latvianRoad, belorussiaRoad );
	techTimes[EsrKit(22)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 12 ) ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, latvianRoad, belorussiaRoad );
	techTimes[EsrKit(23)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 13 ) ) );

	return techTimes;
}

void TC_AThread::testTechStation()
{// ���� ������� � ���.�������������
	RoadKit belorussiaRoad( RoadCode::belorussia,L"��������" );
	RoadKit latvianRoad( RoadCode::latvianNationalCode );
	// ���������, 3 ���� �� �������
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, makeTimeForTestTechStation() );
	TC_Atopology tpl( tplParams );

	LONG_VECTOR r(16,0);
	for (unsigned i=0;i<r.size();i++ )
		r[i]=i+10;

	TechnicalTime::Specification spec1f(Oddness::ODD,TechnicalTime::Fullness::Any, belorussiaRoad, latvianRoad )
		,spec2f(Oddness::EVEN,TechnicalTime::Fullness::Any, belorussiaRoad, latvianRoad )
		,spec1t(Oddness::ODD,TechnicalTime::Fullness::Any, latvianRoad, belorussiaRoad )
		,spec2t(Oddness::EVEN,TechnicalTime::Fullness::Any, latvianRoad, belorussiaRoad )
		,spec1(Oddness::ODD,TechnicalTime::Fullness::Any, latvianRoad, latvianRoad )
		,spec2(Oddness::EVEN,TechnicalTime::Fullness::Any, latvianRoad, latvianRoad );
	{
		auto m=ATrain::bldTechNecessStop(r,spec1,true);
		CPPUNIT_ASSERT(m.size()==1 && m.begin()->first==17 && m.begin()->second==7*60);
	}
	{
		auto m=ATrain::bldTechNecessStop(r,spec2,true);
		CPPUNIT_ASSERT(m.size()==1 && m.begin()->first==18 && m.begin()->second==8*60);
	}
	{
		auto m=ATrain::bldTechNecessStop(r,spec1f,true);
		auto it=m.begin();
		CPPUNIT_ASSERT(m.size()==2 && it->first==13 && it->second==3*60);
		it++;
		CPPUNIT_ASSERT( it->first==15 && it->second==5*60);
	}
	{
		auto m=ATrain::bldTechNecessStop(r,spec1t,true);
		auto it=m.begin();
		CPPUNIT_ASSERT(m.size()==2 && it->first==21 && it->second==11*60);
		it++;
		CPPUNIT_ASSERT( it->first==23 && it->second==13*60);
	}
	{
		auto m=ATrain::bldTechNecessStop(r,spec2f,true);
		auto it=m.begin();
		CPPUNIT_ASSERT(m.size()==2 && it->first==12 );
		it++;
		CPPUNIT_ASSERT( it->first==14 );
	}
	{
		auto m=ATrain::bldTechNecessStop(r,spec2t,true);
		auto it=m.begin();
		CPPUNIT_ASSERT(m.size()==2 && it->first==20 );
		it++;
		CPPUNIT_ASSERT( it->first==22 );
	}
}

void TC_AThread::testTypeEvIdealFrc()
{ // ���� ������� � ��������� ��������

	typedef TechnicalTime TT;
	map<EsrKit, TechnicalTime> techTimes;
	TT::Specification specification( Oddness::EVEN, TT::Fullness::Any, RoadKit(), RoadKit());
	techTimes[EsrKit(17)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 10 ) ) );
	specification = TT::Specification( Oddness::ODD, TT::Fullness::Any, RoadKit(), RoadKit());
	techTimes[EsrKit(18)].AddTime( TT::SpecifiedTime( specification, TT::Minutes( 30 ) ) );
	
	// ���������, 3 ���� �� �������
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);

	Sevent sEv1(AEvent::REAL ,5/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		2/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain tr1( TrainDescr( L"1001", L"102-100-2"),sEv1, Sevent( NO_FIXED ));
	Sevent sEv2(AEvent::REAL ,50/*������ ����*/,	10/*����� �� �������*/,20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		16/*���.������� (+10 � ������������)*/,1/*����� �������*/);
	Strain tr2( TrainDescr( L"1002", L"101-100-1"),sEv2, Sevent( NO_FIXED ));

	TestAAdmin adm;
	ATrainPtr thr1(tpl.add(tr1));
	thr1->prepare();
	V_EVNT vId1=thr1->buildIdeal().first;// ���������� �� ���
	ATrainPtr thr2(tpl.add(tr2));
	thr2->prepare();
	V_EVNT vId2=thr2->buildIdeal().first;// ���������� �� ���
	V_EVNT noMv;
	for (const auto& t: vId1)
		if(t.getType()!=AEvent::MOVE)
			noMv.push_back(t);
	CPPUNIT_ASSERT(noMv.size()==3 && noMv[0].Esr()==EsrKit(12) && noMv.front().getType()==AEvent::STOP_START);
	CPPUNIT_ASSERT(noMv[1].Esr()==EsrKit(18) && noMv[1].getType()==AEvent::STOP_START && noMv[1].diap.duration()==30*60);
	CPPUNIT_ASSERT(noMv[2].Esr()==EsrKit(20) && noMv[2].getType()==AEvent::STOP);
	noMv.clear();
	for (const auto& t: vId2)
		if(t.getType()!=AEvent::MOVE)
			noMv.push_back(t);
	CPPUNIT_ASSERT(noMv.size()==3 && noMv[0].Esr()==EsrKit(26) && noMv.front().getType()==AEvent::STOP_START);
	CPPUNIT_ASSERT(noMv[1].Esr()==EsrKit(17) && noMv[1].getType()==AEvent::STOP_START && noMv[1].diap.duration()==10*60);
	CPPUNIT_ASSERT(noMv[2].Esr()==EsrKit(10) && noMv[2].getType()==AEvent::STOP);
}

void TC_AThread::testNormOnTechStation()
{// ���� ����� �� ������� � ���.������������� �� ����������

	Sevent sEv1001(//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		10, // ����� �� �������
		0,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		1// ����� �������
		);
	Sevent tNrm1(AEvent::NONE,// ��� �������
		40,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent tNrm2(AEvent::NONE,// ��� �������
		35,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent tNrm4_1(AEvent::NONE,// ��� �������
		195,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent tNrm4_2(AEvent::NONE,// ��� �������
		220,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent tNrm7_1(AEvent::NONE,// ��� �������
		430,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		7, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent tNrm7_2(AEvent::NONE,// ��� �������
		435,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		7, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent tNrm20(AEvent::NONE,// ��� �������
		660,// ������ ����
		20,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		10, // ���.������� (+10 � ������������)
		2// ����� �������
		);

	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
	TechnicalTime::Specification spec= TechnicalTime::MakeSpecification(trn1001.tdescr);
	long tblSt[]={12,14,17,19};
	map<EsrKit, TechnicalTime> techTimes;
	for (const auto& t: tblSt)
		techTimes[EsrKit(t)].AddTime( TechnicalTime::SpecifiedTime( spec, TechnicalTime::Minutes( 20 + t ) ) );
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN, SpanLockType::BIDIR_AUTOBLOCK, techTimes ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes((float)1.0);
	tpl.setPvx(20,&defTimes);


	auto norm1=tpl.norma(tNrm1,1);
	auto norm2=tpl.norma(tNrm2,1);
	auto norm4_1=tpl.norma(tNrm4_1,1);
	auto norm4_2=tpl.norma(tNrm4_2,1);
	auto norm7_1=tpl.norma(tNrm7_1,1);
	auto norm7_2=tpl.norma(tNrm7_2,1);
	auto norm20=tpl.norma(tNrm20,1);
	TRACE("\r\n =========== testNormOnTechStation ============");

	TestAAdmin adm;
	adm.add(tpl.add(trn1001));//�� ��������
	adm.addNorma(TrainDescr( L"1101", L""), norm1 );
	adm.addNorma(TrainDescr( L"101", L""), norm2 );
	adm.addNorma(TrainDescr( L"1401", L""), norm4_1);
	adm.addNorma(TrainDescr( L"1411", L""), norm4_2);
 	adm.addNorma(TrainDescr( L"1701", L""), norm7_1);
 	adm.addNorma(TrainDescr( L"1771", L""), norm7_2);
 	adm.addNorma(TrainDescr( L"1201", L""), norm20);
	// ����������  ���.�������
	
	for (const auto& t: tblSt)
	{
		LONG_VECTOR lv(1,t);
		lv.push_back(t+1);
		ATrain::M_ST_TECH m=ATrain::bldTechNecessStop(lv,TechnicalTime::MakeSpecification(adm.fnd(trn1001)->descr()),true);
		CPPUNIT_ASSERT(m.size()==1 && std::chrono::seconds(m.begin()->second)==std::chrono::minutes(20+t) && "����������� ���.�������");
	}

	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
	auto allNrm=adm.getNorm();
	auto v=trFrc.getSpotLines();
	CPPUNIT_ASSERT(v.size()==1);
	class MyLambda
	{
	public:
		explicit MyLambda(long n) : init(n) { }
		bool operator ()(const AEvent& ev) { return !ev.onSpan() && ev.Esr().getTerm()==init; }
	private:
		long init;
	};
	auto tR=adm.fnd(trn1001);
	auto ev=  tR->origUnionFrc();
	auto it= std::find_if(v.front().second.begin(),v.front().second.end(),MyLambda(12));
	LONG_VECTOR lv(1,12);
	auto mS=ATrain::bldTechNecessStop(lv,spec,true);
	CPPUNIT_ASSERT(mS.empty() && "�� ��������� ������� � �������� �� �������������� ���.������������");
	lv.push_back(21);
	 mS=ATrain::bldTechNecessStop(lv,spec,true);
	CPPUNIT_ASSERT(mS.size()==1 && "���� �� ��������� ������� ��� ���.������������");
	auto tns=std::chrono::seconds(mS.begin()->second).count();
	//int zz=it->diap.duration()+ ev.diap.duration();
	CPPUNIT_ASSERT( it->diap.duration()<tns  && it->diap.duration()+ ev.diap.duration()==tns && 
		"��� ������ �� ����.�������. ������������ �������.");
	it= std::find_if(v.front().second.begin(),v.front().second.end(),MyLambda(14));
	
	lv.front()=14;
	mS=ATrain::bldTechNecessStop(lv,spec,true);
	tns=std::chrono::seconds(mS.begin()->second).count();
	CPPUNIT_ASSERT(it->diap.duration()==tns );
	it= std::find_if(v.front().second.begin(),v.front().second.end(),MyLambda(17));

	lv.front()=17;
	mS=ATrain::bldTechNecessStop(lv,spec,true);
	tns=std::chrono::seconds(mS.begin()->second).count();
	CPPUNIT_ASSERT(	it->diap.duration()==tns);
	
	lv.front()=19;
	it= std::find_if(v.front().second.begin(),v.front().second.end(),MyLambda(lv.front()));
	mS=ATrain::bldTechNecessStop(lv,spec,true);
	tns=std::chrono::seconds(mS.begin()->second).count();
	CPPUNIT_ASSERT(it->diap.duration()==tns );
	for (const auto& ev: v.front().second)
		if(!ev.onSpan() && std::find(tblSt,tblSt+size_array(tblSt),ev.Esr().getTerm())==(tblSt+size_array(tblSt)))
			CPPUNIT_ASSERT(ev.diap.duration()==AAdmin::deltaThrough() && "�� ������� �������� ��� �������");
}

void TC_AThread::testConfigNum()
{// ���� ������� ������������

	std::wstring contents=L"<Augur>"
		L"<Service>"
		L"<Broker>localhost:8672</Broker>"
		L"</Service>"
		L"<ForecastProperty>"
		L"<Time  StopBeforeWrongPath=\"8\" />"
		L"<NumberTrain>"
		L"<Norma  value=\"7-99, 5001-7998\" />"
		L"<Passenger PVX=\"1-698,901-998\"  Limit=\" 1-1998, 5001-5998, 7801-7998\" />"
		L"<Electrosec PVX=\"6001-7798\"  Limit=\"6001-7798\" />"
		L"<Diesel  PVX=\"1701-1899 , 7801 -7998\"  />"
		L"</NumberTrain>"
		L"</ForecastProperty>"
		L"</Augur>";
	attic::a_document configDoc;
	auto r=configDoc.load_wide(contents);
	CPPUNIT_ASSERT(r && configDoc.document_element().name_is("Augur"));
	attic::a_node root=configDoc.child("Augur");
	auto error=AAdmin::config(root.child("ForecastProperty"));
	CPPUNIT_ASSERT(error.empty() && "������ ������������ ��� Augur �� ������ ����� ������.");
	CPPUNIT_ASSERT(AAdmin::normEnable(99) && !AAdmin::normEnable(100) && AAdmin::normEnable(5555));
	CPPUNIT_ASSERT(ATrain::type(1890,PVX)==SttMove::Type::TIME_DIESEL && ATrain::type(1890,LIMIT_TXT)==SttMove::Type::TIME_PASS);
}

