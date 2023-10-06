#pragma once

#include <cppunit/extensions/HelperMacros.h>
//#include "../Hem/HemHelpful.h"
//#include "../Hem/HappenLayer.h"

class TC_GriffinUtility : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_GriffinUtility );
    CPPUNIT_TEST( single );
    CPPUNIT_TEST( both );
    CPPUNIT_TEST( forbid );
    CPPUNIT_TEST( forbidUntilStop );
	CPPUNIT_TEST_SUITE_END();

private:
	void single();
    void both();
    void forbid();
    void forbidUntilStop();
};
