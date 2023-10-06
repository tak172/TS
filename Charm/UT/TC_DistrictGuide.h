#pragma once

#include <cppunit/extensions/HelperMacros.h>

// тесты для проверки работы класса DistrictGuide
class TC_DistrictGuide : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_DistrictGuide );
    CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST( testInvolve );
    CPPUNIT_TEST( testStationList );
    CPPUNIT_TEST( directCreate );
    CPPUNIT_TEST_SUITE_END();

protected:
    void LoadSave();
    void testInvolve();
    void testGetSpan();
    void testStationList();
    void directCreate();
};

