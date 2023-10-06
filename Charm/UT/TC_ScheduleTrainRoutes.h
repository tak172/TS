#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_ScheduleTrainRoutes : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_ScheduleTrainRoutes );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST_SUITE_END();

	void Deserialize();
};