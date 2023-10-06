#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_GObjectManager : 
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_GObjectManager );
	CPPUNIT_TEST( AddRemoveSingleStrip );
	CPPUNIT_TEST( GettingWhileAdding );
	CPPUNIT_TEST_SUITE_END();
	void AddRemoveSingleStrip();
	void GettingWhileAdding();
};