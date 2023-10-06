#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка генерации событий въезда/выезда на станцию

class TC_TrackerStationEntryExit : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerStationEntryExit );
	CPPUNIT_TEST( NoWayEntry );
	CPPUNIT_TEST( ProlongedEntryAndArriving );
	CPPUNIT_TEST( FastTransit );
	CPPUNIT_TEST( FastEntryAndArriving );
	CPPUNIT_TEST( NoADPassingAfterBlockpost );
	CPPUNIT_TEST( StationOutOnEqualStationsWithRoute );
	CPPUNIT_TEST( StationOutOnEqualStationsWithRoute2 );
	CPPUNIT_TEST( StationOutOnEqualStationsWORoute );
	CPPUNIT_TEST( StationOutOnEqualStationsWORoute2 );
	CPPUNIT_TEST( StationOutOnUnequalStationsWithRoute );
	CPPUNIT_TEST( StationOutOnUnequalStationsWORoute );
	CPPUNIT_TEST( StationOutOnUnequalStationsWORoute2 );
	CPPUNIT_TEST_SUITE_END();

	void NoWayEntry(); //въезд на станцию не на п/о путь
	void ProlongedEntryAndArriving(); //продолжительный въезд на станцию и последующее прибытие
	void FastTransit(); //быстрый проезд через станцию без занятия п/о путей
	void FastEntryAndArriving(); //быстрый въезд на станцию и последующее прибытие
	void NoADPassingAfterBlockpost(); //проезд сквозь станцию без занятия п/о путей с предыдущим проездом блок-поста
	void StationOutOnEqualStationsWithRoute(); //выход со станции на эквивалентную станцию по заданному маршруту
	void StationOutOnEqualStationsWithRoute2();
	void StationOutOnEqualStationsWORoute(); //выход со станции на эквивалентную станцию без заданного маршрута
	void StationOutOnEqualStationsWORoute2();
	void StationOutOnUnequalStationsWithRoute(); //выход со станции на неэквивалентную станцию по заданному маршруту
	void StationOutOnUnequalStationsWORoute(); //выход со станции на неэквивалентную станцию без заданного маршрута
	void StationOutOnUnequalStationsWORoute2();
};