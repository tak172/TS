#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"
#include "../helpful/EsrGuide.h"
#include "../Hem/Carrier.h"
class HappenLayer;

class TC_Hem_aeAdiiasReporter : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeAdiiasReporter );
    CPPUNIT_TEST( makeReport );
    CPPUNIT_TEST( reportIntervals );
	CPPUNIT_TEST_SUITE_END();

private:
    void setUp() override;
    void tearDown() override;
    void makeReport();
    void reportIntervals();

    std::unique_ptr< EsrGuide > m_esrGuide;
    std::unique_ptr< Carrier > m_carrier;
    std::unique_ptr< UtLayer<HappenLayer> > m_happenLayer;
    std::unique_ptr< UtLayer<AsoupLayer>  > m_asoupLayer;

    void setUp_EsrGuide();
    void setUp_Carrier();
    void fillCargo();
    std::pair<std::wstring, std::wstring> getHappenAndAsoup1();
    std::pair<std::wstring, std::wstring> getHappenAndAsoup2();
    std::pair<std::wstring, std::wstring> getHappenAndAsoup3();
    std::pair<std::wstring, std::wstring> getHappenAndAsoup4();
    std::pair<std::wstring, std::wstring> getHappenAndAsoup5();
    std::pair< std::wstring, std::wstring > getHappenAndPvLdzElectro(); // пригородные пассажирские
    std::pair< std::wstring, std::wstring > getHappenAndPvLdzDiesel(); // дизельные пассажирские
    std::wstring getSpanWork6();
    std::wstring getSpanWork7();
};
