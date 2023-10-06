#include "stdafx.h"
#include "TC_RackWarning.h"
#include "../Fund/ExcerptWarning.h"
#include "../Fund/RackWarning.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_RackWarning );

void TC_RackWarning::test()
{
    RackWarning rackWar;
    RibMap<int> ribSpeed;

    const wstring warn12 =
        L"<Test>"
        L""
        L"  <FirstWarn Id='2922979' Stan='310700-310823' KmPk_Beg='295.1' KmPk_End='296.9' Tim_Beg='2011-09-07 16:10' >"
        L"    <Object Put='2' Napr='2' />"
        L"    <Speed Gruz='25' Pass='25' Elec='25' />"
        L"  </FirstWarn> "
        L""
        L"  <SecondWarn Id='2922977' Stan='310700-310823' KmPk_Beg='306.1' KmPk_End='306.9' Tim_Beg='2011-09-07 16:10' >"
        L"    <Object Put='2' Napr='2' />"
        L"    <Speed Gruz='25' Pass='25' Elec='25' />"
        L"  </SecondWarn> "
        L""
        L"</Test>";

    attic::a_document doc;
    attic::a_parse_result res = doc.load_wide( warn12 );

    auto lim1 = WarningExcerpt::getNord( doc.child("Test").child("FirstWarn" ) );
    auto lim2 = WarningExcerpt::getNord( doc.child("Test").child("SecondWarn") );
    CPPUNIT_ASSERT( !lim1.empty() );
    CPPUNIT_ASSERT( !lim2.empty() );

    CPPUNIT_ASSERT( rackWar.append( lim1, vector<BadgeE>() ) );
    RibTrack<int>& track1 = ribSpeed.search_with_create(lim1.getSpanLine());
    rackWar.refillTrack( lim1.detail.start()+1, track1 );

    CPPUNIT_ASSERT( rackWar.append( lim2, vector<BadgeE>() ) );
    RibTrack<int>& track2 = ribSpeed.search_with_create(lim2.getSpanLine());
    rackWar.refillTrack( lim1.detail.start(), track2 );
}
