#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//–аспознование поездов

class TC_TrackerTrainRecognition : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainRecognition );
	CPPUNIT_TEST( RouteSetExit );
	CPPUNIT_TEST( OpenAlienHeadExit );
	CPPUNIT_TEST( CloseHeadExit );
	CPPUNIT_TEST( FromStationToSpan );
	CPPUNIT_TEST( SpanMove );
	CPPUNIT_TEST( FormByInfoEvent );
	CPPUNIT_TEST( MoveTrainStationToStation );
	CPPUNIT_TEST( UpgradeNoTrainWhileGoToAlienStation );
	CPPUNIT_TEST( NoUpgradeNoTrainWhileGoToFriendStation );
	CPPUNIT_TEST( NoFormByTrainRoute );
	CPPUNIT_TEST( NoFormByTrainRoute2 );
	CPPUNIT_TEST( NoFormByShuntingRoute );
	CPPUNIT_TEST( NoFormByOnlyTrainHead );
	CPPUNIT_TEST_SUITE_END();

	void RouteSetExit(); //выезд по заданному маршруту -> поезд создаетс€
	void OpenAlienHeadExit(); //выезд по открытому сигналу головы светофора чужой четности -> поезд не создаетс€
	void CloseHeadExit(); //выезд маневровой по закрытому сигналу головы светофора -> поезд не создаетс€
	void FromStationToSpan();
	void SpanMove();
	void FormByInfoEvent();
	void MoveTrainStationToStation(); //перемещение поезда со станции на смежную станцию
	void UpgradeNoTrainWhileGoToAlienStation(); //повышение маневровой до поезда при переезде со станции на чужую станцию
	void NoUpgradeNoTrainWhileGoToFriendStation(); //сохранение статуса маневровой при переезде со дружественную станцию, вход€щую в объединенный комплект
	void NoFormByTrainRoute(); //по поездному маршруту поезд не формируетс€
	void NoFormByTrainRoute2();
	void NoFormByShuntingRoute(); //по маневровому маршруту поезд не формируетс€
	void NoFormByOnlyTrainHead(); //только по открытому поездному светофору (без маршрута) поезд не формируетс€
};