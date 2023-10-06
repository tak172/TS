#include "stdafx.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include "TC_TimeMachine.h"
#include "../Fund/TimeMachine.h"

namespace bt = boost::posix_time;
namespace bg = boost::gregorian;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TimeMachine );

static const bt::ptime ZERO = bt::ptime( bg::date(1970,1,1), bt::time_duration(0,0,0) );

static bt::ptime value( bg::date(2000,5,17) );
static bt::ptime mock_clock()
{
    return value;
}

void TC_TimeMachine::start_stop()
{
    bt::ptime PT = bt::ptime( bg::date(2022,11,17), bt::time_duration(1,25,37) );
    time_t    MOM = (PT-ZERO).total_seconds();
    value = PT;

    TimeMachine machine( mock_clock );
    // простое движение часов    
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    value = PT+bt::seconds(3);
    CPPUNIT_ASSERT ( MOM+3 == machine.getTime() );
    // остановка
    machine.freeze();
    CPPUNIT_ASSERT ( MOM+3 == machine.getTime() );
    value = PT+bt::seconds(20);
    CPPUNIT_ASSERT ( MOM+3 == machine.getTime() );
    // снова пускаем часы
    machine.unfreeze();
    CPPUNIT_ASSERT ( MOM+3 == machine.getTime() );
    value = PT+bt::seconds(22);
    CPPUNIT_ASSERT ( MOM+5 == machine.getTime() );
}

void TC_TimeMachine::speed()
{
    time_t    MOM = 1670000000;
    value = bt::from_time_t( MOM );

    TimeMachine machine( mock_clock );
    // задать скорость 20%
    machine.modifySpeed( 20 );
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    value += bt::seconds(100);
    MOM   += 20;
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    // теперь 1000%
    machine.modifySpeed( 1000 );
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    value += bt::milliseconds(100);
    MOM   += 1;
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    value += bt::seconds(10);
    MOM   += 100;
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    // теперь -200%
    machine.modifySpeed( -200 );
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    value += bt::seconds(50);
    MOM   -= 100;
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
}

void TC_TimeMachine::jump()
{
    time_t    MOM = 1670000000;
    value = bt::from_time_t( MOM );

    TimeMachine machine( mock_clock );

    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    value += bt::seconds(5);
    MOM   += 5;
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
    // через 3 сек прыгнем назад на час
    value += bt::seconds(3);
    MOM += 3;
    machine.jumpInTime( MOM-3600 );
    MOM -= 3600;
    CPPUNIT_ASSERT ( MOM == machine.getTime() );
}

