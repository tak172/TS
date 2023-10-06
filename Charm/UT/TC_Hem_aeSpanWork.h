#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/FutureLayer.h"
#include "UtHemHelper.h"
#include "../Hem/UserChart.h"

class SpanWork;
class SpanWorkInformator;
class EsrGuide;

class UtUserChart : public UserChart
{
public:
    UserChart::getHappenLayer;
    UserChart::getSpanWorkInformator;
};

class TC_Hem_aeSpanWork : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeSpanWork );
    CPPUNIT_TEST( WorkWithReturn );
    CPPUNIT_TEST( WorkWithReturnError );
    CPPUNIT_TEST( WorkWithoutReturnError );
    CPPUNIT_TEST( TestShortZoneInsideOneStrip );
    CPPUNIT_TEST( TestReturnFromSpanWithStopWithoutSpanwork );
    CPPUNIT_TEST( TestReturnFromSpanWithoutSpanwork );
    CPPUNIT_TEST( TestStatusSpanworkAfterDisform );
    CPPUNIT_TEST( InputSpanworkLaterAndTearPath );
    CPPUNIT_TEST( ErrorWaynumInSpanwork );
    CPPUNIT_TEST( EnterSpanWorkAlivePathOnWorkSpan );
    CPPUNIT_TEST( EnterSpanWorkDeathPathOnWorkSpan );
    CPPUNIT_TEST( EnterSpanWorkPostFactumStatStat );
    CPPUNIT_TEST( TwoSpanWorkWithReturnOnTheSamePlace );
    CPPUNIT_TEST( PaintingSpanMoveAndDepartureTheSameTime );
    CPPUNIT_TEST( TearPathPaintingOnSpanWithReturn );
    CPPUNIT_TEST( TearPathWithReturnAfterAttach );
    CPPUNIT_TEST( EditSpanWorkWithWrongKmPk );
    CPPUNIT_TEST( WorkWithReturnOnWrongWay );

    CPPUNIT_TEST_SUITE_END();

private:
    void WorkWithReturn();
    void WorkWithReturnError();
    void WorkWithoutReturnError();

    void setUp() override;
    void tearDown() override;

    bool AddEventToPath(std::shared_ptr<SpotEvent> spotPtr, unsigned tid=64);
    std::shared_ptr<SpotEvent> CreateEvent( const std::wstring& str);
    void createPathAndSpanWorkWithReturnOnStation();
    void createPathAndSpanWorkWithoutReturnOnStation();
    void TestShortZoneInsideOneStrip();
    void TestReturnFromSpanWithStopWithoutSpanwork();
    void TestReturnFromSpanWithoutSpanwork();
    void TestStatusSpanworkAfterDisform();
    void InputSpanworkLaterAndTearPath();
    void ErrorWaynumInSpanwork();
    void EnterSpanWorkAlivePathOnWorkSpan();
    void EnterSpanWorkDeathPathOnWorkSpan();
    void EnterSpanWorkPostFactumStatStat();
    void TwoSpanWorkWithReturnOnTheSamePlace();
    void CreatePathAndSpanWorkOnTheSameSpan();
    void PaintingSpanMoveAndDepartureTheSameTime();
    void TearPathPaintingOnSpanWithReturn();
    void TearPathWithReturnAfterAttach();
    void EditSpanWorkWithWrongKmPk();
    void WorkWithReturnOnWrongWay();

    static griffin::Sender GrifFunc();

    std::shared_ptr<SpanWork> spanWorkPtr;
    UtUserChart userChart;
    std::shared_ptr< Hem::HappenPath > pathPtr;
    std::shared_ptr<SpotEvent> firstEventPtr;
};
