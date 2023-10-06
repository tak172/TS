#include "stdafx.h"

#include <sstream>
#include "TC_ActorPeakMemory.h"
#include "../helpful/StrToTime.h"
#include "../Actor/ActorPeakMemory.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ActorPeakMemory );

using namespace std;
using namespace boost::posix_time;


void TC_ActorPeakMemory::check()
{
    ptime pt(boost::gregorian::date(2015,3,5));
    size_t K = 1024;
    const auto LIMIT = std::make_pair(1200*K, 1500*K ); 
    CPPUNIT_ASSERT( !exceedMemoryBound( LIMIT, 500*K ) );
    CPPUNIT_ASSERT( exceedMemoryBound( LIMIT, 1501*K,UTC_From_Local(pt) ) );

    for( size_t app = 1201*K; app<1500*K; app+=15*K )
    {
        ptime match = pt;
        for( ptime x = pt; match==pt && x<pt+hours(3); x += minutes(15) )
        {
            if ( exceedMemoryBound( LIMIT, app,UTC_From_Local(x) ) )
                match = x;
        }
        CPPUNIT_ASSERT( match == pt ); // до hours(3) не срабатывает
        for( ptime x = pt+hours(3); match==pt && x<pt+hours(6); x += minutes(30) )
        {
            if ( exceedMemoryBound( LIMIT, app, UTC_From_Local(x) ) )
                match = x;
        }
        CPPUNIT_ASSERT( match != pt ); // с 3 до 6 срабатывает
        match = pt;
        for( ptime x = pt+hours(6); match==pt && x<pt+hours(24); x += minutes(30) )
        {
            if ( exceedMemoryBound( LIMIT, app,UTC_From_Local(x) ) )
                match = x;
        }
        CPPUNIT_ASSERT( match == pt ); // после 6 снова не срабатывает
    }
}
