#include "stdafx.h"
#include "TC_TrackerFormDisform.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/FieldGraph.h"
#include "../Guess/TrainInfoPackage.h"
#include "../Guess/ProcessInfoResult.h"
#include "../Guess/UnityConversion.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerFormDisform );

void TC_TrackerFormDisform::setUp()
{
	TI.Reset();
}

void TC_TrackerFormDisform::NoDisformEmptyNumber()
{
	//прогон поезда без номера от одного перегона к другому и обратно
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	wstring stripsArr[] = { L"I", L"X", L"G", L"F", L"C", L"B", L"A", L"Q", L"W" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );

	list <TrackerEventPtr> tevents = tracker->GetTrainEvents();
	auto tevIt = find_if( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevent ){
		return tevent->GetCode() == HCode::DISFORM;
	} );
	CPPUNIT_ASSERT( tevIt == tevents.cend() );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( trains.back()->GetOddness() == Oddness::ODD );
	TrainContainer & mutTrainCont = tracker->GetTrainContainer();
	mutTrainCont.SetTrainOddness( trains.back()->GetId(), Oddness::UNDEF );

	//в обратную сторону
	reverse( stripsSeq.begin(), stripsSeq.end() );
	TI.ImitateMotion( stripsSeq, *tracker );
	tevents = tracker->GetTrainEvents();
	tevIt = find_if( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevent ){
		return tevent->GetCode() == HCode::DISFORM;
	} );
	CPPUNIT_ASSERT( tevIt == tevents.cend() );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::NoDisformCauseLocoMove()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto oldTrainId = train->GetId();
	SetRandomOddInfo( train );

	//отъезд локомотива (формирование нового поезда). расформирования нет:
	TI.DetermineStripBusy( L"F", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	auto newTrainIt = find_if( trains.cbegin(), trains.cend(), [oldTrainId]( TrainUnityCPtr trainPtr ){
		return trainPtr->GetId() != oldTrainId;
	} );
	CPPUNIT_ASSERT( newTrainIt != trains.cend() );
	auto newTrainId = (*newTrainIt)->GetId();
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	trains = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	//формирование старого поезда
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"C" ).bdg && eventsVec[0]->GetId() == oldTrainId );
	//формирование нового поезда
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::FORM && eventsVec[1]->GetBadge() == TI.Get( L"C" ).bdg && eventsVec[1]->GetId() == newTrainId );
	//отправление нового поезда
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::DEPARTURE && eventsVec[2]->GetBadge() == TI.Get( L"C" ).bdg && eventsVec[2]->GetId() == newTrainId );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::LeaveADWayOnOppositeOpenedRoute()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"1" ) ), list<wstring>( 1, L"C" ) );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );

	auto train = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( train );
	auto initTrainId = train->GetId();

	//отъезд нового поезда с расформированием.
	TI.DetermineStripBusy( L"F", *tracker );

	auto trains = trainCont->GetUnities();
	auto newTrainIt = find_if( trains.cbegin(), trains.cend(), [initTrainId]( TrainUnityCPtr trainPtr ){
		return trainPtr->GetId() != initTrainId;
	} );
	CPPUNIT_ASSERT( newTrainIt != trains.cend() );
	auto newTrain = *newTrainIt;
	auto newTrainId = newTrain->GetId();

	CPPUNIT_ASSERT( newTrain->IsTrain() );
	time_t disformTime = TI.DetermineStripFree( L"C", *tracker ); //расформирование
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	TI.DetermineStripBusy( L"G", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 4 );
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	//формирование старого поезда
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"C" ).bdg && eventsVec[0]->GetId() == initTrainId );
	//формирование нового поезда
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::FORM && eventsVec[1]->GetBadge() == TI.Get( L"C" ).bdg && eventsVec[1]->GetId() == newTrainId );
	//отправление нового поезда
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::DEPARTURE && eventsVec[2]->GetBadge() == TI.Get( L"C" ).bdg && eventsVec[2]->GetId() == newTrainId );
	//расформирование старого поезда
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::DISFORM && eventsVec[3]->GetBadge() == TI.Get( L"C" ).bdg && eventsVec[3]->GetId() == initTrainId &&
		eventsVec[3]->GetTime() == disformTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::LeaveADWayOnOppositeClosedRoute()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );

	//формирование поезда с четным номером:
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.front();
	auto oldIden = tunity->GetId();
	SetRandomEvenInfo( tunity );

	//прибытие на п/о путь:
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	auto arrTime = TI.DetermineStripFree( L"B", *tracker );

	//обратный выезд с п/о пути:
	time_t adOutTime = TI.DetermineStripBusy( L"B", *tracker );
	tunities = trainCont->GetUnities();
	auto newTUnityIt = find_if( tunities.cbegin(), tunities.cend(), [oldIden]( TrainUnityCPtr trainPtr ){
		return trainPtr->GetId() != oldIden;
	} );
	CPPUNIT_ASSERT( newTUnityIt != tunities.cend() );
	auto newTUnity = *newTUnityIt;
	CPPUNIT_ASSERT( !newTUnity->IsTrain() );

	time_t delTime = TI.DetermineStripFree( L"C", *tracker );
	tunities = trainCont->GetUnities();

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 4 );
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( all_of( evvec.cbegin(), evvec.cend(), [oldIden]( TrackerEventPtr tevPtr ){
		return tevPtr->GetId() == oldIden;
	} ) );
	//формирование старого поезда
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"Q" ).bdg );
	//въезд на станцию
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::STATION_ENTRY && evvec[1]->GetBadge() == TI.Get( L"A" ).bdg );
	//прибытие на п/о путь
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::ARRIVAL && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg );
	//расформирование старого поезда
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DISFORM && evvec[3]->GetBadge() == TI.Get( L"C" ).bdg && evvec[3]->GetTime() == arrTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::LeaveADWayOnTrainSemaAndDisformOnSema()
{
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );

	//появление четного поезда
	TI.DetermineStripBusy( L"S4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomEvenInfo( train );

	//выезд
	TI.DetermineHeadClose( L"Head400", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker, false );
	TI.DetermineStripFree( L"S4", *tracker, false );
	auto leaveTime = TI.FlushData( *tracker, false );

	//расформирование на светофоре чужой четности
	wstring stripsArr[] = { L"F4", L"G4", L"H4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	TI.DetermineSwitchPlus( L"44", *tracker );
	wstring stripsArr2[] = { L"H4", L"U4", L"V4", L"W4" };
	vector <wstring> stripsSeq2( begin( stripsArr2 ), end( stripsArr2 ) );
	TI.ImitateMotion( stripsSeq2, *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.size() == 1 );
	auto train2 = trains.front();
	CPPUNIT_ASSERT( !train2->IsTrain() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 2 );
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	//формирование
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"S4" ).bdg && eventsVec[0]->GetId() == trainId );
	//расформирование
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::DISFORM && eventsVec[1]->GetBadge() == TI.Get( L"S4" ).bdg && eventsVec[1]->GetId() == trainId 
		&& eventsVec[1]->GetTime() == leaveTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::LeaveADWayOnShuntSemaAndDisformOnSema()
{
	TI.DetermineSwitchMinus( L"41", *tracker );

	//появление нечетного поезда
	TI.DetermineStripBusy( L"R4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );

	//выезд
	TI.DetermineHeadClose( L"Head403S", *tracker );
	TI.DetermineStripBusy( L"O4", *tracker, false );
	TI.DetermineStripFree( L"R4", *tracker, false );
	auto leaveTime = TI.FlushData( *tracker, false );

	//расформирование на светофоре чужой четности
	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripFree( L"O4", *tracker );
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	wstring stripsArr[] = { L"A4", L"B4", L"C4", L"D4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.size() == 1 );
	auto train2 = trains.front();
	CPPUNIT_ASSERT( !train2->IsTrain() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 2 );
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	//формирование
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"R4" ).bdg && eventsVec[0]->GetId() == trainId );
	//расформирование
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::DISFORM && eventsVec[1]->GetBadge() == TI.Get( L"R4" ).bdg && eventsVec[1]->GetId() == trainId &&
		eventsVec[1]->GetTime() == leaveTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::LeaveADWayWOSemaAndDisformOnSema()
{
	//появление нечетного поезда
	TI.DetermineStripBusy( L"K3", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );

	//выезд
	TI.DetermineStripBusy( L"L3", *tracker, false );
	TI.DetermineStripFree( L"K3", *tracker, false );
	auto partLeaveTime = TI.FlushData( *tracker, false );

	//расформирование на светофоре чужой четности
	TI.DetermineStripBusy( L"M3", *tracker );
	TI.DetermineStripFree( L"L3", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.size() == 1 );
	auto train2 = trains.front();
	CPPUNIT_ASSERT( !train2->IsTrain() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 2 );
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	//формирование
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"K3" ).bdg && eventsVec[0]->GetId() == trainId );
	//расформирование
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::DISFORM && eventsVec[1]->GetBadge() == TI.Get( L"K3" ).bdg && eventsVec[1]->GetId() == trainId &&
		eventsVec[1]->GetTime() == partLeaveTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::FormDisform()
{
	TI.DetermineStripBusy( L"C", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	SetRandomOddInfo( tlist.front() );
	changes = tracker->TakeTrainChanges(); //обнуление изменений
	DisformTrain( tlist.front()->GetId() );
	tracker->TakeChanges( changes );
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	const auto & tchange = changes.placeViewChanges.front();
	CPPUNIT_ASSERT( !tchange.appeared );
	CPPUNIT_ASSERT( tchange.place == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( !tchange.tdescrPtr );

	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 2 );
	auto teIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( (*teIt++)->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( (*teIt)->GetCode() == HCode::DISFORM );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::MultiForm()
{
	//имитируем генерацию ПЕ по опорным данным
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.FlushData( *tracker, true );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.front();
	auto tdescr = RandomTrainDescr();
	SetInfo( train, tdescr );

	const auto & trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 1 );
	CPPUNIT_ASSERT( (*trackerEvents.rbegin())->GetCode() == HCode::FORM );

	//формирование поезда еще раз
	SetInfo( train, tdescr );
	const auto & trackerEvents2 = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == trackerEvents2.size() ); //это уже не появление

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::DisformAndDeath()
{
	//въезд поезда на п/о путь
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	//назначение номера
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto adTrain = trains.front();
	auto adTrainId = adTrain->GetId();
	SetRandomOddInfo( adTrain );

	//выезд на неправильный светофор (формирование нового поезда)
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	auto newTrain = ( adTrain == trains.front() ? trains.back() : trains.front() );
	CPPUNIT_ASSERT( newTrain->IsTrain() );
	auto newTrainId = newTrain->GetId();

	//расформирование старого поезда и его исчезновение
	TI.DetermineStripFree( L"C", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1	);
	auto curTrain = trains.front();
	CPPUNIT_ASSERT( curTrain->GetId() == newTrainId );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::NoFormOnOnlyOpenSema()
{
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineHeadOpen( L"Head102", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1	);
	auto train = trains.back();
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( !train->IsTrain() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::FormOnOpenRoute()
{
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1	);
	auto train = trains.back();
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( train->IsTrain() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::FromADWayToSpanOnClosedTrainSemaAndGoToObscurity()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AJ4" ).bdg ) );

	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );

	//появление четного поезда
	TI.DetermineStripBusy( L"S4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomEvenInfo( train );

	//уход за пределы станции
	TI.DetermineStripBusy( L"F4", *tracker );
	TI.DetermineStripFree( L"S4", *tracker );
	wstring stripsArr[] = { L"F4", L"G4", L"H4", L"K4", L"I4", L"AJ4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	time_t deathTime = TI.DetermineStripFree( L"AJ4", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.empty() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 6 );

	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"S4" ).bdg && eventsVec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( eventsVec[4]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( eventsVec[5]->GetCode() == HCode::DEATH && eventsVec[5]->GetBadge() == TI.Get( L"AJ4" ).bdg && eventsVec[5]->GetId() == trainId &&
		eventsVec[5]->GetTime() == deathTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::FromADWayToSpanOnOpenedTrainSemaAndGoToObscurity()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AJ4" ).bdg ) );

	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineRouteSet( L"Head400->K4", *tracker );

	//появление четного поезда
	TI.DetermineStripBusy( L"S4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomEvenInfo( train );

	//уход за пределы станции
	TI.DetermineStripBusy( L"F4", *tracker );
	TI.DetermineStripFree( L"S4", *tracker );
	wstring stripsArr[] = { L"F4", L"G4", L"H4", L"K4", L"I4", L"AJ4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	time_t deathTime = TI.DetermineStripFree( L"AJ4", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.empty() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 6 );

	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"S4" ).bdg && eventsVec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::DEPARTURE );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( eventsVec[4]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( eventsVec[5]->GetCode() == HCode::DEATH && eventsVec[5]->GetBadge() == TI.Get( L"AJ4" ).bdg && eventsVec[5]->GetId() == trainId &&
		eventsVec[5]->GetTime() == deathTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::LeaveADWayWithSemaAndGoToObscurity()
{
	TI.DetermineSwitchMinus( L"41", *tracker );

	//появление нечетного поезда
	TI.DetermineStripBusy( L"R4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );

	//уход за пределы станции
	TI.DetermineStripBusy( L"O4", *tracker );

	time_t adOutTime = TI.DetermineStripFree( L"R4", *tracker );

	wstring stripsArr[] = { L"O4", L"A4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchMinus( L"42", *tracker );
	wstring stripsArr2[] = { L"A4", L"B4", L"P4" };
	vector <wstring> stripsSeq2( begin( stripsArr2 ), end( stripsArr2 ) );
	TI.ImitateMotion( stripsSeq2, *tracker );
	TI.DetermineStripFree( L"P4", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.empty() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 2 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"R4" ).bdg && eventsVec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::DISFORM && eventsVec[1]->GetBadge() == TI.Get( L"R4" ).bdg && 
		eventsVec[1]->GetId() == trainId && eventsVec[1]->GetTime() == adOutTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::LeaveADWayWOSemaAndGoToObscurity()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"T" ).bdg ) );

	//появление нечетного поезда
	TI.DetermineStripBusy( L"D", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );

	//уход за пределы станции
	TI.DetermineSwitchPlus( L"12", *tracker );
	TI.DetermineSwitchMinus( L"14", *tracker );
	TI.DetermineStripBusy( L"S", *tracker );

	time_t adOutTime = TI.DetermineStripFree( L"D", *tracker );
	wstring stripsArr[] = { L"S", L"T" };
	vector<wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	TI.DetermineStripFree( L"T", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.empty() );
	
	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 2 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"D" ).bdg && eventsVec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::DISFORM && eventsVec[1]->GetBadge() == TI.Get( L"D" ).bdg && eventsVec[1]->GetId() == trainId 
		&& eventsVec[1]->GetTime() == adOutTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::StatEntryWOADWayDisformOnSema()
{
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );

	//появление нечетного поезда
	TI.DetermineStripBusy( L"Q4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );

	//въезд на станцию
	time_t entryTime = TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripFree( L"Q4", *tracker );
	wstring stripsArr[] = { L"A4", L"B4", L"C4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	auto tsttevents = tracker->GetTrainEvents();
	TI.DetermineStripBusy( L"D4", *tracker ); //расформирование
	time_t disformTime = TI.DetermineStripFree( L"C4", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.size() == 1 );
	auto train2 = trains.front();
	CPPUNIT_ASSERT( !train2->IsTrain() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	//формирование
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"Q4" ).bdg && eventsVec[0]->GetId() == trainId );
	//въезд на станцию
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::STATION_ENTRY && eventsVec[1]->GetBadge() == TI.Get( L"A4" ).bdg && eventsVec[1]->GetId() == trainId &&
		eventsVec[1]->GetTime() == entryTime );
	//расформирование
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::DISFORM && eventsVec[2]->GetBadge() == TI.Get( L"C4" ).bdg && eventsVec[2]->GetId() == trainId &&
		eventsVec[2]->GetTime() == disformTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::StatEntryWOADWayGoToObscurity()
{
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"46", *tracker );

	//появление четного поезда
	TI.DetermineStripBusy( L"Q4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomEvenInfo( train );

	//въезд на станцию
	time_t entryTime = TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripFree( L"Q4", *tracker );
	wstring stripsArr[] = { L"A4", L"B4", L"C4", L"D4", L"E4", L"G4", L"H4", L"L4", L"N4", L"AI4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	time_t disappTime = TI.DetermineStripFree( L"AI4", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.empty() );
	
	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 3 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"Q4" ).bdg && eventsVec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::STATION_ENTRY && eventsVec[1]->GetBadge() == TI.Get( L"A4" ).bdg && 
		eventsVec[1]->GetId() == trainId && eventsVec[1]->GetTime() == entryTime );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::DISFORM && eventsVec[2]->GetBadge() == TI.Get( L"AI4" ).bdg && eventsVec[2]->GetId() == trainId 
		&& eventsVec[2]->GetTime() == disappTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::StatEntryWOADWayGoToSpanObscurity()
{
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );

	//появление четного поезда
	TI.DetermineStripBusy( L"Q4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomEvenInfo( train );

	//въезд на станцию
	time_t entryTime = TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripFree( L"Q4", *tracker );
	wstring stripsArr[] = { L"A4", L"B4", L"C4", L"D4", L"E4", L"G4", L"H4", L"K4", L"I4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	time_t disappTime = TI.DetermineStripFree( L"I4", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.empty() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 6 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"Q4" ).bdg && eventsVec[0]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::STATION_ENTRY && eventsVec[1]->GetBadge() == TI.Get( L"A4" ).bdg && eventsVec[1]->GetId() == trainId && 
		eventsVec[1]->GetTime() == entryTime );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::TRANSITION && eventsVec[2]->GetBadge() == TI.Get( L"K4" ).bdg && eventsVec[2]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::STATION_EXIT && eventsVec[3]->GetBadge() == TI.Get( L"K4" ).bdg && eventsVec[3]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[4]->GetCode() == HCode::SPAN_MOVE && eventsVec[4]->GetBadge() == TI.Get( L"I4" ).bdg && eventsVec[4]->GetId() == trainId );
	CPPUNIT_ASSERT( eventsVec[5]->GetCode() == HCode::DEATH && eventsVec[5]->GetBadge() == TI.Get( L"I4" ).bdg && eventsVec[5]->GetId() == trainId 
		&& eventsVec[5]->GetTime() == disappTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::StatEntryWOADWayGoToNoTSStrip()
{
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchMinus( L"42", *tracker );

	//появление четного поезда
	TI.DetermineStripBusy( L"Q4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomEvenInfo( train );

	//въезд на станцию
	time_t entryTime = TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripFree( L"Q4", *tracker );
	wstring stripsArr[] = { L"A4", L"B4", L"P4" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	time_t disappTime = TI.DetermineStripFree( L"P4", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.empty() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 3 );
	//формирование
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"Q4" ).bdg && eventsVec[0]->GetId() == trainId );
	//въезд на станцию
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::STATION_ENTRY && eventsVec[1]->GetBadge() == TI.Get( L"A4" ).bdg && eventsVec[1]->GetId() == trainId &&
		eventsVec[1]->GetTime() == entryTime );
	//расформирование
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::DISFORM && eventsVec[2]->GetBadge() == TI.Get( L"P4" ).bdg && eventsVec[2]->GetId() == trainId && 
		eventsVec[2]->GetTime() == disappTime );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::StatEntryAndDeath()
{	
	//появление четного поезда
	TI.DetermineStripBusy( L"Q4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomEvenInfo( train );

	//въезд на станцию
	time_t entryTime = TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripFree( L"Q4", *tracker );
	TI.DetermineStripFree( L"A4", *tracker );
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.empty() );

	//проверка событий
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	//формирование
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetBadge() == TI.Get( L"Q4" ).bdg && eventsVec[0]->GetId() == trainId );
	//въезд на станцию
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::STATION_ENTRY && eventsVec[1]->GetBadge() == TI.Get( L"A4" ).bdg && eventsVec[1]->GetId() == trainId &&
		eventsVec[1]->GetTime() == entryTime );
	//расформирование
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::DEATH && eventsVec[2]->GetBadge() == TI.Get( L"A4" ).bdg && eventsVec[2]->GetId() == trainId );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::MoveADWayToADWayAndSemaDisform()
{
	//появление четного поезда
	auto formTime1 = TI.DetermineStripBusy( L"O3", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto even_train = trains.front();
	auto trainId = even_train->GetId();
	SetRandomEvenInfo( even_train );

	//въезд на 1 п/о путь:
	TI.DetermineHeadClose( L"Head300", *tracker );
	TI.DetermineStripBusy( L"P3", *tracker );
	auto arrivTime1 = TI.DetermineStripFree( L"O3", *tracker );

	//выезд с 1 п/о пути и въезд на 2 п/о путь:
	TI.DetermineHeadClose( L"Head302", *tracker );
	TI.DetermineStripBusy( L"R3", *tracker ); //выезд на закрытый светофор
	auto departTime1 = TI.DetermineStripFree( L"P3", *tracker );
	TI.DetermineStripBusy( L"S3", *tracker );
	auto arrivTime2 = TI.DetermineStripFree( L"R3", *tracker ); //прибытие

	//расформирование поезда и корректное отправление в обратную сторону:
	TI.DetermineRouteSet( L"Head303->P3", *tracker );
	auto formTime2 = TI.DetermineStripBusy( L"R3", *tracker ); //формирование нового нечетного поезда
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	auto trIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"R3" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( trIt != trains.cend() );
	auto odd_train = *trIt;
	auto trainId2 = odd_train->GetId();

	TI.DetermineStripFree( L"S3", *tracker ); //расформирование старого четного поезда

	//проверка событий
	auto tevents1 = tracker->GetTrainEvents( trainId );
	CPPUNIT_ASSERT( tevents1.size() == 5 );
	vector<TrackerEventPtr> eventsVec1( tevents1.cbegin(), tevents1.cend() );
	//формирование поезда 1
	CPPUNIT_ASSERT( eventsVec1[0]->GetCode() == HCode::FORM && eventsVec1[0]->GetBadge() == TI.Get( L"O3" ).bdg  && eventsVec1[0]->GetTime() == formTime1 );
	//прибытие на п/о путь 1
	CPPUNIT_ASSERT( eventsVec1[1]->GetCode() == HCode::ARRIVAL && eventsVec1[1]->GetBadge() == TI.Get( L"P3" ).bdg && eventsVec1[1]->GetTime() == arrivTime1 );
	//отправление с п/о пути 1
	CPPUNIT_ASSERT( eventsVec1[2]->GetCode() == HCode::DEPARTURE && eventsVec1[2]->GetBadge() == TI.Get( L"P3" ).bdg && eventsVec1[2]->GetTime() == departTime1 );
	//прибытие на п/о путь 2
	CPPUNIT_ASSERT( eventsVec1[3]->GetCode() == HCode::ARRIVAL && eventsVec1[3]->GetBadge() == TI.Get( L"S3" ).bdg && eventsVec1[3]->GetTime() == arrivTime2 );
	//расформирование поезда на п/о пути 2
	CPPUNIT_ASSERT( eventsVec1[4]->GetCode() == HCode::DISFORM && eventsVec1[4]->GetBadge() == TI.Get( L"S3" ).bdg && eventsVec1[4]->GetTime() == arrivTime2 );

	auto tevents2 = tracker->GetTrainEvents( trainId2 );
	CPPUNIT_ASSERT( tevents2.size() == 2 );
	vector<TrackerEventPtr> eventsVec2( tevents2.cbegin(), tevents2.cend() );
	//формирование поезда 2
	CPPUNIT_ASSERT( eventsVec2[0]->GetCode() == HCode::FORM && eventsVec2[0]->GetBadge() == TI.Get( L"S3" ).bdg && eventsVec2[0]->GetTime() == formTime2 );
	//отправление с п/о пути 2
	CPPUNIT_ASSERT( eventsVec2[1]->GetCode() == HCode::DEPARTURE && eventsVec2[1]->GetBadge() == TI.Get( L"S3" ).bdg && eventsVec2[1]->GetTime() == formTime2 );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::NoDisformOnSpan()
{
	TI.DetermineStripBusy( L"AH4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto oddTrain = trains.front();
	auto oddTrainId = oddTrain->GetId();
	SetRandomOddInfo( oddTrain );
	auto tdescrPtr = oddTrain->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && !tdescrPtr->GetNumber().empty() );
	auto oddTrainNum = tdescrPtr->GetNumber().getNumberString();
	TI.DetermineStripBusy( L"AG4", *tracker );
	TI.DetermineStripFree( L"AH4", *tracker );
	TI.DetermineStripBusy( L"AA4", *tracker );
	TI.DetermineStripFree( L"AG4", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtrain = trains.front();
	CPPUNIT_ASSERT( oddTrainId == curtrain->GetId() );
	auto curdescrPtr = curtrain->GetDescrPtr();
	CPPUNIT_ASSERT( curdescrPtr && !curdescrPtr->GetNumber().empty() );
	auto curTrainNum = curdescrPtr->GetNumber().getNumberString();
	CPPUNIT_ASSERT( curTrainNum == oddTrainNum );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::StationMigrationFromADWay()
{
	//миграция на другую станцию с ПО-пути
	TI.DetermineStripBusy( L"ST5", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );
	TI.DetermineStripBusy( L"ST4", *tracker );
	TI.DetermineStripFree( L"ST5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.front();
	CPPUNIT_ASSERT( trainId == tunity->GetId() );
	CPPUNIT_ASSERT( tunity->IsTrain() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::PassingTwoAlienSemasWithNumForwarding()
{
	TI.DetermineStripBusy( L"W3", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	const auto & evenTdescr = RandomTrainDescr( false );
	SetInfo( train, evenTdescr );
	TI.DetermineStripBusy( L"V3", *tracker );
	tunities = trainCont->GetUnities();
	auto v3UnityIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"V3" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( v3UnityIt != tunities.cend() );
	auto v3Unity = *v3UnityIt;
	SetInfo( v3Unity, evenTdescr );
	tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"W3", *tracker );
	tunities = trainCont->GetUnities();
	TI.DetermineStripBusy( L"Q3", *tracker, false );
	TI.DetermineStripBusy( L"U3", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::RecreateOnStationFromSpanEntryThroughAlienSema()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8241" ) ), list<wstring>( 1, L"W" ) );
	list<TrainUnityCPtr> tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto oddTrain = tunities.back();
	auto oddTrainIden = oddTrain->GetId();

	TI.DetermineStripBusy( L"Q", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.back()->GetId() == oddTrainIden );
	TI.DetermineStripBusy( L"A", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto evenTrain = tunities.back();
	auto evenTrainIden = evenTrain->GetId();
	CPPUNIT_ASSERT( evenTrainIden != oddTrainIden );
	auto evenTrainDescrPtr = evenTrain->GetDescrPtr();
	CPPUNIT_ASSERT( evenTrainDescrPtr && evenTrainDescrPtr->GetNumber().isEven() );
	const auto & evenTrainPlace = evenTrain->GetPlace();
	CPPUNIT_ASSERT( evenTrainPlace.size() == 3 );
	CPPUNIT_ASSERT( find( evenTrainPlace.cbegin(), evenTrainPlace.cend(), TI.Get( L"W" ).bdg ) != evenTrainPlace.cend() );
	CPPUNIT_ASSERT( find( evenTrainPlace.cbegin(), evenTrainPlace.cend(), TI.Get( L"Q" ).bdg ) != evenTrainPlace.cend() );
	CPPUNIT_ASSERT( find( evenTrainPlace.cbegin(), evenTrainPlace.cend(), TI.Get( L"A" ).bdg ) != evenTrainPlace.cend() );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 4 );
	auto backEventPtr = tevents.back();
	CPPUNIT_ASSERT( backEventPtr->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( backEventPtr->GetPrevId() == oddTrainIden );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::RecreateOnStationFromSpanEntryThroughAlienSema2()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8241" ) ), list<wstring>( 1, L"W" ) );
	list<TrainUnityCPtr> tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto prevIden = tunities.back()->GetId();

	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.back()->GetOddness() == Oddness::EVEN );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	auto backEventPtr = tevents.back();
	CPPUNIT_ASSERT( backEventPtr->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( backEventPtr->GetPrevId() == prevIden );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::RecreateOnFullSpanLeaveThroughAlienSema()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	list <TrainUnityCPtr> tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto oddTrain = tunities.back();
	SetRandomOddInfo( oddTrain );
	auto oddTrainId = oddTrain->GetId();
	TI.DetermineStripFree( L"W", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	CPPUNIT_ASSERT( train->GetId() == oddTrainId );
	TI.DetermineStripFree( L"Q", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	train = tunities.back();
	CPPUNIT_ASSERT( train->GetId() != oddTrainId ); //новый поезд
	auto tdescrPtr = train->GetDescrPtr(); 
	CPPUNIT_ASSERT( tdescrPtr && tdescrPtr->GetNumber().isEven() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::MultiTrainsThroughAlienSema()
{
	TI.DetermineSwitchPlus( L"11", *tracker );

	//поезд 1
	TI.DetermineStripBusy( L"K", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );

	//поезд 2
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );

	//поезд 3
	TI.DetermineStripBusy( L"A", *tracker );
	SetRandomOddInfo( TI.Get( L"A" ).bdg );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	//ПЕ
	TI.DetermineStripBusy( L"D", *tracker );

	unsigned int khTrainId = 0, wTrainId = 0, qTrainId = 0, dUnityId = 0;
	set<unsigned int> allIdens;
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 4 );
	for_each( tunities.cbegin(), tunities.cend(), [&khTrainId, &wTrainId, &dUnityId, &qTrainId, &allIdens, this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		auto tunityIden = tptr->GetId();
		CPPUNIT_ASSERT( tunityIden != 0 );
		allIdens.insert( tunityIden );
		if ( tplace.size() == 2 && 
			any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"K" ).bdg; } ) &&
			any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"H" ).bdg; } ) )
			khTrainId = tptr->GetId();
		else if ( tplace.size() == 1 )
		{
			if ( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"W" ).bdg; } ) )
				wTrainId = tptr->GetId();
			else if ( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"Q" ).bdg; } ) )
				qTrainId = tptr->GetId();
			else if ( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"D" ).bdg; } ) )
				dUnityId = tptr->GetId();
		}
	} );
	CPPUNIT_ASSERT( allIdens.size() == 4 );

	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 3 );
	for ( auto tunity : tunities )
	{
		auto tunityId = tunity->GetId();
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 )
		{
			if ( tplace.front() != TI.Get( L"H" ).bdg )
			{
				CPPUNIT_ASSERT( tplace.front() == TI.Get( L"K" ).bdg );
				CPPUNIT_ASSERT( tunityId == khTrainId );
			}
			else
			{
				CPPUNIT_ASSERT( tplace.front() == TI.Get( L"H" ).bdg );
				CPPUNIT_ASSERT( tunityId == wTrainId );
			}
		}
		else
		{
			CPPUNIT_ASSERT( tplace.size() == 3 );
			CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return TI.Get( L"Q" ).bdg == bdg; } ) );
			CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return TI.Get( L"A" ).bdg == bdg; } ) );
			CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return TI.Get( L"D" ).bdg == bdg; } ) );
			CPPUNIT_ASSERT( tunityId != wTrainId && tunityId != qTrainId && tunityId != dUnityId );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::MultiTrainsThroughAlienSema2()
{
	TI.DetermineStripBusy( L"K", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"W", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for ( auto tunity : tunities )
		SetRandomOddInfo( tunity );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.FlushData( *tracker, false );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::CompeteForPlaceWhileRecreating()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"Q" ).bdg )
			SetRandomOddInfo( tunity );
		else
			SetRandomEvenInfo( tunity );
	}
	
	TI.DetermineStripBusy( L"C", *tracker ); //маневровая
	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.FlushData( *tracker, false );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::LosingOddnesOnDisform()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.back()->IsTrain() && tunities.back()->GetOddness() == Oddness::EVEN );
	DisformTrain( tunities.back()->GetId() );
	CPPUNIT_ASSERT( !tunities.back()->IsTrain() && tunities.back()->GetOddness() == Oddness::UNDEF );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::IgnoringUndefSemaFromSpanToStat()
{
	for ( auto i = 0; i < 2; ++i )
	{
		TI.DetermineStripBusy( L"Q4", *tracker );
		auto q4trainPtr = ( i == 0 ? SetRandomEvenInfo( TI.Get( L"Q4" ).bdg ) : SetRandomOddInfo( TI.Get( L"Q4" ).bdg ) );
		auto q4TrainId = q4trainPtr->GetId();
		auto tdescrPtr = q4trainPtr->GetDescrPtr();
		CPPUNIT_ASSERT( tdescrPtr );
		const auto & initDescr = *tdescrPtr;
		auto initOddness = q4trainPtr->GetOddness();
		TI.DetermineStripBusy( L"A4", *tracker );

		auto tunities = trainCont->GetUnities();
		CPPUNIT_ASSERT( tunities.size() == 1 );
		auto curtrain = tunities.back();
		CPPUNIT_ASSERT( curtrain->Have( TI.Get( L"Q4" ).bdg ) );
		CPPUNIT_ASSERT( curtrain->Have( TI.Get( L"A4" ).bdg ) );
		CPPUNIT_ASSERT( curtrain->GetOddness() == initOddness );
		auto curdescrPtr = curtrain->GetDescrPtr();
		CPPUNIT_ASSERT( curdescrPtr );
		CPPUNIT_ASSERT( *curdescrPtr == initDescr );
		CPPUNIT_ASSERT( !TI.TakeExceptions() );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
	}
}

void TC_TrackerFormDisform::LongtimeDisformLosing()
{
	const auto & timconstr = tracker->GetTimeConstraints();
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	auto bornMoment = TI.GetTimeMoment();
	SetRandomOddInfo( TI.Get( L"R4" ).bdg );

	auto tevents = tracker->GetTrainEvents();
	TI.DetermineStripBusy( L"O4", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker );
	auto disformMoment = TI.DetermineStripFree( L"R4", *tracker );
	TI.DetermineStripFree( L"O4", *tracker );
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineStripBusy( L"B4", *tracker );
	TI.DetermineStripBusy( L"C4", *tracker );
	TI.DetermineStripBusy( L"D4", *tracker );

	TI.DetermineStripFree( L"A4", *tracker );
	TI.DetermineStripFree( L"B4", *tracker );
	tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 1 );
	auto formEvent = tevents.back();
	CPPUNIT_ASSERT( formEvent->GetCode() == HCode::FORM && formEvent->GetTime() == bornMoment );

	//создается событие расформирования по моменту выезда с ПО-пути,
	//но оно не должно быть потеряно, т.к. при подчистке опираться следует не на время самого события, а на время создания события 
	TI.DetermineStripFree( L"C4", *tracker, true, timconstr.bufferLimitMs / 1000 + 5 );

	tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 1 );
	auto disformEvent = tevents.back();
	CPPUNIT_ASSERT( disformEvent->GetCode() == HCode::DISFORM && disformEvent->GetTime() == disformMoment );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::RebornInsteadOutThenDeath()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"5389" ) ), list<wstring>( 1, L"F" ) );
	TI.DetermineStripBusy( L"C", *tracker );
	auto arrTime = TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineRouteSet( L"Head101->A", *tracker );
	auto deathTime1 = TI.DetermineStripFree( L"C", *tracker ); //death
	auto rebornTime = TI.DetermineStripBusy( L"B", *tracker ); //form
	auto deathTime2 = TI.DetermineStripFree( L"B", *tracker ); //death

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );

	CPPUNIT_ASSERT( evvec.size() == 5 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::ARRIVAL && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetTime() == arrTime );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DEATH && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg && evvec[2]->GetTime() == deathTime1 );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::FORM && evvec[3]->GetBadge() == TI.Get( L"B" ).bdg && evvec[3]->GetTime() == rebornTime );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::DEATH && evvec[4]->GetBadge() == TI.Get( L"B" ).bdg && evvec[4]->GetTime() == deathTime2 );

	CPPUNIT_ASSERT( CheckTimeSequence( tevents ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::ADFormNearObscurity()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AI" ).bdg ) );
	TI.DetermineRouteSet( L"Head107->AM", *tracker );
	TI.DetermineStripBusy( L"AI", *tracker, false );
	TI.DetermineStripBusy( L"AK", *tracker, false );
	auto bornMom = TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunityPtr = tunities.back();
	CPPUNIT_ASSERT( tunityPtr->IsTrain() && tunityPtr->Head() == TI.Get( L"AK" ).bdg );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 2 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"AI" ).bdg && evvec[0]->GetTime() == bornMom );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"AI" ).bdg && evvec[1]->GetTime() == bornMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::ADFormNearObscurity2()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AI" ).bdg ) );
	TI.DetermineRouteSet( L"Head107->AM", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2006" ) ), list<wstring>( 1, L"AI" ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto initTrainPtr = tunities.back();
	auto initTrainId = initTrainPtr->GetId();
	TI.DetermineStripBusy( L"AK", *tracker, false );
	TI.DetermineStripBusy( L"AL", *tracker, false );
	TI.DetermineStripBusy( L"AM", *tracker, false );
	auto departMom = TI.FlushData( *tracker, false );

	//формирование нового поезда
	TrainUnityCPtr adTrainPtr, outTrainPtr;
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for ( auto train : tunities )
	{
		const auto & tplace = train->GetPlace();
		if ( train->Have( TI.Get( L"AI" ).bdg ) && tplace.size() == 1 )
			adTrainPtr = train;
		else if ( train->Have( TI.Get( L"AK" ).bdg ) && train->Have( TI.Get( L"AL" ).bdg ) && train->Have( TI.Get( L"AM" ).bdg ) && tplace.size() == 3 )
			outTrainPtr = train;
	}
	CPPUNIT_ASSERT( adTrainPtr && outTrainPtr && adTrainPtr->IsTrain() && outTrainPtr->IsTrain() && adTrainPtr->GetId() == initTrainId );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 3 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"AI" ).bdg && evvec[0]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::FORM && evvec[1]->GetBadge() == TI.Get( L"AI" ).bdg && evvec[1]->GetId() == outTrainPtr->GetId() &&
		evvec[1]->GetTime() == departMom );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DEPARTURE && evvec[2]->GetBadge() == TI.Get( L"AI" ).bdg && evvec[2]->GetId() == outTrainPtr->GetId() &&
		evvec[2]->GetTime() == departMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::ADNoFormNearObscurity()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AI" ).bdg ) );
	TI.DetermineRouteUnset( L"Head107->AM", *tracker );
	TI.DetermineStripBusy( L"AI", *tracker, false );
	auto bornMom = TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunityPtr = tunities.back();
	CPPUNIT_ASSERT( !tunityPtr->IsTrain() );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.empty() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::ADNoFormNearObscurity2()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AI" ).bdg ) );
	TI.DetermineRouteSet( L"Head107->AM", *tracker );
	TI.DetermineStripBusy( L"AI", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunityPtr = tunities.back();
	CPPUNIT_ASSERT( !tunityPtr->IsTrain() );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.empty() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::EventMomentOnDispositionDisform()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"B" ) );
	auto trains = trainCont->GetUnities();
	auto trainPtr = trains.back();
	auto initId = trainPtr->GetId();

	TI.DetermineStripBusy( L"C", *tracker );
	auto arrivalMom = TI.DetermineStripFree( L"B", *tracker );
	auto disformMom = TI.IncreaseTime( 5000 );
	DisformTrain( initId, TI.Get( L"C" ).bdg, TrainDescr( L"2516" ), TrainCharacteristics::Source::Disposition );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 3 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"B" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::ARRIVAL && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg && evvec[1]->GetTime() == arrivalMom );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DISFORM && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg && evvec[2]->GetTime() == disformMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::ClosedWayOutAndGoToObscurity()
{	
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AI4" ).bdg ) );

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6236" ) ), list<wstring>( 1, L"S4" ) );

	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"46", *tracker );

	TI.DetermineStripBusy( L"F4", *tracker );
	auto outWayMom = TI.DetermineStripFree( L"S4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineStripFree( L"F4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker );
	TI.DetermineStripFree( L"G4", *tracker );
	TI.DetermineStripBusy( L"L4", *tracker );
	TI.DetermineStripFree( L"H4", *tracker );
	TI.DetermineStripBusy( L"N4", *tracker );
	TI.DetermineStripFree( L"L4", *tracker );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 1 );
	CPPUNIT_ASSERT( tevents.back()->GetCode() == HCode::FORM );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.back()->GetId();
	CPPUNIT_ASSERT( tunities.back()->IsTrain() );
	TI.DetermineStripBusy( L"AI4", *tracker );
	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	CPPUNIT_ASSERT( !tunities2.back()->IsTrain() && tunities2.back()->GetId() != trainId );

	auto tevents2 = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents2.size() == 2 );
	vector<TrackerEventPtr> evvec( tevents2.cbegin(), tevents2.cend() );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"S4" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DISFORM && evvec[1]->GetBadge() == TI.Get( L"S4" ).bdg && evvec[1]->GetTime() == outWayMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::OpenedWayOutAndGoToObscurity()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AI4" ).bdg ) );

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6236" ) ), list<wstring>( 1, L"S4" ) );
	auto trains = trainCont->GetUnities();
	auto trainPtr = trains.back();
	auto initId = trainPtr->GetId();

	TI.DetermineRouteSet( L"Head400->K4", *tracker );
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"46", *tracker );

	auto departureMom = TI.DetermineStripBusy( L"F4", *tracker );
	TI.DetermineStripFree( L"S4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineStripFree( L"F4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker );
	TI.DetermineStripFree( L"G4", *tracker );
	TI.DetermineStripBusy( L"L4", *tracker );
	TI.DetermineStripFree( L"H4", *tracker );
	TI.DetermineStripBusy( L"N4", *tracker );
	TI.DetermineStripFree( L"L4", *tracker );
	auto disformMom = TI.DetermineStripBusy( L"AI4", *tracker );
	
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.size() == 1 && trains2.back()->GetId() != initId && !trains2.back()->IsTrain() );

	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"S4" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"S4" ).bdg && evvec[1]->GetTime() == departureMom );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DISFORM && evvec[2]->GetBadge() == TI.Get( L"AI4" ).bdg && evvec[2]->GetTime() == disformMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::HistoryLimitationObserve()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"B3" ).bdg ) );

	TI.DetermineSwitchMinus( L"31", *tracker );
	TI.DetermineSwitchPlus( L"32", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6235" ) ), list<wstring>( 1, L"N3" ) );
	TI.DetermineStripBusy( L"M3", *tracker );
	TI.DetermineStripBusy( L"L3", *tracker );
	TI.DetermineStripBusy( L"K3", *tracker );
	TI.DetermineStripFree( L"N3", *tracker );
	TI.DetermineStripFree( L"M3", *tracker );
	TI.DetermineStripFree( L"L3", *tracker ); //arrival на K3

	TI.DetermineStripBusy( L"J3", *tracker );
	TI.DetermineStripBusy( L"I3", *tracker );
	TI.DetermineStripFree( L"K3", *tracker );
	TI.DetermineStripFree( L"J3", *tracker ); //arrival на I3

	TI.DetermineStripBusy( L"H3", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripFree( L"I3", *tracker );
	TI.DetermineStripFree( L"H3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"D3", *tracker );
	TI.DetermineStripBusy( L"C3", *tracker );
	TI.DetermineStripBusy( L"B3", *tracker );

	TI.DetermineStripFree( L"Y3", *tracker );
	TI.DetermineStripFree( L"X3", *tracker );
	TI.DetermineStripFree( L"F3", *tracker );
	TI.DetermineStripFree( L"D3", *tracker );
	TI.DetermineStripFree( L"C3", *tracker );

	//расформирование
	auto disformMoment = TI.DetermineStripFree( L"B3", *tracker );

	auto tevents = tracker->GetTrainEvents();

	CPPUNIT_ASSERT( !tevents.empty() );
	auto disformEventPtr = tevents.back();
	CPPUNIT_ASSERT( disformEventPtr->GetCode() == HCode::DISFORM && disformEventPtr->GetPureTime() == disformMoment );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerFormDisform::ExtFormNSemaDisform()
{
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	auto enterMom = TI.DetermineStripFree( L"B", *tracker );
	auto formMom = TI.IncreaseTime( 2000 );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );
	TI.DetermineStripBusy( L"F", *tracker );
	auto disformMom = TI.DetermineStripFree( L"C", *tracker );
	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> tevvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( tevvec.size() == 2 );
	CPPUNIT_ASSERT( tevvec[0]->GetCode() == HCode::FORM && tevvec[0]->GetTime() == formMom );
	CPPUNIT_ASSERT( tevvec[1]->GetCode() == HCode::DISFORM && tevvec[1]->GetTime() == disformMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}