#include "stdafx.h"

#include "TC_EventTrio.h"
#include "../Hem/EventTrio.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_EventTrio );

void TC_EventTrio::simple()
{
    attic::a_document d;
    {
        d.load_wide( 
            L"<Test>"
            L"  <Trio>"
            L"    <Pre name='Departure' Bdg='701SP[11321]' create_time='20150805T200420Z' waynum='1' parknum='1' />"
            L"    <Body name='Span_move' Bdg='NR1PC[11272:11321]' create_time='20150805T200426Z' waynum='1' />"
            L"  </Trio>"
            L"</Test>"
            );
    }
    TrioSpot et1;
    TrioSpot et2;
    attic::a_document d2("output");

    d.document_element().first_child() >> et1;
    d2.document_element() << et1;
    d2.document_element().first_child() >> et2;

    CPPUNIT_ASSERT( !et1.empty() );
    CPPUNIT_ASSERT( et1 == et2 );
}

void TC_EventTrio::with_cover()
{
    attic::a_document d;
    {
        d.load_wide( 
            L"<Test>"
            L"  <Trio>"
            L"    <Pre name='Departure' Bdg='701SP[11321]' create_time='20150805T200420Z' waynum='1' parknum='1' cover='Y' />"
            L"    <Body name='Span_move' Bdg='NR1PC[11272:11321]' create_time='20150805T200422Z' waynum='1' />"
            L"    <Body name='Span_move' Bdg='NR2PC[11272:11321]' create_time='20150805T200433Z' waynum='1' />"
            L"    <Body name='Span_move' Bdg='NR3PC[11272:11321]' create_time='20150805T200444Z' waynum='1' />"
            L"    <Post name='Departure' Bdg='xxxxx[11272]' create_time='20150805T200555Z' waynum='5' parknum='5' />"
            L"  </Trio>"
            L"</Test>"
            );
    }
    TrioSpot et1;
    TrioSpot et2;
    attic::a_document d2("output");

    d.document_element().first_child() >> et1;
    d2.document_element() << et1;
    d2.document_element().first_child() >> et2;

    CPPUNIT_ASSERT( !et1.empty() );
    CPPUNIT_ASSERT( et1.coverPre() );
    CPPUNIT_ASSERT( et1 == et2 );
    et1.coverPre( false );
    CPPUNIT_ASSERT( et1 != et2 );
}
