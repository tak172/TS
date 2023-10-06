#include "stdafx.h"
#include "TC_AugurRequester_InputRoutesComposer.h"
#include "../Hem/AugurRequester.h"
#include "../Hem/AugurRequester_InputRoutesComposer.h"
#include "../helpful/Time_Iso.h"
#include "../Hem/RouteIntrusionKeeper.h"
#include "../Hem/RouteIntrusion.h"
#include "../helpful/TrainsPriorityOffsets.h"
#include "../helpful/SpanTravelTime.h"
#include "../helpful//Junctions.h"
#include "../helpful/Topology.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_InputRoutesComposer );

void TC_InputRoutesComposer::HappenLive()
{
    // подготовка
    setLiveMoment("20181115T131000Z");
    happen_layer->createPath(
        L"<HemPath>"
        L"  <SpotEvent" + HMS(1246) + L"name='Form'  index='' num='6420' Bdg='1p[09730]' waynum='1' parknum='1' />"
        L"  <SpotEvent" + HMS(1246) + L"name='Departure' Bdg='1p[09730]' waynum='1' parknum='1' />"
        L"  <SpotEvent" + HMS(1300) + L"name='Arrival' Bdg='3p[09715]' waynum='3' parknum='1' />"
        L"  <SpotEvent" + HMS(1301) + L"name='Departure' Bdg='3p[09715]' waynum='3' parknum='1' />"
        L"  <SpotEvent" + HMS(1309) + L"name='Span_move' Bdg='KCPU2_D[09676:09715]' waynum='2' parknum='0' />"
        L"</HemPath>"
        );
    happen_layer->createPath(
        L"<HemPath>"
        L"  <SpotEvent" + HMS(1309) + L"name='Span_move' Bdg='starter_IS6[09511:09520]' waynum='2' index='' num='844' />"
        L"  <SpotEvent" + HMS(1314) + L"name='Arrival' Bdg='mock[09511]' waynum='4' parknum='1' />"
        L"</HemPath>"
        );
    pathForFutureMap.insert( getHappenPath(L"844"), true );
    CPPUNIT_ASSERT_EQUAL( size_t(2), happen_layer->path_count() );
    CPPUNIT_ASSERT( pathForFutureMap.sign( getHappenPath(L"844") ) );

    // исполнение
    ReqLines reqLines = processing();

    // проверка
    CPPUNIT_ASSERT_EQUAL( size_t(2), reqLines.size() );
    CPPUNIT_ASSERT( findItem( reqLines, TrainDescr( L"6420" ) ) );
    CPPUNIT_ASSERT( findItem( reqLines, TrainDescr( L"844" ) ) );
}

void TC_InputRoutesComposer::HappenDeath()
{
    // подготовка
    setLiveMoment("20181115T141000Z");
    happen_layer->createPath(
        L"<HemPath>"
        L"  <SpotEvent" + HMS(1324) + L"name='Departure' index='1131-038-0900' num='2333' Bdg='?p1w1[11760]' waynum='1' parknum='1' />"
        L"  <SpotEvent" + HMS(1349) + L"name='Arrival' Bdg='?p1w1[09360]' waynum='1' parknum='1' />"
        L"  <SpotEvent" + HMS(1349) + L"name='Death' Bdg='?p1w1[09360]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    happen_layer->createPath(
        L"<HemPath>"
        L"  <SpotEvent" + HMS(1314) + L"name='Departure' index='2345-959-1919' num='2221' Bdg='?p1w1[11760]' waynum='1' parknum='1' />"
        L"  <SpotEvent" + HMS(1329) + L"name='Arrival' Bdg='?p1w1[09360]' waynum='1' parknum='1' />"
        L"  <SpotEvent" + HMS(1329) + L"name='Death' Bdg='?p1w1[09360]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    pathForFutureMap.insert( getHappenPath(L"2221"), true );
    CPPUNIT_ASSERT_EQUAL( size_t(2), happen_layer->path_count() );
    CPPUNIT_ASSERT( pathForFutureMap.sign( getHappenPath( L"2221" ) ) );

    // исполнение
    ReqLines reqLines = processing();

    // проверка
    CPPUNIT_ASSERT_EQUAL( size_t(1), reqLines.size() );
    CPPUNIT_ASSERT( findItem( reqLines, TrainDescr( L"2221", L"2345-959-1919" ) ) );
}

void TC_InputRoutesComposer::FutureSimple()
{
    // подготовка
    setLiveMoment("20181208T100000Z");
    future_layer->createPath( // fixed events + destination
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1010)+L"name='Departure' Bdg='zz[11420]' index='0900-777-1100' num='FD2777' waynum='3' parknum='1' >"
        L"    <feat_texts finaldest='11443' />"
        L"  </SpotEvent>"
        L"  <SpotEvent"+HMS(1020)+L"name='Arrival' Bdg='mock[11430]' waynum='5' parknum='1' sourcetype='fix' />"
        L"  <SpotEvent"+HMS(1030)+L"name='Departure' Bdg='mock[11430]' waynum='5' parknum='1' sourcetype='fix' />"
        L"  <SpotEvent"+HMS(1040)+L"name='Arrival' Bdg='mock[11443]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath( // movable events + destination
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1011)+L"name='Departure' Bdg='zz[11420]' index='0900-666-1100' num='D2661' waynum='3' parknum='1' >"
        L"    <feat_texts finaldest='11443' />"
        L"  </SpotEvent>"
        L"  <SpotEvent"+HMS(1021)+L"name='Arrival' Bdg='mock[11430]' waynum='5' parknum='1' />"
        L"  <SpotEvent"+HMS(1031)+L"name='Departure' Bdg='mock[11430]' waynum='5' parknum='1' />"
        L"  <SpotEvent"+HMS(1041)+L"name='Arrival' Bdg='mock[11443]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath( // advent event + destination
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1012)+L"name='Departure' Bdg='zz[11420]' index='0900-555-1100' num='A2555' sourcetype='advent' waynum='3' parknum='1' >"
        L"    <feat_texts finaldest='11443' />"
        L"  </SpotEvent>"
        L"  <SpotEvent"+HMS(1022)+L"name='Arrival' Bdg='mock[11430]' waynum='5' parknum='1' />"
        L"  <SpotEvent"+HMS(1032)+L"name='Departure' Bdg='mock[11430]' waynum='5' parknum='1' />"
        L"  <SpotEvent"+HMS(1042)+L"name='Arrival' Bdg='mock[11443]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT_EQUAL( size_t(3), future_layer->path_count() );

    // исполнение
    ReqLines reqLines = processing();

    // проверка
    CPPUNIT_ASSERT_EQUAL( size_t(1), reqLines.size() );
    CPPUNIT_ASSERT( findItem( reqLines, TrainDescr( L"A2555", L"0900-555-1100" ) ) );
}

void TC_InputRoutesComposer::FutureWithHappen()
{
    // подготовка
    setLiveMoment("20181208T140000Z");
    happen_layer->createPath( // happen + destination 11555
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1010)+L"name='Departure' Bdg='zz[11301]' index='0900-777-1100' num='FD2777' waynum='3' parknum='1' >"
        L"    <feat_texts finaldest='11555' />"
        L"  </SpotEvent>"
        L"  <SpotEvent"+HMS(1020)+L"name='Transition' Bdg='mock[11302]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath( // fixed events + destination 11000
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1210)+L"name='Departure' Bdg='zz[11420]' index='0900-777-1100' num='FD2777' waynum='3' parknum='1' >"
        L"    <feat_texts finaldest='11000' />"
        L"  </SpotEvent>"
        L"  <SpotEvent"+HMS(1220)+L"name='Transition' Bdg='mock[11430]' waynum='5' parknum='1' sourcetype='fix' />"
        L"  <SpotEvent"+HMS(1240)+L"name='Arrival' Bdg='mock[11443]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    attachFutu2Happen( L"FD2777" );

    happen_layer->createPath( // happen + destination 12221
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1014)+L"name='Departure' Bdg='zz[11301]' index='0900-444-1100' num='D2441' waynum='3' parknum='1' >"
        L"    <feat_texts finaldest='12221' />"
        L"  </SpotEvent>"
        L"  <SpotEvent"+HMS(1024)+L"name='Transition' Bdg='mock[11302]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath( // movable + destination 11000
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1214)+L"name='Departure' Bdg='zz[11420]' index='0900-444-1100' num='D2441' waynum='3' parknum='1' >"
        L"    <feat_texts finaldest='11000' />"
        L"  </SpotEvent>"
        L"  <SpotEvent"+HMS(1224)+L"name='Transition' Bdg='mock[11430]' waynum='5' parknum='1' />"
        L"  <SpotEvent"+HMS(1244)+L"name='Arrival' Bdg='mock[11443]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    attachFutu2Happen( L"D2441" );

    happen_layer->createPath( // happen
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1013)+L"name='Departure' Bdg='zz[11301]' index='0900-555-1100' num='SD6211' waynum='3' parknum='1' />"
        L"  <SpotEvent"+HMS(1023)+L"name='Transition' Bdg='mock[11302]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath( // fixed departure event
        L"<HemPath>"
        L"  <SpotEvent"+HMS(1113)+L"name='Departure'  Bdg='zz[11420]' index='0900-555-1100' num='SD6211' waynum='3' parknum='1' />"
        L"  <SpotEvent"+HMS(1123)+L"name='Transition' Bdg='mock[11430]' waynum='5' parknum='1' />"
        L"  <SpotEvent"+HMS(1133)+L"name='Arrival'    Bdg='mock[11443]' waynum='3' parknum='1' />"
        L"  <SpotEvent"+HMS(1143)+L"name='Departure'  Bdg='mock[11450]' waynum='2' parknum='2' sourcetype='fix' />"
        L"  <SpotEvent"+HMS(1153)+L"name='Arrival'    Bdg='mock[11451]' waynum='2' parknum='2' />"
        L"</HemPath>"
        );
    attachFutu2Happen( L"SD6211" );

    CPPUNIT_ASSERT_EQUAL( size_t(3), happen_layer->path_count() );
    CPPUNIT_ASSERT_EQUAL( size_t(3), future_layer->path_count() );

    // исполнение
    ReqLines reqLines = processing();

    // проверка
    CPPUNIT_ASSERT_EQUAL( size_t(3), reqLines.size() );
    CPPUNIT_ASSERT( findItem( reqLines, TrainDescr( L"FD2777", L"0900-777-1100" ) ) );
    auto item = findItem( reqLines, TrainDescr(  L"D2441", L"0900-444-1100" ) );
    CPPUNIT_ASSERT( item );
    auto FIN = TrainCharacteristics::TrainFeature::FinalDestination;
    CPPUNIT_ASSERT( L"12221" == item->first.GetFeatureText( FIN ) );

    auto itemSD = findItem( reqLines, TrainDescr(  L"SD6211", L"0900-555-1100" ) );
    CPPUNIT_ASSERT( itemSD );
    auto& evSD = itemSD->second;
    CPPUNIT_ASSERT( evSD.size()==4 );

    auto depFix = []( const FixableEvent& ev ){
        return ev.GetCode() == HCode::DEPARTURE &&
               ev.GetBadge().num() == EsrKit(11450) &&
               ev.IsFixated();
    };
    CPPUNIT_ASSERT( std::any_of( evSD.begin(), evSD.end(), depFix ) );

    auto movFix = []( const FixableEvent& ev ){
        return ev.GetCode() == HCode::SPAN_MOVE &&
            ev.GetBadge().num() == EsrKit(11450,11451) &&
            ev.IsFixated();
    };
    CPPUNIT_ASSERT( std::any_of( evSD.begin(), evSD.end(), movFix ) );
}

void TC_InputRoutesComposer::setUp()
{
    happen_layer = std::make_shared< UtLayer<HappenLayer> >();
    future_layer = std::make_shared< UtLayer<FutureLayer> >();
    pathForFutureMap.clear();
    liveMoment = 0;
}

void TC_InputRoutesComposer::tearDown()
{
}

// установка текущего времени - эта же дата используется для построения данных
void TC_InputRoutesComposer::setLiveMoment( std::string _currentMoment )
{
    liveMoment = time_from_iso(_currentMoment);
    CPPUNIT_ASSERT( liveMoment );
}

// генератор времени события в виде атрибута XML
std::wstring TC_InputRoutesComposer::HMS( unsigned val ) const
{
    CPPUNIT_ASSERT( liveMoment );

    std::string hms = std::to_string(val);
    if ( hms.size()==4 )
        hms += "00";
    CPPUNIT_ASSERT( hms.size() == 4 || hms.size() == 6 ); // час_минута или час_минута_секунда

    return FromUtf8(" create_time='" + time_to_iso(liveMoment).substr(0,9) + hms + "Z' ");
}

// получить Happen-нитку по номеру поезда
HappenLayer::PathPtr TC_InputRoutesComposer::getHappenPath( std::wstring _numTrain ) const
{
    TrainNumber tn(_numTrain);

    auto happenRA = happen_layer->base().GetReadAccess();
    auto allP = happenRA->getAllPathes();
    for( auto path : allP )
    {
        if ( auto td = path->GetLastFragment() )
        {
            if ( tn == td->GetNumber() )
                return path;
        }
    }
    return HappenLayer::PathPtr();
}

// получить Future-нитку по номеру поезда
std::shared_ptr<const Hem::FixablePath> TC_InputRoutesComposer::getFuturePath( std::wstring _numTrain ) const
{
    TrainNumber tn(_numTrain);

    auto futureRA = future_layer->base().GetReadAccess();
    auto allP = futureRA->getAllPathes();
    for( std::shared_ptr<const Hem::FixablePath> path : allP )
    {
        if ( auto td = path->GetFirstFragment() )
        {
            if ( tn == td->GetNumber() )
                return path;
        }
    }
    return std::shared_ptr<Hem::FixablePath>();
}

void TC_InputRoutesComposer::attachFutu2Happen( std::wstring trainNum )
{
    std::shared_ptr<const Hem::FixablePath> futuPath = getFuturePath( trainNum );
    HappenLayer::PathPtr happPath = getHappenPath( trainNum );
    auto futFirstEvent = futuPath->GetFirstEvent();
    auto happEvents = happPath->GetAllEvents();
    std::vector<SpotEvent> v;
    for( auto hh : happEvents )
        v.push_back( *hh );
    future_layer->Bond().AttachEvents( *futFirstEvent, v );
}

// построение результата для проверки
std::vector<InputRoutesComposer::EventLine> TC_InputRoutesComposer::processing()
{
    RouteIntrusionKeeper rintrusionKeeper;
    TrainsPriorityOffsets trPriorityOffsets;
    SpanTravelTime userSpanTravelTime;
    DistrictGuide dg;
    ApproachMap approachMap;
    AugurRequester::AugurReqContext context(pathForFutureMap, rintrusionKeeper, trPriorityOffsets, userSpanTravelTime, uncontrolledStations, dg, nullptr, nullptr);
    InputRoutesComposer inputRoutesComposer( context, approachMap );
    return inputRoutesComposer.build(&*happen_layer, &*future_layer, liveMoment);
}


const TC_InputRoutesComposer::ReqItem* TC_InputRoutesComposer::findItem( const ReqLines& reqLines, const TrainDescr& testDescr )
{
    auto it = std::find_if( reqLines.begin(), reqLines.end(),
        [ &testDescr ]( const InputRoutesComposer::EventLine& eventLine ){
            auto& lineD = eventLine.first;
            return ( testDescr.GetNumber() == lineD.GetNumber() && testDescr.GetIndex()  == lineD.GetIndex() );
    } );
    const ReqItem* ri = nullptr;
    if ( reqLines.end() != it )
        ri = &*it;
    return ri;
}
