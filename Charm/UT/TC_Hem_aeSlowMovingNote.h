#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"
#include "../Hem/UserChart.h"
#include "../helpful/SpanTravelTime.h"

class UtUserChart : public UserChart
{
public:
    UserChart::getHappenLayer;
    UserChart::getEsrGuide;
};

class TC_Hem_aeSlowMovingNote : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeSlowMovingNote );
    CPPUNIT_TEST( AddNote );
    CPPUNIT_TEST( AddNoteOnSpanWithLimit );
    CPPUNIT_TEST( AddNoteOnSpanWithForbid );

    CPPUNIT_TEST_SUITE_END();

private:
    void setUp() override;
    void tearDown() override;

    void AddNote();
    void AddNoteOnSpanWithLimit();
    void AddNoteOnSpanWithForbid();

    void createPath();
    bool AddEventToPath(const std::wstring& str);

    UtUserChart userChart;
    SpanTravelTime spanTravelTime;
    std::shared_ptr< Hem::HappenPath > pathPtr;
};
