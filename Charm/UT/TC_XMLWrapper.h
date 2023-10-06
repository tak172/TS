#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Logic.h"

class TC_XMLWrapper : 
	private TC_Logic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_XMLWrapper );
	CPPUNIT_TEST( SuccESD );
    CPPUNIT_TEST( FailESD );
	CPPUNIT_TEST_SUITE_END();

    void setUp() override;
    void tearDown() override;
protected:
    void SuccESD();
    void FailESD();
private:
    void test_for_EDS_is( bool eds_value );
};
