#pragma once

#include <cppunit/extensions/HelperMacros.h>

// тесты для проверки работы класса CStatWokModeGuide
class TC_StatWorkModeGuide : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_StatWorkModeGuide );
    CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST( testGetTime );
    CPPUNIT_TEST_SUITE_END();

protected:
    void LoadSave();
    void testGetTime();
};

