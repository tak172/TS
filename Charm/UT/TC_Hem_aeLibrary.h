#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/HappenLayer.h"

class TC_Hem_aeLibrary : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeLibrary );
    CPPUNIT_TEST( SuffixMM );
    CPPUNIT_TEST( SuffixMA );
    CPPUNIT_TEST( SuffixAD_theSame );
    CPPUNIT_TEST( SuffixDA_notSame );
    CPPUNIT_TEST( Prefix );
//      CPPUNIT_TEST( Bridge );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;

private:
    // создание суффикса
    void SuffixMM();
    void SuffixMA();
    void SuffixAD_theSame();
    void SuffixDA_notSame();
    // создание префикса
    void Prefix();
    // заполнение промежутка
    void Bridge();

    static const time_t T=3600*(240+23);
    std::shared_ptr<HappenLayer> happen_layer;
    EsrGuide esrGuide;

    bool is( const SpotEvent& ev, HCode code );
    bool is( const SpotEvent& ev, EsrKit kit );
    bool is( const SpotEvent& ev, HCode code, EsrKit kit );
};
