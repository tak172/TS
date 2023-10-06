#include "stdafx.h"

#include "TC_Hem_aeSpotToRegulatory.h"
#include "TC_Hem_aeUserIdentify.h"
#include "TC_Hem_ae_helper.h"
#include "UtHelpfulDateTime.h"
#include "../Hem/Appliers/aeSpotToRegulatory.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/UncontrolledStations.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeSpotToRegulatory );

const EsrKit c_station1(100);
const EsrKit c_station2(200);
const EsrKit c_station3(300);
const EsrKit c_station4(400);
const EsrKit c_station5(500);

const HCode c_codeArrival = HCode::ARRIVAL;
const HCode c_codeDeparture = HCode::DEPARTURE;
const HCode c_codeTransition = HCode::TRANSITION;
const HCode c_codeForm = HCode::FORM;
const HCode c_codeDisform = HCode::DISFORM;
const HCode c_codeStationEntry = HCode::STATION_ENTRY;
const HCode c_codeStationExit = HCode::STATION_EXIT;
const HCode c_codeSpanMove = HCode::SPAN_MOVE;

using namespace HemHelpful;

void TC_Hem_aeSpotToRegulatory::setUp()
{
    c_time1 = timeForDateTime(2015, 8, 30, 10, 10);
    c_time2 = timeForDateTime(2015, 8, 30, 10, 20);
    c_time3 = timeForDateTime(2015, 8, 30, 10, 30);
    c_time4 = timeForDateTime(2015, 8, 30, 10, 40);
    c_time5 = timeForDateTime(2015, 8, 30, 10, 50);

    SpotEvent path1[] = {
        SpotEvent( c_codeForm, BadgeE(L"1p", c_station1), c_time1 ),
        SpotEvent( c_codeDeparture, BadgeE(L"1p", c_station1), c_time1 ),
        SpotEvent( c_codeTransition, BadgeE(L"2p", c_station2), c_time2 ),
        SpotEvent( c_codeTransition, BadgeE(L"3p", c_station3), c_time3 ),
        SpotEvent( c_codeTransition, BadgeE(L"3p", c_station4), c_time4 ),
        SpotEvent( c_codeTransition, BadgeE(L"4p", c_station5), c_time5 )
    };

    SpotEvent path2[] = {
        SpotEvent( c_codeForm, BadgeE(L"1p", c_station1), c_time1 + 10 + 60 ),
        SpotEvent( c_codeDeparture,  BadgeE(L"1p", c_station1), c_time1 + 15 + 60 ),
        SpotEvent( c_codeTransition, BadgeE(L"2p", c_station2), c_time2 + 20 + 60 ),
        SpotEvent( c_codeTransition, BadgeE(L"3p", c_station3), c_time3 + 30 + 60 ),
        SpotEvent( c_codeTransition, BadgeE(L"4p", c_station4), c_time4 + 40 + 60 )
    };

    c_time2_1 = timeForDateTime(2015, 8, 30, 11, 00);
    c_time2_2 = timeForDateTime(2015, 8, 30, 12, 00);
    c_time2_3 = timeForDateTime(2015, 8, 30, 13, 00);
    c_time2_4 = timeForDateTime(2015, 8, 30, 14, 00);

    SpotEvent path3[] = {
        SpotEvent( c_codeForm, BadgeE(L"1p", c_station5), c_time2_1 ),
        SpotEvent( c_codeDeparture, BadgeE(L"1p", c_station5), c_time2_1 ),
        SpotEvent( c_codeTransition, BadgeE(L"1p", c_station4), c_time2_2 ),
        SpotEvent( c_codeTransition, BadgeE(L"1p", c_station3), c_time2_3 ),
        SpotEvent( c_codeTransition, BadgeE(L"1p", c_station2), c_time2_4 )
    };

    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    m_happenLayer->createPath(path1);
    m_happenLayer->createPath(path2);
    m_happenLayer->createPath(path3);
    CPPUNIT_ASSERT_EQUAL(3u, (unsigned)m_happenLayer->path_count());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2015-Aug-30 10:00:00/2015-Aug-31 10:00:00]'>\n"
        L"  <ScheduledPath>\n"
        L"    <SpotEvent name='Info_changing' Bdg='1p[100]' create_time='20150830T101000Z' index='' num='6024' fretran='Y' />\n" // c_time1
        L"    <SpotEvent name='Departure' Bdg='1p[100]' create_time='20150830T101000Z' />\n" // c_time1
        L"    <SpotEvent name='Transition' Bdg='2p[200]' create_time='20150830T102000Z' />\n" // c_time2
        L"    <SpotEvent name='Arrival' Bdg='3p[300]' create_time='20150830T103000Z' />\n" // c_time3
        L"    <SpotEvent name='Departure' Bdg='4p[400]' create_time='20150830T104000Z' />\n" // c_time4
        L"    <SpotEvent name='Arrival' Bdg='5p[500]' create_time='20150830T105000Z' />\n" // c_time5
        L"  </ScheduledPath>\n"
        L"  <ScheduledPath>\n"
        L"    <SpotEvent name='Info_changing' Bdg='1p[100]' create_time='20150830T101100Z' index='' num='2024' fretran='Y' />\n" // c_time1 (Номер 2024 не подходит)
        L"    <SpotEvent name='Departure' Bdg='1p[100]' create_time='20150830T101100Z' />\n" // c_time1
        L"    <SpotEvent name='Transition' Bdg='2p[200]' create_time='20150830T102100Z' />\n" // c_time2
        L"    <SpotEvent name='Arrival' Bdg='3p[300]' create_time='20150830T103100Z' />\n" // c_time3
        L"    <SpotEvent name='Departure' Bdg='4p[400]' create_time='20150830T104100Z' />\n" // c_time4
        L"    <SpotEvent name='Arrival' Bdg='5p[500]' create_time='20150830T105100Z' />\n" // c_time5
        L"  </ScheduledPath>\n"
        L"  <ScheduledPath>\n"
        L"    <SpotEvent name='Info_changing' Bdg='1p[500]' create_time='20150830T110000Z' index='' num='6048' fretran='Y' />\n" // c_time2_1
        L"    <SpotEvent name='Departure' Bdg='1p[500]' create_time='20150830T110000Z' />\n" // c_time2_1
        L"    <SpotEvent name='Transition' Bdg='1p[400]' create_time='20150830T120000Z' />\n" // c_time2_2
        L"    <SpotEvent name='Arrival' Bdg='3p[300]' create_time='20150830T130000Z' />\n" // c_time2_3
        L"    <SpotEvent name='Departure' Bdg='4p[200]' create_time='20150830T140000Z' />\n" // c_time2_4
        L"  </ScheduledPath>\n"
        L"</ScheduledLayer>\n"
        L"</export>\n";
    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
        auto strPtr = m_regulatoryLayer->GetFlatContent();
    }

    CPPUNIT_ASSERT(!GetPath(c_time1, c_time5, 0).empty());
    CPPUNIT_ASSERT(!GetPath(c_time1, c_time5, 1).empty());
    CPPUNIT_ASSERT(!GetPath(c_time2_1, c_time2_4, 0).empty());
    m_uncontrolledStations.reset( new UncontrolledStations() );
}


HappenLayer::EventWithDescrList TC_Hem_aeSpotToRegulatory::GetPath(time_t from, time_t to, unsigned short number)
{
    std::list<HappenLayer::EventWithDescrList> pathes = m_happenLayer->GetPaths_EventAndDescr(from, to);
    CPPUNIT_ASSERT(number < (unsigned short)pathes.size());

    std::vector<HappenLayer::EventWithDescrList> pathesVec(pathes.begin(), pathes.end());
    std::sort(pathesVec.begin(), pathesVec.end(), [](const HappenLayer::EventWithDescrList& first, const HappenLayer::EventWithDescrList& second) {
        if (first.empty())
            return !second.empty();
        if (second.empty())
            return false;
        return (*first.front().first) < (*second.front().first);
    });

    return pathesVec[number];
}

void TC_Hem_aeSpotToRegulatory::DontIdentify()
{
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeDeparture, BadgeE(L"1p", c_station1), c_time1);
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(!applier.identifiedPath());
}

void TC_Hem_aeSpotToRegulatory::Identify()
{
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeTransition, BadgeE(L"4p", c_station5), c_time5);
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    HappenLayer::EventWithDescrList path1 = GetPath(c_time1, c_time5, 0u);
    CPPUNIT_ASSERT(!path1.empty());
    auto path1Begin = path1.begin();
    std::advance(path1Begin, 1);
    bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
        return spot.second && spot.second->GetNumber().getNumber() == 6024;
    });
    CPPUNIT_ASSERT(correct);
}

void TC_Hem_aeSpotToRegulatory::Identify2()
{
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeTransition, BadgeE(L"1p", c_station2), c_time2_4);
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    HappenLayer::EventWithDescrList path1 = GetPath(c_time2_1, c_time2_4, 0u);
    CPPUNIT_ASSERT(!path1.empty());
    auto path1Begin = path1.begin();
    std::advance(path1Begin, 1);
    bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
        return spot.second && spot.second->GetNumber().getNumber() == 6048;
    });
    CPPUNIT_ASSERT(correct);
}

void TC_Hem_aeSpotToRegulatory::IdentifyUsingCorrectEvents()
{
    // Пересоздаем слой исполненного расписания с лишними событиями (входа/выхода со станции)
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    {
        SpotEvent path1[] = {
            SpotEvent( c_codeForm, BadgeE(L"1p", c_station1), c_time1 ),
            SpotEvent( c_codeDeparture, BadgeE(L"1p", c_station1), c_time1 ),
            SpotEvent( c_codeStationExit, BadgeE(L"1p", c_station1), c_time1 + 4 * 60),
            SpotEvent( c_codeStationEntry, BadgeE(L"2p", c_station2), c_time2 - 4 * 60),
            SpotEvent( c_codeTransition, BadgeE(L"2p", c_station2), c_time2 ),
            SpotEvent( c_codeStationExit, BadgeE(L"2p", c_station2), c_time2 + 4 * 60),
            SpotEvent( c_codeTransition, BadgeE(L"3p", c_station3), c_time3 ),
            SpotEvent( c_codeTransition, BadgeE(L"3p", c_station4), c_time4 ),
            SpotEvent( c_codeTransition, BadgeE(L"4p", c_station5), c_time5 )
        };
        m_happenLayer->createPath(path1);
    }
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(c_time1, c_time5, 0).empty());

    // Идентифицируем нить как в тесте Identify()
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeTransition, BadgeE(L"4p", c_station5), c_time5);
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(c_time1, c_time5, 0u);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return spot.second && spot.second->GetNumber().getNumber() == 6024;
        });
        CPPUNIT_ASSERT(correct);
    }
}

void TC_Hem_aeSpotToRegulatory::IdentifyByStationAndSpandir1()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2018, 4, 28, 1, 48);
    time_t time2 = timeForDateTime(2018, 4, 28, 1, 49);
    {
        SpotEvent path1[] = {
            SpotEvent( c_codeForm, BadgeE(L"1CC[09750]", EsrKit(9750)), time1 ),
            SpotEvent( c_codeDeparture, BadgeE(L"1CC[09750]", EsrKit(9750)), time1 ),
            SpotEvent( c_codeSpanMove, BadgeE(L"1p", EsrKit(9736,9750)), time2 ),
        };
        m_happenLayer->createPath(path1);
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time1+10*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2018-Apr-27 03:00:00/2018-Apr-29 02:59:59.999997]'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180428T014700Z' name='Info_changing' Bdg='№6502[09750]' index='' num='6502' suburbreg='Y' />"
        L"<SpotEvent create_time='20180428T014700Z' name='Departure' Bdg='№6502[09750]' />"
        L"<SpotEvent create_time='20180428T015100Z' name='Arrival' Bdg='№6502[09736]' />"
        L"<SpotEvent create_time='20180428T015200Z' name='Departure' Bdg='№6502[09736]' />"
        L"<SpotEvent create_time='20180428T020900Z' name='Arrival' Bdg='№6502[09732]' />"
        L"<SpotEvent create_time='20180428T021000Z' name='Departure' Bdg='№6502[09732]' />"
        L"<SpotEvent create_time='20180428T021830Z' name='Arrival' Bdg='№6502[09730]' />"
        L"<SpotEvent create_time='20180428T021930Z' name='Departure' Bdg='№6502[09730]' />"
        L"<SpotEvent create_time='20180428T023630Z' name='Arrival' Bdg='№6502[09715]' />"
        L"<SpotEvent create_time='20180428T023730Z' name='Departure' Bdg='№6502[09715]' />"
        L"<SpotEvent create_time='20180428T024830Z' name='Arrival' Bdg='№6502[09676]' />"
        L"<SpotEvent create_time='20180428T024900Z' name='Departure' Bdg='№6502[09676]' />"
        L"<SpotEvent create_time='20180428T030130Z' name='Arrival' Bdg='№6502[09670]' />"
        L"<SpotEvent create_time='20180428T030230Z' name='Departure' Bdg='№6502[09670]' />"
        L"<SpotEvent create_time='20180428T030600Z' name='Arrival' Bdg='№6502[09100]' />"
        L"<SpotEvent create_time='20180428T030700Z' name='Departure' Bdg='№6502[09100]' />"
        L"<SpotEvent create_time='20180428T031100Z' name='Arrival' Bdg='№6502[09010]' />"
        L"</ScheduledPath>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180428T021300Z' name='Info_changing' Bdg='№7701[09732]' index='' num='7701' temot='Y' />"
        L"<SpotEvent create_time='20180428T021300Z' name='Departure' Bdg='№7701[09732]' />"
        L"<SpotEvent create_time='20180428T022800Z' name='Transition' Bdg='№7701[09736]' />"
        L"<SpotEvent create_time='20180428T023100Z' name='Arrival' Bdg='№7701[09750]' />"
        L"</ScheduledPath>"
        L"</ScheduledLayer>"
        L"</export>";
    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }


    // Идентифицируем нить как в тесте Identify()
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeDeparture, BadgeE(L"1CC[09750]", EsrKit(9750)), time1);
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, false, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(time1, time1+10*60, 0);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return spot.second && spot.second->GetNumber().getNumber() == 6502;
        });
        CPPUNIT_ASSERT(correct);
    }

}

void TC_Hem_aeSpotToRegulatory::IdentifyByStationAndSpandir2()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2018, 4, 28, 3, 9);
    time_t time2 = timeForDateTime(2018, 4, 28, 3, 11);
    {
        SpotEvent path1[] = {
            SpotEvent( c_codeForm, BadgeE(L"1CC", EsrKit(9010)), time1 ),
            SpotEvent( c_codeDeparture, BadgeE(L"1CC", EsrKit(9010)), time1 ),
            SpotEvent( c_codeSpanMove, BadgeE(L"V8P", EsrKit(9010, 9100)), time2 ),
        };
        m_happenLayer->createPath(path1);
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time1+10*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2018-Apr-27 03:00:00/2018-Apr-29 02:59:59.999997]'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180428T030800Z' name='Info_changing' Bdg='№6703[09010]' index='' num='6703' suburbreg='Y' />"
        L"<SpotEvent create_time='20180428T030800Z' name='Departure' Bdg='№6703[09010]' />"
        L"<SpotEvent create_time='20180428T031200Z' name='Arrival' Bdg='№6703[09100]' />"
        L"<SpotEvent create_time='20180428T031300Z' name='Departure' Bdg='№6703[09100]' />"
        L"<SpotEvent create_time='20180428T031820Z' name='Transition' Bdg='№6703[09104]' />"
        L"<SpotEvent create_time='20180428T033300Z' name='Arrival' Bdg='№6703[09150]' />"
        L"<SpotEvent create_time='20180428T033400Z' name='Departure' Bdg='№6703[09150]' />"
        L"<SpotEvent create_time='20180428T034400Z' name='Arrival' Bdg='№6703[09160]' />"
        L"<SpotEvent create_time='20180428T034430Z' name='Departure' Bdg='№6703[09160]' />"
        L"<SpotEvent create_time='20180428T035230Z' name='Arrival' Bdg='№6703[09162]' />"
        L"<SpotEvent create_time='20180428T035330Z' name='Departure' Bdg='№6703[09162]' />"
        L"<SpotEvent create_time='20180428T035700Z' name='Arrival' Bdg='№6703[09180]' />"
        L"</ScheduledPath>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180428T031200Z' name='Info_changing' Bdg='№6401[09010]' index='' num='6401' suburbreg='Y' />"
        L"<SpotEvent create_time='20180428T031200Z' name='Departure' Bdg='№6401[09010]' />"
        L"<SpotEvent create_time='20180428T031600Z' name='Arrival' Bdg='№6401[09100]' />"
        L"<SpotEvent create_time='20180428T031700Z' name='Departure' Bdg='№6401[09100]' />"
        L"<SpotEvent create_time='20180428T032030Z' name='Arrival' Bdg='№6401[09670]' />"
        L"<SpotEvent create_time='20180428T032130Z' name='Departure' Bdg='№6401[09670]' />"
        L"<SpotEvent create_time='20180428T033330Z' name='Arrival' Bdg='№6401[09676]' />"
        L"<SpotEvent create_time='20180428T033400Z' name='Departure' Bdg='№6401[09676]' />"
        L"<SpotEvent create_time='20180428T034600Z' name='Arrival' Bdg='№6401[09715]' />"
        L"<SpotEvent create_time='20180428T034700Z' name='Departure' Bdg='№6401[09715]' />"
        L"<SpotEvent create_time='20180428T040100Z' name='Arrival' Bdg='№6401[09730]' />"
        L"</ScheduledPath>"
        L"</ScheduledLayer>"
        L"</export>";
    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }


    // Идентифицируем нить как в тесте Identify()
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeSpanMove, BadgeE(L"V8P", EsrKit(9010, 9100)), time2);
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, false, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(time1, time1+10*60, 0);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return spot.second && spot.second->GetNumber().getNumber() == 6703;
        });
        CPPUNIT_ASSERT(correct);
    }
}

void TC_Hem_aeSpotToRegulatory::IdentifyByStationAndSpandir3()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2018, 5, 5, 4, 55, 26);
    time_t time2 = timeForDateTime(2018, 5, 5, 4, 57, 30);
    {
        SpotEvent path1[] = {
            SpotEvent( c_codeForm, BadgeE(L"10C", EsrKit(9010)), time1 ),
            SpotEvent( c_codeDeparture, BadgeE(L"10C", EsrKit(9010)), time1 ),
            SpotEvent( c_codeSpanMove, BadgeE(L"1UPP", EsrKit(9010, 9500)), time2 ),
        };
        m_happenLayer->createPath(path1);
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time1+3*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2018-May-04 15:00:00/2018-May-06 14:59:59.999997]'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180505T045400Z' name='Info_changing' Bdg='№838[09010]' index='' num='838' mvps='Y' />"
        L"<SpotEvent create_time='20180505T045400Z' name='Departure' Bdg='№838[09010]' />"
        L"<SpotEvent create_time='20180505T050200Z' name='Arrival' Bdg='№838[09500]' />"
        L"<SpotEvent create_time='20180505T050300Z' name='Departure' Bdg='№838[09500]' />"
        L"<SpotEvent create_time='20180505T050700Z' name='Arrival' Bdg='№838[09580]' />"
        L"<SpotEvent create_time='20180505T050800Z' name='Departure' Bdg='№838[09580]' />"
        L"<SpotEvent create_time='20180505T051300Z' name='Arrival' Bdg='№838[09570]' />"
        L"<SpotEvent create_time='20180505T051400Z' name='Departure' Bdg='№838[09570]' />"
        L"<SpotEvent create_time='20180505T052630Z' name='Arrival' Bdg='№838[09550]' />"
        L"<SpotEvent create_time='20180505T052730Z' name='Departure' Bdg='№838[09550]' />"
        L"<SpotEvent create_time='20180505T053430Z' name='Arrival' Bdg='№838[09540]' />"
        L"<SpotEvent create_time='20180505T053530Z' name='Departure' Bdg='№838[09540]' />"
        L"<SpotEvent create_time='20180505T054200Z' name='Arrival' Bdg='№838[09530]' />"
        L"<SpotEvent create_time='20180505T054300Z' name='Departure' Bdg='№838[09530]' />"
        L"<SpotEvent create_time='20180505T054930Z' name='Arrival' Bdg='№838[09520]' />"
        L"<SpotEvent create_time='20180505T055030Z' name='Departure' Bdg='№838[09520]' />"
        L"<SpotEvent create_time='20180505T060800Z' name='Arrival' Bdg='№838[09511]' />"
        L"</ScheduledPath>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180506T045400Z' name='Info_changing' Bdg='№838[09010]' index='' num='838' mvps='Y' />"
        L"<SpotEvent create_time='20180506T045400Z' name='Departure' Bdg='№838[09010]' />"
        L"<SpotEvent create_time='20180506T050200Z' name='Arrival' Bdg='№838[09500]' />"
        L"<SpotEvent create_time='20180506T050300Z' name='Departure' Bdg='№838[09500]' />"
        L"<SpotEvent create_time='20180506T050700Z' name='Arrival' Bdg='№838[09580]' />"
        L"<SpotEvent create_time='20180506T050800Z' name='Departure' Bdg='№838[09580]' />"
        L"<SpotEvent create_time='20180506T051300Z' name='Arrival' Bdg='№838[09570]' />"
        L"<SpotEvent create_time='20180506T051400Z' name='Departure' Bdg='№838[09570]' />"
        L"<SpotEvent create_time='20180506T052630Z' name='Arrival' Bdg='№838[09550]' />"
        L"<SpotEvent create_time='20180506T052730Z' name='Departure' Bdg='№838[09550]' />"
        L"<SpotEvent create_time='20180506T053430Z' name='Arrival' Bdg='№838[09540]' />"
        L"<SpotEvent create_time='20180506T053530Z' name='Departure' Bdg='№838[09540]' />"
        L"<SpotEvent create_time='20180506T054200Z' name='Arrival' Bdg='№838[09530]' />"
        L"<SpotEvent create_time='20180506T054300Z' name='Departure' Bdg='№838[09530]' />"
        L"<SpotEvent create_time='20180506T054930Z' name='Arrival' Bdg='№838[09520]' />"
        L"<SpotEvent create_time='20180506T055030Z' name='Departure' Bdg='№838[09520]' />"
        L"<SpotEvent create_time='20180506T060800Z' name='Arrival' Bdg='№838[09511]' />"
        L"</ScheduledPath>"
        L"</ScheduledLayer>"
        L"</export>";
    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }


    // Идентифицируем нить как в тесте Identify()
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeSpanMove, BadgeE(L"1UPP", EsrKit(9010, 9500)), time2);
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, false, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(time1, time1+10*60, 0);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return spot.second && spot.second->GetNumber().getNumber() == 838;
        });
        CPPUNIT_ASSERT(correct);
    }
}


void TC_Hem_aeSpotToRegulatory::IdentifyOnlyForEqualBeginRoute()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2018, 5, 5, 3, 40, 27);
    time_t time2 = timeForDateTime(2018, 5, 5, 3, 42, 17);
    time_t time11000 = timeForDateTime(2018, 5, 5, 3, 15, 00);
    time_t time11002 = timeForDateTime(2018, 5, 5, 3, 17, 40);
    time_t time11451 = timeForDateTime(2018, 5, 5, 3, 19, 00);
    {
        SpotEvent path1[] = {
            SpotEvent( c_codeForm, BadgeE(L"1C", EsrKit(11446)), time1 ),
            SpotEvent( c_codeDeparture, BadgeE(L"1C", EsrKit(11446)), time1 ),
            SpotEvent( c_codeSpanMove, BadgeE(L"P16P", EsrKit(11432, 11446)), time2 ),
        };
        SpotEvent path2[] = {
            SpotEvent( c_codeForm, BadgeE(L"1C", EsrKit(11000)), time11000 - 50 ),
            SpotEvent( c_codeDeparture, BadgeE(L"1C", EsrKit(11000)), time11000 - 50 ),
            SpotEvent( c_codeTransition, BadgeE(L"1C", EsrKit(11002)), time11002 - 50 ),
            SpotEvent( c_codeTransition, BadgeE(L"1C", EsrKit(11451)), time11451 + 30 ),
            SpotEvent( c_codeSpanMove, BadgeE(L"P16P", EsrKit(11445, 11451)), time11451 + 60 ),
        };
        m_happenLayer->createPath(path1);
        m_happenLayer->createPath(path2);
    }

    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time1+3*60, 0).empty());
    CPPUNIT_ASSERT(!GetPath(time11000, time11451+3*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2018-May-04 15:00:00/2018-May-06 14:59:59.999997]'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180505T031500Z' name='Info_changing' Bdg='№703[11000]' index='' num='703' rapid='Y' />"
        L"<SpotEvent create_time='20180505T031500Z' name='Departure' Bdg='№703[11000]' />"
        L"<SpotEvent create_time='20180505T031740Z' name='Transition' Bdg='№703[11002]' />"
        L"<SpotEvent create_time='20180505T031900Z' name='Transition' Bdg='№703[11451]' />"
        L"<SpotEvent create_time='20180505T032300Z' name='Transition' Bdg='№703[11445]' />"
        L"<SpotEvent create_time='20180505T032700Z' name='Transition' Bdg='№703[11443]' />"
        L"<SpotEvent create_time='20180505T033000Z' name='Transition' Bdg='№703[11442]' />"
        L"<SpotEvent create_time='20180505T033800Z' name='Transition' Bdg='№703[11446]' />"
        L"<SpotEvent create_time='20180505T034200Z' name='Transition' Bdg='№703[11432]' />"
        L"<SpotEvent create_time='20180505T034700Z' name='Transition' Bdg='№703[11431]' />"
        L"<SpotEvent create_time='20180505T035600Z' name='Arrival' Bdg='№703[11430]' />"
        L"<SpotEvent create_time='20180505T035800Z' name='Departure' Bdg='№703[11430]' />"
        L"<SpotEvent create_time='20180505T040900Z' name='Transition' Bdg='№703[11422]' />"
        L"<SpotEvent create_time='20180505T041400Z' name='Transition' Bdg='№703[11419]' />"
        L"<SpotEvent create_time='20180505T042200Z' name='Arrival' Bdg='№703[11420]' />"
        L"<SpotEvent create_time='20180505T042300Z' name='Departure' Bdg='№703[11420]' />"
        L"<SpotEvent create_time='20180505T043700Z' name='Arrival' Bdg='№703[11760]' />"
        L"<SpotEvent create_time='20180505T043800Z' name='Departure' Bdg='№703[11760]' />"
        L"<SpotEvent create_time='20180505T045300Z' name='Arrival' Bdg='№703[09360]' />"
        L"<SpotEvent create_time='20180505T045400Z' name='Departure' Bdg='№703[09360]' />"
        L"<SpotEvent create_time='20180505T050400Z' name='Arrival' Bdg='№703[09370]' />"
        L"<SpotEvent create_time='20180505T050500Z' name='Departure' Bdg='№703[09370]' />"
        L"<SpotEvent create_time='20180505T051400Z' name='Transition' Bdg='№703[09380]' />"
        L"<SpotEvent create_time='20180505T052700Z' name='Transition' Bdg='№703[09400]' />"
        L"<SpotEvent create_time='20180505T053800Z' name='Arrival' Bdg='№703[09410]' />"
        L"<SpotEvent create_time='20180505T053900Z' name='Departure' Bdg='№703[09410]' />"
        L"<SpotEvent create_time='20180505T055100Z' name='Transition' Bdg='№703[09420]' />"
        L"<SpotEvent create_time='20180505T055800Z' name='Transition' Bdg='№703[09000]' />"
        L"<SpotEvent create_time='20180505T060000Z' name='Transition' Bdg='№703[09006]' />"
        L"<SpotEvent create_time='20180505T060700Z' name='Arrival' Bdg='№703[09010]' />"
        L"</ScheduledPath>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180506T031500Z' name='Info_changing' Bdg='№703[11000]' index='' num='703' rapid='Y' />"
        L"<SpotEvent create_time='20180506T031500Z' name='Departure' Bdg='№703[11000]' />"
        L"<SpotEvent create_time='20180506T031740Z' name='Transition' Bdg='№703[11002]' />"
        L"<SpotEvent create_time='20180506T031900Z' name='Transition' Bdg='№703[11451]' />"
        L"<SpotEvent create_time='20180506T032300Z' name='Transition' Bdg='№703[11445]' />"
        L"<SpotEvent create_time='20180506T032700Z' name='Transition' Bdg='№703[11443]' />"
        L"<SpotEvent create_time='20180506T033000Z' name='Transition' Bdg='№703[11442]' />"
        L"<SpotEvent create_time='20180506T033800Z' name='Transition' Bdg='№703[11446]' />"
        L"<SpotEvent create_time='20180506T034200Z' name='Transition' Bdg='№703[11432]' />"
        L"<SpotEvent create_time='20180506T034700Z' name='Transition' Bdg='№703[11431]' />"
        L"<SpotEvent create_time='20180506T035600Z' name='Arrival' Bdg='№703[11430]' />"
        L"<SpotEvent create_time='20180506T035800Z' name='Departure' Bdg='№703[11430]' />"
        L"<SpotEvent create_time='20180506T040900Z' name='Transition' Bdg='№703[11422]' />"
        L"<SpotEvent create_time='20180506T041400Z' name='Transition' Bdg='№703[11419]' />"
        L"<SpotEvent create_time='20180506T042200Z' name='Arrival' Bdg='№703[11420]' />"
        L"<SpotEvent create_time='20180506T042300Z' name='Departure' Bdg='№703[11420]' />"
        L"<SpotEvent create_time='20180506T043700Z' name='Arrival' Bdg='№703[11760]' />"
        L"<SpotEvent create_time='20180506T043800Z' name='Departure' Bdg='№703[11760]' />"
        L"<SpotEvent create_time='20180506T045300Z' name='Arrival' Bdg='№703[09360]' />"
        L"<SpotEvent create_time='20180506T045400Z' name='Departure' Bdg='№703[09360]' />"
        L"<SpotEvent create_time='20180506T050400Z' name='Arrival' Bdg='№703[09370]' />"
        L"<SpotEvent create_time='20180506T050500Z' name='Departure' Bdg='№703[09370]' />"
        L"<SpotEvent create_time='20180506T051400Z' name='Transition' Bdg='№703[09380]' />"
        L"<SpotEvent create_time='20180506T052700Z' name='Transition' Bdg='№703[09400]' />"
        L"<SpotEvent create_time='20180506T053800Z' name='Arrival' Bdg='№703[09410]' />"
        L"<SpotEvent create_time='20180506T053900Z' name='Departure' Bdg='№703[09410]' />"
        L"<SpotEvent create_time='20180506T055100Z' name='Transition' Bdg='№703[09420]' />"
        L"<SpotEvent create_time='20180506T055800Z' name='Transition' Bdg='№703[09000]' />"
        L"<SpotEvent create_time='20180506T060000Z' name='Transition' Bdg='№703[09006]' />"
        L"<SpotEvent create_time='20180506T060700Z' name='Arrival' Bdg='№703[09010]' />"
        L"</ScheduledPath>"
        L"</export>";

    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }


    // Идентифицируем нить как в тесте Identify()
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeSpanMove, BadgeE(L"P16P", EsrKit(11432, 11446)), time2);
    Hem::aeSpotToRegulatory applier1(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier1.Action();
    CPPUNIT_ASSERT(!applier1.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(time1, time1+10*60, 0);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return !spot.second;
        });
        CPPUNIT_ASSERT(correct);
    }

    spotPtr = std::make_shared<SpotEvent>(c_codeSpanMove, BadgeE(L"P16P", EsrKit(11445, 11451)), time11451 + 60);
    Hem::aeSpotToRegulatory applier2(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier2.Action();
    CPPUNIT_ASSERT(applier2.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(time11000, time11451+3*60, 0);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return spot.second && spot.second->GetNumber().getNumber() == 703;
        });
        CPPUNIT_ASSERT(correct);
    }
}

void TC_Hem_aeSpotToRegulatory::IdentifyByDeparture()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2018, 5, 5, 4, 56, 45);
    SpotDetails sd;
    sd.optCode = EsrKit(9010,9100);

    {
        SpotEvent path1[] = {
            SpotEvent( c_codeForm, BadgeE(L"1C", EsrKit(9010)), time1 ),
            SpotEvent( c_codeDeparture, BadgeE(L"1C", EsrKit(9010)), time1, std::make_shared<SpotDetails>(sd) ),
        };
        m_happenLayer->createPath(path1);
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time1+1*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2018-May-04 15:00:00/2018-May-06 14:59:59.999997]'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180505T045600Z' name='Info_changing' Bdg='№6407[09010]' index='' num='6407' suburbreg='Y' />"
        L"<SpotEvent create_time='20180505T045600Z' name='Departure' Bdg='№6407[09010]' />"
        L"<SpotEvent create_time='20180505T050000Z' name='Arrival' Bdg='№6407[09100]' />"
        L"<SpotEvent create_time='20180505T050100Z' name='Departure' Bdg='№6407[09100]' />"
        L"<SpotEvent create_time='20180505T050430Z' name='Arrival' Bdg='№6407[09670]' />"
        L"<SpotEvent create_time='20180505T050530Z' name='Departure' Bdg='№6407[09670]' />"
        L"<SpotEvent create_time='20180505T051800Z' name='Arrival' Bdg='№6407[09676]' />"
        L"<SpotEvent create_time='20180505T051830Z' name='Departure' Bdg='№6407[09676]' />"
        L"<SpotEvent create_time='20180505T053000Z' name='Arrival' Bdg='№6407[09715]' />"
        L"<SpotEvent create_time='20180505T053100Z' name='Departure' Bdg='№6407[09715]' />"
        L"<SpotEvent create_time='20180505T054500Z' name='Arrival' Bdg='№6407[09730]' />"
        L"</ScheduledPath>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180506T045600Z' name='Info_changing' Bdg='№6407[09010]' index='' num='6407' suburbreg='Y' />"
        L"<SpotEvent create_time='20180506T045600Z' name='Departure' Bdg='№6407[09010]' />"
        L"<SpotEvent create_time='20180506T050000Z' name='Arrival' Bdg='№6407[09100]' />"
        L"<SpotEvent create_time='20180506T050100Z' name='Departure' Bdg='№6407[09100]' />"
        L"<SpotEvent create_time='20180506T050430Z' name='Arrival' Bdg='№6407[09670]' />"
        L"<SpotEvent create_time='20180506T050530Z' name='Departure' Bdg='№6407[09670]' />"
        L"<SpotEvent create_time='20180506T051800Z' name='Arrival' Bdg='№6407[09676]' />"
        L"<SpotEvent create_time='20180506T051830Z' name='Departure' Bdg='№6407[09676]' />"
        L"<SpotEvent create_time='20180506T053000Z' name='Arrival' Bdg='№6407[09715]' />"
        L"<SpotEvent create_time='20180506T053100Z' name='Departure' Bdg='№6407[09715]' />"
        L"<SpotEvent create_time='20180506T054500Z' name='Arrival' Bdg='№6407[09730]' />"
        L"</ScheduledPath>"
        L"</export>";

    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }


    // Идентифицируем нить как в тесте Identify()
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeDeparture, BadgeE(L"1C", EsrKit(9010)), time1, std::make_shared<SpotDetails>(sd));
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, false, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(time1, time1+1*60, 0);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return spot.second && spot.second->GetNumber().getNumber() == 6407;
        });
        CPPUNIT_ASSERT(correct);
    }
}

void TC_Hem_aeSpotToRegulatory::IdentifyPathFromUncontrolledZone()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2018, 5, 2, 9, 39, 11);
    time_t time2 = timeForDateTime(2018, 5, 2, 9, 39, 19);
    time_t time3 = timeForDateTime(2018, 5, 2, 9, 43, 18);

    {
        SpotEvent path1[] = {
            SpotEvent( c_codeForm, BadgeE(L"1BP", EsrKit(9041,9650)), time1 ),
            SpotEvent( c_codeTransition, BadgeE(L"209SP+", EsrKit(9041)), time2 ),
            SpotEvent( c_codeTransition, BadgeE(L"1C", EsrKit(9500)), time3 ),
        };
        m_happenLayer->createPath(path1);
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time3+1*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2018-May-04 15:00:00/2018-May-06 14:59:59.999997]'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180502T084000Z' name='Info_changing' Bdg='№6121[09070]' index='' num='6121' suburbreg='Y' />"
        L"<SpotEvent create_time='20180502T084000Z' name='Departure' Bdg='№6121[09070]' />"
        L"<SpotEvent create_time='20180502T084900Z' name='Arrival' Bdg='№6121[09600]' />"
        L"<SpotEvent create_time='20180502T085000Z' name='Departure' Bdg='№6121[09600]' />"
        L"<SpotEvent create_time='20180502T085630Z' name='Arrival' Bdg='№6121[09602]' />"
        L"<SpotEvent create_time='20180502T085700Z' name='Departure' Bdg='№6121[09602]' />"
        L"<SpotEvent create_time='20180502T090200Z' name='Arrival' Bdg='№6121[09603]' />"
        L"<SpotEvent create_time='20180502T090230Z' name='Departure' Bdg='№6121[09603]' />"
        L"<SpotEvent create_time='20180502T090700Z' name='Arrival' Bdg='№6121[09611]' />"
        L"<SpotEvent create_time='20180502T090730Z' name='Departure' Bdg='№6121[09611]' />"
        L"<SpotEvent create_time='20180502T090930Z' name='Arrival' Bdg='№6121[09612]' />"
        L"<SpotEvent create_time='20180502T091030Z' name='Departure' Bdg='№6121[09612]' />"
        L"<SpotEvent create_time='20180502T092200Z' name='Arrival' Bdg='№6121[09616]' />"
        L"<SpotEvent create_time='20180502T092300Z' name='Departure' Bdg='№6121[09616]' />"
        L"<SpotEvent create_time='20180502T092730Z' name='Arrival' Bdg='№6121[09630]' />"
        L"<SpotEvent create_time='20180502T092830Z' name='Departure' Bdg='№6121[09630]' />"
        L"<SpotEvent create_time='20180502T093130Z' name='Arrival' Bdg='№6121[09640]' />"
        L"<SpotEvent create_time='20180502T093230Z' name='Departure' Bdg='№6121[09640]' />"
        L"<SpotEvent create_time='20180502T093530Z' name='Arrival' Bdg='№6121[09650]' />"
        L"<SpotEvent create_time='20180502T093630Z' name='Departure' Bdg='№6121[09650]' />"
        L"<SpotEvent create_time='20180502T093830Z' name='Arrival' Bdg='№6121[09041]' />"
        L"<SpotEvent create_time='20180502T093930Z' name='Departure' Bdg='№6121[09041]' />"
        L"<SpotEvent create_time='20180502T094130Z' name='Arrival' Bdg='№6121[09500]' />"
        L"<SpotEvent create_time='20180502T094230Z' name='Departure' Bdg='№6121[09500]' />"
        L"<SpotEvent create_time='20180502T094800Z' name='Arrival' Bdg='№6121[09010]' />"
        L"</ScheduledPath>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180503T084000Z' name='Info_changing' Bdg='№6121[09070]' index='' num='6121' suburbreg='Y' />"
        L"<SpotEvent create_time='20180503T084000Z' name='Departure' Bdg='№6121[09070]' />"
        L"<SpotEvent create_time='20180503T084900Z' name='Arrival' Bdg='№6121[09600]' />"
        L"<SpotEvent create_time='20180503T085000Z' name='Departure' Bdg='№6121[09600]' />"
        L"<SpotEvent create_time='20180503T085630Z' name='Arrival' Bdg='№6121[09602]' />"
        L"<SpotEvent create_time='20180503T085700Z' name='Departure' Bdg='№6121[09602]' />"
        L"<SpotEvent create_time='20180503T090200Z' name='Arrival' Bdg='№6121[09603]' />"
        L"<SpotEvent create_time='20180503T090230Z' name='Departure' Bdg='№6121[09603]' />"
        L"<SpotEvent create_time='20180503T090700Z' name='Arrival' Bdg='№6121[09611]' />"
        L"<SpotEvent create_time='20180503T090730Z' name='Departure' Bdg='№6121[09611]' />"
        L"<SpotEvent create_time='20180503T090930Z' name='Arrival' Bdg='№6121[09612]' />"
        L"<SpotEvent create_time='20180503T091030Z' name='Departure' Bdg='№6121[09612]' />"
        L"<SpotEvent create_time='20180503T092200Z' name='Arrival' Bdg='№6121[09616]' />"
        L"<SpotEvent create_time='20180503T092300Z' name='Departure' Bdg='№6121[09616]' />"
        L"<SpotEvent create_time='20180503T092730Z' name='Arrival' Bdg='№6121[09630]' />"
        L"<SpotEvent create_time='20180503T092830Z' name='Departure' Bdg='№6121[09630]' />"
        L"<SpotEvent create_time='20180503T093130Z' name='Arrival' Bdg='№6121[09640]' />"
        L"<SpotEvent create_time='20180503T093230Z' name='Departure' Bdg='№6121[09640]' />"
        L"<SpotEvent create_time='20180503T093530Z' name='Arrival' Bdg='№6121[09650]' />"
        L"<SpotEvent create_time='20180503T093630Z' name='Departure' Bdg='№6121[09650]' />"
        L"<SpotEvent create_time='20180503T093830Z' name='Arrival' Bdg='№6121[09041]' />"
        L"<SpotEvent create_time='20180503T093930Z' name='Departure' Bdg='№6121[09041]' />"
        L"<SpotEvent create_time='20180503T094130Z' name='Arrival' Bdg='№6121[09500]' />"
        L"<SpotEvent create_time='20180503T094230Z' name='Departure' Bdg='№6121[09500]' />"
        L"<SpotEvent create_time='20180503T094800Z' name='Arrival' Bdg='№6121[09010]' />"
        L"</ScheduledPath>"
        L"</export>";

    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }

    {
        const std::wstring uncontrolledData = L"<export>\n"
            L"<UncontrolledSCB ESR='09650' />"
            L"<UncontrolledSCB ESR='09640' />"
            L"</export>";

        attic::a_document doc;
        doc.load_wide(uncontrolledData);
        m_uncontrolledStations->deserialize(doc.document_element());
        ASSERT( m_uncontrolledStations->contains(EsrKit(9650)));
        ASSERT( m_uncontrolledStations->contains(EsrKit(9640)));
    }

    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeTransition, BadgeE(L"1C", EsrKit(9500)), time3 );

    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(time1, time3+1*60, 0);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return spot.second && spot.second->GetNumber().getNumber() == 6121;
        });
        CPPUNIT_ASSERT(correct);
    }
}

void TC_Hem_aeSpotToRegulatory::AutoIdentifyIfAdvanceLessThenLimit()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2018, 5, 2, 9, 30, 11);
    time_t time2 = timeForDateTime(2018, 5, 2, 9, 37, 19);
    time_t time3 = timeForDateTime(2018, 5, 2, 9, 43, 18);
    time_t time4 = timeForDateTime(2018, 5, 2, 9, 48, 18);
    time_t time5 = timeForDateTime(2018, 5, 2, 9, 49, 18);

    {
        SpotEvent path1[] = {
            SpotEvent( c_codeForm, BadgeE(L"1BP", EsrKit(9041,9650)), time1 ),
            SpotEvent( c_codeTransition, BadgeE(L"209SP+", EsrKit(9041)), time2 ),
            SpotEvent( c_codeTransition, BadgeE(L"1C", EsrKit(9500)), time3 ),
            SpotEvent( c_codeArrival, BadgeE(L"3C", EsrKit(9010)), time4 ),
            SpotEvent( c_codeDeparture, BadgeE(L"3C", EsrKit(9010)), time5 ),
        };
        m_happenLayer->createPath(path1);
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time3+1*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2018-May-04 15:00:00/2018-May-06 14:59:59.999997]'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180502T084000Z' name='Info_changing' Bdg='№6121[09070]' index='' num='6121' suburbreg='Y' />"
        L"<SpotEvent create_time='20180502T084000Z' name='Departure' Bdg='№6121[09070]' />"
        L"<SpotEvent create_time='20180502T084900Z' name='Arrival' Bdg='№6121[09600]' />"
        L"<SpotEvent create_time='20180502T085000Z' name='Departure' Bdg='№6121[09600]' />"
        L"<SpotEvent create_time='20180502T085630Z' name='Arrival' Bdg='№6121[09602]' />"
        L"<SpotEvent create_time='20180502T085700Z' name='Departure' Bdg='№6121[09602]' />"
        L"<SpotEvent create_time='20180502T090200Z' name='Arrival' Bdg='№6121[09603]' />"
        L"<SpotEvent create_time='20180502T090230Z' name='Departure' Bdg='№6121[09603]' />"
        L"<SpotEvent create_time='20180502T090700Z' name='Arrival' Bdg='№6121[09611]' />"
        L"<SpotEvent create_time='20180502T090730Z' name='Departure' Bdg='№6121[09611]' />"
        L"<SpotEvent create_time='20180502T090930Z' name='Arrival' Bdg='№6121[09612]' />"
        L"<SpotEvent create_time='20180502T091030Z' name='Departure' Bdg='№6121[09612]' />"
        L"<SpotEvent create_time='20180502T092200Z' name='Arrival' Bdg='№6121[09616]' />"
        L"<SpotEvent create_time='20180502T092300Z' name='Departure' Bdg='№6121[09616]' />"
        L"<SpotEvent create_time='20180502T092730Z' name='Arrival' Bdg='№6121[09630]' />"
        L"<SpotEvent create_time='20180502T092830Z' name='Departure' Bdg='№6121[09630]' />"
        L"<SpotEvent create_time='20180502T093130Z' name='Arrival' Bdg='№6121[09640]' />"
        L"<SpotEvent create_time='20180502T093230Z' name='Departure' Bdg='№6121[09640]' />"
        L"<SpotEvent create_time='20180502T093530Z' name='Arrival' Bdg='№6121[09650]' />"
        L"<SpotEvent create_time='20180502T093630Z' name='Departure' Bdg='№6121[09650]' />"
        L"<SpotEvent create_time='20180502T093830Z' name='Arrival' Bdg='№6121[09041]' />"
        L"<SpotEvent create_time='20180502T093930Z' name='Departure' Bdg='№6121[09041]' />"
        L"<SpotEvent create_time='20180502T094130Z' name='Arrival' Bdg='№6121[09500]' />"
        L"<SpotEvent create_time='20180502T094230Z' name='Departure' Bdg='№6121[09500]' />"
        L"<SpotEvent create_time='20180502T094800Z' name='Arrival' Bdg='№6121[09010]' />"
        L"</ScheduledPath>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20180503T084000Z' name='Info_changing' Bdg='№6121[09070]' index='' num='6121' suburbreg='Y' />"
        L"<SpotEvent create_time='20180503T084000Z' name='Departure' Bdg='№6121[09070]' />"
        L"<SpotEvent create_time='20180503T084900Z' name='Arrival' Bdg='№6121[09600]' />"
        L"<SpotEvent create_time='20180503T085000Z' name='Departure' Bdg='№6121[09600]' />"
        L"<SpotEvent create_time='20180503T085630Z' name='Arrival' Bdg='№6121[09602]' />"
        L"<SpotEvent create_time='20180503T085700Z' name='Departure' Bdg='№6121[09602]' />"
        L"<SpotEvent create_time='20180503T090200Z' name='Arrival' Bdg='№6121[09603]' />"
        L"<SpotEvent create_time='20180503T090230Z' name='Departure' Bdg='№6121[09603]' />"
        L"<SpotEvent create_time='20180503T090700Z' name='Arrival' Bdg='№6121[09611]' />"
        L"<SpotEvent create_time='20180503T090730Z' name='Departure' Bdg='№6121[09611]' />"
        L"<SpotEvent create_time='20180503T090930Z' name='Arrival' Bdg='№6121[09612]' />"
        L"<SpotEvent create_time='20180503T091030Z' name='Departure' Bdg='№6121[09612]' />"
        L"<SpotEvent create_time='20180503T092200Z' name='Arrival' Bdg='№6121[09616]' />"
        L"<SpotEvent create_time='20180503T092300Z' name='Departure' Bdg='№6121[09616]' />"
        L"<SpotEvent create_time='20180503T092730Z' name='Arrival' Bdg='№6121[09630]' />"
        L"<SpotEvent create_time='20180503T092830Z' name='Departure' Bdg='№6121[09630]' />"
        L"<SpotEvent create_time='20180503T093130Z' name='Arrival' Bdg='№6121[09640]' />"
        L"<SpotEvent create_time='20180503T093230Z' name='Departure' Bdg='№6121[09640]' />"
        L"<SpotEvent create_time='20180503T093530Z' name='Arrival' Bdg='№6121[09650]' />"
        L"<SpotEvent create_time='20180503T093630Z' name='Departure' Bdg='№6121[09650]' />"
        L"<SpotEvent create_time='20180503T093830Z' name='Arrival' Bdg='№6121[09041]' />"
        L"<SpotEvent create_time='20180503T093930Z' name='Departure' Bdg='№6121[09041]' />"
        L"<SpotEvent create_time='20180503T094130Z' name='Arrival' Bdg='№6121[09500]' />"
        L"<SpotEvent create_time='20180503T094230Z' name='Departure' Bdg='№6121[09500]' />"
        L"<SpotEvent create_time='20180503T094800Z' name='Arrival' Bdg='№6121[09010]' />"
        L"</ScheduledPath>"
        L"</export>";

    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }

    {
        const std::wstring uncontrolledData = L"<export>\n"
            L"<UncontrolledSCB ESR='09650' />"
            L"<UncontrolledSCB ESR='09640' />"
            L"</export>";

        attic::a_document doc;
        doc.load_wide(uncontrolledData);
        m_uncontrolledStations->deserialize(doc.document_element());
        ASSERT( m_uncontrolledStations->contains(EsrKit(9650)));
        ASSERT( m_uncontrolledStations->contains(EsrKit(9640)));
    }

    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeTransition, BadgeE(L"1C", EsrKit(9500)), time3 );

    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(!applier.identifiedPath());
}

void TC_Hem_aeSpotToRegulatory::IdentifyByStationAndSpandir4()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2019, 3, 26, 4, 29, 38);
    time_t time2 = timeForDateTime(2019, 3, 26, 4, 30, 48);
    {
        SpotEvent path1[] = {
            SpotEvent( c_codeDeparture, BadgeE(L"7C", EsrKit(9010)), time1 ),
            SpotEvent( c_codeSpanMove, BadgeE(L"КНПУ1", EsrKit(9010,9100)), time2 ),
        };
        m_happenLayer->createPath(path1);
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time1+10*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[20190325T160000Z/20190327T155959Z)'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20190326T043100Z' name='Info_changing' Bdg='№7709[09010]' index='' num='7709' temot='Y' />"
        L"<SpotEvent create_time='20190326T043100Z' name='Departure' Bdg='№7709[09010]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190326T043500Z' name='Transition' Bdg='№7709[09100]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190326T043910Z' name='Transition' Bdg='№7709[09104]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190326T044900Z' name='Transition' Bdg='№7709[09150]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190326T045530Z' name='Transition' Bdg='№7709[09160]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190326T045930Z' name='Transition' Bdg='№7709[09162]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190326T050300Z' name='Arrival' Bdg='№7709[09180]' waynum='1' parknum='1' />"
        L"</ScheduledPath>"
        L"</export>";
    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }


    // Идентифицируем нить как в тесте Identify()
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeDeparture, BadgeE(L"7C", EsrKit(9010)), time1);
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(applier.identifiedPath());

    {
        HappenLayer::EventWithDescrList path1 = GetPath(time1, time1+10*60, 0);
        CPPUNIT_ASSERT(!path1.empty());
        auto path1Begin = path1.begin();
        std::advance(path1Begin, 1);
        bool correct = std::all_of(path1Begin, path1.end(), [](const std::pair<HemHelpful::SpotEventPtr, Hem::HappenPath::ConstTrainDescrPtr>& spot) {
            return spot.second && spot.second->GetNumber().getNumber() == 6502;
        });
        CPPUNIT_ASSERT(correct);
    }

}

void TC_Hem_aeSpotToRegulatory::WrongIdentifyByStationAndSpandir_5644()
{
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    time_t time1 = timeForDateTime(2020, 1, 19, 13, 52, 31);
    time_t time2 = timeForDateTime(2020, 1, 19, 13, 54, 58);
//     time_t time3 = timeForDateTime(2020, 1, 19, 13, 59, 37);
//     time_t time4 = timeForDateTime(2020, 1, 19, 14, 01, 03);
//     time_t time5 = timeForDateTime(2020, 1, 19, 13, 54, 58);
    {
        SpotEvent path1[] = {
            SpotEvent( c_codeSpanMove, BadgeE(L"2UP_PK", EsrKit(9042, 9501)), time1 ),
            SpotEvent( c_codeTransition, BadgeE(L"7SP", EsrKit(9042)), time2 ),
            SpotEvent( c_codeSpanMove, BadgeE(L"1BGP", EsrKit(9042,9500)), time2 )
        };
        m_happenLayer->createPath(path1);
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_happenLayer->path_count());
    CPPUNIT_ASSERT(!GetPath(time1, time1+10*60, 0).empty());

    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[20200118T160000Z/20200120T155959Z)'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20200119T132700Z' name='Info_changing' Bdg='№6133[09612]' index='' num='6133' suburbreg='Y' />"
        L"<SpotEvent create_time='20200119T132700Z' name='Departure' Bdg='№6133[09612]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T133800Z' name='Arrival' Bdg='№6133[09616]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T133900Z' name='Departure' Bdg='№6133[09616]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T134230Z' name='Arrival' Bdg='№6133[09630]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T134330Z' name='Departure' Bdg='№6133[09630]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T134630Z' name='Arrival' Bdg='№6133[09640]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T134730Z' name='Departure' Bdg='№6133[09640]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T135030Z' name='Arrival' Bdg='№6133[09650]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T135130Z' name='Departure' Bdg='№6133[09650]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T135330Z' name='Arrival' Bdg='№6133[09042]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T135430Z' name='Departure' Bdg='№6133[09042]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T135630Z' name='Arrival' Bdg='№6133[09500]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T135730Z' name='Departure' Bdg='№6133[09500]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T140300Z' name='Arrival' Bdg='№6133[09010]' waynum='1' parknum='1' />"
        L"</ScheduledPath>"
        L"</export>";
    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    {
        attic::a_document doc;
        doc.load_wide(layerData);
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(doc.document_element());
    }

    {
        const std::wstring uncontrolledData = L"<export>\n"
            L"<UncontrolledSCB ESR='09501' />"
            L"</export>";

        attic::a_document doc;
        doc.load_wide(uncontrolledData);
        m_uncontrolledStations->deserialize(doc.document_element());
        ASSERT( m_uncontrolledStations->contains(EsrKit(9501)));
    }


    // Идентифицируем нить как в тесте Identify()
    SpotEventPtr spotPtr = std::make_shared<SpotEvent>(c_codeSpanMove, BadgeE(L"1BGP", EsrKit(9042,9500)), time2 );
    Hem::aeSpotToRegulatory applier(*m_uncontrolledStations, m_identifyCategoryList, m_happenLayer->GetWriteAccess(), 
		m_regulatoryLayer->getReadAccess(), m_HappenRegular, true, spotPtr);
    applier.Action();
    CPPUNIT_ASSERT(!applier.identifiedPath());
}

