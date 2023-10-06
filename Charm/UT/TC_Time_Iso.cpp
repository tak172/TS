#include "stdafx.h"

#include "TC_Time_Iso.h"
// #include "../helpful/Attic.h"
#include "../helpful/Time_Iso.h"
// #include "../helpful/Log.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Time_Iso );

void TC_Time_Iso::big_time()
{
    std::string s3k( "30001122T000000Z" );
    std::string s39( "20390101T000000Z" );
    std::string s38( "20380119T031407Z" ); // max 32-bit time-t
    std::string s2k( "20001122T000000Z" );

    auto cnv3k = time_to_iso( time_from_iso( s3k ) );
    CPPUNIT_ASSERT( cnv3k == s3k );
    auto cnv39 = time_to_iso( time_from_iso( s39 ) );
    CPPUNIT_ASSERT( cnv39 == s39 );
    auto cnv38 = time_to_iso( time_from_iso( s38 ) );
    CPPUNIT_ASSERT( cnv38 == s38 );
    auto cnv2k = time_to_iso( time_from_iso( s2k ) );
    CPPUNIT_ASSERT( cnv2k == s2k );
}
