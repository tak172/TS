#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Logic.h"

class TC_LogicElement :
	public TC_Logic,
	public CPPUNIT_NS::TestFixture
{
    void setUp() override;
    void tearDown() override;
private:
	CPPUNIT_TEST_SUITE( TC_LogicElement );
	CPPUNIT_TEST( LinkSorting );
	CPPUNIT_TEST_SUITE_END();

	void LinkSorting();
};