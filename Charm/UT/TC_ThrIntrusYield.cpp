/*!
	created:	2019/07/05
	created: 	18:57  05 Июль 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_ThrIntrusYield.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_ThrIntrusYield
	file ext:	cpp
	author:		 Dremin
	
	purpose:	уступка ПЕ по интрузии
  
  */
 
#include "stdafx.h"
#include "TC_Thread.h"
#include "TC_fabrClass.h" 
#include "TC_data.h"
#include "../Hem/RouteIntrusion.h"
using namespace std;
 
static const string intrusTestStr = 
	"<ArtificalRouteIntrusions>"\
	"<RouteIntrusion>"\
	"<TrainDescr index=\"2-100-1\" num=\"1\" >"\
	"</TrainDescr>"\
	"<Station esrCode=\"15\">"\
	"<OrderPriority>"\
	"<YieldFor>"\
	"<TrainDescr index=\"2-100-2\" num=\"2\" >"\
	"</TrainDescr>"\
	"</YieldFor>"\
	"</OrderPriority>"\
	"</Station>"\
	"<Station esrCode=\"14\">"\
	"<OrderPriority>"\
	"<YieldFor>"\
	"<TrainDescr index=\"2-100-2\" num=\"4\" >"\
	"</TrainDescr>"\
	"</YieldFor>"\
	"</OrderPriority>"\
	"</Station>"\
	"</RouteIntrusion>"
	"</ArtificalRouteIntrusions>";
 
void TC_AThread::testIntrusionYieldMany()
{//много заданий по отправлению после другого ПЕ
	TRACE("\r\n =========== testIntrusionYieldMany ============");
	TC_AtopologyParams tplParams( stationsCount, 3, 1, SpanBreadth::ONE_WAY_SPAN); // однопутка, 3 пути на станции
	Sevent sEv1(AEvent::REAL ,5/*начало нити*/,	10/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		6/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv2(AEvent::REAL ,30/*начало нити*/,	10/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		4/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);
	Sevent sEv4(AEvent::REAL ,70/*начало нити*/,	10/*время на станции*/,60/*время на перегоне*/,0/*время на последнем перегоне*/,
		2/*нач.станция (+10 в конструкторе)*/,1/*число станций*/);

	Strain trn1( TrainDescr( L"1", L"2-100-1"),sEv1, Sevent( NO_FIXED ));
	Strain trn2( TrainDescr( L"2", L"2-100-2"),sEv2, Sevent( NO_FIXED ));
	Strain trn4( TrainDescr( L"4", L"2-100-2"),sEv4, Sevent( NO_FIXED ));
	st_intervals_t defTimes(1.f);
	defTimes.Iot= chrono::seconds(5);
	defTimes.tsk= chrono::seconds(10);
	defTimes.Ipk=chrono::seconds(15);
	auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>( intrusTestStr);
	TC_Atopology tpl( tplParams );
	tpl.setPvx(60,&defTimes);
	TestAAdmin adm;
	TestAAdmin::noSort=true;// не сортируем ПЕ для точной  расстановки конфликтов
	TestAAdmin::deltaIntrusion= AAdmin::GapStop();// зазор для расчета при ликвидации обгона 
	TestAAdmin::useBackAdjust=true;// использовать обратную коррекцию после разрешения конфликта
	adm.add(tpl.add(trn1),routeIntensionsPtr);//ПЕ реальное
	adm.add(tpl.add(trn2));//ПЕ реальное
	adm.add(tpl.add(trn4));//ПЕ реальное
	auto trFrc= adm.forecastGlobal();
	tstTRACE(adm);
	CPPUNIT_ASSERT(adm.getClash().empty() && "Исключений не должно быть.");
	auto v=trFrc.getSpotLines();
	CPPUNIT_ASSERT(v.size()==3);
	M_INT_EV m15=fltrStatEv(15,trFrc),m14=fltrStatEv(14,trFrc);
	CPPUNIT_ASSERT(m15[4].getType()==AEvent::MOVE && m15[2].getType()==AEvent::MOVE);
	CPPUNIT_ASSERT(m14[4].getType()==AEvent::MOVE );
	CPPUNIT_ASSERT(m15[1].getType()==AEvent::STOP_START && m14[1].getType()==AEvent::STOP_START);
	CPPUNIT_ASSERT(m15[1].diap.enclose(m15[2].diap.getOrig()));
	CPPUNIT_ASSERT(abs(m15[2].diap.getEnd()+defTimes.tsk.count() -m15[1].diap.getEnd())< AAdmin::GapStop());
	CPPUNIT_ASSERT(m14[1].diap.enclose(m14[4].diap.getOrig()));
	CPPUNIT_ASSERT(abs(m14[4].diap.getEnd()+defTimes.tsk.count() -m14[1].diap.getEnd())< AAdmin::GapStop());
}

void TC_AThread::testGenerateIntrusion()
{// генерация заданий по обгонам при неразрешимом конфликте с путями на станции
	TRACE("\r\n =========== testGenerateIntrusion ============");
	// однопутка, 2 пути на станции
	TC_AtopologyParams tplParams( stationsCount, 2, 1, SpanBreadth::ONE_WAY_SPAN );
	TC_Atopology tpl( tplParams );
	st_intervals_t defTimes(1.f);
	defTimes.tsk=chrono::seconds(5);
	// 	defTimes.Iot=chrono::seconds(12);
	tpl.setPvx(60,&defTimes);
	// нормативки
// на подходе
	Sevent sN1(AEvent::NONE,0/*начало нити*/,2/*на станции*/,30/*на перегоне*/,0,3/*нач.станция*/,8/*число станций*/) ;
	Sevent sN2(AEvent::NONE,0/*начало нити*/,2/*на станции*/,30/*на перегоне*/,0,6/*нач.станция*/,5/*число станций*/) ;
	auto norm1=tpl.norma(sN1,1);
	auto norm2=tpl.norma(sN2,2);
	//реальная нить
	Sevent sEv1(AEvent::REAL,0/*начало нити*/,1/*на станции*/,20/*на перегоне*/,0,3/*нач.станция*/,1/*число станций*/) ;
	Sevent sEv2(AEvent::REAL,0/*начало нити*/,1/*на станции*/,20/*на перегоне*/,0,6/*нач.станция*/,1/*число станций*/) ;
	Strain trn1( TrainDescr( L"1", L"1-1-2" ),	sEv1, Sevent( NO_FIXED));
	Strain trn2( TrainDescr( L"2", L"1-1-1" ),	sEv2, Sevent( NO_FIXED));
// на станции
	Sevent sN3(AEvent::NONE,0/*начало нити*/,2/*на станции*/,70/*на перегоне*/,0,5/*нач.станция*/,3/*число станций*/) ;
	Sevent sN4(AEvent::NONE,0/*начало нити*/,2/*на станции*/,60/*на перегоне*/,0,5/*нач.станция*/,3/*число станций*/) ;
	auto norm3=tpl.norma(sN3,1);
	auto norm4=tpl.norma(sN4,2);
	//реальная нить
	Sevent sEv3(AEvent::REAL,0/*начало нити*/,10/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,1/*число станций*/) ;
	Sevent sEv4(AEvent::REAL,0/*начало нити*/,10/*на станции*/,20/*на перегоне*/,0,5/*нач.станция*/,1/*число станций*/) ;
	Strain trn3( TrainDescr( L"3", L"" ),	sEv3, Sevent( NO_FIXED));
	Strain trn4( TrainDescr( L"4", L"" ),	sEv4, Sevent( NO_FIXED));

	{
		TestAAdmin adm;
		TestAAdmin::noSort=true;
		TestAAdmin::dispNorm=1;//  допустимое отклонение от нормативки не для эл.секций
		TestAAdmin::deltaIntrusion=10;
		adm.add(tpl.add(trn1,1));//ПЕ реальное
		adm.add(tpl.add(trn2,2));//ПЕ реальное
		adm.add(tpl.add(trn3,1));//ПЕ реальное
		adm.add(tpl.add(trn4,2));//ПЕ реальное

		adm.addNorma(TrainDescr( L"1", L""), norm1 );
		adm.addNorma(TrainDescr( L"2", L""), norm2 );
		adm.addNorma(TrainDescr( L"3", L""), norm3 );
		adm.addNorma(TrainDescr( L"4", L""), norm4 );
		auto v=adm.forecastGlobal();
		tstTRACE(adm);
		auto m14=fltrStatEv(14,v),m15=fltrStatEv(15,v),m16=fltrStatEv(16,v);
		CPPUNIT_ASSERT(m14[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m14[4].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m15[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m15[2].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m15[4].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m15[4].diap.enclose(m15[1].diap.getOrig()));
		CPPUNIT_ASSERT(m15[4].diap.getEnd()<=m15[1].diap.getOrig()+defTimes.tsk.count()+AAdmin::GapStop());
		CPPUNIT_ASSERT(m15[4].diap.getEnd()>=m15[1].diap.getOrig()+defTimes.tsk.count());

		CPPUNIT_ASSERT(m16[1].getType()==AEvent::MOVE);
		CPPUNIT_ASSERT(m16[2].getType()==AEvent::STOP_START);
		CPPUNIT_ASSERT(m16[3].getType()==AEvent::STOP_START);
		
		CPPUNIT_ASSERT(m16[2].diap.enclose(m16[1].diap.getOrig()));
		CPPUNIT_ASSERT(m16[2].diap.getEnd()<=m16[1].diap.getOrig()+defTimes.tsk.count()+AAdmin::GapStop());
		CPPUNIT_ASSERT(m16[2].diap.getEnd()>=m16[1].diap.getOrig()+defTimes.tsk.count());

		CPPUNIT_ASSERT(m16[3].diap.enclose(m16[1].diap));
		CPPUNIT_ASSERT(m16[3].diap.enclose(m16[2].diap.getEnd()));
		CPPUNIT_ASSERT(m16[3].diap.getEnd()<=m16[2].diap.getEnd()+AAdmin::GapStop());
	}
}