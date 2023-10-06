/*!
	created:	2016/12/21
	created: 	16:21  21 ������� 2016
	filename: 	D:\potap\Charm\UT\TC_Thread.cpp
	file path:	D:\potap\Charm\UT
	file base:	TC_Thread
	file ext:	cpp
	author:		 Dremin
	
	purpose:	
  
  */
 
 #include "stdafx.h"
#include "TC_data.h"
#include "TC_Thread.h"
#include "../Augur/AThread.h"
#include "../helpful/Attic.h"
#include "../Augur/ATrain.h"
#include "../helpful/NsiBasis.h"
#include "../Augur/TrainRoutes.h"
#include "../Augur/ALogWrn.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AThread );

TC_AThread::TC_AThread() : sd( SData::instance() ){}

typedef std::map<EsrKit,int> MAP_ESR;

static MAP_ESR bldMapObj(const V_EVNT& vR)
{
	MAP_ESR m;
	AEvent prev;
	for (const auto& t: vR)
	{
		if( !prev.equObj(t))
		{
			MAP_ESR::iterator it=m.find(t.Esr());
			if(it==m.end())
				m[t.Esr()]=1;
			else
				m[t.Esr()]++;
			prev=t;
		}
	}
	return m;
}

void TC_AThread::testFiltr()
{

Strain	tblTr3( Strain( TrainDescr( L"3", L"1-100-18" ),		
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		108,// ������ ����
		11,// ����� �� �������
		21,// ����� �� ��������
		0,// ����� �� ��������� ��������
		2, // ���.�������
		6// ����� �������
		),
		Sevent(
		// ����.�������
		AEvent::FIXED,// ��� �������
		140,// ������ ����
		20,// ����� �� �������
		15,// ����� �� ��������
		0,// ����� �� ��������� ��������
		5, // ���.�������
		5// ����� �������
		) ) );

	TC_AtopologyParams tplParams( stationsCount, 3, 2, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	{
		ATrainPtr thr(tpl.add(tblTr3));
		AThread tRaw(thr->get(AEvent::REAL));// �������� ���� �����.����
		AThread tFix(thr->get(AEvent::FIXED));// �������� ���� �����.����
		AThread tNone(thr->get(AEvent::NONE));// �������� ���� �����.����
		__int64 cNone=tNone.size();
		__int64 cReal=tRaw.size();
		__int64 cFixed=tFix.size();
		CPPUNIT_ASSERT(cNone==0 && cReal==((tblTr3.evn[0].nSt-1)*4 + 1+ (tblTr3.evn[0].tEndSp?3:0)) 
			/* 2 ������� �� ������� +2 �� ��������*/
			&& cFixed==tblTr3.evn[1].nSt);
	}
	{
		ATrainPtr thr(tpl.add(tblTr3));
		V_EVNT vR=thr->get(AEvent::REAL);
		long esrSt=0;
		int nSt=0;
		int nMove=0;
		int nStartStop=0;
		for (auto& t: vR)
		{
			if(!t.Esr().span())
			{
				if(t.Esr().getTerm()==esrSt)
				{
					if(nSt%2)
					{
						t.setType(AEvent::MOVE);
						nMove++;
						if(nMove%2)
							t.setObj(2,1);

					}
					else
					{
						nStartStop++;
						if(nStartStop%2)
							t.setObj(2,1);
					}
				}
				else
				{
					nSt++;
					esrSt=t.Esr().getTerm();
					if(nSt%3)
						t.setType(AEvent::MOVE);
				}
			}
		}

		AThread tRaw(vR);// �������� ���� �����.����
		V_EVNT vFix=thr->get(AEvent::FIXED);
		V_EVNT v= tRaw.compress();
		typedef std::map<EsrKit,int> MAP_ESR;
		MAP_ESR m0=bldMapObj(vR),m1=bldMapObj(v);
		CPPUNIT_ASSERT(m0==m1 /* 1 ������� �� ������� ������� + 1 �� ��������*/
			&& "���������� ������ �������");
	}
}

void TC_AThread::testTopology()
{
	TC_AtopologyParams tplParams( stationsCount, 3, 2, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	std::stringstream str;
	const auto & statCodes = tpl.getStatCodes();
	for ( const EsrKit & ecode : statCodes )
	{
		auto vw= TestAAdmin::getNsi()->stationsRegPtr->getWays( ecode );
		if(vw.size()!=3*2)
			str << " �� ������� N" << ecode.to_string() << " ����� ����� " << vw.size() << " (������ ���� 6)" << std::endl;
		for (int p = 1; p < 3; p++)
			for (int n = 1; n < 4; n++)
			{
				StatWayInfoCPtr way= TestAAdmin::getNsi()->stationsRegPtr->getWay(ecode,n,p);
				if( !way || way->Num()!=n || way->Park()!=p )
					str << " �� ������� N" << ecode.to_string() << " ��� ���� " << n << " �����" << p << std::endl;
			}
	}

	const auto & spanCodes = tpl.getSpanCodes();
	for ( const EsrKit & ecode : spanCodes )
	{
		auto vw= TestAAdmin::getNsi()->spansRegPtr->getWays( ecode );
		if(vw.size()!=2)
			str<< " �� �������� "<< ecode.to_string() << " ����� ����� " << vw.size() << " (������ ���� 2)" << std::endl;
		for (int n = 1; n < 3; n++)
		{
			SpanWayInfoCPtr way= TestAAdmin::getNsi()->spansRegPtr->getWay( ecode, n );
			if( !way || way->Num()!=n )
				str << " �� �������� "<< ecode.to_string() << " ��� ���� " << n << std::endl;
		}
	}
	
	ATrainPtr thr(tpl.add(sd.tblTr3));
	V_EVNT r= thr->get(AEvent::REAL);
	AEvent p,ps;
	for(const auto& t:r)
	{
		if(t.onSpan())
		{// �������
			if(!t.Esr().occured(ps.Esr().getTerm()))
				str<<"������� "<<t.Esr().getSpan().first<<","<<t.Esr().getSpan().second<<" ����� ��="<<p.Esr().getTerm()<<std::endl;
		}
		else
		{// �������
			if(p.Esr().span() && !p.Esr().occured(t.Esr().getTerm()))
				str<<"������� "<<p.Esr().getSpan().first<<","<<p.Esr().getSpan().second<<" ����� ��="<<t.Esr().getTerm()<<std::endl;
			ps=t;
		}
		p=t;
	}
	std::string t=str.str();
	CPPUNIT_ASSERT_MESSAGE( t.c_str(),t.empty());
}

static void tstRoute(LONG_VECTOR& r)
{
	long prev=0;
	for(LONG_VECTOR::iterator it=r.begin();it!=r.end();it++)
	{
		CPPUNIT_ASSERT(std::find(it+1,r.end(),*it)==r.end() && " � �������� ���� ���������");
		CPPUNIT_ASSERT( ( it==r.begin() || ( prev < *it ) && " �������� ������� �������") );
		prev=*it;
	}
}

 void TC_AThread::testBldIdeal()
 {// ���������� ���������� �������� ����������+���
	 TRACE("\r\n   ----  testBldIdeal  -----");
	 TC_AtopologyParams tplParams( stationsCount, 3, 2, SpanBreadth::DOUBLE_WAY_SPAN );
	 TC_Atopology tpl( tplParams );
	 tpl.setPvx(30);
#define  INDEX L"1-100-2"
#define  BORDER_STATION 2*10
	 auto norm2=tpl.norma(sd.tblNorma[0],2);
	 Sevent sN(// ����������
		 AEvent::NONE,// ��� �������
		 30,// ������ ����
		 100,// ����� �� �������
		 20,// ����� �� ��������
		 0,// ����� �� ��������� ��������
		 1, // ���.�������(+10 � ������������)
		 6 // ����� �������
		 ) ;
	 auto norm3=tpl.norma(sN,3);
	 Sevent sEv(//�������� ����
		 AEvent::REAL,// ��� �������
		 20,// ������ ����
		 15,// ����� �� �������
		 25,// ����� �� ��������
		 5,// ����� �� ��������� ��������
		 2, // ���.������� (+10 � ������������)
		 1// ����� �������
		 );
	 Strain trn( TrainDescr( L"3", INDEX/*L"1-100-2"*/ ),
			sEv, Sevent( NO_FIXED )
		 );
	 TestAAdmin adm;
	 {// �� ����������
		 ATrainPtr thr3(tpl.add(trn));
		 thr3->setNorm(&norm3);
		 V_EVNT vC= thr3->prepare();
		 TC_Atopology::tstNormalize(vC);
		 LONG_VECTOR r3=thr3->getRoute();
		 tstRoute(r3);
		 CPPUNIT_ASSERT( r3.front() == (trn.evn[0].e1) &&  r3.back() == max( (unsigned)norm3.back().Esr().getTerm(), (unsigned)BORDER_STATION) );
		 thr3->firstStepForecast();
		 V_EVNT vId=thr3->buildIdeal().first;// ���������� �� ����������( ���� ��� �� �� ���)
		 AEvent prev;
		 for (const auto& ev: vId)
			 if(!ev.onSpan())
			 {
				 std::stringstream ss;
				 ss<<"\r\n esr="<<ev.Esr().getTerm() <<"  �� �������� "<< (prev.empty()?0:ev.diap.getOrig()-prev.diap.getEnd())
					 <<"  �� ������� "<<ev.diap.getOrig() <<"-"<<ev.diap.getEnd() <<" {"<<ev.diap.duration() <<"} ";
				 TRACE(ss.str().c_str());
				 prev=ev;
			 }
		 bool endNorm=false;
		 STT_Cat_Val pvx=thr3->getPMT(AEvent(12,13));
		 time_t prevSt=0;
		 for (const auto& t: r3)
		 {
			 AEvent evSt=thr3->fndEv(t,0);// ����� ������� �� �����.������� + ����� �� ������� �������
			 V_EVNT::const_iterator it=std::find_if(vId.begin(),vId.end(),[t](const AEvent&ev){return ev.Esr()==EsrKit(t);});
			 V_EVNT::const_iterator itN=std::find_if(norm3.begin(),norm3.end(),[t](const AEvent&ev){return ev.Esr()==EsrKit(t);});
			 if(it!=vId.end())
			 {
				 CPPUNIT_ASSERT(!(endNorm && itN!=norm3.end()) && " �������� ������� �� ����������");
				 endNorm|=itN==norm3.end();
				 if(!endNorm)
				 {
					 if(it==vId.begin())
					 {
						 auto evR=thr3->evFrc(0,false);
						 auto evPR=thr3->evFrc(-1,false);
						 CPPUNIT_ASSERT(it->diap.getEnd() && !evR && "������� ���������� ������ ���������� �������� �� ������� �� ����������");
						 CPPUNIT_ASSERT(evPR->diap.getEnd() && "������� ��������� ������� � ���������� �������� �� ������� �� ����������");
					 }
					 else
						CPPUNIT_ASSERT(it->diap.duration()==itN->diap.duration() && "������� ���������� ������� � ��������� �������� �� ������� �� ����������");
					 CPPUNIT_ASSERT(it->diap.getEnd()==itN->diap.getEnd() && "������� ��������� ��������� ������� �� ������� �� ����������");
				 }
				 else
				 {// ��� ��������� �� ���
					 static bool firstPVX=true;
					CPPUNIT_ASSERT( (it->diap.getOrig()-prevSt)== (pvx.full+ (firstPVX?pvx.start:0)) && "������������ ���������� �������� �� ��� �� �������� " );
					CPPUNIT_ASSERT(it->diap.duration()==AAdmin::deltaThrough() && "������������� ������� � ��������� �������� �� ���");
					 firstPVX=false;
				 }
				 prevSt=it->diap.getEnd();
			 }
		 }
	 }
	 {// �� ���
		 ATrainPtr thr3(tpl.add(trn));
		 V_EVNT vC= thr3->prepare();// ���������� ��������.�������� ����
		 LONG_VECTOR r3=thr3->getRoute();
		 TC_Atopology::tstNormalize(vC);
		 tstRoute(r3);
		 const auto & statCodes = tpl.getStatCodes();
		 ASSERT( ( unsigned ) trn.evn[0].e1 < statCodes.size() );
		 CPPUNIT_ASSERT(r3.front() == trn.evn[0].e1 && r3.back() == BORDER_STATION );
		 V_EVNT vId=thr3->buildIdeal().first;// ���������� �� ���
		 const AEvent&evOr= thr3->origUnionFrc();// ������ ��� �������� � ������� �������(��������� � ����.�������� ��� �������� ��������)
		 long esrOr=evOr.onSpan()?0:evOr.Esr().getTerm();
		 bool fndFrc=false;
		 for (const auto& t: r3)
		 {
			 V_EVNT::const_iterator it=std::find_if(vId.begin(),vId.end(),[t](const AEvent&ev){return ev.Esr()==EsrKit(t);});
			 if(it!=vId.end())
			 {
				 fndFrc=true;
				 CPPUNIT_ASSERT(it->diap.duration()>0 && "������� ��������� ��������� ������� �� ������� �� ���");
				 if(t==esrOr)
				 {
					 CPPUNIT_ASSERT(it->diap.duration()+evOr.diap.duration()>AAdmin::deltaThrough() 
						 && it->diap.duration()<=AAdmin::deltaThrough() && "������������ ������ ���������� �������� �� ��� �� ������� " );
				 }
				 else
					 CPPUNIT_ASSERT(it->diap.duration()==AAdmin::deltaThrough() && "������� ������������� � ��������� �������� ����� ������� �� ���");
			 }
			 else
				 CPPUNIT_ASSERT(!fndFrc && "������ �������� � ��������� �������� �� ���");
		 }
	 }
	 {// �� ����������
		 trn.evn.front().or+=300;
		 ATrainPtr thr3(tpl.add(trn));
		 thr3->setNorm(&norm3);
		 V_EVNT vC= thr3->prepare();
		 TC_Atopology::tstNormalize(vC);
		 LONG_VECTOR r3=thr3->getRoute();
		 tstRoute(r3);
		 thr3->firstStepForecast();
		 V_EVNT vId=thr3->buildIdeal().first;// ���������� �� ����������( ���� ��� �� �� ���)
		 bool endNorm=false;
		 time_t prevSt=0;
		 STT_Cat_Val pvx=thr3->getPMT(AEvent(12,13));
		 for (const auto& t: r3)
		 {
			 AEvent evSt=thr3->fndEv(t,0);// ����� ������� �� �����.������� + ����� �� ������� �������
			 V_EVNT::const_iterator it=std::find_if(vId.begin(),vId.end(),[t](const AEvent&ev){return ev.Esr()==EsrKit(t);});
			 V_EVNT::const_iterator itN=std::find_if(norm3.begin(),norm3.end(),[t](const AEvent&ev){return ev.Esr()==EsrKit(t);});
			 if(it!=vId.end())
			 {
				 CPPUNIT_ASSERT(!(endNorm && itN!=norm3.end()) && " �������� ������� �� ����������");
				 endNorm|=itN==norm3.end();
				 if(!endNorm)
				 {
					 if(it==vId.begin())
					 {
						auto evR=thr3->origUnionFrc();
						CPPUNIT_ASSERT(evR.onSpan() && it->diap==itN->diap	&& "����������� ������� �� ���������� � ��������") ;
					 }
					 else
					 {
						 CPPUNIT_ASSERT(it->diap.duration()==itN->diap.duration() && "������� ���������� ������� � ��.�������� �� ������� �� ����������");
						 CPPUNIT_ASSERT(it->diap.getEnd()==itN->diap.getEnd() && "������� ��������� ��.������� �� ������� �� ����������");
					 }
				 }
				 else
				 {// ��� ��������� �� ���
					 static bool firstPVX=true;
					 CPPUNIT_ASSERT( (it->diap.getOrig()-prevSt)== (pvx.full+ (firstPVX?pvx.start:0)) && "������������ ���������� �������� �� ��� �� �������� " );
					 CPPUNIT_ASSERT(it->diap.duration()==AAdmin::deltaThrough() && "������������� ������� � ��������� �������� �� ���");
					 firstPVX=false;
				 }
				 prevSt=it->diap.getEnd();
			 }
		 }
	 }
 }
  
  void TC_Atopology::tstNormalize(V_EVNT& v)// �������� ������������ ������� �������
 {
	 if(!v.empty())
	 {
		 AEvent prev;
		 for (const auto& t: v)
		 {
			 if(prev.Esr().getSpan().first)
			 {
				 CPPUNIT_ASSERT((t.diap.getOrig()-prev.diap.getEnd()==1 || t.diap.getOrig()-prev.diap.getEnd()==0)&&
					 "��������� ������ � ��������") ;

				 CPPUNIT_ASSERT((t.onSpan()^prev.onSpan()) &&
					 "������� ����������� ������� - �������- �������� "); 

				 CPPUNIT_ASSERT((t.Esr().occured(prev.Esr().getTerm()) || prev.Esr().occured(t.Esr().getTerm())) &&
					 "������� �� �� �������� ������� "); 
			 }
			 prev=t;
		 }
	 }
 }

 int TC_Atopology::tstComplete(V_EVNT& v )
 {// ���������� ����� �������
	 int res=0;
	 if(!v.empty())
	 {
		 CPPUNIT_ASSERT(!v.back().onSpan() && !v.front().onSpan() && "������� ������ ���������� � ��������� �� �������");
		 AEvent prev;
		 for (const auto& t: v)
		 {
			 if(!t.onSpan())
				 res++;
			 if(prev.Esr().getSpan().first)
			 {
				 CPPUNIT_ASSERT((t.diap.getOrig()-prev.diap.getEnd()==1 || t.diap.getOrig()-prev.diap.getEnd()==0)&&
					 "��������� ������ � ��������") ;

				 CPPUNIT_ASSERT((t.onSpan()^prev.onSpan()) &&
					 "������� ����������� ������� - �������- �������� "); 

				 CPPUNIT_ASSERT((t.Esr().occured(prev.Esr().getSpan().first) || prev.Esr().occured(t.Esr().getTerm())) &&
					 "������� �� �� �������� ������� "); 
			 }
			 prev=t;
		 }
	 }
	 return res;
 }

void TC_AThread::testSemiLock_(bool dbl)
{
	TC_AtopologyParams tplParams( stationsCount, 3, 2, dbl ? SpanBreadth::DOUBLE_WAY_SPAN : SpanBreadth::ONE_WAY_SPAN, SpanLockType::SEMI_AUTOBLOCK );
	 TC_Atopology tpl( tplParams );
	 tpl.setPvx(PVX_STANDART);
	 TestAAdmin adm;
 	 adm.add(tpl.add(sd.tblSTr1));
 	 adm.add(tpl.add(sd.tblSTr3));
 	 adm.add(tpl.add(sd.tblSTr1003));
	 adm.add(tpl.add(sd.tblSTr2));
 	 adm.add(tpl.add(sd.tblSTr4));
 	 adm.add(tpl.add(sd.tblSTr1002));
	 TRACE("\r\n =========== testSemiLock ============");
	 tstTRACE(*adm.getNsi());
//	 tstTRACE(adm);
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	 tstTRACE(adm);
	 auto evTr=trFrc.getSpotLines();
	 auto spttPtr = TestAAdmin::getSpanTravelTime();
	 auto sttVal12 = spttPtr->GetSpanTime( SpanTravelTimeInfo( EsrKit( 1 ), EsrKit( 2 ) ) );
	 auto sttVal21 = spttPtr->GetSpanTime( SpanTravelTimeInfo( EsrKit( 2 ), EsrKit( 1 ) ) );
     int start12 = sttVal12.get_data((int) SttMove::Type::TIME_GOODS) ? sttVal12.get_data((int) SttMove::Type::TIME_GOODS)->start : 0;
     int start21 = sttVal21.get_data((int)  SttMove::Type::TIME_GOODS) ? sttVal21.get_data((int)  SttMove::Type::TIME_GOODS)->start : 0;
	 unsigned int tSpeedup=max(start12,start21);
	 st_intervals_t stInt= getInterval(StationEsr(1),StationEsr(2),adm.getNsi()->stIntPtr);
	 for (const auto& t: evTr)
	 {
		 chrono::seconds affinityMin(-1);
		 for (const auto& evt:t.second )
			 if(evt.onSpan())
			 {
				 ATrainPtr tp= adm.get(std::to_string(t.first.GetNumber().getNumber()));
				 AEvent evX=tp->fndEv(evt.Esr());
				 if(evt.diap==evX.diap)
				 {// ������� �� ����� �� ��������
					 int isT=0;
					 chrono::seconds affinity( -1 );
					 for(const auto& trn:adm.getTrn())
						 if (trn->getNum()!=t.first.GetNumber().getNumber())
						 {
							 AEvent ev=trn->fndEv(evt.Esr());
							 if(!ev.empty() && ev.equObj(evt))
							 {
								 isT++;
								 chrono::seconds dist=evt.diap.distance<chrono::seconds>(ev.diap);
								 affinity=(affinity < chrono::seconds( 0 )?dist:min( affinity,dist));
								 if(dbl)
									CPPUNIT_ASSERT(dist>=stInt.tp && "��������� ������� � ������ ��� ���");
								else
								{// ���������
									if(tp->getPassing(trn).first==PASS)
										CPPUNIT_ASSERT(dist>=stInt.tp && "��������� ������� � ������ �� ��������� ��� ���");
									else
										CPPUNIT_ASSERT(dist>=stInt.tnp && "��������� ������� ����.�������� �� ��������� ��� ���");
								}
							 }
						 }
					 affinityMin=(affinityMin<chrono::seconds( 0 )?affinity:min(affinity,affinityMin));
					if(dbl)
						CPPUNIT_ASSERT(isT && affinity>=stInt.tp && affinity<=stInt.tp+chrono::seconds( tSpeedup ) && " ��� �������������� ������� � ������");
					else// ���������
					{
//						TRACE("\r\n isT=%d affinity=%d",isT , affinity);
						CPPUNIT_ASSERT(isT && affinity>=min(stInt.tnp,min(stInt.tp,stInt.tsk) )
							&& " ������� ��������� ������� �� ��������� � ���");
					}
				 }
				 CPPUNIT_ASSERT(affinityMin<=max(stInt.tnp,max(stInt.tp,stInt.tsk))+ chrono::seconds( tSpeedup ) && 	" ������ ������� � ���"	);
			 }
	 }
 }

void TC_AThread::outTRACE(std::string t)
{
	while (!t.empty())
	{
		if(t.length()<400)
		{
			TRACE(t.c_str());
			break;
		}
		else
		{
			std::string tmp=t.substr(0,400);
			TRACE(tmp.c_str());
			t.erase(0,400);
		}
	}
}

void TC_AThread::outTRACE(std::wstring t)
{
	outTRACE(ALogWrn::convWstring(t));
}

void TC_AThread::testSemiLock()
{// ���� ��� ��� (������������������)
	st_intervals_t t;
	t.Ipr=chrono::minutes( 3 );
	t.tsk=chrono::minutes( 4 );
	t.Ipk=chrono::minutes( 7 );
	t.tbsk=chrono::minutes( 17 );
	t.tn=chrono::minutes( 15 );
	t.tnp=chrono::minutes( 16 );
	CPPUNIT_ASSERT(t.maxDeviation()==chrono::minutes( 17 ) && t.maxDeviationStat()==chrono::minutes( 16 ));
	t.tn=chrono::minutes( 25 );
	t.Ipk=chrono::minutes( 26 );
	CPPUNIT_ASSERT(t.maxDeviation()==chrono::minutes( 26 ) && t.maxDeviationStat()==chrono::minutes( 25 ));
	t.tpr=chrono::minutes( 33 );
	CPPUNIT_ASSERT(t.maxDeviation()==chrono::minutes( 33 ));
	testSemiLock_(false);
	testSemiLock_(true);
}

void TC_AThread::testEmptyTrain()
{// ���� ������ ���� ��
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	tpl.setPvx(30);
	Sevent tNrm(AEvent::NONE,// ��� �������
		30,// ������ ����
		410,// ����� �� �������
		20,// ����� �� ��������
		0,// ����� �� ��������� ��������
		10, // ���.�������
		10// ����� �������
		);
	Strain trn1001( TrainDescr( L"1001" ),
		Sevent(	//�������� ����
		AEvent::REAL,// ��� �������
		20,// ������ ����
		60,// ����� �� �������
		20,// ����� �� ��������
		5,// ����� �� ��������� ��������
		10, // ���.�������
		4// ����� �������
		),
		Sevent( NO_FIXED )
		) ;

	Strain trn1( TrainDescr( L"1" ),Sevent(NO_REAL),Sevent( NO_FIXED )) ;
	Strain trn2( TrainDescr( L"2" ),Sevent(NO_REAL),Sevent( NO_FIXED )) ;

	auto norm1=tpl.norma(tNrm,1);
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn2));//�� ������
		adm.add(tpl.add(trn1));//�� �� ���������� ��� �������� �������
		adm.add(tpl.add(trn1001));//�� ��������
		adm.addNorma( trn1.tdescr, norm1 );
		TRACE("\r\n =========== testEmptyTrain (������+�� ����������+��������) ============");
//		tstTRACE(adm);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);

		VCLASH vCl=adm.getClash();
		CPPUNIT_ASSERT(vCl.empty() && " �� ������ �������� ����������");
		adm.outLog();
		ALogWrn().write(vCl, std::wstring());

		auto evLn= trFrc.getSpotLines();
		CPPUNIT_ASSERT(evLn.size()==3  && std::count_if(evLn.begin(),evLn.end(),[](const std::pair<TrainDescr, V_EVNT>&v){return !v.second.empty();}) ==1 && " ������� ������ ���� �� 1-�� ������");
		auto it1001= std::find_if(evLn.begin(),evLn.end(),[](const TrainRoutes<AEvent>::EventLine& l){return l.first.GetNumber().getNumber()==1001;});

		CPPUNIT_ASSERT( it1001!=evLn.end() && " ������� ������ ���� �� ������ 1001");
		auto t1001=adm.get("1001");// �������� �����(��� ���� )
		AEvent ev1001=t1001->fndEv(EsrKit(24));// ����� ������� �� �����.����� 
		CPPUNIT_ASSERT(!ev1001.empty() );
	}
	{
		Strain trn1003( TrainDescr( L"1003" ),
			Sevent(	//�������� ����
			AEvent::REAL,// ��� �������
			80,// ������ ����
			60,// ����� �� �������
			20,// ����� �� ��������
			0,// ����� �� ��������� ��������
			10, // ���.�������
			4// ����� �������
			),
			Sevent( NO_FIXED )
			) ;
		TestAAdmin adm;
		adm.add(tpl.add(trn2));//�� ������
		adm.add(tpl.add(trn1));//�� �� ���������� ��� �������� �������
		adm.add(tpl.add(trn1001));//�� �������� + ����������
		adm.addNorma( trn1.tdescr, norm1 );
		adm.add(tpl.add(trn1003,2));//�� ��������
		TRACE("\r\n =========== testEmptyTrain ============");
		// 	tstTRACE(*adm.getNsi());
		// 	tstTRACE(adm);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);

		auto evLn= trFrc.getSpotLines();
		CPPUNIT_ASSERT(evLn.size()==4  && std::count_if(evLn.begin(),evLn.end(),[](const std::pair<TrainDescr, V_EVNT>&v){return !v.second.empty();}) ==1 && " ������� ������ ���� �� 1-�� ������");
		auto it1= std::find_if(evLn.begin(),evLn.end(),[](const TrainRoutes<AEvent>::EventLine& l){return l.first.GetNumber().getNumber()==1001;});
		CPPUNIT_ASSERT(it1!=evLn.end() && it1->second.size()==2 && "������ ���� ������� �� �� 1001, ������������ �� �������" );
		AEvent ev1=it1->second.front();
		AEvent ev2=it1->second.back();
		CPPUNIT_ASSERT( ev1.diap.getEnd()<=ev2.diap.getOrig() && ev1.onSpan() && !ev2.onSpan() && ev1.Esr().occured(ev2.Esr().getTerm()) &&  " ����� 1001 ������� �� �������");
	}
}

void TC_AThread::testCurrTime()
{// ���� ���������� � �������� ���� ������� � ������� �������� �������� 
	TRACE("\r\n =========== testCurrTime ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN ); // ���������, 3 ���� �� �������
	TC_Atopology tpl( tplParams );
	tpl.setPvx(20);
	st_intervals_t defTimes;
	Sevent sEv2(//�������� ����
		AEvent::REAL,// ��� �������
		5,// ������ ����
		10, // ����� �� �������
		20,// ����� �� ��������
		5,// ����� �� ��������� ��������
		6, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Sevent sEv1001(//�������� ����
		AEvent::REAL,// ��� �������
		120,// ������ ����
		12, // ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		4, // ���.������� (+10 � ������������)
		2// ����� �������
		);
	Strain trn2( TrainDescr( L"2", L"2-100-1"),sEv2, Sevent( NO_FIXED ));
	Strain trn1001( TrainDescr( L"1001", L"1-100-2"),sEv1001, Sevent( NO_FIXED ));
	TrainRoutes<AEvent> trFrc0(TrainRoutes<AEvent>::Type::ForecastIn);
	{
		TestAAdmin adm;
		adm.add(tpl.add(trn1001));//�� ��������
		adm.add(tpl.add(trn2));//�� ��������
		trFrc0= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(trFrc0.getSpotLines().size()==2 );
		CPPUNIT_ASSERT(trFrc0.getSpotLines().front().first==trn1001.tdescr && trFrc0.getSpotLines().back().first==trn2.tdescr);
		CPPUNIT_ASSERT(!trFrc0.getSpotLines().front().second.front().onSpan() && trFrc0.getSpotLines().back().second.front().onSpan());
	}
	struct MyStruct
	{
		std::wstring ntr;
		ADiap d;
	};
	std::vector<MyStruct> vs;
	for (const auto& t: trFrc0.getSpotLines())
	{
		MyStruct m;
		m.ntr=t.first.GetNumber().getString();
		m.d=t.second.front().diap;
		if(t.second.front().onSpan())
			m.d.setEnd(t.second[1].diap.getOrig());
		m.d.grow(1);
		vs.push_back(m);
	}

	for (const auto& t: vs)
	{
		for (time_t i=t.d.getOrig();i<=t.d.getEnd();i++ )
		{
			TestAAdmin adm(i);
			adm.add(tpl.add(trn1001));//�� ��������
			adm.add(tpl.add(trn2));//�� ��������
			auto trF= adm.forecastGlobal();
			AEvent or;
			for (const auto& tF: trF.getSpotLines())
			{
				if(tF.first.GetNumber().getString()==t.ntr)
				{
					or=tF.second.front();
					if(or.onSpan())
						or.diap.setEnd(tF.second[1].diap.getOrig());
				}
			}
			
			if(i<t.d.getEnd())
				CPPUNIT_ASSERT( t.d.getEnd()-or.diap.getEnd()<=1 && or.diap.getOrig()==max(i,t.d.getOrig()+1));
			else
				CPPUNIT_ASSERT(or.diap.getOrig()==t.d.getEnd());
		}
	}
}
