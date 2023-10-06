#include "stdafx.h"
#include "TC_TrackerTrainsMigration.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTrainsMigration );

void TC_TrackerTrainsMigration::setUp()
{
	TI.Reset();
}

void TC_TrackerTrainsMigration::BidMigrationThrouBadDirectedSwitch()
{
	TI.DetermineStripBusy( L"A4", *tracker );
	SetRandomOddInfo( TI.Get( L"A4" ).bdg );
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"C4", *tracker );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMigration::BidMigrationOverOther()
{
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineStripBusy( L"E4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );
	auto d4TrainPtr = SetRandomOddInfo( TI.Get( L"D4" ).bdg );
	CPPUNIT_ASSERT( d4TrainPtr );
	auto d4Iden = d4TrainPtr->GetId();

	auto tunities = trainCont->GetUnities();
	TI.DetermineSwitchMinus( L"43", *tracker, false );
	TI.DetermineSwitchPlus( L"44", *tracker, false );
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 3 );
	bool trainStayed = any_of( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 2 && tptr->Have( TI.Get( L"D4" ).bdg ) && tptr->Have( TI.Get( L"E4" ).bdg );
	} );
	CPPUNIT_ASSERT( trainStayed );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMigration::MigrationVsOccupation()
{
	TI.DetermineSwitchMinus( L"17", *tracker );
	TI.DetermineStripBusy( L"V", *tracker ); //маневровая
	TI.DetermineStripBusy( L"Z", *tracker );
	TI.DetermineStripBusy( L"Y", *tracker, false );
	TI.DetermineStripFree( L"Z", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"Z", *tracker );
	TI.DetermineStripBusy( L"AA", *tracker );
	auto ytrainPtr = SetRandomEvenInfo( TI.Get( L"Y" ).bdg );
	auto ytrainId = ytrainPtr->GetId();

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"R", *tracker, false );
	TI.DetermineStripFree( L"V", *tracker, false );
	TI.DetermineStripBusy( L"AB", *tracker, false );
	TI.DetermineStripBusy( L"AC", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	auto ytrainIt = find_if( tunities.cbegin(), tunities.cend(), [ytrainId]( TrainUnityCPtr tptr ){
		return tptr->GetId() == ytrainId;
	} );
	CPPUNIT_ASSERT( ytrainIt != tunities.cend() && ( *ytrainIt )->IsTrain() );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMigration::BidJumpToSpan()
{
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchPlus( L"11", *tracker );

	list<wstring> tplace;
	tplace.push_back( L"E" );
	tplace.push_back( L"D" );
	tplace.push_back( L"A" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace );
	auto tunities = trainCont->GetUnities();
	auto trainPtr = tunities.front();
	auto trainId = trainPtr->GetId();

	LocateUnityByForce( nullptr, list<wstring>( 1, L"W" ) );

	TI.DetermineStripFree( L"E", *tracker );
	auto trainPtr2 = trainCont->GetUnity( trainId );
	CPPUNIT_ASSERT( trainPtr2->Have( TI.Get( L"A" ).bdg ) );
	CPPUNIT_ASSERT( trainPtr2->Have( TI.Get( L"D" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}