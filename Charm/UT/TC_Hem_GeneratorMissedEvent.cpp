#include "stdafx.h"

#include "TC_Hem_GeneratorMissedEvent.h"
#include "UtHelpfulDateTime.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/GeneratorMissedEvent.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GeneratorMissedEvent );

const BadgeE c_stationA(L"Bdg_A", EsrKit(1000));
const BadgeE c_stationB(L"Bdg_B", EsrKit(1100));
const BadgeE c_stationC(L"Bdg_C", EsrKit(1000,1100));
const BadgeE c_stationD(L"Bdg_D", EsrKit(2000,2100));

void TC_GeneratorMissedEvent::testDeparture()
{
	time_t curtime = timeForDate(2023, 01, 01);
	time_t currtime_10 = curtime + 10;
	time_t currtime_20 = currtime_10 + 10;

	{
		// корректная последовательность станция->станция, генерация отправления с предыдущей станции
		auto spotOne =	std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationA, currtime_10 );
		auto spotTwo = std::make_shared<SpotEvent>( HCode::STATION_ENTRY, c_stationB, currtime_20 );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		auto departureEvent = gme.Perform();
		CPPUNIT_ASSERT (  departureEvent );
		CPPUNIT_ASSERT ( departureEvent->GetCode()==HCode::DEPARTURE && departureEvent->GetBadge()==spotOne->GetBadge() && departureEvent->GetTime()==spotOne->GetTime() );
	}

	{
		// корректная последовательность станция->перегон, генерация отправления с предыдущей станции
		SpotDetailsPtr spd = std::make_shared<SpotDetails>( ParkWayKit(1), std::make_pair(PicketingInfo(),PicketingInfo()) );
		auto spotOne =	std::make_shared<SpotEvent>( HCode::FORM, c_stationA, currtime_10, spd );
		auto spotTwo = std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, currtime_20 );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		auto departureEvent = gme.Perform();
		CPPUNIT_ASSERT (  departureEvent );
		CPPUNIT_ASSERT ( departureEvent->GetCode()==HCode::DEPARTURE && departureEvent->GetBadge()==spotOne->GetBadge() && departureEvent->GetTime()==spotOne->GetTime() );
	}

}

void TC_GeneratorMissedEvent::testArrival()
{
	time_t curtime = timeForDate(2023, 01, 01);
	time_t currtime_10 = curtime + 10;
	time_t currtime_20 = currtime_10 + 10;
	SpotDetailsPtr spd = std::make_shared<SpotDetails>( ParkWayKit(1), std::make_pair(PicketingInfo(),PicketingInfo()) );


	{
		// корректная последовательность станция->станция, генерация прибытия на следующую станцию
		auto spotOne =	std::make_shared<SpotEvent>( HCode::DEPARTURE, c_stationA, currtime_10 );
		auto spotTwo = std::make_shared<SpotEvent>( HCode::DISFORM, c_stationB, currtime_20, spd );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		auto arrivalEvent = gme.Perform();
		CPPUNIT_ASSERT (  arrivalEvent );
		CPPUNIT_ASSERT ( arrivalEvent->GetCode()==HCode::ARRIVAL && arrivalEvent->GetBadge()==spotTwo->GetBadge() && arrivalEvent->GetTime()==spotTwo->GetTime() );
	}

	{
		// корректная последовательность перегон->станция, генерация прибытия на следующую станцию
		auto spotOne =	std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, currtime_10 );
		auto spotTwo = std::make_shared<SpotEvent>( HCode::STATION_EXIT, c_stationA, currtime_20, spd );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		auto transitionEvent = gme.Perform();
		CPPUNIT_ASSERT (  transitionEvent );
		CPPUNIT_ASSERT ( transitionEvent->GetCode()==HCode::TRANSITION && transitionEvent->GetBadge()==spotTwo->GetBadge() && transitionEvent->GetTime()==spotTwo->GetTime() );
	}

}

void TC_GeneratorMissedEvent::testNoGenerationForCorrect()
{
	time_t curtime = timeForDate(2023, 01, 01);
	time_t currtime_10 = curtime + 10;
	time_t currtime_20 = currtime_10 + 10;

	auto spotOne = std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationA, currtime_10 );

	{
		//нет предыдущего события
		CGeneratorMissedEvent gme(nullptr, spotOne);
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

	{
		//нет следующего события
		CGeneratorMissedEvent gme(spotOne, nullptr);
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

	{
		// события на одной станции, генерация не требуется
		auto spotTwo =	std::make_shared<SpotEvent>( HCode::DEPARTURE, c_stationA, currtime_20 );
		CGeneratorMissedEvent gme(spotOne, spotTwo);
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

	{
		// события на одном перегоне, генерация не требуется
		auto spotTwo = std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, currtime_10 );
		auto spotThree =	std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, currtime_20 );
		CGeneratorMissedEvent gme(spotTwo, spotThree);
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}
 
	{
		// корректная последовательность на разных станциях, генерация не требуется
		auto spotTwo = std::make_shared<SpotEvent>( HCode::TRANSITION, c_stationB, currtime_20 );
		CheckAllCorrectEventOnPrevStation(currtime_10, c_stationA, spotTwo);
	}


	{
		// корректная последовательность станция->перегон, генерация не требуется
		auto spotTwo =	std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, currtime_20 );
		CheckAllCorrectEventOnPrevStation(currtime_10, c_stationA, spotTwo);
	}

	{
		// корректная последовательность перегон->станция, генерация не требуется
		auto spotTwo = std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationA, currtime_20 );
		CheckAllCorrectEventOnPrevSpan(currtime_10, c_stationC, spotTwo);
	}

}

void TC_GeneratorMissedEvent::CheckAllCorrectEventOnPrevStation(time_t timePrev, BadgeE bdgPrev, std::shared_ptr<SpotEvent> spotNext)
{
	HCode codeArr[] = {HCode::TRANSITION, HCode::DEPARTURE, HCode::STATION_EXIT, HCode::DEATH};
	for ( auto i=0; i!=size_array(codeArr); i++ )
		CheckNoGeneration(codeArr[i], bdgPrev, timePrev, spotNext);
}

void TC_GeneratorMissedEvent::CheckAllCorrectEventOnPrevSpan(time_t timePrev, BadgeE bdgPrev, std::shared_ptr<SpotEvent> spotNext)
{
	HCode codeArr[] = {HCode::SPAN_MOVE, HCode::SPAN_STOPPING_BEGIN, HCode::SPAN_STOPPING_END, HCode::WRONG_SPANWAY, HCode::TOKEN_SPANWAY, HCode::POCKET_ENTRY, HCode::POCKET_EXIT };
	for ( auto ca : codeArr )
		CheckNoGeneration( ca, bdgPrev, timePrev, spotNext);
}

void TC_GeneratorMissedEvent::CheckNoGeneration(HCode codeEvent, BadgeE bdgPrev, time_t timePrev, std::shared_ptr<SpotEvent> spotNext)
{
	auto spotPrev =	std::make_shared<SpotEvent>( codeEvent, bdgPrev, timePrev );
	CGeneratorMissedEvent gme(spotPrev, spotNext);
	CPPUNIT_ASSERT ( gme.Perform() == nullptr );
}

void TC_GeneratorMissedEvent::testNoGenerationForError()
{
	time_t curtime = timeForDate(2023, 01, 01);
	time_t currtime_10 = curtime + 10;
	time_t currtime_20 = currtime_10 + 10;


	{
		// некорректная последовательность перегон->перегон, генерация невозможна
		auto spotOne =	std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, currtime_10 );
		auto spotTwo = std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationD, currtime_20 );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

	{
		// некорректная последовательность станция->перегон - не соседний перегон, генерация невозможна
		auto spotOne =	std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationA, currtime_10 );
		auto spotTwo = std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationD, currtime_20 );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

	{
		// некорректная последовательность станция->перегон - нарушение хронологии, генерация невозможна
		auto spotOne =	std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationA, currtime_20 );
		auto spotTwo = std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationD, currtime_10 );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

	{
		// некорректная последовательность перегон->станция - не соседняя станция, генерация невозможна
		auto spotOne = std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationD, currtime_20 );
		auto spotTwo =	std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationA, currtime_10 );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

	{
		// некорректная последовательность перегон->станция - нарушение хронологии, генерация невозможна
		auto spotOne = std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationD, currtime_20 );
		auto spotTwo =	std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationA, currtime_10 );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

	{
		// некорректная последовательность станция->станция - нарушение хронологии, генерация невозможна
		auto spotOne =	std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationA, currtime_20 );
		auto spotTwo = std::make_shared<SpotEvent>( HCode::STATION_EXIT, c_stationB, currtime_10 );
		CGeneratorMissedEvent gme(spotOne, spotTwo );
		CPPUNIT_ASSERT ( gme.Perform() == nullptr );
	}

}

void TC_GeneratorMissedEvent::test_7081_6()
{
	time_t curtime = timeForDate(2023, 01, 01);
	time_t currtime_10 = curtime + 10;
	time_t currtime_20 = currtime_10 + 10;
	SpotDetailsPtr spd = std::make_shared<SpotDetails>( ParkWayKit(1), std::make_pair(PicketingInfo(),PicketingInfo()) );


	// станция->станция, генерация отправления
	auto spotOne =	std::make_shared<SpotEvent>( HCode::FORM, c_stationA, currtime_10, spd );
	auto spotTwo = std::make_shared<SpotEvent>( HCode::STATION_EXIT, c_stationB, currtime_20 );
	CGeneratorMissedEvent gme(spotOne, spotTwo );
	auto event = gme.Perform();
	CPPUNIT_ASSERT ( event );
	CPPUNIT_ASSERT ( event->GetCode()==HCode::DEPARTURE && event->GetBadge()==spotOne->GetBadge() && event->GetTime()==spotOne->GetTime() );

}
