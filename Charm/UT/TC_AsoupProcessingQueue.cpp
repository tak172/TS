#include "stdafx.h"
#include "TC_AsoupProcessingQueue.h"
#include "../helpful/Badge.h"
#include "../helpful/ParkWayKit.h"
#include "../helpful/StrToTime.h"
#include "../helpful/TrainDescr.h"
#include "../Hem/AsoupProcessingQueue.h"
#include "../Hem/AsoupEvent.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AsoupProcessingQueue );

typedef std::shared_ptr<const AsoupEvent> AsoupPtr;

void TC_AsoupProcessingQueue::single()
{
    const time_t T = time_from_iso("20180507T100000Z");
    EsrKit esr(12345);
    BadgeE badge(L"AsoupString", esr);
    AsoupPtr asoup1 = std::make_shared<AsoupEvent>(
        HCode::TRANSITION, badge, ParkWayKit(1,11), T,
        TrainDescr(L"9509", L"1234-567-8901" ) );
    AsoupPtr asoup2 = std::make_shared<AsoupEvent>(
        HCode::TRANSITION, badge, ParkWayKit(1,22), T+100,
        TrainDescr(L"7777", L"7777-567-8901" ) );


    Hem::AsoupProcessingQueue jobs;
    CPPUNIT_ASSERT( jobs.empty() );
    const unsigned BEG = 10;
// ?????    jobs.pushForLaterProcessing( asoup1, BEG );
//     jobs.pushForLaterProcessing( asoup2, BEG );
//     CPPUNIT_ASSERT( nullptr == jobs.pop( T + BEG-1 ) );
//     CPPUNIT_ASSERT(   asoup1 == jobs.popFromLaterProcessingQueue( T + BEG ) );
//     CPPUNIT_ASSERT( nullptr == jobs.popFromLaterProcessingQueue( T + BEG+1 ) );
//     CPPUNIT_ASSERT(   asoup2 == jobs.popFromLaterProcessingQueue( T+100 + BEG ) );
//     CPPUNIT_ASSERT( jobs.empty() ); // лишь однажды
}

void TC_AsoupProcessingQueue::repeated()
{
    const time_t T = time_from_iso("20180507T100000Z");
    EsrKit esr(12345);
    BadgeE badge(L"AsoupString", esr);
    const time_t T1 = T;
    const time_t T2 = T+7;
    AsoupPtr asoup1 = std::make_shared<AsoupEvent>(
        HCode::TRANSITION, badge, ParkWayKit(1,11), T1,
        TrainDescr(L"9509", L"1234-567-8901" ) );
    AsoupPtr asoup2 = std::make_shared<AsoupEvent>(
        HCode::TRANSITION, badge, ParkWayKit(1,22), T2,
        TrainDescr(L"7777", L"7777-567-8901" ) );


    Hem::AsoupProcessingQueue jobs;
    CPPUNIT_ASSERT( jobs.empty() );
    const unsigned BEG1 = 11;
    const unsigned BEG2 = 13;
    const unsigned PER = 20;
//  ????   jobs.pushForLaterProcessing( asoup1, BEG1, PER, BEG1 + 3*PER );
//     jobs.pushForLaterProcessing( asoup2, BEG2, PER, BEG2 + 2*PER );

    std::pair<time_t,AsoupPtr> sample[]=
    {
        std::make_pair(T1+BEG1,      asoup1),
        std::make_pair(T1+BEG1+1*PER,asoup1),
        std::make_pair(T1+BEG1+2*PER,asoup1),
        std::make_pair(T1+BEG1+3*PER,asoup1),
        std::make_pair(T2+BEG2,      asoup2),
        std::make_pair(T2+BEG2+1*PER,asoup2),
        std::make_pair(T2+BEG2+2*PER,asoup2),
    };
    std::sort( std::begin(sample), std::end(sample) );

// ????    CPPUNIT_ASSERT( nullptr == jobs.popFromLaterProcessingQueue( sample[0].first - 1 ) );
//     for( auto& s : sample )
//         CPPUNIT_ASSERT( s.second == jobs.popFromLaterProcessingQueue( s.first ) );
//     CPPUNIT_ASSERT( jobs.empty() );
}
