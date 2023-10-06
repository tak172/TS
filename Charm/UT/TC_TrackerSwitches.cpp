#include "stdafx.h"
#include "TC_TrackerSwitches.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerSwitches );

void TC_TrackerSwitches::setUp()
{
	TI.Reset();
}

void TC_TrackerSwitches::Teleport()
{
	TI.DetermineSwitchMinus( L"23", *tracker );
	TI.DetermineSwitchPlus( L"22", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"K2" ) ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initId = tunities.back()->GetId();

	TI.DetermineStripBusy( L"M2", *tracker );
	TI.DetermineStripFree( L"K2", *tracker );
	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto curtrain = tunities2.back();
	CPPUNIT_ASSERT( curtrain->GetId() == initId && curtrain->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSwitches::Teleport2()
{
	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineSwitchPlus( L"22", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"F2" ) ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initId = tunities.back()->GetId();

	TI.DetermineStripBusy( L"M2", *tracker );
	TI.DetermineStripFree( L"F2", *tracker );
	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto curtrain = tunities2.back();
	CPPUNIT_ASSERT( tunities2.back()->GetId() == initId && curtrain->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSwitches::BidTeleport()
{
	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineSwitchPlus( L"22", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"K2" ) ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initId = tunities.back()->GetId();

	TI.DetermineStripBusy( L"M2", *tracker );
	TI.DetermineStripFree( L"K2", *tracker );
	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto curtrain = tunities2.back();
	CPPUNIT_ASSERT( curtrain->GetId() != initId && !curtrain->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSwitches::BidTeleport2()
{
	TI.DetermineSwitchPlus( L"22", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"G2" ) ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initId = tunities.back()->GetId();

	TI.DetermineStripBusy( L"M2", *tracker );
	TI.DetermineStripFree( L"G2", *tracker );
	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto curtrain = tunities2.back();
	CPPUNIT_ASSERT( curtrain->GetId() != initId && !curtrain->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSwitches::Occupation()
{
	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineSwitchPlus( L"23", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"L2" ) ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initId = tunities.back()->GetId();
	list<wstring> tplace2;
	tplace2.push_back( L"E2" );
	tplace2.push_back( L"D2" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripFree( L"L2", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"F2", *tracker );

	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto curtrain = tunities2.back();
	CPPUNIT_ASSERT( curtrain->GetId() == initId && curtrain->IsTrain() );
}

void TC_TrackerSwitches::Occupation2()
{
	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineSwitchPlus( L"23", *tracker );

	list<wstring> tplace;
	tplace.push_back( L"E2" );
	tplace.push_back( L"D2" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"4458" ) ), tplace ) );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initId = tunities.back()->GetId();
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2131" ) ), list<wstring>( 1, L"L2" ) ) );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripFree( L"L2", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"F2", *tracker );

	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto curtrain = tunities2.back();
	CPPUNIT_ASSERT( curtrain->GetId() == initId && curtrain->IsTrain() );
}