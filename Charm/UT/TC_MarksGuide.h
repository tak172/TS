#pragma once

#include <cppunit/extensions/HelperMacros.h>

// тесты для классификатора пометок
class TC_MarksGuide : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_MarksGuide );
    CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST_SUITE_END();

protected:
    void LoadSave();
};
