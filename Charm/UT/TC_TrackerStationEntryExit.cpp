#include "stdafx.h"
#include "TC_TrackerStationEntryExit.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerStationEntryExit );

void TC_TrackerStationEntryExit::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.woADWaysTransMs = 20000;
}

void TC_TrackerStationEntryExit::NoWayEntry()
{
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );

	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::ProlongedEntryAndArriving()
{
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ARRIVAL;
	} );
	CPPUNIT_ASSERT( teIt == trackerEvents.cend() );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> tevvec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( tevvec.size() == 4 );
	CPPUNIT_ASSERT( tevvec[0]->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( tevvec[1]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( tevvec[2]->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( tevvec[3]->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::FastTransit()
{
	TI.DetermineSwitchPlus( L"31", *tracker );
	TI.DetermineSwitchPlus( L"32", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker ); //въезд на станцию
	TI.DetermineStripBusy( L"D3", *tracker );
	TI.DetermineStripBusy( L"C3", *tracker );
	TI.DetermineStripBusy( L"A3", *tracker );
	TI.DetermineStripFree( L"Y3", *tracker );
	TI.DetermineStripFree( L"X3", *tracker );
	TI.DetermineStripFree( L"F3", *tracker );
	TI.DetermineStripFree( L"D3", *tracker );
	TI.DetermineStripFree( L"C3", *tracker );
	TI.DetermineStripBusy( L"G3", *tracker ); //выезд со станции
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 6 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::FastEntryAndArriving()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::NoADPassingAfterBlockpost()
{
	TI.DetermineSwitchMinus( L"BPSW", *tracker );
	TI.DetermineStripBusy( L"SBP5", *tracker );
	wstring stripsArr[] = { L"SBP5", L"SBP6", L"BP5", L"BP6", L"SBP8", L"SBP9", L"SBP10", L"SBP11" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 10 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"SBP5" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"SBP6" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"BP5" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"BP5" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"BP6" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"SBP8" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"SBP9" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"SBP10" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"SBP10" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"SBP11" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}


void TC_TrackerStationEntryExit::StationOutOnEqualStationsWithRoute()
{
	TI.DetermineRouteSet( L"Head512->AO5", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2686" ) ), list<wstring>( 1, L"R5" ) ) );
	TI.DetermineStripBusy( L"AO5", *tracker );
	TI.DetermineStripBusy( L"AP5", *tracker );
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> evec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evec[0]->GetCode() == HCode::FORM && evec[0]->GetBadge() == TI.Get( L"R5" ).bdg );
	CPPUNIT_ASSERT( evec[1]->GetCode() == HCode::STATION_EXIT && evec[1]->GetBadge() == TI.Get( L"AO5" ).bdg );
	CPPUNIT_ASSERT( evec[2]->GetCode() == HCode::STATION_ENTRY && evec[2]->GetBadge() == TI.Get( L"AP5" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::StationOutOnEqualStationsWithRoute2()
{
	TI.DetermineRouteSet( L"Head514->AQ5", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2686" ) ), list<wstring>( 1, L"AO5" ) ) );
	TI.DetermineStripBusy( L"AP5", *tracker );

	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> evec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evec[0]->GetCode() == HCode::FORM && evec[0]->GetBadge() == TI.Get( L"AO5" ).bdg );
	CPPUNIT_ASSERT( evec[1]->GetCode() == HCode::STATION_EXIT && evec[1]->GetBadge() == TI.Get( L"AO5" ).bdg );
	CPPUNIT_ASSERT( evec[2]->GetCode() == HCode::STATION_ENTRY && evec[2]->GetBadge() == TI.Get( L"AP5" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::StationOutOnEqualStationsWORoute()
{
	TI.DetermineRouteUnset( L"Head512->AO5", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2686" ) ), list<wstring>( 1, L"R5" ) ) );
	TI.DetermineStripBusy( L"AO5", *tracker );
	TI.DetermineStripBusy( L"AP5", *tracker );
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 2 );
	vector<TrackerEventPtr> evec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evec[0]->GetCode() == HCode::FORM && evec[0]->GetBadge() == TI.Get( L"R5" ).bdg );
	CPPUNIT_ASSERT( evec[1]->GetCode() == HCode::DISFORM && evec[1]->GetBadge() == TI.Get( L"AP5" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::StationOutOnEqualStationsWORoute2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2686" ) ), list<wstring>( 1, L"BI5" ) ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initId = tunities.back()->GetId();

	TI.DetermineStripBusy( L"BJ5", *tracker );
	TI.DetermineStripFree( L"BI5", *tracker );
	TI.DetermineRouteSet( L"Head522->BK5", *tracker );
	TI.DetermineStripBusy( L"BK5", *tracker );
	TI.DetermineStripFree( L"BJ5", *tracker );
	TI.DetermineRouteUnset( L"Head522->BK5", *tracker );
	TI.DetermineStripBusy( L"BL5", *tracker );
	TI.DetermineStripFree( L"BK5", *tracker );
	TI.DetermineRouteSet( L"Head524->BN5", *tracker );
	TI.DetermineStripBusy( L"BM5", *tracker );
	TI.DetermineStripFree( L"BL5", *tracker );
	TI.DetermineStripBusy( L"BN5", *tracker );

	const auto & tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evec.size() == 4 );
	CPPUNIT_ASSERT( evec[0]->GetCode() == HCode::FORM && evec[0]->GetBadge() == TI.Get( L"BI5" ).bdg && evec[0]->GetId() == initId );
	CPPUNIT_ASSERT( evec[1]->GetCode() == HCode::ARRIVAL && evec[1]->GetBadge() == TI.Get( L"BJ5" ).bdg && evec[1]->GetId() == initId );
	CPPUNIT_ASSERT( evec[2]->GetCode() == HCode::DEPARTURE && evec[2]->GetBadge() == TI.Get( L"BJ5" ).bdg && evec[2]->GetId() == initId );
	CPPUNIT_ASSERT( evec[3]->GetCode() == HCode::TRANSITION && evec[3]->GetBadge() == TI.Get( L"BM5" ).bdg && evec[3]->GetId() == initId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::StationOutOnUnequalStationsWithRoute()
{
	TI.DetermineRouteSet( L"Head514->AQ5", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2686" ) ), list<wstring>( 1, L"AP5" ) ) );
	TI.DetermineStripBusy( L"AQ5", *tracker );
	TI.DetermineStripBusy( L"AR5", *tracker );
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> evec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evec[0]->GetCode() == HCode::FORM && evec[0]->GetBadge() == TI.Get( L"AP5" ).bdg );
	CPPUNIT_ASSERT( evec[1]->GetCode() == HCode::STATION_EXIT && evec[1]->GetBadge() == TI.Get( L"AQ5" ).bdg );
	CPPUNIT_ASSERT( evec[2]->GetCode() == HCode::STATION_ENTRY && evec[2]->GetBadge() == TI.Get( L"AR5" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::StationOutOnUnequalStationsWORoute()
{
	TI.DetermineRouteUnset( L"Head514->AQ5", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2686" ) ), list<wstring>( 1, L"AP5" ) ) );
	TI.DetermineStripBusy( L"AQ5", *tracker );
	TI.DetermineStripBusy( L"AR5", *tracker );
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> evec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evec[0]->GetCode() == HCode::FORM && evec[0]->GetBadge() == TI.Get( L"AP5" ).bdg );
	CPPUNIT_ASSERT( evec[1]->GetCode() == HCode::STATION_EXIT && evec[1]->GetBadge() == TI.Get( L"AQ5" ).bdg );
	CPPUNIT_ASSERT( evec[2]->GetCode() == HCode::STATION_ENTRY && evec[2]->GetBadge() == TI.Get( L"AR5" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerStationEntryExit::StationOutOnUnequalStationsWORoute2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"BC5" ) ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initId = tunities.back()->GetId();

	TI.DetermineStripBusy( L"BB5", *tracker );
	TI.DetermineStripFree( L"BC5", *tracker );
	TI.DetermineStripBusy( L"AS5", *tracker );
	TI.DetermineStripFree( L"BB5", *tracker );
	TI.DetermineStripBusy( L"AR5", *tracker );
	TI.DetermineStripFree( L"AS5", *tracker );

	TI.DetermineStripBusy( L"AQ5", *tracker, false );
	TI.DetermineStripBusy( L"AP5", *tracker, false );
	TI.FlushData( *tracker, false );

	const auto & tevents = tracker->GetTrainEvents();

	CPPUNIT_ASSERT( tevents.size() == 4 );
	vector<TrackerEventPtr> evec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evec[0]->GetCode() == HCode::FORM && evec[0]->GetBadge() == TI.Get( L"BC5" ).bdg && evec[0]->GetId() == initId );
	CPPUNIT_ASSERT( evec[1]->GetCode() == HCode::DEPARTURE && evec[1]->GetBadge() == TI.Get( L"BC5" ).bdg && evec[1]->GetId() == initId );
	CPPUNIT_ASSERT( evec[2]->GetCode() == HCode::STATION_EXIT && evec[2]->GetBadge() == TI.Get( L"AR5" ).bdg && evec[2]->GetId() == initId );
	CPPUNIT_ASSERT( evec[3]->GetCode() == HCode::STATION_ENTRY && evec[3]->GetBadge() == TI.Get( L"AQ5" ).bdg && evec[3]->GetId() == initId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}