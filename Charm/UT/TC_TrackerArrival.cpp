#include "stdafx.h"
#include "TC_TrackerArrival.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerArrival );

void TC_TrackerArrival::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.closwayADArrivMs = 3000;
	timConstr.cargoTransArrivDurMs = 9000;
	timConstr.passenNotSpecTransArrivDurMs = 7000;
}

void TC_TrackerArrival::ArrivingToOpenWay()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker ); //на пути C открыт попутный светофор
    bool arrived = PullingTailToCWay();
    CPPUNIT_ASSERT( !arrived );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::ArrivingToClosedWay()
{
	TI.DetermineHeadClose( L"Head102", *tracker ); //на пути C закрыт попутный светофор
    bool arrived = PullingTailToCWay();
    CPPUNIT_ASSERT( arrived );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::ArrivingToWayWOSema()
{
	wstring stripsArr[] = { L"W", L"Q", L"A" };
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
	CPPUNIT_ASSERT( arrEvIt == trackerEvents.cend() );
	TI.DetermineStripBusy( L"D", *tracker ); //A-D
    TI.DetermineStripFree( L"A", *tracker ); //D
	trackerEvents = tracker->GetTrainEvents();
	arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
    CPPUNIT_ASSERT( arrEvIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::FastArrivingToWayWOSema()
{
	wstring stripsArr[] = { L"W", L"Q", L"A" };
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );

	//быстрый проезд п/о пути
	TI.DetermineStripBusy( L"D", *tracker, false );
	TI.DetermineStripBusy( L"E", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.DetermineStripFree( L"E", *tracker, false );
	TI.FlushData( *tracker, false );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
	CPPUNIT_ASSERT( arrEvIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::CoupledSlowArriving()
{
	TI.DetermineStripBusy( L"O4", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.front();
	SetRandomEvenInfo( train );
	TI.DetermineStripBusy( L"R4", *tracker );
	TI.DetermineStripFree( L"O4", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 2 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM ); //O4
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL ); //R4
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::OppositeArriving()
{
	TI.DetermineHeadClose( L"Head101", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.front();
	SetRandomOddInfo( train );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 2 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );

	tracker->Reset();
	TI.DetermineRouteSet( L"Head101->A", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	train = tlist.front();
	SetRandomEvenInfo( train );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 2 );
	teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::CloseWayUncontainedArriving()
{
	TI.DetermineHeadClose( L"Head102", *tracker ); //на пути C закрыт попутный светофор
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	time_t adEntryMom = TI.DetermineStripBusy( L"C", *tracker );

	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	list<TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );

	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.closwayADArrivMs + 1000 );
	time_t arrivalMom = adEntryMom + tmConstr.closwayADArrivMs / 1000;

	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
	CPPUNIT_ASSERT( arrEvIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *arrEvIt )->GetTime() == arrivalMom );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::OpenWayAsyncArrivingPassenger()
{
	const auto & tmConstr = tracker->GetTimeConstraints();
	OpenWayAsyncArriving( TrainCharacteristics::TrainFeature::FastAllYear, tmConstr.passenNotSpecTransArrivDurMs + 1500 );
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::OpenWayAsyncArrivingCargo()
{
	const auto & tmConstr = tracker->GetTimeConstraints();
	OpenWayAsyncArriving( TrainCharacteristics::TrainFeature::Outbound, tmConstr.cargoTransArrivDurMs + 1500 );
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::ArrivingToOpenWayAndCloseSema()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	PullingTailToCWay();
	TI.DetermineHeadClose( L"Head102", *tracker ); //должно быть сгенерировано событие прибытия
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::ArrivingToOpenWayAndCloseSema2()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker ); //события прибытия пока еще нет
	TI.DetermineStripFree( L"B", *tracker ); //должно быть сгенерировано событие прибытия
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::ArrivingToClosedWayAndReversiveExit()
{
	TI.DetermineHeadClose( L"Head101", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );

	//имитируем въезд поезда на станцию с перегона
	wstring stripsArr[] = { L"I", L"X", L"G", L"F" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto initTrainId = trains.front()->GetId();
	
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( timConstr.closwayADArrivMs + 1000 ); //асинхронно генерируем занятие закрытого п/о пути

	//возвращение на тот же перегон
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto nowTrain = trains.back();
	CPPUNIT_ASSERT( nowTrain->Have( TI.Get( L"G" ).bdg ) );
	CPPUNIT_ASSERT( nowTrain->Have( TI.Get( L"X" ).bdg ) );
	CPPUNIT_ASSERT( nowTrain->GetOddness() == Oddness::EVEN );

	auto trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 7 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"I" ).bdg && evvec[0]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::SPAN_MOVE && evvec[1]->GetBadge() == TI.Get( L"X" ).bdg && evvec[1]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_ENTRY && evvec[2]->GetBadge() == TI.Get( L"G" ).bdg && evvec[2]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::ARRIVAL && evvec[3]->GetBadge() == TI.Get( L"C" ).bdg && evvec[3]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::DEPARTURE && evvec[4]->GetBadge() == TI.Get( L"C" ).bdg && evvec[4]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::STATION_EXIT && evvec[5]->GetBadge() == TI.Get( L"G" ).bdg && evvec[5]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[6]->GetCode() == HCode::SPAN_MOVE && evvec[6]->GetBadge() == TI.Get( L"X" ).bdg && evvec[6]->GetId() == initTrainId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::UncontainedArrivingPassAndBack()
{
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );

	//имитируем въезд поезда на станцию с перегона
	wstring stripsArr[] = { L"W", L"Q", L"A", L"B" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	time_t adEntryMom = TI.DetermineStripBusy( L"C", *tracker );

	//выезд с п/о пути и возвращение
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.closwayADArrivMs + 1000 );;
	time_t arrivalMom = adEntryMom + tmConstr.closwayADArrivMs / 1000;

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	vector<TrackerEventPtr> eventsVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"W" ).bdg );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::SPAN_MOVE && eventsVec[1]->GetBadge() == TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::STATION_ENTRY && eventsVec[2]->GetBadge() == TI.Get( L"A" ).bdg );
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::ARRIVAL && eventsVec[3]->GetBadge() == TI.Get( L"C" ).bdg && 
		eventsVec[3]->GetTime() == arrivalMom );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::OpenWayAsyncArrivingMomentFullEntry()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.back();
	TrainDescr tdescr;
	tdescr.SetFeature( TrainCharacteristics::TrainFeature::Outbound );
	const auto & tcharacts = TrainCharacteristics::instanceCRef();
	CPPUNIT_ASSERT( tcharacts.IsCargoTrain( TrainCharacteristics::TrainFeature::Outbound ) );
	SetInfo( train, tdescr );

	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	time_t adFullEntryMom = TI.DetermineStripFree( L"B", *tracker );

	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.cargoTransArrivDurMs + 1000 );
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
	CPPUNIT_ASSERT( arrEvIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *arrEvIt )->GetTime() == adFullEntryMom );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::OpenWayAsyncArrivingMomentFullEntry2()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2816" ) ), list<wstring>( 1, L"W" ) );
	const auto & tcharacts = TrainCharacteristics::instanceCRef();
	const auto & trainPtr = trainCont->GetUnity( TI.Get( L"W" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto numfeauture = tcharacts.GetPurposeFeature( trainPtr->GetDescrPtr()->GetNumber().getNumber() );
	CPPUNIT_ASSERT( tcharacts.IsCargoTrain( numfeauture ) );

	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	time_t adFullEntryMom = TI.FlushData( *tracker, false );

	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( 3 * tmConstr.cargoTransArrivDurMs ); //очень долгая стоянка - должно приводить к прибытию
	TI.DetermineStripBusy( L"F", *tracker );
	const auto & trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 5 );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::ARRIVAL && evvec[3]->GetTime() == adFullEntryMom );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::OpenWayAsyncArrivingMomentPartialEntry()
{
	auto arrived = [this]( TrainCharacteristics::TrainFeature tfeature, unsigned int waitingPeriodMs )->bool{
		const auto & tmconstr = tracker->GetTimeConstraints();
		const auto & tcharacts = TrainCharacteristics::instanceCRef();
		bool isPassenFeature = tcharacts.CanTransportPassengers( tfeature );
		auto arrivalEventDelayMs = isPassenFeature ? tmconstr.passenNotSpecTransArrivDurMs : tmconstr.cargoTransArrivDurMs;

		TI.DetermineHeadOpen( L"Head102", *tracker );
		TI.DetermineSwitchMinus( L"11", *tracker );
		TI.DetermineStripBusy( L"W", *tracker );
		auto tlist = trainCont->GetUnities();
		CPPUNIT_ASSERT( tlist.size() == 1 );
		auto train = tlist.back();
		TrainDescr tdescr;
		tdescr.SetFeature( tfeature );
		SetInfo( train, tdescr );

		TI.DetermineStripBusy( L"Q", *tracker );
		TI.DetermineStripBusy( L"A", *tracker );
		TI.DetermineStripBusy( L"B", *tracker );
		time_t adEntryMom = TI.DetermineStripBusy( L"C", *tracker );

		TI.DetermineStripFree( L"W", *tracker );
		TI.DetermineStripFree( L"Q", *tracker );
		TI.DetermineStripFree( L"A", *tracker );

		auto tevents = tracker->GetTrainEvents();

		TI.IncreaseTime( waitingPeriodMs );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );

		time_t arrivalMom = adEntryMom + arrivalEventDelayMs / 1000;
		const auto & trackerEvents = tracker->GetTrainEvents();
		
		auto arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
			return tevPtr->GetCode() == HCode::ARRIVAL;
		} );
		bool haveArrivalEvent = ( arrEvIt != trackerEvents.cend() );
		if ( haveArrivalEvent )
		{
			CPPUNIT_ASSERT( ( *arrEvIt )->GetTime() == arrivalMom );
			CPPUNIT_ASSERT( trackerEvents.size() == 4 );
		}
		else
			CPPUNIT_ASSERT( trackerEvents.size() == 3 );
		tracker->Reset();
		return haveArrivalEvent;
	};

	const auto & tmConstr = tracker->GetTimeConstraints();
	typedef TrainCharacteristics::TrainFeature TrainFeature;
	TrainFeature passenFeatures[] = { TrainFeature::SuburbanRegional, TrainFeature::FastAllYear, TrainFeature::MVPS_Served, TrainFeature::Rapid, TrainFeature::Highrapid };
	for ( auto featIt = begin( passenFeatures ); featIt != end( passenFeatures ); ++featIt )
	{
		auto passenFeature = *featIt;
		//оба утверждения срабатывают, т.к. для пассажирского поезда требуется меньший интервал для генерации события прибытия
		CPPUNIT_ASSERT( !arrived( passenFeature, tmConstr.passenNotSpecTransArrivDurMs - 4000 ) );
		CPPUNIT_ASSERT( arrived( passenFeature, tmConstr.passenNotSpecTransArrivDurMs ) );
		CPPUNIT_ASSERT( arrived( passenFeature, tmConstr.passenNotSpecTransArrivDurMs + 4000 ) );
	}

	TrainFeature cargoFeatures[] = { TrainFeature::Refrigerator, TrainFeature::Heavy_g8000, TrainFeature::TechnicalPassenger, TrainFeature::Refrigerator };
	for ( auto featIt = begin( cargoFeatures ); featIt != end( cargoFeatures ); ++featIt )
	{
		auto cargoFeature = *featIt;
		CPPUNIT_ASSERT( !arrived( cargoFeature, tmConstr.passenNotSpecTransArrivDurMs ) );
		CPPUNIT_ASSERT( arrived( cargoFeature, tmConstr.cargoTransArrivDurMs ) );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::ForbidArrivingAfterFormOnSameADWay()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomEvenInfo( tunities.back() );

	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.FlushData( *tracker, false );
	
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 1 );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::ForbidArrivingAfterFormOnSameADWay2()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomEvenInfo( tunities.back() );

	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 1 );
	TI.DetermineHeadClose( L"Head102", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 1 );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::PermitArrivingAfterForm()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomEvenInfo( tunities.back() );

	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.FlushData( *tracker, false );

	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 2 );
	CPPUNIT_ASSERT( trackerEvents.front()->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::ArrivingToBusyWay()
{
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2816" ) ), list<wstring>( 1, L"C" ) );
	auto ctrainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( ctrainPtr );
	auto ctrainId = ctrainPtr->GetId();
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"3234" ) ), list<wstring>( 1, L"A" ) );
	auto atrainPtr = trainCont->GetUnity( TI.Get( L"A" ).bdg );
	CPPUNIT_ASSERT( atrainPtr );
	auto atrainId = atrainPtr->GetId();
	CPPUNIT_ASSERT( ctrainId != atrainId );

	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"B", *tracker );

	auto tunities = trainCont->GetUnities();
	const auto & trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetId() == ctrainId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::FORM && evvec[1]->GetBadge() == TI.Get( L"A" ).bdg && evvec[1]->GetId() == atrainId );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::ARRIVAL && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg && evvec[2]->GetId() == atrainId );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DEATH && evvec[3]->GetBadge() == TI.Get( L"C" ).bdg && evvec[3]->GetId() == atrainId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::NoArrivingOnHeadCloseWithNoTrain()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"B" ) );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker );
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.empty() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerArrival::NoAsyncArrivalForNoTrain()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"C", *tracker );
	
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1608" ), TrainCharacteristics::Source::Disposition );
	auto tunities = trainCont->GetUnities();
	const auto & timconstr = tracker->GetTimeConstraints();
	
	auto maxarval = timconstr.maxArrivalIntervalMs();
	auto formMoment = TI.IncreaseTime( timconstr.maxArrivalIntervalMs() );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 1 );
	CPPUNIT_ASSERT( tevents.back()->GetCode() == HCode::FORM && tevents.back()->GetPureTime() == formMoment );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}