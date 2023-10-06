#include "stdafx.h"
#include "TC_TrackerADMultiways.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerADMultiways );

void TC_TrackerADMultiways::setUp()
{
	TI.Reset();
	tracker->Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.cargoTransArrivDurMs = 6000;
	MakeActivities();
}

void TC_TrackerADMultiways::MakeActivities()
{
	goodWaysPassingActs.resize( 6 );
	//инициализация
	goodWaysPassingActs[0] = nullptr;

	//движение до первого светофора
	goodWaysPassingActs[1] = []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
		wstring stripsArr[] = { L"M3", L"N3", L"O3", L"P3" };
		TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), trackerRef );
	};

	//сигнал для первого светофора по ходу движения
	goodWaysPassingActs[2] = nullptr;

	//движение до второго светофора
	goodWaysPassingActs[3] = []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
		TI.DetermineStripBusy( L"R3", trackerRef );
		TI.DetermineStripBusy( L"S3", trackerRef ); //P3-R3-S3
		TI.DetermineStripFree( L"P3", trackerRef );
		TI.DetermineStripFree( L"R3", trackerRef ); //S3
	};

	//сигнал для второго светофора по ходу движения
	goodWaysPassingActs[4] = nullptr;

	//завершение движения (выезд со станции)
	goodWaysPassingActs[5] = []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
		TI.DetermineStripBusy( L"T3", trackerRef );
		TI.DetermineStripFree( L"S3", trackerRef );
		TI.DetermineStripBusy( L"U3", trackerRef );
		TI.DetermineStripFree( L"T3", trackerRef );
	};
}

void TC_TrackerADMultiways::ArriveTransit()
{
	SetPreMoveActivity( []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
			TI.DetermineHeadClose( L"Head300", trackerRef );
			TI.DetermineRouteSet( L"Head302->T3", trackerRef );
	} );
	SetSemaActivities( 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head300->S3", trackerRef ); },
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head302->T3", trackerRef ); }
	);
	TI.DoActivities( goodWaysPassingActs, *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 9 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::ARRIVAL && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"T3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"U3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::TransitTransit()
{
	SetPreMoveActivity( []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
		TI.DetermineRouteSet( L"Head300->S3", trackerRef );
		TI.DetermineRouteSet( L"Head302->T3", trackerRef );
	} );
	SetSemaActivities( 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head300->S3", trackerRef ); },
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head302->T3", trackerRef ); }
	);
	TI.DoActivities( goodWaysPassingActs, *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 8 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"T3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"U3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::TransitArrive()
{
	SetPreMoveActivity( []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
		TI.DetermineRouteSet( L"Head300->S3", trackerRef );
		TI.DetermineHeadClose( L"Head302", trackerRef );
	} );
	SetSemaActivities( 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head300->S3", trackerRef ); }, 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head302->T3", trackerRef ); } 
	);
	TI.DoActivities( goodWaysPassingActs, *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 9 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::ARRIVAL && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"T3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"U3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::PassArrive()
{
	SetPreMoveActivity( []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
		TI.DetermineHeadClose( L"Head300", trackerRef );
		TI.DetermineHeadClose( L"Head302", trackerRef );
	} );
	SetSemaActivities( 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineHeadClose( L"Head300", trackerRef ); }, 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head302->T3", trackerRef ); } 
	);
	TI.DoActivities( goodWaysPassingActs, *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 10 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::ARRIVAL && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::ARRIVAL && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"T3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"U3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::PassTransit()
{
	TI.DetermineHeadClose( L"Head300", *tracker );
	TI.DetermineRouteSet( L"Head302->T3", *tracker );

	wstring stripsArr[] = { L"M3", L"N3", L"O3", L"P3" };
	TI.ImitateMotion( vector<wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );

	TI.DetermineHeadClose( L"Head300", *tracker );

	TI.DetermineStripBusy( L"R3", *tracker );
	TI.DetermineStripBusy( L"S3", *tracker ); //P3-R3-S3
	TI.DetermineStripFree( L"P3", *tracker );
	TI.DetermineStripFree( L"R3", *tracker ); //S3
	TI.DetermineRouteSet( L"Head302->T3", *tracker );

	TI.DetermineStripBusy( L"T3", *tracker );
	TI.DetermineStripFree( L"S3", *tracker );
	TI.DetermineStripBusy( L"U3", *tracker );
	TI.DetermineStripFree( L"T3", *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 9 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::ARRIVAL && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"T3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"U3" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::ArrivePass()
{
	SetPreMoveActivity( []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
		TI.DetermineHeadClose( L"Head300", trackerRef );
		TI.DetermineHeadClose( L"Head302", trackerRef );
	} );
	SetSemaActivities( 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head300->S3", trackerRef ); }, 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineHeadClose( L"Head302", trackerRef ); } 
	);
	TI.DoActivities( goodWaysPassingActs, *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 10 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::ARRIVAL && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::ARRIVAL && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"T3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"U3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::TransitPass()
{
	SetPreMoveActivity( []( TrackerInfrastructure & TI, TestTracker & trackerRef ){
		TI.DetermineRouteSet( L"Head300->S3", trackerRef );
		TI.DetermineHeadClose( L"Head302", trackerRef );
	} );
	SetSemaActivities( 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineRouteSet( L"Head300->S3", trackerRef ); }, 
		[]( TrackerInfrastructure & TI, TestTracker & trackerRef ){ TI.DetermineHeadClose( L"Head302", trackerRef ); } 
	);
	TI.DoActivities( goodWaysPassingActs, *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 9 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::ARRIVAL && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"T3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"U3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::ReversiveTransitTransit()
{
	TI.DetermineStripBusy( L"U3", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomOddInfo( trains.front() );

	TI.DetermineRouteSet( L"Head301->O3", *tracker );
	TI.DetermineRouteSet( L"Head303->P3", *tracker );

	wstring stripsArr[] = { L"U3", L"T3", L"S3", L"R3", L"P3", L"O3", L"N3" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 6 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"U3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"T3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::ReversiveDepartTransit()
{
	TI.DetermineRouteSet( L"Head301->O3", *tracker );
	TI.DetermineRouteSet( L"Head303->P3", *tracker );
	wstring stripsArr[] = { L"S3", L"R3", L"P3", L"O3", L"N3" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( trackerEvents.cbegin(), trackerEvents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, trackerEvents.size() == 5 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"S3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"N3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::MultiArrdepEventsOnUnstableSignal()
{
	//поезд на C
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomOddInfo( trains.front() );

	//имитация множественных прибытий/отправлений с п/о путей
	unsigned int repcount = 10;
	for ( unsigned int ind = 0; ind < repcount; ++ind )
	{
		//отправление через Head3
		TI.DetermineRouteSet( L"Head101->A", *tracker );
		TI.DetermineStripBusy( L"B", *tracker );

		//прибытие
		TI.DetermineRouteUnset( L"Head101->A", *tracker );
		TI.DetermineStripFree( L"B", *tracker );
	}

	auto tevents = tracker->GetTrainEvents();
	string eventsContent;
	for_each( tevents.cbegin(), tevents.cend(), [&eventsContent]( TrackerEventPtr tevPtr ){
		eventsContent += To1251( tevPtr->GetString() + L"\n" );
	} );
	auto arrCount = count_if( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevent ){
		return tevent->GetCode() == HCode::ARRIVAL;
	} );
	auto depCount = count_if( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevent ){
		return tevent->GetCode() == HCode::DEPARTURE;
	} );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, arrCount == repcount );
	CPPUNIT_ASSERT_MESSAGE( eventsContent, depCount == repcount );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::MultiWayBornAndGoSpan()
{
	TI.DetermineStripBusy( L"T3", *tracker, false );
	TI.DetermineStripBusy( L"S3", *tracker, false );
	TI.DetermineStripBusy( L"R3", *tracker, false );
	TI.DetermineStripBusy( L"P3", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineHeadClose( L"Head303", *tracker );
	TI.DetermineRouteSet( L"Head301->O3", *tracker );
	TI.DetermineStripFree( L"T3", *tracker );
	TI.DetermineStripBusy( L"O3", *tracker );
	TI.DetermineHeadClose( L"Head301", *tracker );
	TI.DetermineStripFree( L"S3", *tracker );
	TI.DetermineStripFree( L"R3", *tracker );
	TI.DetermineStripBusy( L"N3", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( tevents.size() == 4 );
	auto tevIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"S3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEPARTURE && (*tevIt)->GetBadge() == TI.Get( L"P3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"O3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"N3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::UpgradeOnTwoADWaysAndLeave()
{
	TI.DetermineStripBusy( L"T3", *tracker );
	TI.DetermineStripBusy( L"S3", *tracker );
	TI.DetermineStripFree( L"T3", *tracker );
	TI.DetermineStripBusy( L"R3", *tracker );
	TI.DetermineStripBusy( L"P3", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomOddInfo( trains.front() );
	TI.DetermineStripFree( L"S3", *tracker ); //выезд с п/о пути
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::ComplexWayBornAndGoOut()
{
	TI.DetermineStripBusy( L"R4", *tracker, false );
	TI.DetermineStripBusy( L"S4", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineRouteSet( L"Head400->K4", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( tevents.size() == 2 );
	auto tevIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"S4" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEPARTURE && (*tevIt)->GetBadge() == TI.Get( L"S4" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::OutFromADWayWithMidsema()
{
	TI.DetermineRouteSet( L"Head305->Q3", *tracker );
	TI.DetermineRouteSet( L"Head307->V3", *tracker );

	TI.DetermineStripBusy( L"W3", *tracker );
	TI.DetermineStripBusy( L"V3", *tracker );
	TI.DetermineStripFree( L"W3", *tracker );
	TI.DetermineStripBusy( L"Q3", *tracker );
	TI.DetermineStripFree( L"V3", *tracker );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 2 );
	auto tevIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"W3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEPARTURE && (*tevIt)->GetBadge() == TI.Get( L"V3" ).bdg );
	tracker->TakeEvents( tevents );
	TI.DetermineStripBusy( L"U3", *tracker );
	tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 2 );
	tevIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"Q3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"U3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::GoingThroughADWayWithMidsema1()
{
	TI.DetermineRouteSet( L"Head304->Z3", *tracker );
	TI.DetermineStripBusy( L"U3", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomEvenInfo( trains.front() );
	TI.DetermineStripBusy( L"Q3", *tracker );
	TI.DetermineStripBusy( L"V3", *tracker );
	TI.DetermineStripFree( L"U3", *tracker );

	TI.DetermineStripBusy( L"W3", *tracker );
	TI.DetermineStripFree( L"Q3", *tracker );
	TI.DetermineStripBusy( L"Z3", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( tevents.size() == 3 );
	auto tevIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"U3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_ENTRY && (*tevIt)->GetBadge() == TI.Get( L"Q3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::TRANSITION && (*tevIt)->GetBadge() == TI.Get( L"W3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::GoingThroughADWayWithMidsema2()
{
	TI.DetermineRouteSet( L"Head305->Q3", *tracker );
	TI.DetermineHeadClose( L"Head307", *tracker );

	TI.DetermineStripBusy( L"AA3", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomOddInfo( trains.front() );

	TI.DetermineStripBusy( L"Z3", *tracker );
	TI.DetermineStripFree( L"AA3", *tracker );
	TI.DetermineStripBusy( L"W3", *tracker );
	TI.DetermineStripFree( L"Z3", *tracker );
	TI.DetermineHeadOpen( L"Head307", *tracker );
	TI.DetermineStripBusy( L"V3", *tracker );
	TI.DetermineStripFree( L"W3", *tracker );
	TI.DetermineStripBusy( L"Q3", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	auto tevIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"AA3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_ENTRY && (*tevIt)->GetBadge() == TI.Get( L"Z3" ).bdg );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::TRANSITION && (*tevIt)->GetBadge() == TI.Get( L"V3" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerADMultiways::ExcessiveDeparture()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"210" ) ), list<wstring>( 1, L"N3" ) );
	TI.DetermineRouteSet( L"Head300->S3", *tracker );
	TI.DetermineRouteSet( L"Head302->T3", *tracker );

	TI.DetermineStripBusy( L"O3", *tracker );
	TI.DetermineStripFree( L"N3", *tracker );
	TI.DetermineStripBusy( L"P3", *tracker );
	TI.DetermineStripBusy( L"R3", *tracker );
	TI.DetermineStripBusy( L"S3", *tracker );
	TI.DetermineStripFree( L"O3", *tracker );
	TI.DetermineStripFree( L"P3", *tracker );
	TI.DetermineStripFree( L"R3", *tracker );
	TI.DetermineRouteUnset( L"Head300->S3", *tracker );

	//поезд проследовал P3 и втянулся на S3

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();

	//прибытие с одновременной смертью
	TI.DetermineStripFree( L"S3", *tracker, false );
	TI.DetermineRouteUnset( L"Head302->T3", *tracker, false );
	TI.FlushData( *tracker, false );

	tevents = tracker->GetTrainEvents();
	tunities = trainCont->GetUnities();

	//восстановление (в т.ч. и на предыдущем ПО-пути)
	TI.DetermineStripBusy( L"P3", *tracker, false );
	TI.DetermineStripBusy( L"R3", *tracker, false );
	TI.DetermineStripBusy( L"S3", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	auto trainHead = tunities.back()->Head();
	tevents = tracker->GetTrainEvents();

	//выезд на следующий перегон
	TI.DetermineRouteSet( L"Head302->T3", *tracker );
	TI.DetermineStripFree( L"P3", *tracker );

	tevents = tracker->GetTrainEvents();

	auto isP3WayEvent = [this]( TrackerEventPtr tevPtr ){
		return tevPtr->GetBadge() == TI.Get( L"P3" ).bdg;
	};
	CPPUNIT_ASSERT( count_if( tevents.cbegin(), tevents.cend(), isP3WayEvent ) == 1 );

	auto p3EventIt = find_if( tevents.cbegin(), tevents.cend(), isP3WayEvent );
	CPPUNIT_ASSERT( p3EventIt != tevents.cend() );
	CPPUNIT_ASSERT( ( *p3EventIt )->GetCode() == HCode::TRANSITION  );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}