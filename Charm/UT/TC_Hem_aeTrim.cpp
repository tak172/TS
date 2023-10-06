#include "stdafx.h"

#include "TC_Hem_aeTrim.h"
#include "UtHemHelper.h"
#include "../helpful/WorkHours.h"
#include "../helpful/Time_Iso.h"
#include "../Hem/Appliers/aeTrimHappen.h"
#include "../Hem/Appliers/aeTrimHungPaths.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeTrim );

void TC_Hem_aeTrim::LastLiveHappen()
{
    UtLayer<HappenLayer> happen;
    happen.createPath( // эта нить будет закрыта
        L"<HemPath>"
        L"  <SpotEvent name='Form'       Bdg='A[11292]' create_time='20151207T010101Z' waynum='1' />"
        L"  <SpotEvent name='Transition' Bdg='B[11292]' create_time='20151207T020202Z' waynum='1' />"
        L"</HemPath>"
        );
    happen.createPath( // эта нить не изменится т.к. уже закрыта 
        L"<HemPath>"
        L"  <SpotEvent name='Span_move' Bdg='D[11292:11300]' create_time='20151207T030303Z' waynum='1' />"
        L"  <SpotEvent name='Death'     Bdg='D[11292:11300]' create_time='20151207T040404Z' waynum='1' />"
        L"</HemPath>"
        );
    happen.createPath( // эта нить будет закрыта
        L"<HemPath>"
        L"<SpotEvent create_time='20151207T010101Z' name='Span_move' Bdg='PM8P[11010:11363]' waynum='2' />"
        L"<SpotEvent create_time='20151207T011002Z' name='Span_stopping_begin' Bdg='PM8P[11010:11363]' waynum='2' intervalSec='541' />"
        L"</HemPath>"
        );
    // две похожих нити - одна будет закрыта, вторая - удалена
    happen.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20151207T000044Z' name='Form' Bdg='1SP[11231]' />"
        L"  <SpotEvent create_time='20151207T000044Z' name='Station_exit' Bdg='1SP[11231]' />"
        L"</HemPath>"
        );
    happen.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20151207T001144Z' name='Form' Bdg='1SP[11231]' />"
        L"  <SpotEvent create_time='20151207T001144Z' name='Station_exit' Bdg='1SP[11231]' />"
        L"</HemPath>"
        );

    const std::string big = " 20151207T020202Z";
    const std::string big1= " 20151207T020203Z"; // на секунду позже big
    const std::string nameX = "Form             Transition       ";
    const std::string timeX = "20151207T010101Z 20151207T020202Z ";
    const std::string nameY = "Span_move        Death            ";
    const std::string timeY = "20151207T030303Z 20151207T040404Z ";
    const std::string nameZ = "Span_move        Span_stopping_begin ";
    const std::string timeZ = "20151207T010101Z 20151207T011002Z    ";
    const std::string nameA = "Form             Station_exit ";
    const std::string timeA = "20151207T000044Z 20151207T000044Z    ";
    const std::string nameB = "Form             Station_exit ";
    const std::string timeB = "20151207T001144Z 20151207T001144Z    ";

    CPPUNIT_ASSERT( happen.path_count() == 5 );
    auto check = [&happen](const std::string nameStr, const std::string timeStr)
    {
        return happen.exist_series( "name",         nameStr ) &&
               happen.exist_series( "create_time",  timeStr );
    };
    CPPUNIT_ASSERT( check( nameX, timeX ) );
    CPPUNIT_ASSERT( check( nameY, timeY ) );
    CPPUNIT_ASSERT( check( nameZ, timeZ ) );
    CPPUNIT_ASSERT( check( nameA, timeA ) );
    CPPUNIT_ASSERT( check( nameB, timeB ) );
    time_t trimMom = happen.LatestMomentLiveThreads();
    Hem::aeTrimHappen trimmer( happen.GetWriteAccess(), trimMom );
    trimmer.Action();
    CPPUNIT_ASSERT( happen.path_count() == 5 );
    CPPUNIT_ASSERT( check( nameX + "Death", timeX +  big ) );
    CPPUNIT_ASSERT( check( nameY, timeY ) );
    CPPUNIT_ASSERT( check( nameZ + "Span_stopping_end Death ", timeZ +  big +             big ) );
    // неизвестно какая нить будет первой
    // первая нить длится до big, а вторая на секунду меньше
    bool a0 = check( nameA + "Death", timeA +  big );
    bool b0 = check( nameB + "Death", timeB +  big );
    bool a1 = check( nameA + "Death", timeA +  big1 );
    bool b1 = check( nameB + "Death", timeB +  big1 );
    CPPUNIT_ASSERT( a0 != a1 ); // есть один и только один вариант первой нити
    CPPUNIT_ASSERT( b0 != b1 ); // есть один и только один вариант второй нити
    CPPUNIT_ASSERT( a0 == b1 ); // вариант 0 первой нити соответствует варианту 1 второй нити
}

void TC_Hem_aeTrim::TrimHung()
{
    UtLayer<HappenLayer> happen;
    happen.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form'       Bdg='A[11111]' create_time='20151207T110000Z' waynum='1' />"
        L"  <SpotEvent name='Transition' Bdg='B[22222]' create_time='20151207T120000Z' waynum='1' />"
        L"</HemPath>"
        );
    happen.createPath(
        L"<HemPath>"
        L"<SpotEvent name='Span_move'           Bdg='Z[33333:44444]' create_time='20161105T110000Z' waynum='2' />"
        L"<SpotEvent name='Span_stopping_begin' Bdg='S[33333:44444]' create_time='20161105T120000Z' waynum='2' intervalSec='541' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen.path_count() == 2 );
    CPPUNIT_ASSERT( happen.exist_series( "name", "Form      Transition" ) );
    CPPUNIT_ASSERT( happen.exist_series( "name", "Span_move Span_stopping_begin" ) );
    auto HB = []( boost::string_ref s ){
        return WorkHours::HemBounds( time_from_iso( s ) ).lower();
    };
    {
        // графиковая смена около второй нити - урежется только первая
        Hem::aeTrimHungPaths trimHung( happen.GetWriteAccess(), HB("20161105T140000Z") );
        trimHung.Action();
        CPPUNIT_ASSERT( happen.path_count() == 2 );
        CPPUNIT_ASSERT( happen.exist_series( "name",        "Form             Transition       Death " ) );
        CPPUNIT_ASSERT( happen.exist_series( "create_time", "20151207T110000Z 20151207T120000Z 20151207T120000Z " ) );

        CPPUNIT_ASSERT( happen.exist_series( "name",        "Span_move        Span_stopping_begin" ) );
        CPPUNIT_ASSERT( happen.exist_series( "create_time", "20161105T110000Z 20161105T120000Z " ) );
    }
    {
        // графиковая смена далеко после второй нити - урежется и вторая тоже
        Hem::aeTrimHungPaths trimHung( happen.GetWriteAccess(), HB("20161125T140000Z") );
        trimHung.Action();
        CPPUNIT_ASSERT( happen.path_count() == 2 );
        CPPUNIT_ASSERT( happen.exist_series( "name",        "Form             Transition       Death " ) );
        CPPUNIT_ASSERT( happen.exist_series( "create_time", "20151207T110000Z 20151207T120000Z 20151207T120000Z " ) );

        CPPUNIT_ASSERT( happen.exist_series( "name",        "Span_move        Span_stopping_begin Death" ) );
        CPPUNIT_ASSERT( happen.exist_series( "create_time", "20161105T110000Z 20161105T120000Z    20161105T120000Z " ) );
    }
}
