/*!
	created:	2018/03/21
	created: 	13:13  21 ���� 2018
	filename: 	F:\potap\Charm\UT\TC_ThrFix.cpp
	file path:	F:\potap\Charm\UT
	file base:	TC_ThrFix
	file ext:	cpp
	author:		 Dremin
	
	purpose:	
  
  */
 
 #include "stdafx.h"
 #include "TC_Thread.h"
#include "TC_data.h"
#include "../Augur/AClash.h"
#include "../Augur/TxtComment.h"


 V_EVNT fnd(const V_EVNT& v,const EsrKit& esr, AEvent::Source s)
{
	V_EVNT res;
	for (const auto& t: v)
		if((t.Esr()==esr || esr.empty()) && (t.getSource()==s || s==AEvent::NONE))
			res.push_back(t);
	return res;
}

void TC_AThread::testFixFrcConfl()
{// ���������� �������� � ������� �������������� �������� � ���������� ��� �������� � ����.���������
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,40/*������ ����*/,60/*����� �� �������*/,	20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent fEv1(	AEvent::FIXED,140/*������ ����*/,30/*����� �� �������*/,	40/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	2/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, fEv1);
	ATrainPtr trF=tpl.add(trn1,1);
	Stech tWnd( 1, 9999 /*������������*/, 1, 1/*park*/, 7/*e1*/,-10/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	Stech tW2( 1000, 10000 /*������������*/, 1, 1/*park*/, 6/*e1*/,-10/*e2*/, nullptr/*limit*/ );
	std::vector<RailLimitationCPtr> vR16(1,tpl.tech(tW2));
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tpl.fnd(tWnd), vR );
	p.setLimitation(tpl.fnd(tW2),vR16);
	TestAAdmin adm(p);
	adm.add(trF);//�� ��������
	V_EVNT fxEv;
	fxEv.push_back(trF->fndEv(EsrKit(15)));
	fxEv.push_back(trF->fndEv(EsrKit(16)));

	TRACE("\r\n =========== testFixFrc ============");
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto evTr=trFrc.getSpotLines();
	ATrainPtr  t1=adm.fnd(trn1.tdescr);
	auto clFix=t1->conflFix();// ������������ ��������� ��� �������� � ����.���������
	CPPUNIT_ASSERT(clFix.size()==2  && "������������ ��������� ��� �������� � ����.���������");
	CPPUNIT_ASSERT(clFix.find(EsrKit(16))!=clFix.end() );
	CPPUNIT_ASSERT(clFix.find(EsrKit(15))!=clFix.end() );
	auto clsh=adm.getClash();
	CPPUNIT_ASSERT(clsh.size()==2 && "���������� ��� ��������.");
	CPPUNIT_ASSERT( count_if(clsh.begin(),clsh.end(),[](const P_CLSH& t)
	{ return (t->event()->Esr()==EsrKit(15) ||t->event()->Esr()==EsrKit(16)) && t->Comment()==ERROR_LNK_FIX_FRC;})==2
	&& "���������� ��� ��������.");

	CPPUNIT_ASSERT(evTr.size()==1 );
	V_EVNT evF;
	std::for_each(evTr.front().second.begin(),evTr.front().second.end(),[&evF](const AEvent& ev){
		if(ev.getSource()==AEvent::FIXED)
			evF.push_back(ev);
	});
	CPPUNIT_ASSERT( evF.size()==2  && evF.front().Esr()==EsrKit(15) && evF.back().Esr()==EsrKit(16) &&" 2 ����.�������");
	CPPUNIT_ASSERT(evF.front().diap.getEnd()==fxEv.front().diap.getEnd() && " ����������� �� ����.��������");

	auto itSpan= std::find_if(evTr.front().second.begin(),evTr.front().second.end(),[](const AEvent& ev){
		return ev.Esr()==EsrKit(16,17);});

	CPPUNIT_ASSERT(itSpan!=evTr.front().second.end() && itSpan->diap.duration()>30 && itSpan->diap.duration()<50 && itSpan->diap.getEnd()>10000 && 
		itSpan->diap.getEnd()<10009 && "�������� ����� ���� ��  17 ");
}


void TC_AThread::testFixFrc()
{// ���������� �������� � ������� �������������� ��������
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,40/*������ ����*/,60/*����� �� �������*/,	20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent fEv1(	AEvent::FIXED,150/*������ ����*/,30/*����� �� �������*/,	40/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	2/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, fEv1);
	ATrainPtr trF=tpl.add(trn1,1);
	Stech tWnd( 1, 9999 /*������������*/, 1, 1/*park*/, 7/*e1*/,-10/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tpl.fnd(tWnd), vR );
	TestAAdmin adm(p);
	adm.add(trF);//�� ��������
	V_EVNT fxEv;
	fxEv.push_back(trF->fndEv(EsrKit(15)));
	fxEv.push_back(trF->fndEv(EsrKit(16)));

	TRACE("\r\n =========== testFixFrc ============");
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto evTr=trFrc.getSpotLines();
	ATrainPtr  t1=adm.fnd(trn1.tdescr);
	auto clFix=t1->conflFix();// ������������ ��������� ��� �������� � ����.���������
	CPPUNIT_ASSERT(clFix.empty() && "������������ ���������� ��� �������� � ����.��������� ���");
	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
	CPPUNIT_ASSERT(evTr.size()==1 );
	V_EVNT evF;
	std::for_each(evTr.front().second.begin(),evTr.front().second.end(),[&evF](const AEvent& ev){
		if(ev.getSource()==AEvent::FIXED)
			evF.push_back(ev);
	});
	CPPUNIT_ASSERT( evF.size()==2  && evF.front().Esr()==EsrKit(15) && evF.back().Esr()==EsrKit(16) &&" 2 ����.�������");
	CPPUNIT_ASSERT(evF.front().diap.getEnd()==fxEv.front().diap.getEnd() && " ����������� �� ����.��������");

	auto itSpan= std::find_if(evTr.front().second.begin(),evTr.front().second.end(),[](const AEvent& ev){
		return ev.Esr()==EsrKit(16,17);});

		CPPUNIT_ASSERT(itSpan!=evTr.front().second.end() && itSpan->diap.duration()>30 && itSpan->diap.duration()<50 && itSpan->diap.getEnd()>10000 && 
			itSpan->diap.getEnd()<10009 && "�������� ����� ���� ��  17 ");
}

void TC_AThread::testFixSimplFrc()
{// ���������� �������� �������� � ������� �������������� ��������
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,40/*������ ����*/,10/*����� �� �������*/,	20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent fEv1(	AEvent::FIXED,140/*������ ����*/,30/*����� �� �������*/,	40/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	2/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, fEv1);
	ATrainPtr trF=tpl.add(trn1,1);

	V_EVNT fxEv;
	fxEv.push_back(trF->fndEv(EsrKit(15)));
	fxEv.push_back(trF->fndEv(EsrKit(16)));
	TestAAdmin adm;
	adm.add(trF);//�� ��������

	TRACE("\r\n =========== testFixFrc ============");
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto evTr=trFrc.getSpotLines();
	CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
	CPPUNIT_ASSERT(evTr.size()==1 );
	int cF=0;
	size_t cIn=std::count_if(evTr.front().second.begin(),evTr.front().second.end(),[&cF,&fxEv](const AEvent& ev){
		if(ev.getSource()==AEvent::FIXED)
		{
			auto it=std::find_if(fxEv.begin(),fxEv.end(),[&ev](const AEvent& evF){return evF.Esr()==ev.Esr();});
			if (it!=fxEv.end() && ev.diap.enclose(it->diap))
				cF++;
			return true;
		}
		return false;
	});
	CPPUNIT_ASSERT( cF==cIn && cIn==2 && " 2 ����.�������");
	auto itSpan= std::find_if(evTr.front().second.begin(),evTr.front().second.end(),[](const AEvent& ev){
		return ev.Esr()==EsrKit(15,16);});
		CPPUNIT_ASSERT(itSpan!=evTr.front().second.end() && itSpan->diap.getOrig()==trF->fndEv(EsrKit(15)).diap.getEnd());
}

void TC_AThread::testFixFrcPoss()
{// ������� � ������� �������������� �������� � ���������� ��� ������� �����

	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,40/*������ ����*/,10/*����� �� �������*/,	20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent fEv1(	AEvent::FIXED,140/*������ ����*/,30/*����� �� �������*/,	40/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	2/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1, fEv1);
	ATrainPtr trF=tpl.add(trn1,1);
//-----------------------------------------
	V_EVNT fxEv;
	fxEv.push_back(trF->fndEv(EsrKit(15)));
	fxEv.push_back(trF->fndEv(EsrKit(16)));
//-------------------------------------------
	Stech tWnd( 1, 9999 /*������������*/, 1, 1/*park*/, 7/*e1*/,-10/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	Sevent fEv2(	AEvent::FIXED,1000/*������ ����*/,1000/*����� �� �������*/,	40/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv111(	AEvent::REAL,1000/*������ ����*/,10/*����� �� �������*/,	20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Strain trn111( TrainDescr( L"111", L""),sEv111, fEv2);
	ATrainPtr trN=tpl.add(trn111,1);

	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tpl.fnd(tWnd), vR );
	TestAAdmin adm(p,1);

	adm.add(trF);//�� ��������
	adm.add(trN);//�� ����.

	TRACE("\r\n =========== testFixFrcPoss ============");
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto evTr=trFrc.getSpotLines();
	auto clash=adm.getClash();
	CPPUNIT_ASSERT(clash.size()==2 && "���������� ��� �������� ������ ����.");
	CPPUNIT_ASSERT(clash[0]->event() && clash[0]->event()->Esr()==EsrKit(16) && clash[0]->event()->getSource()==AEvent::FIXED
		&& clash[0]->ConflTrn()==nullptr && clash[0]->Trn()->getNum()==1);
	CPPUNIT_ASSERT(clash[1]->event() && clash[1]->event()->Esr()==EsrKit(16) && clash[1]->ConflTrn()
		&& clash[1]->ConflTrn()->getNum()==111 && clash[1]->Trn()->getNum()==1);

	CPPUNIT_ASSERT(evTr.size()==2  && evTr[0].first==trn1.tdescr && evTr[1].first==trn111.tdescr);
	CPPUNIT_ASSERT(evTr[1].second.size()==1 && evTr[1].second.front().Esr()==EsrKit(16) );
	CPPUNIT_ASSERT(evTr[1].second.front().diap==ADiap(1000,2000) && evTr[1].second.front().getSource()==AEvent::FIXED);
// 	auto itSpan= std::find_if(evTr.front().second.begin(),evTr.front().second.end(),[](const AEvent& ev){
// 		return ev.Esr()==EsrKit(15,16);});
// 		CPPUNIT_ASSERT(itSpan!=evTr.front().second.end() && itSpan->diap.getOrig()==trF->fndEv(EsrKit(15)).diap.getEnd());
}
void TC_AThread::testAdvFix()
{// ���������� �������� � ��������� ������� � ����.��������� 
	TRACE("\r\n =========== testAdvFix ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	V_EVNT v(2,AEvent(16));
	for (auto& t:v )
	{
		t.setObj(1,1);
		t.setSource(AEvent::ADV);
		t.diap.set(10,10);
	}
	v[1].setSource(AEvent::FIXED);
	v[1].diap+=30;
	TrainDescr descr( L"1", L"1-100-2");
	ATrainPtr trF=tpl.add(descr,v);
	TestPossessions p( TestAAdmin::getNsi() );
// ---- ��������� ���� -----------
	Stech tWnd( 1, 9999 /*������������*/, 1, 1/*park*/, 7/*e1*/,-10/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	p.setLimitation( tpl.fnd(tWnd), vR );
	TestAAdmin adm(p);
	adm.add(trF);//�� ��������
	V_EVNT fxEv;
	fxEv.push_back(trF->fndEv(EsrKit(16)));

	ATrainPtr  t1=adm.fnd(descr);
	V_EVNT vFix=t1->get(AEvent::FIXED);
	CPPUNIT_ASSERT(vFix.size()==2 && vFix[0].Esr()==EsrKit(16)&& vFix[1].Esr()==EsrKit(16) && vFix[1].getSourceExt()==AEvent::FIXED && vFix.front().getSourceExt()==AEvent::ADV);
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto evTr=trFrc.getSpotLines();
	CPPUNIT_ASSERT(evTr.size()==1 );
	V_EVNT vOut;
	std::for_each(evTr[0].second.begin(),evTr[0].second.end(),[&vOut](const AEvent& t){
		if(t.Esr()==EsrKit(16))
			vOut.push_back(t);
	});
	CPPUNIT_ASSERT( vOut.size()==3 && " ����� �������� � ����.�������� �� ������� �������");
	CPPUNIT_ASSERT( vOut[0].getSourceExt()==AEvent::ADV && vOut[0].diap==ADiap(10,10) &&"������� � �������");
	CPPUNIT_ASSERT( vOut[1].getSourceExt()==AEvent::FIXED && vOut[1].diap==ADiap(40,40) &&"������� � �������");
	CPPUNIT_ASSERT( vOut[2].getSourceExt()==AEvent::FRC && vOut[2].diap.getEnd()<10009 && "�������� ����� ���� ��  17 ");
}

void TC_AThread::testAdvFrc()
{// ���������� �������� � ��������� �������
	TRACE("\r\n =========== testAdvFrc ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent advEv(	AEvent::ADV,10/*������ ����*/,1/*����� �� �������*/,	0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv(	AEvent::NONE,0/*������ ����*/,0/*����� �� �������*/,	0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		0 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	Strain trn1( TrainDescr( L"1", L"1-100-2"),Sevent(NO_REAL),advEv);
	ATrainPtr trF=tpl.add(trn1,1);
	Stech tWnd( 1, 9999 /*������������*/, 1, 1/*park*/, 7/*e1*/,-10/*e2*/, nullptr/*limit*/ );
	RailLimitationCPtr railLimitCPtr = tpl.tech( tWnd );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tpl.fnd(tWnd), vR );
	TestAAdmin adm(p);
	adm.add(trF);//�� ��������
	V_EVNT fxEv;
	fxEv.push_back(trF->fndEv(EsrKit(16)));

	ATrainPtr  t1=adm.fnd(trn1.tdescr);
	V_EVNT vFix=t1->get(AEvent::FIXED);
	CPPUNIT_ASSERT(vFix.size()==1 && vFix[0].Esr()==EsrKit(16) && vFix.front().getSourceExt()==AEvent::ADV);
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto evTr=trFrc.getSpotLines();
	auto clFix=t1->conflFix();// ������������ ��������� ��� �������� � ����.���������
	CPPUNIT_ASSERT(clFix.empty() && "������������ ���������� ��� �������� � ����.��������� ���");
	CPPUNIT_ASSERT(adm.getClash().size()==0);
	CPPUNIT_ASSERT(evTr.size()==1 );
	V_EVNT evF=fnd(evTr.front().second,EsrKit(),AEvent::FIXED);
	CPPUNIT_ASSERT( evF.size()==1  && evF.front().Esr()==EsrKit(16) && evF.back().getSourceExt()==AEvent::ADV &&" 1 ������");
	AEvent ev=evF.front();		
	V_EVNT evN=fnd(evTr.front().second,EsrKit(16),AEvent::NONE);
	CPPUNIT_ASSERT( evN.size()==2  && evN.back().getSourceExt()==AEvent::FRC && evN.front().diap.getEnd()<=evN.back().diap.getOrig() &&"������� � �������");
	CPPUNIT_ASSERT(evN.back().getType()==AEvent::START && evN.back().diap.duration()==0);
	evF=fnd(evTr.front().second,EsrKit(16,17),AEvent::NONE);
	CPPUNIT_ASSERT(evF.size()==1 &&  evF.front().diap.duration()>30 && evF.front().diap.duration()<50  && " ����� ������ ");
	CPPUNIT_ASSERT( evF.back().diap.getEnd()>10000 && evF.back().diap.getEnd()<10009 && "�������� ����� ���� ��  17 ");
}

void TC_AThread::testMoveFix()
{// ���������� �������� � ������������� �������������� 
	TRACE("\r\n =========== testMoveFix ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent fixEv(	AEvent::FIXED,100/*������ ����*/,1/*����� �� �������*/,	0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		6 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent sEv(	AEvent::REAL,10/*������ ����*/,4/*����� �� �������*/,	20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv,fixEv);
	ATrainPtr trF=tpl.add(trn1,1);
	TestAAdmin adm;
	adm.add(trF);//�� ��������
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto evTr=trFrc.getSpotLines();
	CPPUNIT_ASSERT(evTr.size()==1 && evTr.front().first==trn1.tdescr);
	V_EVNT v;
	std::for_each(evTr.front().second.begin(),evTr.front().second.end(),[&v](const AEvent& t){if(t.Esr()==EsrKit(16))
		v.push_back(t);
	});
	CPPUNIT_ASSERT(v.size()==2);
	CPPUNIT_ASSERT(v.front().getType()==AEvent::STOP/*_START*/ && v.front().getSource()==AEvent::FRC && v.front().diap.duration()==0/*fixEv.or*/ && v.front().diap.getOrig()<60);
	CPPUNIT_ASSERT(v.back().getType()==AEvent::MOVE && v.back().getSource()==AEvent::FIXED && v.back().diap.getOrig()==fixEv.or && v.back().diap.duration()==1);
}
 
void TC_AThread::testFixFrcSpan()
{// ���� �������� �� �� ��������
	TRACE("\r\n =========== testFixFrcSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 1, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv(	AEvent::REAL,10/*������ ����*/,4/*����� �� �������*/,	20/*����� �� ��������*/,3/*����� �� ��������� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	V_EVNT lnEv1,lnEv2;
	Sevent fixEv(	AEvent::FIXED,75/*������ ����*/,1/*����� �� �������*/,	0/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
	 	7 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	{
		Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv,fixEv);
		ATrainPtr trF=tpl.add(trn1,1);
		TestAAdmin adm;
		adm.add(trF);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto lnEv=trFrc.getSpotLines();
		CPPUNIT_ASSERT(lnEv.size()==1 && lnEv.front().first==trn1.tdescr);
		lnEv2=lnEv.front().second;
	}
	{
		Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv,Sevent(NO_FIXED)/*fixEv*/);
		ATrainPtr trF=tpl.add(trn1,1);
		TestAAdmin adm;
		adm.add(trF);//�� ��������
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto lEv=trFrc.getSpotLines();
		CPPUNIT_ASSERT(lEv.size()==1 && lEv.front().first==trn1.tdescr);
		lnEv1=lEv.front().second;
	}
	CPPUNIT_ASSERT(lnEv1.size()==lnEv2.size());
	for (uint i = 0; i < lnEv1.size(); i++)
	{
		const AEvent& ev1=lnEv1[i];
		V_EVNT::iterator it=std::find_if(lnEv2.begin(),lnEv2.end(),[&ev1](const AEvent& t){
				return t.Esr()==ev1.Esr();});
			CPPUNIT_ASSERT(it!=lnEv2.end());
			CPPUNIT_ASSERT(ev1.getSource()==AEvent::FRC );
			if(it->getSource()==AEvent::FRC)
				CPPUNIT_ASSERT(ev1==*it);
			else
				CPPUNIT_ASSERT(ev1.diap.enclose(it->diap));
		
	}
}

void TC_AThread::testFixSpan()
{// ���� ��������� ����.������� �� �������� ( ������������+ ����)
	TRACE("\r\n =========== testFixFrcSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv(	AEvent::REAL,10/*������ ����*/,4/*����� �� �������*/,	20/*����� �� ��������*/,3/*����� �� ��������� ��������*/,
		5 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent fixEv(	AEvent::FIXED,75/*������ ����*/,1/*����� �� �������*/,	10/*����� �� ��������*/,1/*������� ������� �� ��������*/,
		7 /*���.������� (+10 � ������������)*/,	2/*����� �������*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv,fixEv);

	Sevent sN(	AEvent::REAL,50/*������ ����*/,4/*����� �� �������*/,	20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		7 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Strain trn2( TrainDescr( L"3", L"1-100-2"),sN,Sevent(NO_FIXED));

	Sevent tNrm(AEvent::NONE,// ��� �������
		50,// ������ ����
		8,// ����� �� �������
		40,// ����� �� ��������
		0,// ����� �� ��������� ��������
		7, // ���.�������
		3// ����� �������
		);
	auto norm=tpl.norma(tNrm,1);
	ATrainPtr trF=tpl.add(trn1,1);
	ATrainPtr tr2=tpl.add(trn2,2);
	TestAAdmin adm;
	adm.add(trF);//�� ��������
	adm.add(tr2);
	adm.addNorma( trn2.tdescr, norm );
	TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	auto lnEv=trFrc.getSpotLines();
	CPPUNIT_ASSERT(lnEv.size()==2 );
	V_EVNT* v1=nullptr;
	CPPUNIT_ASSERT(lnEv.size()==2 );
	V_EVNT* v3=nullptr;
	for (auto& t: lnEv)
	{
		if(t.first==trn1.tdescr)
			v1=&t.second;
		if(t.first==trn2.tdescr)
			v3=&t.second;
	}
	CPPUNIT_ASSERT(v1 && v3);
	auto it1=std::find_if(v1->begin(),v1->end(),[](const AEvent& t){return t.Esr()==EsrKit(17);});
	auto it3=std::find_if(v3->begin(),v3->end(),[](const AEvent& t){return t.Esr()==EsrKit(17);});
	CPPUNIT_ASSERT(it1!=v1->end() && it3!=v3->end());
	CPPUNIT_ASSERT(it3->diap.enclose(it1->diap));
}

void TC_AThread::testFixOnSpan()
{// ���� ��������� ����.������� ��������� �� �������� ( ����)
	TRACE("\r\n =========== testFixOnSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN);
//	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv(	AEvent::REAL,10/*������ ����*/,4/*����� �� �������*/,	20/*����� �� ��������*/,3/*����� �� ��������� ��������*/,
		3 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent fixEv16(	AEvent::FIXED,200/*������ ����*/,5/*����� �� �������*/,	1/*����� �� ��������*/,1/*������� ������� �� ��������*/,
		6 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Sevent fixEv17(	AEvent::FIXED,265/*������ ����*/,5/*����� �� �������*/,	1/*����� �� ��������*/,1/*������� ������� �� ��������*/,
		7 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);

	Sevent s2(	AEvent::REAL,170/*������ ����*/,4/*����� �� �������*/,	20/*����� �� ��������*/,0/*����� �� ��������� ��������*/,
		8 /*���.������� (+10 � ������������)*/,	1/*����� �������*/);
	Strain trn2( TrainDescr( L"2", L"2-100-1"),s2,Sevent(NO_FIXED));
	{
		TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN);
		TC_Atopology tpl( tplParams );
		tpl.setPvx(PVX_STANDART);
		ATrainPtr tr2=tpl.add(trn2,2);
		Strain trn1( TrainDescr( L"1001", L"1-100-2"),sEv,fixEv17);
		ATrainPtr trF=tpl.add(trn1,1,TC_Atopology::BOTH);
		TestAAdmin adm;
		adm.add(trF);//�� ��������
		adm.add(tr2);
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
		ATrainPtr t1=adm.fnd(trn1);
		ATrainPtr t2=adm.fnd(trn2);
		CPPUNIT_ASSERT( t1->outrun(t2).empty() && " ������ �� 1001 �� ������� �� 2 � ��������� �� ������ ����") ;
		CPPUNIT_ASSERT( t2->outrun(t1).empty() && " ������ �� 2 �� �������  �� 1001  � ��������� �� ������ ����") ;
	}
	{
		TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN);
		//	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::DOUBLE_WAY_SPAN);
		TC_Atopology tpl( tplParams );
		tpl.setPvx(PVX_STANDART);
		ATrainPtr tr2=tpl.add(trn2,2);

		{	
			Strain trn1( TrainDescr( L"1001", L"1-100-2"),sEv,Sevent(NO_FIXED));
			ATrainPtr trF=tpl.add(trn1,1,TC_Atopology::END);
			TestAAdmin adm;
			adm.add(trF);//�� ��������
			adm.add(tr2);
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
			// 		tstTRACE(adm);
			auto lnEv=trFrc.getSpotLines();
			CPPUNIT_ASSERT(lnEv.size()==2 );
			ATrainPtr t1=adm.fnd(trn1);
			ATrainPtr t2=adm.fnd(trn2);
			CPPUNIT_ASSERT( t2->outrun(t1).empty() && " ������ �� 2 �� ������� �� 1001 ��� �������� �� ������ ����") ;
			long tx[]={16};
			CPPUNIT_ASSERT( t1->outrun(t2)==LONG_SET(ARRAY_IT(tx)) && " ����� �� 1001 �� �������  �� 2 ��� �������� ���������� �� ��. 16") ;
		}

		{	
			Strain trn1( TrainDescr( L"1001", L"1-100-2"),sEv,fixEv16);
			ATrainPtr trF=tpl.add(trn1,1,TC_Atopology::END);
			TestAAdmin adm;
			adm.add(trF);//�� ��������
			adm.add(tr2);
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			// 		tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
			ATrainPtr t1=adm.fnd(trn1);
			ATrainPtr t2=adm.fnd(trn2);
			CPPUNIT_ASSERT( t1->outrun(t2).empty() && " ������ �� 1001 �� ������� �� 2 � ��������� �� ������ ����") ;
			long tx[]={17};
			CPPUNIT_ASSERT( t2->outrun(t1)==LONG_SET(ARRAY_IT(tx)) && " ����� �� 2 �� �������  �� 1001  � ��������� ���������� �� ��. 17") ;
		}

		{	
			Strain trn1( TrainDescr( L"1001", L"1-100-2"),sEv,fixEv17);
			ATrainPtr trF=tpl.add(trn1,1,TC_Atopology::BOTH);
			TestAAdmin adm;
			adm.add(trF);//�� ��������
			adm.add(tr2);
			TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
			tstTRACE(adm);
			CPPUNIT_ASSERT(adm.getClash().empty() && "���������� �� ������ ����.");
			ATrainPtr t1=adm.fnd(trn1);
			ATrainPtr t2=adm.fnd(trn2);
			CPPUNIT_ASSERT( t1->outrun(t2).empty() && " ������ �� 1001 �� ������� �� 2 � ��������� �� ������ ����") ;
			long tx[]={17};
			CPPUNIT_ASSERT( t2->outrun(t1)==LONG_SET(ARRAY_IT(tx)) && " ����� �� 2 �� �������  �� 1001  � ��������� ���������� �� ��. 17") ;
		}
	}

}