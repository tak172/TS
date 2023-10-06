#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

//Проверка сетки графического документа

class TC_GraphicGrid : 
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_GraphicGrid );
	CPPUNIT_TEST( ObjectsValidness );
	CPPUNIT_TEST( PixelRound );
	CPPUNIT_TEST_SUITE_END();

protected:
	void ObjectsValidness();
	void PixelRound();
};
