#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_StationsRegistry : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_StationsRegistry );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( DeserializeWays );
	CPPUNIT_TEST( DeserializeTechTime );
	CPPUNIT_TEST( DeserializePicketing );
	CPPUNIT_TEST( DeserializeCommon );
	CPPUNIT_TEST( LengthBetween );
	CPPUNIT_TEST( SerializeRoutes );
	CPPUNIT_TEST( DeserializeRoutes );
	CPPUNIT_TEST( TransroutePostprocess );
    CPPUNIT_TEST( SerializeBypass );
    CPPUNIT_TEST( DeserializeBypass );
    CPPUNIT_TEST( XtransBypass );
	CPPUNIT_TEST_SUITE_END();

	void Serialize();
	void DeserializeWays();
	void DeserializeTechTime();
	void DeserializePicketing();
	void DeserializeCommon();
	void SerializeRoutes();
	void DeserializeRoutes();
	void LengthBetween(); //расчет расстояния между осями станций
	void TransroutePostprocess(); //постобработка сквозных маршрутов при необходимости их ограничения перегонными кодами
    void SerializeBypass();   // сквозные проходы станции (с пути перегона на путь другого перегона минуя ПО-пути)
    void DeserializeBypass(); // сквозные проходы станции (с пути перегона на путь другого перегона минуя ПО-пути)
    void XtransBypass(); // конвертация частный путь перегона <-> обобщенный путь перегона
};