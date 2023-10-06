#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_LOS_Route : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_Route );
  CPPUNIT_TEST( setTrainRouteByAllLocked );
  CPPUNIT_TEST( noTrainRouteByLockedNBusy );
  CPPUNIT_TEST( setShuntRouteByAllLocked );
  CPPUNIT_TEST( setShuntRouteByLockedNBusy );
  CPPUNIT_TEST_SUITE_END();

protected:
	void setTrainRouteByAllLocked(); //установка поездного маршрута по всем замкнутым участкам
	void noTrainRouteByLockedNBusy(); //нет установки поездного маршрута по замкнутым внутренним участкам и занятому последнему ПО-пути
	void setShuntRouteByAllLocked(); //установка маневрового маршрута по всем замкнутым участкам
	void setShuntRouteByLockedNBusy(); //установка маневрового маршрута по замкнутым внутренним участкам и занятому последнему ПО-пути
};