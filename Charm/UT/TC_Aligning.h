#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_Aligning :
	public TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Aligning );
	CPPUNIT_TEST( SemaLeftAligning );
	CPPUNIT_TEST( SemaTopAligning );
	CPPUNIT_TEST( LeftAligning );
	CPPUNIT_TEST( RightAligning );
	CPPUNIT_TEST( TopAligning );
	CPPUNIT_TEST( BottomAligning );
	CPPUNIT_TEST( VerticalCentring );
	CPPUNIT_TEST( HorizontalCentring );
	CPPUNIT_TEST( VerticalDistribution );
	CPPUNIT_TEST( HorizontalDistribution );
	CPPUNIT_TEST( OverlappingBan );
	CPPUNIT_TEST_SUITE_END();

private:
	void SemaLeftAligning();
	void SemaTopAligning();
	void CreateSetToAlign( std::vector <const CGraphicElement *> & alignvec );
	void LeftAligning();
	void RightAligning();
	void TopAligning();
	void BottomAligning();
	void VerticalCentring();
	void HorizontalCentring();
	void VerticalDistribution();
	void HorizontalDistribution();
	void OverlappingBan(); //запрет на выравнивание при перекрытии объектов
};