#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtTearDown.h"

class TC_Hem_aeGetAnnex : public UtTearDowner // CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeGetAnnex );
    CPPUNIT_TEST( exoticPath );
	CPPUNIT_TEST_SUITE_END();

private:
    void setUp() override;

    void exoticPath();
};
