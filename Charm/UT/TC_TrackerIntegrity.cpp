#include "stdafx.h"
#include "TC_TrackerIntegrity.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerIntegrity );
void TC_TrackerIntegrity::setUp()
{
	TI.Reset();
	tracker->Reset();
}

void TC_TrackerIntegrity::RecoveringIntegrity()
{
	TI.DetermineSwitchPlus( L"51", *tracker );
	TI.DetermineSwitchMinus( L"52", *tracker );
	TI.DetermineSwitchMinus( L"53", *tracker );

	TI.DetermineStripBusy( L"W5", *tracker, false );
	TI.DetermineStripBusy( L"V5", *tracker, false );
	TI.DetermineStripBusy( L"A5", *tracker, false );
	TI.DetermineStripBusy( L"B5", *tracker, false );
	TI.DetermineStripBusy( L"D5", *tracker, false );
	TI.FlushData( *tracker, false );

	SetRandomEvenInfo( TI.Get( L"W5" ).bdg );

	TI.DetermineStripFree( L"W5", *tracker, false );
	TI.DetermineStripFree( L"V5", *tracker, false );
	TI.DetermineStripFree( L"A5", *tracker, false );
	TI.DetermineStripFree( L"B5", *tracker, false );
	TI.DetermineStripFree( L"D5", *tracker, false );
	TI.FlushData( *tracker, false );

	list<wstring> overlappedPlace;
	overlappedPlace.push_back( L"B5" );
	overlappedPlace.push_back( L"D5" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, overlappedPlace ) );
	
	list<wstring> unityPlace;
	unityPlace.push_back( L"G5" );
	unityPlace.push_back( L"H5" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, unityPlace ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"B5", *tracker, false );
	TI.DetermineStripBusy( L"D5", *tracker, false );
	TI.DetermineStripBusy( L"H5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() ); //здесь целостность восстановлена
}

void TC_TrackerIntegrity::RecoveringIntegrity2()
{
	TI.DetermineSwitchMinus( L"58", *tracker );
	TI.DetermineSwitchMinus( L"55", *tracker );

	list<wstring> tplace;
	tplace.push_back( L"AK5" );
	tplace.push_back( L"Y5" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"100" ) ), tplace ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"102" ) ), list<wstring>( 1, L"Z5" ) ) );
	auto recovTrainPtr = trainCont->GetUnity( TI.Get( L"Z5" ).bdg );
	auto recovIden = recovTrainPtr->GetId();
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"AB5" ) ) );
	TI.DetermineStripFree( L"Z5", *tracker );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"AK5", *tracker, false );
	TI.DetermineStripBusy( L"Y5", *tracker, false );
	TI.DetermineStripBusy( L"Z5", *tracker, false );
	TI.DetermineStripBusy( L"AC5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::RecoveringIntegrity3()
{
	TI.DetermineSwitchPlus( L"51", *tracker );
	TI.DetermineSwitchMinus( L"52", *tracker );
	TI.DetermineSwitchMinus( L"53", *tracker );

	TI.DetermineStripBusy( L"W5", *tracker );
	SetRandomEvenInfo( TI.Get( L"W5" ).bdg );
	TI.DetermineStripFree( L"W5", *tracker );

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"262" ) ), list<wstring>( 1, L"F5" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"H5" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"G5" ) ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"H5", *tracker, false );
	TI.DetermineStripBusy( L"V5", *tracker, false );
	TI.DetermineStripBusy( L"A5", *tracker, false );
	TI.DetermineStripBusy( L"B5", *tracker, false );
	TI.DetermineStripBusy( L"D5", *tracker, false );
	TI.DetermineStripBusy( L"F5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::RecoveringIntegrity4()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"N4" ) ) );
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"N4", *tracker );
	TI.DetermineSwitchMinus( L"46", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"O4" );
	tplace.push_back( L"R4" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace ) );
	list<wstring> tplace2;
	tplace2.push_back( L"F4" );
	tplace2.push_back( L"G4" );
	tplace2.push_back( L"H4" );
	tplace2.push_back( L"K4" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"160" ) ), tplace2 ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"I4" ) ) );

	tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"O4", *tracker, false );
	TI.DetermineStripFree( L"R4", *tracker, false );
	TI.DetermineStripBusy( L"S4", *tracker, false );
	TI.DetermineStripFree( L"K4", *tracker, false );
	TI.DetermineStripBusy( L"L4", *tracker, false );
	TI.DetermineSwitchMinus( L"45", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::RecoveringIntegrity5()
{
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"408" ) ), list<wstring>( 1, L"S4" ) ) );
	TI.DetermineStripFree( L"S4", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"640" ) ), list<wstring>( 1, L"F4" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"G4" ) ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.DetermineStripBusy( L"K4", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::RecoveringIntegrity6()
{
	TI.DetermineSwitchMinus( L"46", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"204" ) ), list<wstring>( 1, L"N4" ) ) );
	TI.DetermineStripFree( L"N4", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"L4" );
	tplace.push_back( L"H4" );
	tplace.push_back( L"G4" );
	tplace.push_back( L"E4" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"560" ) ), tplace ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.DetermineStripFree( L"E4", *tracker, false );
	TI.DetermineStripBusy( L"U4", *tracker, false );
	TI.DetermineSwitchPlus( L"44", *tracker, false );
	TI.DetermineSwitchMinus( L"43", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::RecoveringIntegrity7()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	list<wstring> tplace( 1, L"G" );
	tplace.emplace_back( L"E" );
	LocateUnityByForce( nullptr, tplace );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.DetermineStripFree( L"E", *tracker, false );
	TI.FlushData( *tracker, false );

	list<wstring> tplace2( 1, L"A" );
	tplace2.emplace_back( L"Q" );
	tplace2.emplace_back( L"W" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2517" ) ), tplace2 );

	TI.DetermineStripBusy( L"H", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerIntegrity::RecoveringIntegrity8()
{
	TI.DetermineSwitchMinus( L"KJ1", *tracker );
	TI.DetermineSwitchMinus( L"KJ3", *tracker );
	TI.DetermineSwitchPlus( L"KJ7", *tracker );

	LocateUnityByForce( nullptr, list<wstring>( 1, L"KJ_3C" ) );

	list<wstring> trainPlace;
	trainPlace.push_back( L"KJ_NAP" );
	trainPlace.push_back( L"KJ_N1P_KRA" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"3668" ) ), trainPlace ); //четный поезд

	list<wstring> notrainPlace;
	notrainPlace.push_back( L"KJ_1-5SP" );
	notrainPlace.push_back( L"KJ_1-5SP:1-" );
	notrainPlace.push_back( L"KJ_3SP:3-" );
	notrainPlace.push_back( L"KJ_3SP" );
	notrainPlace.push_back( L"KJ_7-9SP" );
	notrainPlace.push_back( L"KJ_7-9SP:7+" );
	LocateUnityByForce( nullptr, notrainPlace );

	TI.DetermineStripFree( L"KJ_7-9SP", *tracker, false );
	TI.DetermineStripFree( L"KJ_7-9SP:7+", *tracker, false );
	TI.DetermineStripFree( L"KJ_N1P_KRA", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"KJ_7-9SP", *tracker, false );
	TI.DetermineStripBusy( L"KJ_7-9SP:7+", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerIntegrity::RecoveringIntegrity9()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"12", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"14", *tracker );

	list<wstring> tplace1;
	tplace1.push_back( L"Q" );
	tplace1.push_back( L"A" );
	tplace1.push_back( L"D" );
	tplace1.push_back( L"S" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace1 ) );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.DetermineStripFree( L"S", *tracker, false );
	TI.FlushData( *tracker, false );

	list<wstring> tplace2;
	tplace2.push_back( L"X" );
	tplace2.push_back( L"G" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2523" ) ) , tplace2 ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"J" ) ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"X" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto trainId = trainPtr->GetId();

	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineStripFree( L"J", *tracker );
	auto curtrainPtr = trainCont->GetUnity( trainId );
	CPPUNIT_ASSERT( curtrainPtr );
	CPPUNIT_ASSERT( curtrainPtr->GetPlace().size() == 2 && curtrainPtr->Have( TI.Get( L"X" ).bdg ) && curtrainPtr->Have( TI.Get( L"G" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::BreakingIntegrityByIdensOrder()
{
	TI.DetermineSwitchPlus( L"21", *tracker );
	TI.DetermineSwitchMinus( L"22", *tracker );
	TI.DetermineSwitchMinus( L"23", *tracker );

	//порядок создания ПЕ важен, т.к. влияет на распределение идентификаторов
	TI.DetermineStripBusy( L"F2", *tracker );
	TI.DetermineStripBusy( L"D2", *tracker );
	TI.DetermineStripBusy( L"E2", *tracker );
	TI.DetermineStripBusy( L"O2", *tracker );
	TI.DetermineStripBusy( L"P2", *tracker );
	TI.DetermineStripBusy( L"R2", *tracker );

	auto tunities = trainCont->GetUnities();
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 && tplace.front() == TI.Get( L"F2" ).bdg )
			SetRandomEvenInfo( tunity );
	}

	TI.DetermineSwitchPlus( L"22", *tracker, false );
	TI.DetermineSwitchPlus( L"23", *tracker, false );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.DetermineStripBusy( L"D2", *tracker, false );
	TI.DetermineStripBusy( L"M2", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::SwitchToggleBreakIntegrity()
{
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );

	list<wstring> tplace;
	tplace.push_back( L"S4" );
	tplace.push_back( L"F4" );
	tplace.push_back( L"G4" );
	tplace.push_back( L"H4" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"314" ) ), tplace ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"R4" ) ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineSwitchPlus( L"43", *tracker, false );
	TI.DetermineStripBusy( L"D4", *tracker, false );
	TI.DetermineStripBusy( L"E4", *tracker, false );
	TI.DetermineStripFree( L"F4", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::BreakIntegrityOnMerging()
{
	TI.DetermineSwitchPlus( L"21", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"R2" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"O2" );
	tplace.push_back( L"M2" );
	tplace.push_back( L"D2" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"E2" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"F2" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"560" ) ), list<wstring>( 1, L"L2" ) ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineSwitchPlus( L"23", *tracker, false );
	TI.DetermineSwitchPlus( L"22", *tracker, false );
	TI.DetermineStripFree( L"R2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::BreakIntegrityOnMerging2()
{
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8671" ) ), list<wstring>( 1, L"A4" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"B4" );
	tplace.push_back( L"C4" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"D4" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"E4" ) ) );
	TI.DetermineStripFree( L"E4", *tracker );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::ChainSplittingCausingUnintegrity()
{
	TI.DetermineSwitchMinus( L"17", *tracker );

	TI.DetermineStripBusy( L"AC", *tracker );
	SetRandomEvenInfo( TI.Get( L"AC" ).bdg );
	TI.DetermineStripBusy( L"Y", *tracker );
	TI.DetermineStripBusy( L"Z", *tracker );
	TI.DetermineStripBusy( L"AA", *tracker );
	TI.DetermineStripBusy( L"AB", *tracker );
	SetRandomEvenInfo( TI.Get( L"Y" ).bdg );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"AB", *tracker, false );
	TI.DetermineStripFree( L"AA", *tracker, false );
	TI.DetermineStripBusy( L"AG", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::WrongCheckingLiveTrainWhileRecovery()
{
	TI.DetermineSwitchMinus( L"46", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"204" ) ), list<wstring>( 1, L"N4" ) ) );
	TI.DetermineStripFree( L"N4", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"L4" );
	tplace.push_back( L"H4" );
	tplace.push_back( L"G4" );
	tplace.push_back( L"E4" );
	tplace.push_back( L"D4" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"560" ) ), tplace ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.DetermineStripFree( L"E4", *tracker, false );
	TI.DetermineStripBusy( L"U4", *tracker, false );
	TI.DetermineSwitchPlus( L"44", *tracker, false );
	TI.DetermineSwitchMinus( L"43", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::BreakIntegrityOnMergingCausedByInfoChange()
{
	TI.DetermineSwitchMinus( L"17", *tracker );
	TI.DetermineStripBusy( L"Z", *tracker );
	TI.DetermineStripBusy( L"AA", *tracker );
	TI.DetermineStripBusy( L"AD", *tracker );
	TI.DetermineStripBusy( L"AE", *tracker );
	SetInfo( TI.Get( L"AA" ).bdg, TrainDescr( L"208" ) );
	SetInfo( TI.Get( L"AD" ).bdg, TrainDescr( L"208" ) ); //поезд не может смещаться на чужую ЦЗ

	auto trainPtr1 = trainCont->GetUnity( TI.Get( L"Z" ).bdg );
	CPPUNIT_ASSERT( trainPtr1 );
	CPPUNIT_ASSERT( trainPtr1->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( trainPtr1->Have( TI.Get( L"Z" ).bdg ) );
	CPPUNIT_ASSERT( trainPtr1->Have( TI.Get( L"AA" ).bdg ) );
	auto tdescrPtr = trainPtr1->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"208" ) );
	auto trainPtr2 = trainCont->GetUnity( TI.Get( L"AD" ).bdg );
	CPPUNIT_ASSERT( trainPtr2 );
	CPPUNIT_ASSERT( trainPtr2->GetPlace().size() == 2 );
	CPPUNIT_ASSERT( trainPtr2->Have( TI.Get( L"AD" ).bdg ) );
	CPPUNIT_ASSERT( trainPtr2->Have( TI.Get( L"AE" ).bdg ) );
	CPPUNIT_ASSERT( !trainPtr2->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::WrongTeleportOverBlockedStrips()
{
	list<wstring> tplace;
	tplace.push_back( L"Q5" );
	tplace.push_back( L"R5" );
	tplace.push_back( L"AO5" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"O5" ) ) );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto q5tunity = trainCont->GetUnity( TI.Get( L"Q5" ).bdg );
	auto o5tunity = trainCont->GetUnity( TI.Get( L"O5" ).bdg );
	CPPUNIT_ASSERT( q5tunity && o5tunity && q5tunity->GetId() != o5tunity->GetId() );

	TI.DetermineStripBusy( L"AO5", *tracker, false );
	TI.DetermineStripFree( L"O5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.front()->GetId() == q5tunity->GetId() && !tunities.front()->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::FarTeleportWhilePathDeficite()
{
	shared_ptr<const TestGuessNsiBasis> nsiBasisPtr( new TestGuessNsiBasis() );
	TrainFieldPtr tstFieldPtr( new TrainField( 3, nsiBasisPtr ) );
	TestTracker tstTracker( tstFieldPtr, TrackingConfiguration() );
	TI.CreateStation( *tstFieldPtr, true );
	tstTracker.Reset();
	const auto & tcontainer = tstTracker.GetTrainContainer();

	TI.DetermineSwitchPlus( L"11", tstTracker );
	list<wstring> tplace;
	tplace.push_back( L"K" );
	tplace.push_back( L"H" );
	tplace.push_back( L"W" );
	tplace.push_back( L"Q" );
	tplace.push_back( L"A" );
	LocateUnityByForce( nullptr, tplace, &tstTracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2517" ) ), list<wstring>( 1, L"D" ), &tstTracker );

	TI.DetermineStripFree( L"A", tstTracker, false );
	TI.DetermineStripFree( L"Q", tstTracker, false );
	TI.DetermineStripFree( L"H", tstTracker, false );
	TI.DetermineStripFree( L"D", tstTracker, false );
	TI.FlushData( tstTracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::TeleportIntegrity()
{
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"LS7" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"3331" ) ), list<wstring>( 1, L"LS5" ) ) );
	TI.DetermineStripBusy( L"LS3", *tracker );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIntegrity::BreakSequenceIntegrityDueToMultipath()
{
	TI.DetermineSwitchMinus( L"BA4", *tracker );
	TI.DetermineSwitchPlus( L"BA1", *tracker );
	list<wstring> tplace1;
	tplace1.push_back( L"BA_BV1" );
	tplace1.push_back( L"BA_NDP" );
	LocateUnityByForce( nullptr, tplace1 );
	list<wstring> tplace2;
	tplace2.push_back( L"BA_4SP" );
	tplace2.push_back( L"BA_4SP:4-" );
	tplace2.push_back( L"BA_1C" );
	tplace2.push_back( L"BA_1SP:1+" );
	tplace2.push_back( L"BA_1SP" );
	LocateUnityByForce( nullptr, tplace2 );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"BA_PDP", *tracker, false );
	TI.DetermineStripBusy( L"BA_NDP", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}