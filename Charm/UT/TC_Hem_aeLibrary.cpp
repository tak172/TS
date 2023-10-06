#include "stdafx.h"

#include "TC_Hem_aeLibrary.h"
#include "UtHemHelper.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/Appliers/aeLibrary.h"

using namespace std;
using namespace HemHelpful;
using namespace Hem;
using namespace Hem::ApplierCarryOut;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeLibrary );

void TC_Hem_aeLibrary::setUp()
{
    std::string source;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<My>"
            L"<EsrGuide kit='00100' name='станция 1' picketing1_val='5100~0' picketing1_comm='даль' />"
            L"<EsrGuide kit='00200' name='станция 2' picketing1_val='5200~0' picketing1_comm='даль' />"
            L"<EsrGuide kit='00300' name='станция 3' picketing1_val='5300~0' picketing1_comm='даль' />"
            L"<EsrGuide kit='00400' name='станция 4' picketing1_val='5400~0' picketing1_comm='даль' />"
            L"<EsrGuide kit='00500' name='станция 5' picketing1_val='5500~0' picketing1_comm='даль' />"
            L"</My>"
            );
        esrGuide.load_from(doc.document_element());
    }

    SpotEvent pat[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    UtLayer<HappenLayer> happen_layer;
    happen_layer.createPath( pat );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_path_size(6) );
}

void TC_Hem_aeLibrary::SuffixMM()
{
    SpotEvent m1( HCode::SPAN_MOVE, BadgeE(L"xx",EsrKit(200,300)), T+0 );
    SpotEvent m2( HCode::SPAN_MOVE, BadgeE(L"xx",EsrKit(200,300)), T+100 );
    vector<SpotEvent> res = makeSuffix( esrGuide, m1, m2 );
    CPPUNIT_ASSERT_EQUAL( size_t(2), res.size() );
    CPPUNIT_ASSERT( is( res.front(), HCode::SPAN_MOVE, EsrKit(200,300) ) );
    CPPUNIT_ASSERT( is( res.back(),  HCode::DEATH ) );
}

void TC_Hem_aeLibrary::SuffixMA()
{
    SpotEvent m( HCode::SPAN_MOVE, BadgeE(L"xx",EsrKit(200,300)), T+0 );
    SpotEvent a( HCode::ARRIVAL,   BadgeE(L"xx",EsrKit(200)),     T+100 );
    vector<SpotEvent> res = makeSuffix( esrGuide, m, a );
    CPPUNIT_ASSERT_EQUAL( size_t(2), res.size() );
    CPPUNIT_ASSERT( is( res.front(), HCode::SPAN_MOVE, EsrKit(200,300) ) );
    CPPUNIT_ASSERT( is( res.back(), HCode::DEATH ) );
}

void TC_Hem_aeLibrary::SuffixAD_theSame()
{
    SpotEvent a( HCode::ARRIVAL,   BadgeE(L"xx",EsrKit(400)), T+0 );
    SpotEvent d( HCode::DEPARTURE, BadgeE(L"xx",EsrKit(400)), T+100 );
    vector<SpotEvent> res = makeSuffix( esrGuide, a, d );
    CPPUNIT_ASSERT_EQUAL( size_t(1), res.size() );
    CPPUNIT_ASSERT( is( res.front(), HCode::DEATH, EsrKit(400) ) );
}

void TC_Hem_aeLibrary::SuffixDA_notSame()
{
    SpotEvent d( HCode::DEPARTURE,   BadgeE(L"xx",EsrKit(400)), T+0 );
    SpotEvent a( HCode::ARRIVAL, BadgeE(L"xx",EsrKit(500)), T+100 );
    vector<SpotEvent> res = makeSuffix( esrGuide, a, d );
    CPPUNIT_ASSERT_EQUAL( size_t(2), res.size() );
    CPPUNIT_ASSERT( is( res.front(), HCode::SPAN_MOVE, EsrKit(400,500) ) );
    CPPUNIT_ASSERT( is( res.back(), HCode::DEATH ) );
}

void TC_Hem_aeLibrary::Prefix()
{
    SpotEvent last( HCode::SPAN_MOVE, BadgeE(L"xx",EsrKit(200,300)), T+0 );
    SpotEvent next( HCode::SPAN_MOVE, BadgeE(L"xx",EsrKit(200,300)), T+100 );
    auto res = makePrefix( last, next, esrGuide );
    CPPUNIT_ASSERT_EQUAL( size_t(1), res.size() );
}

void TC_Hem_aeLibrary::Bridge()
{
}

bool TC_Hem_aeLibrary::is(const SpotEvent& ev, HCode code )
{
    return code == ev.GetCode();
}

bool TC_Hem_aeLibrary::is(const SpotEvent& ev, EsrKit kit)
{
    return kit == ev.GetBadge().num();
}

bool TC_Hem_aeLibrary::is(const SpotEvent& ev, HCode code, EsrKit kit)
{
    return is(ev,code) && is(ev,kit);
}
