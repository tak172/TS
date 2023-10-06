#pragma once

#include <cppunit/extensions/HelperMacros.h>
//#include "../Hem/HemHelpful.h"
//#include "../Hem/HappenLayer.h"

class TC_Hem_aeTrim : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeTrim );
    CPPUNIT_TEST( LastLiveHappen );
    CPPUNIT_TEST( TrimHung );
	CPPUNIT_TEST_SUITE_END();

private:
	void LastLiveHappen();
    void TrimHung();
};
