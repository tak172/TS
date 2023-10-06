#include "stdafx.h"

#include "../helpful/ParkWayKit.h"
#include "TC_ParkWayKit.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ParkWayKit );

void TC_ParkWayKit::simple()
{
    ParkWayKit e;
    ParkWayKit w( 3 );
    ParkWayKit pw( 2, 14 );

    CPPUNIT_ASSERT(     L"" == e.to_wstring(  '/' ) );
    CPPUNIT_ASSERT(    L"3" == w.to_wstring(  '/' ) );
    CPPUNIT_ASSERT( L"2/14" == pw.to_wstring( '/' ) );
}

void TC_ParkWayKit::create()
{
    CPPUNIT_ASSERT( ParkWayKit::from_separated_string("") == ParkWayKit() );
    CPPUNIT_ASSERT( ParkWayKit::from_separated_string("13") == ParkWayKit( 13 ) );
    CPPUNIT_ASSERT( ParkWayKit::from_separated_string("5/42") == ParkWayKit( 5, 42 ) );
}
