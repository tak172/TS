#include "stdafx.h"

#include "TC_Hem_aeReplace.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "../Hem/GuessTransciever.h"
#include "../Hem/Bill.h"
#include "../Hem/UserChart.h"
#include "../Hem/LimitLayer.h"
#include "../Hem/ForbidLayer.h"
#include "../Hem/NoteLayer.h"
#include "../Hem/TaskScheduler.h"
#include "../Hem/FutureLayer.h"
#include "../Hem/Appliers/aeReplace.h"
#include "../helpful/LocoCharacteristics.h"
#include "../helpful/DistrictGuide.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/NsiBasis.h"

using namespace std;
using namespace Hem::ApplierCarryOut;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeReplace );

void TC_Hem_aeReplace::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_Hem_aeReplace::CoveredPreposition()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;

    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='A[11292:11300]' create_time='20151207T010347Z' waynum='1' num='1111' index=''>"
        L"    <rwcoord picketing1_val='250~500' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='248~800' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='B[11292:11300]' create_time='20151207T011856Z' waynum='1'>"
        L"    <rwcoord picketing1_val='255~600' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='253~900' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='C[11292:11300]' create_time='20151207T011857Z' waynum='1'>"
        L"    <rwcoord picketing1_val='255~600' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='253~900' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Form' cover='Y' Bdg='A[11292:11300]' create_time='20151207T010347Z' waynum='1' num='1111' index=''>"
        L"    <rwcoord picketing1_val='249~650' picketing1_comm='Посинь' />"
        L"  </PrePoint>"
        L"  <Action code='Replace'>"
        L"    <EventPoint name='Form' Bdg='?[11292:11300]' create_time='20151207T011440Z' num='1111' index=''>"
        L"      <rwcoord picketing1_val='248~579' picketing1_comm='Посинь' />"
        L"    </EventPoint>"
        L"  </Action>"
        L"  <PostPoint name='Span_move' Bdg='B[11292:11300]' create_time='20151207T011856Z' waynum='1'>"
        L"    <rwcoord picketing1_val='254~750' picketing1_comm='Посинь' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form           Span_move      Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "A[11292:11300] B[11292:11300] C[11292:11300]" ) );
    CPPUNIT_ASSERT( hl.exist_series( "num",  "     1111          -              -         " ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time",  "20151207T010347Z 20151207T011856Z 20151207T011857Z"  ) );

    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() );
    repl.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form           Span_move      Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "?[11292:11300] B[11292:11300] C[11292:11300]" ) );
    CPPUNIT_ASSERT( hl.exist_series( "num",  "     1111          -              -         " ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time",  "20151207T011440Z 20151207T011856Z 20151207T011857Z"  ) );
}

void TC_Hem_aeReplace::AppendBack()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170518T093253Z' name='Form' Bdg='PV1IP[09813:09820]' waynum='1' index='0982-084-1800' num='V2216' length='57' weight='1391' fretran='Y'>"
        L"    <rwcoord picketing1_val='4~200' picketing1_comm='Вентспилс' />"
        L"    <Locomotive Series='2ТЭ116' NumLoc='788' Dor='0' Depo='0' Kod='0'>"
        L"      <Crew />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170518T093450Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170518T125111Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20170518T135637Z' name='Arrival' Bdg='8C[09180]' waynum='8' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Arrival' Bdg='8C[09180]' create_time='20170518T135637Z' waynum='8' parknum='1' />"
        L"  <Action code='Replace'>"
        L"    <EventPoint name='Disform' Bdg='?[09180]' create_time='20170518T141037Z' waynum='8' parknum='1' />"
        L"  </Action>"
        L"  <PostPoint name='Arrival' cover='Y' Bdg='8C[09180]' create_time='20170518T135637Z' waynum='8' parknum='1' />"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form               Transition Transition Arrival " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "PV1IP[09813:09820] 1C[09813]  3C[09750]  8C[09180] " ) );
    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() );
    repl.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form               Transition Transition Arrival   Disform " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "PV1IP[09813:09820] 1C[09813]  3C[09750]  8C[09180] ?[09180]" ) );
}

void TC_Hem_aeReplace::RemoveInner()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170715T210803Z' name='Form' Bdg='11AC[09000]' waynum='11' parknum='1' />"
        L"  <SpotEvent create_time='20170715T210803Z' name='Departure' Bdg='11AC[09000]' waynum='11' parknum='1' />"
        L"  <SpotEvent create_time='20170715T211412Z' name='Wrong_way' Bdg='30JC[09000:09006]' waynum='30'>"
        L"    <rwcoord picketing1_val='6~651' picketing1_comm='Рига Пассажирская' />"
        L"    <rwcoord picketing1_val='5~771' picketing1_comm='Рига Пассажирская' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170715T211614Z' name='Entra_stopping' Bdg='30JC[09000:09006]' waynum='30' intervalSec='122'>"
        L"    <rwcoord picketing1_val='6~651' picketing1_comm='Рига Пассажирская' />"
        L"    <rwcoord picketing1_val='5~771' picketing1_comm='Рига Пассажирская' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170715T211932Z' name='Arrival' Bdg='14JC[09006]' waynum='14' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint create_time='20170715T210803Z' name='Departure' Bdg='11AC[09000]' waynum='11' parknum='1' />"
        L"  <Action code='Replace' />"
        L"  <PostPoint create_time='20170715T211614Z' name='Entra_stopping' Bdg='30JC[09000:09006]' waynum='30' intervalSec='122' />"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form        Departure   Wrong_way         Entra_stopping    Arrival " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "11AC[09000] 11AC[09000] 30JC[09000:09006] 30JC[09000:09006] 14JC[09006] " ) );
    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() );
    repl.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form        Departure                     Entra_stopping    Arrival " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "11AC[09000] 11AC[09000]                   30JC[09000:09006] 14JC[09006] " ) );
}

void TC_Hem_aeReplace::InsertInner()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170622T235758Z' name='Form' Bdg='X[11000:11451]' waynum='3' index='8614-212-0983' num='J2235' length='57' weight='5375' net_weight='4009' fretran='Y'>"
        L"    <rwcoord picketing1_val='389~476' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='388~658' picketing1_comm='Вентспилс' />"
        L"    <feat_texts typeinfo='Т' />"
        L"    <Locomotive Series='2ТЭ116' NumLoc='1679' Depo='0' Consec='1'>"
        L"      <Crew EngineDriver='VOLOSOVS' Tim_Beg='2017-06-23 01:20' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170622T235954Z' name='Transition' Bdg='Y[11451]'>"
        L"    <rwcoord picketing1_val='387~395' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170623T000750Z' name='Transition' Bdg='Z[11445]'>"
        L"    <rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170623T001922Z' name='Transition' Bdg='Q[11443]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Transition' Bdg='Y[11451]' create_time='20170622T235954Z' />"
        L"  <Action code='Replace' SaveLog='Y'>"
        L"    <EventPoint name='Wrong_way' Bdg='?[11445:11451]' create_time='20170622T235954Z' index='8614-212-0983' num='J2235' length='57' weight='5375' net_weight='4009' fretran='Y'>"
        L"      <feat_texts typeinfo='Т' />"
        L"      <Locomotive Series='2ТЭ116' NumLoc='1679' Depo='0' Consec='1'>"
        L"        <Crew EngineDriver='VOLOSOVS' Tim_Beg='2017-06-23 01:20' />"
        L"      </Locomotive>"
        L"    </EventPoint>"
        L"  </Action>"
        L"  <PostPoint name='Transition' Bdg='Z[11445]' create_time='20170623T000750Z' />"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form           Transition                Transition Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "X[11000:11451] Y[11451]                  Z[11445]   Q[11443]   " ) );
    CPPUNIT_ASSERT( hl.exist_series( "num",  "J2235              -                        -           -   " ) );
    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() );
    repl.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form           Transition Wrong_way      Transition Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "X[11000:11451] Y[11451]   ?[11445:11451] Z[11445]   Q[11443]   " ) );
    CPPUNIT_ASSERT( hl.exist_series( "num",  "J2235              -         -              -           -   " ) );
}

void TC_Hem_aeReplace::InsertWrongway()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170622T235758Z' name='Form' Bdg='P3P[11000:11451]' waynum='3' index='8614-212-0983' num='J2235' length='57' weight='5375' net_weight='4009' fretran='Y'>"
        L"    <rwcoord picketing1_val='389~476' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='388~658' picketing1_comm='Вентспилс' />"
        L"    <feat_texts typeinfo='Т' />"
        L"    <Locomotive Series='2ТЭ116' NumLoc='1679' Depo='0' Consec='1'>"
        L"      <Crew EngineDriver='VOLOSOVS' Tim_Beg='2017-06-23 01:20' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170622T235954Z' name='Transition' Bdg='2SP[11451]'>"
        L"    <rwcoord picketing1_val='387~395' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170622T235954Z' name='Wrong_way' Bdg='MP2P[09813:11451]' waynum='2'>"
        L"    <rwcoord picketing1_val='387~' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='387~395' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170623T091134Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170623T091204Z' name='Span_move' Bdg='PV1IP[09813:09820]' waynum='1'>"
        L"    <rwcoord picketing1_val='4~200' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='2~' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170623T100900Z' name='Disform' Bdg='ASOUP 1042[09820]' waynum='3' parknum='3' />"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Form' Bdg='P3P[11000:11451]' create_time='20170622T235758Z' index='8614-212-0983' num='J2235' length='57' weight='5375' net_weight='4009' fretran='Y' waynum='3'>"
        L"    <rwcoord picketing1_val='389~67' picketing1_comm='Вентспилс' />"
        L"    <feat_texts typeinfo='Т' />"
        L"    <Locomotive Series='2ТЭ116' NumLoc='1679' Depo='0' Consec='1'>"
        L"      <Crew EngineDriver='VOLOSOVS' Tim_Beg='2017-06-23 01:20' />"
        L"    </Locomotive>"
        L"  </PrePoint>"
        L"  <Action code='Replace' SaveLog='Y'>"
        L"  <EventPoint name='Wrong_way' Bdg='?[11000:11451]' create_time='20170622T235758Z' index='8614-212-0983' num='V2235' length='57' weight='5375' net_weight='4009' fretran='Y' waynum='3'>"
        L"    <feat_texts typeinfo='Т' />"
        L"    <Locomotive Series='2ТЭ116' NumLoc='1679' Depo='0' Consec='1'>"
        L"      <Crew />"
        L"    </Locomotive>"
        L"  </EventPoint>"
        L"  </Action>"
        L"  <PostPoint name='Transition' Bdg='2SP[11451]' create_time='20170622T235954Z' />"
        L"</A2F_HINT>"
       );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form             Transition Wrong_way         Transition Span_move          Disform " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "P3P[11000:11451] 2SP[11451] MP2P[09813:11451] 1C[09813]  PV1IP[09813:09820] ASOUP 1042[09820] " ) );
    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() );
    repl.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form             Wrong_way      Transition Wrong_way         Transition Span_move          Disform " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "P3P[11000:11451] ?[11000:11451] 2SP[11451] MP2P[09813:11451] 1C[09813]  PV1IP[09813:09820] ASOUP 1042[09820] " ) );
}

class UtUserChart : public UserChart
{
public:
    UserChart::getHappenLayer;
};

void TC_Hem_aeReplace::InsertWrongway_2()
{
    UtUserChart userChart;
    auto& pure_happen = userChart.getHappenLayer();
    UtLayer<HappenLayer>& happen = reinterpret_cast< UtLayer<HappenLayer>& >(pure_happen);
    happen.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20161110T054252Z' name='Form' Bdg='2AC[11111]' waynum='2' parknum='1' index='1142-060-1100' num='3002' length='22' weight='511' fretran='Y' />"
        L"  <SpotEvent create_time='20161110T054252Z' name='Departure' Bdg='2BC[11111]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20161110T055835Z' name='Transition' Bdg='5SP[22222]'>"
        L"    <rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
// это будущая вставка
//         L"  <SpotEvent create_time='20161110T055838Z' name='Wrong_way' Bdg='N1P_BP387[22222:33333]' waynum='1'>"
//         L"    <rwcoord picketing1_val='383~39' picketing1_comm='Вентспилс' />"
//         L"    <rwcoord picketing1_val='383~710' picketing1_comm='Вентспилс' />"
//         L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161110T060522Z' name='Transition' Bdg='4/6SP:4+[33333]'>"
        L"    <rwcoord picketing1_val='387~399' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161110T061300Z' name='Arrival' Bdg='ASOUP 1042[44444]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161110T061300Z' name='Death' Bdg='ASOUP 1042[44444]' waynum='1' parknum='1' />"
        L"</HemPath>"
       );
    Hem::Unbill unbill = createUnbill(
        L"<SoxPostUndoBill mark='valet#1' issue_moment='20161110T025959Z'>"
        L"  <UndoChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Pre create_time='20161110T055835Z' name='Transition' Bdg='5SP[22222]'>"
        L"          <rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"        </Pre>"
        L"        <Post create_time='20161110T060522Z' name='Transition' Bdg='4/6SP:4+[33333]'>"
        L"          <rwcoord picketing1_val='387~399' picketing1_comm='Вентспилс' />"
        L"        </Post>"
        L"        <Body create_time='20161110T055838Z' name='Wrong_way' Bdg='N1P_BP387[22222:33333]' waynum='1'>"
        L"          <rwcoord picketing1_val='383~39' picketing1_comm='Вентспилс' />"
        L"          <rwcoord picketing1_val='383~710' picketing1_comm='Вентспилс' />"
        L"        </Body>"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UndoChanges>"
        L"</SoxPostUndoBill>"
       );

    CPPUNIT_ASSERT( happen.path_count() == 1 );
    CPPUNIT_ASSERT( happen.exist_path_size(6) );
    CPPUNIT_ASSERT( happen.exist_series( "name", "Form       Departure  Transition                        Transition      Arrival           Death " ) );
    CPPUNIT_ASSERT( happen.exist_series( "Bdg",  "2AC[11111] 2BC[11111] 5SP[22222]                        4/6SP:4+[33333] ASOUP 1042[44444] ASOUP 1042[44444] " ) );
    userChart.carryOut(unbill, 0);
    CPPUNIT_ASSERT( happen.path_count() == 1 );
    CPPUNIT_ASSERT( happen.exist_path_size(7) );
    CPPUNIT_ASSERT( happen.exist_series( "name", "Form       Departure  Transition Wrong_way              Transition      Arrival           Death " ) );
    CPPUNIT_ASSERT( happen.exist_series( "Bdg",  "2AC[11111] 2BC[11111] 5SP[22222] N1P_BP387[22222:33333] 4/6SP:4+[33333] ASOUP 1042[44444] ASOUP 1042[44444] " ) );
}

void TC_Hem_aeReplace::InsertWrongway_4310()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170724T082434Z' name='Form' Bdg='PV1IP[11111:22222]' waynum='1' index='' num='V2224' fretran='Y'>"
        L"    <rwcoord picketing1_val='4~200' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='2~' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170724T082646Z' name='Transition' Bdg='1C[11111]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170724T090945Z' name='Transition' Bdg='1C[33333]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170724T091849Z' name='Station_entry' Bdg='PDP[44444]'>"
        L"    <rwcoord picketing1_val='56~187' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170724T091914Z' name='Death' Bdg='PDP[44444]'>"
        L"    <rwcoord picketing1_val='56~187' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Transition' Bdg='1C[33333]' create_time='20170724T090945Z' waynum='1' parknum='1' />"
        L"  <Action code='Replace'>"
        L"    <EventPoint name='Wrong_way' Bdg='?[44444:33333]' create_time='20170724T090945Z' />"
        L"  </Action>"
        L"  <PostPoint name='Station_entry' Bdg='PDP[44444]' create_time='20170724T091849Z' />"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form               Transition Transition                Station_entry Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "PV1IP[11111:22222] 1C[11111]  1C[33333]                 PDP[44444]    PDP[44444] " ) );
    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() );
    repl.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form               Transition Transition Wrong_way      Station_entry Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "PV1IP[11111:22222] 1C[11111]  1C[33333]  ?[33333:44444] PDP[44444]    PDP[44444] " ) );
}

void TC_Hem_aeReplace::insertDuplicate_4431()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20171103T092400Z' name='Form'       Bdg='?[09070]' index='0907-003-0950' num='4809' resloc='Y' />"
        L"  <SpotEvent create_time='20171103T093100Z' name='Departure'  Bdg='?[09070]' />"
        L"  <SpotEvent create_time='20171103T094400Z' name='Transition' Bdg='?[09600]' />"
        L"  <SpotEvent create_time='20171103T095100Z' name='Transition' Bdg='?[09602]' />"
        L"  <SpotEvent create_time='20171103T095711Z' name='Transition' Bdg='?[09603]'/>"
        L"  <SpotEvent create_time='20171103T102900Z' name='Span_move'  Bdg='?[09603:09611]' >"
        L"    <rwcoord picketing1_val='34~950' picketing1_comm='Рига-Скулте' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20171103T102901Z' name='Death'      Bdg='?[09603:09611]' >"
        L"    <rwcoord picketing1_val='34~950' picketing1_comm='Рига-Скулте' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    // команда вставки двух неразличимых событий - не должна давать никакого эффекта
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Transition' Bdg='?[09602]' create_time='20171103T095100Z' />"
        L"  <Action code='Replace'>"
        L"    <EventPoint name='Transition' Bdg='?[09603]' create_time='20171103T095800Z' waynum='3' parknum='1' />"
        L"    <EventPoint name='Transition' Bdg='?[09603]' create_time='20171103T095800Z' />"
        L"  </Action>"
        L"  <PostPoint name='Span_move' Bdg='?[09603:09611]' create_time='20171103T102900Z'>"
        L"    <rwcoord picketing1_val='34~950' picketing1_comm='Рига-Скулте' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    const std::string textN = "Form             Departure        Transition       Transition       Transition       Span_move        Death ";
    const std::string textT = "20171103T092400Z 20171103T093100Z 20171103T094400Z 20171103T095100Z 20171103T095711Z 20171103T102900Z 20171103T102901Z ";
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
    CPPUNIT_ASSERT( hl.exist_series( "name",         textN ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time",  textT ) );
    //
    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() );
    repl.Action();
    // и ничего не произошло - команда отвергнута
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
    CPPUNIT_ASSERT( hl.exist_series( "name",         textN ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time",  textT ) );
}

void TC_Hem_aeReplace::ReplaceBadEvent()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='A[11292:11300]' create_time='20151207T010347Z' waynum='1'>"
        L"    <rwcoord picketing1_val='250~500' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='248~800' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='B[11292:11300]' create_time='20151207T011856Z' waynum='1'>"
        L"    <rwcoord picketing1_val='255~600' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='253~900' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='C[11292:11300]' create_time='20151207T011857Z' waynum='1'>"
        L"    <rwcoord picketing1_val='255~600' picketing1_comm='Посинь' />"
        L"    <rwcoord picketing1_val='253~900' picketing1_comm='Посинь' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Form' cover='Y' Bdg='A[11292:11300]' create_time='20151207T010347Z' waynum='1'>"
        L"    <rwcoord picketing1_val='249~650' picketing1_comm='Посинь' />"
        L"  </PrePoint>"
        L"  <Action code='Replace'>"
        L"    <EventPoint xxxxxxxxx='0000000000000000000'/>"
        L"  </Action>"
        L"  <PostPoint name='Span_move' Bdg='B[11292:11300]' create_time='20151207T011856Z' waynum='1'>"
        L"    <rwcoord picketing1_val='254~750' picketing1_comm='Посинь' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form           Span_move      Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "A[11292:11300] B[11292:11300] C[11292:11300]" ) );
    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    CPPUNIT_ASSERT_THROW( aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() ), HemHelpful::HemException );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form           Span_move      Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "A[11292:11300] B[11292:11300] C[11292:11300]" ) );
}

void TC_Hem_aeReplace::ReplaceFutureEvent()
{
    UtLayer<FutureLayer> future;
    {
        attic::a_document doc;
    doc.load_wide(
        L"<Test>"
        L"<FutureLayer>"
        L"<HemPath>"
        L"  <SpotEvent create_time='20180515T113734Z' name='Departure' Bdg='№2582[11720]' index='' num='2582' through='Y' />"
        L"  <SpotEvent create_time='20180515T113734Z' name='Transition' Bdg='№2582[11760]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180515T115932Z' name='Transition' Bdg='№2582[09360]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180515T121352Z' name='Transition' Bdg='№2582[09370]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180515T122707Z' name='Arrival' Bdg='№2582[09380]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20180515T124000Z' name='Disform' Bdg='№2582[09380]' waynum='3' parknum='1' />"
        L"</HemPath>"
        L"</FutureLayer>"
        L"</Test>"
        );
    future.Deserialize( doc.document_element() );
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180516T025959Z'>"
        L"<PrePoint layer='future' name='Transition' Bdg='№2582[11760]' create_time='20180515T113734Z' waynum='1' parknum='1' />"
        L"<Action code='Replace' District='09000-11760' DistrictRight='Y' SaveLog='Y'>"
        L"<EventPoint layer='future' sourcetype='fix' name='Transition' Bdg='№2582[09360]' create_time='20180515T115100Z' waynum='1' parknum='1' />"
        L"</Action>"
        L"<PostPoint layer='future' name='Transition' Bdg='№2582[09370]' create_time='20180515T121352Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
    L"<DistrictList>"
        L"<District code='09000-11760' name='xxx'>"
        L"<Involve esr='11720'/>"
        L"<Involve esr='11760'/>"
        L"<Involve esr='09360'/>"
        L"<Involve esr='09370'/>"
        L"<Involve esr='09380'/>"
        L"<Involve esr='09400'/>"
        L"<Involve esr='09410'/>"
        L"<Involve esr='09420'/>"
        L"<Involve esr='09000:09420'/>"
        L"</District>"
    L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }

    CPPUNIT_ASSERT( future.path_count() == 1 );
    CPPUNIT_ASSERT( future.exist_series( "name", "         Departure    Transition        Transition      Transition         Arrival          Disform" ) );
    CPPUNIT_ASSERT( future.exist_series( "create_time", "20180515T113734Z 20180515T113734Z 20180515T115932Z  20180515T121352Z 20180515T122707Z  20180515T124000Z"   ) );
    CPPUNIT_ASSERT( future.exist_series( "sourcetype", "        -              -                  -               -                -                 -"   ) );
    CPPUNIT_ASSERT( future.exist_series( "num", "             2582             -                  -               -                -                 -"   ) );
    aeReplaceFuture repl( future.GetWriteAccess(), bill.getChance(), dg, bill.getHeadFixableThrows(), bill.coverHead(), bill.getTailFixableThrows(), bill.coverTail(), bill.getReplaceFixableEvents() );
    repl.Action();
    CPPUNIT_ASSERT( future.path_count() == 1 );
    CPPUNIT_ASSERT( future.exist_series( "name", "         Departure    Transition        Transition      Transition         Arrival          Disform" ) );
    CPPUNIT_ASSERT( future.exist_series( "create_time", "20180515T113734Z 20180515T113734Z 20180515T115100Z  20180515T121352Z 20180515T122707Z  20180515T124000Z"   ) );
    CPPUNIT_ASSERT( future.exist_series( "sourcetype", "        -              -                 fix              -                -                 -"   ) );
    CPPUNIT_ASSERT( future.exist_series( "num", "             2582             -                  -               -                -                 -"   ) );
}

void TC_Hem_aeReplace::ReplaceFutureChronoBreak()
{
    UtLayer<FutureLayer> fl;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<Test>"
            L"  <FutureLayer>"
            L"    <HemPath>"
            L"      <SpotEvent create_time='20180515T101111Z' name='Departure'  Bdg='aa[11111]' index='' num='6543' />"
            L"      <SpotEvent create_time='20180515T102222Z' name='Transition' Bdg='bb[22222]' waynum='1' parknum='1' />"
            L"      <SpotEvent create_time='20180515T103333Z' name='Transition' Bdg='cc[33333]' waynum='1' parknum='1' />"
            L"      <SpotEvent create_time='20180515T104444Z' name='Transition' Bdg='dd[44444]' waynum='1' parknum='1' />"
            L"    </HemPath>"
            L"  </FutureLayer>"
            L"</Test>"
            );
        fl.Deserialize( doc.document_element() );
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180516T000000Z'>"
        L"  <PrePoint layer='future' name='Transition' cover='Y' Bdg='bb[22222]' create_time='20180515T102222Z' waynum='1' parknum='1' />"
        L"  <Action code='Replace' District='qwe' DistrictRight='Y'>"
        L"    <EventPoint layer='future' name='Transition' Bdg='WW[22222]' create_time='20180515T105555Z' waynum='1' parknum='1' sourcetype='fix' />"
        L"  </Action>"
        L"  <PostPoint layer='future' name='Transition' cover='Y' Bdg='bb[22222]' create_time='20180515T102222Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<DistrictList>"
            L"  <District code='qwe' name='xxx'>"
            L"  <Involve esr='11111'/>"
            L"  <Involve esr='22222'/>"
            L"  <Involve esr='33333'/>"
            L"  <Involve esr='44444'/>"
            L"  <Involve esr='55555'/>"
            L"  </District>"
            L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }

    CPPUNIT_ASSERT( fl.path_count() == 1 );
    CPPUNIT_ASSERT( fl.exist_series( "name",        "Departure        Transition       Transition       Transition      " ) );
    CPPUNIT_ASSERT( fl.exist_series( "create_time", "20180515T101111Z 20180515T102222Z 20180515T103333Z 20180515T104444Z"   ) );
    aeReplaceFuture repl( fl.GetWriteAccess(), bill.getChance(), dg, bill.getHeadFixableThrows(), bill.coverHead(), bill.getTailFixableThrows(), bill.coverTail(), bill.getReplaceFixableEvents() );
    repl.Action();
    CPPUNIT_ASSERT( fl.path_count() == 1 );
    CPPUNIT_ASSERT( fl.exist_series( "name",        "Departure        Transition      " ) );
    CPPUNIT_ASSERT( fl.exist_series( "create_time", "20180515T101111Z 20180515T105555Z"   ) );
}

void TC_Hem_aeReplace::UndoCommandWithReplaceByRectify()
{
    UtUserChart userChart;
    auto& pure_happen = userChart.getHappenLayer();
    UtLayer<HappenLayer>& happen = reinterpret_cast< UtLayer<HappenLayer>& >(pure_happen);

    Hem::Unbill unbill = createUnbill(
        L"<SoxPostUndoBill mark='valet#1' issue_moment='20161110T025959Z'>"
        L"  <UndoChanges>"
        L"    <HappenLayer>"
        L"<Trio>"
        L"<Body create_time='20200212T081409Z' name='Form' Bdg='LI5[09510:11130]' waynum='1'>"
        L"<rwcoord picketing1_val='67~300' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='69~300' picketing1_comm='Земитани-Валга' />"
        L"</Body>"
        L"<Body create_time='20200212T081457Z' name='Arrival' Bdg='5SP[09510]' waynum='3' parknum='1' />"
        L"<Body create_time='20200212T081656Z' name='Death' Bdg='5SP[09510]' waynum='3' parknum='1' />"
        L"<Body create_time='20200212T081701Z' name='Form' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"<Body create_time='20200212T081752Z' name='Death' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"</Trio>"
        L"<Trio>"
        L"<Body create_time='20200212T081639Z' name='Form' Bdg='6-8SP:6-[09510]' />"
        L"<Body create_time='20200212T081701Z' name='Arrival' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"<Body create_time='20200212T081728Z' name='Departure' Bdg='14SP:14+[09510]' waynum='3' parknum='1' optCode='09510:09511' />"
        L"<Body create_time='20200212T081827Z' name='Span_move' Bdg='SL7[09510:09511]' waynum='1'>"
        L"<rwcoord picketing1_val='59~600' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='61~600' picketing1_comm='Земитани-Валга' />"
        L"</Body>"
        L"<Body create_time='20200212T081832Z' name='Death' Bdg='SL7[09510:09511]' waynum='1'>"
        L"<rwcoord picketing1_val='59~600' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='61~600' picketing1_comm='Земитани-Валга' />"
        L"</Body>"
        L"</Trio>"
        L"    </HappenLayer>"
        L"  </UndoChanges>"
        L"</SoxPostUndoBill>"
        );

    CPPUNIT_ASSERT( happen.path_count() == 0 );
    userChart.carryOut(unbill, 0);
    CPPUNIT_ASSERT( happen.path_count() == 2 );
    CPPUNIT_ASSERT( happen.exist_path_size(5) );
    CPPUNIT_ASSERT( happen.exist_series( "name", "Form       Arrival  Death       Form    Death " ) );
    CPPUNIT_ASSERT( happen.exist_series( "name", "Form       Arrival  Departure Span_move Death " ) );
}

void TC_Hem_aeReplace::WrongUndoCommandWithDublicat()
{
    UtUserChart userChart;
    auto& pure_happen = userChart.getHappenLayer();
    UtLayer<HappenLayer>& happen = reinterpret_cast< UtLayer<HappenLayer>& >(pure_happen);

    Hem::Unbill unbill = createUnbill(
        L"<SoxPostUndoBill mark='valet#1' issue_moment='20161110T025959Z'>"
        L"  <UndoChanges>"
        L"    <HappenLayer>"
        L"<Trio>"
        L"<Body create_time='20200212T081409Z' name='Form' Bdg='LI5[09510:11130]' waynum='1'>"
        L"<rwcoord picketing1_val='67~300' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='69~300' picketing1_comm='Земитани-Валга' />"
        L"</Body>"
        L"<Body create_time='20200212T081457Z' name='Arrival' Bdg='5SP[09510]' waynum='3' parknum='1' />"
        L"<Body create_time='20200212T081656Z' name='Departure' Bdg='5SP[09510]' waynum='3' parknum='1' />"
        L"<Body create_time='20200212T081701Z' name='Arrival' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"<Body create_time='20200212T081752Z' name='Death' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"</Trio>"
        L"<Trio>"
        L"<Body create_time='20200212T081639Z' name='Form' Bdg='6-8SP:6-[09510]' />"
        L"<Body create_time='20200212T081701Z' name='Arrival' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"<Body create_time='20200212T081728Z' name='Departure' Bdg='14SP:14+[09510]' waynum='3' parknum='1' optCode='09510:09511' />"
        L"<Body create_time='20200212T081827Z' name='Span_move' Bdg='SL7[09510:09511]' waynum='1'>"
        L"<rwcoord picketing1_val='59~600' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='61~600' picketing1_comm='Земитани-Валга' />"
        L"</Body>"
        L"<Body create_time='20200212T081832Z' name='Death' Bdg='SL7[09510:09511]' waynum='1'>"
        L"<rwcoord picketing1_val='59~600' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='61~600' picketing1_comm='Земитани-Валга' />"
        L"</Body>"
        L"</Trio>"
        L"    </HappenLayer>"
        L"  </UndoChanges>"
        L"</SoxPostUndoBill>"
        );

    CPPUNIT_ASSERT( happen.path_count() == 0 );
    userChart.carryOut(unbill, 0);
    CPPUNIT_ASSERT( happen.path_count() == 1 );
    CPPUNIT_ASSERT( happen.exist_path_size(5) );
    CPPUNIT_ASSERT( happen.exist_series( "name", "Form       Arrival  Departure       Arrival    Death " ) );
}

void TC_Hem_aeReplace::ReplaceArrivalDepartureTheSameTime()
{
    UtLayer<HappenLayer> hl;
    UtLayer<AsoupLayer> al;

    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200909T100500Z' name='Arrival' Bdg='?p1w4[11001]' waynum='4' parknum='1' index='1514-908-0822' num='2781' length='57' weight='5547' net_weight='3910' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200909T100500Z' name='Departure' Bdg='?p1w4[11001]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20200909T102600Z' name='Transition' Bdg='1Pd[11002]' />"
        L"<SpotEvent create_time='20200909T103131Z' name='Transition' Bdg='2SP[11451]' />"
        L"</HemPath>"
        );
    al.createPath(
        L"<AsoupEvent create_time='20200909T102000Z' name='Departure' Bdg='ASOUP 1042_1p[11001]' index='1514-908-0822' num='2781' length='57' weight='5547' net_weight='3910' through='Y' parknum='1' dirTo='11420' adjTo='11002' linkBadge='?p1w4[11001]' linkTime='20200909T100500Z' linkCode='Departure'><![CDATA[(:1042 909/000+11000 2781 1514 908 0822 03 11420 09 09 13 20 02/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 05547 03910 260 74890864 42279992 000 000 00 065 000.40 001 000 000 70 062 000 000 73 062 000 000 90 002 000 000 96 002 000 000\n"
        L"Ю3 583 00000041 1 11 50 0000 00000 M-SEROV     \n"
        L"Ю3 583 00000042 9\n"
        L"Ю4 08620 2 08 065 000.40 001 000 70 062 000 73 062 000 90 002 000 96 002 000 \n"
        L"Ю12 00 74890864 1 061 08220 21503 3089 360 01 00 00 00 00 0264 20 70 0748 04 086 08220 00000 08620 08 15140 03 15140 2424 021 1 1080 ДTT    132 000 00000000\n"
        L"Ю12 00 74742776 191237255247 05802320730151441\n"
        L"Ю12 00 74890377 191237255255 06102640748\n"
        L"Ю12 00 72033053 191237255247 049024607200     \n"
        L"Ю12 00 73050445 191237255255 05902400730\n"
        L"Ю12 00 73067969 255239255255 0232\n"
        L"Ю12 00 74750886 255255255255 \n"
        L"Ю12 00 73054421 255255255255 \n"
        L"Ю12 00 73038358 255255255255 \n"
        L"Ю12 00 74755026 191239255255 0640270\n"
        L"Ю12 00 73928020 191237255255 06602550732\n"
        L"Ю12 00 74754946 191237255255 06502700730\n"
        L"Ю12 00 74812504 191237255255 06202610748\n"
        L"Ю12 00 73958621 191237255255 06702550732\n"
        L"Ю12 00 74747387 191237255255 06502700730\n"
        L"Ю12 00 74945965 191237255255 06402660732\n"
        L"Ю12 00 73044059 191237255255 05902400730\n"
        L"Ю12 00 74706078 251239255255 4600232\n"
        L"Ю12 00 72043243 191237255255 05102530720\n"
        L"Ю12 00 72046733 255239255255 0249\n"
        L"Ю12 00 74800756 191237255255 06002650748\n"
        L"Ю12 00 75137554 191253255255 0650732\n"
        L"Ю12 00 74870270 191253255255 0590748\n"
        L"Ю12 00 74890617 191239255255 0600264\n"
        L"Ю12 00 75156786 191237255255 06702540732\n"
        L"Ю12 00 73901571 255239255255 0255\n"
        L"Ю12 00 74873001 191237255255 06202650748\n"
        L"Ю12 00 74805029 191239255255 0590264\n"
        L"Ю12 00 74706268 191237255255 05602530730\n"
        L"Ю12 00 74891508 191237255255 06102640748\n"
        L"Ю12 00 74811878 191239255255 0600270\n"
        L"Ю12 00 74089863 191237255255 05902320730\n"
        L"Ю12 00 73957813 191237255255 06602550732\n"
        L"Ю12 00 74042789 191237255255 05802400730\n"
        L"Ю12 00 73053209 187239255255 0605600232\n"
        L"Ю12 00 74753963 191239255255 0650270\n"
        L"Ю12 00 72727167 191237255255 05002500720\n"
        L"Ю12 00 74752288 191237255255 06402700730\n"
        L"Ю12 00 74133612 191239255255 0600232\n"
        L"Ю12 00 74870080 191237255255 06202660748\n"
        L"Ю12 00 74890252 255239255255 0267\n"
        L"Ю12 00 74811563 191239255255 0630260\n"
        L"Ю12 00 73047839 191237255255 05902320730\n"
        L"Ю12 00 74133851 255239255255 0240\n"
        L"Ю12 00 74890286 191237255255 05802800748\n"
        L"Ю12 00 73071284 191237255255 05902320730\n"
        L"Ю12 00 74891615 255237255255 02680748\n"
        L"Ю12 00 75050161 191237255255 06502700732\n"
        L"Ю12 00 74969858 191239255255 0660274\n"
        L"Ю12 00 74974411 191239255255 0650262\n"
        L"Ю12 00 74974601 255255255255 \n"
        L"Ю12 00 74970971 251239255255 3600273\n"
        L"Ю12 00 73988354 191239255255 0640274\n"
        L"Ю12 00 73069742 191237255255 05902320730\n"
        L"Ю12 00 75156752 191237255255 06702560732\n"
        L"Ю12 00 73053936 191237255255 05902400730\n"
        L"Ю12 00 72033970 191237255255 05102440720\n"
        L"Ю12 00 74812256 191237255255 06102700748\n"
        L"Ю12 00 74889866 191239255255 0620268\n"
        L"Ю12 00 73033664 191237255255 05802320730\n"
        L"Ю12 00 72039167 191237255255 04802500720\n"
        L"Ю12 00 73063612 191237255255 05902320730\n"
        L"Ю12 00 94360823 129169124051 053083500030000120600602021096094210542648202989020OXP   148\n"
        L"Ю12 00 94343266 255239255255 0206\n"
        L"Ю12 00 42279992 187233255255 0540000207400404:)]]><feat_texts typeinfo='Т' />"
        L"</AsoupEvent>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",          "Arrival          Departure       Transition       Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",           "?p1w4[11001]    ?p1w4[11001]      1Pd[11002]       2SP[11451]" ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time",  "20200909T100500Z 20200909T100500Z 20200909T102600Z 20200909T103131Z"  ) );

    auto asoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( asoupList.size() == 1 );
    auto asoup = asoupList.front();
    CPPUNIT_ASSERT( al.IsServed( asoup ) );
    Hem::EventAddress<SpotEvent> sea = al.GetServedMessage(asoup);
    CPPUNIT_ASSERT (!sea.empty() && hl.GetWriteAccess()->getByAddress(sea) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200909T112819Z'>"
        L"<PrePoint name='Arrival' cover='Y' Bdg='?p1w4[11001]' create_time='20200909T100500Z' index='1514-908-0822' num='2781' length='57' weight='5547' net_weight='3910' through='Y' waynum='4' parknum='1'>"
        L"<feat_texts typeinfo='Т' />"
        L"</PrePoint>"
        L"<Action code='Replace' District='11000-12910' DistrictRight='Y' SaveLog='Y'>"
        L"<EventPoint name='Arrival' Bdg='?p1w3[11001]' create_time='20200909T101700Z' index='1514-908-0822' num='2781' length='57' weight='5547' net_weight='3910' through='Y' waynum='3' parknum='1'>"
        L"<feat_texts typeinfo='Т' />"
        L"</EventPoint>"
        L"<EventPoint name='Departure' Bdg='?p1w3[11001]' create_time='20200909T101700Z' waynum='3' parknum='1' />"
        L"</Action>"
        L"<PostPoint name='Transition' Bdg='1Pd[11002]' create_time='20200909T102600Z' />"
        L"</A2F_HINT>"
        );

    Context context(Hem::Chance::System(), eg, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    aeReplaceHappen repl( hl.GetWriteAccess(), al.GetWriteAccess(), context, bill.getHeadSpotThrows(), bill.coverHead(), bill.getTailSpotThrows(), bill.coverTail(), bill.getReplaceSpotEvents() );
    repl.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",          "Arrival          Departure       Transition       Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",           "?p1w3[11001]    ?p1w3[11001]      1Pd[11002]       2SP[11451]" ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time",  "20200909T101700Z 20200909T101700Z 20200909T102600Z 20200909T103131Z"  ) );
    asoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( asoupList.size() == 1 );
    asoup = asoupList.front();
    CPPUNIT_ASSERT( al.IsServed( asoup ) );
    sea = al.GetServedMessage(asoup);
    CPPUNIT_ASSERT (!sea.empty() && hl.GetWriteAccess()->getByAddress(sea) );
}

