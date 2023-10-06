#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class CGridPoint;
class TC_FlashHandler :
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_FlashHandler );
	CPPUNIT_TEST( FigureMoving );
	CPPUNIT_TEST( SemaMoving );
	CPPUNIT_TEST( SemaRotate );
	CPPUNIT_TEST( StripsCrossing );
	CPPUNIT_TEST( TryingSwitchMoving );
	CPPUNIT_TEST( SwitchedStripMoving );
	CPPUNIT_TEST( SwitchedOrthoStripMoving );
	CPPUNIT_TEST( OrthoStripMoving );
	CPPUNIT_TEST( StripsFragmentationBid );
	CPPUNIT_TEST( NoMovingOfTextsQuasiNames );
	CPPUNIT_TEST_SUITE_END();

	void FigureMoving();
	void SemaMoving();
	void SemaRotate();
	void StripsCrossing();
	void TryingSwitchMoving();
	void SwitchedStripMoving(); //смещение участка, связанного со стрелкой
	void SwitchedOrthoStripMoving(); //смещение участка, связанного со стрелкой под прямым углом
	void OrthoStripMoving(); //смещение участка, расположенного под прямым углом к другому (без образования стрелки)
	void StripsFragmentationBid(); //запрет разбиения участков
	void NoMovingOfTextsQuasiNames(); //запрет на смещение псевдоимен для текстовых объектов (текстовые объекты не имеют имен)
};