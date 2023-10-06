#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_HemEvent : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_HemEvent );
    CPPUNIT_TEST( Compare );
    CPPUNIT_TEST( Integrity );
	CPPUNIT_TEST_SUITE_END();

private:
	void Compare();
    void Integrity();
};
