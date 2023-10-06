#include "stdafx.h"
#include "TC_TrackerTrainOddness.h"
#include "../Guess/TrainContainer.h"
#include "TestTracker.h"
#include "../Guess/ProcessInfoResult.h"
#include "../Guess/UnityConversion.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTrainOddness );

void TC_TrackerTrainOddness::setUp()
{
	TI.Reset();
}

void TC_TrackerTrainOddness::ThroughEvenTrainSema( Oddness initOddness, Oddness newOddness, bool withNumber )
{
	TrainContainer & muttrainCont = ( static_cast <TestTracker *>( tracker ) )->GetTrainContainer();
	wstring trainNum;
	if ( initOddness == Oddness::EVEN )
		trainNum = L"4456";
	else if ( initOddness == Oddness::ODD )
		trainNum = L"2311";
	else
		ASSERT( initOddness == Oddness::UNDEF );
	TrainDescr trainDescr( trainNum );

	//рассматриваются оба состояния головы светофора
	for ( unsigned int i = 0; i < 2; ++i )
	{
		TI.Reset();
		bool headIsClosed = ( i == 0 );
		headIsClosed ? TI.DetermineHeadClose( L"Head102", *tracker ) : TI.DetermineRouteSet( L"Head102->G", *tracker );
		TI.DetermineStripBusy( L"B", *tracker );
		TI.DetermineStripBusy( L"C", *tracker );

		//исходная четность
		auto trains = trainCont->GetUnities();
		CPPUNIT_ASSERT( trains.size() == 1 );
		auto train = trains.front();
		auto initId = train->GetId();
		if ( withNumber )
			SetInfo( train, trainDescr );
		else
		{
			SetInfo( train, TrainDescr() );
			muttrainCont.SetTrainOddness( initId, initOddness );
		}
		CPPUNIT_ASSERT( train->IsTrain() );
		CPPUNIT_ASSERT( train->GetOddness() == initOddness );

		//итоговая четность
		TI.DetermineStripFree( L"B", *tracker );
		TI.DetermineStripFree( L"C", *tracker, false );
		TI.DetermineStripBusy( L"F", *tracker, false );
		TI.FlushData( *tracker, false );
		trains = trainCont->GetUnities();
		auto trIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr train ){
			const auto & tplace = train->GetPlace();
			return ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"F" ).bdg ) != tplace.cend() );
		} );
		CPPUNIT_ASSERT( trIt != trains.cend() );
		auto ftrain = *trIt;
		if ( headIsClosed && initOddness != newOddness && initOddness != Oddness::UNDEF )
		{
			CPPUNIT_ASSERT( !ftrain->IsTrain() );
		}
		else
		{
			CPPUNIT_ASSERT( ftrain->IsTrain() );
			CPPUNIT_ASSERT( ftrain->GetOddness() == newOddness );
		}
		CPPUNIT_ASSERT( !TI.TakeExceptions() );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	}
}

void TC_TrackerTrainOddness::WoOddnessToEvenByTrainSema()
{
	ThroughEvenTrainSema( Oddness::UNDEF, Oddness::EVEN, true );
	ThroughEvenTrainSema( Oddness::UNDEF, Oddness::EVEN, false );
}

void TC_TrackerTrainOddness::WoOddnessToEvenByShuntSema()
{
	//рассматриваются оба состояния головы светофора
	TI.DetermineHeadClose( L"Head201S", *tracker );
	for ( unsigned int i = 0; i < 2; ++i )
	{
		TI.DetermineStripBusy( L"M2", *tracker );

		//исходная четность
		auto trains = trainCont->GetUnities();
		CPPUNIT_ASSERT( trains.size() == 1 );
		auto train = trains.front();
		auto initId = train->GetId();
		SetInfo( train, TrainDescr() );
		CPPUNIT_ASSERT( train->IsTrain() );
		CPPUNIT_ASSERT( train->GetOddness() == Oddness::UNDEF );

		//итоговая четность
		TI.DetermineStripBusy( L"D2", *tracker );
		trains = trainCont->GetUnities();
		CPPUNIT_ASSERT( trains.size() == 1 );
		auto train2 = trains.front();
		CPPUNIT_ASSERT( initId == train2->GetId() );
		CPPUNIT_ASSERT( train2->GetOddness() == Oddness::EVEN );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		TI.Reset();
		TI.DetermineHeadOpen( L"Head200", *tracker );
	}
}

void TC_TrackerTrainOddness::ThroughOddTrainSema( Oddness initOddness, Oddness newOddness, bool withNumber )
{
	TrainContainer & muttrainCont = ( static_cast <TestTracker *>( tracker ) )->GetTrainContainer();
	wstring trainNum;
	if ( initOddness == Oddness::EVEN )
		trainNum = L"4456";
	else if ( initOddness == Oddness::ODD )
		trainNum = L"2311";
	else
		ASSERT( initOddness == Oddness::UNDEF );
	TrainDescr trainDescr( trainNum );

	//рассматриваются оба состояния головы светофора
	for ( unsigned int i = 0; i < 2; ++i )
	{
		TI.Reset();
		bool headIsClosed = ( i == 0 );
		headIsClosed ? TI.DetermineHeadClose( L"Head101", *tracker ) : TI.DetermineRouteSet( L"Head101->A", *tracker );
		TI.DetermineStripBusy( L"F", *tracker );
		TI.DetermineStripBusy( L"C", *tracker );

		//исходная четность
		auto trains = trainCont->GetUnities();
		CPPUNIT_ASSERT( trains.size() == 1 );
		auto train = trains.front();
		auto initId = train->GetId();
		if ( withNumber )
			SetInfo( train, trainDescr );
		else
		{
			SetInfo( train, TrainDescr() );
			muttrainCont.SetTrainOddness( initId, initOddness );
		}
		CPPUNIT_ASSERT( train->IsTrain() );
		CPPUNIT_ASSERT( train->GetOddness() == initOddness );

		//итоговая четность
		TI.DetermineStripFree( L"F", *tracker );
		TI.DetermineStripBusy( L"B", *tracker, false );
		TI.DetermineStripFree( L"C", *tracker, false );
		TI.FlushData( *tracker, false );
		trains = trainCont->GetUnities();
		auto trIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr train ){
			const auto & tplace = train->GetPlace();
			return ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"B" ).bdg ) != tplace.cend() );
		} );
		CPPUNIT_ASSERT( trIt != trains.cend() );
		auto btrain = *trIt;
		if ( headIsClosed && initOddness != newOddness && initOddness != Oddness::UNDEF )
		{
			CPPUNIT_ASSERT( !btrain->IsTrain() );
		}
		else
		{
			CPPUNIT_ASSERT( btrain->IsTrain() );
			CPPUNIT_ASSERT( btrain->GetOddness() == newOddness );
		}
		CPPUNIT_ASSERT( !TI.TakeExceptions() );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	}
}

void TC_TrackerTrainOddness::WoOddnessToOddByTrainSema()
{
	ThroughOddTrainSema( Oddness::UNDEF, Oddness::ODD, true );
	ThroughOddTrainSema( Oddness::UNDEF, Oddness::ODD, false );
}

void TC_TrackerTrainOddness::OddToEvenByTrainSema()
{
	ThroughEvenTrainSema( Oddness::ODD, Oddness::EVEN, true );
	ThroughEvenTrainSema( Oddness::ODD, Oddness::EVEN, false );
}

void TC_TrackerTrainOddness::EvenToOddByTrainSema()
{
	ThroughOddTrainSema( Oddness::EVEN, Oddness::ODD, true );
	ThroughOddTrainSema( Oddness::EVEN, Oddness::ODD, false );
}

void TC_TrackerTrainOddness::EvenToEvenByTrainSema()
{
	ThroughEvenTrainSema( Oddness::EVEN, Oddness::EVEN, true );
	ThroughEvenTrainSema( Oddness::EVEN, Oddness::EVEN, false );
}

void TC_TrackerTrainOddness::OddToOddByTrainSema()
{
	ThroughOddTrainSema( Oddness::ODD, Oddness::ODD, true );
	ThroughOddTrainSema( Oddness::ODD, Oddness::ODD, false );
}

void TC_TrackerTrainOddness::WoOddnessToEvenByOppTrainSema()
{
	TI.DetermineStripBusy( L"B", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetInfo( *trains.cbegin(), TrainDescr() );
	CPPUNIT_ASSERT( ( *trains.cbegin() )->GetOddness() == Oddness::UNDEF );
	TI.DetermineStripBusy( L"C", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( ( *trains.cbegin() )->GetOddness() == Oddness::EVEN );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainOddness::WoOddnessToEvenByOppShuntSema()
{
	TI.DetermineStripBusy( L"O4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetInfo( *trains.cbegin(), TrainDescr() );
	CPPUNIT_ASSERT( ( *trains.cbegin() )->GetOddness() == Oddness::UNDEF );
	TI.DetermineStripBusy( L"R4", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( ( *trains.cbegin() )->GetOddness() == Oddness::EVEN );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainOddness::WoOddnessToOddByOppTrainSema()
{
	TI.DetermineStripBusy( L"F", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetInfo( *trains.cbegin(), TrainDescr() );
	CPPUNIT_ASSERT( ( *trains.cbegin() )->GetOddness() == Oddness::UNDEF );
	TI.DetermineStripBusy( L"C", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( ( *trains.cbegin() )->GetOddness() == Oddness::ODD );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainOddness::ChangeOddnessByStandardSpan()
{
	//смена четности (номера нет)
	SpanChangingOddnessCheck( L"G", TrainDescr(), Oddness::UNDEF, L"X", TrainDescr(), Oddness::EVEN );
	SpanChangingOddnessCheck( L"G", TrainDescr(), Oddness::ODD, L"X", TrainDescr(), Oddness::EVEN );
	SpanChangingOddnessCheck( L"G", TrainDescr(), Oddness::EVEN, L"X", TrainDescr(), Oddness::EVEN );
	SpanChangingOddnessCheck( L"J", TrainDescr(), Oddness::UNDEF, L"I", TrainDescr(), Oddness::ODD );
	SpanChangingOddnessCheck( L"J", TrainDescr(), Oddness::ODD, L"I", TrainDescr(), Oddness::ODD );
	SpanChangingOddnessCheck( L"J", TrainDescr(), Oddness::EVEN, L"I", TrainDescr(), Oddness::ODD );

	//смена номера и четности
	SpanChangingOddnessCheck( L"G", TrainDescr( L"2357" ), Oddness::ODD, L"X", TrainDescr( L"2358" ), Oddness::EVEN );
	SpanChangingOddnessCheck( L"G", TrainDescr( L"2357" ), Oddness::EVEN, L"X", TrainDescr( L"2358" ), Oddness::EVEN );
	SpanChangingOddnessCheck( L"J", TrainDescr( L"7258" ), Oddness::ODD, L"I", TrainDescr( L"7257" ), Oddness::ODD );
	SpanChangingOddnessCheck( L"J", TrainDescr( L"7258" ), Oddness::EVEN, L"I", TrainDescr( L"7257" ), Oddness::ODD );

	//смена четности (номер сохраняется)
	SpanChangingOddnessCheck( L"G", TrainDescr( L"3544" ), Oddness::ODD, L"X", TrainDescr( L"3544" ), Oddness::EVEN );
	SpanChangingOddnessCheck( L"G", TrainDescr( L"3544" ), Oddness::EVEN, L"X", TrainDescr( L"3544" ), Oddness::EVEN );
	SpanChangingOddnessCheck( L"J", TrainDescr( L"4521" ), Oddness::ODD, L"I", TrainDescr( L"4521" ), Oddness::ODD );
	SpanChangingOddnessCheck( L"J", TrainDescr( L"4521" ), Oddness::EVEN, L"I", TrainDescr( L"4521" ), Oddness::ODD );
}

void TC_TrackerTrainOddness::SpanChangingOddnessCheck( wstring fromPlace, const TrainDescr & initDescr, Oddness initOddness, wstring toPlace, const TrainDescr & resDescr, 
			Oddness resOddness )
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( initDescr ) ), list<wstring>( 1, fromPlace ) );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto initTrain = trains.back();
	auto & nconstTrainCont = tracker->GetTrainContainer();
	nconstTrainCont.SetTrainOddness( initTrain->GetId(), initOddness );
	TI.DetermineStripBusy( toPlace, *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto resTrain = trains.back();
	CPPUNIT_ASSERT( resTrain->GetId() == initTrain->GetId() );
	auto tdescrPtr = resTrain->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == resDescr );
	CPPUNIT_ASSERT( resTrain->GetOddness() == resOddness );
	DisformTrain( resTrain->GetId() );
	TI.DetermineStripFree( fromPlace, *tracker );
	TI.DetermineStripFree( toPlace, *tracker );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	tracker->Reset();
}

void TC_TrackerTrainOddness::ChangeOddnessByNonStandardSpan()
{
	//смена четности (номера нет)
	SpanChangingOddnessCheck( L"W2", TrainDescr(), Oddness::UNDEF, L"X2", TrainDescr(), Oddness::ODD );
	SpanChangingOddnessCheck( L"W2", TrainDescr(), Oddness::ODD, L"X2", TrainDescr(), Oddness::ODD );
	SpanChangingOddnessCheck( L"W2", TrainDescr(), Oddness::EVEN, L"X2", TrainDescr(), Oddness::ODD );
	SpanChangingOddnessCheck( L"Z2", TrainDescr(), Oddness::UNDEF, L"Y2", TrainDescr(), Oddness::EVEN );
	SpanChangingOddnessCheck( L"Z2", TrainDescr(), Oddness::ODD, L"Y2", TrainDescr(), Oddness::EVEN );
	SpanChangingOddnessCheck( L"Z2", TrainDescr(), Oddness::EVEN, L"Y2", TrainDescr(), Oddness::EVEN );

	//смена номера и четности
	SpanChangingOddnessCheck( L"W2", TrainDescr( L"4652" ), Oddness::ODD, L"X2", TrainDescr( L"4651" ), Oddness::ODD );
	SpanChangingOddnessCheck( L"W2", TrainDescr( L"4652" ), Oddness::EVEN, L"X2", TrainDescr( L"4651" ), Oddness::ODD );
	SpanChangingOddnessCheck( L"Z2", TrainDescr( L"9524" ), Oddness::ODD, L"Y2", TrainDescr( L"9524" ), Oddness::EVEN );
	SpanChangingOddnessCheck( L"Z2", TrainDescr( L"9524" ), Oddness::EVEN, L"Y2", TrainDescr( L"9524" ), Oddness::EVEN );

	//смена четности (номер сохраняется)
	SpanChangingOddnessCheck( L"W2", TrainDescr( L"6259" ), Oddness::ODD, L"X2", TrainDescr( L"6259" ), Oddness::ODD );
	SpanChangingOddnessCheck( L"W2", TrainDescr( L"6259" ), Oddness::EVEN, L"X2", TrainDescr( L"6259" ), Oddness::ODD );
	SpanChangingOddnessCheck( L"Z2", TrainDescr( L"6259" ), Oddness::ODD, L"Y2", TrainDescr( L"6260" ), Oddness::EVEN );
	SpanChangingOddnessCheck( L"Z2", TrainDescr( L"6259" ), Oddness::EVEN, L"Y2", TrainDescr( L"6260" ), Oddness::EVEN );
}

void TC_TrackerTrainOddness::ChangeOddnessForHouseholdOnSpanToStation()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8240" ) ), list<wstring>( 1, L"G" ) );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	const auto & tcharacts = TrainCharacteristics::instanceCRef();
	auto tdescrPtr = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && train->GetOddness() == Oddness::EVEN );
	auto inumfeature = tcharacts.GetPurposeFeature( tdescrPtr->GetNumber().getNumber() );
	CPPUNIT_ASSERT( tcharacts.IsHouseHoldTrain( inumfeature ) );

	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );

	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.size() == 1 );
	auto train2 = trains2.back();
	auto tdescrPtr2 = train2->GetDescrPtr();
	CPPUNIT_ASSERT( train2->IsTrain() );
	CPPUNIT_ASSERT( tdescrPtr2 );
	CPPUNIT_ASSERT( tdescrPtr2->GetNumber().getNumber() == 8239 && train2->GetOddness() == Oddness::ODD );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainOddness::UnidentifyForNoHouseholdOnSpanToStation()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"G" ) );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	const auto & tcharacts = TrainCharacteristics::instanceCRef();
	auto tdescrPtr = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && train->GetOddness() == Oddness::EVEN );
	auto inumfeature = tcharacts.GetPurposeFeature( tdescrPtr->GetNumber().getNumber() );
	CPPUNIT_ASSERT( !tcharacts.IsHouseHoldTrain( inumfeature ) );

	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );

	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.size() == 1 );
	auto train2 = trains2.back();
	auto tdescrPtr2 = train2->GetDescrPtr();
	CPPUNIT_ASSERT( train2->IsTrain() );
	CPPUNIT_ASSERT( tdescrPtr2 );
	CPPUNIT_ASSERT( tdescrPtr2->GetNumber().empty() && train2->GetOddness() == Oddness::UNDEF );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}