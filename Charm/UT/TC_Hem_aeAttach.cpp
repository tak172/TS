#include "stdafx.h"
#include <boost/date_time/posix_time/time_parsers.hpp>
#include "TC_Hem_aeAttach.h"
#include "TC_Hem_ae_helper.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/Bill.h"
#include "../Hem/Appliers/aeAttach.h"
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Hem/Appliers/aeRectify.h"
#include "../Hem/NoteLayer.h"
#include "../helpful/StationsDigraph.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/LocoCharacteristics.h"
#include "../helpful/Time_Iso.h"
#include "TopologyTest.h"
#include "AutoChartTest.h"
#include "../Hem/Appliers/Context.h"
#include "../Hem/GuessTransciever.h"
#include "../helpful/Crew.h"
#include "../Hem/UserChart.h"
#include "../Hem/AsoupToSpot.h"
#include "UtNsiBasis.h"

using namespace std;
using namespace HemHelpful;
using namespace Hem;
using namespace Hem::ApplierCarryOut;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeAttach );

static Hem::Chance sysChance = Hem::Chance::System();

void TC_Hem_aeAttach::setUp()
{
    happen_layer = make_shared< UtLayer<HappenLayer> >();
    asoup_layer = make_shared< UtLayer<AsoupLayer> >();
    topology.reset( new TopologyTest );
    esrGuide.reset( new EsrGuide );
    undoer.reset();
    guessTransciever.reset( new Hem::GuessTransciever );
}

void TC_Hem_aeAttach::Normal_NoneTopology()
{
    SpotEvent one[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
    };
    SpotEvent two[] = {
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    happen_layer->createPath( one );
    happen_layer->createPath( two );

    SpotEvent a( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 );
    SpotEvent b( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 );
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, *prev(end(one)), *begin(two), false );
    att.Action();
    CPPUNIT_ASSERT( happen_layer->path_count() == 2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1p[00100] 1p[00100] 2p[00200]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "4p[00400]  5p[00500]" ) );
}

void TC_Hem_aeAttach::ArrDep_Form_MatchWay()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='12w[11111:22222]' create_time='20151224T085854Z' waynum='1'>"
        L"    <rwcoord picketing1_val='423~500' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='422~500' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Arrival' Bdg='2w[22222]' create_time='20151224T090351Z' waynum='2' parknum='1' index='' num='D2819' fretran='Y' />"
        L"  <SpotEvent name='Departure' Bdg='2w[22222]' create_time='20151224T091125Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Death' Bdg='2w[22222]' create_time='20151224T091233Z' waynum='2' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='2w[22222]' create_time='20151224T091324Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Station_exit' Bdg='2x[22222]' create_time='20151224T091324Z' />"
        L"  <SpotEvent name='Arrival' Bdg='3w[33333]' create_time='20151224T092851Z' waynum='1' parknum='1' index='' num='D2819' fretran='Y' />"
        L"  <SpotEvent name='Departure' Bdg='3w[33333]' create_time='20151224T093432Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='4w[44444]' create_time='20151224T094542Z' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Departure' Bdg='2w[22222]' create_time='20151224T091125Z' index='' num='D2819' fretran='Y' waynum='2' parknum='1' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='2w[22222]' create_time='20151224T091324Z' waynum='2' parknum='1' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att(  AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form             Arrival   Departure  Station_exit Arrival   Departure Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "12w[11111:22222] 2w[22222] 2w[22222]    2x[22222] 3w[33333]  3w[33333] 4w[44444] " ) );
}

void TC_Hem_aeAttach::Entry_FormExit()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='2C[11111]' create_time='20151224T083646Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='2C[11111]' create_time='20151224T084705Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Station_entry' Bdg='PAP[22222]' create_time='20151224T085446Z' />"
        L"  <SpotEvent name='Death' Bdg='PAP[22222]' create_time='20151224T085711Z' />"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='2AC[22222]' create_time='20151224T091308Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Station_exit' Bdg='NAP[22222]' create_time='20151224T091308Z' />"
        L"  <SpotEvent name='Arrival' Bdg='2C[33333]' create_time='20151224T092902Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='2C[33333]' create_time='20151224T094927Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Arrival' Bdg='3C[44444]' create_time='20151224T100417Z' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Station_entry' Bdg='PAP[22222]' create_time='20151224T085446Z' index='0905-014-1600' num='2724' fretran='Y' />"
        L"<Action code='Attach' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='2AC[22222]' create_time='20151224T091308Z' waynum='2' parknum='1' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att(  AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form      Departure Station_entry Station_exit   Arrival  Departure Arrival " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "2C[11111] 2C[11111] PAP[22222]     NAP[22222]   2C[33333] 2C[33333] 3C[44444] " ) );
}

void TC_Hem_aeAttach::Some_removableTerminalForm()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Transition' Bdg='1C[11111]' create_time='20151223T075606Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[22222]' create_time='20151223T081216Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='LT8C[22222:33333]' create_time='20151223T081825Z' waynum='1'>"
        L"    <rwcoord picketing1_val='237~500' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='238~200' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='LT6C[22222:33333]' create_time='20151223T081937Z' waynum='1'>"
        L"    <rwcoord picketing1_val='236~100' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='236~800' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='NDP[33333]' create_time='20151223T082350Z' />"
        L"  <SpotEvent name='Station_exit' Bdg='NDP[33333]' create_time='20151223T082350Z' />"
        L"  <SpotEvent name='Transition' Bdg='1C[44444]' create_time='20151223T083208Z' waynum='1' parknum='1' index='' num='J2212' fretran='Y' />"
        L"  <SpotEvent name='Transition' Bdg='1C[55555]' create_time='20151223T084310Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Death'      Bdg='1C[55555]' create_time='20151223T100321Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Span_move' Bdg='LT8C[22222:33333]' create_time='20151223T081825Z' index='' num='J2212' fretran='Y' waynum='1'>"
        L"<rwcoord picketing1_val='237~850' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='Attach' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='NDP[33333]' create_time='20151223T082350Z' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Transition      Span_move     Station_entry Station_exit Transition Transition  Death" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "1C[11111]  1C[22222]  LT8C[22222:33333]   NDP[33333]    NDP[33333]   1C[44444]  1C[55555] 1C[55555]" ) );
}

void TC_Hem_aeAttach::EntraStopping_Some()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Transition' Bdg='1C[11302]' create_time='20151223T061744Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Entra_stopping' Bdg='N2P[11301:11302]' create_time='20151223T062342Z' waynum='1'>"
        L"    <rwcoord picketing1_val='235~398' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='234~250' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Disform' Bdg='N2P[11301:11302]' create_time='20151223T062442Z' waynum='1'>"
        L"    <rwcoord picketing1_val='235~398' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='234~250' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='1C[11301]' create_time='20151223T062513Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='1C[11301]' create_time='20151223T062713Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[11300]' create_time='20151223T063730Z' waynum='1' parknum='1' index='' num='2614' fretran='Y' />"
        L"  <SpotEvent name='Death' Bdg='N1P_PAS[06610:11290]' create_time='20151223T074931Z' waynum='1'>"
        L"    <rwcoord picketing1_val='280~200' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Entra_stopping' Bdg='N2P[11301:11302]' create_time='20151223T062342Z' index='8626-158-0906' num='2419' fretran='Y' waynum='1'>"
        L"    <rwcoord picketing1_val='234~824' picketing1_comm='Посинь' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='1C[11301]' create_time='20151223T062513Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Entra_stopping   Arrival   Departure Transition Death" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "1C[11302]  N2P[11301:11302] 1C[11301] 1C[11301] 1C[11300]  N1P_PAS[06610:11290]" ) );
}

void TC_Hem_aeAttach::Some_FormArrivalOrTransition()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='1C[11070]' create_time='20151218T092951Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='1C[11070]' create_time='20151218T094026Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1SP[11063]' create_time='20151218T094308Z' index='' num='2838' fretran='Y' />"
        L"  <SpotEvent name='Span_move' Bdg='NB5C[11062:11063]' create_time='20151218T095011Z' waynum='1'>"
        L"    <rwcoord picketing1_val='404~600' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='405~400' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='NB5C[11062:11063]' create_time='20151218T095109Z' waynum='1'>"
        L"    <rwcoord picketing1_val='404~600' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='405~400' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='1C[11062]' create_time='20151218T095447Z' waynum='1' parknum='1' index='' num='2838' fretran='Y' />"
        L"  <SpotEvent name='Arrival' Bdg='1C[11062]' create_time='20151218T095807Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='1C[11062]' create_time='20151218T100344Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[11060]' create_time='20151218T101802Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Death' Bdg='NB5C[11062:11063]' create_time='20151218T095109Z' index='' num='2838' fretran='Y' waynum='1'>"
        L"    <rwcoord picketing1_val='404~1000' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='1C[11062]' create_time='20151218T095447Z' index='' num='2838' fretran='Y' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    setTopologyPath(11062, 11063);

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att(AccessHappenAsoup( hl.base(), asoup_layer->base() ), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form      Departure Transition    Span_move       Arrival   Departure Transition" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "1C[11070] 1C[11070] 1SP[11063] NB5C[11062:11063] 1C[11062]  1C[11062]  1C[11060] " ) );
}

void TC_Hem_aeAttach::Normal_SimpleTopology()
{
    SpotEvent one[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
    };
    SpotEvent two[] = {
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    happen_layer->createPath( one );
    happen_layer->createPath( two );

    SpotEvent a( *(end(one)-1) );
    SpotEvent b( *begin(two) );

    topology->Load( L"*100,500 {100,200,300,400,500}\n"
                     L"=100,500,700\n"
                     );

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, a, b, false );
    att.Action();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Transition Transition Transition Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1p[00100] 1p[00100] 2p[00200]  ?[00300]   4p[00400]  5p[00500]" ) );
}

void TC_Hem_aeAttach::Normal_and_Undo()
{
    SpotEvent one[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
    };
    SpotEvent two[] = {
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    happen_layer->createPath( one );
    happen_layer->createPath( two );

    setTopologyPath(200, 400);

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, *prev(end(one)), *begin(two), false );
    att.Action();
    collectUndo();
    attic::a_document dd("do");
    happen_layer->TakeChanges( dd.document_element() );
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Transition Transition Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1p[00100] 1p[00100] 2p[00200]  4p[00400]  5p[00500]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(2) );
}

void TC_Hem_aeAttach::AttachExistToNew()
{
    SpotEvent one[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
    };
    happen_layer->createPath( one );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

    setTopologyPath(200, 777);

    SpotEvent x( HCode::TRANSITION, BadgeE(L"7p",EsrKit(777)), T+20 );
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, *prev(end(one)), x, false );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Transition Transition Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1p[00100] 1p[00100] 2p[00200]  7p[00777]  7p[00777]" ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
}

void TC_Hem_aeAttach::AttachNewDepartureToExist()
{
    SpotEvent one[] = {
        SpotEvent( HCode::ARRIVAL,    BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
    };
    happen_layer->createPath( one );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

    setTopologyPath(777, 100);

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, 
                  SpotEvent( HCode::DEPARTURE,  BadgeE(L"7p",EsrKit(777)), T+5 ),
                  *begin(one), false
                  );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Arrival   Departure Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "7p[00777] 1p[00100] 1p[00100] 2p[00200]" ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
}

void TC_Hem_aeAttach::AttachNewArrivalToExist()
{
    SpotEvent one[] = {
        SpotEvent( HCode::ARRIVAL,    BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
    };
    happen_layer->createPath( one );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

    setTopologyPath(777, 100);

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, 
        SpotEvent( HCode::ARRIVAL,  BadgeE(L"7p",EsrKit(777)), T+5 ),
        *begin(one), false
        );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Arrival   Departure  Arrival   Departure Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "7p[00777] 7p[00777] 1p[00100] 1p[00100] 2p[00200]" ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
}

void TC_Hem_aeAttach::AttachNewArrivalToExist_SomeWay()
{
    auto& hl = *happen_layer;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form'       Bdg='1p[00100]' create_time='20151224T000010Z' index='1234-456-678' num='9876' />"
        L"  <SpotEvent name='Departure'  Bdg='1p[00100]' create_time='20151224T000015Z' />"
        L"  <SpotEvent name='Transition' Bdg='2p[00200]' create_time='20151224T000022Z' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",        "Form             Departure        Transition" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",         "1p[00100]        1p[00100]        2p[00200]" ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time", "20151224T000010Z 20151224T000015Z 20151224T000022Z" ) );

    namespace bt = boost::posix_time;
    time_t TT = (bt::from_iso_string("20151224T000000")-bt::from_time_t(0)).total_seconds();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, 
        SpotEvent( HCode::ARRIVAL,  BadgeE(L"1p",EsrKit(100)), TT+5 ),
        SpotEvent( HCode::FORM,     BadgeE(L"1p",EsrKit(100)), TT+10 ), false
        );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT_EQUAL(size_t(1), hl.path_count());
    CPPUNIT_ASSERT( hl.exist_series( "name",        "Arrival             Departure        Transition" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",         "1p[00100]        1p[00100]        2p[00200]" ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time", "20151224T000005Z 20151224T000015Z 20151224T000022Z " ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",        "Form             Departure        Transition" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",         "1p[00100]        1p[00100]        2p[00200]" ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time", "20151224T000010Z 20151224T000015Z 20151224T000022Z " ) );
}

void TC_Hem_aeAttach::AttachNewToExist2()
{
    // существующая нить (вторая)
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='1N2IP[11042:16170]' create_time='20151127T094341Z' waynum='1'>"
        L"    <rwcoord picketing1_val='461~800' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Arrival' Bdg='7BC[11042]' create_time='20151127T094727Z' waynum='7' parknum='1' index='' num='D2829' fretran='Y' />"
        L"  <SpotEvent name='Departure' Bdg='7AC[11042]' create_time='20151127T101128Z' waynum='7' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C&apos;2[11041]' create_time='20151127T102416Z' waynum='1' parknum='2' />"
        L"</HemPath>"
        );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint layer='asoup' Bdg='ASOUP 1042[16169]' name='Departure' create_time='20151127T091900Z' index='1600-656-0959' num='2829' fretran='Y'><![CDATA[(:1042 909/000+16169 2829 1600 656 0959 03 11000 27 11 11 19 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 053 05256 03719 244 50100924 50059872 000 000 00 061 000.70 061 000 000\n"
        L"Ю3 575 00029331 1 07 47 1317 00482 ЗИHEBИЧ     \n"
        L"Ю3 575 00029332 9:)]]></PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Arrival' Bdg='7BC[11042]' create_time='20151127T094727Z' index='' num='D2829' fretran='Y' waynum='7' parknum='1' />"
        L"</A2F_HINT>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );

    setTopologyPath(16169, 11042);

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure         Arrival    Departure  Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "ASOUP 1042[16169] 7BC[11042] 7AC[11042] 1C'2[11041] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
}

void TC_Hem_aeAttach::AttachExistToNew2()
{
    //существующая нить (первая)
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20180219T065054Z' name='Transition' Bdg='ASOUP 1042[11451]' waynum='15' parknum='8' index='0982-066-1100' num='V2222' length='57' weight='1352' through='Y'>"
        L"    <Locomotive Series='2ТЭ116' NumLoc='875' Depo='0' Consec='1'>"
        L"      <Crew EngineDriver='USACKIS' Tim_Beg='2018-02-18 19:33' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180219T065306Z' name='Span_move' Bdg='edit[11002:11451]'>"
        L"    <rwcoord picketing1_val='388~157' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180219T065306Z' name='Death' Bdg='edit[11002:11451]'>"
        L"    <rwcoord picketing1_val='388~157' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='edit[11002:11451]' create_time='20180219T065306Z'>"
        L"<rwcoord picketing1_val='388~157' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='Attach' District='11000-12910' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP 1042[11000]' name='ExplicitForm' create_time='20180219T071400Z' index='1100-813-1613' num='' fretran='Y'><![CDATA[(:1042 909/000+16169 2829 1600 656 0959 03 11000 27 11 11 19 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 053 05256 03719 244 50100924 50059872 000 000 00 061 000.70 061 000 000\n"
        L"Ю3 575 00029331 1 07 47 1317 00482 ЗИHEBИЧ     \n"
        L"Ю3 575 00029332 9:)]]></PostPoint>"
        L"</A2F_HINT>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

    topology->Load(  L"*11451,11000 {11451,11002,11000} \n" );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition        Span_move            Transition     Arrival          Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "ASOUP 1042[11451]  edit[11002:11451]   ?[11002]   ASOUP 1042[11000] ASOUP 1042[11000] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
}

void TC_Hem_aeAttach::AttachNewArrivalToNewDeparture()
{
    setTopologyPath(100, 200);

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, 
        SpotEvent( HCode::ARRIVAL,    BadgeE(L"1p",EsrKit(100)), T+5 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"2p",EsrKit(200)), T+20 ), false
        );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Arrival   Departure Arrival   Departure  Death"     ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1p[00100] 1p[00100] 2p[00200] 2p[00200] 2p[00200]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==0 );
}

void TC_Hem_aeAttach::AttachNewMoveToNewMove_SomeSpan()
{
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_move' Bdg='?[11342:11343]' create_time='20151203T045216Z'>"
        L"    <rwcoord picketing1_val='504~192' picketing1_comm='Россия' />"
        L"  </PrePoint>"
        L"  <Action code='Create' />"
        L"  <PostPoint name='Span_move' Bdg='?[11342:11343]' create_time='20151203T062152Z'>"
        L"    <rwcoord picketing1_val='501~953' picketing1_comm='Россия' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==0 );
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    auto nNa = make_pair(string("rwcoord"), string("picketing1_val"));
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move      Span_move      Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[11342:11343] ?[11342:11343] ?[11342:11343]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series2( nNa,    "504~192        501~953        501~953" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==0 );
}

void TC_Hem_aeAttach::AttachNewMoveToNewMove_Over1Terminal()
{
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_move' Bdg='?[11360:11363]' create_time='20151203T042640Z'>"
        L"    <rwcoord picketing1_val='518~189' picketing1_comm='Россия' />"
        L"  </PrePoint>"
        L"  <Action code='Create' />"
        L"  <PostPoint name='Span_move' Bdg='?[11343:11360]' create_time='20151203T043304Z'>"
        L"    <rwcoord picketing1_val='512~914' picketing1_comm='Россия' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[11360:11363] ?[11360]   ?[11343:11360] ?[11343:11360]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move      Transition Span_move      Death" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==0 );
}

void TC_Hem_aeAttach::AttachNewMoveToNewArrival_nearSpan()
{
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_move' Bdg='?[11343:11360]' create_time='20151203T042224Z'>"
        L"    <rwcoord picketing1_val='516~528' picketing1_comm='Россия' />"
        L"  </PrePoint>"
        L"  <Action code='Create' />"
        L"  <PostPoint name='Arrival' Bdg='?[11343]' create_time='20151203T044552Z' waynum='2' parknum='1' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move      Arrival  Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[11343:11360] ?[11343] ?[11343]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==0 );
}

void TC_Hem_aeAttach::Death_Form()
{
    SpotDetailsPtr spdet = std::make_shared<SpotDetails>();
    spdet->parkway = ParkWayKit(3,8);

    SpotEvent patA[] = {
        SpotEvent( HCode::SPAN_MOVE, BadgeE(L"1a",EsrKit(302,312)), T+10 ),
        SpotEvent( HCode::ARRIVAL,   BadgeE(L"2a",EsrKit(302)), T+20 ),
        SpotEvent( HCode::DEATH,     BadgeE(L"2a",EsrKit(302)), T+20 ),
    };
    SpotEvent patB[] = {
        SpotEvent( HCode::FORM,      BadgeE(L"22b",EsrKit(302)), T+30, spdet ),
        SpotEvent( HCode::DEPARTURE, BadgeE(L"22b",EsrKit(302)), T+40, spdet ),
    };
    happen_layer->createPath( patA );
    happen_layer->createPath( patB );
    CPPUNIT_ASSERT( happen_layer->path_count() == 2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move Arrival Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure" ) );

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, patA[2], patB[0], false );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move       Arrival   Departure" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1a[00302:00312] 2a[00302] 22b[00302]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count() == 2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move Arrival Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure" ) );
}

void TC_Hem_aeAttach::Overlap_Time_SpanMove()
{
    SpotEvent patA[] = {
        SpotEvent( HCode::SPAN_MOVE,  BadgeE(L"12",EsrKit(100,200)),  T+10 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2",EsrKit(200)),       T+20 ),
        SpotEvent( HCode::DEATH,      BadgeE(L"23a",EsrKit(200,300)), T+27 ),
    };
    SpotEvent patB[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"23b",EsrKit(200,300)), T+23 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3",EsrKit(300)),       T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4",EsrKit(400)),       T+40 ),
    };
    setTopologyPath(200, 300);
    happen_layer->createPath( patA );
    happen_layer->createPath( patB );
    CPPUNIT_ASSERT( happen_layer->path_count() == 2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move Transition Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Transition Transition" ) );

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, patA[2], patB[0], false );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move       Transition Transition Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series(  "Bdg",  "12[00100:00200] 2[00200]   3[00300]   4[00400]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
}

void TC_Hem_aeAttach::Transition_EntraStopping()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='N3P_SKA[07792:11265]' create_time='20151013T050545Z' waynum='1' />"
        L"    <SpotEvent name='Transition' Bdg='1AC[11265]' create_time='20151013T054838Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='N7P[11265:11266]' create_time='20151013T055818Z' waynum='1'>"
        L"        <rwcoord picketing1_val='413~200' picketing1_comm='Россия' />"
        L"        <rwcoord picketing1_val='411~800' picketing1_comm='Россия' />"
        L"    </SpotEvent>"
        L"</HemPath>"
        );

    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='N5P[11265:11266]' create_time='20151013T054934Z' waynum='1'>"
        L"        <rwcoord picketing1_val='414~600' picketing1_comm='Россия' />"
        L"        <rwcoord picketing1_val='413~200' picketing1_comm='Россия' />"
        L"    </SpotEvent>"
        L"    <SpotEvent name='Entra_stopping' Bdg='N1P_PUR[11265:11266]' create_time='20151013T060011Z' waynum='1'>"
        L"        <rwcoord picketing1_val='416~' picketing1_comm='Россия' />"
        L"        <rwcoord picketing1_val='417~400' picketing1_comm='Россия' />"
        L"    </SpotEvent>"
        L"    <SpotEvent name='Transition' Bdg='1C[11266]' create_time='20151013T060323Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"    <PrePoint name='Death' Bdg='N7P[11265:11266]' create_time='20151013T055818Z' waynum='1'>"
        L"        <rwcoord picketing1_val='412~500' picketing1_comm='Россия' />"
        L"    </PrePoint>"
        L"    <Action code='Attach' />"
        L"    <PostPoint name='Form' Bdg='N5P[11265:11266]' create_time='20151013T054934Z' waynum='1'>"
        L"        <rwcoord picketing1_val='413~900' picketing1_comm='Россия' />"
        L"    </PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
	att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form                 Transition Entra_stopping       Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "N3P_SKA[07792:11265] 1AC[11265] N1P_PUR[11265:11266] 1C[11266]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::FormAndStationexit()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"    <SpotEvent name='Form'       Bdg='1C[11401]' create_time='20151007T234153Z' waynum='4' parknum='1' />"
        L"    <SpotEvent name='Transition' Bdg='1C[11401]' create_time='20151008T000958Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Arrival' Bdg='1C[11391]' create_time='20151008T001913Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='1C[11391]' create_time='20151008T002002Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='1-3SP:1+[11391]' create_time='20151008T001933Z' />"
        L"    <SpotEvent name='Station_exit' Bdg='NAP[11391]' create_time='20151008T001933Z' />"
        L"    <SpotEvent name='Transition' Bdg='1C[11390]' create_time='20151008T003505Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Arrival' Bdg='2C[11381]' create_time='20151008T004630Z' waynum='2' parknum='1' index='0905-087-3100' num='2750' fretran='Y' />"
        L"    <SpotEvent name='Departure' Bdg='2C[11381]' create_time='20151008T012658Z' waynum='2' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='2C[11381]' create_time='20151008T012658Z' waynum='2' parknum='1' index='0001-087-1131' num='4502' resloc='Y' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"    <PrePoint name='Death' Bdg='1C[11391]' create_time='20151008T002002Z' waynum='1' parknum='1' />"
        L"    <Action code='Attach' />"
        L"    <PostPoint name='Form' Bdg='1-3SP:1+[11391]' create_time='20151008T001933Z' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Transition Arrival   Departure Station_exit Transition Arrival   Departure Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1C[11401] 1C[11401]  1C[11391] 1C[11391] NAP[11391]  1C[11390]  2C[11381] 2C[11381] 2C[11381]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::MoveDeathAndSome()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='1C[11062]' create_time='20151120T153910Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='1C[11062]' create_time='20151120T154217Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='3SP[11063]' create_time='20151120T155201Z' />"
        L"  <SpotEvent name='Span_move' Bdg='N15P[11063:11775]' create_time='20151120T155427Z' waynum='1'>"
        L"    <rwcoord picketing1_val='16~400' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='16~700' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='N15P[11063:11775]' create_time='20151120T155442Z' waynum='1'>"
        L"    <rwcoord picketing1_val='16~400' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='16~700' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Span_move' Bdg='N5P[11063:11775]' create_time='20151120T155702Z' waynum='1'>"
        L"    <rwcoord picketing1_val='14~900' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='15~200' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Transition' Bdg='PAP[11775]' create_time='20151120T155819Z' />"
        L"  <SpotEvent name='Transition' Bdg='1SP[11363]' create_time='20151120T160028Z' />"
        L"  <SpotEvent name='Span_move' Bdg='PM8P[11010:11363]' create_time='20151120T160550Z' waynum='2'>"
        L"    <rwcoord picketing1_val='5~500' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='6~500' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='PM8P[11010:11363]' create_time='20151120T161058Z' waynum='2'>"
        L"    <rwcoord picketing1_val='5~500' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='6~500' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Span_move' Bdg='N15P[11063:11775]' create_time='20151120T155427Z' index='' num='D1821' liqrr='Y' waynum='1'>"
        L"<rwcoord picketing1_val='16~550' picketing1_comm='524km' />"
        L"</PrePoint>"
        L"<Action code='Attach' />"
        L"<PostPoint name='Span_move' Bdg='N5P[11063:11775]' create_time='20151120T155702Z' waynum='1'>"
        L"<rwcoord picketing1_val='15~50' picketing1_comm='524km' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure Transition Span_move Span_move Transition Transition Span_move Death " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::SpanMoveAndSome()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Span_move' Bdg='N14P[11063:11775]' create_time='20151120T155427Z' waynum='1'>"
        L"    <rwcoord picketing1_val='15~100' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='15~111' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='N15P[11063:11775]' create_time='20151120T155427Z' waynum='1'>"
        L"    <rwcoord picketing1_val='16~400' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='16~700' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='N15P[11063:11775]' create_time='20151120T155442Z' waynum='1'>"
        L"    <rwcoord picketing1_val='16~400' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='16~700' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='N16P[11063:11775]' create_time='20151120T155666Z' waynum='1'>"
        L"    <rwcoord picketing1_val='17~700' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='17~777' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Transition' Bdg='PAP[11775]' create_time='20151120T155819Z' />"
        L"  <SpotEvent name='Span_move' Bdg='PM8P[11775:11363]' create_time='20151120T160550Z' waynum='2'>"
        L"    <rwcoord picketing1_val='5~500' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='6~500' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='PM8P[11775:11363]' create_time='20151120T161058Z' waynum='2'>"
        L"    <rwcoord picketing1_val='5~500' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='6~500' picketing1_comm='524km' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_move' Bdg='N15P[11063:11775]' create_time='20151120T155427Z' waynum='1'>"
        L"    <rwcoord picketing1_val='16~400' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='16~700' picketing1_comm='524km' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='N16P[11063:11775]' create_time='20151120T155666Z' waynum='1'>"
        L"    <rwcoord picketing1_val='17~700' picketing1_comm='524km' />"
        L"    <rwcoord picketing1_val='17~777' picketing1_comm='524km' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move         Transition Span_move         Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "N14P[11063:11775] PAP[11775] PM8P[11363:11775] PM8P[11363:11775]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::DisformAndSome()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='4C[09750]' create_time='20151123T091958Z' waynum='4' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='4C[09750]' create_time='20151123T094543Z' waynum='4' parknum='1' index='0918-052-0982' num='3441' fretran='Y' />"
        L"  <SpotEvent name='Span_move' Bdg='TS15C[09750:09772]' create_time='20151123T100114Z' waynum='1'>"
        L"    <rwcoord picketing1_val='120~551' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='121~202' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Disform' Bdg='TS15C[09750:09772]' create_time='20151123T100302Z' waynum='1'>"
        L"    <rwcoord picketing1_val='120~551' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='121~202' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='TS16C[09750:09772]' create_time='20151123T100252Z' waynum='1'>"
        L"    <rwcoord picketing1_val='121~202' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='121~976' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Arrival' Bdg='1C[09772]' create_time='20151123T100719Z' waynum='1' parknum='1' index='' num='V2218' fretran='Y' />"
        L"  <SpotEvent name='Disform' Bdg='1C[09772]' create_time='20151123T102443Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_move' Bdg='TS15C[09750:09772]' create_time='20151123T100114Z' index='0918-052-0982' num='3441' fretran='Y' waynum='1'>"
        L"    <rwcoord picketing1_val='120~876' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='TS16C[09750:09772]' create_time='20151123T100252Z' waynum='1'>"
        L"    <rwcoord picketing1_val='121~589' picketing1_comm='Вентспилс' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure Span_move Arrival Disform" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::Disform_FormStationexit()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='3C[11420]' create_time='20151210T130841Z' waynum='3' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='3C[11420]' create_time='20151210T130841Z' waynum='3' parknum='1' />"
        L"  <SpotEvent name='Arrival' Bdg='1C[11391]' create_time='20151210T134924Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Disform' Bdg='1C[11391]' create_time='20151210T134924Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='NAP[11391]' create_time='20151210T135036Z' />"
        L"  <SpotEvent name='Station_exit' Bdg='NAP[11391]' create_time='20151210T135036Z' />"
        L"  <SpotEvent name='Span_move' Bdg='N5PC[11390:11391]' create_time='20151210T135211Z' waynum='1'>"
        L"    <rwcoord picketing1_val='166~328' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='166~628' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Disform' Bdg='1C[11391]' create_time='20151210T134924Z' waynum='1' parknum='1' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='NAP[11391]' create_time='20151210T135036Z' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Arrival   Departure Station_exit  Span_move         " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "3C[11420] 3C[11420] 1C[11391] 1C[11391]  NAP[11391]  N5PC[11390:11391] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::SentryDeath_Form()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='1/3SP[11390]' create_time='20151207T053950Z' />"
        L"  <SpotEvent name='Transition' Bdg='1C[11390]' create_time='20151207T055040Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Station_entry' Bdg='NAP[11391]' create_time='20151207T060740Z' />"
        L"  <SpotEvent name='Death' Bdg='1-3SP[11391]' create_time='20151207T060854Z' />"
        L"</HemPath>"
        );

    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='1C[11391]' create_time='20151207T060855Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='1C[11391]' create_time='20151207T061055Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[11401]' create_time='20151207T061847Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='1-3SP[11391]' create_time='20151207T060854Z' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='1C[11391]' create_time='20151207T060855Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form         Transition Station_entry Departure Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1/3SP[11390] 1C[11390]   NAP[11391]  1C[11391] 1C[11401] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::DepartureDeath_Form()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='X[11340]' create_time='20151209T065902Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='X[11340]' create_time='20151209T070002Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Death' Bdg='PAP[11340]' create_time='20151209T070030Z' />"
        L"</HemPath>"
        );

    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='N9P[11340:11342]' create_time='20151209T070035Z' waynum='1'>"
        L"    <rwcoord picketing1_val='492~800' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='493~800' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Transition' Bdg='Y[11342]' create_time='20151209T070758Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Death' Bdg='PAP[11342]' create_time='20151209T070831Z' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='PAP[11340]' create_time='20151209T070030Z' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='N9P[11340:11342]' create_time='20151209T070035Z' waynum='1'>"
        L"<rwcoord picketing1_val='493~300' picketing1_comm='Россия' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    setTopologyPath(11340, 11342);

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form     Departure Transition Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "X[11340] X[11340]  Y[11342]   PAP[11342]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::TransitionDeath_More()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Transition' Bdg='1C[11270]' create_time='20151209T150207Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1AC[11267]' create_time='20151209T151026Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Death'      Bdg='1AC[11267]' create_time='20151209T151110Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='N3P[11266:11267]' create_time='20151209T151542Z' waynum='1'>"
        L"    <rwcoord picketing1_val='425~600' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='423~800' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Entra_stopping' Bdg='N9P[11266:11267]' create_time='20151209T151951Z' waynum='1'>"
        L"    <rwcoord picketing1_val='418~400' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='420~200' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Transition' Bdg='1C[11266]' create_time='20151209T152125Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Death' Bdg='1AC[11267]' create_time='20151209T151110Z' waynum='1' parknum='1' />"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='N3P[11266:11267]' create_time='20151209T151542Z' waynum='1'>"
        L"    <rwcoord picketing1_val='424~700' picketing1_comm='Россия' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition Transition Entra_stopping   Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1C[11270]  1AC[11267] N9P[11266:11267] 1C[11266] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::Sentry_ToNew()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='N1P[11000:11070]' create_time='20160201T062924Z' waynum='3'>"
        L"    <rwcoord picketing1_val='390~800' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='391~220' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Transition' Bdg='1C[11070]' create_time='20160201T064207Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[11051]' create_time='20160201T071323Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[11050]' create_time='20160201T072254Z' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );

    attic::a_document temp;
    auto x = createEvent<SpotEvent>(L"<SpotEvent name='Station_entry'  Bdg='PDP[11050]' create_time='20160201T072254Z' />");
    auto y = createEvent<SpotEvent>(L"<SpotEvent name='Arrival' Bdg='ASOUP 1042[16169]' create_time='20160201T084000Z' />");

    setTopologyPath(11050, 16169);

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, x, y, false );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "    Form         Transition Transition Station_entry  Station_exit    Arrival           Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "N1P[11000:11070] 1C[11070]   1C[11051]  PDP[11050]     PDP[11050]  ASOUP 1042[16169] ASOUP 1042[16169]" ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
}

void TC_Hem_aeAttach::SentrySexit_balance()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='PV2[09813:09820]' create_time='20160210T060540Z' waynum='2'>"
        L"    <rwcoord picketing1_val='1~5' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Arrival'   Bdg='1C[09813]' create_time='20160210T061012Z' waynum='1' parknum='1' index='' num='V8302' util='Y' />"
        L"  <SpotEvent name='Departure' Bdg='1C[09813]' create_time='20160210T062153Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[09812]' create_time='20160210T064056Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Arrival'   Bdg='3AC[09810]' create_time='20160210T070023Z' waynum='3' parknum='1' index='0982-066-0980' num='8302' util='Y' />"
        L"  <SpotEvent name='Departure' Bdg='3BC[09810]' create_time='20160210T072648Z' waynum='3' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[09803]' create_time='20160210T073925Z' waynum='1' parknum='1' index='' num='V8302' util='Y' />"
        L"  <SpotEvent name='Arrival'   Bdg='5AC[09802]' create_time='20160210T074900Z' waynum='5' parknum='1' index='' num='V8303' util='Y' />"
        L"  <SpotEvent name='Departure' Bdg='5AC[09802]' create_time='20160210T075840Z' waynum='5' parknum='1' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T085939Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T085949Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T090007Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T090030Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T090118Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T090157Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T090413Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T090415Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T090551Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T091957Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T094428Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T094437Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T094806Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T095400Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T095725Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T095727Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T095927Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]' create_time='20160210T095927Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='PDP[09802]' create_time='20160210T101315Z' />"
        L"  <SpotEvent name='Station_exit'  Bdg='PDP[09802]'       create_time='20160210T101330Z' />"
        L"  <SpotEvent name='Span_move' Bdg='SU8C[09802:09803]' create_time='20160210T101330Z' waynum='1'>"
        L"    <rwcoord picketing1_val='45~306' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='44~428' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Disform' Bdg='SU8C[09802:09803]' create_time='20160210T103042Z' waynum='1'>"
        L"    <rwcoord picketing1_val='45~306' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='44~428' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='5BC[09802]' create_time='20160210T104809Z' waynum='5' parknum='1' index='' num='V8301' util='Y' />"
        L"  <SpotEvent name='Departure' Bdg='5AC[09802]' create_time='20160210T111111Z' waynum='5' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[09803]' create_time='20160210T112214Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[09810]' create_time='20160210T113420Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[09812]' create_time='20160210T115107Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[09813]' create_time='20160210T115912Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='PV1IP[09813:09820]' create_time='20160210T115940Z' waynum='2'>"
        L"    <rwcoord picketing1_val='3~5' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='1~5' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='PV1IP[09813:09820]' create_time='20160210T120105Z' waynum='2'>"
        L"    <rwcoord picketing1_val='3~5' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='1~5' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(31) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(8) );

    attic::a_document temp;
    auto x = createEvent<SpotEvent>(
         L"  <SpotEvent name='Span_move' Bdg='SU8C[09802:09803]' create_time='20160210T101330Z' waynum='1'>"
         L"    <rwcoord picketing1_val='45~306' picketing1_comm='Вентспилс' />"
         L"    <rwcoord picketing1_val='44~428' picketing1_comm='Вентспилс' />"
         L"  </SpotEvent>"
        );
    auto y = createEvent<SpotEvent>(
        L"  <SpotEvent name='Form' Bdg='5BC[09802]' create_time='20160210T104809Z' waynum='5' parknum='1' index='' num='V8301' util='Y' />"
        );

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, x, y, false );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(38) );
    x = createEvent<SpotEvent>(L"  <SpotEvent name='Span_move' Bdg='PV1IP[09813:09820]' create_time='20160210T115940Z' waynum='2'>"
        L"    <rwcoord picketing1_val='3~5' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='1~5' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        );
    auto eventPtr = std::make_shared<SpotEvent>(x);
    Hem::aeRectify rectifyEngine(happen_layer->GetWriteAccess(), eventPtr );
    rectifyEngine.Action();

    CPPUNIT_ASSERT( happen_layer->exist_path_size(19) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(31) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(8) );
}

bool TC_Hem_aeAttach::implementUndo()
{
    bool res = true;
    for( auto trNode : undoer.document_element().child(HappenLayer_xAttr).children() )
    {
        TrioSpot eTrio;
        trNode >> eTrio;
        Hem::aePusherUserHappen pusher( happen_layer->GetWriteAccess(), eTrio, identifyCategory );
        pusher.Action();
        if ( pusher.haveError() )
            res = false;
    }
    return res;
}

void TC_Hem_aeAttach::collectUndo()
{
    undoer.reset();
    happen_layer->GetReverseChanges( undoer.set_document_element("undo") );
    wstring ws = undoer.pretty_wstr();
    CPPUNIT_ASSERT( !ws.empty() );
}

void TC_Hem_aeAttach::Some_ToLive()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form'      Bdg='1C[11022]' create_time='20151120T153910Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='1C[11022]' create_time='20151120T154217Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='N15P[11022:11033]' create_time='20151120T155427Z' waynum='1' />"
        L"  <SpotEvent name='Death'     Bdg='N15P[11022:11033]' create_time='20151120T155442Z' waynum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Span_move' Bdg='N5P[11022:11033]' create_time='20151120T155702Z' waynum='1' />"
        L"  <SpotEvent name='Transition' Bdg='PAP[11033]' create_time='20151120T155819Z' />"
        L"  <SpotEvent name='Span_move' Bdg='PM8P[11033:11044]' create_time='20151120T160550Z' waynum='2' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure Span_move Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move Transition Span_move" ) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

    // билль на склейку двух нитей
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Span_move' Bdg='N15P[11022:11033]' create_time='20151120T155427Z' waynum='1' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Span_move' Bdg='N5P[11022:11033]' create_time='20151120T155702Z' waynum='1' />"
        L"</A2F_HINT>"
        );
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    // результат выполнения билля
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure Span_move Span_move Transition Span_move" ) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(6) );

    // дополнение живой нити некими событиями
    {
        SpotEventPtr mov;
        mov = make_shared<SpotEvent>( createEvent<SpotEvent>(
            L"<SpotEvent name='Span_move' Bdg='PM8P[11033:11044]' create_time='20151120T160550Z' waynum='2' />") );
        std::array<SpotEventPtr,2> ad;
        ad[0] = make_shared<SpotEvent>( createEvent<SpotEvent>(
            L"<SpotEvent name='Arrival'   Bdg='2p[11044]'         create_time='20151120T163300Z' />" ) );
        ad[1] = make_shared<SpotEvent>( createEvent<SpotEvent>(
            L"<SpotEvent name='Departure' Bdg='2p[11044]'         create_time='20151120T163444Z' />" ) );
        TrioSpot nextTrio;
        nextTrio.pre( mov );
        nextTrio.body( ad.begin(), ad.end() );
        Hem::aePusherUserHappen pusher( happen_layer->GetWriteAccess(), nextTrio, identifyCategory );
        pusher.Action();
//         Hem::aeRectify rectify( happen_layer->GetWriteAccess(), ad.front() );
//         rectify.Action();
    }
     // проверка дополнения
     CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
//     CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure Transition Arrival Departure" ) );
//     CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );

    std::wstring stringRepresentation = happen_layer->result()->pretty_wstr();
    CPPUNIT_ASSERT(!stringRepresentation.empty());

    // откат билля
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure Span_move Death" ) );
//     CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move Transition Arrival Departure" ) );
//     CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
}

void TC_Hem_aeAttach::Exist_to_IncompleteExist()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form'       Bdg='1PG[11312]' create_time='20160218T132941Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure'  Bdg='1PG[11312]' create_time='20160218T133053Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[11302]' create_time='20160218T133908Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Arrival'    Bdg='2C[11301]' create_time='20160218T135040Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Departure'  Bdg='2C[11301]' create_time='20160218T140448Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[11300]' create_time='20160218T142121Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Span_move'  Bdg='N4P[11292:11300]' create_time='20160218T142619Z' waynum='1'>"
        L"    <rwcoord picketing1_val='255~873' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='253~800' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='N4P[11292:11300]' create_time='20160218T142704Z' waynum='1'>"
        L"    <rwcoord picketing1_val='255~873' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='253~800' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='N4P[11292:11300]' create_time='20160218T142855Z' waynum='1'>"
        L"    <rwcoord picketing1_val='255~873' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='253~800' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Station_entry' Bdg='PAP[11292]' create_time='20160218T143113Z' />"
        L"  <SpotEvent name='Disform' Bdg='3C[11292]' create_time='20160218T143145Z' waynum='3' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(8) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Span_move' Bdg='N4P[11292:11300]' create_time='20160218T142619Z' waynum='1'>"
        L"<rwcoord picketing1_val='254~836' picketing1_comm='Рига-Зилупе' />"
        L"</PrePoint>"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='N4P[11292:11300]' create_time='20160218T142855Z' waynum='1'>"
        L"<rwcoord picketing1_val='254~836' picketing1_comm='Рига-Зилупе' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(9) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(8) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
}

void TC_Hem_aeAttach::GenerationNote_EntranceStopping()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form'      Bdg='N2[11111:22222]' create_time='20151120T151111Z' waynum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='N2[11111:22222]' create_time='20151120T151111Z' waynum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='N1[11111:22222]' create_time='20151120T152222Z' waynum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Span_move Span_move" ) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

    // слой пометок сначала пуст
    UtLayer<Note::Layer> noteLayer;
    CPPUNIT_ASSERT( noteLayer.path_count()==0 );
    // дополнение событием "стоянка перед входным"
    {
        TrioSpot tri;
        tri.pre( make_shared<SpotEvent>( createEvent<SpotEvent>(
            L"  <SpotEvent name='Span_move' Bdg='N1[11111:22222]' create_time='20151120T152222Z' waynum='1' />"
            ) ) );
        tri.body( make_shared<SpotEvent>( createEvent<SpotEvent>(
            L"<SpotEvent name='Entra_stopping' Bdg='N1[11111:22222]' create_time='20151120T153333Z' waynum='1' />"
            ) ) );
        Hem::aePusherUserHappen pusher( happen_layer->GetWriteAccess(), tri, identifyCategory, &noteLayer );
        pusher.Action();
        Hem::aeRectify rectify( happen_layer->GetWriteAccess(), tri.body().back() );
        rectify.Action();
    }
    // проверка дополнения - пометка ещё не появилась
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Entra_stopping" ) );
    CPPUNIT_ASSERT( noteLayer.path_count()==0 );
    // дополнение событием на станции
    {
        TrioSpot tri;
        tri.pre( make_shared<SpotEvent>( createEvent<SpotEvent>(
            L"<SpotEvent name='Entra_stopping' Bdg='N1[11111:22222]' create_time='20151120T153333Z' waynum='1' />"
            ) ) );
        tri.body( make_shared<SpotEvent>( createEvent<SpotEvent>(
            L"<SpotEvent name='Transition'     Bdg='1p[22222]' create_time='20151120T154444Z' />"
            ) ) );
        Hem::aePusherUserHappen pusher( happen_layer->GetWriteAccess(), tri, identifyCategory, &noteLayer );
        pusher.Action();
        Hem::aeRectify rectify( happen_layer->GetWriteAccess(), tri.body().back() );
        rectify.Action();
    }
    // проверка дополнения - пометка появилась
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Entra_stopping Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
    CPPUNIT_ASSERT( noteLayer.path_count()==1 );
}

void TC_Hem_aeAttach::WrongwayDeath_To_Arrival()
{
   // первая нить
    happen_layer->createPath( 
    L"<HemPath>"
    L"  <SpotEvent create_time='20160808T024054Z' name='Form'       Bdg='1SP[11063]' />"
    L"  <SpotEvent create_time='20160808T024717Z' name='Transition' Bdg='2-4SP:2+[11775]' />"
    L"  <SpotEvent create_time='20160808T024914Z' name='Transition' Bdg='11SP[11363]' />"
    L"  <SpotEvent create_time='20160808T025012Z' name='Wrong_way'  Bdg='PM2P[11010:11363]' waynum='2' />"
    L"  <SpotEvent create_time='20160808T025557Z' name='Death'      Bdg='PM8P[11010:11363]' waynum='2' />"
    L"</HemPath>"
    );

    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='PM8P[11010:11363]' create_time='20160808T025557Z' waynum='2'/>"
        L"<Action code='Attach' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP1042[11010]' name='Arrival' create_time='20160808T025700Z' index='1105-008-1100' num='3565' length='7' weight='559' freloc='Y' waynum='2' parknum='1'><![CDATA[(:1042 909/000+11000 3565 1105 008 1100 01 11020 08 08 05 57 03/02 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 007 00559 00419 024 95169389 95331427 000 000 00 006 000.90 006 000 000 95 006 000 000\n"
        L"Ю12 00 95169389 1 069 09590 10300 8239 000 00 00 00 00 00 0232 30 95 0950 04 106 11000 09000 00000 01 09630 24 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95872115 191255255255 070:)]]></PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form       Transition      Transition  Wrong_way         Arrival          Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1SP[11063] 2-4SP:2+[11775] 11SP[11363] PM2P[11010:11363] ASOUP1042[11010] ASOUP1042[11010] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
}

void TC_Hem_aeAttach::WrongwaySpanmoveDeath_To_Arrival()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20160808T024054Z' name='Form'       Bdg='1SP[11063]' />"
        L"  <SpotEvent create_time='20160808T024717Z' name='Transition' Bdg='2-4SP:2+[11775]' />"
        L"  <SpotEvent create_time='20160808T024914Z' name='Transition' Bdg='11SP[11363]' />"
        L"  <SpotEvent create_time='20160808T025012Z' name='Wrong_way'  Bdg='PM2P[11010:11363]' waynum='2' />"
        L"  <SpotEvent create_time='20160808T025407Z' name='Span_move'  Bdg='PM8P[11010:11363]' waynum='2' />"
        L"  <SpotEvent create_time='20160808T025557Z' name='Death'      Bdg='PM8P[11010:11363]' waynum='2' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Span_move' Bdg='PM8P[11010:11363]' create_time='20160808T025407Z' waynum='2'>"
        L"<rwcoord picketing1_val='527~450' picketing1_comm='Россия' />"
        L"</PrePoint>"
        L"<Action code='Attach' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP1042[11010]' name='Arrival' create_time='20160808T025700Z' index='1105-008-1100' num='3565' length='7' weight='559' freloc='Y' waynum='2' parknum='1'><![CDATA[(:1042 909/000+11000 3565 1105 008 1100 01 11020 08 08 05 57 03/02 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 007 00559 00419 024 95169389 95331427 000 000 00 006 000.90 006 000 000 95 006 000 000\n"
        L"Ю12 00 95169389 1 069 09590 10300 8239 000 00 00 00 00 00 0232 30 95 0950 04 106 11000 09000 00000 01 09630 24 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95872115 191255255255 070:)]]></PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form       Transition      Transition  Wrong_way         Arrival          Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1SP[11063] 2-4SP:2+[11775] 11SP[11363] PM2P[11010:11363] ASOUP1042[11010] ASOUP1042[11010] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
}

void TC_Hem_aeAttach::setTopologyPath( unsigned from, unsigned to, std::vector<unsigned> path, unsigned third )
{
    std::wstringstream wss;
    if (!path.empty())
    {
        wss << L"*" << from << L"," << to;
        wss << L" {";
        for (auto it = path.cbegin(); it != path.cend(); ++it)
        {
            if (path.cbegin() != it)
                wss << L",";
            wss << *it;
        }
        wss << L"}\n";
    }
    wss << L"=" << from << L"," << to << L"," << third << L"\n";

    std::wstring topologyParseError = topology->Load(wss);
    CPPUNIT_ASSERT_MESSAGE(To1251(topologyParseError), topologyParseError.empty());

    auto pathes = topology->findPaths(EsrKit(from), EsrKit(to));
    CPPUNIT_ASSERT(!pathes.empty());
    CPPUNIT_ASSERT_EQUAL(size_t(1), pathes.size());
}

void TC_Hem_aeAttach::TransitionDeath_More2()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20160823T115603Z' name='Form' Bdg='N9P_NIC[11432:11446]' waynum='1'>"
        L"    <rwcoord picketing1_val='352~954' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='352~654' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160823T115845Z' name='Transition' Bdg='1C[11432]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20160823T120716Z' name='Station_entry' Bdg='NAP[11431]' />"
        L"  <SpotEvent create_time='20160823T120716Z' name='Death' Bdg='NAP[11431]' />"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20160823T121513Z' name='Form' Bdg='N23P_JER[11430:11431]' waynum='1' index='' num='' noinfo='Y'>"
        L"    <rwcoord picketing1_val='335~549' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='336~67' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160823T122032Z' name='Transition' Bdg='1C[11430]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20160823T123002Z' name='Span_move' Bdg='N17P_LIV[11422:11430]' waynum='1'>"
        L"    <rwcoord picketing1_val='323~18' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='323~318' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160823T123002Z' name='Death' Bdg='N17P_LIV[11422:11430]' waynum='1'>"
        L"    <rwcoord picketing1_val='323~18' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='323~318' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Death' Bdg='NAP[11431]' create_time='20160823T120716Z' />"
        L"  <Action code='Attach' SaveLog='Y' />"
        L"  <PostPoint name='Form' Bdg='N23P_JER[11430:11431]' create_time='20160823T121513Z' index='' num='' noinfo='Y' waynum='1'>"
        L"    <rwcoord picketing1_val='335~808' picketing1_comm='Вентспилс' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    setTopologyPath(11431, 11430);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name",
        "Form                 Transition Station_entry Station_exit Transition        Span_move             Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",
        "N9P_NIC[11432:11446] 1C[11432]  NAP[11431]     NAP[11431]    1C[11430]  N17P_LIV[11422:11430] N17P_LIV[11422:11430]" ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::SpanStop_continue()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20161130T081341Z' name='Form'      Bdg='1C[09780]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161130T085629Z' name='Departure' Bdg='1C[09780]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161130T091336Z' name='Span_stopping_begin' Bdg='KS9C[09780:09790]' waynum='1' intervalSec='751'>"
        L"    <rwcoord picketing1_val='80~110' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='79~360' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161130T094057Z' name='Span_stopping_end' Bdg='KS9C[09780:09790]' waynum='1'>"
        L"    <rwcoord picketing1_val='80~110' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='79~360' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161130T094118Z' name='Span_move' Bdg='KS7C[09780:09790]' waynum='1'>"
        L"    <rwcoord picketing1_val='77~842' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='78~601' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161130T094211Z' name='Death' Bdg='KS7C[09780:09790]' waynum='1'>"
        L"    <rwcoord picketing1_val='77~842' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='78~601' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20161130T094355Z' name='Form' Bdg='1C[09801]' waynum='1' parknum='1' index='1100-076-0983' num='V2209' length='52' weight='4855' fretran='Y'>"
        L"    <feat_texts typeinfo='Т' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161130T094755Z' name='Departure' Bdg='1C[09801]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161130T095330Z' name='Arrival' Bdg='1C[09802]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161130T100338Z' name='Disform' Bdg='1C[09802]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Death' Bdg='KS7C[09780:09790]' create_time='20161130T094211Z' waynum='1'>"
        L"    <rwcoord picketing1_val='78~221' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='1C[09801]' create_time='20161130T094355Z' index='1100-076-0983' num='V2209' length='52' weight='4855' fretran='Y' waynum='1' parknum='1'>"
        L"    <feat_texts typeinfo='Т' />"
        L"  </PostPoint>"
        L"  <EsrList>"
        L"    <Station esr='09790' />"
        L"    <Station esr='09800' />"
        L"    <Station esr='09801' />"
        L"    <Station esr='09802' />"
        L"  </EsrList>"
        L"</A2F_HINT>"
        );

    topology->Load(  L"*09780,09803 {09780,09790,09800,09801,09802,09803} \n" );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure  Span_stopping_begin Span_stopping_end Transition Transition Arrival   Departure Arrival   Disform   " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1C[09780] 1C[09780]  KS9C[09780:09790]   KS9C[09780:09790] ?[09790]   ?[09800]   1C[09801] 1C[09801] 1C[09802] 1C[09802] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::SpanStop_continue2()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20161130T085632Z' name='Form'      Bdg='2C[09803]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20161130T085632Z' name='Departure' Bdg='2C[09803]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20161130T091330Z' name='Span_stopping_begin' Bdg='SU5C[09802:09803]' waynum='1' intervalSec='727'>"
        L"    <rwcoord picketing1_val='41~970' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='42~760' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161130T091330Z' name='Death' Bdg='SU5C[09802:09803]' waynum='1' />"
        L"</HemPath>"
        );
    // вторая нить - отсутствует
    // 
    // выполняем билль
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Death' Bdg='SU5C[09802:09803]' create_time='20161130T091330Z' waynum='1' />"
        L"  <Action code='PushBack' />"
        L"  <PostPoint name='Arrival' Bdg='?[09800]' create_time='20161130T092700Z' waynum='3' parknum='1' />"
        L"  <EsrList>"
        L"    <Station esr='09800' />"
        L"    <Station esr='09801' />"
        L"    <Station esr='09802' />"
        L"  </EsrList>"
        L"</A2F_HINT>"
        );
    // topology->Load(  L"*09780,09803 {09780,09790,09800,09801,09802,09803} \n" );
    topology->Load(  L"*09803,09780 {09803,09802,09801,09800,09790,09780} \n" );

    // до
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    // после
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(8) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Span_stopping_begin Span_stopping_end Transition Transition Arrival  Death    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "2C[09803] 2C[09803] SU5C[09802:09803]   SU5C[09802:09803] ?[09802]   ?[09801]   ?[09800] ?[09800] " ) );
    // а теперь после отката
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
}

void TC_Hem_aeAttach::SpanStop_interval3()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170116T062631Z' name='Transition' Bdg='3p[09160]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20170116T063754Z' name='Transition' Bdg='5p[09150]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20170116T070832Z' name='Span_stopping_begin' Bdg='NBKPU2_O[09104:09150]' waynum='2' intervalSec='1441'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170116T071606Z' name='Death' Bdg='NBKPU2_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170116T065054Z' name='Form' Bdg='KPU4V_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='8~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='10~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170116T065221Z' name='Transition' Bdg='NBP:2/4+[09104]'>"
        L"    <rwcoord picketing1_val='7~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170116T065909Z' name='Transition' Bdg='1p[09100]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    // выполняем билль
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Death' Bdg='NBKPU2_O[09104:09150]' create_time='20170116T071606Z' waynum='2'>"
        L"    <rwcoord picketing1_val='18~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='KPU4V_O[09104:09150]' create_time='20170116T065054Z' waynum='2'>"
        L"    <rwcoord picketing1_val='9~600' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </PostPoint>"
        L"  <EsrList>"
        L"    <Station esr='09104' />"
        L"  </EsrList>"
        L"</A2F_HINT>"
        );
    setTopologyPath(9150, 9104);

    // до
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition Transition Span_stopping_begin   Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "3p[09160]  5p[09150]  NBKPU2_O[09104:09150] NBKPU2_O[09104:09150] " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form                 Transition      Transition " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "KPU4V_O[09104:09150] NBP:2/4+[09104] 1p[09100] " ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    // после
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition Transition Transition      Transition " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "3p[09160]  5p[09150]  NBP:2/4+[09104] 1p[09100] " ) );
    // а теперь после отката
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "3p[09160]  5p[09150]  NBKPU2_O[09104:09150] NBKPU2_O[09104:09150] " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form                 Transition      Transition " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "KPU4V_O[09104:09150] NBP:2/4+[09104] 1p[09100] " ) );
}

void TC_Hem_aeAttach::PocketDeath_Form()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170116T035027Z' name='Transition' Bdg='3p[09160]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20170116T040634Z' name='Transition' Bdg='5p[09150]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20170116T041241Z' name='Span_move' Bdg='KPU10V/5V_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='15~100' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170116T041555Z' name='Pocket_entry' Bdg='KPU10V/5V_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170116T042602Z' name='Death' Bdg='NBKPU2_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170116T041907Z' name='Form' Bdg='KPU8V/7V_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='15~100' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='12~900' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170116T042702Z' name='Transition' Bdg='NBP:2/4+[09104]'>"
        L"    <rwcoord picketing1_val='7~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170116T043446Z' name='Arrival' Bdg='1p[09100]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170116T043450Z' name='Disform' Bdg='1p[09100]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Death' Bdg='NBKPU2_O[09104:09150]' create_time='20170116T042602Z' waynum='2'>"
        L"    <rwcoord picketing1_val='18~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='KPU8V/7V_O[09104:09150]' create_time='20170116T041907Z' waynum='2'>"
        L"    <rwcoord picketing1_val='13~1000' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition Transition       Span_move         Transition      Arrival   Disform " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "3p[09160]  5p[09150] KPU10V/5V_O[09104:09150] NBP:2/4+[09104] 1p[09100] 1p[09100] " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::DeathForm_onSpan()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170712T050633Z' name='Form'      Bdg='5p[09715]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20170712T050633Z' name='Departure' Bdg='5p[09715]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20170712T053914Z' name='Span_move' Bdg='KNU17_S[09715:09730]' waynum='1' />"
        L"  <SpotEvent create_time='20170712T054017Z' name='Death'     Bdg='KNU17_S[09715:09730]' waynum='1' />"
        L"  <SpotEvent create_time='20170712T054018Z' name='Form'      Bdg='KNU15_S[09715:09730]' waynum='1' />"
        L"  <SpotEvent create_time='20170712T054024Z' name='Death'     Bdg='KNU15_S[09715:09730]' waynum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170712T054356Z' name='Form'       Bdg='KNU11_S[09715:09730]' waynum='1' />"
        L"  <SpotEvent create_time='20170712T055147Z' name='Transition' Bdg='2p[09730]' waynum='2' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='KNU15_S[09715:09730]' create_time='20170712T054024Z' waynum='1' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='KNU11_S[09715:09730]' create_time='20170712T054356Z' waynum='1' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Span_move            Transition " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "5p[09715] 5p[09715] KNU17_S[09715:09730]  2p[09730]  " ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::Wrong_and_Span_stopping()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170726T065937Z' name='Form' Bdg='X[11760]' index='0001-009-1142' num='8616' util='Y'>"
        L"    <rwcoord picketing1_val='113~206' picketing1_comm='Рига-Зилупе' />"
        L"    <Locomotive Series='ДГКУ' NumLoc='1861' Depo='0' Consec='1'>"
        L"      <Crew EngineDriver='IVANOVS' Tim_Beg='2017-07-26 08:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170726T065937Z' name='Departure' Bdg='X[11760]'>"
        L"    <rwcoord picketing1_val='113~206' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170726T065937Z' name='Wrong_way' Bdg='Y[11420:11760]' waynum='1'>"
        L"    <rwcoord picketing1_val='113~564' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='113~206' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170726T070019Z' name='Span_move' Bdg='N5P[11420:11760]' waynum='1'>"
        L"    <rwcoord picketing1_val='113~992' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='114~280' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170726T070023Z' name='Death' Bdg='N5P[11420:11760]' waynum='1'>"
        L"    <rwcoord picketing1_val='113~992' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='114~280' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170726T070321Z' name='Form' Bdg='N13P[11420:11760]' waynum='1'>"
        L"    <rwcoord picketing1_val='115~354' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='115~712' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170726T070830Z' name='Span_stopping_begin' Bdg='Z[11420:11760]' waynum='1' intervalSec='215'>"
        L"    <rwcoord picketing1_val='117~502' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='117~860' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Death' Bdg='N5P[11420:11760]' create_time='20170726T070023Z' waynum='1'>"
        L"    <rwcoord picketing1_val='114~136' picketing1_comm='Рига-Зилупе' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' />"
        L"  <PostPoint name='Form' Bdg='N13P[11420:11760]' create_time='20170726T070321Z' waynum='1'>"
        L"    <rwcoord picketing1_val='115~533' picketing1_comm='Рига-Зилупе' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form     Departure Wrong_way      Span_stopping_begin " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "X[11760] X[11760]  Y[11420:11760] Z[11420:11760]      " ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
}

void TC_Hem_aeAttach::ManualExtendFront()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170822T130300Z' name='Span_move' Bdg='?[09764:09772]'>"
        L"    <rwcoord picketing1_val='134~653' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170822T130700Z' name='Arrival' Bdg='?[09772]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170822T130701Z' name='Death' Bdg='?[09772]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить - не существует
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20170822T145959Z'>"
        L"  <PrePoint name='Death' Bdg='?[09772]' create_time='20170822T130701Z' waynum='1' parknum='1' />"
        L"  <Action code='PushBack' SaveLog='Y' />"
        L"  <PostPoint name='Span_move' Bdg='?[09750:09772]' create_time='20170822T131200Z'>"
        L"    <rwcoord picketing1_val='116~1000' picketing1_comm='Вентспилс' />"
        L"  </PostPoint>"
        L"  <EsrList>"
        L"    <Station esr='09772' />"
        L"  </EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move      Arrival  Death    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09764:09772] ?[09772] ?[09772] " ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move      Arrival   Departure  Span_move      Death    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09764:09772] ?[09772]   ?[09772]  ?[09750:09772] ?[09750:09772] " ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move      Arrival  Death    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09764:09772] ?[09772] ?[09772] " ) );
}

void TC_Hem_aeAttach::ManualExtendBack()
{
    // первая нить - не существует
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20171220T125700Z' name='Departure' Bdg='?[09200]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20171220T131300Z' name='Span_move' Bdg='?[09193:09200]'>"
        L"    <rwcoord picketing1_val='265~210' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20171220T131301Z' name='Death' Bdg='?[09193:09200]'>"
        L"    <rwcoord picketing1_val='265~210' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20171220T125643Z'>"
        L"<PrePoint name='Arrival' Bdg='?[09202]' create_time='20171220T123600Z' waynum='1' parknum='1' />"
        L"<Action code='PushFront' District='09180-11420' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Departure' Bdg='?[09200]' create_time='20171220T125700Z' waynum='1' parknum='1' />"
        L"<EsrList>"        L"<Station esr='09200' />"
        L"<Station esr='09202' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Span_move  Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09200] ?[09193:09200] ?[09193:09200]" ) );

    //Jelgava-Krustpils
    topology->Load(  L"*09180,11420 {09180,09181,09251,09241,09240,09230,09220,09211,09210,09202,09200,09193,09191,09190,11420} \n" );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Arrival Departure Arrival Departure  Span_move        Death    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09202] ?[09202] ?[09200] ?[09200] ?[09193:09200] ?[09193:09200]" ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Span_move  Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09200] ?[09193:09200] ?[09193:09200]" ) );
}

void TC_Hem_aeAttach::setTerminalCoord( int esr, const std::string& km )
{
    PicketingInfo pi;
    pi.SetCoord( rwRuledCoord( L"testDir", rwCoord(km) ) );
    esrGuide->setNameAndAxis( EsrKit(esr), L"Term"+std::to_wstring(esr), pi );
}

void TC_Hem_aeAttach::TimeFromDistance()
{
    SpotEvent a( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+0 );
    SpotEvent b( HCode::ARRIVAL,    BadgeE(L"4p",EsrKit(400)), T+60 );

    setTerminalCoord( 100, "1" );
    setTerminalCoord( 200, "41" );  // 100-200 = 40km
    setTerminalCoord( 300, "46" );  // 200-300 =  5km
    setTerminalCoord( 400, "61" );  // 300-400 = 15km

    topology->Load(
    L"*100,500 {100,200,300,400,500}\n"
    L"=100,500,700\n"
        );
    
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, a, b, false );
    att.Action();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name",         "Departure        Transition       Transition       Arrival          Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",          "1p[00100]        ?[00200]         ?[00300]         4p[00400]        4p[00400] " ) );
    // время пропорционально расстоянию
    CPPUNIT_ASSERT( happen_layer->exist_series( "create_time",  "19700111T230000Z 19700111T230040Z 19700111T230045Z 19700111T230100Z 19700111T230100Z " ) );
    // было равномерно                                          "19700111T230000Z 19700111T230020Z 19700111T230040Z 19700111T230100Z 19700111T230100Z "
    auto listEvlist = happen_layer->GetPaths_OnlyEvents(T,T+100);
    CPPUNIT_ASSERT( listEvlist.size() == 1 );
    auto evs = listEvlist.front();
    CPPUNIT_ASSERT( evs.size() == 5 );
}

// путь при генерации промежуточных событий идентифицированного поезда
void TC_Hem_aeAttach::GenerateTransitionWay()
{
    SpotEvent b1[] = {
        SpotEvent( HCode::DEPARTURE, BadgeE(L"1p",EsrKit(100)), T+15,
                    make_shared< SpotDetails>( TrainDescr( L"K1001" ) ) )
    };
    SpotEvent e1[] = {
        SpotEvent( HCode::ARRIVAL, BadgeE(L"4p",EsrKit(400)), T+50 ),
    };
    SpotEvent b2[] = {
        SpotEvent( HCode::TRANSITION, BadgeE(L"1p",EsrKit(100)), T+15,
        make_shared< SpotDetails>( TrainDescr( L"V2002" ) ) )
    };
    SpotEvent e2[] = {
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    happen_layer->createPath( b1 );
    happen_layer->createPath( e1 );
    happen_layer->createPath( b2 );
    happen_layer->createPath( e2 );

    topology->Load( L"*100,500 {100,200,300,400,500}\n"
        L"=100,500,700\n"
        );

    esrGuide->setTermWay( EsrKit(200,0), ParkWayKit(1,1), EsrGuide::Major );
    esrGuide->setTermWay( EsrKit(200,0), ParkWayKit(1,2), EsrGuide::Major );
    esrGuide->setTermWay( EsrKit(300,0), ParkWayKit(2,3), EsrGuide::Major );
    esrGuide->setTermWay( EsrKit(300,0), ParkWayKit(2,6), EsrGuide::Major );
    esrGuide->setTermWay( EsrKit(400,0), ParkWayKit(1,2), EsrGuide::Major );
    esrGuide->setSpan( EsrKit(100,200), Oddness::ODD, false );
    esrGuide->setSpan( EsrKit(200,300), Oddness::ODD, false );
    esrGuide->setSpan( EsrKit(300,400), Oddness::ODD, false );
    esrGuide->setSpan( EsrKit(400,500), Oddness::ODD, false );

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    {
        aeAttach att1( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, b1[0], e1[0], false );
        att1.Action();
    }
    {
        aeAttach att2( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, b2[0], e2[0], false );
        att2.Action();
    }
    CPPUNIT_ASSERT( happen_layer->path_count() == 2 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name",    "Departure Transition   Transition   Arrival " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",     "1p[00100] ?p1w1[00200] ?p2w3[00300] 4p[00400] " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "waynum",  "-         1            3            -" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "parknum", "-         1            2            -" ) );

    CPPUNIT_ASSERT( happen_layer->exist_series( "name",    "Transition Transition   Transition   Transition   Transition" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",     "1p[00100]  ?p1w1[00200] ?p2w3[00300] ?p1w2[00400] 5p[00500] " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "waynum",  "-          1            3            2            -" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "parknum", "-          1            2            1            -" ) );
}

void TC_Hem_aeAttach::ArrivalTermDeathSpan_Extension()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20171219T163059Z' name='Transition' Bdg='2p[09100]' waynum='2' parknum='1' index='' num='721' rapid='Y'/>"
        L"  <SpotEvent create_time='20171219T170650Z' name='Arrival' Bdg='AGP[09162]' waynum='1' parknum='1'>"
        L"    <rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20171219T170807Z' name='Departure' Bdg='AGP[09162]' waynum='1' parknum='1'>"
        L"    <rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20171219T171031Z' name='Transition' Bdg='1C[09180]' waynum='1' parknum='1' index='' num='V881' mvps='Y' />"
        L"  <SpotEvent create_time='20171219T171916Z' name='Death' Bdg='PPG2[09180:09280]' waynum='1'>"
        L"    <rwcoord picketing1_val='54~' picketing1_comm='Реньге' />"
        L"    <rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить - отсутствует
    // 
    // выполняем билль
    Hem::Bill bill = createBill( //Jelgava - Glūda ==> "Dobele"
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20171219T174612Z'>"
        L"<PrePoint name='Death' Bdg='PPG2[09180:09280]' create_time='20171219T171916Z' waynum='1'>"
        L"<rwcoord picketing1_val='51~1000' picketing1_comm='Реньге' />"
        L"</PrePoint>"
        L"<Action code='PushBack' District='09180-09860' DistrictRight='Y' FixBound='20171211T080800Z' />"
        L"<PostPoint name='Arrival' Bdg='?[09290]' create_time='20171219T173200Z' />"
        L"<EsrList>"
        L"<Station esr='09280' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    topology->Load( 
        L"//22.Olaine-Jelgava-Gluda \n"
        L"=09150,09180,09280 \n"
        L"//27.Jelgava-Gluda-Liepaja pas \n"
        L"=09180,09280,09860 \n"
        L"//Olaine-Jelgava \n"
        L"*09150,09180 {09150,09160,09162,09180} \n"
        L"//Gluda-Liepaja pas \n"
        L"*09280,09860 {09280,09290,09303,09320,09330,09340,09350,09351,09355,09860} \n"
        );

    // до
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition Arrival    Departure  Transition   Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "2p[09100]  AGP[09162] AGP[09162] 1C[09180] PPG2[09180:09280]  " ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
	att.Action();
    collectUndo();

    // после
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(7) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition Arrival    Departure  Transition Transition   Arrival  Death    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "2p[09100]  AGP[09162] AGP[09162] 1C[09180]  ?[09280]   ?[09290] ?[09290] " ) );
    // а теперь после отката
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Transition Arrival    Departure  Transition   Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "2p[09100]  AGP[09162] AGP[09162] 1C[09180] PPG2[09180:09280]  " ) );
}

void TC_Hem_aeAttach::ArrivalTermDeathArrivalOtherWay_Extension()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20180108T091000Z' name='Departure' Bdg='?p1w1[09330]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180108T092800Z' name='Arrival' Bdg='?[09320]' waynum='4' parknum='1' />"
        L"  <SpotEvent create_time='20180108T093300Z' name='Departure' Bdg='?[09320]' waynum='4' parknum='1' />"
        L"  <SpotEvent create_time='20180108T093300Z' name='Death' Bdg='?[09320]' waynum='4' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить - отсутствует
    // 
    // выполняем билль
    Hem::Bill bill = createBill( 
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180108T145959Z'>"
        L"<PrePoint name='Death' Bdg='?[09320]' create_time='20180108T093300Z' waynum='4' parknum='1' />"
        L"<Action code='PushBack' District='09180-09860' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Arrival' Bdg='?[09320]' create_time='20180108T093400Z' waynum='1' parknum='1' />"
        L"<EsrList>"
        L"<Station esr='09320' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );


    // до
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Arrival    Departure   Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?p1w1[09330]  ?[09320] ?[09320] ?[09320]  " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "waynum",  "1          4            4            4" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "parknum", "1          1            1            1" ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    // после
    CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Arrival    Departure Arrival  Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?p1w1[09330]  ?[09320] ?[09320] ?[09320]  ?[09320] " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "waynum",  "1          4            4            1     1") );
    CPPUNIT_ASSERT( happen_layer->exist_series( "parknum", "1          1            1            1     1") );


    // выполняем билль
    Hem::Bill bill2 = createBill( 
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180108T145959Z'>"
        L"<PrePoint name='Death' Bdg='?[09320]' create_time='20180108T093400Z' waynum='1' parknum='1' />"
        L"<Action code='PushBack' District='09180-09860' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Arrival' Bdg='?p1w1[09320]' create_time='20180108T094400Z' waynum='1' parknum='1' />"
        L"<EsrList>"
        L"<Station esr='09320' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Context context2(bill2.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att2( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context2, bill2 );
    att2.Action();
    collectUndo();
     CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
     CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
     CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Arrival    Departure Arrival  Death    " ) );
     CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?p1w1[09330]  ?[09320] ?[09320] ?[09320] ?p1w1[09320] " ) );
     CPPUNIT_ASSERT( happen_layer->exist_series( "waynum",  "1          4            4            1          1" ) );
     CPPUNIT_ASSERT( happen_layer->exist_series( "parknum", "1          1            1            1          1" ) );
    // а теперь после отката
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Arrival    Departure Arrival    Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?p1w1[09330]  ?[09320] ?[09320] ?[09320]  ?[09320]  " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "waynum",  "1          4            4       1       1" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "parknum", "1          1            1       1       1" ) );
}

void TC_Hem_aeAttach::MoveDeathAndFormDepartureSamePoint()
{
    // первая нить
     happen_layer->createPath( 
         L"<HemPath>"
         L"<SpotEvent create_time='20171220T083019Z' name='Span_move' Bdg='MD2C[09200:09202]' waynum='1'>"
         L"<rwcoord picketing1_val='252~725' picketing1_comm='Вентспилс' />"
         L"<rwcoord picketing1_val='253~450' picketing1_comm='Вентспилс' />"
         L"</SpotEvent>"
         L"<SpotEvent create_time='20171220T083131Z' name='Death' Bdg='MD2C[09200:09202]' waynum='1'>"
         L"<rwcoord picketing1_val='252~725' picketing1_comm='Вентспилс' />"
         L"<rwcoord picketing1_val='253~450' picketing1_comm='Вентспилс' />"
         L"</SpotEvent>"
         L"</HemPath>"
         );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"<SpotEvent create_time='20171220T083945Z' name='Form' Bdg='1C[09202]' waynum='1' parknum='1' index='1800-924-0983' num='J2417' length='57' weight='4666' net_weight='2944' fretran='Y' />"
        L"<SpotEvent create_time='20171220T083945Z' name='Departure' Bdg='1C[09202]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20171220T085407Z' name='Transition' Bdg='1C[09210]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
     Hem::Bill bill = createBill(
         L"<A2F_HINT Hint='cmd_edit' issue_moment='20171220T125643Z'>"
         L"<PrePoint name='Death' Bdg='MD2C[09200:09202]' create_time='20171220T083131Z' waynum='1'>"
         L"<rwcoord picketing1_val='253~87' picketing1_comm='Вентспилс' />"
         L"</PrePoint>"
         L"<Action code='Attach' District='09180-11420' DistrictRight='Y' />"
         L"<PostPoint name='Form' Bdg='1C[09202]' create_time='20171220T083945Z' index='1800-924-0983' num='J2417' length='57' weight='4666' net_weight='2944' fretran='Y' waynum='1' parknum='1'>"
         L"</PostPoint>"
         L"<EsrList>"
         L"<Station esr='09202' />"
         L"</EsrList>"
         L"</A2F_HINT>"
         );

 
     Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move          Arrival Departure Transition    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "MD2C[09200:09202] 1C[09202] 1C[09202] 1C[09210] " ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(2) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
}

void TC_Hem_aeAttach::PushFrontWithSaveNumber()
{
    // первая нить - не существует
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20180108T073000Z' name='Form' Bdg='?[09320]' waynum='6' parknum='1' index='0986-067-0900' num='3138' length='57' weight='1283' fretran='Y'>"
        L"    <feat_texts typeinfo='Д' />"
        L"    <Locomotive Series='2ТЭ10М' NumLoc='3450' Depo='0' Consec='1'>"
        L"      <Crew EngineDriver='MONAHOVS' Tim_Beg='2018-01-08 06:05' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180108T073700Z' name='Departure' Bdg='?[09320]' waynum='6' parknum='1'>"
        L"    <Pickup AddCars='2' DelCars='4' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180108T081100Z' name='Arrival' Bdg='?[09303]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20180108T081101Z' name='Death' Bdg='?[09303]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180108T145959Z'>"
        L"<PrePoint name='Arrival' Bdg='?[09355]' create_time='20180108T065300Z' />"
        L"<Action code='PushFront' District='09180-09860' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='?[09320]' create_time='20180108T073000Z' index='0986-067-0900' num='3138' length='57' weight='1283' fretran='Y' waynum='6' parknum='1'>"
        L"<feat_texts typeinfo='Д' />"
        L"<Locomotive Series='2ТЭ10М' NumLoc='3450' Depo='0' Consec='1'>"
        L"<Crew EngineDriver='MONAHOVS' Tim_Beg='2018-01-08 06:05' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09320' />"
        L"<Station esr='09330' />"
        L"<Station esr='09340' />"
        L"<Station esr='09350' />"
        L"<Station esr='09351' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure Arrival  Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09320] ?[09320] ?[09303] ?[09303]" ) );

    //Gluda-Liepaja pas
    topology->Load( 
        L"*09280,09860 {09280,09290,09303,09320,09330,09340,09350,09351,09355,09860} \n"
        );
    esrGuide->setTermWay( EsrKit(9355,0), ParkWayKit(1,1), EsrGuide::Major );
    esrGuide->setTermWay( EsrKit(9351,0), ParkWayKit(1,1), EsrGuide::Major );
    esrGuide->setTermWay( EsrKit(9350,0), ParkWayKit(1,1), EsrGuide::Major );
    esrGuide->setTermWay( EsrKit(9340,0), ParkWayKit(1,2), EsrGuide::Major );
    esrGuide->setTermWay( EsrKit(9330,0), ParkWayKit(1,1), EsrGuide::Major );
    esrGuide->setSpan( EsrKit(9320,9330), Oddness::ODD, false );
    esrGuide->setSpan( EsrKit(9330,9340), Oddness::ODD, false );
    esrGuide->setSpan( EsrKit(9340,9350), Oddness::ODD, false );
    esrGuide->setSpan( EsrKit(9350,9351), Oddness::ODD, false );
    esrGuide->setSpan( EsrKit(9351,9355), Oddness::ODD, false );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Arrival       Departure     Transition   Transition  Transition   Transition   Arrival Departure  Arrival Death    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?p1w1[09355] ?p1w1[09355] ?p1w1[09351] ?p1w1[09350] ?p1w2[09340] ?p1w1[09330] ?[09320] ?[09320] ?[09303] ?[09303]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "num",  "3138             -          -             -             -            -          -          -        -        -  " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "index",  "0986-067-0900  -          -             -             -            -          -          -        -        -  " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form Departure Arrival  Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09320] ?[09320] ?[09303] ?[09303]" ) );


};


void TC_Hem_aeAttach::PushFrontWithSaveNumberOnTheSamePlace()
{
    // первая нить - не существует
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20171201T043400Z' name='Departure' Bdg='?[09780]' waynum='1' parknum='1' index='' num='333' psng='Y' />"
        L"  <SpotEvent create_time='20171201T051400Z' name='Arrival' Bdg='?[09790]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20171201T051401Z' name='Death' Bdg='?[09790]' waynum='2' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20171201T082715Z'>"
        L"<PrePoint name='Arrival' Bdg='?[09780]' create_time='20171201T042200Z' waynum='1' parknum='1' />"
        L"<Action code='PushFront' District='09180-09820' DistrictRight='Y' FixBound='20171103T160000Z' SaveLog='Y' />"
        L"<PostPoint name='Departure' Bdg='?[09780]' create_time='20171201T043400Z' index='' num='333' psng='Y' waynum='1' parknum='1' />"
        L"<EsrList>"
        L"<Station esr='09780' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Arrival  Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09780] ?[09790] ?[09790]" ) );

    esrGuide->setTermWay( EsrKit(9780,0), ParkWayKit(1,1), EsrGuide::Major );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Arrival Departure Arrival  Death    " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09780] ?[09780] ?[09790] ?[09790]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "num",  "333   -      -          -  " ) );

    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Departure Arrival  Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09780] ?[09790] ?[09790]" ) );
};


void TC_Hem_aeAttach::AttachExistDeathAndFormOnTheSameStation()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20171004T020709Z' name='Form' Bdg='1C[09370]' waynum='1' parknum='1' index='1131-058-0900' num='J2405' length='56' weight='4836' net_weight='3274' fretran='Y'>"
        L"    <feat_texts typeinfo='Т' />"
        L"    <Locomotive Series='2М62У' NumLoc='97' Depo='0' Consec='1'>"
        L"      <Crew EngineDriver='MARKUSINS' Tim_Beg='2017-10-04 00:15' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20171004T020709Z' name='Departure' Bdg='1C[09370]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20171004T021801Z' name='Transition' Bdg='1C[09380]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20171004T024149Z' name='Transition' Bdg='1C[09400]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20171004T025951Z' name='Transition' Bdg='3C[09410]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20171004T031656Z' name='Arrival' Bdg='1C[09420]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20171004T031832Z' name='Death' Bdg='1C[09420]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20171004T031719Z' name='Form' Bdg='8SP+[09420]' index='1131-058-0900' num='2405' length='56' weight='4836' net_weight='3274' fretran='Y'>"
        L"    <feat_texts typeinfo='Т' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20171004T031739Z' name='Departure' Bdg='CDP[09420]'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20171004T033245Z' name='Arrival' Bdg='10AC[09000]' waynum='10' parknum='1' />"
        L"  <SpotEvent create_time='20171004T060200Z' name='Disform' Bdg='ASOUP 1042[09000]' waynum='10' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20171004T145753Z'>"
        L"<PrePoint name='Death' Bdg='1C[09420]' create_time='20171004T031832Z' waynum='1' parknum='1' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='8SP+[09420]' create_time='20171004T031719Z' index='1131-058-0900' num='2405' length='56' weight='4836' net_weight='3274' fretran='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09420' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );


    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    //Такие хвосты должна склеивать автосклейка при появлении нитей
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(9) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Transition Transition Transition Arrival  Departure   Arrival        Disform  " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "1C[09370] 1C[09370]  1C[09380]  1C[09400] 3C[09410] 1C[09420] CDP[09420] 10AC[09000]   ASOUP 1042[09000]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "num",  "J2405        -          -          -        -           -        -           -               -     " ) );

   CPPUNIT_ASSERT( implementUndo() );
   CPPUNIT_ASSERT( happen_layer->path_count()==2 );
   CPPUNIT_ASSERT( happen_layer->exist_path_size(7) );
   CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );

}

void TC_Hem_aeAttach::AttachSpanMoveWrongwayEvents()
{
        // первая нить
        happen_layer->createPath( 
            L"<HemPath>"
            L"  <SpotEvent create_time='20170623T013900Z' name='Span_move' Bdg='?[11443:11445]'>"
            L"    <rwcoord picketing1_val='381~43' picketing1_comm='Вентспилс' />"
            L"  </SpotEvent>"
            L"  <SpotEvent create_time='20170623T014915Z' name='Transition' Bdg='?[11443]' />"
            L"  <SpotEvent create_time='20170623T015930Z' name='Transition' Bdg='?[11442]' />"
            L"  <SpotEvent create_time='20170623T015930Z' name='Wrong_way' Bdg='?[11442:11446]' />"
            L"  <SpotEvent create_time='20170623T020203Z' name='Span_move' Bdg='edit[11442:11446]'>"
            L"    <rwcoord picketing1_val='367~625' picketing1_comm='Вентспилс' />"
            L"  </SpotEvent>"
            L"  <SpotEvent create_time='20170623T020203Z' name='Death' Bdg='edit[11442:11446]'>"
            L"    <rwcoord picketing1_val='367~625' picketing1_comm='Вентспилс' />"
            L"  </SpotEvent>"
            L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20170623T020712Z' name='Wrong_way' Bdg='?[11442:11446]' />"
        L"  <SpotEvent create_time='20170623T020712Z' name='Span_move' Bdg='edit[11442:11446]'>"
        L"    <rwcoord picketing1_val='361~675' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170623T020945Z' name='Transition' Bdg='?[11446]' />"
        L"  <SpotEvent create_time='20170623T022000Z' name='Span_move' Bdg='?[11432:11446]'>"
        L"    <rwcoord picketing1_val='356~672' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170623T022001Z' name='Death' Bdg='?[11432:11446]'>"
        L"    <rwcoord picketing1_val='356~672' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='edit[11442:11446]' create_time='20170623T020203Z'>"
        L"<rwcoord picketing1_val='367~625' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='Attach' SaveLog='Y' />"
        L"<PostPoint name='Span_move' Bdg='edit[11442:11446]' create_time='20170623T020712Z'>"
        L"<rwcoord picketing1_val='361~675' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );


    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size( 8) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Span_move     Transition Transition Wrong_way       Span_move     Transition   Span_move        Death " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[11443:11445] ?[11443]  ?[11442] ?[11442:11446] edit[11442:11446] ?[11446]  ?[11432:11446] ?[11432:11446]" ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(6) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );

}

void TC_Hem_aeAttach::AttachEntraStoppingToSpanMoveOnTheSamePlace()
{
    // первая нить
    happen_layer->createPath(  
        L"<HemPath>"
        L"  <SpotEvent create_time='20180620T054008Z' name='Form' Bdg='3C[09193]' waynum='3' parknum='1' index='0001-020-0919' num='J8604' motortroll='Y'>"
        L"    <Locomotive Series='К-32' NumLoc='162' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='TIMOFEJEVS' Tim_Beg='2018-06-20 06:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T054430Z' name='Departure' Bdg='3C[09193]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20180620T073933Z' name='Span_stopping_begin' Bdg='SS17C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='282~250' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T075304Z' name='Span_stopping_end' Bdg='SS17C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='282~250' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T075304Z' name='Span_move' Bdg='SS18C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='283~503' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T075505Z' name='Entra_stopping' Bdg='SS18C[09191:09193]' waynum='1' intervalSec='121'>"
        L"    <rwcoord picketing1_val='283~503' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T075734Z' name='Death' Bdg='SS18C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='283~503' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20180620T081118Z' name='Form' Bdg='SS18C[09191:09193]' waynum='1' index='0001-020-0919' num='J8604' motortroll='Y'>"
        L"    <rwcoord picketing1_val='283~503' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T081118Z' name='Span_stopping_begin' Bdg='SS17C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='282~250' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T083937Z' name='Span_stopping_end' Bdg='SS17C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='282~250' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T083937Z' name='Span_move' Bdg='SS18C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='283~503' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T084138Z' name='Entra_stopping' Bdg='SS18C[09191:09193]' waynum='1' intervalSec='121'>"
        L"    <rwcoord picketing1_val='283~503' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='282~877' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180620T093847Z' name='Arrival' Bdg='3C[09191]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20180620T093847Z' name='Disform' Bdg='3C[09191]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180620T111302Z'>"
        L"<PrePoint name='Death' Bdg='SS18C[09191:09193]' create_time='20180620T075734Z' waynum='1'>"
        L"<rwcoord picketing1_val='283~190' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='Attach' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint name='Form' Bdg='SS18C[09191:09193]' create_time='20180620T081118Z' index='0001-020-0919' num='J8604' motortroll='Y' waynum='1'>"
        L"<rwcoord picketing1_val='283~190' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_series("name", "Form      Departure Span_stopping_begin Span_stopping_end  Span_move          Entra_stopping     Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series("Bdg",  "3C[09193] 3C[09193] SS17C[09191:09193]  SS17C[09191:09193] SS18C[09191:09193] SS18C[09191:09193] SS18C[09191:09193]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series("name", "Form               Span_stopping_begin  Span_stopping_end  Span_move          Entra_stopping     Arrival   Disform" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series("Bdg",  "SS18C[09191:09193] SS17C[09191:09193]  SS17C[09191:09193] SS18C[09191:09193]  SS18C[09191:09193] 3C[09191] 3C[09191]" ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Departure Span_stopping_begin  Span_stopping_end  Span_move            Span_stopping_begin  Span_stopping_end  Span_move          Entra_stopping     Arrival   Disform" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "3C[09193] 3C[09193] SS17C[09191:09193]  SS17C[09191:09193]  SS18C[09191:09193]   SS17C[09191:09193]  SS17C[09191:09193] SS18C[09191:09193]  SS18C[09191:09193] 3C[09191] 3C[09191]" ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_series("name", "Form      Departure Span_stopping_begin Span_stopping_end  Span_move          Entra_stopping     Death" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series("Bdg",  "3C[09193] 3C[09193] SS17C[09191:09193]  SS17C[09191:09193] SS18C[09191:09193] SS18C[09191:09193] SS18C[09191:09193]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series("name", "Form               Span_stopping_begin  Span_stopping_end  Span_move          Entra_stopping     Arrival   Disform" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series("Bdg",  "SS18C[09191:09193] SS17C[09191:09193]  SS17C[09191:09193] SS18C[09191:09193]  SS18C[09191:09193] 3C[09191] 3C[09191]" ) );

}

void TC_Hem_aeAttach::Attach_dropDisform_4871()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180914T053941Z' name='Form' Bdg='6C[11111]' waynum='6' parknum='1' index='0001-037-0978' num='V8601' motortroll='Y'>"
        L"    <Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180914T054654Z' name='Departure' Bdg='6C[11111]' waynum='6' parknum='1' />"
        L"  <SpotEvent create_time='20180914T054707Z' name='Transition' Bdg='PVDP[11111]'>"
        L"    <rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180914T060359Z' name='Span_stopping_begin' Bdg='TZ4C[11111:22222]' waynum='1'>"
        L"    <rwcoord picketing1_val='101~437' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='102~274' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180914T083012Z' name='Span_stopping_end' Bdg='TZ4C[11111:22222]' waynum='1'>"
        L"    <rwcoord picketing1_val='101~437' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='102~274' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180914T083549Z' name='Span_move' Bdg='TZ11C[11111:22222]' waynum='1'>"
        L"    <rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180914T083645Z' name='Death' Bdg='TZ11C[11111:22222]' waynum='1'>"
        L"    <rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180914T083645Z' name='Form' Bdg='PVDP[11111]' index='0001-037-0978' num='V8602' motortroll='Y'>"
        L"    <rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"    <Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180914T083802Z' name='Arrival' Bdg='5C[11111]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20180914T083802Z' name='Disform' Bdg='5C[11111]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180914T084333Z'>"
        L"  <PrePoint name='Death' Bdg='TZ11C[11111:22222]' create_time='20180914T083645Z' waynum='1'>"
        L"    <rwcoord picketing1_val='106~983' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' District='09180-09820' DistrictRight='Y' SaveLog='Y' />"
        L"  <PostPoint name='Form' Bdg='PVDP[11111]' create_time='20180914T083645Z' index='0001-037-0978' num='V8602' motortroll='Y'>"
        L"    <Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"    </Locomotive>"
        L"  </PostPoint>"
        L"  <EsrList>"
        L"    <Station esr='11111' />"
        L"  </EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Departure Transition  Span_stopping_begin Span_stopping_end Span_move          Death " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "6C[11111] 6C[11111] PVDP[11111] TZ4C[11111:22222]   TZ4C[11111:22222] TZ11C[11111:22222] TZ11C[11111:22222] " ) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form        Arrival   Disform " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "PVDP[11111] 5C[11111] 5C[11111] " ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Departure Transition  Span_stopping_begin Span_stopping_end                                                   Arrival   Disform   " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "6C[11111] 6C[11111] PVDP[11111] TZ4C[11111:22222]   TZ4C[11111:22222]                                                   5C[11111] 5C[11111] " ) );
}

void TC_Hem_aeAttach::Attach_form_4875()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20180914T054701Z' name='Form' Bdg='6C[11111]' waynum='6' parknum='1' index='0001-037-0978' num='V8601' motortroll='Y'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20180914T054854Z' name='Departure' Bdg='6C[11111]' waynum='6' parknum='1' />" 
        L"<SpotEvent create_time='20180914T083548Z' name='Span_move' Bdg='TZ11C[11111:22222]' waynum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20180914T083645Z' name='Death' Bdg='TZ11C[11111:22222]' waynum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>" 
        L"</HemPath>");

    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20180914T083645Z' name='Form' Bdg='PVDP[22222]' index='0001-037-0978' num='V8602' motortroll='Y'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180914T084333Z'>"
        L"  <PrePoint name='Death' Bdg='TZ11C[11111:22222]' create_time='20180914T083645Z' waynum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Attach' District='09180-09820' DistrictRight='Y' SaveLog='Y' />"
        L"  <PostPoint name='Form' Bdg='PVDP[22222]' create_time='20180914T083645Z' index='0001-037-0978' num='V8602' motortroll='Y'>"
        L"    <Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"    </Locomotive>"
        L"  </PostPoint>"
        L"  <EsrList>"
        L"    <Station esr='22222' />"
        L"  </EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( hl.exist_path_size(1) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Departure    Span_move Death " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "6C[11111] 6C[11111] TZ11C[11111:22222] TZ11C[11111:22222]" ) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "PVDP[22222]" ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Departure      Span_move         Arrival " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "6C[11111] 6C[11111]  TZ11C[11111:22222]  PVDP[22222]" ) );
}

void TC_Hem_aeAttach::Attach_disform_nowayform_spanmove()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
    L"<HemPath>"
        L"<SpotEvent create_time='20180914T053941Z' name='Form' Bdg='6C[11111]' waynum='6' parknum='1' index='' num='V8601' motortroll='Y' />"
        L"<SpotEvent create_time='20180914T054654Z' name='Disform' Bdg='6C[11111]' waynum='6' parknum='1' />"
    L"</HemPath>"
    );

    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20180914T054701Z' name='Form' Bdg='PVDP[11111]' index='0001-037-0978' num='V8601' motortroll='Y'>"
        L"<Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
//         L"<SpotEvent create_time='20180914T054707Z' name='Station_exit' Bdg='PVDP[11111]'>"
//         L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
//         L"</SpotEvent>"
        L"<SpotEvent create_time='20180914T054707Z' name='Span_move' Bdg='TZ11C[11111:22222]' waynum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
    );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180914T084333Z'>"
        L"  <PrePoint name='Disform' Bdg='6C[11111]' create_time='20180914T054654Z' waynum='6' parknum='1' />"
        L"  <Action code='Attach' District='09180-09820' DistrictRight='Y' SaveLog='Y' />"
        L"  <PostPoint name='Form' Bdg='PVDP[11111]' create_time='20180914T054701Z' index='0001-037-0978' num='V8601' motortroll='Y'>"
        L"    <Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"    </Locomotive>"
        L"  </PostPoint>"
        L"  <EsrList>"
        L"    <Station esr='11111' />"
        L"  </EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size(2) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Disform " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "6C[11111] 6C[11111] " ) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form         Span_move" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "PVDP[11111] TZ11C[11111:22222]" ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Departure  Span_move" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "6C[11111] 6C[11111]  TZ11C[11111:22222]") );
}

void TC_Hem_aeAttach::DeathSpanstoppingBeginAndClosedPathOnOneStation()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20181206T062659Z' name='Form' Bdg='2aC[11111]' waynum='2' parknum='1' index='0001-310-0923' num='J8202' stone='Y'/>"
        L"<SpotEvent create_time='20181206T070252Z' name='Departure' Bdg='2bC[11111]' waynum='2' parknum='1' optCode='09230:09240' />"
        L"<SpotEvent create_time='20181206T071632Z' name='Span_stopping_begin' Bdg='IM1C[11111:22222]' waynum='1' intervalSec='770'>"
        L"<rwcoord picketing1_val='197~380' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='196~663' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20181206T075121Z' name='Death' Bdg='IM1C[11111:22222]' waynum='1'>"
        L"<rwcoord picketing1_val='197~380' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='196~663' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20181206T075121Z' name='Form' Bdg='NDP[11111]' index='0001-310-0923' num='J8202' stone='Y'>"
        L"<rwcoord picketing1_val='196~663' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='BDS-200' NumLoc='745' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='LUKJANOVS' Tim_Beg='2018-12-06 07:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20181206T075443Z' name='Station_entry' Bdg='NDP[11111]'>"
        L"<rwcoord picketing1_val='196~663' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20181206T080850Z' name='Disform' Bdg='NDP[11111]'>"
        L"<rwcoord picketing1_val='196~663' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20181206T080850Z'>"
        L"<PrePoint name='Death' Bdg='IM1C[11111:22222]' create_time='20181206T075121Z' waynum='1'>"
        L"<rwcoord picketing1_val='197~21' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='Attach' District='09180-11420' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='NDP[11111]' create_time='20181206T075121Z' index='0001-310-0923' num='J8202' stone='Y'>"
        L"<Locomotive Series='BDS-200' NumLoc='745' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='LUKJANOVS' Tim_Beg='2018-12-06 07:30' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09240' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Departure  Span_stopping_begin    Death" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "2aC[11111] 2bC[11111] IM1C[11111:22222]   IM1C[11111:22222]" ) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Station_entry     Disform " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "NDP[11111] NDP[11111]       NDP[11111]" ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Departure  Span_stopping_begin Span_stopping_end  Station_entry Disform" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "2aC[11111] 2bC[11111] IM1C[11111:22222]   IM1C[11111:22222] NDP[11111] NDP[11111]") );
}

void TC_Hem_aeAttach::Jelgava2Jelgava()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190108T053428Z' name='Form' Bdg='P8P[11432:11446]' waynum='1' index='1800-248-0983' num='J2717' length='57' weight='5301' net_weight='3946' through='Y'>"
        L"  <rwcoord picketing1_val='356~383' picketing1_comm='Вентспилс' />"
        L"  <rwcoord picketing1_val='356~683' picketing1_comm='Вентспилс' />"
        L"  <feat_texts typeinfo='Т' />"
        L"  <Locomotive Series='2ТЭ116' NumLoc='959' Consec='1' CarrierCode='2'>"
        L"    <Crew EngineDriver='MINCONOKS' Tim_Beg='2019-01-08 05:02' />"
        L"  </Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190108T054030Z' name='Transition' Bdg='1C[11432]' waynum='1' parknum='1' intervalSec='17' optCode='11431:11432' />"
        L"<SpotEvent create_time='20190108T091834Z' name='Arrival' Bdg='1C[09251]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190108T091900Z' name='Departure' Bdg='1C[09251]' waynum='1' parknum='1' optCode='09181:09251' />"
        L"<SpotEvent create_time='20190108T093911Z' name='Station_entry' Bdg='NGP[09181]'>"
        L"  <rwcoord picketing1_val='166~890' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190108T094244Z' name='Disform' Bdg='4AC[09181]' waynum='4' parknum='1' />"
        L"</HemPath>"
        );

    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190108T094500Z' name='Form' Bdg='4AC[09181]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20190108T094501Z' name='Departure' Bdg='4AC[09181]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20190108T094919Z' name='Arrival' Bdg='8C[09180]' waynum='8' parknum='1' />"
        L"</HemPath>"
       );

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );

    SpotEvent d( HCode::DISFORM, BadgeE(L"4AC",EsrKit(9181)), time_from_iso("20190108T094244Z") );
    SpotEvent f( HCode::FORM,    BadgeE(L"4AC",EsrKit(9181)), time_from_iso("20190108T094500Z") );


    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, d, f, false );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form             Transition Arrival   Departure Station_entry Departure  Arrival    ") );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "P8P[11432:11446] 1C[11432]  1C[09251] 1C[09251]  NGP[09181]  4AC[09181] 8C[09180] ") );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
}

void TC_Hem_aeAttach::PushFrontToPathWithLimit()
{
    auto& hl = *happen_layer;
    // первая нить - не существует
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190117T080649Z' name='Form' Bdg='SV23C[09764:09772]' waynum='1' index='' num='V2412' through='Y'>"
        L"<rwcoord picketing1_val='143~550' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='142~650' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190117T080820Z' name='Transition' Bdg='1C[09764]' waynum='1' parknum='1' intervalSec='3' optCode='09180:09764' index='' num='V2412' through='Y'>"
        L"<feat_texts limitspeed='66' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190117T082017Z' name='Span_move' Bdg='LJ14C[09180:09764]' waynum='1'>"
        L"<rwcoord picketing1_val='155~800' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='156~650' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Span_move' Bdg='?[09750:09772]' create_time='20190117T071200Z'>"
        L"<rwcoord picketing1_val='101~475' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='PushFront' District='09180-09820' DistrictRight='Y' />"
        L"<PostPoint name='Form' Bdg='SV23C[09764:09772]' create_time='20190117T080649Z' index='' num='V2412' through='Y' waynum='1'>"
        L"<rwcoord picketing1_val='143~100' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09772' />"
        L"<Station esr='09750' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Transition  Span_move" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "SV23C[09764:09772] 1C[09764] LJ14C[09180:09764]" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "num",  "V2412   V2412              -  " ) );

    //Jelgava-Tukums 2
    topology->Load(  L"*09180,09750 {09180,09764,09772,09750} \n" );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Span_move      Transition    Transition  Span_move" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "?[09750:09772] ?[09772] 1C[09764]  LJ14C[09180:09764]") );
    CPPUNIT_ASSERT( happen_layer->exist_series( "num",  "V2412   -          V2412      -  " ) );
}

void TC_Hem_aeAttach::AttachAsoupDepartureToFormSpan()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190122T131800Z' name='Departure' Bdg='ASOUP 1042[08620]'  index='0001-075-0900' num='4403' />"
        L"</HemPath>"
        );

    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190122T132327Z' name='Form' Bdg='LV1[08620:11081]' waynum='1' index='0001-075-0900' num='4403' reslocfast='Y'>"
        L"<rwcoord picketing1_val='165~' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='164~600' picketing1_comm='Земитани-Валга' />"
        L"<Locomotive Series='2М62УМ' NumLoc='10' Consec='1'>"
        L"<Crew EngineDriver='CHERPINSKIS' Tim_Beg='2019-01-22 09:44' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190122T132327Z' name='Span_move' Bdg='LV1[08620:11081]' waynum='1'>"
        L"<rwcoord picketing1_val='165~' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='164~600' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190122T132327Z' name='Station_entry' Bdg='NDP[11081]'>"
        L"<rwcoord picketing1_val='164~600' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190122T132502Z' name='Transition' Bdg='1C[11081]' waynum='1' parknum='1' intervalSec='64' optCode='11081:11082' />"
        L"<SpotEvent create_time='20190122T132529Z' name='Death' Bdg='PDP[11081]'>"
        L"<rwcoord picketing1_val='163~500' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190122T132534Z' name='Form' Bdg='SL13[11081:11082]' waynum='1'>"
        L"<rwcoord picketing1_val='163~500' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='162~500' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190122T132539Z' name='Span_move' Bdg='SL11[11081:11082]' waynum='1'>"
        L"<rwcoord picketing1_val='162~500' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='160~' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size(1) );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );

    SpotEvent d = createEvent<SpotEvent>(L"<SpotEvent create_time='20190122T131800Z' name='Departure' Bdg='ASOUP 1042[08620]'  index='0001-075-0900' num='4403' />");
    SpotEvent f = createEvent<SpotEvent>(L"<SpotEvent create_time='20190122T132327Z' name='Form' Bdg='LV1[08620:11081]' waynum='1' index='0001-075-0900' num='4403' reslocfast='Y'/>" );


    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, d, f, false );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Departure         Span_move       Station_entry Transition      Death      Form              Span_move ") );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[08620] LV1[08620:11081]  NDP[11081]   1C[11081]  PDP[11081] SL13[11081:11082] SL11[11081:11082] ") );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
}

void TC_Hem_aeAttach::AttachAsoupDepartureToPath()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190826T082437Z' name='Form' Bdg='2UP_PK[09042:09501]' waynum='2' index='0905-002-3100' num='3603' through='Y'>"
        L"<rwcoord picketing1_val='0~900' picketing1_comm='Рига-Кр' />"
        L"<rwcoord picketing1_val='1~300' picketing1_comm='Рига-Кр' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190826T082641Z' name='Transition' Bdg='7SP[09042]'>"
        L"<rwcoord picketing1_val='5~857' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190826T083159Z' name='Transition' Bdg='1C[09500]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190826T084508Z' name='Arrival' Bdg='5JC[09006]' waynum='5' parknum='1' index='0905-002-3100' num='2746' />"
        L"<SpotEvent create_time='20190826T105242Z' name='Departure' Bdg='5JC[09006]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20190826T105950Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' intervalSec='31' />"
        L"</HemPath>"
        );


    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20190826T105950Z'>"
        L"<PrePoint layer='asoup' Bdg='ASOUP 1042[09050]' name='Departure' create_time='20190826T075000Z' index='0905-002-3100' num='3603' length='56' weight='1265' transfer='Y' waynum='16' parknum='2'><![CDATA[(:1042 909/000+09050 3603 0905 002 3100 03 09000 26 08 10 50 02/16 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 01265 00000 224 53248878 59236620 000 000 00 000 056.90 000 056 000 92 000 056 000\n"
        L"Ю3 579 00001261 1 07 00 0000 00000 FEDOTOVSKIS \n"
        L"Ю3 579 00001262 9\n"
        L"Ю4 11290 2 17 000 002.90 000 002 92 000 002 \n"
        L"Ю4 11290 2 24 000 009.90 000 009 92 000 009 \n"
        L"Ю4 11290 2 76 000 045.90 000 045 92 000 045 \n"
        L"Ю12 00 53248878 1 000 76900 43403 4953 300 00 00 00 00 00 0227 12 92 5931 04 106 31000 00000 11290 76 09050 17 00000 0000 020 1 1250 09069- 128 000 00000000\n"
        L"Ю12 00 58320029 255255255255 \n"
        L"Ю12 00 59851618 255237127255 02405905100\n"
        L"Ю12 00 53366613 255237127255 02255931106\n"
        L"Ю12 00 90126194 255253255247 0901APEHДA\n"
        L"Ю12 00 59815175 255253127247 590409509069-\n"
        L"Ю12 00 59811109 255239255255 0229\n"
        L"Ю12 00 53246286 255237127255 02275931106\n"
        L"Ю12 00 53248597 255255255255 \n"
        L"Ю12 00 54944699 255255255255 \n"
        L"Ю12 00 59056986 255237127255 02355904095\n"
        L"Ю12 00 53369849 195237127255 7694043304284340002255931106\n"
        L"Ю12 00 53370318 255255255255 \n"
        L"Ю12 00 53359386 255255255255 \n"
        L"Ю12 00 53821112 255237127255 02265904095\n"
        L"Ю12 00 59255513 255239255255 0223\n"
        L"Ю12 00 53820759 255239255255 0226\n"
        L"Ю12 00 58898115 255239255255 0225\n"
        L"Ю12 00 53293650 255237127255 02265931106\n"
        L"Ю12 00 59256115 255237127255 02235904095\n"
        L"Ю12 00 90135393 251237127247 00002310901106APEHДA\n"
        L"Ю12 00 59262386 251237127247 4000223590409509069-\n"
        L"Ю12 00 58888397 255239255255 0229\n"
        L"Ю12 00 53333670 255237127255 02255931106\n"
        L"Ю12 00 53331963 255255255255 \n"
        L"Ю12 00 59041707 195253119255 22640436195291560590409517\n"
        L"Ю12 00 59041368 255239255255 0228\n"
        L"Ю12 00 59041384 195255247255 7694043307284336076\n"
        L"Ю12 00 90053596 255237127247 02270901106APEHДA\n"
        L"Ю12 00 58653890 255237127247 0224590409509069-\n"
        L"Ю12 00 53821682 255239255255 0227\n"
        L"Ю12 00 59054205 195239247255 27190433071666060022424\n"
        L"Ю12 00 90009994 199237119247 769404330728430234090110676APEHДA\n"
        L"Ю12 00 59047076 251237127247 3600224590409509069-\n"
        L"Ю12 00 59038406 255255255255 \n"
        L"Ю12 00 58898230 255239255255 0219\n"
        L"Ю12 00 53357919 255237127255 02255931106\n"
        L"Ю12 00 59043968 255253127255 5904095\n"
        L"Ю12 00 59233601 255255255255 \n"
        L"Ю12 00 58652371 255239255255 0220\n"
        L"Ю12 00 59251033 255239255255 0223\n"
        L"Ю12 00 53363404 255237127255 02255931106\n"
        L"Ю12 00 90232638 195237247247 273604330769874600228090124APEHДA\n"
        L"Ю12 00 59233288 255237127247 0225590409509069-\n"
        L"Ю12 00 90232406 255237127247 02280901106APEHДA\n"
        L"Ю12 00 59030817 255237127247 0226590409509069-\n"
        L"Ю12 00 58892969 255239255255 0225\n"
        L"Ю12 00 53821245 255239255255 0227\n"
        L"Ю12 00 59038125 255239255255 0224\n"
        L"Ю12 00 53380184 255237127255 02285931106\n"
        L"Ю12 00 59041442 195253119255 76940433072843360590409576\n"
        L"Ю12 00 59036319 255239255255 0217\n"
        L"Ю12 00 59037952 255239255255 0225\n"
        L"Ю12 00 59047357 255239255255 0220\n"
        L"Ю12 00 53821450 255239255255 0227\n"
        L"Ю12 00 59236620 255239255255 0222:)]]></PrePoint>"
        L"<Action code='Attach' District='09000-09640' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='2UP_PK[09042:09501]' create_time='20190826T082437Z' index='0905-002-3100' num='3603' transfer='Y' waynum='2'>"
        L"<rwcoord picketing1_val='1~100' picketing1_comm='Рига-Кр' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09050' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    topology->Load(
        L"*09050,09008 {09050,09501,09042,09500,09006,09008}\n");

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );

//     CPPUNIT_ASSERT( implementUndo() );
//     CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(7) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "num",  "3603             -          -             -             2746            -          -   " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "index",  "0905-002-3100  -          -             -         0905-002-3100       -          -   " ) );
}

void TC_Hem_aeAttach::AsoupToAsoupDifferentCategories()
{
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20190828T040340Z'>"
        L"<PrePoint layer='asoup' Bdg='ASOUP 1042[11310]' name='Arrival' create_time='20190828T031700Z' index='0000-028-0001' num='0001' length='25' weight='813' fastyear='Y'><![CDATA[(:1042 909/000+11310 0001 0000 028 0001 01 11290 28 08 06 17 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 025 00813 00000 056 00920116 00905539 000 000 14 000 000\n"
        L"Ю3 506 00002670 1 20 10 0956 18233 ЛЯЛИH       \n"
        L"Ю12 00 00920116 1 001 09010 00000 0000 000 00 00 00 00 00 0560 70 01 0010 04 175 00000 00000 00000 01 00000 00 00000 0000 025 0 0000 0      196 000 00000000\n"
        L"Ю12 00 00920264 255255255255 \n"
        L"Ю12 00 00920058 255255255255 \n"
        L"Ю12 00 00910174 255255255251 132\n"
        L"Ю12 00 00963025 255237255243 06900040ПP    196\n"
        L"Ю12 00 00911248 255237255243 056000100     132\n"
        L"Ю12 00 00916049 255255255255 \n"
        L"Ю12 00 00905653 255239255255 0600\n"
        L"Ю12 00 00905661 255255255255 \n"
        L"Ю12 00 00905679 255255255251 196\n"
        L"Ю12 00 00920256 255239255255 0560\n"
        L"Ю12 00 00920165 255255255255 \n"
        L"Ю12 00 00916098 255255255255 \n"
        L"Ю12 00 00905539 255239255251 0600132:)]]>"
        L"<Locomotive Series='ТЭП70' NumLoc='267' Depo='956' Consec='1' CarrierCode='27'>"
        L"<Crew EngineDriver='ЛЯЛИH' TabNum='18233' Tim_Beg='2019-08-27 20:10' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<Action code='Attach' District='11420-11310-11260-11290' DistrictRight='Y' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP 1042[11310]' name='Departure' create_time='20190828T035400Z' index='1131-054-0900' num='2415' length='57' weight='5201' net_weight='3939' through='Y' waynum='11' parknum='1'><![CDATA[(:1042 909/000+11310 2415 1131 054 0900 03 11420 28 08 06 54 03/11 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 05201 03939 220 53378410 95453049 000 000 00 055 000.90 055 000 000 92 030 000 000 95 025 000 000\n"
        L"Ю3 530 00005841 1 05 37 0000 00000 TIVANJUK    \n"
        L"Ю3 530 00005842 9\n"
        L"Ю4 00000 1 01 055 000.90 055 000 92 030 000 95 025 000 \n"
        L"Ю4 09060 1 01 030 000.90 030 000 92 030 000 \n"
        L"Ю4 09590 1 01 025 000.90 025 000 95 025 000 \n"
        L"Ю12 00 53378410 1 070 09060 43307 7846 460 02 00 00 00 00 0228 30 92 5931 04 106 09000 09050 00000 01 09050 17 00000 0000 020 0 0000 OXP    132 000 00000000\n"
        L"Ю12 00 53820932 191237127255 06802265904095\n"
        L"Ю12 00 90231978 191237127255 07002280901106\n"
        L"Ю12 00 90016494 191239255255 0750235\n"
        L"Ю12 00 58899345 191237127255 06902285904095\n"
        L"Ю12 00 59263848 255239255255 0223\n"
        L"Ю12 00 53822508 255239255255 0227\n"
        L"Ю12 00 58653296 255239255255 0224\n"
        L"Ю12 00 59038679 255239255255 0221\n"
        L"Ю12 00 59038737 255239255255 0225\n"
        L"Ю12 00 90020199 191237127255 07502340901106\n"
        L"Ю12 00 90231002 191239255255 0700228\n"
        L"Ю12 00 53280293 191237255255 06902295931\n"
        L"Ю12 00 90212630 185237255255 0760600402350901\n"
        L"Ю12 00 90221581 187239255255 0755600234\n"
        L"Ю12 00 58652801 189237127255 0690202245904095\n"
        L"Ю12 00 59049676 189239255255 063010223\n"
        L"Ю12 00 53361549 189237127255 0700402255931106\n"
        L"Ю12 00 58898669 189253127255 069025904095\n"
        L"Ю12 00 90250101 189237127255 0750502380901106\n"
        L"Ю12 00 59266536 189237127255 0690202245904095\n"
        L"Ю12 00 59264846 255239255255 0223\n"
        L"Ю12 00 58788415 253239255255 010232\n"
        L"Ю12 00 58651662 253239255255 020224\n"
        L"Ю12 00 58787755 189239255255 068010231\n"
        L"Ю12 00 58653643 189239255255 069020225\n"
        L"Ю12 00 58652983 191255255255 068\n"
        L"Ю12 00 53361556 189253127255 070045931106\n"
        L"Ю12 00 58892340 189253127255 069025904095\n"
        L"Ю12 00 58898180 255239255255 0218\n"
        L"Ю12 00 95766747 131233089247 075095900110015023000232950950106096300900034ЗEPHO \n"
        L"Ю12 00 95764858 255255255255 \n"
        L"Ю12 00 95725719 255239255255 0230\n"
        L"Ю12 00 95728333 255255255255 \n"
        L"Ю12 00 95629176 255239255255 0232\n"
        L"Ю12 00 95878047 255239255255 0238\n"
        L"Ю12 00 95724902 255239255255 0230\n"
        L"Ю12 00 95725347 255255255255 \n"
        L"Ю12 00 95432233 251239255255 5000232\n"
        L"Ю12 00 95766978 255255255255 \n"
        L"Ю12 00 95720579 255239255255 0230\n"
        L"Ю12 00 95718656 255239255255 0229\n"
        L"Ю12 00 95721106 255239255255 0230\n"
        L"Ю12 00 95723417 255255255255 \n"
        L"Ю12 00 95723250 255255255255 \n"
        L"Ю12 00 95766903 255239255255 0232\n"
        L"Ю12 00 95580635 187239255191 0703000235027\n"
        L"Ю12 00 95274601 255255255255 \n"
        L"Ю12 00 95510723 255255255255 \n"
        L"Ю12 00 95725438 187239255183 07500002300200     \n"
        L"Ю12 00 95744926 255239255247 0231ПШEHИЦ\n"
        L"Ю12 00 95072526 189239127247 070030237104Э\n"     
        L"Ю12 00 95410619 185239127255 074300020231106\n"
        L"Ю12 00 95387049 191239255255 0690235\n"
        L"Ю12 00 95453049 253255255255 04:)]]>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2ТЭ116' NumLoc='584' Consec='1' CarrierCode='3'>"
        L"<Crew EngineDriver='TIVANJUK' Tim_Beg='2019-08-28 05:37' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='11310' />"
        L"</EsrList>"
        L"</A2F_HINT>"
);

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
}


void TC_Hem_aeAttach::Attach_DeathWithStationExit_5124()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190123T030256Z' name='Form' Bdg='3p[09160]' waynum='3' parknum='1' index='0918-043-0900' num='3242' length='56' weight='1896' net_weight='622' divisional='Y'>"
        L"<feat_texts typeinfo='Д' />"
        L"<Locomotive Series='2ТЭ10М' NumLoc='3449' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='HOTULEVS' Tim_Beg='2019-01-22 23:20' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190123T031208Z' name='Departure' Bdg='3p[09160]' waynum='3' parknum='1' intervalSec='9' optCode='09150:09160' />"
        L"<SpotEvent create_time='20190123T032631Z' name='Arrival' Bdg='5Ap[09150]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20190123T033106Z' name='Death' Bdg='11SP[09150]' />"
        L"<SpotEvent create_time='20190123T033136Z' name='Form' Bdg='NBP:1/3+[09150]' />"
        L"<SpotEvent create_time='20190123T033208Z' name='Death' Bdg='NBP:1/3+[09150]' />"
        L"</HemPath>"
        );

    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190123T033208Z' name='Station_exit' Bdg='NBP:1/3+[09150]' />"
        L"<SpotEvent create_time='20190123T034642Z' name='Arrival' Bdg='NBP[09104]' />"
        L"<SpotEvent create_time='20190123T035255Z' name='Disform' Bdg='NBP[09104]' />"
        L"</HemPath>"
        );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20190123T054455Z'>"
        L"<PrePoint name='Death' Bdg='NBP:1/3+[09150]' create_time='20190123T033208Z' />"
        L"<Action code='Attach' District='09010-09180' DistrictRight='Y' />"
        L"<PostPoint name='Station_exit' Bdg='NBP:1/3+[09150]' create_time='20190123T033208Z' />"
        L"<EsrList>"
        L"<Station esr='09150' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    topology->Load( 
        L"//Tornakalns-Olaine\n"
        L"*09100,09150 {09100,09104,09150}\n"
        );

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    SpotEventPtr event = make_shared<SpotEvent>(HCode::FORM, BadgeE(L"3p", EsrKit(9160)), time_from_iso("20190123T030256Z"));
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();

    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    CPPUNIT_ASSERT( hl.exist_series("name", "  Form    Departure   Arrival    Departure          Death" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "3p[09160] 3p[09160]  5Ap[09150]  5Ap[09150]  NBP:1/3+[09150]" ) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Station_exit    Arrival    Disform" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "NBP:1/3+[09150] NBP[09104] NBP[09104]" ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form      Departure  Arrival    Departure   Station_exit      Arrival    Disform" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "3p[09160] 3p[09160]  5Ap[09150] 5Ap[09150]  NBP:1/3+[09150] NBP[09104] NBP[09104]" ) );
}

void TC_Hem_aeAttach::Attach_DeathWithAsopupDisform_4807()
{
    asoup_layer->createPath(
        L"<AsoupEvent create_time='20180824T055500Z' name='Disform' Bdg='ASOUP 1042[09290]' index='0918-028-0929' num='3519' length='14' weight='875' net_weight='584' outbnd='Y' waynum='4' parknum='1'><![CDATA[(:1042 909/000+09290 3519 0918 028 0929 05 00000 24 08 08 55 01/04 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 014 00875 00584 052 95697967 95033478 000 000 00 013 000.90 013 000 000 95 013 000 000\n"
        L"Ю4 00000 1 01 013 000.90 013 000 95 013 000 \n"
        L"Ю4 09290 1 01 013 000.90 013 000 95 013 000 \n"
        L"Ю12 00 95697967 1 043 09290 01300 5900 300 04 00 00 00 00 0236 30 95 0950 04 106 09290 00000 00000 01 09180 34 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95699112 255239255255 0237\n"
        L"Ю12 00 95045928 191239255255 0350220\n"
        L"Ю12 00 95845665 191239255255 0430237\n"
        L"Ю12 00 95068086 191239255255 0390220\n"
        L"Ю12 00 95486957 187255255255 044000\n"
        L"Ю12 00 95048880 171255255255 05701100400\n"
        L"Ю12 00 95044863 255255255255 \n"
        L"Ю12 00 95574059 171255255255 03901300500\n"
        L"Ю12 00 95063715 255239255255 0217\n"
        L"Ю12 00 95039368 171239255255 058011000000220\n"
        L"Ю12 00 95045324 171255255255 03901300500\n"
        L"Ю12 00 95033478 171255255255 04801400000:)]]>"
        L"</AsoupEvent>"
        );

    {
    AsoupLayer::ReadAccessor container = asoup_layer->GetReadAccess();
    auto pathList = container->UT_GetEvents();
    CPPUNIT_ASSERT( pathList.size() == 1 );
    auto asoup = *pathList.cbegin();
    CPPUNIT_ASSERT( !asoup_layer->IsServed( asoup ) );
    }

    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20180824T032700Z' name='ExplicitForm' Bdg='ASOUP 1042[09180]' index='0918-028-0929' num='3519' length='14' weight='875' net_weight='584' outbnd='Y'>"
        L"<Locomotive Series='M62' NumLoc='1235' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='FOMENKO' Tim_Beg='2018-08-24 06:36' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20180824T050318Z' name='Departure' Bdg='PpGP[09180]'>"
        L"<rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20180824T051641Z' name='Transition' Bdg='?p1w1[09280]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20180824T054100Z' name='Arrival' Bdg='ASOUP 1042[09290]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20180824T054101Z' name='Death' Bdg='ASOUP 1042[09290]' waynum='4' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_series("name", "  ExplicitForm     Departure   Transition     Arrival      Death" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[09180] PpGP[09180]  ?p1w1[09280] ASOUP 1042[09290]  ASOUP 1042[09290]" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180824T054101Z'>"
        L"<PrePoint name='Death' Bdg='ASOUP 1042[09290]' create_time='20180824T054101Z' waynum='4' parknum='1' />"
        L"<Action code='Attach' District='09180-09860' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP 1042[09290]' name='Disform' create_time='20180824T055500Z' index='0918-028-0929' num='3519' length='14' weight='875' net_weight='584' outbnd='Y' waynum='4' parknum='1'><![CDATA[(:1042 909/000+09290 3519 0918 028 0929 05 00000 24 08 08 55 01/04 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 014 00875 00584 052 95697967 95033478 000 000 00 013 000.90 013 000 000 95 013 000 000\n"
        L"Ю4 00000 1 01 013 000.90 013 000 95 013 000 \n"
        L"Ю4 09290 1 01 013 000.90 013 000 95 013 000 \n"
        L"Ю12 00 95697967 1 043 09290 01300 5900 300 04 00 00 00 00 0236 30 95 0950 04 106 09290 00000 00000 01 09180 34 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95699112 255239255255 0237\n"
        L"Ю12 00 95045928 191239255255 0350220\n"
        L"Ю12 00 95845665 191239255255 0430237\n"
        L"Ю12 00 95068086 191239255255 0390220\n"
        L"Ю12 00 95486957 187255255255 044000\n"
        L"Ю12 00 95048880 171255255255 05701100400\n"
        L"Ю12 00 95044863 255255255255 \n"
        L"Ю12 00 95574059 171255255255 03901300500\n"
        L"Ю12 00 95063715 255239255255 0217\n"
        L"Ю12 00 95039368 171239255255 058011000000220\n"
        L"Ю12 00 95045324 171255255255 03901300500\n"
        L"Ю12 00 95033478 171255255255 04801400000:)]]></PostPoint>"
        L"<EsrList>"
        L"<Station esr='09290' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	AsoupQueueMgr asoupQMgr;
    aeAttach_HappenToAsoup att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, asoupQMgr, context, bill.getHeadSpotThrows(), bill.getTailAsoupThrows(), false );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "ExplicitForm     Departure   Transition        Arrival          Disform" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[09180] PpGP[09180]  ?p1w1[09280] ASOUP 1042[09290]  ASOUP 1042[09290]" ) );
    AsoupLayer::ReadAccessor container = asoup_layer->GetReadAccess();
    auto pathList = container->UT_GetEvents();
    CPPUNIT_ASSERT( pathList.size() == 1 );
    auto asoup = *pathList.cbegin();
    CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
}

void TC_Hem_aeAttach::ArDep_ArDep_MatchWay()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='12w[11111:22222]' create_time='20151224T085854Z' waynum='1'>"
        L"    <rwcoord picketing1_val='423~500' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='422~500' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Arrival' Bdg='2w[22222]' create_time='20151224T090351Z' waynum='2' parknum='1' index='' num='D2819' fretran='Y' />"
        L"  <SpotEvent name='Departure' Bdg='2w[22222]' create_time='20151224T091125Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Death' Bdg='2w[22222]' create_time='20151224T091233Z' waynum='2' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='2w[22222]' create_time='20151224T091324Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Arrival' Bdg='2w[22222]' create_time='20151224T091324Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='2w[22222]' create_time='20151224T091524Z' waynum='2' parknum='1'/>"
        L"  <SpotEvent name='Arrival' Bdg='3w[33333]' create_time='20151224T092851Z' waynum='1' parknum='1' index='' num='D2819' fretran='Y' />"
        L"  <SpotEvent name='Departure' Bdg='3w[33333]' create_time='20151224T093432Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='4w[44444]' create_time='20151224T094542Z' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Departure' Bdg='2w[22222]' create_time='20151224T091125Z' index='' num='D2819' fretran='Y' waynum='2' parknum='1' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='2w[22222]' create_time='20151224T091324Z' waynum='2' parknum='1' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form             Arrival   Departure  Arrival   Departure Arrival   Departure Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "12w[11111:22222] 2w[22222] 2w[22222]  2w[22222] 2w[22222] 3w[33333] 3w[33333] 4w[44444] " ) );
}

void TC_Hem_aeAttach::DisformToForm()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='2w[22222]' create_time='20151224T090351Z' waynum='2' parknum='1' index='' num='D2819' fretran='Y' />"
        L"  <SpotEvent name='Disform' Bdg='2w[22222]' create_time='20151224T091125Z' waynum='2' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='2w[22222]' create_time='20151224T091324Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='2w[22222]' create_time='20151224T091524Z' waynum='2' parknum='1'/>"
        L"  <SpotEvent name='Arrival' Bdg='3w[33333]' create_time='20151224T092851Z' waynum='1' parknum='1' index='' num='D2819' fretran='Y' />"
        L"  <SpotEvent name='Departure' Bdg='3w[33333]' create_time='20151224T093432Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='4w[44444]' create_time='20151224T094542Z' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Disform' Bdg='2w[22222]' create_time='20151224T091125Z' index='' num='D2819' fretran='Y' waynum='2' parknum='1' />"
        L"<Action code='Attach' />"
        L"<PostPoint name='Form' Bdg='2w[22222]' create_time='20151224T091324Z' waynum='2' parknum='1' />"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "   Form   Departure   Arrival   Departure Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "2w[22222] 2w[22222]  3w[33333] 3w[33333]   4w[44444] " ) );
}

void TC_Hem_aeAttach::AttachWithFormOnStation_5030()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20181127T112700Z' name='Departure' Bdg='ASOUP 1042[09820]' waynum='14' parknum='8' index='0982-016-1800' num='V2238' length='57' weight='1360' through='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='1494' Consec='1' CarrierCode='2'>"
        L"<Crew EngineDriver='LAZDINIEKS' Tim_Beg='2018-11-27 13:09' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20181127T123621Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' intervalSec='17' optCode='09812:09813' />"
        L"<SpotEvent create_time='20181127T124418Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' intervalSec='3' optCode='09810:09812' />"
        L"<SpotEvent create_time='20181127T125006Z' name='Span_move' Bdg='UE7C[09810:09812]' waynum='1'>"
        L"<rwcoord picketing1_val='18~340' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='19~239' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20181127T125040Z' name='Death' Bdg='UE7C[09810:09812]' waynum='1'>"
        L"<rwcoord picketing1_val='18~340' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='19~239' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20181127T130239Z' name='Form' Bdg='1C[09810]' waynum='1' parknum='1' index='' num='V2238' through='Y' />"
        L"<SpotEvent create_time='20181127T131154Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' intervalSec='2' optCode='09802:09803' />"
        L"<SpotEvent create_time='20181127T132235Z' name='Arrival' Bdg='3C[09802]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='UE7C[09810:09812]' create_time='20181127T125040Z' waynum='1'>"                                       
        L"<rwcoord picketing1_val='18~789' picketing1_comm='Вентспилс' />"                                                               
        L"</PrePoint>"                                                                                                                     
        L"<Action code='Attach' District='09180-09820' DistrictRight='Y' />"                                                               
        L"<PostPoint name='Form' Bdg='1C[09810]' create_time='20181127T130239Z' index='' num='V2238' through='Y' waynum='1' parknum='1' />"
        L"<EsrList>"                                                                                                                       
        L"<Station esr='09810' />"                                                                                                       
        L"</EsrList>"                                                                                                                      
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Departure         Transition  Transition      Span_move     Transition Transition Arrival" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[09820]  1C[09813]   1C[09812]  UE7C[09810:09812] 1C[09810]   1C[09803] 3C[09802]" ) );
}

void TC_Hem_aeAttach::CheckError()
{
    asoup_layer->createPath(
        L"<AsoupEvent create_time='20191111T043000Z' name='ExplicitForm' Bdg='ASOUP 1042[09820]' index='0982-027-1800' num='2442' length='57' weight='1392' through='Y' dirTo='09750' adjTo='09813'><![CDATA[(:1042 909/000+09820 2442 0982 027 1800 07 09750 11 11 06 30 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01392 00000 228 62712617 64313885 000 000 00 000 057.60 000 057 000\n"
        L"Ю4 11290 2 83 000 057.60 000 057 \n"
        L"Ю12 00 62712617 1 000 85270 16118 2738 300 00 00 00 00 00 0249 12 60 0600 04 100 18000 00000 11290 83 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        L"Ю12 00 62817507 255255255255 \n"
        L"Ю12 00 62822085 255239255255 0248\n"
        L"Ю12 00 54017116 203237255255 864701611740002405600\n"
        L"Ю12 00 54770508 255239255255 0236\n"
        L"Ю12 00 60517828 219237255255 8527050002460600\n"
        L"Ю12 00 62952478 255239255255 0249\n"
        L"Ю12 00 64039274 195239255255 861401611780230000238\n"
        L"Ю12 00 63854251 195239255255 862201611758603000245\n"
        L"Ю12 00 62639539 255255255255 \n"
        L"Ю12 00 63450647 255255255255 \n"
        L"Ю12 00 62981717 255255255255 \n"
        L"Ю12 00 62981519 255255255255 \n"
        L"Ю12 00 62842679 251255255255 400\n"
        L"Ю12 00 55183958 255253255255 5600\n"
        L"Ю12 00 61955415 219237255255 8621000002390600\n"
        L"Ю12 00 55184022 223237255255 8622002455600\n"
        L"Ю12 00 62305842 223237255255 8621002390600\n"
        L"Ю12 00 56147176 199237255255 8647016117273802345600\n"
        L"Ю12 00 63910236 195237255255 8622016117586050002450600\n"
        L"Ю12 00 60831682 255255255255 \n"
        L"Ю12 00 61484531 255255255255 \n"
        L"Ю12 00 61989620 195239255255 852701611827380000244\n"
        L"Ю12 00 61957817 235239255255 161173000247\n"
        L"Ю12 00 62710330 255239255255 0249\n"
        L"Ю12 00 62795133 255239255255 0247\n"
        L"Ю12 00 63172084 255239255255 0248\n"
        L"Ю12 00 62809629 255239255255 0250\n"
        L"Ю12 00 63636724 195239255255 862201611758600000245\n"
        L"Ю12 00 62296777 199239255255 852701611827380243\n"
        L"Ю12 00 63636914 199239255255 862201611758600245\n"
        L"Ю12 00 62813571 195239255255 852701611827384000248\n"
        L"Ю12 00 62954839 255239255255 0249\n"
        L"Ю12 00 64319031 195239255255 862201611758605000245\n"
        L"Ю12 00 64316425 255255255255 \n"
        L"Ю12 00 64319080 255255255255 \n"
        L"Ю12 00 60481116 195239255255 861401611780233000240\n"
        L"Ю12 00 64121148 255239255255 0238\n"
        L"Ю12 00 61756680 255239255255 0239\n"
        L"Ю12 00 61765160 255239255255 0240\n"
        L"Ю12 00 64061583 255239255255 0239\n"
        L"Ю12 00 60593787 251239255255 4000235\n"
        L"Ю12 00 64085616 255239255255 0237\n"
        L"Ю12 00 64084585 255239255255 0239\n"
        L"Ю12 00 61140638 195239255255 862101611758600000238\n"
        L"Ю12 00 63810659 199255255255 86140161178023\n"
        L"Ю12 00 62824826 195239255255 852701611727385000250\n"
        L"Ю12 00 64313448 255239255255 0247\n"
        L"Ю12 00 64313455 255255255255 \n"
        L"Ю12 00 64313463 255239255255 0248\n"
        L"Ю12 00 61934444 255239255255 0246\n"
        L"Ю12 00 62797048 235239255255 161183000247\n"
        L"Ю12 00 62772629 255239255255 0249\n"
        L"Ю12 00 63230320 255239255255 0248\n"
        L"Ю12 00 64313505 255239255255 0247\n"
        L"Ю12 00 64313513 255255255255 \n"
        L"Ю12 00 64313885 251255255255 000:)]]>"
        L"</AsoupEvent>"
        );

    asoup_layer->createPath(
        L"<AsoupEvent create_time='20191111T043000Z' name='Info_changing' Bdg='ASOUP 904[09820]' index='0982-027-1800' num='2442' length='57' weight='1392' through='Y'><![CDATA[(:904 0982 2442 0982 27 1800 1 11 11 06 30 057 01392 0 0000 0 0\n"
        L"01 62712617 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"02 62817507 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"03 62822085 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"04 54017116 0202 000 86475  16117 2738 4 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"05 54770508 0202 000 86475  16117 2738 4 0 0 0 00/00 11020 000 09826  0236 0100 0700\n"
        L"06 60517828 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0246 0100 0750\n"
        L"07 62952478 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"08 64039274 0202 000 86146  16117 8023 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"09 63854251 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"10 62639539 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"11 63450647 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"12 62981717 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"13 62981519 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"14 62842679 0202 000 86221  16117 5860 4 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"15 55183958 0202 000 86221  16117 5860 4 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"16 61955415 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"17 55184022 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"18 62305842 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"19 56147176 0202 000 86475  16117 2738 0 0 0 0 00/00 11020 000 09826  0234 0100 0700\n"
        L"20 63910236 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"21 60831682 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"22 61484531 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"23 61989620 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0244 0100 0750\n"
        L"24 61957817 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"25 62710330 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"26 62795133 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"27 63172084 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"28 62809629 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0250 0100 0750\n"
        L"29 63636724 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"30 62296777 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0243 0100 0750\n"
        L"31 63636914 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"32 62813571 0202 000 85278  16118 2738 4 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"33 62954839 0202 000 85278  16118 2738 4 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"34 64319031 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"35 64316425 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"36 64319080 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"37 60481116 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"38 64121148 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"39 61756680 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"40 61765160 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"41 64061583 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"42 60593787 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0235 0100 0700\n"
        L"43 64085616 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0237 0100 0695\n"
        L"44 64084585 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"45 61140638 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"46 63810659 0202 000 86146  16117 8023 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"47 62824826 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0250 0100 0750\n"
        L"48 64313448 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"49 64313455 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"50 64313463 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"51 61934444 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0246 0100 0750\n"
        L"52 62797048 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"53 62772629 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"54 63230320 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"55 64313505 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"56 64313513 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"57 64313885 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0247 0100 0750:)]]>"
        L"</AsoupEvent>"
        );

    asoup_layer->createPath(
        L"<AsoupEvent create_time='20191111T192100Z' name='Info_changing' Bdg='ASOUP 1042[09820]' index='0982-027-1800' num='2442' length='57' weight='1392' through='Y' waynum='13' parknum='8' dirTo='09750' adjTo='09813'><![CDATA[(:1042 909/000+09820 2442 0982 027 1800 02 09750 11 11 21 21 08/13 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01392 00000 228 62712617 64313885 000 000 00 000 057.60 000 057 000\n"
        L"Ю4 11290 2 83 000 057.60 000 057 :)]]>"
        L"</AsoupEvent>"
        );

    asoup_layer->createPath(
        L"<AsoupEvent create_time='20191111T193600Z' name='Departure' Bdg='ASOUP 1042[09820]' index='0982-027-1800' num='2442' length='57' weight='1392' through='Y' waynum='13' parknum='8' dirTo='09750' adjTo='09813' linkBadge='1C[09813]' linkTime='20191111T195851Z' linkCode='Form'><![CDATA[(:1042 909/000+09820 2442 0982 027 1800 03 09750 11 11 21 36 08/13 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01392 00000 228 62712617 64313885 000 000 00 000 057.60 000 057 000\n"
        L"Ю3 530 00010601 1 16 15 0000 00000 GRODIS      \n"
        L"Ю3 530 00010602 9\n"
        L"Ю4 11290 2 83 000 057.60 000 057 \n"
        L"Ю12 00 62712617 1 000 85270 16118 2738 300 00 00 00 00 00 0249 12 60 0600 04 100 18000 00000 11290 83 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        L"Ю12 00 62817507 255255255255 \n"
        L"Ю12 00 62822085 255239255255 0248\n"
        L"Ю12 00 54017116 203237255255 864701611740002405600\n"
        L"Ю12 00 54770508 255239255255 0236\n"
        L"Ю12 00 60517828 219237255255 8527050002460600\n"
        L"Ю12 00 62952478 255239255255 0249\n"
        L"Ю12 00 64039274 195239255255 861401611780230000238\n"
        L"Ю12 00 63854251 195239255255 862201611758603000245\n"
        L"Ю12 00 62639539 255255255255 \n"
        L"Ю12 00 63450647 255255255255 \n"
        L"Ю12 00 62981717 255255255255 \n"
        L"Ю12 00 62981519 255255255255 \n"
        L"Ю12 00 62842679 251255255255 400\n"
        L"Ю12 00 55183958 255253255255 5600\n"
        L"Ю12 00 61955415 219237255255 8621000002390600\n"
        L"Ю12 00 55184022 223237255255 8622002455600\n"
        L"Ю12 00 62305842 223237255255 8621002390600\n"
        L"Ю12 00 56147176 199237255255 8647016117273802345600\n"
        L"Ю12 00 63910236 195237255255 8622016117586050002450600\n"
        L"Ю12 00 60831682 255255255255 \n"
        L"Ю12 00 61484531 255255255255 \n"
        L"Ю12 00 61989620 195239255255 852701611827380000244\n"
        L"Ю12 00 61957817 235239255255 161173000247\n"
        L"Ю12 00 62710330 255239255255 0249\n"
        L"Ю12 00 62795133 255239255255 0247\n"
        L"Ю12 00 63172084 255239255255 0248\n"
        L"Ю12 00 62809629 255239255255 0250\n"
        L"Ю12 00 63636724 195239255255 862201611758600000245\n"
        L"Ю12 00 62296777 199239255255 852701611827380243\n"
        L"Ю12 00 63636914 199239255255 862201611758600245\n"
        L"Ю12 00 62813571 195239255255 852701611827384000248\n"
        L"Ю12 00 62954839 255239255255 0249\n"
        L"Ю12 00 64319031 195239255255 862201611758605000245\n"
        L"Ю12 00 64316425 255255255255 \n"
        L"Ю12 00 64319080 255255255255 \n"
        L"Ю12 00 60481116 195239255255 861401611780233000240\n"
        L"Ю12 00 64121148 255239255255 0238\n"
        L"Ю12 00 61756680 255239255255 0239\n"
        L"Ю12 00 61765160 255239255255 0240\n"
        L"Ю12 00 64061583 255239255255 0239\n"
        L"Ю12 00 60593787 251239255255 4000235\n"
        L"Ю12 00 64085616 255239255255 0237\n"
        L"Ю12 00 64084585 255239255255 0239\n"
        L"Ю12 00 61140638 195239255255 862101611758600000238\n"
        L"Ю12 00 63810659 199255255255 86140161178023\n"
        L"Ю12 00 62824826 195239255255 852701611727385000250\n"
        L"Ю12 00 64313448 255239255255 0247\n"
        L"Ю12 00 64313455 255255255255 \n"
        L"Ю12 00 64313463 255239255255 0248\n"
        L"Ю12 00 61934444 255239255255 0246\n"
        L"Ю12 00 62797048 235239255255 161183000247\n"
        L"Ю12 00 62772629 255239255255 0249\n"
        L"Ю12 00 63230320 255239255255 0248\n"
        L"Ю12 00 64313505 255239255255 0247\n"
        L"Ю12 00 64313513 255255255255 \n"
        L"Ю12 00 64313885 251255255255 000:)]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='1060' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GRODIS' Tim_Beg='2019-11-11 16:15' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    asoup_layer->createPath(
        L"<AsoupEvent create_time='20191111T193600Z' name='Info_changing' Bdg='ASOUP 904[09820]' index='0982-027-1800' num='2442' length='57' weight='1392' through='Y'><![CDATA[(:904 0982 2442 0982 27 1800 1 11 11 21 36 057 01392 0 0000 0 0\n"
        L"01 62712617 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"02 62817507 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"03 62822085 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"04 54017116 0202 000 86475  16117 2738 4 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"05 54770508 0202 000 86475  16117 2738 4 0 0 0 00/00 11020 000 09826  0236 0100 0700\n"
        L"06 60517828 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0246 0100 0750\n"
        L"07 62952478 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"08 64039274 0202 000 86146  16117 8023 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"09 63854251 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"10 62639539 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"11 63450647 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"12 62981717 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"13 62981519 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"14 62842679 0202 000 86221  16117 5860 4 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"15 55183958 0202 000 86221  16117 5860 4 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"16 61955415 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"17 55184022 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"18 62305842 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"19 56147176 0202 000 86475  16117 2738 0 0 0 0 00/00 11020 000 09826  0234 0100 0700\n"
        L"20 63910236 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"21 60831682 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"22 61484531 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"23 61989620 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0244 0100 0750\n"
        L"24 61957817 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"25 62710330 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"26 62795133 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"27 63172084 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"28 62809629 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0250 0100 0750\n"
        L"29 63636724 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"30 62296777 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0243 0100 0750\n"
        L"31 63636914 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"32 62813571 0202 000 85278  16118 2738 4 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"33 62954839 0202 000 85278  16118 2738 4 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"34 64319031 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"35 64316425 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"36 64319080 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"37 60481116 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"38 64121148 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"39 61756680 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"40 61765160 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"41 64061583 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"42 60593787 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0235 0100 0700\n"
        L"43 64085616 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0237 0100 0695\n"
        L"44 64084585 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"45 61140638 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"46 63810659 0202 000 86146  16117 8023 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"47 62824826 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0250 0100 0750\n"
        L"48 64313448 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"49 64313455 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"50 64313463 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"51 61934444 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0246 0100 0750\n"
        L"52 62797048 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"53 62772629 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"54 63230320 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"55 64313505 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"56 64313513 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"57 64313885 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0247 0100 0750:)]]>"
        L"</AsoupEvent>"
        );

    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20191111T195851Z' name='Form' Bdg='1C[09813]' waynum='1' parknum='1' index='0982-027-1800' num='V2442' length='57' weight='1392' through='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='1060' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GRODIS' Tim_Beg='2019-11-11 16:15' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191111T195924Z' name='Departure' Bdg='1C[09813]' waynum='1' parknum='1' optCode='09812:09813' />"
        L"<SpotEvent create_time='20191111T200955Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' intervalSec='4' optCode='09810:09812' />"
        L"<SpotEvent create_time='20191111T202850Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' intervalSec='9' optCode='09803:09810' />"
        L"<SpotEvent create_time='20191111T204037Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' intervalSec='3' optCode='09802:09803' />"
        L"<SpotEvent create_time='20191111T205141Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' intervalSec='4' optCode='09801:09802' />"
        L"<SpotEvent create_time='20191111T210356Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' intervalSec='10' optCode='09800:09801' />"
        L"<SpotEvent create_time='20191111T211827Z' name='Transition' Bdg='2C[09800]' waynum='2' parknum='1' intervalSec='7' optCode='09790:09800' />"
        L"<SpotEvent create_time='20191111T212742Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' intervalSec='8' optCode='09780:09790' />"
        L"<SpotEvent create_time='20191111T213912Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' intervalSec='2' optCode='09751:09780' />"
        L"<SpotEvent create_time='20191111T215335Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' intervalSec='3' optCode='09750:09751' />"
        L"<SpotEvent create_time='20191111T220405Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' intervalSec='3' optCode='09750:09772' />"
        L"<SpotEvent create_time='20191111T222653Z' name='Arrival' Bdg='2C[09772]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191111T224239Z' name='Departure' Bdg='2C[09772]' waynum='2' parknum='1' optCode='09764:09772' />"
        L"<SpotEvent create_time='20191111T230209Z' name='Transition' Bdg='1C[09764]' waynum='1' parknum='1' intervalSec='1' optCode='09180:09764' />"
        L"<SpotEvent create_time='20191111T233203Z' name='Arrival' Bdg='8C[09180]' waynum='8' parknum='1' />"
        L"<SpotEvent create_time='20191112T015118Z' name='Departure' Bdg='8C[09180]' waynum='8' parknum='1' index='0982-027-1800' num='V2442' length='57' weight='1392' through='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='100' Consec='1'>"
        L"<Crew EngineDriver='CIMOTISHS' Tim_Beg='2019-11-12 01:52' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191112T015749Z' name='Transition' Bdg='3AC[09181]' waynum='3' parknum='1' intervalSec='27' optCode='09181:09251' />"
        L"<SpotEvent create_time='20191112T021214Z' name='Transition' Bdg='1C[09251]' waynum='1' parknum='1' optCode='09241:09251' index='0982-027-1800' num='J2442' length='57' weight='1392' through='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='100' Consec='1'>"
        L"<Crew EngineDriver='CIMOTISHS' Tim_Beg='2019-11-12 01:52' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191112T022209Z' name='Transition' Bdg='1C[09241]' waynum='1' parknum='1' optCode='09240:09241' />"
        L"<SpotEvent create_time='20191112T023252Z' name='Transition' Bdg='1C[09240]' waynum='1' parknum='1' intervalSec='1' optCode='09230:09240' />"
        L"<SpotEvent create_time='20191112T024328Z' name='Transition' Bdg='1bC[09230]' waynum='1' parknum='1' intervalSec='7' optCode='09220:09230' />"
        L"<SpotEvent create_time='20191112T025246Z' name='Transition' Bdg='1C[09220]' waynum='1' parknum='1' intervalSec='2' optCode='09211:09220' />"
        L"<SpotEvent create_time='20191112T030834Z' name='Transition' Bdg='1C[09211]' waynum='1' parknum='1' intervalSec='1' optCode='09210:09211' />"
        L"<SpotEvent create_time='20191112T032154Z' name='Arrival' Bdg='2C[09210]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191112T032622Z' name='Departure' Bdg='2C[09210]' waynum='2' parknum='1' optCode='09202:09210' />"
        L"<SpotEvent create_time='20191112T033921Z' name='Transition' Bdg='1C[09202]' waynum='1' parknum='1' optCode='09200:09202' />"
        L"<SpotEvent create_time='20191112T034841Z' name='Transition' Bdg='1C[09200]' waynum='1' parknum='1' intervalSec='2' optCode='09193:09200' />"
        L"<SpotEvent create_time='20191112T035715Z' name='Transition' Bdg='1C[09193]' waynum='1' parknum='1' intervalSec='2' optCode='09191:09193' />"
        L"<SpotEvent create_time='20191112T041506Z' name='Transition' Bdg='1C[09191]' waynum='1' parknum='1' intervalSec='7' optCode='09190:09191' />"
        L"<SpotEvent create_time='20191112T042209Z' name='Transition' Bdg='1C[09190]' waynum='1' parknum='1' intervalSec='4' optCode='09190:11420' />"
        L"<SpotEvent create_time='20191112T042417Z' name='Span_move' Bdg='DK4C[09190:11420]' waynum='1'>"
        L"<rwcoord picketing1_val='294~66' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='294~932' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20191112T042756Z'>"
        L"<PrePoint layer='asoup' Bdg='ASOUP 1042[09820]' name='Departure' create_time='20191111T193600Z' index='0982-027-1800' num='2442' length='57' weight='1392' through='Y' waynum='13' parknum='8'><![CDATA[(:1042 909/000+09820 2442 0982 027 1800 03 09750 11 11 21 36 08/13 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01392 00000 228 62712617 64313885 000 000 00 000 057.60 000 057 000\n"
        L"Ю3 530 00010601 1 16 15 0000 00000 GRODIS      \n"
        L"Ю3 530 00010602 9\n"
        L"Ю4 11290 2 83 000 057.60 000 057 \n"
        L"Ю12 00 62712617 1 000 85270 16118 2738 300 00 00 00 00 00 0249 12 60 0600 04 100 18000 00000 11290 83 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        L"Ю12 00 62817507 255255255255 \n"
        L"Ю12 00 62822085 255239255255 024\n"
        L"Ю12 00 54017116 203237255255 864701611740002405600\n"
        L"Ю12 00 54770508 255239255255 0236\n"
        L"Ю12 00 60517828 219237255255 8527050002460600\n"
        L"Ю12 00 62952478 255239255255 0249\n"
        L"Ю12 00 64039274 195239255255 861401611780230000238\n"
        L"Ю12 00 63854251 195239255255 862201611758603000245\n"
        L"Ю12 00 62639539 255255255255 \n"
        L"Ю12 00 63450647 255255255255 \n"
        L"Ю12 00 62981717 255255255255 \n"
        L"Ю12 00 62981519 255255255255 \n"
        L"Ю12 00 62842679 251255255255 400\n"
        L"Ю12 00 55183958 255253255255 5600\n"
        L"Ю12 00 61955415 219237255255 8621000002390600\n"
        L"Ю12 00 55184022 223237255255 8622002455600\n"
        L"Ю12 00 62305842 223237255255 8621002390600\n"
        L"Ю12 00 56147176 199237255255 8647016117273802345600\n"
        L"Ю12 00 63910236 195237255255 8622016117586050002450600\n"
        L"Ю12 00 60831682 255255255255 \n"
        L"Ю12 00 61484531 255255255255 \n"
        L"Ю12 00 61989620 195239255255 852701611827380000244\n"
        L"Ю12 00 61957817 235239255255 161173000247\n"
        L"Ю12 00 62710330 255239255255 0249\n"
        L"Ю12 00 62795133 255239255255 0247\n"
        L"Ю12 00 63172084 255239255255 0248\n"
        L"Ю12 00 62809629 255239255255 0250\n"
        L"Ю12 00 63636724 195239255255 862201611758600000245\n"
        L"Ю12 00 62296777 199239255255 852701611827380243\n"
        L"Ю12 00 63636914 199239255255 862201611758600245\n"
        L"Ю12 00 62813571 195239255255 852701611827384000248\n"
        L"Ю12 00 62954839 255239255255 0249\n"
        L"Ю12 00 64319031 195239255255 862201611758605000245\n"
        L"Ю12 00 64316425 255255255255 \n"
        L"Ю12 00 64319080 255255255255 \n"
        L"Ю12 00 60481116 195239255255 861401611780233000240\n"
        L"Ю12 00 64121148 255239255255 0238\n"
        L"Ю12 00 61756680 255239255255 0239\n"
        L"Ю12 00 61765160 255239255255 0240\n"
        L"Ю12 00 64061583 255239255255 0239\n"
        L"Ю12 00 60593787 251239255255 4000235\n"
        L"Ю12 00 64085616 255239255255 0237\n"
        L"Ю12 00 64084585 255239255255 0239\n"
        L"Ю12 00 61140638 195239255255 862101611758600000238\n"
        L"Ю12 00 63810659 199255255255 86140161178023\n"
        L"Ю12 00 62824826 195239255255 852701611727385000250\n"
        L"Ю12 00 64313448 255239255255 0247\n"
        L"Ю12 00 64313455 255255255255 \n"
        L"Ю12 00 64313463 255239255255 0248\n"
        L"Ю12 00 61934444 255239255255 0246\n"
        L"Ю12 00 62797048 235239255255 161183000247\n"
        L"Ю12 00 62772629 255239255255 0249\n"
        L"Ю12 00 63230320 255239255255 0248\n"
        L"Ю12 00 64313505 255239255255 0247\n"
        L"Ю12 00 64313513 255255255255 \n"
        L"Ю12 00 64313885 251255255255 000:)]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='1060' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GRODIS' Tim_Beg='2019-11-11 16:15' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<Action code='Attach' District='09180-09820' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='1C[09813]' create_time='20191111T195851Z' index='0982-027-1800' num='V2442' length='57' weight='1392' through='Y' waynum='1' parknum='1'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='1060' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GRODIS' Tim_Beg='2019-11-11 16:15' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09813' />"
        L"<Station esr='09820' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    topology->Load(  L"*09790,09820 {09790,09800,09801,09802,09803,09810,09812,09813,09820} \n" );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "   Form   Departure   Arrival   Departure Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "2w[22222] 2w[22222]  3w[33333] 3w[33333]   4w[44444] " ) );
}

void TC_Hem_aeAttach::AttachAsoupDepartureToPathOnBorderStation_5596()
{
    asoup_layer->createPath(
        L"<AsoupEvent create_time='20191117T032300Z' name='Departure' Bdg='ASOUP 1042[11451]' index='1048-390-1801' num='3062' length='56' weight='1592' net_weight='354' divisional='Y' parknum='1'><![CDATA[(:1042 909/000+11000 3062 1048 390 1801 03 11310 17 11 05 23 02/03 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 01592 00354 128 58601659 92700202 000 000 00 032 000.90 032 000 000\n"
        L"Ю3 531 00012020 1 03 56 0000 00000 MARKOVS     \n"
        L"Ю4 11290 2 17 032 000.90 032 000 \n"
        L"Ю12 00 58601659 1 011 18010 38108 1069 307 04 00 00 00 00 0384 20 90 5100 04 174 18010 00000 11290 17 10480 15 00000 0000 020 1 1280 OXPAHA 132 000 00000000\n"
        L"Ю12 00 51015535 255255254127 103003916\n"
        L"Ю12 00 58623869 255255254127 000000000\n"
        L"Ю12 00 92700277 255237255255 04010926\n"
        L"Ю12 00 58604059 255237255255 03835100\n"
        L"Ю12 00 58616426 255239254127 0384103003916\n"
        L"Ю12 00 51009280 255255255255 \n"
        L"Ю12 00 58601634 255255255255 \n"
        L"Ю12 00 58616459 191255255255 010\n"
        L"Ю12 00 58602707 255255255255 \n"
        L"Ю12 00 59616011 191255255255 011\n"
        L"Ю12 00 92700541 255237255255 03990926\n"
        L"Ю12 00 58627522 255237254127 03845100000000000\n"
        L"Ю12 00 92700376 255237254127 04010926103003916\n"
        L"Ю12 00 59617894 255237255255 03845100\n"
        L"Ю12 00 58616434 255255255255 \n"
        L"Ю12 00 58600883 255255255255 \n"
        L"Ю12 00 58602988 255255254127 000000000\n"
        L"Ю12 00 58616350 255255255255 \n"
        L"Ю12 00 58616673 255255255255 \n"
        L"Ю12 00 58616392 191255254127 010103003916\n"
        L"Ю12 00 58633512 191255255255 011\n"
        L"Ю12 00 58603044 191255254127 012000000000\n"
        L"Ю12 00 59616003 255255254127 103003916\n"
        L"Ю12 00 51009132 255255255255 \n"
        L"Ю12 00 59618116 191255254127 011000000000\n"
        L"Ю12 00 92700228 255237255255 04020926\n"
        L"Ю12 00 58624164 253237254127 0603845100103003916\n"
        L"Ю12 00 58631896 253255254127 04000000000\n"
        L"Ю12 00 58602624 255255255255 \n"
        L"Ю12 00 58611880 191255254127 012103003916\n"
        L"Ю12 00 92700202 255237254127 04010926000000000:)]]><Locomotive Series='М62' NumLoc='1202' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='MARKOVS' Tim_Beg='2019-11-17 03:56' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20191117T034945Z' name='Form' Bdg='P4P[11445:11451]' />"
        L"<SpotEvent create_time='20191117T035636Z' name='Transition' Bdg='5SP[11445]'>"
        L"<rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191117T040950Z' name='Arrival' Bdg='3BC[11443]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20190826T105950Z'>"
        L"<PrePoint layer='asoup' Bdg='ASOUP 1042[11451]' name='Departure' create_time='20191117T032300Z' index='1048-390-1801' num='3062' length='56' weight='1592' net_weight='354' divisional='Y' parknum='1'><![CDATA[(:1042 909/000+11000 3062 1048 390 1801 03 11310 17 11 05 23 02/03 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 01592 00354 128 58601659 92700202 000 000 00 032 000.90 032 000 000\n"
        L"Ю3 531 00012020 1 03 56 0000 00000 MARKOVS     \n"
        L"Ю4 11290 2 17 032 000.90 032 000 \n"
        L"Ю12 00 58601659 1 011 18010 38108 1069 307 04 00 00 00 00 0384 20 90 5100 04 174 18010 00000 11290 17 10480 15 00000 0000 020 1 1280 OXPAHA 132 000 00000000\n"
        L"Ю12 00 51015535 255255254127 103003916\n"
        L"Ю12 00 58623869 255255254127 000000000\n"
        L"Ю12 00 92700277 255237255255 04010926\n"
        L"Ю12 00 58604059 255237255255 03835100\n"
        L"Ю12 00 58616426 255239254127 0384103003916\n"
        L"Ю12 00 51009280 255255255255 \n"
        L"Ю12 00 58601634 255255255255 \n"
        L"Ю12 00 58616459 191255255255 010\n"
        L"Ю12 00 58602707 255255255255 \n"
        L"Ю12 00 59616011 191255255255 011\n"
        L"Ю12 00 92700541 255237255255 03990926\n"
        L"Ю12 00 58627522 255237254127 03845100000000000\n"
        L"Ю12 00 92700376 255237254127 04010926103003916\n"
        L"Ю12 00 59617894 255237255255 03845100\n"
        L"Ю12 00 58616434 255255255255 \n"
        L"Ю12 00 58600883 255255255255 \n"
        L"Ю12 00 58602988 255255254127 000000000\n"
        L"Ю12 00 58616350 255255255255 \n"
        L"Ю12 00 58616673 255255255255 \n"
        L"Ю12 00 58616392 191255254127 010103003916\n"
        L"Ю12 00 58633512 191255255255 011\n"
        L"Ю12 00 58603044 191255254127 012000000000\n"
        L"Ю12 00 59616003 255255254127 103003916\n"
        L"Ю12 00 51009132 255255255255 \n"
        L"Ю12 00 59618116 191255254127 011000000000\n"
        L"Ю12 00 92700228 255237255255 04020926\n"
        L"Ю12 00 58624164 253237254127 0603845100103003916\n"
        L"Ю12 00 58631896 253255254127 04000000000\n"
        L"Ю12 00 58602624 255255255255 \n"
        L"Ю12 00 58611880 191255254127 012103003916\n"
        L"Ю12 00 92700202 255237254127 04010926000000000:)]]><Locomotive Series='М62' NumLoc='1202' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='MARKOVS' Tim_Beg='2019-11-17 03:56' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<Action code='Attach' District='11000-12910' DistrictRight='Y' />"
        L"<PostPoint name='Form' Bdg='P4P[11445:11451]' create_time='20191117T034945Z' />"
        L"<EsrList />"
        L"</A2F_HINT>"
        );
    topology->Load(
        L"*11445,11443 {11445,11451,11443}\n");

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	AsoupQueueMgr asoupQMgr;
    aeAttach_AsoupToHappen att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, asoupQMgr, context, bill.getHeadAsoupThrows(), bill.getTailSpotThrows(), false );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );

    //     CPPUNIT_ASSERT( implementUndo() );
    //     CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "num",  "3062             -          -             " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "index",  "1048-390-1801  -          -             " ) );
}

void TC_Hem_aeAttach::ExtendCarrierCode()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200621T032631Z' name='Arrival' Bdg='3B[11120]' waynum='3' parknum='1' Bdg='№875[08620]' waynum='1' parknum='1' index='' num='875' length='6' weight='185' mvps='Y' >"
        L"<Locomotive Series='DR1AC' NumLoc='311'>"
        L"<Crew EngineDriver='Rusmanis' Tim_Beg='2020-06-21 05:22' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200621T032928Z' name='Departure' Bdg='3A[11120]' waynum='3' parknum='1' optCode='11120:11121' />"
        L"<SpotEvent create_time='20200621T033711Z' name='Transition' Bdg='1C[11121]' waynum='1' parknum='1' intervalSec='31' optCode='11121:11130' />"
        L"<SpotEvent create_time='20200621T034403Z' name='Transition' Bdg='1C[11130]' waynum='1' parknum='1' intervalSec='42' optCode='09510:11130' />"
        L"<SpotEvent create_time='20200621T035000Z' name='Arrival' Bdg='1C[09510]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200621T035148Z' name='Departure' Bdg='1C[09510]' waynum='1' parknum='1' optCode='09510:09511' />"
        L"<SpotEvent create_time='20200621T035721Z' name='Span_move' Bdg='SL1[09510:09511]' waynum='1'>"
        L"<rwcoord picketing1_val='55~600' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='53~600' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200621T035828Z' name='Death' Bdg='SL1[09510:09511]' waynum='1'>"
        L"<rwcoord picketing1_val='55~600' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='53~600' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200621T040124Z' name='Form' Bdg='1C[09511]' waynum='1' parknum='1' index='' num='875' length='6' weight='185' mvps='Y'>"
        L"<Locomotive Series='DR1AC' NumLoc='311' CarrierCode='24'>"
        L"<Crew EngineDriver='Rusmanis' Tim_Beg='2020-06-21 05:22' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200621T040124Z' name='Departure' Bdg='1C[09511]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200621T041217Z' name='Transition' Bdg='1C[09520]' waynum='1' parknum='1' intervalSec='129' />"
        L"<SpotEvent create_time='20200621T041751Z' name='Transition' Bdg='1C[09530]' waynum='1' parknum='1' intervalSec='32' optCode='09530:09540' />"
        L"<SpotEvent create_time='20200621T044857Z' name='Disform' Bdg='1C[09530]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200621T144500Z'>"
        L"<PrePoint name='Death' Bdg='SL1[09510:09511]' create_time='20200621T035828Z' waynum='1'>"
        L"<rwcoord picketing1_val='54~600' picketing1_comm='Земитани-Валга' />"
        L"</PrePoint>"
        L"<Action code='Attach' District='09580-08620' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='1C[09511]' create_time='20200621T040124Z' index='' num='875' length='6' weight='185' mvps='Y' waynum='1' parknum='1'>"
        L"<Locomotive Series='DR1AC' NumLoc='311' CarrierCode='24'>"
        L"<Crew EngineDriver='Rusmanis' Tim_Beg='2020-06-21 05:22' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09511' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    auto listFrags = hl.GetPaths_OnlyFrags(time_from_iso("20200621T032631Z"),time_from_iso("20200621T044857Z"));
    CPPUNIT_ASSERT( listFrags.size() == 1 && listFrags.front().size()==1);
    auto vLoco = listFrags.front().front().second->Locomotives();
    CPPUNIT_ASSERT(vLoco.size()==1 && vLoco.front().getCarrier()==24);
}

// void TC_Hem_aeAttach::SpanStoppingAttach()
// {
//     auto& hl = *happen_layer;
//     // первая нить
//     hl.createPath(
//         L"<HemPath>"
//         <SpotEvent create_time="20200707T053651Z" name="Form" Bdg="1C[09251]" waynum="1" parknum="1" />
//         <SpotEvent create_time="20200707T053651Z" name="Departure" Bdg="1C[09251]" waynum="1" parknum="1" />
//         <SpotEvent create_time="20200707T053801Z" name="Span_stopping_begin" Bdg="spanwork[09241:09251]" waynum="1">
//         <rwcoord picketing1_val="181~500" picketing1_comm="Вентспилс" />
//         <rwcoord picketing1_val="181~600" picketing1_comm="Вентспилс" />
//         </SpotEvent>
//         <SpotEvent create_time="20200707T054132Z" name="Span_stopping_end" Bdg="spanwork[09241:09251]" waynum="1">
//         <rwcoord picketing1_val="181~500" picketing1_comm="Вентспилс" />
//         <rwcoord picketing1_val="181~600" picketing1_comm="Вентспилс" />
//         </SpotEvent>
//         <SpotEvent create_time="20200707T054132Z" name="Span_stopping_begin" Bdg="GZ4C[09241:09251]" waynum="1">
//         <rwcoord picketing1_val="180~800" picketing1_comm="Вентспилс" />
//         <rwcoord picketing1_val="181~900" picketing1_comm="Вентспилс" />
//         </SpotEvent>
//         <SpotEvent create_time="20200707T054150Z" name="Span_stopping_end" Bdg="GZ4C[09241:09251]" waynum="1">
//         <rwcoord picketing1_val="180~800" picketing1_comm="Вентспилс" />
//         <rwcoord picketing1_val="181~900" picketing1_comm="Вентспилс" />
//         </SpotEvent>
//         <SpotEvent create_time="20200707T054150Z" name="Death" Bdg="GZ4C[09241:09251]" waynum="1">
//         <rwcoord picketing1_val="180~800" picketing1_comm="Вентспилс" />
//         <rwcoord picketing1_val="181~900" picketing1_comm="Вентспилс" />
//         </SpotEvent>
//         L"</HemPath>"
//         );
//     // вторая нить
//     hl.createPath(
//         L"<HemPath>"
//         L"<SpotEvent create_time='20200621T040124Z' name='Form' Bdg='1C[09511]' waynum='1' parknum='1' index='' num='875' length='6' weight='185' mvps='Y'>"
//         L"<Locomotive Series='DR1AC' NumLoc='311' CarrierCode='24'>"
//         L"<Crew EngineDriver='Rusmanis' Tim_Beg='2020-06-21 05:22' />"
//         L"</Locomotive>"
//         L"</SpotEvent>"
//         L"<SpotEvent create_time='20200621T040124Z' name='Departure' Bdg='1C[09511]' waynum='1' parknum='1' />"
//         L"<SpotEvent create_time='20200621T041217Z' name='Transition' Bdg='1C[09520]' waynum='1' parknum='1' intervalSec='129' />"
//         L"<SpotEvent create_time='20200621T041751Z' name='Transition' Bdg='1C[09530]' waynum='1' parknum='1' intervalSec='32' optCode='09530:09540' />"
//         L"<SpotEvent create_time='20200621T044857Z' name='Disform' Bdg='1C[09530]' waynum='1' parknum='1' />"
//         L"</HemPath>"
//         );
//     Hem::Bill bill = createBill(
//         L"<A2F_HINT Hint='cmd_edit' issue_moment='20200621T144500Z'>"
//         L"<PrePoint name='Death' Bdg='SL1[09510:09511]' create_time='20200621T035828Z' waynum='1'>"
//         L"<rwcoord picketing1_val='54~600' picketing1_comm='Земитани-Валга' />"
//         L"</PrePoint>"
//         L"<Action code='Attach' District='09580-08620' DistrictRight='Y' SaveLog='Y' />"
//         L"<PostPoint name='Form' Bdg='1C[09511]' create_time='20200621T040124Z' index='' num='875' length='6' weight='185' mvps='Y' waynum='1' parknum='1'>"
//         L"<Locomotive Series='DR1AC' NumLoc='311' CarrierCode='24'>"
//         L"<Crew EngineDriver='Rusmanis' Tim_Beg='2020-06-21 05:22' />"
//         L"</Locomotive>"
//         L"</PostPoint>"
//         L"<EsrList>"
//         L"<Station esr='09511' />"
//         L"</EsrList>"
//         L"</A2F_HINT>"
//         );
// 
//     Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, topology, nullptr, 0);
//     aeAttach att( hl.GetWriteAccess(), nullptr, context, bill );
//     att.Action();
// 
//     CPPUNIT_ASSERT( hl.path_count() == 1 );
//     auto listFrags = hl.GetPaths_OnlyFrags(time_from_iso("20200621T032631Z"),time_from_iso("20200621T044857Z"));
//     CPPUNIT_ASSERT( listFrags.size() == 1 && listFrags.front().size()==1);
//     auto vLoco = listFrags.front().front().second->Locomotives();
//     CPPUNIT_ASSERT(vLoco.size()==1 && vLoco.front().getCarrier()==24);
// }


void TC_Hem_aeAttach::AttachWithPocketExit()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200816T185000Z' name='Arrival' Bdg='?[12600]' />"
        L"<SpotEvent create_time='20200816T185000Z' name='Departure' Bdg='?[12600]' />"
        L"<SpotEvent create_time='20200816T191000Z' name='Transition' Bdg='ASOUP 1042[09170]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20200816T191000Z' name='Death' Bdg='ASOUP 1042[09170]' waynum='4' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200816T193550Z' name='Pocket_exit' Bdg='PMIP[09170:09180]' waynum='1' />"
        L"<SpotEvent create_time='20200816T193751Z' name='Entra_stopping' Bdg='PMIP[09170:09180]' waynum='1'/>"
        L"<SpotEvent create_time='20200816T194152Z' name='Arrival' Bdg='2C[09180]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200816T194152Z' name='Disform' Bdg='2C[09180]' waynum='2' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='ASOUP 1042[09170]' create_time='20200816T191000Z' waynum='4' parknum='1' />"                                       
        L"<Action code='Attach' District='09180-09860' DistrictRight='Y' />"                                                               
        L"<PostPoint name='Pocket_exit' Bdg='PMIP[09170:09180]' create_time='20200816T193550Z' waynum='1' />"
        L"<EsrList>"                                                                                                                       
        L"<Station esr='09810' />"                                                                                                       
        L"</EsrList>"                                                                                                                      
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Arrival       Departure   Transition       Span_move         Entra_stopping      Arrival   Disform" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "?[12600] ?[12600]  ASOUP 1042[09170]  PMIP[09170:09180] PMIP[09170:09180]   2C[09180] 2C[09180]" ) );
}

class UtUserChart : public UserChart
{
public:
	UserChart::getHappenLayer;
	UserChart::getAsoupLayer;
	UserChart::SimilarFreeAsoupToQueue;
};


void TC_Hem_aeAttach::AttachAsoupToAsoup()
{

	UtUserChart chart;
	UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( chart.getHappenLayer() );

    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200819T022330Z' name='Form' Bdg='5C[11750]' waynum='5' parknum='1' index='0001-024-1142' num='4906' joincrgloc='Y'>"
        L"<Locomotive Series='ЧМЭ3' NumLoc='4852' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='SAKUROVS' Tim_Beg='2020-08-18 18:18' />"
        L"</Locomotive>"
        L"<Locomotive Series='ЧМЭ3М' NumLoc='6192' Consec='4' CarrierCode='4'>"
        L"<Crew EngineDriver='ALIHNOVICH' Tim_Beg='2020-08-18 18:18' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200819T022330Z' name='Departure' Bdg='5C[11750]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20200819T024931Z' name='Arrival' Bdg='10CA[11760]' waynum='10' parknum='1' />"
        L"<SpotEvent create_time='20200819T024931Z' name='Disform' Bdg='10CA[11760]' waynum='10' parknum='1' />"
        L"</HemPath>"
        );
	chart.processNewAsoupText(L"(:1042 909/000+11160 4805 0001 024 1142 03 11760 19 08 03 26 01/04 0 0/00 00 0\n"
		         L"Ю3 561 00061920 1 18 18 0000 00000 ALIHNOVICH  \n"
		         L"Ю3 557 00048520 4 18 18 0000 00000 SAKUROVS    :)", boost::gregorian::date(2020, 8, 19 ), nullptr, 0);

	chart.processNewAsoupText( L"(:1042 909/000+11750 4805 0001 024 1142 01 11750 19 08 05 09 01/05 0 0/00 00 0\n"
		L"Ю3 561 00061920 1 18 18 0000 00000 ALIHNOVICH  \n"
		L"Ю3 557 00048520 4 18 18 0000 00000 SAKUROVS    :)", boost::gregorian::date(2020, 8, 19 ), nullptr, 0);

	chart.processNewAsoupText( L"(:1042 909/000+11750 4906 0001 024 1142 03 11420 19 08 05 23 01/05 0 0/00 00 0\n"
		L"Ю3 557 00048520 1 18 18 0000 00000 SAKUROVS    \n"
		L"Ю3 561 00061920 4 18 18 0000 00000 ALIHNOVICH  :)", boost::gregorian::date(2020, 8, 19 ), nullptr, 0);

	chart.processNewAsoupText( L"(:1042 909/000+11750 4806 0001 024 1142 02 11420 19 08 05 10 01/05 0 0/00 00 0\n"
		L"Ю3 561 00061920 1 18 18 0000 00000 ALIHNOVICH  \n"
		L"Ю3 557 00048520 4 18 18 0000 00000 SAKUROVS    :)", boost::gregorian::date(2020, 8, 19 ), nullptr, 0);

	UtLayer<AsoupLayer>& al = UtLayer<AsoupLayer>::upgrade( chart.getAsoupLayer() );
    CPPUNIT_ASSERT( al.path_count() == 4 );
    CPPUNIT_ASSERT( hl.path_count() == 1 );

	AsoupEventPtr departure11160;
	AsoupEventPtr arrival11750;
	auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
	for ( auto asoup : pathAsoupList )
	{
		if ( asoup->GetCode()==HCode::DEPARTURE && asoup->GetBadge().num()==EsrKit(11160) )
			departure11160 = asoup;
		if ( asoup->GetCode()==HCode::ARRIVAL && asoup->GetBadge().num()==EsrKit(11750) )
			arrival11750 = asoup;
	}

	topology->Load(
	         L"*11173,11750 {11173,11164,11160,11710,11750}\n"
	         );
	CPPUNIT_ASSERT( departure11160 && arrival11750 );
	Context context(Hem::Chance::System(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach_AsoupToAsoup att( AccessHappenAsoup(hl.base(), al.base()), nullptr, asoupQueue, context, *departure11160, *arrival11750, false );
    att.Action();
    collectUndo();

	//вызвать update_time в главном цикле!
	chart.updateTime( time_from_iso("20200819T024931Z"), nullptr );
	CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
	CPPUNIT_ASSERT( hl.exist_series("name", "Departure Transition Arrival Departure Arrival Disform"));
    CPPUNIT_ASSERT( hl.exist_series( "num",  "4805    -    4806 4906  -  -  " ) );
}

void TC_Hem_aeAttach::AsoupToAsoup_LinkNotBorderEvent()
{
    auto& al = *asoup_layer;
    auto& hl = *happen_layer;

    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200825T221412Z' name='Form' Bdg='6JC[09006]' waynum='6' parknum='1' index='0970-091-8014' num='2704' length='50' weight='4756' net_weight='3557' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2М62УМ' NumLoc='119' Consec='1'>"
        L"<Crew EngineDriver='M-VOLODJKO' Tim_Beg='2020-08-25 23:38' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T221412Z' name='Departure' Bdg='6JC[09006]' waynum='6' parknum='1' />"
        L"<SpotEvent create_time='20200825T222013Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' intervalSec='38' />"
        L"<SpotEvent create_time='20200825T222250Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='29' optCode='09000:09420' index='0970-091-8014' num='2704' length='50' weight='4756' net_weight='3557' through='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='119' Consec='1'>"
        L"<Crew EngineDriver='M-VOLODJKO' Tim_Beg='2020-08-25 23:38' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T223741Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' intervalSec='24' />"
        L"<SpotEvent create_time='20200825T225659Z' name='Transition' Bdg='1C[09410]' waynum='1' parknum='1' intervalSec='6' />"
        L"<SpotEvent create_time='20200825T232309Z' name='Arrival' Bdg='5C15+[09400]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20200826T003408Z' name='Departure' Bdg='5C[09400]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20200826T011125Z' name='Transition' Bdg='2C[09380]' waynum='2' parknum='1' intervalSec='25' optCode='09370:09380' />"
        L"<SpotEvent create_time='20200826T012408Z' name='Transition' Bdg='2C[09370]' waynum='2' parknum='1' intervalSec='34' />"
        L"<SpotEvent create_time='20200826T012506Z' name='Wrong_way' Bdg='N1P[09360:09370]' waynum='1'>"
        L"<rwcoord picketing1_val='83~500' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='83~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200826T014835Z' name='Arrival' Bdg='1C[09360]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200826T014926Z' name='Departure' Bdg='1C[09360]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200826T021144Z' name='Transition' Bdg='2AC[11760]' waynum='2' parknum='2' intervalSec='6' />"
        L"<SpotEvent create_time='20200826T021210Z' name='Transition' Bdg='2C[11760]' waynum='2' parknum='1' optCode='11420:11760' index='0970-091-8014' num='2704' length='50' weight='4756' net_weight='3557' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2М62УМ' NumLoc='119' Consec='1'>"
        L"<Crew EngineDriver='M-VOLODJKO' Tim_Beg='2020-08-25 23:38' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200826T023545Z' name='Transition' Bdg='1C[11420]' waynum='1' parknum='1' intervalSec='16' />"
        L"<SpotEvent create_time='20200826T030257Z' name='Arrival' Bdg='1C[11412]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200826T031122Z' name='Departure' Bdg='1C[11412]' waynum='1' parknum='1' optCode='11401:11412' />"
        L"<SpotEvent create_time='20200826T032335Z' name='Transition' Bdg='1C[11401]' waynum='1' parknum='1' intervalSec='11' optCode='11391:11401' />"
        L"<SpotEvent create_time='20200826T033246Z' name='Transition' Bdg='1C[11391]' waynum='1' parknum='1' intervalSec='6' optCode='11390:11391' />"
        L"<SpotEvent create_time='20200826T035025Z' name='Transition' Bdg='1C[11390]' waynum='1' parknum='1' intervalSec='9' optCode='11381:11390' />"
        L"<SpotEvent create_time='20200826T035836Z' name='Transition' Bdg='1C[11381]' waynum='1' parknum='1' intervalSec='8' optCode='11380:11381' />"
        L"<SpotEvent create_time='20200826T041026Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' intervalSec='9' optCode='11311:11380' />"
        L"<SpotEvent create_time='20200826T042443Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' intervalSec='16' optCode='11311:11321' />"
        L"<SpotEvent create_time='20200826T043656Z' name='Transition' Bdg='701SP:701+[11321]'>"
        L"<rwcoord picketing1_val='223~200' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200826T044337Z' name='Arrival' Bdg='4C[11310]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20200826T074217Z' name='Departure' Bdg='4C[11310]' waynum='4' parknum='1' index='0970-091-8014' num='2956' length='50' weight='4756' net_weight='3557' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2ТЭ116' NumLoc='1060' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ДУДAPEB' TabNum='30016' Tim_Beg='2020-08-26 09:18' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200826T075035Z' name='Transition' Bdg='1SP[11271]'>"
        L"<rwcoord picketing1_val='440~780' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200826T080411Z' name='Transition' Bdg='1C[11270]' waynum='1' parknum='1' intervalSec='18' optCode='11267:11270' />"
        L"<SpotEvent create_time='20200826T081220Z' name='Transition' Bdg='1AC[11267]' waynum='1' parknum='1' intervalSec='38' optCode='11266:11267' />"
        L"<SpotEvent create_time='20200826T081337Z' name='Transition' Bdg='1C&apos;2[11267]' waynum='1' parknum='2' intervalSec='22' optCode='11266:11267' />"
        L"<SpotEvent create_time='20200826T082239Z' name='Transition' Bdg='1C[11266]' waynum='1' parknum='1' intervalSec='22' optCode='11265:11266' />"
        L"<SpotEvent create_time='20200826T083010Z' name='Transition' Bdg='1AC[11265]' waynum='1' parknum='1' intervalSec='9' optCode='11260:11265' />"
        L"<SpotEvent create_time='20200826T083109Z' name='Transition' Bdg='1C&apos;2[11265]' waynum='1' parknum='2' intervalSec='13' optCode='11260:11265' />"
        L"<SpotEvent create_time='20200826T084219Z' name='Arrival' Bdg='3C[11260]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200826T084219Z' name='Death' Bdg='3C[11260]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200826T092824Z' name='Form' Bdg='3C[11260]' waynum='3' parknum='1' index='0970-091-8014' num='2956' length='50' weight='4756' net_weight='3557' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2ТЭ116' NumLoc='1060' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ДУДAPEB' TabNum='30016' Tim_Beg='2020-08-26 09:18' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200826T092824Z' name='Departure' Bdg='3C[11260]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200826T094000Z' name='Arrival' Bdg='ASOUP 1042[07792]' />"
        L"<SpotEvent create_time='20200826T094000Z' name='Death' Bdg='ASOUP 1042[07792]' />"
        L"</HemPath>"
        );

    al.createPath(
        L"<AsoupEvent create_time='20200826T084400Z' name='Arrival' Bdg='ASOUP 1042[11260]' index='0970-091-8014' num='2956' length='50' weight='4756' net_weight='3557' through='Y' waynum='3' parknum='1' dirFrom='11310' adjFrom='11265' linkBadge='3C[11260]' linkTime='20200826T084219Z' linkCode='Arrival'><![CDATA[(:1042 909/000+11260 2956 0970 091 8014 01 11310 26 08 11 44 01/03 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 050 04756 03557 200 62916721 63285878 000 000 00 050 000.60 050 000 000\n"
        L"Ю3 530 00010601 1 09 18 0000 30016 ДУДAPEB     \n"
        L"Ю3 530 00010602 9\n"
        L"Ю4 07792 2 80 050 000.60 050 000 \n"
        L"Ю12 00 62916721 1 075 80140 14206 5341 300 00 00 00 00 00 0245 20 60 0600 04 100 80140 00000 07792 80 09700 07 09740 8517 020 1 1280 097495 132 000 00000000\n"
        L"Ю12 00 62753207 255255255255 \n"
        L"Ю12 00 63453740 255255255255 \n"
        L"Ю12 00 63903819 255255255255 \n"
        L"Ю12 00 62514856 255239255255 0244\n"
        L"Ю12 00 63292759 255239255255 0245\n"
        L"Ю12 00 56963093 191237255255 07002355600\n"
        L"Ю12 00 54017546 191239255255 0690240\n"
        L"Ю12 00 53546594 255239255255 0238\n"
        L"Ю12 00 56160252 251255255255 500\n"
        L"Ю12 00 52804044 191239255239 07002351250\n"
        L"Ю12 00 54700224 191239255239 06902401280\n"
        L"Ю12 00 63256556 187237255255 07530002450600\n"
        L"Ю12 00 54002928 191237255255 07002435600\n"
        L"Ю12 00 61004479 255237255255 02320600\n"
        L"Ю12 00 60777729 255239255255 0235\n"
        L"Ю12 00 55816318 255253255255 5600\n"
        L"Ю12 00 60145778 187237255255 06950002390600\n"
        L"Ю12 00 56044159 255237255255 02385600\n"
        L"Ю12 00 61826954 191253255255 0700600\n"
        L"Ю12 00 59391334 255237255255 02355600\n"
        L"Ю12 00 55759237 191239255255 0690240\n"
        L"Ю12 00 52220597 255239255239 02361250\n"
        L"Ю12 00 61195269 191237255239 070024006001280\n"
        L"Ю12 00 53555140 191253255255 0695600\n"
        L"Ю12 00 56220155 191239255255 0700233\n"
        L"Ю12 00 54755418 255239255255 0240\n"
        L"Ю12 00 56255045 255239255255 0235\n"
        L"Ю12 00 56176563 255239255255 0234\n"
        L"Ю12 00 60270949 191237255255 07402460600\n"
        L"Ю12 00 55977847 191237255255 07002405600\n"
        L"Ю12 00 56130578 255239255255 0233\n"
        L"Ю12 00 55579718 255239255255 0235\n"
        L"Ю12 00 56022312 255239255255 0231\n"
        L"Ю12 00 56832199 255239255255 0234\n"
        L"Ю12 00 56219306 255255255255 \n"
        L"Ю12 00 55179162 255239255255 0238\n"
        L"Ю12 00 53175378 187239255255 0693000241\n"
        L"Ю12 00 56081607 191239255255 0700234\n"
        L"Ю12 00 59398263 191239255255 0690240\n"
        L"Ю12 00 55976799 191239255255 0700233\n"
        L"Ю12 00 60646478 187237255255 06940002440600\n"
        L"Ю12 00 60067683 191239255255 0700243\n"
        L"Ю12 00 60997368 187239255255 0753000246\n"
        L"Ю12 00 62599642 251239255255 4000248\n"
        L"Ю12 00 62484134 255239255255 0247\n"
        L"Ю12 00 60379450 187239255255 0703000242\n"
        L"Ю12 00 62782123 191239255255 0750249\n"
        L"Ю12 00 62710439 255255255255 \n"
        L"Ю12 00 63285878 255239255255 0248:)]]><feat_texts typeinfo='Т' />"
        L"</AsoupEvent>"
        );

    al.createPath(
        L"<AsoupEvent create_time='20200826T093000Z' name='Departure' Bdg='ASOUP 1042[11260]' index='0970-091-8014' num='2956' length='50' weight='4756' net_weight='3557' through='Y' waynum='3' parknum='1' dirTo='07792' adjTo='07792' linkBadge='3C[11260]' linkTime='20200826T092824Z' linkCode='Departure'><![CDATA[(:1042 909/000+11260 2956 0970 091 8014 03 07792 26 08 12 30 01/03 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 050 04756 03557 200 62916721 63285878 000 000 00 050 000.60 050 000 000\n"
        L"Ю3 530 00010601 1 09 18 0000 30016 ДУДAPEB     \n"
        L"Ю3 530 00010602 9\n"
        L"Ю4 07792 2 80 050 000.60 050 000 \n"
        L"Ю12 00 62916721 1 075 80140 14206 5341 300 00 00 00 00 00 0245 20 60 0600 04 100 80140 00000 07792 80 09700 07 09740 8517 020 1 1280 097495 132 000 00000000\n"
        L"Ю12 00 62753207 255255255255 \n"
        L"Ю12 00 63453740 255255255255 \n"
        L"Ю12 00 63903819 255255255255 \n"
        L"Ю12 00 62514856 255239255255 0244\n"
        L"Ю12 00 63292759 255239255255 0245\n"
        L"Ю12 00 56963093 191237255255 07002355600\n"
        L"Ю12 00 54017546 191239255255 0690240\n"
        L"Ю12 00 53546594 255239255255 0238\n"
        L"Ю12 00 56160252 251255255255 500\n"
        L"Ю12 00 52804044 191239255239 07002351250\n"
        L"Ю12 00 54700224 191239255239 06902401280\n"
        L"Ю12 00 63256556 187237255255 07530002450600\n"
        L"Ю12 00 54002928 191237255255 07002435600\n"
        L"Ю12 00 61004479 255237255255 02320600\n"
        L"Ю12 00 60777729 255239255255 0235\n"
        L"Ю12 00 55816318 255253255255 5600\n"
        L"Ю12 00 60145778 187237255255 06950002390600\n"
        L"Ю12 00 56044159 255237255255 02385600\n"
        L"Ю12 00 61826954 191253255255 0700600\n"
        L"Ю12 00 59391334 255237255255 02355600\n"
        L"Ю12 00 55759237 191239255255 0690240\n"
        L"Ю12 00 52220597 255239255239 02361250\n"
        L"Ю12 00 61195269 191237255239 070024006001280\n"
        L"Ю12 00 53555140 191253255255 0695600\n"
        L"Ю12 00 56220155 191239255255 0700233\n"
        L"Ю12 00 54755418 255239255255 0240\n"
        L"Ю12 00 56255045 255239255255 0235\n"
        L"Ю12 00 56176563 255239255255 0234\n"
        L"Ю12 00 60270949 191237255255 07402460600\n"
        L"Ю12 00 55977847 191237255255 07002405600\n"
        L"Ю12 00 56130578 255239255255 0233\n"
        L"Ю12 00 55579718 255239255255 0235\n"
        L"Ю12 00 56022312 255239255255 0231\n"
        L"Ю12 00 56832199 255239255255 0234\n"
        L"Ю12 00 56219306 255255255255 \n"
        L"Ю12 00 55179162 255239255255 0238\n"
        L"Ю12 00 53175378 187239255255 0693000241\n"
        L"Ю12 00 56081607 191239255255 0700234\n"
        L"Ю12 00 59398263 191239255255 0690240\n"
        L"Ю12 00 55976799 191239255255 0700233\n"
        L"Ю12 00 60646478 187237255255 06940002440600\n"
        L"Ю12 00 60067683 191239255255 0700243\n"
        L"Ю12 00 60997368 187239255255 0753000246\n"
        L"Ю12 00 62599642 251239255255 4000248\n"
        L"Ю12 00 62484134 255239255255 0247\n"
        L"Ю12 00 60379450 187239255255 0703000242\n"
        L"Ю12 00 62782123 191239255255 0750249\n"
        L"Ю12 00 62710439 255255255255 \n"
        L"Ю12 00 63285878 255239255255 0248:)]]><feat_texts typeinfo='Т' />"
        L"</AsoupEvent>"
        );

    {
    CPPUNIT_ASSERT( hl.path_count() == 2 );
    auto pathList = al.UT_GetEvents();
    CPPUNIT_ASSERT( pathList.size() == 2 );
    auto asoup = *pathList.cbegin();
    CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
    asoup = *pathList.crbegin();
    CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200826T105059Z'>"
        L"<PrePoint layer='asoup' Bdg='ASOUP 1042[11260]' name='Arrival' create_time='20200826T084400Z' index='0970-091-8014' num='2956' length='50' weight='4756' net_weight='3557' through='Y' waynum='3' parknum='1'><![CDATA[(:1042 909/000+11260 2956 0970 091 8014 01 11310 26 08 11 44 01/03 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 050 04756 03557 200 62916721 63285878 000 000 00 050 000.60 050 000 000\n"
        L"Ю3 530 00010601 1 09 18 0000 30016 ДУДAPEB     \n"
        L"Ю3 530 00010602 9\n"
        L"Ю4 07792 2 80 050 000.60 050 000 \n"
        L"Ю12 00 62916721 1 075 80140 14206 5341 300 00 00 00 00 00 0245 20 60 0600 04 100 80140 00000 07792 80 09700 07 09740 8517 020 1 1280 097495 132 000 00000000\n"
        L"Ю12 00 62753207 255255255255 \n"
        L"Ю12 00 63453740 255255255255 \n"
        L"Ю12 00 63903819 255255255255 \n"
        L"Ю12 00 62514856 255239255255 0244\n"
        L"Ю12 00 63292759 255239255255 0245\n"
        L"Ю12 00 56963093 191237255255 07002355600\n"
        L"Ю12 00 54017546 191239255255 0690240\n"
        L"Ю12 00 53546594 255239255255 0238\n"
        L"Ю12 00 56160252 251255255255 500\n"
        L"Ю12 00 52804044 191239255239 07002351250\n"
        L"Ю12 00 54700224 191239255239 06902401280\n"
        L"Ю12 00 63256556 187237255255 07530002450600\n"
        L"Ю12 00 54002928 191237255255 07002435600\n"
        L"Ю12 00 61004479 255237255255 02320600\n"
        L"Ю12 00 60777729 255239255255 0235\n"
        L"Ю12 00 55816318 255253255255 5600\n"
        L"Ю12 00 60145778 187237255255 06950002390600\n"
        L"Ю12 00 56044159 255237255255 02385600\n"
        L"Ю12 00 61826954 191253255255 0700600\n"
        L"Ю12 00 59391334 255237255255 02355600\n"
        L"Ю12 00 55759237 191239255255 0690240\n"
        L"Ю12 00 52220597 255239255239 02361250\n"
        L"Ю12 00 61195269 191237255239 070024006001280\n"
        L"Ю12 00 53555140 191253255255 0695600\n"
        L"Ю12 00 56220155 191239255255 0700233\n"
        L"Ю12 00 54755418 255239255255 0240\n"
        L"Ю12 00 56255045 255239255255 0235\n"
        L"Ю12 00 56176563 255239255255 0234\n"
        L"Ю12 00 60270949 191237255255 07402460600\n"
        L"Ю12 00 55977847 191237255255 07002405600\n"
        L"Ю12 00 56130578 255239255255 0233\n"
        L"Ю12 00 55579718 255239255255 0235\n"
        L"Ю12 00 56022312 255239255255 0231\n"
        L"Ю12 00 56832199 255239255255 0234\n"
        L"Ю12 00 56219306 255255255255 \n"
        L"Ю12 00 55179162 255239255255 0238\n"
        L"Ю12 00 53175378 187239255255 0693000241\n"
        L"Ю12 00 56081607 191239255255 0700234\n"
        L"Ю12 00 59398263 191239255255 0690240\n"
        L"Ю12 00 55976799 191239255255 0700233\n"
        L"Ю12 00 60646478 187237255255 06940002440600\n"
        L"Ю12 00 60067683 191239255255 0700243\n"
        L"Ю12 00 60997368 187239255255 0753000246\n"
        L"Ю12 00 62599642 251239255255 4000248\n"
        L"Ю12 00 62484134 255239255255 0247\n"
        L"Ю12 00 60379450 187239255255 0703000242\n"
        L"Ю12 00 62782123 191239255255 0750249\n"
        L"Ю12 00 62710439 255255255255 \n"
        L"Ю12 00 63285878 255239255255 0248:)]]><feat_texts typeinfo='Т' />"
        L"</PrePoint>"
        L"<Action code='Attach' District='11420-11310-11260-11290' DistrictRight='Y' FixBound='20190824T165000Z' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP 1042[11260]' name='Departure' create_time='20200826T093000Z' index='0970-091-8014' num='2956' length='50' weight='4756' net_weight='3557' through='Y' waynum='3' parknum='1'><![CDATA[(:1042 909/000+11260 2956 0970 091 8014 03 07792 26 08 12 30 01/03 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 050 04756 03557 200 62916721 63285878 000 000 00 050 000.60 050 000 000\n"
        L"Ю3 530 00010601 1 09 18 0000 30016 ДУДAPEB     \n"
        L"Ю3 530 00010602 9\n"
        L"Ю4 07792 2 80 050 000.60 050 000 \n"
        L"Ю12 00 62916721 1 075 80140 14206 5341 300 00 00 00 00 00 0245 20 60 0600 04 100 80140 00000 07792 80 09700 07 09740 8517 020 1 1280 097495 132 000 00000000\n"
        L"Ю12 00 62753207 255255255255 \n"
        L"Ю12 00 63453740 255255255255 \n"
        L"Ю12 00 63903819 255255255255 \n"
        L"Ю12 00 62514856 255239255255 0244\n"
        L"Ю12 00 63292759 255239255255 0245\n"
        L"Ю12 00 56963093 191237255255 07002355600\n"
        L"Ю12 00 54017546 191239255255 0690240\n"
        L"Ю12 00 53546594 255239255255 0238\n"
        L"Ю12 00 56160252 251255255255 500\n"
        L"Ю12 00 52804044 191239255239 07002351250\n"
        L"Ю12 00 54700224 191239255239 06902401280\n"
        L"Ю12 00 63256556 187237255255 07530002450600\n"
        L"Ю12 00 54002928 191237255255 07002435600\n"
        L"Ю12 00 61004479 255237255255 02320600\n"
        L"Ю12 00 60777729 255239255255 0235\n"
        L"Ю12 00 55816318 255253255255 5600\n"
        L"Ю12 00 60145778 187237255255 06950002390600\n"
        L"Ю12 00 56044159 255237255255 02385600\n"
        L"Ю12 00 61826954 191253255255 0700600\n"
        L"Ю12 00 59391334 255237255255 02355600\n"
        L"Ю12 00 55759237 191239255255 0690240\n"
        L"Ю12 00 52220597 255239255239 02361250\n"
        L"Ю12 00 61195269 191237255239 070024006001280\n"
        L"Ю12 00 53555140 191253255255 0695600\n"
        L"Ю12 00 56220155 191239255255 0700233\n"
        L"Ю12 00 54755418 255239255255 0240\n"
        L"Ю12 00 56255045 255239255255 0235\n"
        L"Ю12 00 56176563 255239255255 0234\n"
        L"Ю12 00 60270949 191237255255 07402460600\n"
        L"Ю12 00 55977847 191237255255 07002405600\n"
        L"Ю12 00 56130578 255239255255 0233\n"
        L"Ю12 00 55579718 255239255255 0235\n"
        L"Ю12 00 56022312 255239255255 0231\n"
        L"Ю12 00 56832199 255239255255 0234\n"
        L"Ю12 00 56219306 255255255255 \n"
        L"Ю12 00 55179162 255239255255 0238\n"
        L"Ю12 00 53175378 187239255255 0693000241\n"
        L"Ю12 00 56081607 191239255255 0700234\n"
        L"Ю12 00 59398263 191239255255 0690240\n"
        L"Ю12 00 55976799 191239255255 0700233\n"
        L"Ю12 00 60646478 187237255255 06940002440600\n"
        L"Ю12 00 60067683 191239255255 0700243\n"
        L"Ю12 00 60997368 187239255255 0753000246\n"
        L"Ю12 00 62599642 251239255255 4000248\n"
        L"Ю12 00 62484134 255239255255 0247\n"
        L"Ю12 00 60379450 187239255255 0703000242\n"
        L"Ю12 00 62782123 191239255255 0750249\n"
        L"Ю12 00 62710439 255255255255 \n"
        L"Ю12 00 63285878 255239255255 0248:)]]><feat_texts typeinfo='Т' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='11260' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach_AsoupToAsoup att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, asoupQueue, context, bill.getHeadAsoupThrows(), bill.getTailAsoupThrows(), false );
    att.Action();
    collectUndo();
    {
        CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
        CPPUNIT_ASSERT( happen_layer->exist_path_size(38) );
        CPPUNIT_ASSERT( al.path_count() == 2 );
        auto pathList = al.UT_GetEvents();
        CPPUNIT_ASSERT( pathList.size() == 2 );
        auto asoup = *pathList.cbegin();
        CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
        asoup = *pathList.crbegin();
        CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
    }
}

void TC_Hem_aeAttach::ContinuePathAndRelinkAsoup()
{
    happen_layer->createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200830T091027Z' name='Form' Bdg='2UP_PK[09042:09490]' waynum='2' index='0001-051-0950' num='4869' reslocoutbnd='Y'>"
        L"<rwcoord picketing1_val='0~900' picketing1_comm='Рига-Кр' />"
        L"<rwcoord picketing1_val='1~300' picketing1_comm='Рига-Кр' />"
        L"<Locomotive Series='ЧМЭ3' NumLoc='3757' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ZUBKO' Tim_Beg='2020-08-30 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200830T091102Z' name='Transition' Bdg='203SP+[09042]'>"
        L"<rwcoord picketing1_val='0~400' picketing1_comm='Рига-Кр' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200830T091548Z' name='Arrival' Bdg='11C[09500]' waynum='11' parknum='1' />"
        L"<SpotEvent create_time='20200830T091548Z' name='Disform' Bdg='11C[09500]' waynum='11' parknum='1' />"
        L"</HemPath>"
        );

    asoup_layer->createPath(
        L"<AsoupEvent create_time='20200830T085500Z' name='Departure' Bdg='ASOUP 1042_2p16w[09500]' index='0001-051-0950' num='4869' reslocoutbnd='Y' waynum='16' parknum='2' dirTo='09490' adjTo='09042' linkBadge='2UP_PK[09042:09490]' linkTime='20200830T091027Z' linkCode='Form'>"
        L"<![CDATA[(:1042 909/000+09500 4869 0001 051 0950 03 09480 30 08 11 55 02/16 0 0/00 00 0\n"
        L"Ю3 557 00037570 1 08 00 0000 00000 ZUBKO       :)]]><Locomotive Series='ЧМЭ3' NumLoc='3757' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ZUBKO' Tim_Beg='2020-08-30 08:00' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    asoup_layer->createPath(
        L"<AsoupEvent create_time='20200830T091500Z' name='Arrival' Bdg='ASOUP 1042_1p5w[09500]' index='0001-051-0950' num='4869' reslocoutbnd='Y' waynum='5' parknum='1' dirFrom='09580' adjFrom='09580' linkBadge='11C[09500]' linkTime='20200830T091548Z' linkCode='Arrival'><![CDATA[(:1042 909/000+09500 4869 0001 051 0950 01 09580 30 08 12 15 03/05 1 0/00 00 0:)]]></AsoupEvent>"
        );

    auto& al = *asoup_layer;
    auto& hl = *happen_layer;
    {
        CPPUNIT_ASSERT( hl.path_count() == 1 );
        CPPUNIT_ASSERT( happen_layer->exist_path_size(4) );
        CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "2UP_PK[09042:09490] 203SP+[09042] 11C[09500] 11C[09500]" ) );
        auto pathList = al.UT_GetEvents();
        CPPUNIT_ASSERT( pathList.size() == 2 );
        Hem::EventAddress<SpotEvent> servedAddress = al.GetServedMessage(pathList.front());
        CPPUNIT_ASSERT (!servedAddress.empty() && hl.GetWriteAccess()->getByAddress(servedAddress) );
        servedAddress = al.GetServedMessage(pathList.back());
        CPPUNIT_ASSERT (!servedAddress.empty() && hl.GetWriteAccess()->getByAddress(servedAddress) );
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200830T170622Z'>"
        L"<PrePoint name='Arrival' Bdg='?[09490]' create_time='20200830T090800Z' />"
        L"<Action code='PushFront' District='09000-09640' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='2UP_PK[09042:09490]' create_time='20200830T091027Z' index='0001-051-0950' num='4869' reslocoutbnd='Y' waynum='2'>"
        L"<rwcoord picketing1_val='1~100' picketing1_comm='Рига-Кр' />"
        L"<Locomotive Series='ЧМЭ3' NumLoc='3757' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ZUBKO' Tim_Beg='2020-08-30 08:00' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09490' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );
         topology->Load(
             L"*9490,9500 {9490, 9042, 9500}\n"
             );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();
    {
        CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
        CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
        CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "?[09490] ?[09490] 203SP+[09042] 11C[09500] 11C[09500]" ) );
        CPPUNIT_ASSERT( al.path_count() == 2 );
        auto pathList = al.UT_GetEvents();
        CPPUNIT_ASSERT( al.IsServed( pathList.front() ) );
        Hem::EventAddress<SpotEvent> servedAddress = al.GetServedMessage(pathList.front());
        CPPUNIT_ASSERT (!servedAddress.empty() && hl.GetWriteAccess()->getByAddress(servedAddress) );
        servedAddress = al.GetServedMessage(pathList.back());
        CPPUNIT_ASSERT (!servedAddress.empty() && hl.GetWriteAccess()->getByAddress(servedAddress) );
    }
}

void TC_Hem_aeAttach::AttachHapenToOwnLaterAsoup()
{
    auto& hl = *happen_layer;
    auto& al = *asoup_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200829T105600Z' name='ExplicitForm' Bdg='ASOUP 1042[09340]' waynum='3' parknum='1' index='0934-046-0933' num='3536' length='7' weight='168' outbnd='Y'>"
        L"<Locomotive Series='ЧМЭ3М' NumLoc='5964' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ABAKUMOVS' Tim_Beg='2020-08-29 05:50' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200829T114800Z' name='Departure' Bdg='ASOUP 1042_1p3w[09340]' waynum='3' parknum='1'>"
        L"<Pickup AddCars='0/7' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200829T122700Z' name='Arrival' Bdg='?p1w1[09330]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200829T122700Z' name='Death' Bdg='?p1w1[09330]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    al.createPath(
        L"<AsoupEvent create_time='20200829T123700Z' name='Arrival' Bdg='ASOUP 1042_1p2w[09330]' index='0934-046-0933' num='3536' length='7' weight='168' outbnd='Y' waynum='2' parknum='1' dirFrom='09860' adjFrom='09340' linkBadge='?p1w1[09330]' linkTime='20200829T122700Z' linkCode='Arrival'><![CDATA[(:1042 909/000+09330 3536 0934 046 0933 01 09860 29 08 15 37 01/02 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 007 00168 00000 028 60281508 61797866 000 000 00 000 007.60 000 007 000\n"
        L"Ю4 11290 2 24 000 001.60 000 001 \n"
        L"Ю4 11290 2 83 000 005.60 000 005 \n"
        L"Ю4 16169 2 13 000 001.60 000 001 \n"
        L"Ю12 00 60281508 1 000 86210 43619 5860 300 00 00 00 00 00 0235 12 60 0600 04 100 09330 09180 11290 83 09340 17 00000 0000 020 1 1280 09349  128 000 00000000\n"
        L"Ю12 00 52956315 195237247255 273604361969870000241560024\n"
        L"Ю12 00 62150172 195237247255 862104361958603000242060083\n"
        L"Ю12 00 61580122 255239255255 0238\n"
        L"Ю12 00 52791886 195237231239 140404361963020000241560016169131020\n"
        L"Ю12 00 64213325 195237231239 862104361958603000238060011290831280\n"
        L"Ю12 00 61797866 255255255255 :)]]></AsoupEvent>"
        );

    {
        CPPUNIT_ASSERT( hl.path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size(4) );
        CPPUNIT_ASSERT( hl.exist_series("name", "  ExplicitForm        Departure             Arrival            Death" ) );
        CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[09340] ASOUP 1042_1p3w[09340]  ?p1w1[09330]    ?p1w1[09330]" ) );

        auto pathList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathList.size() == 1 );
        auto asoup = *pathList.cbegin();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
    }


    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200829T123843Z'>"
        L"<PrePoint name='Death' Bdg='?p1w1[09330]' create_time='20200829T122700Z' waynum='1' parknum='1' />"
        L"<Action code='Attach' District='09180-09860' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP 1042_1p2w[09330]' name='Arrival' create_time='20200829T123700Z' index='0934-046-0933' num='3536' length='7' weight='168' outbnd='Y' waynum='2' parknum='1'><![CDATA[(:1042 909/000+09330 3536 0934 046 0933 01 09860 29 08 15 37 01/02 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 007 00168 00000 028 60281508 61797866 000 000 00 000 007.60 000 007 000\n"
        L"Ю4 11290 2 24 000 001.60 000 001 \n"
        L"Ю4 11290 2 83 000 005.60 000 005 \n"
        L"Ю4 16169 2 13 000 001.60 000 001 \n"
        L"Ю12 00 60281508 1 000 86210 43619 5860 300 00 00 00 00 00 0235 12 60 0600 04 100 09330 09180 11290 83 09340 17 00000 0000 020 1 1280 09349  128 000 00000000\n"
        L"Ю12 00 52956315 195237247255 273604361969870000241560024\n"
        L"Ю12 00 62150172 195237247255 862104361958603000242060083\n"
        L"Ю12 00 61580122 255239255255 0238\n"
        L"Ю12 00 52791886 195237231239 140404361963020000241560016169131020\n"
        L"Ю12 00 64213325 195237231239 862104361958603000238060011290831280\n"
        L"Ю12 00 61797866 255255255255 :)]]></PostPoint>"
        L"<EsrList>"
        L"<Station esr='09330' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	AsoupQueueMgr asoupQMgr;
    aeAttach_HappenToAsoup att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, asoupQMgr, context, bill.getHeadSpotThrows(), bill.getTailAsoupThrows(), false );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "ExplicitForm           Departure           Arrival        Departure          Arrival                Death" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[09340] ASOUP 1042_1p3w[09340]  ?p1w1[09330]    ?p1w1[09330]  ASOUP 1042_1p2w[09330]  ASOUP 1042_1p2w[09330]") );
    auto pathList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathList.size() == 1 );
    auto asoup = *pathList.cbegin();
    CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
}

void TC_Hem_aeAttach::AttachHapenToFreeAsoup()
{
    auto& hl = *happen_layer;
    auto& al = *asoup_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200829T105600Z' name='ExplicitForm' Bdg='ASOUP 1042[09340]' waynum='3' parknum='1' index='0934-046-0933' num='3536' length='7' weight='168' outbnd='Y'>"
        L"<Locomotive Series='ЧМЭ3М' NumLoc='5964' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ABAKUMOVS' Tim_Beg='2020-08-29 05:50' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200829T114800Z' name='Departure' Bdg='ASOUP 1042_1p3w[09340]' waynum='3' parknum='1'>"
        L"<Pickup AddCars='0/7' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200829T122700Z' name='Arrival' Bdg='?p1w1[09330]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200829T122700Z' name='Death' Bdg='?p1w1[09330]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    al.createPath(
        L"<AsoupEvent create_time='20200829T123700Z' name='Arrival' Bdg='ASOUP 1042_1p2w[09330]' index='0934-046-0933' num='3536' length='7' weight='168' outbnd='Y' waynum='2' parknum='1' dirFrom='09860' adjFrom='09340' ><![CDATA[(:1042 909/000+09330 3536 0934 046 0933 01 09860 29 08 15 37 01/02 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 007 00168 00000 028 60281508 61797866 000 000 00 000 007.60 000 007 000\n"
        L"Ю4 11290 2 24 000 001.60 000 001 \n"
        L"Ю4 11290 2 83 000 005.60 000 005 \n"
        L"Ю4 16169 2 13 000 001.60 000 001 \n"
        L"Ю12 00 60281508 1 000 86210 43619 5860 300 00 00 00 00 00 0235 12 60 0600 04 100 09330 09180 11290 83 09340 17 00000 0000 020 1 1280 09349  128 000 00000000\n"
        L"Ю12 00 52956315 195237247255 273604361969870000241560024\n"
        L"Ю12 00 62150172 195237247255 862104361958603000242060083\n"
        L"Ю12 00 61580122 255239255255 0238\n"
        L"Ю12 00 52791886 195237231239 140404361963020000241560016169131020\n"
        L"Ю12 00 64213325 195237231239 862104361958603000238060011290831280\n"
        L"Ю12 00 61797866 255255255255 :)]]></AsoupEvent>"
        );

    {
        CPPUNIT_ASSERT( hl.path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size(4) );
        CPPUNIT_ASSERT( hl.exist_series("name", "  ExplicitForm        Departure             Arrival            Death" ) );
        CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[09340] ASOUP 1042_1p3w[09340]  ?p1w1[09330]    ?p1w1[09330]" ) );

        auto pathList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathList.size() == 1 );
        auto asoup = *pathList.cbegin();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }


    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200829T123843Z'>"
        L"<PrePoint name='Death' Bdg='?p1w1[09330]' create_time='20200829T122700Z' waynum='1' parknum='1' />"
        L"<Action code='Attach' District='09180-09860' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP 1042_1p2w[09330]' name='Arrival' create_time='20200829T123700Z' index='0934-046-0933' num='3536' length='7' weight='168' outbnd='Y' waynum='2' parknum='1'><![CDATA[(:1042 909/000+09330 3536 0934 046 0933 01 09860 29 08 15 37 01/02 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 007 00168 00000 028 60281508 61797866 000 000 00 000 007.60 000 007 000\n"
        L"Ю4 11290 2 24 000 001.60 000 001 \n"
        L"Ю4 11290 2 83 000 005.60 000 005 \n"
        L"Ю4 16169 2 13 000 001.60 000 001 \n"
        L"Ю12 00 60281508 1 000 86210 43619 5860 300 00 00 00 00 00 0235 12 60 0600 04 100 09330 09180 11290 83 09340 17 00000 0000 020 1 1280 09349  128 000 00000000\n"
        L"Ю12 00 52956315 195237247255 273604361969870000241560024\n"
        L"Ю12 00 62150172 195237247255 862104361958603000242060083\n"
        L"Ю12 00 61580122 255239255255 0238\n"
        L"Ю12 00 52791886 195237231239 140404361963020000241560016169131020\n"
        L"Ю12 00 64213325 195237231239 862104361958603000238060011290831280\n"
        L"Ю12 00 61797866 255255255255 :)]]></PostPoint>"
        L"<EsrList>"
        L"<Station esr='09330' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	AsoupQueueMgr asoupQMgr;
    aeAttach_HappenToAsoup att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, asoupQMgr, context, bill.getHeadSpotThrows(), bill.getTailAsoupThrows(), false );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "ExplicitForm           Departure           Arrival        Departure          Arrival                Death" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[09340] ASOUP 1042_1p3w[09340]  ?p1w1[09330]    ?p1w1[09330]  ASOUP 1042_1p2w[09330]  ASOUP 1042_1p2w[09330]") );
    auto pathList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathList.size() == 1 );
    auto asoup = *pathList.cbegin();
    CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
}

void TC_Hem_aeAttach::AttachAsoupToAsoup_2()
{
    auto& al = *asoup_layer;
    auto& hl = *happen_layer;

    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200901T224400Z' name='ExplicitForm' Bdg='ASOUP 1042[11420]' waynum='14' parknum='1' index='1142-084-1171' num='3556' length='13' weight='269' outbnd='Y'>"
        L"<Locomotive Series='ЧМЭ3' NumLoc='3950' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='VIKSNA' Tim_Beg='2020-09-02 06:21' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200902T043507Z' name='Departure' Bdg='14C[11420]' waynum='14' parknum='1' />"
        L"<SpotEvent create_time='20200902T045808Z' name='Arrival' Bdg='5C[11760]' waynum='5' parknum='1' index='1142-084-1171' num='3556' length='13' weight='269' outbnd='Y'>"
        L"<Locomotive Series='ЧМЭ3' NumLoc='3950' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='VIKSNA' Tim_Beg='2020-09-02 06:21' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200902T061203Z' name='Departure' Bdg='5C[11760]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20200902T061321Z' name='Span_move' Bdg='NJ1P[11750:11760]' waynum='1'>"
        L"<rwcoord picketing1_val='3~' picketing1_comm='Плявиняс' />"
        L"<rwcoord picketing1_val='1~' picketing1_comm='Плявиняс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200902T061645Z' name='Death' Bdg='NJ1P[11750:11760]' waynum='1'>"
        L"<rwcoord picketing1_val='3~' picketing1_comm='Плявиняс' />"
        L"<rwcoord picketing1_val='1~' picketing1_comm='Плявиняс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    al.createPath(
        L"<AsoupEvent create_time='20200902T061200Z' name='Departure' Bdg='ASOUP 1042_1p5w[11760]' index='1142-084-1171' num='3556' length='13' weight='269' outbnd='Y' waynum='5' parknum='1' dirTo='11750' adjTo='11750' linkBadge='5C[11760]' linkTime='20200902T061203Z' linkCode='Departure'>"
        L"<![CDATA[(:1042 909/000+11760 3556 1142 084 1171 03 11750 02 09 09 12 01/05 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 013 00269 00000 048 95089173 95698452 000 000 00 000 012.90 000 012 000 95 000 012 000\n"
        L"Ю3 557 00039500 1 06 21 0000 00000 VIKSNA      \n"
        L"Ю4 00000 1 02 000 012.90 000 012 95 000 012 \n"
        L"Ю4 11710 1 02 000 012.90 000 012 95 000 012 \n"
        L"Ю12 00 95089173 1 000 11710 01200 8057 000 00 00 00 00 00 0220 13 95 0950 04 106 11710 11760 00000 02 11420 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
        L"Ю12 00 95408373 255255255255 \n"
        L"Ю12 00 95126975 255255255255 \n"
        L"Ю12 00 95088944 255255255255 \n"
        L"Ю12 00 95064242 255255255255 \n"
        L"Ю12 00 95487047 255255255255 \n"
        L"Ю12 00 95223871 255255255255 \n"
        L"Ю12 00 95128062 255255255255 \n"
        L"Ю12 00 95845822 255239255255 0236\n"
        L"Ю12 00 95043253 255239255255 0220\n"
        L"Ю12 00 95164141 255239255255 0232\n"
        L"Ю12 00 95698452 255239255255 0234:)]]><Locomotive Series='ЧМЭ3' NumLoc='3950' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='VIKSNA' Tim_Beg='2020-09-02 06:21' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    al.createPath(
        L"<AsoupEvent create_time='20200902T070900Z' name='Arrival' Bdg='ASOUP 1042_1p1w[11710]' index='1142-084-1171' num='3556' length='13' weight='269' outbnd='Y' waynum='1' parknum='1' dirFrom='11760' adjFrom='11750'>"
        L"<![CDATA[(:1042 909/000+11710 3556 1142 084 1171 01 11760 02 09 10 09 01/01 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 013 00269 00000 048 95089173 95698452 000 000 00 000 012.90 000 012 000 95 000 012 000\n"
        L"Ю4 00000 1 02 000 012.90 000 012 95 000 012 \n"
        L"Ю4 11710 1 02 000 012.90 000 012 95 000 012 \n"
        L"Ю12 00 95089173 1 000 11710 01200 8057 000 00 00 00 00 00 0220 13 95 0950 04 106 11710 11760 00000 02 11420 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
        L"Ю12 00 95408373 255255255255 \n"
        L"Ю12 00 95126975 255255255255 \n"
        L"Ю12 00 95088944 255255255255 \n"
        L"Ю12 00 95064242 255255255255 \n"
        L"Ю12 00 95487047 255255255255 \n"
        L"Ю12 00 95223871 255255255255 \n"
        L"Ю12 00 95128062 255255255255 \n"
        L"Ю12 00 95845822 255239255255 0236\n"
        L"Ю12 00 95043253 255239255255 0220\n"
        L"Ю12 00 95164141 255239255255 0232\n"
        L"Ю12 00 95698452 255239255255 0234:)]]></AsoupEvent>"
        );

    CPPUNIT_ASSERT( al.path_count() == 2 );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[11420] 14C[11420] 5C[11760]  5C[11760] NJ1P[11750:11760] NJ1P[11750:11760]") );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200902T092228Z'>"
        L"<PrePoint layer='asoup' Bdg='ASOUP 1042_1p5w[11760]' name='Departure' create_time='20200902T061200Z' index='1142-084-1171' num='3556' length='13' weight='269' outbnd='Y' waynum='5' parknum='1'>"
        L"<![CDATA[(:1042 909/000+11760 3556 1142 084 1171 03 11750 02 09 09 12 01/05 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 013 00269 00000 048 95089173 95698452 000 000 00 000 012.90 000 012 000 95 000 012 000\n"
        L"Ю3 557 00039500 1 06 21 0000 00000 VIKSNA      \n"
        L"Ю4 00000 1 02 000 012.90 000 012 95 000 012 \n"
        L"Ю4 11710 1 02 000 012.90 000 012 95 000 012 \n"
        L"Ю12 00 95089173 1 000 11710 01200 8057 000 00 00 00 00 00 0220 13 95 0950 04 106 11710 11760 00000 02 11420 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
        L"Ю12 00 95408373 255255255255 \n"
        L"Ю12 00 95126975 255255255255 \n"
        L"Ю12 00 95088944 255255255255 \n"
        L"Ю12 00 95064242 255255255255 \n"
        L"Ю12 00 95487047 255255255255 \n"
        L"Ю12 00 95223871 255255255255 \n"
        L"Ю12 00 95128062 255255255255 \n"
        L"Ю12 00 95845822 255239255255 0236\n"
        L"Ю12 00 95043253 255239255255 0220\n"
        L"Ю12 00 95164141 255239255255 0232\n"
        L"Ю12 00 95698452 255239255255 0234:)]]><Locomotive Series='ЧМЭ3' NumLoc='3950' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='VIKSNA' Tim_Beg='2020-09-02 06:21' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<Action code='Attach' District='11000-11042' DistrictRight='Y' />"
        L"<PostPoint layer='asoup' Bdg='ASOUP 1042_1p1w[11710]' name='Arrival' create_time='20200902T070900' index='1142-084-1171' num='3556' length='13' weight='269' outbnd='Y' waynum='1' parknum='1'>"
        L"<![CDATA[(:1042 909/000+11710 3556 1142 084 1171 01 11760 02 09 10 09 01/01 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 013 00269 00000 048 95089173 95698452 000 000 00 000 012.90 000 012 000 95 000 012 000\n"
        L"Ю4 00000 1 02 000 012.90 000 012 95 000 012 \n"
        L"Ю4 11710 1 02 000 012.90 000 012 95 000 012 \n"
        L"Ю12 00 95089173 1 000 11710 01200 8057 000 00 00 00 00 00 0220 13 95 0950 04 106 11710 11760 00000 02 11420 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
        L"Ю12 00 95408373 255255255255 \n"
        L"Ю12 00 95126975 255255255255 \n"
        L"Ю12 00 95088944 255255255255 \n"
        L"Ю12 00 95064242 255255255255 \n"
        L"Ю12 00 95487047 255255255255 \n"
        L"Ю12 00 95223871 255255255255 \n"
        L"Ю12 00 95128062 255255255255 \n"
        L"Ю12 00 95845822 255239255255 0236\n"
        L"Ю12 00 95043253 255239255255 0220\n"
        L"Ю12 00 95164141 255239255255 0232\n"
        L"Ю12 00 95698452 255239255255 0234:)]]></PostPoint>"
        L"<EsrList>"
        L"<Station esr='11710' />"
        L"<Station esr='11750' />"
        L"<Station esr='11760' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );
    topology->Load(
        L"*11710,11760 {11710,11750,11760}\n"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach_AsoupToAsoup att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, asoupQueue, context, bill.getHeadAsoupThrows(), bill.getTailAsoupThrows(), false );
    att.Action();
    collectUndo();
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );

    CPPUNIT_ASSERT( hl.exist_path_size(8) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[11420] 14C[11420] 5C[11760]  5C[11760] NJ1P[11750:11760] ?[11750] ASOUP 1042_1p1w[11710]  ASOUP 1042_1p1w[11710]") );
}

void TC_Hem_aeAttach::AttachAsoupDepartureToOwnLinkedPath_5849()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200909T102946Z' name='Form' Bdg='1N2IP[11042:16169]' waynum='1' index='1600-897-1100' num='D2831' length='57' weight='3875' net_weight='2654' through='Y'>"
        L"<rwcoord picketing1_val='461~750' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='465~' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='2ТЭ10УК' NumLoc='196' Depo='1317' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ЯHУШ' TabNum='2550' Tim_Beg='2020-09-09 09:27' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200909T103500Z' name='Arrival' Bdg='ASOUP 1042_1p3w[11042]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200909T110350Z' name='Departure' Bdg='3C[11042]' waynum='3' parknum='1' optCode='11041:11042' />"
        L"<SpotEvent create_time='20200909T111330Z' name='Transition' Bdg='1C[11041]' waynum='1' parknum='2' intervalSec='5' optCode='11041:11052' />"
        L"<SpotEvent create_time='20200909T111526Z' name='Transition' Bdg='1AC[11041]' waynum='1' parknum='1' intervalSec='4' optCode='11041:11052' />"
        L"<SpotEvent create_time='20200909T111656Z' name='Span_move' Bdg='SN5C[11041:11052]' waynum='1'>"
        L"<rwcoord picketing1_val='448~100' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='448~850' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );

    asoup_layer->createPath(
        L"<AsoupEvent create_time='20200909T101100Z' name='Departure' Bdg='ASOUP 1042[16169]' index='1600-897-1100' num='2831' length='57' weight='3875' net_weight='2654' through='Y' dirTo='11000' adjTo='11042' linkBadge='ASOUP 1042_1p3w[11042]' linkTime='20200909T103500Z' linkCode='Arrival'><![CDATA[(:1042 909/000+16169 2831 1600 897 1100 03 11000 09 09 13 11 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 03875 02654 180 62849203 53009742 000 000 00 045 000.60 012 000 000 70 001 000 000 90 032 000 000 95 013 000 000\n"
        L"Ю3 585 00001961 1 09 27 1317 02550 ЯHУШ        \n"
        L"Ю3 585 00001962 9\n"
        L"Ю4 08620 2 08 001 000.70 001 000 \n"
        L"Ю4 00000 1 01 028 000.60 012 000 90 016 000 95 013 000 \n"
        L"Ю4 00000 1 02 016 000.90 016 000 \n"
        L"Ю4 09740 1 01 002 000.60 002 000 \n"
        L"Ю4 09900 1 01 010 000.60 010 000 \n"
        L"Ю4 09590 1 01 011 000.90 011 000 95 011 000 \n"
        L"Ю4 09660 1 01 003 000.90 003 000 \n"
        L"Ю4 09870 1 01 002 000.90 002 000 95 002 000 \n"
        L"Ю4 11050 1 02 012 000.90 012 000 \n"
        L"Ю4 11160 1 02 004 000.90 004 000 \n"
        L"Ю12 00 62849203 1 075 09900 16117 6302 000 00 00 00 00 00 0245 30 60 0600 04 100 11000 09000 00000 01 16000 01 00000 0000 020 0 0000 0      132 000 00000000\n"
        L"Ю12 00 62161989 191239255255 0690239\n"
        L"Ю12 00 62808571 191239255255 0750249\n"
        L"Ю12 00 57381998 129225101191 0570835021103212606001026920705700086086200803026\n"
        L"Ю12 00 61069266 129225101191 0750990016117207200000024830600600100000000101020\n"
        L"Ю12 00 64190010 167239255255 0691611721250242\n"
        L"Ю12 00 64481302 191239255255 0750248\n"
        L"Ю12 00 64399561 255255255255 \n"
        L"Ю12 00 63164560 255255255255 \n"
        L"Ю12 00 95256855 197233121255 095900110030050402349509501060963034\n"
        L"Ю12 00 64341779 197233121255 099001611721250002476006001001600001\n"
        L"Ю12 00 62810650 255239255255 0249\n"
        L"Ю12 00 95849717 129233089191 05009870103047610080070216950950106098700986024021\n"
        L"Ю12 00 59591099 129233083191 04211050103078321305000330905917167110500216000026\n"
        L"Ю12 00 59592550 255255255255 \n"
        L"Ю12 00 59590513 255255255255 \n"
        L"Ю12 00 59433946 255255255255 \n"
        L"Ю12 00 59459396 155255219255 043111604051142011160\n"
        L"Ю12 00 59592998 255255255255 \n"
        L"Ю12 00 95021416 129233083191 05109870103047610000070226950950106098700109860021\n"
        L"Ю12 00 56790256 129233091191 030096601030785153070002659056101220900009640026\n"
        L"Ю12 00 56790124 255239255255 0266\n"
        L"Ю12 00 56790041 255239255255 0265\n"
        L"Ю12 00 63737282 131233123191 05009740091114694000023360060010016000021\n"
        L"Ю12 00 63363964 255239255255 0241\n"
        L"Ю12 00 95727665 129233121191 070095900110015024000202309509501060963034020\n"
        L"Ю12 00 95766374 255239255255 0232\n"
        L"Ю12 00 95726675 191239255255 0710230\n"
        L"Ю12 00 95767273 255239255255 0232\n"
        L"Ю12 00 95713111 255255255255 \n"
        L"Ю12 00 95718888 191239255255 0720230\n"
        L"Ю12 00 95720470 255239255255 0229\n"
        L"Ю12 00 95719704 255255255255 \n"
        L"Ю12 00 95725305 191239255255 0710230\n"
        L"Ю12 00 95725370 191255255255 070\n"
        L"Ю12 00 53009940 129233081191 0521105010304823936700033090591716711050021600024026\n"
        L"Ю12 00 59592329 255255255255 \n"
        L"Ю12 00 59592352 255255255255 \n"
        L"Ю12 00 53009882 255255255191 025\n"
        L"Ю12 00 91593160 191253255191 0530934024\n"
        L"Ю12 00 91599373 139239091191 056111601030740703001601142011160026\n"
        L"Ю12 00 98043714 255255255255 \n"
        L"Ю12 00 59590505 139237091255 0521105010304367033059171671105016000\n"
        L"Ю12 00 53009924 255255255255 \n"
        L"Ю12 00 53009742 255255255191 025:)]]><Locomotive Series='2ТЭ10УК' NumLoc='196' Depo='1317' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ЯHУШ' TabNum='2550' Tim_Beg='2020-09-09 09:27' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    {
        AsoupLayer::ReadAccessor container = asoup_layer->GetReadAccess();
        auto pathList = container->UT_GetEvents();
        CPPUNIT_ASSERT( pathList.size() == 1 );
        auto asoup = *pathList.cbegin();
        CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200909T111737Z'>"
        L"<PrePoint layer='asoup' Bdg='ASOUP 1042[16169]' name='Departure' create_time='20200909T101100Z' index='1600-897-1100' num='2831' length='57' weight='3875' net_weight='2654' through='Y'><![CDATA[(:1042 909/000+16169 2831 1600 897 1100 03 11000 09 09 13 11 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 03875 02654 180 62849203 53009742 000 000 00 045 000.60 012 000 000 70 001 000 000 90 032 000 000 95 013 000 000\n"
        L"Ю3 585 00001961 1 09 27 1317 02550 ЯHУШ        \n"
        L"Ю3 585 00001962 9\n"
        L"Ю4 08620 2 08 001 000.70 001 000 \n"
        L"Ю4 00000 1 01 028 000.60 012 000 90 016 000 95 013 000 \n"
        L"Ю4 00000 1 02 016 000.90 016 000 \n"
        L"Ю4 09740 1 01 002 000.60 002 000 \n"
        L"Ю4 09900 1 01 010 000.60 010 000 \n"
        L"Ю4 09590 1 01 011 000.90 011 000 95 011 000 \n"
        L"Ю4 09660 1 01 003 000.90 003 000 \n"
        L"Ю4 09870 1 01 002 000.90 002 000 95 002 000 \n"
        L"Ю4 11050 1 02 012 000.90 012 000 \n"
        L"Ю4 11160 1 02 004 000.90 004 000 \n"
        L"Ю12 00 62849203 1 075 09900 16117 6302 000 00 00 00 00 00 0245 30 60 0600 04 100 11000 09000 00000 01 16000 01 00000 0000 020 0 0000 0      132 000 00000000\n"
        L"Ю12 00 62161989 191239255255 0690239\n"
        L"Ю12 00 62808571 191239255255 0750249\n"
        L"Ю12 00 57381998 129225101191 0570835021103212606001026920705700086086200803026\n"
        L"Ю12 00 61069266 129225101191 0750990016117207200000024830600600100000000101020\n"
        L"Ю12 00 64190010 167239255255 0691611721250242\n"
        L"Ю12 00 64481302 191239255255 0750248\n"
        L"Ю12 00 64399561 255255255255 \n"
        L"Ю12 00 63164560 255255255255 \n"
        L"Ю12 00 95256855 197233121255 095900110030050402349509501060963034\n"
        L"Ю12 00 64341779 197233121255 099001611721250002476006001001600001\n"
        L"Ю12 00 62810650 255239255255 0249\n"
        L"Ю12 00 95849717 129233089191 05009870103047610080070216950950106098700986024021\n"
        L"Ю12 00 59591099 129233083191 04211050103078321305000330905917167110500216000026\n"
        L"Ю12 00 59592550 255255255255 \n"
        L"Ю12 00 59590513 255255255255 \n"
        L"Ю12 00 59433946 255255255255 \n"
        L"Ю12 00 59459396 155255219255 043111604051142011160\n"
        L"Ю12 00 59592998 255255255255 \n"
        L"Ю12 00 95021416 129233083191 05109870103047610000070226950950106098700109860021\n"
        L"Ю12 00 56790256 129233091191 030096601030785153070002659056101220900009640026\n"
        L"Ю12 00 56790124 255239255255 0266\n"
        L"Ю12 00 56790041 255239255255 0265\n"
        L"Ю12 00 63737282 131233123191 05009740091114694000023360060010016000021\n"
        L"Ю12 00 63363964 255239255255 0241\n"
        L"Ю12 00 95727665 129233121191 070095900110015024000202309509501060963034020\n"
        L"Ю12 00 95766374 255239255255 0232\n"
        L"Ю12 00 95726675 191239255255 0710230\n"
        L"Ю12 00 95767273 255239255255 0232\n"
        L"Ю12 00 95713111 255255255255 \n"
        L"Ю12 00 95718888 191239255255 0720230\n"
        L"Ю12 00 95720470 255239255255 0229\n"
        L"Ю12 00 95719704 255255255255 \n"
        L"Ю12 00 95725305 191239255255 0710230\n"
        L"Ю12 00 95725370 191255255255 070\n"
        L"Ю12 00 53009940 129233081191 0521105010304823936700033090591716711050021600024026\n"
        L"Ю12 00 59592329 255255255255 \n"
        L"Ю12 00 59592352 255255255255 \n"
        L"Ю12 00 53009882 255255255191 025\n"
        L"Ю12 00 91593160 191253255191 0530934024\n"
        L"Ю12 00 91599373 139239091191 056111601030740703001601142011160026\n"
        L"Ю12 00 98043714 255255255255 \n"
        L"Ю12 00 59590505 139237091255 0521105010304367033059171671105016000\n"
        L"Ю12 00 53009924 255255255255 \n"
        L"Ю12 00 53009742 255255255191 025:)]]><Locomotive Series='2ТЭ10УК' NumLoc='196' Depo='1317' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ЯHУШ' TabNum='2550' Tim_Beg='2020-09-09 09:27' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<Action code='Attach' District='11000-11042' DistrictRight='Y' FixBound='20191212T180000Z' />"
        L"<PostPoint name='Form' Bdg='1N2IP[11042:16169]' create_time='20200909T102946Z' index='1600-897-1100' num='D2831' length='57' weight='3875' net_weight='2654' through='Y' waynum='1'>"
        L"<rwcoord picketing1_val='463~375' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='2ТЭ10УК' NumLoc='196' Depo='1317' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ЯHУШ' TabNum='2550' Tim_Beg='2020-09-09 09:27' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='16169' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    setTopologyPath(11042, 16169);

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	AsoupQueueMgr asoupQMgr;
    aeAttach_AsoupToHappen att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, asoupQMgr, context, bill.getHeadAsoupThrows(), bill.getTailSpotThrows(), false );
    att.Action();
    collectUndo();
    {
        CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size(6) );
        CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042[16169] ASOUP 1042_1p3w[11042]  3C[11042] 1C[11041] 1AC[11041] SN5C[11041:11052]") );
    }

    {
        CPPUNIT_ASSERT( implementUndo() );
        CPPUNIT_ASSERT( hl.path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size(6) );
        CPPUNIT_ASSERT( hl.exist_series("Bdg",  "1N2IP[11042:16169] ASOUP 1042_1p3w[11042]  3C[11042] 1C[11041] 1AC[11041] SN5C[11041:11052]") );
    }
}

void TC_Hem_aeAttach::AttachHappenToAsoup()
{
    auto& hl = *happen_layer;

    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200919T111647Z' name='Form' Bdg='34-40SP:40+[09180]' index='0986-050-1142' num='3026' length='56' weight='1170' divisional='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
        L"<Crew />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200919T114939Z' name='Arrival' Bdg='7C[09180]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20200919T115228Z' name='Departure' Bdg='7C[09180]' waynum='7' parknum='1' index='0986-050-1142' num='3026' length='56' weight='1170' divisional='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
        L"<Crew EngineDriver='KLIMASEVSKIS' Tim_Beg='2020-09-19 11:39' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200919T115942Z' name='Transition' Bdg='3AC[09181]' waynum='3' parknum='1' intervalSec='35' optCode='09181:09251' index='0986-050-1142' num='3026' length='56' weight='1170' divisional='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    auto pathList = happen_layer->GetReadAccess()->getAllPathes();
    CPPUNIT_ASSERT( pathList.size() == 1 );
    auto spot = (*pathList.cbegin())->GetFirstEvent();
    CPPUNIT_ASSERT( spot );

    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200919T083000Z' name='Transition' Bdg='?p1w2[09290]' waynum='2' parknum='1' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y'>"
        L"<feat_texts typeinfo='Д' />"
        L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
        L"<Crew EngineDriver='M-MAKUSEVS' Tim_Beg='2020-09-19 07:02' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200919T084600Z' name='Transition' Bdg='?p1w2[09280]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200919T090232Z' name='Entra_stopping' Bdg='1GPP[09180:09280]' waynum='2' intervalSec='121' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y'>"
        L"<rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"<feat_texts typeinfo='Д' />"
        L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
        L"<Crew />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200919T090956Z' name='Arrival' Bdg='7C[09180]' waynum='7' parknum='1' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200919T100157Z' name='Departure' Bdg='7C[09180]' waynum='7' parknum='1' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y'>"
        L"<feat_texts typeinfo='Д' />"
        L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
        L"<Crew EngineDriver='KLIMASEVSKIS' Tim_Beg='2020-09-19 11:39' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200919T111245Z' name='Disform' Bdg='31-33SP:31+,33+[09180]' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 2 );

    asoup_layer->createPath(
        L"<AsoupEvent create_time='20200919T083000Z' name='Transition' Bdg='ASOUP 1042[09290]' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y' dirFrom='09860' dirTo='09180' adjFrom='09303' adjTo='09280' linkBadge='?p1w2[09290]' linkTime='20200919T083000Z' linkCode='Transition'><![CDATA[(:1042 909/000+09290 3026 0986 050 1142 04 09860+09180 19 09 11 30 00/00 0 0/00 00 0\n"
        L"Ю2 0 00  Д 00 00 0000 0 0 057 01192 00000 212 95696928 95173423 000 000 00 000 053.90 000 053 000 95 000 053 000\n"
        L"Ю3 583 00001101 1 07 02 0000 00000 M-MAKUSEVS  \n"
        L"Ю3 583 00001102 9\n"
        L"Ю4 00000 1 02 000 053.90 000 053 95 000 053 \n"
        L"Ю4 11420 1 02 000 053.90 000 053 95 000 053 \n"
        L"Ю12 00 95696928 1 000 11420 01400 5552 000 00 00 00 00 00 0236 13 95 0950 04 106 11420 11420 00000 02 09860 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
        L"Ю12 00 95872248 255239255255 0232\n"
        L"Ю12 00 95029583 255239255255 0231\n"
        L"Ю12 00 95336905 255239255255 0233\n"
        L"Ю12 00 95040002 255239255255 0220\n"
        L"Ю12 00 95128609 255255255255 \n"
        L"Ю12 00 95486940 255255255255 \n"
        L"Ю12 00 95044285 255255255255 \n"
        L"Ю12 00 95657532 255239255255 0214\n"
        L"Ю12 00 95693834 255239255255 0235\n"
        L"Ю12 00 95998985 255239255255 0232\n"
        L"Ю12 00 95042271 255239255255 0220\n"
        L"Ю12 00 95043766 255255255255 \n"
        L"Ю12 00 95684809 255255255255 \n"
        L"Ю12 00 95046520 255255255255 \n"
        L"Ю12 00 95037461 255255255255 \n"
        L"Ю12 00 95169355 255239255255 0233\n"
        L"Ю12 00 95070561 255239255255 0220\n"
        L"Ю12 00 95573911 255255255255 \n"
        L"Ю12 00 95685046 255255255255 \n"
        L"Ю12 00 95806436 255255255255 \n"
        L"Ю12 00 95486809 255255255255 \n"
        L"Ю12 00 95684668 255255255255 \n"
        L"Ю12 00 95012688 255239255255 0234\n"
        L"Ю12 00 95173118 255239255255 0233\n"
        L"Ю12 00 95070678 255239255255 0220\n"
        L"Ю12 00 95173357 255239255255 0232\n"
        L"Ю12 00 95693784 231239255255 0120080570236\n"
        L"Ю12 00 95026472 255239255255 0233\n"
        L"Ю12 00 95699971 231239255255 0140055520235\n"
        L"Ю12 00 95046512 255239255255 0220\n"
        L"Ю12 00 95846077 255239255255 0237\n"
        L"Ю12 00 95716684 255239255255 0220\n"
        L"Ю12 00 95716387 255255255255 \n"
        L"Ю12 00 95408357 255255255255 \n"
        L"Ю12 00 95806477 255239255255 0229\n"
        L"Ю12 00 95031415 255239255255 0220\n"
        L"Ю12 00 95126967 255255255255 \n"
        L"Ю12 00 95698833 239239255255 021080235\n"
        L"Ю12 00 95716718 255239255255 0220\n"
        L"Ю12 00 95846150 255239255255 0236\n"
        L"Ю12 00 95486551 255239255255 0220\n"
        L"Ю12 00 95173050 255239255255 0232\n"
        L"Ю12 00 95047874 239239255255 014000220\n"
        L"Ю12 00 95698718 255239255255 0236\n"
        L"Ю12 00 95459848 255239255255 0213\n"
        L"Ю12 00 95684742 255239255255 0220\n"
        L"Ю12 00 95806733 255239255255 0225\n"
        L"Ю12 00 95046124 255239255255 0220\n"
        L"Ю12 00 95652350 255239255255 0213\n"
        L"Ю12 00 95127106 255239255255 0220\n"
        L"Ю12 00 95716205 255255255255 \n"
        L"Ю12 00 95173423 255239255255 0232:)]]><feat_texts typeinfo='Д' />"
        L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
        L"<Crew EngineDriver='M-MAKUSEVS' Tim_Beg='2020-09-19 07:02' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    AsoupEventPtr asoup9280 = nullptr;
    {
    AsoupLayer::ReadAccessor container = asoup_layer->GetReadAccess();
    auto pathList = container->UT_GetEvents();
    CPPUNIT_ASSERT( pathList.size() == 1 );
    asoup9280 = *pathList.cbegin();
    CPPUNIT_ASSERT( asoup_layer->IsServed( asoup9280 ) );
    }

        asoup_layer->createPath(
            L"<AsoupEvent create_time='20200919T084600Z' name='Transition' Bdg='ASOUP 1042[09280]' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y' dirFrom='09860' dirTo='09180' adjFrom='09290' adjTo='09180' linkBadge='?p1w2[09280]' linkTime='20200919T084600Z' linkCode='Transition'><![CDATA[(:1042 909/000+09280 3026 0986 050 1142 04 09860+09180 19 09 11 46 00/00 0 0/00 00 0\n"
            L"Ю2 0 00  Д 00 00 0000 0 0 057 01192 00000 212 95696928 95173423 000 000 00 000 053.90 000 053 000 95 000 053 000\n"
            L"Ю3 583 00001101 1 07 02 0000 00000 M-MAKUSEVS  \n"
            L"Ю3 583 00001102 9\n"
            L"Ю4 00000 1 02 000 053.90 000 053 95 000 053 \n"
            L"Ю4 11420 1 02 000 053.90 000 053 95 000 053 \n"
            L"Ю12 00 95696928 1 000 11420 01400 5552 000 00 00 00 00 00 0236 13 95 0950 04 106 11420 11420 00000 02 09860 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
            L"Ю12 00 95872248 255239255255 0232\n"
            L"Ю12 00 95029583 255239255255 0231\n"
            L"Ю12 00 95336905 255239255255 0233\n"
            L"Ю12 00 95040002 255239255255 0220\n"
            L"Ю12 00 95128609 255255255255 \n"
            L"Ю12 00 95486940 255255255255 \n"
            L"Ю12 00 95044285 255255255255 \n"
            L"Ю12 00 95657532 255239255255 0214\n"
            L"Ю12 00 95693834 255239255255 0235\n"
            L"Ю12 00 95998985 255239255255 0232\n"
            L"Ю12 00 95042271 255239255255 0220\n"
            L"Ю12 00 95043766 255255255255 \n"
            L"Ю12 00 95684809 255255255255 \n"
            L"Ю12 00 95046520 255255255255 \n"
            L"Ю12 00 95037461 255255255255 \n"
            L"Ю12 00 95169355 255239255255 0233\n"
            L"Ю12 00 95070561 255239255255 0220\n"
            L"Ю12 00 95573911 255255255255 \n"
            L"Ю12 00 95685046 255255255255 \n"
            L"Ю12 00 95806436 255255255255 \n"
            L"Ю12 00 95486809 255255255255 \n"
            L"Ю12 00 95684668 255255255255 \n"
            L"Ю12 00 95012688 255239255255 0234\n"
            L"Ю12 00 95173118 255239255255 0233\n"
            L"Ю12 00 95070678 255239255255 0220\n"
            L"Ю12 00 95173357 255239255255 0232\n"
            L"Ю12 00 95693784 231239255255 0120080570236\n"
            L"Ю12 00 95026472 255239255255 0233\n"
            L"Ю12 00 95699971 231239255255 0140055520235\n"
            L"Ю12 00 95046512 255239255255 0220\n"
            L"Ю12 00 95846077 255239255255 0237\n"
            L"Ю12 00 95716684 255239255255 0220\n"
            L"Ю12 00 95716387 255255255255 \n"
            L"Ю12 00 95408357 255255255255 \n"
            L"Ю12 00 95806477 255239255255 0229\n"
            L"Ю12 00 95031415 255239255255 0220\n"
            L"Ю12 00 95126967 255255255255 \n"
            L"Ю12 00 95698833 239239255255 021080235\n"
            L"Ю12 00 95716718 255239255255 0220\n"
            L"Ю12 00 95846150 255239255255 0236\n"
            L"Ю12 00 95486551 255239255255 0220\n"
            L"Ю12 00 95173050 255239255255 0232\n"
            L"Ю12 00 95047874 239239255255 014000220\n"
            L"Ю12 00 95698718 255239255255 0236\n"
            L"Ю12 00 95459848 255239255255 0213\n"
            L"Ю12 00 95684742 255239255255 0220\n"
            L"Ю12 00 95806733 255239255255 0225\n"
            L"Ю12 00 95046124 255239255255 0220\n"
            L"Ю12 00 95652350 255239255255 0213\n"
            L"Ю12 00 95127106 255239255255 0220\n"
            L"Ю12 00 95716205 255255255255 \n"
            L"Ю12 00 95173423 255239255255 0232:)]]><feat_texts typeinfo='Д' />"
            L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
            L"<Crew EngineDriver='M-MAKUSEVS' Tim_Beg='2020-09-19 07:02' />"
            L"</Locomotive>"
            L"</AsoupEvent>"
            );

        asoup_layer->createPath(
            L"<AsoupEvent create_time='20200919T091000Z' name='Arrival' Bdg='ASOUP 1042_1p7w[09180]' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y' waynum='7' parknum='1' dirFrom='09860' adjFrom='09280' linkBadge='7C[09180]' linkTime='20200919T090956Z' linkCode='Arrival'><![CDATA[(:1042 909/000+09180 3026 0986 050 1142 01 09860 19 09 12 10 01/07 2 0/00 00 0\n"
            L"Ю2 0 00  Д 00 00 0000 0 0 057 01192 00000 212 95696928 95173423 000 000 00 000 053.90 000 053 000 95 000 053 000\n"
            L"Ю3 583 00001101 1 00 00 0000 00000 0           \n"
            L"Ю3 583 00001102 9\n"
            L"Ю4 00000 1 02 000 053.90 000 053 95 000 053 \n"
            L"Ю4 11420 1 02 000 053.90 000 053 95 000 053 \n"
            L"Ю12 00 95696928 1 000 11420 01400 5552 000 00 00 00 00 00 0236 13 95 0950 04 106 11420 11420 00000 02 09860 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
            L"Ю12 00 95872248 255239255255 0232\n"
            L"Ю12 00 95029583 255239255255 0231\n"
            L"Ю12 00 95336905 255239255255 0233\n"
            L"Ю12 00 95040002 255239255255 0220\n"
            L"Ю12 00 95128609 255255255255 \n"
            L"Ю12 00 95486940 255255255255 \n"
            L"Ю12 00 95044285 255255255255 \n"
            L"Ю12 00 95657532 255239255255 0214\n"
            L"Ю12 00 95693834 255239255255 0235\n"
            L"Ю12 00 95998985 255239255255 0232\n"
            L"Ю12 00 95042271 255239255255 0220\n"
            L"Ю12 00 95043766 255255255255 \n"
            L"Ю12 00 95684809 255255255255 \n"
            L"Ю12 00 95046520 255255255255 \n"
            L"Ю12 00 95037461 255255255255 \n"
            L"Ю12 00 95169355 255239255255 0233\n"
            L"Ю12 00 95070561 255239255255 0220\n"
            L"Ю12 00 95573911 255255255255 \n"
            L"Ю12 00 95685046 255255255255 \n"
            L"Ю12 00 95806436 255255255255 \n"
            L"Ю12 00 95486809 255255255255 \n"
            L"Ю12 00 95684668 255255255255 \n"
            L"Ю12 00 95012688 255239255255 0234\n"
            L"Ю12 00 95173118 255239255255 0233\n"
            L"Ю12 00 95070678 255239255255 0220\n"
            L"Ю12 00 95173357 255239255255 0232\n"
            L"Ю12 00 95693784 231239255255 0120080570236\n"
            L"Ю12 00 95026472 255239255255 0233\n"
            L"Ю12 00 95699971 231239255255 0140055520235\n"
            L"Ю12 00 95046512 255239255255 0220\n"
            L"Ю12 00 95846077 255239255255 0237\n"
            L"Ю12 00 95716684 255239255255 0220\n"
            L"Ю12 00 95716387 255255255255 \n"
            L"Ю12 00 95408357 255255255255 \n"
            L"Ю12 00 95806477 255239255255 0229\n"
            L"Ю12 00 95031415 255239255255 0220\n"
            L"Ю12 00 95126967 255255255255 \n"
            L"Ю12 00 95698833 239239255255 021080235\n"
            L"Ю12 00 95716718 255239255255 0220\n"
            L"Ю12 00 95846150 255239255255 0236\n"
            L"Ю12 00 95486551 255239255255 0220\n"
            L"Ю12 00 95173050 255239255255 0232\n"
            L"Ю12 00 95047874 239239255255 014000220\n"
            L"Ю12 00 95698718 255239255255 0236\n"
            L"Ю12 00 95459848 255239255255 0213\n"
            L"Ю12 00 95684742 255239255255 0220\n"
            L"Ю12 00 95806733 255239255255 0225\n"
            L"Ю12 00 95046124 255239255255 0220\n"
            L"Ю12 00 95652350 255239255255 0213\n"
            L"Ю12 00 95127106 255239255255 0220\n"
            L"Ю12 00 95716205 255255255255 \n"
            L"Ю12 00 95173423 255239255255 0232:)]]><feat_texts typeinfo='Д' />"
            L"<Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
            L"<Crew />"
            L"</Locomotive>"
            L"</AsoupEvent>"
            );

        asoup_layer->createPath(
            L"<AsoupEvent create_time='20200919T091000Z' name='Info_changing' Bdg='ASOUP 904[09180]' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y' linkBadge='7C[09180]' linkTime='20200919T090956Z' linkCode='Arrival'><![CDATA[(:904 0918 3026 0986 50 1142 1 19 09 12 10 057 01192 0 0000 0 0\n"
            L"01 95696928 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"02 95872248 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"03 95029583 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0231 0106 0705\n"
            L"04 95336905 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"05 95040002 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"06 95128609 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"07 95486940 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"08 95044285 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"09 95657532 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0214 0106 0700\n"
            L"10 95693834 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"11 95998985 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"12 95042271 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"13 95043766 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"14 95684809 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"15 95046520 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"16 95037461 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"17 95169355 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"18 95070561 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"19 95573911 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"20 95685046 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"21 95806436 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"22 95486809 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"23 95684668 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"24 95012688 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0234 0106 0705\n"
            L"25 95173118 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"26 95070678 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"27 95173357 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"28 95693784 0253 000 11421  01200 8057 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"29 95026472 0252 000 11421  01200 8057 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"30 95699971 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"31 95046512 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"32 95846077 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0237 0106 0702\n"
            L"33 95716684 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"35 95408357 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"36 95806477 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0229 0106 0700\n"
            L"37 95031415 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"38 95126967 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"39 95698833 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"40 95716718 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"41 95846150 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"42 95486551 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"43 95173050 0252 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"44 95047874 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"45 95698718 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"46 95459848 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0213 0106 0700\n"
            L"47 95684742 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"48 95806733 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0225 0106 0700\n"
            L"49 95046124 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"50 95652350 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0213 0106 0700\n"
            L"51 95127106 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"52 95716205 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"53 95173423 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705:)]]>"
            L"</AsoupEvent>"
            );

        asoup_layer->createPath(
            L"<AsoupEvent create_time='20200919T100000Z' name='Info_changing' Bdg='ASOUP 904[09180]' index='0986-050-1142' num='3026' length='57' weight='1192' divisional='Y' linkBadge='7C[09180]' linkTime='20200919T100157Z' linkCode='Departure'><![CDATA[(:904 0918 3026 0986 50 1142 1 19 09 13 00 057 01192 0 0000 0 0\n"
            L"01 95696928 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"02 95872248 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"03 95029583 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0231 0106 0705\n"
            L"04 95336905 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"05 95040002 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"06 95128609 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"07 95486940 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"08 95044285 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"09 95657532 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0214 0106 0700\n"
            L"10 95693834 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"11 95998985 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"12 95042271 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"13 95043766 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"14 95684809 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"15 95046520 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"16 95037461 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"17 95169355 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"18 95070561 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"19 95573911 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"20 95685046 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"21 95806436 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"22 95486809 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"23 95684668 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"24 95012688 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0234 0106 0705\n"
            L"25 95173118 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"26 95070678 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"27 95173357 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"28 95693784 0253 000 11421  01200 8057 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"29 95026472 0252 000 11421  01200 8057 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"30 95699971 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"31 95046512 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"32 95846077 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0237 0106 0702\n"
            L"33 95716684 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"34 95716387 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"35 95408357 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"36 95806477 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0229 0106 0700\n"
            L"37 95031415 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"38 95126967 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"39 95698833 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"40 95716718 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"41 95846150 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"42 95486551 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"43 95173050 0252 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"44 95047874 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"45 95698718 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"46 95459848 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0213 0106 0700\n"
            L"47 95684742 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"48 95806733 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0225 0106 0700\n"
            L"49 95046124 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"50 95652350 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0213 0106 0700\n"
            L"51 95127106 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"52 95716205 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"53 95173423 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705:)]]></AsoupEvent>"
            );

        asoup_layer->createPath(
            L"<AsoupEvent create_time='20200919T105500Z' name='Info_changing' Bdg='ASOUP 1042_1p7w[09180]' index='0986-050-1142' num='3026' length='56' weight='1170' divisional='Y' waynum='7' parknum='1' linkBadge='34-40SP:40+[09180]' linkTime='20200919T111647Z' linkCode='Form'><![CDATA[(:1042 909/000+09180 3026 0986 050 1142 12 00000 19 09 13 55 01/07 0 0/00 00 0\n"
            L"Ю2 0 00 00 00 00 0000 0 0 056 01170 00000 208 95696928 95173423 000 000 00 000 052.90 000 052 000 95 000 052 000\n"
            L"Ю3 583 00001101 1 00 00 0000 00000 0           \n"
            L"Ю3 583 00001102 9\n"
            L"Ю4 00000 1 02 000 052.90 000 052 95 000 052 \n"
            L"Ю4 11420 1 02 000 052.90 000 052 95 000 052 \n"
            L"Ю12 00 95696928 1 000 11420 01400 5552 000 00 00 00 00 00 0236 13 95 0950 04 106 11420 11420 00000 02 09860 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
            L"Ю12 00 95872248 255239255255 0232\n"
            L"Ю12 00 95029583 255239255255 0231\n"
            L"Ю12 00 95336905 255239255255 0233\n"
            L"Ю12 00 95040002 255239255255 0220\n"
            L"Ю12 00 95128609 255255255255 \n"
            L"Ю12 00 95486940 255255255255 \n"
            L"Ю12 00 95044285 255255255255 \n"
            L"Ю12 00 95657532 255239255255 0214\n"
            L"Ю12 00 95693834 255239255255 0235\n"
            L"Ю12 00 95998985 255239255255 0232\n"
            L"Ю12 00 95042271 255239255255 0220\n"
            L"Ю12 00 95043766 255255255255 \n"
            L"Ю12 00 95684809 255255255255 \n"
            L"Ю12 00 95046520 255255255255 \n"
            L"Ю12 00 95037461 255255255255 \n"
            L"Ю12 00 95169355 255239255255 0233\n"
            L"Ю12 00 95070561 255239255255 0220\n"
            L"Ю12 00 95573911 255255255255 \n"
            L"Ю12 00 95685046 255255255255 \n"
            L"Ю12 00 95806436 255255255255 \n"
            L"Ю12 00 95486809 255255255255 \n"
            L"Ю12 00 95684668 255255255255 \n"
            L"Ю12 00 95012688 255239255255 0234\n"
            L"Ю12 00 95173118 255239255255 0233\n"
            L"Ю12 00 95070678 255239255255 0220\n"
            L"Ю12 00 95173357 255239255255 0232\n"
            L"Ю12 00 95693784 231239255255 0120080570236\n"
            L"Ю12 00 95026472 255239255255 0233\n"
            L"Ю12 00 95699971 231239255255 0140055520235\n"
            L"Ю12 00 95046512 255239255255 0220\n"
            L"Ю12 00 95846077 255239255255 0237\n"
            L"Ю12 00 95716387 255239255255 0220\n"
            L"Ю12 00 95408357 255255255255 \n"
            L"Ю12 00 95806477 255239255255 0229\n"
            L"Ю12 00 95031415 255239255255 0220\n"
            L"Ю12 00 95126967 255255255255 \n"
            L"Ю12 00 95698833 239239255255 021080235\n"
            L"Ю12 00 95716718 255239255255 0220\n"
            L"Ю12 00 95846150 255239255255 0236\n"
            L"Ю12 00 95486551 255239255255 0220\n"
            L"Ю12 00 95173050 255239255255 0232\n"
            L"Ю12 00 95047874 239239255255 014000220\n"
            L"Ю12 00 95698718 255239255255 0236\n"
            L"Ю12 00 95459848 255239255255 0213\n"
            L"Ю12 00 95684742 255239255255 0220\n"
            L"Ю12 00 95806733 255239255255 0225\n"
            L"Ю12 00 95046124 255239255255 0220\n"
            L"Ю12 00 95652350 255239255255 0213\n"
            L"Ю12 00 95127106 255239255255 0220\n"
            L"Ю12 00 95716205 255255255255 \n"
            L"Ю12 00 95173423 255239255255 0232\n"
            L"Ю12 11 95716684:)]]><Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
            L"<Crew />"
            L"</Locomotive>"
            L"</AsoupEvent>"
);
        asoup_layer->createPath(L"<AsoupEvent create_time='20200919T114900Z' name='Info_changing' Bdg='ASOUP 1042_1p7w[09180]' index='0986-050-1142' num='3026' length='56' weight='1170' divisional='Y' waynum='7' parknum='1' dirTo='11420' adjTo='09181' linkBadge='7C[09180]' linkTime='20200919T114939Z' linkCode='Arrival'><![CDATA[(:1042 909/000+09180 3026 0986 050 1142 02 11420 19 09 14 49 01/07 0 0/00 00 0\n"
            L"Ю2 0 00 00 00 00 0000 0 0 056 01170 00000 208 95696928 95173423 000 000 00 000 052.90 000 052 000 95 000 052 000\n"
            L"Ю3 583 00001101 1 00 00 0000 00000 0           \n"
            L"Ю3 583 00001102 9\n"
            L"Ю4 00000 1 02 000 052.90 000 052 95 000 052 \n"
            L"Ю4 11420 1 02 000 052.90 000 052 95 000 052 :)]]><Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
            L"<Crew />"
            L"</Locomotive>"
            L"</AsoupEvent>");

            asoup_layer->createPath(L"<AsoupEvent create_time='20200919T115200Z' name='Departure' Bdg='ASOUP 1042_1p7w[09180]' index='0986-050-1142' num='3026' length='56' weight='1170' divisional='Y' waynum='7' parknum='1' dirTo='11420' adjTo='09181' linkBadge='7C[09180]' linkTime='20200919T115228Z' linkCode='Departure'><![CDATA[(:1042 909/000+09180 3026 0986 050 1142 03 11420 19 09 14 52 01/07 0 0/00 00 0\n"
            L"Ю2 0 00 00 00 00 0000 0 0 056 01170 00000 208 95696928 95173423 000 000 00 000 052.90 000 052 000 95 000 052 000\n"
            L"Ю3 583 00001101 1 11 39 0000 00000 KLIMASEVSKIS\n"
            L"Ю3 583 00001102 9\n"
            L"Ю4 00000 1 02 000 052.90 000 052 95 000 052 \n"
            L"Ю4 11420 1 02 000 052.90 000 052 95 000 052 \n"
            L"Ю12 00 95696928 1 000 11420 01400 5552 000 00 00 00 00 00 0236 13 95 0950 04 106 11420 11420 00000 02 09860 34 00000 0000 025 0 0000 09874  128 000 00000000\n"
            L"Ю12 00 95872248 255239255255 0232\n"
            L"Ю12 00 95029583 255239255255 0231\n"
            L"Ю12 00 95336905 255239255255 0233\n"
            L"Ю12 00 95040002 255239255255 0220\n"
            L"Ю12 00 95128609 255255255255 \n"
            L"Ю12 00 95486940 255255255255 \n"
            L"Ю12 00 95044285 255255255255 \n"
            L"Ю12 00 95657532 255239255255 0214\n"
            L"Ю12 00 95693834 255239255255 0235\n"
            L"Ю12 00 95998985 255239255255 0232\n"
            L"Ю12 00 95042271 255239255255 0220\n"
            L"Ю12 00 95043766 255255255255 \n"
            L"Ю12 00 95684809 255255255255 \n"
            L"Ю12 00 95046520 255255255255 \n"
            L"Ю12 00 95037461 255255255255 \n"
            L"Ю12 00 95169355 255239255255 0233\n"
            L"Ю12 00 95070561 255239255255 0220\n"
            L"Ю12 00 95573911 255255255255 \n"
            L"Ю12 00 95685046 255255255255 \n"
            L"Ю12 00 95806436 255255255255 \n"
            L"Ю12 00 95486809 255255255255 \n"
            L"Ю12 00 95684668 255255255255 \n"
            L"Ю12 00 95012688 255239255255 0234\n"
            L"Ю12 00 95173118 255239255255 0233\n"
            L"Ю12 00 95070678 255239255255 0220\n"
            L"Ю12 00 95173357 255239255255 0232\n"
            L"Ю12 00 95693784 231239255255 0120080570236\n"
            L"Ю12 00 95026472 255239255255 0233\n"
            L"Ю12 00 95699971 231239255255 0140055520235\n"
            L"Ю12 00 95046512 255239255255 0220\n"
            L"Ю12 00 95846077 255239255255 0237\n"
            L"Ю12 00 95716387 255239255255 0220\n"
            L"Ю12 00 95408357 255255255255 \n"
            L"Ю12 00 95806477 255239255255 0229\n"
            L"Ю12 00 95031415 255239255255 0220\n"
            L"Ю12 00 95126967 255255255255 \n"
            L"Ю12 00 95698833 239239255255 021080235\n"
            L"Ю12 00 95716718 255239255255 0220\n"
            L"Ю12 00 95846150 255239255255 0236\n"
            L"Ю12 00 95486551 255239255255 0220\n"
            L"Ю12 00 95173050 255239255255 0232\n"
            L"Ю12 00 95047874 239239255255 014000220\n"
            L"Ю12 00 95698718 255239255255 0236\n"
            L"Ю12 00 95459848 255239255255 0213\n"
            L"Ю12 00 95684742 255239255255 0220\n"
            L"Ю12 00 95806733 255239255255 0225\n"
            L"Ю12 00 95046124 255239255255 0220\n"
            L"Ю12 00 95652350 255239255255 0213\n"
            L"Ю12 00 95127106 255239255255 0220\n"
            L"Ю12 00 95716205 255255255255 \n"
            L"Ю12 00 95173423 255239255255 0232:)]]><Locomotive Series='2М62УМ' NumLoc='110' Consec='1'>"
            L"<Crew EngineDriver='KLIMASEVSKIS' Tim_Beg='2020-09-19 11:39' />"
            L"</Locomotive>"
            L"</AsoupEvent>");

            asoup_layer->createPath(
            L"<AsoupEvent create_time='20200919T115200Z' name='Info_changing' Bdg='ASOUP 904[09180]' index='0986-050-1142' num='3026' length='56' weight='1170' divisional='Y' linkBadge='7C[09180]' linkTime='20200919T115228Z' linkCode='Departure'><![CDATA[(:904 0918 3026 0986 50 1142 1 19 09 14 52 056 01170 0 0000 0 0\n"
            L"01 95696928 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"02 95872248 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"03 95029583 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0231 0106 0705\n"
            L"04 95336905 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"05 95040002 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"06 95128609 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"07 95486940 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"08 95044285 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"10 95693834 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"11 95998985 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"12 95042271 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"13 95043766 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"14 95684809 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"15 95046520 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"16 95037461 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"17 95169355 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"18 95070561 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"19 95573911 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"20 95685046 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"21 95806436 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"22 95486809 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"23 95684668 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"24 95012688 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0234 0106 0705\n"
            L"25 95173118 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"26 95070678 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"27 95173357 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"28 95693784 0253 000 11421  01200 8057 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"29 95026472 0252 000 11421  01200 8057 0 0 0 0 00/00 00000 000 09874  0233 0106 0705\n"
            L"30 95699971 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"31 95046512 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"32 95846077 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0237 0106 0702\n"
            L"33 95716387 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"34 95408357 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"35 95806477 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0229 0106 0700\n"
            L"36 95031415 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0650\n"
            L"37 95126967 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"38 95698833 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0235 0106 0702\n"
            L"39 95716718 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"40 95846150 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"41 95486551 0253 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"42 95173050 0252 000 11421  02108 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705\n"
            L"43 95047874 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"44 95698718 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0236 0106 0702\n"
            L"45 95459848 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0213 0106 0700\n"
            L"46 95684742 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"47 95806733 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0225 0106 0700\n"
            L"48 95046124 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"49 95652350 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0213 0106 0700\n"
            L"50 95127106 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"51 95716205 0253 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0220 0106 0700\n"
            L"52 95173423 0252 000 11421  01400 5552 0 0 0 0 00/00 00000 000 09874  0232 0106 0705:)]]></AsoupEvent>");

        {
        AsoupLayer::ReadAccessor container = asoup_layer->GetReadAccess();
        auto pathList = container->UT_GetEvents();
        CPPUNIT_ASSERT( pathList.size() == 9 );
        for ( auto asoup : pathList )
            CPPUNIT_ASSERT( asoup_layer->IsServed( asoup ) );
        }

    setTopologyPath(11042, 16169);

 		AsoupQueueMgr asoupQMgr;
   Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    {
        aeAttach_AsoupToHappen att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, asoupQMgr, context, *asoup9280, *spot, true );
        att.Action();
        CPPUNIT_ASSERT( happen_layer->path_count() == 2 );
    }
    
    aeAttach_AsoupToHappen att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, asoupQMgr, context, *asoup9280, *spot, false );
    att.Action();
    collectUndo();
    {
        CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size(8) );
    }
}

void TC_Hem_aeAttach::FormArrivalDisformToForm_5947()
{
    auto& hl = *happen_layer;
    // первая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20201011T055221Z' name='Form' Bdg='9JC[09006]' waynum='9' parknum='1' index='' num='4803M-Kr' reslocoutbnd='Y' />"
        L"<SpotEvent create_time='20201011T055511Z' name='Arrival' Bdg='9JC[09006]' waynum='9' parknum='1' />"
        L"<SpotEvent create_time='20201011T055522Z' name='Disform' Bdg='9JC[09006]' waynum='9' parknum='1' />"
        L"</HemPath>"
        );
    // вторая нить
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20201011T055522Z' name='Form' Bdg='9JC[09006]' index='' num='4803M' waynum='9' parknum='1' />"
        L"</HemPath>"
        );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200621T144500Z'>"
        L"<PrePoint create_time='20201011T055522Z' name='Disform' Bdg='9JC[09006]' waynum='9' parknum='1' />"
        L"<Action code='Attach' District='09000-09640' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint create_time='20201011T055522Z' name='Form' Bdg='9JC[09006]' index='' num='4803M' waynum='9' parknum='1'/>"
        L"<EsrList>"
        L"<Station esr='09006' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "num",  "4803M-Kr     -  " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form      Arrival" ) );
}

void TC_Hem_aeAttach::SpanStop_OnTheSamePlace_3889()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"<SpotEvent create_time='20201017T061051Z' name='Form' Bdg='PpGP[09180]' index='0001-013-0928' num='8202' stone='Y' />"
        L"<SpotEvent create_time='20201017T061051Z' name='Departure' Bdg='PpGP[09180]' />"
        L"<SpotEvent create_time='20201017T061141Z' name='Wrong_way' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"<rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201017T061141Z' name='Span_stopping_begin' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"<rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201017T062837Z' name='Span_stopping_end' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"<rwcoord picketing1_val='46~1000' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201017T062837Z' name='Death' Bdg='edit[09180:09280]' waynum='1'>"
        L"<rwcoord picketing1_val='46~1000' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить 
    happen_layer->createPath( 
        L"<HemPath>"
        L"<SpotEvent create_time='20201017T070231Z' name='Form' Bdg='NGJR[09180:09280]' waynum='1' index='0001-013-0928' num='8202' stone='Y'>"
        L"<rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201017T070231Z' name='Wrong_way' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"<rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201017T070231Z' name='Span_stopping_begin' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"<rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201017T071927Z' name='Span_stopping_end' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"<rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201017T072332Z' name='Arrival' Bdg='5C[09180]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20201017T072332Z' name='Disform' Bdg='5C[09180]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );

        // 
    // выполняем билль
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20201017T094540Z'>"
        L"<PrePoint name='Death' Bdg='edit[09180:09280]' create_time='20201017T062837Z' waynum='1'>"
        L"<rwcoord picketing1_val='46~1000' picketing1_comm='Реньге' />"
        L"</PrePoint>"
        L"<Action code='Attach' District='09180-09860' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='NGJR[09180:09280]' create_time='20201017T070231Z' index='0001-013-0928' num='8202' stone='Y' waynum='1'>"
        L"<rwcoord picketing1_val='46~1000' picketing1_comm='Реньге' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    // до
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(6) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    // после
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(7) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form         Departure   Wrong_way        Span_stopping_begin  Span_stopping_end    Arrival Disform" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "PpGP[09180] PpGP[09180] NGJR[09180:09280] NGJR[09180:09280]    NGJR[09180:09280]   5C[09180] 5C[09180]" ) );
    // а теперь после отката
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(6) );
}

void TC_Hem_aeAttach::SpanStopEntraStop_OnTheSamePlace_6155()
{
    // первая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"<SpotEvent create_time='20210303T104404Z' name='Form' Bdg='5C15+[09400]' waynum='5' parknum='1' index='0001-750-0941' num='8223' stone='Y' />"
        L"<SpotEvent create_time='20210303T104404Z' name='Departure' Bdg='5C15+[09400]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20210303T104656Z' name='Span_stopping_begin' Bdg='15P_O[09400:09410]' waynum='1'>"
        L"<rwcoord picketing1_val='49~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210303T113603Z' name='Span_stopping_end' Bdg='15P_O[09400:09410]' waynum='1'>"
        L"<rwcoord picketing1_val='49~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210303T113603Z' name='Death' Bdg='15P_O[09400:09410]' waynum='1'>"
        L"<rwcoord picketing1_val='49~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );
    // вторая нить 
    happen_layer->createPath( 
        L"<HemPath>"
        L"<SpotEvent create_time='20210303T123234Z' name='Form' Bdg='15P_O[09400:09410]' waynum='1'>"
        L"<rwcoord picketing1_val='49~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210303T123234Z' name='Entra_stopping' Bdg='17P_O[09400:09410]' waynum='1'>"
        L"<rwcoord picketing1_val='51~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='50~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210303T123903Z' name='Arrival' Bdg='5C15+[09400]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );

    // 
    // выполняем билль
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20210303T124236Z'>"
        L"<PrePoint name='Death' Bdg='15P_O[09400:09410]' create_time='20210303T113603Z' waynum='1'>"
        L"<rwcoord picketing1_val='49~500' picketing1_comm='Рига-Зилупе' />"
        L"</PrePoint>"
        L"<Action code='Attach' District='09000-11760' DistrictRight='Y' FixBound='20210303T054700Z' />"
        L"<PostPoint name='Form' Bdg='15P_O[09400:09410]' create_time='20210303T123234Z' waynum='1'>"
        L"<rwcoord picketing1_val='49~500' picketing1_comm='Рига-Зилупе' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09400' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    // до
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    // после
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(6) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form         Departure   Span_stopping_begin  Span_stopping_end  Entra_stopping     Arrival " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "5C15+[09400] 5C15+[09400] 15P_O[09400:09410] 15P_O[09400:09410] 17P_O[09400:09410]  5C15+[09400]" ) );
    // а теперь после отката
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==2 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
}

void TC_Hem_aeAttach::SaveDriver_6328()
{
    // вторая нить
    happen_layer->createPath( 
        L"<HemPath>"
        L"<SpotEvent create_time='20210611T020345Z' name='Form' Bdg='2C[09370]' waynum='2' parknum='1' index='0900-285-0705' num='2732' length='57' weight='3035' net_weight='1640' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='TURBAN' Tim_Beg='2021-06-10 23:45' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210611T020345Z' name='Departure' Bdg='2C[09370]' waynum='2' parknum='1' index='0900-285-0705' num='2732' length='57' weight='3035' net_weight='1640' through='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='TURBAN' Tim_Beg='2021-06-10 23:45' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210611T021852Z' name='Transition' Bdg='2C[09360]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210611T023831Z' name='Transition' Bdg='2AC[11760]' waynum='2' parknum='2' index='0900-285-0705' num='2732' length='57' weight='3035' net_weight='1640' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='TURBAN' Tim_Beg='2021-06-10 23:45' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210611T023831Z' name='Transition' Bdg='2C[11760]' waynum='2' parknum='1' optCode='11420:11760' />"
        L"<SpotEvent create_time='20210611T025955Z' name='Arrival' Bdg='4C[11420]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210611T031543Z' name='Departure' Bdg='4C[11420]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210611T034001Z' name='Arrival' Bdg='3C[11412]' waynum='3' parknum='1' />"
//         L"<SpotEvent create_time='20210611T040524Z' name='Departure' Bdg='3C[11412]' waynum='3' parknum='1' optCode='11401:11412' />"
//         L"<SpotEvent create_time='20210611T042020Z' name='Transition' Bdg='1C[11401]' waynum='1' parknum='1' optCode='11391:11401' />"
//         L"<SpotEvent create_time='20210611T043137Z' name='Transition' Bdg='1C[11391]' waynum='1' parknum='1' optCode='11390:11391' />"
//         L"<SpotEvent create_time='20210611T044941Z' name='Transition' Bdg='1C[11390]' waynum='1' parknum='1' optCode='11381:11390' />"
//         L"<SpotEvent create_time='20210611T045839Z' name='Transition' Bdg='1C[11381]' waynum='1' parknum='1' optCode='11380:11381' />"
//         L"<SpotEvent create_time='20210611T051538Z' name='Arrival' Bdg='3C[11380]' waynum='3' parknum='1' />"
//         L"<SpotEvent create_time='20210611T053001Z' name='Departure' Bdg='3C[11380]' waynum='3' parknum='1' optCode='11311:11380' />"
//         L"<SpotEvent create_time='20210611T054936Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' optCode='11311:11321' />"
//         L"<SpotEvent create_time='20210611T060247Z' name='Transition' Bdg='701SP:701+[11321]'>"
//         L"<rwcoord picketing1_val='223~200' picketing1_comm='Рига-Зилупе' />"
//         L"</SpotEvent>"
//         L"<SpotEvent create_time='20210611T061047Z' name='Arrival' Bdg='9C[11310]' waynum='9' parknum='1' />"
        L"</HemPath>"
        );

    // 
    // выполняем билль
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20210611T145454Z'>"
        L"<PrePoint layer='asoup' Bdg='ASOUP 1042_1p2w[09370]' name='Arrival' create_time='20210610T221500Z' index='0900-285-0705' num='2732' length='57' weight='3035' net_weight='1640' through='Y' waynum='2' parknum='1'>"
        L"<![CDATA[(:1042 909/000+09370 2732 0900 285 0705 01 09000 11 06 01 15 01/02 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 03035 01640 208 73998163 52106523 000 000 00 026 026.20 000 001 000 40 000 012 000 60 000 002 000 70 000 008 000 73 000 001 000 90 026 003 000 93 000 003 000\n"
        L"Ю3 583 00000861 1 13 10 0000 00000 M-GORJAJEVS \n"
        L"Ю3 583 00000862 9\n"
        L"Ю4 07792 2 01 026 026.20 000 001 40 000 012 60 000 002 70 000 008 73 000 001 90 026 003 93 000 003 \n"
        L"Ю12 00 73998163 1 000 04520 21403 9403 460 01 00 00 00 00 0244 12 71 0791 04 086 07050 00000 07792 01 09000 03 00000 0000 020 1 1250 COBETC 128 000 00000000\n"
        L"Ю12 00 51570885 255233255255 0256705700\n"
        L"Ю12 00 51696912 255239255255 0272\n"
        L"Ю12 00 53907663 255239255255 0258\n"
        L"Ю12 00 51074144 255239255255 0247\n"
        L"Ю12 00 54631486 251239255255 3600266\n"
        L"Ю12 00 51303089 255239255255 0268\n"
        L"Ю12 00 51819431 255239255255 0267\n"
        L"Ю12 00 54406061 193233125247 033400911187450000002174054041052411106 \n"
        L"Ю12 00 42244244 255237255255 02110404\n"
        L"Ю12 00 54515465 255237255255 02055404\n"
        L"Ю12 00 54514484 255255255255 \n"
        L"Ю12 00 43031954 199237255255 0350009111200602250404\n"
        L"Ю12 00 42007492 195239255183 02550081184977400025002509336 \n"
        L"Ю12 00 54805734 255237255255 02555406\n"
        L"Ю12 00 54805833 255255255255 \n"
        L"Ю12 00 42007534 255237255255 02500404\n"
        L"Ю12 00 59342774 195233125183 0730028104146000001939353300862302009439 \n"
        L"Ю12 00 58951922 255255255255 \n"
        L"Ю12 00 58985037 255239255255 0195\n"
        L"Ю12 00 60499837 199233125255 02170267128078023960060010031\n"
        L"Ю12 00 56106867 199237255255 0231026712463802325600\n"
        L"Ю12 00 59611806 129225125243 0630535015106504650009029820905958119080     132\n"
        L"Ю12 00 59614040 255239255255 0299\n"
        L"Ю12 00 59707299 255239255255 0296\n"
        L"Ю12 00 59611285 255239255255 0298\n"
        L"Ю12 00 59610444 255255255255 \n"
        L"Ю12 00 59615005 255239255255 0297\n"
        L"Ю12 00 59614701 255239255255 0295\n"
        L"Ю12 00 59614461 255239255255 0298\n"
        L"Ю12 00 59614123 255239255255 0299\n"
        L"Ю12 00 59612739 255239255255 0298\n"
        L"Ю12 00 59613257 255255255255 \n"
        L"Ю12 00 59615716 255239255255 0297\n"
        L"Ю12 00 59611939 255239255255 0298\n"
        L"Ю12 00 59614925 251239255255 3000297\n"
        L"Ю12 00 59705996 255239255255 0295\n"
        L"Ю12 00 59706796 255255255255 \n"
        L"Ю12 00 59611632 255239255255 0299\n"
        L"Ю12 00 59613885 255255255255 \n"
        L"Ю12 00 59700997 255239255255 0296\n"
        L"Ю12 00 59612580 255239255255 0298\n"
        L"Ю12 00 59702795 255239255255 0294\n"
        L"Ю12 00 59612275 255239255255 0299\n"
        L"Ю12 00 59615732 255255255255 \n"
        L"Ю12 00 59613604 255255255255 \n"
        L"Ю12 00 58591314 191237127255 06402845681122\n"
        L"Ю12 00 58590233 255239255255 0286\n"
        L"Ю12 00 54824040 133225125179 00007090081188330000257124054061052402609336-128\n"
        L"Ю12 00 54817846 255239255255 0260\n"
        L"Ю12 00 54826847 255239255255 0257\n"
        L"Ю12 00 52106523 195233125183 0707075344616800002582052061221802009490-:)]]><feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-GORJAJEVS' Tim_Beg='2021-06-10 13:10' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<Action code='Attach' District='09000-11760' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Form' Bdg='2C[09370]' create_time='20210611T020345Z' index='0900-285-0705' num='2732' length='57' weight='3035' net_weight='1640' through='Y' waynum='2' parknum='1'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='TURBAN' Tim_Beg='2021-06-10 23:45' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09360' />"
        L"<Station esr='09370' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    // до
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(8) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form  Departure  Transition  Transition  Transition  Arrival     Departure   Arrival" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "2C[09370] 2C[09370] 2C[09360]   2AC[11760]   2C[11760]  4C[11420]   4C[11420]   3C[11412] " ) );

    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();
    collectUndo();

    // после
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(8) );
    auto listFrags = happen_layer->GetPaths_OnlyFrags(time_from_iso("20210610T221500Z"),time_from_iso("20210611T034001Z"));
    CPPUNIT_ASSERT( listFrags.size() == 1 && listFrags.front().size()==3);
    auto vLoks = listFrags.front();
    Crew crew;
    vLoks.front().second->get_crew( crew );
    CPPUNIT_ASSERT( vLoks.front().first == time_from_iso("20210610T221500Z") && crew.get_engineDriver()==L"M-GORJAJEVS" );
    auto it = std::next(vLoks.begin());
    it->second->get_crew( crew );
    CPPUNIT_ASSERT( it->first == time_from_iso("20210611T020345Z") && crew.get_engineDriver()==L"TURBAN" );
    vLoks.back().second->get_crew( crew );
    CPPUNIT_ASSERT( vLoks.back().first== time_from_iso("20210611T023831Z") && crew.get_engineDriver()==L"TURBAN" );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Arrival  Departure  Transition  Transition  Transition  Arrival     Departure   Arrival" ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "ASOUP 1042_1p2w[09370]  2C[09370] 2C[09360]   2AC[11760]   2C[11760]  4C[11420]   4C[11420]   3C[11412] " ) );
    // а теперь после отката
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count()==1 );
    CPPUNIT_ASSERT( happen_layer->exist_path_size(8) );
}

void TC_Hem_aeAttach::AsoupToAsoup_WithThrow_6646()
{
	setTopologyPath(31000, 31310);

	Hem::Bill bill = createBill(
		L"<A2F_HINT Hint='cmd_edit'>"
		L"<PrePoint layer='asoup' Bdg='ASOUP 1042[31000]' name='Departure' create_time='20211211T170000Z' index='3100-027-3101' num='3731' length='57' weight='2598' net_weight='1123' transfer='Y' waynum='1' parknum='5'><![CDATA[(:1042 928/400+31000 3731 3100 027 3101 03 31010 11 12 20 00 05/01 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 057 02598 01123 232 51606754 95181566 000 016 00 017 025.20 000 002 001 40 000 002 000 60 006 002 000 70 000 016 015 87 000 003 000 90 011 000 000 95 011 000 000\n"
		L"Ю3 627 00000830 1 09 00 2801 17247 EPMOЛEHKO    00000 216 2801 00054208 0 1 00 00 00 00\n"
		L"Ю4 31550 1 01 000 002 000.20 000 002 000\n"
		L"Ю4 31020 1 01 006 019 000.40 000 002 000 60 006 001 000 70 000 016 000\n"
		L"Ю4 31010 1 01 011 004 000.60 000 001 000 87 000 003 000 90 011 000 000 95 011 000 000\n"
		L"Ю4 00000 1 01 017 025 000.20 000 002 000 40 000 002 000 60 006 002 000 70 000 016 000 87 000 003 000 90 011 000 000 95 011 000 000\n"
		L"Ю6 51606754 20211210\n"
		L"Ю6 50450550 20211210\n"
		L"Ю6 51644045 20211210\n"
		L"Ю6 51717387 20211210\n"
		L"Ю6 50403351 20211210\n"
		L"Ю6 54092135 20211210\n"
		L"Ю6 50777242 20211210\n"
		L"Ю6 53908992 20211210\n"
		L"Ю6 29147642 20211212\n"
		L"Ю6 52714631 20211214\n"
		L"Ю6 51380863 20211213\n"
		L"Ю6 54090204 20211213\n"
		L"Ю6 51628774 20211213\n"
		L"Ю6 53950663 20211216\n"
		L"Ю6 51638088 20211216\n"
		L"Ю6 29495991 20211212\n"
		L"Ю6 54091780 20211214\n"
		L"Ю6 51638054 20211214\n"
		L"Ю6 57473498 20211214\n"
		L"Ю6 52304946 20211212\n"
		L"Ю6 51445112 20211211\n"
		L"Ю6 50443209 20211209\n"
		L"Ю6 58581588 20211212\n"
		L"Ю6 50686443 20211214\n"
		L"Ю6 57617193 20211214\n"
		L"Ю6 51511640 20211214\n"
		L"Ю6 51444644 20211214\n"
		L"Ю6 50775030 20211214\n"
		L"Ю6 50365246 20211214\n"
		L"Ю6 50387448 20211214\n"
		L"Ю6 51716397 20211214\n"
		L"Ю6 95537833 20211216\n"
		L"Ю6 95035036 20211216\n"
		L"Ю6 50475136 20211210\n"
		L"Ю6 58387176 20211211\n"
		L"Ю6 53923744 20211211\n"
		L"Ю6 53913414 20211211\n"
		L"Ю6 51420966 20211211\n"
		L"Ю6 54662242 20211211\n"
		L"Ю6 28035723 20211214\n"
		L"Ю6 42911560 20211213\n"
		L"Ю6 42910711 20211213\n"
		L"Ю6 55137616 20211214\n"
		L"Ю6 60506516 20211214\n"
		L"Ю6 53011110 20211214\n"
		L"Ю6 52966850 20211214\n"
		L"Ю6 56075419 20211214\n"
		L"Ю6 54119219 20211214\n"
		L"Ю6 58383183 20211212\n"
		L"Ю6 58566316 20211223\n"
		L"Ю6 59891390 20211223\n"
		L"Ю6 95273314 20211223\n"
		L"Ю6 95927208 20211223\n"
		L"Ю6 95216115 20211223\n"
		L"Ю6 95931119 20211223\n"
		L"Ю6 95464723 20211223\n"
		L"Ю6 95992004 20211223\n"
		L"Ю6 95181566 20211223\n"
		L"Ю8 3100 027 3101 31000 62 11 12 20 00 3731 310005 027 310109\n"
		L"Ю12 00 51606754 1 000 31020 22106 4529 080 01 00 00 00 00 0271 13 70 5700 04 086 31010 31020 00000 01 31000 00 31490 6520 020 0 0000 ЭAK906 128 000 10000000\n"
		L"Ю12 00 50450550 255239255255 0275\n"
		L"Ю12 00 51644045 235239255245 214040600264БAЛTC 008\n"
		L"Ю12 00 51717387 235231255245 22106980026540ПEPECЛ000\n"
		L"Ю12 00 50403351 239239255255 221100275\n"
		L"Ю12 00 54092135 239239255255 221060252\n"
		L"Ю12 00 50777242 255239255255 0269\n"
		L"Ю12 00 53908992 195239223127 310102110567589600265310107366\n"
		L"Ю12 00 29147642 193225092119 31550132171481000020267132002111353155039231004430Э     \n"
		L"Ю12 00 52714631 197233092127 310203241120040002426056001003102000310006336\n"
		L"Ю12 00 51380863 225225126119 22106452998001028140705700086314906520ПEPECЛ\n"
		L"Ю12 00 54090204 255239255255 0252\n"
		L"Ю12 00 51628774 195239223127 310102110567589600264310107366\n"
		L"Ю12 00 53950663 195231220119 310202260245290600254133102041020705789ЭAK305\n"
		L"Ю12 00 51638088 235239254119 221100800288035606520ЭAK906\n"
		L"Ю12 00 29495991 193233092119 315502412514810000202682002111353155000318604430ПГK   \n"
		L"Ю12 00 54091780 193225094119 310202210645299800102524070570008631020314906520ПEPECЛ\n"
		L"Ю12 00 51638054 255239255255 0288\n"
		L"Ю12 00 57473498 255239255255 0252\n"
		L"Ю12 00 52304946 193225094119 310104330410050000002401360560010031010303604430Э     \n"
		L"Ю12 00 51445112 193233092119 310202140345290600102577057000863102003248506520БAЛTC \n"
		L"Ю12 00 50443209 235239254247 21341000027179040Э     \n"
		L"Ю12 00 58581588 197233092127 310106932235620203108759521223101000288303562\n"
		L"Ю12 00 50686443 225225126119 21105675896001024740705700086314907366ПEPECЛ\n"
		L"Ю12 00 57617193 195231223119 31020221064529060026713310206520БAЛTC \n"
		L"Ю12 00 51511640 195231223119 31010211056758960027040310107366ПEPECЛ\n"
		L"Ю12 00 51444644 211231220119 3102045293600257133102003176305853БAЛTC \n"
		L"Ю12 00 50775030 255239255255 0269\n"
		L"Ю12 00 50365246 255239255255 0268\n"
		L"Ю12 00 50387448 255239255255 0267\n"
		L"Ю12 00 51716397 255239255255 0268\n"
		L"Ю12 00 95537833 129225092113 06831010233117627380070220309509501063101019202109109KЛИHЦЫ132008\n"
		L"Ю12 00 95035036 191239255255 0670230\n"
		L"Ю12 00 50475136 129225092113 00031020213414529000010271137057000863102003790406520Э     128000\n"
		L"Ю12 00 58387176 197233092119 310105720835620203108759521223101027270303562ЦXBH  \n"
		L"Ю12 00 53923744 193225092119 31020221064529980010263407057000863102000314906520ПEPECЛ\n"
		L"Ю12 00 53913414 251231255183 080026113026ЭAK906\n"
		L"Ю12 00 51420966 251231255183 980025540020ПEPECЛ\n"
		L"Ю12 00 54662242 235231255183 21105060026613026БAЛTC \n"
		L"Ю12 00 28035723 193225094055 310103512922549000202674020020812231010290002254020ЭCTH  \n"
		L"Ю12 00 42911560 193225094119 310206930820063000002101340040410531020304602006Э     \n"
		L"Ю12 00 42910711 255239255255 0205\n"
		L"Ю12 00 55137616 163225124115 04032305662200002423060560010009302505010ПГK   132\n"
		L"Ю12 00 60506516 175237255247 0663241602400600XBHECT\n"
		L"Ю12 00 53011110 187237255247 06730002245608ГPЗKO \n"
		L"Ю12 00 52966850 191237255255 06802325600\n"
		L"Ю12 00 56075419 187239255247 0690000240ПГK   \n"  
		L"Ю12 00 54119219 239239255247 324180241Э     \n"
		L"Ю12 00 58383183 133225092123 00031010592023562020310138759521223101000283903562128\n"
		L"Ю12 00 58566316 169225124115 0680140030004022730955935106346003053800     132\n"
		L"Ю12 00 59891390 191239255255 0700233\n"
		L"Ю12 00 95273314 191237255255 06102110950\n"
		L"Ю12 00 95927208 189239255255 070020234\n"
		L"Ю12 00 95216115 255239255255 0224\n"
		L"Ю12 00 95931119 255239255255 0228\n"
		L"Ю12 00 95464723 189239255255 061040205\n"
		L"Ю12 00 95992004 189239255255 068020232\n"
		L"Ю12 00 95181566 191239255255 0700227:)]]><Locomotive Series='ТЭМ14' NumLoc='83' Depo='2801' Consec='1'>"
		L"<Crew EngineDriver='EPMOЛEHKO' TabNum='17247' Tim_Beg='2021-12-11 09:00' />"
		L"</Locomotive>"
		L"</PrePoint>"
		L"<Action code='Attach' District='31000-05110' DistrictRight='Y' />"
		L"<PostPoint layer='asoup' Bdg='ASOUP 1042[31000]' name='Transition' create_time='20211211T171100Z' index='3100-027-3101' num='3731' length='57' weight='2598' net_weight='1123' transfer='Y' waynum='1' parknum='1'><![CDATA[(:1042 928/400+31001 3731 3100 027 3101 04 31000+31010 11 12 20 11 01/01 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 057 02598 01123 232 51606754 95181566 000 016 00 017 025.20 000 002 001 40 000 002 000 60 006 002 000 70 000 016 015 87 000 003 000 90 011 000 000 95 011 000 000\n"
		L"Ю3 627 00000830 1 09 00 2801 17247 EPMOЛEHKO    00000 216 2801 00054508 0 1 00 00 00 00\n"
		L"Ю4 31550 1 01 000 002 000.20 000 002 000\n"
		L"Ю4 31020 1 01 006 019 000.40 000 002 000 60 006 001 000 70 000 016 000\n"
		L"Ю4 31010 1 01 011 004 000.60 000 001 000 87 000 003 000 90 011 000 000 95 011 000 000\n"
		L"Ю4 00000 1 01 017 025 000.20 000 002 000 40 000 002 000 60 006 002 000 70 000 016 000 87 000 003 000 90 011 000 000 95 011 000 000\n"
		L"Ю6 51606754 20211210\n"
		L"Ю6 50450550 20211210\n"
		L"Ю6 51644045 20211210\n"
		L"Ю6 51717387 20211210\n"
		L"Ю6 50403351 20211210\n"
		L"Ю6 54092135 20211210\n"
		L"Ю6 50777242 20211210\n"
		L"Ю6 53908992 20211210\n"
		L"Ю6 29147642 20211212\n"
		L"Ю6 52714631 20211214\n"
		L"Ю6 51380863 20211213\n"
		L"Ю6 54090204 20211213\n"
		L"Ю6 51628774 20211213\n"
		L"Ю6 53950663 20211216\n"
		L"Ю6 51638088 20211216\n"
		L"Ю6 29495991 20211212\n"
		L"Ю6 54091780 20211214\n"
		L"Ю6 51638054 20211214\n"
		L"Ю6 57473498 20211214\n"
		L"Ю6 52304946 20211212\n"
		L"Ю6 51445112 20211211\n"
		L"Ю6 50443209 20211209\n"
		L"Ю6 58581588 20211212\n"
		L"Ю6 50686443 20211214\n"
		L"Ю6 57617193 20211214\n"
		L"Ю6 51511640 20211214\n"
		L"Ю6 51444644 20211214\n"
		L"Ю6 50775030 20211214\n"
		L"Ю6 50365246 20211214\n"
		L"Ю6 50387448 20211214\n"
		L"Ю6 51716397 20211214\n"
		L"Ю6 95537833 20211216\n"
		L"Ю6 95035036 20211216\n"
		L"Ю6 50475136 20211210\n"
		L"Ю6 58387176 20211211\n"
		L"Ю6 53923744 20211211\n"
		L"Ю6 53913414 20211211\n"
		L"Ю6 51420966 20211211\n"
		L"Ю6 54662242 20211211\n"
		L"Ю6 28035723 20211214\n"
		L"Ю6 42911560 20211213\n"
		L"Ю6 42910711 20211213\n"
		L"Ю6 55137616 20211214\n"
		L"Ю6 60506516 20211214\n"
		L"Ю6 53011110 20211214\n"
		L"Ю6 52966850 20211214\n"
		L"Ю6 56075419 20211214\n"
		L"Ю6 54119219 20211214\n"
		L"Ю6 58383183 20211212\n"
		L"Ю6 58566316 20211223\n"
		L"Ю6 59891390 20211223\n"
		L"Ю6 95273314 20211223\n"
		L"Ю6 95927208 20211223\n"
		L"Ю6 95216115 20211223\n"
		L"Ю6 95931119 20211223\n"
		L"Ю6 95464723 20211223\n"
		L"Ю6 95992004 20211223\n"
		L"Ю6 95181566 20211223\n"
		L"Ю8 3100 027 3101 31001 03 11 12 20 11 3731 310005 027 310109\n"
		L"Ю12 00 51606754 1 000 31020 22106 4529 080 01 00 00 00 00 0271 13 70 5700 04 086 31010 31020 00000 01 31000 00 31490 6520 020 0 0000 ЭAK906 128 000 10000000\n"
		L"Ю12 00 50450550 255239255255 0275\n"
		L"Ю12 00 51644045 235239255245 214040600264БAЛTC 008\n"
		L"Ю12 00 51717387 235231255245 22106980026540ПEPECЛ000\n"
		L"Ю12 00 50403351 239239255255 221100275\n"
		L"Ю12 00 54092135 239239255255 221060252\n"
		L"Ю12 00 50777242 255239255255 0269\n"
		L"Ю12 00 53908992 195239223127 310102110567589600265310107366\n"
		L"Ю12 00 29147642 193225092119 31550132171481000020267132002111353155039231004430Э     \n"
		L"Ю12 00 52714631 197233092127 310203241120040002426056001003102000310006336\n"
		L"Ю12 00 51380863 225225126119 22106452998001028140705700086314906520ПEPECЛ\n"
		L"Ю12 00 54090204 255239255255 0252\n"
		L"Ю12 00 51628774 195239223127 310102110567589600264310107366\n"
		L"Ю12 00 53950663 195231220119 310202260245290600254133102041020705789ЭAK305\n"
		L"Ю12 00 51638088 235239254119 221100800288035606520ЭAK906\n"
		L"Ю12 00 29495991 193233092119 315502412514810000202682002111353155000318604430ПГK   \n"
		L"Ю12 00 54091780 193225094119 310202210645299800102524070570008631020314906520ПEPECЛ\n"
		L"Ю12 00 51638054 255239255255 0288\n"
		L"Ю12 00 57473498 255239255255 0252\n"
		L"Ю12 00 52304946 193225094119 310104330410050000002401360560010031010303604430Э     \n"
		L"Ю12 00 51445112 193233092119 310202140345290600102577057000863102003248506520БAЛTC \n"
		L"Ю12 00 50443209 235239254247 21341000027179040Э     \n"
		L"Ю12 00 58581588 197233092127 310106932235620203108759521223101000288303562\n"
		L"Ю12 00 50686443 225225126119 21105675896001024740705700086314907366ПEPECЛ\n"
		L"Ю12 00 57617193 195231223119 31020221064529060026713310206520БAЛTC \n"
		L"Ю12 00 51511640 195231223119 31010211056758960027040310107366ПEPECЛ\n"
		L"Ю12 00 51444644 211231220119 3102045293600257133102003176305853БAЛTC \n"
		L"Ю12 00 50775030 255239255255 0269\n"
		L"Ю12 00 50365246 255239255255 0268\n"
		L"Ю12 00 50387448 255239255255 0267\n"
		L"Ю12 00 51716397 255239255255 0268\n"
		L"Ю12 00 95537833 129225092113 06831010233117627380070220309509501063101019202109109KЛИHЦЫ132008\n"
		L"Ю12 00 95035036 191239255255 0670230\n"
		L"Ю12 00 50475136 129225092113 00031020213414529000010271137057000863102003790406520Э     128000\n"
		L"Ю12 00 58387176 197233092119 310105720835620203108759521223101027270303562ЦXBH  \n"
		L"Ю12 00 53923744 193225092119 31020221064529980010263407057000863102000314906520ПEPECЛ\n"
		L"Ю12 00 53913414 251231255183 080026113026ЭAK906\n"
		L"Ю12 00 51420966 251231255183 980025540020ПEPECЛ\n"
		L"Ю12 00 54662242 235231255183 21105060026613026БAЛTC \n"
		L"Ю12 00 28035723 193225094055 310103512922549000202674020020812231010290002254020ЭCTH  \n"
		L"Ю12 00 42911560 193225094119 310206930820063000002101340040410531020304602006Э     \n"
		L"Ю12 00 42910711 255239255255 0205\n"
		L"Ю12 00 55137616 163225124115 04032305662200002423060560010009302505010ПГK   132\n"
		L"Ю12 00 60506516 175237255247 0663241602400600XBHECT\n"
		L"Ю12 00 53011110 187237255247 06730002245608ГPЗKO \n"
		L"Ю12 00 52966850 191237255255 06802325600\n"
		L"Ю12 00 56075419 187239255247 0690000240ПГK   \n"
		L"Ю12 00 54119219 239239255247 324180241Э     \n"
		L"Ю12 00 58383183 133225092123 00031010592023562020310138759521223101000283903562128\n"
		L"Ю12 00 58566316 169225124115 0680140030004022730955935106346003053800     132\n"
		L"Ю12 00 59891390 191239255255 0700233\n"
		L"Ю12 00 95273314 191237255255 06102110950\n"
		L"Ю12 00 95927208 189239255255 070020234\n"
		L"Ю12 00 95216115 255239255255 0224\n"
		L"Ю12 00 95931119 255239255255 0228\n"
		L"Ю12 00 95464723 189239255255 061040205\n"
		L"Ю12 00 95992004 189239255255 068020232\n"
		L"Ю12 00 95181566 191239255255 0700227:)]]><Locomotive Series='ТЭМ14' NumLoc='83' Depo='2801' Consec='1'>"
		L"<Crew EngineDriver='EPMOЛEHKO' TabNum='17247' Tim_Beg='2021-12-11 09:00' />"
		L"</Locomotive>"
		L"</PostPoint>"
		L"<EsrList>"
		L"<Station esr='31000' />"
		L"</EsrList>"
		L"</A2F_HINT>"
		);

	UtIdentifyCategoryList identifyCategory;

	Context context(Hem::Chance::System(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	//Context context(Hem::Chance::System(), m_chart->getEsrGuide(), m_chart->getGuessTransciever(), m_chart->getUncontrolledStations(), identifyCategory, m_chart->getTopology(), nullptr, 0);
	Hem::ApplierCarryOut::aeAttach_AsoupToAsoup att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, asoupQueue, context, bill.getHeadAsoupThrows(), bill.getTailAsoupThrows(), false );
	att.Action();
}

void TC_Hem_aeAttach::AttachDiffNumbers()
{
    auto& hl = *happen_layer;
	auto& al = *asoup_layer;

	al.createPath(L"<AsoupEvent create_time='20221117T213000Z' name='Departure' Bdg='ASOUP 1042_1p3w[09630]' index='0963-005-5970' num='2707' length='56' weight='1226' through='Y' waynum='3' parknum='1' dirTo='09000' adjTo='09640'>"
        L"<![CDATA[(:1042 909/000+09630 2707 0963 005 5970 03 09000 17 11 23 30 01/03 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 056 01226 00000 212 95512943 98237241 000 000 00 000 053.90 000 053 000 95 000 053 000\n"
		L"Ю3 530 00009481 1 21 14 0000 00000 PAVLJUCHENKO\n"
		L"Ю3 530 00009482 9\n"
		L"Ю4 11290 2 58 000 053.90 000 053 95 000 053 \n"
		L"Ю12 00 95512943 1 000 59700 01100 8192 300 00 00 00 00 00 0232 12 95 0950 04 106 59700 00000 11290 58 09630 34 00000 0000 020 1 1280 09593  128 000 00000000\n"
		L"Ю12 00 95672697 255239255255 0230\n"
		L"Ю12 00 95919403 255239127255 0234102\n"
		L"Ю12 00 91109405 255239127255 0230106\n"
		L"Ю12 00 95461489 255239255255 0232\n"
		L"Ю12 00 95462560 255255255255 \n"
		L"Ю12 00 95517942 255239255255 0230\n"
		L"Ю12 00 59452771 255237255255 02335935\n"
		L"Ю12 00 95349619 255237255255 02340950\n"
		L"Ю12 00 95926838 255239255255 0229\n"
		L"Ю12 00 55016349 251237255255 40002235935\n"
		L"Ю12 00 95992152 255237255255 02300950\n"
		L"Ю12 00 95497483 255239255255 0232\n"
		L"Ю12 00 95869194 255255255255 \n"
		L"Ю12 00 95881827 255255255255 \n"
		L"Ю12 00 95883898 255255255255 \n"
		L"Ю12 00 95881173 255255255255 \n"
		L"Ю12 00 95897294 255239255255 0229\n"
		L"Ю12 00 91108068 255239255255 0232\n"
		L"Ю12 00 91101378 255255255255 \n"
		L"Ю12 00 95197240 251239255255 5000233\n"
		L"Ю12 00 95548111 255239127255 0235102\n"
		L"Ю12 00 95428314 255239127255 0229106\n"
		L"Ю12 00 95287488 255239255255 0228\n"
		L"Ю12 00 95987848 255239255255 0229\n"
		L"Ю12 00 95297636 255239255255 0232\n"
		L"Ю12 00 95282943 255255255255 \n"
		L"Ю12 00 95615092 255239255255 0230\n"
		L"Ю12 00 95734166 255239255255 0232\n"
		L"Ю12 00 95399432 255239255255 0231\n"
		L"Ю12 00 95740809 251239255255 0000230\n"
		L"Ю12 00 59893586 251237255255 30002275935\n"
		L"Ю12 00 95927166 255237255255 02300950\n"
		L"Ю12 00 95788766 251239127255 4000234102\n"
		L"Ю12 00 95341301 255239127255 0233106\n"
		L"Ю12 00 95670691 255239255255 0230\n"
		L"Ю12 00 91105411 255255255255 \n"
		L"Ю12 00 95527818 255239255255 0232\n"
		L"Ю12 00 95995148 255239255255 0230\n"
		L"Ю12 00 95564977 255255255255 \n"
		L"Ю12 00 95893830 255239255255 0229\n"
		L"Ю12 00 95522256 255239255255 0232\n"
		L"Ю12 00 59097212 255237255255 02335935\n"
		L"Ю12 00 95657599 251237255255 50002300950\n"
		L"Ю12 00 95512240 255239255255 0229\n"
		L"Ю12 00 91106625 255239255255 0230\n"
		L"Ю12 00 95836664 255255255255 \n"
		L"Ю12 00 91106708 255255255255 \n"
		L"Ю12 00 95589032 255239127255 0235102\n"
		L"Ю12 00 95940243 255239127255 0229106\n"
		L"Ю12 00 98236326 255239127255 0235102\n"
		L"Ю12 00 98237357 255255255255 \n"
		L"Ю12 00 98237241 255255255255 :)]]><Locomotive Series='2ТЭ116' NumLoc='948' Consec='1' CarrierCode='2'>"
		L"<Crew EngineDriver='PAVLJUCHENKO' Tim_Beg='2022-11-17 21:14' />"
		L"</Locomotive>"
		L"</AsoupEvent>");
	
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20221117T214736Z' name='Form' Bdg='209SP+[09042]' index='0963-005-5970' num='2707' length='56' weight='1226' through='Y'>"
          L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
          L"<Locomotive Series='2ТЭ116' NumLoc='948' Consec='1' CarrierCode='2'>"
            L"<Crew EngineDriver='PAVLJUCHENKO' Tim_Beg='2022-11-17 21:14' />"
          L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20221117T214736Z' name='Departure' Bdg='209SP+[09042]'>"
          L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20221117T215206Z' name='Transition' Bdg='1C[09500]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20221117T220811Z' name='Arrival' Bdg='1C[09500]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20221117T221319Z' name='Departure' Bdg='1C[09500]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20221117T222811Z' name='Arrival' Bdg='5JC[09006]' waynum='5' parknum='1' index='0963-005-5970' num='2708' length='56' weight='1226' through='Y'>"
		L"<Locomotive Series='2ТЭ116' NumLoc='948' Consec='1' CarrierCode='2'>"
		L"<Crew EngineDriver='PAVLJUCHENKO' Tim_Beg='2022-11-17 21:14' />"
		L"</Locomotive>"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221117T232917Z' name='Departure' Bdg='5JC[09006]' waynum='5' parknum='1' />"
		L"<SpotEvent create_time='20221117T233431Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' />"
		L"<SpotEvent create_time='20221117T233700Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' optCode='09000:09420' />"
		L"<SpotEvent create_time='20221117T235020Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' />"
		L"<SpotEvent create_time='20221118T000735Z' name='Transition' Bdg='1C[09410]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20221118T002637Z' name='Transition' Bdg='2C[09400]' waynum='2' parknum='1' optCode='09380:09400' />"
		L"<SpotEvent create_time='20221118T005249Z' name='Transition' Bdg='2C[09380]' waynum='2' parknum='1' optCode='09370:09380' />"
		L"<SpotEvent create_time='20221118T010350Z' name='Transition' Bdg='2C[09370]' waynum='2' parknum='1' />"
        L"</HemPath>"
        );

	{
		CPPUNIT_ASSERT( hl.path_count() == 1 );
		auto pathList = al.UT_GetEvents();
		CPPUNIT_ASSERT( pathList.size() == 1 );
		auto asoup = *pathList.cbegin();
		CPPUNIT_ASSERT( !asoup_layer->IsServed( asoup ) );
	}

    Hem::Bill bill = createBill(
		L"<A2F_HINT Hint='cmd_edit' issue_moment='20221118T023325Z'>"
		L"<PrePoint layer='asoup' Bdg='ASOUP 1042_1p3w[09630]' name='Departure' create_time='20221117T213000Z' index='0963-005-5970' num='2707' length='56' weight='1226' through='Y' waynum='3' parknum='1'>"
        L"<![CDATA[(:1042 909/000+09630 2707 0963 005 5970 03 09000 17 11 23 30 01/03 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 056 01226 00000 212 95512943 98237241 000 000 00 000 053.90 000 053 000 95 000 053 000\n"
		L"Ю3 530 00009481 1 21 14 0000 00000 PAVLJUCHENKO\n"
		L"Ю3 530 00009482 9\n"
		L"Ю4 11290 2 58 000 053.90 000 053 95 000 053 \n"
		L"Ю12 00 95512943 1 000 59700 01100 8192 300 00 00 00 00 00 0232 12 95 0950 04 106 59700 00000 11290 58 09630 34 00000 0000 020 1 1280 09593  128 000 00000000\n"
		L"Ю12 00 95672697 255239255255 0230\n"
		L"Ю12 00 95919403 255239127255 0234102\n"
		L"Ю12 00 91109405 255239127255 0230106\n"
		L"Ю12 00 95461489 255239255255 0232\n"
		L"Ю12 00 95462560 255255255255 \n"
		L"Ю12 00 95517942 255239255255 0230\n"
		L"Ю12 00 59452771 255237255255 02335935\n"
		L"Ю12 00 95349619 255237255255 02340950\n"
		L"Ю12 00 95926838 255239255255 0229\n"
		L"Ю12 00 55016349 251237255255 40002235935\n"
		L"Ю12 00 95992152 255237255255 02300950\n"
		L"Ю12 00 95497483 255239255255 0232\n"
		L"Ю12 00 95869194 255255255255 \n"
		L"Ю12 00 95881827 255255255255 \n"
		L"Ю12 00 95883898 255255255255 \n"
		L"Ю12 00 95881173 255255255255 \n"
		L"Ю12 00 95897294 255239255255 0229\n"
		L"Ю12 00 91108068 255239255255 0232\n"
		L"Ю12 00 91101378 255255255255 \n"
		L"Ю12 00 95197240 251239255255 5000233\n"
		L"Ю12 00 95548111 255239127255 0235102\n"
		L"Ю12 00 95428314 255239127255 0229106\n"
		L"Ю12 00 95287488 255239255255 0228\n"
		L"Ю12 00 95987848 255239255255 0229\n"
		L"Ю12 00 95297636 255239255255 0232\n"
		L"Ю12 00 95282943 255255255255 \n"
		L"Ю12 00 95615092 255239255255 0230\n"
		L"Ю12 00 95734166 255239255255 0232\n"
		L"Ю12 00 95399432 255239255255 0231\n"
		L"Ю12 00 95740809 251239255255 0000230\n"
		L"Ю12 00 59893586 251237255255 30002275935\n"
		L"Ю12 00 95927166 255237255255 02300950\n"
		L"Ю12 00 95788766 251239127255 4000234102\n"
		L"Ю12 00 95341301 255239127255 0233106\n"
		L"Ю12 00 95670691 255239255255 0230\n"
		L"Ю12 00 91105411 255255255255 \n"
		L"Ю12 00 95527818 255239255255 0232\n"
		L"Ю12 00 95995148 255239255255 0230\n"
		L"Ю12 00 95564977 255255255255 \n"
		L"Ю12 00 95893830 255239255255 0229\n"
		L"Ю12 00 95522256 255239255255 0232\n"
		L"Ю12 00 59097212 255237255255 02335935\n"
		L"Ю12 00 95657599 251237255255 50002300950\n"
		L"Ю12 00 95512240 255239255255 0229\n"
		L"Ю12 00 91106625 255239255255 0230\n"
		L"Ю12 00 95836664 255255255255 \n"
		L"Ю12 00 91106708 255255255255 \n"
		L"Ю12 00 95589032 255239127255 0235102\n"
		L"Ю12 00 95940243 255239127255 0229106\n"
		L"Ю12 00 98236326 255239127255 0235102\n"
		L"Ю12 00 98237241 255255255255 :)]]><Locomotive Series='2ТЭ116' NumLoc='948' Consec='1' CarrierCode='2'>"
		L"<Crew EngineDriver='PAVLJUCHENKO' Tim_Beg='2022-11-17 21:14' />"
		L"</Locomotive>"
		L"</PrePoint>"
		L"<Action code='Attach' District='09000-09640' DistrictRight='Y' SaveLog='Y' />"
		L"<PostPoint name='Form' Bdg='209SP+[09042]' create_time='20221117T214736Z' index='0963-005-5970' num='2707' length='56' weight='1226' through='Y'>"
		L"<Locomotive Series='2ТЭ116' NumLoc='948' Consec='1' CarrierCode='2'>"
		L"<Crew EngineDriver='PAVLJUCHENKO' Tim_Beg='2022-11-17 21:14' />"
		L"</Locomotive>"
		L"</PostPoint>"
		L"<EsrList>"
		L"<Station esr='09630' />"
		L"</EsrList>"
		L"</A2F_HINT>"
        );

	std::vector<unsigned int> vPath;
	vPath.push_back(9630);
	vPath.push_back(9640);
	vPath.push_back(9650);
	vPath.push_back(9042);
	setTopologyPath(9630, 9042, vPath);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeAttach att(  AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, bill );
    att.Action();

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series("name", "Departure               Transition   Transition   Arrival      Departure    Transition  Arrival  Departure  Arrival    Departure  Transition Transition Transition Transition Transition Transition Transition" ) );
	CPPUNIT_ASSERT( hl.exist_series("Bdg",  "ASOUP 1042_1p3w[09630]   ?[09640]     ?[09650]   209SP+[09042] 209SP+[09042] 1C[09500] 1C[09500] 1C[09500]  5JC[09006] 5JC[09006] 3C[09008]  2AC[09000]  2C[09420] 1C[09410] 2C[09400]   2C[09380] 2C[09370]" ) );
    CPPUNIT_ASSERT( hl.exist_series("num",  "2707                       -            -           -            -           -           -            -       2708         -           -            -          -       -          -         -           -     " ) );
}

void TC_Hem_aeAttach::FindAndGluePathesByNumber_7077_1()
{
	auto& hl = *happen_layer;
	auto& al = *asoup_layer;
	// первая нить
	hl.createPath(
		L"<HemPath>"
		L"<SpotEvent create_time='20221221T183613Z' name='Arrival' Bdg='2C[11432]' waynum='2' parknum='1' index='1100-413-0900' num='2719' length='56' weight='3424' net_weight='2189' through='Y' />"
		L"<SpotEvent create_time='20221221T184703Z' name='Departure' Bdg='2C[11432]' waynum='2' parknum='1' optCode='11431:11432' />"
		L"<SpotEvent create_time='20221221T184759Z' name='Station_exit' Bdg='PAP[11432]'>"
		L"<rwcoord picketing1_val='349~940' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T184759Z' name='Span_move' Bdg='P2P[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='349~610' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='349~940' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T184848Z' name='Span_move' Bdg='P4P[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='349~610' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='349~190' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T184848Z' name='Span_stopping_begin' Bdg='P4P[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='349~610' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='349~190' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T185620Z' name='Death' Bdg='2C[11432]' waynum='2' parknum='1' />"
		L"</HemPath>"
		);
	// вторая нить
	hl.createPath(
		L"<HemPath>"
		L"<SpotEvent create_time='20221221T185651Z' name='Form' Bdg='N17P_SER[11431:11432]' waynum='1' index='1100-413-0900' num='2719' length='56' weight='3424' net_weight='2189' through='Y'>"
		L"<rwcoord picketing1_val='345~346' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='345~943' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T185651Z' name='Span_move' Bdg='N13P_SER[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='344~346' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='344~746' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T185725Z' name='Span_move' Bdg='N11P_SER[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='344~346' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='344~46' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T185751Z' name='Span_move' Bdg='N9P_SER[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='344~46' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='343~746' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T185815Z' name='Span_move' Bdg='N7P_SER[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='343~446' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='343~746' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T185836Z' name='Span_move' Bdg='N5P_SER[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='343~26' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='343~446' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T185909Z' name='Span_move' Bdg='N3P_SER[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='342~601' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='343~26' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T185946Z' name='Span_move' Bdg='N1P_SER[11431:11432]' waynum='1'>"
		L"<rwcoord picketing1_val='342~401' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='342~601' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T190004Z' name='Station_entry' Bdg='NAP[11431]'>"
		L"<rwcoord picketing1_val='342~401' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T190100Z' name='Transition' Bdg='1AC[11431]' waynum='1' parknum='2' optCode='11430:11431' />"
		L"</HemPath>"
		);
	CPPUNIT_ASSERT( hl.path_count() == 2 );
	CPPUNIT_ASSERT( happen_layer->exist_path_size(7) );
	CPPUNIT_ASSERT( happen_layer->exist_path_size(10) );

// 	std::vector<unsigned int> vPath;
// 	vPath.push_back(9630);
// 	vPath.push_back(9640);
// 	vPath.push_back(9650);
// 	vPath.push_back(9042);
// 	setTopologyPath(9630, 9042, vPath);
	Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	AsoupToSpot::PathPtr pathFirst, pathSecond;
	{
		auto container = hl.GetWriteAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size()==2 );
		auto path1 = *pathSet.begin();
		auto path2 = *pathSet.rbegin();
		if ( !path1->isDeathPath() )
			std::swap( path1, path2 );
		CPPUNIT_ASSERT( path1->isDeathPath() );
		pathFirst = container->getPath( path1->GetLastEvent() );
		pathSecond = container->getPath( path2->GetFirstEvent() );
	}

	{
	CPPUNIT_ASSERT( pathFirst && pathFirst->GetEventsCount()==7 );
	pathFirst = AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation( hl.GetWriteAccess(), al.GetWriteAccess(), pathFirst, context );
	CPPUNIT_ASSERT( pathFirst && pathFirst->GetEventsCount()==7 );
	CPPUNIT_ASSERT( hl.path_count() == 2 );
	}

	{
		CPPUNIT_ASSERT( pathSecond && pathSecond->GetEventsCount()==10 );
		pathSecond = AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation( hl.GetWriteAccess(), al.GetWriteAccess(), pathSecond, context );

		CPPUNIT_ASSERT( pathSecond && pathSecond->GetEventsCount()==10 );
		CPPUNIT_ASSERT( hl.path_count() == 2 );
	}
}

void TC_Hem_aeAttach::FindAndGluePathesByNumber_7077_2()
{
	auto& hl = *happen_layer;
	auto& al = *asoup_layer;
	// первая нить
	hl.createPath(
		L"<HemPath>"
		L"<SpotEvent create_time='20221221T160049Z' name='Form' Bdg='1C[09010]' waynum='1' parknum='1' index='' num='877' length='6' weight='183' mvps='Y'>"
		L"<Locomotive Series='ДР1АМ' NumLoc='222' CarrierCode='24'>"
		L"<Crew EngineDriver='Rudņickis' Tim_Beg='2022-12-21 18:25' PersonMode='1' />"
		L"</Locomotive>"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T162555Z' name='Departure' Bdg='1C[09010]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20221221T163001Z' name='Arrival' Bdg='2p[09100]' waynum='2' parknum='1' />"
		L"<SpotEvent create_time='20221221T163114Z' name='Departure' Bdg='2p[09100]' waynum='2' parknum='1' />"
		L"<SpotEvent create_time='20221221T163653Z' name='Transition' Bdg='KPU13A/6A[09104]'>"
		L"<rwcoord picketing1_val='5~750' picketing1_comm='Торнянкалнс-Елгава' />"
		L"<rwcoord picketing1_val='8~800' picketing1_comm='Торнянкалнс-Елгава' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T164554Z' name='Arrival' Bdg='4p[09150]' waynum='4' parknum='1' />"
		L"<SpotEvent create_time='20221221T164827Z' name='Departure' Bdg='4Ap[09150]' waynum='4' parknum='1' optCode='09150:09160' />"
		L"<SpotEvent create_time='20221221T165542Z' name='Transition' Bdg='2p[09160]' waynum='2' parknum='1' optCode='09160:09162' />"
		L"<SpotEvent create_time='20221221T170114Z' name='Arrival' Bdg='AGP[09162]' waynum='1' parknum='1'>"
		L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T170357Z' name='Departure' Bdg='AGP[09162]' waynum='1' parknum='1' optCode='09180:09280'>"
		L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T170600Z' name='Arrival' Bdg='1C[09180]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20221221T172449Z' name='Death' Bdg='1C[09180]' waynum='1' parknum='1' />"
		L"</HemPath>"
		);
	// вторая нить
	hl.createPath(
		L"<HemPath>"
		L"<SpotEvent create_time='20221221T171500Z' name='Span_move' Bdg='#877[09180:09280]' index='' num='877' mvps='Y'>"
		L"<rwcoord picketing1_val='54~463' picketing1_comm='Реньге' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221221T171900Z' name='Transition' Bdg='№877[09280]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20221221T171900Z' name='Death' Bdg='№877[09280]' waynum='1' parknum='1' />"
		L"</HemPath>"
		);
	CPPUNIT_ASSERT( hl.path_count() == 2 );
	CPPUNIT_ASSERT( happen_layer->exist_path_size(12) );
	CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );

// 		std::vector<unsigned int> vPath;
// 	 	vPath.push_back(9180);
// 	 	vPath.push_back(9280);
	 	setTopologyPath(9180, 9280/*, vPath*/);
	Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	AsoupToSpot::PathPtr pathFirst, pathSecond;
	{
		auto container = hl.GetWriteAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size()==2 );
		auto path1 = *pathSet.begin();
		auto path2 =  *std::next(pathSet.begin());
		if ( path1->GetEventsCount()==3 )
			std::swap(path1, path2);
		CPPUNIT_ASSERT( path1->GetEventsCount()==12 );
		pathFirst = container->getPath( path1->GetLastEvent() );
		pathSecond = container->getPath( path2->GetLastEvent() );
	}

	//проверяем склейку в обоих направлениях для двух нитей
	{
	CPPUNIT_ASSERT( pathFirst && pathFirst->GetEventsCount()==12 );
	pathFirst = AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation( hl.GetWriteAccess(), al.GetWriteAccess(), pathFirst, context );
	CPPUNIT_ASSERT( pathFirst );
	CPPUNIT_ASSERT( happen_layer->exist_path_size(12) );
	CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
	CPPUNIT_ASSERT( pathFirst->GetEventsCount()==12 );
	}

	{
	CPPUNIT_ASSERT( pathSecond && pathSecond->GetEventsCount()==3 );
	pathSecond = AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation( hl.GetWriteAccess(), al.GetWriteAccess(), pathSecond, context );
	CPPUNIT_ASSERT( happen_layer->exist_path_size(12) );
	CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
	CPPUNIT_ASSERT( pathSecond->GetEventsCount()==3 );
	}
}


void TC_Hem_aeAttach::GlueExplicitFormWithSpanMove_7081_3()
{
	UtUserChart chart;
	{
		attic::a_document doc;
		EsrGuide eg;
		doc.load_wide(
			L"<EsrGuide>"
			L"<EsrGuide kit='09813' name='StationA' picketing1_val='5~100' picketing1_comm='StationC' >"
			L"<Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
			L"<Way waynum='2' parknum='1' major='Y' />"
			L"<Way waynum='3' parknum='1' />"
			L"</EsrGuide>"
			L"<EsrGuide kit='09820' name='StationB' picketing1_val='0~1' picketing1_comm='StationC' >"
			L"<Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
			L"<Way waynum='2' parknum='1' major='Y' />"
			L"<Way waynum='3' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
			L"<Way waynum='4' parknum='1' passenger='Y' embarkation='Y' />"
			L"</EsrGuide>"
			L"<SpanGuide kit='09813:09820' orientation='odd'>"
			L"<Way wayNum='1'>"
			L"<rwcoord picketing1_val='4~200' picketing1_comm='StationC' />"
			L"<rwcoord picketing1_val='2~' picketing1_comm='StationC' />"
			L"</Way>"
			L"</SpanGuide>"
			L"</EsrGuide>");
		eg.load_from( doc.document_element() );
		chart.setupEsrGuide(eg);
	}

	UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( chart.getHappenLayer() );

	SpotDetails sd( TrainDescr( L"3014", L"0983-227-0918") );
	SpotEvent a( HCode::EXPLICIT_FORM, BadgeE(L"ASOUP 1042",EsrKit(9820)), time_from_iso("20230102T114600Z"), std::make_shared< SpotDetails>(sd) );

	hl.createPath(
		L"<HemPath>"
		L"<SpotEvent create_time='20230102T210853Z' name='Form' Bdg='PMN1IP[09813:09840]' waynum='1' index='' num='V3014' />"
		L"<SpotEvent create_time='20230102T210853Z' name='Station_entry' Bdg='PMNDP[09813]' />"
		L"</HemPath>"
		);
	CPPUNIT_ASSERT( hl.exist_series( "name", "Form  Station_entry" ) );

	setTopologyPath(9820, 9813);

	SpotEventPtr firstSpotPath;
	{
		auto container = hl.GetWriteAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size()==1 );
		auto path1 = *pathSet.begin();
		CPPUNIT_ASSERT ( path1->GetEventsCount()==2 );
		firstSpotPath = path1->GetFirstEvent();
	}
	Context context(sysChance, chart.getEsrGuide(), *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	Hem::ApplierCarryOut::aeAttach worker( AccessHappenAsoup(hl.base(), asoup_layer->base()), nullptr, context, a, *firstSpotPath, false );
	worker.Action();
	auto path = worker.getResult();
	CPPUNIT_ASSERT( path );
	CPPUNIT_ASSERT( path->GetEventsCount()==3 );
	auto firstSpot = path->GetFirstEvent();
	CPPUNIT_ASSERT( firstSpot );
	//проверяем наличие парка-пути (сгенерирован при склейке)
	CPPUNIT_ASSERT( firstSpot->GetDetails() && !firstSpot->GetDetails()->parkway.empty() );	
}

void TC_Hem_aeAttach::GlueExplicitFormWithSpanMove_7081_4()
{
	const std::string nsiBaseStr = 
		"<Test>"
		"<Junctions>"
		"<Technodes>"
		"<Union>"   
		"<Station esrCode='09820'/>"  
		"<Station esrCode='09840'/>"  
		"<Station esrCode='09850'/>"  
		"<Station esrCode='09880'/>"  
		"</Union>"
		"</Technodes>"
		"</Junctions>"
		"</Test>";

	std::shared_ptr<const NsiBasis> utNsi = std::make_shared<const NsiBasis>( UtNsiBasis(nsiBaseStr) );

		Hem::Bill bill = createBill( L"<A2F_HINT Hint='cmd_edit' issue_moment='20230102T220000Z'>"
		L"<PrePoint layer='asoup' Bdg='ASOUP 1042[09820]' name='ExplicitForm' create_time='20230102T114600Z' index='0983-227-0918' num='3014' length='57' weight='1580' divisional='Y'>"
        L"<![CDATA[(:1042 909/000+09830 3014 0983 227 0918 07 09820 02 01 13 46 00/00 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 057 01580 00000 248 95574562 57968505 000 000 00 000 062.60 000 001 000 70 000 055 000 73 000 033 000 90 000 006 000 95 000 003 000\n"
		L"Ю4 00000 0 00 000 031.70 000 028 73 000 028 90 000 003 \n"
		L"Ю4 09170 2 12 000 022.70 000 022 \n"
		L"Ю4 00000 1 01 000 009.60 000 001 70 000 005 73 000 005 90 000 003 95 000 003 \n"
		L"Ю4 09740 1 01 000 001.60 000 001 \n"
		L"Ю4 09170 1 01 000 005.70 000 005 73 000 005 \n"
		L"Ю4 09180 1 01 000 003.90 000 003 95 000 003 \n"
		L"Ю12 00 95574562 1 000 09180 02108 7051 000 00 00 00 00 00 0226 13 95 0950 04 106 09180 09180 00000 01 09830 34 00000 0000 025 0 0000 0982E  128 000 00000000\n"
		L"Ю12 00 95573911 255239255255 0220\n"
		L"Ю12 00 95486775 255255255255 \n"
		L"Ю12 00 52789054 199233093183 097401611285170234605600100090000102009836 \n"
		L"Ю12 00 91505677 199225085183 0000008104569803381190091416700900002402409826 \n"
		L"Ю12 00 91503946 255239255255 0327\n"
		L"Ю12 00 91503938 255255255255 \n"
		L"Ю12 00 73962219 199225085183 091705562270510243137007320860917001260250983E \n"
		L"Ю12 00 73962243 255255255255 \n"
		L"Ю12 00 73962102 255239255255 0245\n"
		L"Ю12 00 73962052 255239255255 0240\n"
		L"Ю12 00 73961930 255239255255 0244\n"
		L"Ю12 00 50733963 195229197167 12630211032441360025912570012550091701203024914009836 \n"
		L"Ю12 00 50719012 255239255255 0269\n"
		L"Ю12 00 51155687 255239255255 0247\n"
		L"Ю12 00 51259570 255239255255 0272\n"
		L"Ю12 00 51116465 255239255255 0247\n"
		L"Ю12 00 51155588 255239255255 0248\n"
		L"Ю12 00 50595925 255239255255 0247\n"
		L"Ю12 00 50613900 255239255255 0246\n"
		L"Ю12 00 50734037 255239255255 0260\n"
		L"Ю12 00 51251288 255255255255 \n"
		L"Ю12 00 57968927 251239255255 4600266\n"
		L"Ю12 00 51116275 255239255255 0247\n"
		L"Ю12 00 51252948 255239255255 0261\n"
		L"Ю12 00 51251106 255239255255 0260\n"
		L"Ю12 00 57968810 255239255255 0267\n"
		L"Ю12 00 51154953 255239255255 0247\n"
		L"Ю12 00 51252807 255239255255 0257\n"
		L"Ю12 00 57935157 255239255255 0267\n"
		L"Ю12 00 51260180 255239255255 0261\n"
		L"Ю12 00 75082628 195225199239 000002110359060600262117107910071000000000000\n"
		L"Ю12 00 75076000 255239255255 0254\n"
		L"Ю12 00 75082008 255239255255 0262\n"
		L"Ю12 00 73904419 255239255255 0244\n"
		L"Ю12 00 75081828 255239255255 0262\n"
		L"Ю12 00 75069104 255239255255 0258\n"
		L"Ю12 00 73137184 255239255255 0235\n"
		L"Ю12 00 75075937 255239255255 0260\n"
		L"Ю12 00 75082222 255239255255 0262\n"
		L"Ю12 00 73134256 255239255255 0230\n"
		L"Ю12 00 75068627 255239255255 0254\n"
		L"Ю12 00 73962649 255239255255 0245\n"
		L"Ю12 00 74055351 255239255255 0235\n"
		L"Ю12 00 74719329 255239255255 0248\n"
		L"Ю12 00 73173759 255239255255 0231\n"
		L"Ю12 00 73904351 255239255255 0246\n"
		L"Ю12 00 73136244 255239255255 0227\n"
		L"Ю12 00 75068700 255239255255 0258\n"
		L"Ю12 00 75069021 255239255255 0259\n"
		L"Ю12 00 75082081 255239255255 0262\n"
		L"Ю12 00 75069237 255239255255 0254\n"
		L"Ю12 00 75083204 255239255255 0262\n"
		L"Ю12 00 73124117 255239255255 0235\n"
		L"Ю12 00 73148744 255239255255 0230\n"
		L"Ю12 00 75069211 255239255255 0248\n"
		L"Ю12 00 75082503 255239255255 0262\n"
		L"Ю12 00 75076604 255239255255 0258\n"
		L"Ю12 00 75081901 255239255255 0262\n"
		L"Ю12 00 51259976 195225199239 126302110324415600267127057001255009170129140\n"
		L"Ю12 00 50734052 255239255255 0260\n"
		L"Ю12 00 57968505 255239255255 0266:)]]>"
        L"</PrePoint>"
		L"<Action code='Attach' District='09180-09820' DistrictRight='Y' />"
		L"<PostPoint layer='asoup' Bdg='ASOUP 1042_6p7w[09850]' name='Departure' create_time='20230102T205400Z' index='0983-227-0918' num='3014' length='57' weight='1580' divisional='Y' waynum='7' parknum='6'>"
        L"<![CDATA[(:1042 909/000+09830 3014 0983 227 0918 03 09750 02 01 22 54 06/07 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 057 01580 00000 248 95574562 57968505 000 000 00 000 062.60 000 001 000 70 000 055 000 73 000 033 000 90 000 006 000 95 000 003 000\n"
		L"Ю3 583 00000962 1 15 40 0000 00000 M-SAKNELS   \n"
		L"Ю3 583 00000941 9\n"
		L"Ю4 00000 0 00 000 031.70 000 028 73 000 028 90 000 003 \n"
		L"Ю4 09170 2 12 000 022.70 000 022 \n"
		L"Ю4 00000 1 01 000 009.60 000 001 70 000 005 73 000 005 90 000 003 95 000 003 \n"
		L"Ю4 09740 1 01 000 001.60 000 001 \n"
		L"Ю4 09170 1 01 000 005.70 000 005 73 000 005 \n"
		L"Ю4 09180 1 01 000 003.90 000 003 95 000 003 \n"
		L"Ю12 00 95574562 1 000 09180 02108 7051 000 00 00 00 00 00 0226 13 95 0950 04 106 09180 09180 00000 01 09830 34 00000 0000 025 0 0000 0982E  128 000 00000000\n"
		L"Ю12 00 95573911 255239255255 0220\n"
		L"Ю12 00 95486775 255255255255 \n"
		L"Ю12 00 52789054 199233093183 097401611285170234605600100090000102009836 \n"
		L"Ю12 00 91505677 199225085183 0000008104569803381190091416700900002402409826\n" 
		L"Ю12 00 91503946 255239255255 0327\n"
		L"Ю12 00 91503938 255255255255 \n"
		L"Ю12 00 73962219 199225085183 091705562270510243137007320860917001260250983E \n"
		L"Ю12 00 73962243 255255255255 \n"
		L"Ю12 00 73962102 255239255255 0245\n"
		L"Ю12 00 73962052 255239255255 0240\n"
		L"Ю12 00 73961930 255239255255 0244\n"
		L"Ю12 00 50733963 195229197167 12630211032441360025912570012550091701203024914009836 \n"
		L"Ю12 00 50719012 255239255255 0269\n"
		L"Ю12 00 51155687 255239255255 0247\n"
		L"Ю12 00 51259570 255239255255 0272\n"
		L"Ю12 00 51116465 255239255255 0247\n"
		L"Ю12 00 51155588 255239255255 0248\n"
		L"Ю12 00 50595925 255239255255 0247\n"
		L"Ю12 00 50613900 255239255255 0246\n"
		L"Ю12 00 50734037 255239255255 0260\n"
		L"Ю12 00 51251288 255255255255 \n"
		L"Ю12 00 57968927 251239255255 4600266\n"
		L"Ю12 00 51116275 255239255255 0247\n"
		L"Ю12 00 51252948 255239255255 0261\n"
		L"Ю12 00 51251106 255239255255 0260\n"
		L"Ю12 00 57968810 255239255255 0267\n"
		L"Ю12 00 51154953 255239255255 0247\n"
		L"Ю12 00 51252807 255239255255 0257\n"
		L"Ю12 00 57935157 255239255255 0267\n"
		L"Ю12 00 51260180 255239255255 0261\n"
		L"Ю12 00 75082628 195225199239 000002110359060600262117107910071000000000000\n"
		L"Ю12 00 75076000 255239255255 0254\n"
		L"Ю12 00 75082008 255239255255 0262\n"
		L"Ю12 00 73904419 255239255255 0244\n"
		L"Ю12 00 75081828 255239255255 0262\n"
		L"Ю12 00 75069104 255239255255 0258\n"
		L"Ю12 00 73137184 255239255255 0235\n"
		L"Ю12 00 75075937 255239255255 0260\n"
		L"Ю12 00 75082222 255239255255 0262\n"
		L"Ю12 00 73134256 255239255255 0230\n"
		L"Ю12 00 75068627 255239255255 0254\n"
		L"Ю12 00 73962649 255239255255 0245\n"
		L"Ю12 00 74055351 255239255255 0235\n"
		L"Ю12 00 74719329 255239255255 0248\n"
		L"Ю12 00 73173759 255239255255 0231\n"
		L"Ю12 00 73904351 255239255255 0246\n"
		L"Ю12 00 73136244 255239255255 0227\n"
		L"Ю12 00 75068700 255239255255 0258\n"
		L"Ю12 00 75069021 255239255255 0259\n"
		L"Ю12 00 75082081 255239255255 0262\n"
		L"Ю12 00 75069237 255239255255 0254\n"
		L"Ю12 00 75083204 255239255255 0262\n"
		L"Ю12 00 73124117 255239255255 0235\n"
		L"Ю12 00 73148744 255239255255 0230\n"
		L"Ю12 00 75069211 255239255255 0248\n"
		L"Ю12 00 75082503 255239255255 0262\n"
		L"Ю12 00 75076604 255239255255 0258\n"
		L"Ю12 00 75081901 255239255255 0262\n"
		L"Ю12 00 51259976 195225199239 126302110324415600267127057001255009170129140\n"
		L"Ю12 00 50734052 255239255255 0260\n"
		L"Ю12 00 57968505 255239255255 0266:)]]>"
        L"<Locomotive Series='2М62УМ' NumLoc='96' Consec='1' CarrierCode='4'>"
		L"<Crew EngineDriver='M-SAKNELS' Tim_Beg='2023-01-02 15:40' />"
		L"</Locomotive>"
		L"<Locomotive Series='2М62УМ' NumLoc='94' Consec='9' CarrierCode='4'>"
		L"<Crew EngineDriver='M-SAKNELS' Tim_Beg='2023-01-02 15:40' />"
		L"</Locomotive>"
		L"</PostPoint>"
		L"<EsrList>"
		L"<Station esr='09820' />"
		L"<Station esr='09840' />"
		L"<Station esr='09859' />"
		L"<Station esr='09850' />"
		L"</EsrList>"
		L"</A2F_HINT>"
		);

	UtIdentifyCategoryList identifyCategory;

	Context context(Hem::Chance::System(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, utNsi, topology, nullptr, 0);
	Hem::ApplierCarryOut::aeAttach_AsoupToAsoup att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, asoupQueue, context, bill.getHeadAsoupThrows(), bill.getTailAsoupThrows(), false );
	att.Action();
	{
		auto container = happen_layer->GetWriteAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size()==1 );
		auto path = *pathSet.begin();
		CPPUNIT_ASSERT( path->GetEventsCount()==3 );
		auto spot = path->GetFirstEvent();
		CPPUNIT_ASSERT( spot->GetBadge()==BadgeE(L"ASOUP 1042_6p7w", EsrKit(9850)) );
		CPPUNIT_ASSERT( spot->GetDetails() && spot->GetDetails()->parkway==ParkWayKit(6,7) && spot->GetTime()==time_from_iso("20230102T114600Z") );
	}
}

void TC_Hem_aeAttach::Attach_ManualExtentBack_7213()
{
    // первая нить
    happen_layer->createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20230417T195100Z' name='ExplicitForm' Bdg='ASOUP 1042[09400]' waynum='1' parknum='1' index='0940-617-0938' num='8211' length='2' weight='26' net_weight='4' stone='Y'>"
        L"    <Locomotive Series='АДМ' NumLoc='1037' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='KOBJAKOVS' Tim_Beg='2023-04-17 19:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20230417T195639Z' name='Departure' Bdg='1C[09400]' waynum='1' parknum='1' index='0940-617-0938' num='8212' length='2' weight='26' net_weight='4' stone='Y'>"
        L"    <Locomotive Series='АДМ' NumLoc='1037' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='KOBJAKOVS' Tim_Beg='2023-04-17 19:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20230417T195756Z' name='Wrong_way' Bdg='1P_L[09380:09400]' waynum='1'>"
        L"    <rwcoord picketing1_val='54~' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='52~' picketing1_comm='Рига-Зилупе' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20230417T200900Z' name='Disform' Bdg='ASOUP 1042_1p3w[09400]' waynum='3' parknum='1' index='0940-617-0938' num='8211' length='2' weight='26' net_weight='4' stone='Y'>"
        L"    <Locomotive Series='АДМ' NumLoc='1037' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='KOBJAKOVS' Tim_Beg='2023-04-17 19:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"</HemPath>"
    );
    // вторая нить - не существует
    // задание
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20230417T234415Z'>"
        L"  <PrePoint name='Disform' Bdg='ASOUP 1042_1p3w[09400]' create_time='20230417T200900Z' index='0940-617-0938' num='8211' length='2' weight='26' net_weight='4' stone='Y' waynum='3' parknum='1'>"
        L"    <Locomotive Series='АДМ' NumLoc='1037' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='KOBJAKOVS' Tim_Beg='2023-04-17 19:00' />"
        L"    </Locomotive>"
        L"  </PrePoint>"
        L"  <Action code='PushBack' District='09000-11760' DistrictRight='Y' FixBound='20230415T161400Z' />"
        L"  <PostPoint name='Span_move' Bdg='?[09380:09400]' create_time='20230417T202100Z'>"
        L"    <rwcoord picketing1_val='63~868' picketing1_comm='Рига-Зилупе' />"
        L"  </PostPoint>"
        L"  <EsrList>"
        L"    <Station esr='09400' />"
        L"  </EsrList>"
        L"</A2F_HINT>"
    );

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "ExplicitForm      Departure Wrong_way         Disform                " ) ); 
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "ASOUP 1042[09400] 1C[09400] 1P_L[09380:09400] ASOUP 1042_1p3w[09400] " ) );

    Context context( bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0 );
    aeAttach att( AccessHappenAsoup( happen_layer->base(), asoup_layer->base() ), nullptr, context, bill );
    att.Action();
    collectUndo();

    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "ExplicitForm      Departure Wrong_way         Span_move      Death          " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "ASOUP 1042[09400] 1C[09400] 1P_L[09380:09400] ?[09380:09400] ?[09380:09400] " ) );
    CPPUNIT_ASSERT( implementUndo() );
    CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer->exist_series( "name", "ExplicitForm      Departure Wrong_way         Disform                " ) );
    CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg", "ASOUP 1042[09400] 1C[09400] 1P_L[09380:09400] ASOUP 1042_1p3w[09400] " ) );
}

// void TC_Hem_aeAttach::AsoupLinkDeparture_SpanMove_6262()
// {
//     // первая нить
//     happen_layer->createPath( 
//         L"<HemPath>"
//         <SpotEvent create_time="20210328T064600Z" name="ExplicitForm" Bdg="ASOUP 1042[11310]" waynum="11" parknum="1" index="1131-079-0918" num="2483" length="57" weight="4973" net_weight="3359" through="Y">
//         <feat_texts typeinfo="Т" />
//         <Locomotive Series="2ТЭ10М" NumLoc="3422" Consec="1" CarrierCode="4">
//         <Crew EngineDriver="ANISIMOV" Tim_Beg="2021-03-28 12:50" />
//         </Locomotive>
//         </SpotEvent>
//         <SpotEvent create_time="20210328T112006Z" name="Departure" Bdg="11C[11310]" waynum="11" parknum="1" />
//         <SpotEvent create_time="20210328T112554Z" name="Transition" Bdg="701SP[11321]">
//         <rwcoord picketing1_val="222~474" picketing1_comm="Рига-Зилупе" />
//         </SpotEvent>
//         <SpotEvent create_time="20210328T114117Z" name="Transition" Bdg="1C[11311]" waynum="1" parknum="1" optCode="11311:11380" />
//         <SpotEvent create_time="20210328T115506Z" name="Transition" Bdg="1C[11380]" waynum="1" parknum="1" optCode="11380:11381" />
//         <SpotEvent create_time="20210328T120736Z" name="Transition" Bdg="1C[11381]" waynum="1" parknum="1" optCode="11381:11390" />
//         <SpotEvent create_time="20210328T121618Z" name="Transition" Bdg="1C[11390]" waynum="1" parknum="1" optCode="11390:11391" />
//         <SpotEvent create_time="20210328T123319Z" name="Transition" Bdg="1C[11391]" waynum="1" parknum="1" optCode="11391:11401" />
//         <SpotEvent create_time="20210328T124449Z" name="Transition" Bdg="1C[11401]" waynum="1" parknum="1" optCode="11401:11412" />
//         <SpotEvent create_time="20210328T125709Z" name="Transition" Bdg="1C[11412]" waynum="1" parknum="1" optCode="11412:11420" />
//         <SpotEvent create_time="20210328T131856Z" name="Transition" Bdg="4C[11420]" waynum="4" parknum="1" />
//         <SpotEvent create_time="20210328T132201Z" name="Death" Bdg="PJP[11420]">
//         <rwcoord picketing1_val="300~341" picketing1_comm="Вентспилс" />
//         </SpotEvent>
//         L"</HemPath>"
//         );
//     // вторая нить 
//     happen_layer->createPath( 
//         L"<HemPath>"
//         L"<SpotEvent create_time='20210303T123234Z' name='Form' Bdg='15P_O[09400:09410]' waynum='1'>"
//         L"<rwcoord picketing1_val='49~' picketing1_comm='Рига-Зилупе' />"
//         L"<rwcoord picketing1_val='50~' picketing1_comm='Рига-Зилупе' />"
//         L"</SpotEvent>"
//         L"<SpotEvent create_time='20210303T123234Z' name='Entra_stopping' Bdg='17P_O[09400:09410]' waynum='1'>"
//         L"<rwcoord picketing1_val='51~' picketing1_comm='Рига-Зилупе' />"
//         L"<rwcoord picketing1_val='50~' picketing1_comm='Рига-Зилупе' />"
//         L"</SpotEvent>"
//         L"<SpotEvent create_time='20210303T123903Z' name='Arrival' Bdg='5C15+[09400]' waynum='5' parknum='1' />"
//         L"</HemPath>"
//         );
// 
//     // 
//     // выполняем билль
//     Hem::Bill bill = createBill(
//         L"<A2F_HINT Hint='cmd_edit' issue_moment='20210303T124236Z'>"
//         L"<PrePoint name='Death' Bdg='15P_O[09400:09410]' create_time='20210303T113603Z' waynum='1'>"
//         L"<rwcoord picketing1_val='49~500' picketing1_comm='Рига-Зилупе' />"
//         L"</PrePoint>"
//         L"<Action code='Attach' District='09000-11760' DistrictRight='Y' FixBound='20210303T054700Z' />"
//         L"<PostPoint name='Form' Bdg='15P_O[09400:09410]' create_time='20210303T123234Z' waynum='1'>"
//         L"<rwcoord picketing1_val='49~500' picketing1_comm='Рига-Зилупе' />"
//         L"</PostPoint>"
//         L"<EsrList>"
//         L"<Station esr='09400' />"
//         L"</EsrList>"
//         L"</A2F_HINT>"
//         );
// 
//     // до
//     CPPUNIT_ASSERT( happen_layer->path_count()==2 );
//     CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
//     CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
// 
//     Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, topology, nullptr, 0);
//     aeAttach att( AccessHappenAsoup(happen_layer->base(), asoup_layer->base()), nullptr, context, bill );
//     att.Action();
//     collectUndo();
// 
//     // после
//     CPPUNIT_ASSERT( happen_layer->path_count() == 1 );
//     CPPUNIT_ASSERT( happen_layer->exist_path_size(6) );
//     CPPUNIT_ASSERT( happen_layer->exist_series( "name", "Form         Departure   Span_stopping_begin  Span_stopping_end  Entra_stopping     Arrival " ) );
//     CPPUNIT_ASSERT( happen_layer->exist_series( "Bdg",  "5C15+[09400] 5C15+[09400] 15P_O[09400:09410] 15P_O[09400:09410] 17P_O[09400:09410]  5C15+[09400]" ) );
//     // а теперь после отката
//     CPPUNIT_ASSERT( implementUndo() );
//     CPPUNIT_ASSERT( happen_layer->path_count()==2 );
//     CPPUNIT_ASSERT( happen_layer->exist_path_size(5) );
//     CPPUNIT_ASSERT( happen_layer->exist_path_size(3) );
// }
// 
