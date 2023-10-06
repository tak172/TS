#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Hem_aeTrimForbid : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeTrimForbid );
    CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();

private:
    void test();
};
