#include "stdafx.h"

#include "TC_HappenLayer.h"
#include "UtHemHelper.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/GuessTransciever.h"
#include "../Hem/AsoupLinker.h"
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Hem/Appliers/aeAsoupToSpot.h"
#include "../Hem/Appliers/aeRectify.h"
#include "../Hem/Appliers/Context.h"
#include "../Hem/Appliers/aeAsoupToSpot.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/Junctions.h"
#include "TopologyTest.h"
#include "../Hem/UserEditInventory.h"
#include "../helpful/NsiBasis.h"
#include "UtNsiBasis.h"

using namespace std;
using namespace HemHelpful;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_HappenLayer );

// построитель события
static SpotEventPtr EV( HCode code, const std::string& iu, time_t mom, EsrKit esrKit=EsrKit(100,200), SpotDetailsPtr spdet = SpotDetailsPtr() )
{
    return std::make_shared<SpotEvent>( code, BadgeE( FromUtf8(iu), esrKit ), mom, spdet );
}

template< class ThePusher >
void TC_HappenLayer::AnyCreate()
{
    HappenLayer happen_layer;

    // создаваемая нить
    SpotEventPtr pat[] = {
        EV( HCode::SPAN_MOVE, "1w", T+1 ),
        EV( HCode::SPAN_MOVE, "2w", T+2 ),
        EV( HCode::SPAN_MOVE, "3w", T+3 ),
    };

    TrioSpot trio;
    trio.body( pat );
    ThePusher pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
    pusher.Action();
    auto sz = pusher.AppliedEvents().size();
    CPPUNIT_ASSERT( trio.body().size() == sz );

    happen_layer.TakeChanges( attic::a_node() );
    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath/SpotEvent").size() == size_array(pat) );
}

template< class ThePusher >
void TC_HappenLayer::AnyAppend()
{
    HappenLayer happen_layer;

    // две половинки для создания нити
    SpotEventPtr one[] = {
        EV( HCode::SPAN_MOVE, "1w", T+1 ),
        EV( HCode::SPAN_MOVE, "2w", T+2 ),
    };
        
    SpotEventPtr two[] = {
        EV( HCode::SPAN_MOVE, "3w", T+3 ),
        EV( HCode::SPAN_MOVE, "4w", T+4 ),
        EV( HCode::SPAN_MOVE, "5w", T+5 ),
    };

    {
        TrioSpot trio1;
        trio1.body( one );
        ThePusher pusher( happen_layer.GetWriteAccess(), trio1, identifyCategory );
        pusher.Action();
        auto sz1 = pusher.AppliedEvents().size();
        CPPUNIT_ASSERT( size_array(one) == sz1 );
    }
    {
        TrioSpot trio2;
        trio2.pre( one[ size_array(one)-1 ] );
        trio2.body( two );
        ThePusher pusher( happen_layer.GetWriteAccess(), trio2, identifyCategory );
        pusher.Action();
        auto sz2 = pusher.AppliedEvents().size();
        CPPUNIT_ASSERT( size_array(two) == sz2 );
    }

    HappenLayer::SpotsVector chart_changes; 
    std::tie(std::ignore, chart_changes) = happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT_EQUAL( 1u, (unsigned)chart_changes.size() );
    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath/SpotEvent").size() == size_array(one)+size_array(two) );
}

template< class ThePusher >
void TC_HappenLayer::AnyChange()
{
    HappenLayer happen_layer;

    // исходные события нити
    {
        SpotEventPtr pat123[] = {
            EV( HCode::SPAN_MOVE, "1w", T+1 ),
            EV( HCode::SPAN_MOVE, "2w", T+2 ),
            EV( HCode::SPAN_MOVE, "3w", T+3 ),
        };
        TrioSpot trio;
        trio.body( pat123 );
        ThePusher pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        auto sz = pusher.AppliedEvents().size();
    }
    // изменения
    {
        SpotEventPtr pat21_22[] = {
            EV( HCode::SPAN_MOVE, "xx21", T+2 ),
            EV( HCode::SPAN_MOVE, "xx22", T+2 ),
        };
        TrioSpot trio;
        trio.pre( EV( HCode::SPAN_MOVE, "1w", T+1 ) );
        trio.body( pat21_22 );
        trio.post( EV( HCode::SPAN_MOVE, "3w", T+3 ) );

        ThePusher pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        CPPUNIT_ASSERT_EQUAL( size_array(pat21_22), pusher.AppliedEvents().size() );
    }

    HappenLayer::SpotsVector chart_changes; 
    std::tie(std::ignore, chart_changes) = happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT_EQUAL( 1u, (unsigned)chart_changes.size() );
    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath/SpotEvent").size() == 4 );
}

template< class ThePusher >
void TC_HappenLayer::AnyDelete()
{
    HappenLayer happen_layer;

    // исходные события нити
    {
        SpotEventPtr pat123[] = {
            EV( HCode::SPAN_MOVE, "1w", T+1 ),
            EV( HCode::SPAN_MOVE, "2w", T+2 ),
            EV( HCode::SPAN_MOVE, "3w", T+3 ),
        };
        TrioSpot trio;
        trio.body( pat123 );
        ThePusher pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        auto sz = pusher.AppliedEvents().size();
    }
    // удаление
    {
        TrioSpot trio;
        trio.pre( EV( HCode::SPAN_MOVE, "1w", T+1 ) );
        trio.post( EV( HCode::SPAN_MOVE, "3w", T+3 ) );
        trio.coverPre( true );
        trio.coverPost( true );
        ThePusher pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        auto sz = pusher.AppliedEvents().size();
        CPPUNIT_ASSERT( 0 == sz );
    }

    HappenLayer::SpotsVector chart_changes; 
    std::tie(std::ignore, chart_changes) = happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT_EQUAL( 0u, (unsigned)chart_changes.size() );
    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath/SpotEvent").empty() );
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath"          ).empty() );
}

void TC_HappenLayer::AutoTimeError()
{
    HappenLayer happen_layer;
    // Неверный порядок времени - трио откидывается польностью
    SpotEventPtr pat[] = {
        EV( HCode::SPAN_MOVE, "1w", T+1 ),
        EV( HCode::SPAN_MOVE, "2w", T+5 ),
        EV( HCode::SPAN_MOVE, "3w", T+2 ),
    };
    TrioSpot trio;
    trio.body( pat );
    Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
    pusher.Action();
    HappenLayer::SpotsVector chart_changes; 
    std::tie(std::ignore, chart_changes) = happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT_EQUAL( 0u, (unsigned)chart_changes.size() );
    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath").size() == 0 );
}

void TC_HappenLayer::UserTimeError()
{
    HappenLayer happen_layer;
    // Неверный порядок времени - трио откидывается полностью
    SpotEventPtr pat[] = {
        EV( HCode::SPAN_MOVE, "1w", T+1 ),
        EV( HCode::SPAN_MOVE, "2w", T+5 ),
        EV( HCode::SPAN_MOVE, "3w", T+2 ),
    };

    TrioSpot trio;
    trio.body( pat );
    Hem::aePusherUserHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory  );
    CPPUNIT_ASSERT_NO_THROW( pusher.Action() );
    CPPUNIT_ASSERT_EQUAL( size_t(0), pusher.AppliedEvents().size() );
    CPPUNIT_ASSERT_EQUAL( size_t(3), pusher.UnapplyableEvents().size() );
    HappenLayer::SpotsVector chart_changes; 
    std::tie(std::ignore, chart_changes) = happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT_EQUAL( 0u, (unsigned)chart_changes.size() );
    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath").size() == 0 );
}

void TC_HappenLayer::AutoEventCollision()
{
    HappenLayer happen_layer;

    // две пересекающихся нити
    SpotEventPtr one[] = {
        EV( HCode::SPAN_MOVE, "1w", T+1 ),
        EV( HCode::SPAN_MOVE, "2w", T+2 ),
    };
    SpotEventPtr two[] = {
        EV( HCode::SPAN_MOVE, "77w", T+1 ),
        EV( HCode::SPAN_MOVE, "2w",  T+2 ),
    };

    {
        TrioSpot trio;
        trio.body( one );
        Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
        pusher.Action();
        auto sz = pusher.AppliedEvents().size();
        CPPUNIT_ASSERT( size_array(one) == sz );
    }
    {
        TrioSpot trio;
        trio.body( two );
        Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory  );
        pusher.Action();
        CPPUNIT_ASSERT( pusher.AppliedEvents().size()==0 );
        CPPUNIT_ASSERT( pusher.UnapplyableEvents().size()==2 );
    }

    HappenLayer::SpotsVector chart_changes; 
    std::tie(std::ignore, chart_changes) = happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT_EQUAL( 1u, (unsigned)chart_changes.size() );
    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath/SpotEvent").size() == 2 );
}

void TC_HappenLayer::UserEventCollision()
{
    HappenLayer happen_layer;

    // две пересекающихся нити
    SpotEventPtr one[] = {
        EV( HCode::SPAN_MOVE, "1w", T+1 ),
        EV( HCode::SPAN_MOVE, "2w", T+2 ),
    };
    SpotEventPtr two[] = {
        EV( HCode::SPAN_MOVE, "77w", T+1 ),
        EV( HCode::SPAN_MOVE, "2w",  T+2 ),
        EV( HCode::SPAN_MOVE, "2w",  T+2 ),
    };

    {
        TrioSpot trio;
        trio.body( one );
        Hem::aePusherUserHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory  );
        pusher.Action();
        CPPUNIT_ASSERT_EQUAL( size_array(one), pusher.AppliedEvents().size() );
    }
    {
        TrioSpot trio;
        trio.body( two );
        Hem::aePusherUserHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory  );
        CPPUNIT_ASSERT_NO_THROW( pusher.Action() );
        CPPUNIT_ASSERT_EQUAL( size_t(0), pusher.AppliedEvents().size() );
        CPPUNIT_ASSERT_EQUAL( size_t(3), pusher.UnapplyableEvents().size() );
    }

    HappenLayer::SpotsVector chart_changes; 
    std::tie(std::ignore, chart_changes) = happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT_EQUAL( 1u, (unsigned)chart_changes.size() );
    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT( doc.select_nodes("//HemPath/SpotEvent").size() == 2 );
}

void TC_HappenLayer::UserBadPreposition()
{
    UtLayer<HappenLayer> happen_layer;

    {
        auto lastEvent = EV( HCode::SPAN_MOVE, "77w", T+1 );
        TrioSpot trio;
        trio.pre( lastEvent );
        trio.post( lastEvent );
        trio.coverPost(true);
        trio.body( EV( HCode::SPAN_MOVE, "2w", T+2 ) );
        Hem::aePusherUserHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory  );
        CPPUNIT_ASSERT_NO_THROW( pusher.Action() );
        CPPUNIT_ASSERT_EQUAL( size_t(1), pusher.AppliedEvents().size() );
        CPPUNIT_ASSERT_EQUAL( size_t(0), pusher.UnapplyableEvents().size() );
    }

    CPPUNIT_ASSERT( happen_layer.path_count()==1 );
    CPPUNIT_ASSERT( happen_layer.exist_path_size(1) );
}

// чистка конфликтов (см. #3723)
void TC_HappenLayer::ConflictCleaning()
{
    const EsrKit E(11111,22222);
    UtLayer<HappenLayer> hA, hU;
    // Одномоментное движение a-b-c-b-a
    SpotEventPtr pat[] = {
        EV( HCode::SPAN_MOVE, "X", T+0 ), // некая исходная точка
        EV( HCode::SPAN_MOVE,  "a",      T+5 ),
        EV( HCode::SPAN_MOVE,   "b",     T+5 ),
        EV( HCode::SPAN_MOVE,    "c",    T+5 ),
        EV( HCode::DEATH,         "d",   T+5 ),
        EV( HCode::FORM,          "d",   T+5 ),
        EV( HCode::DEATH,         "d",   T+5 ),
        EV( HCode::FORM,          "d",   T+5 ),
        EV( HCode::STATION_ENTRY,   "s", T+5, EsrKit(100) ),
        EV( HCode::STATION_EXIT,    "s", T+5, EsrKit(100) ),
        EV( HCode::STATION_ENTRY,   "s", T+5, EsrKit(100) ),
        EV( HCode::STATION_EXIT,    "s", T+5, EsrKit(100) ),
        EV( HCode::SPAN_MOVE,    "c",    T+5 ),
        EV( HCode::SPAN_MOVE,   "b",     T+5 ),
        EV( HCode::SPAN_MOVE, "a",       T+5 ),
    };
    // добавляем события по одному (так они поступают из Guess)
    for( unsigned i=0; i<size_array(pat); ++i )
    {
        // сигнал передать в автографик
        TrioSpot trio;
        trio.body( pat[i] );
        if ( i!=0 )
            trio.pre( pat[i-1] );
        Hem::aePusherAutoHappen pusher( hA.GetWriteAccess(), trio, identifyCategory  );
        pusher.Action();
        CPPUNIT_ASSERT( pusher.UnapplyableEvents().empty() );
        // изменения передать в пользовательский график
        bool have_changes;
        attic::a_document chang("test");
        std::tie(have_changes,std::ignore) = hA.TakeChanges( chang.document_element() );
        CPPUNIT_ASSERT( have_changes );
        // применить изменения к пользовательскому графику
        wstring wch = chang.pretty_wstr();
        ASSERT( !wch.empty() );
        for( auto trNode : chang.document_element().child(HappenLayer_xAttr).children() )
        {
            TrioSpot uTrio;
            trNode >> uTrio;
            Hem::aePusherUserHappen pushU( hU.GetWriteAccess(), uTrio, identifyCategory );
            pushU.Action();
            CPPUNIT_ASSERT( pushU.UnapplyableEvents().empty() );
            Hem::aeRectify rectifyEngine( hU.GetWriteAccess(), pat[0] );
            rectifyEngine.Action();
        }
        CPPUNIT_ASSERT( hA.path_count()==hU.path_count() );
        CPPUNIT_ASSERT( hA.exist_event( "@Bdg='" + pat[i]->GetBadge().to_string() + "'") );
        CPPUNIT_ASSERT( hU.exist_event( "@Bdg='" + pat[i]->GetBadge().to_string() + "'") );
    }
    CPPUNIT_ASSERT( hA.path_count()==1 );
    CPPUNIT_ASSERT( hA.exist_series("Bdg",
        "X[00100:00200] "
        " d[00100:00200] d[00100:00200] "
        " s[00100]       s[00100] "
        "  c[00100:00200] "
        " b[00100:00200] "
        "a[00100:00200] " ) );
    CPPUNIT_ASSERT( hU.path_count()==1 );
    CPPUNIT_ASSERT( hU.exist_series("Bdg", "X[00100:00200] s[00100] a[00100:00200] " ) );
}

template< class ThePusher >
void TC_HappenLayer::AnySerialize()
{
    UtLayer<HappenLayer> happen_layer;

    SpotDetailsPtr spot(new SpotDetails(ParkWayKit(1, 1), std::make_pair(PicketingInfo(), PicketingInfo())));

    // создаваемая нить
    SpotEventPtr pat[] = {
        EV( HCode::TRANSITION, "1w", T+1, EsrKit(100), spot ),
        EV( HCode::TRANSITION, "2w", T+2, EsrKit(100), spot ),
        EV( HCode::TRANSITION, "3w", T+3, EsrKit(100), spot ),
    };

    TrioSpot trio;
    trio.body( pat );
    ThePusher pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
    pusher.Action();
    auto sz = pusher.AppliedEvents().size();
    CPPUNIT_ASSERT_EQUAL( sz, trio.body().size() );

    AsoupLayer asoup_layer;
    AsoupEventPtr asoupPtr( new AsoupEvent( HCode::ARRIVAL, BadgeE(L"1w",EsrKit(100)), 
        ParkWayKit(1, 1), 
        T+1, TrainDescr(L"N1379S", L"1-2-3") ) );
    asoup_layer.AddAsoupEvent( asoupPtr );
    
    {
        EsrGuide esrGuide;
        std::shared_ptr<TopologyTest> topology;
		DistrictGuide dg;
        topology.reset( new TopologyTest );
        UtLayer<Regulatory::Layer> regular;
        const std::string nsiBaseStr = "<Test><Junctions /></Test>";
        UtNsiBasis utNsi( nsiBaseStr );
        Hem::GuessTransciever guessTransciever;
        std::shared_ptr<const NsiBasis> nsiBasisPtr = std::make_shared<const NsiBasis>(utNsi);
        UncontrolledStations uncontrolledStations;
        CPPUNIT_ASSERT( nsiBasisPtr );
        Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
		std::pair<Hem::AsoupToSpotResult, SpotEventPtr> res = std::make_pair(Hem::AsoupToSpotResult::Impossible, nullptr);

		{
        Hem::aeAsoupToSpotFind applier( happen_layer.GetWriteAccess(), asoup_layer.GetWriteAccess(), 
                                    regular, context, dg, nsiBasisPtr, asoupPtr, false, nullptr );
        CPPUNIT_ASSERT_NO_THROW(applier.Action());
		res = applier.getResult();
		}

		CPPUNIT_ASSERT( res.first==Hem::AsoupToSpotResult::Succeeded && res.second );
		auto path = happen_layer.GetWriteAccess()->getPath(res.second);
		CPPUNIT_ASSERT( path );
		{
		Hem::AsoupLinker linkerStrict(happen_layer.GetWriteAccess(), asoup_layer.GetWriteAccess(), context, Hem::StrictIdentifyPolicy(), nullptr);
		CPPUNIT_ASSERT(linkerStrict.identify(asoupPtr, std::make_pair(res.second, path)));
		}

        happen_layer.TakeChanges( attic::a_node() );
    }

    attic::a_document doc("test");
    happen_layer.Serialize(doc.document_element());
    CPPUNIT_ASSERT_EQUAL( size_array(pat), doc.select_nodes("//HemPath/SpotEvent").size() );
    CPPUNIT_ASSERT_EQUAL( 1u, (unsigned)doc.select_nodes("//HemPath/SpotEvent[1]/@num").size() );
    CPPUNIT_ASSERT_EQUAL( 1u, (unsigned)doc.select_nodes("//HemPath/SpotEvent[1]/@index").size() );
    CPPUNIT_ASSERT_EQUAL( 0u, (unsigned)doc.select_nodes("//HemPath/SpotEvent[2]/@num").size() );
    CPPUNIT_ASSERT_EQUAL( 0u, (unsigned)doc.select_nodes("//HemPath/SpotEvent[2]/@index").size() );
    CPPUNIT_ASSERT_EQUAL( 0u, (unsigned)doc.select_nodes("//HemPath/SpotEvent[3]/@num").size() );
    CPPUNIT_ASSERT_EQUAL( 0u, (unsigned)doc.select_nodes("//HemPath/SpotEvent[3]/@index").size() );
    CPPUNIT_ASSERT_EQUAL( 0u, (unsigned)doc.select_nodes("//HemPath/SpotEvent/TrainDescr").size() );
}

void TC_HappenLayer::AutoCreate() { AnyCreate<Hem::aePusherAutoHappen>(); }
void TC_HappenLayer::UserCreate() { AnyCreate<Hem::aePusherUserHappen>(); }

void TC_HappenLayer::AutoAppend() { AnyAppend<Hem::aePusherAutoHappen>(); }
void TC_HappenLayer::UserAppend() { AnyAppend<Hem::aePusherUserHappen>(); }

void TC_HappenLayer::AutoChange() { AnyChange<Hem::aePusherAutoHappen>(); }
void TC_HappenLayer::UserChange() { AnyChange<Hem::aePusherUserHappen>(); }

void TC_HappenLayer::AutoDelete() { AnyDelete<Hem::aePusherAutoHappen>(); }
void TC_HappenLayer::UserDelete() { AnyDelete<Hem::aePusherUserHappen>(); }

void TC_HappenLayer::AutoSerialization() { AnySerialize<Hem::aePusherAutoHappen>(); }
void TC_HappenLayer::UserSerialization() { AnySerialize<Hem::aePusherUserHappen>(); }