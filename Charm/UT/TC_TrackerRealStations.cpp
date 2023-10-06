#include "stdafx.h"
#include "TC_TrackerRealStations.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerRealStations );

void TC_TrackerRealStations::setUp()
{
	TI.Reset();
	tracker->Reset();
}

void TC_TrackerRealStations::JelgavaSequencyIntegrity()
{
	TI.DetermineSwitchPlus( L"JE12", *tracker );
	TI.DetermineSwitchPlus( L"JE56", *tracker );
	TI.DetermineSwitchMinus( L"JE58", *tracker );
	TI.DetermineSwitchPlus( L"JE62", *tracker );
	TI.DetermineSwitchMinus( L"JE76", *tracker );

	LocateUnityByForce( nullptr, list<wstring>(1, L"JE_62SP") );

	list<wstring> tplace;
	tplace.push_back( L"JE_12SP" );
	tplace.push_back( L"JE_12SP:12+" );
	tplace.push_back( L"JE_12/65P" );
	tplace.push_back( L"JE_56SP:56+" );
	tplace.push_back( L"JE_56SP" );
	tplace.push_back( L"JE_58SP" );
	tplace.push_back( L"JE_58SP:58-" );
	tplace.push_back( L"JE_62SP:62+" );
	LocateUnityByForce( nullptr, tplace );

	//ситуация нарушения целостности:
	TI.DetermineStripFree( L"JE_12SP", *tracker, false );
	TI.DetermineStripBusy( L"JE_76SP", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::KarsavaRecoveryIntegrity()
{
	TI.DetermineSwitchPlus( L"KR13", *tracker );
	TI.DetermineSwitchPlus( L"KR15", *tracker );
	TI.DetermineSwitchMinus( L"KR1", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"KR_13-17SP:15+" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"KR_13-17SP:15-" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"KR_13-17SP:13-" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"KR_1SP:1+" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"KR_1SP" ) ) );

	//под восстановление:
	TI.DetermineStripFree( L"KR_13-17SP:15+", *tracker );
	TI.DetermineStripFree( L"KR_13-17SP:15-", *tracker );
	TI.DetermineStripFree( L"KR_13-17SP:13-", *tracker );

	//ситуация нарушения целостности
	TI.DetermineStripBusy( L"KR_13-17SP:15+", *tracker, false );
	TI.DetermineStripBusy( L"KR_13-17SP", *tracker, false );
	TI.DetermineStripBusy( L"KR_13-17SP:13+", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::KarsavaRecoveryIntegrity2()
{
	TI.DetermineSwitchPlus( L"KR2", *tracker );
	TI.DetermineSwitchMinus( L"KR4", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"KR_2-4SP" );
	tplace.push_back( L"KR_2-4SP:2+" );
	tplace.push_back( L"KR_2-4SP:2+,4-" );
	LocateUnityByForce( nullptr, tplace );
	TI.DetermineStripFree( L"KR_2-4SP", *tracker, false );
	TI.DetermineStripFree( L"KR_2-4SP:2+", *tracker, false );
	TI.DetermineStripFree( L"KR_2-4SP:2+,4-", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineSwitchMinus( L"KR2", *tracker );

	TI.DetermineStripBusy( L"KR_2-4SP", *tracker, false );
	TI.DetermineStripBusy( L"KR_2-4SP:2-", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::KarsavaRecoveryIntegrity3()
{
	TI.DetermineSwitchPlus( L"KR19", *tracker );

	list<wstring> tplace;
	tplace.push_back( L"KR_19SP" );
	tplace.push_back( L"KR_19SP:19+" );
	tplace.push_back( L"KR_2C" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"KR_2AC" ) ) );
	TI.DetermineStripFree( L"KR_19SP", *tracker, false );
	TI.DetermineStripFree( L"KR_19SP:19+", *tracker, false );
	TI.DetermineStripFree( L"KR_2C", *tracker, false );
	TI.DetermineStripFree( L"KR_2AC", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineSwitchMinus( L"KR19", *tracker );

	TI.DetermineStripBusy( L"KR_19SP:19-", *tracker, false );
	TI.DetermineStripBusy( L"KR_19SP", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::KraslavaSequencyIntegrity ()
{
	TI.DetermineSwitchMinus( L"KS20", *tracker );
	TI.DetermineSwitchMinus( L"KS12", *tracker );
	TI.DetermineSwitchMinus( L"KS14", *tracker );
	TI.DetermineSwitchPlus( L"KS10", *tracker );

	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"KS_7AC" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"KS_12-20SP:12-,$" );
	tplace.push_back( L"KS_12-20SP:12-" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	list<wstring> tplace2;
	tplace2.push_back( L"KS_12-20SP" );
	tplace2.push_back( L"KS_10SP" );
	tplace2.push_back( L"KS_10SP:10+" );
	tplace2.push_back( L"KS_14SP:14-" );
	tplace2.push_back( L"KS_14SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"KS_12-20SP:12-,$", *tracker, false );
	TI.DetermineStripFree( L"KS_12-20SP:12-", *tracker, false );
	TI.DetermineStripFree( L"KS_12-20SP", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::KemeriOccupancyIntegrity()
{
	list<wstring> tplace1, tplace2, tplace3;
	tplace1.push_back( L"KM_CP" );
	tplace1.push_back( L"KM_CP:4-" );
	tplace1.push_back( L"KM_3p" );
	tplace2.push_back( L"KM_CP:4+" );
	tplace2.push_back( L"KM_CP:4+,8+" );
	tplace3.push_back( L"KM_TK_CKPU1_T" );
	tplace3.push_back( L"TK_NDP" );
	TI.DetermineSwitchMinus( L"KM4", *tracker );
	TI.DetermineSwitchMinus( L"KM8", *tracker );
	LocateUnityByForce( nullptr, tplace1 );
	LocateUnityByForce( nullptr, tplace2 );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8245" ) ), tplace3 );
	auto tunities = trainCont->GetUnities();

	TI.DetermineSwitchPlus( L"KM4", *tracker, false );
	TI.DetermineSwitchPlus( L"KM8", *tracker, false );
	TI.DetermineStripFree( L"KM_CP:4-", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::LiepajaSequencyIntegrity()
{
	TI.DetermineSwitchPlus( L"LP75", *tracker );
	TI.DetermineSwitchMinus( L"LP73", *tracker );
	TI.DetermineSwitchMinus( L"LP71", *tracker );
	TI.DetermineSwitchMinus( L"LP69", *tracker );
	TI.DetermineSwitchMinus( L"LP65", *tracker );

	list<wstring> tplace;
	tplace.push_back( L"LP_75-77SP:75+" );
	tplace.push_back( L"LP_75-77SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"LP_12C", *tracker, false );
	TI.DetermineStripBusy( L"LP_69-73SP:69-,71-,73-", *tracker, false );
	TI.DetermineStripBusy( L"LP_69-73SP:69-,71-", *tracker, false );
	TI.DetermineStripBusy( L"LP_69-73SP:69-", *tracker, false );
	TI.DetermineStripBusy( L"LP_69-73SP", *tracker, false );
	TI.DetermineStripBusy( L"LP_65SP", *tracker, false );
	TI.DetermineStripBusy( L"LP_65SP:65-", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LiepajaChainsIntegrity()
{
	TI.DetermineSwitchMinus( L"LP30", *tracker );
	TI.DetermineSwitchPlus( L"LP32", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"LP_9C" ) ) );
	list<wstring> tplace( 1, L"LP_30-32SP:30-" );
	tplace.push_back( L"LP_30-32SP:30-,32+" );
	
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );	
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"LP_30-32SP" ) ) );

	TI.DetermineStripBusy( L"LP_30-32SP", *tracker, false );
	TI.DetermineStripBusy( L"LP_30-32SP:30-", *tracker, false );
	TI.DetermineStripBusy( L"LP_30-32SP:30-,32+", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LiepajaChainsIntegrity2()
{
	TI.DetermineSwitchMinus( L"LP73", *tracker );
	TI.DetermineSwitchMinus( L"LP75", *tracker );
	TI.DetermineSwitchPlus( L"LP77", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"LP_69-73SP:69-,71-" ) ) );
	list<wstring> tplace1;
	tplace1.push_back( L"LP_69-73SP:69-,71-,73-" );
	tplace1.push_back( L"LP_75-77SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace1 ) );
	list<wstring> tplace2;
	tplace2.push_back( L"LP_75-77SP:75-" );
	tplace2.push_back( L"LP_75-77SP:75-,77+" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );

	TI.DetermineStripBusy( L"LP_69-73SP:69-,71-", *tracker, false );
	TI.DetermineStripBusy( L"LP_69-73SP:69-,71-,73-", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneChainsIntegrity()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_1-3SP:1+" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_1-3SP" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_2C" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"131" ) ), list<wstring>( 1, L"LG_NDP" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_LI3" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"LG_1-3SP:1+,3-" ) ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineSwitchPlus( L"LG1", *tracker, false );
	TI.DetermineSwitchMinus( L"LG3", *tracker, false );
	TI.DetermineStripBusy( L"LG_1C", *tracker, false );
	TI.DetermineStripBusy( L"LG_1-3SP:1+,3+", *tracker, false );
	TI.DetermineStripBusy( L"LG_1-3SP:1+,3-", *tracker, false );
	TI.DetermineStripBusy( L"LG_1-3SP:1+", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI1", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI5", *tracker, false );
	TI.DetermineStripFree( L"LG_1-3SP:1-", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneChainsIntegrity2()
{
	TI.DetermineSwitchPlus( L"LG5", *tracker );
	TI.DetermineSwitchPlus( L"LG6", *tracker );
	TI.DetermineSwitchPlus( L"LG14", *tracker );

	list<wstring> tplace1;
	tplace1.push_back( L"LG_PDP" );
	tplace1.push_back( L"LG_6-8SP" );
	tplace1.push_back( L"LG_6-8SP:6+" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2821" ) ), tplace1 ) );
	list<wstring> tplace2;
	tplace2.push_back( L"LG_SL5" );
	tplace2.push_back( L"LG_SL7" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), tplace2 ) );
	list<wstring> tplace3;
	tplace3.push_back( L"LG_14SP:14+" );
	tplace3.push_back( L"LG_14SP" );
	tplace3.push_back( L"LG_3C" );
	tplace3.push_back( L"LG_5SP:5+" );
	tplace3.push_back( L"LG_5SP" );
	tplace3.push_back( L"LG_1-3SP:1+,3-" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace3 ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"LG_SL9" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"LG_2C" ) ) );

	TI.DetermineStripBusy( L"LG_6-8SP:6-", *tracker, false );
	TI.DetermineStripFree( L"LG_6-8SP:6+", *tracker, false );
	TI.DetermineSwitchMinus( L"LG6", *tracker, false );
	TI.DetermineSwitchOutOfControl( L"LG8", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneChainsIntegrity3()
{
	TI.DetermineSwitchPlus( L"LG6", *tracker );
	TI.DetermineSwitchPlus( L"LG8", *tracker );

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"258" ) ), list<wstring>( 1, L"LG_SL5" ) ) );
	list<wstring> tplace1;
	tplace1.push_back( L"LG_SL9" );
	tplace1.push_back( L"LG_SL7" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"309" ) ), tplace1 ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"LG_1C" ) ) );

	TI.DetermineStripBusy( L"LG_SL5", *tracker, false );
	TI.DetermineStripBusy( L"LG_PDP", *tracker, false );
	TI.DetermineStripBusy( L"LG_6-8SP", *tracker, false );
	TI.DetermineStripBusy( L"LG_6-8SP:6+", *tracker, false );
	TI.DetermineStripBusy( L"LG_6-8SP:6+,8+", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneRecoveryIntegrity()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"258" ) ), list<wstring>( 1, L"LG_LI3" ) ) );
	TI.DetermineStripFree( L"LG_LI3", *tracker ); //под восстановление
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_LI1" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"620" ) ), list<wstring>( 1, L"LG_LI5" ) ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"LG_LI1", *tracker, false );
	TI.DetermineStripFree( L"LG_LI3", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI5", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneRecoveryIntegrity2()
{
	TI.DetermineSwitchMinus( L"LG6", *tracker );
	TI.DetermineSwitchPlus( L"LG14", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_14SP" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_14SP:14+" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"LG_PDP" );
	tplace.push_back( L"LG_6-8SP" );
	tplace.push_back( L"LG_6-8SP:6-" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"LG_6-8SP:6-", *tracker );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneRecoveryIntegrity3()
{
	TI.DetermineSwitchPlus( L"LG1", *tracker );
	TI.DetermineSwitchMinus( L"LG3", *tracker );
	TI.DetermineSwitchPlus( L"LG5", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"LG_LI5" );
	tplace.push_back( L"LG_LI3" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"204" ) ), tplace ) );
	list<wstring> tplace2;
	tplace2.push_back( L"LG_5SP:5+" );
	tplace2.push_back( L"LG_5SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace2 ) );

	//под восстановление:
	list<wstring> tplace3;
	tplace3.push_back( L"LG_NDP" );
	tplace3.push_back( L"LG_1-3SP" );
	tplace3.push_back( L"LG_1-3SP:1+" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"707" ) ), tplace3 ) );
	TI.DetermineStripFree( L"LG_NDP", *tracker, false );
	TI.DetermineStripFree( L"LG_1-3SP", *tracker, false );
	TI.DetermineStripFree( L"LG_1-3SP:1+", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"LG_LI1", *tracker, false );
	TI.DetermineStripBusy( L"LG_NDP", *tracker, false );
	TI.DetermineStripBusy( L"LG_1-3SP", *tracker, false );
	TI.DetermineStripFree( L"LG_1C", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneRecoveryIntegrity4()
{
	TI.DetermineSwitchPlus( L"LG14", *tracker );
	TI.DetermineSwitchMinus( L"LG6", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"LG_3C" );
	tplace.push_back( L"LG_14SP" );
	tplace.push_back( L"LG_14SP:14+" );
	tplace.push_back( L"LG_6-8SP:6-" );
	tplace.push_back( L"LG_6-8SP" );
	tplace.push_back( L"LG_PDP" );
	tplace.push_back( L"LG_SL9" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"203" ) ), tplace ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_SL5" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"640" ) ), list<wstring>( 1, L"LG_SL7" ) ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"LG_SL5", *tracker, false );
	TI.DetermineStripFree( L"LG_SL9", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneRecoveryIntegrity5()
{
	TI.DetermineSwitchPlus( L"LG6", *tracker );
	TI.DetermineSwitchPlus( L"LG8", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_SL7" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"309" ) ), list<wstring>( 1, L"LG_SL9" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"LG_1C" );
	tplace.push_back( L"LG_6-8SP:6+,8+" );
	tplace.push_back( L"LG_6-8SP:6+" );
	tplace.push_back( L"LG_6-8SP" );
	tplace.push_back( L"LG_PDP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"203" ) ), tplace ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"LG_SL5", *tracker, false );
	TI.DetermineStripFree( L"LG_SL7", *tracker, false );
	TI.DetermineStripFree( L"LG_PDP", *tracker, false );
	TI.DetermineStripFree( L"LG_6-8SP", *tracker, false );
	TI.DetermineStripFree( L"LG_6-8SP:6+", *tracker, false );
	TI.DetermineStripFree( L"LG_6-8SP:6+,8+", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneRecoveryIntegrity6()
{
	TI.DetermineSwitchPlus( L"LG1", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"LG_LI5" );
	tplace.push_back( L"LG_LI3" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"422" ) ), tplace ) );
	list<wstring> tplace2;
	tplace2.push_back( L"LG_NDP" );
	tplace2.push_back( L"LG_1-3SP" );
	tplace2.push_back( L"LG_1-3SP:1+" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"LG_LI3", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI1", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"LG_LI3", *tracker );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneStripUniquenessIntegrity()
{
	TI.DetermineSwitchPlus( L"LG6", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"LG_6-8SP" );
	tplace.push_back( L"LG_6-8SP:6+" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace ) ); //под восстановление
	TI.DetermineStripFree( L"LG_6-8SP", *tracker, false );
	TI.DetermineStripFree( L"LG_6-8SP:6+", *tracker, false );
	TI.FlushData( *tracker, false );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"303" ) ), list<wstring>( 1, L"LG_SL9" ) ) );
	list<wstring> tplace2;
	tplace2.push_back( L"LG_SL5" );
	tplace2.push_back( L"LG_SL7" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace2 ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"LG_SL5", *tracker, false );
	TI.DetermineStripBusy( L"LG_SL7", *tracker, false );
	TI.DetermineStripBusy( L"LG_PDP", *tracker, false );
	TI.DetermineStripBusy( L"LG_6-8SP", *tracker, false );
	TI.DetermineStripFree( L"LG_6-8SP:6+", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneStripUniquenessIntegrity2()
{
	TI.DetermineStripBusy( L"LG_NDP", *tracker );
	wstring stripsarr[] = { L"LG_NDP", L"LG_LI1", L"LG_LI3", L"LG_LI5" };
	vector<wstring> stripsvec( begin( stripsarr ), end( stripsarr ) );
	TI.ImitateMotion( stripsvec, *tracker );
	TI.DetermineStripBusy( L"LG_LI7", *tracker );
	auto recovTrainPtr = trainCont->GetUnity( TI.Get( L"LG_LI5" ).bdg );
	CPPUNIT_ASSERT( recovTrainPtr );
	SetInfo( recovTrainPtr, TrainDescr( L"2963" ) ); //под восстановление

	auto recovTrainId = recovTrainPtr->GetId();
	TI.DetermineStripFree( L"LG_LI5", *tracker, false );
	TI.DetermineStripFree( L"LG_LI7", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8150" ) ), list<wstring>( 1, L"LG_LI1" ) ) );
	TI.DetermineStripBusy( L"LG_NDP", *tracker );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto ndpTrainPtr = trainCont->GetUnity( TI.Get( L"LG_NDP" ).bdg );
	CPPUNIT_ASSERT( ndpTrainPtr );
	auto tdescrPtr = ndpTrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && tdescrPtr->GetNumber().getNumber() == 8149 );
	CPPUNIT_ASSERT( ndpTrainPtr->GetPlace().size() == 1 );
	auto li1TrainPtr = trainCont->GetUnity( TI.Get( L"LG_LI1" ).bdg );
	CPPUNIT_ASSERT( li1TrainPtr && li1TrainPtr->GetId() == recovTrainId );
	auto tdescrPtr2 = li1TrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr2 && tdescrPtr2->GetNumber().getNumber() == 2963 );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneCriticalIntentions()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_LI5" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_NDP" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_LI3" ) ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"LG_LI5", *tracker );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneEmptyRecoveryPlace()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_LI5" ) ) );
	TI.DetermineStripFree( L"LG_LI5", *tracker ); //под восстановление
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_LI3" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"LG_NDP" );
	tplace.push_back( L"LG_1-3SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"LG_1-3SP", *tracker, false );
	TI.DetermineStripFree( L"LG_NDP", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneStripSequenceCorrectness()
{
	list<wstring> tplace;
	tplace.push_back( L"LG_SL5" );
	tplace.push_back( L"LG_SL7" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	list<wstring> tplace2;
	tplace2.push_back( L"LG_SL9" );
	tplace2.push_back( L"LG_PDP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"480" ) ), tplace2 ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"LG_6-8SP" ) ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineSwitchPlus( L"LG6", *tracker, false );
	TI.DetermineStripBusy( L"LG_SL7", *tracker, false );
	TI.DetermineStripBusy( L"LG_SL9", *tracker, false );
	TI.DetermineStripBusy( L"LG_6-8SP", *tracker, false );
	TI.DetermineStripBusy( L"LG_6-8SP:6+", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneStripSequenceCorrectness2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"961" ) ), list<wstring>( 1, L"LG_LI1" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2151" ) ), list<wstring>( 1, L"LG_LI3" ) ) );

	auto tunities = trainCont->GetUnities();
	auto tunityPtr1 = trainCont->GetUnity( TI.Get( L"LG_LI1" ).bdg );
	auto trainId1 = tunityPtr1->GetId();
	auto tunityPtr2 = trainCont->GetUnity( TI.Get( L"LG_LI3" ).bdg );
	auto trainId2 = tunityPtr2->GetId();
	TI.DetermineStripFree( L"LG_1-3SP", *tracker, false );
	TI.DetermineStripFree( L"LG_NDP", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI1", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI3", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI5", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId1]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId1; } ) );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), [trainId2]( TrainUnityCPtr tptr ){ return tptr->GetId() == trainId2; } ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneInvalidTrainsExchange()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8672" ) ), list<wstring>( 1, L"LG_LI5" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"LG_LI1" );
	tplace.push_back( L"LG_LI3" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8614" ) ), tplace ) );

	TI.DetermineStripFree( L"LG_LI1", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI3", *tracker, false );
	TI.DetermineStripFree( L"LG_LI5", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI7", *tracker, false );
	TI.DetermineStripFree( L"LG_LI9", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto li3Unity = trainCont->GetUnity( TI.Get( L"LG_LI3" ).bdg );
	CPPUNIT_ASSERT( li3Unity );
	CPPUNIT_ASSERT( li3Unity->GetPlace().size() == 1 );
	auto li3descrPtr = li3Unity->GetDescrPtr();
	CPPUNIT_ASSERT( li3descrPtr && li3descrPtr->GetNumber().getNumber()== 8614 );
	auto li7Unity = trainCont->GetUnity( TI.Get( L"LG_LI7" ).bdg );
	CPPUNIT_ASSERT( li7Unity );
	CPPUNIT_ASSERT( li7Unity->GetPlace().size() == 1 );
	auto li7descrPtr = li7Unity->GetDescrPtr();
	CPPUNIT_ASSERT( li7descrPtr && li7descrPtr->GetNumber().getNumber()== 8672 );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::LigatneOccupancyIntegrity()
{
	TI.DetermineSwitchPlus( L"LG6", *tracker );
	TI.DetermineSwitchPlus( L"LG8", *tracker );
	list<wstring> tplace1;
	tplace1.push_back( L"LG_6-8SP:6+,8+" );
	tplace1.push_back( L"LG_6-8SP:6+" );
	tplace1.push_back( L"LG_6-8SP" );
	tplace1.push_back( L"LG_PDP" );
	tplace1.push_back( L"LG_SL9" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), tplace1 ) );
	list<wstring> tplace2;
	tplace2.push_back( L"LG_SL1" );
	tplace2.push_back( L"LG_SL3" );
	tplace2.push_back( L"LG_SL5" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );

	TI.DetermineStripBusy( L"LG_PDP", *tracker, false );
	TI.DetermineStripBusy( L"LG_6-8SP", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::LigatneOccupancyIntegrity2()
{
	TI.DetermineSwitchPlus( L"LG1", *tracker );

	list<wstring> tplace1;
	tplace1.push_back( L"LG_LI3" );
	tplace1.push_back( L"LG_LI5" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"1826" ) ), tplace1 ) );

	list<wstring> tplace2;
	tplace2.push_back( L"LG_NDP" );
	tplace2.push_back( L"LG_1-3SP" );
	tplace2.push_back( L"LG_1-3SP:1+" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineSwitchOutOfControl( L"LG1", *tracker, false );
	TI.DetermineStripBusy( L"LG_LI1", *tracker, false );
	TI.DetermineStripFree( L"LG_LI5", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::LigatneOccupancyIntegrity3()
{
	TI.DetermineSwitchPlus( L"LG6", *tracker );
	TI.DetermineSwitchPlus( L"LG8", *tracker );
	TI.DetermineSwitchPlus( L"LG3", *tracker );

	//под восстановление
	list<wstring> recplace;
	recplace.push_back( L"LG_1-3SP:1+,3+" );
	recplace.push_back( L"LG_1-3SP:1+" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"1" ) ), recplace ) );
	TI.DetermineStripFree( L"LG_1-3SP:1+,3+", *tracker, false );
	TI.DetermineStripFree( L"LG_1-3SP:1+", *tracker, false );
	TI.FlushData( *tracker, false );

	//поезд 1
	list<wstring> tplace1;
	tplace1.push_back( L"LG_1C" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"3" ) ), tplace1 ) );

	//поезд 2
	list<wstring> tplace2;
	tplace2.push_back( L"LG_PDP" );
	tplace2.push_back( L"LG_6-8SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"5" ) ), tplace2 ) );

	auto tunities1 = trainCont->GetUnities();

	TI.DetermineStripBusy( L"LG_6-8SP:6+", *tracker, false );
	TI.DetermineStripBusy( L"LG_6-8SP:6+,8+", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities2 = trainCont->GetUnities();

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::MezvidiRecoveryIntegrity()
{
	TI.DetermineSwitchPlus( L"MZ1", *tracker );
	TI.DetermineSwitchPlus( L"MZ6", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"MZ_1C" );
	tplace.push_back( L"MZ_1-3SP:1+" );
	tplace.push_back( L"MZ_1-3SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2517" ) ), tplace ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"MZ_5C:6+" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"MZ_3C" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6213" ) ), list<wstring>( 1, L"MZ_5C" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"MZ_N1P_PUR" ) ) ); //под восстановление
	auto recovTrainPtr = trainCont->GetUnity( TI.Get( L"MZ_N1P_PUR" ).bdg );
	CPPUNIT_ASSERT( recovTrainPtr );
	auto recovTrainIden = recovTrainPtr->GetId();
	TI.DetermineStripFree( L"MZ_N1P_PUR", *tracker );

	auto tunityPtr_5C = trainCont->GetUnity( TI.Get( L"MZ_5C" ).bdg );
	auto tunityPtr_3C = trainCont->GetUnity( TI.Get( L"MZ_3C" ).bdg );
	auto tunityPtr_1C = trainCont->GetUnity( TI.Get( L"MZ_1C" ).bdg );
	CPPUNIT_ASSERT( tunityPtr_1C && tunityPtr_3C && tunityPtr_5C );
	auto tunityIden_5C = tunityPtr_5C->GetId();
	auto tunityIden_3C = tunityPtr_3C->GetId();
	auto tunityIden_1C = tunityPtr_1C->GetId();

	TI.DetermineSwitchMinus( L"MZ1", *tracker, false );
	TI.DetermineSwitchMinus( L"MZ3", *tracker, false );
	TI.DetermineStripFree( L"MZ_3C", *tracker, false );
	TI.DetermineStripFree( L"MZ_1-3SP:1-,3+", *tracker, false );
	TI.DetermineStripFree( L"MZ_1C", *tracker, false );
	TI.DetermineStripFree( L"MZ_1-3SP:1+", *tracker, false );
	TI.DetermineStripBusy( L"MZ_1-3SP:1-,3-", *tracker, false );
	TI.DetermineStripBusy( L"MZ_1-3SP:1-", *tracker, false );
	TI.DetermineStripBusy( L"MZ_1-3SP", *tracker, false );
	TI.DetermineStripFree( L"MZ_NP", *tracker, false );
	TI.DetermineHeadClose( L"MZ_HeadP1", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::RigaLastPlaceLiberation()
{
	TI.DetermineSwitchPlus( L"RG81", *tracker );
	TI.DetermineSwitchMinus( L"RG63", *tracker );
	TI.DetermineSwitchPlus( L"RG83", *tracker );

	list<wstring> tplace;
	tplace.push_back( L"RG_10C" );
	tplace.push_back( L"RG_81SP" );
	tplace.push_back( L"RG_81SP+" );
	tplace.push_back( L"RG_63SP-" );
	tplace.push_back( L"RG_63SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"314" ) ), tplace ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"RG_83SP+", *tracker, false );
	TI.DetermineStripBusy( L"RG_83SP", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::SlokaSequencyIntegrity()
{
	TI.DetermineSwitchMinus( L"SL_17", *tracker );
	TI.DetermineSwitchPlus( L"SL_9", *tracker );
	TI.DetermineSwitchPlus( L"SL_5", *tracker );
	TI.DetermineSwitchPlus( L"SL_3", *tracker );
	TI.DetermineSwitchPlus( L"SL_11", *tracker );

	list<wstring> tplace1( 1, L"SL_5/11sp:5/7+,9-,11/13-" );
	tplace1.emplace_back( L"SL_5/11sp:5/7+,9-" );
	LocateUnityByForce( nullptr, tplace1 );
	list<wstring> tplace2( 1, L"SL_17sp:17/19-" );
	tplace2.emplace_back( L"SL_17sp" );
	tplace2.emplace_back( L"SL_5/11sp:5/7+,9+" );
	tplace2.emplace_back( L"SL_5/11sp:5/7+" );
	tplace2.emplace_back( L"SL_5/11sp" );
	tplace2.emplace_back( L"SL_3sp" );
	tplace2.emplace_back( L"SL_3sp:1/3+" );
	LocateUnityByForce( nullptr, tplace2 );

	TI.DetermineSwitchMinus( L"SL_11", *tracker, false );
	TI.DetermineStripBusy( L"SL_17sp", *tracker, false );
	TI.DetermineStripBusy( L"SL_17sp:17/19-", *tracker, false );
	TI.DetermineStripBusy( L"SL_5/11sp", *tracker, false );
	TI.DetermineStripBusy( L"SL_5/11sp:5/7+", *tracker, false );
	TI.DetermineStripFree( L"SL_5/11sp:5/7+,9+", *tracker, false );
	TI.DetermineSwitchMinus( L"SL_9", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::VangaziSequencyIntegrity()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8672" ) ), list<wstring>( 1, L"VG_2C" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_3-9SP" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"VG_NpDP" );
	tplace.push_back( L"VG_VI6" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );

	TI.DetermineStripFree( L"VG_2C", *tracker, false );
	TI.DetermineStripBusy( L"VG_3-9SP:5+,9+", *tracker, false );
	TI.DetermineStripBusy( L"VG_3-9SP:5+", *tracker, false );
	TI.DetermineStripBusy( L"VG_3-9SP", *tracker, false );
	TI.DetermineStripBusy( L"VG_3-9SP:3+", *tracker, false );
	TI.DetermineSwitchPlus( L"VG3", *tracker, false );
	TI.DetermineSwitchPlus( L"VG5", *tracker, false );
	TI.DetermineSwitchPlus( L"VG9", *tracker, false );
	TI.DetermineStripFree( L"VG_VI6", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::VangaziSequencyIntegrity2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_2S-6SP:2S-" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_2S-6SP:2S+" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_2S-6SP:2S+,6-" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_2S-6SP:2S+,6+" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"VG_PDP" );
	tplace.push_back( L"VG_2S-6SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"VG_2S-6SP:2S+,6-", *tracker, false );
	TI.DetermineStripBusy( L"VG_2S-6SP:2S+,6+", *tracker, false );
	TI.DetermineStripBusy( L"VG_2S-6SP:2S+", *tracker, false );
	TI.DetermineStripFree( L"VG_2S-6SP:2S-", *tracker, false );
	TI.DetermineStripBusy( L"VG_2S-6SP", *tracker, false );
	TI.DetermineStripBusy( L"VG_PDP", *tracker, false );
	TI.DetermineSwitchPlus( L"VG6", *tracker, false );
	TI.DetermineSwitchMinus( L"VG2S", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::VangaziSequencyIntegrity3()
{
	TI.DetermineSwitchMinus( L"VG2S", *tracker );
	TI.DetermineSwitchPlus( L"VG7", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_2S-6SP:2S+,6-" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_2S-6SP:2S+,6+" ) ) );
	list<wstring> tplace;
	tplace.push_back( L"VG_2S-6SP" );
	tplace.push_back( L"VG_2S-6SP:2S+" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	list<wstring> tplace2;
	tplace2.push_back( L"VG_3C" );
	tplace2.push_back( L"VG_7-11SP:11-" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );
	list<wstring> tplace3;
	tplace3.push_back( L"VG_KV3" );
	tplace3.push_back( L"VG_PDP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), tplace3 ) );
	list<wstring> tplace4;
	tplace4.push_back( L"VG_7-11SP" );
	tplace4.push_back( L"VG_1-11SP:1+,7+" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace4 ) );

	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"VG_KV3", *tracker, false );
	TI.DetermineStripFree( L"VG_PDP", *tracker, false );
	TI.DetermineStripFree( L"VG_2S-6SP", *tracker, false );
	TI.DetermineStripBusy( L"VG_2S-6SP:2S+", *tracker, false );
	TI.DetermineStripBusy( L"VG_2S-6SP:2S+,6-", *tracker, false );
	TI.DetermineStripFree( L"VG_2S-6SP:2S+,6+", *tracker, false );
	TI.DetermineStripBusy( L"VG_1C", *tracker, false );
	TI.DetermineStripFree( L"VG_7-11SP:11+", *tracker, false );
	TI.DetermineStripFree( L"VG_7-11SP:11-", *tracker, false );
	TI.DetermineStripFree( L"VG_1-11SP:1+,7+", *tracker, false );
	TI.DetermineSwitchMinus( L"VG6", *tracker, false );
	TI.DetermineSwitchMinus( L"VG11", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::VangaziChainsIntegrity()
{
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_7-11SP" ) ) );
	list<wstring> tplace( 1, L"VG_NDP" );
	tplace.push_back( L"VG_1SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"847" ) ), tplace ) );

	TI.DetermineStripBusy( L"VG_7-11SP:11+", *tracker, false );
	TI.DetermineStripBusy( L"VG_7-11SP", *tracker, false );
	TI.DetermineStripBusy( L"VG_1-11SP:1+,7+", *tracker, false );
	TI.DetermineStripBusy( L"VG_1SP", *tracker, false );
	TI.DetermineStripBusy( L"VG_NDP", *tracker, false );
	TI.DetermineStripBusy( L"VG_VI1", *tracker, false );
	TI.DetermineSwitchPlus( L"VG11", *tracker, false );
	TI.DetermineSwitchPlus( L"VG7", *tracker, false );
	TI.DetermineSwitchPlus( L"VG1", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::VangaziChainsIntegrity2()
{
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_3-9SP:5-" ) ) );
	TI.DetermineStripFree(L"VG_3-9SP:5-", *tracker);
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"847" ) ), list<wstring>( 1, L"VG_VI1" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_NDP" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_1SP" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_1-11SP:1+,7+" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"VG_1SP:1-" ) ) );

	TI.DetermineStripFree( L"VG_7-11SP", *tracker, false );
	TI.DetermineStripFree( L"VG_1-11SP:1+,7+", *tracker, false );
	TI.DetermineStripFree( L"VG_1SP", *tracker, false );
	TI.DetermineStripFree( L"VG_1SP:1-", *tracker, false );
	TI.DetermineStripFree( L"VG_3-9SP:3+", *tracker, false );
	TI.DetermineStripBusy( L"VG_3-9SP:5-", *tracker, false );
	TI.DetermineStripBusy( L"VG_3-9SP:3-", *tracker, false );
	TI.DetermineStripBusy( L"VG_3-9SP:5+", *tracker, false );
	TI.DetermineStripBusy( L"VG_3-9SP", *tracker, false );
	TI.DetermineStripBusy( L"VG_NpDP", *tracker, false );
	TI.DetermineSwitchMinus( L"VG1", *tracker, false );
	TI.DetermineSwitchMinus( L"VG3", *tracker, false );
	TI.DetermineSwitchOutOfControl( L"VG5", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerRealStations::Rezekne2OccupancyIntegrity()
{
	TI.DetermineSwitchPlus( L"RZ2_17", *tracker );
	TI.DetermineSwitchMinus( L"RZ2_21", *tracker );
	TI.DetermineSwitchMinus( L"RZ2_43", *tracker );
	TI.DetermineSwitchMinus( L"RZ2_45", *tracker );

	//поезд под восстановление
	list<wstring> recovplace;
	recovplace.push_back( L"RZ2_17-21SP:17+" );
	recovplace.push_back( L"RZ2_17-21SP" );
	recovplace.push_back( L"RZ2_17-21SP:21-" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), recovplace ) );
	auto recovTrainPtr = trainCont->GetUnity( TI.Get( L"RZ2_17-21SP" ).bdg );
	CPPUNIT_ASSERT( recovTrainPtr && recovTrainPtr->IsTrain() );
	auto recovIden = recovTrainPtr->GetId();
	TI.DetermineStripFree( L"RZ2_17-21SP:17+", *tracker, false );
	TI.DetermineStripFree( L"RZ2_17-21SP", *tracker, false );
	TI.DetermineStripFree( L"RZ2_17-21SP:21-", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"RZ2_43-45SP" ) ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"RZ2_43-45SP" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId = trainPtr->GetId();
	list<wstring> tplace;
	tplace.push_back( L"RZ2_43-45SP:43-" );
	tplace.push_back( L"RZ2_43-45SP:43-,45-" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"RZ2_43-45SP:43-" ).bdg );
	CPPUNIT_ASSERT( notrainPtr );
	auto notrainId = notrainPtr->GetId();

	TI.DetermineStripBusy( L"RZ2_43-45SP", *tracker, false );
	TI.DetermineStripBusy( L"RZ2_43-45SP:43-", *tracker, false );
	TI.DetermineStripBusy( L"RZ2_43-45SP:43-,45-", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::Rezekne2OccupancyIntegrity2()
{
	TI.DetermineSwitchPlus( L"RZ2_17", *tracker );
	TI.DetermineSwitchMinus( L"RZ2_21", *tracker );
	TI.DetermineSwitchMinus( L"RZ2_43", *tracker );
	TI.DetermineSwitchMinus( L"RZ2_45", *tracker );

	//маневровая под восстановление
	list<wstring> recovplace;
	recovplace.push_back( L"RZ2_17-21SP:17+" );
	recovplace.push_back( L"RZ2_17-21SP" );
	recovplace.push_back( L"RZ2_17-21SP:21-" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, recovplace ) );
	auto recovTrainPtr = trainCont->GetUnity( TI.Get( L"RZ2_17-21SP" ).bdg );
	CPPUNIT_ASSERT( recovTrainPtr && !recovTrainPtr->IsTrain() );
	auto recovIden = recovTrainPtr->GetId();
	TI.DetermineStripFree( L"RZ2_17-21SP:17+", *tracker, false );
	TI.DetermineStripFree( L"RZ2_17-21SP", *tracker, false );
	TI.DetermineStripFree( L"RZ2_17-21SP:21-", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"RZ2_43-45SP" ) ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"RZ2_43-45SP" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId = trainPtr->GetId();
	list<wstring> tplace;
	tplace.push_back( L"RZ2_43-45SP:43-" );
	tplace.push_back( L"RZ2_43-45SP:43-,45-" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace ) );
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"RZ2_43-45SP:43-" ).bdg );
	CPPUNIT_ASSERT( notrainPtr );
	auto notrainId = notrainPtr->GetId();

	TI.DetermineStripBusy( L"RZ2_43-45SP", *tracker, false );
	TI.DetermineStripBusy( L"RZ2_43-45SP:43-", *tracker, false );
	TI.DetermineStripBusy( L"RZ2_43-45SP:43-,45-", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( trainCont->IsDeadUnity( recovIden ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::BaleException()
{
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"BA_4SP:4+" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"BA_4SP:4-" ) ) );
	list<wstring> tplace1;
	tplace1.push_back( L"BA_PDP" );
	tplace1.push_back( L"BA_4SP" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace1 ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"BA_1SP:1+" ) ) );
	list<wstring> tplace2;
	tplace2.push_back( L"BA_2C" );
	tplace2.push_back( L"BA_1SP:1-" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );
	list<wstring> tplace3;
	tplace3.push_back( L"BA_LB9" );
	tplace3.push_back( L"BA_LB11" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace3 ) );
	list<wstring> tplace4;
	tplace4.push_back( L"BA_1SP" );
	tplace4.push_back( L"BA_NDP" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace4 ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2828" ) ), list<wstring>( 1, L"BA_BV3" ) ) );
	TI.DetermineStripFree( L"BA_BV3", *tracker );

	TI.DetermineStripFree( L"BA_LB9", *tracker, false );
	TI.DetermineStripFree( L"BA_LB11", *tracker, false );
	TI.DetermineStripFree( L"BA_PDP", *tracker, false );
	TI.DetermineStripFree( L"BA_4SP:4-", *tracker, false );
	TI.DetermineStripFree( L"BA_1SP:1+", *tracker, false );
	TI.DetermineStripFree( L"BA_1SP:1-", *tracker, false );
	TI.DetermineStripFree( L"BA_1SP", *tracker, false );
	TI.DetermineStripFree( L"BA_NDP", *tracker, false );
	TI.DetermineStripBusy( L"BA_4SP", *tracker, false );
	TI.DetermineStripBusy( L"BA_4SP:4+", *tracker, false );
	TI.DetermineStripBusy( L"BA_1C", *tracker, false );
	TI.DetermineStripBusy( L"BA_BV3", *tracker, false );
	TI.DetermineSwitchPlus( L"BA4", *tracker, false );
	TI.DetermineSwitchMinus( L"BA1", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerRealStations::BaleSequencyIntegrity()
{
	list<wstring> tplace1;
	tplace1.push_back( L"BA_2C" );
	tplace1.push_back( L"BA_1SP:1-" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace1 ) );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"511" ) ), list<wstring>( 1, L"BA_1SP" ) ) );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, list<wstring>( 1, L"BA_BV1" ) ) );
	list<wstring> tplace2;
	tplace2.push_back( L"BA_1C" );
	tplace2.push_back( L"BA_1SP:1+" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );
	list<wstring> tplace3;
	tplace3.push_back( L"BA_LB11" );
	tplace3.push_back( L"BA_LB9" );
	tplace3.push_back( L"BA_LB7" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace3 ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"BA_BV3", *tracker, false );
	TI.DetermineSwitchMinus( L"BA1", *tracker, false );
	TI.DetermineStripFree( L"BA_1SP:1+", *tracker, false );
	TI.DetermineStripBusy( L"BA_1SP:1-", *tracker, false );
	TI.DetermineSwitchMinus( L"BA4", *tracker, false );
	TI.DetermineStripFree( L"BA_4SP:4-", *tracker, false );
	TI.DetermineStripBusy( L"BA_NDP", *tracker, false );
	TI.DetermineStripBusy( L"BA_PDP", *tracker, false );
	TI.DetermineStripFree( L"BA_LB11", *tracker, false );
	TI.DetermineStripFree( L"BA_LB9", *tracker, false );
	TI.DetermineStripFree( L"BA_LB7", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}