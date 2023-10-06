#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"
#include "UtTearDown.h"
#include "../Hem/FutureLayer.h"
#include "../Hem/UserChart.h"

class LimitTrain;
class LimitTrainKeeper;
class EsrGuide;

class UtUserChart : public UserChart
{
public:
    UserChart::getHappenLayer;
    UserChart::getLimitTrainKeeper;
};

class TC_Hem_aeLimitTrain : public UtTearDowner
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeLimitTrain );
    CPPUNIT_TEST( EditLimit );
    CPPUNIT_TEST( SaveLimitOnLastEventAfterSpanMove);
    CPPUNIT_TEST( ResetFirstLimitInPair );

    CPPUNIT_TEST_SUITE_END();

private:
    void setUp() override;

    void EditLimit();
    void SaveLimitOnLastEventAfterSpanMove();

    void createPath();
    bool AddEventToPath(const std::wstring& str);
    void ResetFirstLimitInPair();

    static griffin::Sender GrifFunc();

    UtUserChart userChart;
    std::shared_ptr< Hem::HappenPath > pathPtr;
    std::shared_ptr<const SpotEvent> limitEventPtr;
};
