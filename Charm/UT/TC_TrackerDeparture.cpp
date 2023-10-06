#include "stdafx.h"
#include "TC_TrackerDeparture.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerDeparture );

void TC_TrackerDeparture::setUp()
{
	TI.Reset();
	tracker->Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.cargoTransArrivDurMs = 2000;
	timConstr.passenNotSpecTransArrivDurMs = 2000;
	timConstr.passenSpecTransArrivDurMs = 2000;
}

void TC_TrackerDeparture::Departure()
{
	const auto & tmConstr = tracker->GetTimeConstraints();
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	TI.IncreaseTime( max( tmConstr.cargoTransArrivDurMs, tmConstr.passenNotSpecTransArrivDurMs ) + 1000 );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 5 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( eventsVec[4]->GetCode() == HCode::DEPARTURE );
	auto detPtr = eventsVec[4]->GetDetails();
	CPPUNIT_ASSERT( detPtr && detPtr->optCode == TI.Get( L"X" ).bdg.num() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::OpenSemaDepartureWithRoute()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head102", *tracker );
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

	//задание маршрута
	TI.DetermineRouteSet( L"Head102->G", *tracker );

	//отправление с п/о пути
	TI.DetermineStripBusy( L"F", *tracker );
	
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::OpenSemaDepartureWORoute()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head102", *tracker );
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

	//открытие светофора без задания маршрута
	TI.DetermineHeadOpen( L"Head102", *tracker );

	//отправление с п/о пути
	TI.DetermineStripBusy( L"F", *tracker );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::OpenMultiWayDeparture()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head400", *tracker );
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( !trains.empty() );
	auto train = trains.back();
	SetRandomEvenInfo( train );
	TI.DetermineStripBusy( L"O4", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );
	TI.DetermineStripFree( L"A4", *tracker );
	TI.DetermineStripFree( L"O4", *tracker );

	//открытие светофора
	TI.DetermineRouteSet( L"Head400->K4", *tracker );

	//отправление с п/о пути
	TI.DetermineStripFree( L"R4", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker );

	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::CloseWayExit()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head102", *tracker );
	PullingTailToCWay();
	//выезд на запрещающий сигнал светофора
	TI.DetermineStripBusy( L"F", *tracker );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::CloseWayExitAndStationExit()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head102", *tracker );
	PullingTailToCWay();
	//выезд на запрещающий сигнал светофора
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	//выезд со станции
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 7 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::WOSemaWayExit()
{
	PullingTailToEWay();
	TI.DetermineStripBusy( L"G", *tracker );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::WOSemaWayExitAndStationExit()
{
	PullingTailToEWay();
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"E", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 7 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::ARRIVAL );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::BanTransitionAfterStart()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	wstring stripsArr[] = { L"A", L"B", L"C", L"F", L"G" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEATH );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::FormDepartureAndSpanMove()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker ); //отправление
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker ); //ошибочная генерация второго события отправления
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 4 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::FormSpanMoveAndLeaveAD()
{
	//открытие светофоров и установка стрелок
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"15", *tracker );
	TI.DetermineSwitchPlus( L"16", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineRouteSet( L"Head106->P", *tracker );

	//появление поезда на п/о пути
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"B", *tracker );
	TI.DetermineStripBusy( L"F", *tracker, false ); //частичный выезд
	TI.DetermineHeadClose( L"Head102", *tracker, false ); //перекрытие светофора
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false );

	//сначала выход со станции...
	TI.DetermineStripBusy( L"X", *tracker );

	//..потом полный выезд с п/о пути
	TI.DetermineStripFree( L"C", *tracker );

	//проследование следующей станции
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );
	TI.DetermineStripFree( L"X", *tracker );
	TI.DetermineStripBusy( L"J", *tracker ); //вход на станцию
	TI.DetermineStripFree( L"I", *tracker );
	TI.DetermineStripBusy( L"L", *tracker );
	TI.DetermineStripBusy( L"M", *tracker );
	TI.DetermineStripBusy( L"O", *tracker );
	TI.DetermineStripBusy( L"P", *tracker );
	TI.DetermineStripBusy( L"R", *tracker ); //выход со станции

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 9 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt++ )->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt++ )->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"X" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"I" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_ENTRY && ( *teIt++ )->GetBadge() == TI.Get( L"J" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::TRANSITION && ( *teIt++ )->GetBadge() == TI.Get( L"M" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::STATION_EXIT && ( *teIt++ )->GetBadge() == TI.Get( L"P" ).bdg );
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::SPAN_MOVE && ( *teIt++ )->GetBadge() == TI.Get( L"R" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::CompositeRouteDeparture()
{
	TI.DetermineSwitchPlus( L"62", *tracker );
	TI.DetermineRouteSet( L"Head602T->H6", *tracker );
	TI.DetermineRouteSet( L"Head604->I6", *tracker );
	TI.DetermineStripBusy( L"F6", *tracker );
	SetRandomEvenInfo( TI.Get( L"F6" ).bdg );
	TI.DetermineStripBusy( L"G6", *tracker );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DuplicateEventOnReborn()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"540" ) ), list<wstring>( 1, L"C" ) ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	auto trainId = trainPtr->GetId();
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineRouteUnset( L"Head102->G", *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );

	trackerEvents = tracker->GetTrainEvents();

	vector<TrackerEventPtr> evVec( trackerEvents.cbegin(), trackerEvents.cend() );
	CPPUNIT_ASSERT( evVec.size() == 6 );
	CPPUNIT_ASSERT( evVec[0]->GetId() == trainId && evVec[0]->GetCode() == HCode::FORM && evVec[0]->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( evVec[1]->GetId() == trainId && evVec[1]->GetCode() == HCode::DEPARTURE && evVec[1]->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( evVec[2]->GetId() == trainId && evVec[2]->GetCode() == HCode::DEATH && evVec[2]->GetBadge() == TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( evVec[3]->GetId() == trainId && evVec[3]->GetCode() == HCode::FORM && evVec[3]->GetBadge() == TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( evVec[4]->GetId() == trainId && evVec[4]->GetCode() == HCode::STATION_EXIT && evVec[4]->GetBadge() == TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( evVec[5]->GetId() == trainId && evVec[5]->GetCode() == HCode::SPAN_MOVE && evVec[5]->GetBadge() == TI.Get( L"X" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForExitingNoTrain()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );
	auto tunities = trainCont->GetUnities();

	auto depMom = TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	auto outMom = TI.DetermineStripBusy( L"X", *tracker );

	tunities = trainCont->GetUnities();

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetTime() == depMom );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetTime() == depMom );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetBadge() == TI.Get( L"G" ).bdg && evvec[2]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetBadge() == TI.Get( L"X" ).bdg && evvec[3]->GetTime() == outMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForExitingNoTrain2()
{
	//отложенное отправление в случае кратковременного исчезновения маневровой
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );

	auto depMom = TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripFree( L"G", *tracker ); //исчезновение

	auto outMom = TI.DetermineStripBusy( L"X", *tracker ); //восстановление

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();

	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetTime() == depMom );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetTime() == depMom );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetBadge() == TI.Get( L"G" ).bdg && evvec[2]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetBadge() == TI.Get( L"X" ).bdg && evvec[3]->GetTime() == outMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForExitingNoTrain3()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2513" ) ), list<wstring>( 1, L"C" ) );
	auto depMom = TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker ); //исчезновение
	auto outMom = TI.DetermineStripBusy( L"X", *tracker ); //восстановление

	auto tunities = trainCont->GetUnities();

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 6 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetDetails()->tdptr && *evvec[0]->GetDetails()->tdptr == TrainDescr( L"2513" ) );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DISFORM );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::FORM && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg && evvec[2]->GetTime() == depMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DEPARTURE && evvec[3]->GetBadge() == TI.Get( L"C" ).bdg && evvec[3]->GetTime() == depMom );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::STATION_EXIT && evvec[4]->GetBadge() == TI.Get( L"G" ).bdg && evvec[4]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::SPAN_MOVE && evvec[5]->GetBadge() == TI.Get( L"X" ).bdg && evvec[5]->GetTime() == outMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForExitingNoTrain4()
{
	TI.DetermineSwitchPlus( L"51", *tracker );
	TI.DetermineSwitchMinus( L"52", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"5656" ) ), list<wstring>( 1, L"F5" ) );
	auto depMom = TI.DetermineStripBusy( L"D5", *tracker );
	TI.DetermineStripFree( L"F5", *tracker );
	wstring arr[] = { L"D5", L"B5", L"A5", L"V5", L"W5", L"X5" };
	TI.ImitateMotion( vector<wstring>( begin( arr ), end( arr ) ), *tracker );

	TI.DetermineStripFree( L"X5", *tracker ); //исчезновение
	auto recovMom = TI.DetermineStripBusy( L"BH5", *tracker ); //восстановление

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 8 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetDetails()->tdptr && *evvec[0]->GetDetails()->tdptr == TrainDescr( L"5656" ) );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DISFORM );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::FORM && evvec[2]->GetBadge() == TI.Get( L"F5" ).bdg && evvec[2]->GetTime() == depMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DEPARTURE && evvec[3]->GetBadge() == TI.Get( L"F5" ).bdg && evvec[3]->GetTime() == depMom );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::STATION_EXIT && evvec[4]->GetBadge() == TI.Get( L"X5" ).bdg && evvec[4]->GetTime() == recovMom );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::STATION_ENTRY && evvec[5]->GetBadge() == TI.Get( L"BG5" ).bdg && evvec[5]->GetTime() == recovMom );

	CPPUNIT_ASSERT( evvec[6]->GetCode() == HCode::STATION_EXIT && evvec[6]->GetBadge() == TI.Get( L"BG5" ).bdg && evvec[6]->GetTime() == recovMom );
	CPPUNIT_ASSERT( evvec[7]->GetCode() == HCode::SPAN_MOVE && evvec[7]->GetBadge() == TI.Get( L"BH5" ).bdg && evvec[7]->GetTime() == recovMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::ADOutThenReturnThenUpgradeThenExit()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );
	auto adOutMom1 = TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	auto tevents3 = tracker->GetTrainEvents();
	auto trainPtr = SetRandomEvenInfo( TI.Get( L"C" ).bdg );
	auto formMom = TI.GetTimeMoment();
	auto tevents2 = tracker->GetTrainEvents();
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	auto adOutMom2 = TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );

	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetTime() == formMom );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetTime() == adOutMom2 );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetBadge() == TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetBadge() == TI.Get( L"X" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForFormingTrain()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2566" ) ), list<wstring>( 1, L"C" ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto trainId = trainPtr->GetId();
	auto outMom = TI.DetermineStripBusy( L"F", *tracker );
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( notrainPtr && !notrainPtr->IsTrain() );
	auto notrainId = notrainPtr->GetId();
	CPPUNIT_ASSERT( trainId != notrainId );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	auto exitMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 5 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::FORM && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetId() == notrainId && 
		evvec[1]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DEPARTURE && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg && evvec[2]->GetId() == notrainId && 
		evvec[2]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::STATION_EXIT && evvec[3]->GetBadge() == TI.Get( L"G" ).bdg && evvec[3]->GetId() == notrainId && 
		evvec[3]->GetTime() == exitMom );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::SPAN_MOVE && evvec[4]->GetBadge() == TI.Get( L"X" ).bdg && evvec[4]->GetId() == notrainId && 
		evvec[4]->GetTime() == exitMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForFormingTrain2()
{
	TI.DetermineSwitchMinus( L"56", *tracker );
	TI.DetermineSwitchMinus( L"57", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"AC5" ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"AC5" ).bdg );
	auto trainId = trainPtr->GetId();
	auto outMom = TI.DetermineStripBusy( L"AD5", *tracker );
	TI.DetermineStripBusy( L"AF5", *tracker );
	TI.DetermineStripFree( L"AC5", *tracker );
	TI.DetermineStripFree( L"AD5", *tracker );
	TI.DetermineRouteSet( L"Head510->AJ5", *tracker );
	auto upgradeMom = TI.DetermineStripBusy( L"AG5", *tracker );
	CPPUNIT_ASSERT( outMom != upgradeMom );

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 2 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"AC5" ).bdg && evvec[0]->GetId() == trainId && 
		evvec[0]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"AC5" ).bdg && evvec[1]->GetId() == trainId && 
		evvec[1]->GetTime() == outMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForFormingTrain3()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2567" ) ), list<wstring>( 1, L"C" ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto trainId = trainPtr->GetId();
	auto outMom = TI.DetermineStripBusy( L"F", *tracker );
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( notrainPtr && !notrainPtr->IsTrain() );
	auto notrainId = notrainPtr->GetId();
	CPPUNIT_ASSERT( trainId != notrainId );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	auto exitMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 6 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DISFORM && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetId() == trainId );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::FORM && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg && evvec[2]->GetId() == notrainId && 
		evvec[2]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DEPARTURE && evvec[3]->GetBadge() == TI.Get( L"C" ).bdg && evvec[3]->GetId() == notrainId && 
		evvec[3]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::STATION_EXIT && evvec[4]->GetBadge() == TI.Get( L"G" ).bdg && evvec[4]->GetId() == notrainId && 
		evvec[4]->GetTime() == exitMom );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::SPAN_MOVE && evvec[5]->GetBadge() == TI.Get( L"X" ).bdg && evvec[5]->GetId() == notrainId && 
		evvec[5]->GetTime() == exitMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForFormingTrain4()
{
	TI.DetermineSwitchMinus( L"52", *tracker );
	TI.DetermineSwitchPlus( L"51", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2566" ) ), list<wstring>( 1, L"F5" ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"F5" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto trainId = trainPtr->GetId();
	auto outMom = TI.DetermineStripBusy( L"D5", *tracker );
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"D5" ).bdg );
	CPPUNIT_ASSERT( notrainPtr && !notrainPtr->IsTrain() );
	auto notrainId = notrainPtr->GetId();
	CPPUNIT_ASSERT( trainId != notrainId );

	TI.DetermineStripFree( L"F5", *tracker );

	TI.DetermineStripBusy( L"B5", *tracker );
	TI.DetermineStripBusy( L"A5", *tracker );
	TI.DetermineStripBusy( L"V5", *tracker );
	TI.DetermineStripBusy( L"W5", *tracker );
	TI.DetermineStripBusy( L"X5", *tracker );
	TI.DetermineStripFree( L"D5", *tracker );
	TI.DetermineStripFree( L"B5", *tracker );
	TI.DetermineStripFree( L"A5", *tracker );
	TI.DetermineStripFree( L"V5", *tracker );
	TI.DetermineStripFree( L"W5", *tracker );

	TI.DetermineStripFree( L"X5", *tracker );
	auto exitMom = TI.DetermineStripBusy( L"BG5", *tracker );

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 6 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"F5" ).bdg && evvec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DISFORM && evvec[1]->GetBadge() == TI.Get( L"F5" ).bdg && evvec[1]->GetId() == trainId );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::FORM && evvec[2]->GetBadge() == TI.Get( L"F5" ).bdg && evvec[2]->GetId() == notrainId && 
		evvec[2]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DEPARTURE && evvec[3]->GetBadge() == TI.Get( L"F5" ).bdg && evvec[3]->GetId() == notrainId && 
		evvec[3]->GetTime() == outMom );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::STATION_EXIT && evvec[4]->GetBadge() == TI.Get( L"X5" ).bdg && evvec[4]->GetId() == notrainId && 
		evvec[4]->GetTime() == exitMom );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::STATION_ENTRY && evvec[5]->GetBadge() == TI.Get( L"BG5" ).bdg && evvec[5]->GetId() == notrainId && 
		evvec[5]->GetTime() == exitMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForFormingTrain5()
{
	TI.DetermineSwitchMinus( L"56", *tracker );
	TI.DetermineSwitchMinus( L"57", *tracker );

	TI.DetermineRouteSet( L"Head510->AJ5", *tracker );
	TI.DetermineStripBusy( L"AC5", *tracker );
	auto headOutMom = TI.DetermineStripBusy( L"AD5", *tracker );
	TI.DetermineStripBusy( L"AF5", *tracker );
	TI.DetermineStripBusy( L"AG5", *tracker ); //FORM
	TI.DetermineStripBusy( L"AI5", *tracker );
	TI.DetermineStripBusy( L"AJ5", *tracker );
	TI.DetermineStripFree( L"AD5", *tracker );
	TI.DetermineStripBusy( L"AT5", *tracker );

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetTime() == headOutMom );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetTime() == headOutMom );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureADOutUpdate()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"P3" ) );
	TI.DetermineStripBusy( L"R3", *tracker );
	TI.DetermineStripFree( L"P3", *tracker );
	TI.DetermineStripBusy( L"S3", *tracker );
	TI.DetermineStripFree( L"R3", *tracker );
	auto secOutMom = TI.DetermineStripBusy( L"T3", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	const auto & tunity = tunities.back();
	const auto & thistory = tunity->GetHistory();
	auto lastADOutPtr = thistory.RecentIncident( ADIncident::ArrdepState::PARTIAL_LEAVED );

	CPPUNIT_ASSERT( lastADOutPtr && lastADOutPtr->from == TI.Get( L"S3" ).bdg && lastADOutPtr->moment == secOutMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureADOutUpdate2()
{
	LocateUnityByForce( nullptr, list<wstring>( 1, L"D" ) );
	auto tunityPtr = trainCont->GetUnity( TI.Get( L"D" ).bdg );
	CPPUNIT_ASSERT( tunityPtr );
	auto tunityId = tunityPtr->GetId();

	TI.DetermineSwitchPlus( L"11", *tracker );
	auto doutMom = TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"D", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	auto semaOpenMom = TI.DetermineRouteSet( L"Head102->G", *tracker );
	auto adOutMom = TI.DetermineStripBusy( L"F", *tracker );

	CPPUNIT_ASSERT( semaOpenMom < adOutMom );

	auto tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();

	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 2 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetId() == tunityId && 
		evvec[0]->GetTime() == adOutMom );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetId() == tunityId && 
		evvec[1]->GetTime() == adOutMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DelayedDepartureForLongTrain()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"C" ) );

	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	auto exitMom = TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripFree( L"C", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg /*&& evvec[1]->GetTime() == depMom*/ /*это требует доработки*/  );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetBadge() == TI.Get( L"G" ).bdg && evvec[2]->GetTime() == exitMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetBadge() == TI.Get( L"X" ).bdg && evvec[3]->GetTime() == exitMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DepartureWithRecoveryForExitingNoTrain()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );
	auto adOutMom1 = TI.DetermineStripBusy( L"F", *tracker ); //фиксация первого выхода с ПО-пути
	TI.DetermineStripFree( L"F", *tracker ); //возвращение

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunityPtr = tunities.back();
	CPPUNIT_ASSERT( !tunityPtr->IsTrain() );

	//вторичный выход с ПО-пути через исчезновение с восстановлением
	TI.DetermineStripFree( L"C", *tracker );
	auto adOutMom2 = TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	auto statExitMom = TI.DetermineStripBusy( L"X", *tracker );

	tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetTime() == adOutMom2 );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetTime() == adOutMom2 );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetBadge() == TI.Get( L"G" ).bdg && evvec[2]->GetTime() == statExitMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetBadge() == TI.Get( L"X" ).bdg && evvec[3]->GetTime() == statExitMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::DepartureWithRecoveryForExitingNoTrain2()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"C" ) );

	auto adOutMom1 = TI.DetermineStripBusy( L"F", *tracker ); //фиксация первого выхода с ПО-пути
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker ); //возвращение

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunityPtr = tunities.back();
	CPPUNIT_ASSERT( !tunityPtr->IsTrain() );

	//вторичный выход с ПО-пути
	auto adOutMom2 = TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripFree( L"G", *tracker ); //пропадание

	auto statExitMom = TI.DetermineStripBusy( L"X", *tracker ); //восстановление

	tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"C" ).bdg && evvec[0]->GetTime() == adOutMom2 );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetTime() == adOutMom2 );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetBadge() == TI.Get( L"G" ).bdg && evvec[2]->GetTime() == statExitMom );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetBadge() == TI.Get( L"X" ).bdg && evvec[3]->GetTime() == statExitMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDeparture::MixingUpDepartureFactOnRecovering()
{
	TI.DetermineStripBusy( L"AH", *tracker ); //маневровая ПЕ1
	TI.DetermineSwitchMinus( L"17", *tracker );
	TI.DetermineStripBusy( L"AC", *tracker ); //маневровая ПЕ2
	TI.DetermineStripBusy( L"AB", *tracker );
	TI.DetermineStripBusy( L"AA", *tracker );
	TI.DetermineStripFree( L"AC", *tracker );
	TI.DetermineStripFree( L"AB", *tracker );
	TI.DetermineSwitchPlus( L"17", *tracker );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"AD", *tracker );
	TI.DetermineStripBusy( L"AE", *tracker, false );
	TI.DetermineStripFree( L"AA", *tracker, false );
	TI.DetermineStripFree( L"AD", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"AE", *tracker ); //уничтожение ПЕ2 с восстановлением

	tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"AE", *tracker, false );
	TI.DetermineStripFree( L"AD", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"AE", *tracker );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}