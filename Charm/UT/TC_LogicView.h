#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Logic.h"

class TC_LogicView : 
	private TC_Logic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_LogicView );
	CPPUNIT_TEST( AlphanumComparing );
	CPPUNIT_TEST_SUITE_END();

    void setUp() override;
    void tearDown() override;
protected:
	void AlphanumComparing();
};
