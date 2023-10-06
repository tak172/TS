#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

//Проверка графического элемента

class TC_GraphicElement : 
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_GraphicElement );
		CPPUNIT_TEST( Move );
		CPPUNIT_TEST( LegOverlapping );
		CPPUNIT_TEST( RevertedStripHit );
	CPPUNIT_TEST_SUITE_END();

protected:
	void Move();
	void LegOverlapping();
	void RevertedStripHit();
};
