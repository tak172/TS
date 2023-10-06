#include "stdafx.h"
#include "TC_TrackerSpanMoving.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerSpanMoving );

void TC_TrackerSpanMoving::setUp()
{
	TI.Reset();
}

void TC_TrackerSpanMoving::PushForwardOnDisappear()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"821" ) ), list<wstring>( 1, L"LS10") ); //рождение на станции
	TI.DetermineStripBusy( L"LS9", *tracker ); //последующий выезд на перегон
	TI.DetermineStripBusy( L"LS8", *tracker );
	TI.DetermineStripFree( L"LS10", *tracker );
	TI.DetermineStripFree( L"LS9", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6205" ) ), list<wstring>( 1, L"LS6") );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"LS4") );

	auto train821Ptr = trainCont->GetUnity( TI.Get( L"LS8" ).bdg );
	auto train821Id = train821Ptr->GetId();
	auto train6205Ptr = trainCont->GetUnity( TI.Get( L"LS6" ).bdg );
	auto train6205Id = train6205Ptr->GetId();
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"LS4" ).bdg );
	auto notrainId = notrainPtr->GetId();

	auto tunities3 = trainCont->GetUnities();
	TI.DetermineStripFree( L"LS8", *tracker ); //исчезновение поезда под восстановление
	auto tunities2 = trainCont->GetUnities();
	TI.DetermineStripBusy( L"LS3", *tracker ); //новый поезд появляться не должен

	auto tunities = trainCont->GetUnities();

	//должно произойти смещение с восстановлением
	CPPUNIT_ASSERT( tunities.size() == 2 );
	CPPUNIT_ASSERT( !trainCont->IsDeadUnity( train821Id ) );
	train821Ptr = trainCont->GetUnity( train821Id );
	CPPUNIT_ASSERT( train821Ptr );
	const auto & tplace1 = train821Ptr->GetPlace();
	CPPUNIT_ASSERT( tplace1.size() == 1 && tplace1.back() == TI.Get( L"LS6" ).bdg );
	train6205Ptr = trainCont->GetUnity( train6205Id );
	const auto & tplace2 = train6205Ptr->GetPlace();
	CPPUNIT_ASSERT( tplace2.size() == 2 &&
		find( tplace2.cbegin(), tplace2.cend(), TI.Get( L"LS4" ).bdg ) != tplace2.cend() && 
		find( tplace2.cbegin(), tplace2.cend(), TI.Get( L"LS3" ).bdg ) != tplace2.cend() 
		);

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanMoving::Jump()
{
	TI.DetermineStripBusy( L"LG_PDP", *tracker );
	TI.DetermineStripBusy( L"LG_SL9", *tracker );
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"LG_PDP", *tracker );
	TI.DetermineStripFree( L"LG_SL9", *tracker );

	TI.DetermineStripBusy( L"LG_SL5", *tracker, false );
	TI.DetermineStripBusy( L"LG_SL3", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"LG_SL1", *tracker );

	list<TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 7 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get(L"LG_PDP").bdg );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::STATION_EXIT && eventsVec[1]->GetBadge() == TI.Get(L"LG_PDP").bdg  );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::SPAN_MOVE && eventsVec[2]->GetBadge() == TI.Get(L"LG_SL9").bdg  );
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::DEATH && eventsVec[3]->GetBadge() == TI.Get(L"LG_SL9").bdg  );
	CPPUNIT_ASSERT( eventsVec[4]->GetCode() == HCode::FORM && eventsVec[4]->GetBadge() == TI.Get(L"LG_SL5").bdg  );
	CPPUNIT_ASSERT( eventsVec[5]->GetCode() == HCode::SPAN_MOVE && eventsVec[5]->GetBadge() == TI.Get(L"LG_SL3").bdg  );
	CPPUNIT_ASSERT( eventsVec[6]->GetCode() == HCode::SPAN_MOVE && eventsVec[6]->GetBadge() == TI.Get(L"LG_SL1").bdg  );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}