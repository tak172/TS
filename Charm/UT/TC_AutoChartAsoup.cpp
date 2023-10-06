#include "stdafx.h"
#include "TC_AutoChartAsoup.h"
#include "../UT/AutoChartTest.h"
#include "../Hem/SpotEvent.h"
#include "../helpful/TrainDescr.h"
#include "../Hem/AsoupEvent.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/RegulatoryLayer.h"
#include "../Guess/Msg.h"
#include "../Asoup/Message.h"
#include "UtHelpfulDateTime.h"
#include "UtHemHelper.h"

using namespace HemHelpful;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AutoChartAsoup );

BadgeE CreateRealBadge(const BadgeE& testBadge)
{
    return BadgeE(L"АСОУП НОМЕР", testBadge.num());
}

TC_AutoChartAsoup::TC_AutoChartAsoup() : TI( *TrackerInfrastructure::instance() )
{
    autoChart.reset( new AutoChartTest() );
}

TC_AutoChartAsoup::~TC_AutoChartAsoup()
{
    HemEventRegistry::Shutdowner();
    TrackerInfrastructure::Shutdowner();
}

void TC_AutoChartAsoup::AsoupMessage()
{
    time_t curtime = time( NULL );
    time_t form_time = curtime;
    time_t span_move_time = curtime;
    time_t entry_time = span_move_time + 10;
    time_t arrival_time = entry_time + 10;

    //имитируем вход ПЕ на станцию (путь A)
    SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"W").bdg, form_time ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"Q").bdg, span_move_time ) ),
        SpotEventPtr( new SpotEvent( HCode::ARRIVAL, TI.Get(L"C").bdg, arrival_time ) ) };
    set <HemEventPtr> forbiddenEvents;
    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
    //
    //отправляем АСОУП-сообщение о прибытии ПЕ на ПО путь "C"
    TrainDescr testTrainDescr;
    testTrainDescr.SetIndex( L"1-2-3" );
    testTrainDescr.SetNumber( L"asoup1329" );
    testTrainDescr.SetRelLength( 88 );
    AsoupEventPtr asoup( new AsoupEvent( HCode::ARRIVAL, CreateRealBadge(TI.Get(L"C").bdg), 
        ParkWayKit(), 
        arrival_time, testTrainDescr ) );

    autoChart->updateTime(timeForDate(2015, 10, 10), nullptr);
    CPPUNIT_ASSERT(PassAsoup(*autoChart, asoup, asoup->GetTime()+30));

    CPPUNIT_ASSERT_EQUAL(size_t(1), GetTrainDescrCount());
    CPPUNIT_ASSERT(GetTrainDescr(0));
    CPPUNIT_ASSERT(*GetTrainDescr(0) == testTrainDescr);

    //проверяем верность интерпретации событий
    list <HappenLayer::EventWithDescrList> edLists = autoChart->GetPaths_EventAndDescr( span_move_time, span_move_time );
    CPPUNIT_ASSERT( edLists.size() == 1 );
    const HappenLayer::EventWithDescrList & EDList = edLists.front();
    pair <time_t, time_t> result_interval;
    CPPUNIT_ASSERT( FindTrainDescr( EDList, testTrainDescr, result_interval ) );
    // Устаревшая проверка, фрагменты изменяются при добавлении
    // CPPUNIT_ASSERT( result_interval.first == arrival_time && result_interval.second == arrival_time );
}

void TC_AutoChartAsoup::ArrivingAsoupBeforeLOS()
{
    time_t curtime = time( NULL );
    time_t appearance_time = curtime + 10;
    time_t entry_time = appearance_time + 10;
    time_t arrival_time = entry_time + 10;
    time_t departure_time = arrival_time + 10;
    time_t exit_time = departure_time + 10;
    time_t span_move_time = exit_time + 10;

    //отправляем АСОУП-сообщение о прибытии ПЕ на ПО путь "C"
    TrainDescr testTrainDescr;
    testTrainDescr.SetIndex( L"1-2-3" );
    testTrainDescr.SetNumber( L"asoup1234" );
    testTrainDescr.SetRelLength( 88 );
    AsoupEventPtr asoup( new AsoupEvent( HCode::ARRIVAL, CreateRealBadge(TI.Get(L"C").bdg), ParkWayKit(), arrival_time, testTrainDescr ) );
    autoChart->updateTime(curtime, nullptr);
    CPPUNIT_ASSERT(PassAsoup( *autoChart, asoup ));

    //имитируем проход ПЕ сквозь станцию (через путь C)
    SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"Q").bdg, appearance_time ) ),
        SpotEventPtr( new SpotEvent( HCode::ARRIVAL, TI.Get(L"C").bdg, arrival_time ) ),
        SpotEventPtr( new SpotEvent( HCode::DEPARTURE, TI.Get(L"C").bdg, departure_time ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"X").bdg, span_move_time ) )
    };

    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));

    CPPUNIT_ASSERT_EQUAL(size_t(0), GetTrainDescrCount());
    // Идентификация по АСОУП во время появления нового события исполненного расписания
    // отключена
    CPPUNIT_ASSERT(!GetTrainDescr(0));

    //удостоверяемся, что АСОУП-фрагмент начинается с нужного момента
    list <HappenLayer::EventWithDescrList> edLists = autoChart->GetPaths_EventAndDescr( entry_time, exit_time );
    CPPUNIT_ASSERT( edLists.size() == 1 );
}

void TC_AutoChartAsoup::ArrivingAsoupAfterLOS()
{
    time_t curtime = time( NULL );

    time_t appearance_time = curtime + 10;
    time_t entry_time = appearance_time + 10;
    time_t arrival_time = entry_time + 10;
    time_t departure_time = arrival_time + 10;
    time_t exit_time = departure_time + 10;
    time_t span_move_time = exit_time + 10;

    //имитируем проход ПЕ сквозь станцию (через путь C)
    SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"Q").bdg, appearance_time ) ),
        SpotEventPtr( new SpotEvent( HCode::ARRIVAL, TI.Get(L"C").bdg, arrival_time ) ),
        SpotEventPtr( new SpotEvent( HCode::DEPARTURE, TI.Get(L"C").bdg, departure_time ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"X").bdg, span_move_time ) )
    };
    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));

    //отправляем АСОУП-сообщение о прибытии ПЕ на ПО путь "C"
    TrainDescr testTrainDescr;
    testTrainDescr.SetIndex( L"1-2-3" );
    testTrainDescr.SetNumber( L"1234GF" );
    testTrainDescr.SetRelLength( 88 );
    AsoupEventPtr asoup( new AsoupEvent( HCode::ARRIVAL, CreateRealBadge(TI.Get(L"C").bdg), ParkWayKit(), arrival_time, testTrainDescr ) );

    autoChart->updateTime(arrival_time, nullptr);
    CPPUNIT_ASSERT(PassAsoup(*autoChart, asoup, asoup->GetTime()+30));

    CPPUNIT_ASSERT_EQUAL(size_t(1), GetTrainDescrCount());
    CPPUNIT_ASSERT(GetTrainDescr(0));
    CPPUNIT_ASSERT(*GetTrainDescr(0) == testTrainDescr);

    //удостоверяемся, что АСОУП-фрагмент начинается с нужного момента
    list <HappenLayer::EventWithDescrList> edLists = autoChart->GetPaths_EventAndDescr( entry_time, exit_time );
    CPPUNIT_ASSERT( edLists.size() == 1 );
    const HappenLayer::EventWithDescrList & EDList = edLists.front();
    pair <time_t, time_t> result_interval;
    CPPUNIT_ASSERT( FindTrainDescr( EDList, testTrainDescr, result_interval ) );
}

bool TC_AutoChartAsoup::PassAsoupMessageThrough(const std::wstring& messageText)
{
    return PassAsoupRawText(*autoChart, messageText);
}

void TC_AutoChartAsoup::ParseRawAsoupText()
{
    const std::wstring c_1042 = L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
        L"Ю3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
        L"Ю3 539 00012332 9:)";

    CPPUNIT_ASSERT(PassAsoupMessageThrough(c_1042));

    attic::a_node expectedAsoupLayerNode = GetChangesNode().child(AsoupLayer_xAttr);
    CPPUNIT_ASSERT(expectedAsoupLayerNode);

    size_t count = 0u;
    for (attic::a_node asoupChangeTrioNode : expectedAsoupLayerNode.children(Trio_xTag))
    {
        TrioAsoup expectedAsoupTrio;
        expectedAsoupTrio << asoupChangeTrioNode;
        CPPUNIT_ASSERT(!expectedAsoupTrio.empty());

        CPPUNIT_ASSERT( !expectedAsoupTrio.body().empty() );
        CPPUNIT_ASSERT( expectedAsoupTrio.body().front() );
        ++count;
    }
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)count);

    CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());
}

void TC_AutoChartAsoup::WithdrawalAsoupMessage()
{
    const std::wstring c_333 = L"(:333 1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)";

    // Пока поддержки отмены 333 сообщения нет, оно не должно добавляться
    CPPUNIT_ASSERT(!PassAsoupMessageThrough(c_333));

    CPPUNIT_ASSERT_EQUAL(size_t(0), GetAsoupTrioCount());
}

void TC_AutoChartAsoup::WithdrawalDuplicateMessages()
{
    const std::wstring c_1042 = L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
        L"Ю3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
        L"Ю3 539 00012332 9:)";

    CPPUNIT_ASSERT(PassAsoupMessageThrough(c_1042));
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());
    
    // Временное поведение: дублирующие сообщения принимаются чартом,
    // но изменений не дают (то есть откидываются без ошибки)
    CPPUNIT_ASSERT(PassAsoupMessageThrough(c_1042));
    CPPUNIT_ASSERT_EQUAL(size_t(0), GetAsoupTrioCount());
}

void TC_AutoChartAsoup::ShrinkAsoup()
{
    using namespace attic;
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    std::wstring const aug27_0830 =
        L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 27 08 08 30 00/00 0 0/00 00 0\n"
        L"Ю3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
        L"Ю3 539 00012332 9:)";
    std::wstring const aug28_0830 =
        L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 28 08 08 30 00/00 0 0/00 00 0\n"
        L"Ю3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
        L"Ю3 539 00012332 9:)";

    CPPUNIT_ASSERT(PassAsoupMessageThrough(aug27_0830));
    CPPUNIT_ASSERT(PassAsoupMessageThrough(aug28_0830));

    auto ChildCount = []( const attic::a_document& doc) {
        auto xq = doc.document_element().select_nodes("//AsoupLayer/AsoupEvent");
        size_t count = xq.size();
        return count;
    };

    attic::a_document doc("xx");
    autoChart->serialize(doc.document_element());
    CPPUNIT_ASSERT_EQUAL( (size_t)2u, ChildCount( doc ) );
    autoChart->reduceEvents( timeForDate(2015, 8, 28), 0 );
    autoChart->reduceTransceiver( timeForDate(2015, 8, 28) );
    autoChart->takeChanges( attic::a_node(), attic::a_node(), 0 ); // применить все изменения к графику (без передачи куда-либо)

    autoChart->serialize(doc.document_element());
    CPPUNIT_ASSERT_EQUAL( (size_t)1u, ChildCount( doc ) );
    autoChart->reduceEvents( timeForDate(2015, 8, 28), 0 );
    autoChart->reduceTransceiver( timeForDate(2015, 8, 28) );
    autoChart->takeChanges( attic::a_node(), attic::a_node(), 0 ); // применить все изменения к графику (без передачи куда-либо)

    autoChart->serialize(doc.document_element());
    CPPUNIT_ASSERT_EQUAL( (size_t)1u, ChildCount( doc ) );
}

void TC_AutoChartAsoup::ArrivingAsoupAfterLOSRealBadges()
{
    time_t curtime = time( NULL );

    time_t appearance_time = curtime + 10;
    time_t entry_time = appearance_time + 10;
    time_t arrival_time = entry_time + 10;
    time_t departure_time = arrival_time + 10;
    time_t exit_time = departure_time + 10;
    time_t span_move_time = exit_time + 10;

    //имитируем проход ПЕ сквозь станцию (через путь C)
    SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"Q").bdg, appearance_time ) ),
        SpotEventPtr( new SpotEvent( HCode::ARRIVAL, TI.Get(L"C").bdg, arrival_time ) ),
        SpotEventPtr( new SpotEvent( HCode::DEPARTURE, TI.Get(L"C").bdg, departure_time ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"X").bdg, span_move_time ) )
    };
    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
    //отправляем АСОУП-сообщение о прибытии ПЕ на ПО путь "C"
    TrainDescr testTrainDescr;
    testTrainDescr.SetIndex( L"1-2-3" );
    testTrainDescr.SetNumber( L"FG2323" );
    testTrainDescr.SetRelLength( 88 );
    AsoupEventPtr asoupArrival( new AsoupEvent( HCode::ARRIVAL, CreateRealBadge(TI.Get(L"C").bdg), ParkWayKit(), arrival_time, testTrainDescr ) );
    autoChart->updateTime(arrival_time, nullptr);
    CPPUNIT_ASSERT(PassAsoup( *autoChart, asoupArrival, asoupArrival->GetTime() + 30 ));

    CPPUNIT_ASSERT_EQUAL(size_t(1), GetTrainDescrCount());
    CPPUNIT_ASSERT(GetTrainDescr(0));
    CPPUNIT_ASSERT(*GetTrainDescr(0) == testTrainDescr);

    //удостоверяемся, что АСОУП-фрагмент начинается с нужного момента
    list <HappenLayer::EventWithDescrList> edLists = autoChart->GetPaths_EventAndDescr( entry_time, exit_time );
    CPPUNIT_ASSERT( edLists.size() == 1 );
    const HappenLayer::EventWithDescrList & EDList = edLists.front();
    pair <time_t, time_t> result_interval;
    CPPUNIT_ASSERT( FindTrainDescr( EDList, testTrainDescr, result_interval ) );
}

void TC_AutoChartAsoup::ServedChanges()
{
    time_t curtime = time( NULL );

    time_t appearance_time = curtime + 10;
    time_t entry_time = appearance_time + 10;
    time_t arrival_time = entry_time + 10;
    time_t departure_time = arrival_time + 10;
    time_t exit_time = departure_time + 10;
    time_t span_move_time = exit_time + 10;

    //имитируем проход ПЕ сквозь станцию (через путь C)
    SpotEventPtr events_array[] = { SpotEventPtr( new SpotEvent( HCode::FORM, TI.Get(L"Q").bdg, appearance_time ) ),
        SpotEventPtr( new SpotEvent( HCode::ARRIVAL, TI.Get(L"C").bdg, arrival_time ) ),
        SpotEventPtr( new SpotEvent( HCode::DEPARTURE, TI.Get(L"C").bdg, departure_time ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, TI.Get(L"X").bdg, span_move_time ) )
    };

    CPPUNIT_ASSERT(CreatePath(*autoChart, events_array, 64));
    TrainDescr testTrainDescr;
    testTrainDescr.SetIndex( L"1-2-3" );
    testTrainDescr.SetNumber( L"F2323" );
    testTrainDescr.SetRelLength( 88 );

        time_t asoupTime = departure_time + 1115;
    {
        AsoupEventPtr asoupDep(new AsoupEvent(HCode::DEPARTURE, CreateRealBadge(TI.Get(L"C").bdg), ParkWayKit(), asoupTime, testTrainDescr));

        CPPUNIT_ASSERT(PassAsoup( *autoChart, asoupDep, asoupTime ));

        CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());
        CPPUNIT_ASSERT_EQUAL(size_t(0), GetTrainDescrCount());
    }

    {
        AsoupEventPtr asoupArr( new AsoupEvent( HCode::ARRIVAL, CreateRealBadge(TI.Get(L"C").bdg), ParkWayKit(), arrival_time, testTrainDescr ) );

        CPPUNIT_ASSERT(PassAsoup( *autoChart, asoupArr, asoupTime + 30 ));

        // Факт привязывания должен быть один
        CPPUNIT_ASSERT_EQUAL(size_t(1), GetTrainDescrCount());

        // По совпадению номера и индекса привязаться должны оба 
        // сообщения. Одно по прибытию - в строгом диапазоне времени, 
        // второе по отправлени - в расширенном диапазоне времени ( по совпадению номера )
        CPPUNIT_ASSERT_EQUAL(size_t(2), GetAsoupTrioCount());
    }	
}

void TC_AutoChartAsoup::WithdrawalAsoupMessageUndoGlue()
{
	auto path = loaderSpot(
		L"<HemPath>"
		L"<SpotEvent create_time='20220607T073400Z' name='Arrival' Bdg='ASOUP 1042_1p7w[31170]' waynum='7' parknum='1' index='3100-495-0300' num='2307' length='57' weight='2092' net_weight='1161' through='Y'>"
		L"<Locomotive Series='2ТЭ116УД' NumLoc='37' Depo='104' Consec='1'>"
		L"<Crew EngineDriver='ПOЛEЩУK' TabNum='52875' Tim_Beg='2022-06-07 03:45' />"
		L"</Locomotive>"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T073600Z' name='Departure' Bdg='ASOUP 1042_1p7w[31170]' waynum='7' parknum='1' optCode='31160:31170' />"
		L"<SpotEvent create_time='20220607T074400Z' name='Span_stopping_begin' Bdg='guiNote[31160:31170]'>"
		L"<rwcoord picketing1_val='45~775' picketing1_comm='Рыбинск' />"
		L"<rwcoord picketing1_val='45~775' picketing1_comm='Рыбинск' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T074700Z' name='Span_stopping_end' Bdg='guiNote[31160:31170]'>"
		L"<rwcoord picketing1_val='45~775' picketing1_comm='Рыбинск' />"
		L"<rwcoord picketing1_val='45~775' picketing1_comm='Рыбинск' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T075200Z' name='Transition' Bdg='ASOUP 1042_1p1w[31160]' waynum='1' parknum='1' optCode='31151:31160' />"
		L"<SpotEvent create_time='20220607T075900Z' name='Span_stopping_begin' Bdg='guiNote[31151:31160]'>"
		L"<rwcoord picketing1_val='59~150' picketing1_comm='Рыбинск' />"
		L"<rwcoord picketing1_val='59~150' picketing1_comm='Рыбинск' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T080100Z' name='Span_stopping_end' Bdg='guiNote[31151:31160]'>"
		L"<rwcoord picketing1_val='59~150' picketing1_comm='Рыбинск' />"
		L"<rwcoord picketing1_val='59~150' picketing1_comm='Рыбинск' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T081000Z' name='Arrival' Bdg='ASOUP 1042_1p3w[31151]' waynum='3' parknum='1' />"
		L"<SpotEvent create_time='20220607T081800Z' name='Departure' Bdg='ASOUP 1042_1p3w[31151]' waynum='3' parknum='1' optCode='31139:31151' />"
		L"<SpotEvent create_time='20220607T083300Z' name='Span_stopping_begin' Bdg='guiNote[31139:31151]'>"
		L"<rwcoord picketing1_val='72~900' picketing1_comm='Рыбинск' />"
		L"<rwcoord picketing1_val='72~900' picketing1_comm='Рыбинск' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T083500Z' name='Span_stopping_end' Bdg='guiNote[31139:31151]'>"
		L"<rwcoord picketing1_val='72~900' picketing1_comm='Рыбинск' />"
		L"<rwcoord picketing1_val='72~900' picketing1_comm='Рыбинск' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T084000Z' name='Arrival' Bdg='ASOUP 1042_1p2w[31139]' waynum='2' parknum='1' />"
		L"<SpotEvent create_time='20220607T085200Z' name='Departure' Bdg='ASOUP 1042_1p2w[31139]' waynum='2' parknum='1' optCode='31132:31139' />"
		L"<SpotEvent create_time='20220607T085909Z' name='Transition' Bdg='?p1w1[31132]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20220607T090700Z' name='Arrival' Bdg='ASOUP 1042_1p1w[31130]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20220607T090700Z' name='Death' Bdg='ASOUP 1042_1p1w[31130]' waynum='1' parknum='1' />"
		L"</HemPath>"
		);

	auto arrival31130 = L"(:1042 928/400+31130 2307 3100 495 0300 01 31132 07 06 12 07 01/01 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 057 02092 01161 152 55560692 61343778 000 000 00 038 000.20 001 000 000 60 013 000 000 90 024 000 000 95 001 000 000 96 023 000 000\n"
		L"Ю3 629 00000371 1 03 45 0104 52875 ПOЛEЩУK      00000 040 2805 00054208 0 1 00 00 00 00\n"
		L"Ю3 629 00000372 9\n"
		L"Ю4 05110 2 01 037 000 000.20 001 000 000 60 013 000 000 90 023 000 000 96 023 000 000\n"
		L"Ю4 05110 2 08 001 000 000.90 001 000 000 95 001 000 000\n"
		L"Ю6 55560692 20220611\n"
		L"Ю6 62675152 20220612\n"
		L"Ю6 53100962 20220611\n"
		L"Ю6 56594336 20220612\n"
		L"Ю6 52407335 20220614\n"
		L"Ю6 95006656 20220615\n"
		L"Ю6 98162886 20220612\n"
		L"Ю6 98162910 20220612\n"
		L"Ю6 94947678 20220612\n"
		L"Ю6 98161276 20220612\n"
		L"Ю6 54001599 20220612\n"
		L"Ю6 98014202 20220612\n"
		L"Ю6 98191224 20220612\n"
		L"Ю6 98127525 20220612\n"
		L"Ю6 98101819 20220612\n"
		L"Ю6 98122419 20220612\n"
		L"Ю6 98135965 20220612\n"
		L"Ю6 98133275 20220612\n"
		L"Ю6 98136120 20220612\n"
		L"Ю6 98136138 20220612\n"
		L"Ю6 98163017 20220612\n"
		L"Ю6 98161219 20220612\n"
		L"Ю6 98161227 20220612\n"
		L"Ю6 98161185 20220612\n"
		L"Ю6 98161193 20220612\n"
		L"Ю6 98161284 20220612\n"
		L"Ю6 98123045 20220612\n"
		L"Ю6 98018625 20220612\n"
		L"Ю6 98301294 20220612\n"
		L"Ю6 55984272 20220618\n"
		L"Ю6 60536372 20220618\n"
		L"Ю6 53636858 20220618\n"
		L"Ю6 63785364 20220618\n"
		L"Ю6 56403496 20220618\n"
		L"Ю6 56423197 20220618\n"
		L"Ю6 54789474 20220618\n"
		L"Ю6 56415599 20220618\n"
		L"Ю6 61343778 20220618\n"
		L"Ю8 3100 495 0300 31130 01 07 06 12 07 2307 310005 495 030006\n"
		L"Ю12 00 55560692 1 065 03080 32416 5588 000 00 00 00 00 00 0244 20 60 5600 04 100 03000 03070 05110 03 03000 09 81760 5010 020 0 0000 Э      132 000 10000000\n"
		L"Ю12 00 62675152 151237215255 052035001296023706000350002\n"
		L"Ю12 00 53100962 151237215255 064030805588024056000307003\n"
		L"Ю12 00 56594336 151239215247 061035806265024503580020     \n"
		L"Ю12 00 52407335 133249084127 0680309050103168202205204106030700335222802887\n"
		L"Ю12 00 95006656 133233212039 050081900210718520402259509500330001416911060660277600ЗEPHO \n"
		L"Ю12 00 98162886 129201084039 00803690003006141305000202509609441810374003121910076590200000Э     \n"
		L"Ю12 00 98162910 255255255255\n"
		L"Ю12 00 94947678 255253127255 0945185\n"
		L"Ю12 00 98161276 255253127255 0944181\n"
		L"Ю12 00 54001599 255253255255 5933\n"
		L"Ю12 00 98014202 255253255255 0944\n"
		L"Ю12 00 98191224 255255255255\n"
		L"Ю12 00 98127525 255255255255\n"
		L"Ю12 00 98101819 255255255255\n"
		L"Ю12 00 98122419 255255255255\n"
		L"Ю12 00 98135965 255255255255\n"
		L"Ю12 00 98133275 255255255255\n"
		L"Ю12 00 98136120 255255255255\n"
		L"Ю12 00 98136138 255255255255\n"
		L"Ю12 00 98163017 255255255255\n"
		L"Ю12 00 98161219 255255255255\n"
		L"Ю12 00 98161227 255255255255\n"
		L"Ю12 00 98161185 255255255255\n"
		L"Ю12 00 98161193 255255255255\n"
		L"Ю12 00 98161284 255255255255\n"
		L"Ю12 00 98123045 255255255255\n"
		L"Ю12 00 98018625 255255255255\n"
		L"Ю12 00 98301294 255237127255 02550945185\n"
		L"Ю12 00 55984272 131201084053 06707640487166302060000235605600100076300218230006302027XBH   008\n"
		L"Ю12 00 60536372 191237255247 069023606000     \n"
		L"Ю12 00 53636858 191237255255 06802355600\n"
		L"Ю12 00 63785364 191237255255 06902400600\n"
		L"Ю12 00 56403496 255237255255 02325600\n"
		L"Ю12 00 56423197 191239255247 0680231XBH   \n"
		L"Ю12 00 54789474 191239255247 06902350     \n"
		L"Ю12 00 56415599 191239255255 0700231\n"
		L"Ю12 00 61343778 191237255255 06902350600:)";

	CPPUNIT_ASSERT(CreatePath(*autoChart, path, 64));
	CPPUNIT_ASSERT(PassAsoupRawText(*autoChart, arrival31130, boost::gregorian::date(2022, 06, 07) ));
	autoChart->updateTime( time_from_iso("20220607T090800"), nullptr );

	AsoupLayer& al = autoChart->getAsoupLayer();
	auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
	CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
	auto asoup = *pathAsoupList.cbegin();
	CPPUNIT_ASSERT( al.IsServed( asoup ) );

	HappenLayer& hl = autoChart->getHappenLayer();

	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==16 );
		CPPUNIT_ASSERT( pathCreate->GetLastEvent()->GetBadge().num() == EsrKit(31130) );
	}
	auto asoupWithdrawal = L"(:333 1042 928/400+31130 2307 3100 495 0300 01:)";
	CPPUNIT_ASSERT(PassAsoupRawText(*autoChart, asoupWithdrawal, boost::gregorian::date(2022, 06, 07)));

	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==14 );
		CPPUNIT_ASSERT( pathCreate->GetLastEvent()->GetBadge().num() == EsrKit(31139) );

	}
}

void TC_AutoChartAsoup::WithdrawalAsoupMessageUndoGlueWithSpanStopping()
{
	auto path = loaderSpot(
		L"<HemPath>"
		L"<SpotEvent create_time='20220607T112100Z' name='Departure' Bdg='ASOUP 1042_3p1w[31000]' waynum='1' parknum='3' optCode='31000:31005' index='0258-006-2300' num='2531' length='52' weight='4837' net_weight='3591' through='Y'>"
		L"<feat_texts typeinfo='Т' />"
		L"<Locomotive Series='ВЛ11М' NumLoc='444' Depo='2851' Consec='1'>"
		L"<Crew EngineDriver='ЛOБAHOB' TabNum='12368' Tim_Beg='2022-06-07 09:50' />"
		L"</Locomotive>"
		L"<Locomotive Series='ВЛ11М' NumLoc='447' Depo='2851' Consec='9'>"
		L"<Crew EngineDriver='ЛOБAHOB' TabNum='12368' Tim_Beg='2022-06-07 09:50' />"
		L"</Locomotive>"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T112755Z' name='Transition' Bdg='?[31005]' />"
		L"<SpotEvent create_time='20220607T113100Z' name='Span_stopping_begin' Bdg='guiNote[31001:31005]'>"
		L"<rwcoord picketing1_val='279~725' picketing1_comm='Москва' />"
		L"<rwcoord picketing1_val='279~725' picketing1_comm='Москва' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T113200Z' name='Span_stopping_end' Bdg='guiNote[31001:31005]'>"
		L"<rwcoord picketing1_val='279~725' picketing1_comm='Москва' />"
		L"<rwcoord picketing1_val='279~725' picketing1_comm='Москва' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220607T113200Z' name='Death' Bdg='guiNote[31001:31005]'>"
		L"<rwcoord picketing1_val='279~725' picketing1_comm='Москва' />"
		L"<rwcoord picketing1_val='279~725' picketing1_comm='Москва' />"
		L"</SpotEvent>"
		L"</HemPath>"
		);

	auto departure31000 = 
//		<AsoupEvent create_time="20220607T112100Z" name="Departure" Bdg="ASOUP 1042_3p1w[31000]" index="0258-006-2300" num="2531" length="52" weight="4837" net_weight="3591" through="Y" waynum="1" parknum="3" dirTo="23700" adjTo="31005" linkBadge="ASOUP 1042_3p1w[31000]" linkTime="20220607T112100Z" linkCode="Departure"><![CDATA[(:1042 928/400+31000 2531 0258 006 2300 03 23700 07 06 14 21 03/01 0 0/00 00 0
		L"(:1042 928/400+31000 2531 0258 006 2300 03 23700 07 06 14 21 03/01 0 0/00 00 0\n"
		L"Ю2 0  T 00 00 00 0000 0 0 052 04837 03591 208 56134166 64563380 000 000 00 052 000.60 052 000 000\n"
		L"Ю3 153 00004441 1 09 50 2851 12368 ЛOБAHOB      00000 018 2801 00040808 5 1 00 00 00 00\n"
		L"Ю3 153 00004442 9\n"
		L"Ю3 153 00004471 9\n"
		L"Ю4 23700 2 24 017 000 000.60 017 000 000\n"
		L"Ю4 23700 2 17 035 000 000.60 035 000 000\n"
		L"Ю6 56134166 20220618\n"
		L"Ю6 55333553 20220618\n"
		L"Ю6 53417192 20220618\n"
		L"Ю6 55193031 20220618\n"
		L"Ю6 60099181 20220617\n"
		L"Ю6 57597775 20220617\n"
		L"Ю6 60182805 20220617\n"
		L"Ю6 62123732 20220617\n"
		L"Ю6 62179403 20220617\n"
		L"Ю6 60065364 20220617\n"
		L"Ю6 60416666 20220617\n"
		L"Ю6 63366926 20220617\n"
		L"Ю6 62398284 20220617\n"
		L"Ю6 59607978 20220617\n"
		L"Ю6 56333909 20220618\n"
		L"Ю6 56379514 20220618\n"
		L"Ю6 56925936 20220618\n"
		L"Ю6 55393110 20220618\n"
		L"Ю6 63838015 20220618\n"
		L"Ю6 63027783 20220618\n"
		L"Ю6 62721048 20220618\n"
		L"Ю6 58438482 20220618\n"
		L"Ю6 60647120 20220618\n"
		L"Ю6 56530090 20220618\n"
		L"Ю6 56433816 20220619\n"
		L"Ю6 56115462 20220619\n"
		L"Ю6 61153151 20220619\n"
		L"Ю6 62246335 20220619\n"
		L"Ю6 64312648 20220619\n"
		L"Ю6 62811286 20220619\n"
		L"Ю6 62834080 20220619\n"
		L"Ю6 64285638 20220619\n"
		L"Ю6 62808100 20220619\n"
		L"Ю6 60082682 20220619\n"
		L"Ю6 58059445 20220620\n"
		L"Ю6 59526715 20220620\n"
		L"Ю6 55849574 20220620\n"
		L"Ю6 55937932 20220620\n"
		L"Ю6 53103859 20220620\n"
		L"Ю6 57962235 20220620\n"
		L"Ю6 56134950 20220620\n"
		L"Ю6 60188711 20220620\n"
		L"Ю6 56070345 20220620\n"
		L"Ю6 62019609 20220620\n"
		L"Ю6 64186067 20220618\n"
		L"Ю6 60367703 20220618\n"
		L"Ю6 62421748 20220618\n"
		L"Ю6 60226008 20220618\n"
		L"Ю6 57526477 20220618\n"
		L"Ю6 61616678 20220618\n"
		L"Ю6 61323473 20220618\n"
		L"Ю6 64563380 20220618\n"
		L"Ю8 0258 006 2300 31000 62 07 06 14 21 2531 025807 006 230008\n"
		L"Ю12 00 56134166 1 068 19290 23239 3390 000 00 00 00 00 00 0240 20 60 5600 04 100 23000 19000 23700 01 23000 19 02580 6302 020 0 0000 Э      132 000 10000000\n"
		L"Ю12 00 55333553 191239255255 0690235\n"
		L"Ю12 00 53417192 255239255255 0234\n"
		L"Ю12 00 55193031 191239255255 0680242\n"
		L"Ю12 00 60099181 199237223255 240502324365710240060024000\n"
		L"Ю12 00 57597775 255237255255 02325600\n"
		L"Ю12 00 60182805 255237255255 02430600\n"
		L"Ю12 00 62123732 255239255255 0239\n"
		L"Ю12 00 62179403 255239255255 0240\n"
		L"Ю12 00 60065364 255239255255 0243\n"
		L"Ю12 00 60416666 255239255255 0245\n"
		L"Ю12 00 63366926 255239255255 0244\n"
		L"Ю12 00 62398284 255239255255 0245\n"
		L"Ю12 00 59607978 135237222255 06923050232394275023356002305002650\n"
		L"Ю12 00 56333909 211255215127 23820270930018000034554\n"
		L"Ю12 00 56379514 255239255255 0235\n"
		L"Ю12 00 56925936 255239255255 0234\n"
		L"Ю12 00 55393110 255239255255 0238\n"
		L"Ю12 00 63838015 191237255255 06802430600\n"
		L"Ю12 00 63027783 255239255255 0242\n"
		L"Ю12 00 62721048 255239255255 0248\n"
		L"Ю12 00 58438482 191237255255 06902365600\n"
		L"Ю12 00 60647120 255237255255 02370600\n"
		L"Ю12 00 56530090 191237255255 06802425600\n"
		L"Ю12 00 56433816 151239215255 06923020331802351938001\n"
		L"Ю12 00 56115462 255239255255 0233\n"
		L"Ю12 00 61153151 255237255255 02400600\n"
		L"Ю12 00 62246335 191239255254 074024410001000\n"
		L"Ю12 00 64312648 255239255255 0248\n"
		L"Ю12 00 62811286 255255255255\n"
		L"Ю12 00 62834080 255239255255 0249\n"
		L"Ю12 00 64285638 255239255255 0247\n"
		L"Ю12 00 62808100 255239255255 0249\n"
		L"Ю12 00 60082682 191239255254 068024010000000\n"
		L"Ю12 00 58059445 151237255255 06919980301402365600\n"
		L"Ю12 00 59526715 255239255255 0237\n"
		L"Ю12 00 55849574 255239255255 0236\n"
		L"Ю12 00 55937932 255255255255\n"
		L"Ю12 00 53103859 255239255255 0233\n"
		L"Ю12 00 57962235 255255255255\n"
		L"Ю12 00 56134950 191239255255 0680240\n"
		L"Ю12 00 60188711 255237255255 02420600\n"
		L"Ю12 00 56070345 191237255255 06902335600\n"
		L"Ю12 00 62019609 191237255255 06802370600\n"
		L"Ю12 00 64186067 151239215255 06926270626402382627002\n"
		L"Ю12 00 60367703 191239255255 0680242\n"
		L"Ю12 00 62421748 191239255255 0690237\n"
		L"Ю12 00 60226008 191239255255 0680243\n"
		L"Ю12 00 57526477 187237255255 06950002375600\n"
		L"Ю12 00 61616678 255237255255 02360600\n"
		L"Ю12 00 61323473 191239255255 0680243\n"
		L"Ю12 00 64563380 255239255255 0242:)";

	CPPUNIT_ASSERT(CreatePath(*autoChart, path, 64));
	CPPUNIT_ASSERT(PassAsoupRawText(*autoChart, departure31000, boost::gregorian::date(2022, 06, 07) ));
	autoChart->updateTime( time_from_iso("20220607T112100Z") + 30, nullptr );

	AsoupLayer& al = autoChart->getAsoupLayer();
	auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
	CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
	auto asoup = *pathAsoupList.cbegin();
	CPPUNIT_ASSERT( al.IsServed( asoup ) );

	UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( autoChart->getHappenLayer() );

	{
		CPPUNIT_ASSERT( hl.path_count() == 1 );
		CPPUNIT_ASSERT( hl.exist_path_size( 5 ) );
		CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "ASOUP 1042_3p1w[31000] ?[31005] guiNote[31001:31005] guiNote[31001:31005] guiNote[31001:31005] "));
	}

	auto asoupWithdrawal = L"(:333 1042 928/400+31000 2531 0258 006 2300 03:)";
	CPPUNIT_ASSERT(PassAsoupRawText(*autoChart, asoupWithdrawal, boost::gregorian::date(2022, 06, 07)));

	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		//нить была сгенерирована (по АСОУП + гидуральские пометки) - должна быть удалена
		CPPUNIT_ASSERT( pathSet.size() == 0 );
	}
}