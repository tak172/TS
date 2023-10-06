#include "stdafx.h"
#include "TC_TrackerTrainDisappearance.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTrainDisappearance );
void TC_TrackerTrainDisappearance::setUp()
{
	TI.Reset();
	TrainContainer & mutTCont = tracker->GetTrainContainer();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.delnotifSec = 8;
}

void TC_TrackerTrainDisappearance::TwinklingDisappearance()
{
	const TrainDescr & tdescr = RandomTrainDescr();
	TI.DetermineStripBusy( L"C", *tracker );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	auto train = trainCont->GetUnities().front();
	SetInfo( train, tdescr );
	changes = tracker->TakeTrainChanges(); //сброс изменений
	const auto & timConstr = tracker->GetTimeConstraints();

	//мерцание
	for ( unsigned int k = 0; k < 5; ++k )
	{
		TI.DetermineStripFree( L"C", *tracker );
		changes = tracker->TakeTrainChanges();
		CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
		TI.DetermineStripBusy( L"C", *tracker );
		changes = tracker->TakeTrainChanges();
		CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 ); //сообщение об изменении носит некоторую избыточность. но это лучше, чем его потерять
		CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
		auto train = trainCont->GetUnities().front();
		SetInfo( train, tdescr );
		TI.IncreaseTime( ( timConstr.delnotifSec / 2 ) * 1000 );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::DisapperanceAndTeleport()
{
	const TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const TrainDescr & tdescr = RandomTrainDescr();
	TI.DetermineStripBusy( L"C", *tracker );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	auto trainC = trainCont->GetUnities().front();
	SetInfo( trainC, tdescr );
	changes = tracker->TakeTrainChanges(); //сброс изменений

	//гашение и мгновенное зажигание в отдалении
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.IncreaseTime( ( timConstr.delnotifSec + 1 ) * 1000 );
	auto trainQ = trainCont->GetUnities().front();
	SetInfo( trainQ, tdescr );

	tracker->TakeChanges( changes );
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get( L"C" ).bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get( L"Q" ).bdg ) );
	for ( const auto & change : changes.placeViewChanges )
	{
		if ( change.place == TI.Get( L"C" ).bdg )
		{
			CPPUNIT_ASSERT( !change.appeared );
			CPPUNIT_ASSERT( change.trainId == trainC->GetId() );
		}
		else
		{
			CPPUNIT_ASSERT( change.place == TI.Get( L"Q" ).bdg );
			CPPUNIT_ASSERT( change.appeared );
			CPPUNIT_ASSERT( change.trainId == trainQ->GetId() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::DisappearanceAndRecoverNear()
{
	const TrainDescr & tdescr = RandomTrainDescr();
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"D3", *tracker );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	auto train = trainCont->GetUnities().front();
	SetInfo( train, tdescr );
	changes = tracker->TakeTrainChanges();

	//гашение
	TI.DetermineStripFree( L"X3", *tracker, false );
	TI.DetermineStripFree( L"F3", *tracker, false );
	TI.DetermineStripFree( L"D3", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"X3", *tracker, false );
	TI.DetermineStripBusy( L"F3", *tracker, false );
	TI.FlushData( *tracker, false );
	changes = tracker->TakeTrainChanges();

	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 3 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get( L"F3" ).bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get( L"X3" ).bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get( L"D3" ).bdg ) );
	for ( const auto & tchange : changes.placeViewChanges )
	{
		if ( tchange.place == TI.Get( L"F3" ).bdg ||
			tchange.place == TI.Get( L"X3" ).bdg )
			CPPUNIT_ASSERT( tchange.appeared );
		else
			CPPUNIT_ASSERT( !tchange.appeared );
		CPPUNIT_ASSERT( *tchange.tdescrPtr == *train->GetDescrPtr() );
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::RealDisappearance()
{
	const TrainDescr & tdescr = RandomTrainDescr();
	TI.DetermineStripBusy( L"C", *tracker );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	auto train = trainCont->GetUnities().front();
	SetInfo( train, tdescr );
	changes = tracker->TakeTrainChanges(); //сброс изменений

	//гашение
	TI.DetermineStripFree( L"C", *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	const auto & timConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( ( timConstr.delnotifSec + 1 ) * 1000 );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	const PlaceViewChange & tchange = changes.placeViewChanges.front();
	CPPUNIT_ASSERT( tchange.appeared == false );
	CPPUNIT_ASSERT( tchange.place == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tchange.trainId == train->GetId() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::HeterogeneousDisappearance()
{
	const TrainDescr & tdescrC = RandomTrainDescr();
	const TrainDescr & tdescrQ = RandomTrainDescr();
	TI.DetermineStripBusy( L"C", *tracker ); //действительно исчезает

	TI.DetermineStripBusy( L"A", *tracker );
	wstring stripsarr[] = { L"A", L"Q" };
	vector<wstring> stripsvec( begin( stripsarr ), end( stripsarr ) );
	TI.ImitateMotion( stripsvec, *tracker ); //моргает

	auto trains = trainCont->GetUnities();
	TrainUnityCPtr trainC, trainQ;
	for ( auto train : trains )
	{
		const auto & place = train->GetPlace();
		CPPUNIT_ASSERT( place.size() == 1 );
		if ( place.front() == TI.Get( L"C" ).bdg )
			trainC = train;
		else
			trainQ = train;
	}
	SetInfo( trainC, tdescrC );
	SetInfo( trainQ, tdescrQ );
	changes = tracker->TakeTrainChanges(); //сброс изменений
	const auto & timConstr = tracker->GetTimeConstraints();

	//мерцание
	for ( unsigned int k = 0; k < 5; ++k )
	{
		if ( k == 0 )
		{
			//первый раз гасится одновременно
			TI.DetermineStripFree( L"C", *tracker, false );
			TI.DetermineStripFree( L"Q", *tracker, false );
			TI.FlushData( *tracker, false );
		}
		else
			TI.DetermineStripFree( L"Q", *tracker );
		TI.DetermineStripBusy( L"Q", *tracker );
		SetInfo( trainQ, tdescrQ );
		TI.IncreaseTime( ( timConstr.delnotifSec / 2 ) * 1000 );
	}
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get( L"Q" ).bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get( L"C" ).bdg ) );
	for ( const auto & tchange : changes.placeViewChanges )
	{
		if ( tchange.place == TI.Get( L"Q" ).bdg )
		{
			CPPUNIT_ASSERT( tchange.appeared );
			CPPUNIT_ASSERT( *tchange.tdescrPtr == *trainQ->GetDescrPtr() );
		}
		else
		{
			CPPUNIT_ASSERT( !tchange.appeared );
			CPPUNIT_ASSERT( *tchange.tdescrPtr == *trainC->GetDescrPtr() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::DisappearanceVsSplitting()
{
	//пусть ПЕ занимает три участка
	TI.DetermineStripBusy( L"H", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomEvenInfo( trains.front() );
	unsigned int trainId = trains.front()->GetId();
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 3 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"H").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"W").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"Q").bdg ) );

	//формируем инкрементную посылку, освобождающую участки с середины
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( trainId == trains.front()->GetId() );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"W").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"Q").bdg ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::LengthyDisapperance()
{
	//пусть ПЕ занимает три участка
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	SetRandomOddInfo( trainList.front() );

	//формируем инкрементную посылку, освобождающую ВСЕ участки с середины
	TI.DetermineStripFree( L"C", *tracker, false );
	TI.DetermineStripFree( L"F", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.FlushData( *tracker, false );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.empty() );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 3 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"B").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"C").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"F").bdg ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::DeadEndDisappearance()
{
	//на B4 - поезд
	TI.DetermineSwitchMinus( L"42", *tracker );
	TI.DetermineStripBusy( L"B4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto initTrainId = trains.front()->GetId();
	SetRandomOddInfo( trains.front() );

	//поезд исчезает на P4 (перед тупиком T4)
	TI.DetermineStripBusy( L"P4", *tracker );
	TI.DetermineStripFree( L"B4", *tracker );
	TI.DetermineStripFree( L"P4", *tracker );

	//из тупика выезжает другой поезд
	TI.DetermineStripBusy( L"P4", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.front()->GetId() != initTrainId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::DisappearanceVsTeleportToTrain()
{
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"K4" ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"K4" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto initTrainId = trainPtr->GetId();

	list<wstring> nearPlace;
	nearPlace.push_back( L"D4" );
	nearPlace.push_back( L"E4" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), nearPlace );
	auto nearUnity = trainCont->GetUnity( TI.Get( L"D4" ).bdg );
	CPPUNIT_ASSERT( nearUnity && nearUnity->IsTrain() );
	auto nearTrainId = nearUnity->GetId();

	TI.DetermineStripFree( L"K4", *tracker ); //поезд исчезает

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	CPPUNIT_ASSERT( curtrain->GetId() == nearTrainId );
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( curtrain->Have( TI.Get( L"D4" ).bdg ) );
	CPPUNIT_ASSERT( curtrain->Have( TI.Get( L"E4" ).bdg ) );

	TI.DetermineStripBusy( L"K4", *tracker ); //восстановление

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto k4Unity = trainCont->GetUnity( TI.Get( L"K4" ).bdg );
	CPPUNIT_ASSERT( k4Unity && k4Unity->GetId() == initTrainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainDisappearance::DisappearanceVsTeleportToNoTrain()
{
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"H4" ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"H4" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto initTrainId = trainPtr->GetId();

	list<wstring> nearPlace;
	nearPlace.push_back( L"D4" );
	nearPlace.push_back( L"E4" );
	LocateUnityByForce( nullptr, nearPlace );
	auto nearUnity = trainCont->GetUnity( TI.Get( L"D4" ).bdg );
	CPPUNIT_ASSERT( nearUnity && !nearUnity->IsTrain() );
	auto nearUnityId = nearUnity->GetId();

	TI.DetermineStripFree( L"H4", *tracker );

	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	CPPUNIT_ASSERT( curtrain->GetId() == initTrainId );
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( curtrain->Have( TI.Get( L"D4" ).bdg ) );
	CPPUNIT_ASSERT( curtrain->Have( TI.Get( L"E4" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}