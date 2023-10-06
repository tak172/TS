#include "stdafx.h"

#include <vector>
#include <thread>

#include "TC_HemEvent.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/HemHelpful.h"
#include "../helpful/Dump.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_HemEvent );

void TC_HemEvent::Compare()
{
    const BadgeE b(L"a", EsrKit(1111));
    const time_t m(1234567);

    HemHelpful::HemEventPtr f( new SpotEvent(HCode::FORM,          b, m ) );
    HemHelpful::HemEventPtr d( new SpotEvent(HCode::DEPARTURE,     b, m ) );
    HemHelpful::HemEventPtr i( new SpotEvent(HCode::INFO_CHANGING, b, m ) );
    CPPUNIT_ASSERT( *f != *d );
    CPPUNIT_ASSERT( *f != *i );
    CPPUNIT_ASSERT( *d != *i );
    CPPUNIT_ASSERT( *f < *d );
    CPPUNIT_ASSERT( *f < *i );
    CPPUNIT_ASSERT( *d < *i );
}

void TC_HemEvent::Integrity()
{
    HCode only_term[]=
    {
        HCode::ARRIVAL,
        HCode::DEPARTURE,
        HCode::TRANSITION,
        HCode::EXPLICIT_FORM,
        HCode::WAY_BLACK,
        HCode::WAY_RED,
        HCode::STATION_ENTRY,
        HCode::BANNED_ENTRY,
        HCode::STATION_EXIT,
    };

    HCode only_span[]=
    {
        HCode::ENTRANCE_STOPPING,
        HCode::POCKET_ENTRY,
        HCode::POCKET_EXIT,
        HCode::SPAN_MOVE,
        HCode::SPAN_STOPPING_BEGIN,
        HCode::SPAN_STOPPING_END,
        HCode::WRONG_SPANWAY,
        HCode::TOKEN_SPANWAY,
    };

    const BadgeE termBdg(L"1p", EsrKit(100));
    const BadgeE spanBdg(L"u9", EsrKit(100,200));
    const time_t moment(1234567);

    for( auto code : only_term )
    {
        CPPUNIT_ASSERT_NO_THROW( SpotEvent(code, termBdg, moment ) );
        CPPUNIT_ASSERT_THROW(    SpotEvent(code, spanBdg, moment ), HemHelpful::HemException );
    }

    for( auto code : only_span )
    {
        CPPUNIT_ASSERT_THROW(    SpotEvent(code, termBdg, moment ), HemHelpful::HemException );
        CPPUNIT_ASSERT_NO_THROW( SpotEvent(code, spanBdg, moment ) );
    }
    // убрать за собой дампы
    auto count_succ = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT( count_succ.second );
}
