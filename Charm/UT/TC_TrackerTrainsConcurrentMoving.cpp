#include "stdafx.h"
#include "TC_TrackerTrainsConcurrentMoving.h"
#include "../Guess/TrainContainer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTrainsConcurrentMoving );
void TC_TrackerTrainsConcurrentMoving::setUp()
{
	TI.Reset();
}

void TC_TrackerTrainsConcurrentMoving::UnknownMoving()
{
	//располагаем две ПЕ на расстояние одного участка (ПЕ1 - D3, ПЕ2 - A3)
	TI.DetermineSwitchPlus( L"31", *tracker );
	TI.DetermineSwitchPlus( L"32", *tracker );
	TI.DetermineStripBusy( L"C3", *tracker );
	TI.DetermineStripBusy( L"D3", *tracker );
	TI.DetermineStripFree( L"C3", *tracker ); //для ПЕ1 задаем направление движения
	TI.DetermineStripBusy( L"A3", *tracker ); //ПЕ2

	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	TrainUnityCPtr d3Train, a3Train;
	for ( auto train : trainList )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"A3" ).bdg )
			a3Train = train;
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"D3" ).bdg );
			d3Train = train;
		}
	}

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );

	//смещаем обе ПЕ
	TI.DetermineStripBusy( L"C3", *tracker, false );
	TI.DetermineStripBusy( L"F3", *tracker, false );
	TI.FlushData( *tracker, false );
	trainList = trainCont->GetUnities();

	CPPUNIT_ASSERT( trainList.size() == 2 );

	const list <BadgeE> & place1 = a3Train->GetPlace();
	const list <BadgeE> & place2 = d3Train->GetPlace();
	CPPUNIT_ASSERT( place1.size() == 2 );
	CPPUNIT_ASSERT( find( place1.begin(), place1.end(), TI.Get(L"A3").bdg ) != place1.end() );
	CPPUNIT_ASSERT( find( place1.begin(), place1.end(), TI.Get(L"C3").bdg ) != place1.end() );
	CPPUNIT_ASSERT( place2.size() == 2 );
	CPPUNIT_ASSERT( find( place2.begin(), place2.end(), TI.Get(L"D3").bdg ) != place2.end() );
	CPPUNIT_ASSERT( find( place2.begin(), place2.end(), TI.Get(L"F3").bdg ) != place2.end() );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::RacingMovingSinglePlaces()
{
	//располагаем две ПЕ друг за другом (поезд: Q, маневровая: A)
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	TI.DetermineStripBusy( L"A", *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.FlushData( *tracker, false );
	SetRandomOddInfo( TI.Get( L"Q" ).bdg );

	list <TrainUnityCPtr> tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	unsigned int qtrainId = 0, atunityId = 0;
	TrainUnityCPtr qtrainPtr, atunityPtr;
	for( auto tunityPtr : tunities )
	{
		const auto & tplace = tunityPtr->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.back() == TI.Get( L"Q" ).bdg )
		{
			qtrainId = tunityPtr->GetId();
			qtrainPtr = tunityPtr;
		}
		else
		{
			CPPUNIT_ASSERT( tplace.back() == TI.Get( L"A" ).bdg );
			atunityId = tunityPtr->GetId();
			atunityPtr = tunityPtr;
		}
	}
	CPPUNIT_ASSERT( qtrainId != 0 && atunityId != 0 && qtrainId != atunityId );
	CPPUNIT_ASSERT( qtrainPtr->IsTrain() && !atunityPtr->IsTrain() );

	//смещаем обе ПЕ на один участок
	TI.DetermineStripBusy( L"W", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	CPPUNIT_ASSERT( train->IsTrain() && train->GetId() == qtrainId );
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get(L"W").bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get(L"Q").bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::RacingMovingMultiPlaces()
{
	list<wstring> tplace;
	tplace.push_back( L"O2" );
	tplace.push_back( L"M2" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2488" ) ), tplace );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2268" ) ), list<wstring>( 1, L"P2" ) );

	unsigned int trainId1 = 0, trainId2 = 0;
	auto tunities = trainCont->GetUnities();
	for( auto tunity : tunities )
	{
		if ( tunity->Have( TI.Get( L"O2" ).bdg ) )
			trainId1 = tunity->GetId();
		else if ( tunity->Have( TI.Get( L"P2" ).bdg ) )
			trainId2 = tunity->GetId();
	}

	TI.DetermineStripFree( L"P2", *tracker );
	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 2 );

	for( auto tunity : tunities2 )
	{
		auto tdescrPtr = tunity->GetDescrPtr();
		CPPUNIT_ASSERT( tdescrPtr );
		CPPUNIT_ASSERT( tunity->GetPlace().size() == 1 );
		CPPUNIT_ASSERT( tunity->GetId() == trainId1 || tunity->GetId() == trainId2 );
		if ( tunity->GetId() == trainId1 )
			CPPUNIT_ASSERT( *tdescrPtr == TrainDescr( L"2488" ) );
		else
			CPPUNIT_ASSERT( *tdescrPtr == TrainDescr( L"2268" ) );
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::CloseMoving()
{
	//устанавливаем вплотную две ПЕ (A3-C3 и D3-F3-X3-Y3)
	TI.DetermineSwitchPlus( L"31", *tracker );
	TI.DetermineSwitchMinus( L"32", *tracker );
	TI.DetermineStripBusy( L"A3", *tracker );
	TI.DetermineStripBusy( L"C3", *tracker );
	TI.DetermineStripBusy( L"D3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineSwitchPlus( L"32", *tracker );

	//идентифицируем каждую из ПЕ
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr front_train = trainList.front();
	TrainUnityCPtr back_train = trainList.back();
	auto place_front = front_train->GetPlace();
	auto a3It = find( place_front.cbegin(), place_front.cend(), TI.Get(L"A3").bdg );
	TrainUnityCPtr a3train = ( a3It != place_front.cend() ) ? front_train : back_train; //ПЕ, содержащая A3
	TrainUnityCPtr y3train = a3train == front_train ? back_train : front_train; //ПЕ, содержащая Y3
	SetRandomOddInfo( a3train );
	SetRandomOddInfo( y3train );
	changes = tracker->TakeTrainChanges();

	//гасим участок X3 (в середине одной из ПЕ)
	//должно стать A3-C3-D3-F3 и Y3.
	TI.DetermineStripFree( L"X3", *tracker );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	for ( auto train : trainList )
	{
		auto place = train->GetPlace();
		if ( train == a3train )
		{
			CPPUNIT_ASSERT( find( place.begin(), place.end(), TI.Get(L"A3").bdg ) != place.end() );
			CPPUNIT_ASSERT( find( place.begin(), place.end(), TI.Get(L"C3").bdg ) != place.end() );
			CPPUNIT_ASSERT( find( place.begin(), place.end(), TI.Get(L"D3").bdg ) != place.end() );
			CPPUNIT_ASSERT( find( place.begin(), place.end(), TI.Get(L"F3").bdg ) != place.end() );
		}
		else
			CPPUNIT_ASSERT( find( place.begin(), place.end(), TI.Get(L"Y3").bdg ) != place.end() );
	}

	//проверка изменений
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 3 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"D3").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"F3").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"X3").bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::CloseMoving2()
{
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"46", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );

	//ПЕ1 (N4-L4-H4-G4-E4-D4)
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineStripBusy( L"E4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker );
	TI.DetermineStripBusy( L"L4", *tracker );
	TI.DetermineStripBusy( L"N4", *tracker );

	//ПЕ2 (A4-B4-C4)
	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripBusy( L"B4", *tracker );
	TI.DetermineStripBusy( L"C4", *tracker );

	//идентифицируем ПЕ
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr left_train = ( trainList.front()->GetPlace().size() == 3 ? trainList.front() : trainList.back() );
	TrainUnityCPtr right_train = ( left_train == trainList.front() ? trainList.back() : trainList.front() );
	SetRandomEvenInfo( left_train );
	SetRandomEvenInfo( right_train );
	changes = tracker->TakeTrainChanges();

	//двигаем
	TI.DetermineStripFree( L"A4", *tracker, false ); //A4
	TI.DetermineStripFree( L"B4", *tracker, false ); //B4
	TI.DetermineStripFree( L"C4", *tracker, false ); //C4
	TI.DetermineStripFree( L"E4", *tracker, false ); //E4
	TI.DetermineStripFree( L"G4", *tracker, false ); //G4
	TI.FlushData( *tracker, false );

	//проверка положений
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	for ( auto train : trainList )
	{
		const auto & place = train->GetPlace();
		CPPUNIT_ASSERT( place.size() == 1 || place.size() == 3 );
		if ( place.size() == 1 )
		{
			BadgeE tbdg = place.front();
			CPPUNIT_ASSERT( tbdg == TI.Get( L"D4" ).bdg );
			CPPUNIT_ASSERT( left_train->GetId() == train->GetId() );
		}
		else
		{
			CPPUNIT_ASSERT( find( place.begin(), place.end(), TI.Get(L"N4").bdg ) != place.end() );
			CPPUNIT_ASSERT( find( place.begin(), place.end(), TI.Get(L"L4").bdg ) != place.end() );
			CPPUNIT_ASSERT( find( place.begin(), place.end(), TI.Get(L"H4").bdg ) != place.end() );
			CPPUNIT_ASSERT( right_train->GetId() == train->GetId() );
		}
	}

	//проверка изменений
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 6 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"A4").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"B4").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"C4").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"D4").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"E4").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"G4").bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::CloseMoving3()
{
	TI.DetermineSwitchPlus( L"31", *tracker );
	TI.DetermineSwitchMinus( L"32", *tracker );

	//ПЕ1
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"C3" ) ) );

	//ПЕ2
	list<wstring> tplace;
	tplace.push_back( L"Y3" );
	tplace.push_back( L"X3" );
	tplace.push_back( L"F3" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace ) );

	TrainUnityCPtr trainC, trainFXY;
	unsigned int trainId = 0;
	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	for ( auto train : trainList )
	{
		const auto & tplace = train->GetPlace();
		if ( tplace.size() == 1 )
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"C3" ).bdg );
			trainC = train;
		}
		else
		{
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"F3" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"X3" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"Y3" ).bdg ) != tplace.cend() );
			trainFXY = train;
			trainId = train->GetId();
		}
	}
	CPPUNIT_ASSERT( trainId != 0 );

	//одновременное смещение
	TI.DetermineStripBusy( L"A3", *tracker, false );
	TI.DetermineStripBusy( L"D3", *tracker, false );
	TI.DetermineStripFree( L"Y3", *tracker, false );
	TI.DetermineStripFree( L"X3", *tracker, false );
	TI.DetermineSwitchPlus( L"32", *tracker, false ); //внешне не важный, но ключевой фактор для всего теста (влияет на внутренню логику работы Guess'а)
	TI.FlushData( *tracker, false );

	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	auto curtrainPtr = trainCont->GetUnity( trainId );
	CPPUNIT_ASSERT( curtrainPtr );
	CPPUNIT_ASSERT( curtrainPtr->Have( TI.Get( L"F3" ).bdg ) );
	CPPUNIT_ASSERT( curtrainPtr->Have( TI.Get( L"D3" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::CloseMoving4()
{
	//четный поезд
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomEvenInfo( trains.front() );

	//смежный не поезд
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"46", *tracker );
	TI.DetermineStripBusy( L"B4", *tracker );
	TI.DetermineStripBusy( L"C4", *tracker );
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineStripBusy( L"E4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker );
	TI.DetermineStripBusy( L"L4", *tracker );
	TI.DetermineStripBusy( L"N4", *tracker );

	TI.DetermineSwitchPlus( L"41", *tracker, false );
	TI.DetermineStripFree( L"A4", *tracker, false );
	TI.DetermineStripFree( L"B4", *tracker, false );
	TI.DetermineStripFree( L"C4", *tracker, false );
	TI.FlushData( *tracker, false );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ChainShortening()
{
	//располагаем две ПЕ друг за другом (ПЕ1: Q-A, ПЕ2: D-E-G)
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchPlus( L"11", *tracker );

	list<wstring> tplace1;
	tplace1.push_back( L"Q" );
	tplace1.push_back( L"A" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace1 ) );
	list<wstring> tplace2;
	tplace2.push_back( L"D" );
	tplace2.push_back( L"E" );
	tplace2.push_back( L"G" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, tplace2 ) );
	
	changes = tracker->TakeTrainChanges();

	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );

	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.FlushData( *tracker, false );

	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 ); //стрелочную секцию может занимать только одна ПЕ

	auto curtrain = trainList.back();
	curtrain->Have( TI.Get( L"E" ).bdg );
	curtrain->Have( TI.Get( L"G" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ChainShortening2()
{
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );
	TI.DetermineStripBusy( L"L4", *tracker ); //ПЕ1 (L4)
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker ); //ПЕ2 (G4-H4)
	TI.DetermineStripBusy( L"C4", *tracker );
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineStripBusy( L"E4", *tracker ); //ПЕ3 (C4-D4-E4)
	auto trains = trainCont->GetUnities();
	auto trainIt = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr train ){ return train->GetPlace().size() == 1; } );
	CPPUNIT_ASSERT( trainIt != trains.cend() );
	auto train1Id = ( *trainIt )->GetId();
	trainIt = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr train ){ return train->GetPlace().size() == 2; } );
	CPPUNIT_ASSERT( trainIt != trains.cend() );
	auto train2Id = ( *trainIt )->GetId();
	trainIt = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr train ){ return train->GetPlace().size() == 3; } );
	CPPUNIT_ASSERT( trainIt != trains.cend() );
	auto train3Id = ( *trainIt )->GetId();

	//формирование цепочки
	TI.DetermineSwitchPlus( L"43", *tracker, false );
	TI.DetermineSwitchMinus( L"45", *tracker, false );
	TI.DetermineStripFree( L"L4", *tracker, false );
	TI.DetermineStripFree( L"H4", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ChainShortening3()
{
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchPlus( L"45", *tracker );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"L4") );
	list<wstring> tplace2;
	tplace2.push_back( L"G4" );
	tplace2.push_back( L"H4" );
	LocateUnityByForce( nullptr, tplace2 );

	list<wstring> tplace3;
	tplace3.push_back( L"C4" );
	tplace3.push_back( L"D4" );
	tplace3.push_back( L"E4" );
	LocateUnityByForce( nullptr, tplace3 );

	//формирование цепочки
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );

	//гашение
	TI.DetermineStripFree( L"L4", *tracker, false );
	TI.DetermineStripFree( L"H4", *tracker, false );
	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerTrainsConcurrentMoving::ChainShortening4()
{
	//поезд на R2 и ПЕ на P2-O2-M2-D2-E2
	TI.DetermineSwitchMinus( L"21", *tracker );
	TI.DetermineStripBusy( L"R2", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );

	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineStripBusy( L"P2", *tracker );
	TI.DetermineStripBusy( L"O2", *tracker );
	TI.DetermineStripBusy( L"M2", *tracker );
	TI.DetermineStripBusy( L"D2", *tracker );
	TI.DetermineStripBusy( L"E2", *tracker );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TI.DetermineSwitchPlus( L"21", *tracker );
	changes = tracker->TakeTrainChanges();

	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	unsigned int trainId = 0;
	TrainContainer & mutTrainCont = tracker->GetTrainContainer();
	for ( auto train : trainList )
	{
		if ( train->Have( TI.Get( L"R2" ).bdg ) )
		{
			SetRandomEvenInfo( train );
			trainId = train->GetId();
		}
	}

	//укорачивание ЦЗ до M2-D2-E2
	TI.DetermineStripFree( L"R2", *tracker, false );
	TI.DetermineStripFree( L"P2", *tracker, false );
	TI.DetermineStripFree( L"O2", *tracker, false );
	TI.FlushData( *tracker, false );

	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );

	auto curtrain = trainList.back();
	CPPUNIT_ASSERT( curtrain->IsTrain() && curtrain->GetId() == trainId );
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"M2" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D2" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"E2" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ChainShortening5()
{
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );

	LocateUnityByForce( nullptr, list<wstring>( 1, L"A" ) );
	list<wstring> dePlace;
	dePlace.push_back( L"D" );
	dePlace.push_back( L"E" );
	LocateUnityByForce( nullptr, dePlace );
	LocateUnityByForce( nullptr, list<wstring>( 1, L"G" ) );

	auto atunityPtr = trainCont->GetUnity( TI.Get( L"A" ).bdg );
	auto atrainId = atunityPtr->GetId();
	auto detunityPtr = trainCont->GetUnity( TI.Get( L"D" ).bdg );
	auto detrainId = detunityPtr->GetId();
	auto gtunityPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	auto gtrainId = gtunityPtr->GetId();

	TI.DetermineStripFree( L"E", *tracker, false );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	auto trains = trainCont->GetUnities();

	CPPUNIT_ASSERT( trains.size() == 2 );
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"A" ).bdg )
			CPPUNIT_ASSERT( train->GetId() == atrainId );
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"D" ).bdg );
			CPPUNIT_ASSERT( train->GetId() == detrainId || train->GetId() == gtrainId );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ChainShortening6()
{
	TI.DetermineSwitchMinus( L"44", *tracker, false );
	TI.DetermineStripBusy( L"G4", *tracker, false );
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.DetermineStripBusy( L"C4", *tracker, false );
	TI.DetermineStripBusy( L"D4", *tracker, false );
	TI.DetermineStripBusy( L"E4", *tracker, false );
	TI.DetermineStripBusy( L"A4", *tracker, false );
	TI.DetermineStripBusy( L"B4", *tracker, false );
	TI.FlushData( *tracker, false );

	SetInfo( TI.Get( L"H4" ).bdg, TrainDescr() );
	SetRandomEvenInfo( TI.Get( L"E4" ).bdg );

	auto trains = trainCont->GetUnities();

	TI.DetermineSwitchPlus( L"42", *tracker, false );
	TI.DetermineSwitchPlus( L"43", *tracker, false );
	TI.DetermineSwitchPlus( L"41", *tracker, false );
	TI.DetermineStripBusy( L"Q4", *tracker, false );
	TI.DetermineStripFree( L"H4", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ChainShortening7()
{
	TI.DetermineSwitchPlus( L"51", *tracker );
	TI.DetermineSwitchMinus( L"52", *tracker );
	TI.DetermineStripBusy( L"W5", *tracker );
	TI.DetermineStripBusy( L"V5", *tracker );
	SetRandomEvenInfo( TI.Get( L"W5" ).bdg );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"A5" ) );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"B5" ) );

	auto trains = trainCont->GetUnities();

	TI.DetermineStripFree( L"W5", *tracker, false );
	TI.DetermineStripFree( L"V5", *tracker, false );
	TI.FlushData( *tracker, false );

	trains = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ChainBreaking()
{
	//располагаем два поезда друг за другом (ПЕ1: Q-A, ПЕ2: D-E-G)
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"D", *tracker, false );
	TI.DetermineSwitchMinus( L"11", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"E", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );

	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	SetRandomOddInfo( trainList.front() );
	SetRandomEvenInfo( trainList.back() );
	changes = tracker->TakeTrainChanges();

	//смещаем
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.DetermineStripFree( L"E", *tracker, false );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.DetermineStripBusy( L"X", *tracker, false );
	TI.FlushData( *tracker, false );

	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr trainPtr1 = trainList.front();
	TrainUnityCPtr trainPtr2 = trainList.back();
	const list <BadgeE> & place1 = trainPtr1->GetPlace();
	const list <BadgeE> & place2 = trainPtr2->GetPlace();
	CPPUNIT_ASSERT( place1.size() == 2 );
	CPPUNIT_ASSERT( find( place1.begin(), place1.end(), TI.Get(L"Q").bdg ) != place1.end() );
	CPPUNIT_ASSERT( find( place1.begin(), place1.end(), TI.Get(L"A").bdg ) != place1.end() );
	CPPUNIT_ASSERT( place2.size() == 1 );
	CPPUNIT_ASSERT( find( place2.begin(), place2.end(), TI.Get(L"X").bdg ) != place2.end() );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 4 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"D").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"E").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"G").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"X").bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ParallelMovingNearWrongSema()
{
	TI.DetermineSwitchMinus( L"15", *tracker );
	TI.DetermineSwitchPlus( L"16", *tracker );
	TI.DetermineStripBusy( L"O", *tracker );
	auto trains = trainCont->GetUnities();
	SetRandomOddInfo( *trains.cbegin() );
	TI.DetermineStripBusy( L"J", *tracker );
	TI.DetermineStripBusy( L"L", *tracker );
	trains = trainCont->GetUnities();
	auto trIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr tcptr ){
		const auto & tplace = tcptr->GetPlace();
		return find_if( tplace.cbegin(), tplace.cend(), [this]( const BadgeE & plcBdg ){
			return plcBdg == TI.Get( L"L" ).bdg;
		} ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( trIt != trains.cend() );
	SetRandomOddInfo( *trIt );
	const auto & initTdescr = *( *trIt )->GetDescrPtr();
	TI.DetermineStripBusy( L"P", *tracker );
	trains = trainCont->GetUnities();
	TI.DetermineStripBusy( L"M", *tracker );
	trains = trainCont->GetUnities();

	trains = trainCont->GetUnities();
	auto head1 = trains.front()->Head();
	auto head2 = trains.back()->Head();

	TI.DetermineStripFree( L"J", *tracker, false );
	TI.DetermineStripBusy( L"R", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 2 || tplace.size() == 3 );
		if ( tplace.size() == 2 )
		{
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"L" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"M" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( train->GetDescrPtr() && initTdescr == *train->GetDescrPtr() );
		}
		else
		{
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"O" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"P" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"R" ).bdg ) != tplace.cend() );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::TrainChaseNoTrainThroughWrongSema()
{
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker ); //A4 - поезд
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	auto trainId = train->GetId();
	SetRandomOddInfo( train );
	TI.DetermineStripBusy( L"B4", *tracker );
	TI.DetermineStripBusy( L"C4", *tracker );
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineStripBusy( L"E4", *tracker ); //B4-C4-D4-E4 - не поезд
	TI.DetermineSwitchPlus( L"41", *tracker );
	trains = trainCont->GetUnities();
	unsigned int notrainId = 0;
	for ( auto tunity : trains )
	{
		if ( !tunity->IsTrain() )
			notrainId = tunity->GetId();
	}
	CPPUNIT_ASSERT( notrainId != 0 && notrainId != trainId );

	TI.DetermineStripFree( L"A4", *tracker, false );
	TI.DetermineStripFree( L"B4", *tracker, false );
	TI.DetermineStripFree( L"C4", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( trains.front()->GetId() == notrainId );
	const auto & tplace = trains.front()->GetPlace();
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"E4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::TrainChaseNoTrainThroughGoodSema()
{
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineStripBusy( L"Q4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.back()->GetId();
	TI.DetermineStripBusy( L"B4", *tracker );
	TI.DetermineSwitchPlus( L"41", *tracker, false );
	TI.DetermineStripBusy( L"A4", *tracker, false );
	TI.DetermineStripFree( L"Q4", *tracker, false );
	TI.DetermineStripBusy( L"C4", *tracker, false );
	TI.DetermineStripBusy( L"D4", *tracker, false );
	TI.DetermineStripBusy( L"E4", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	auto a4trainIt = find_if( trains.cbegin(), trains.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 1 && find( tplace.cbegin(), tplace.cend(), TI.Get( L"A4" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( a4trainIt != trains.cend() );
	SetRandomEvenInfo( *a4trainIt );
	trains = trainCont->GetUnities(); //A4 - поезд, B4-C4-D4-E4 - не поезд
	unsigned int notrainId = 0;
	for ( auto tunity : trains )
	{
		if ( !tunity->IsTrain() )
			notrainId = tunity->GetId();
	}
	CPPUNIT_ASSERT( notrainId != 0 && notrainId != trainId );

	TI.DetermineStripFree( L"A4", *tracker, false );
	TI.DetermineStripFree( L"B4", *tracker, false );
	TI.DetermineStripFree( L"C4", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtrain = trains.back();

	CPPUNIT_ASSERT( curtrain->IsTrain() && curtrain->GetId() == trainId );
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"E4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::TrainChaseTwoNoTrainsThroughWrongSema()
{
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker ); //A4 - поезд
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	SetRandomOddInfo( train );
	auto trainId = train->GetId();
	TI.DetermineStripBusy( L"G4", *tracker ); //G4 - не поезд
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineStripBusy( L"B4", *tracker );
	TI.DetermineStripBusy( L"C4", *tracker );
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineStripBusy( L"E4", *tracker ); //B4-C4-D4-E4 - не поезд
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );
	
	trains = trainCont->GetUnities();
	unsigned int d4tunityId = 0, g4tunityId = 0;
	for ( auto tunity : trains )
	{
		const auto & tplace = tunity->GetPlace();
		if ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D4" ).bdg ) != tplace.cend() )
			d4tunityId = tunity->GetId();
		else if ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"G4" ).bdg ) != tplace.cend() )
			g4tunityId = tunity->GetId();
	}
	CPPUNIT_ASSERT( d4tunityId != 0 && g4tunityId != 0 && d4tunityId != trainId && g4tunityId != trainId );
	TI.DetermineStripFree( L"A4", *tracker, false );
	TI.DetermineStripFree( L"B4", *tracker, false );
	TI.DetermineStripFree( L"C4", *tracker, false );
	TI.FlushData( *tracker, false );

	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	CPPUNIT_ASSERT( find_if( trains.cbegin(), trains.cend(), [&d4tunityId]( TrainUnityCPtr tunity ){ return tunity->GetId() == d4tunityId; } ) != trains.cend() );
	CPPUNIT_ASSERT( find_if( trains.cbegin(), trains.cend(), [&g4tunityId]( TrainUnityCPtr tunity ){ return tunity->GetId() == g4tunityId; } ) != trains.cend() );

	list <BadgeE> d4trainPlace, g4trainPlace;
	for ( auto tunity : trains )
	{
		CPPUNIT_ASSERT( !tunity->IsTrain() );
		const auto & tplace = tunity->GetPlace();
		if ( tunity->GetId() == d4tunityId )
			d4trainPlace = tplace;
		else
			g4trainPlace = tplace;
	}
	CPPUNIT_ASSERT( find( d4trainPlace.cbegin(), d4trainPlace.cend(), TI.Get( L"D4" ).bdg ) != d4trainPlace.cend() );
	CPPUNIT_ASSERT( find( g4trainPlace.cbegin(), g4trainPlace.cend(), TI.Get( L"G4" ).bdg ) != g4trainPlace.cend() );
	CPPUNIT_ASSERT( find( d4trainPlace.cbegin(), d4trainPlace.cend(), TI.Get( L"E4" ).bdg ) != d4trainPlace.cend() ||
		find( g4trainPlace.cbegin(), g4trainPlace.cend(), TI.Get( L"E4" ).bdg ) != g4trainPlace.cend() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::TrainChaseTwoNoTrainsThroughGoodSema()
{
	TI.DetermineSwitchPlus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );

	TI.DetermineStripBusy( L"A4", *tracker ); //A4 - поезд
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	SetRandomEvenInfo( train );
	auto trainId = train->GetId();

	list<wstring> place1;
	place1.push_back( L"B4" );
	place1.push_back( L"C4" );
	place1.push_back( L"D4" );
	place1.push_back( L"E4" );
	LocateUnityByForce( nullptr, place1 );

	list<wstring> place2;
	place2.push_back( L"G4" );
	LocateUnityByForce( nullptr, place2 );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	trains = trainCont->GetUnities();
	unsigned int d4tunityId = 0, g4tunityId = 0;
	for ( auto tunity : trains )
	{
		const auto & tplace = tunity->GetPlace();
		if ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D4" ).bdg ) != tplace.cend() )
			d4tunityId = tunity->GetId();
		else if ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"G4" ).bdg ) != tplace.cend() )
			g4tunityId = tunity->GetId();
	}
	CPPUNIT_ASSERT( d4tunityId != 0 && g4tunityId != 0 && d4tunityId != trainId && g4tunityId != trainId );
	TI.DetermineStripFree( L"A4", *tracker, false );
	TI.DetermineStripFree( L"B4", *tracker, false );
	TI.DetermineStripFree( L"C4", *tracker, false );
	TI.FlushData( *tracker, false );

	//должны иметь те же самые ПЕ расположенные на участках D4, E4, G4 в порядке своего следования
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtrain = trains.back();
	CPPUNIT_ASSERT( curtrain->GetId() == trainId );
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 3 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"E4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"G4" ).bdg ) != tplace.cend() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::TrainChaseTwoNoTrainsThroughGoodSema2()
{
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker ); //A4 - поезд
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	SetRandomEvenInfo( train );
	auto trainId = train->GetId();
	TI.DetermineStripBusy( L"G4", *tracker ); //G4-E4 - не поезд
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineSwitchPlus( L"43", *tracker );

	TI.DetermineStripBusy( L"E4", *tracker, false );
	TI.DetermineStripBusy( L"C4", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"B4", *tracker ); //B4-C4-D4 - не поезд
	TI.DetermineSwitchPlus( L"41", *tracker );
	
	trains = trainCont->GetUnities();
	unsigned int d4tunityId = 0, g4tunityId = 0;
	for ( auto tunity : trains )
	{
		const auto & tplace = tunity->GetPlace();
		if ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D4" ).bdg ) != tplace.cend() )
			d4tunityId = tunity->GetId();
		else if ( find( tplace.cbegin(), tplace.cend(), TI.Get( L"G4" ).bdg ) != tplace.cend() )
			g4tunityId = tunity->GetId();
	}
	CPPUNIT_ASSERT( d4tunityId != 0 && g4tunityId != 0 && d4tunityId != trainId && g4tunityId != trainId );
	TI.DetermineStripFree( L"A4", *tracker, false );
	TI.DetermineStripFree( L"B4", *tracker, false );
	TI.DetermineStripFree( L"C4", *tracker, false );
	TI.FlushData( *tracker, false );

	//на участках D4, E4, G4 остается поезд
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto curtrain = trains.back();
	CPPUNIT_ASSERT( curtrain->GetId() == trainId );
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"E4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"G4" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::TrainAbsorbsNoTrainThroughGoodSema()
{
	TI.DetermineSwitchMinus( L"42", *tracker );
	TI.DetermineStripBusy( L"B4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	auto trainId = train->GetId();
	SetRandomEvenInfo( train );

	TI.DetermineStripBusy( L"C4", *tracker );
	TI.DetermineStripBusy( L"D4", *tracker );
	TI.DetermineSwitchPlus( L"42", *tracker );

	TI.DetermineStripFree( L"B4", *tracker, false );
	TI.DetermineStripFree( L"C4", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( trains.back()->GetId() == trainId );
	const auto & tplace = trains.back()->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 );
	CPPUNIT_ASSERT( tplace.back() == TI.Get( L"D4" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::SpanChainShortening()
{
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	SetRandomOddInfo( TI.Get( L"A" ).bdg );
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trains = trainCont->GetUnities();
	auto hwTrainId = 0, qTrainId = 0;
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		if ( tplace.size() == 2 )
		{
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"H" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"W" ).bdg ) != tplace.cend() );
			hwTrainId = train->GetId();
		}
		else
		{
			CPPUNIT_ASSERT( tplace.back() == TI.Get( L"Q" ).bdg );
			qTrainId = train->GetId();
		}
	}
	TI.DetermineStripFree( L"Q", *tracker );
	auto restrains = trainCont->GetUnities();
	CPPUNIT_ASSERT( restrains.size() == 2 );
	for ( auto train : restrains )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if( tplace.back() == TI.Get( L"H" ).bdg )
			CPPUNIT_ASSERT( hwTrainId == train->GetId() );
		else
		{
			CPPUNIT_ASSERT( tplace.back() == TI.Get( L"W" ).bdg );
			CPPUNIT_ASSERT( qTrainId == train->GetId() );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::PushByDyingThroughStripSlow()
{
	PushByDyingThroughStrip( true );
}

void TC_TrackerTrainsConcurrentMoving::PushByDyingThroughStripFast()
{
	PushByDyingThroughStrip( false );
}

void TC_TrackerTrainsConcurrentMoving::PushByDyingThroughStrip( bool slowly )
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	auto trains = trainCont->GetUnities();
	auto statTrainId = 0, spanTrainId = 0;
	for ( auto train : trains )
		SetRandomOddInfo( train );
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );

	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"D", *tracker, false );
	TI.FlushData( *tracker, false );

	if ( slowly )
		TI.IncreaseTime( 1000 );
	
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 3 );
	auto notrainId = 0;
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"A" ).bdg )
			statTrainId = tunity->GetId();
		else if ( tplace.front() == TI.Get( L"Q" ).bdg )
			spanTrainId = tunity->GetId();
		else
			notrainId = tunity->GetId();
	}

	//гашение единственного участка крайнего поезда приводит к цепному смещению на место маневровой (маневровая на перегоне не сохраняется)
	TI.DetermineStripFree( L"A", *tracker );

	if ( slowly )
	{
		tunities = trainCont->GetUnities();
		CPPUNIT_ASSERT( tunities.size() == 2 );
		for ( auto tunity : tunities )
		{
			const auto & tplace = tunity->GetPlace();
			CPPUNIT_ASSERT( tplace.size() == 1 );
			if ( tplace.front() == TI.Get( L"H" ).bdg )
				CPPUNIT_ASSERT( tunity->GetId() == spanTrainId ); 
			else
				CPPUNIT_ASSERT( tunity->GetId() == statTrainId );
		}
	}
	else
		//в случае быстрого продвижения ограничиваемся проверкой целостности.
		;
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::ApproachingSpanTrainVsExitingStatTrain()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"805" ) ), list<wstring>( 1, L"TIM3" ) );
	auto statTrain = trainCont->GetUnity( TI.Get( L"TIM3" ).bdg );
	CPPUNIT_ASSERT( statTrain );
	auto statTrainId = statTrain->GetId();
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"807" ) ), list<wstring>( 1, L"TIM_E6" ) );

	TI.DetermineStripBusy( L"TIM_E5", *tracker, false );
	TI.DetermineStripFree( L"TIM_E6", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripBusy( L"TIM_E1", *tracker );

	statTrain = trainCont->GetUnity( statTrainId );
	CPPUNIT_ASSERT( statTrain );
	const auto & tplace = statTrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 );
	CPPUNIT_ASSERT( tplace.front() == TI.Get( L"TIM3" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::LeavingSpanTrainVsExitingStatTrain()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"808" ) ), list<wstring>( 1, L"TIM_E1" ) );
	TI.DetermineStripBusy( L"TIM_E5", *tracker, false );
	TI.DetermineStripFree( L"TIM_E1", *tracker, false );
	TI.FlushData( *tracker, false );

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"806" ) ), list<wstring>( 1, L"TIM3" ) );
	auto statTrain = trainCont->GetUnity( TI.Get( L"TIM3" ).bdg );
	CPPUNIT_ASSERT( statTrain );
	auto statTrainId = statTrain->GetId();
	
	auto tunities = trainCont->GetUnities();
	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"TIM_E1", *tracker );

	statTrain = trainCont->GetUnity( statTrainId );
	CPPUNIT_ASSERT( statTrain );
	const auto & tplace = statTrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( statTrain->Have( TI.Get( L"TIM3" ).bdg ) );
	CPPUNIT_ASSERT( statTrain->Have( TI.Get( L"TIM_E1" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsConcurrentMoving::LeavingSpanTrainVsExitingStatTrain2()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"806" ) ), list<wstring>( 1, L"TIM3" ) );
	auto statTrain = trainCont->GetUnity( TI.Get( L"TIM3" ).bdg );
	CPPUNIT_ASSERT( statTrain );
	auto statTrainId = statTrain->GetId();
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"808" ) ), list<wstring>( 1, L"TIM_E1" ) );

	auto tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"TIM_E5", *tracker, false );
	TI.DetermineStripFree( L"TIM_E1", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	TI.DetermineStripBusy( L"TIM_E1", *tracker );

	statTrain = trainCont->GetUnity( statTrainId );
	CPPUNIT_ASSERT( statTrain );
	const auto & tplace = statTrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( statTrain->Have( TI.Get( L"TIM3" ).bdg ) );
	CPPUNIT_ASSERT( statTrain->Have( TI.Get( L"TIM_E1" ).bdg ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}