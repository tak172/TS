#include "stdafx.h"
#include "TC_TrackerRecovery.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/FieldGraph.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerRecovery );
void TC_TrackerRecovery::setUp()
{
	TI.Reset();
	tracker->Reset();
}

void TC_TrackerRecovery::DiscordantMoveRecovery()
{
	TI.DetermineStripBusy( L"F3", *tracker );
	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	SetRandomOddInfo( trainList.front() );
	unsigned int trId = trainList.back()->GetId();
	TI.DetermineStripFree( L"F3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker ); //это тот же поезд
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	CPPUNIT_ASSERT( trainList.back()->GetId() == trId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::BoundDissapearanceUnrecovery()
{
	//пропадание поезда на границе полигона отслеживания
	TI.DetermineStripBusy( L"U3", *tracker );
	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	SetRandomOddInfo( trainList.front() );
	unsigned int disapptrId = trainList.back()->GetId();
	TI.DetermineStripFree( L"U3", *tracker );

	//возникновение поезда
	TI.DetermineStripBusy( L"M3", *tracker );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	CPPUNIT_ASSERT( trainList.back()->GetId() != disapptrId ); //здесь поезд не восстанавливается
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryCleanAfterSetInfo()
{
	//пропадание на F3 некого поезда
	TI.DetermineStripBusy( L"F3", *tracker );
	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	SetRandomOddInfo( trainList.front() );
	auto f3train = trainList.back();
	ConstTrainDescrPtr tdptr = f3train->GetDescrPtr();
	CPPUNIT_ASSERT( tdptr );
	TI.DetermineStripFree( L"F3", *tracker );

	//появление другой ПЕ (в удалении от F3)
	TI.DetermineStripBusy( L"G3", *tracker );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto g3train = trainList.back();
	SetInfo( g3train, *tdptr );

	//возникновение поезда на X3.
	//восстановления информации быть не может, т.к. она уже выдана другому поезду
	TI.DetermineStripBusy( L"X3", *tracker );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	for ( auto train : trainList )
	{
		auto tdescr = train->GetDescrPtr();
		if ( train == g3train )
			CPPUNIT_ASSERT( tdescr && *tdescr == *tdptr );
		else
			CPPUNIT_ASSERT( !tdescr );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::SamePlaceRecovery()
{
	//моргание участка с поездом
	TI.DetermineStripBusy( L"C", *tracker );
	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	SetRandomOddInfo( trainList.front() );
	unsigned int disapptrId = trainList.back()->GetId();
	TI.DetermineStripFree( L"C", *tracker );

	//восстановление поезда
	TI.DetermineStripBusy( L"C", *tracker );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	CPPUNIT_ASSERT( trainList.back()->GetId() == disapptrId ); //здесь поезд восстанавливается
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::DiscordantJumpRecovery()
{
	TI.DetermineSwitchPlus( L"32", *tracker );
	TI.DetermineSwitchPlus( L"31", *tracker );

	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripFree( L"Y3", *tracker );
	TI.DetermineStripFree( L"X3", *tracker );
	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto tunity = trainList.front();
	CPPUNIT_ASSERT( tunity->IsTrain() );
	unsigned int disapptrId = tunity->GetId();

	TI.DetermineStripFree( L"F3", *tracker ); //пропадание поезда
	TI.DetermineStripBusy( L"C3", *tracker ); //здесь поезд восстанавливается
	TI.DetermineStripBusy( L"D3", *tracker );
	
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	CPPUNIT_ASSERT( trainList.back()->GetId() == disapptrId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::StripJump()
{
	//например, подобная ситуация возникает на перегоне Nerza-Zulupe
	//при следующем движении: NAP -> N1P_ZLP -> N3P -> N10P -> N8P
	TI.DetermineStripBusy( L"K", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripFree( L"K", *tracker ); //H
	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto tunity = trainList.front();
	CPPUNIT_ASSERT( tunity->IsTrain() );
	unsigned int trainId = tunity->GetId();

	TI.DetermineStripBusy( L"W", *tracker ); //H-W
	TI.DetermineStripFree( L"W", *tracker ); //H
	TI.DetermineStripBusy( L"Q", *tracker ); //H and Q
	TI.DetermineStripFree( L"H", *tracker ); //Q

	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	tunity = trainList.front();
	CPPUNIT_ASSERT( tunity->IsTrain() );
	CPPUNIT_ASSERT( tunity->GetId() == trainId );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt )->GetBadge() == TI.Get( L"K" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt )->GetBadge() == TI.Get( L"H" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt )->GetBadge() == TI.Get( L"W" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt )->GetBadge() == TI.Get( L"H" ).bdg ); //из-за "отката" назад
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt )->GetBadge() == TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryForbidness()
{
	TrackingConfiguration trackingConfig;
	trackingConfig.noMemoryStrips.insert( TI.Get( L"W" ).bdg );
	TestTracker tstTracker( TI.TField(), trackingConfig );
	tstTracker.Reset();
	const auto & trainCont = tstTracker.GetTrainContainer();
	TI.DetermineStripBusy( L"K", tstTracker );
	auto trainList = trainCont.GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto tunity = trainList.front();
	auto initId = tunity->GetId();
	TI.DetermineStripBusy( L"H", tstTracker );
	TI.DetermineStripBusy( L"W", tstTracker );
	TI.DetermineStripFree( L"K", tstTracker );
	TI.DetermineStripFree( L"H", tstTracker );
	TI.DetermineStripFree( L"W", tstTracker ); //пропадание
	TI.DetermineStripBusy( L"W", tstTracker ); //восстановление
	auto trainList2 = trainCont.GetUnities();
	CPPUNIT_ASSERT( trainList2.size() == 1 );
	auto tunity2 = trainList2.front();
	CPPUNIT_ASSERT( tunity2->GetId() != initId );
	CPPUNIT_ASSERT( tstTracker.CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveringInsteadMoving()
{
	//создаем два нечетных поезда
	TI.DetermineStripBusy( L"A", *tracker );
	wstring warr[] = { L"A", L"Q", L"W", L"H" };
	TI.ImitateMotion( vector<wstring>( begin( warr ), end( warr ) ), *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	wstring warr2[] = { L"A", L"Q" };
	TI.ImitateMotion( vector<wstring>( begin( warr2 ), end( warr2 ) ), *tracker );
	auto tunities = trainCont->GetUnities();
	TrainUnityCPtr frontTrain, backTrain;
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"H" ).bdg )
			frontTrain = tunity;
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"Q" ).bdg );
			backTrain = tunity;
		}
	}
	SetRandomOddInfo( frontTrain );
	SetRandomOddInfo( backTrain );
	CPPUNIT_ASSERT( frontTrain->IsTrain() && backTrain->IsTrain() );
	unsigned int frontTrainId = frontTrain->GetId();
	TrainDescr frontTrainDescr = *frontTrain->GetDescrPtr();
	unsigned int backTrainId = backTrain->GetId();
	TrainDescr backTrainDescr = *backTrain->GetDescrPtr();

	//придвигаем вплотную
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );

	//моргание заднего участка не должно приводить к пропаданию ни одного из поездов (задний поезд исчезает и восстанавливается)
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	CPPUNIT_ASSERT( find_if( tunities.cbegin(), tunities.cend(), [frontTrainId]( TrainUnityCPtr tptr ){ return tptr->GetId() == frontTrainId; } ) != tunities.cend() );
	CPPUNIT_ASSERT( find_if( tunities.cbegin(), tunities.cend(), [backTrainId]( TrainUnityCPtr tptr ){ return tptr->GetId() == backTrainId; } ) != tunities.cend() );
	for ( auto tunity : tunities )
	{
		CPPUNIT_ASSERT( tunity->IsTrain() );
		auto trainId = tunity->GetId();
		auto tdescrPtr = tunity->GetDescrPtr();
		CPPUNIT_ASSERT( tdescrPtr );
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( trainId == frontTrainId )
		{
			CPPUNIT_ASSERT( *tdescrPtr == frontTrainDescr );
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"H" ).bdg );
		}
		else
		{
			CPPUNIT_ASSERT( *tdescrPtr == backTrainDescr );
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"W" ).bdg );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveringInsteadMoving2()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );

	//запоминание исчезнувшего поезда
	TI.DetermineStripBusy( L"E", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomOddInfo( tunities.back() );
	auto recovTrainId = tunities.back()->GetId();
	TI.DetermineStripFree( L"E", *tracker, false );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.FlushData( *tracker, false );

	LocateUnityByForce( nullptr, list<wstring>( 1, L"Q" ) );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto QTrainId = tunities.back()->GetId();
	CPPUNIT_ASSERT( QTrainId != recovTrainId );

	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"D", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainPtr = tunities.back();
	CPPUNIT_ASSERT( trainPtr->GetId() == recovTrainId ); //маневровая не сохраняется
	CPPUNIT_ASSERT( trainPtr->Have( TI.Get( L"Q" ).bdg ) );
	CPPUNIT_ASSERT( trainPtr->Have( TI.Get( L"A" ).bdg ) );
	CPPUNIT_ASSERT( trainPtr->Have( TI.Get( L"D" ).bdg ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveringInsteadMoving3()
{
	//создаем номерной и безномерной поезд
	TI.DetermineStripBusy( L"A", *tracker );
	wstring warr[] = { L"A", L"Q", L"W", L"H", L"K" };
	TI.ImitateMotion( vector<wstring>( begin( warr ), end( warr ) ), *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	wstring warr2[] = { L"A", L"Q", L"W" };
	TI.ImitateMotion( vector<wstring>( begin( warr2 ), end( warr2 ) ), *tracker );
	auto tunities = trainCont->GetUnities();
	TrainUnityCPtr numTrain, nonumTrain;
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"W" ).bdg )
			nonumTrain = tunity;
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"K" ).bdg );
			numTrain = tunity;
		}
	}
	SetRandomOddInfo( numTrain );
	SetInfo( nonumTrain, TrainDescr() );
	//придвигаем вплотную
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.DetermineStripFree( L"K", *tracker, false );
	TI.FlushData( *tracker, false );
	CPPUNIT_ASSERT( numTrain->IsTrain() && nonumTrain->IsTrain() );
	unsigned int nonumTrainId = nonumTrain->GetId();
	TrainDescr nonumTrainDescr = *nonumTrain->GetDescrPtr();
	unsigned int numTrainId = numTrain->GetId();
	TrainDescr numTrainDescr = *numTrain->GetDescrPtr();

	//моргание заднего участка не должно приводить к пропаданию ни одного из поездов (задний поезд исчезает и восстанавливается)
	TI.DetermineStripFree( L"H", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.back()->GetId() == nonumTrainId );
	TI.DetermineStripBusy( L"H", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	CPPUNIT_ASSERT( find_if( tunities.cbegin(), tunities.cend(), [numTrainId]( TrainUnityCPtr tptr ){ return tptr->GetId() == numTrainId; } ) != tunities.cend() );
	CPPUNIT_ASSERT( find_if( tunities.cbegin(), tunities.cend(), [nonumTrainId]( TrainUnityCPtr tptr ){ return tptr->GetId() == nonumTrainId; } ) != tunities.cend() );
	for ( auto tunity : tunities )
	{
		CPPUNIT_ASSERT( tunity->IsTrain() );
		auto trainId = tunity->GetId();
		auto tdescrPtr = tunity->GetDescrPtr();
		CPPUNIT_ASSERT( tdescrPtr );
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( trainId == numTrainId )
		{
			CPPUNIT_ASSERT( *tdescrPtr == numTrainDescr );
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"H" ).bdg );
		}
		else
		{
			CPPUNIT_ASSERT( *tdescrPtr == nonumTrainDescr );
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"W" ).bdg );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveringAndCountraryMovingOnSingleStrip()
{
	//создаем два поезда и придвигаем вплотную
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"K", *tracker );
	SetRandomOddInfo( TI.Get( L"Q" ).bdg );
	SetRandomOddInfo( TI.Get( L"K" ).bdg );
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.DetermineStripFree( L"K", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	TrainUnityCPtr wTrain, hTrain;
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"W" ).bdg )
			wTrain = tunity;
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"H" ).bdg );
			hTrain = tunity;
		}
	}

	//пропадание одного из поездов
	TI.DetermineStripFree( L"W", *tracker );

	tunities = trainCont->GetUnities();

	//мгоновенный переезд другого поезда на место исчезнувшего
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"H", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::SameTrainFarPlacesRecovering()
{
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );

	TI.DetermineSwitchPlus( L"11", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tunities.empty() );
	auto recoTrainId = tunities.back()->GetId();

	TI.DetermineStripFree( L"H", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto aqTrainIt = find_if( tunities.cbegin(), tunities.cend(), [recoTrainId, this]( TrainUnityCPtr tunityPtr ){
		const auto & tplace = tunityPtr->GetPlace();
		bool aexists = find( tplace.cbegin(), tplace.cend(), TI.Get( L"A" ).bdg ) != tplace.cend();
		bool qexists = find( tplace.cbegin(), tplace.cend(), TI.Get( L"Q" ).bdg ) != tplace.cend();
		return tunityPtr->GetId() == recoTrainId && tplace.size() == 2 && aexists && qexists;
	} );
	auto hTrainIt = find_if( tunities.cbegin(), tunities.cend(), [recoTrainId, this]( TrainUnityCPtr tunityPtr ){
		const auto & tplace = tunityPtr->GetPlace();
		bool hexists = find( tplace.cbegin(), tplace.cend(), TI.Get( L"H" ).bdg ) != tplace.cend();
		return tunityPtr->GetId() != recoTrainId && tplace.size() == 1 && hexists;
	} );
	CPPUNIT_ASSERT( aqTrainIt != tunities.cend() );
	CPPUNIT_ASSERT( hTrainIt != tunities.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryOnThreeStrips()
{
	TI.DetermineStripBusy( L"A", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomOddInfo( tunities.back() );
	auto trainId = tunities.back()->GetId();
	auto tdescr = *tunities.back()->GetDescrPtr();
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 3 );
	CPPUNIT_ASSERT( curtrain->GetId() == trainId );
	CPPUNIT_ASSERT( curtrain->IsTrain() && *curtrain->GetDescrPtr() == tdescr );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryOverBlockedStrips()
{
	//поезд
	TI.DetermineStripBusy( L"R5", *tracker );
	wstring warr[] = { L"R5", L"Q5", L"P5", L"O5", L"N5", L"M5", L"L5" };
	TI.ImitateMotion( vector<wstring>( begin( warr ), end( warr ) ), *tracker );

	TI.DetermineStripBusy( L"M5", *tracker );
	TI.DetermineStripFree( L"L5", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.front()->GetId();

	//пропадание
	TI.DetermineStripFree( L"M5", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.empty() );

	//восстановление через блок-участок
	TI.DetermineStripBusy( L"P5", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	CPPUNIT_ASSERT( train->GetId() == trainId );
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"P5" ).bdg ) != tplace.cend() );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::InterferringOfEmptyInfoRecovering()
{
	//поезд
	TI.DetermineStripBusy( L"R5", *tracker );
	wstring warr[] = { L"R5", L"Q5", L"P5", L"O5", L"N5", L"M5", L"L5" };
	TI.ImitateMotion( vector<wstring>( begin( warr ), end( warr ) ), *tracker );
	TI.DetermineStripBusy( L"M5", *tracker );
	TI.DetermineStripFree( L"L5", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.front()->GetId();

	//пропадание
	TI.DetermineStripFree( L"M5", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.empty() );

	//пропадание другого поезда на другом полигоне
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripFree( L"H", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	//восстановление через блок-участок
	TI.DetermineStripBusy( L"P5", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	CPPUNIT_ASSERT( train->GetId() == trainId );
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"P5" ).bdg ) != tplace.cend() );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryThroughLive()
{
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	auto trains = trainCont->GetUnities();
	for_each( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr train ){
		SetRandomOddInfo( train );
	} );
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	auto backTrainIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr train ){
		const auto & tplace = train->GetPlace();
		return ( tplace.size() == 1 && tplace.front() == TI.Get( L"Q" ).bdg );
	} );
	CPPUNIT_ASSERT( backTrainIt != trains.cend() );
	auto frontTrainIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr train ){
		const auto & tplace = train->GetPlace();
		return ( tplace.size() == 1 && tplace.front() == TI.Get( L"W" ).bdg );
	} );
	CPPUNIT_ASSERT( frontTrainIt != trains.cend() );
	unsigned int backTrainId = ( *backTrainIt )->GetId();
	unsigned int frontTrainId = ( *frontTrainIt )->GetId();
	CPPUNIT_ASSERT( backTrainId != frontTrainId );

	//исчезновение заднего поезда и зажигание следующего участка далее по перегону
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	backTrainIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr train ){
		const auto & tplace = train->GetPlace();
		return ( tplace.size() == 1 && tplace.front() == TI.Get( L"W" ).bdg );
	} );
	CPPUNIT_ASSERT( backTrainIt != trains.cend() );
	frontTrainIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr train ){
		const auto & tplace = train->GetPlace();
		return ( tplace.size() == 1 && tplace.front() == TI.Get( L"H" ).bdg );
	} );
	CPPUNIT_ASSERT( frontTrainIt != trains.cend() );

	//порядок следования нарушаться не должен
	CPPUNIT_ASSERT( ( *backTrainIt )->GetId() == backTrainId );
	CPPUNIT_ASSERT( ( *frontTrainIt )->GetId() == frontTrainId );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::TopologicallyUnreachableRecover()
{
	TI.DetermineStripBusy( L"C", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tunities.empty() );
	SetRandomEvenInfo( tunities.back() );
	auto trainId = tunities.back()->GetId();
	CPPUNIT_ASSERT( tunities.back()->IsTrain() );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripFree( L"C", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripBusy( L"E", *tracker );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.back()->GetId() != trainId );
	CPPUNIT_ASSERT( !tunities.back()->IsTrain() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveringOnSinglePlaceUnity()
{
	TI.DetermineStripBusy( L"C", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tunities.empty() );
	SetRandomEvenInfo( tunities.back() );
	auto trainId = tunities.back()->GetId();
	CPPUNIT_ASSERT( tunities.back()->IsTrain() );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripFree( L"C", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryVersusOccupation()
{
	TI.DetermineStripBusy( L"F2", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tunities.empty() );
	SetRandomEvenInfo( tunities.back() );
	auto recovIden = tunities.back()->GetId();

	TI.DetermineStripFree( L"F2", *tracker ); //пропадание с запоминанем
	TI.DetermineStripBusy( L"A2", *tracker ); //новый поезд
	tunities = trainCont->GetUnities();
	auto notrainIden = tunities.back()->GetId();

	TI.DetermineStripBusy( L"B2", *tracker, false );
	TI.DetermineStripBusy( L"G2", *tracker, false );
	TI.DetermineStripBusy( L"H2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineSwitchPlus( L"23", *tracker, false );
	TI.DetermineSwitchMinus( L"22", *tracker, false );
	TI.DetermineSwitchMinus( L"24", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto recovtrain = tunities.back();
	CPPUNIT_ASSERT( recovtrain->GetId() == recovIden );
	const auto & tplace = recovtrain->GetPlace();
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"A2" ).bdg; } ) );
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"B2" ).bdg; } ) );
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"G2" ).bdg; } ) );
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"H2" ).bdg; } ) );
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"E2" ).bdg; } ) );
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"F2" ).bdg; } ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryVersusOccupation2()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );

	SetRandomOddInfo( TI.Get( L"A" ).bdg );
	SetRandomOddInfo( TI.Get( L"B" ).bdg );

	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	TI.FlushData( *tracker, false );

	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );

	auto tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.FlushData( *tracker, false ); //возможен конфликт между восстановлением и занятием

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::PermittedRecoveryOverSwitch()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	wstring stripsarr[] = { L"W", L"Q", L"A", L"D" };
	vector<wstring> stripsvec( begin( stripsarr ), end( stripsarr ) );
	TI.ImitateMotion( stripsvec, *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	auto initIden = train->GetId();

	TI.DetermineStripFree( L"D", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	tunities = trainCont->GetUnities();
	auto recovtrain = tunities.back();
	CPPUNIT_ASSERT( recovtrain->GetId() == initIden );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::ForbiddenRecoveryOverSwitch()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	wstring stripsarr[] = { L"W", L"Q", L"A", L"D" };
	vector<wstring> stripsvec( begin( stripsarr ), end( stripsarr ) );
	TI.ImitateMotion( stripsvec, *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	auto initIden = train->GetId();

	TI.DetermineStripFree( L"D", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tunities.empty() );
	auto newtunity = tunities.back();
	CPPUNIT_ASSERT( newtunity->GetId() != initIden );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::DeathVersusDisform()
{
	TI.DetermineStripBusy( L"A", *tracker );
	wstring stripsarr[] = { L"A", L"Q", L"W" };
	vector<wstring> stripsvec( begin( stripsarr ), end( stripsarr ) );
	TI.ImitateMotion( stripsvec, *tracker );
	TI.DetermineStripBusy( L"A", *tracker );

	auto tunities = trainCont->GetUnities();
	auto forwardTrainId = 0, backwardTrainId = 0;
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.back() == TI.Get( L"A" ).bdg )
		{
			SetInfo( tunity, TrainDescr(), tracker );
			backwardTrainId = tunity->GetId();
		}
		else
		{
			SetRandomOddInfo( tunity );
			forwardTrainId = tunity->GetId();
		}
	}
	CPPUNIT_ASSERT( forwardTrainId != 0 && backwardTrainId != 0 && forwardTrainId != backwardTrainId );
	
	//кратковременное исчезновение
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 ) ;
		if ( tplace.back() == TI.Get( L"A" ).bdg )
			CPPUNIT_ASSERT( tunity->GetId() == backwardTrainId );
		else
			CPPUNIT_ASSERT( tunity->GetId() == forwardTrainId );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::MultiRecovery()
{
	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripBusy( L"D2", *tracker, false );
	TI.DetermineStripBusy( L"P2", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trainPtr1 = SetRandomEvenInfo( TI.Get( L"F2" ).bdg );
	CPPUNIT_ASSERT( trainPtr1 );
	auto trainId1 = trainPtr1->GetId();
	auto trainPtr2 = SetRandomEvenInfo( TI.Get( L"D2" ).bdg );
	CPPUNIT_ASSERT( trainPtr2 );
	auto trainId2 = trainPtr2->GetId();
	auto trainPtr3 = SetRandomEvenInfo( TI.Get( L"P2" ).bdg );
	CPPUNIT_ASSERT( trainPtr3 );
	auto trainId3 = trainPtr3->GetId();

	TI.DetermineStripFree( L"F2", *tracker, false );
	TI.DetermineStripFree( L"P2", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripBusy( L"P2", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 3 );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId1]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId1; } ) );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId2]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId2; } ) );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId3]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId3; } ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::MultiRecoveryWithinOneChain()
{
	TI.DetermineStripBusy( L"A", *tracker );
	wstring stripsarr[] = { L"A", L"Q", L"W", L"H" };
	vector<wstring> stripsvec( begin( stripsarr ), end( stripsarr ) );
	TI.ImitateMotion( stripsvec, *tracker );

	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trainPtr1 = SetRandomEvenInfo( TI.Get( L"A" ).bdg );
	auto trainPtr2 = SetRandomEvenInfo( TI.Get( L"H" ).bdg );
	CPPUNIT_ASSERT( trainPtr1 && trainPtr2 );
	auto trainId1 = trainPtr1->GetId();
	auto trainId2 = trainPtr2->GetId();
	CPPUNIT_ASSERT( trainId1 != trainId2 );

	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.DetermineStripFree( L"H", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.FlushData( *tracker, false );

	//восстановление обоих поездов
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	CPPUNIT_ASSERT( count_if( tunities.cbegin(), tunities.cend(), [trainId1]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId1; } ) == 1 );
	CPPUNIT_ASSERT( count_if( tunities.cbegin(), tunities.cend(), [trainId2]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId2; } ) == 1 );

	TI.DetermineStripBusy( L"K", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::MultiRecoveryWithinOneChain2()
{
	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripBusy( L"D2", *tracker, false );
	TI.DetermineStripBusy( L"P2", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trainPtr1 = SetRandomEvenInfo( TI.Get( L"F2" ).bdg );
	CPPUNIT_ASSERT( trainPtr1 );
	auto trainId1 = trainPtr1->GetId();
	auto trainPtr2 = SetRandomEvenInfo( TI.Get( L"D2" ).bdg );
	CPPUNIT_ASSERT( trainPtr2 );
	auto trainId2 = trainPtr2->GetId();
	auto trainPtr3 = SetRandomEvenInfo( TI.Get( L"P2" ).bdg );
	CPPUNIT_ASSERT( trainPtr3 );
	auto trainId3 = trainPtr3->GetId();

	TI.DetermineStripFree( L"F2", *tracker, false );
	TI.DetermineStripFree( L"P2", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.DetermineStripBusy( L"D2", *tracker, false );
	TI.DetermineStripBusy( L"M2", *tracker, false );
	TI.DetermineStripBusy( L"O2", *tracker, false );
	TI.DetermineStripBusy( L"P2", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 3 );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId1]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId1; } ) );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId2]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId2; } ) );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId3]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId3; } ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::MultiRecoveryWithinOneChain3()
{
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	
	TI.DetermineStripBusy( L"D", *tracker, false );
	TI.DetermineStripBusy( L"E", *tracker, false );
	TI.FlushData( *tracker, false );
	auto eTrainPtr = SetRandomOddInfo( TI.Get( L"E" ).bdg );

	TI.DetermineStripBusy( L"G", *tracker );
	auto gTrainPtr = SetInfo( TI.Get( L"G" ).bdg, TrainDescr() );

	TI.DetermineStripFree( L"D", *tracker, false );
	TI.DetermineStripFree( L"E", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripFree( L"G", *tracker, false );
	TI.DetermineStripBusy( L"D", *tracker, false );
	TI.DetermineStripBusy( L"E", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineStripBusy( L"E", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );

	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::MultiRecoveryWithinOneChain4()
{
	TI.DetermineSwitchPlus( L"41", *tracker );

	TI.DetermineStripBusy( L"R4", *tracker, false );
	TI.DetermineStripBusy( L"S4", *tracker, false );
	TI.FlushData( *tracker, false );
	auto s4TrainPtr = SetRandomOddInfo( TI.Get( L"S4" ).bdg );
	CPPUNIT_ASSERT( s4TrainPtr );
	auto s4TrainId = s4TrainPtr->GetId();

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"O4" ) ) );
	auto o4TrainPtr = trainCont->GetUnity( TI.Get( L"O4" ).bdg );
	CPPUNIT_ASSERT( o4TrainPtr );
	auto o4TrainId = o4TrainPtr->GetId();

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"A4" ) ) );
	auto a4TrainPtr = trainCont->GetUnity( TI.Get( L"A4" ).bdg );
	CPPUNIT_ASSERT( a4TrainPtr );
	auto a4TrainId = a4TrainPtr->GetId();

	TI.DetermineStripFree( L"R4", *tracker, false );
	TI.DetermineStripFree( L"S4", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripFree( L"A4", *tracker, false );
	TI.DetermineStripFree( L"O4", *tracker, false );
	TI.DetermineStripBusy( L"R4", *tracker, false );
	TI.DetermineStripBusy( L"S4", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker, false ); //не форсирует восстановление близлежащих поездов, т.к. поезд не может покинуть ПО-путь
	TI.DetermineStripBusy( L"F4", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	auto trainPtr = trainCont->GetUnity( s4TrainId );
	CPPUNIT_ASSERT( trainPtr );
	CPPUNIT_ASSERT( trainPtr->IsTrain() );
	CPPUNIT_ASSERT( trainPtr->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( trainPtr->Have( TI.Get( L"S4" ).bdg ) );
	CPPUNIT_ASSERT( trainPtr->Have( TI.Get( L"R4" ).bdg ) );

	auto newUnityPtr = trainCont->GetUnity( TI.Get( L"F4" ).bdg );
	CPPUNIT_ASSERT( newUnityPtr );
	CPPUNIT_ASSERT( !newUnityPtr->IsTrain() );
	auto newUnityId = newUnityPtr->GetId();
	CPPUNIT_ASSERT( newUnityId != s4TrainId && newUnityId != o4TrainId && newUnityId != a4TrainId );
	CPPUNIT_ASSERT( trainCont->IsDeadUnity( a4TrainId ) && trainCont->IsDeadUnity( o4TrainId ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryWithTaking()
{
	TI.DetermineStripBusy( L"AF5", *tracker );
	TI.DetermineStripBusy( L"AB5", *tracker );
	TI.DetermineStripBusy( L"AC5", *tracker );
	SetRandomEvenInfo( TI.Get( L"AB5" ).bdg );
	auto trainToRecover = SetRandomEvenInfo( TI.Get( L"AF5" ).bdg );
	auto idToRecover = trainToRecover->GetId();
	TI.DetermineStripBusy( L"Z5", *tracker );
	auto z5TrainPtr = SetRandomEvenInfo( TI.Get( L"Z5" ).bdg );
	CPPUNIT_ASSERT( z5TrainPtr );
	TI.DetermineStripBusy( L"Y5", *tracker );
	SetRandomEvenInfo( TI.Get( L"Y5" ).bdg );
	DisformTrain( z5TrainPtr->GetId() );

	TI.DetermineStripFree( L"AB5", *tracker, false );
	TI.DetermineStripFree( L"AC5", *tracker, false );
	TI.DetermineStripFree( L"AF5", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"AB5", *tracker, false );
	TI.DetermineStripBusy( L"AC5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"AC5", *tracker, false );
	TI.DetermineStripBusy( L"Y5", *tracker, false );
	TI.DetermineSwitchMinus( L"55", *tracker, false );
	TI.DetermineSwitchMinus( L"56", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	//поезд должен восстанавливаться
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [idToRecover]( TrainUnityCPtr tptr ){
		return tptr->GetId() == idToRecover;
	} ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryWithTaking2()
{
	TI.DetermineSwitchMinus( L"56", *tracker );
	TI.DetermineStripBusy( L"AF5", *tracker, false );
	TI.DetermineStripBusy( L"AD5", *tracker, false );
	TI.DetermineStripBusy( L"AC5", *tracker, false );
	TI.DetermineStripBusy( L"AB5", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();

	SetRandomOddInfo( TI.Get( L"AF5" ).bdg );
	TI.DetermineStripBusy( L"AG5", *tracker );
	TI.DetermineStripBusy( L"AI5", *tracker );
	SetRandomEvenInfo( TI.Get( L"AG5" ).bdg );
	SetRandomEvenInfo( TI.Get( L"AI5" ).bdg );

	tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"AB5", *tracker, false );
	TI.DetermineStripFree( L"AC5", *tracker, false );
	TI.DetermineStripFree( L"AD5", *tracker, false );
	TI.DetermineStripFree( L"AF5", *tracker, false );
	TI.DetermineStripFree( L"AG5", *tracker, false );
	TI.DetermineStripFree( L"AI5", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"AB5", *tracker, false );
	TI.DetermineStripBusy( L"AC5", *tracker, false );
	TI.DetermineStripBusy( L"AD5", *tracker, false );
	TI.DetermineStripBusy( L"AF5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"Z5", *tracker );
	TI.DetermineStripBusy( L"Y5", *tracker );
	TI.DetermineStripBusy( L"AM5", *tracker );
	TI.DetermineStripBusy( L"AN5", *tracker );
	SetRandomEvenInfo( TI.Get( L"AM5" ).bdg );

	tunities = trainCont->GetUnities();

	TI.DetermineSwitchMinus( L"55", *tracker, false );
	TI.DetermineSwitchMinus( L"57", *tracker, false );
	TI.DetermineSwitchMinus( L"58", *tracker, false );
	TI.DetermineStripBusy( L"AK5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::RecoveryWithTaking3()
{
	TI.DetermineSwitchMinus( L"56", *tracker );
	TI.DetermineStripBusy( L"AF5", *tracker, false );
	TI.DetermineStripBusy( L"AD5", *tracker, false );
	TI.DetermineStripBusy( L"AC5", *tracker, false );
	TI.DetermineStripBusy( L"AB5", *tracker, false );
	TI.FlushData( *tracker, false );

	SetRandomOddInfo( TI.Get( L"AF5" ).bdg );
	TI.DetermineStripBusy( L"AG5", *tracker );
	TI.DetermineStripBusy( L"AI5", *tracker );
	SetRandomEvenInfo( TI.Get( L"AG5" ).bdg );
	SetRandomEvenInfo( TI.Get( L"AI5" ).bdg );

	TI.DetermineStripFree( L"AB5", *tracker, false );
	TI.DetermineStripFree( L"AC5", *tracker, false );
	TI.DetermineStripFree( L"AD5", *tracker, false );
	TI.DetermineStripFree( L"AF5", *tracker, false );
	TI.DetermineStripFree( L"AG5", *tracker, false );
	TI.DetermineStripFree( L"AI5", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"AB5", *tracker, false );
	TI.DetermineStripBusy( L"AC5", *tracker, false );
	TI.DetermineStripBusy( L"AD5", *tracker, false );
	TI.DetermineStripBusy( L"AF5", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"Z5", *tracker );
	SetInfo( TI.Get( L"Z5" ).bdg, TrainDescr() );
	TI.DetermineSwitchMinus( L"55", *tracker );
	TI.DetermineStripBusy( L"AK5", *tracker );
	TI.DetermineStripBusy( L"AM5", *tracker );
	SetRandomEvenInfo( TI.Get( L"AM5" ).bdg );
	TI.DetermineSwitchMinus( L"58", *tracker );

	TI.DetermineSwitchMinus( L"57", *tracker, false );
	TI.DetermineStripBusy( L"Y5", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::ForbidMultiPlacesRecovery()
{
	TI.DetermineStripBusy( L"B5", *tracker );
	auto trainPtr = SetRandomEvenInfo( TI.Get( L"B5" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId1 = trainPtr->GetId();
	CPPUNIT_ASSERT( trainPtr );
	auto tdescrPtr1 = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr1 );

	TI.DetermineStripBusy( L"A5", *tracker );
	trainPtr = SetRandomEvenInfo( TI.Get( L"A5" ).bdg );
	auto trainId2 = trainPtr->GetId();
	CPPUNIT_ASSERT( trainPtr );
	auto tdescrPtr2 = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr2 );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"X5", *tracker, false );
	TI.DetermineStripBusy( L"W5", *tracker, false );
	TI.FlushData( *tracker, false );
	trainPtr = SetRandomEvenInfo( TI.Get( L"X5" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto tdescrPtr3 = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr3 );
	auto trainId3 = trainPtr->GetId();

	tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"B5", *tracker, false );
	TI.DetermineSwitchPlus( L"51", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"V5", *tracker, false );
	TI.DetermineStripBusy( L"B5", *tracker, false );
	TI.FlushData( *tracker, false );

	//три исходных ПЕ должны сохраниться
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId1]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId1; } ) );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId2]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId2; } ) );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId3]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId3; } ) );

	auto nowTrainPtr = trainCont->GetUnity( trainId1 );
	CPPUNIT_ASSERT( nowTrainPtr );
	auto tdescrPtr = nowTrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == *tdescrPtr1 );

	nowTrainPtr = trainCont->GetUnity( trainId2 );
	CPPUNIT_ASSERT( nowTrainPtr );
	tdescrPtr = nowTrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == *tdescrPtr2 );

	nowTrainPtr = trainCont->GetUnity( trainId3 );
	CPPUNIT_ASSERT( nowTrainPtr );
	tdescrPtr = nowTrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == *tdescrPtr3 );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::HeadPositioningAfterRecovery()
{
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"340" ) ), list<wstring>( 1, L"E4" ) );
	TI.DetermineStripBusy( L"G4", *tracker );
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"H4", *tracker );
	tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"E4", *tracker, false );
	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"H4", *tracker );

	tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"E4", *tracker, false );
	TI.DetermineStripBusy( L"G4", *tracker, false );
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto rectunity = tunities.back();
	CPPUNIT_ASSERT( rectunity->Head() == TI.Get( L"H4" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::OddnessRecoveryForNoident()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"C" ) );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto evenTrain = tunities.back();
	auto trainId = evenTrain->GetId();
	CPPUNIT_ASSERT( evenTrain->GetOddness() == Oddness::EVEN );

	TI.DetermineStripFree( L"F", *tracker );
	tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"F", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto recovTrain = tunities.back();
	CPPUNIT_ASSERT( recovTrain->GetId() == trainId );
	CPPUNIT_ASSERT( recovTrain->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::DeathPlaceShouldBeRecordedInHistory()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"C" ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto recovTrain = tunities.back();
	CPPUNIT_ASSERT( recovTrain->GetId() == trainId );
	const auto & plcHistory = recovTrain->GetHistory();
	CPPUNIT_ASSERT( plcHistory.LookFor( TI.Get( L"C" ).bdg ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::AppearanceVsRecovery()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2551" ) ), list<wstring>( 1, L"V5" ) ); //под восстановление
	auto v5trainPtr = trainCont->GetUnity( TI.Get( L"V5" ).bdg );
	CPPUNIT_ASSERT( v5trainPtr && v5trainPtr->GetDescrPtr() );
	auto v5TrainDescr = *v5trainPtr->GetDescrPtr();
	auto v5TrainId = v5trainPtr->GetId();
	TI.DetermineStripFree( L"V5", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"4520" ) ), list<wstring>( 1, L"A5" ) );
	auto a5trainPtr = trainCont->GetUnity( TI.Get( L"A5" ).bdg );
	CPPUNIT_ASSERT( a5trainPtr && a5trainPtr->GetDescrPtr() );
	auto a5TrainDescr = *a5trainPtr->GetDescrPtr();
	auto a5TrainId = a5trainPtr->GetId();
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C5" ) );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"B5" ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"B5", *tracker, false );
	TI.DetermineStripFree( L"C5", *tracker, false );
	TI.DetermineSwitchPlus( L"51", *tracker, false );
	TI.DetermineStripBusy( L"V5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	auto curV5trainPtr = trainCont->GetUnity( TI.Get( L"V5" ).bdg );
	CPPUNIT_ASSERT( curV5trainPtr && curV5trainPtr->GetId() == v5TrainId && curV5trainPtr->GetDescrPtr() && *curV5trainPtr->GetDescrPtr() == v5TrainDescr );
	auto curA5TrainPtr = trainCont->GetUnity( TI.Get( L"A5" ).bdg );
	CPPUNIT_ASSERT( curA5TrainPtr && curA5TrainPtr->GetId() == a5TrainId && curA5TrainPtr->GetDescrPtr() && *curA5TrainPtr->GetDescrPtr() == a5TrainDescr );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::NoTrainRecovery()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	auto tunity = trainCont->GetUnity( TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( tunity );
	auto thistory = tunity->GetHistory();
	CPPUNIT_ASSERT( !tunity->IsTrain() );
	auto tunityId = tunity->GetId();
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	auto tunity2 = trainCont->GetUnity( TI.Get( L"F" ).bdg );

	CPPUNIT_ASSERT( tunity2 && !tunity2->IsTrain() && tunity2->GetId() == tunityId );
	auto recAdOutPtr1 = thistory.RecentIncident( ADIncident::ArrdepState::PARTIAL_LEAVED );
	auto recAdOutPtr2 = tunity2->GetHistory().RecentIncident( ADIncident::ArrdepState::PARTIAL_LEAVED );
	CPPUNIT_ASSERT( recAdOutPtr1 && recAdOutPtr2 && *recAdOutPtr1 == *recAdOutPtr2 );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::NoTrainRecovery2()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"G" ) );
	auto tunity = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( !tunity->IsTrain() );
	auto tunityId = tunity->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );

	auto tunity2 = trainCont->GetUnity( TI.Get( L"X" ).bdg );
	CPPUNIT_ASSERT( tunity2 );
	auto tdescrPtr = tunity2->GetDescrPtr();
	CPPUNIT_ASSERT( tunity2 && tunity2->IsTrain() && tunity2->GetId() == tunityId && tdescrPtr && *tdescrPtr == TrainDescr() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::NoTrainRecoveryNearADWay()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );
	TI.DetermineStripFree( L"C", *tracker );
	auto recovMom = TI.DetermineStripBusy( L"F", *tracker );
	auto tunity = trainCont->GetUnity( TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( tunity );
	const auto & thistory = tunity->GetHistory();
	auto lastADOutPtr = thistory.RecentIncident( ADIncident::ArrdepState::PARTIAL_LEAVED );
	CPPUNIT_ASSERT( lastADOutPtr );
	const auto & adOutBdg = lastADOutPtr->from;
	const auto & adOutTime = lastADOutPtr->moment;
	CPPUNIT_ASSERT( adOutBdg == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( adOutTime == recovMom );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::NoRememberNoTrainOnSpan()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	list<wstring> tplace( 1, L"H" );
	tplace.emplace_back( L"W" );
	tplace.emplace_back( L"Q" );
	LocateUnityByForce( nullptr, tplace );
	auto notrainId = trainCont->GetUnity( TI.Get( L"H" ).bdg )->GetId();
	TI.DetermineStripFree( L"H", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false ); //исчезновение маневровой (уже не восстановится)

	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"C" ) ); //поезд
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( all_of( tunities.cbegin(), tunities.cend(), [&notrainId]( TrainUnityCPtr tptr ){
		return tptr->GetId() != notrainId;
	} ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::NoRememberSuddenSpanActivity()
{
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"H", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.empty() );
	CPPUNIT_ASSERT( !trainCont->IsDeadUnity( trainId ) ); //не должно быть восстановлений (поезд умер на том же перегоне, что и родился)
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::ForbidRecoveryAfterRouteSet()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2522" ) ), list<wstring>( 1, L"C" ) );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"3329" ) ), list<wstring>( 1, L"G" ) );
	auto waytrainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	auto waytrainId = waytrainPtr->GetId();
	auto disapptrainPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	auto disapptrainId = waytrainPtr->GetId();

	//исчезновение
	TI.DetermineStripFree( L"G", *tracker );

	//задание маршрута и отправление поезда с пути
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripFree( L"G", *tracker );

	//восстановление исчезнувшего на G нечетного поезда подавляется
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainPtr = tunities.back();
	CPPUNIT_ASSERT( trainPtr->GetId() == waytrainId );
	auto tdescrPtr = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2522" ) );
	CPPUNIT_ASSERT( !trainCont->IsDeadUnity( disapptrainId ) ); //больше о нем информации нет

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRecovery::FullStationExitAfterRecoveryOnADWay()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.DetermineStripBusy( L"X", *tracker, false );
	auto recovMoment = TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"F", *tracker, false );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetPureTime() == recovMoment );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetPureTime() == recovMoment );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetPureTime() == recovMoment );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetPureTime() == recovMoment );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}