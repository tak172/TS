#include "stdafx.h"
#include "TC_TrackerRouteSituations.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerRouteSituations );

void TC_TrackerRouteSituations::setUp()
{
	TI.Reset();
}
void TC_TrackerRouteSituations::ParkMigration( const vector<wstring> & routeNames, const vector<wstring> & throughPlaces, bool mustBeTrain )
{
	ASSERT( !throughPlaces.empty() );
	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineSwitchPlus( L"21", *tracker );
	const auto & startPlaceName = throughPlaces.front();
	TI.DetermineStripBusy( startPlaceName, *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	SetRandomOddInfo( train );

	//задание маршрутов
	for( const auto & routeName : routeNames )
		TI.DetermineRouteSet( routeName, *tracker );

	//переезд в парк
	TI.ImitateMotion( throughPlaces, *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.front();
	CPPUNIT_ASSERT( train->GetId() == tunity->GetId() );
	CPPUNIT_ASSERT( mustBeTrain ? tunity->IsTrain() : !tunity->IsTrain() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRouteSituations::ParkMigrationByShortShuntRoute()
{
	vector <wstring> routeNames( 1, L"Head203S->T2" );
	wstring stripsArr[] = { L"M2", L"O2", L"P2", L"R2", L"I2", L"T2" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	ParkMigration( routeNames, stripsSeq, true );
}

void TC_TrackerRouteSituations::ParkMigrationByShortTrainRoute()
{
	vector <wstring> routeNames( 1, L"Head203T->T2" );
	wstring stripsArr[] = { L"M2", L"O2", L"P2", L"R2", L"I2", L"T2" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	ParkMigration( routeNames, stripsSeq, true );
}

void TC_TrackerRouteSituations::ParkMigrationWithoutRoutes()
{
	wstring stripsArr[] = { L"M2", L"O2", L"P2", L"R2", L"I2", L"T2" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	ParkMigration( vector <wstring>(), stripsSeq, true );
}

void TC_TrackerRouteSituations::ParkMigrationByOnOffTrainRoute()
{
	wstring trainRoute = L"Head203T->T2";
	wstring stripsArr[] = { L"M2", L"O2", L"P2", L"R2", L"I2", L"T2" };

	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineSwitchPlus( L"21", *tracker );
	TI.DetermineStripBusy( L"M2", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	SetRandomOddInfo( train );

	//установка и отмена маршрута
	TI.DetermineRouteSet( trainRoute, *tracker );
	TI.DetermineRouteUnset( trainRoute, *tracker );

	//переезд в парк
	TI.ImitateMotion( vector<wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.front();
	CPPUNIT_ASSERT( train->GetId() == tunity->GetId() );
	CPPUNIT_ASSERT( tunity->IsTrain() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRouteSituations::SplitIntoSettledRoute()
{
	TI.DetermineRouteSet( L"Head400->K4", *tracker );
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	SetRandomEvenInfo( train );
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker );
	TI.DetermineStripBusy( L"K4", *tracker );
	TI.DetermineStripBusy( L"I4", *tracker );
	TI.DetermineStripFree( L"H4", *tracker );
	TI.DetermineStripFree( L"F4", *tracker );
	tunities = trainCont->GetUnities();
	TrainUnityCPtr g4tunity, k4i4train;
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 )
		{
			CPPUNIT_ASSERT( tplace.back() == TI.Get( L"G4" ).bdg );
			CPPUNIT_ASSERT( !tunity->IsTrain() ) ;
			g4tunity = tunity;
		}
		else
		{
			CPPUNIT_ASSERT( tplace.size() == 2 );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"K4" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"I4" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( tunity->IsTrain() ) ;
			k4i4train = tunity;
		}
	}
	CPPUNIT_ASSERT( g4tunity && k4i4train );
	auto trainId = k4i4train->GetId();
	auto notrainId = g4tunity->GetId();

	TI.DetermineStripFree( L"G4", *tracker );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curunity = tunities.back();
	CPPUNIT_ASSERT( curunity->IsTrain() );
	CPPUNIT_ASSERT( curunity->GetId() == trainId );
	const auto & tplace = curunity->GetPlace();
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"K4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"I4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRouteSituations::ParkMigrationOnCuttingRoute()
{
	wstring trainRoute = L"Head203T->T2";
	wstring stripsArr[] = { L"O2", L"P2", L"R2", L"I2", L"T2" };

	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineSwitchPlus( L"21", *tracker );
	TI.DetermineStripBusy( L"M2", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	SetRandomOddInfo( train );

	//установка маршрута
	TI.DetermineRouteSet( trainRoute, *tracker );
	
	//начало движения поезда (переход через светофор)
	TI.DetermineStripBusy( L"O2", *tracker, false );
	TI.DetermineStripFree( L"M2", *tracker, false );
	TI.FlushData( *tracker, false );

	//перевод маршрута в состояние разделки
	TI.DetermineRouteCutting( trainRoute, *tracker );

	//переезд в парк
	TI.ImitateMotion( vector<wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.front();
	CPPUNIT_ASSERT( train->GetId() == tunity->GetId() );
	CPPUNIT_ASSERT( tunity->IsTrain() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRouteSituations::UnsetRouteWithBPMigrations()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2056" ) ), list<wstring>( 1, L"ADBP2" ) ) );

	auto tptr = trainCont->GetUnity( TI.Get( L"ADBP2" ).bdg );
	auto trainId = tptr->GetId();

	wstring trainRoute = L"BPHead106->SBP13";
	TI.DetermineRouteSet( trainRoute, *tracker );

	TI.DetermineStripFree( L"ADBP2", *tracker );
	TI.DetermineStripBusy( L"SBP12", *tracker );
	TI.DetermineRouteCutting( trainRoute, *tracker );
	TI.DetermineStripBusy( L"SBP13", *tracker );
	TI.DetermineStripFree( L"SBP12", *tracker );
	
	TI.DetermineStripFree( L"SBP13", *tracker, false );
	TI.DetermineStripBusy( L"BP7", *tracker, false );
	TI.DetermineRouteUnset( trainRoute, *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrainPtr = tunities.back();
	CPPUNIT_ASSERT( curtrainPtr && curtrainPtr->GetDescrPtr() );
	CPPUNIT_ASSERT( *curtrainPtr->GetDescrPtr() == TrainDescr( L"2056" ) );
	CPPUNIT_ASSERT( curtrainPtr->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRouteSituations::UnsetRouteWithBPMigrations2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2056" ) ), list<wstring>( 1, L"ADBP2" ) ) );

	auto tptr = trainCont->GetUnity( TI.Get( L"ADBP2" ).bdg );
	auto trainId = tptr->GetId();

	wstring trainRoute = L"BPHead106->SBP13";
	TI.DetermineRouteSet( trainRoute, *tracker );

	TI.DetermineStripFree( L"ADBP2", *tracker );
	TI.DetermineStripBusy( L"SBP12", *tracker );
	TI.DetermineRouteCutting( trainRoute, *tracker );
	TI.DetermineStripBusy( L"SBP13", *tracker );
	TI.DetermineStripFree( L"SBP12", *tracker );

	//моргание (+плюс снятие маршрута)
	TI.DetermineRouteUnset( trainRoute, *tracker, false );
	TI.DetermineStripFree( L"SBP13", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"SBP13", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrainPtr = tunities.back();
	CPPUNIT_ASSERT( curtrainPtr && curtrainPtr->GetDescrPtr() );
	CPPUNIT_ASSERT( *curtrainPtr->GetDescrPtr() == TrainDescr( L"2056" ) );
	CPPUNIT_ASSERT( curtrainPtr->GetId() == trainId );

	//переезд
	TI.DetermineStripFree( L"SBP13", *tracker, false );
	TI.DetermineStripBusy( L"BP7", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto curtrainPtr2 = tunities2.back();
	CPPUNIT_ASSERT( curtrainPtr2 && curtrainPtr2->GetDescrPtr() );
	CPPUNIT_ASSERT( *curtrainPtr2->GetDescrPtr() == TrainDescr( L"2056" ) );
	CPPUNIT_ASSERT( curtrainPtr2->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRouteSituations::SimultHeadCloseAndADOut()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.cargoTransArrivDurMs = 15000;

	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2836" ) ), list<wstring>( 1, L"Q" ) );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	TI.IncreaseTime( 5000 );

	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineHeadClose( L"Head102", *tracker, false );
	TI.DetermineRouteCutting( L"Head102->G", *tracker, false );
	TI.FlushData( *tracker, false );

	const auto & trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( trackerEvents.cbegin(), trackerEvents.cend() );

	CPPUNIT_ASSERT( evvec.size() == 3 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::TRANSITION );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRouteSituations::DepartureWithIrregularBusyOnRoute()
{
	TI.DetermineSwitchMinus( L"17", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"5161" ) ), list<wstring>( 1, L"AC" ) );
	TI.DetermineRouteSet( L"Head103->Z", *tracker );
	auto tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainPtr = tunities.back();
	CPPUNIT_ASSERT( trainPtr->IsTrain() );
	auto trainId = trainPtr->GetId();

	TI.DetermineStripBusy( L"Z", *tracker );

	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"AA", *tracker, false );
	TI.DetermineStripBusy( L"AB", *tracker, false );
	TI.DetermineStripFree( L"AC", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( count_if( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		return tptr->IsTrain();
	} ) == 1 );
	auto curtrainPtr = trainCont->GetUnity( TI.Get( L"AB" ).bdg );
	CPPUNIT_ASSERT( curtrainPtr );
	CPPUNIT_ASSERT( curtrainPtr->GetId() == trainId && curtrainPtr->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}