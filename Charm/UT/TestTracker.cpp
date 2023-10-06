#include "stdafx.h"
#include "TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/ChainContainer.h"
#include "../Guess/TrainUnity.h"
#include "../Guess/Chain.h"
#include "../Guess/TrainInfoPackage.h"
#include "GuessNsiBasis.h"

using namespace std;

std::ostream& operator <<(std::ostream& stream, const BadgeE& badge)
{
	stream << "Бейдж: " << badge.to_string();
	return stream;
}

TestTracker::TestTracker( TrainFieldCPtr tf, const TrackingConfiguration & trackConf, shared_ptr<const TestGuessNsiBasis> nsiBasisPtr ) : 
	Tracker( tf, trackConf, nsiBasisPtr ), tfieldPtr( tf )
{
	GetTimeConstraints().cargoTransArrivDurMs = 2000;
	GetTimeConstraints().LPFThresholdSec = 0;
}

list <TrackerEventPtr> TestTracker::GetTrainEvents() const
{
	auto tevents = GetEvents();
	tevents.remove_if( []( TrackerEventPtr tevent ){
		return !IsTrainEventCode( tevent->GetCode() );
	} );
	return tevents;
}

list<TrackerEventPtr> TestTracker::GetTrainEvents( unsigned int trainId ) const
{
	auto tevents = GetTrainEvents();
	tevents.remove_if( [trainId]( TrackerEventPtr tevent ){
		return tevent->GetId() != trainId;
	} );
	return tevents;
}

list <TrackerEventPtr> TestTracker::GetTerraEvents() const
{
	auto tevents = GetEvents();
	tevents.remove_if( []( TrackerEventPtr tevent ){
		return !IsTerraEventCode( tevent->GetCode() );
	} );
	return tevents;
}

bool TestTracker::IsTrainEventCode( HCode hcode )
{
	return ( hcode == HCode::FORM || hcode == HCode::DISFORM || hcode == HCode::DEATH || hcode == HCode::ARRIVAL ||
		hcode == HCode::DEPARTURE || hcode == HCode::TRANSITION || hcode == HCode::SPAN_MOVE || hcode == HCode::INFO_CHANGING ||
		hcode == HCode::ENTRANCE_STOPPING || hcode == HCode::WRONG_SPANWAY || hcode == HCode::TOKEN_SPANWAY || hcode == HCode::POCKET_ENTRY ||
		hcode == HCode::POCKET_EXIT || hcode == HCode::STATION_ENTRY || hcode == HCode::BANNED_ENTRY || hcode == HCode::STATION_EXIT || hcode == HCode::SPAN_STOPPING_BEGIN ||
		hcode == HCode::SPAN_STOPPING_END );
}

bool TestTracker::IsTerraEventCode( HCode hcode )
{
	return ( hcode == HCode::WAY_RED || hcode == HCode::WAY_BLACK );
}

ChangesUnion TestTracker::TakeTrainChanges()
{
	ChangesUnion changes;
	TakeChanges( changes );
	changes.placeViewChanges.erase( remove_if( changes.placeViewChanges.begin(), changes.placeViewChanges.end(), []( const PlaceViewChange & pvchange ){
		return !pvchange.tdescrPtr;
	} ), changes.placeViewChanges.cend() );
	return changes;
}

string TestTracker::SetTrainsInfo( TrainInfoPackage && tinfopackage )
{
	return SetTrainsInfo( vector<TrainInfoPackage>( 1, tinfopackage ) );
}

string TestTracker::SetTrainsInfo( std::vector<TrainInfoPackage> && infoPackages )
{
	return CaptureTrainsInfo( move( infoPackages ) );
}