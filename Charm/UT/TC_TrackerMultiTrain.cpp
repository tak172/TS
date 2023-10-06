#include "stdafx.h"
#include "TC_TrackerMultiTrain.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainUnity.h"
#include "TC_TrackerTrainImpl.h"
#include "../Guess/TrainInfoPackage.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerMultiTrain );
void TC_TrackerMultiTrain::setUp()
{
	TI.Reset();
}

void TC_TrackerMultiTrain::AppearanceOnSwitch()
{
	//имитуруем возникновение двух ПЕ на двух участках, прилегающих к стрелке
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineStripBusy( L"F" , *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr trainPtr1 = *trainList.begin();
	TrainUnityCPtr trainPtr2 = *( ++trainList.begin() );
	SetRandomOddInfo( trainPtr1 );
	SetRandomOddInfo( trainPtr2 );
	const list <BadgeE> & trainPlace1 = trainPtr1->GetPlace();
	const list <BadgeE> & trainPlace2 = trainPtr2->GetPlace();
	CPPUNIT_ASSERT( trainPlace1.size() == 1 && trainPlace2.size() == 1 );
	CPPUNIT_ASSERT( trainPlace1.front() == TI.Get(L"F").bdg && trainPlace2.front() == TI.Get(L"G").bdg );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	CPPUNIT_ASSERT( changes.placeViewChanges.begin()->place == TI.Get(L"F").bdg && changes.placeViewChanges.rbegin()->place == TI.Get(L"G").bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::NamedMovingToNamed()
{
	//пусть модуль отслеживания ведет две ПЕ (на участках "B" и "F")
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( !trainList.empty() );
	TrainUnityCPtr left_train = trainList.front();
	TrainUnityCPtr right_train = trainList.back();
	list <BadgeE> ltrainPlace = left_train->GetPlace();
	list <BadgeE> rtrainPlace = right_train->GetPlace();

	//именуем ПЕ на участке "B" и на участке "F"
	TrainDescr bdescr = RandomTrainDescr( false );
	SetInfo( left_train, bdescr );

	TrainDescr fdescr = RandomTrainDescr( true );
	SetInfo( right_train, fdescr );

	changes = tracker->TakeTrainChanges();

	//имитируем изменение положения ПЕ (получение xml-сообщение от Fund'a): зажигание C
	TI.DetermineStripBusy( L"C", *tracker );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	ltrainPlace = left_train->GetPlace();
	rtrainPlace = right_train->GetPlace();
	CPPUNIT_ASSERT( ltrainPlace.size() + rtrainPlace.size() == 3 );
	bool in_left = ( find( ltrainPlace.begin(), ltrainPlace.end(), TI.Get(L"C").bdg ) != ltrainPlace.end() );
	bool in_right = ( find( rtrainPlace.begin(), rtrainPlace.end(), TI.Get(L"C").bdg ) != rtrainPlace.end() );
	CPPUNIT_ASSERT( in_left && !in_right || !in_left && in_right );
	changes = tracker->TakeTrainChanges();

	//проверяем, что Tracker считает измененным
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	CPPUNIT_ASSERT( changes.placeViewChanges.begin()->place == TI.Get(L"C").bdg );
	CPPUNIT_ASSERT( changes.placeViewChanges.begin()->appeared );
	ConstTrainDescrPtr tdescr = changes.placeViewChanges.begin()->tdescrPtr;
	CPPUNIT_ASSERT( tdescr && ( *tdescr == bdescr || *tdescr == fdescr ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::SwitchMergingOfTrainAndNoTrain()
{
	//помещаем около стрелки одну ПЕ (C-F)
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	//именуем
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	TrainUnityCPtr init_train = trains.front();
	SetRandomEvenInfo( init_train );
	auto trainId = init_train->GetId();
	auto tdescrPtr = init_train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );

	//выставляем стрелку в "минус" и помещаем около стрелки вторую ПЕ (участок G)
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );

	//перевод стрелки
	TI.DetermineSwitchPlus( L"13", *tracker );
	trains = trainCont->GetUnities();

	auto cfUnityPtr = trainCont->GetUnity( trainId );
	CPPUNIT_ASSERT( cfUnityPtr );
	auto curdescrPtr = cfUnityPtr->GetDescrPtr();
	CPPUNIT_ASSERT( cfUnityPtr->Have( TI.Get( L"C" ).bdg ) );
	CPPUNIT_ASSERT( cfUnityPtr->Have( TI.Get( L"F" ).bdg ) );
	CPPUNIT_ASSERT( cfUnityPtr->IsTrain() && cfUnityPtr->GetId() == trainId && curdescrPtr && *curdescrPtr == *tdescrPtr );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::SwitchNotMergingOfTrains()
{
	TI.DetermineSwitchMinus( L"13", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"1" ) ), list<wstring>( 1, L"F" ) );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2" ) ), list<wstring>( 1, L"G" ) );

	//очистка накопленных изменений
	changes = tracker->TakeTrainChanges();

	//перевод стрелки в "плюс"
	TI.DetermineSwitchPlus( L"13", *tracker );
	changes = tracker->TakeTrainChanges();

	//ПЕ объединяться не должны
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto ftrainPtr = trainCont->GetUnity( TI.Get( L"F" ).bdg );
	auto gtrainPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );

	const auto & ftrainPlace = ftrainPtr->GetPlace();
	const auto & gtrainPlace = gtrainPtr->GetPlace();
	CPPUNIT_ASSERT( ftrainPlace.size() == 1 && ftrainPtr->Have( TI.Get( L"F" ).bdg ) );
	auto fdescrPtr = ftrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( fdescrPtr && *fdescrPtr == TrainDescr( L"1" ) );
	CPPUNIT_ASSERT( gtrainPlace.size() == 1 && gtrainPtr->Have( TI.Get( L"G" ).bdg ) );
	auto gdescrPtr = gtrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( gdescrPtr && *gdescrPtr == TrainDescr( L"2" ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
};

void TC_TrackerMultiTrain::BusinessAtomicOverlay()
{
	list<wstring> bcPlace;
	bcPlace.push_back( L"B" );
	bcPlace.push_back( L"C" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"460" ) ), bcPlace );
	auto bcUnityPtr = trainCont->GetUnity( TI.Get( L"B" ).bdg );
	auto trainId1 = bcUnityPtr->GetId();

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"331" ) ), list<wstring>( 1, L"F" ) );
	auto fUnityPtr = trainCont->GetUnity( TI.Get( L"F" ).bdg );
	auto trainId2 = fUnityPtr->GetId();

	changes = tracker->TakeTrainChanges();

	//имитируем атомарное наложение лосов занятости
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.FlushData( *tracker, false );

	auto newBCUnity = trainCont->GetUnity( TI.Get( L"B" ).bdg );
	CPPUNIT_ASSERT( newBCUnity && newBCUnity->GetId() == trainId1 );
	CPPUNIT_ASSERT( newBCUnity->GetPlace().size() == 2 && newBCUnity->Have( TI.Get( L"B" ).bdg ) && newBCUnity->Have( TI.Get( L"C" ).bdg ) );
	auto newFUnity = trainCont->GetUnity( TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( newFUnity && newFUnity->GetId() == trainId2 );
	CPPUNIT_ASSERT( newFUnity->GetPlace().size() == 1 && newFUnity->Have( TI.Get( L"F" ).bdg ) );

	//проверяем, что Tracker считает измененным
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::GoingToObscurity()
{
	TI.DetermineStripBusy( L"AA3", *tracker );
	TI.DetermineStripBusy( L"W3", *tracker );
	TI.DetermineStripBusy( L"Z3", *tracker, false );
	TI.DetermineStripFree( L"W3", *tracker, false );
	TI.FlushData( *tracker, false );

	//выясняем идентификаторы левой и правой ПЕ
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr trainPtr1 = trainList.front();
	TrainUnityCPtr trainPtr2 = trainList.back();
	const list <BadgeE> & place1 = trainPtr1->GetPlace();
	TrainUnityCPtr preboundTrain = ( place1.front() == TI.Get(L"Z3").bdg ? trainPtr1 : trainPtr2 );

	//ПЕ2 смещается на место ПЕ1, ПЕ1 исчезает за пределами видимости станции
	TI.DetermineStripFree( L"Z3", *tracker );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr only_train = trainList.front();
	CPPUNIT_ASSERT( only_train == preboundTrain );
	const list <BadgeE> & train_place = only_train->GetPlace();
	CPPUNIT_ASSERT( train_place.size() == 1 && train_place.front() == TI.Get(L"AA3").bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}



void TC_TrackerMultiTrain::HeterogeneousMoving()
{
	TI.DetermineStripBusy( L"D", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineStripBusy( L"D2", *tracker );
	TI.DetermineStripBusy( L"E2", *tracker );
	auto trains = trainCont->GetUnities();
	TrainUnityCPtr dtrain, ctrain, d2e2train;
	for ( auto train : trains )
	{
		const auto & place = train->GetPlace();
		if ( place.size() == 1 && place.front() == TI.Get( L"D" ).bdg )
			dtrain = train;
		else if ( place.size() == 1 && place.front() == TI.Get( L"C" ).bdg )
			ctrain = train;
		else if ( place.size() == 2 )
			d2e2train = train;
	}
	CPPUNIT_ASSERT( dtrain && ctrain && d2e2train );

	TI.DetermineStripFree( L"C", *tracker, false );
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineSwitchPlus( L"22", *tracker, false );
	TI.DetermineStripBusy( L"E", *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		if ( train == ctrain )
			CPPUNIT_ASSERT( tplace.size() == 1 && ( tplace.front() == TI.Get( L"B" ).bdg || tplace.front() == TI.Get( L"F" ).bdg ) );
		else if ( train == dtrain )
			CPPUNIT_ASSERT( tplace.size() == 1 && tplace.front() == TI.Get( L"E" ).bdg );
		else if ( train == d2e2train )
			CPPUNIT_ASSERT( tplace.size() == 2 );
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}



void TC_TrackerMultiTrain::TrainOccupyNoTrain()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	wstring stripsArr[] = { L"K", L"H", L"W", L"Q" };
	TI.ImitateMotion( vector <wstring>( begin( stripsArr ), end( stripsArr ) ), *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	CPPUNIT_ASSERT( train->IsTrain() );
	auto trainId = train->GetId();
	TI.DetermineStripBusy( L"F", *tracker ); //появление новой ПЕ (F)
	TI.DetermineStripBusy( L"C", *tracker ); //F-C
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.FlushData( *tracker, false ); //смещение поезда и ПЕ вплотную

	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.DetermineStripFree( L"C", *tracker, false );
	TI.FlushData( *tracker, false ); //на единственном участке F должен остаться поезд
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	train = trains.front();
	CPPUNIT_ASSERT( train->IsTrain() && train->GetId() == trainId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::BidirCreate()
{
	TI.DetermineStripBusy( L"A", *tracker ); //поезд
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	for ( auto tunity : trains )
	{
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 )
		{
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"A" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( !tunity->IsTrain() );
		}
		else
		{
			CPPUNIT_ASSERT( tplace.size() == 2 );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"Q" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"W" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( tunity->IsTrain() );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::Intersection()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"K", *tracker ); //
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	SetInfo( train, TrainDescr() ); //поезд 1
	TI.DetermineStripBusy( L"W", *tracker ); //не поезд
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	trains = trainCont->GetUnities();
	auto trIt = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr tptr ){
		return tptr->GetPlace().size() != 1;
	} );
	CPPUNIT_ASSERT( trIt != trains.cend() );
	auto statTrain = *trIt;
	SetInfo( statTrain, TrainDescr() ); //поезд 2

	//в результате следующей посылки два поезда могут занять общее место (W)
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );
	
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::CloseWrongSemaPassing()
{
	//выставляем вплотную два поезда (R4 и S4)
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainR4 = trains.front();
	auto trainR4Id = trainR4->GetId();
	SetInfo( trainR4, TrainDescr() );

	TI.DetermineStripBusy( L"G4", *tracker );
	trains = trainCont->GetUnities();
	auto trIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr tcptr ){
		return tcptr->GetPlace().front() == TI.Get( L"G4" ).bdg;
	} );
	ASSERT( trIt != trains.cend() );
	auto trainS4 = *trIt;
	auto trainS4Id = trainS4->GetId();
	SetRandomOddInfo( trainS4 );
	auto trainDescrS4 = *trainS4->GetDescrPtr();
	TI.DetermineStripBusy( L"F4", *tracker );
	TI.DetermineStripFree( L"G4", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker, false );
	TI.DetermineStripFree( L"F4", *tracker, false );
	TI.FlushData( *tracker, false );

	//R4 - гашение. F4 и G4 - занятость. -> неизвестный поезд на R4 уничтожается, а нечетный поезд на S4 сохраняется
	trains = trainCont->GetUnities();
	TI.DetermineStripFree( L"R4", *tracker, false );
	TI.DetermineStripBusy( L"F4", *tracker, false );
	TI.DetermineStripBusy( L"G4", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();

	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		if ( tplace.size() == 1 && tplace.front() == TI.Get( L"S4" ).bdg )
		{
			CPPUNIT_ASSERT( train->GetId() == trainS4Id );
			auto tdescrPtr = train->GetDescrPtr();
			CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == trainDescrS4 );
		}
		else
		{
			CPPUNIT_ASSERT( tplace.size() == 2 );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"F4" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"G4" ).bdg ) != tplace.cend() );
			//новая маневровая ПЕ:
			CPPUNIT_ASSERT( train->GetId() != trainS4Id && train->GetId() != trainR4Id );
			CPPUNIT_ASSERT( !train->IsTrain() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::NoMergeTrainWithUnityWhileDisforming()
{
	TI.DetermineStripBusy( L"M3", *tracker, false );
	TI.DetermineStripBusy( L"N3", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"K3", *tracker );
	TI.DetermineStripBusy( L"L3", *tracker, false );
	TI.DetermineStripFree( L"K3", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trains = trainCont->GetUnities();
	auto trIt1 = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr tcptr ){
		return tcptr->GetPlace().size() == 1;
	} );
	CPPUNIT_ASSERT( trIt1 != trains.cend() );
	auto ftrain = *trIt1;
	SetRandomOddInfo( ftrain );
	auto trIt2 = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr tcptr ){
		return tcptr->GetPlace().size() == 2;
	} );
	CPPUNIT_ASSERT( trIt2 != trains.cend() );
	auto fnotrain = *trIt2;

	TI.DetermineStripFree( L"L3", *tracker, false );
	TI.DetermineStripFree( L"N3", *tracker, false );
	TI.FlushData( *tracker, false );

	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto strainUnity = trains.back();
	const auto & tplace = strainUnity->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 );
	CPPUNIT_ASSERT( tplace.back() == TI.Get( L"M3" ).bdg );
	CPPUNIT_ASSERT( fnotrain->GetId() == strainUnity->GetId() );
	CPPUNIT_ASSERT( !strainUnity->IsTrain() );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::MultiConflict()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"12", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	TI.DetermineStripBusy( L"E", *tracker );
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );

	auto trains = trainCont->GetUnities();
	set <unsigned int> idens;
	for ( auto train : trains )
	{
		idens.insert( train->GetId() );
	}

	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( idens.find( ( *trains.cbegin() )->GetId() ) != idens.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::MultiFreeNBusy()
{
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchMinus( L"42", *tracker );
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"46", *tracker );

	//поезд четный
	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripBusy( L"B4", *tracker );
	auto evenTrainPtr = SetRandomEvenInfo( TI.Get( L"A4" ).bdg );
	CPPUNIT_ASSERT( evenTrainPtr );
	auto evenTrainId = evenTrainPtr->GetId();

	//поезд нечетный
	TI.DetermineStripBusy( L"E4", *tracker );
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineStripBusy( L"C4", *tracker );
	auto oddTrainPtr = SetRandomOddInfo( TI.Get( L"C4" ).bdg );
	auto oddTrainId = oddTrainPtr->GetId();

	//не поезд
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker );

	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );

	TI.DetermineStripFree( L"B4", *tracker, false );
	TI.DetermineStripFree( L"C4", *tracker, false );
	TI.DetermineStripBusy( L"L4", *tracker, false );
	TI.DetermineStripBusy( L"N4", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 3 );
	auto a4TrainPtr = trainCont->GetUnity( TI.Get( L"A4" ).bdg );
	CPPUNIT_ASSERT( a4TrainPtr && a4TrainPtr->GetId() == evenTrainId && a4TrainPtr->GetOddness() == Oddness::EVEN );
	CPPUNIT_ASSERT( a4TrainPtr->GetPlace().size() == 1 );
	auto d4TrainPtr = trainCont->GetUnity( TI.Get( L"D4" ).bdg );
	CPPUNIT_ASSERT( d4TrainPtr && d4TrainPtr->GetId() == oddTrainId && d4TrainPtr->GetOddness() == Oddness::ODD );
	CPPUNIT_ASSERT( d4TrainPtr->GetPlace().size() == 2 && d4TrainPtr->Have( TI.Get( L"E4" ).bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::IndexIgnoringOnTrainPriority()
{
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"I", *tracker, false );
	TI.DetermineStripFree( L"X", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineStripBusy( L"E", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );

	auto ITrainId = 0, GTrainId = 0;
	auto tunities = trainCont->GetUnities();
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		TrainDescr tdescr = RandomTrainDescr( true );
		if ( tplace.size() == 1 )
		{
			tdescr.SetIndex( L"" );
			ITrainId = tunity->GetId();
		}
		else
			GTrainId = tunity->GetId();
		SetInfo( tunity, tdescr );
	}

	CPPUNIT_ASSERT( ITrainId != 0 && GTrainId != 0 && ITrainId != GTrainId );
	TI.DetermineStripFree( L"E", *tracker, false );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.front();
	CPPUNIT_ASSERT( curtrain->GetId() == ITrainId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::TrainExitVsNoTrainMovingOnOpenHead()
{
	TI.DetermineRouteSet( L"Head101->A", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomOddInfo( tunities.back() );
	TI.DetermineStripBusy( L"A", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		return tptr->IsTrain();
	} ) );
	CPPUNIT_ASSERT( any_of( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		return !tptr->IsTrain();
	} ) );
	TI.DetermineStripBusy( L"B", *tracker );
	tunities = trainCont->GetUnities();
	
	//т.к. светофор открыт - это выезд поезда, а не движение маневровой
	auto notrainIt = find_if( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		return !tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( notrainIt == tunities.cend() );
	auto train = tunities.back();
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 3 );
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"C" ).bdg; } ) );
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"B" ).bdg; } ) );
	CPPUNIT_ASSERT( any_of( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & bdg ){ return bdg == TI.Get( L"A" ).bdg; } ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::NoTrainsLessIdenOccupyPriority()
{
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto ltrainId = ( tunities.front()->GetId() < tunities.back()->GetId() ? tunities.front()->GetId() : tunities.back()->GetId() );
	auto gtrainId = ( ltrainId == tunities.front()->GetId() ? tunities.back()->GetId() : tunities.front()->GetId() );

	CPPUNIT_ASSERT( ltrainId < gtrainId );

	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for ( auto tunity : tunities )
	{
		CPPUNIT_ASSERT( !tunity->IsTrain() );
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 2 )
			CPPUNIT_ASSERT( tunity->GetId() == ltrainId );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::ConflictBetweenNearAndFar()
{
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"K", *tracker );
	auto qunity = SetRandomOddInfo( TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( qunity );
	auto qtrainId = qunity->GetId();
	auto kunity = SetRandomOddInfo( TI.Get( L"K" ).bdg );
	auto ktrainId = kunity->GetId();
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.DetermineStripFree( L"K", *tracker, false );
	TI.FlushData( *tracker, false );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for ( auto train : tunities )
	{
		CPPUNIT_ASSERT( train->IsTrain() );
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.back() == TI.Get( L"Q" ).bdg )
			CPPUNIT_ASSERT( train->GetId() == qtrainId );
		else
			CPPUNIT_ASSERT( train->GetId() == ktrainId );
	}
}

void TC_TrackerMultiTrain::TailFadeNearADWayWithTrain()
{
	TI.DetermineStripBusy( L"L", *tracker );
	TI.DetermineStripBusy( L"M", *tracker );
	SetRandomOddInfo( TI.Get( L"M" ).bdg );

	TI.DetermineSwitchPlus( L"16", *tracker );
	TI.DetermineStripBusy( L"O", *tracker, false );
	TI.DetermineStripBusy( L"P", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripFree( L"O", *tracker );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::TailFadeNearADWayWithNoTrain()
{
	TI.DetermineStripBusy( L"L", *tracker );
	TI.DetermineStripBusy( L"M", *tracker );
	SetRandomOddInfo( TI.Get( L"M" ).bdg );

	TI.DetermineSwitchPlus( L"16", *tracker );
	TI.DetermineStripBusy( L"O", *tracker, false );
	TI.DetermineStripBusy( L"P", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	auto mlTrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
	return tptr->Have( TI.Get( L"M" ).bdg );
	} );
	CPPUNIT_ASSERT( mlTrainIt != tunities.cend() );
	DisformTrain( ( *mlTrainIt )->GetId() );
	TI.DetermineStripFree( L"O", *tracker );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::TailFadeNearTwoShortTrains()
{
	TI.DetermineStripBusy( L"O", *tracker, false );
	TI.DetermineStripBusy( L"P", *tracker, false );
	TI.DetermineStripBusy( L"R", *tracker, false );
	TI.DetermineStripBusy( L"V", *tracker, false );
	TI.DetermineSwitchPlus( L"16", *tracker, false );
	TI.FlushData( *tracker, false );

	SetRandomOddInfo( TI.Get( L"O" ).bdg );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"207" ) ), list<wstring>( 1, L"M" ) );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"209" ) ), list<wstring>( 1, L"L" ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"O", *tracker, false );
	TI.DetermineStripFree( L"P", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::TailFadeNearTwoShortTrains2()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"T5" ) );
	list<wstring> midplace;
	midplace.push_back( L"U5" );
	midplace.push_back( L"L5" );
	midplace.push_back( L"M5" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), midplace );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"N5" ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripFree( L"U5", *tracker, false );
	TI.DetermineStripFree( L"L5", *tracker, false );
	TI.DetermineStripBusy( L"M5", *tracker, false );
	TI.DetermineStripBusy( L"N5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::SwitchToggleBreakSequence()
{
	TI.DetermineSwitchMinus( L"51", *tracker );
	TI.DetermineStripBusy( L"W5", *tracker, false );
	TI.DetermineStripBusy( L"V5", *tracker, false );
	TI.FlushData( *tracker, false );
	list<wstring> tplace;
	tplace.push_back( L"A5" );
	tplace.push_back( L"C5" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace );

	auto tunities = trainCont->GetUnities();

	TI.DetermineSwitchPlus( L"51", *tracker, false );
	TI.DetermineStripBusy( L"A5", *tracker, false );
	TI.DetermineStripBusy( L"B5", *tracker, false );
	TI.DetermineStripFree( L"C5", *tracker, false );
	TI.DetermineStripBusy( L"V5", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerMultiTrain::SwitchToggleBreakSequence2()
{
	TI.DetermineSwitchMinus( L"22", *tracker );
	TI.DetermineSwitchMinus( L"23", *tracker );

	list<wstring> tplace;
	tplace.push_back( L"E2" );
	tplace.push_back( L"K2" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace );

	TI.DetermineStripBusy( L"M2", *tracker, false );
	TI.DetermineStripBusy( L"D2", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	TI.DetermineSwitchPlus( L"23", *tracker, false );
	TI.DetermineSwitchPlus( L"22", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripFree( L"K2", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}