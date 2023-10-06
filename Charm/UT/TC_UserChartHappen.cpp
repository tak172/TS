#include "stdafx.h"
#include "TC_UserChartHappen.h"
#include "../UT/AutoChartTest.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/UserChart.h"

using namespace std;
using namespace HemHelpful;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_UserChartHappen );

TC_UserChartHappen::TC_UserChartHappen()
{
	userChart.reset( new UserChart() );
}

TC_UserChartHappen::~TC_UserChartHappen()
{
	HemEventRegistry::Shutdowner();
}

void TC_UserChartHappen::ReduceAmount()
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
    unsigned const id1 = 64;
    SpotEventPtr path1[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE,  BadgeE(L"1U",K),            F(20,20) ) ),
		SpotEventPtr( new SpotEvent( HCode::TRANSITION, BadgeE(L"2U",EsrKit(2000)), F(22,22) ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,      BadgeE(L"2U",EsrKit(2000)), F(22,22) ) ),
	};
    // утренняя нить
    unsigned const id2 = 65;
    SpotEventPtr path2[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE,  BadgeE(L"3U",K), F( 7,17) ) ),
        SpotEventPtr( new SpotEvent( HCode::TRANSITION, BadgeE(L"2U",EsrKit(2000)), F( 9,39) ) ),
        SpotEventPtr( new SpotEvent( HCode::TRANSITION, BadgeE(L"2U",EsrKit(3000)), F(11,40) ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,      BadgeE(L"2U",EsrKit(3000)), F(11,40) ) ),
    };
    // нить с утра до вечера
    unsigned const id3 = 66;
    SpotEventPtr path3[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE,  BadgeE(L"6U",K), F( 8,48) ) ),
        SpotEventPtr( new SpotEvent( HCode::TRANSITION, BadgeE(L"2U",EsrKit(2000)), F(11,11) ) ),
        SpotEventPtr( new SpotEvent( HCode::TRANSITION, BadgeE(L"2U",EsrKit(3000)), F(18,18) ) ),
        SpotEventPtr( new SpotEvent( HCode::TRANSITION, BadgeE(L"2U",EsrKit(4000)), F(21,21) ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,      BadgeE(L"2U",EsrKit(4000)), F(21,21) ) ),
    };

    CPPUNIT_ASSERT(CreatePath(*userChart, path1, id1));
    CPPUNIT_ASSERT(CreatePath(*userChart, path2, id2));
    CPPUNIT_ASSERT(CreatePath(*userChart, path3, id3));

    // до сжатия графика - все нити есть
    {
        attic::a_document out_doc("test");
        userChart->serialize(out_doc.document_element());
        CPPUNIT_ASSERT( !out_doc.select_nodes("//HemPath[count(SpotEvent)=3]").empty() );
        CPPUNIT_ASSERT( !out_doc.select_nodes("//HemPath[count(SpotEvent)=4]").empty() );
        CPPUNIT_ASSERT( !out_doc.select_nodes("//HemPath[count(SpotEvent)=5]").empty() );
    }
    // само сжатие
    userChart->reduceEvents( F(13, 0), 0 );
    userChart->reduceTransceiver( F(13, 0) );
    // применить и извлечь все изменения
    userChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
    // после сжатия - только нити в последних сутках
    {
        attic::a_document out_doc("test");
        userChart->serialize(out_doc.document_element());
        CPPUNIT_ASSERT( !out_doc.select_nodes("//HemPath[count(SpotEvent)=3]").empty() );
        CPPUNIT_ASSERT( out_doc.select_nodes( "//HemPath[count(SpotEvent)=4]").empty() );
        CPPUNIT_ASSERT( !out_doc.select_nodes("//HemPath[count(SpotEvent)=5]").empty() );
    }
}

void TC_UserChartHappen::Rectify()
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    const EsrKit K(1100,1200);
    const time_t T=1234567;

    // нить из двух событий
    unsigned const id2 = 64;
    SpotEventPtr path2[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"1U",K), T+1 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"2U",K), T+2 ) ),
    };
    // нить из четырех событий
    unsigned const id4 = 65;
    SpotEventPtr path4[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"6U",K), T+3 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"7U",K), T+4 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"8U",K), T+5 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"9U",K), T+6 ) ),
    };

    CPPUNIT_ASSERT(CreatePath(*userChart, path2, id2));
    CPPUNIT_ASSERT(CreatePath(*userChart, path4, id4));

    // применить и извлечь все изменения
    userChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
    // проверим, что нить потеряла промежуточные события в ходе заполнения
    attic::a_document out_doc("test");
    userChart->serialize(out_doc.document_element());
    CPPUNIT_ASSERT( !out_doc.select_nodes("//HemPath[count(SpotEvent)=2]").empty() );
    CPPUNIT_ASSERT( out_doc.select_nodes( "//HemPath[count(SpotEvent)=3]").empty() );
    CPPUNIT_ASSERT( out_doc.select_nodes( "//HemPath[count(SpotEvent)=4]").empty() );
}

void TC_UserChartHappen::SafelyDiscard()
{
    unsigned const tid = 64;

    BadgeE A(L"4p", EsrKit(100));
    BadgeE B(L"2p", EsrKit(200));

    CPPUNIT_ASSERT(userChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::FORM,      A, 33), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(userChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DEPARTURE, A, 44), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(userChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::ARRIVAL,   B, 55), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(userChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DISFORM,   B, 66), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(!userChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DISFORM,   B, 77), tid, TrainCharacteristics::Source::Guess));
    //нить уже была расформирована
    CPPUNIT_ASSERT(!userChart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DISFORM,   B, 77), tid, TrainCharacteristics::Source::Hem));
}
