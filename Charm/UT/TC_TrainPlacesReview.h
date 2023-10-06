#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_TrainPlacesReview : public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_TrainPlacesReview );
	CPPUNIT_TEST( PlaceTrain );
	CPPUNIT_TEST( EraseTrain );
	CPPUNIT_TEST( PlaceShunting );
	CPPUNIT_TEST( EraseShunting );
	CPPUNIT_TEST( PlaceTrainOnShunting );
	CPPUNIT_TEST( NonrelevantEraseAfterReplace );
	CPPUNIT_TEST( MultiPlaceIntoSinglePacket );
	CPPUNIT_TEST_SUITE_END();

	void PlaceTrain(); //размещение поезда
	void EraseTrain(); //снятие поезда с участка
	void PlaceShunting(); //размещение маневровой
	void EraseShunting(); //снятие маневровой с участка
	void PlaceTrainOnShunting(); //размещение поезда поверх маневровой
	void NonrelevantEraseAfterReplace(); //снятие маневровой с участка уже после постановки туда нового поезда
	void MultiPlaceIntoSinglePacket(); //изменение нескольких мест, описанных в одной xml-ноде
};