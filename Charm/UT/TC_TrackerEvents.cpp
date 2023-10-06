#include "stdafx.h"
#include "TC_TrackerEvents.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerEvents );
void TC_TrackerEvents::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.cargoTransArrivDurMs = 2000;
	timConstr.woADWaysTransMs = 3000;
}

void TC_TrackerEvents::SpanToSpanByOrdInc()
{
	//имитируем проезд одной ПЕ с от одного до другого перегона

	//возникновение ПЕ
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 2 );
	auto teIt = trackerEvents.cbegin();
    CPPUNIT_ASSERT( chk_eq( *teIt, HCode::FORM, L"W" ) );
	teIt++;
    CPPUNIT_ASSERT( chk_eq( *teIt, HCode::SPAN_MOVE, L"Q" ) );

	//въезд на станцию
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
    CPPUNIT_ASSERT( chk_eq( trackerEvents.back(), HCode::STATION_ENTRY, L"A" ) );

	//появление на приемоотправочном пути
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	TI.DetermineStripBusy( L"C", *tracker );
    TI.DetermineStripFree( L"B", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
    CPPUNIT_ASSERT( chk_eq( trackerEvents.back(), HCode::ARRIVAL, L"C" ) );

	//покидание приемоотправочного пути
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.IncreaseTime( tracker->GetTimeConstraints().cargoTransArrivDurMs + 1000 ); //задержка для блокирования проследования
	TI.DetermineStripBusy( L"F", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
    CPPUNIT_ASSERT( chk_eq( trackerEvents.back(), HCode::DEPARTURE, L"C" ) );

	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );

	//выезд со станции
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	TI.DetermineStripBusy( L"X", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	vector <TrackerEventPtr> evVector( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( evVector.size() == 7 );
    CPPUNIT_ASSERT( chk_eq( evVector[ 5 ], HCode::STATION_EXIT, L"G" ) );
    CPPUNIT_ASSERT( chk_eq( evVector[ 6 ], HCode::SPAN_MOVE, L"X" ) );

	TI.DetermineStripFree( L"G", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 7 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::TwoTrainsByInc()
{
	//имитируем простое возникновение двух ПЕ

	//возникновение первой ПЕ
	TI.DetermineStripBusy( L"C", *tracker );

	//возникновение второй ПЕ
	TI.DetermineStripBusy( L"D", *tracker );

	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	SetRandomOddInfo( tlist.front() );
	SetRandomOddInfo( tlist.back() );
	const auto & trackerEvents = tracker->GetTrainEvents();

	CPPUNIT_ASSERT( trackerEvents.size() == 2 );
	auto teIt1 = trackerEvents.cbegin();
	auto teIt2 = trackerEvents.crbegin();
	CPPUNIT_ASSERT( (*teIt1)->GetCode() == HCode::FORM && (*teIt2)->GetCode() == HCode::FORM );
	if ( (*teIt1)->GetBadge() == TI.Get(L"C").bdg )
		CPPUNIT_ASSERT( (*teIt2)->GetBadge() == TI.Get(L"D").bdg );
	else
	{
		CPPUNIT_ASSERT( (*teIt1)->GetBadge() == TI.Get(L"D").bdg );
		CPPUNIT_ASSERT( (*teIt2)->GetBadge() == TI.Get(L"C").bdg );
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::OneTrainByUnordInc()
{
	//имитируем возникновение одной ПЕ с помощью перемешанной последовательности инкрементных посылок,
	//но имеющих одинаковое время
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"D", *tracker, false );
	TI.DetermineSwitchPlus( L"11", *tracker, false );
	TI.FlushData( *tracker, false );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	SetRandomOddInfo( tlist.front() );

	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 1 );
    CPPUNIT_ASSERT( chk_eq( trackerEvents.back(), HCode::FORM, L"D" ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::OccupationOwnStrip()
{
	//пусть ПЕ занимает 5 участков
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );

	const auto & trackerEvents = tracker->GetTrainEvents();

	//имитация событий занятости в середине ПЕ
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	const auto & trackerEvents2 = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == trackerEvents2.size() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::StationToStation()
{
	//переход со станции на станцию
	TI.DetermineStripBusy( L"ST1", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	SetRandomOddInfo( tlist.front() );
	TI.DetermineStripFree( L"ST1", *tracker, false );
	TI.DetermineStripBusy( L"ST2", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"ST2", *tracker, false );
	TI.DetermineStripBusy( L"ST3", *tracker, false );
	TI.FlushData( *tracker, false );
	const auto & trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> events( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( events.size() == 6 );
    CPPUNIT_ASSERT( chk_eq( events[ 0 ], HCode::FORM, L"ST1" ) );
    CPPUNIT_ASSERT( chk_eq( events[ 1 ], HCode::STATION_EXIT, L"ST1" ) );
    CPPUNIT_ASSERT( chk_eq( events[ 2 ], HCode::STATION_ENTRY, L"ST2" ) );
    CPPUNIT_ASSERT( chk_eq( events[ 3 ], HCode::TRANSITION, L"ST2" ) );
    CPPUNIT_ASSERT( chk_eq( events[ 4 ], HCode::STATION_EXIT, L"ST2" ) );
    CPPUNIT_ASSERT( chk_eq( events[ 5 ], HCode::STATION_ENTRY, L"ST3" ) );
    CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::PocketMove()
{
	TI.DetermineStripBusy( L"PK3", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.front();
	SetRandomOddInfo( train );
	TI.DetermineStripFree( L"PK3", *tracker );
	TI.DetermineStripBusy( L"PK7", *tracker );

	//проверка событий
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::POCKET_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::POCKET_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::JointCoordinates()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
    TI.DetermineStripFree( L"W", *tracker );
    TI.DetermineStripFree( L"Q", *tracker );
    TI.DetermineStripFree( L"A", *tracker );
    TI.DetermineStripFree( L"B", *tracker );

	rwCoord r1( 51, 150 ), r2( 56, 150 ), r3( 57, 700 );
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();

	//путь W (FORM)
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt )->GetBadge() == TI.Get( L"W" ).bdg );
	auto evcoords = ( *teIt )->Coords();
	CPPUNIT_ASSERT( !evcoords.first.empty() && !evcoords.second.empty() );
	auto wcoord1 = evcoords.first.GetCoord( TI.axis_name );
	auto wcoord2 = evcoords.second.GetCoord( TI.axis_name );
	CPPUNIT_ASSERT( wcoord1 != wcoord2 && ( wcoord1 == r1 || wcoord2 == r1 ) && ( wcoord1 == r2 || wcoord2 == r2 ) );

	//путь Q (SPAN_MOVE)
	++teIt;
    CPPUNIT_ASSERT( chk_eq( *teIt, HCode::SPAN_MOVE, L"Q" ) );
    evcoords = ( *teIt )->Coords();
	CPPUNIT_ASSERT( !evcoords.first.empty() && !evcoords.second.empty() );
	auto qcoord1 = evcoords.first.GetCoord( TI.axis_name );
	auto qcoord2 = evcoords.second.GetCoord( TI.axis_name );
	CPPUNIT_ASSERT( qcoord1 != qcoord2 && ( qcoord1 == r2 || qcoord2 == r2 ) && ( qcoord1 == r3 || qcoord2 == r3 ) );

	//вход на станцию (STATION_ENTRY). путь A
	++teIt;
    CPPUNIT_ASSERT( chk_eq( *teIt, HCode::STATION_ENTRY, L"A" ) );

	//путь C (ARRIVAL)
	++teIt;
    CPPUNIT_ASSERT( chk_eq( *teIt, HCode::ARRIVAL, L"C" ) );
    evcoords = ( *teIt )->Coords();
	auto ccoord1 = evcoords.first.GetCoord( TI.axis_name );
	auto ccoord2 = evcoords.second.GetCoord( TI.axis_name );
	CPPUNIT_ASSERT( ccoord1 == rwCoord( 57, 850 ) && ccoord2 == rwCoord( 57, 950 ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::ParkWayNumbers()
{
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
    TI.DetermineStripBusy( L"G", *tracker );
    TI.DetermineStripBusy( L"Q", *tracker );
    TI.DetermineStripBusy( L"X3", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 5 );
	for ( auto train : tlist )
		SetRandomOddInfo( train );
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == tlist.size() );
	for ( TrackerEventPtr tevent : trackerEvents )
	{
		if ( tevent->GetBadge() == TI.Get( L"C" ).bdg )
		{
			CPPUNIT_ASSERT( tevent->ParkWay() == ParkWayKit( 3, 1 ) );
		}
		else if ( tevent->GetBadge() == TI.Get( L"D" ).bdg )
		{
			CPPUNIT_ASSERT( tevent->ParkWay() == ParkWayKit( 2 ) );
		}
		else if ( tevent->GetBadge() == TI.Get( L"G" ).bdg )
		{
			CPPUNIT_ASSERT( tevent->ParkWay().empty() );
		}
		else if ( tevent->GetBadge() == TI.Get( L"Q" ).bdg )
		{
			CPPUNIT_ASSERT( tevent->ParkWay() == ParkWayKit( 2 ) );
		}
		else if ( tevent->GetBadge() == TI.Get( L"X3" ).bdg )
		{
			CPPUNIT_ASSERT( tevent->ParkWay().iWay() == 1 );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::NoTrainDeath()
{
	//появление "не поезда"
	TI.DetermineStripBusy( L"C", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.back();
	CPPUNIT_ASSERT( !train->IsTrain() );
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.empty() );

	//исчезновение "не поезда"
	TI.DetermineStripFree( L"C", *tracker );
	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.empty() );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::BanCoordinatesAtStaion()
{
    TI.DetermineSwitchPlus( L"11", *tracker );
    TI.DetermineStripBusy( L"W", *tracker );
    TI.DetermineStripBusy( L"Q", *tracker );
    TI.DetermineStripBusy( L"A", *tracker );
    TI.DetermineStripBusy( L"D", *tracker );
    TI.DetermineStripFree( L"W", *tracker );
    TI.DetermineStripFree( L"Q", *tracker );
    TI.DetermineStripFree( L"A", *tracker );
    const auto & trackerEvents = tracker->GetTrainEvents();
	auto arrIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
        return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
    CPPUNIT_ASSERT( arrIt != trackerEvents.cend() );
    auto picketage = (*arrIt)->Coords();
    CPPUNIT_ASSERT( picketage.first.empty() && picketage.second.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::YieldNumber()
{
	TI.DetermineStripBusy( L"H", *tracker ); //поезд 1 (H)
	TI.DetermineStripBusy( L"C", *tracker ); //поезд 2 (C)
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	auto trainC = ( trains.front()->GetPlace().size() == 1 ) ? trains.front() : trains.back();
	auto trainCId = trainC->GetId();
	auto trainH = ( trains.front() == trainC ) ? trains.back() : trains.front();
	auto trainHId = trainH->GetId();
	SetRandomOddInfo( trainC );
	auto activeTDescr = trainC->GetDescrPtr();
	CPPUNIT_ASSERT( activeTDescr );
	list <TrackerEventPtr> tevents;
	tracker->TakeEvents( tevents );
	SetInfo( trainH, *activeTDescr );
	tracker->TakeEvents( tevents );
	for ( auto tevent : tevents )
	{
		attic::a_document xdoc( "tstNode" );
		auto xnode = xdoc.document_element();
		const HemEvent & hevent = *tevent;
		xnode << hevent;
		TrainDescr tstdescr( xnode );
		if ( tevent->GetId() == trainCId )
			CPPUNIT_ASSERT( tstdescr.empty() );
		else
		{
			CPPUNIT_ASSERT( tevent->GetId() == trainHId );
			CPPUNIT_ASSERT( tstdescr == *activeTDescr );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::RepeatInfoChanging()
{
    TI.DetermineStripBusy( L"C", *tracker );
    CPPUNIT_ASSERT( tracker->CheckIntegrity() );
    ///
    CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
    SetRandomEvenInfo( trainCont->GetUnities().front() );
    auto train5 = trainCont->GetUnities().front();
    auto activeTDescr = train5->GetDescrPtr();
    CPPUNIT_ASSERT( activeTDescr );
    list <TrackerEventPtr> tevents;
    tracker->TakeEvents( tevents );

    // мелкие изменения дескриптора без изменения номера и индекса
	TI.IncreaseTime( 1000 );
    TrainDescr updTDescr = *activeTDescr;
    updTDescr.SetRelLength( updTDescr.GetRelLength()+1 );
    SetInfo( train5, updTDescr );

    tracker->TakeEvents( tevents );
    CPPUNIT_ASSERT( tevents.size()==1 );
    auto& tevent = *tevents.front();
    CPPUNIT_ASSERT( tevent.GetCode() == HCode::INFO_CHANGING );
    CPPUNIT_ASSERT( tevent.GetId() == train5->GetId() );
    CPPUNIT_ASSERT( tevent.GetBadge().str() == L"C" );
    CPPUNIT_ASSERT( tevent.GetDetails() && tevent.GetDetails()->tdptr );
    CPPUNIT_ASSERT( *tevent.GetDetails()->tdptr == updTDescr );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
    CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::LightFadeArrdep()
{
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	auto tevents = tracker->GetTerraEvents();
	CPPUNIT_ASSERT( tevents.size() == 2 );
	auto tevIt = tevents.cbegin();
    CPPUNIT_ASSERT( chk_eq( *tevIt, HCode::WAY_RED, L"C" ) );
    ++tevIt;
    CPPUNIT_ASSERT( chk_eq( *tevIt, HCode::WAY_BLACK, L"C" ) );
    CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::EntryInfoChangeExit()
{
	TI.DetermineSwitchPlus( L"31", *tracker );
	TI.DetermineSwitchPlus( L"32", *tracker );

	//въезд на станцию
	wstring stripsArr[] = { L"Y3", L"X3", L"F3", L"D3", L"C3" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	const auto & timConstr = tracker->GetTimeConstraints();

	//длительное ожидание
	TI.IncreaseTime( timConstr.woADWaysTransMs );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomOddInfo( trains.front() );
	TI.IncreaseTime( timConstr.woADWaysTransMs );

	//выезд со станции
	wstring stripsArr2[] = { L"C3", L"A3", L"G3" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr2 ), end( stripsArr2 ) ), *tracker );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( tevents.size() == 6 );
	auto teIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::INFO_CHANGING );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::UTurnAfterStationPartialEntryVar1()
{
	//частичный въезд поезда на станцию
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );

	//сразу выезд обратно на перегон
	const auto & timconstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( timconstr.woADWaysTransMs + 1000 );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( tevents.size() == 7 );
	vector<TrackerEventPtr> evVec( tevents.cbegin(), tevents.cend() );
    CPPUNIT_ASSERT( chk_eq( evVec[ 0 ], HCode::FORM, L"W" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 1 ], HCode::SPAN_MOVE, L"Q" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 2 ], HCode::STATION_ENTRY, L"A" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 3 ], HCode::STATION_EXIT, L"A" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 4 ], HCode::SPAN_MOVE, L"Q" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 5 ], HCode::SPAN_MOVE, L"W" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 6 ], HCode::SPAN_MOVE, L"H" ) );
    CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::UTurnAfterStationPartialEntryVar2()
{
	//частичный въезд поезда на станцию
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );

	//сразу выезд обратно на перегон
	const auto & timconstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( timconstr.woADWaysTransMs + 1000 );
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( tevents.size() == 6 );
	vector<TrackerEventPtr> evVec( tevents.cbegin(), tevents.cend() );
    CPPUNIT_ASSERT( chk_eq( evVec[ 0 ], HCode::FORM, L"W" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 1 ], HCode::SPAN_MOVE, L"Q" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 2 ], HCode::STATION_ENTRY, L"A" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 3 ], HCode::STATION_EXIT, L"A" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 4 ], HCode::SPAN_MOVE, L"Q" ) );
    CPPUNIT_ASSERT( chk_eq( evVec[ 5 ], HCode::SPAN_MOVE, L"W" ) );
    CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::HeadPlaceInDeath()
{
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"H", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() > 0 );
	auto deathEvent = tevents.back();
    CPPUNIT_ASSERT( chk_eq( deathEvent, HCode::DEATH, L"A" ) );
    CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::FlashLeaving()
{
	TI.DetermineStripBusy( L"C", *tracker );
	const auto & trainCont = tracker->GetTrainContainer();
	auto tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	SetRandomEvenInfo( train );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );

	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.DetermineStripFree( L"C", *tracker, false );
	TI.DetermineStripFree( L"F", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont.GetUnities();
	auto tevents = tracker->GetTrainEvents();
	auto tevIt = tevents.cbegin();
	CPPUNIT_ASSERT( tevents.size() == 2 );
    CPPUNIT_ASSERT( chk_eq( *tevIt, HCode::FORM, L"C" ) );
    ++tevIt;
    CPPUNIT_ASSERT( chk_eq( *tevIt, HCode::DEPARTURE, L"C" ) );
    CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::PicketageOnForm()
{
	TI.DetermineStripBusy( L"M2", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	SetRandomEvenInfo( train );

	auto tevents = tracker->GetTrainEvents();
	auto tevIt = tevents.cbegin();
	CPPUNIT_ASSERT( tevents.size() == 1 );
	const auto & coords = ( *tevIt )->Coords();
	const PicketingInfo & pickInfo1 = coords.first;
	const PicketingInfo & pickInfo2 = coords.second;
	rwRuledCoord pksingle1, pksingle2;
	pickInfo1.GetFirstCoord( pksingle1 );
	pickInfo2.GetFirstCoord( pksingle2 );
	rwCoord rcoord1 = pksingle1.coord();
	rwCoord rcoord2 = pksingle2.coord();
	CPPUNIT_ASSERT( rcoord1 == rwCoord( 74, 850 ) && rcoord2 == rwCoord( 74, 400 ) );
    CPPUNIT_ASSERT( chk_eq( *tevIt, HCode::FORM, L"M2" ) );
    CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::FaultStickingTrainKnowledge()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"15", *tracker );
	TI.DetermineRouteSet( L"Head106->P", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"268" ) ), list<wstring>( 1, L"Q" ) );

	//въезд и прибытие поезда
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	auto tevents = tracker->GetTrainEvents();

	//отправление поезда и подъезд к перегону
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker );

	tevents = tracker->GetTrainEvents();

	//исчезновение на станции
	TI.DetermineStripFree( L"G", *tracker );

	//восстановление на перегоне
	TI.DetermineStripBusy( L"X", *tracker );

	//въезд на следующую станцию и проследование через ПО-путь
	array<wstring, 5> pathArr = { L"X", L"I", L"J", L"L", L"M" };
	TI.ImitateMotion( vector<wstring>( begin( pathArr ), end( pathArr ) ), *tracker );
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"O", *tracker );
	tevents = tracker->GetTrainEvents();

	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 10 );
    CPPUNIT_ASSERT( chk_eq( evvec[ 0 ], HCode::FORM, L"Q" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 1 ], HCode::STATION_ENTRY, L"A" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 2 ], HCode::ARRIVAL, L"C" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 3 ], HCode::DEPARTURE, L"C" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 4 ], HCode::DEATH, L"G" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 5 ], HCode::FORM, L"X" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 6 ], HCode::SPAN_MOVE, L"X" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 7 ], HCode::SPAN_MOVE, L"I" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 8 ], HCode::STATION_ENTRY, L"J" ) );
    CPPUNIT_ASSERT( chk_eq( evvec[ 9 ], HCode::TRANSITION, L"M" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::Serialize()
{
	auto pickRange = make_pair<PicketingInfo, PicketingInfo>( PicketingInfo(), PicketingInfo() );
	SpotDetailsPtr spotDetPtr( new SpotDetails( ParkWayKit( 3, 31 ), pickRange, shared_ptr<TrainOperations>(), shared_ptr<AcceptanceInfo>(), 298, EsrKit( 54125 ) ) );
	TrackerEvent tevent( 27, HCode::FORM, BadgeE( L"bornPlace", EsrKit( 2, 8 ) ), 883, 
		TrackerEvent::OptionalParams( spotDetPtr, TrainCharacteristics::Source::Disposition, 773 ) );
	attic::a_document xdoc( "root" );
	auto rootNode = xdoc.document_element();
	rootNode << static_cast<const HemEvent &>( tevent );
	auto idAttr = rootNode.attribute( "id" );
	CPPUNIT_ASSERT( idAttr && idAttr.as_string() == string( "27" ) );
	auto nameAttr = rootNode.attribute( "name" );
	CPPUNIT_ASSERT( nameAttr && nameAttr.as_string() == string( "Form" ) );
	auto bdgAttr = rootNode.attribute( "Bdg" );
	CPPUNIT_ASSERT( bdgAttr && bdgAttr.as_string() == string( "bornPlace[00002:00008]" ) );
	auto timeAttr = rootNode.attribute( "create_time" );
	CPPUNIT_ASSERT( timeAttr && timeAttr.as_string() == string( "19700101T001443Z" ) );
	auto parkAttr = rootNode.attribute( "parknum" );
	CPPUNIT_ASSERT( parkAttr && parkAttr.as_string() == string( "3" ) );
	auto wayAttr = rootNode.attribute( "waynum" );
	CPPUNIT_ASSERT( wayAttr && wayAttr.as_string() == string( "31" ) );
	auto intervalAttr = rootNode.attribute( "intervalSec" );
	CPPUNIT_ASSERT( intervalAttr && intervalAttr.as_string() == string( "298" ) );
	auto optECodeAttr = rootNode.attribute( "optCode" );
	CPPUNIT_ASSERT( optECodeAttr && optECodeAttr.as_string() == string( "54125" ) );
	auto srcAttr = rootNode.attribute( "infsrc" );
	CPPUNIT_ASSERT( srcAttr && srcAttr.as_string() == string( "disposition" ) );
	auto prevIdAttr = rootNode.attribute( "prev_id" );
	CPPUNIT_ASSERT( prevIdAttr && prevIdAttr.as_string() == string( "773" ) );

	TrackerEvent tevent2( 27, HCode::FORM, BadgeE( L"bornPlace", EsrKit( 2, 8 ) ), 883 );
	rootNode << static_cast<const HemEvent &>( tevent2 );
	CPPUNIT_ASSERT( !rootNode.attribute( "prev_id" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::Deserialize()
{
	attic::a_document xdoc( "root" );
	auto rootNode = xdoc.document_element();
	rootNode.append_attribute( "id" ).set_value( "2547" );
	rootNode.append_attribute( "name" ).set_value( "Departure" );
	rootNode.append_attribute( "Bdg" ).set_value( "depPlace[24551]" );
	rootNode.append_attribute( "create_time" ).set_value( "19700101T001440Z" );
	rootNode.append_attribute( "parknum" ).set_value( "4" );
	rootNode.append_attribute( "waynum" ).set_value( "15" );
	rootNode.append_attribute( "intervalSec" ).set_value( "88" );
	rootNode.append_attribute( "optCode" ).set_value( "12449" );
	rootNode.append_attribute( "infsrc" ).set_value( "disposition" );
	rootNode.append_attribute( "prev_id" ).set_value( "601" );
	
	TrackerEvent tevent( rootNode );
	CPPUNIT_ASSERT( tevent.GetId() == 2547 );
	CPPUNIT_ASSERT( tevent.GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( tevent.GetBadge() == BadgeE( L"depPlace", EsrKit( 24551 ) ) );
	CPPUNIT_ASSERT( tevent.GetTime() == 880 );
	auto spotDetPtr = tevent.GetDetails();
	CPPUNIT_ASSERT( spotDetPtr );
	CPPUNIT_ASSERT( spotDetPtr->parkway.iPark() == 4 );
	CPPUNIT_ASSERT( spotDetPtr->parkway.iWay() == 15 );
	CPPUNIT_ASSERT( spotDetPtr->timeSecInterval == 88 );
	CPPUNIT_ASSERT( spotDetPtr->optCode == EsrKit( 12449 ) );
	CPPUNIT_ASSERT( tevent.GetSource() == TrainCharacteristics::Source::Disposition );
	CPPUNIT_ASSERT( tevent.GetPrevId() == 601 );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::DisformTimeOnReachingEndOfStation()
{
	TI.DetermineSwitchPlus(L"11", *tracker);
	TI.DetermineSwitchPlus(L"12", *tracker);
	TI.DetermineSwitchMinus(L"14", *tracker);

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6236" ) ), list<wstring>( 1, L"A" ) );
	TI.DetermineStripBusy( L"D", *tracker );
	auto arrivalMom = TI.DetermineStripFree( L"A", *tracker );
	auto deathMom = TI.DetermineStripFree( L"D", *tracker );
	auto formMom2 = TI.DetermineStripBusy( L"S", *tracker );
	auto disformMom = TI.DetermineStripBusy( L"T", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 5 );
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );

	CPPUNIT_ASSERT( chk_eq( evvec[0], HCode::FORM, L"A" ) );
	CPPUNIT_ASSERT( chk_eq( evvec[1], HCode::ARRIVAL, L"D" ) && evvec[1]->GetTime() == arrivalMom );
	CPPUNIT_ASSERT( chk_eq( evvec[2], HCode::DEATH, L"D" ) && evvec[2]->GetTime() == deathMom );
	CPPUNIT_ASSERT( chk_eq( evvec[3], HCode::FORM, L"S" ) && evvec[3]->GetTime() == formMom2 );
	CPPUNIT_ASSERT( chk_eq( evvec[4], HCode::DISFORM, L"T" ) && evvec[4]->GetTime() == disformMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEvents::SameTimeEvents()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	auto initLPFThreshold = timConstr.LPFThresholdSec;
	
	TI.DetermineSwitchMinus(L"11", *tracker);
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6236" ) ), list<wstring>( 1, L"W" ) ); //поезд
	list<wstring> tplace;
	tplace.push_back( L"A" );
	tplace.push_back( L"B" );
	LocateUnityByForce( nullptr, tplace ); //маневровая
	timConstr.LPFThresholdSec = 10;
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.IncreaseTime( 5 * 1000 );
	TI.DetermineStripFree( L"B", *tracker );
	TI.IncreaseTime( 60 * 1000 );

	auto tevents = tracker->GetTrainEvents();
	auto tunities = trainCont->GetUnities();
	auto eventIt1 = find_if( tevents.cbegin(), tevents.cend(), [this]( TrackerEventPtr tevPtr ){
		return tevPtr->GetBadge() == TI.Get( L"Q" ).bdg && tevPtr->GetCode() == HCode::SPAN_MOVE;
	} );
	CPPUNIT_ASSERT( eventIt1 != tevents.cend() );
	auto eventIt2 = find_if( ++eventIt1, tevents.cend(), [this]( TrackerEventPtr tevPtr ){
		return tevPtr->GetBadge() == TI.Get( L"Q" ).bdg && tevPtr->GetCode() == HCode::SPAN_MOVE;
	} );
	CPPUNIT_ASSERT( eventIt2 != tevents.cend() );
	CPPUNIT_ASSERT( ( *eventIt1 )->GetTime() < ( *eventIt2 )->GetTime() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	timConstr.LPFThresholdSec = initLPFThreshold;
}