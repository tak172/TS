/*!
	created:	2018/11/08
	created: 	11:26  08 Ноябрь 2018
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrFixLnk.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrFixLnk
	file ext:	cpp
	author:		 Dremin
	
	purpose: склейка фикс. и расчетного прогнозов	
  
  */

 #include "stdafx.h"
#include "TC_Thread.h"
#include "TC_data.h"
#include "../Augur/AClash.h"
#include "../Augur/TxtComment.h"
#include "../helpful/PrognosisErrorCode.h"
using namespace std;

void TC_AThread::testWarnOnSpanFix()
{// тест возникновения исключений на перегоне при фиксации
	TRACE("\r\n =========== testWarnOnSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
//	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::DOUBLE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,40/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent sEv2(	AEvent::REAL,45/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		8 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);

	Sevent fEv1(AEvent::FIXED,170/*начало нити*/,1/*время на станции*/,	40/*время на перегоне*/,1/*признак событий на перегоне*/,
		5 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent fEv2(AEvent::FIXED,205/*начало нити*/,15/*время на станции*/,	40/*время на перегоне*/,1/*признак событий на перегоне*/,
		5 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);

	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,fEv1);
	ATrainPtr tr1=tpl.add(trn1,1,TC_Atopology::BOTH);
	Strain trn2( TrainDescr( L"2", L"2-200-1"),sEv2,fEv2);
	ATrainPtr tr2=tpl.add(trn2,1,TC_Atopology::BOTH);
	{
		TestPossessions p( TestAAdmin::getNsi() );
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(!v.empty());
		auto it=std::find_if(v.begin(),v.end(),[](P_CLSH& t){return t->Comment()==ON_SPAN_INTERSECT;});//     Недопустимое пересечение поездов на перегоне."
		CPPUNIT_ASSERT(it!=v.end() && (*it)->event()->Esr()==EsrKit(15,16) && " Должно быть недопустимое пересечение поездов на перегоне" );
	}
	//нулевой указатель означает окно
	Stech tWnd( 1, 310 /*длительность*/, 1, 1/*park*/, 5/*e1*/,6/*e2*/,nullptr);
	tWnd.kmLen=.5;
	{
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.size()>=1);
		auto it=std::find_if(v.begin(),v.end(),[](P_CLSH& t){return t->Comment()==ON_SPAN_INTERSECT;});//     Недопустимое пересечение поездов на перегоне."
		CPPUNIT_ASSERT(v.front()->Comment()== INTO_SPAN_WND  && " Должно быть движение в окне на перегоне." );
		CPPUNIT_ASSERT(v.front()->event()->Esr()==EsrKit(15,16) );
	}
}
 
void TC_AThread::testWarnOnSpan()
{// тест возникновения исключений на перегоне
	TRACE("\r\n =========== testWarnOnSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,40/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent sEv2(	AEvent::REAL,45/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		4 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,Sevent(NO_FIXED));
	ATrainPtr tr1=tpl.add(trn1,1);
	Strain trn2( TrainDescr( L"2", L"2-200-1"),sEv2,Sevent(NO_FIXED));
	ATrainPtr tr2=tpl.add(trn2,1,TC_Atopology::BOTH);
	{
		TestPossessions p( TestAAdmin::getNsi() );
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(!v.empty());
		auto it=std::find_if(v.begin(),v.end(),[](P_CLSH& t){return t->Comment()==ON_SPAN_INTERSECT;});//     Недопустимое пересечение поездов на перегоне."
		CPPUNIT_ASSERT(it!=v.end() && (*it)->event()->Esr()==EsrKit(13,14) && " Должно быть недопустимое пересечение поездов на перегоне" );
	}
	//нулевой указатель означает окно
	Stech tWnd( 1, 310 /*длительность*/, 1, 1/*park*/, 3/*e1*/,4/*e2*/,nullptr);
	tWnd.kmLen=.5;
	{
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.size()==1);
		auto it=std::find_if(v.begin(),v.end(),[](P_CLSH& t){return t->Comment()==ON_SPAN_INTERSECT;});//     Недопустимое пересечение поездов на перегоне."
		CPPUNIT_ASSERT(v.front()->Comment()== INTO_SPAN_WND  && " Должно быть движение в окне на перегоне." );
		CPPUNIT_ASSERT(v.front()->event()->Esr()==EsrKit(13,14) );
	}
}

void TC_AThread::testLinkFixFrc()
{// тест склейки фикс. и расчетного прогнозов
	TRACE("\r\n =========== testLinkFixFrc ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	st_intervals_t def(1.f);
	def.tsk=chrono::seconds(5);
	tpl.setPvx(PVX_STANDART,&def);
	Sevent sEv1(	AEvent::REAL,40/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,0/*время на последнем перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent sEv2(	AEvent::REAL,45/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,0/*время на последнем перегоне*/,
		4 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);

	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,Sevent(NO_FIXED));
	ATrainPtr tr1=tpl.add(trn1,1);
	Stech tWnd( 150, 50 /*длительность*/, 1, 0/*park*/, 4/*e1*/,3/*e1*/,nullptr);
	TestPossessions p( TestAAdmin::getNsi() );
	p.setLimitation( tWnd, tpl );
 	{// изменяем тип события отправления+ прибытия
 		V_EVNT v;
 		v.push_back(AEvent(16));
 		v.back().diap.set(15,15);
 		v.back().setSource(AEvent::REAL);
 		v.back().setType(AEvent::MOVE);
 		v.push_back(AEvent(15,16));
 		v.back().diap.set(130,130);
 		v.back().setSource(AEvent::FIXED);
 		v.back().setType(AEvent::MOVE);
 		v.push_back(AEvent(15));
 		v.back().diap.set(130,130);
 		v.back().setSource(AEvent::FIXED);
 		v.back().setType(AEvent::MOVE);
 		v.push_back(AEvent(15,14));
 		v.back().diap.set(130,130);
 		v.back().setSource(AEvent::FIXED);
 		v.back().setType(AEvent::MOVE);
 		AThread tmp(v);
 		ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmp);
 		TestAAdmin adm(p,1);
 		TestAAdmin::noSort=true;
 		TestAAdmin::dispNorm=1;
 		adm.add(tr1);//ПЕ реальное
 		adm.add(tr2);//ПЕ реальное
 		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
 		tstTRACE(adm);
 		auto evTr=trFrc.getSpotLines();
 		CPPUNIT_ASSERT(evTr.size()==2 && evTr.front().first== trn1.tdescr && evTr.back().first== tr2->descr());
 		auto cv= adm.getClash();
 		CPPUNIT_ASSERT(cv.size()==0);
 		M_INT_EV m16=fltrStatEv(16,trFrc),m15=fltrStatEv(15,trFrc),m14=fltrStatEv(14,trFrc);
 		CPPUNIT_ASSERT(m14[1].diap.enclose(m14[2].diap.getOrig()));
 		CPPUNIT_ASSERT(m14[1].diap.getEnd()-m14[2].diap.getOrig()>= def.tsk.count());
 		CPPUNIT_ASSERT(m14[1].diap.getEnd()-m14[2].diap.getOrig()<= def.tsk.count()+ AAdmin::GapStop());
 		CPPUNIT_ASSERT(m14[2].diap.getEnd()>tWnd.or+tWnd.count);
 		CPPUNIT_ASSERT(m14[2].diap.getEnd()<tWnd.or+tWnd.count+AAdmin::GapStop());
 		auto pvx=adm.fnd(tr2->descr())->getPMT(AEvent(13,14));
 		ADiap d2(m15[2].diap.getEnd(),m14[2].diap.getOrig());
 		ADiap d1(m16[2].diap.getEnd(),m15[2].diap.getOrig());
 		CPPUNIT_ASSERT(abs(d2.duration()-(pvx.full+pvx.stop))<2);
 		CPPUNIT_ASSERT(abs(d1.duration()-(pvx.start+pvx.full))<2);
 	}
	{// изменяем тип события
		V_EVNT v;
		v.push_back(AEvent(14));
		v.back().diap.set(55,75);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(13,14));
		v.back().diap.set(146,146);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		v.push_back(AEvent(13));
		v.back().diap.set(158,171);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(13,12));
		v.back().diap.set(171,171);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmp);
		TestAAdmin adm(p);
		TestAAdmin::noSort=true;
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto evTr=trFrc.getSpotLines();
		CPPUNIT_ASSERT(evTr.size()==2 && evTr.front().first== trn1.tdescr && evTr.back().first== tr2->descr());
		auto cv= adm.getClash();
		CPPUNIT_ASSERT(cv.size()==1);
		CPPUNIT_ASSERT(cv.front()->Comment()==INTO_SPAN_WND);
		CPPUNIT_ASSERT(cv.front()->event()->Esr()==EsrKit(tWnd.e1,tWnd.e2));
		M_INT_EV m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m14[2].diap.enclose(m14[1].diap.getOrig()));
		auto pvx=adm.fnd(tr2->descr())->getPMT(AEvent(13,14));
		ADiap d2(m14[2].diap.getEnd(),m[2].diap.getOrig());
		ADiap d1(m[1].diap.getEnd(),m14[1].diap.getOrig());
		CPPUNIT_ASSERT(abs(d2.duration()-(pvx.start+pvx.full+pvx.stop))<2);
		CPPUNIT_ASSERT(abs(d1.duration()-(pvx.start+pvx.full))<2);
	}
	{// изменяем тип события
		V_EVNT v;
		v.push_back(AEvent(14));
		v.back().diap.set(55,75);
		v.back().setSource(AEvent::REAL);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(13,14));
		v.back().diap.set(150,150);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		v.push_back(AEvent(13));
		v.back().diap.set(150,201);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::STOP_START);
		v.push_back(AEvent(13,12));
		v.back().diap.set(201,201);
		v.back().setSource(AEvent::FIXED);
		v.back().setType(AEvent::MOVE);
		AThread tmp(v);
		ATrainPtr tr2=tpl.add(TrainDescr( L"2", L"1-100-1"),&tmp);
		TestAAdmin adm(p);
		TestAAdmin::noSort=true;
		adm.add(tr1);//ПЕ реальное
		adm.add(tr2);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto evTr=trFrc.getSpotLines();
		CPPUNIT_ASSERT(evTr.size()==2 && evTr.front().first== trn1.tdescr && evTr.back().first== tr2->descr());
		auto cv= adm.getClash();
		CPPUNIT_ASSERT(cv.size()==0);
		M_INT_EV m=fltrStatEv(13,trFrc),m14=fltrStatEv(14,trFrc);
		CPPUNIT_ASSERT(m[1].diap.enclose(m[2].diap.getOrig()));
		CPPUNIT_ASSERT(m14[1].getType()==AEvent::MOVE);
		auto pvx=adm.fnd(tr2->descr())->getPMT(AEvent(13,14));
		ADiap d2(m14[2].diap.getEnd(),m[2].diap.getOrig());
		ADiap d1(m[1].diap.getEnd(),m14[1].diap.getOrig());
		CPPUNIT_ASSERT(abs(d2.duration()-(pvx.start+pvx.full+pvx.stop))<2);
		CPPUNIT_ASSERT(abs(d1.duration()-(pvx.start+pvx.full))<2);
	}
}

void TC_AThread::testPrognosisFeedback()
{// построить данные для ответа

	TRACE("\r\n =========== testWarnOnSpan ============");
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,40/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent sEv2(	AEvent::REAL,45/*начало нити*/,60/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		4 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,Sevent(NO_FIXED));
	ATrainPtr tr1=tpl.add(trn1,1);
	Strain trn2( TrainDescr( L"2", L"2-200-1"),sEv2,Sevent(NO_FIXED));
	ATrainPtr tr2=tpl.add(trn2,1,TC_Atopology::BOTH);
	//нулевой указатель означает окно
	Stech tWnd( 1, 310 /*длительность*/, 1, 1/*park*/, 3/*e1*/,4/*e2*/,nullptr);
	tWnd.kmLen=.5;
	{
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		TestAAdmin adm(p);
		adm.add(tr1);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.size()==1);
		auto it=std::find_if(v.begin(),v.end(),[](P_CLSH& t){return t->Comment()==ON_SPAN_INTERSECT;});//     Недопустимое пересечение поездов на перегоне."
		CPPUNIT_ASSERT(v.front()->Comment()== INTO_SPAN_WND  && " Должно быть движение в окне на перегоне." );
		CPPUNIT_ASSERT(v.front()->event()->Esr()==EsrKit(13,14) );
		CPPUNIT_ASSERT(trFrc.getSpotLines().size()==1);
		auto vfr=trFrc.getSpotLines().front();
		auto itEv=std::find_if(vfr.second.begin(),vfr.second.end(),[](const AEvent&t){return t.Esr()==EsrKit(13,14);});
		CPPUNIT_ASSERT(itEv!=vfr.second.end());
		CPPUNIT_ASSERT(itEv->getFeedback());
		auto fd=itEv->getFeedback();
		CPPUNIT_ASSERT(fd);
		CPPUNIT_ASSERT(fd->getCode()==PrognosisErrorCode::SINGLE && fd->getInfo()==INTO_SPAN_WND);
	}
}

void TC_AThread::testFixSpeedupPass()
{// тест обгон фиксированного участка
	TRACE("\r\n =========== testFixSpeedupPass ============");
	TC_AtopologyParams tplParams( stationsCount, 4, 1, SpanBreadth::ONE_WAY_SPAN);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(PVX_STANDART);
	Sevent sEv1(	AEvent::REAL,30/*начало нити*/,10/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		3 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	Sevent sEv3(	AEvent::REAL,45/*начало нити*/,10/*время на станции*/,	20/*время на перегоне*/,1/*время на последнем перегоне*/,
		4 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);

	Sevent fEv1(AEvent::FIXED,120/*начало нити*/,1/*время на станции*/,	0/*время на перегоне*/,0/*признак событий на перегоне*/,
		5 /*нач.станция (+10 в конструкторе)*/,	1/*число станций*/);
	

	Strain trn1( TrainDescr( L"1", L"1-100-2"),sEv1,fEv1);
	ATrainPtr tr1=tpl.add(trn1,1,TC_Atopology::BOTH);
	Strain trn3( TrainDescr( L"3", L"2-200-2"),sEv3,Sevent( NO_FIXED ));
	ATrainPtr tr3=tpl.add(trn3,1,TC_Atopology::BOTH);
	//нулевой указатель означает окно
	Stech tWnd( 0, 310 /*длительность*/, 1, 1/*park*/, 5/*e1*/,6/*e2*/,nullptr);
	tWnd.kmLen=.5;
	{
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		TestAAdmin adm(p);
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.push_back(&AOutrunTaskLst::filtrPassEquPr);// ф-ция  отбора заданий по обгонам
		adm.add(tr1);//ПЕ реальное
		adm.add(tr3);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		M_INT_EV m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m15[3].diap.getEnd()<m15[1].diap.getEnd() && " обгон ликвидирован");
		CPPUNIT_ASSERT(m15[3].diap.getEnd()>tWnd.count && "обход окна");
		CPPUNIT_ASSERT(m15[3].diap.enclose(m15[1].diap.getOrig()));
	}
	{
		TestPossessions p( TestAAdmin::getNsi() );
		p.setLimitation( tWnd, tpl );
		TestAAdmin adm(p);
		TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
		TestAAdmin::fnFltr.clear();
		adm.add(tr1);//ПЕ реальное
		adm.add(tr3);//ПЕ реальное
		TrainRoutes<AEvent> trFrc= adm.forecastGlobal();
		tstTRACE(adm);
		auto v= adm.getClash();
		CPPUNIT_ASSERT(v.empty());
		M_INT_EV m15=fltrStatEv(15,trFrc);
		CPPUNIT_ASSERT(m15[3].diap.enclose(m15[1].diap) && " обгон");
		CPPUNIT_ASSERT(m15[1].diap.getEnd()>tWnd.count && "обход окна");
	}
}

