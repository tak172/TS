#include "stdafx.h"
#include "TC_AutoChartHappen.h"
#include "../UT/AutoChartTest.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/RegulatoryLayer.h"

using namespace std;
using namespace HemHelpful;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AutoChartHappen );

TC_AutoChartHappen::TC_AutoChartHappen() : TI( *TrackerInfrastructure::instance() )
{
	autoChart.reset( new AutoChartTest() );
}

TC_AutoChartHappen::~TC_AutoChartHappen()
{
	HemEventRegistry::Shutdowner();
    TrackerInfrastructure::Shutdowner();
}

void TC_AutoChartHappen::StationEntry()
{
	time_t curtime = time( NULL );
	time_t begin_time = curtime;
	//имитируем вход ПЕ на станцию (путь A)
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"W").bdg, curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"Q").bdg, curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::ARRIVAL, TI.Get(L"C").bdg, ++curtime ) ) };
	CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());

    TrioSpot trio = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trio.pre());
    CPPUNIT_ASSERT(!trio.post());
    CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

	auto eventLists = autoChart->GetPaths_OnlyEvents( begin_time, begin_time );
	CPPUNIT_ASSERT( eventLists.size() == 1 );
	const auto& EList = eventLists.front();
	CPPUNIT_ASSERT( FindEventInfo( EList, make_pair( HCode::SPAN_MOVE, TI.Get(L"Q").bdg ) ) );
	CPPUNIT_ASSERT( FindEventInfo( EList, make_pair( HCode::ARRIVAL, TI.Get(L"C").bdg ) ) );
}

void TC_AutoChartHappen::StationExit()
{
	time_t curtime = time( NULL );
	time_t begin_time = curtime;

	//имитируем выход ПЕ на перегон (путь X)
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"E").bdg, curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"X").bdg, curtime ) ) };
	CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));

    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    TrioSpot trio = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trio.pre());
    CPPUNIT_ASSERT(!trio.post());
    CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

	auto eventLists = autoChart->GetPaths_OnlyEvents( begin_time, begin_time );
	CPPUNIT_ASSERT( eventLists.size() == 1 );
	const auto& EList = eventLists.front();
	CPPUNIT_ASSERT( FindEventInfo( EList, make_pair( HCode::SPAN_MOVE, TI.Get(L"X").bdg ) ) );
}

void TC_AutoChartHappen::SpanMove()
{
	time_t curtime = time( NULL );
	time_t begin_time = curtime;
	//имитируем вход ПЕ на станцию (путь A)
	SpotEventPtr events_array[] = {
        SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"Q").bdg, curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::ARRIVAL, TI.Get(L"D").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::DEPARTURE, TI.Get(L"E").bdg, ++curtime ) ) };
    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
	
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    TrioSpot trio = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trio.pre());
    CPPUNIT_ASSERT(!trio.post());
    CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

	auto eventLists = autoChart->GetPaths_OnlyEvents( begin_time, begin_time );
	CPPUNIT_ASSERT( eventLists.size() == 1 );
	const auto& EList = eventLists.front();
	CPPUNIT_ASSERT( FindEventInfo( EList, make_pair( HCode::FORM, TI.Get(L"Q").bdg ) ) );
	CPPUNIT_ASSERT( FindEventInfo( EList, make_pair( HCode::ARRIVAL, TI.Get(L"D").bdg ) ) );
	CPPUNIT_ASSERT( FindEventInfo( EList, make_pair( HCode::DEPARTURE, TI.Get(L"E").bdg ) ) );
}

void TC_AutoChartHappen::StartValidation()
{
	time_t begin_time = time( NULL );
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"C").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::DISFORM, TI.Get(L"C").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::DEATH, TI.Get(L"C").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::ARRIVAL, TI.Get(L"C").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::DEPARTURE, TI.Get(L"C").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::TRANSITION, TI.Get(L"C").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"X").bdg, begin_time ) ),
		//SpotEventPtr( new SpotEvent( HCode::INFO_CHANGING, TI.Get(L"C").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::ENTRANCE_STOPPING, TI.Get(L"Q").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::WRONG_SPANWAY, TI.Get(L"X").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::POCKET_ENTRY, TI.Get(L"PK3").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::POCKET_EXIT, TI.Get(L"PK7").bdg, begin_time ) )
	};
	for ( unsigned int k = 0; k < size_array( events_array ); ++k )
	{
        SpotEventPtr my_events_array[] = { events_array[k] };
        // Идентификаторы поездов меняем с каждой итерацией
        bool const pathCreated = CreatePath(*autoChart, my_events_array, k + 1);

		auto evcode = events_array[k]->GetCode();
		if ( evcode == HCode::DEATH || evcode == HCode::DISFORM || evcode == HCode::INFO_CHANGING )
		{
            //создание нити запрещено
            CPPUNIT_ASSERT_EQUAL(size_t(0), GetHappenTrioCount());
            CPPUNIT_ASSERT( !pathCreated );
            // AutoChart не отвергает событий, а выбрасывает исключение
			//CPPUNIT_ASSERT( forbiddenEvents.size() == 1 && ( *forbiddenEvents.begin() )->GetCode() == evcode );
			auto eventLists = autoChart->GetPaths_OnlyEvents( begin_time, begin_time );
			CPPUNIT_ASSERT( eventLists.empty() );
		}
        else
		{
			//создание нити разрешено
			CPPUNIT_ASSERT( pathCreated );

            CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
            TrioSpot trio = GetHappenTrio(0);
            CPPUNIT_ASSERT(!trio.pre());
            CPPUNIT_ASSERT(!trio.post());
            CPPUNIT_ASSERT_EQUAL(size_array(my_events_array), trio.body().size());

            auto eventLists = autoChart->GetPaths_OnlyEvents( begin_time, begin_time );
			CPPUNIT_ASSERT_EQUAL( (size_t)1, eventLists.size() );
		}
		autoChart.reset( new AutoChartTest() );
	}
}

void TC_AutoChartHappen::PocketTransition()
{
	time_t curtime = time( NULL );
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"PK2").bdg, curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"PK3").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::POCKET_ENTRY, TI.Get(L"PK3").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::POCKET_EXIT, TI.Get(L"PK7").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"PK7").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"PK8").bdg, ++curtime ) ),
	};

    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
	
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    TrioSpot trio = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trio.pre());
    CPPUNIT_ASSERT(!trio.post());
    CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

	auto eventLists = autoChart->GetPaths_OnlyEvents( curtime, curtime );
	CPPUNIT_ASSERT( eventLists.size() == 1 );
	const auto & evList = eventLists.front();
	CPPUNIT_ASSERT_EQUAL( (size_t)6, evList.size() );
	auto elIt = evList.cbegin();
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::POCKET_ENTRY );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::POCKET_EXIT );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
}

void TC_AutoChartHappen::PocketExit()
{
	time_t curtime = time( NULL );
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::POCKET_EXIT, TI.Get(L"PK7").bdg, curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"PK7").bdg, ++curtime ) )
	};

    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));

    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    TrioSpot trio = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trio.pre());
    CPPUNIT_ASSERT(!trio.post());
    CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

	auto eventLists = autoChart->GetPaths_OnlyEvents( curtime, curtime );
	CPPUNIT_ASSERT( eventLists.size() == 1 );
	const auto & evList = eventLists.front();
	CPPUNIT_ASSERT( evList.size() == 2 );
	auto elIt = evList.cbegin();
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::POCKET_EXIT );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
}

void TC_AutoChartHappen::InvalidForm()
{
	time_t begin_time = time( NULL );
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::POCKET_ENTRY, TI.Get(L"PK3").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::ENTRANCE_STOPPING, TI.Get(L"X3").bdg, begin_time ) ),
		SpotEventPtr( new SpotEvent( HCode::WRONG_SPANWAY, TI.Get(L"X3").bdg, begin_time ) ) };
	for ( unsigned int k = 0; k < size_array( events_array ); ++k )
	{
        SpotEventPtr tstevents[] = { events_array[k], SpotEventPtr( new SpotEvent( HCode::FORM, events_array[k]->GetBadge(), begin_time ) ) };
		CPPUNIT_ASSERT(!CreatePath(*autoChart, tstevents, k + 1));
		auto evcode = events_array[k]->GetCode();
		auto eventLists = autoChart->GetPaths_OnlyEvents( begin_time, begin_time );
		CPPUNIT_ASSERT( eventLists.size() == 2 );
        for ( const auto & evList : eventLists )
        {

            CPPUNIT_ASSERT( evList.size() == 2 || evList.size() == 1 );
            if ( evList.size()==1 )
            {
                auto elIt = evList.cbegin();
                CPPUNIT_ASSERT( (*elIt)->GetCode() == HCode::FORM );
            }
            else if ( evList.size() == 2 )
            {
                auto elIt = evList.cbegin();
                CPPUNIT_ASSERT( (*elIt)->GetCode() == tstevents[0]->GetCode() );
                CPPUNIT_ASSERT( (*next(elIt))->GetCode() == HCode::DEATH );
            }
        }
		autoChart.reset( new AutoChartTest() );
	}
}

void TC_AutoChartHappen::Recovery()
{
	time_t start_time = time( NULL );
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"F3").bdg, start_time ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"X3").bdg, ++start_time ) ),
		SpotEventPtr( new SpotEvent( HCode::DEATH, TI.Get(L"X3").bdg, ++start_time ) ),
		SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"Y3").bdg, ++start_time ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"Y3").bdg, ++start_time ) )
	};

    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
	
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    TrioSpot trio = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trio.pre());
    CPPUNIT_ASSERT(!trio.post());
    CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

	auto eventLists = autoChart->GetPaths_OnlyEvents( start_time, start_time );
	CPPUNIT_ASSERT( eventLists.size() == 1 );
	const auto & evList = eventLists.front();
	CPPUNIT_ASSERT( evList.size() == 5 );
	auto elIt = evList.cbegin();
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::DEATH );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
}

void TC_AutoChartHappen::InfoChanging()
{
	time_t start_time = time( NULL );
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"C").bdg, start_time ) ),
		SpotEventPtr( new SpotEvent( HCode::INFO_CHANGING, TI.Get(L"C").bdg, ++start_time ) ),
        SpotEventPtr( new SpotEvent( HCode::INFO_CHANGING, TI.Get(L"C").bdg, ++start_time ) ),
        SpotEventPtr( new SpotEvent( HCode::POCKET_ENTRY, TI.Get(L"PK3").bdg, ++start_time ) ),
        SpotEventPtr( new SpotEvent( HCode::INFO_CHANGING, TI.Get(L"PK5").bdg, ++start_time ) ),
        SpotEventPtr( new SpotEvent( HCode::POCKET_EXIT, TI.Get(L"PK7").bdg, ++start_time ) ),
        SpotEventPtr( new SpotEvent( HCode::INFO_CHANGING, TI.Get(L"PK7").bdg, ++start_time ) )
	};

    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));

	auto eventLists = autoChart->GetPaths_OnlyEvents( start_time, start_time );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, eventLists.size() );
	const auto & evList = eventLists.front();
	CPPUNIT_ASSERT_EQUAL( (size_t)3, evList.size() );
	CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::FORM, TI.Get(L"C").bdg ) ) );
	//CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::INFO_CHANGING, TI.Get(L"C").bdg ) ) );
    //CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::INFO_CHANGING, TI.Get(L"C").bdg ) ) );
    CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::POCKET_ENTRY, TI.Get(L"PK3").bdg ) ) );
    //CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::INFO_CHANGING, TI.Get(L"PK5").bdg ) ) );
    CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::POCKET_EXIT, TI.Get(L"PK7").bdg ) ) );
    //CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::INFO_CHANGING, TI.Get(L"PK7").bdg ) ) );
}

void TC_AutoChartHappen::EntranceStopping()
{
	HCode hcodes[] = { HCode::FORM, HCode::DISFORM, HCode::DEATH, HCode::ARRIVAL, HCode::DEPARTURE, HCode::TRANSITION, HCode::SPAN_MOVE, 
		HCode::INFO_CHANGING, HCode::ENTRANCE_STOPPING, HCode::WRONG_SPANWAY, HCode::TOKEN_SPANWAY, HCode::POCKET_ENTRY, HCode::POCKET_EXIT
	};
    for (const HCode code : hcodes)
    {
        time_t start_time = time( NULL );
        BadgeE startBdg = HemEventRegistry::MustBeOnStation(code)? TI.Get(L"F3").bdg : TI.Get(L"X3").bdg;
        SpotEventPtr spotEvents[] = {
            SpotEventPtr( new SpotEvent( code, startBdg, start_time ) ),
            SpotEventPtr( new SpotEvent( HCode::ENTRANCE_STOPPING, TI.Get(L"X3").bdg, start_time + 1 ) )
        };
		
        bool const pathCreated = CreatePath(*autoChart, spotEvents, 64);
		auto eventLists = autoChart->GetPaths_OnlyEvents( start_time, start_time + 1 );
        if ( code == HCode::DISFORM || code == HCode::DEATH || code == HCode::INFO_CHANGING )
		{
            // Предыдущая версия теста была некорректной: пробрасывались события с нулевым guessTransciever'ом,
            // который не запоминал и не менял трио
            // Нитку нельзя начинать с расформирования, смерти и изменения информации
            CPPUNIT_ASSERT( !pathCreated );
            // Но, видимо, можно с начала стоянки
            CPPUNIT_ASSERT( !eventLists.empty() );
		}
		else if ( code == HCode::ARRIVAL )
		{
			//ввиду неполного отслеживания мозаичных станций processTrackerEvent корректирует входную  
			//последовательность сигналов Guess - создает недостающее событие Departure. 
			//Поэтому событий в нити будет 3: Arrival !!!Departure!!! Entrance_Stopping (3 по цене 2-х)
			CPPUNIT_ASSERT( pathCreated );
			CPPUNIT_ASSERT_EQUAL( (size_t)1, eventLists.size() );
			const auto & evList = eventLists.front();
			CPPUNIT_ASSERT( pathCreated );
			CPPUNIT_ASSERT( evList.size() == 3 );
			CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::ENTRANCE_STOPPING, TI.Get(L"X3").bdg ) ) );

		}
		else if ( code == HCode::POCKET_ENTRY ||
			code == HCode::ENTRANCE_STOPPING )
		{
            CPPUNIT_ASSERT( !pathCreated );
            CPPUNIT_ASSERT( eventLists.size() == 2 );
		}
		else
		{
            CPPUNIT_ASSERT_EQUAL( (size_t)1, eventLists.size() );
            const auto & evList = eventLists.front();
			CPPUNIT_ASSERT( pathCreated );
			CPPUNIT_ASSERT( evList.size() == 2 );
            CPPUNIT_ASSERT( FindEventInfo( evList, make_pair( HCode::ENTRANCE_STOPPING, TI.Get(L"X3").bdg ) ) );
		}
        autoChart.reset( new AutoChartTest() );
	}
}

void TC_AutoChartHappen::SpanToSpanTransit()
{
	time_t curtime = time( NULL );
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"W").bdg, curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"Q").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::TRANSITION, TI.Get(L"C").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"X").bdg, ++curtime ) ),
	};

    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
	
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    TrioSpot trio = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trio.pre());
    CPPUNIT_ASSERT(!trio.post());
    CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

	auto eventLists = autoChart->GetPaths_OnlyEvents( curtime, curtime );
	CPPUNIT_ASSERT( eventLists.size() == 1 );
	const auto & evList = eventLists.front();
	CPPUNIT_ASSERT( evList.size() == 4 );
	auto elIt = evList.cbegin();
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::SPAN_MOVE );
}

void TC_AutoChartHappen::StatToStatWOArrdep()
{
	time_t curtime = time( NULL );
	SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"ST1").bdg, curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::STATION_EXIT, TI.Get(L"ST1").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::STATION_ENTRY, TI.Get(L"ST2").bdg, ++curtime ) ),
		SpotEventPtr( new SpotEvent( HCode::STATION_EXIT, TI.Get(L"ST2").bdg, ++curtime ) ),
	};

	CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
	
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    TrioSpot trio = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trio.pre());
    CPPUNIT_ASSERT(!trio.post());
    CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

	auto eventLists = autoChart->GetPaths_OnlyEvents( curtime, curtime );
	CPPUNIT_ASSERT( eventLists.size() == 1 );
	const auto & evList = eventLists.front();
	CPPUNIT_ASSERT( evList.size() == 4 );
	auto elIt = evList.cbegin();
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::STATION_EXIT );
}

void TC_AutoChartHappen::ReduceAmount()
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    auto F = []( int hh, int mm ) {
        ptime mom = ptime(date(2015,8,8)) + hours(hh) + minutes(mm);
        ptime epoch( date( 1970, 1, 1 ) );
        return ( mom - epoch ).total_seconds();
    };

    const EsrKit K(1100,1200);

    // вечерняя нить
    SpotEventPtr path1[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"1U",K), F(20,20) ) ),
		SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"2U",K), F(22,22) ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,     BadgeE(L"2U",K), F(22,22) ) ),
	};
    // утренняя нить
    SpotEventPtr path2[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"3U",K), F( 7,17) ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"4U",K), F( 9,39) ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"5U",K), F(11,40) ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,     BadgeE(L"5U",K), F(11,40) ) ),
    };
    // нить с утра до вечера
    SpotEventPtr path3[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"6U",K), F( 8,48) ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"7U",K), F(11,11) ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"8U",K), F(18,18) ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"9U",K), F(21,21) ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,     BadgeE(L"9U",K), F(21,21) ) ),
    };

    CPPUNIT_ASSERT(CreatePath(*autoChart, path1, 1));
    CPPUNIT_ASSERT(CreatePath(*autoChart, path2, 2));
    CPPUNIT_ASSERT(CreatePath(*autoChart, path3, 3));

    auto have_size = [](const list <HappenLayer::EventsList>& elst, unsigned n){
        for( auto item : elst )
        {
            if ( item.size()==n )
                return true;
        }
        return false;
    };
    // до сжатия графика - все нити есть
    auto eventLists = autoChart->GetPaths_OnlyEvents( F(00,00), F(24,00) );
    CPPUNIT_ASSERT( eventLists.size() == 3 );
    CPPUNIT_ASSERT( have_size(eventLists,3) );
    CPPUNIT_ASSERT( have_size(eventLists,4) );
    CPPUNIT_ASSERT( have_size(eventLists,5) );
    // само сжатие
    autoChart->reduceEvents( F(13,0), 0 );
    autoChart->reduceTransceiver( F(13,0) );
    // после сжатия - только нити в последних сутках
    auto eventAfter = autoChart->GetPaths_OnlyEvents( F(00,00), F(24,00) );
    CPPUNIT_ASSERT( eventAfter.size() == 2 );
    CPPUNIT_ASSERT( have_size(eventAfter,3) );
    CPPUNIT_ASSERT( !have_size(eventAfter,4) );
    CPPUNIT_ASSERT( have_size(eventAfter,5) );
}

bool TC_AutoChartHappen::PermittedAsSequence( const pair <HCode, BadgeE> & evpair1, const pair <HCode, BadgeE> & evpair2, time_t momtime )
{
	bool permitted = false;
	HCode hcode1 = evpair1.first;
	BadgeE bdg1 = evpair1.second;
	HCode hcode2 = evpair2.first;
	BadgeE bdg2 = evpair2.second;

	SpotEventPtr events_array1[] = { SpotEventPtr( new SpotEvent( HCode::FORM, bdg1, momtime ) ),
		SpotEventPtr( new SpotEvent( hcode1, bdg1, momtime + 1 ) ),
		SpotEventPtr( new SpotEvent( hcode2, bdg2, momtime + 2 ) )
	};
	SpotEventPtr events_array2[] = { SpotEventPtr( new SpotEvent( hcode1, bdg1, momtime ) ),
		SpotEventPtr( new SpotEvent( hcode2, bdg2, momtime + 1 ) )
	};
	auto arrsize = ( hcode1 == HCode::FORM ? size_array( events_array2 ) : size_array( events_array1 ) );
	set <HemEventPtr> forbiddenEvents;
	bool const succeeded = ( hcode1 == HCode::FORM ? 
		CreatePath( *autoChart, events_array2, 64) : 
		CreatePath( *autoChart, events_array1, 64) );

	if ( GetHappenTrioCount() == 1 && succeeded )
	{
		auto eventLists = autoChart->GetPaths_OnlyEvents( momtime, momtime );
		if ( eventLists.size() == 1 )
		{
			const auto & evList = eventLists.front();
			if ( evList.size() == arrsize )
			{
				vector <HCode> evcodes;
				for( auto event : evList )
					evcodes.push_back( event->GetCode() );
				if ( evcodes.size() == 2 )
					permitted = ( evcodes[0] == hcode1 && evcodes[1] == hcode2 );
				else if ( evcodes.size() == 3 )
					permitted = ( evcodes[0] == HCode::FORM && evcodes[1] == hcode1 && evcodes[2] == hcode2 );
			}
		}
	}

    autoChart.reset(new AutoChartTest);
	return permitted;
}

void TC_AutoChartHappen::TwoEventsValidness()
{
	static const auto & tinfraStruct = TI;
	struct TwoEventsSeq
	{
		TwoEventsSeq( HCode _hcode1, wstring _place1, HCode _hcode2, wstring _place2, bool _validness = true ) :
			hcode1( _hcode1 ), place1( _place1 ), hcode2( _hcode2 ), place2( _place2 ), validness( _validness ){}
		std::pair <HCode, BadgeE> EvBadge1() const { return make_pair( hcode1, tinfraStruct.Get( place1 ).bdg ); }
		std::pair <HCode, BadgeE> EvBadge2() const { return make_pair( hcode2, tinfraStruct.Get( place2 ).bdg ); }
		bool Permitted() const { return validness; }

	private:
		HCode hcode1, hcode2;
		wstring place1, place2;
		bool validness;
	};

	TwoEventsSeq eseqArr[] = {
		//отправление с одного пути, затем прибытие на другой путь:
		TwoEventsSeq( HCode::DEPARTURE, L"P3", HCode::ARRIVAL, L"S3" ),
		//отправление с одного пути, затем проследование другого пути:
		TwoEventsSeq( HCode::DEPARTURE, L"P3", HCode::TRANSITION, L"S3" ),
		//въезд на станцию и прибытие на путь:
		TwoEventsSeq( HCode::STATION_ENTRY, L"H3", HCode::ARRIVAL, L"I3" ),
		//проследование и прибытие на путь:
		TwoEventsSeq( HCode::TRANSITION, L"P3", HCode::ARRIVAL, L"S3" ),
		//движение по перегону и въезд на станцию через светофор чужой четности:
		TwoEventsSeq( HCode::SPAN_MOVE, L"I", HCode::DISFORM, L"J" ),
		//формирование и расформирование (например, из-за движения через несоответствующий светофор):
		TwoEventsSeq( HCode::FORM, L"C", HCode::DISFORM, L"F" ),
		//прибытие последовательно на два п/о пути:
		TwoEventsSeq( HCode::ARRIVAL, L"R4", HCode::ARRIVAL, L"S4", false ),
		//проследование последовательно двух п/о путей:
		TwoEventsSeq( HCode::TRANSITION, L"R4", HCode::TRANSITION, L"C" ),
		//стоянка перед входным и движение по перегону (технически такое возможно):
		TwoEventsSeq( HCode::ENTRANCE_STOPPING, L"W", HCode::SPAN_MOVE, L"W" ),
		//прибытие и расформирование
		TwoEventsSeq( HCode::ARRIVAL, L"C", HCode::DISFORM, L"C" ),
		//вход на станцию и проследование
		TwoEventsSeq( HCode::STATION_ENTRY, L"A", HCode::TRANSITION, L"C" ),
		//проследование и выход со станции
		TwoEventsSeq( HCode::TRANSITION, L"C", HCode::STATION_EXIT, L"A" )
	};
	auto curtime = time( NULL );
	for( unsigned int ind = 0; ind < size_array( eseqArr ); ++ind )
	{
		auto twoEvSeq = eseqArr[ind];
		CPPUNIT_ASSERT( PermittedAsSequence( twoEvSeq.EvBadge1(), twoEvSeq.EvBadge2(), curtime + 10 * ind ) == twoEvSeq.Permitted() );
	}
}

void TC_AutoChartHappen::MultiInfoChanges()
{
	bool permitted = true;
	auto reftime = time( NULL );
	for ( auto i = 0; i < 2; ++i )
	{
		auto curtime = reftime + i * 10;
		auto curtime2 = curtime + 1;
		SpotDetailsPtr spotPtr1( new SpotDetails( TrainDescr( L"V2833" ) ) );
		SpotDetailsPtr anotherNumSpotPtr1( new SpotDetails( TrainDescr( L"J2833" ) ) );
		SpotDetailsPtr anotherNumSpotPtr2( new SpotDetails( TrainDescr( L"N2833" ) ) );
		SpotDetailsPtr spotPtr2 = ( permitted ? anotherNumSpotPtr1 : spotPtr1 );
		SpotDetailsPtr spotPtr3 = ( permitted ? anotherNumSpotPtr2 : spotPtr1 );;
		SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"C").bdg, curtime ) ),
			SpotEventPtr( new SpotEvent( HCode::INFO_CHANGING, TI.Get(L"C").bdg, curtime2, spotPtr1 ) ),
			SpotEventPtr( new SpotEvent( HCode::INFO_CHANGING, TI.Get(L"C").bdg, curtime2, spotPtr2 ) ),
			SpotEventPtr( new SpotEvent( HCode::INFO_CHANGING, TI.Get(L"C").bdg, curtime2, spotPtr3 ) )
		};

		bool const succeeded = CreatePath( *autoChart, events_array, i + 1);

        CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
        TrioSpot trio = GetHappenTrio(0);
        CPPUNIT_ASSERT(!trio.pre());
        CPPUNIT_ASSERT(!trio.post());
        CPPUNIT_ASSERT_EQUAL(size_array(events_array), trio.body().size());

		if ( permitted )
			CPPUNIT_ASSERT( succeeded );
		else
			CPPUNIT_ASSERT( !succeeded );

		auto eventLists = autoChart->GetPaths_OnlyEvents( curtime, curtime2 );
		CPPUNIT_ASSERT( eventLists.size() == 1 );
		const auto & evList = eventLists.front();

		if ( permitted )
			CPPUNIT_ASSERT( evList.size() == 3 );
		else
			CPPUNIT_ASSERT( evList.size() == 2 );

		auto elIt = evList.cbegin();
		CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::FORM );
		CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::INFO_CHANGING );

		if ( permitted )
			CPPUNIT_ASSERT( (*(elIt++))->GetCode() == HCode::INFO_CHANGING );

		permitted = false;
	}
}

void TC_AutoChartHappen::SafelyDiscard()
{
    unsigned const tid = 64;

    BadgeE A(L"4p", EsrKit(100));
    BadgeE B(L"2p", EsrKit(200));

    CPPUNIT_ASSERT(autoChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::FORM,      A, 33), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(autoChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DEPARTURE, A, 44), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(autoChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::ARRIVAL,   B, 55), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(autoChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DISFORM,   B, 66), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(!autoChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DISFORM,  B, 77), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(!autoChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DISFORM,   B, 77), tid, TrainCharacteristics::Source::Hem));
}
