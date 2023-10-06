#pragma once

#include <ostream>
#include "../Guess/Tracker.h"
#include "GuessNsiBasis.h"

class TestTracker : protected Tracker
{
public:
	TestTracker( TrainFieldCPtr, const TrackingConfiguration &, GuessNsiBasisCPtr nsiBasisPtr = GuessNsiBasisCPtr( new TestGuessNsiBasis() ) );
	using Tracker::GetTrainContainer;
	using Tracker::GetChainContainer;
    using Tracker::GetTimeConstraints;
	using Tracker::GetTrackingConfiguration;
	using Tracker::Reset;
	using Tracker::TakeChanges;
	using Tracker::CaptureTrainsInfo;
	using Tracker::TakeEvents;
	using Tracker::Track;
	using Tracker::SetTime;
	using Tracker::GetLastTime;
	using Tracker::CheckIntegrity;
	using Tracker::GetLosKeeper;
	using Tracker::GetTrackTime;

	std::list <TrackerEventPtr> GetTrainEvents() const;
	std::list <TrackerEventPtr> GetTrainEvents( unsigned int trainId ) const;
	std::list <TrackerEventPtr> GetTerraEvents() const;
	TrainFieldCPtr GetTrainField() const { return tfieldPtr; }
	ChangesUnion TakeTrainChanges();
	std::string SetTrainsInfo( TrainInfoPackage && ); //результат - информация для логирования
	std::string SetTrainsInfo( std::vector<TrainInfoPackage> && ); //результат - информация для логирования

private:
	static bool IsTrainEventCode( HCode );
	static bool IsTerraEventCode( HCode );
	TrainFieldCPtr tfieldPtr;
};

std::ostream& operator <<(std::ostream& stream, const BadgeE& badge);