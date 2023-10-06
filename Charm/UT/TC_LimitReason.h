#pragma once

#include <cppunit/extensions/HelperMacros.h>

// тесты для класса LimitReasonList
class TC_LimitReason : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_LimitReason );
    CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST_SUITE_END();

protected:
    void LoadSave();
};
