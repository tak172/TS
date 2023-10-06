#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Planned : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Planned );
		CPPUNIT_TEST( EventFromXml );
		CPPUNIT_TEST( PathFromXml );
		CPPUNIT_TEST( Serialization );
        CPPUNIT_TEST( GuardZone );
	CPPUNIT_TEST_SUITE_END();

	void EventFromXml();
	void PathFromXml();
	void Serialization();
    void GuardZone();
};

