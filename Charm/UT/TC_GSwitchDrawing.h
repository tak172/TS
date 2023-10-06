#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_GSwitchDrawing :
	public TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_GSwitchDrawing );
	CPPUNIT_TEST( PlusMarkDraw );
	CPPUNIT_TEST( EdgeCase );
	CPPUNIT_TEST_SUITE_END();

private:
	void PlusMarkDraw(); //простая отрисовка шильда плюса
	void EdgeCase(); //отрисовка шильда плюса у левого края
};