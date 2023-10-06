#include "stdafx.h"
#include "TC_TrackerChangings.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerChangings );
void TC_TrackerChangings::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.delnotifSec = 1;
}

void TC_TrackerChangings::tearDown()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.LPFThresholdSec = 0;
	TI.Reset();
}

void TC_TrackerChangings::StandardPassing()
{
	auto trainNum = StandardPassingInput();
	StandardPassingInputCheck( trainNum );
	StandardPassingOutput();
	StandardPassingOutputCheck( trainNum );
}

wstring TC_TrackerChangings::StandardPassingInput()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	const TrainDescr & tdescr = RandomTrainDescr( false );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	auto train = trainCont->GetUnities().front();
	SetInfo( train, tdescr );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	return tdescr.GetNumber().getString();
}

void TC_TrackerChangings::StandardPassingInputCheck( wstring trainNum )
{
	//забор и проверка изменений
	ChangesUnion changes = tracker->TakeTrainChanges();
	const auto & placeChangings = changes.placeViewChanges;
	CPPUNIT_ASSERT( placeChangings.size() == 5 );
	auto pcIt = placeChangings.cbegin();
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"W" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );
	++pcIt;
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"Q" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );
	++pcIt;
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"A" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );
	++pcIt;
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"B" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );
	++pcIt;
	CPPUNIT_ASSERT( pcIt->appeared && pcIt->place == TI.Get( L"C" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );

	const auto & headMovings = changes.headMotions;
	CPPUNIT_ASSERT( headMovings.size() == 5 );
	auto hmIt = headMovings.cbegin();
	CPPUNIT_ASSERT( hmIt->from.empty() && hmIt->to == TI.Get( L"W" ).bdg && hmIt->future.empty() &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"W" ).bdg && hmIt->to == TI.Get( L"Q" ).bdg && hmIt->future == TI.Get( L"A" ).bdg &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"Q" ).bdg && hmIt->to == TI.Get( L"A" ).bdg && hmIt->future.empty() &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"A" ).bdg && hmIt->to == TI.Get( L"B" ).bdg && hmIt->future == TI.Get( L"C" ).bdg &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"B" ).bdg && hmIt->to == TI.Get( L"C" ).bdg && hmIt->future == TI.Get( L"F" ).bdg &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	CPPUNIT_ASSERT( changes.headMotions.empty() );
}

void TC_TrackerChangings::StandardPassingOutput()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripFree( L"G", *tracker );
	//исчезновение поезда с необходимой выдержкой времени:
	TI.DetermineStripFree( L"X", *tracker );
	const auto & timConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( ( timConstr.delnotifSec + 1 ) * 1000 );
}

void TC_TrackerChangings::StandardPassingOutputCheck( std::wstring trainNum )
{
	//забор и проверка изменений
	ChangesUnion changes = tracker->TakeTrainChanges();
	const auto & placeChangings = changes.placeViewChanges;
	CPPUNIT_ASSERT( placeChangings.size() == 4 );
	auto pcIt = placeChangings.cbegin();
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"F" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );
	++pcIt;
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"G" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );
	++pcIt;
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"C" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );
	++pcIt;
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"X" ).bdg && pcIt->tdescrPtr->GetNumber().getString() == trainNum );

	const auto & headMovings = changes.headMotions;
	CPPUNIT_ASSERT( headMovings.size() == 4 );
	auto hmIt = headMovings.cbegin();
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"C" ).bdg && hmIt->to == TI.Get( L"F" ).bdg && hmIt->future == TI.Get( L"G" ).bdg &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"F" ).bdg && hmIt->to == TI.Get( L"G" ).bdg && hmIt->future == TI.Get( L"X" ).bdg &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"G" ).bdg && hmIt->to == TI.Get( L"X" ).bdg && hmIt->future == TI.Get( L"I" ).bdg &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"X" ).bdg && hmIt->to.empty() && hmIt->future.empty() &&
		hmIt->tdescrPtr->GetNumber().getString() == trainNum );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	CPPUNIT_ASSERT( changes.headMotions.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::ElementaryMove()
{
	TI.DetermineStripBusy( L"W", *tracker );
	const TrainDescr & tdescr = RandomTrainDescr( false );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	auto train = trainCont->GetUnities().front();
	SetInfo( train, tdescr );
	ChangesUnion changes;
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	auto placeChange = changes.placeViewChanges.front();
	CPPUNIT_ASSERT( placeChange.appeared && placeChange.place == TI.Get( L"W" ).bdg && *placeChange.tdescrPtr == tdescr );
	CPPUNIT_ASSERT( changes.headMotions.size() == 1 );
	auto headMove = changes.headMotions.front();
	CPPUNIT_ASSERT( headMove.from.empty() && headMove.to == TI.Get( L"W" ).bdg && *headMove.tdescrPtr == tdescr );

	TI.DetermineStripBusy( L"Q", *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	placeChange = changes.placeViewChanges.front();
	CPPUNIT_ASSERT( placeChange.appeared && placeChange.place == TI.Get( L"Q" ).bdg && *placeChange.tdescrPtr == tdescr );
	CPPUNIT_ASSERT( changes.headMotions.size() == 1 );
	headMove = changes.headMotions.front();
	CPPUNIT_ASSERT( headMove.from == TI.Get( L"W" ).bdg && headMove.to == TI.Get( L"Q" ).bdg && *headMove.tdescrPtr == tdescr );

	TI.DetermineStripFree( L"W", *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	placeChange = changes.placeViewChanges.front();
	CPPUNIT_ASSERT( !placeChange.appeared && placeChange.place == TI.Get( L"W" ).bdg && *placeChange.tdescrPtr == tdescr );
	CPPUNIT_ASSERT( changes.headMotions.empty() );

	TI.DetermineStripFree( L"Q", *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() ); //задерживается к отправке
	CPPUNIT_ASSERT( changes.headMotions.size() == 1 );
	headMove = changes.headMotions.front();
	CPPUNIT_ASSERT( headMove.from == TI.Get( L"Q" ).bdg && headMove.to.empty() && *headMove.tdescrPtr == tdescr );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::OneEndBlinking()
{
	TI.DetermineStripBusy( L"W", *tracker );
	const TrainDescr & tdescr = RandomTrainDescr( false );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	auto train = trainCont->GetUnities().front();
	SetInfo( train, tdescr );
	TI.DetermineStripBusy( L"Q", *tracker );
	for ( int i = 0; i < 10; ++i )
	{
		TI.DetermineStripFree( L"Q", *tracker );
		TI.DetermineStripBusy( L"Q", *tracker );
	}
	ChangesUnion changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	auto plcChange1 = changes.placeViewChanges.front();
	auto plcChange2 = changes.placeViewChanges.back();
	CPPUNIT_ASSERT( plcChange1.appeared && plcChange1.place == TI.Get( L"W" ).bdg && *plcChange1.tdescrPtr == tdescr );
	CPPUNIT_ASSERT( plcChange2.appeared && plcChange2.place == TI.Get( L"Q" ).bdg && *plcChange2.tdescrPtr == tdescr );
	CPPUNIT_ASSERT( changes.headMotions.size() == 22 );
	for ( auto hmIt = changes.headMotions.cbegin(); hmIt != changes.headMotions.cend(); ++hmIt )
	{
		if ( hmIt == changes.headMotions.cbegin() )
			CPPUNIT_ASSERT( hmIt->from.empty() );
		else
			CPPUNIT_ASSERT( hmIt->from == TI.Get( L"Q" ).bdg );
		CPPUNIT_ASSERT( hmIt->to == TI.Get( L"W" ).bdg && *hmIt->tdescrPtr == tdescr );
		++hmIt;
		CPPUNIT_ASSERT( hmIt->from == TI.Get( L"W" ).bdg && hmIt->to == TI.Get( L"Q" ).bdg && *hmIt->tdescrPtr == tdescr );
	}

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::UTurn()
{
	TI.DetermineStripBusy( L"W", *tracker );
	const TrainDescr & tdescr = RandomTrainDescr( false );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	auto train = trainCont->GetUnities().front();
	SetInfo( train, tdescr );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	ChangesUnion changes = tracker->TakeTrainChanges();

	const auto & placeChangings = changes.placeViewChanges;
	CPPUNIT_ASSERT( placeChangings.size() == 2 );
	auto pcIt = placeChangings.cbegin();
	CPPUNIT_ASSERT( pcIt->appeared && pcIt->place == TI.Get( L"W" ).bdg && *pcIt->tdescrPtr == tdescr );
	++pcIt;
	CPPUNIT_ASSERT( !pcIt->appeared && pcIt->place == TI.Get( L"Q" ).bdg && *pcIt->tdescrPtr == tdescr );

	const auto & headMovings = changes.headMotions;
	CPPUNIT_ASSERT( headMovings.size() == 3 );
	auto hmIt = headMovings.cbegin();
	CPPUNIT_ASSERT( hmIt->from.empty() && hmIt->to == TI.Get( L"W" ).bdg && *hmIt->tdescrPtr == tdescr );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"W" ).bdg && hmIt->to == TI.Get( L"Q" ).bdg && *hmIt->tdescrPtr == tdescr );
	++hmIt;
	CPPUNIT_ASSERT( hmIt->from == TI.Get( L"Q" ).bdg && hmIt->to == TI.Get( L"W" ).bdg && *hmIt->tdescrPtr == tdescr );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::LengthyLiberationWithFiltering()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.LPFThresholdSec = 1;
	TI.Reset();

	//E-D-A - четный поезд
	//Q-W-H - поезд (не важно какой)
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.IncreaseTime( timConstr.LPFThresholdSec );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	for ( auto tunity : trains )
		SetRandomEvenInfo( tunity );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripBusy( L"D", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"E", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.IncreaseTime( timConstr.LPFThresholdSec );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );

	ChangesUnion changes = tracker->TakeTrainChanges();

	//гашение W-Q (при наличии сглаживающей задержки)
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	changes = tracker->TakeTrainChanges();
	TI.IncreaseTime( timConstr.LPFThresholdSec );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	tracker->TakeChanges( changes );

	const auto & placeChangings = changes.placeViewChanges;
	CPPUNIT_ASSERT( placeChangings.size() == 2 );
	CPPUNIT_ASSERT( find_if( placeChangings.cbegin(), placeChangings.cend(), [this]( const PlaceViewChange & pvChange ){
		return !pvChange.appeared && pvChange.place == TI.Get( L"W" ).bdg;
	} ) != placeChangings.cend() );
	CPPUNIT_ASSERT( find_if( placeChangings.cbegin(), placeChangings.cend(), [this]( const PlaceViewChange & pvChange ){
		return !pvChange.appeared && pvChange.place == TI.Get( L"Q" ).bdg;
	} ) != placeChangings.cend() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::UnknownUnityADWayExitNGoSpan()
{
	TI.DetermineHeadClose( L"Head101", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );

	TI.DetermineStripBusy( L"F", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomOddInfo( *trains.cbegin() );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker );

	ChangesUnion changes = tracker->TakeTrainChanges();
	const auto & headMotions = changes.headMotions;
	CPPUNIT_ASSERT( headMotions.size() == 2 );

	//выезд на закрытый светофор
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( headMotions.empty() );

	//выезд на перегон
	TI.DetermineStripBusy( L"Q", *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( headMotions.size() == 1 );
	const auto & theadMotion = headMotions.front();
	CPPUNIT_ASSERT( theadMotion.from == TI.Get( L"A" ).bdg );
	CPPUNIT_ASSERT( theadMotion.to == TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( theadMotion.future == TI.Get( L"W" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::UnknownUnityADWayExitNGoThroughSema()
{
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );

	TI.DetermineStripBusy( L"A", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomEvenInfo( *trains.cbegin() );
	wstring stripsArr[] = { L"A", L"B", L"C" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );

	ChangesUnion changes = tracker->TakeTrainChanges();
	const auto & headMotions = changes.headMotions;
	CPPUNIT_ASSERT( headMotions.size() == 3 );

	//выезд на закрытый светофор
	wstring stripsArr2[] = { L"C", L"F", L"G" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr2 ), end( stripsArr2 ) ), *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchPlus( L"11", *tracker );
	wstring stripsArr3[] = { L"G", L"E", L"D", L"A" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr3 ), end( stripsArr3 ) ), *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	wstring stripsArr4[] = { L"A", L"B", L"C" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr4 ), end( stripsArr4 ) ), *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( headMotions.empty() );

	//проезд открытого светофора
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( headMotions.size() == 1 );
	const auto & theadMotion = headMotions.front();
	CPPUNIT_ASSERT( theadMotion.from == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( theadMotion.to == TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( theadMotion.future == TI.Get( L"G" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::HeadMotionsOnSetTrainInfo()
{
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	SetRandomEvenInfo( train );
	auto changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.headMotions.size() == 1 );
	const auto & headMotion = changes.headMotions.back();
	CPPUNIT_ASSERT( headMotion.from.empty() && headMotion.to == TI.Get( L"A" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::HeadMotionsOnUnsetTrainInfo()
{
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	SetRandomEvenInfo( train );
	auto changes = tracker->TakeTrainChanges();
	DisformTrain( train->GetId() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.headMotions.size() == 1 );
	const auto & headMotion = changes.headMotions.back();
	CPPUNIT_ASSERT( headMotion.from == train->Head() && headMotion.to.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::HeadMotionsOnChangeInfoForUnmovingTrain()
{
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	SetRandomEvenInfo( train );
	auto changes = tracker->TakeTrainChanges();
	SetRandomEvenInfo( train );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.headMotions.empty() ); //непонятно, насколько это правильно. но пока должно работать так
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerChangings::DisappearWithRecovery()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2434" ) ), list<wstring>( 1, L"C" ) );
	auto changes = tracker->TakeTrainChanges();

	TI.DetermineStripFree( L"C", *tracker );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() ); //мгновенных изменений нет

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerChangings::DisappearWithoutRecovery()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2434" ) ), list<wstring>( 1, L"C" ) );
	auto tunity = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	auto trainId = tunity->GetId();
	TI.DetermineStripBusy( L"B", *tracker );
	tracker->TakeChanges( changes );
	TI.DetermineStripFree( L"C", *tracker );
	tracker->TakeChanges( changes );
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	const auto & plcchange = *changes.placeViewChanges.cbegin();
	CPPUNIT_ASSERT( !plcchange.appeared && plcchange.place == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerChangings::ForceDisappearAfterRouteSet()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.delnotifSec = 10;
	TI.Reset();

	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2434" ) ), list<wstring>( 1, L"F" ) );
	tracker->TakeChanges( changes );
	TI.DetermineStripFree( L"F", *tracker );
	tracker->TakeChanges( changes );
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	tracker->TakeChanges( changes );
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	const auto & pvc = changes.placeViewChanges.back();
	CPPUNIT_ASSERT( !pvc.appeared && pvc.place == TI.Get( L"F" ).bdg && pvc.tdescrPtr && *pvc.tdescrPtr == TrainDescr( L"2434" ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerChangings::InfoExtensionNoCausesExternalTrainChanges()
{
	TrainDescrPtr tdescrPtr = TrainDescrPtr( new TrainDescr( L"6236" ) );
	LocateUnityByForce( tdescrPtr, list<wstring>( 1, L"S4" ) );
	ChangesUnion changes = tracker->TakeTrainChanges();

	tdescrPtr->SetBrutto( 200 );
	SetInfo( TI.Get( L"S4" ).bdg, *tdescrPtr, TrainCharacteristics::Source::Disposition );

	ChangesUnion changes2 = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes2.headMotions.empty() );
	CPPUNIT_ASSERT( !changes2.placeViewChanges.empty() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}