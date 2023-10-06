#pragma once

#include <cppunit/extensions/HelperMacros.h>
//#include "../Hem/HemHelpful.h"
//#include "../Hem/HappenLayer.h"

class TC_Hem_aeAdvanceFuture : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeAdvanceFuture );
    CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();

private:
    void tearDown() override;
	void test();
};
