#include "stdafx.h"

#include "TC_Hem_aeLimit.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "TC_LimitLayer.h"
#include "../Hem/LimitLayer.h"
#include "../Hem/Bill.h"
#include "../Hem/Appliers/aeLimit.h"

using namespace std;
using namespace Hem::ApplierCarryOut;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeLimit );

void TC_Hem_aeLimit::Create()
{
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_mww'>"
        L"  <Action code='Create' layer='limit' />"
        L"  <Data Id='0' StartTime='20151019T223024Z' StopTime='20151019T233024Z' esr='11050:11051' Reason='1601' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
        L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
        L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
        L"  </Data>"
        L"</A2F_HINT>"
        );
    UtLayer<Limit::Layer> limit_layer;

    aeLimitCreate aeLim( limit_layer, bill.getTheEvent<Limit::Event>() );
    aeLim.Action();

    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Reason=1601") );
}

void TC_Hem_aeLimit::Replace()
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
    }
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_mww'>"
        L"  <Pre Id='i25' />"
        L"  <Action code='Replace' layer='limit' />"
        L"  <Data Id='i25' StartTime='20151019T233024Z' StopTime='20151020T003024Z' esr='11050:11051' Reason='2222' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
        L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
        L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
        L"  </Data>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i25' and @Reason=1111") );
     
    aeLimitReplace aeRepl( limit_layer, bill.getTheEvent<Limit::Event>() );
    aeRepl.Action();

    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i25' and @Reason=2222") );
}

void TC_Hem_aeLimit::Remove()
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
            L"  <Data Id='i33' StartTime='20151019T223024Z' StopTime='20151019T233024Z' esr='11050:11051' Reason='2222' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"  </Data>"
            );
        limit_layer.Push(evSource);
    }
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_mww'>"
        L"  <Pre Id='i25' />"
        L"  <Action code='Remove' layer='limit' />"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( limit_layer.path_count() == 2 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i25'") );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i33'") );

    aeLimitRemove aeRem( limit_layer, bill.getTheEvent<Limit::Event>() );
    aeRem.Action();

    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i33'") );
}

void TC_Hem_aeLimit::Expire()
{
    UtLayer<Limit::Layer> limit_layer;
    {
        Limit::Event evSource = createEvent<Limit::Event>(
            L"<Data Id='G102030405060' StartTime='20151019T223024Z' StopTime='20151020T000000Z' esr='11050:11051' Reason='1111' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61' >"
            L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
            L"</Data>"
            );
        limit_layer.Push(evSource);
    }
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_mww'>"
        L"  <Action code='Expire' layer='limit' />"
        L"  <Data Id='G102030405060' StopTime='20161022T111111Z' />"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='R102030405060' and @StopTime='20151020T000000Z'") );

    aeLimitExpire aeRepl( limit_layer, bill.getTheEvent<Limit::Event>() );
    aeRepl.Action();

    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='R102030405060' and @StopTime='20161022T111111Z'") );
}
