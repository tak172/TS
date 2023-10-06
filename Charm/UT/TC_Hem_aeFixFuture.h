#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/FutureLayer.h"

class TC_Hem_aeFixFuture : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeFixFuture );
    CPPUNIT_TEST( FixFutureFragment );
    CPPUNIT_TEST( UnfixFutureFragment );
    CPPUNIT_TEST( FixFuturePathInDistrict );
    CPPUNIT_TEST( UnfixFuturePathInDistrict );
	CPPUNIT_TEST_SUITE_END();

private:
    void FixFutureFragment();
    void UnfixFutureFragment();
    void FixFuturePathInDistrict();
    void UnfixFuturePathInDistrict(); 
};
