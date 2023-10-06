#include "stdafx.h"
#include "TC_TrackerEntranceStopping.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"

using namespace std;

static const std::string intervalSec_xAttr = "intervalSec";

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerEntranceStopping );
void TC_TrackerEntranceStopping::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.entranceStoppingLimitSec = 5;
	timConstr.bufferLimitMs = static_cast<unsigned int>( chrono::duration_cast<chrono::milliseconds>( chrono::hours( 10 ) ).count() );
}

void TC_TrackerEntranceStopping::DelayAndGenerate()
{
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	time_t QEntryMom = TI.DetermineStripBusy( L"Q", *tracker );

	const auto & tmConstr = tracker->GetTimeConstraints();
	time_t currentMom = TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );

	list<TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this, &tmConstr, currentMom, QEntryMom]( TrackerEventPtr tevtPtr ){
		auto spotDetPtr = tevtPtr->GetDetails();
		auto timeIValIsCorrect = spotDetPtr && spotDetPtr->timeSecInterval == currentMom - QEntryMom;
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING && tevtPtr->GetBadge() == TI.Get( L"Q" ).bdg && timeIValIsCorrect;
	} );

	CPPUNIT_ASSERT( teIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	tracker->Reset();
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::ShortDelay()
{
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 - 100 );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING && tevtPtr->GetBadge() == TI.Get( L"Q" ).bdg;
	} );

	CPPUNIT_ASSERT( teIt == trackerEvents.cend() );
	CPPUNIT_ASSERT( trackerEvents.size() < 3 );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	tracker->Reset();
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::FormWOGeneration()
{
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING && tevtPtr->GetBadge() == TI.Get( L"Q" ).bdg;
	} );

	CPPUNIT_ASSERT( teIt == trackerEvents.cend() );
	CPPUNIT_ASSERT( trackerEvents.size() < 3 );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	tracker->Reset();
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::ShortDelayAndMove()
{
	TI.DetermineHeadClose( L"Head100", *tracker );
	auto occMom = TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
    auto lastActMom = TI.DetermineStripFree( L"W", *tracker );
	unsigned int waitingSec = static_cast<unsigned int>( lastActMom - occMom );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( ( tmConstr.entranceStoppingLimitSec - waitingSec - 1 ) * 1000 );
    TI.DetermineStripBusy( L"A", *tracker );
	TI.IncreaseTime( ( tmConstr.entranceStoppingLimitSec + 1 ) * 1000 );
    list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING;
	} );
	CPPUNIT_ASSERT( teIt == trackerEvents.cend() );
    CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::JointPicketage()
{
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING && tevtPtr->GetBadge() == TI.Get( L"Q" ).bdg;
	} );

	CPPUNIT_ASSERT( teIt != trackerEvents.cend() );
    auto entStopEvent = *teIt;
    auto coords = entStopEvent->Coords();
    auto coord1 = coords.first.GetCoord( TI.axis_name );
    auto coord2 = coords.second.GetCoord( TI.axis_name );
    CPPUNIT_ASSERT( coord1 == rwCoord( 57, 700 ) && coord2 == rwCoord( 56, 150 ) || 
		coord1 == rwCoord( 56, 150 ) && coord2 == rwCoord( 57, 700 ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	tracker->Reset();
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::SpanExitAndStop()
{
    TI.DetermineHeadClose( L"Head100", *tracker );
    TI.DetermineStripBusy( L"A", *tracker );
    TI.DetermineStripBusy( L"Q", *tracker );
    TI.DetermineStripFree( L"A", *tracker );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING && tevtPtr->GetBadge() == TI.Get( L"Q" ).bdg;
	} );
    CPPUNIT_ASSERT( teIt == trackerEvents.cend() );
    CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::BlockpostStopping()
{
	TI.DetermineHeadClose( L"BPHead100", *tracker );
	TI.DetermineStripBusy( L"SBP3", *tracker );
	TI.DetermineStripBusy( L"SBP4", *tracker );
	TI.DetermineStripFree( L"SBP3", *tracker );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING;
	} );
	CPPUNIT_ASSERT( teIt == trackerEvents.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::OpenSemaStopping()
{
	TI.DetermineHeadOpen( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING && tevtPtr->GetBadge() == TI.Get( L"Q" ).bdg;
	} );
	CPPUNIT_ASSERT( teIt == trackerEvents.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::StopEventOnSemaClose()
{
	TI.DetermineHeadOpen( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineHeadClose( L"Head100", *tracker );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING && tevtPtr->GetBadge() == TI.Get( L"Q" ).bdg;
	} );
	CPPUNIT_ASSERT( teIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::NoStopEventOnSemaClose()
{
	TI.DetermineHeadOpen( L"Head102", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto teIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), [this]( TrackerEventPtr tevtPtr ){
		return tevtPtr->GetCode() == HCode::ENTRANCE_STOPPING && tevtPtr->GetBadge() == TI.Get( L"C" ).bdg;
	} );
	CPPUNIT_ASSERT( teIt == trackerEvents.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::ShortBlockpostFastPassing()
{
	TI.DetermineHeadClose( L"BPHead101", *tracker );
	TI.DetermineHeadClose( L"BPHead104", *tracker );
	TI.DetermineStripBusy( L"SBP5", *tracker );
	TI.DetermineStripBusy( L"SBP6", *tracker );
	TI.DetermineStripFree( L"SBP5", *tracker );
	const auto & tmConstr = tracker->GetTimeConstraints();

	TI.DetermineStripBusy( L"BP5", *tracker, false );
	TI.DetermineStripBusy( L"BP6", *tracker, false );
	TI.DetermineStripBusy( L"SBP8", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.IncreaseTime( 1100 ); //чтобы между проследованием и стоянкой была разница во времени

	//проследование блок-поста
	TI.DetermineStripFree( L"SBP6", *tracker );
	auto tevents2 = tracker->GetTrainEvents();

	//добиваемся генерации стоянки перед входным: 
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );
	
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( tevents.size() == 7 );
	auto teIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt )->GetBadge() == TI.Get( L"SBP5" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt )->GetBadge() == TI.Get( L"SBP6" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt )->GetBadge() == TI.Get( L"BP5" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt )->GetBadge() == TI.Get( L"BP6" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt )->GetBadge() == TI.Get( L"BP6" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt )->GetBadge() == TI.Get( L"SBP8" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::ENTRANCE_STOPPING && ( *teIt )->GetBadge() == TI.Get( L"SBP8" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::DeathAfterStop()
{
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.IncreaseTime( 1100 );
	TI.DetermineStripFree( L"Q", *tracker );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( tevents.size() >= 3 );
	auto teIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt )->GetBadge() == TI.Get( L"W" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt )->GetBadge() == TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( ( tevents.back() )->GetCode() == HCode::DEATH && ( *teIt )->GetBadge() == TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::RecloseSemaAfterStop()
{
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	list <TrackerEventPtr> tevents;
	tracker->TakeEvents( tevents );
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );
	auto stopMoment = tracker->GetLastTime();
	TI.DetermineHeadOpen( L"Head100", *tracker );
	TI.DetermineHeadClose( L"Head100", *tracker );
	const auto & trainEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trainEvents.size() == 1 );
	CPPUNIT_ASSERT( trainEvents.front()->GetCode() == HCode::ENTRANCE_STOPPING && trainEvents.front()->GetBadge() == TI.Get( L"Q" ).bdg &&
		trainEvents.front()->GetTime() == stopMoment );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::LittleStopOnShortSpan()
{
	TI.DetermineHeadClose( L"Head406", *tracker );
	TI.DetermineHeadOpen( L"Head405", *tracker );
	TI.DetermineStripBusy( L"AD4", *tracker );
	TI.DetermineStripBusy( L"AE4", *tracker );
	list <TrackerEventPtr> tevents;
	tracker->TakeEvents( tevents );
	TI.DetermineHeadClose( L"Head405", *tracker );
	const auto & stopEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( stopEvents.empty() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::NoEntStopOnSingleStripSABWay()
{
	TI.DetermineHeadClose( L"Head306", *tracker );
	TI.DetermineStripBusy( L"M3", *tracker );
	TI.DetermineStripBusy( L"N3", *tracker );
	TI.DetermineStripFree( L"M3", *tracker );

	auto trackerEvents2 = tracker->GetTrainEvents();

	const auto & tcont = tracker->GetTrainContainer();
	const auto & tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 && tunities.back()->IsTrain() );

	chrono::minutes standingDurMinutes( 2 );
	chrono::milliseconds standingDurMillis = chrono::duration_cast<chrono::milliseconds>( standingDurMinutes );
	TI.IncreaseTime( static_cast<unsigned int>( standingDurMillis.count() ) );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	vector<TrackerEventPtr> eventVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( eventVec[0]->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( eventVec[1]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( eventVec[2]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::EntStopOnMultiStripSABWay()
{
	TI.DetermineHeadClose( L"Head310", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripFree( L"F3", *tracker );
	TI.DetermineStripFree( L"X3", *tracker );

	const auto & tcont = tracker->GetTrainContainer();
	const auto & tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 && tunities.back()->IsTrain() );

	chrono::minutes standingDurMinutes( 2 );
	chrono::milliseconds standingDurMillis = chrono::duration_cast<chrono::milliseconds>( standingDurMinutes );
	TI.IncreaseTime( static_cast<unsigned int>( standingDurMillis.count() ) );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	vector<TrackerEventPtr> eventVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( eventVec[0]->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( eventVec[1]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( eventVec[2]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( eventVec[3]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( eventVec[4]->GetCode() == HCode::ENTRANCE_STOPPING );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerEntranceStopping::EntStopOnSingleStripFABWay()
{
	TI.DetermineHeadClose( L"Head308", *tracker );
	TI.DetermineStripBusy( L"T3", *tracker );
	TI.DetermineStripBusy( L"U3", *tracker );
	TI.DetermineStripFree( L"T3", *tracker );

	const auto & tcont = tracker->GetTrainContainer();
	const auto & tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 && tunities.back()->IsTrain() );

	chrono::minutes standingDurMinutes( 2 );
	chrono::milliseconds standingDurMillis = chrono::duration_cast<chrono::milliseconds>( standingDurMinutes );
	TI.IncreaseTime( static_cast<unsigned int>( standingDurMillis.count() ) );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	vector<TrackerEventPtr> eventVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( eventVec[0]->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( eventVec[1]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( eventVec[2]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( eventVec[3]->GetCode() == HCode::ENTRANCE_STOPPING );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}