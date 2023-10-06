#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_Superselected :
	public TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_Superselected );
	CPPUNIT_TEST( ResetSelection );
	CPPUNIT_TEST( Initialization );
	CPPUNIT_TEST_SUITE_END();

	void ResetSelection();
	void Initialization();
};