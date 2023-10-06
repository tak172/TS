#include "stdafx.h"

#include "TC_interval_time_t.h"
#include "../helpful/interval_time_t.h"
#include "../helpful/StrToTime.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_interval_time_t );

void TC_interval_time_t::Dirt()
{
    interval_time_t zero;
    interval_time_t bad( 456, 321 );
    interval_time_t some( 100, 200 );

    CPPUNIT_ASSERT( zero.empty() );
    CPPUNIT_ASSERT( zero.width()==0 );
    CPPUNIT_ASSERT( bad.empty() );
    CPPUNIT_ASSERT( bad.width()==0 );
    CPPUNIT_ASSERT( !some.empty() );
    CPPUNIT_ASSERT( some.width()!=0 );
}

void TC_interval_time_t::Compare()
{
    interval_time_t a( 100, 200 );
    CPPUNIT_ASSERT( a.contains(100) );
    CPPUNIT_ASSERT( a.contains(199) );
    CPPUNIT_ASSERT( !a.contains(200) );
    CPPUNIT_ASSERT( a.contains(a) );
    CPPUNIT_ASSERT( a.contains( interval_time_t( 100, 110 ) ) );
    CPPUNIT_ASSERT( a.contains( interval_time_t( 190, 200 ) ) );
    CPPUNIT_ASSERT( !a.contains( interval_time_t( 200, 205 ) ) );
    CPPUNIT_ASSERT( !a.contains( interval_time_t(  90, 100 ) ) );

    interval_time_t b( 150, 250 );
    CPPUNIT_ASSERT( a.overlap(b) );
    CPPUNIT_ASSERT( b.overlap(a) );

    interval_time_t c( 200, 300 );
    CPPUNIT_ASSERT( b.overlap(c) );
    CPPUNIT_ASSERT( c.overlap(b) );

    CPPUNIT_ASSERT( !a.overlap(c) );
    CPPUNIT_ASSERT( !c.overlap(a) );

    interval_time_t d( 100, 300 );
    interval_time_t ac = hull(a,c);
    CPPUNIT_ASSERT( ac == d );
    CPPUNIT_ASSERT( a != d );

    interval_time_t e; // пустой
    CPPUNIT_ASSERT( hull(a,e) == a );
    CPPUNIT_ASSERT( hull(e,a) == a );

    CPPUNIT_ASSERT( ac.contains(a) );
    CPPUNIT_ASSERT( !a.contains(ac) );
    CPPUNIT_ASSERT( d.contains(ac) );
    CPPUNIT_ASSERT( ac.contains(d) );
}

void TC_interval_time_t::Overlap()
{
    interval_time_t a(100, 200);
    interval_time_t b(200, 300);
    CPPUNIT_ASSERT(!a.overlap(b));

    interval_time_t c(100, 100);
    CPPUNIT_ASSERT(!a.overlap(c));
    CPPUNIT_ASSERT(!c.overlap(a));
}

void TC_interval_time_t::ioString()
{
    time_t m5 = StrDataTime_To_UTC( "2011-06-16 05:11:22" );
    time_t m7 = StrDataTime_To_UTC( "2011-06-16 07:33:44" );
    time_t m1 = StrDataTime_To_UTC( "2011-06-17 01:00:00" );
    interval_time_t iNone;
    interval_time_t i57( m5, m7 );
    interval_time_t i71( m7, m1 );

    std::string sNone = iNone.to_string();
    interval_time_t rNone = interval_time_t::from_string( sNone );
    CPPUNIT_ASSERT( rNone == iNone );

    std::string s57 = i57.to_string();
    interval_time_t r57 = interval_time_t::from_string( s57 );
    CPPUNIT_ASSERT( r57 == i57 );

    std::string s71 = i71.to_string();
    interval_time_t r71 = interval_time_t::from_string( s71 );
    CPPUNIT_ASSERT( r71 == i71 );
}

void TC_interval_time_t::View()
{
    time_t a = StrDataTime_To_UTC( "2011-06-17 22:00:00" );
    time_t b = StrDataTime_To_UTC( "2011-06-17 23:00:00" );
    time_t d15m = 60*15;
    time_t d27s = 27;
    time_t d2h = 2*3600;
    CPPUNIT_ASSERT_EQUAL( std::string("[2011-06-17 22 - 23)"),  interval_time_t( a,     b ).to_view() );
    CPPUNIT_ASSERT_EQUAL( std::string("[2011-06-17 22:15 - 23:00)"),interval_time_t( a+d15m,b ).to_view() );
    CPPUNIT_ASSERT_EQUAL( std::string("[2011-06-17 22:00 - 23:15)"),interval_time_t( a,b+d15m ).to_view() );
    CPPUNIT_ASSERT_EQUAL( std::string("[2011-06-17 22 - 2011-06-18 01)"),interval_time_t( a,b+d2h ).to_view() );
    CPPUNIT_ASSERT_EQUAL( std::string("[2011-06-17 22:00:27 - 2011-06-18 01:00:00)"),interval_time_t( a+d27s,b+d2h ).to_view() );
}

void TC_interval_time_t::Insert()
{
    interval_time_t a;
    CPPUNIT_ASSERT( a.empty() );
    a.insert( 47 );
    CPPUNIT_ASSERT( a == interval_time_t(47,47+1) );
    a.insert( 25 );
    CPPUNIT_ASSERT( a == interval_time_t(25,47+1) );
    a.insert( 93 );
    CPPUNIT_ASSERT( a == interval_time_t(25,93+1) );

    interval_time_t b;
    b.insert( interval_time_t( 10, 20 ) );
    CPPUNIT_ASSERT( b == interval_time_t( 10, 20 ) );
    b.insert( interval_time_t( 5, 7 ) );
    CPPUNIT_ASSERT( b == interval_time_t( 5, 20 ) );
    b.insert( interval_time_t( 9, 44 ) );
    CPPUNIT_ASSERT( b == interval_time_t( 5, 44 ) );
}

void TC_interval_time_t::semiOpenClose()
{
    semi_open_time_t no;
    CPPUNIT_ASSERT( 0 == no.lower() );
    CPPUNIT_ASSERT( 0 == no.upper() );
    CPPUNIT_ASSERT( no.empty() );
    CPPUNIT_ASSERT( !no.open() );
    CPPUNIT_ASSERT( !no.close() );

    semi_open_time_t op( 125 );
    CPPUNIT_ASSERT( 125 == op.lower() );
    CPPUNIT_ASSERT( !op.empty() );
    CPPUNIT_ASSERT( op.open() );
    CPPUNIT_ASSERT( !op.close() );

    semi_open_time_t opp( 125, 0 );
    CPPUNIT_ASSERT( op == opp );

    semi_open_time_t mix( 111, 222 );
    CPPUNIT_ASSERT( 111 == mix.lower() );
    CPPUNIT_ASSERT( 222 == mix.upper() );
    CPPUNIT_ASSERT( !mix.empty() );
    CPPUNIT_ASSERT( !mix.open() );
    CPPUNIT_ASSERT( mix.close() );
    mix.set_open();
    CPPUNIT_ASSERT( 111 == mix.lower() );
    CPPUNIT_ASSERT( !mix.empty() );
    CPPUNIT_ASSERT( mix.open() );
    CPPUNIT_ASSERT( !mix.close() );
}

void TC_interval_time_t::semiOverlap()
{
    semi_open_time_t s( 111, 222 );
    CPPUNIT_ASSERT( !s.overlap( interval_time_t( 44, 55 ) ) );
    CPPUNIT_ASSERT( !s.overlap( interval_time_t( 44, 111 ) ) );
    CPPUNIT_ASSERT( s.overlap( interval_time_t( 44, 150 ) ) );
    CPPUNIT_ASSERT( s.overlap( interval_time_t( 120, 150 ) ) );
    CPPUNIT_ASSERT( !s.overlap( interval_time_t( 222, 333 ) ) );
    CPPUNIT_ASSERT( !s.overlap( interval_time_t( 888, 999 ) ) );

    semi_open_time_t w( 111 );
    CPPUNIT_ASSERT( !w.overlap( interval_time_t( 44, 55 ) ) );
    CPPUNIT_ASSERT( !w.overlap( interval_time_t( 44, 111 ) ) );
    CPPUNIT_ASSERT( w.overlap( interval_time_t( 44, 150 ) ) );
    CPPUNIT_ASSERT( w.overlap( interval_time_t( 120, 150 ) ) );
}

void TC_interval_time_t::semiContains()
{
    semi_open_time_t a( 111, 222 );
    CPPUNIT_ASSERT( !a.contains( 44 ) );
    CPPUNIT_ASSERT( a.contains( 111 ) );
    CPPUNIT_ASSERT( a.contains( 221 ) );
    CPPUNIT_ASSERT( !a.contains( 222 ) );
    CPPUNIT_ASSERT( !a.contains( 777 ) );

    semi_open_time_t b( 111 );
    CPPUNIT_ASSERT( !b.contains( 44 ) );
    CPPUNIT_ASSERT( b.contains( 111 ) );
    CPPUNIT_ASSERT( b.contains( 555 ) );
}

void TC_interval_time_t::semiReduce()
{
    semi_open_time_t x( 222, 444 );
    CPPUNIT_ASSERT( x.reduce() == interval_time_t(222,444) );

    semi_open_time_t y( 222 );
    CPPUNIT_ASSERT( y.reduce().lower() == 222 );
    CPPUNIT_ASSERT( y.reduce().upper() > time_from_iso( "20501011T000000Z" ) ); // некое очень большое значение
}

void TC_interval_time_t::semiToString()
{
    semi_open_time_t x( time_from_iso( "20110102T112233Z" ), time_from_iso( "20220304T000000Z" ) );
    std::wstring wx = x.to_wstring(L"--", L"mmmm");
    CPPUNIT_ASSERT( wx == L"20110102T112233Z--20220304T000000Z" );

    semi_open_time_t y( time_from_iso( "20110102T112233Z" ) );
    std::wstring wy = y.to_wstring( L"--", L"mmmm" );
    CPPUNIT_ASSERT( wy == L"20110102T112233Zmmmm" );
}

void TC_interval_time_t::semiReadWrite()
{
    attic::a_document doc( "test" );

    const semi_open_time_t e;
    attic::a_node ne = doc.document_element().append_child( "E" );
    e.write_to( ne, "Start", "Stop", "Open" );
    std::string se = ne.to_str();
    CPPUNIT_ASSERT( se == "<E />" );
    semi_open_time_t re_e;
    re_e.read_from( ne, "Start", "Stop", "Open" );
    CPPUNIT_ASSERT( e == re_e );

    const semi_open_time_t a( time_from_iso( "20111112T111111Z" ), time_from_iso( "20220304T222222Z" ) );
    attic::a_node na = doc.document_element().append_child( "A" );
    a.write_to( na, "Start", "Stop", "Open" );
    std::string sa = na.to_str();
    CPPUNIT_ASSERT( sa == "<A Start=\"20111112T111111Z\" Stop=\"20220304T222222Z\" />" );
    semi_open_time_t re_a;
    re_a.read_from( na, "Start", "Stop", "Open" );
    CPPUNIT_ASSERT( a == re_a );

    const semi_open_time_t b( time_from_iso( "20111112T111111Z" ) );
    attic::a_node nb1 = doc.document_element().append_child( "B1" );
    b.write_to( nb1, "From", "To", "Until" );
    std::string sb1 = nb1.to_str();
    CPPUNIT_ASSERT( sb1 == "<B1 From=\"20111112T111111Z\" Until=\"Y\" />" );
    semi_open_time_t re_b1;
    re_b1.read_from( nb1, "From", "To", "Until" );
    CPPUNIT_ASSERT( b == re_b1 );

    attic::a_node nb2 = doc.document_element().append_child( "B2" );
    b.write_to( nb2, "From", "To", "" );
    std::string sb2 = nb2.to_str();
    CPPUNIT_ASSERT( sb2 == "<B2 From=\"20111112T111111Z\" />" );
    semi_open_time_t re_b2;
    re_b2.read_from( nb2, "From", "To", "" );
    CPPUNIT_ASSERT( b == re_b2 );
}
