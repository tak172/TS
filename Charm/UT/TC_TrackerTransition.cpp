#include "stdafx.h"
#include "TC_TrackerTransition.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTransition );

void TC_TrackerTransition::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.closwayADArrivMs = 12000;
	timConstr.cargoTransArrivDurMs = 3000;
	timConstr.passenNotSpecTransArrivDurMs = 2000;
	timConstr.passenSpecTransArrivDurMs = 15000;
	timConstr.woADWaysTransMs = 20000;
}

void TC_TrackerTransition::UncontainedTransition()
{
	TI.DetermineHeadClose( L"Head102", *tracker ); //на пути C закрыт попутный светофор
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	list<TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker ); //формируется событие проследования
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::TRANSITION;
	} );
	CPPUNIT_ASSERT( arrEvIt != trackerEvents.cend() );

	//убеждаемся в отсутствии асинхронного события прибытия
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.closwayADArrivMs + 1000 );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::AsyncTransitionCargo()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.closwayADArrivMs = 0;
	timConstr.cargoTransArrivDurMs = 5000;
	timConstr.passenNotSpecTransArrivDurMs = 0;
	timConstr.passenSpecTransArrivDurMs = 0;

	const auto & tmConstr = tracker->GetTimeConstraints();
	OpenWayAsyncArriving( TrainCharacteristics::TrainFeature::Outbound, tmConstr.cargoTransArrivDurMs - 3000 );
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::AsyncTransitionPassengerSpec()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.closwayADArrivMs = 0;
	timConstr.cargoTransArrivDurMs = 0;
	timConstr.passenNotSpecTransArrivDurMs = 0;
	timConstr.passenSpecTransArrivDurMs = 5000;

	TI.DetermineRouteSet( L"Head400->K4", *tracker );
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"Q4", *tracker );
	const auto & tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.front();
	SetRandomEvenInfo( train );
	auto tdescrPtr = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	TrainDescr tdescr( *tdescrPtr );
	SetInfo( train, tdescr );
	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripBusy( L"O4", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );
	TI.DetermineStripFree( L"Q4", *tracker );
	TI.DetermineStripFree( L"A4", *tracker );
	TI.DetermineStripFree( L"O4", *tracker );

	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.passenSpecTransArrivDurMs - 3000 );
	TI.DetermineStripBusy( L"F4", *tracker );

	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::AsyncTransitionPassengerNoSpec()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.closwayADArrivMs = 0;
	timConstr.cargoTransArrivDurMs = 0;
	timConstr.passenNotSpecTransArrivDurMs = 5000;
	timConstr.passenSpecTransArrivDurMs = 0;

	const auto & tmConstr = tracker->GetTimeConstraints();
	OpenWayAsyncArriving( TrainCharacteristics::TrainFeature::FastAllYear, tmConstr.passenNotSpecTransArrivDurMs - 3000 );
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::StandardTransition()
{
	TI.DetermineRouteSet( L"Head100->C", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	wstring stripsArr[] = { L"Q", L"A", L"B", L"C" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker ); //перекрытие светофора перед покиданием п/о пути
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	auto trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 5 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::TRANSITION );
	auto transPtr = eventsVec[2]->GetDetails();
	CPPUNIT_ASSERT( transPtr && transPtr->optCode == TI.Get( L"X" ).bdg.num() );
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( eventsVec[4]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::BlockpostTransition()
{
	//проследование через блок-пост
	TI.DetermineStripBusy( L"SBP3", *tracker );
	TI.DetermineStripBusy( L"SBP4", *tracker );
	TI.DetermineStripFree( L"SBP3", *tracker );
	TI.DetermineStripBusy( L"BP3", *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	TI.DetermineStripFree( L"SBP4", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::OpenedBlockpostTransition()
{
	TI.DetermineRouteSet( L"BPHead100->BP4", *tracker );
	BlockpostTransition();
}

void TC_TrackerTransition::ClosedBlockpostTransition()
{
	TI.DetermineHeadClose( L"BPHead100", *tracker );
	BlockpostTransition();
}

void TC_TrackerTransition::WOWaysTransition()
{
	TI.DetermineSwitchPlus( L"15", *tracker );
	TI.DetermineSwitchMinus( L"16", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );
	auto enterTime = TI.DetermineStripBusy( L"J", *tracker );
	TI.DetermineStripFree( L"X", *tracker );
	TI.DetermineStripFree( L"I", *tracker );
	TI.DetermineStripBusy( L"N", *tracker );
	TI.DetermineStripBusy( L"P", *tracker );
	TI.DetermineStripFree( L"J", *tracker );
	TI.DetermineStripFree( L"N", *tracker );
	auto exitTime = TI.DetermineStripBusy( L"R", *tracker );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( evVec.size() == 6 );
	CPPUNIT_ASSERT( evVec[0]->GetCode() == HCode::FORM && evVec[0]->GetBadge() == TI.Get( L"X" ).bdg );
	CPPUNIT_ASSERT( evVec[1]->GetCode() == HCode::SPAN_MOVE && evVec[1]->GetBadge() == TI.Get( L"I" ).bdg );
	CPPUNIT_ASSERT( evVec[2]->GetCode() == HCode::STATION_ENTRY && evVec[2]->GetBadge() == TI.Get( L"J" ).bdg && evVec[2]->GetTime() == enterTime );
	CPPUNIT_ASSERT( evVec[3]->GetCode() == HCode::TRANSITION && evVec[3]->GetBadge() == TI.Get( L"P" ).bdg && evVec[3]->GetTime() == exitTime );
	CPPUNIT_ASSERT( evVec[4]->GetCode() == HCode::STATION_EXIT && evVec[4]->GetBadge() == TI.Get( L"P" ).bdg && evVec[4]->GetTime() == exitTime );
	CPPUNIT_ASSERT( evVec[5]->GetCode() == HCode::SPAN_MOVE && evVec[5]->GetBadge() == TI.Get( L"R" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::WOWaysPassing()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	TI.DetermineSwitchPlus( L"15", *tracker );
	TI.DetermineSwitchMinus( L"16", *tracker );

	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );
	auto enterTime = TI.DetermineStripBusy( L"J", *tracker );
	TI.DetermineStripFree( L"X", *tracker );
	TI.DetermineStripFree( L"I", *tracker );
	TI.DetermineStripBusy( L"N", *tracker );
	TI.DetermineStripBusy( L"P", *tracker );
	TI.DetermineStripFree( L"J", *tracker );
	TI.DetermineStripFree( L"N", *tracker );

	TI.IncreaseTime( timConstr.woADWaysTransMs + 1000 );
	auto exitTime = TI.DetermineStripBusy( L"R", *tracker );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( evVec.size() == 5 );
	CPPUNIT_ASSERT( evVec[0]->GetCode() == HCode::FORM && evVec[0]->GetBadge() == TI.Get( L"X" ).bdg );
	CPPUNIT_ASSERT( evVec[1]->GetCode() == HCode::SPAN_MOVE && evVec[1]->GetBadge() == TI.Get( L"I" ).bdg );
	CPPUNIT_ASSERT( evVec[2]->GetCode() == HCode::STATION_ENTRY && evVec[2]->GetBadge() == TI.Get( L"J" ).bdg && evVec[2]->GetTime() == enterTime );
	CPPUNIT_ASSERT( evVec[3]->GetCode() == HCode::STATION_EXIT && evVec[3]->GetBadge() == TI.Get( L"P" ).bdg && evVec[3]->GetTime() == exitTime );
	CPPUNIT_ASSERT( evVec[4]->GetCode() == HCode::SPAN_MOVE && evVec[4]->GetBadge() == TI.Get( L"R" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::DeparturingFormAndBPTransition()
{
	//появление на п/о пути и выезд с него
	TI.DetermineStripBusy( L"ADBP1", *tracker, false );
	TI.DetermineStripBusy( L"SBP7", *tracker, false );
	TI.FlushData( *tracker, false );

	const auto & tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	TI.DetermineStripFree( L"ADBP1", *tracker );

	//проследование блок-поста
	TI.DetermineStripBusy( L"SBP5", *tracker );
	TI.DetermineStripFree( L"SBP7", *tracker );
	TI.DetermineStripBusy( L"SBP6", *tracker );
	TI.DetermineStripBusy( L"BP5", *tracker );
	TI.DetermineStripBusy( L"BP6", *tracker );
	TI.DetermineStripFree( L"SBP5", *tracker );
	TI.DetermineStripFree( L"SBP6", *tracker );
	TI.DetermineStripBusy( L"SBP8", *tracker );

	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 9 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"ADBP1" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"ADBP1" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"SBP7" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"SBP5" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"SBP6" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"BP5" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"BP5" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"BP6" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"SBP8" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::TransitionWhileMultiWaysLiberation()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineRouteSet( L"Head100->C", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	//частичный въезд на п/о путь
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );

	//одномоментное гашение "хвоста" поезда
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.FlushData( *tracker, false );

	//генерация проследования
	TI.DetermineStripBusy( L"F", *tracker );

	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"W" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"A" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::LongTransitionThroughEmbarkWay()
{
	TI.DetermineStripBusy( L"Q4", *tracker );
	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomEvenInfo( tunities.front() );
	TI.DetermineRouteSet( L"Head400->K4", *tracker );
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripBusy( L"O4", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker );
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto transEvent = trackerEvents.back();
	CPPUNIT_ASSERT( transEvent->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( transEvent->GetOffsetInterval() == 0 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::FastTransitionThroughEmbarkWay()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.closwayADArrivMs = 0;
	timConstr.cargoTransArrivDurMs = 2000;
	timConstr.passenNotSpecTransArrivDurMs = 3000;
	timConstr.passenSpecTransArrivDurMs = 15000;

	TI.DetermineStripBusy( L"Q4", *tracker );
	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetInfo( tunities.front(), TrainDescr( L"7100" ) ); //пригородный
	TI.DetermineRouteSet( L"Head400->K4", *tracker );
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripBusy( L"O4", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );
	TI.DetermineStripFree( L"Q4", *tracker );
	TI.DetermineStripFree( L"A4", *tracker );
	time_t wholeInTime = TI.DetermineStripFree( L"O4", *tracker );
	TI.IncreaseTime( 7000 );
	time_t wayExitTime = TI.DetermineStripBusy( L"F4", *tracker );
	auto offsetTimeSec = wayExitTime - wholeInTime;
	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto transEvent = trackerEvents.back();
	CPPUNIT_ASSERT( transEvent->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( transEvent->GetOffsetInterval() == offsetTimeSec );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::MoveThroughSeveralBlockposts()
{
	TI.DetermineSwitchMinus( L"BPSW", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2835" ) ), list<wstring>( 1, L"ADBP1" ) );
	auto depMom = TI.DetermineStripBusy( L"SBP7", *tracker );
	TI.DetermineStripFree( L"ADBP1", *tracker );
	wstring warr[] = { L"SBP7", L"SBP5", L"SBP6", L"BP5", L"BP6", L"SBP8", L"SBP9", L"SBP10", L"SBP11" };
	TI.ImitateMotion( vector<wstring>( begin( warr ), end( warr ) ), *tracker );

	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 13 );
	vector<TrackerEventPtr> evvec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"ADBP1" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"ADBP1" ).bdg );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetBadge() == TI.Get( L"SBP7" ).bdg );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetBadge() == TI.Get( L"SBP5" ).bdg );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::SPAN_MOVE && evvec[4]->GetBadge() == TI.Get( L"SBP6" ).bdg );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::STATION_ENTRY && evvec[5]->GetBadge() == TI.Get( L"BP5" ).bdg );
	CPPUNIT_ASSERT( evvec[6]->GetCode() == HCode::TRANSITION && evvec[6]->GetBadge() == TI.Get( L"BP5" ).bdg );
	CPPUNIT_ASSERT( evvec[7]->GetCode() == HCode::STATION_EXIT && evvec[7]->GetBadge() == TI.Get( L"BP6" ).bdg );
	CPPUNIT_ASSERT( evvec[8]->GetCode() == HCode::SPAN_MOVE && evvec[8]->GetBadge() == TI.Get( L"SBP8" ).bdg );
	CPPUNIT_ASSERT( evvec[9]->GetCode() == HCode::STATION_ENTRY && evvec[9]->GetBadge() == TI.Get( L"SBP9" ).bdg );
	CPPUNIT_ASSERT( evvec[10]->GetCode() == HCode::TRANSITION && evvec[10]->GetBadge() == TI.Get( L"SBP10" ).bdg );
	CPPUNIT_ASSERT( evvec[11]->GetCode() == HCode::STATION_EXIT && evvec[11]->GetBadge() == TI.Get( L"SBP10" ).bdg );
	CPPUNIT_ASSERT( evvec[12]->GetCode() == HCode::SPAN_MOVE && evvec[12]->GetBadge() == TI.Get( L"SBP11" ).bdg );

	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTransition::HistoryLimitationOnTransitionGeneration()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"15", *tracker );
	TI.DetermineRouteSet( L"Head106->P", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	//TI.DetermineStripFree( L"B", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );
	TI.DetermineStripBusy( L"J", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripFree( L"X", *tracker );
	TI.DetermineStripFree( L"I", *tracker );

	//проследование
	TI.DetermineStripBusy( L"L", *tracker );
	TI.DetermineStripBusy( L"M", *tracker );
	TI.DetermineStripBusy( L"O", *tracker );

	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 7 );
	auto transEvent = tevents.back();
	CPPUNIT_ASSERT( transEvent->GetCode() == HCode::TRANSITION && transEvent->GetOffsetInterval() == 0 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}