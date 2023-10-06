#include "stdafx.h"
#include "TC_TrackerSingleTrain.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainUnity.h"
#include "../Guess/TrainInfoPackage.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerSingleTrain );
void TC_TrackerSingleTrain::setUp()
{
	TI.Reset();
	tracker->Reset();
}

void TC_TrackerSingleTrain::Appearance()
{
	//имитируем возникновение одной ПЕ на одном участке
	TI.DetermineStripBusy( L"C", *tracker );
	CPPUNIT_ASSERT( trainCont->GetUnities().size() == 1 );
	SetRandomOddInfo( trainCont->GetUnities().front() );
	for ( unsigned int k = 0; k < 2; ++k )
	{
		list <TrainUnityCPtr> trainList = trainCont->GetUnities();
		CPPUNIT_ASSERT( trainList.size() == 1 );
		TrainUnityCPtr trainPtr = *trainList.begin();
		const list <BadgeE> & trainPlace = trainPtr->GetPlace();
		CPPUNIT_ASSERT( trainPlace.size() == 1 );
		CPPUNIT_ASSERT( *trainPlace.begin() == TI.Get(L"C").bdg );
		changes = tracker->TakeTrainChanges();
		if ( k == 0 )
		{
			CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
			CPPUNIT_ASSERT( changes.placeViewChanges.begin()->place == TI.Get(L"C").bdg );
		}
		else
			CPPUNIT_ASSERT( changes.placeViewChanges.empty() ); //повторные сообщения ничего не меняют
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		TI.DetermineStripBusy( L"C", *tracker ); //дублируем сообщение - ничего не должно меняться
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::AppearanceOnSwitch()
{
	//имитируем возникновение ПЕ на двух участках, прилегающих к стрелке
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"F" , *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr = trainList.front();
	SetRandomOddInfo( trainPtr );
	TI.DetermineStripBusy( L"G", *tracker );

	const list <BadgeE> & trainPlace = trainPtr->GetPlace();
	CPPUNIT_ASSERT( trainPlace.size() == 2 );
	CPPUNIT_ASSERT ( find ( trainPlace.begin(), trainPlace.end(), TI.Get(L"F").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT ( find ( trainPlace.begin(), trainPlace.end(), TI.Get(L"G").bdg ) != trainPlace.end() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"F").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"G").bdg ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::AtomicMultiSwitchMultiAppearance()
{
	//имитируем возникновение нескольких ПЕ ( в том числе и ПЕ, пролегающей через 2 стрелки)
	TI.DetermineStripBusy( L"F2", *tracker, false );
	TI.DetermineStripBusy( L"L2", *tracker, false );
	TI.DetermineStripFree( L"K2", *tracker, false );
	
	TI.DetermineStripBusy( L"B2", *tracker, false );

	TI.DetermineSwitchPlus( L"22", *tracker, false );
	TI.DetermineSwitchPlus( L"23", *tracker, false );
	TI.DetermineStripFree( L"H2", *tracker, false );
	TI.DetermineStripBusy( L"M2", *tracker, false );
	TI.DetermineStripBusy( L"D2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );

	
	TI.FlushData( *tracker, false );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr train1, train2;
	train1 = ( trainList.front()->GetPlace().size() == 1 ) ? trainList.front() : trainList.back();
	train2 = ( train1 == trainList.front() ) ? trainList.back() : trainList.front();
	SetRandomOddInfo( train1 );
	SetRandomOddInfo( train2 );
	const list <BadgeE> & train1Place = train1->GetPlace();
	const list <BadgeE> & train2Place = train2->GetPlace();
	CPPUNIT_ASSERT( train1Place.size() == 1 );
	CPPUNIT_ASSERT ( find ( train1Place.begin(), train1Place.end(), TI.Get(L"B2").bdg ) != train1Place.end() );
	CPPUNIT_ASSERT( train2Place.size() == 5 );
	CPPUNIT_ASSERT ( find ( train2Place.begin(), train2Place.end(), TI.Get(L"L2").bdg ) != train2Place.end() );
	CPPUNIT_ASSERT ( find ( train2Place.begin(), train2Place.end(), TI.Get(L"F2").bdg ) != train2Place.end() );
	CPPUNIT_ASSERT ( find ( train2Place.begin(), train2Place.end(), TI.Get(L"E2").bdg ) != train2Place.end() );
	CPPUNIT_ASSERT ( find ( train2Place.begin(), train2Place.end(), TI.Get(L"D2").bdg ) != train2Place.end() );
	CPPUNIT_ASSERT ( find ( train2Place.begin(), train2Place.end(), TI.Get(L"M2").bdg ) != train2Place.end() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 6 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"B2").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"L2").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"F2").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"E2").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"D2").bdg ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"M2").bdg ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::NamedMoving()
{
	//пусть модуль отслеживания ведет одну ПЕ
	TI.DetermineStripBusy( L"C", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr = *trainList.begin();
	list <BadgeE> trainPlace = trainPtr->GetPlace();

	//именуем ПЕ
	SetRandomEvenInfo( trainPtr );
	ConstTrainDescrPtr initDescrPtr = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( initDescrPtr );
	TrainDescr testTrainDescr = *initDescrPtr;
	changes = tracker->TakeTrainChanges();

	//имитируем изменение положения ПЕ (получение xml-сообщение от Fund'a): смещение с C на F
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	trainPlace = trainPtr->GetPlace();
	CPPUNIT_ASSERT( trainPlace.size() == 2 );
	CPPUNIT_ASSERT ( find ( trainPlace.begin(), trainPlace.end(), TI.Get(L"F").bdg ) != trainPlace.end() );
	TI.DetermineStripFree( L"C", *tracker );
	trainPlace = trainPtr->GetPlace();
	CPPUNIT_ASSERT( trainPlace.size() == 1 );
	CPPUNIT_ASSERT ( find ( trainPlace.begin(), trainPlace.end(), TI.Get(L"F").bdg ) != trainPlace.end() );
	changes = tracker->TakeTrainChanges();

	//проверяем, что Tracker считает измененным
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	auto FIt = FindIn( changes.placeViewChanges, TI.Get(L"F").bdg );
	auto CIt = FindIn( changes.placeViewChanges, TI.Get(L"C").bdg );
	CPPUNIT_ASSERT( FIt != nullptr && CIt != nullptr );
	CPPUNIT_ASSERT( FIt->appeared );
	ConstTrainDescrPtr tdescr = FIt->tdescrPtr;
	CPPUNIT_ASSERT( tdescr && *tdescr == testTrainDescr );
	CPPUNIT_ASSERT( !CIt->appeared );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::NamedDoubling()
{
	//пусть модуль отслеживания ведет одну ПЕ
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	//имитируем дублирование занятости участка в середине ПЕ
	TI.DetermineStripBusy( L"C", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr = *trainList.begin();
	SetRandomOddInfo( trainPtr );
	const list <BadgeE> & trainPlace = trainPtr->GetPlace();
	CPPUNIT_ASSERT( trainPlace.size() == 3 );
	CPPUNIT_ASSERT ( find ( trainPlace.begin(), trainPlace.end(), TI.Get(L"B").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT ( find ( trainPlace.begin(), trainPlace.end(), TI.Get(L"C").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT ( find ( trainPlace.begin(), trainPlace.end(), TI.Get(L"F").bdg ) != trainPlace.end() );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::AtomicMoving()
{
	//пусть модуль отслеживания ведет одну ПЕ
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr = *trainList.begin();

	//именуем ПЕ
	TrainDescr testTrainDescr = RandomTrainDescr();
	SetInfo( trainPtr, testTrainDescr );

	changes = tracker->TakeTrainChanges();

	//имитируем атомарное изменение положения ПЕ (получение xml-сообщение от Fund'a): смещение на G
	TI.DetermineStripFree( L"B", *tracker, false );
	TI.DetermineStripFree( L"C", *tracker, false );
	TI.DetermineStripFree( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr new_trainPtr = *trainList.begin();
	CPPUNIT_ASSERT( trainPtr->GetId() == new_trainPtr->GetId() );
	auto tdescr_ptr = new_trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescr_ptr != nullptr );
	CPPUNIT_ASSERT( *tdescr_ptr == testTrainDescr );
	const list <BadgeE> & newTrainPlace = new_trainPtr->GetPlace();
	CPPUNIT_ASSERT( newTrainPlace.size() == 1 );
	CPPUNIT_ASSERT( newTrainPlace.front() == TI.Get(L"G").bdg );

	//проверяем, что Tracker считает измененным
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 4 );
	auto BIt = FindIn( changes.placeViewChanges, TI.Get(L"B").bdg );
	auto CIt = FindIn( changes.placeViewChanges, TI.Get(L"C").bdg );
	auto FIt = FindIn( changes.placeViewChanges, TI.Get(L"F").bdg );
	auto GIt = FindIn( changes.placeViewChanges, TI.Get(L"G").bdg );
	CPPUNIT_ASSERT( BIt && CIt && FIt && GIt );
	CPPUNIT_ASSERT( !BIt->appeared );
	CPPUNIT_ASSERT( !CIt->appeared );
	CPPUNIT_ASSERT( !FIt->appeared );
	CPPUNIT_ASSERT( GIt->appeared );
	ConstTrainDescrPtr tdescr = GIt->tdescrPtr;
	CPPUNIT_ASSERT( tdescr && *tdescr == testTrainDescr );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::AtomicMoving2()
{
	//пусть модуль отслеживания ведет одну ПЕ
	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineStripBusy( L"F2", *tracker );
	TI.DetermineStripBusy( L"E2", *tracker );
	TI.DetermineStripBusy( L"D2", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr = *trainList.begin();
	//const list <BadgeE> & trainPlace = trainPtr->GetPlace();

	//именуем ПЕ
	TrainDescr testTrainDescr = RandomTrainDescr();
	SetInfo( trainPtr, testTrainDescr );

	changes = tracker->TakeTrainChanges();

	//имитируем атомарное изменение положения ПЕ (получение xml-сообщение от Fund'a): смещение на M2
	TI.DetermineStripFree( L"F2", *tracker, false );
	TI.DetermineStripFree( L"K2", *tracker, false );
	TI.DetermineSwitchPlus( L"22", *tracker, false );
	TI.DetermineSwitchPlus( L"23", *tracker, false );
	TI.DetermineStripFree( L"H2", *tracker, false );
	TI.DetermineStripBusy( L"M2", *tracker, false );
	TI.DetermineStripFree( L"D2", *tracker, false );
	TI.DetermineStripFree( L"E2", *tracker, false );
	TI.FlushData( *tracker, false );

	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::MultiSwitchMoving()
{
	//выставляем стрелку и помещаем на стрелку ПЕ (участки A2, B2, G2)
	TI.DetermineSwitchMinus( L"24", *tracker );
	TI.DetermineStripBusy( L"A2" , *tracker );
	TI.DetermineStripBusy( L"B2" , *tracker );
	TI.DetermineStripBusy( L"G2" , *tracker );

	//именуем ПЕ
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr train = trainList.front();
	unsigned int before_iden = train->GetId();
	TrainDescr initDescr = RandomTrainDescr();
	SetInfo( train, initDescr );

	//очистка накопленных изменений
	changes = tracker->TakeTrainChanges();

	//смещение ПЕ через 2 стрелки
	TI.DetermineStripBusy( L"H2", *tracker, false );
	TI.DetermineStripBusy( L"K2", *tracker, false );
	TI.DetermineSwitchMinus( L"22", *tracker, false );
	TI.DetermineSwitchMinus( L"23", *tracker, false );
	TI.DetermineStripFree( L"D2", *tracker, false );
	TI.DetermineStripBusy( L"E2", *tracker, false );
	TI.FlushData( *tracker, false );

	//должна остаться та же единственная ПЕ
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr train_after = trainList.front();
	CPPUNIT_ASSERT( before_iden == train_after->GetId() );
	list <BadgeE> trainPlace = train_after->GetPlace();
	CPPUNIT_ASSERT( trainPlace.size() == 6 );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"A2").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"B2").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"G2").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"H2").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"E2").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"K2").bdg ) != trainPlace.end() );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 3 );
	auto H2It = FindIn( changes.placeViewChanges, TI.Get(L"H2").bdg );
	auto E2It = FindIn( changes.placeViewChanges, TI.Get(L"E2").bdg );
	auto K2It = FindIn( changes.placeViewChanges, TI.Get(L"K2").bdg );
	CPPUNIT_ASSERT( H2It && E2It && K2It );
	CPPUNIT_ASSERT( H2It->appeared );
	CPPUNIT_ASSERT( E2It->appeared );
	CPPUNIT_ASSERT( K2It->appeared );
	ConstTrainDescrPtr tdescr = H2It->tdescrPtr;
	CPPUNIT_ASSERT( tdescr && *tdescr == initDescr && H2It->trainId == E2It->trainId && H2It->trainId == K2It->trainId );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::MultiSwitchMoving2()
{
	//пусть модуль отслеживания ведет одну ПЕ
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );

	//именуем ПЕ
	list<TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr = trainList.front();
	unsigned int before_iden = trainPtr->GetId();
	TrainDescr initDescr = RandomTrainDescr();
	SetInfo( trainPtr, initDescr );

	changes = tracker->TakeTrainChanges();

	//имитируем атомарное смещение ПЕ
	TI.DetermineStripFree( L"E3", *tracker, false );
	TI.DetermineSwitchPlus( L"31", *tracker, false );
	TI.DetermineSwitchPlus( L"32", *tracker, false );
	TI.DetermineStripFree( L"B3", *tracker, false );
	TI.DetermineStripBusy( L"A3", *tracker, false );
	TI.DetermineStripBusy( L"C3", *tracker, false );
	TI.DetermineStripBusy( L"D3", *tracker, false );
	TI.FlushData( *tracker, false );

	//должна остаться та же единственная ПЕ
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr train_after = trainList.front();
	CPPUNIT_ASSERT( before_iden == train_after->GetId() );
	list <BadgeE> trainPlace = train_after->GetPlace();
	CPPUNIT_ASSERT( trainPlace.size() == 6 );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"Y3").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"X3").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"F3").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"D3").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"C3").bdg ) != trainPlace.end() );
	CPPUNIT_ASSERT( find( trainPlace.begin(), trainPlace.end(), TI.Get(L"A3").bdg ) != trainPlace.end() );

	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 3 );
	auto D3It = FindIn( changes.placeViewChanges, TI.Get(L"D3").bdg );
	auto C3It = FindIn( changes.placeViewChanges, TI.Get(L"C3").bdg );
	auto A3It = FindIn( changes.placeViewChanges, TI.Get(L"A3").bdg );
	CPPUNIT_ASSERT( D3It && C3It && A3It );
	CPPUNIT_ASSERT( D3It->appeared );
	CPPUNIT_ASSERT( C3It->appeared );
	CPPUNIT_ASSERT( A3It->appeared );
	ConstTrainDescrPtr tdescr = D3It->tdescrPtr;
	CPPUNIT_ASSERT( tdescr && *tdescr == initDescr && D3It->trainId == C3It->trainId && D3It->trainId == A3It->trainId );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::WrongSortingSideEffect()
{
	//начальные условия: ПЕ на участках K2-E2-H2.
	TI.DetermineSwitchMinus( L"23", *tracker );
	TI.DetermineSwitchMinus( L"22", *tracker );
	TI.DetermineStripBusy( L"K2", *tracker );
	TI.DetermineStripBusy( L"E2", *tracker );
	TI.DetermineStripBusy( L"H2", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.front()->GetPlace().size() == 3 );
	SetRandomOddInfo( trainList.front() );
	changes = tracker->TakeTrainChanges();

	//смещение на G2
	//в результате побочного эффекта ошибочного помещения участков в set смежным с G2 никогда не оказывался участок E2
	TI.DetermineStripBusy( L"G2", *tracker );

	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	CPPUNIT_ASSERT( trainList.front()->GetPlace().size() == 4 );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"G2").bdg ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::RefSameTrain()
{
	//имитируем приход двух одинаковых reference-посылок о размещении ПЕ на одном участке 
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.FlushData( *tracker, true );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr = *trainList.begin();
	CPPUNIT_ASSERT( trainPtr->GetPlace().size() == 1 && trainPtr->GetPlace().front() == TI.Get(L"C").bdg );
	unsigned int trainId = trainPtr->GetId();
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.FlushData( *tracker, true );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	trainPtr = *trainList.begin();
	CPPUNIT_ASSERT( trainPtr->GetPlace().size() == 1 && trainPtr->GetPlace().front() == TI.Get(L"C").bdg );
	unsigned int trainId2 = trainPtr->GetId();
	CPPUNIT_ASSERT( trainId == trainId2 ); //ПЕ должна восстановиться

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::RefDiffTrain()
{
	//имитируем приход двух reference-посылок о размещении ПЕ на разных участках
	TI.DetermineStripBusy( L"B", *tracker, false );
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.FlushData( *tracker, true );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr = *trainList.begin();
	list <BadgeE> trnPlace = trainPtr->GetPlace();
	CPPUNIT_ASSERT( trnPlace.size() == 2 );
	CPPUNIT_ASSERT( find( trnPlace.begin(), trnPlace.end(), TI.Get(L"B").bdg ) != trnPlace.end() 
		&& find( trnPlace.begin(), trnPlace.end(), TI.Get(L"C").bdg ) != trnPlace.end() );
	unsigned int trainId = trainPtr->GetId();
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.FlushData( *tracker, true );
	trainList = trainCont->GetUnities();
	trainPtr = *trainList.begin();
	trnPlace = trainPtr->GetPlace();
	CPPUNIT_ASSERT( trnPlace.size() == 2 );
	CPPUNIT_ASSERT( find( trnPlace.begin(), trnPlace.end(), TI.Get(L"C").bdg ) != trnPlace.end() 
		&& find( trnPlace.begin(), trnPlace.end(), TI.Get(L"F").bdg ) != trnPlace.end() );
	unsigned int trainId2 = trainPtr->GetId();
	CPPUNIT_ASSERT( trainId != trainId2 ); //должна создаться новая ПЕ

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::BidirectionalMovingForbid()
{
	TI.DetermineStripBusy( L"C3", *tracker );
	TI.DetermineSwitchMinus( L"31", *tracker );
	TI.DetermineSwitchMinus( L"32", *tracker );

	TI.DetermineStripBusy( L"A3", *tracker, false );
	TI.DetermineStripBusy( L"D3", *tracker, false );
	TI.DetermineSwitchPlus( L"31", *tracker, false );
	TI.DetermineSwitchPlus( L"32", *tracker, false );
	TI.FlushData( *tracker, false );

	set<BadgeE> plcbadges;
	list<TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	for ( auto train : trainList )
	{
		const auto & place = train->GetPlace();
		CPPUNIT_ASSERT( place.size() == 1 || place.size() == 2 );
		plcbadges.insert( place.cbegin(), place.cend() );
	}
	CPPUNIT_ASSERT( plcbadges.find( TI.Get( L"A3" ).bdg ) != plcbadges.cend() );
	CPPUNIT_ASSERT( plcbadges.find( TI.Get( L"C3" ).bdg ) != plcbadges.cend() );
	CPPUNIT_ASSERT( plcbadges.find( TI.Get( L"D3" ).bdg ) != plcbadges.cend() );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::MultiADWaysDiffStations()
{
	TI.DetermineHeadClose( L"Head300", *tracker );
	TI.DetermineStripBusy( L"K3", *tracker );
	TI.DetermineStripBusy( L"L3", *tracker );
	TI.DetermineStripBusy( L"M3", *tracker );
	TI.DetermineStripBusy( L"N3", *tracker );
	TI.DetermineStripBusy( L"O3", *tracker );
	TI.DetermineStripBusy( L"P3", *tracker );
	TI.DetermineStripFree( L"K3", *tracker );
	TI.DetermineStripFree( L"L3", *tracker );
	TI.DetermineStripFree( L"M3", *tracker );
	TI.DetermineStripFree( L"N3", *tracker );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::SimultaneousBusyAndFree()
{
	TI.DetermineStripBusy( L"V", *tracker );
	TI.DetermineStripBusy( L"Y", *tracker );
	TI.DetermineStripBusy( L"Z", *tracker );
	TI.DetermineSwitchMinus( L"17", *tracker );
	TI.DetermineStripBusy( L"AA", *tracker, false );
	TI.DetermineStripBusy( L"AB", *tracker, false );
	TI.DetermineStripBusy( L"AC", *tracker, false );
	TI.DetermineStripFree( L"V", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 5 );
	vector<BadgeE> tplaceVec( tplace.cbegin(), tplace.cend() );
	CPPUNIT_ASSERT( tplaceVec[0] == TI.Get( L"Y" ).bdg );
	CPPUNIT_ASSERT( tplaceVec[1] == TI.Get( L"Z" ).bdg );
	CPPUNIT_ASSERT( tplaceVec[2] == TI.Get( L"AA" ).bdg );
	CPPUNIT_ASSERT( tplaceVec[3] == TI.Get( L"AB" ).bdg );
	CPPUNIT_ASSERT( tplaceVec[4] == TI.Get( L"AC" ).bdg );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::MicrostationOut()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"210" ) ), list<wstring>( 1, L"BG5" ) );
	TI.DetermineStripBusy( L"BH5", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	const auto & tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"BG5" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::DEPARTURE && evvec[1]->GetBadge() == TI.Get( L"BG5" ).bdg );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::STATION_EXIT && evvec[2]->GetBadge() == TI.Get( L"BG5" ).bdg );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::SPAN_MOVE && evvec[3]->GetBadge() == TI.Get( L"BH5" ).bdg );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::SaveTrainWithinComplexADWay()
{
	TI.DetermineSwitchPlus( L"SK39", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"SK_4AAC" );
	tplace.push_back( L"SK_39SP:39+" );
	tplace.push_back( L"SK_39SP" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8202" ) ), tplace );
	auto tunities = trainCont->GetUnities();
	auto begtrain = tunities.back();
	auto initId = begtrain->GetId();

	TI.DetermineStripFree( L"SK_4AAC", *tracker );
	TI.DetermineStripBusy( L"SK_4ABC", *tracker );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto endtrain = tunities.back();
	CPPUNIT_ASSERT( endtrain->IsTrain() );
	CPPUNIT_ASSERT( endtrain->GetId() == initId );
	auto tdescrPtr = endtrain->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8202" ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerSingleTrain::NoSaveTrainWithinComplexADWay()
{
	TI.DetermineSwitchPlus( L"SK39", *tracker );
	list<wstring> tplace;
	tplace.push_back( L"SK_4AAC" );
	tplace.push_back( L"SK_39SP:39+" );
	tplace.push_back( L"SK_39SP" );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"8201" ) ), tplace );
	auto tunities = trainCont->GetUnities();
	auto begtrain = tunities.back();
	auto initId = begtrain->GetId();

	TI.DetermineStripFree( L"SK_4AAC", *tracker );
	TI.DetermineStripBusy( L"SK_4ABC", *tracker );
	TI.DetermineStripFree( L"SK_39SP:39+", *tracker );
	TI.DetermineStripFree( L"SK_39SP", *tracker );

	//т.к. движение поезда происходит в пределах станции, принадлежащей тех.узлу с игнорированием четности, он сохраняет свой статус
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto endtrain = tunities.back();
	CPPUNIT_ASSERT( endtrain->IsTrain() );
	CPPUNIT_ASSERT( endtrain->GetId() == initId );
	auto tdescrPtr = endtrain->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"8201" ) );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}