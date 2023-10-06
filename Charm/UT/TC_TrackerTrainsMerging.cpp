#include "stdafx.h"
#include "TC_TrackerTrainsMerging.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTrainsMerging );
void TC_TrackerTrainsMerging::setUp()
{
	TI.Reset();
}

void TC_TrackerTrainsMerging::MergingTrainWithNearUnity()
{
	TI.DetermineSwitchMinus( L"24", *tracker );
	TI.DetermineStripBusy( L"G2", *tracker ); //поезд
	TI.DetermineStripBusy( L"B2", *tracker );
	TI.DetermineStripFree( L"G2", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto initId = train->GetId();
	SetRandomOddInfo( train );
	TI.DetermineStripBusy( L"A2", *tracker, false ); //поезд
	TI.DetermineStripBusy( L"H2", *tracker, false ); //ПЕ
	TI.DetermineStripBusy( L"G2", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"A2", *tracker );

	TI.DetermineStripFree( L"B2", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	train = trains.front();
	CPPUNIT_ASSERT( train->GetId() == initId );
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"G2" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"H2" ).bdg ) != tplace.cend() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::MergeTrainWithUnityOnSpan()
{
	//создание поезда и не поезда
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"K", *tracker );
	auto trains = trainCont->GetUnities();
	TrainUnityCPtr train, noTrain;
	for ( auto tptr : trains )
	{
		if ( tptr->IsTrain() )
			train = tptr;
		else
			noTrain = tptr;
	}
	CPPUNIT_ASSERT( train->IsTrain() );
	CPPUNIT_ASSERT( train->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( train->Have( TI.Get( L"H" ).bdg ) );
	CPPUNIT_ASSERT( train->Have( TI.Get( L"K" ).bdg ) );
	CPPUNIT_ASSERT( !noTrain->IsTrain() && noTrain->GetPlace().size() == 1 );
	CPPUNIT_ASSERT( noTrain->Have( TI.Get( L"Q" ).bdg ) );

	//гашение не поезда. должен остаться один исходный поезд, а новая ПЕ должна пропасть
	TI.DetermineStripFree( L"Q", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto nowUnity = trains.front();
	CPPUNIT_ASSERT( nowUnity->GetId() == train->GetId() && nowUnity->IsTrain() );
	CPPUNIT_ASSERT( nowUnity->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( nowUnity->Have( TI.Get( L"H" ).bdg ) );
	CPPUNIT_ASSERT( nowUnity->Have( TI.Get( L"K" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::MergeTrainWithUnityOnSema()
{
	//на W - поезд, на A - маневровая
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	unsigned int trainId = 0;
	auto trains = trainCont->GetUnities();
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		if ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"W" ).bdg ) != tplace.cend() )
		{
			SetRandomOddInfo( train );
			trainId = train->GetId();
		}
	}
	CPPUNIT_ASSERT( trainId != 0 );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( ( *trains.cbegin() )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::TrainAbsorptionOfNotrain()
{
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() );
	auto train = tunities.back();
	auto trainId = train->GetId();
	SetRandomEvenInfo( tunities.back() );

	TI.DetermineStripBusy( L"H4", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker, false );
	TI.DetermineStripBusy( L"G4", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto trainIt = find_if( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		return tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( trainIt != tunities.cend() );
	
	const auto & tplace = ( *trainIt )->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 && tplace.front() == TI.Get( L"S4" ).bdg );

	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.DetermineStripFree( L"F4", *tracker, false );
	TI.DetermineStripFree( L"H4", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainIt3 = find_if( tunities.cbegin(), tunities.cend(), [trainId]( TrainUnityCPtr tptr ){
		return tptr->GetId() == trainId;
	} );
	CPPUNIT_ASSERT( trainIt3 != tunities.cend() );
	auto train3 = *trainIt3;
	CPPUNIT_ASSERT( train3->GetPlace().size() == 1 );
	CPPUNIT_ASSERT( train3->GetPlace().front() == TI.Get( L"S4" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::TrainAbsorptionOfNotrain2()
{
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	auto trainId = train->GetId();
	SetRandomOddInfo( tunities.back() );

	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripBusy( L"O4", *tracker );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto trainIt = find_if( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		return tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( trainIt != tunities.cend() );

	const auto & tplace = ( *trainIt )->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 && tplace.front() == TI.Get( L"R4" ).bdg );

	TI.DetermineStripFree( L"A4", *tracker, false );
	TI.DetermineStripFree( L"O4", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainIt3 = find_if( tunities.cbegin(), tunities.cend(), [trainId]( TrainUnityCPtr tptr ){
		return tptr->GetId() == trainId;
	} );
	CPPUNIT_ASSERT( trainIt3 != tunities.cend() );
	auto train3 = *trainIt3;
	CPPUNIT_ASSERT( train3->GetPlace().size() == 1 );
	CPPUNIT_ASSERT( train3->GetPlace().front() == TI.Get( L"R4" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::TrainAbsorptionOfNotrain3()
{
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );

	TI.DetermineStripBusy( L"S4", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker );
	auto trainPtr = SetRandomEvenInfo( TI.Get( L"S4" ).bdg );
	auto trainId = trainPtr->GetId();

	TI.DetermineStripBusy( L"G4", *tracker, false );
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.DetermineStripBusy( L"K4", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	trainPtr = trainCont->GetUnity( TI.Get( L"S4" ).bdg );
	CPPUNIT_ASSERT( trainPtr->IsTrain() && trainPtr->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( trainPtr->Have( TI.Get( L"S4" ).bdg ) && trainPtr->Have( TI.Get( L"F4" ).bdg ) );
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"G4" ).bdg );
	CPPUNIT_ASSERT( !notrainPtr->IsTrain() && notrainPtr->GetPlace().size() == 3 );
	
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineStripBusy( L"I4", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	CPPUNIT_ASSERT( train->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::NotrainEmbeddingIntoTrain()
{
	TI.DetermineStripBusy( L"B", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomEvenInfo( tunities.back() );
	auto trainIden = tunities.back()->GetId();
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.DetermineStripFree( L"F", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.back()->GetId() == trainIden );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::NotrainEmbeddingIntoTrain2()
{
	TI.DetermineStripBusy( L"AB4", *tracker );
	TI.DetermineStripBusy( L"AD4", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto ad4trainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		return tptr->Have( TI.Get( L"AD4" ).bdg );
	} );
	CPPUNIT_ASSERT( ad4trainIt != tunities.cend() );
	auto trainId = ( *ad4trainIt )->GetId();
	TI.DetermineStripBusy( L"AE4", *tracker );
	TI.DetermineStripBusy( L"AF4", *tracker );
	tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"AB4", *tracker );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"AD4", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.back()->GetId() == trainId );
	CPPUNIT_ASSERT( tunities.back()->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::NotrainEmbeddingIntoTrain3()
{
	//не делаем еще один поезд при подъезде маневровой к уже существующему поезду
	TI.DetermineStripBusy( L"W", *tracker ); //поезд
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	CPPUNIT_ASSERT( train->IsTrain() );
	auto trainId = train->GetId();
	TI.DetermineStripBusy( L"H", *tracker ); //сюда перебрасывается поезд
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.back()->IsTrain() && tunities.back()->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::NotrainEmbeddingIntoTrain4()
{
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );

	//порядок зажигания участков критичен, т.к. необходимо разместить две маневровые и 
	//поезд со строго определенным распределением идентификаторов

	TI.DetermineStripBusy( L"G", *tracker );
	SetRandomEvenInfo( TI.Get( L"G" ).bdg );
	TI.DetermineStripBusy( L"W", *tracker );
	SetRandomEvenInfo( TI.Get( L"W" ).bdg );
	TI.DetermineStripBusy( L"D", *tracker );
	SetRandomEvenInfo( TI.Get( L"D" ).bdg );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"X", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"E", *tracker, false );
	TI.DetermineStripBusy( L"I", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 3 );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		return tptr->Have( TI.Get( L"W" ).bdg ) && tptr->Have( TI.Get( L"Q" ).bdg ) && tptr->Have( TI.Get( L"A" ).bdg );
	} ) 
		);
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		return tptr->Have( TI.Get( L"D" ).bdg ) && tptr->Have( TI.Get( L"E" ).bdg );
	} ) 
		);
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		return tptr->Have( TI.Get( L"G" ).bdg ) && tptr->Have( TI.Get( L"X" ).bdg ) && tptr->Have( TI.Get( L"I" ).bdg );
	} ) 
		);

	for ( auto tunity : tunities )
		DisformTrain( tunity->GetId() );

	auto gtrainPtr = SetRandomOddInfo( TI.Get( L"G" ).bdg );
	auto gtrainIden = gtrainPtr->GetId();

	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.DetermineStripFree( L"I", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	auto trainIt = find_if( tunities.cbegin(), tunities.cend(), [gtrainIden]( TrainUnityCPtr tptr ){
		return tptr->GetId() == gtrainIden;
	} );
	CPPUNIT_ASSERT( trainIt != tunities.cend() );
	auto trainPtr = *trainIt;
	CPPUNIT_ASSERT( trainPtr->Have( TI.Get( L"X" ).bdg ) );
	CPPUNIT_ASSERT( trainPtr->Have( TI.Get( L"G" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsMerging::NoMergingWithLeftTrain()
{
	TI.DetermineSwitchMinus( L"23", *tracker );
	TI.DetermineSwitchMinus( L"22", *tracker );
	TI.DetermineSwitchMinus( L"24", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"K2" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"E2" ) ) );
	SetInfo( TI.Get( L"E2" ).bdg, TrainDescr() );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"K2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.DetermineStripBusy( L"G2", *tracker, false );
	TI.DetermineStripBusy( L"B2", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}