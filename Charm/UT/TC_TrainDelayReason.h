#pragma once

#include <cppunit/extensions/HelperMacros.h>

// тесты для класса TrainDelayList
class TC_TrainDelayReason : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_TrainDelayReason );
	CPPUNIT_TEST( Load );
	CPPUNIT_TEST_SUITE_END();

protected:
	void Load();
};
