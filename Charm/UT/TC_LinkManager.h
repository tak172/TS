#pragma once

#include "TC_Project.h"

//ѕроверка св€зывани€ логических элементов

class CLogicDocumentTest;
class CLinkManager;
class CStationManager;
class CLObjectManager;
class EsrKit;
class TC_LinkManager : public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_LinkManager );
	CPPUNIT_TEST( StripJointLinking );
	CPPUNIT_TEST( LegHeadLinking );
	CPPUNIT_TEST( DepthSearching );
	CPPUNIT_TEST( StripRouteUnlinking );
	CPPUNIT_TEST( StripSemaUnlinking );
	CPPUNIT_TEST( CommutatorRouteUnlinking );
	CPPUNIT_TEST( SwitchStripUndefLinking );
	CPPUNIT_TEST( LinksDeficitForHeads );
	CPPUNIT_TEST( StripRouteLinking );
	CPPUNIT_TEST( StripWithStripPossibility );
	CPPUNIT_TEST_SUITE_END();

protected:
	void StripJointLinking();
	void LegHeadLinking();
	void DepthSearching();
	void StripRouteUnlinking();
	void StripSemaUnlinking();
	void CommutatorRouteUnlinking();
	void SwitchStripUndefLinking();
	void LinksDeficitForHeads(); //возвращение дефицитных линков дл€ светофорных голов разных типов
	void StripRouteLinking(); //св€зывание участка с маршрутом
	void StripWithStripPossibility(); //возможность св€зывани€ участков
};
