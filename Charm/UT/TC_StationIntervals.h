#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_StationIntervals : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_StationIntervals );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST_SUITE_END();

	void Deserialize();
};