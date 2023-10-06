#include "stdafx.h"

#include "TC_Hem_PathXmlCache.h"
#include "UtHemHelper.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/GuessTransciever.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/FutureLayer.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Hem/Appliers/aeAttach.h"
#include "../Hem/Appliers/aeRectify.h"
#include "../Hem/Appliers/Context.h"
#include "../Hem/UncontrolledStations.h"
#include "../helpful/StationsDigraph.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/LocoCharacteristics.h"
#include "TopologyTest.h"
#include "../helpful/NsiBasis.h"

using namespace std;
using namespace HemHelpful;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_PathXmlCache );

void TC_Hem_PathXmlCache::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_Hem_PathXmlCache::ChangesHappenLayer()
{
    const EsrKit K(1100,1200);
    const time_t T=3600*(240+23);
    const auto SPAN_MOVE = HCode::SPAN_MOVE;

    UtLayer<HappenLayer> layer;

    {
        // начать нить
        SpotEventPtr path1[] = {
            SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"1w",K), T+1 ) ),
            SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"2w",K), T+2 ) ),
        };
        TrioSpot trio;
        trio.body( begin(path1), end(path1) );
        Hem::aePusherAutoHappen pusher( layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( size_array(path1) == pusher.AppliedEvents().size() );

        attic::a_document chan("ch");
        bool have_changes;
        std::tie(have_changes,std::ignore) = layer.TakeChanges( chan.document_element() );
        CPPUNIT_ASSERT( have_changes );
        CPPUNIT_ASSERT( chan.select_nodes("//Trio/Pre").empty() );
        CPPUNIT_ASSERT( chan.select_nodes("//Trio/Body").size() == 2 );
    }
    {
        // продолжить нить
        TrioSpot trio;
        trio.pre( SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"2w",K), T+2 ) ) );
        trio.body( SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"3w",K), T+3 ) ) );

        Hem::aePusherAutoHappen pusher( layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( 1 == pusher.AppliedEvents().size() );

        attic::a_document chan("ch");
        bool have_changes;
        std::tie(have_changes,std::ignore) = layer.TakeChanges( chan.document_element() );
        CPPUNIT_ASSERT( have_changes );
        CPPUNIT_ASSERT( chan.select_nodes("//Trio/Pre").size() == 1 );
        CPPUNIT_ASSERT( chan.select_nodes("//Trio/Pre[@cover='Y']").empty() );
        CPPUNIT_ASSERT( chan.select_nodes("//Trio/Body").size() == 1 );
    }
    {
        // взять нить для изменений, но никаких изменений не делать
        TrioSpot trio;
        trio.pre( SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"3w",K), T+3 ) ) );
        Hem::aePusherAutoHappen pusher( layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( 0 == pusher.AppliedEvents().size() );

        attic::a_document chan("ch");
        bool have_changes;
        std::tie(have_changes,std::ignore) = layer.TakeChanges( chan.document_element() );
        CPPUNIT_ASSERT( !have_changes );
        CPPUNIT_ASSERT( chan.select_nodes("//Trio/Pre").empty() );
        CPPUNIT_ASSERT( chan.select_nodes("//Trio/Body").empty() );
    }
}

void TC_Hem_PathXmlCache::ChangesAllEvents()
{
    const EsrKit K(1100,1200);
    const time_t T=3600*(240+23);
    const auto SPAN_MOVE = HCode::SPAN_MOVE;

    UtLayer<HappenLayer> layer;

    SpotEventPtr path1[] = {
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"1w",K), T+1 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"2w",K), T+2 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"3w",K), T+3 ) ),
    };
    SpotEventPtr path2[] = {
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"zz1",K), T+1 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"zz2",K), T+2 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"zz3",K), T+3 ) ),
    };
    // сделать нить из 3 событий
    {
        TrioSpot trio;
        trio.body( begin(path1), end(path1) );
        Hem::aePusherAutoHappen pusher( layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT_EQUAL( size_array(path1), pusher.AppliedEvents().size() );
        layer.TakeChanges( attic::a_node() );
    }
    // подменим все события поштучно - нить сохранится, но будет вся измененная
    {
        for( size_t i=0; i<size_array(path2); ++i )
        {
            TrioSpot trioHead;
            trioHead.pre ( path1[i] );
            trioHead.post( path1[i] );
            trioHead.coverPre( true );
            trioHead.coverPost( true );
            trioHead.body( path2[i] );
            Hem::aePusherAutoHappen pusher( layer.GetWriteAccess(), trioHead, identifyCategory );
            pusher.Action();
            CPPUNIT_ASSERT_EQUAL( (size_t)1, pusher.AppliedEvents().size() );
        }

        attic::a_document chan("ch");
        bool have_changes;
        std::tie(have_changes,std::ignore) = layer.TakeChanges( chan.document_element() );
        CPPUNIT_ASSERT( have_changes );
        wstring ws = chan.pretty_wstr();
        CPPUNIT_ASSERT( !ws.empty() );
        CPPUNIT_ASSERT_EQUAL( (size_t)1, chan.select_nodes("//Trio/Pre[@cover='Y']").size() );
        CPPUNIT_ASSERT_EQUAL( (size_t)1, chan.select_nodes("//Trio/Post[@cover='Y']").size() );
        CPPUNIT_ASSERT_EQUAL( (size_t)3, chan.select_nodes("//Trio/Body").size() );
    }
}

void TC_Hem_PathXmlCache::ChangesFutureLayer()
{
    const EsrKit K(1100,1200);
    const time_t T=3600*(240+23);
    const auto SPAN_MOVE = HCode::SPAN_MOVE;

    FutureLayer layer;

    FixableEventPtr path1[] = {
        FixableEventPtr( new FixableEvent( SpotEvent( SPAN_MOVE, BadgeE(L"1w",K), T+1 ), FixableEvent::FixatingKind::MOVEABLE ) ),
        FixableEventPtr( new FixableEvent( SpotEvent( SPAN_MOVE, BadgeE(L"2w",K), T+2 ), FixableEvent::FixatingKind::MOVEABLE ) ),
        FixableEventPtr( new FixableEvent( SpotEvent( SPAN_MOVE, BadgeE(L"3w",K), T+3 ), FixableEvent::FixatingKind::MOVEABLE ) ),
    };
    FixableEvent path2[] = {
        FixableEvent( SpotEvent( SPAN_MOVE, BadgeE(L"xx",K), T+1 ), FixableEvent::FixatingKind::MOVEABLE ),
        FixableEvent( SpotEvent( SPAN_MOVE, BadgeE(L"2w",K), T+2 ), FixableEvent::FixatingKind::FIXATED ),
        FixableEvent( SpotEvent( SPAN_MOVE, BadgeE(L"3w",K), T+3 ), FixableEvent::FixatingKind::FIXATED ),
    };
    CPPUNIT_ASSERT( size_array(path1) == size_array(path2) );
    // сделать нить из 3 событий
    {
        auto futureWriteAccess = layer.GetWriteAccess();
        bool success = Hem::CreatePath<FixableEvent>( futureWriteAccess, path1[0] );
        CPPUNIT_ASSERT( success );
        std::shared_ptr<Hem::FixablePath> pathPtr;
        pathPtr = futureWriteAccess->getPath( path1[0] );
        for( auto it = std::begin(path1)+1; std::end(path1) != it; ++it )
        {
            success = futureWriteAccess->pushBack( *it, pathPtr );
            CPPUNIT_ASSERT( success );
        }
    }
    layer.TakeChanges( attic::a_node() );
    // подменим все события поштучно - нить сохранится, но будет вся измененная
    {
        auto futureWriteAccess = layer.GetWriteAccess();
        std::wstring errmsg;
        std::vector<FixableEvent> insertEvents( std::begin(path2), std::end(path2) );
        bool success = futureWriteAccess->replaceEvents( path1[0], path1[ size_array(path1)-1 ], insertEvents, errmsg );
        CPPUNIT_ASSERT( success );
    }
    {
        attic::a_document chan("ch");
        bool have_changes = layer.TakeChanges( chan.document_element() );
        CPPUNIT_ASSERT( have_changes );
        wstring ws = chan.pretty_wstr();
        CPPUNIT_ASSERT( !ws.empty() );
        CPPUNIT_ASSERT_EQUAL( (size_t)1, chan.select_nodes("//Trio/Pre[@cover='Y']").size() );
        CPPUNIT_ASSERT_EQUAL( (size_t)1, chan.select_nodes("//Trio/Post[@cover='Y']").size() );
        CPPUNIT_ASSERT_EQUAL( (size_t)3, chan.select_nodes("//Trio/Body").size() );
        CPPUNIT_ASSERT_EQUAL( (size_t)2, chan.select_nodes("//Trio/Body[@sourcetype='fix']").size() );
    }
}


void TC_Hem_PathXmlCache::ReplaceSingleEvent()
{
    UtLayer<HappenLayer> happen_layer;
    // создать нить
    attic::a_document doc;
    doc.load_wide(
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='1C[11412]' create_time='20151007T213432Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Departure' Bdg='1C[11412]' create_time='20151007T213432Z' waynum='1' parknum='1' index='' num='2457' fretran='Y' />"
        L"    <SpotEvent name='Span_move' Bdg='P2PC[11412:11420]' create_time='20151007T213555Z' waynum='1' />"
        L"    <SpotEvent name='Span_move' Bdg='N1PC[11412:11420]' create_time='20151007T215345Z' waynum='1' />"
        L"    <SpotEvent name='Transition' Bdg='3C[11420]' create_time='20151007T215713Z' waynum='3' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='PRPP[11420]' create_time='20151007T215959Z' />"
        L"</HemPath>"
        );
    happen_layer.createPath( doc.pretty_wstr() );

    // подменим единственное событие
    {
        attic::a_node trans_node = doc.document_element().last_child().previous_sibling();
        TrioSpot trioHead;
        trioHead.pre ( SpotEventPtr( new SpotEvent( trans_node ) ) );
        trioHead.post( trioHead.pre() );
        trioHead.coverPre( true );
        trioHead.coverPost( true );
        attic::a_document subst;
        subst.load_wide(
            L"<SpotEvent name='Transition' Bdg='xxx[11420]' create_time='20151007T215713Z' waynum='33' parknum='11' />"
            );
        trioHead.body( SpotEventPtr( new SpotEvent( subst.document_element() ) ) );

        Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trioHead, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( pusher.AppliedEvents().size()==1 );

        attic::a_document chan("ch");
        bool have_changes;
        std::tie(have_changes,std::ignore) = happen_layer.TakeChanges( chan.document_element() );
        CPPUNIT_ASSERT( have_changes );
        wstring ws = chan.pretty_wstr();
        CPPUNIT_ASSERT( !ws.empty() );
        CPPUNIT_ASSERT_EQUAL( (size_t)1, chan.select_nodes("//Trio/Pre[@cover='Y']").size() );
        CPPUNIT_ASSERT_EQUAL( (size_t)1, chan.select_nodes("//Trio/Post[@cover='Y']").size() );
        CPPUNIT_ASSERT_EQUAL( (size_t)1, chan.select_nodes("//Trio/Body").size() );
    }
}

void TC_Hem_PathXmlCache::ChangesBySplit()
{
    const EsrKit K(1100,1200);
    const time_t T=3600*(240+23);
    const auto SPAN_MOVE = HCode::SPAN_MOVE;

    UtLayer<HappenLayer> layer;

    SpotEventPtr path1[] = {
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"0w",K), T+0 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"1w",K), T+1 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"2w",K), T+2 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"3w",K), T+3 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"4w",K), T+4 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"5w",K), T+5 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"6w",K), T+6 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"7w",K), T+7 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"8w",K), T+8 ) ),
        SpotEventPtr( new SpotEvent( SPAN_MOVE, BadgeE(L"9w",K), T+9 ) ),
    };
    const size_t N = size_array(path1);
    // сделать нить
    {
        TrioSpot trio;
        trio.body( begin(path1), end(path1) );
        Hem::aePusherAutoHappen pusher( layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        layer.TakeChanges( attic::a_node() ); // фиксируем все изменения
        CPPUNIT_ASSERT( layer.path_count()==1 );
        CPPUNIT_ASSERT( layer.exist_path_size( N ) );
    }
    // разрываем после index и восстанавливаем обратно
    for( size_t index = 2; index < N-2; ++index )
    {
        TrioSpot trioCut;
        trioCut.pre ( path1[index] );
        trioCut.post( path1[ N-1 ] );
        trioCut.coverPre( true );
        trioCut.coverPost( true );
        TrioSpot trioMake;
        trioMake.body( begin(path1)+index, end(path1) );

        Hem::aePusherAutoHappen cutter( layer.GetWriteAccess(), trioCut, identifyCategory );
        cutter.Action();
        Hem::aePusherAutoHappen maker( layer.GetWriteAccess(), trioMake, identifyCategory );
        maker.Action();

        attic::a_document ud("undo");
        layer.GetReverseChanges( ud.document_element() );

        bool have_changes;
        std::tie(have_changes,std::ignore) = layer.TakeChanges( attic::a_node() );
        CPPUNIT_ASSERT( have_changes );

        CPPUNIT_ASSERT( layer.path_count()==2 );
        CPPUNIT_ASSERT( layer.exist_path_size(index) );
        CPPUNIT_ASSERT( layer.exist_path_size(N-index) );

        for( auto trNode : ud.document_element().child(HappenLayer_xAttr).children() )
        {
            TrioSpot eTrio;
            trNode >> eTrio;
            Hem::aePusherUserHappen pusher( layer.GetWriteAccess(), eTrio, identifyCategory );
            pusher.Action();
        }
        CPPUNIT_ASSERT( layer.path_count()==1 );
        CPPUNIT_ASSERT( layer.exist_path_size(N) );
    }
}

void TC_Hem_PathXmlCache::setTopologyPath( unsigned from, unsigned to, TopologyTest* topology, unsigned third )
{
    std::wstringstream wss;
    wss << L"=" << from << L"," << to << L"," << third << L"\n";

    std::wstring topologyParseError = topology->Load(wss);
    CPPUNIT_ASSERT_MESSAGE(To1251(topologyParseError), topologyParseError.empty());

    auto pathes = topology->findPaths(EsrKit(from), EsrKit(to));
    CPPUNIT_ASSERT(!pathes.empty());
    CPPUNIT_ASSERT_EQUAL(size_t(1), pathes.size());
}

void TC_Hem_PathXmlCache::ChangesByConcatenate()
{
    const time_t T=3600*(240+23);
    const auto TRANSIT = HCode::TRANSITION;

    SpotEventPtr path1[] = {
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"0w",EsrKit(1000+0)), T+0 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"1w",EsrKit(1000+1)), T+1 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"2w",EsrKit(1000+2)), T+2 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"3w",EsrKit(1000+3)), T+3 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"4w",EsrKit(1000+4)), T+4 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"5w",EsrKit(1000+5)), T+5 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"6w",EsrKit(1000+6)), T+6 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"7w",EsrKit(1000+7)), T+7 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"8w",EsrKit(1000+8)), T+8 ) ),
        SpotEventPtr( new SpotEvent( TRANSIT, BadgeE(L"9w",EsrKit(1000+9)), T+9 ) ),
    };
    const size_t N = size_array(path1);
    // разрываем после index и восстанавливаем обратно
    for( size_t index = 2; index < N-2; ++index )
    {
        UtLayer<HappenLayer> layer;
        UtLayer<AsoupLayer> asouplayer;
        // сделать две нити
        {
            TrioSpot trio1, trio2;
            trio1.body( begin(path1),       begin(path1)+index );
            trio2.body( begin(path1)+index, end(path1) );
            {
                Hem::aePusherAutoHappen pusher2( layer.GetWriteAccess(), trio2, identifyCategory );
                pusher2.Action();
            }
            {
                Hem::aePusherAutoHappen pusher1( layer.GetWriteAccess(), trio1, identifyCategory );
                pusher1.Action();
            }
            layer.TakeChanges( attic::a_node() ); // фиксируем все изменения

            CPPUNIT_ASSERT( layer.path_count()==2 );
            CPPUNIT_ASSERT( layer.exist_path_size( index ) );
            CPPUNIT_ASSERT( layer.exist_path_size( N-index ) );
        }

        // склеить их
        std::shared_ptr<TopologyTest> topology( new TopologyTest );
        setTopologyPath(path1[index - 1]->GetBadge().num().getTerm(), 
            path1[index]->GetBadge().num().getTerm(), topology.get());
        
        EsrGuide esrGuide;
        Hem::GuessTransciever guessTransciever;
        UncontrolledStations uncontrolledStations;
        Context context(Hem::Chance::System(),  esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
		Hem::ApplierCarryOut::aeAttach att( Hem::AccessHappenAsoup(layer.base(), asouplayer.base()), nullptr, context, *path1[index-1], *path1[index], false );
        att.Action();
        attic::a_document ud("undo");
        layer.GetReverseChanges( ud.document_element() );
        
        // проверить, что получилась одна нить
        bool have_changes;
        std::tie(have_changes,std::ignore) = layer.TakeChanges( attic::a_node() );
        CPPUNIT_ASSERT( have_changes );
        CPPUNIT_ASSERT( layer.path_count()==1 );
        CPPUNIT_ASSERT( layer.exist_series( "name", "Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition " ) );
        CPPUNIT_ASSERT( layer.exist_path_size(N) );
        // выполнить откат
        for( auto trNode : ud.document_element().child(HappenLayer_xAttr).children() )
        {
            TrioSpot eTrio;
            trNode >> eTrio;
            Hem::aePusherUserHappen pusher( layer.GetWriteAccess(), eTrio, identifyCategory );
            pusher.Action();
        }
        CPPUNIT_ASSERT( layer.path_count()==2 );
        CPPUNIT_ASSERT( layer.exist_path_size( index ) );
        CPPUNIT_ASSERT( layer.exist_path_size( N-index ) );
    }
}

// Тест на применимость измений из AutoChart для UserChart
void TC_Hem_PathXmlCache::Auto2User()
{
    UtLayer<HappenLayer> autohappen;
    // создать нить AutoChart с множеством промежуточных событий, которые в UserChart не сохраняются
    attic::a_document doc;
    doc.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Span_move' Bdg='P12P_LIK[11442:11443]' create_time='20160316T222449Z' waynum='1'>"
        L"    <rwcoord picketing1_val='373~215' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='372~905' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='P14P_LIK[11442:11443]' create_time='20160316T222519Z' waynum='1'>"
        L"    <rwcoord picketing1_val='372~905' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='372~125' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='P16P_LIK[11442:11443]' create_time='20160316T222644Z' waynum='1'>"
        L"    <rwcoord picketing1_val='371~361' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='372~125' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Station_entry' Bdg='NAP[11442]' create_time='20160316T222829Z' />"
        L"  <SpotEvent name='Transition' Bdg='1AC[11442]' create_time='20160316T223050Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Station_exit' Bdg='PAP[11442]' create_time='20160316T223115Z' />"
        L"  <SpotEvent name='Span_move' Bdg='N9P_NIC[11432:11442]' create_time='20160316T225256Z' waynum='1'>"
        L"    <rwcoord picketing1_val='352~954' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='352~654' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='N7P_NIC[11432:11442]' create_time='20160316T225326Z' waynum='1'>"
        L"    <rwcoord picketing1_val='352~174' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='352~654' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='N5P_NIC[11432:11442]' create_time='20160316T225416Z' waynum='1'>"
        L"    <rwcoord picketing1_val='351~922' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='352~174' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='N3P_NIC[11432:11442]' create_time='20160316T225439Z' waynum='1'>"
        L"    <rwcoord picketing1_val='351~672' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='351~922' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='N1P_NIC[11432:11442]' create_time='20160316T225458Z' waynum='1'>"
        L"    <rwcoord picketing1_val='351~454' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='351~672' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Station_entry' Bdg='NAP[11432]' create_time='20160316T225518Z' />"
        L"</HemPath>"
        );
    autohappen.createPath( doc.pretty_wstr() );
    // изменение 1 - подмена одного события в середине (из сохраняющихся в UserChart)
    {
        auto x = createEvent<SpotEvent>(
            L"<SpotEvent name='Transition' Bdg='1AC[11442]' create_time='20160316T223050Z' waynum='1' parknum='1' />"
            );
        auto y = createEvent<SpotEvent>(
            L"<SpotEvent name='Transition' Bdg='1AC[11442]' create_time='20160316T223050Z' waynum='1' parknum='1' index='' num='2703' fretran='Y' />"
            );
        TrioSpot trio;
        trio.pre ( SpotEventPtr( new SpotEvent( x ) ) );
        trio.post( trio.pre() );
        trio.coverPre( true );
        trio.coverPost( true );
        trio.body( SpotEventPtr( new SpotEvent( y ) ) );

        Hem::aePusherAutoHappen replacer( autohappen.GetWriteAccess(), trio, identifyCategory );
        replacer.Action();
        CPPUNIT_ASSERT( replacer.AppliedEvents().size()==1 );
    }
    // изменение 2 - добавить событие в конец ( последнее событие всегда сохраняется в UserChart )
    {
        auto a = createEvent<SpotEvent>(
            L"<SpotEvent name='Station_entry' Bdg='NAP[11432]' create_time='20160316T225518Z' />"
            );
        auto b = createEvent<SpotEvent>(
            L"<SpotEvent name='Arrival' Bdg='1C[11432]' create_time='20160316T225703Z' waynum='1' parknum='1' />"
            );
        TrioSpot trio;
        trio.pre ( SpotEventPtr( new SpotEvent( a ) ) );
        trio.post( trio.pre() );
        trio.coverPre( false );
        trio.coverPost( true );
        trio.body( SpotEventPtr( new SpotEvent( b ) ) );

        Hem::aePusherAutoHappen appender( autohappen.GetWriteAccess(), trio, identifyCategory );
        appender.Action();
        CPPUNIT_ASSERT( appender.AppliedEvents().size()==1 );
    }
    // получим и проверим сразу все изменения этой нити
    attic::a_document chan("ch");
    {
        autohappen.TakeChanges( chan.document_element() );
        CPPUNIT_ASSERT_EQUAL( (size_t)2, chan.select_nodes("//Trio").size() );
        attic::a_node preNode = chan.select_node("//Trio/Pre").node();
        CPPUNIT_ASSERT( preNode.attribute("cover").as_bool() );
        attic::a_node postNode = chan.select_node("//Trio/Post").node();
        CPPUNIT_ASSERT( postNode.attribute("cover").as_bool() );
        CPPUNIT_ASSERT_EQUAL( (size_t)2, chan.select_nodes("//Trio/Body").size() ); // фактических изменений всего
    }
    // теперь построим пользовательский график на тех же исходных данных
    UtLayer<HappenLayer> userhappen;
    userhappen.createPath( doc.pretty_wstr() );
    {
        // почистим нить - такой она будет в UserChart
        auto some = createEvent<SpotEvent>(
            L"<SpotEvent name='Transition' Bdg='1AC[11442]' create_time='20160316T223050Z' waynum='1' parknum='1' />"
            );
        Hem::aeRectify rectifier( userhappen.GetWriteAccess(), SpotEventPtr( new SpotEvent( some ) ) );
        rectifier.Action();
        CPPUNIT_ASSERT( userhappen.path_count() == 1 );
        CPPUNIT_ASSERT( userhappen.exist_series( "name", "Span_move Transition Station_entry" ) );
    }
    // и изменения из AutoChart приложим к UserChart
    for( auto trNode : chan.document_element().child(HappenLayer_xAttr).children() )
    {
        TrioSpot eTrio;
        trNode >> eTrio;
        Hem::aePusherUserHappen pusher( userhappen.GetWriteAccess(), eTrio, identifyCategory );
        pusher.Action();
        Hem::aeRectify rectifier( userhappen.GetWriteAccess(), eTrio.body().front() );
        rectifier.Action();
    }
    // проверим, что изменения подошли
    CPPUNIT_ASSERT( userhappen.exist_series( "name", "Span_move Transition Arrival" ) );
}

void TC_Hem_PathXmlCache::Auto2User_Misplaced()
{
    UtLayer<HappenLayer> hA;
   // построить нити Auto и User
    // создать нить AutoChart с множеством промежуточных событий, которые в UserChart не сохраняются
    const wstring data=  
        L"<HemPath>"
        L" <SpotEvent name='Form'          Bdg='2C[11260]'        create_time='20160521T110013Z' waynum='2' parknum='1' />"
        L" <SpotEvent name='Departure'     Bdg='2C[11260]'        create_time='20160521T112838Z' waynum='2' parknum='1' />"
        L" <SpotEvent name='Station_exit'  Bdg='CP[11260]'        create_time='20160521T113024Z' />"
        L" <SpotEvent name='Span_move'     Bdg='N7P[11260:11265]' create_time='20160521T113024Z' waynum='1' />"
        L" <SpotEvent name='Span_move'     Bdg='N5P[11260:11265]' create_time='20160521T113025Z' waynum='1' />"
        L" <SpotEvent name='Span_move'     Bdg='N7P[11260:11265]' create_time='20160521T113050Z' waynum='1' />"
        L" <SpotEvent name='Station_entry' Bdg='CP[11260]'        create_time='20160521T113050Z' />"
        L"</HemPath>";
    SpotEventPtr starter( new SpotEvent( createEvent<SpotEvent>(
        L" <SpotEvent name='Form'          Bdg='2C[11260]'        create_time='20160521T110013Z' waynum='2' parknum='1' />"
        ) ) );
    hA.createPath( data );
    auto wholeBefore = hA.result()->pretty_wstr();
    CPPUNIT_ASSERT( !wholeBefore.empty() );

    // изменения
    {
        TrioSpot trio1 = createEvent<TrioSpot>(
            L"<Trio>"
            L"<Pre name='Station_entry' Bdg='CP[11260]' create_time='20160521T113050Z' />"
            L"<Body name='Station_exit' Bdg='CP[11260]' create_time='20160521T113050Z' />"
            L"</Trio>" );
        TrioSpot trio2 = createEvent<TrioSpot>(
            L"<Trio>"
            L"<Pre name='Station_exit' Bdg='CP[11260]' create_time='20160521T113050Z' />"
            L"<Body name='Span_move' Bdg='N7P[11260:11265]' create_time='20160521T113050Z' waynum='1' />"
            L"</Trio>" );
        TrioSpot trio3 = createEvent<TrioSpot>(
            L"<Trio>"
            L"<Pre name='Span_move' Bdg='N7P[11260:11265]' create_time='20160521T113050Z' waynum='1' />"
            L"<Body name='Span_move' Bdg='N5P[11260:11265]' create_time='20160521T113050Z' waynum='1' />"
            L"</Trio>" );
        Hem::aePusherAutoHappen pusher1( hA.GetWriteAccess(), trio1, identifyCategory );
        pusher1.Action();
        CPPUNIT_ASSERT( pusher1.AppliedEvents().size()==1 );
        Hem::aePusherAutoHappen pusher2( hA.GetWriteAccess(), trio2, identifyCategory );
        pusher2.Action();
        CPPUNIT_ASSERT( pusher2.AppliedEvents().size()==1 );
        Hem::aePusherAutoHappen pusher3( hA.GetWriteAccess(), trio3, identifyCategory );
        pusher3.Action();
        CPPUNIT_ASSERT( pusher3.AppliedEvents().size()==1 );
    }
    // получим сразу все изменения этой нити
    attic::a_document chan("ch");
    {
        hA.TakeChanges( chan.document_element() );
        
        auto wholeAfter = hA.result()->pretty_wstr();
        CPPUNIT_ASSERT( !wholeAfter.empty() );
        auto ws = chan.pretty_wstr();
        CPPUNIT_ASSERT( !ws.empty() );
    }
    // теперь построим пользовательский график на тех же исходных данных
    UtLayer<HappenLayer> hU;
    hU.createPath(data );
    {
        // почистим нить - такой она будет в UserChart
        Hem::aeRectify rectifier( hU.GetWriteAccess(), starter );
        rectifier.Action();
        CPPUNIT_ASSERT( hU.path_count() == 1 );
        CPPUNIT_ASSERT( hU.exist_series( "name", "Form Departure Station_entry" ) );
    }
    // и изменения из AutoChart приложим к UserChart
    for( auto trNode : chan.document_element().child(HappenLayer_xAttr).children() )
    {
        TrioSpot eTrio;
        trNode >> eTrio;
        Hem::aePusherUserHappen pusher( hU.GetWriteAccess(), eTrio, identifyCategory );
        pusher.Action();
        Hem::aeRectify rectifier( hU.GetWriteAccess(), starter );
        rectifier.Action();
    }
    // проверим, что изменения подошли
    CPPUNIT_ASSERT( hU.exist_series( "name", "Form Departure Station_entry Station_exit Span_move" ) );
}

void TC_Hem_PathXmlCache::MultiReplaceSingleEvent()
{
    UtLayer<HappenLayer> happen_layer;
    // создать нить
    attic::a_document doc;
    doc.load_wide(
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='1C[11412]' create_time='20151007T213432Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Departure' Bdg='1C[11412]' create_time='20151007T213432Z' waynum='1' parknum='1' index='' num='2457' fretran='Y' />"
        L"    <SpotEvent name='Span_move' Bdg='P2PC[11412:11420]' create_time='20151007T213555Z' waynum='1' />"
        L"    <SpotEvent name='Span_move' Bdg='N1PC[11412:11420]' create_time='20151007T215345Z' waynum='1' />"
        L"    <SpotEvent name='Transition' Bdg='3C[11420]' create_time='20151007T215713Z' waynum='3' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='PRPP[11420]' create_time='20151007T215959Z' />"
        L"</HemPath>"
        );
    happen_layer.createPath( doc.pretty_wstr() );

    // подменим одно событие в конце
    {
        attic::a_node trans_node = doc.document_element().last_child().previous_sibling();
        TrioSpot trioHead;
        trioHead.pre ( SpotEventPtr( new SpotEvent( trans_node ) ) );
        trioHead.post( trioHead.pre() );
        trioHead.coverPre( true );
        trioHead.coverPost( true );
        attic::a_document subst;
        subst.load_wide(
            L"<SpotEvent name='Transition' Bdg='xxx[11420]' create_time='20151007T215713Z' waynum='33' parknum='11' />"
            );
        trioHead.body( SpotEventPtr( new SpotEvent( subst.document_element() ) ) );

        Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trioHead, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( pusher.AppliedEvents().size()==1 );
    }
    // подменим одно событие в начале
    {
        attic::a_node trans_node = doc.document_element().first_child();
        TrioSpot trioHead;
        trioHead.pre ( SpotEventPtr( new SpotEvent( trans_node ) ) );
        trioHead.post( trioHead.pre() );
        trioHead.coverPre( true );
        trioHead.coverPost( true );
        attic::a_document subst;
        subst.load_wide(
            L"<SpotEvent name='Form' Bdg='Z-Z-Z[11412]' create_time='20151007T213432Z' waynum='1' parknum='1' />"
            );
        trioHead.body( SpotEventPtr( new SpotEvent( subst.document_element() ) ) );

        Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trioHead, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( pusher.AppliedEvents().size()==1 );
    }

    // проверим, что изменений два, в каждом по одному событию
    {
        attic::a_document chan("ch");
        bool have_changes;
        std::tie(have_changes,std::ignore) = happen_layer.TakeChanges( chan.document_element() );
        CPPUNIT_ASSERT( have_changes );
        wstring ws = chan.pretty_wstr();
        CPPUNIT_ASSERT( !ws.empty() );
        CPPUNIT_ASSERT_EQUAL( (size_t)2, chan.select_nodes("//Trio").size() );
        CPPUNIT_ASSERT_EQUAL( (size_t)2, chan.select_nodes("//Trio/Body").size() );
    }
}

void TC_Hem_PathXmlCache::Auto2User_IdentifyNonexistent()
{
    UtLayer<HappenLayer> autohappen;
    // создать нить
    attic::a_document doc;
    doc.load_wide(
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='1C[11412]' create_time='20151007T213432Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Departure' Bdg='1C[11412]' create_time='20151007T213432Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Span_move' Bdg='P2PC[11412:11420]' create_time='20151007T213555Z' waynum='1' />"
        L"    <SpotEvent name='Transition' Bdg='3C[11420]' create_time='20151007T215713Z' waynum='3' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='PRPP[11420]' create_time='20151007T215959Z' />"
        L"</HemPath>"
        );
    autohappen.createPath( doc.pretty_wstr() );

    // идентифицируем одно событие
    {
        attic::a_node depart_node = doc.document_element().first_child().next_sibling();
        TrioSpot trioHead;
        trioHead.pre ( SpotEventPtr( new SpotEvent( depart_node ) ) );
        trioHead.post( trioHead.pre() );
        trioHead.coverPre( true );
        trioHead.coverPost( true );
        SpotDetails spdet;
        TrainDescr td( L"7777", L"1234-567-8901" );
        trioHead.body( make_shared<SpotEvent>( trioHead.pre()->GetCode(),
                                               trioHead.pre()->GetBadge(),
                                               trioHead.pre()->GetTime(),
                                               make_shared<SpotDetails>(td) ) );

        Hem::aePusherAutoHappen pusher( autohappen.GetWriteAccess(), trioHead, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( pusher.AppliedEvents().size()==1 );
    }
    // получим единственное изменение в автографике
    attic::a_document chan("ch");
    bool have_changes;
    std::tie(have_changes,std::ignore) = autohappen.TakeChanges( chan.document_element() );
    CPPUNIT_ASSERT( have_changes );
    wstring ws = chan.pretty_wstr();
    CPPUNIT_ASSERT( !ws.empty() );
    // и приложим его к пустому пользовательскому графику
    UtLayer<HappenLayer> userhappen;
    size_t cnt = 0;
    for( auto trNode : chan.document_element().child(HappenLayer_xAttr).children() )
    {
        TrioSpot eTrio;
        trNode >> eTrio;
        Hem::aePusherUserHappen pusher( userhappen.GetWriteAccess(), eTrio, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( !pusher.haveError() );
        ++cnt;
    }
    CPPUNIT_ASSERT( cnt == 1 ); // изменение должно быть только одно
    // проверим, что в пользовательском графике НЕ появилась ложная нить
    CPPUNIT_ASSERT( autohappen.path_count()==1 );
    CPPUNIT_ASSERT( userhappen.path_count()==0 );
}

void TC_Hem_PathXmlCache::AddManyEventsInHead()
{
    wstring textTrio = 
        L"<Trio>"
        L"  <Pre  create_time='20160906T075426Z' name='Form' Bdg='1C[11420]' waynum='1' parknum='1' cover='Y' />"
        L"  <Post create_time='20160906T075426Z' name='Form' Bdg='1C[11420]' waynum='1' parknum='1' />"
        L"  <Body create_time='20160906T070427Z' name='Form' Bdg='N18P_KOK[09360:11760]' waynum='2' index='0900-008-1600' num='2728' length='57' weight='1349' fretran='Y'>"
        L"    <rwcoord picketing1_val='98~500' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='99~' picketing1_comm='Рига-Зилупе' />"
        L"  <Locomotive Series='2ТЭ10У' NumLoc='224' Dor='0' Depo='0' Kod='0'>"
        L"    <Crew EngineDriver='GORJACHOVS' Tim_Beg='2016-09-06 07:04' />"
        L"  </Locomotive>"
        L"  </Body>"
        L"  <Body create_time='20160906T070747Z' name='Span_move' Bdg='N38P_KOK[09360:11760]' waynum='2'>"
        L"    <rwcoord picketing1_val='103~500' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='104~' picketing1_comm='Рига-Зилупе' />"
        L"  </Body>"
        L"  <Body create_time='20160906T071518Z' name='Station_entry' Bdg='PP_PLA[11760]'>"
        L"    <rwcoord picketing1_val='111~500' picketing1_comm='Рига-Зилупе' />"
        L"  </Body>"
        L"  <Body create_time='20160906T071621Z' name='Transition' Bdg='2AC[11760]' waynum='2' parknum='1' />"
        L"  <Body create_time='20160906T071719Z' name='Station_exit' Bdg='NpP[11760]'>"
        L"    <rwcoord picketing1_val='113~206' picketing1_comm='Рига-Зилупе' />"
        L"  </Body>"
        L"  <Body create_time='20160906T072450Z' name='Span_move' Bdg='N46P[11420:11760]' waynum='2'>"
        L"    <rwcoord picketing1_val='121~82' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='121~440' picketing1_comm='Рига-Зилупе' />"
        L"  </Body>"
        L"  <Body create_time='20160906T073432Z' name='Station_entry' Bdg='PRP_[11420]'>"
        L"    <rwcoord picketing1_val='127~886' picketing1_comm='Рига-Зилупе' />"
        L"  </Body>"
        L"  <Body create_time='20160906T073642Z' name='Death' Bdg='PRP_[11420]'>"
        L"    <rwcoord picketing1_val='127~886' picketing1_comm='Рига-Зилупе' />"
        L"  </Body>"
        L"</Trio>"
        ;

    UtLayer<HappenLayer> userHappen;
    // создать нити
    {
        attic::a_document doc;
        doc.load_wide(
            L"<HemPath>"
            L"  <SpotEvent create_time='20160906T075426Z' name='Form' Bdg='1C[11420]' waynum='1' parknum='1' />"
            L"  <SpotEvent create_time='20160906T075426Z' name='Departure' Bdg='1C[11420]' waynum='1' parknum='1' />"
            L"  <SpotEvent create_time='20160906T080555Z' name='Transition' Bdg='1C[11419]' waynum='1' parknum='1' />"
            L"  <SpotEvent create_time='20160906T081053Z' name='Span_move' Bdg='P6P[11419:11422]' waynum='1'>"
            L"    <rwcoord picketing1_val='317~330' picketing1_comm='Вентспилс' />"
            L"    <rwcoord picketing1_val='316~850' picketing1_comm='Вентспилс' />"
            L"  </SpotEvent>"
            L"</HemPath>"
            );
        userHappen.createPath( doc.pretty_wstr() );
        CPPUNIT_ASSERT( userHappen.path_count()==1 );
    }

    // применим изменения к пользовательскому графику
    {
        TrioSpot trio = createEvent<TrioSpot>( textTrio );
        Hem::aePusherAutoHappen pusher( userHappen.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT( pusher.AppliedEvents().size()==8 );
        CPPUNIT_ASSERT( userHappen.exist_series("name", "Form Span_move Station_entry Transition Station_exit Span_move Station_entry Death Form Departure Transition Span_move ") );
        Hem::aeRectify rectifier( userHappen.GetWriteAccess(), trio.body().front() );
        rectifier.Action();
    }
    // проверим, что в пользовательском графике НЕ появилась ложная нить
    CPPUNIT_ASSERT( userHappen.path_count()==1 );
    CPPUNIT_ASSERT( userHappen.exist_series("name", "Form Transition Arrival Departure Transition Span_move") );
}
