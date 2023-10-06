#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_GraphicDocument :
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_GraphicDocument );
	CPPUNIT_TEST( Creating );
	CPPUNIT_TEST_SUITE_END();
	void Creating();
};