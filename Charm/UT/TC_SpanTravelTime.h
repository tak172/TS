#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/SpanTravelTime.h"

/* 
 * Класс для проверки работы класса SpanTravelTime
 */

class TC_SpanTravelTime : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_SpanTravelTime );
    CPPUNIT_TEST( GetSet );
    CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST( LoadErrorCheck );
    CPPUNIT_TEST( SubtractUnchanged );
    CPPUNIT_TEST( Invert );
    CPPUNIT_TEST( BadEsr );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;

protected:
    void GetSet();
    void LoadSave();
    void LoadErrorCheck();
    void SubtractUnchanged();
    void Invert();
    void BadEsr();

private:
    SpanTravelTimeInfo span12;
    SpanTravelTimeInfo span34;
    STT_Val span12_time;
    STT_Val span34_time;
};

