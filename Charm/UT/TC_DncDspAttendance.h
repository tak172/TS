#pragma once
#include <cppunit/extensions/HelperMacros.h>

// тесты для DncDsp::Attendance (приход ДНЦ и ДСП)
class TC_DncDspAttendance : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_DncDspAttendance );
    CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST( Push );
    CPPUNIT_TEST( Changes );
    CPPUNIT_TEST_SUITE_END();

protected:
    void LoadSave();
    void Push();
    void Changes();
};

