#include "stdafx.h"
#include "TC_TrackerTrainsSplitting.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainInfoPackage.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTrainsSplitting );
void TC_TrackerTrainsSplitting::setUp()
{
	TI.Reset();
}

void TC_TrackerTrainsSplitting::SwitchSplittingOfTrain()
{
	//выставляем стрелку в "плюс" и помещаем на стрелку ПЕ (участки F и G)
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );

	//именуем ПЕ
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr train = trainList.front();
	TrainDescr tdescr = RandomTrainDescr();
	SetInfo( train, tdescr );

	//очистка накопленных изменений
	tracker->TakeChanges( changes );

	//перевод стрелки в "минус", приводящий к разбиению ПЕ
	TI.DetermineSwitchMinus( L"13", *tracker );
	tracker->TakeChanges( changes );

	//ПЕ объединяться не должны
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr train1 = trainList.front();
	TrainUnityCPtr train2 = trainList.back();
	list <BadgeE> train1Place = train1->GetPlace();
	list <BadgeE> train2Place = train2->GetPlace();
	CPPUNIT_ASSERT( train1Place.size() == 1 && train2Place.size() == 1 );
	CPPUNIT_ASSERT(  ( train1Place.front() == TI.Get(L"F").bdg || train1Place.front() == TI.Get(L"G").bdg ) && 
		( train2Place.front() == TI.Get(L"F").bdg || train2Place.front() == TI.Get(L"G").bdg ) );
	TrainUnityCPtr ftrain = ( train1Place.front() == TI.Get(L"F").bdg ? train1 : train2 );
	TrainUnityCPtr gtrain = ( ftrain == train1 ? train2 : train1 );
	auto fdescr_ptr = ftrain->GetDescrPtr();
	if ( fdescr_ptr != nullptr )
	{
		CPPUNIT_ASSERT( *fdescr_ptr == tdescr );
		CPPUNIT_ASSERT( !gtrain->IsTrain() );
	}
	else
	{
		auto gdescr_ptr = gtrain->GetDescrPtr();
		CPPUNIT_ASSERT( gdescr_ptr && *gdescr_ptr == tdescr );
		CPPUNIT_ASSERT( !ftrain->IsTrain() );
	}

	//появление новой ПЕ
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	CPPUNIT_ASSERT( changes.placeViewChanges.begin()->appeared );
	CPPUNIT_ASSERT( ( changes.placeViewChanges.begin()->place == TI.Get(L"G").bdg || changes.placeViewChanges.begin()->place == TI.Get(L"F").bdg ) );
	CPPUNIT_ASSERT( !changes.placeViewChanges.cbegin()->tdescrPtr );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsSplitting::StripSplittingOfTrain()
{
	//помещаем ПЕ на участки B, C и F
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	//именуем ПЕ
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	unsigned int before_iden = trainList.front()->GetId();
	SetRandomEvenInfo( trainList.back() );
	auto initDescrPtr = trainList.back()->GetDescrPtr();
	CPPUNIT_ASSERT( initDescrPtr );

	//очистка накопленных изменений
	tracker->TakeChanges( changes );

	//гашение участка в середине (участок C)
	TI.DetermineStripFree( L"C", *tracker );
	tracker->TakeChanges( changes );

	//должно образоваться две различные ПЕ (поезд и не поезд)
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr train = ( !trainList.front()->IsTrain() ? trainList.back() : trainList.front() );
	TrainUnityCPtr no_train = ( train == trainList.front() ? trainList.back() : trainList.front() );

	list <BadgeE> namPlace = train->GetPlace();
	list <BadgeE> unkPlace = no_train->GetPlace();
	CPPUNIT_ASSERT( namPlace.size() == 1 && unkPlace.size() == 1 );
	CPPUNIT_ASSERT(  ( namPlace.front() == TI.Get(L"B").bdg || namPlace.front() == TI.Get(L"F").bdg ) && 
		( unkPlace.front() == TI.Get(L"B").bdg || unkPlace.front() == TI.Get(L"F").bdg ) );
	CPPUNIT_ASSERT( train->GetId() == before_iden );
	auto tdescr_ptr = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescr_ptr != nullptr && *tdescr_ptr == *initDescrPtr );
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 2 );
	for ( auto cit = changes.placeViewChanges.begin(); cit != changes.placeViewChanges.end(); ++cit )
	{
		if ( cit->place == TI.Get(L"C").bdg )
			CPPUNIT_ASSERT( !cit->appeared );
		else
		{
			if ( cit->place == TI.Get(L"B").bdg )
				CPPUNIT_ASSERT( namPlace.front() == TI.Get(L"F").bdg );
			else
				CPPUNIT_ASSERT( namPlace.front() == TI.Get(L"B").bdg );
		}
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsSplitting::StripSplittingOfUnknown()
{
	//помещаем ПЕ на участки F3, X3 и Y3
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );

	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr train = trainList.front();
	unsigned int train_id = train->GetId();

	//очистка накопленных изменений
	changes = tracker->TakeTrainChanges();

	//гашение участка в середине (участок X3)
	TI.DetermineStripFree( L"X3", *tracker );
	changes = tracker->TakeTrainChanges();

	//должно создаться еще одна ПЕ
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	TrainUnityCPtr train1 = trainList.front();
	TrainUnityCPtr train2 = trainList.back();
	CPPUNIT_ASSERT( train1 == train || train2 == train );
	CPPUNIT_ASSERT( train_id == train1->GetId() || train_id == train2->GetId() );
	auto place1 = train1->GetPlace();
	auto place2 = train2->GetPlace();
	CPPUNIT_ASSERT( place1.size() == 1 && place2.size() == 1 );
	CPPUNIT_ASSERT( place1.front() == TI.Get( L"F3" ).bdg || place1.front() == TI.Get( L"Y3" ).bdg );
	CPPUNIT_ASSERT( place2.front() == TI.Get( L"F3" ).bdg || place2.front() == TI.Get( L"Y3" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsSplitting::LengthyDeclutching()
{
	//ставим длинную ПЕ
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"45", *tracker );
	TI.DetermineSwitchMinus( L"46", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripBusy( L"O4", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );
	TI.DetermineStripBusy( L"F4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker );
	TI.DetermineStripBusy( L"L4", *tracker );
	list <TrainUnityCPtr> trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	CPPUNIT_ASSERT( train->GetPlace().size() == 8 );

	//часть отцепляется с одного конца
	TI.DetermineStripFree( L"O4", *tracker, false );
	TI.DetermineStripFree( L"R4", *tracker, false );
	TI.DetermineStripFree( L"S4", *tracker, false );
	TI.DetermineStripFree( L"F4", *tracker, false );
	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.DetermineStripFree( L"H4", *tracker, false );
	TI.DetermineStripFree( L"L4", *tracker, false );
	TI.DetermineStripBusy( L"N4", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		CPPUNIT_ASSERT( tbdg == TI.Get(L"A4").bdg || tbdg == TI.Get(L"N4").bdg );
	}
}

void TC_TrackerTrainsSplitting::MultiSeparation()
{
	//ПЕ1
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );

	//ПЕ2
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );

	//разделение сразу обоих
	TI.DetermineStripFree( L"X3", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.FlushData( *tracker, false );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 4 );
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		CPPUNIT_ASSERT( tbdg == TI.Get( L"H" ).bdg || tbdg == TI.Get( L"Q" ).bdg || 
			tbdg == TI.Get( L"Y3" ).bdg || tbdg == TI.Get( L"F3" ).bdg );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsSplitting::SplittingAndRecovering()
{
	TI.DetermineSwitchPlus( L"17", *tracker );
	TI.DetermineStripBusy( L"AC", *tracker );
	auto tunities = trainCont->GetUnities();
	auto actrain = tunities.back();
	SetRandomEvenInfo( actrain );
	TI.DetermineStripBusy( L"P", *tracker );
	TI.DetermineStripBusy( L"V", *tracker );
	TI.DetermineStripBusy( L"Z", *tracker );

	TI.DetermineStripBusy( L"R", *tracker, false );
	TI.DetermineStripBusy( L"Y", *tracker, false );
	TI.DetermineStripBusy( L"AA", *tracker, false );
	TI.DetermineStripBusy( L"AD", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripFree( L"P", *tracker );

	TI.DetermineSwitchOutOfControl( L"17", *tracker, false );
	TI.DetermineStripBusy( L"AB", *tracker, false );
	TI.FlushData( *tracker, false );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	tunities = trainCont->GetUnities();
	auto abtrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"AB" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( abtrainIt != tunities.cend() );
	auto abtrain = *abtrainIt;
	SetRandomOddInfo( abtrain );

	auto adtrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"AD" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( adtrainIt != tunities.cend() );
	auto adtrain = *adtrainIt;
	auto adtrainIden = adtrain->GetId();
	SetRandomOddInfo( adtrain );

	TI.DetermineSwitchMinus( L"17", *tracker, false );
	TI.DetermineStripFree( L"AD", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	auto rtunityIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return find( tplace.cbegin(), tplace.cend(), TI.Get( L"R" ).bdg ) != tplace.cend();
	} );
	CPPUNIT_ASSERT( rtunityIt != tunities.cend() );
	auto rtunity = *rtunityIt;
	DisformTrain( rtunity->GetId() );

	TI.DetermineHeadOpen( L"Head103", *tracker, false );
	TI.DetermineStripBusy( L"AC", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	auto recovADTrainIt = find_if( tunities.cbegin(), tunities.cend(), [adtrainIden]( TrainUnityCPtr tptr ){
		return tptr->GetId() == adtrainIden;
	} );

	CPPUNIT_ASSERT( recovADTrainIt == tunities.cend() ); //поезду не может восстанавливаться в середине ЦЗ
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsSplitting::SplitTrainNearAnotherOne()
{
	TI.DetermineSwitchPlus( L"17", *tracker );
	TI.DetermineStripBusy( L"V", *tracker );
	TI.DetermineStripBusy( L"Z", *tracker );
	TI.DetermineStripBusy( L"Y", *tracker, false );
	TI.DetermineStripBusy( L"AA", *tracker, false );
	TI.DetermineStripBusy( L"AD", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();

	TI.DetermineSwitchOutOfControl( L"17", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsSplitting::OccupyNoTrainAfterADOut()
{
	TI.DetermineStripBusy( L"C", *tracker );
	auto trainPtr = SetRandomEvenInfo( TI.Get( L"C" ).bdg );
	auto tdescrPtr = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	auto trainId = trainPtr->GetId();
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	CPPUNIT_ASSERT( curtrain->GetId() == trainId && curtrain->GetDescrPtr() && *curtrain->GetDescrPtr() == *tdescrPtr );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTrainsSplitting::MultiTrainRoutesOnADWayOut()
{
	for ( auto i = 0; i < 2; ++i )
	{
		auto routeName = ( ( i == 0 ) ? L"Head400->K4" : L"Head400->M4" );
		CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"260" ) ), list<wstring>( 1, L"S4" ) ) );
		auto s4unityPtr = trainCont->GetUnity( TI.Get( L"S4" ).bdg );
		CPPUNIT_ASSERT( s4unityPtr );
		auto initTrainId = s4unityPtr->GetId();

		//имитируем задание маршрута на Head400->AK4:
		TI.DetermineSwitchMinus( L"43", *tracker );
		TI.DetermineSwitchMinus( L"44", *tracker );
		TI.DetermineSwitchMinus( L"45", *tracker );
		TI.DetermineSwitchPlus( L"46", *tracker );
		TI.DetermineRouteSet( routeName, *tracker );
		TI.DetermineStripBusy( L"F4", *tracker );

		auto tunities = trainCont->GetUnities();
		CPPUNIT_ASSERT( tunities.size() == 1 && tunities.back()->GetId() == initTrainId && tunities.back()->IsTrain() );
		tracker->Reset();
		TI.Reset();
	}

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}