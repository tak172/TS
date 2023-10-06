#include "stdafx.h"
#include "TC_TrackerDynamics.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainUnity.h"
#include "../Guess/FieldGraph.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerDynamics );

void TC_TrackerDynamics::setUp()
{
	TI.Reset();
}

void TC_TrackerDynamics::StationEntry()
{
	//ПЕ въезжает на станцию
	TI.DetermineSwitchPlus( L"32", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"D3", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr1 = *trainList.begin();

	//ПЕ покидает перегон
	TI.DetermineStripFree( L"F3", *tracker );
	TI.DetermineStripFree( L"X3", *tracker, false );
	TI.DetermineStripFree( L"Y3", *tracker, false );
	TI.FlushData( *tracker, false );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr trainPtr2 = *trainList.begin();

	//ПЕ сохраняется, исходя из направления движения
	CPPUNIT_ASSERT( trainPtr1->GetId() == trainPtr2->GetId() );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::HistoryDepth()
{
	//ПЕ въезжает на станцию
	TI.DetermineSwitchPlus( L"32", *tracker );
	TI.DetermineSwitchPlus( L"31", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"D3", *tracker );
	TI.DetermineStripBusy( L"C3", *tracker );
	TI.DetermineStripBusy( L"A3", *tracker );
	auto trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr initTrain = *trainList.begin();
	
	//гашение середины
	TI.DetermineStripFree( L"D3", *tracker );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	for ( TrainUnityCPtr tptr : trainList )
	{
		if ( tptr->GetPlace().size() == 2 )
			CPPUNIT_ASSERT( tptr->GetId() == initTrain->GetId() );
	}

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::Declutching()
{
	//появление и движение ПЕ к п/о пути
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.DetermineStripBusy( L"G4", *tracker, false );
	TI.DetermineStripBusy( L"F4", *tracker, false );
	TI.DetermineStripBusy( L"S4", *tracker, false );
	TI.FlushData( *tracker, false );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	SetRandomOddInfo( tlist.front() );
	CPPUNIT_ASSERT( tlist.front()->IsTrain() );
	TI.DetermineStripFree( L"H4", *tracker );
	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( !tlist.empty() );
	CPPUNIT_ASSERT( tlist.front()->Head() == TI.Get( L"S4" ).bdg );

	//отцепка
	TI.DetermineStripFree( L"F4", *tracker, false );
	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.FlushData( *tracker, false );
	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	for ( auto train : tlist )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		if ( tbdg == TI.Get( L"S4" ).bdg )
			CPPUNIT_ASSERT( train->IsTrain() );
		else
		{
			CPPUNIT_ASSERT( tbdg == TI.Get( L"H4" ).bdg );
			CPPUNIT_ASSERT( !train->IsTrain() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::OpenWayDeclutching()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head102", *tracker );
	PullingTailToCWay();
	TI.DetermineSwitchPlus( L"13", *tracker );

	//задание маршрута
	TI.DetermineRouteSet( L"Head102->G", *tracker );

	//отъезд поезда (маневровая остается на п/о пути)
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );

	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	for ( auto train : tlist )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		if ( tbdg == TI.Get( L"G" ).bdg )
			CPPUNIT_ASSERT( train->IsTrain() );
		else
		{
			CPPUNIT_ASSERT( tbdg == TI.Get( L"C" ).bdg );
			CPPUNIT_ASSERT( !train->IsTrain() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::OpenWayDeclutchingWithDelayedSema()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head102", *tracker );
	PullingTailToCWay();
	TI.DetermineSwitchPlus( L"13", *tracker );

	//задание маршрута
	TI.DetermineRouteSet( L"Head102->G", *tracker );

	//отъезд поезда
	TI.DetermineStripBusy( L"F", *tracker );


	//вторичное задание маршрута (эффект аналогичен задержке светофора на закрытие)
	TI.DetermineRouteSet( L"Head102->G", *tracker );

	//продолжение отъезда поезда
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	for ( auto train : tlist )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		if ( tbdg == TI.Get( L"G" ).bdg )
			CPPUNIT_ASSERT( train->IsTrain() );
		else
		{
			CPPUNIT_ASSERT( tbdg == TI.Get( L"C" ).bdg );
			CPPUNIT_ASSERT( !train->IsTrain() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::OpenWayExitAndDeclutchingCloseSema()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head102", *tracker );
	PullingTailToCWay();
	TI.DetermineSwitchPlus( L"13", *tracker );

	//задание маршрута
	TI.DetermineRouteSet( L"Head102->G", *tracker );

	//строго: сначала открытие светофора, потом отъезд поезда

	//отъезд поезда с одновременным закрытием светфора -> маневровая остается на п/о пути
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineHeadClose( L"Head102", *tracker, false );
	TI.FlushData( *tracker, false );

	TI.DetermineStripBusy( L"G", *tracker );

	//продолжение движения поезда
	TI.DetermineStripFree( L"F", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		if ( tbdg == TI.Get( L"G" ).bdg )
			CPPUNIT_ASSERT( tunity->IsTrain() );
		else
		{
			CPPUNIT_ASSERT( tbdg == TI.Get( L"C" ).bdg );
			CPPUNIT_ASSERT( !tunity->IsTrain() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::CloseWayDeclutching()
{
	//въезд на п/о путь (светофор закрыт)
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.front();
	auto trainId = train->GetId();
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	SetRandomEvenInfo( train );

	//отъезд локомотива (поезд остается на п/о пути)
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"F", *tracker );

	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	for ( auto train : tlist )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();

		if ( tbdg == TI.Get( L"C" ).bdg )
		{
			CPPUNIT_ASSERT( train->IsTrain() );
			CPPUNIT_ASSERT( trainId == train->GetId() );
		}
		else
		{
			CPPUNIT_ASSERT( !train->IsTrain() );
			CPPUNIT_ASSERT( tbdg == TI.Get( L"G" ).bdg );
			CPPUNIT_ASSERT( trainId != train->GetId() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::OpenShuntingDeclutching()
{
	TI.DetermineSwitchMinus( L"41", *tracker );
	TI.DetermineStripBusy( L"R4", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	SetRandomOddInfo( tunities.back() );
	auto trainIt = find_if( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		return tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( trainIt != tunities.cend() );

	TI.DetermineRouteSet( L"Head403S->A4", *tracker );
	TI.DetermineStripBusy( L"O4", *tracker );
	TI.DetermineStripBusy( L"A4", *tracker );
	TI.DetermineStripFree( L"O4", *tracker );

	tunities = trainCont->GetUnities();
	trainIt = find_if( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		return tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( trainIt != tunities.cend() );
	TrainUnityCPtr r4train = *trainIt;
	const auto & tplace = r4train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 && tplace.front() == TI.Get( L"R4" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::WOSemaWayDeclutching()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	wstring stripsArr[] = { L"W", L"Q", L"A", L"D", L"E" };
	vector <wstring> stripsSeq( begin( stripsArr ), end( stripsArr ) );
	TI.ImitateMotion( stripsSeq, *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"D", *tracker );

	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	for ( auto train : tlist )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		if ( tbdg == TI.Get( L"E" ).bdg )
			CPPUNIT_ASSERT( train->IsTrain() );
		else
		{
			CPPUNIT_ASSERT( tbdg == TI.Get( L"A" ).bdg );
			CPPUNIT_ASSERT( !train->IsTrain() );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::StationExitMissingArrdep()
{
	TI.DetermineStripBusy( L"D3", *tracker );
	TI.DetermineStripBusy( L"F3", *tracker );
	TI.DetermineStripBusy( L"X3", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.back();
	CPPUNIT_ASSERT( train->IsTrain() );
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 );
	auto teIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::SpanDeclutching()
{
	TI.DetermineRouteSet( L"Head101->A", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.back();
	SetRandomOddInfo( train );
	auto tdescrPtr = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	TrainDescr refDescr = *tdescrPtr;
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );

	//отъезд локомотива
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.FlushData( *tracker, false );
	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	for ( auto train : tlist )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		auto tdescrPtr = train->GetDescrPtr();
		if ( tbdg == TI.Get( L"H" ).bdg )
		{
			CPPUNIT_ASSERT( tdescrPtr );
			CPPUNIT_ASSERT( *tdescrPtr == refDescr );
		}
		else
		{
			CPPUNIT_ASSERT( tbdg == TI.Get( L"Q" ).bdg );
			CPPUNIT_ASSERT( !tdescrPtr );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::SpanDeclutching2()
{
	TI.DetermineStripBusy( L"F3", *tracker );
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.back();
	SetRandomOddInfo( train );
	auto tdescrPtr = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	TrainDescr refDescr( *tdescrPtr );
	TI.DetermineStripBusy( L"X3", *tracker );
	TI.DetermineStripBusy( L"Y3", *tracker );
	TI.DetermineStripFree( L"F3", *tracker );

	//отъезд локомотива
	TI.DetermineStripBusy( L"F3", *tracker, false );
	TI.DetermineStripFree( L"X3", *tracker, false );
	TI.FlushData( *tracker, false );
	tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 2 );
	for ( auto train : tlist )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		const auto & tbdg = tplace.front();
		auto tdescrPtr = train->GetDescrPtr();
		if ( tbdg == TI.Get( L"Y3" ).bdg )
		{
			CPPUNIT_ASSERT( tdescrPtr );
			CPPUNIT_ASSERT( *tdescrPtr == refDescr );
		}
		else
		{
			CPPUNIT_ASSERT( tbdg == TI.Get( L"F3" ).bdg );
			CPPUNIT_ASSERT( !tdescrPtr );
		}
	}
}

void TC_TrackerDynamics::AnotherOddnessCoSemaTransit()
{
	TI.DetermineSwitchMinus( L"11", *tracker );

	//занятие четным поездом п/о пути C
	TI.DetermineStripBusy( L"C", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto initTrain = trains.front();
	auto initId = initTrain->GetId();
	SetInfo( initTrain, RandomTrainDescr( false ) );

	//проезд через открытый нечетный светофор
	TI.DetermineRouteSet( L"Head101->A", *tracker );

	//отправление с п/о пути
	wstring stripsArr2[] = { L"C", L"B", L"A", L"Q"  };
	vector <wstring> stripsSeq2( begin( stripsArr2 ), end( stripsArr2 ) );
	TI.ImitateMotion( stripsSeq2, *tracker );

	//происходит расформирование поезда из-за проезда светофора чужой четности
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto nowTrain = trains.front();
	CPPUNIT_ASSERT( initId != nowTrain->GetId() );
	CPPUNIT_ASSERT( nowTrain->IsTrain() );
	auto tdescrPtr = nowTrain->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	CPPUNIT_ASSERT( tdescrPtr->GetNumber().getString().empty() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::AnotherOddnessCoSemaFastTransit()
{
	TI.DetermineStripBusy( L"L", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto initId = trains.front()->GetId();
	SetRandomOddInfo( trains.front() );

	TI.DetermineStripBusy( L"M", *tracker, false );
	TI.DetermineStripBusy( L"O", *tracker, false );
	TI.FlushData( *tracker, false );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	CPPUNIT_ASSERT( find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr train ){
		return train->GetPlace().size() == 1;
	} ) != trains.cend() );
	CPPUNIT_ASSERT( find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr train ){
		return train->GetPlace().size() == 2;
	} ) != trains.cend() );

	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		if ( tplace.size() == 1 )
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"O" ).bdg );
			CPPUNIT_ASSERT( !train->IsTrain() );
			CPPUNIT_ASSERT( train->GetId() != initId );
		}
		else
		{
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"L" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"M" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( train->IsTrain() && train->GetOddness() == Oddness::ODD );
			CPPUNIT_ASSERT( train->GetId() == initId );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::AnotherOddnessOppSemaTransit()
{
	TI.DetermineSwitchMinus( L"11", *tracker );

	//занятие нечетным поездом пути B
	TI.DetermineStripBusy( L"B", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto initTrain = trains.front();
	auto initId = initTrain->GetId();
	SetInfo( initTrain, RandomTrainDescr( true ) );

	//проезд через встречный нечетный светофор
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	//происходит расформирование поезда из-за проезда светофора не подходящей четности
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto nowTrain = trains.front();
	CPPUNIT_ASSERT( initId != nowTrain->GetId() );
	CPPUNIT_ASSERT( !nowTrain->IsTrain() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::ReversiveMoveAfterPartialArrive()
{
	//въезд поезда на п/о путь с хвостом на перегоне
	TI.DetermineStripBusy( L"N3", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	SetRandomEvenInfo( trains.front() );
	TI.DetermineStripBusy( L"O3", *tracker );
	auto events = tracker->GetTrainEvents();
	TI.DetermineStripBusy( L"P3", *tracker );
	events = tracker->GetTrainEvents();
	TI.DetermineStripFree( L"P3", *tracker );

	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 4 );
	auto teIt = tevents.cbegin();
	auto tevent = *teIt;
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::FORM  );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( ( *teIt++ )->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::LongTrainAtypicalStationExit()
{
	//поезд с п/о пути выходит на перегон, оставляя на п/о пути занятость
	TI.DetermineStripBusy( L"C", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.front()->GetId();
	SetRandomEvenInfo( trains.front() );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );

	TI.DetermineHeadClose( L"Head102", *tracker ); //это ключевое условие для теста
	TI.DetermineStripFree( L"F", *tracker, false );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.FlushData( *tracker, false );

	//поезд должен выехать на перегон
	trains = trainCont->GetUnities();
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		if ( tplace.size() == 1 )
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"C" ).bdg );
			CPPUNIT_ASSERT( !train->IsTrain() );
		}
		else
		{
			CPPUNIT_ASSERT( tplace.size() == 2 );
			CPPUNIT_ASSERT( train->IsTrain() );
			CPPUNIT_ASSERT( train->GetId() == trainId );
		}
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::MovingPriority()
{
	TI.DetermineSwitchPlus( L"22", *tracker );
	TI.DetermineSwitchPlus( L"23", *tracker );
	TI.DetermineSwitchPlus( L"21", *tracker );

	TI.DetermineStripBusy( L"F2", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.front()->GetId();
	SetRandomOddInfo( trains.front() );
	TI.DetermineStripBusy( L"E2", *tracker );
	TI.DetermineStripBusy( L"D2", *tracker );
	TI.DetermineStripBusy( L"M2", *tracker );
	TI.DetermineStripBusy( L"O2", *tracker );
	TI.DetermineStripBusy( L"P2", *tracker );
	TI.DetermineStripBusy( L"R2", *tracker );
	TI.DetermineStripFree( L"F2", *tracker );

	//"разрыв" поезда
	TI.DetermineStripFree( L"O2", *tracker, false );
	TI.DetermineStripFree( L"M2", *tracker, false );
	TI.FlushData( *tracker, false );

	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	auto trainIt = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr tunity ){
		return tunity->IsTrain();
	} );
	auto tunityIt = find_if( trains.cbegin(), trains.cend(), []( TrainUnityCPtr tunity ){
		return !tunity->IsTrain();
	} );
	CPPUNIT_ASSERT( trainIt != trains.cend() && tunityIt != trains.cend() && trainIt != tunityIt );
	const auto & tplace = ( *trainIt )->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"P2" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"R2" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::DeclutchingAfterTurn()
{
	//въезд поезда на п/о путь (хвост на перегоне)
	TI.DetermineHeadClose( L"Head101", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.front()->GetId();
	SetRandomEvenInfo( trains.front() );

	//здесь разворот должен быть запрещен
	TI.DetermineStripBusy( L"W", *tracker );

	//должна создаваться новая ПЕ
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	auto tunityIt = find_if( trains.cbegin(), trains.cend(), [trainId]( TrainUnityCPtr tptr ){
		return tptr->GetId() != trainId;
	} );
	CPPUNIT_ASSERT( tunityIt != trains.cend() );
	auto tunity = *tunityIt;
	const auto & tplace = tunity->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 && tplace.front() == TI.Get( L"W" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::InstantDoubleTrainsSpanMove()
{
	//постановка поезда 1 на W, поезда 2 на Q
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	
	SetRandomOddInfo( TI.Get( L"A" ).bdg );
	SetRandomOddInfo( TI.Get( L"W" ).bdg );

	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false ); //поезд 2 на Q
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );

	//перемещение сразу обоих поездов через участок
	TI.DetermineStripBusy( L"K", *tracker, false );
	TI.DetermineStripBusy( L"H", *tracker, false );
	TI.DetermineStripFree( L"W", *tracker, false );
	TI.DetermineStripFree( L"Q", *tracker, false );
	TI.FlushData( *tracker, false );

	//запрещено сразу для двух поездов проезжать один участок в течении одного момента времени (одной секунды)
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto restrain = trains.front();
	const auto & tplace = restrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"K" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"H" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::InstantMixedUnitiesTightMove()
{
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.front()->GetId();
	SetRandomOddInfo( trains.front() );
	TI.DetermineStripBusy( L"E", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	TI.DetermineStripFree( L"E", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	TI.DetermineStripFree( L"G", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( trains.front()->GetId() == trainId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::TrainJumpToNoTrain()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.front()->GetId();
	SetRandomOddInfo( trains.front() );
	TI.DetermineStripBusy( L"E", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripFree( L"E", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineStripFree( L"G", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainIt = find_if( trains.cbegin(), trains.cend(), [trainId]( TrainUnityCPtr tptr ){
		return tptr->GetId() == trainId;
	} );
	CPPUNIT_ASSERT( trainIt != trains.cend() );
	auto curtrain = *trainIt;
	CPPUNIT_ASSERT( curtrain->IsTrain() );
	const auto & tplace = curtrain->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"A" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::TrainJumpToNoTrainOverBlockedPart()
{
	//поезд
	TI.DetermineStripBusy( L"L5", *tracker );
	TI.DetermineStripBusy( L"M5", *tracker );
	TI.DetermineStripFree( L"L5", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainId = trains.front()->GetId();

	//маневровая
	TI.DetermineStripBusy( L"P5", *tracker, false );
	TI.DetermineStripBusy( L"Q5", *tracker, false );
	TI.FlushData( *tracker, false );
	trains = trainCont->GetUnities();

	//прыжок через блок-участок
	TI.DetermineStripFree( L"M5", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto train = trains.back();
	CPPUNIT_ASSERT( train->GetId() == trainId );
	const auto & tplace = train->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"P5" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"Q5" ).bdg ) != tplace.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::TrainJumpBidToNoTrainOverBlockedPart()
{
	//маневровая
	TI.DetermineStripBusy( L"S5", *tracker );
	TI.FlushData( *tracker, false );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto shuntId = tunities.front()->GetId();

	//поезд
	TI.DetermineStripBusy( L"N5", *tracker );
	TI.DetermineStripBusy( L"M5", *tracker );
	TI.DetermineStripFree( L"N5", *tracker );

	//прыжок через блок-участок, состоящий из более чем двух участков, невозможен
	TI.DetermineStripFree( L"M5", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	CPPUNIT_ASSERT( tunity->GetId() == shuntId );
	const auto & tplace = tunity->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 1 );
	CPPUNIT_ASSERT( tplace.back() == TI.Get( L"S5" ).bdg );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::TrainJumpToNoTrainOverSwitchSection()
{
	TI.DetermineSwitchMinus( L"24", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"H2" ) );
	TI.DetermineStripBusy( L"A2", *tracker );

	auto trainPtr = trainCont->GetUnity( TI.Get( L"H2" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId = trainPtr->GetId();
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"A2" ).bdg );
	CPPUNIT_ASSERT( notrainPtr );
	auto notrainId = notrainPtr->GetId();

	TI.DetermineStripFree( L"H2", *tracker ); //есть телепорт

	auto curtrainPtr = trainCont->GetUnity( TI.Get( L"A2" ).bdg );
	CPPUNIT_ASSERT( curtrainPtr && curtrainPtr->IsTrain() );
	auto curtrainId = curtrainPtr->GetId();
	CPPUNIT_ASSERT( curtrainId == trainId );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerDynamics::TrainJumpBidToNoTrainOverSwitchSection()
{
	TI.DetermineSwitchPlus( L"24", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"H2" ) );
	TI.DetermineStripBusy( L"A2", *tracker );

	auto trainPtr = trainCont->GetUnity( TI.Get( L"H2" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId = trainPtr->GetId();
	auto notrainPtr = trainCont->GetUnity( TI.Get( L"A2" ).bdg );
	CPPUNIT_ASSERT( notrainPtr );
	auto notrainId = notrainPtr->GetId();

	TI.DetermineStripFree( L"H2", *tracker ); //нет телепорта

	auto tunityPtr = trainCont->GetUnity( TI.Get( L"A2" ).bdg );
	CPPUNIT_ASSERT( tunityPtr && !tunityPtr->IsTrain() );
	auto tunityId = tunityPtr->GetId();
	CPPUNIT_ASSERT( tunityId == notrainId );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
}

void TC_TrackerDynamics::StatMotionVsPresence()
{
	TI.DetermineSwitchPlus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineStripBusy( L"D4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainIden = trains.back()->GetId();
	SetRandomOddInfo( trains.front() );
	TI.DetermineStripBusy( L"E4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );

	//разделение
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.FlushData( *tracker, false );
	auto restunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( restunities.size() == 2 );
	TrainUnityCPtr h4trainPtr, d4e4tunityPtr;
	for ( auto tunity : restunities  )
	{
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 )
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"H4" ).bdg );
			h4trainPtr = tunity;
		}
		else
		{
			CPPUNIT_ASSERT( tplace.size() == 2 );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D4" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"E4" ).bdg ) != tplace.cend() );
			d4e4tunityPtr = tunity;
		}
	}
	CPPUNIT_ASSERT( h4trainPtr->IsTrain() );
	CPPUNIT_ASSERT( h4trainPtr->GetId() == trainIden );
	CPPUNIT_ASSERT( !d4e4tunityPtr->IsTrain() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::SpanMotionVsArrdep()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineStripBusy( L"E", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainIden = trains.back()->GetId();
	SetRandomOddInfo( trains.front() );
	TI.DetermineStripBusy( L"A", *tracker );

	//разделение
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false );
	auto restunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( restunities.size() == 2 );
	TrainUnityCPtr qtrainPtr, edtunityPtr;
	for ( auto tunity : restunities  )
	{
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 )
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"Q" ).bdg );
			qtrainPtr = tunity;
		}
		else
		{
			CPPUNIT_ASSERT( tplace.size() == 2 );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"E" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"D" ).bdg ) != tplace.cend() );
			edtunityPtr = tunity;
		}
	}
	CPPUNIT_ASSERT( qtrainPtr->IsTrain() );
	CPPUNIT_ASSERT( edtunityPtr->IsTrain() && edtunityPtr->GetId() == trainIden );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::StatMotionVsArrdep()
{
	TI.DetermineRouteSet( L"Head400->K4", *tracker );
	TI.DetermineSwitchMinus( L"43", *tracker );
	TI.DetermineSwitchMinus( L"44", *tracker );
	TI.DetermineStripBusy( L"S4", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto trainIden = trains.back()->GetId();
	SetRandomEvenInfo( trains.front() );
	TI.DetermineStripBusy( L"F4", *tracker );
	TI.DetermineStripBusy( L"G4", *tracker );

	//разделение
	TI.DetermineStripBusy( L"H4", *tracker, false );
	TI.DetermineStripFree( L"G4", *tracker, false );
	TI.FlushData( *tracker, false );
	auto restunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( restunities.size() == 2 );
	TrainUnityCPtr h4trainPtr, s4f4tunityPtr;
	for ( auto tunity : restunities  )
	{
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 )
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"H4" ).bdg );
			h4trainPtr = tunity;
		}
		else
		{
			CPPUNIT_ASSERT( tplace.size() == 2 );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"S4" ).bdg ) != tplace.cend() );
			CPPUNIT_ASSERT( find( tplace.cbegin(), tplace.cend(), TI.Get( L"F4" ).bdg ) != tplace.cend() );
			s4f4tunityPtr = tunity;
		}
	}
	CPPUNIT_ASSERT( h4trainPtr->IsTrain() );
	CPPUNIT_ASSERT( h4trainPtr->GetId() == trainIden );
	CPPUNIT_ASSERT( !s4f4tunityPtr->IsTrain() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::HeadMovingsOnSpanOnlyForward()
{
	//проезд по перегону при непоследовательно зажигаемых участках
	TI.DetermineStripBusy( L"A", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.back();
	auto trainId = train->GetId();
	TI.DetermineStripBusy( L"Q", *tracker );
	CPPUNIT_ASSERT( train->Head() == TI.Get( L"Q" ).bdg );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	auto tunities2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities2.size() == 1 );
	auto train2 = tunities2.back();
	CPPUNIT_ASSERT( train2->GetId() == trainId );
	CPPUNIT_ASSERT( train2->Head() == TI.Get( L"H" ).bdg );
	TI.DetermineStripBusy( L"W", *tracker );
	CPPUNIT_ASSERT( train2->Head() == TI.Get( L"H" ).bdg ); //голова не меняется
	TI.DetermineStripBusy( L"K", *tracker );
	CPPUNIT_ASSERT( train2->Head() == TI.Get( L"K" ).bdg ); //едем вперед
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"H", *tracker );

	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::SpanMotionVsKeepingLastPlace()
{
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripBusy( L"K", *tracker );

	list <TrainUnityCPtr> tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto farTrainId = 0, nearTrainId = 0;
	for( auto tunity : tunities )
	{
		SetRandomEvenInfo( tunity );
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 )
			farTrainId = tunity->GetId();
		else
			nearTrainId = tunity->GetId();
	}

	TI.DetermineStripBusy( L"K", *tracker, false );
	TI.DetermineStripFree( L"A", *tracker, false );
	TI.FlushData( *tracker, false ); //ничего не должно измениться

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	for( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		if ( tplace.size() == 1 )
			CPPUNIT_ASSERT( farTrainId == tunity->GetId() );
		else
			CPPUNIT_ASSERT( nearTrainId == tunity->GetId() );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::ChoosingTrainByMoveDirection()
{
	TI.DetermineStripBusy( L"Y", *tracker );
	TI.DetermineStripBusy( L"V", *tracker );
	TI.DetermineStripBusy( L"R", *tracker );
	TI.DetermineStripFree( L"Y", *tracker );
	TI.DetermineStripFree( L"V", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto rTrainId = tunities.back()->GetId();

	TI.DetermineSwitchMinus( L"17", *tracker );
	TI.DetermineStripBusy( L"AB", *tracker );
	TI.DetermineStripBusy( L"AA", *tracker );
	TI.DetermineStripBusy( L"Z", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto zTrainIt = find_if( tunities.cbegin(), tunities.cend(), []( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 3;
	} );
	CPPUNIT_ASSERT( zTrainIt != tunities.cend() );
	auto zTrainPtr = *zTrainIt;
	SetRandomOddInfo( zTrainPtr );
	auto ztrainId = zTrainPtr->GetId();

	TI.DetermineStripBusy( L"V", *tracker, false );
	TI.DetermineStripBusy( L"Y", *tracker, false );
	TI.FlushData( *tracker, false );

	tunities = trainCont->GetUnities();
	auto shortTrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 1 && tplace.back() == TI.Get( L"R" ).bdg;
	} );
	CPPUNIT_ASSERT( shortTrainIt != tunities.cend() );
	auto shortTrainPtr = *shortTrainIt;
	CPPUNIT_ASSERT( shortTrainPtr->GetId() == rTrainId );
	auto longTrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 5;
	} );
	CPPUNIT_ASSERT( longTrainIt != tunities.cend() );
	auto longTrainPtr = *longTrainIt;
	CPPUNIT_ASSERT( longTrainPtr->GetId() == ztrainId );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::TwoSideTrainObstacles()
{
	TI.DetermineStripBusy( L"LP_LT_1C", *tracker );
	TI.DetermineStripBusy( L"LP_NGP", *tracker );
	auto oddTrain = SetInfo( TI.Get( L"LP_NGP" ).bdg, TrainDescr( L"107" ) );
	CPPUNIT_ASSERT( oddTrain && oddTrain->IsTrain() );
	auto oddTrainId = oddTrain->GetId();
	auto oddTrainDescr = *oddTrain->GetDescrPtr();
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"360" ) ), list<wstring>( 1, L"LP_NP" ) );
	auto evenTrain = trainCont->GetUnity( TI.Get( L"LP_NP" ).bdg );
	CPPUNIT_ASSERT( evenTrain && evenTrain->IsTrain() );
	auto evenTrainId = evenTrain->GetId();
	auto evenTrainDescr = *evenTrain->GetDescrPtr();

	//отъезд маневровой
	TI.DetermineStripBusy( L"LP_9-13SP:13+,9+", *tracker );
	auto tunity = trainCont->GetUnity( TI.Get( L"LP_9-13SP:13+,9+" ).bdg );
	auto shuntUnityId = tunity->GetId();

	//здесь четный поезд должен исчезнуть, т.к. не может поехать ни через чужой светофор, ни навстречу другому поезду на перегон
	TI.DetermineStripFree( L"LP_NP", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto nowOddTrain = trainCont->GetUnity( oddTrainId );
	CPPUNIT_ASSERT( nowOddTrain && nowOddTrain->IsTrain() );
	const auto & oddTrainPlace = nowOddTrain->GetPlace();
	CPPUNIT_ASSERT( *nowOddTrain->GetDescrPtr() == oddTrainDescr );
	CPPUNIT_ASSERT( oddTrainPlace.size() == 2 );
	CPPUNIT_ASSERT( nowOddTrain->Have( TI.Get( L"LP_NGP" ).bdg ) );
	CPPUNIT_ASSERT( nowOddTrain->Have( TI.Get( L"LP_LT_1C" ).bdg ) );
	auto nowShuntTrain = trainCont->GetUnity( shuntUnityId );
	CPPUNIT_ASSERT( nowShuntTrain );
	CPPUNIT_ASSERT( nowShuntTrain->GetPlace().size() == 1 );
	CPPUNIT_ASSERT( nowShuntTrain->Have( TI.Get( L"LP_9-13SP:13+,9+" ).bdg ) );

	TI.DetermineStripBusy( L"LP_NP", *tracker ); //поезд не восстанавливается
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( none_of( tunities.cbegin(), tunities.cend(), [evenTrainId]( TrainUnityCPtr tptr ){
		return tptr->GetId() == evenTrainId;
	} ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::FormAndDepartureFromObscurity()
{
	const auto & fldgraph = TI.TField()->GetComponent<TrainField::FieldGraph>();
	CPPUNIT_ASSERT( fldgraph.IsTerminating( TI.Get( L"AI" ).bdg ) );
	TI.DetermineRouteSet( L"Head107->AM", *tracker );
	TI.DetermineStripBusy( L"AI", *tracker, false );
	TI.DetermineStripBusy( L"AK", *tracker, false );
	TI.DetermineStripBusy( L"AL", *tracker, false );
	TI.FlushData( *tracker, false );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"AI" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId = trainPtr->GetId();
	auto tdescrPtr = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	TI.DetermineHeadClose( L"Head107", *tracker );
	TI.DetermineStripBusy( L"AL", *tracker );
	TI.DetermineStripBusy( L"AM", *tracker );
	TI.DetermineStripBusy( L"AN", *tracker );
	TI.DetermineStripFree( L"AK", *tracker );

	auto tunities = trainCont->GetUnities();
	auto trainPtr2 = trainCont->GetUnity( TI.Get( L"AN" ).bdg );
	CPPUNIT_ASSERT( trainPtr2 );
	CPPUNIT_ASSERT( trainPtr2->GetId() == trainId && trainPtr2->GetDescrPtr() && *trainPtr2->GetDescrPtr() == *tdescrPtr );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::TailBusyOnSpan()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2515" ) ), list<wstring>( 1, L"X" ) );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"X", *tracker );
	auto initTrainPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( initTrainPtr );
	auto initTrainId = initTrainPtr->GetId();

	TI.DetermineStripBusy( L"X", *tracker ); //занятость в хвосте - это маневровая

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto xunityPtr = trainCont->GetUnity( TI.Get( L"X" ).bdg );
	CPPUNIT_ASSERT( xunityPtr && !xunityPtr->IsTrain() );
	auto gunityPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( gunityPtr && gunityPtr->IsTrain() && gunityPtr->GetId() == initTrainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::TailBusyOnSpan2()
{
	TI.DetermineSwitchMinus( L"13", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2515" ) ), list<wstring>( 1, L"X" ) );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"X", *tracker );
	auto initTrainPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( initTrainPtr );
	auto initTrainId = initTrainPtr->GetId();

	TI.DetermineStripBusy( L"X", *tracker ); //занятость в хвосте - это маневровая..
	TI.DetermineStripBusy( L"I", *tracker ); //..которая становится поездом
	TI.DetermineStripBusy( L"E", *tracker ); //наш поезд едет дальше

	TI.DetermineStripFree( L"X", *tracker, false );
	TI.DetermineStripFree( L"I", *tracker, false );
	TI.FlushData( *tracker, false ); //гашение перегона не приводит к сохранению поезда

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto gunityPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( gunityPtr && gunityPtr->IsTrain() );
	const auto & tplace = gunityPtr->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 && gunityPtr->Have( TI.Get( L"G" ).bdg ) && gunityPtr->Have( TI.Get( L"E" ).bdg ) );
	CPPUNIT_ASSERT( gunityPtr->GetId() == initTrainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::TailBusyOnSpan3()
{
	TI.DetermineSwitchMinus( L"13", *tracker );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"X" ) );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"X", *tracker );
	auto initTrainPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( initTrainPtr );
	auto initTrainId = initTrainPtr->GetId();

	TI.DetermineStripBusy( L"I", *tracker ); //занятость за хвостом - это маневровая
	TI.DetermineStripBusy( L"E", *tracker ); //поезд отъезжает от перегона
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker ); //маневровая становится поездом и подъезжает к станции
	TI.DetermineStripFree( L"I", *tracker );
	TI.DetermineStripBusy( L"G", *tracker, false ); //это наш исходный поезд..
	TI.DetermineStripFree( L"E", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"G", *tracker ); //..а новоявленная маневровая должна умереть

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto gunityPtr = trainCont->GetUnity( TI.Get( L"G" ).bdg );
	CPPUNIT_ASSERT( gunityPtr && gunityPtr->IsTrain() );
	const auto & tplace = gunityPtr->GetPlace();
	CPPUNIT_ASSERT( tplace.size() == 2 && gunityPtr->Have( TI.Get( L"G" ).bdg ) && gunityPtr->Have( TI.Get( L"E" ).bdg ) );
	CPPUNIT_ASSERT( gunityPtr->GetId() == initTrainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerDynamics::GoOutFromStationAfterRecover()
{
	TI.DetermineSwitchPlus( L"13", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"5566" ) ), list<wstring>( 1, L"C" ) ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	auto initIden = tunity->GetId();

	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"G", *tracker, false );
	TI.DetermineStripBusy( L"X", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities2 = trainCont->GetUnities();
	auto curtrain = tunities2.back();
	CPPUNIT_ASSERT( curtrain && curtrain->GetId() == initIden );
	auto tdescrPtr = curtrain->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"5566" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}