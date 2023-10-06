#include "stdafx.h"
#include "TC_TrackerTechnodes.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTechnodes );

void TC_TrackerTechnodes::setUp()
{
	TI.Reset();
	tracker->Reset();
}

void TC_TrackerTechnodes::CargoIntoTechnode()
{
	//четность не совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2517" ) ), list<wstring>( 1, L"AR5" ) ) );
	TI.DetermineStripBusy( L"AS5", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2517" ) );
	TI.DetermineStripFree( L"AR5", *tracker );
	TI.DetermineStripFree( L"AS5", *tracker );
	tracker->Reset();

	//четность совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2518" ) ), list<wstring>( 1, L"AR5" ) ) );
	TI.DetermineStripBusy( L"AS5", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	tunity = tunities.back();
	tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2518" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoIntoTechnode2()
{
	list<wstring> tplace;
	tplace.push_back( L"AS5" );
	tplace.push_back( L"AR5" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2518" ) ), tplace ) );

	TI.DetermineStripFree( L"AS5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2518" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoOnTechnodeInput()
{
	//четность не совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2640" ) ), list<wstring>( 1, L"BA5" ) ) );
	TI.DetermineStripBusy( L"AZ5", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2640" ) );
	TI.DetermineStripFree( L"BA5", *tracker );
	TI.DetermineStripFree( L"AZ5", *tracker );
	tracker->Reset();

	//четность совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2641" ) ), list<wstring>( 1, L"BA5" ) ) );
	TI.DetermineStripBusy( L"AZ5", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	tunity = tunities.back();
	tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2641" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoOnTechnodeOutput()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2517" ) ), list<wstring>( 1, L"AZ5" ) ) );
	TI.DetermineStripBusy( L"BA5", *tracker );
	TI.DetermineStripFree( L"AZ5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2518" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoOnTechnodeOutput2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2518" ) ), list<wstring>( 1, L"AZ5" ) ) );
	TI.DetermineStripBusy( L"BA5", *tracker );
	TI.DetermineStripFree( L"AZ5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2518" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoRightdirRoutedDeparture()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2518" ) ), list<wstring>( 1, L"BC5" ) ) );
	TI.DetermineRouteSet( L"Head520->BD5", *tracker );
	TI.DetermineStripBusy( L"BD5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2518" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoRightdirRoutedDeparture2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2518" ) ), list<wstring>( 1, L"BC5" ) ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"BC5" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId = trainPtr->GetId();
	TI.DetermineRouteSet( L"Head520->BD5", *tracker );
	TI.DetermineStripBusy( L"BD5", *tracker );
	TI.DetermineHeadClose( L"Head520", *tracker );
	TI.DetermineStripBusy( L"AY5", *tracker );
	TI.DetermineStripFree( L"BD5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto ay5trainPtr = trainCont->GetUnity( TI.Get( L"AY5" ).bdg );
	CPPUNIT_ASSERT( ay5trainPtr );
	auto ay5descrPtr = ay5trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( ay5trainPtr->GetId() == trainId && ay5descrPtr && *ay5descrPtr == TrainDescr( L"2518" ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoWrongdirRoutedDeparture()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2518" ) ), list<wstring>( 1, L"BC5" ) ) );
	TI.DetermineRouteSet( L"Head521->AR5", *tracker );
	TI.DetermineStripBusy( L"BB5", *tracker );
	TI.DetermineStripFree( L"BC5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2518" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoRightdirUnroutedDeparture()
{
	TrainDescr tdescr( L"2518" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( tdescr ) ), list<wstring>( 1, L"BC5" ) ) );
	TI.DetermineStripBusy( L"BD5", *tracker );

	//отъезжает именно маневровая..
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto bd5Unity = trainCont->GetUnity( TI.Get( L"BD5" ).bdg );
	CPPUNIT_ASSERT( bd5Unity && !bd5Unity->IsTrain() );
	auto bc5Unity = trainCont->GetUnity( TI.Get( L"BC5" ).bdg );
	CPPUNIT_ASSERT( bc5Unity && bc5Unity->IsTrain() );
	auto bc5UnityIden = bc5Unity->GetId();

	//..но потом становится поездом
	TI.DetermineStripFree( L"BC5", *tracker );
	
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = trainCont->GetUnity( TI.Get( L"BD5" ).bdg );
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == tdescr );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::CargoWrongdirUnroutedDeparture()
{
	TrainDescr tdescr( L"2518" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( tdescr ) ), list<wstring>( 1, L"BC5" ) ) );
	TI.DetermineStripBusy( L"BB5", *tracker );

	//отъезжает именно маневровая..
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto bb5Unity = trainCont->GetUnity( TI.Get( L"BB5" ).bdg );
	CPPUNIT_ASSERT( bb5Unity && !bb5Unity->IsTrain() );
	auto bc5Unity = trainCont->GetUnity( TI.Get( L"BC5" ).bdg );
	CPPUNIT_ASSERT( bc5Unity && bc5Unity->IsTrain() );
	auto bc5UnityIden = bc5Unity->GetId();

	//..но потом становится поездом
	TI.DetermineStripFree( L"BC5", *tracker );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = trainCont->GetUnity( TI.Get( L"BB5" ).bdg );
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == tdescr );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::DisformOnTechnodeWOOddnessIgnore()
{
	//четность не совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2517" ) ), list<wstring>( 1, L"AW5" ) ) );
	TI.DetermineStripBusy( L"AX5", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto aw5UnityPtr = trainCont->GetUnity( TI.Get( L"AW5" ).bdg );
	CPPUNIT_ASSERT( aw5UnityPtr );
	auto tdescrPtr = aw5UnityPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2517" ) );
	auto ax5UnityPtr = trainCont->GetUnity( TI.Get( L"AX5" ).bdg );
	CPPUNIT_ASSERT( ax5UnityPtr && !ax5UnityPtr->IsTrain() );
	TI.DetermineStripFree( L"AW5", *tracker );
	TI.DetermineStripFree( L"AX5", *tracker );
	tracker->Reset();

	//четность совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2518" ) ), list<wstring>( 1, L"AW5" ) ) );
	TI.DetermineStripBusy( L"AX5", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2518" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdIntoTechnode()
{
	//четность не совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8053" ) ), list<wstring>( 1, L"AR5" ) ) );
	TI.DetermineStripBusy( L"AS5", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8053" ) );
	TI.DetermineStripFree( L"AR5", *tracker );
	TI.DetermineStripFree( L"AS5", *tracker );
	tracker->Reset();

	//четность совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), list<wstring>( 1, L"AR5" ) ) );
	TI.DetermineStripBusy( L"AS5", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	tunity = tunities.back();
	tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8054" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdIntoTechnode2()
{
	list<wstring> tplace;
	tplace.push_back( L"AS5" );
	tplace.push_back( L"AR5" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), tplace ) );

	TI.DetermineStripFree( L"AS5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8054" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdOnTechnodeInput()
{
	//четность не совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), list<wstring>( 1, L"BA5" ) ) );
	TI.DetermineStripBusy( L"AZ5", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8054" ) );
	TI.DetermineStripFree( L"BA5", *tracker );
	TI.DetermineStripFree( L"AZ5", *tracker );
	tracker->Reset();

	//четность совпадает с четностью светофора
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8053" ) ), list<wstring>( 1, L"BA5" ) ) );
	TI.DetermineStripBusy( L"AZ5", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	tunity = tunities.back();
	tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8053" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdOnTechnodeOutput()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8053" ) ), list<wstring>( 1, L"AZ5" ) ) );
	TI.DetermineStripBusy( L"BA5", *tracker );
	TI.DetermineStripFree( L"AZ5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8054" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdOnTechnodeOutput2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), list<wstring>( 1, L"AZ5" ) ) );
	TI.DetermineStripBusy( L"BA5", *tracker );
	TI.DetermineStripFree( L"AZ5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8054" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdRightdirRoutedDeparture()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), list<wstring>( 1, L"BC5" ) ) );
	TI.DetermineRouteSet( L"Head520->BD5", *tracker );
	TI.DetermineStripBusy( L"BD5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8054" ) );
	CPPUNIT_ASSERT( tunity->GetOddness() == Oddness::EVEN );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdWrongdirRoutedDeparture()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), list<wstring>( 1, L"BC5" ) ) );
	TI.DetermineRouteSet( L"Head521->AR5", *tracker );
	TI.DetermineStripBusy( L"BB5", *tracker );
	TI.DetermineStripFree( L"BC5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	CPPUNIT_ASSERT( !tunity->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdRightdirUnroutedDeparture()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), list<wstring>( 1, L"BC5" ) ) );
	TI.DetermineStripBusy( L"BD5", *tracker );
	TI.DetermineStripFree( L"BC5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	CPPUNIT_ASSERT( !tunity->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdWrongdirUnroutedDeparture()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), list<wstring>( 1, L"BC5" ) ) );
	TI.DetermineStripBusy( L"BB5", *tracker );
	TI.DetermineStripFree( L"BC5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	CPPUNIT_ASSERT( !tunity->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::HouseholdADInputOnTechnode()
{
	list<wstring> tplace;
	tplace.push_back( L"BB5" );
	tplace.push_back( L"BC5" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8054" ) ), tplace ) );
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"BB5", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto tdescrPtr = tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8054" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTechnodes::DeparturingWhileTailInput()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"AS5" ) ); //маневровая
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2052" ) ), list<wstring>( 1, L"BC5" ) ); //четный поезд

	auto trainPtr = trainCont->GetUnity( TI.Get( L"BC5" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId = trainPtr->GetId();

	//отправление поезда
	TI.DetermineRouteSet( L"Head520->BD5", *tracker );
	TI.DetermineStripBusy( L"BD5", *tracker );
	TI.DetermineHeadClose( L"Head520", *tracker );

	//подъезд маневровой (слияния нет)
	TI.DetermineStripBusy( L"BB5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto testTrainPtr = trainCont->GetUnity( trainId );
	CPPUNIT_ASSERT( testTrainPtr && testTrainPtr->GetId() == trainId && testTrainPtr->Have( TI.Get( L"BC5" ).bdg ) && testTrainPtr->Have( TI.Get( L"BD5" ).bdg ) );

	TI.DetermineStripFree( L"AS5", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto testTrainPtr2 = trainCont->GetUnity( trainId );
	CPPUNIT_ASSERT( testTrainPtr2 && testTrainPtr2->GetId() == trainId && testTrainPtr2->Have( TI.Get( L"BC5" ).bdg ) && testTrainPtr2->Have( TI.Get( L"BD5" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}