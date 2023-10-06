#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"
#include "../StatBuilder/Router.h"

class CRouter;
class TC_Router : 
	public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_Router );
	CPPUNIT_TEST( SimpleBlindRoute ); 
	CPPUNIT_TEST( SimpleSemaRoute );
	CPPUNIT_TEST( OutrouteAlienEsr );
	CPPUNIT_TEST( OutrouteApproachingType );
	CPPUNIT_TEST( OutrouteApproachingTypeTwice );
	CPPUNIT_TEST( DoubleLegsInJointRoute );
	CPPUNIT_TEST( BlindRouteToOppositeLeg );
	CPPUNIT_TEST( ArrDepPermission );
	CPPUNIT_TEST( ArrDepForbidness );
	CPPUNIT_TEST( RouteThroughSwitch );
	CPPUNIT_TEST( InterstationsSemaroute );
	CPPUNIT_TEST( WOSemaStationRouteForbid );
	CPPUNIT_TEST( WOSemaStationRouteForbid2 );
	CPPUNIT_TEST( WOSemaStationRouteUnforbid );
	CPPUNIT_TEST( InterstatBoundSemaRoutes );
	CPPUNIT_TEST_SUITE_END();

protected:
	void SimpleBlindRoute(); //тупиковый маршрут с одним участком
	void SimpleSemaRoute(); //светофорный маршрут с одним участком
	void OutrouteAlienEsr(); //выходной маршрут (на станцию с другим ЕСР-кодом)
	void OutrouteApproachingType(); //выходной маршрут на участок с типом "приближения"
	void OutrouteApproachingTypeTwice(); //выходной маршрут на два участка с типом "приближения"
	void DoubleLegsInJointRoute(); //два тупиковых маршрута из двух мачт на одном стыке
	void BlindRouteToOppositeLeg(); //тупиковый маршрут с окончанием на встречной мачте
	void ArrDepPermission(); //просмотр маршрутов на наличие и порядок ПО путей
	void ArrDepForbidness(); //просмотр маршрутов на наличие и порядок ПО путей
	void RouteThroughSwitch(); //маршрут через стрелку
	void InterstationsSemaroute(); //светофорный маршрут между станциями
	void WOSemaStationRouteForbid(); //запрет прохождения маршрута через станцию без светофоров
	void WOSemaStationRouteForbid2();
	void WOSemaStationRouteUnforbid(); //построение маршрута со светофороного стыка, граничащего со станцией без светофоров, но ведущего в сторону светофорной станции
	void InterstatBoundSemaRoutes(); //маршруты от светофора на границе двух станций

private:
	CRouter router;
};
