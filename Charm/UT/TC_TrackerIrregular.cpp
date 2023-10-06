#include "stdafx.h"
#include "TC_TrackerIrregular.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainUnity.h"
#include "../Guess/TrainInfoPackage.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerIrregular );
void TC_TrackerIrregular::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.entranceStoppingLimitSec = 1;
	timConstr.disappTrainsHolding = std::chrono::seconds( 5 );
}

void TC_TrackerIrregular::UnlinkedSpanStrip()
{
    CPPUNIT_ASSERT( tracker == TI.Tracker() );
    CPPUNIT_ASSERT( trainCont == &TI.Tracker()->GetTrainContainer() );

	TI.DetermineStripBusy( L"Unlinked", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
    CPPUNIT_ASSERT_EQUAL( size_t(0), trainList.size() );
	CPPUNIT_ASSERT( trainList.empty() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.empty() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::AbnormalLightningOnSwitch()
{
	//выставляем поезд на две стрелки
	TI.DetermineStripBusy( L"G2", *tracker, false );
	TI.DetermineStripBusy( L"H2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineSwitchMinus( L"22", *tracker, false );
	TI.DetermineSwitchPlus( L"23", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	unsigned int trId = trainList.back()->GetId();

	//загорание участков по противоположным направлениям
	TI.DetermineStripFree( L"G2", *tracker, false );
	TI.DetermineStripFree( L"H2", *tracker, false );
	TI.DetermineSwitchPlus( L"22", *tracker, false );
	TI.DetermineStripBusy( L"M2", *tracker, false );
	TI.DetermineStripBusy( L"D2", *tracker, false );
	TI.DetermineStripBusy( L"L2", *tracker, false );
	TI.FlushData( *tracker, false );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	auto mainTrain = trainCont->GetUnity( trId );
	auto addTrain = trainList.front() == mainTrain ? trainList.back() : trainList.front();

	const auto & mtlist = mainTrain->GetPlace();
	CPPUNIT_ASSERT( mtlist.size() == 3 );
	CPPUNIT_ASSERT( find( mtlist.cbegin(), mtlist.cend(), TI.Get( L"L2" ).bdg ) != mtlist.cend() );
	CPPUNIT_ASSERT( find( mtlist.cbegin(), mtlist.cend(), TI.Get( L"F2" ).bdg ) != mtlist.cend() );
	CPPUNIT_ASSERT( find( mtlist.cbegin(), mtlist.cend(), TI.Get( L"E2" ).bdg ) != mtlist.cend() );
	const auto & atlist = addTrain->GetPlace();
	CPPUNIT_ASSERT( atlist.size() == 2 );
	CPPUNIT_ASSERT( find( atlist.cbegin(), atlist.cend(), TI.Get( L"M2" ).bdg ) != atlist.cend() );
	CPPUNIT_ASSERT( find( atlist.cbegin(), atlist.cend(), TI.Get( L"D2" ).bdg ) != atlist.cend() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::TrainVsNotrainDisappearance()
{
	//один не поезд...
	TI.DetermineStripBusy( L"F3", *tracker );

	//... и один поезд
	TI.DetermineStripBusy( L"Y3", *tracker );
	auto y3TrainPtr = trainCont->GetUnity( TI.Get( L"Y3" ).bdg );
	CPPUNIT_ASSERT( y3TrainPtr );
	auto y3trainId = y3TrainPtr->GetId();
	TrainDescr tdescr = RandomTrainDescr();
	SetInfo( y3TrainPtr, tdescr );

	ChangesUnion changes = tracker->TakeTrainChanges();

	//F3->X3 & Y3->X3
	TI.DetermineStripBusy( L"X3", *tracker, false );
	TI.DetermineStripFree( L"F3", *tracker, false );
	TI.DetermineStripFree( L"Y3", *tracker, false );
	TI.FlushData( *tracker, false );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );

	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	CPPUNIT_ASSERT( tlist.back()->GetId() == y3trainId ); //остается тот, что с номером
	auto descrPtr = tlist.back()->GetDescrPtr();
	CPPUNIT_ASSERT( descrPtr && *descrPtr == tdescr ); //остается тот, что с номером

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::TrainContentVsDirectionDisappearance()
{
	//ПЕ без номера
	TI.DetermineStripBusy( L"H", *tracker );

	//поезд с номером
	TI.DetermineStripBusy( L"Q", *tracker );

	SetInfo( TI.Get( L"H" ).bdg, TrainDescr() );
	SetRandomOddInfo( TI.Get( L"Q" ).bdg );
	auto tlist = trainCont->GetUnities();
	auto qtrainId = 0;
	TrainDescr qtrainDescr;
	for ( auto tunity : tlist )
	{
		CPPUNIT_ASSERT( tunity->IsTrain() );
		CPPUNIT_ASSERT( tunity->GetPlace().size() == 1 );
		if ( tunity->Have( TI.Get( L"Q" ).bdg ) )
		{
			qtrainId = tunity->GetId();
			qtrainDescr = *tunity->GetDescrPtr();
		}
	}
	CPPUNIT_ASSERT( qtrainId != 0 );

	TI.DetermineStripFree( L"H", *tracker, false );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.FlushData( *tracker, false );

	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );

	//занятие общего участка Q
	TI.DetermineStripFree( L"W", *tracker );
	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train_after = tlist.front();
	CPPUNIT_ASSERT( train_after->GetId() == qtrainId );
	auto tdescPtr = train_after->GetDescrPtr();
	CPPUNIT_ASSERT( tdescPtr && *tdescPtr == qtrainDescr );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::AbnormalLightningOnSwitch2()
{
	//ПЕ1 (B-A)
	TI.DetermineSwitchMinus( L"11", *tracker );
	list<wstring> abplace;
	abplace.push_back( L"A" );
	abplace.push_back( L"B" );
	LocateUnityByForce( nullptr, abplace );

	//ПЕ2 (Q)
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"Q" ) );

	ChangesUnion changes;
	changes = tracker->TakeTrainChanges();

	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	auto abtunity = trainCont->GetUnity( TI.Get( L"A" ).bdg );
	CPPUNIT_ASSERT( abtunity );
	auto abunityId = abtunity->GetId();
	auto qtunity = trainCont->GetUnity( TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( qtunity );

	//невалидное переключение (после таких сигналов ПЕ на A-B не может попасть на D)
	TI.DetermineSwitchPlus( L"11", *tracker, false );
	TI.DetermineStripBusy( L"D", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 3 );

	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	//принимаем, что ПЕ, находившаяся на B-A, после таких сигналов перестает существовать
	CPPUNIT_ASSERT( trainCont->GetUnity( abunityId ) == nullptr );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::ChangesAfterCrush()
{
	//один поезд в "кармане" 1 (PK5)
	TI.DetermineStripBusy( L"PK2", *tracker );
	TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripFree( L"PK2", *tracker );
	TI.DetermineStripFree( L"PK3", *tracker );

	//один на перегоне (Y3-X3)
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );

	//один на станции (C)
	TI.DetermineStripBusy( L"C", *tracker );
	auto Ctrain = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( Ctrain );
	SetRandomOddInfo( Ctrain );

	//маневровая ПЕ
	TI.DetermineStripBusy( L"D2", *tracker );

	ChangesUnion tchanges;
	tracker->TakeChanges( tchanges );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 4 );

	//сброс трекера
	tchanges = tracker->Reset();
	CPPUNIT_ASSERT( tchanges.placeViewChanges.size() == 5 );
	CPPUNIT_ASSERT( FindIn( tchanges.placeViewChanges, TI.Get( L"PK5" ).bdg ) );
	CPPUNIT_ASSERT( FindIn( tchanges.placeViewChanges, TI.Get( L"Y3" ).bdg ) );
	CPPUNIT_ASSERT( FindIn( tchanges.placeViewChanges, TI.Get( L"X3" ).bdg ) );
	CPPUNIT_ASSERT( FindIn( tchanges.placeViewChanges, TI.Get( L"C" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::DoubleArrivingByReturn()
{
    //въезд на п/о путь
    TI.DetermineSwitchMinus( L"11", *tracker );
    TI.DetermineStripBusy( L"W", *tracker );
    TI.DetermineStripBusy( L"Q", *tracker );
    TI.DetermineStripBusy( L"A", *tracker );
    TI.DetermineStripBusy( L"B", *tracker );
    TI.DetermineStripBusy( L"C", *tracker );

    TI.DetermineStripFree( L"W", *tracker );
    TI.DetermineStripFree( L"Q", *tracker );
    TI.DetermineStripFree( L"A", *tracker );
    TI.DetermineStripFree( L"B", *tracker );

    //выезд на перегон
    TI.DetermineSwitchPlus( L"13", *tracker );
    TI.DetermineStripBusy( L"F", *tracker );
    TI.DetermineStripBusy( L"G", *tracker );
    TI.DetermineStripBusy( L"X", *tracker );

    //мигание п/о пути
    TI.DetermineStripFree( L"C", *tracker );
    TI.DetermineStripBusy( L"C", *tracker );

    list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	auto arriveCount = count_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
        return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
    CPPUNIT_ASSERT( arriveCount == 1 );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
    CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::WreckingADPassing()
{
	//въезд на п/о путь при открытом светофоре
	TI.DetermineHeadOpen( L"Head102", *tracker );
	PullingTailToCWay();
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripBusy( L"F", *tracker ); //поезд продолжает двигаться по инерции
	TI.DetermineStripFree( L"F", *tracker ); //осаживание

	//стандартное отправление..
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	//.. и выезд на перегон
	TI.DetermineStripBusy( L"X", *tracker ); 
	TI.DetermineStripFree( L"G", *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 7 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::DiscordantMoveMerging()
{
	TI.DetermineStripBusy( L"H", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.front();
	SetRandomOddInfo( train );
	auto tdescrPtr = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	TrainDescr refDescr = *tdescrPtr;
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train2 = tlist.front();
	CPPUNIT_ASSERT( train->GetId() == train2->GetId() );
	auto tdescrPtr2 = train2->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr2 );
	CPPUNIT_ASSERT( refDescr == *tdescrPtr2 );
	const auto & tplace = train2->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 3 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"H" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"W" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"Q" ).bdg ) != tplace.cend() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::DeathRecoverAndErrorStopping()
{
	TI.DetermineSwitchPlus( L"15", *tracker );
	TI.DetermineHeadClose( L"Head108", *tracker );

	//поезд едет со станции на перегон
	TI.DetermineStripBusy( L"N", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( !trains.empty() );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );
	TI.DetermineStripBusy( L"J", *tracker );
	TI.DetermineStripFree( L"N", *tracker );

	//пропадание поезда:
	TI.DetermineStripFree( L"J", *tracker );

	//восстановление поезда на соседних участках в обратном порядке:
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"I", *tracker ); //якобы встали перед входным светофором

	//едем дальше по перегону
	TI.DetermineStripFree( L"I", *tracker );

	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( tmConstr.entranceStoppingLimitSec * 1000 + 1000 );
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && (*teIt)->GetId() == trainId );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::DEATH && (*teIt)->GetId() == trainId && (*teIt)->GetBadge() == TI.Get( L"J" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && (*teIt)->GetId() == trainId && (*teIt)->GetBadge() == TI.Get( L"X" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && (*teIt)->GetId() == trainId && (*teIt)->GetBadge() == TI.Get( L"I" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && (*teIt)->GetId() == trainId && (*teIt)->GetBadge() == TI.Get( L"X" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::AsyncEraseDisappeared()
{
	//формируем поезд
	TI.DetermineStripBusy( L"X3", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );

	//поезд исчезает..
	TI.DetermineStripFree( L"X3", *tracker );

	//..и не восстанавливается
	const auto & timConstr = tracker->GetTimeConstraints();
	TI.IncreaseTime( timConstr.disappTrainsHoldingMs() );
	
	TI.DetermineStripBusy( L"X3", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto newTrain = trains.front();
	CPPUNIT_ASSERT( newTrain->GetId() != trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::TransitionReturnExit()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	SetRandomEvenInfo( train ); //четный поезд на W

	wstring stripsArr[] = { L"W", L"Q", L"A", L"B", L"C" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.DetermineStripBusy( L"X", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 8 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::BlinkSeveralEntranceAfterArrival()
{
	TI.DetermineSwitchPlus( L"16", *tracker );
	TI.DetermineHeadClose( L"Head106", *tracker );
	TI.DetermineStripBusy( L"R", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	SetRandomOddInfo( train ); //нечетный поезд на R
	wstring stripsArr[] = { L"R", L"P", L"O", L"M" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	TI.DetermineStripBusy( L"O", *tracker );
	TI.DetermineStripBusy( L"P", *tracker );
	TI.DetermineStripFree( L"O", *tracker, false );
	TI.DetermineStripFree( L"P", *tracker, false );
	TI.FlushData( *tracker, false );
	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::InternalUnlightWithNeighbour()
{
	TI.DetermineSwitchMinus( L"31", *tracker );
	TI.DetermineSwitchPlus( L"32", *tracker );
	TI.DetermineStripBusy( L"A3", *tracker );
	TI.DetermineStripBusy( L"C3", *tracker );
	TI.DetermineStripBusy( L"D3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripBusy( L"H3", *tracker );
	TI.DetermineStripBusy( L"I3", *tracker );
	TI.DetermineStripBusy( L"J3", *tracker );
	TI.DetermineSwitchPlus( L"31", *tracker );
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"F3", *tracker, false );
	TI.DetermineStripFree( L"X3", *tracker, false );
	TI.FlushData( *tracker, false );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 3 );
	bool hasTrain = any_of( trains.cbegin(), trains.cend(), []( TrainUnityCPtr train ){
		return train->IsTrain();
	} );
	bool hasNoTrain = any_of( trains.cbegin(), trains.cend(), []( TrainUnityCPtr train ){
		return !train->IsTrain();
	} );
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		if ( train->IsTrain() )
		{
			CPPUNIT_ASSERT( tplace.size() == 3 );
			CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){
				return ( bdg == TI.Get( L"Y3" ).bdg );
			} ) );
			CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){
				return ( bdg == TI.Get( L"H3" ).bdg );
			} ) );
			CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){
				return ( bdg == TI.Get( L"I3" ).bdg );
			} ) );
			break;
		}
	}

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::TrainPassThroughFakeBusy()
{
	TI.DetermineSwitchMinus( L"15", *tracker );
	TI.DetermineStripBusy( L"J", *tracker, false ); //псевдо-ПЕ (ложная занятость)
	TI.DetermineStripBusy( L"L", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"X", *tracker ); //поезд
	auto initTrain = SetRandomEvenInfo( TI.Get( L"X" ).bdg );
	CPPUNIT_ASSERT( initTrain );
	auto trainId = initTrain->GetId();

	TI.DetermineStripBusy( L"I", *tracker, false );
	TI.DetermineStripFree( L"X", *tracker, false );
	TI.FlushData( *tracker, false );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtrain = trains.front();
	CPPUNIT_ASSERT( curtrain->IsTrain() && curtrain->GetId() == trainId );

	//поезд проезжает через ложную занятость и поглощает ПЕ (т.к. стрелочный участок обслуживается общим ТС)
	TI.DetermineStripFree( L"I", *tracker );
	TI.DetermineStripBusy( L"M", *tracker );
	TI.DetermineRouteSet( L"Head106->P", *tracker );
	TI.DetermineStripBusy( L"O", *tracker );
	TI.DetermineStripFree( L"M", *tracker );

	trains = trainCont->GetUnities();
	auto otrainIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"O" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( otrainIt != trains.cend() );
	auto otrain = *otrainIt;
	CPPUNIT_ASSERT( otrain->GetId() == trainId );
	CPPUNIT_ASSERT( otrain->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::ReversiveMoveOnSpan()
{
	//выезд поезда на перегон
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = *trains.cbegin();
	auto tdescrPtr = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	CPPUNIT_ASSERT( tdescrPtr->GetNumber().empty() );
	CPPUNIT_ASSERT( train->GetOddness() == Oddness::ODD );
	auto trainId = train->GetId();

	//обратное движение
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"H", *tracker );

	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtrain = trains.back();
	CPPUNIT_ASSERT( curtrain->GetOddness() == Oddness::UNDEF );
	CPPUNIT_ASSERT( curtrain->GetId() != trainId );
	const auto & trainPlace = curtrain->GetPlace();
	CPPUNIT_ASSERT( trainPlace.size() == 4 );
	CPPUNIT_ASSERT( find( trainPlace.cbegin(), trainPlace.cend(), TI.Get( L"Q" ).bdg ) != trainPlace.cend() );
	CPPUNIT_ASSERT( find( trainPlace.cbegin(), trainPlace.cend(), TI.Get( L"A" ).bdg ) != trainPlace.cend() );
	CPPUNIT_ASSERT( find( trainPlace.cbegin(), trainPlace.cend(), TI.Get( L"B" ).bdg ) != trainPlace.cend() );
	CPPUNIT_ASSERT( find( trainPlace.cbegin(), trainPlace.cend(), TI.Get( L"C" ).bdg ) != trainPlace.cend() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::ReversiveMoveOnSema()
{
	TI.DetermineHeadClose( L"Head500", *tracker );
	TI.DetermineHeadClose( L"Head501", *tracker );
	TI.DetermineSwitchPlus( L"51", *tracker );
	TI.DetermineSwitchMinus( L"53", *tracker );
	TI.DetermineSwitchPlus( L"54", *tracker );

	TI.DetermineStripBusy( L"F5", *tracker ); //поезд
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tunities.empty() );
	auto trainIden = tunities.back()->GetId();
	SetRandomOddInfo( tunities.back() );

	TI.DetermineStripBusy( L"K5", *tracker ); //ПЕ1
	TI.DetermineStripBusy( L"H5", *tracker );

	TI.DetermineStripBusy( L"A5", *tracker ); //ПЕ2

	//смещение
	auto tunities2 = trainCont->GetUnities();
	TI.DetermineStripBusy( L"B5", *tracker, false );
	TI.DetermineStripBusy( L"D5", *tracker, false );
	TI.DetermineStripBusy( L"G5", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineSwitchMinus( L"52", *tracker );
	auto tunities3 = trainCont->GetUnities();

	auto shuntIt1 = find_if( tunities3.cbegin(), tunities3.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"A5" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( shuntIt1 != tunities3.cend() );
	auto shuntIden1 = ( *shuntIt1 )->GetId();

	auto shuntIt2 = find_if( tunities3.cbegin(), tunities3.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"K5" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( shuntIt2 != tunities3.cend() );
	auto shuntIden2 = ( *shuntIt2 )->GetId();

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	//гашение -> поезд сохраняется, остальное не важно
	TI.DetermineStripFree( L"A5", *tracker, false );
	TI.DetermineStripFree( L"B5", *tracker, false );
	TI.DetermineStripFree( L"D5", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	auto tunities_final = trainCont->GetUnities();

	auto trainIt = find_if( tunities_final.cbegin(), tunities_final.cend(), []( TrainUnityCPtr tptr ){
		return tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( trainIt != tunities_final.cend() );
	const auto & trainPlace = ( *trainIt )->GetPlace();
	CPPUNIT_ASSERT( trainPlace.size() == 1 );
	CPPUNIT_ASSERT( trainPlace.front() == TI.Get( L"F5" ).bdg );
	CPPUNIT_ASSERT( ( *trainIt )->GetId() == trainIden );
	auto appIt = find_if( tunities_final.cbegin(), tunities_final.cend(), []( TrainUnityCPtr tptr ){
		return !tptr->IsTrain();
	} );

	CPPUNIT_ASSERT( appIt != tunities_final.cend() );
	const auto & appPlace = ( *appIt )->GetPlace();
	CPPUNIT_ASSERT( appPlace.size() == 3 );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::MultiSectionFaultBusy()
{
	//2 ПЕ(E4, G4-H4) -> поезд при подъезде к маневровым должен их полностью поглощать
	TI.DetermineSwitchPlus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineStripBusy( L"E4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );

	TI.DetermineStripBusy( L"I4", *tracker ); //поезд
	auto trainIden = 0;
	auto tunities = trainCont->GetUnities();
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 && tplace.front() == TI.Get( L"I4" ).bdg )
		{
			SetRandomOddInfo( tunity );
			trainIden = tunity->GetId();
		}
	}
	CPPUNIT_ASSERT( trainIden > 0 );
	CPPUNIT_ASSERT( tunities.size() == 3 );
	TI.DetermineStripBusy( L"K4", *tracker );
	auto restunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( restunities.size() == 1 );
	auto restrainPtr = restunities.back();
	CPPUNIT_ASSERT( restrainPtr->GetId() == trainIden );
	CPPUNIT_ASSERT( restrainPtr->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::TrainThroughFaultFreeOnSpan()
{
	TI.DetermineStripBusy( L"A", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tunities.empty() );
	SetRandomOddInfo( tunities.back() );
	auto forwardTrainId = tunities.back()->GetId();
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"K", *tracker );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	tunities = trainCont->GetUnities();
	auto ktrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 1 && tplace.front() == TI.Get( L"K" ).bdg;
	} );
	CPPUNIT_ASSERT( ktrainIt != tunities.cend() );
	CPPUNIT_ASSERT( ( *ktrainIt )->GetId() == forwardTrainId );
	TI.DetermineStripFree( L"W", *tracker );

	auto restunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( restunities.size() == 1 );
	CPPUNIT_ASSERT( restunities.front()->GetId() == forwardTrainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::TrainThroughFaultFreeOnADOut()
{
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineRouteSet( L"Head400->K4", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	CPPUNIT_ASSERT( tunities.back()->IsTrain() );
	TI.DetermineStripBusy( L"H4", *tracker );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto h4trainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		return tptr->Have( TI.Get( L"H4" ).bdg );
	} );
	CPPUNIT_ASSERT( h4trainIt != tunities.cend() );
	auto h4train = *h4trainIt;
	CPPUNIT_ASSERT( h4train->IsTrain() && h4train->GetId() == trainId );
	auto notrain = ( tunities.back() == h4train ? tunities.front() : tunities.back() );
	CPPUNIT_ASSERT( !notrain->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::TrainThroughFaultFreeOnShuntSema()
{
	TI.DetermineStripBusy( L"C4", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	SetRandomOddInfo( tunities.back() );
	TI.DetermineStripBusy( L"E4", *tracker );
	tunities = trainCont->GetUnities();
	auto c4trainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		return tptr->Have( TI.Get( L"C4" ).bdg );
	} );
	CPPUNIT_ASSERT( c4trainIt != tunities.cend() );
	auto c4train = *c4trainIt;
	CPPUNIT_ASSERT( c4train->IsTrain() && c4train->GetId() == trainId );
	auto notrain = ( tunities.back() == c4train ? tunities.front() : tunities.back() );
	CPPUNIT_ASSERT( !notrain->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::TrainThroughFaultFreeOnShuntSema2()
{
	//тест нацелен на побочный эффект сортировки участков по имени
	TI.DetermineStripBusy( L"W5", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	SetRandomOddInfo( tunities.back() );
	TI.DetermineStripBusy( L"A5", *tracker );
	tunities = trainCont->GetUnities();
	auto w5trainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		return tptr->Have( TI.Get( L"W5" ).bdg );
	} );
	CPPUNIT_ASSERT( w5trainIt != tunities.cend() );
	auto w5train = *w5trainIt;
	CPPUNIT_ASSERT( w5train->IsTrain() && w5train->GetId() == trainId );
	auto notrain = ( tunities.back() == w5train ? tunities.front() : tunities.back() );
	CPPUNIT_ASSERT( !notrain->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}


void TC_TrackerIrregular::TwoTrainsThroughFaultFree()
{
	TI.DetermineStripBusy( L"A", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tunities.empty() );
	SetRandomOddInfo( tunities.back() );
	auto forwardTrainId = tunities.back()->GetId();
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities5 = trainCont->GetUnities();

	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker );
	SetRandomOddInfo( TI.Get( L"A" ).bdg );
	auto tunities2 = trainCont->GetUnities();
	auto atrainIt = find_if( tunities2.cbegin(), tunities2.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"A" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( atrainIt != tunities2.cend() );
	auto backTrainId = ( *atrainIt )->GetId();
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities3 = trainCont->GetUnities();

	TI.DetermineStripBusy( L"K", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	auto restrains = trainCont->GetUnities();
	CPPUNIT_ASSERT( restrains.size() == 2 );
	for ( auto train : restrains )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"K" ).bdg )
			CPPUNIT_ASSERT( train->GetId() == forwardTrainId );
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"Q" ).bdg );
			CPPUNIT_ASSERT( train->GetId() == backTrainId );
		}
	}

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::InsensitivityToInsignificantFree()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"J", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );
	TI.DetermineStripFree( L"J", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for_each( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		SetRandomOddInfo( tptr );
	} );
	auto cfgTrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return ( tplace.size() == 3 &&
			find( tplace.cbegin(), tplace.cend(), TI.Get( L"C" ).bdg ) != tplace.cend() &&
			find( tplace.cbegin(), tplace.cend(), TI.Get( L"F" ).bdg ) != tplace.cend() &&
			find( tplace.cbegin(), tplace.cend(), TI.Get( L"G" ).bdg ) != tplace.cend() &&
			tptr->IsTrain() );
	} );
	CPPUNIT_ASSERT( cfgTrainIt != tunities.cend() );
	auto cfgTrainPtr = *cfgTrainIt;
	auto cfgTrainId = cfgTrainPtr->GetId();
	const auto & cfgTrainPlace = cfgTrainPtr->GetPlace();

	auto iTrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return ( tplace.size() == 1 &&
			find( tplace.cbegin(), tplace.cend(), TI.Get( L"I" ).bdg ) != tplace.cend() &&
			tptr->IsTrain() );
	} );
	CPPUNIT_ASSERT( iTrainIt != tunities.cend() );
	auto iTrainPtr = *iTrainIt;
	auto iTrainId = iTrainPtr->GetId();
	const auto & iTrainPlace = iTrainPtr->GetPlace();
	CPPUNIT_ASSERT( cfgTrainId != 0 && iTrainId != 0 && cfgTrainId != iTrainId );

	TI.DetermineStripFree( L"J", *tracker ); //ничего не должно произойти

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );

	auto cfgTrainIt2 = find_if( tunities.cbegin(), tunities.cend(), [cfgTrainId]( TrainUnityCPtr tptr ){
		return ( tptr->GetId() == cfgTrainId );
	} );
	CPPUNIT_ASSERT( cfgTrainIt2 != tunities.cend() );
	CPPUNIT_ASSERT( ( *cfgTrainIt2 )->GetPlace() == cfgTrainPlace );

	auto iTrainIt2 = find_if( tunities.cbegin(), tunities.cend(), [iTrainId]( TrainUnityCPtr tptr ){
		return ( tptr->GetId() == iTrainId );
	} );
	CPPUNIT_ASSERT( iTrainIt2 != tunities.cend() );
	CPPUNIT_ASSERT( ( *iTrainIt2 )->GetPlace() == iTrainPlace );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerIrregular::BlinkingOnActiveRoute()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"102" ) ), list<wstring>( 1, L"Q" ) );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker ); //Arrival

	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false ); //Departure
	auto tunities = trainCont->GetUnities();
	list<TrackerEventPtr> tevents;
	TI.DetermineStripFree( L"F", *tracker, false );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.FlushData( *tracker, false ); //Снова Arrival

	TI.DetermineRouteUnset( L"Head102->G", *tracker );

	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false ); //выезд на закрытый светофор
	TI.DetermineStripBusy( L"X", *tracker ); // выход со станции с генерацией Departure, StationExit и SpanMove
	TI.DetermineStripFree( L"F", *tracker, false );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	tracker->TakeEvents( tevents );
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 9 );
	CPPUNIT_ASSERT( evvec[tevents.size()-3]->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( evvec[tevents.size()-2]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( evvec[tevents.size()-1]->GetCode() == HCode::SPAN_MOVE );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}