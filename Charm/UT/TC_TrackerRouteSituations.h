#pragma once

//работа отслеживания на задании/отмене маршрутов

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerRouteSituations : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerRouteSituations );
	CPPUNIT_TEST( ParkMigrationByShortShuntRoute );
	CPPUNIT_TEST( ParkMigrationByShortTrainRoute );
	CPPUNIT_TEST( ParkMigrationWithoutRoutes );
	CPPUNIT_TEST( ParkMigrationByOnOffTrainRoute );
	CPPUNIT_TEST( SplitIntoSettledRoute );
	CPPUNIT_TEST( ParkMigrationOnCuttingRoute );
	CPPUNIT_TEST( UnsetRouteWithBPMigrations );
	CPPUNIT_TEST( UnsetRouteWithBPMigrations2 );
	CPPUNIT_TEST( SimultHeadCloseAndADOut );
	CPPUNIT_TEST( DepartureWithIrregularBusyOnRoute );
	CPPUNIT_TEST_SUITE_END();

	//переезд поезда в парк по простому маневровому маршруту
	void ParkMigrationByShortShuntRoute();
	//переезд поезда в парк по простому поездному маршруту
	void ParkMigrationByShortTrainRoute();
	//переезд поезда в парк без маршрутов
	void ParkMigrationWithoutRoutes();
	//переезд поезда в парк после установки и снятия маршрута
	void ParkMigrationByOnOffTrainRoute();
	//расщепление поезда в момента проезда по заданному маршруту
	void SplitIntoSettledRoute();
	//переезд поезда в парк на разделываемом маршруте
	void ParkMigrationOnCuttingRoute();
	//снятие маршрута при переезде со станции на блок-пост
	void UnsetRouteWithBPMigrations(); //успешная миграция (снятие маршрута и переезд проиходит одновременно)
	void UnsetRouteWithBPMigrations2(); //не успешная миграция
	void ParkMigration( const std::vector <std::wstring> & routeNames, const std::vector <std::wstring> & throughPlaces, bool mustBeTrain );
	void SimultHeadCloseAndADOut(); //одновременное закрытие светофора с выходом с ПО-пути
	void DepartureWithIrregularBusyOnRoute(); //отправление по маршруту с неравномерной занятостью участков
};