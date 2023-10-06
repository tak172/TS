#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Тесты на миграцию поездов с одной ЦЗ на другую

class TC_TrackerTrainsMigration : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainsMigration );
	//CPPUNIT_TEST( BidMigrationThrouBadDirectedSwitch );
	CPPUNIT_TEST( BidMigrationOverOther );
	CPPUNIT_TEST( MigrationVsOccupation );
	CPPUNIT_TEST( BidJumpToSpan );
	CPPUNIT_TEST_SUITE_END();

	void BidMigrationThrouBadDirectedSwitch(); //запрет миграции через стрелку (нужно доработать вместе с требованиями на движение через стрелочную секцию)
	void BidMigrationOverOther(); //запрет миграции через другую занятость (через другую ПЕ)
	void MigrationVsOccupation(); //ошибка зачистки ЦЗ после миграции поезда с нее
	void BidJumpToSpan(); //запрет резкого прыжка поезда с одного региона (перегон/станция) на другой
};