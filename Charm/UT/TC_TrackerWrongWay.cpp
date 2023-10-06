#include "stdafx.h"
#include "TC_TrackerWrongWay.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/ProcessInfoResult.h"
#include "../Guess/UnityConversion.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerWrongWay );
void TC_TrackerWrongWay::setUp()
{
	TI.Reset();
}

bool TC_TrackerWrongWay::GoodMove( const std::wstring & strip1, const std::wstring & strip2, Oddness trainOddness )
{
	TI.DetermineStripBusy( strip1, *tracker );
	TrainContainer & tcontMutable = tracker->GetTrainContainer();
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto initId = train->GetId();
	if ( trainOddness == Oddness::UNDEF )
		tcontMutable.SetTrainInfo( train->GetId(), TrainDescr() );
	else
		trainOddness == Oddness::ODD ? SetRandomOddInfo( train ) : SetRandomEvenInfo( train );
	TI.DetermineStripBusy( strip2, *tracker );
	const auto & tevents = tracker->GetTrainEvents();
	bool good_way = find_if( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevent ){
		return tevent->GetCode() == HCode::WRONG_SPANWAY;
	} ) == tevents.cend();
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.front()->GetId() == initId );
	tracker->Reset();
	return good_way;
}

void TC_TrackerWrongWay::UnorientedNoNumber()
{
	CPPUNIT_ASSERT( GoodMove( L"TIM1", L"TIM2", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM1", L"TIM2", Oddness::EVEN ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM1", L"TIM2", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::UnorientedEvenNumber()
{
	CPPUNIT_ASSERT( !GoodMove( L"TIM3", L"TIM_E1", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( !GoodMove( L"TIM3", L"TIM_E1", Oddness::EVEN ) );
	CPPUNIT_ASSERT( !GoodMove( L"TIM3", L"TIM_E1", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::UnorientedOddNumber()
{
	CPPUNIT_ASSERT( GoodMove( L"TIM4", L"TIM_O1", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM4", L"TIM_O1", Oddness::EVEN ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM4", L"TIM_O1", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::UnorientedEvenNumberWithSema()
{
	CPPUNIT_ASSERT( !GoodMove( L"TIM5", L"TIM_E2", Oddness::UNDEF ) );
	//четный поезд проехать не сможет
	CPPUNIT_ASSERT( !GoodMove( L"TIM5", L"TIM_E2", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::SpanOrientedNoNumber()
{
	CPPUNIT_ASSERT( GoodMove( L"TIM6", L"TIM7", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM6", L"TIM7", Oddness::EVEN ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM6", L"TIM7", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::SpanOrientedEvenNumber()
{
	CPPUNIT_ASSERT( GoodMove( L"TIM8", L"TIM_E3", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM8", L"TIM_E3", Oddness::EVEN ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM8", L"TIM_E3", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::SpanOrientedOddNumber()
{
	CPPUNIT_ASSERT( GoodMove( L"TIM9", L"TIM_O2", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM9", L"TIM_O2", Oddness::EVEN ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM9", L"TIM_O2", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::StatOrientedNoNumber()
{
	CPPUNIT_ASSERT( !GoodMove( L"TIM10", L"TIM11", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( !GoodMove( L"TIM10", L"TIM11", Oddness::EVEN ) );
	CPPUNIT_ASSERT( !GoodMove( L"TIM10", L"TIM11", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::StatOrientedEvenNumber()
{
	CPPUNIT_ASSERT( !GoodMove( L"TIM12", L"TIM_E4", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( !GoodMove( L"TIM12", L"TIM_E4", Oddness::EVEN ) );
	CPPUNIT_ASSERT( !GoodMove( L"TIM12", L"TIM_E4", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::StatOrientedOddNumber()
{
	CPPUNIT_ASSERT( !GoodMove( L"TIM13", L"TIM_O3", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( !GoodMove( L"TIM13", L"TIM_O3", Oddness::EVEN ) );
	CPPUNIT_ASSERT( !GoodMove( L"TIM13", L"TIM_O3", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::CheckOrder()
{
	TI.DetermineStripBusy( L"TIM3", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );
	TI.DetermineStripBusy( L"TIM_E1", *tracker );
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 4 );
	auto teIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && (*teIt)->GetId() == trainId && (*teIt)->GetBadge() == TI.Get( L"TIM3" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_EXIT && (*teIt)->GetId() == trainId && (*teIt)->GetBadge() == TI.Get( L"TIM3" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::WRONG_SPANWAY && (*teIt)->GetId() == trainId && (*teIt)->GetBadge() == TI.Get( L"TIM_E1" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && (*teIt)->GetId() == trainId && (*teIt)->GetBadge() == TI.Get( L"TIM_E1" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::OneWaySpan()
{
	CPPUNIT_ASSERT( GoodMove( L"TIM14", L"TIM_OOW", Oddness::UNDEF ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM14", L"TIM_OOW", Oddness::EVEN ) );
	CPPUNIT_ASSERT( GoodMove( L"TIM14", L"TIM_OOW", Oddness::ODD ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::NoWrongWayWithFictiveStrip()
{
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	auto tunities = trainCont->GetUnities();
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( none_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::WRONG_SPANWAY;
	} ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::DeathThenRecoveryOnWrongSpanWithEvent()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"209" ) ), list<wstring>( 1, L"TIM3" ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	TI.DetermineStripFree( L"TIM3", *tracker );
	TI.DetermineStripBusy( L"TIM_E1", *tracker );
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 4 );
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
    CPPUNIT_ASSERT( chk_eq( evvec[ 0 ], HCode::FORM, L"TIM3" ) && evvec[ 0 ]->GetId() == trainId );
    CPPUNIT_ASSERT( chk_eq( evvec[ 1 ], HCode::DEATH, L"TIM3" ) && evvec[ 1 ]->GetId() == trainId );
    CPPUNIT_ASSERT( chk_eq( evvec[ 2 ], HCode::FORM, L"TIM_E1" ) && evvec[ 2 ]->GetId() == trainId );
    CPPUNIT_ASSERT( chk_eq( evvec[ 3 ], HCode::WRONG_SPANWAY, L"TIM_E1" ) && evvec[ 3 ]->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::DeathThenRecoveryOnWrongSpanWOEvent()
{
	//кратковременный возврат поезда на станцию и снова выход на перегон
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"210" ) ), list<wstring>( 1, L"TIM_E1" ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	TI.DetermineStripBusy( L"TIM3", *tracker );
	TI.DetermineStripFree( L"TIM_E1", *tracker );
	TI.DetermineStripFree( L"TIM3", *tracker );
	TI.DetermineStripBusy( L"TIM_E1", *tracker );

	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 4 );
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
    CPPUNIT_ASSERT( chk_eq( evvec[ 0 ], HCode::FORM, L"TIM_E1" ) && evvec[ 0 ]->GetId() == trainId );
    CPPUNIT_ASSERT( chk_eq( evvec[ 1 ], HCode::STATION_ENTRY, L"TIM3" ) && evvec[ 1 ]->GetId() == trainId );
    CPPUNIT_ASSERT( chk_eq( evvec[ 2 ], HCode::DEATH, L"TIM3" ) && evvec[ 2 ]->GetId() == trainId );
    CPPUNIT_ASSERT( chk_eq( evvec[ 3 ], HCode::FORM, L"TIM_E1" ) && evvec[ 3 ]->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::DeathThenRecoveryOnWrongSpanUniqueEvent()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"209" ) ), list<wstring>( 1, L"TIM3" ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	TI.DetermineStripFree( L"TIM3", *tracker );
	TI.DetermineStripBusy( L"TIM_E1", *tracker );
	TI.DetermineStripFree( L"TIM_E1", *tracker );
	TI.DetermineStripBusy( L"TIM_E5", *tracker );

	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 6 );
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( chk_eq( evvec[0], HCode::FORM, L"TIM3" ) && evvec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( chk_eq( evvec[1], HCode::DEATH, L"TIM3" ) && evvec[1]->GetId() == trainId );
	CPPUNIT_ASSERT( chk_eq( evvec[2], HCode::FORM, L"TIM_E1" ) && evvec[2]->GetId() == trainId );
	CPPUNIT_ASSERT( chk_eq( evvec[3], HCode::WRONG_SPANWAY, L"TIM_E1" ) && evvec[3]->GetId() == trainId );
	CPPUNIT_ASSERT( chk_eq( evvec[4], HCode::DEATH, L"TIM_E1" ) && evvec[4]->GetId() == trainId );
	CPPUNIT_ASSERT( chk_eq( evvec[5], HCode::FORM, L"TIM_E5" ) && evvec[5]->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::BadHeadOrientationWhileWrongWayWithRecovery()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"209" ) ), list<wstring>( 1, L"TIM3" ) );
	TI.DetermineStripFree( L"TIM3", *tracker );

	TI.DetermineStripBusy( L"TIM3", *tracker, false );
	TI.DetermineStripBusy( L"TIM_E1", *tracker, false );
	TI.DetermineStripBusy( L"TIM_E5", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerWrongWay::BadHeadOrientationWhileWrongWayWithRecovery2()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"209" ) ), list<wstring>( 1, L"TIM15" ) );
	TI.DetermineStripBusy( L"TIM3", *tracker );
	TI.DetermineStripFree( L"TIM15", *tracker );
	TI.DetermineStripFree( L"TIM3", *tracker );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"TIM_E1", *tracker, false );
	TI.DetermineStripBusy( L"TIM3", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}