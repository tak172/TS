#include "stdafx.h"
#include "TC_TrackerTrainRecognition.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainInfoPackage.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTrainRecognition );
void TC_TrackerTrainRecognition::setUp()
{
	TI.Reset();
}

void TC_TrackerTrainRecognition::RouteSetExit()
{
	TI.DetermineStripBusy( L"C", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	CPPUNIT_ASSERT( train->IsTrain() );
	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto train2 = tunities2.front();
	CPPUNIT_ASSERT( trainId == train2->GetId() );
	const auto & tplace = train2->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"C" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"F" ).bdg ) != tplace.cend() );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"C").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"F").bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::OpenAlienHeadExit()
{
	TI.DetermineStripBusy( L"C", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	SetRandomOddInfo( train );
	auto trainId = train->GetId();
	TI.DetermineHeadOpen( L"Head102", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() );
	auto trainIt = find_if( tunities2.cbegin(), tunities2.cend(), []( TrainUnityCPtr tptr ){
		return tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( trainIt != tunities2.cend() );
	const auto & tplace = ( *trainIt )->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 );
	CPPUNIT_ASSERT( tplace.front() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( ( *trainIt )->GetId() == trainId );
	auto shuntIt = find_if( tunities2.cbegin(), tunities2.cend(), []( TrainUnityCPtr tptr ){
		return !tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( shuntIt != tunities2.cend() );
	const auto & tplace2 = ( *shuntIt )->GetPlace();
	CPPUNIT_ASSERT( tplace2.size() == 1 );
	CPPUNIT_ASSERT( tplace2.front() == TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( ( *shuntIt )->GetId() != trainId );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"C").bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::CloseHeadExit()
{
	TI.DetermineStripBusy( L"C", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.front();
	auto tunityId = tunity->GetId();
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto tunity2 = tunities2.front();
	CPPUNIT_ASSERT( tunity2->GetId() == tunityId );
	const auto & tplace = tunity2->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"C" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"F" ).bdg ) != tplace.cend() );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::FromStationToSpan()
{
	TI.DetermineStripBusy( L"G", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train = trainList.front();
	CPPUNIT_ASSERT( !train->IsTrain() );
	TI.DetermineStripBusy( L"X", *tracker );
	CPPUNIT_ASSERT( train->IsTrain() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"G").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"X").bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::SpanMove()
{
	TI.DetermineStripBusy( L"X3", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train = trainList.front();
	CPPUNIT_ASSERT( !train->IsTrain() );
	TI.DetermineStripBusy( L"Y3", *tracker );
	CPPUNIT_ASSERT( train->IsTrain() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"X3").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"Y3").bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::FormByInfoEvent()
{
	TI.DetermineStripBusy( L"D", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train = trainList.front();
	CPPUNIT_ASSERT( !train->IsTrain() );

	SetInfo( train, RandomTrainDescr() );

	CPPUNIT_ASSERT( train->IsTrain() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"D").bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::MoveTrainStationToStation()
{
	TI.DetermineStripBusy( L"ST1", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = *trains.cbegin();
	SetRandomOddInfo( train );
	CPPUNIT_ASSERT( train->GetDescrPtr() );
	auto tdescr = *train->GetDescrPtr();
	auto trainId = train->GetId();
	TI.DetermineStripBusy( L"ST2", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtunity = *trains.cbegin();
	CPPUNIT_ASSERT( curtunity->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( curtunity->IsTrain() );
	CPPUNIT_ASSERT( trainId == curtunity->GetId() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::UpgradeNoTrainWhileGoToAlienStation()
{
	TI.DetermineStripBusy( L"ST1", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = *trains.cbegin();
	auto trainId = train->GetId();
	CPPUNIT_ASSERT( !train->IsTrain() );
	TI.DetermineStripBusy( L"ST2", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtrain = *trains.cbegin();
	auto curtrainId = curtrain->GetId();
	CPPUNIT_ASSERT( curtrain->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( curtrain->IsTrain() );
	CPPUNIT_ASSERT( trainId == curtrainId );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::NoUpgradeNoTrainWhileGoToFriendStation()
{
	TI.DetermineStripBusy( L"AO5", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = *trains.cbegin();
	auto trainId = train->GetId();
	CPPUNIT_ASSERT( !train->IsTrain() );
	TI.DetermineStripBusy( L"AP5", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtrain = *trains.cbegin();
	auto curtrainId = curtrain->GetId();
	CPPUNIT_ASSERT( curtrain->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( !curtrain->IsTrain() );
	CPPUNIT_ASSERT( trainId == curtrainId );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::NoFormByTrainRoute()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );
	TI.DetermineRouteSet( L"Head101->A", *tracker );
	auto tunity = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tunity && !tunity->IsTrain() );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.empty() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::NoFormByTrainRoute2()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	auto tunity = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tunity && !tunity->IsTrain() );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.empty() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::NoFormByShuntingRoute()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"R4" ) );
	TI.DetermineRouteSet( L"Head403S->A4", *tracker );
	auto tunity = trainCont->GetUnity( TI.Get( L"R4" ).bdg );
	CPPUNIT_ASSERT( tunity && !tunity->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainRecognition::NoFormByOnlyTrainHead()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"R4" ) );
	TI.DetermineHeadOpen( L"Head403T", *tracker );
	auto tunity = trainCont->GetUnity( TI.Get( L"R4" ).bdg );
	CPPUNIT_ASSERT( tunity && !tunity->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}