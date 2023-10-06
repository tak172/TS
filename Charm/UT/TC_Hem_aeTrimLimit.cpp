#include "stdafx.h"

#include "TC_Hem_aeTrimLimit.h"
#include "UtHemHelper.h"
#include "TC_LimitLayer.h"
#include "../Hem/LimitLayer.h"
#include "../Hem/Bill.h"
#include "../Hem/Appliers/aeTrimLimit.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeTrimLimit );

void TC_Hem_aeTrimLimit::obsolete()
{
    UtLayer<Limit::Layer> limit_layer;
    {
        Limit::Event evSource = createEvent<Limit::Event>(
            L"  <Data Id='i25' StartTime='20151019T223024Z' StopTime='20151019T233024Z' esr='11050:11051' Reason='1111' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"  </Data>"
            );
        limit_layer.Push(evSource);
        evSource = createEvent<Limit::Event>(
            L"  <Data Id='G33' StartTime='20151021T223024Z' StopTime='20151021T233024Z' esr='11050:11051' Reason='2222' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"  </Data>"
            );
        limit_layer.Push(evSource);
    }

    CPPUNIT_ASSERT( limit_layer.path_count() == 2 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i25'") );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='R33'") );

    namespace bg = boost::gregorian;
    namespace bt = boost::posix_time;
    bt::ptime pt1   ( bg::date(2015,10,20), bt::hours(0) );
    bt::ptime pt2   ( bg::date(2015,10,20), bt::hours(23) );
    bt::ptime epoch = bt::from_time_t(0);
    time_t const seconds = (pt1-epoch).total_seconds();
    time_t const latest  = (pt2-epoch).total_seconds();
    Hem::aeTrimLimit tri( limit_layer, seconds, latest );
    tri.Action();

    limit_layer.ForEach([seconds](const Limit::Event& event) {
        CPPUNIT_ASSERT(!event.isLeft(seconds));
    });

    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='R33'") );
}

void TC_Hem_aeTrimLimit::cutBisK()
{
    UtLayer<Limit::Layer> limit_layer;
    {
        Limit::Event evSource = createEvent<Limit::Event>(
            L"  <Data Id='G25' StartTime='20151019T223024Z' esr='11050:11051' Reason='1111' UntilStop='Y' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"  </Data>"
            );
        limit_layer.Push(evSource);
        evSource = createEvent<Limit::Event>(
            L"  <Data Id='G33' StartTime='20151021T223024Z' esr='11050:11051' Reason='2222' UntilStop='Y' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"  </Data>"
            );
        limit_layer.Push(evSource);
        evSource = createEvent<Limit::Event>(
            L"  <Data Id='i33' StartTime='20151021T223024Z' esr='11050:11051' Reason='2222' UntilStop='Y' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"  </Data>"
            );
        limit_layer.Push(evSource);
    }

    CPPUNIT_ASSERT( limit_layer.path_count() == 3 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='R25'") );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='R33'") );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i33'") );

    namespace bg = boost::gregorian;
    namespace bt = boost::posix_time;
    bt::ptime pt1   ( bg::date(2015,10,20), bt::hours(0) );
    bt::ptime pt2   ( bg::date(2015,10,20), bt::hours(23) );
    bt::ptime epoch = bt::from_time_t(0);
    time_t const seconds = (pt1-epoch).total_seconds();
    time_t const latest  = (pt2-epoch).total_seconds();
    Hem::aeTrimLimit tri( limit_layer, seconds, latest );
    tri.Action();

    limit_layer.ForEach( [](const Limit::Event& event) {
        if ( event.core().getIdentifer().is_external() )
            CPPUNIT_ASSERT( !event.core().detail.untilStop() );
        else
            CPPUNIT_ASSERT( event.core().detail.untilStop() );
    });

    CPPUNIT_ASSERT( limit_layer.path_count() == 2 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='R25'") );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i33'") );
}
