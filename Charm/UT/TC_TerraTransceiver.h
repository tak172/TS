#pragma once

#include <cppunit/extensions/HelperMacros.h>

//Тест TerraChart сохранение и загрузка XML-файла
class TC_TerraTransceiver : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_TerraTransceiver );
	CPPUNIT_TEST( checkBusy );
    CPPUNIT_TEST( checkSilence );
	CPPUNIT_TEST_SUITE_END();

protected:
	void checkBusy();
    void checkSilence();
};
