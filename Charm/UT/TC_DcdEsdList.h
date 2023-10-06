#pragma once

#include <cppunit/extensions/HelperMacros.h>

// тесты для классификатора состава смены
class TC_DcdEsdList : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_DcdEsdList );
    //CPPUNIT_TEST( LoadSave );
    //CPPUNIT_TEST( getDistrict );
    CPPUNIT_TEST_SUITE_END();

protected:
    void LoadSave();
    void getDistrict();
};

