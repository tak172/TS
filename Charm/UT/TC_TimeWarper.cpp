#include "stdafx.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include "TC_TimeWarper.h"
#include "../Actor/TimeWarper.h"

namespace bt = boost::posix_time;
namespace bg = boost::gregorian;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TimeWarper );

static const bt::ptime ZERO = bt::ptime( bg::date(1970,1,1), bt::time_duration(0,0,0) );

static bt::ptime tw_value( bg::date(2000,5,17) );
static bt::ptime tw_clock()
{
    return tw_value;
}

void TC_TimeWarper::test()
{
    bt::ptime PT = bt::ptime( bg::date(2022,11,17), bt::time_duration(1,25,37) );
    time_t    MOM = (PT-ZERO).total_seconds();
    tw_value = PT;

    CTimeWarper warper( MOM, +1, tw_clock );
    // простое движение часов вперед
    CPPUNIT_ASSERT ( MOM == warper.getCurrentTime() );
    tw_value += bt::seconds(10);
    CPPUNIT_ASSERT ( MOM+10 == warper.getCurrentTime() );
    // остановка
    warper.setPause();
    tw_value += bt::seconds(17);
    CPPUNIT_ASSERT ( MOM+10 == warper.getCurrentTime() );
    // пускаем часы назад
    warper.setSpeedRatio( -10 );
    CPPUNIT_ASSERT ( MOM+10 == warper.getCurrentTime() );
    tw_value += bt::milliseconds(100);
    CPPUNIT_ASSERT ( MOM+ 9 == warper.getCurrentTime() );
    tw_value += bt::milliseconds(900);
    CPPUNIT_ASSERT ( MOM+ 0 == warper.getCurrentTime() );
}
