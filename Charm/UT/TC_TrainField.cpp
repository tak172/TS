#include "stdafx.h"
#include "TC_TrainField.h"
#include "../Guess/FieldGraph.h"
#include "TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../helpful/Pilgrim.h"
#include "../Guess/SwitchSections.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrainField );

void TC_TrainField::PathThroughTwoSwitches()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	const auto & pathBetween = fldgraph.PathBetween( TI.Get( L"B2" ).bdg, TI.Get( L"F2" ).bdg, 5 );
	CPPUNIT_ASSERT( pathBetween.size() == 5 );
	vector <BadgeE> bdgvec( pathBetween.cbegin(), pathBetween.cend() );
	CPPUNIT_ASSERT( bdgvec[0] == TI.Get( L"B2" ).bdg );
	CPPUNIT_ASSERT( bdgvec[1] == TI.Get( L"G2" ).bdg );
	CPPUNIT_ASSERT( bdgvec[2] == TI.Get( L"H2" ).bdg );
	CPPUNIT_ASSERT( bdgvec[3] == TI.Get( L"E2" ).bdg );
	CPPUNIT_ASSERT( bdgvec[4] == TI.Get( L"F2" ).bdg );
}

void TC_TrainField::BidStationPockets()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & pockets = tfieldPtr->GetComponent<vector<PocketDef>>();
	CPPUNIT_ASSERT( !pockets.empty() );
	for ( const auto & pocket : pockets )
	{
		const auto & gut = pocket.Gut();
		CPPUNIT_ASSERT( find( gut.cbegin(), gut.cend(), TI.Get( L"X4" ).bdg ) == gut.cend() );
	}
}

void TC_TrainField::PathFindNearWOTsStrip()
{
	auto tfieldPtr = tracker->GetTrainField();
	unsigned int graph_dist = 0;
	TI.DetermineSwitchMinus( L"14", *tracker );
	TI.DetermineSwitchPlus( L"12", *tracker );
	const auto & switchSections = tfieldPtr->GetComponent<TrainField::SwitchSections>();
	const auto & switchBdg = switchSections.GetCommonSwitch( TI.Get( L"S" ).bdg, TI.Get( L"T" ).bdg );
	CPPUNIT_ASSERT( switchBdg == TI.Get( L"14" ).bdg );
}


void TC_TrainField::PathSearchingDepthDeficit()
{
	shared_ptr<const TestGuessNsiBasis> nsiBasisPtr( new TestGuessNsiBasis() );
	TrainFieldPtr tstFieldPtr( new TrainField( 3, nsiBasisPtr ) );
	TestTracker tstTracker( tstFieldPtr, TrackingConfiguration() );
	TI.CreateStation( *tstFieldPtr, true );
	tstTracker.Reset();
	const auto & tcontainer = tstTracker.GetTrainContainer();

	TI.DetermineSwitchPlus( L"11", tstTracker );
	TI.DetermineSwitchMinus( L"12", tstTracker );
	TI.DetermineSwitchMinus( L"13", tstTracker );

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"560" ) ), list<wstring>( 1, L"A" ), &tstTracker ) );

	auto trains = tcontainer.GetUnities();
	auto trainId = trains.front()->GetId();
	list<wstring> tunityPlace;
	tunityPlace.push_back( L"D" );
	tunityPlace.push_back( L"E" );
	tunityPlace.push_back( L"G" );
	tunityPlace.push_back( L"X" );
	tunityPlace.push_back( L"I" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tunityPlace, &tstTracker ) );
	
	trains = tcontainer.GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );

	TI.DetermineStripFree( L"A", tstTracker, false );
	TI.DetermineStripFree( L"D", tstTracker, false );
	TI.DetermineStripFree( L"E", tstTracker, false );
	TI.DetermineStripFree( L"G", tstTracker, false );
	TI.FlushData( tstTracker, false );

	trains = tcontainer.GetUnities();

	auto trainIt = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr train ){
		return train->IsTrain();
	} );
	CPPUNIT_ASSERT( trainIt != trains.cend() );
	auto curtrain = *trainIt;
	CPPUNIT_ASSERT_MESSAGE( "Identifiers are not equal", curtrain->GetId() == trainId );
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT_MESSAGE( "Bad place size", tplace.size() == 2 );
	CPPUNIT_ASSERT_MESSAGE( "No X-strip in place", find( tplace.cbegin(), tplace.cend(), TI.Get( L"X" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT_MESSAGE( "No I-strip in place", find( tplace.cbegin(), tplace.cend(), TI.Get( L"I" ).bdg ) != tplace.cend() );
	tracker->Reset();
	CPPUNIT_ASSERT_MESSAGE( "Exceptions were generated", !TI.TakeExceptions() );
	CPPUNIT_ASSERT_MESSAGE( "Integrity problem", tracker->CheckIntegrity() );
}

void TC_TrainField::PathSearchingDepthDeficit2()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	wstring obnames1[] = { L"G3", L"A3", L"C3", L"D3", L"F3", L"X3", L"Y3", L"H3", L"I3", L"J3", L"K3", L"L3", L"M3", L"N3",
		L"O3", L"P3", L"R3"
	};
	wstring obnames2[] = { L"U3", L"T3", L"S3" };
	list <BadgeE> oblist1, oblist2;
	for_each( begin( obnames1 ), end( obnames1 ), [&oblist1, this]( wstring obname ){
		oblist1.push_back( TI.Get( obname ).bdg );
	} );
	for_each( begin( obnames2 ), end( obnames2 ), [&oblist2, this]( wstring obname ){
		oblist2.push_back( TI.Get( obname ).bdg );
	} );

	const auto & reslist = fldgraph.PavePath( oblist1, oblist2 );
	list <BadgeE> tstlist( oblist1 );
	tstlist.insert( tstlist.cend(), oblist2.crbegin(), oblist2.crend() );
	CPPUNIT_ASSERT( tstlist == reslist );
}

void TC_TrainField::PathSearchingDepthDeficit3()
{
	shared_ptr<const TestGuessNsiBasis> nsiBasisPtr( new TestGuessNsiBasis() );
	TrainFieldPtr tstFieldPtr( new TrainField( 2, nsiBasisPtr ) );
	TestTracker tstTracker( tstFieldPtr, TrackingConfiguration() );
	TI.CreateStation( *tstFieldPtr, true );
	tstTracker.Reset();

	TI.DetermineSwitchMinus( L"43", tstTracker );
	TI.DetermineSwitchMinus( L"44", tstTracker );
	TI.DetermineSwitchPlus( L"45", tstTracker );

	//поезд
	TI.DetermineStripBusy( L"S4", tstTracker );
	const auto & tstTrainCont = tstTracker.GetTrainContainer();
	auto tunities = tstTrainCont.GetUnities();
	auto train = tunities.back();
	SetRandomEvenInfo( train, &tstTracker );
	auto trainId = train->GetId();
	auto trainDescrPtr = train->GetDescrPtr();
	CPPUNIT_ASSERT( trainDescrPtr );

	//отъезд маневровой:
	TI.DetermineStripBusy( L"F4", tstTracker, false );
	TI.DetermineStripBusy( L"G4", tstTracker, false );
	TI.DetermineStripBusy( L"H4", tstTracker, false );
	TI.DetermineStripBusy( L"K4", tstTracker, false );
	TI.DetermineStripBusy( L"I4", tstTracker, false );
	TI.FlushData( tstTracker, false );

	//отъезд поезда:
	TI.DetermineStripFree( L"S4", tstTracker );

	tunities = tstTrainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto nowTrain = tunities.back();
	CPPUNIT_ASSERT( trainId == nowTrain->GetId() );
	auto nowTrainDescrPtr = nowTrain->GetDescrPtr();
	CPPUNIT_ASSERT( nowTrainDescrPtr && *nowTrainDescrPtr == *trainDescrPtr );
	CPPUNIT_ASSERT( tstTracker.CheckIntegrity() );
}

void TC_TrainField::BranchedPathSearching()
{
	list<BadgeE> oblist1, oblist2;
	oblist1.push_back( TI.Get( L"Q" ).bdg );
	oblist1.push_back( TI.Get( L"A" ).bdg );
	oblist1.push_back( TI.Get( L"B" ).bdg );
	oblist1.push_back( TI.Get( L"C" ).bdg );
	oblist1.push_back( TI.Get( L"F" ).bdg );
	oblist1.push_back( TI.Get( L"G" ).bdg );
	oblist2.push_back( TI.Get( L"X" ).bdg );

	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	const auto & reslist = fldgraph.PavePath( oblist1, oblist2 );
	CPPUNIT_ASSERT( reslist.size() == 7 );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"Q" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"A" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"B" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"C" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"F" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"G" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"X" ).bdg ) != reslist.cend() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrainField::BranchedPathSearching2()
{
	list<BadgeE> oblist1, oblist2;
	oblist1.push_back( TI.Get( L"Q" ).bdg );
	oblist1.push_back( TI.Get( L"A" ).bdg );
	oblist1.push_back( TI.Get( L"D" ).bdg );
	oblist1.push_back( TI.Get( L"E" ).bdg );
	oblist1.push_back( TI.Get( L"G" ).bdg );
	oblist2.push_back( TI.Get( L"X" ).bdg );

	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	const auto & reslist = fldgraph.PavePath( oblist1, oblist2 );
	CPPUNIT_ASSERT( reslist.size() == 6 );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"Q" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"A" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"D" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"E" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"G" ).bdg ) != reslist.cend() );
	CPPUNIT_ASSERT( find( reslist.cbegin(), reslist.cend(), TI.Get( L"X" ).bdg ) != reslist.cend() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrainField::PathSearching()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	wstring obnames1[] = { L"B", L"A" };
	wstring obnames2[] = { L"G" };
	list <BadgeE> oblist1, oblist2;
	for_each( begin( obnames1 ), end( obnames1 ), [&oblist1, this]( wstring obname ){
		oblist1.push_back( TI.Get( obname ).bdg );
	} );
	for_each( begin( obnames2 ), end( obnames2 ), [&oblist2, this]( wstring obname ){
		oblist2.push_back( TI.Get( obname ).bdg );
	} );

	const auto & reslist = fldgraph.PavePath( oblist1, oblist2 );

	wstring tstnames[] = { L"A", L"B", L"C", L"F", L"G" };
	list <BadgeE> tstlist;
	for_each( begin( tstnames ), end( tstnames ), [&tstlist, this]( wstring obname ){
		tstlist.push_back( TI.Get( obname ).bdg );
	} );
	CPPUNIT_ASSERT( tstlist == reslist );
}

void TC_TrainField::PathSearching2()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	wstring obnames1[] = { L"A" };
	wstring obnames2[] = { L"X", L"G", L"E" };
	list <BadgeE> oblist1, oblist2;
	for_each( begin( obnames1 ), end( obnames1 ), [&oblist1, this]( wstring obname ){
		oblist1.push_back( TI.Get( obname ).bdg );
	} );
	for_each( begin( obnames2 ), end( obnames2 ), [&oblist2, this]( wstring obname ){
		oblist2.push_back( TI.Get( obname ).bdg );
	} );

	const auto & reslist = fldgraph.PavePath( oblist1, oblist2 );

	wstring tstnames[] = { L"A", L"D", L"E", L"G", L"X" };
	list <BadgeE> tstlist;
	for_each( begin( tstnames ), end( tstnames ), [&tstlist, this]( wstring obname ){
		tstlist.push_back( TI.Get( obname ).bdg );
	} );
	CPPUNIT_ASSERT( tstlist == reslist );
}

void TC_TrainField::PathSearching3()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	wstring obnames1[] = { L"A" };
	wstring obnames2[] = { L"X", L"G", L"F" };
	list <BadgeE> oblist1, oblist2;
	for_each( begin( obnames1 ), end( obnames1 ), [&oblist1, this]( wstring obname ){
		oblist1.push_back( TI.Get( obname ).bdg );
	} );
	for_each( begin( obnames2 ), end( obnames2 ), [&oblist2, this]( wstring obname ){
		oblist2.push_back( TI.Get( obname ).bdg );
	} );

	const auto & reslist = fldgraph.PavePath( oblist1, oblist2 );

	wstring tstnames[] = { L"A", L"B", L"C", L"F", L"G", L"X" };
	list <BadgeE> tstlist;
	for_each( begin( tstnames ), end( tstnames ), [&tstlist, this]( wstring obname ){
		tstlist.push_back( TI.Get( obname ).bdg );
	} );
	CPPUNIT_ASSERT( tstlist == reslist );
}

void TC_TrainField::PathSearching4()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	wstring obnames1[] = { L"N4" };
	wstring obnames2[] = { L"H4", L"K4" };
	list <BadgeE> oblist1, oblist2;
	for_each( begin( obnames1 ), end( obnames1 ), [&oblist1, this]( wstring obname ){
		oblist1.push_back( TI.Get( obname ).bdg );
	} );
	for_each( begin( obnames2 ), end( obnames2 ), [&oblist2, this]( wstring obname ){
		oblist2.push_back( TI.Get( obname ).bdg );
	} );

	const auto & reslist = fldgraph.PavePath( oblist1, oblist2 );
	CPPUNIT_ASSERT( reslist.empty() );
}

void TC_TrainField::PaveOverlappedPaths()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();

	//полное вхождение одного подпути в другой
	wstring obnames1[] = { L"A", L"B", L"C", L"F", L"G", L"X", L"I" };
	list <BadgeE> oblist1;
	for_each( begin( obnames1 ), end( obnames1 ), [&oblist1, this]( wstring obname ){
		oblist1.push_back( TI.Get( obname ).bdg );
	} );
	wstring obnames2[] = { L"A", L"B", L"C", L"F", L"G" };
	list <BadgeE> oblist2;
	for_each( begin( obnames2 ), end( obnames2 ), [&oblist2, this]( wstring obname ){
		oblist2.push_back( TI.Get( obname ).bdg );
	} );
	for ( auto i = 0; i < 2; ++i )
	{
		auto path12 = fldgraph.PavePath( oblist1, oblist2 );
		if ( path12.front() != oblist1.front() )
			path12.reverse();
		CPPUNIT_ASSERT( path12 == oblist1 );
		oblist2.reverse();
	}

	wstring obnames3[] = { L"B", L"C", L"F", L"G", L"X", L"I" };
	list <BadgeE> oblist3;
	for_each( begin( obnames3 ), end( obnames3 ), [&oblist3, this]( wstring obname ){
		oblist3.push_back( TI.Get( obname ).bdg );
	} );
	for ( auto i = 0; i < 2; ++i )
	{
		auto path13 = fldgraph.PavePath( oblist1, oblist3 );
		if ( path13.front() != oblist1.front() )
			path13.reverse();
		CPPUNIT_ASSERT( path13 == oblist1 );
		oblist3.reverse();
	}

	//частичное вхождение одного из подпутей
	wstring obnames4[] = { L"W", L"Q", L"A", L"B", L"C" };
	list <BadgeE> oblist4;
	for_each( begin( obnames4 ), end( obnames4 ), [&oblist4, this]( wstring obname ){
		oblist4.push_back( TI.Get( obname ).bdg );
	} );
	for( auto i = 0; i < 2; ++i )
	{
		auto path14 = fldgraph.PavePath( oblist1, oblist4 );
		CPPUNIT_ASSERT( path14.size() == 9 );
		if ( path14.front() != TI.Get( L"W" ).bdg )
			path14.reverse();
		auto pathIt = path14.cbegin();
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"W" ).bdg );
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"Q" ).bdg );
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"A" ).bdg );
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"B" ).bdg );
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"C" ).bdg );
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"F" ).bdg );
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"G" ).bdg );
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"X" ).bdg );
		CPPUNIT_ASSERT( *pathIt++ == TI.Get( L"I" ).bdg );
		oblist4.reverse();
	}

	wstring obnames5[] = { L"X", L"I", L"J", L"N" };
	list <BadgeE> oblist5;
	for_each( begin( obnames5 ), end( obnames5 ), [&oblist5, this]( wstring obname ){
		oblist5.push_back( TI.Get( obname ).bdg );
	} );
	for( auto i = 0; i < 2; ++i )
	{
		auto path15 = fldgraph.PavePath( oblist1, oblist5 );
		CPPUNIT_ASSERT( path15.size() == 9 );
		if ( path15.front() != TI.Get( L"A" ).bdg )
			path15.reverse();
		auto pathIt2 = path15.cbegin();
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"A" ).bdg );
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"B" ).bdg );
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"C" ).bdg );
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"F" ).bdg );
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"G" ).bdg );
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"X" ).bdg );
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"I" ).bdg );
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"J" ).bdg );
		CPPUNIT_ASSERT( *pathIt2++ == TI.Get( L"N" ).bdg );
		oblist5.reverse();
	}
}

void TC_TrainField::PaveOverlappedPaths2()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	//полное вхождение одного подпути в другой
	wstring obnames1[] = { L"H", L"W" };
	list <BadgeE> oblist1;
	for_each( begin( obnames1 ), end( obnames1 ), [&oblist1, this]( wstring obname ){
		oblist1.push_back( TI.Get( obname ).bdg );
	} );
	wstring obnames2[] = { L"W" };
	list <BadgeE> oblist2;
	for_each( begin( obnames2 ), end( obnames2 ), [&oblist2, this]( wstring obname ){
		oblist2.push_back( TI.Get( obname ).bdg );
	} );

	auto path12 = fldgraph.PavePath( oblist1, oblist2 );
	CPPUNIT_ASSERT( path12 == oblist1 );
	auto path21 = fldgraph.PavePath( oblist2, oblist1 );
	list<BadgeE> oblist1Rev( oblist1.crbegin(), oblist1.crend() );
	CPPUNIT_ASSERT( path21 == oblist1Rev );
}

void TC_TrainField::TopologicallyUnreachable()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	const auto & pathBetween = fldgraph.PathBetween( TI.Get( L"F" ).bdg, TI.Get( L"D" ).bdg, fldgraph.GetDefSearchPathDepth() );
	CPPUNIT_ASSERT( pathBetween.empty() );
}

void TC_TrainField::PathSearchingSymmetry()
{
	auto tfieldPtr = tracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	auto path1 = fldgraph.PathBetween( TI.Get( L"X" ).bdg, TI.Get( L"Q" ).bdg, fldgraph.GetDefSearchPathDepth() );
	auto path2 = fldgraph.PathBetween( TI.Get( L"Q" ).bdg, TI.Get( L"X" ).bdg, fldgraph.GetDefSearchPathDepth() );
	CPPUNIT_ASSERT( !path1.empty() && !path2.empty() );
	CPPUNIT_ASSERT( path1.front() == TI.Get( L"X" ).bdg && path2.back() == TI.Get( L"X" ).bdg );
	CPPUNIT_ASSERT( path1.back() == TI.Get( L"Q" ).bdg && path2.front() == TI.Get( L"Q" ).bdg );
	path1.sort();
	path2.sort();
	CPPUNIT_ASSERT( path1 == path2 );
}