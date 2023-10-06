#include "stdafx.h"

#include "TC_Hem_aeRemove.h"
#include "../helpful/Attic.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "TopologyTest.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/Appliers/aeRemove.h"
#include "../Hem/Appliers/Context.h"
#include "../Hem/Bill.h"
#include "../Hem/GuessTransciever.h"
#include "../Hem/AsoupLinker.h"
#include "../Hem/UserIdentifyPolicy.h"
#include "UtHelpfulDateTime.h"
#include "../helpful/EsrGuide.h"
#include "TopologyTest.h"
#include "../Hem/UserEditInventory.h"
#include "../helpful/NsiBasis.h"
#include "../Hem/UncontrolledStations.h"
#include "../helpful/Junctions.h"

using namespace std;
using namespace attic;
using namespace HemHelpful;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeRemove );


void TC_Hem_aeRemove::makeStageData( unsigned count_of_event )
{
    doc.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Form'       Bdg='2SP+[09010]' create_time='20160627T042822Z' index='' num='6503' subur='Y' />"
        L"  <SpotEvent name='Departure'  Bdg='2SP[09010]' create_time='20160627T043822Z' />"
        L"  <SpotEvent name='Transition' Bdg='2p[09730]' create_time='20160627T052815Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Span_move'     Bdg='some[09730:09732]' create_time='20160627T052900Z' waynum='1' />"
        L"  <SpotEvent name='Pocket_entry'        Bdg='CKPU2_K[09730:09732]' create_time='20160627T052944Z' waynum='1' />"
        L"  <SpotEvent name='Span_stopping_begin' Bdg='CKPU2_K[09730:09732]' create_time='20160627T052951Z' waynum='1' />"
        L"  <SpotEvent name='Death'               Bdg='CKPU2_K[09730:09732]' create_time='20160627T053033Z' waynum='1' />"
        L"  <SpotEvent name='Form'                Bdg='KU6/7_K[09730:09732]' create_time='20160627T053033Z' waynum='1' />"
        L"  <SpotEvent name='Span_stopping_end'   Bdg='CKPU2_K[09730:09732]' create_time='20160627T053033Z' waynum='1' />"
        L"  <SpotEvent name='Pocket_exit'         Bdg='CKPU2_K[09730:09732]' create_time='20160627T053033Z' waynum='1' />"
        L"  <SpotEvent name='Span_move'           Bdg='KU6/7_K[09730:09732]' create_time='20160627T053033Z' waynum='1' />"
        L"  <SpotEvent name='Span_move'           Bdg='edit[09730:09732]'    create_time='20160627T054044Z' waynum='1' />"
        L"  <SpotEvent name='Death'               Bdg='edit[09730:09732]'    create_time='20160627T054044Z' waynum='1' />"
        L"</HemPath>"
        );

    vector<a_node> childs;
    for( a_node n : doc.document_element().children() )
        childs.push_back( n );
    while( childs.size() > count_of_event )
    {
        doc.document_element().remove_child( childs.back() );
        childs.pop_back();
    }
}

void TC_Hem_aeRemove::events13() { makeStageData( 13 ); runStage( "Form Death" ); }
void TC_Hem_aeRemove::events12() { makeStageData( 12 ); runStage( "Span_move" ); }
void TC_Hem_aeRemove::events11() { makeStageData( 11 ); runStage( "Span_move" ); }
void TC_Hem_aeRemove::events10() { makeStageData( 10 ); runStage( "Pocket_entry Pocket_exit " ); }
void TC_Hem_aeRemove::events9()  { makeStageData( 9 );  runStage( "Pocket_entry Span_stopping_begin Span_stopping_end" ); }
void TC_Hem_aeRemove::events8()  { makeStageData( 8 );  runStage( "Pocket_entry Span_stopping_begin Death Form" ); }
void TC_Hem_aeRemove::events7()  { makeStageData( 7 );  runStage( "Form Death" ); }
void TC_Hem_aeRemove::events6()  { makeStageData( 6 );  runStage( "Pocket_entry Span_stopping_begin" ); }
void TC_Hem_aeRemove::events5()  { makeStageData( 5 );  runStage( "Pocket_entry" ); }
void TC_Hem_aeRemove::events4()  { makeStageData( 4 );  runStage( "Span_move" ); }
void TC_Hem_aeRemove::events3()  { makeStageData( 3 );  runStage( "Transition" ); }
void TC_Hem_aeRemove::events2()  { makeStageData( 2 );  runStage( "Departure" ); }
void TC_Hem_aeRemove::events1()  { makeStageData( 1 );  runStage( "Form" ); }

void TC_Hem_aeRemove::runStage( const string& cutted )
{
    string name_all;
    for( a_node n : doc.document_element().children() )
    {
        name_all += n.attribute("name").as_string() + string(" ");
    }

    UtLayer<HappenLayer> hl;
    AsoupLayer al;

    hl.createPath( doc.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", name_all ) );

    AsoupEventPtr asoup = std::make_shared<AsoupEvent>(HCode::DEPARTURE, BadgeE(L"ASOUP_BDG", EsrKit(9010)),
        ParkWayKit(), timeForDateTime(2016, 6, 27, 4, 28), TrainDescr(L"1379", L"1234-444-4321"));
    al.AddAsoupEvent(asoup);
    CPPUNIT_ASSERT(!al.IsServed(asoup));

    auto allPathes = hl.GetWriteAccess()->getAllPathes();
    CPPUNIT_ASSERT_EQUAL(size_t(1), allPathes.size());
    auto path = hl.GetWriteAccess()->getPath( (*allPathes.begin())->GetLastEvent() );
    guessTransciever.registerPath(path, 1);

    auto pretenders = Hem::AsoupToSpot(Hem::SoftIdentifyPolicy(), 0).getPretendersFromPath(*asoup, path);
    bool identified = size_t(1) == pretenders.size();
    
    if (identified)
    {
        EsrGuide esrGuide;
        std::shared_ptr<TopologyTest> topology;
        topology.reset( new TopologyTest );
        UncontrolledStations uncontrolledStations;
        Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
        Hem::AsoupLinker linker(hl.GetWriteAccess(),  al.GetWriteAccess(), context, Hem::UserIdentifyPolicy(), nullptr);
        CPPUNIT_ASSERT(linker.identify(asoup, pretenders.front()));
        CPPUNIT_ASSERT(al.IsServed(asoup));
    }
    else
    {
        CPPUNIT_ASSERT(!al.IsServed(asoup));
    }

    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( doc.document_element().first_child() );
    UncontrolledStations uncontrolledStations;
    Hem::ApplierCarryOut::aeRemove remo( uncontrolledStations, guessTransciever, hl.GetWriteAccess(), al.GetWriteAccess(), *event );
    remo.Action();
	CPPUNIT_ASSERT( !al.IsServed(asoup) );
    // проверка результата
    attic::a_document undoer;
    collectUndo( undoer, hl );

    attic::a_document dd("do");
    hl.TakeChanges( dd.document_element() );
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", cutted ) );
    // и отмена с проверкой 
    CPPUNIT_ASSERT( implementUndo(undoer,hl) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", name_all ) );

	//При удаления нити привязка АСОУП удаляется - при откате 
    CPPUNIT_ASSERT( !al.IsServed(asoup) );
}

void TC_Hem_aeRemove::DisformEnded()
{
    UtLayer<HappenLayer> hl;
    AsoupLayer asoupLayer;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20160901T080917Z' name='Form' Bdg='PAP[11343]'>"
        L"    <rwcoord picketing1_val='505~400' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160901T080917Z' name='Departure' Bdg='PAP[11343]'>"
        L"    <rwcoord picketing1_val='505~400' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160901T080925Z' name='Span_move' Bdg='N15P[11343:11360]' waynum='1'>"
        L"    <rwcoord picketing1_val='505~400' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='506~720' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160901T080925Z' name='Disform' Bdg='N15P[11343:11360]' waynum='1'>"
        L"    <rwcoord picketing1_val='505~400' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='506~720' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Span_move Disform" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_move' Bdg='N15P[11343:11360]' create_time='20160901T080925Z' waynum='1'>"
        L"    <rwcoord picketing1_val='506~60' picketing1_comm='Россия' />"
        L"  </PrePoint>"
        L"  <Action code='Remove' />"
        L"</A2F_HINT>"
        );

    UncontrolledStations uncontrolledStations;
    Hem::ApplierCarryOut::aeRemove remo( uncontrolledStations, guessTransciever, hl.GetWriteAccess(), asoupLayer.GetWriteAccess(), bill.getHeadSpotThrows() );
    remo.Action();
    
    // данные отката
    attic::a_document undoer;
    collectUndo( undoer, hl );
    CPPUNIT_ASSERT( hl.path_count()==0 );
    // отмена
    CPPUNIT_ASSERT( implementUndo(undoer,hl) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Span_move Disform" ) );
}

bool TC_Hem_aeRemove::implementUndo( attic::a_document& undoer, HappenLayer& happen_layer )
{
    bool res = true;
    for( auto trNode : undoer.document_element().child(HappenLayer_xAttr).children() )
    {
        TrioSpot eTrio;
        trNode >> eTrio;
        UtIdentifyCategoryList identifyCategory;
        Hem::aePusherUserHappen pusher( happen_layer.GetWriteAccess(), eTrio, identifyCategory );
        pusher.Action();
        if ( pusher.haveError() )
            res = false;
    }
    return res;
}

void TC_Hem_aeRemove::collectUndo( attic::a_document& undoer, HappenLayer& happen_layer )
{
    undoer.reset();
    happen_layer.GetReverseChanges( undoer.set_document_element("undo") );
    wstring ws = undoer.pretty_wstr();
    CPPUNIT_ASSERT( !ws.empty() );
}

void TC_Hem_aeRemove::RemoveFutureEvent()
{
        UtLayer<FutureLayer> fl;
        {
        attic::a_document doc;
        doc.load_wide(
            L"<Test>"
            L"<FutureLayer>"
            L"<HemPath>"
            L"<SpotEvent create_time='20180620T152600Z' name='Departure' Bdg='?[09210]' waynum='1' parknum='1' sourcetype='advent' index='1111-111-1111' num='1111' cont='Y'>"
            L"<feat_texts finaldest='09193' />"
            L"</SpotEvent>"
            L"</HemPath>"
            L"</FutureLayer>"
            L"</Test>"
            );
        fl.Deserialize( doc.document_element() );
        }

    CPPUNIT_ASSERT( fl.path_count()==1 );
    CPPUNIT_ASSERT( fl.exist_series( "name", "Departure" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint layer='future' name='Departure' sourcetype='advent' Bdg='?[09210]' create_time='20180620T152600Z' index='1111-111-1111' num='1111' cont='Y' waynum='1' parknum='1'>"
            L"<feat_texts finaldest='09193' />"
        L"</PrePoint>"
        L"<Action code='Remove' District='09180-11420' DistrictRight='Y' />"
        L"</A2F_HINT>"
        );

    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<DistrictList>"
            L"<District code='09180-11420' name='xxx'>"
            L"<Involve esr='09202'/>"
            L"<Involve esr='09210'/>"
            L"<Involve esr='09211'/>"
            L"</District>"
            L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }

    Hem::ApplierCarryOut::aeFutureRemove remo( fl.GetWriteAccess(), dg.get_district(L"09180-11420"),  bill.getHeadFixableThrows() );
    remo.Action();

    CPPUNIT_ASSERT( fl.path_count()==0 );
}

void TC_Hem_aeRemove::RemoveFutureEventOnBoardStation()
{
    UtLayer<FutureLayer> fl;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<Test>"
            L"<FutureLayer>"
            L"<HemPath>"
            L"<SpotEvent create_time='20190109T142500Z' name='Form' Bdg='?[11420]' waynum='6' parknum='1' sourcetype='advent' index='' num='2222' through='Y'>"
            L"<feat_texts finaldest='09180' />"
            L"</SpotEvent>"
            L"<SpotEvent create_time='20190109T143714Z' name='Departure' Bdg='?[11420]' waynum='6' parknum='1' />"
            L"<SpotEvent create_time='20190109T145514Z' name='Transition' Bdg='mock[09190]' waynum='1' parknum='1' />"
            L"<SpotEvent create_time='20190109T150319Z' name='Arrival' Bdg='mock[09191]' waynum='1' parknum='1' />"
            L"</HemPath>"
            L"</FutureLayer>"
            L"</Test>"
            );
        fl.Deserialize( doc.document_element() );
    }

    CPPUNIT_ASSERT( fl.path_count()==1 );
    CPPUNIT_ASSERT( fl.exist_series( "name", "Form Departure Transition Arrival" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20190109T142936Z'>"
        L"<PrePoint layer='future' name='Form' sourcetype='advent' Bdg='?[11420]' create_time='20190109T142500Z' index='' num='2222' through='Y' waynum='6' parknum='1'>"
        L"<feat_texts finaldest='09180' />"
        L"</PrePoint>"
        L"<Action code='Remove' District='09180-11420' DistrictRight='Y' />"
        L"</A2F_HINT>"
        );

    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<DistrictList>"
            L"<District code='09180-11420' name='xxx'>"
            L"<Involve esr='09190:11420'/>"
            L"<Involve esr='09190'/>"
            L"<Involve esr='09191'/>"
            L"</District>"
            L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }

    Hem::ApplierCarryOut::aeFutureRemove remo( fl.GetWriteAccess(), dg.get_district(L"09180-11420"),  bill.getHeadFixableThrows() );
    remo.Action();

    CPPUNIT_ASSERT( fl.path_count()==0 );
}

void TC_Hem_aeRemove::UncontrolledZoneEnded()
{
    UtLayer<HappenLayer> hl;
    AsoupLayer asoupLayer;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20201002T043400Z' name='Transition' Bdg='ASOUP 1042?p1w1[09280]' waynum='1' parknum='1' index='0918-089-0933' num='3535' length='1' weight='19' outbnd='Y'>"
        L"<Locomotive Series='ЧМЭ3М' NumLoc='5953' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GARKULJS' Tim_Beg='2020-10-02 05:50' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201002T045200Z' name='Transition' Bdg='ASOUP 1042?p1w2[09290]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20201002T051700Z' name='Transition' Bdg='ASOUP 1042?p1w1[09303]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20201002T055500Z' name='Arrival' Bdg='ASOUP 1042_1p3w[09320]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20201002T060100Z' name='Departure' Bdg='ASOUP 1042_1p3w[09320]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20201002T061000Z' name='Arrival' Bdg='ASOUP 1042_1p2w[09330]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20201002T061000Z' name='Death' Bdg='ASOUP 1042_1p2w[09330]' waynum='2' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Transition Transition Arrival Departure Arrival Death" ) );
    auto allPathes = hl.GetWriteAccess()->getAllPathes();
    CPPUNIT_ASSERT_EQUAL(size_t(1), allPathes.size());
    auto path = hl.GetWriteAccess()->getPath( (*allPathes.begin())->GetLastEvent() );
    guessTransciever.registerPath(path, 1);

    UncontrolledStations uncontrolledStations;
    {
    const std::wstring uncontrolledStr = L"<export>\n"
        L"<UncontrolledSCB ESR='09320' />"
        L"<UncontrolledSCB ESR='09330' />"
        L"</export>";

    attic::a_document doc;
    doc.load_wide(uncontrolledStr);
    uncontrolledStations.deserialize(doc.document_element());
    ASSERT( uncontrolledStations.contains(EsrKit(9330)));
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Transition' Bdg='ASOUP 1042?p1w2[09290]' create_time='20201002T045200Z' waynum='2' parknum='1' />"
        L"  <Action code='Remove' />"
        L"</A2F_HINT>"
        );
    Hem::ApplierCarryOut::aeRemove remo( uncontrolledStations, guessTransciever, hl.GetWriteAccess(), asoupLayer.GetWriteAccess(), bill.getHeadSpotThrows() );
    remo.Action();

    // данные отката
    attic::a_document undoer;
    collectUndo( undoer, hl );
    CPPUNIT_ASSERT( hl.path_count()==0 );
    // отмена
    CPPUNIT_ASSERT( implementUndo(undoer,hl) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Transition Transition Arrival Departure Arrival Death" ) );
}

void TC_Hem_aeRemove::GuessUnknownedPath()
{
    UtLayer<HappenLayer> hl;
    AsoupLayer asoupLayer;
    hl.createPath(
        L"<HemPath>"
        L"  <SpotEvent create_time='20160901T080917Z' name='Form' Bdg='PAP[11343]'>"
        L"    <rwcoord picketing1_val='505~400' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160901T080917Z' name='Departure' Bdg='PAP[11343]'>"
        L"    <rwcoord picketing1_val='505~400' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160901T080925Z' name='Span_move' Bdg='N15P[11343:11360]' waynum='1'>"
        L"    <rwcoord picketing1_val='505~400' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='506~720' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160901T080925Z' name='Death' Bdg='N15P[11343:11360]' waynum='1'>"
        L"    <rwcoord picketing1_val='505~400' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='506~720' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Span_move Death" ) );
    auto allPathes = hl.GetWriteAccess()->getAllPathes();
    CPPUNIT_ASSERT_EQUAL(size_t(1), allPathes.size());
    auto path = hl.GetWriteAccess()->getPath( (*allPathes.begin())->GetLastEvent() );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_move' Bdg='N15P[11343:11360]' create_time='20160901T080925Z' waynum='1'>"
        L"    <rwcoord picketing1_val='506~60' picketing1_comm='Россия' />"
        L"  </PrePoint>"
        L"  <Action code='Remove' />"
        L"</A2F_HINT>"
        );

    UncontrolledStations uncontrolledStations;
    Hem::ApplierCarryOut::aeRemove remo( uncontrolledStations, guessTransciever, hl.GetWriteAccess(), asoupLayer.GetWriteAccess(), bill.getHeadSpotThrows() );
    remo.Action();

    // данные отката
    attic::a_document undoer;
    collectUndo( undoer, hl );
    CPPUNIT_ASSERT( hl.path_count()==0 );
    // отмена
    CPPUNIT_ASSERT( implementUndo(undoer,hl) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Span_move Death" ) );
}
