#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Hem_aeTrimLimit : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeTrimLimit );
    CPPUNIT_TEST( obsolete );
    CPPUNIT_TEST( cutBisK );
	CPPUNIT_TEST_SUITE_END();

private:
	void obsolete();  // проверка очистки устаревших
    void cutBisK();   // обрезка BisK "до отмены"
};
