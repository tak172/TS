#include "stdafx.h"

#include "TC_Trindex.h"
#include "../helpful/Trindex.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Trindex );

/*
проверить класс Trindex
*/
void TC_Trindex::normal()
{
    Trindex a( 1234, 1, 5678 );
    CPPUNIT_ASSERT( L"1234-001-5678" == a.str() );
    CPPUNIT_ASSERT( EsrKit(12340) == a.source() );
    CPPUNIT_ASSERT( EsrKit(56780) == a.destination() );
    CPPUNIT_ASSERT( 1 == a.sequence() );
    CPPUNIT_ASSERT( !a.empty() );

    Trindex b( 1000, 777, 9999 );
    CPPUNIT_ASSERT( L"1000-777-9999" == b.str() );
    CPPUNIT_ASSERT( EsrKit(10000) == b.source() );
    CPPUNIT_ASSERT( EsrKit(99990) == b.destination() );
    CPPUNIT_ASSERT( 777 == b.sequence() );
    CPPUNIT_ASSERT( ! b.empty() );

    CPPUNIT_ASSERT( a==a );
    CPPUNIT_ASSERT( !(a==b) );
    CPPUNIT_ASSERT( a!=b );
    CPPUNIT_ASSERT( b<a );
    CPPUNIT_ASSERT( !(a<b) );
    CPPUNIT_ASSERT( !(a<a) );
    CPPUNIT_ASSERT( !(a>a) );

    a = b;
    CPPUNIT_ASSERT( a==b );

    Trindex c( L"1234-1-5678" );
    Trindex d( L"1234-001-5678" );
    CPPUNIT_ASSERT ( Trindex(1234, 1, 5678 )==c );
    CPPUNIT_ASSERT ( c==d );
    Trindex::TUPLE_STR idx_str = c.part_index_str();
    CPPUNIT_ASSERT ( std::get<0>(idx_str)==L"1234" );
    CPPUNIT_ASSERT ( std::get<1>(idx_str)==L"001" );
    CPPUNIT_ASSERT ( std::get<2>(idx_str)==L"5678" );
    
    Trindex e( L"" );
    CPPUNIT_ASSERT ( e==Trindex() );
    idx_str = e.part_index_str();
    CPPUNIT_ASSERT ( std::get<0>(idx_str).empty() &&  
                     std::get<1>(idx_str).empty() && 
                     std::get<2>(idx_str).empty() );
}

void TC_Trindex::empty()
{
    Trindex trix;
    CPPUNIT_ASSERT( trix.empty() );
    CPPUNIT_ASSERT( L"" == trix.str() );
    CPPUNIT_ASSERT( EsrKit() == trix.source() );
    CPPUNIT_ASSERT( EsrKit() == trix.destination() );
    CPPUNIT_ASSERT( ! trix.sequence() );
}

void TC_Trindex::bad()
{
    CPPUNIT_ASSERT( Trindex(  -456, 333, 5678 ).empty() );
    CPPUNIT_ASSERT( Trindex( 77777, 333, 5678 ).empty() );

    CPPUNIT_ASSERT( Trindex( 1234,   -73, 5678 ).empty() );
    CPPUNIT_ASSERT( Trindex( 1234, 12345, 5678 ).empty() );

    CPPUNIT_ASSERT( Trindex( 1234, 333,  -578 ).empty() );
    CPPUNIT_ASSERT( Trindex( 1234, 333, 99999 ).empty() );

    CPPUNIT_ASSERT( Trindex( L"1234-001" ).empty() );
    CPPUNIT_ASSERT( Trindex( L"1234 001 5678" ).empty() );
    CPPUNIT_ASSERT( Trindex( L"001" ).empty() );
    CPPUNIT_ASSERT( Trindex( L"1234-001 5678" ).empty() );
}
