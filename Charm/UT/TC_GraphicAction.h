#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_GraphicAction : 
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_GraphicAction );
	CPPUNIT_TEST( FigureMoving );
	CPPUNIT_TEST_SUITE_END();
	void FigureMoving();
};