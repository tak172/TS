#include "stdafx.h"

#include "../helpful/Attic.h"
#include "../StatBuilder/additional_func.h"
#include "../helpful/Log.h"
#include "../helpful/StrToTime.h"
#include "../helpful/Interval_Time_t.h"
#include "../Fund/ExcerptWarning.h"
#include "Parody.h"
#include "TC_WeightIndicator.h"

using namespace std;
using namespace Parody;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_WeightIndicator );

void TC_WeightIndicator::setUp()
{
    InterlockedLog::instance();
    Etui::instance();
    ribSpeed.clear();

    /*
    .        ст1 ------------ ст2 ---------- ст3       Станции и путь перегона
    .            [w1=30]            [w2=50]
    . вес 2000
    . вес 3000   >30        
    . вес 4000       >40 >50
    . вес 5000                        >70
    */

    // инициализация предупреждений
    makeWarn( &w1b, &w1e,
              L"<Warn Id='100' Stan='111110-222220' KmPk_Beg='12' KmPk_End='15' Tim_Beg='2011-05-05 01:00' Tim_End='2011-05-05 03:00' >"
              L"  <Object Napr='2' />"
              L"  <Speed           Pass='30' Elec='30' />"
              L"</Warn>" );

    makeWarn( &w2b, &w2e,
              L"<Warn Id='200' Stan='222220-333330' KmPk_Beg='21' KmPk_End='29' Tim_Beg='2011-05-05 02:00' Tim_End='2011-05-05 04:00' >"
              L"  <Object Napr='2' />"
              L"  <Speed Gruz='50' Pass='50'           />"
              L"</Warn>" );

    // инициализация индикаторов
    Elements::commonEsr( EsrKit(77777,55555) );
    Elements temp( FIGURE, "отображение", SIMPLE_TEXT );
    Etui::instance()->complete();

    makeWI( L"<WEIGHT>"
            L"  <TARGET>"
            L"    <FIELD Name='отображение'   Esr='77777:55555' />"
            L"    <GROUP Text='вес 2000'/>"
            L"    <GROUP Text='вес 3000'>"
            L"      <COND SpeedAbove='30' Km='11-13' WayNum='2' Esr='11111:22222'/>"
            L"    </GROUP>"
            L"    <GROUP Text='вес 4000'>"
            L"      <COND SpeedAbove='40' Km='15'     WayNum='2' Esr='11111:22222'/>"
            L"      <COND SpeedAbove='50' Km='16-17'  WayNum='2' Esr='11111:22222'/>"
            L"    </GROUP>"
            L"    <GROUP Text=''>"
            L"      <COND SpeedAbove='70' Km='22-28'  WayNum='2' Esr='22222:33333'/>"
            L"    </GROUP>"
            L"  </TARGET>"
            L"</WEIGHT>" );

}
void TC_WeightIndicator::tearDown()
{
    // удаление индикаторов
    // удаление предупреждений
    Etui::Shutdowner();
}

void TC_WeightIndicator::test1()
{
    const CLogicElement* le=NULL;
    {
        set<BadgeE> bg = weIdxs.getAllTargets();
        if ( bg.empty() )
            CPPUNIT_FAIL( "отсутствует отображаемый объект" );
        le = Etui::instance()->lex().LogElBy( *bg.begin() );
    }

    wstring ws;
    CPPUNIT_ASSERT( w1b < w2b && w2b < w1e && w1e < w2e );
    // До предупреждений
    ws = check_state( le, w1b - 1 );
    CPPUNIT_ASSERT( ws == L"" );
    // только первое предупреждение
    ws = check_state( le, w1b + 1 );
    CPPUNIT_ASSERT( ws == L"вес 2000" );
    // первое И второе предупреждение
    ws = check_state( le, w2b + 1 );
    CPPUNIT_ASSERT( ws == L"вес 2000" );
    // только второе предупреждение
    ws = check_state( le, w2e - 5 );
    CPPUNIT_ASSERT( ws == L"вес 4000" );
    // После предупреждений
    ws = check_state( le, w2e + 11 );
    CPPUNIT_ASSERT( ws == L"" );
}

void TC_WeightIndicator::makeWarn( time_t* mom_beg, time_t* mom_end, const std::wstring& warn_text )
{
    attic::a_document doc;
    CPPUNIT_ASSERT( doc.load_wide(warn_text) );
    WarningExcerpt w( WarningExcerpt::getNord(doc.child("Warn")) );
    CPPUNIT_ASSERT( !w.core().empty() );
    *mom_beg = w.actBeg();
    *mom_end = w.actEnd();
    rackWar.append( w.core(), vector<BadgeE>() );
}

void TC_WeightIndicator::makeWI( const std::wstring& warn_text )
{
    attic::a_document doc;
    attic::a_parse_result res = doc.load_wide( warn_text );
    CPPUNIT_ASSERT( res );

    Lexicon& lex = Etui::instance()->lex();
    weIdxs.addIndicatorsFrom( doc, lex, L"test_doc_file" );
}

std::wstring TC_WeightIndicator::check_state( const CLogicElement* le, time_t just_moment )
{
    const auto limChanged = rackWar.changedWithin( interval_time_t(0,just_moment) );
    for( auto lim : limChanged )
    {
        if ( const Excerpt::SpanLot* spanlot = lim.lot.get<Excerpt::SpanLot>() )
        {
            SPANLINE sl = SPANLINE( spanlot->getEsr(), spanlot->getParkWay().iWay() );
            RibTrack<int>& track = ribSpeed.search_with_create(sl);
            rackWar.refillTrack(just_moment,track);
            Lexicon& lex = Etui::instance()->lex();
            weIdxs.acceptChanged( track, lex );
        }
    }

    LOS_base* los = Etui::instance()->lex().getLos(le);
    return los->getContent();
}