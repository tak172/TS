#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/RegulatoryLayer.h"
#include "../Hem/IdentifyCategory.h"

class TC_Hem_aeSpotToRegulatory : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeSpotToRegulatory );
    CPPUNIT_TEST( DontIdentify );    // Нельзя идентифицировать по маленькому количеству станций
    CPPUNIT_TEST( Identify );        // Простой сценарий идентификации по нормативке
    CPPUNIT_TEST( Identify2 );       // Простой сценарий идентификации по нормативке-2
    CPPUNIT_TEST( IdentifyUsingCorrectEvents );
    CPPUNIT_TEST( IdentifyByStationAndSpandir1 );
    CPPUNIT_TEST( IdentifyByStationAndSpandir2 );
    CPPUNIT_TEST( IdentifyByStationAndSpandir3 );
    CPPUNIT_TEST( IdentifyOnlyForEqualBeginRoute );
    CPPUNIT_TEST( IdentifyByDeparture );
    CPPUNIT_TEST( IdentifyPathFromUncontrolledZone );
    CPPUNIT_TEST( AutoIdentifyIfAdvanceLessThenLimit );
    CPPUNIT_TEST( WrongIdentifyByStationAndSpandir_5644 );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;

private:
    time_t c_time1;
    time_t c_time2;
    time_t c_time3;
    time_t c_time4;
    time_t c_time5;
    time_t c_time2_1;
    time_t c_time2_2;
    time_t c_time2_3;
    time_t c_time2_4;
    
    std::unique_ptr<UncontrolledStations> m_uncontrolledStations;
    std::shared_ptr< UtLayer<HappenLayer> > m_happenLayer;
    std::shared_ptr<Regulatory::Layer> m_regulatoryLayer;
    Regulatory::Layer::HappenRegularMap m_HappenRegular;
    UtIdentifyCategoryList m_identifyCategoryList;
    HappenLayer::EventWithDescrList GetPath(time_t from, time_t to, unsigned short number);

    void DontIdentify();
    void Identify();
    void Identify2();
    void IdentifyUsingCorrectEvents();
    void IdentifyByStationAndSpandir1();
    void IdentifyByStationAndSpandir2();
    void IdentifyByStationAndSpandir3();
    void IdentifyByStationAndSpandir4();
    void IdentifyOnlyForEqualBeginRoute();
    void IdentifyByDeparture();
    void IdentifyPathFromUncontrolledZone();
    void AutoIdentifyIfAdvanceLessThenLimit();
    void WrongIdentifyByStationAndSpandir_5644();
};