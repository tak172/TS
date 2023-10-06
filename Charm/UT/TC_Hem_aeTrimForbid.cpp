#include "stdafx.h"

#include "TC_Hem_aeTrimForbid.h"
#include "UtHemHelper.h"
#include "../Hem/ForbidLayer.h"
#include "../Hem/Appliers/aeTrimForbid.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeTrimForbid );

void TC_Hem_aeTrimForbid::test()
{
    UtLayer<Forbid::Layer> forbid_layer;
    {
        Forbid::Event evSource = createEvent<Forbid::Event>(
            L"  <Data Id='G25' StartTime='20151019T223024Z' StopTime='20151019T233024Z' esr='11050:11051' Reason='1111' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"  </Data>"
            );
        forbid_layer.Push(evSource);
        evSource = createEvent<Forbid::Event>(
            L"  <Data Id='G33' StartTime='20151021T223024Z' StopTime='20151021T233024Z' esr='11050:11051' Reason='2222' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"  </Data>"
            );
        forbid_layer.Push(evSource);
    }

    CPPUNIT_ASSERT( forbid_layer.path_count() == 2 );
    CPPUNIT_ASSERT( forbid_layer.exist_event("@Id='R25'") );
    CPPUNIT_ASSERT( forbid_layer.exist_event("@Id='R33'") );

    namespace bg = boost::gregorian;
    namespace bt = boost::posix_time;
    bt::ptime pt   ( bg::date(2015,10,20), bt::hours(0) );
    bt::ptime epoch = bt::from_time_t(0);
    time_t const seconds = (pt-epoch).total_seconds();
    Hem::aeTrimForbid ftrim( forbid_layer, seconds );
    ftrim.Action();

    forbid_layer.ForEach([seconds](const Forbid::Event& event) {
        CPPUNIT_ASSERT(!event.isLeft(seconds));
    });

    CPPUNIT_ASSERT( forbid_layer.path_count() == 1 );
    CPPUNIT_ASSERT( forbid_layer.exist_event("@Id='R33'") );
}
