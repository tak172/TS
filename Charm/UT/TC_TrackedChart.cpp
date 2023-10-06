#include "stdafx.h"
#include "TC_TrackedChart.h"
#include "../Hem/TrackedChart.h"
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Guess/TrackerEvent.h"
#include "UtHelpfulDateTime.h"
#include "UtHemHelper.h"
#include "TopologyTest.h"
#include "../Hem/Appliers/aeAsoupToSpot.h"
#include "../Hem/RegulatoryLayer.h"
#include "../Hem/HappenLayer.h"
#include "../helpful/LocoCharacteristics.h"
#include "../helpful/StrToTime.h"
#include "UtNsiBasis.h"
#include "../helpful/StationsRegistry.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/Crew.h"
#include "../Hem/GuessTransciever.h"
#include "../Hem/AccessHappenAsoupRegulatory.h"
#include "../Hem/Appliers/aeUserIdentify.h"
#include "TC_Hem_ae_helper.h"
#include "../helpful/Serialization.h"
#include "../Hem/AsoupProcessingQueue.h"
#include "../helpful/CustomMode.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackedChart );

const std::string c_utNodeName = "UtTrackedChart";

typedef HemHelpful::SpotEventPtr SpotPtr;

const BadgeE c_stationA(L"Bdg_A", EsrKit(1000));
const BadgeE c_stationB(L"Bdg_B", EsrKit(1100));
const BadgeE c_stationC(L"Bdg_C", EsrKit(1200,1201));
const BadgeE c_stationD(L"Bdg_D", EsrKit(1300));
const BadgeE c_stationE(L"Bdg_E", EsrKit(1400));

class UtTrackedChart : public Hem::TrackedChart
{
public:
    UtTrackedChart()
        : Hem::TrackedChart(c_utNodeName)
    {
        startHookTrainChanges();
    }
    // открыть методы из базового класса
    Hem::TrackedChart::getHappenLayer;
    Hem::TrackedChart::getAsoupLayer;
    Hem::TrackedChart::getRegulatoryLayer;
    Hem::TrackedChart::removePreviousMessage;
    Hem::TrackedChart::getGuessTransciever;
    Hem::TrackedChart::getTopology;
    Hem::TrackedChart::getAsoupOperationMode;
	Hem::TrackedChart::getAsoupQueueMgr;

    void deserializeRegulatoryLayer(const std::string& regulatoryLayer)
    {
        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_utf8(regulatoryLayer));
        getRegulatoryLayer().deserialize(doc.document_element());
        getRegulatoryLayer().takeChanges(attic::a_node());

        attic::a_document copy(doc.document_element().name());
        getRegulatoryLayer().serialize(copy.document_element());
        CPPUNIT_ASSERT_EQUAL(doc.pretty_str(), copy.pretty_str());
    }
    
    bool acceptEventFromStr(const std::wstring& str, const unsigned trainID, TrainCharacteristics::Source source, const unsigned int prevID)
    {
        return TrackedChart::acceptTrackerEvent( ::deserialize<SpotEvent>(ToUtf8(str)), trainID, TrainCharacteristics::Source::Guess, 0);
    }

	bool acceptEvent(const SpotPtr& spot, const unsigned trainID)
	{
		return TrackedChart::acceptTrackerEvent( spot, trainID, TrainCharacteristics::Source::Guess, 0);
	}

	void clearAsoupQueue()
	{
		getAsoupQueueMgr().clear();
	}

private:
    virtual std::pair<bool, Hem::TrioOperationType> onSpotTrioTracked( const TrioSpot& spotTrio, HemEventsSet& /*rejectedEvents*/, std::vector<HemHelpful::SpotEventPtr>& /*acceptedEvents*/, TrainCharacteristics::Source /*source*/ ) override
    {
        UtIdentifyCategoryList identifyCategory;
        Hem::aePusherAutoHappen pusher(getHappenLayer().GetWriteAccess(), spotTrio, identifyCategory);
        auto pusherResult = pusher.performAction();
        return std::make_pair(pusher.UnapplyableEvents().empty(), pusherResult);
    }
    // вызывается для выполнения рутинных операций после обработки spotTrio (сглаживание нити, дополнительная обработка в зависимости от места нахождения и т.п.)
    virtual void PostSpotTrioOperations( const std::vector<HemHelpful::SpotEventPtr>& /*vEvents*/ ) override
    { }

    virtual void onSerialize( attic::a_node& chartNode ) const override
    {
        // Do nothing
    }

    virtual void onDeserialize( const attic::a_node& chartNode ) override
    {
        // Do nothing
    }

    virtual bool onTakeChanges( attic::a_node& parentNode ) override
    {
        return false;
    }

    virtual bool onTakeAnalysis( attic::a_node& analysisParentNode ) override
    {
        return false;
    }
    void obligatoryPostAction() override
    {}
};

TC_TrackedChart::TC_TrackedChart()
{
}

TC_TrackedChart::~TC_TrackedChart()
{
}

void CheckSerialization(const Hem::TrackedChart& chart)
{
    attic::a_document doc("TC");
    std::string chartSerialization;
    {
        attic::a_node root = doc.document_element();
        root.append_child("RandomNode");

        chart.serialize(root);
        CPPUNIT_ASSERT(root.child("RandomNode"));
        CPPUNIT_ASSERT(root.child(c_utNodeName));

        chartSerialization = root.pretty_str(2);
    }    

    {
        UtTrackedChart secondChart;
        secondChart.deserialize(doc.document_element());
        secondChart.takeChanges( attic::a_node(), attic::a_node(), 0 );

        attic::a_document secondDoc("TC");
        attic::a_node secondRoot = secondDoc.document_element();

        secondRoot.append_child("RandomNode");
        secondRoot.append_child(c_utNodeName);

        secondChart.serialize(secondRoot);
        CPPUNIT_ASSERT(secondRoot.child("RandomNode"));
        CPPUNIT_ASSERT(secondRoot.child(c_utNodeName));

		std::string chartSecond = secondRoot.pretty_str(2);
		auto diff = std::mismatch( chartSecond.cbegin(), chartSecond.cend(), chartSerialization.cbegin() );
        CPPUNIT_ASSERT_EQUAL(chartSerialization, secondRoot.pretty_str(2));

    }    
}

void TC_TrackedChart::Serialization()
{
    typedef std::function<void(Hem::TrackedChart&)> Modifier;
    Modifier modifiers[] = 
    {
        [this](Hem::TrackedChart& chart) {
            // Проверка пустого чарта
        },
        [this](Hem::TrackedChart& chart) {
            // Проверка чарта с событиями слоя исполненного графика
            unsigned const ID = 64;
            CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(HemHelpful::SpotEventPtr(new TrackerEvent(
                ID, HCode::FORM, BadgeE(L"WSTR_FORM", EsrKit(1000)), time_t(1000))), ID,
                TrainCharacteristics::Source::Guess));
            CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(HemHelpful::SpotEventPtr(new TrackerEvent(
                ID, HCode::DEPARTURE, BadgeE(L"WSTR_DEP", EsrKit(1000)), time_t(1100))), ID,
                TrainCharacteristics::Source::Guess));
        },
        [this](Hem::TrackedChart& chart) {
            // Проверка чарта с событиями АСОУП
            const std::wstring asoupCargoCode00300station09836noteAKRIL = L"(:1042 909/000+09180 2212 0982 009 5851 03 11420 18 05 18 30 00/00 0 0/00 00 0\n"
                L"Ю2 0 00 00 00 00 0000 0 0 056 01194 00000 212 95153169 95263745 000 000 00 000 053.90 000 053 000 92 000 005 000 95 000 048 000\n"
                L"Ю3 530 00009591 1 18 00 0000 00000 KOVALJONOKS \n"
                L"Ю3 530 00009592 9\n"
                L"Ю4 11290 2 58 000 053.90 000 053 92 000 005 95 000 048 \n"
                L"Ю12 00 95153169 1 000 09836 00300 2870 300 00 00 00 00 00 0220 12 95 0950 04 106 58510 00000 11290 58 09820 17 00000 0000 020 1 1280 АКРИЛ  128 000 00000000\n"
                L"Ю12 00 95283487 255239255255 0215:)";
            CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoupCargoCode00300station09836noteAKRIL,
                boost::gregorian::date(2016, 05, 18), nullptr, 0) );
        }
    };

    for (Modifier& modifier : modifiers)
    {
        m_chart.reset(new UtTrackedChart);
        modifier(*m_chart);
        m_chart->takeChanges(attic::a_node(), attic::a_node(), 0);
        CheckSerialization(*m_chart);
    }
}

void TC_TrackedChart::setUp()
{
    m_chart.reset(new UtTrackedChart);
    m_chart->setupIdentifyCategory(identifyCategory);
    setupTopology();
}

void TC_TrackedChart::AsoupIdentification_OnePass()
{
    time_t curtime = timeForDate(2016, 01, 01);
    time_t appearance_time = curtime + 10;
    time_t entry_time = appearance_time + 10;
    time_t arrival_time = entry_time + 10;
    time_t departure_time = arrival_time + 10;
    time_t exit_time = departure_time + 10;
    time_t span_move_time = exit_time + 10;

    // Строгая идентификация с первого раза
    SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
        ParkWayKit(1, 1), std::make_pair(PicketingInfo(), PicketingInfo()));

    SpotPtr pathEvents[] = { std::make_shared<SpotEvent>( HCode::FORM, c_stationA, appearance_time ),
        std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationB, arrival_time, spotDetails ),
        std::make_shared<SpotEvent>( HCode::DEPARTURE, c_stationB, departure_time ),
        std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, span_move_time )
    };

    CPPUNIT_ASSERT(CreatePath(*m_chart, pathEvents, 64));
    CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::ARRIVAL, c_stationB,
        ParkWayKit(1, 1), arrival_time, TrainDescr(L"1379", L"1111-222-3333")), arrival_time + 30));
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetTrainDescrCount());
    CPPUNIT_ASSERT_EQUAL(1379u, GetTrainDescr(0)->GetNumber().getNumber());

    CheckSerialization(*m_chart);
}

void TC_TrackedChart::AsoupIdentification_TwoPass()
{
    time_t curtime = timeForDate(2016, 01, 01);
    time_t appearance_time = curtime + 10;
    time_t entry_time = appearance_time + 10;
    time_t arrival_time = entry_time + 10;
    time_t departure_time = arrival_time + 10;
    time_t exit_time = departure_time + 10;
    time_t span_move_time = exit_time + 10;

    SpotPtr pathEvents[] = { std::make_shared<SpotEvent>( HCode::FORM, c_stationA, appearance_time ),
        std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationB, arrival_time ),
        std::make_shared<SpotEvent>( HCode::DEPARTURE, c_stationB, departure_time ),
        std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, span_move_time )
    };

    CPPUNIT_ASSERT(CreatePath(*m_chart, pathEvents, 64));
	auto asoup = std::make_shared<AsoupEvent>(HCode::ARRIVAL, c_stationB,
		ParkWayKit(1, 1), arrival_time, TrainDescr(L"1379", L"1111-222-3333"));
	CPPUNIT_ASSERT( asoup );
    CPPUNIT_ASSERT(PassAsoup(*m_chart, asoup));

    // Строгая идентификация с первого раза невозможна, 
    // так как в событии отправления не указаны парк и путь, а в АСОУП они заданы
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());
    CPPUNIT_ASSERT_EQUAL(size_t(0), GetTrainDescrCount());

    CheckSerialization(*m_chart);

    // Зато после заданного интервала времени производится повторная попытка идентификации,
    // которая должна завершиться успешно
    m_chart->updateTime(Hem::aeAsoupToSpotFind::getSecondaryAttemptTime(*asoup), nullptr);
    ProcessChanges(*m_chart);
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetTrainDescrCount());
    CPPUNIT_ASSERT_EQUAL(1379u, GetTrainDescr(0)->GetNumber().getNumber());

    CheckSerialization(*m_chart);
}

void TC_TrackedChart::AsoupIdentificationWithIndexChange()
{
    time_t curtime = timeForDate(2016, 01, 01);
    time_t appearance_time = curtime + 10;
    time_t entry_time = appearance_time + 10;
    time_t arrival_time = entry_time + 10;
    time_t departure_time = arrival_time + 10;
    time_t exit_time = departure_time + 10;
    time_t span_move_time = exit_time + 10;

    {
        const ParkWayKit c_parkWay(1, 1);

        // Строгая идентификация с первого раза
        SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
            c_parkWay, std::make_pair(PicketingInfo(), PicketingInfo()));

        const TrainDescr number(L"1010", L"1010-010-2020");
        SpotDetailsCPtr formDetails = std::make_shared<SpotDetails>(number);

        SpotPtr pathEvents[] = { std::make_shared<SpotEvent>( HCode::FORM, c_stationA, appearance_time, formDetails ),
            std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationB, arrival_time, spotDetails ),
            std::make_shared<SpotEvent>( HCode::DEPARTURE, c_stationB, departure_time, spotDetails ),
            std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, span_move_time )
        };

        CPPUNIT_ASSERT(CreatePath(*m_chart, pathEvents, 64));
        CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
        CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::DEPARTURE, c_stationB,
            c_parkWay, departure_time, TrainDescr(L"1379", L"1111-222-3333")), departure_time+30));
        CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());

        CPPUNIT_ASSERT_EQUAL(size_t(2), GetHappenTrioCount());
        CPPUNIT_ASSERT_EQUAL(1379u, GetTrainDescr(0)->GetNumber().getNumber());

        CheckSerialization(*m_chart);
    }
}

void TC_TrackedChart::RegulatoryIdentification()
{
    time_t c_time1 = timeForDateTime(2015, 8, 30, 10, 10);
    time_t c_time2 = timeForDateTime(2015, 8, 30, 10, 20);
    time_t c_time3 = timeForDateTime(2015, 8, 30, 10, 30);
    time_t c_time4 = timeForDateTime(2015, 8, 30, 10, 40);
    time_t c_time5 = timeForDateTime(2015, 8, 30, 10, 50);

    const BadgeE c_station1(L"1", EsrKit(100));
    const BadgeE c_station2(L"2", EsrKit(200));
    const BadgeE c_station3(L"3", EsrKit(300));
    const BadgeE c_station4(L"4", EsrKit(400));
    const BadgeE c_station5(L"5", EsrKit(500));

    const std::string layerData = "<export>\n"
        "<ScheduledLayer timePeriod='[20150830T100000Z/20160831T100000Z)'>\n"
        "  <ScheduledPath>\n"
        "    <SpotEvent create_time='20150830T101000Z' name='Info_changing' Bdg='1p[00100]' index='' num='6024' suburbreg='Y' />\n" // c_time1
        "    <SpotEvent create_time='20150830T101000Z' name='Departure' Bdg='1p[00100]' />\n" // c_time1
        "    <SpotEvent create_time='20150830T102000Z' name='Transition' Bdg='2p[00200]' />\n" // c_time2
        "    <SpotEvent create_time='20150830T103000Z' name='Arrival' Bdg='3p[00300]' />\n" // c_time3
        "    <SpotEvent create_time='20150830T104000Z' name='Departure' Bdg='4p[00400]' />\n" // c_time4
        "    <SpotEvent create_time='20150830T105000Z' name='Arrival' Bdg='5p[00500]' />\n" // c_time5
        "  </ScheduledPath>\n"
        "  <ScheduledPath>\n"
        "    <SpotEvent create_time='20150830T101100Z' name='Info_changing' Bdg='1p[00100]' index='' num='2024' through='Y' />\n" // c_time1 (Номер 2024 не подходит)
        "    <SpotEvent create_time='20150830T101100Z' name='Departure' Bdg='1p[00100]' />\n" // c_time1
        "    <SpotEvent create_time='20150830T102100Z' name='Transition' Bdg='2p[00200]' />\n" // c_time2
        "    <SpotEvent create_time='20150830T103100Z' name='Arrival' Bdg='3p[00300]' />\n" // c_time3
        "    <SpotEvent create_time='20150830T104100Z' name='Departure' Bdg='4p[00400]' />\n" // c_time4
        "    <SpotEvent create_time='20150830T105100Z' name='Arrival' Bdg='5p[00500]' />\n" // c_time5
        "  </ScheduledPath>\n"
        "  <ScheduledPath>\n"
        "    <SpotEvent create_time='20150830T110000Z' name='Info_changing' Bdg='1p[00500]' index='' num='6048' suburbreg='Y' />\n" // c_time2_1
        "    <SpotEvent create_time='20150830T110000Z' name='Departure' Bdg='1p[00500]' />\n" // c_time2_1
        "    <SpotEvent create_time='20150830T120000Z' name='Transition' Bdg='1p[00400]' />\n" // c_time2_2
        "    <SpotEvent create_time='20150830T130000Z' name='Arrival' Bdg='3p[00300]' />\n" // c_time2_3
        "    <SpotEvent create_time='20150830T140000Z' name='Departure' Bdg='4p[00200]' />\n" // c_time2_4
        "  </ScheduledPath>\n"
        "</ScheduledLayer>\n"
        "</export>\n";

    SpotPtr pathEvents[] = { std::make_shared<SpotEvent>( HCode::FORM, c_station1, c_time1 ),
        std::make_shared<SpotEvent>( HCode::DEPARTURE, c_station1, c_time1 + 10 ),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station2, c_time2),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station3, c_time3 + 20),
        std::make_shared<SpotEvent>( HCode::ARRIVAL, c_station4, c_time4)
    };

    m_chart->deserializeRegulatoryLayer(layerData);
    ProcessChanges(*m_chart);

    CPPUNIT_ASSERT(CreatePath(*m_chart, pathEvents, 64));
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetTrainDescrCount());
    CPPUNIT_ASSERT_EQUAL(6024u, GetTrainDescr(0)->GetNumber().getNumber());
}

void CheckChanges(Hem::TrackedChart& chart, bool shouldBeChanges, bool shouldBeAnalysis)
{
    attic::a_document doc("TC");
    attic::a_node root = doc.document_element();
    attic::a_node changes = root.append_child("Changes");
    attic::a_node analysis = root.append_child("Analysis");
    
    if (shouldBeAnalysis || shouldBeChanges)
        CPPUNIT_ASSERT_MESSAGE("There should be some changes", chart.takeChanges(changes, analysis, 0));
    else
        CPPUNIT_ASSERT_MESSAGE("There should be no changes", !chart.takeChanges(changes, analysis, 0));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Changes existance", shouldBeChanges, pugi::xml_node_type::node_null != changes.first_child().type());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Analysis existance", shouldBeAnalysis, pugi::xml_node_type::node_null != analysis.first_child().type());
}

void TC_TrackedChart::Changes()
{
    attic::a_document doc("TC");
    attic::a_node root = doc.document_element();
    attic::a_node changes = root.append_child("Changes");
    attic::a_node analysis = root.append_child("Analysis");
    CheckChanges(*m_chart, false, false);
    
    CPPUNIT_ASSERT(!m_chart->acceptTrackerEvent(std::make_shared<SpotEvent>(HCode::DISFORM, c_stationA, timeForDate(2015, 10, 10)), 64, TrainCharacteristics::Source::Guess));
    CheckChanges(*m_chart, false, false);

    CPPUNIT_ASSERT(!m_chart->processNewAsoupText(L"Wrong asoup text", boost::gregorian::date(2015, 10, 10),nullptr, 0));
    CheckChanges(*m_chart, false, false);

    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(std::make_shared<SpotEvent>(HCode::FORM, c_stationA, timeForDate(2015, 10, 10)), 65, TrainCharacteristics::Source::Guess));
    CheckChanges(*m_chart, true, false);

    m_chart->serialize(root);
    CheckChanges(*m_chart, false, false);

    m_chart->deserialize(root);
    m_chart->takeChanges( attic::a_node(), attic::a_node(), 0 );
    CheckChanges(*m_chart, false, false);

    UtTrackedChart secondChart;
    secondChart.deserialize(root);
    secondChart.takeChanges( attic::a_node(), attic::a_node(), 0 );
    CheckChanges(secondChart, false, false);
}

void TC_TrackedChart::InfoChanging()
{
    SpotPtr pathEvents[] = { std::make_shared<SpotEvent>( HCode::FORM, c_stationA, 100 ),
        std::make_shared<SpotEvent>( HCode::ARRIVAL, c_stationB, 200 ),
        std::make_shared<SpotEvent>( HCode::DEPARTURE, c_stationB, 300),
        std::make_shared<SpotEvent>( HCode::SPAN_MOVE, c_stationC, 400)
    };

    unsigned const trainID = 64;
    CPPUNIT_ASSERT(CreatePath(*m_chart, pathEvents, trainID));

    unsigned const trainNumber = 1379u;
    TrainDescr const trainDescr(std::to_wstring(trainNumber), L"7-7-7");
    SpotDetailsCPtr spotDetails(new SpotDetails(trainDescr));
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
        std::make_shared<SpotEvent>(HCode::INFO_CHANGING, c_stationC, 500, spotDetails), trainID, TrainCharacteristics::Source::Guess));

    ProcessChanges(*m_chart);
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetTrainDescrCount());
    CPPUNIT_ASSERT_EQUAL(trainNumber, GetTrainDescr(0)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
    TrioSpot const trioGenerated = GetHappenTrio(0);
    CPPUNIT_ASSERT(!trioGenerated.empty());
    CPPUNIT_ASSERT(trioGenerated.pre());
    CPPUNIT_ASSERT(*trioGenerated.pre() == *pathEvents[0]);
    CPPUNIT_ASSERT(trioGenerated.body().size() > size_t(0u));
    CPPUNIT_ASSERT(*trioGenerated.body().front() == *pathEvents[0]);
}

void TC_TrackedChart::AsoupWithdrawal()
{
    // Отменяемое сообщение
    CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)"));
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());
    TrioAsoup trio = GetAsoupTrio(0);
    CPPUNIT_ASSERT(!trio.pre() && !trio.post());
    CPPUNIT_ASSERT_EQUAL(size_t(1), trio.body().size());
    CPPUNIT_ASSERT(trio.body().front()->GetAsoupEvent());

    // Отменяющее сообщение
    CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:333 1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)"));
    CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());
    trio = GetAsoupTrio(0);
    CPPUNIT_ASSERT(trio.pre());
    CPPUNIT_ASSERT(trio.pre()->GetAsoupEvent());
    CPPUNIT_ASSERT(trio.body().empty() && !trio.post());

    // Вызов сообщения об отмене, когда предыдущего сообщения нет
    CPPUNIT_ASSERT(!PassAsoupRawText(*m_chart, L"(:333 1042 909/000+11290 2609 1800 541 1131 04 11310 27 01 12 45 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 04713 03417 220 75085555 95316873 000 000 00 055 000.60 001 000 000 70 009 000 000 73 002 000 000 90 045 000 000 95 045 000 000:)"));
    ProcessChanges(*m_chart);
    CPPUNIT_ASSERT_EQUAL(size_t(0), GetAsoupTrioCount());
}

// добавление неприемлемого события к незакрытой нити - нить должна закрыться
void TC_TrackedChart::SafelyReject_Live()
{
    BadgeE X( L"x",  EsrKit(100)    );
    BadgeE XY(L"xy", EsrKit(100,200));
    BadgeE Y(  L"Y", EsrKit(200)    );

    unsigned const tid = 1234; // произвольный идентификатор поезда в Guess
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::FORM,  X, 1000), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DEATH, X, 1010), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::FORM,  Y, 1020), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(!m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DEATH,Y, 1010), tid, TrainCharacteristics::Source::Guess));

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "x[00100] x[00100]   Y[00200] Y[00200] "));
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form     Death      Form     Death " ) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
}

void TC_TrackedChart::SafelyReject_Dead()
{
    BadgeE X( L"x",  EsrKit(100)    );
    BadgeE XY(L"xy", EsrKit(100,200));
    BadgeE Y(  L"Y", EsrKit(200)    );

    // добавление неприемлемого события к закрытой нити - нить не изменяется
    unsigned const tid = 4321; // произвольный идентификатор поезда в Guess
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DEPARTURE,Y, 1000), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::ARRIVAL,  X, 1010), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DISFORM,  X, 1020), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(!m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DEPARTURE,Y,1030), tid, TrainCharacteristics::Source::Guess));

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "Y[00200]   x[00100] x[00100] "));
    CPPUNIT_ASSERT( hl.exist_series( "name", "Departure  Arrival  Disform  " ) );
    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "Y[00200] "));
    CPPUNIT_ASSERT( hl.exist_series( "name", "Departure " ) );
}

void TC_TrackedChart::SafelyReject_NotExist()
{
    BadgeE X( L"x",  EsrKit(100)    );
    BadgeE XY(L"xy", EsrKit(100,200));
    BadgeE Y(  L"Y", EsrKit(200)    );

    // добавление неприемлемого события к несуществующей нити - никаких изменений
    unsigned const tid = 2222; // произвольный идентификатор поезда в Guess
    CPPUNIT_ASSERT(!m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DISFORM, X, 1200), tid, TrainCharacteristics::Source::Guess));

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    CPPUNIT_ASSERT( hl.path_count() == 0 );
}

void TC_TrackedChart::SafelyReject_Obstacle()
{
    BadgeE X( L"x",  EsrKit(100)    );
    BadgeE XY(L"xy", EsrKit(100,200));
    BadgeE Y(  L"Y", EsrKit(200)    );

    // добавление неприемлемого события к незакрытой нити - нить удается закрыть лишь со второй попытки!
    unsigned const tod = 4411; // произвольный идентификатор МЕШАЮЩЕГО поезда
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::FORM,  X, 2100), tod, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::DEATH, X, 2200), tod, TrainCharacteristics::Source::Guess));
    unsigned const tid = 4455; // произвольный идентификатор поезда
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::TRANSITION,Y, 1000), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::ARRIVAL,   X, 1020), tid, TrainCharacteristics::Source::Guess));
    CPPUNIT_ASSERT(!m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::ARRIVAL,   X, 1030), tid, TrainCharacteristics::Source::Guess));

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "Y[00200]   x[00100] fin.x[00100] "));
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Arrival  Death           " ) );
}

void TC_TrackedChart::UncontrolledMiddle()
{
    Uncontrolled_helper();

    auto path1 = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T065601Z' name='Form' Bdg='6C[09180]' waynum='6' parknum='1' index='0918-031-0987' num='9509' length='57' weight='4946' net_weight='3624' heavy='Y'>"
        L"    <Locomotive Series='2ТЭ10У' NumLoc='185' Consec='1' CarrierCode='4'>"
        L"      <Crew EngineDriver='ABRAMOVS' Tim_Beg='2018-05-18 09:32' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T080206Z' name='Departure' Bdg='6C[09180]' waynum='6' parknum='1' optCode='09180:09280' />"
        L"  <SpotEvent create_time='20180518T081000Z' name='Span_move' Bdg='2GPP[09180:09280]' waynum='1' >"
        L"    <rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"    <rwcoord picketing1_val='54~' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T081000Z' name='Death' Bdg='2GPP[09180:09280]' waynum='1' >"
        L"    <rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"    <rwcoord picketing1_val='54~' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    auto path2 = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T121643Z' name='Form' Bdg='LT-1C[09355:09860]' waynum='1' index='0918-031-0987' num='9509' length='57' weight='4946' net_weight='3624' heavy='Y'>"
        L"    <rwcoord picketing1_val='226~' picketing1_comm='Лиепая' />"
        L"    <rwcoord picketing1_val='227~700' picketing1_comm='Лиепая' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T122024Z' name='Transition' Bdg='55C[09860]' waynum='55' parknum='2' intervalSec='8' optCode='09860' />"
        L"  <SpotEvent create_time='20180518T122344Z' name='Arrival' Bdg='5C[09860]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20180518T135149Z' name='Disform' Bdg='5C[09860]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );

    const TrainDescr td(L"9509", L"0918-031-0987");

    AsoupEvent begAsoup( HCode::DEPARTURE,  BadgeE( L"any", EsrKit(9180) ),
        ParkWayKit(1,6), time_from_iso("20180518T080000Z"), td, L"",      L"09280" );
    AsoupEvent midAsoup( HCode::TRANSITION, BadgeE( L"any", EsrKit(9330) ),
        ParkWayKit(1,6), time_from_iso("20180518T102300Z"), td, L"09320", L"09340" );
    AsoupEvent endAsoup( HCode::DEPARTURE,  BadgeE( L"any", EsrKit(9860) ),
        ParkWayKit(1,5), time_from_iso("20180518T122300Z"), td, L"09355", L"" );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path1, 1) );
    CPPUNIT_ASSERT( CreatePath(*m_chart, path2, 2) );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 2 );
    }

    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(begAsoup) ) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(midAsoup) ) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(endAsoup) ) );

    m_chart->updateTime( time_from_iso("20180518T102300Z"), nullptr );
    m_chart->updateTime( time_from_iso("20180518T102301Z"), nullptr );
	m_chart->updateTime( time_from_iso("20180518T121643Z"), nullptr );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
    }
}

void TC_TrackedChart::UncontrolledDepartureWithHappenDeparture()
{
    Uncontrolled_helper();

    auto path = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20190827T030002Z' name='Form' Bdg='1C[11260]' waynum='1' parknum='1' optCode='11260:11265' />"
        L"<SpotEvent create_time='20190827T030002Z' name='Departure' Bdg='1C[11260]' waynum='1' parknum='1' optCode='11260:11265' />"
        L"<SpotEvent create_time='20190827T031305Z' name='Transition' Bdg='1C&apos;2[11265]' waynum='1' parknum='2' intervalSec='14' optCode='11265:11266' />"
        L"<SpotEvent create_time='20190827T031434Z' name='Transition' Bdg='1AC[11265]' waynum='1' parknum='1' intervalSec='13' optCode='11265:11266' />"
        L"</HemPath>"
        );

    const TrainDescr td(L"2961", L"2736-204-0906");

    AsoupEvent begAsoup( HCode::DEPARTURE,  BadgeE( L"any", EsrKit(7792) ),
        ParkWayKit(1,1), time_from_iso("20190827T023000Z"), td, L"",      L"11260" );
    AsoupEvent endAsoup( HCode::DEPARTURE,  BadgeE( L"any", EsrKit(11260) ),
        ParkWayKit(1,1), time_from_iso("20190827T030000Z"), td, L"", L"11265" );

    auto begAsoupPtr = std::make_shared<AsoupEvent>(begAsoup);
    auto endAsoupPtr = std::make_shared<AsoupEvent>(endAsoup);
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, begAsoupPtr ) );
    HappenLayer& hl = m_chart->getHappenLayer();
    AsoupLayer& al = m_chart->getAsoupLayer();

    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );	//реальная нить + нить, порожденная по АСОУП
        auto path = *pathSet.cbegin();
		CPPUNIT_ASSERT( path );
        CPPUNIT_ASSERT( /*path->GetEventsCount()==2 && path->GetFirstFragment() != nullptr ||*/
			path->GetEventsCount()==4 && path->GetFirstFragment() == nullptr );
		CPPUNIT_ASSERT ( !al.IsServed( begAsoupPtr ) );
    }

    CPPUNIT_ASSERT( PassAsoup( *m_chart, endAsoupPtr ) );
	m_chart->updateTime( time_from_iso("20190827T030000Z"), nullptr );

    {
		CPPUNIT_ASSERT( al.IsServed( endAsoupPtr) );
		CPPUNIT_ASSERT ( al.IsServed( begAsoupPtr ) );
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetEventsCount()==5 && path->GetFirstFragment() && 
                        path->GetFirstFragment()->GetNumber().getNumber() == endAsoup.GetNumber() &&
                        path->GetFirstFragment()->GetIndex() == endAsoup.GetTrainIndex() );
    }
}

void TC_TrackedChart::UncontrolledWithGroupNumber()
{
    Uncontrolled_helper();

    auto path = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20190827T022419Z' name='Form' Bdg='N3P_SKA[07792:11260]' waynum='1' index='' num='1001' refri='Y'>"
        L"<rwcoord picketing1_val='398~300' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='396~' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190827T022959Z' name='Arrival' Bdg='1C[11260]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190827T022959Z' name='Death' Bdg='1C[11260]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    const TrainDescr td(L"2961", L"2736-204-0906");

	auto begTime = time_from_iso("20190827T021600Z");
	auto endTime = time_from_iso("20190827T023000Z");
    AsoupEvent begAsoup( HCode::DEPARTURE,  BadgeE( L"any", EsrKit(7792) ),
        ParkWayKit(1,1), begTime, td, L"",      L"11260" );
    AsoupEvent endAsoup( HCode::ARRIVAL, BadgeE( L"any", EsrKit(11260) ),
        ParkWayKit(1,1), endTime, td, L"07792", L"" );

    auto begAsoupPtr = std::make_shared<AsoupEvent>(begAsoup);
    auto endAsoupPtr = std::make_shared<AsoupEvent>(endAsoup);
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, begAsoupPtr, begTime) );
    HappenLayer& hl = m_chart->getHappenLayer();
    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetEventsCount()==3 && path->GetFirstEvent()->GetBadge().num()==EsrKit(7792,11260) );
        CPPUNIT_ASSERT( path->GetFirstFragment() && path->GetFirstFragment()->GetNumber().getNumber()==1001 );
		CPPUNIT_ASSERT ( !al.IsServed( begAsoupPtr) );
    }

    m_chart->updateTime( begTime + 30, nullptr );
	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		CPPUNIT_ASSERT( path && path->GetEventsCount()==3 && path->GetFirstEvent()->GetBadge().num()==EsrKit(7792,11260) );
		CPPUNIT_ASSERT( path->GetFirstFragment() && path->GetFirstFragment()->GetNumber().getNumber()==1001 );
		CPPUNIT_ASSERT ( !al.IsServed( begAsoupPtr) );
	}
    CPPUNIT_ASSERT( PassAsoup( *m_chart, endAsoupPtr, endTime + 30 ) );
    {   //проверим смену номера
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        //проверим дотягивание до первого сообщения АСОУП
        CPPUNIT_ASSERT( path && path->GetEventsCount()==3 && path->GetFirstEvent()->GetBadge().num()==begAsoup.GetBadge().num() && 
            path->GetFirstFragment() && 
            path->GetFirstFragment()->GetNumber().getNumber() == endAsoup.GetNumber() &&
            path->GetFirstFragment()->GetIndex() == endAsoup.GetTrainIndex() );
        CPPUNIT_ASSERT ( al.IsServed( begAsoupPtr ) );
        CPPUNIT_ASSERT ( al.IsServed( endAsoupPtr) );
    }
}

void TC_TrackedChart::IdentifyAndWrongAttachSameNumberedPathes_5570()
{
    Uncontrolled_helper();
    auto path1 = loaderSpot(            
        L"<HemPath>"
        L"<SpotEvent create_time='20191030T041023Z' name='Form' Bdg='1C[09010]' waynum='1' parknum='1' index='' num='7732' length='6' weight='344' temot='Y'>"
        L"<Locomotive Series='ЭР2' NumLoc='1307' CarrierCode='24'>"
        L"<Crew EngineDriver='Kononovs' Tim_Beg='2019-10-30 06:20' PersonMode='1' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T042201Z' name='Departure' Bdg='1C[09010]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20191030T042751Z' name='Arrival' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T042854Z' name='Departure' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T043151Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='51' optCode='09000:09420' />"
        L"<SpotEvent create_time='20191030T043804Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' intervalSec='41' />"
        L"<SpotEvent create_time='20191030T044758Z' name='Arrival' Bdg='1AC[09410]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20191030T044838Z' name='Departure' Bdg='1C[09410]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20191030T044918Z' name='Span_move' Bdg='20P_O[09400:09410]' waynum='2'>"
        L"<rwcoord picketing1_val='36~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='35~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T044923Z' name='Disform' Bdg='20P_O[09400:09410]' waynum='2'>"
        L"<rwcoord picketing1_val='36~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='35~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );


    auto path = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20191030T044956Z' name='Form' Bdg='6C[09010]' waynum='6' parknum='1' >"
        L"<Locomotive Series='ЭР2' NumLoc='1307' CarrierCode='24'>"
        L"<Crew EngineDriver='Kononovs' Tim_Beg='2019-10-30 06:20' PersonMode='1' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T045409Z' name='Departure' Bdg='6C[09010]' waynum='6' parknum='1' />"
        L"<SpotEvent create_time='20191030T050101Z' name='Arrival' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T050205Z' name='Departure' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T050845Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='53' optCode='09000:09420' />"
        L"<SpotEvent create_time='20191030T051648Z' name='Arrival' Bdg='2C[09420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191030T051851Z' name='Departure' Bdg='2C[09420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191030T053211Z' name='Arrival' Bdg='1AC[09410]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    const TrainDescr td(L"7732", L"");

    AsoupEvent asoup( HCode::ARRIVAL, BadgeE( L"any", EsrKit(9410) ),
        ParkWayKit(1,1), time_from_iso("20191030T053200Z"), td, L"", L"" );

    auto asoupPtr = std::make_shared<AsoupEvent>(asoup);
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );
    CPPUNIT_ASSERT( CreatePath(*m_chart, path1, 2) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, asoupPtr ) );
    HappenLayer& hl = m_chart->getHappenLayer();
    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        CPPUNIT_ASSERT ( al.IsServed( asoupPtr) );
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 2 );
        auto path = *pathSet.cbegin();
    }
}


// достраивание до АСОУП: несколько сообщений в неконтролируемой середине
void TC_TrackedChart::MultipleAsoupUncontrolledArea()
{
    Uncontrolled_helper();

    //
    auto path1 = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T065601Z' name='Form' Bdg='6C[09180]' waynum='6' parknum='1' index='0918-031-0987' num='3526' >"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T080206Z' name='Departure' Bdg='6C[09180]' waynum='6' parknum='1' optCode='09180:09280' />"
        L"  <SpotEvent create_time='20180518T081000Z' name='Span_move' Bdg='2GPP[09180:09280]' waynum='1' />"
        L"  <SpotEvent create_time='20180518T081000Z' name='Death' Bdg='2GPP[09180:09280]' waynum='1' />"
        L"</HemPath>");
    auto path2 = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T121643Z' name='Form' Bdg='LT[09355:09860]' waynum='1' index='0918-031-0987' num='3526' />"
        L"  <SpotEvent create_time='20180518T122344Z' name='Arrival' Bdg='5C[09860]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20180518T135149Z' name='Disform' Bdg='5C[09860]' waynum='5' parknum='1' />"
        L"</HemPath>");

    // начальные 2 нити
    CPPUNIT_ASSERT( CreatePath(*m_chart, path1, 1) );
	UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
	
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 4 ) );

    auto currentTime = time_from_iso("20180518T083030Z");
    // выставляем время - одна нить до, вторая после текущего времени (как бы нарисована вручную)
    m_chart->updateTime( currentTime, nullptr );

    // добавляем сразу все события АСОУП - одно запоздалое, остальные опережающие
    const TrainDescr td(L"3526", L"0918-031-0987");
    AsoupEvent begAsoup( HCode::DEPARTURE,  BadgeE( L"aLT", EsrKit(9180) ),
        ParkWayKit(1,6), time_from_iso("20180518T080000Z"), td, L"",      L"09280" );
    AsoupEvent midT20( HCode::TRANSITION,   BadgeE( L"T20", EsrKit(9320) ),
        ParkWayKit(1,1), time_from_iso("20180518T100000Z"), td, L"09303", L"09330" );
    AsoupEvent midT40( HCode::TRANSITION, BadgeE( L"T40", EsrKit(9340) ),
        ParkWayKit(1,3), time_from_iso("20180518T111100Z"), td, L"09330", L"09350" );
    AsoupEvent endAsoup( HCode::DEPARTURE,  BadgeE( L"aRT", EsrKit(9860) ),
        ParkWayKit(1,5), time_from_iso("20180518T122300Z"), td, L"09355", L"" );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(begAsoup), currentTime ) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(midT20), currentTime ) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(midT40), currentTime ) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(endAsoup), currentTime ) );

    // никакие события АСОУП пока не использовались - время не наступило
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 4 ) );

    // сдвигаем время за Т20 - попытка обработать T20 в обеих очередях
    // Очередь из прошлого до АСОУП обработает T20, но не Т40 (время не дошло)
    // Очередь от АСОУП до будущего вообще не сработает - время Т40 ещё не подошло, а Т20 ждет (т.к. для того же поезда)
    m_chart->updateTime( time_from_iso("20180518T100500Z"), nullptr );
    {
		hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] 2GPP[09180:09280] ?[09280] ?[09290] ?[09303] T20[09320] T20[09320] "));

    // сдвигаем время за Т40 - попытка обработать T40 в обеих очередях
    // Очередь из прошлого до АСОУП обработает T40 (там будет на конце и Т20 и Т40)
    m_chart->updateTime( time_from_iso("20180518T112200Z"), nullptr );
    {
        hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }
	CPPUNIT_ASSERT( hl.path_count() == 1 );
	CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] 2GPP[09180:09280] ?[09280] ?[09290] ?[09303] T20[09320] ?[09330] T40[09340] T40[09340] "));

	CPPUNIT_ASSERT( CreatePath(*m_chart, path2, 2) );;
	CPPUNIT_ASSERT( hl.path_count() == 2 );
	CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] 2GPP[09180:09280] ?[09280] ?[09290] ?[09303] T20[09320] ?[09330] T40[09340] T40[09340] "));
	CPPUNIT_ASSERT( hl.exist_series( "Bdg", "LT[09355:09860] 5C[09860] 5C[09860] "));

    //Сдвигаем время за начало второй нити
    // Очередь от АСОУП до будущего обработает Т40 и обе нити сольются т.к. приклеились к одному событию T40
    m_chart->updateTime( time_from_iso("20180518T121700Z"), nullptr );
    {
        hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 14 ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] 2GPP[09180:09280] ?[09280] ?[09290] ?[09303] T20[09320] ?[09330] T40[09340] ?[09350] ?[09351] ?[09355] 5C[09860] 5C[09860] "));
}

// достраивание до АСОУП: несколько сообщений в неконтролируемой середине и даже на одной станции ARRIVAL-DEPARTURE
// Фактическая нить находится ранее всех событий; удлинение происходит вперед по оси времени
void TC_TrackedChart::MultipleAsoupUncontrolledArea_4818_fwd()
{
    Uncontrolled_helper();
    // исходные данные 
    auto path = 
        L"<HemPath>"
        L"  <SpotEvent create_time='20180906T040532Z' name='Form' Bdg='18C[09180]' index='0918-052-0933' num='3519' />"
        L"  <SpotEvent create_time='20180906T040848Z' name='Departure' Bdg='PpGP[09180]' />"
        L"  <SpotEvent create_time='20180906T041213Z' name='Span_move' Bdg='PPG2[09180:09280]' waynum='1' />"
        L"  <SpotEvent create_time='20180906T041533Z' name='Death'     Bdg='PPG2[09180:09280]' waynum='1' />"
        L"</HemPath>";

    const TrainDescr td(L"3519", L"0918-052-0933");

    const time_t t_bgJ( time_from_iso("20180906T040500Z") );
    const time_t t_doA( time_from_iso("20180906T044900Z") );
    const time_t t_doD( time_from_iso("20180906T054400Z") );
    const time_t t_brA( time_from_iso("20180906T072000Z") );
    const time_t t_brD( time_from_iso("20180906T073400Z") );
    const time_t t_saA( time_from_iso("20180906T074500Z") );

    AsoupEvent doA( HCode::ARRIVAL,  BadgeE( L"dA", EsrKit(9290) ), ParkWayKit(1,3), t_doA, td, L"09280", L"" );
    AsoupEvent doD( HCode::DEPARTURE,BadgeE( L"dD", EsrKit(9290) ), ParkWayKit(1,3), t_doD, td,      L"", L"09303" );
    AsoupEvent brA( HCode::ARRIVAL,  BadgeE( L"bA", EsrKit(9320) ), ParkWayKit(1,2), t_brA, td, L"09303", L"" );
    AsoupEvent brD( HCode::DEPARTURE,BadgeE( L"bD", EsrKit(9320) ), ParkWayKit(1,2), t_brD, td,      L"", L"09330" );
    AsoupEvent saA( HCode::ARRIVAL,  BadgeE( L"sA", EsrKit(9330) ), ParkWayKit(1,2), t_saA, td, L"09320", L"" );

    // создать начальную нить
	CPPUNIT_ASSERT( CreatePathWithMsgBorderQueue( path, 1 ) );
    
	UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 4 ) );

    // добавить первое прибытие на Добеле и обработать его
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(doA), t_doA + 5*60 ) );
    CPPUNIT_ASSERT( hl.get_series( "Bdg" ) == "18C[09180] PpGP[09180] PPG2[09180:09280] ?[09280] dA[09290] dA[09290] " );

    // добавить отправление с Добеле и обработать его
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(doD) , t_doD + 8*60 ) );
    CPPUNIT_ASSERT( hl.get_series( "Bdg" ) == "18C[09180] PpGP[09180] PPG2[09180:09280] ?[09280] dA[09290] dD[09290] dD[09290] " );

    // добавить прибытие уже на Броцени и обработать его
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(brA), t_brA + 5*60 ) );
    CPPUNIT_ASSERT( hl.get_series( "Bdg" ) == "18C[09180] PpGP[09180] PPG2[09180:09280] ?[09280] dA[09290] dD[09290] ?[09303] bA[09320] bA[09320] " );

    // добавить отправление теперь с Броцени и обработать его
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(brD), t_brD + 5*60 ) );
    CPPUNIT_ASSERT( hl.get_series( "Bdg" ) == "18C[09180] PpGP[09180] PPG2[09180:09280] ?[09280] dA[09290] dD[09290] ?[09303] bA[09320] bD[09320] bD[09320] " );

    // прибытие на Салдус
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(saA), t_saA + 5*60 ) );
    CPPUNIT_ASSERT( hl.get_series( "Bdg" ) == "18C[09180] PpGP[09180] PPG2[09180:09280] ?[09280] dA[09290] dD[09290] ?[09303] bA[09320] bD[09320] sA[09330] sA[09330] " );
}

// достраивание до АСОУП: несколько сообщений в неконтролируемой середине и даже на одной станции ARRIVAL-DEPARTURE
// Фактическая нить находится после всех событий; удлиняется головная часть нить (т.е. назад по оси времени)
void TC_TrackedChart::MultipleAsoupUncontrolledArea_4818_bkw()
{
    Uncontrolled_helper();
    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    // исходные данные 
    const TrainDescr td(L"3519", L"0918-052-0933");

    const time_t t_doA( time_from_iso("20180906T044900Z") );
    const time_t t_doD( time_from_iso("20180906T054400Z") );
    const time_t t_brT( time_from_iso("20180906T073000Z") );
    const time_t t_saX( time_from_iso("20180906T074900Z") );

    AsoupEvent doA( HCode::ARRIVAL,   BadgeE( L"dA", EsrKit(9290) ), ParkWayKit(1,3), t_doA, td, L"09280", L"" );
    AsoupEvent doD( HCode::DEPARTURE, BadgeE( L"dD", EsrKit(9290) ), ParkWayKit(1,3), t_doD, td,      L"", L"09303" );
    AsoupEvent brT( HCode::TRANSITION,BadgeE( L"bT", EsrKit(9320) ), ParkWayKit(1,2), t_brT, td, L"09303", L"09330" );

    // добавляем все события АСОУП сразу т.к. поочередно их добавлять нет смысла
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(doA) ) );
    m_chart->updateTime( t_doA + 5*60, nullptr );
    CPPUNIT_ASSERT( hl.path_count() == 0 );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(doD) ) );
    m_chart->updateTime( t_doD + 8*60, nullptr );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 3 ) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(brT) ) );
    m_chart->updateTime( t_brT + 5*60, nullptr );
    // Теперь создать конечную нить и сдвигаем время за ее конец
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 5 ) );
    auto pathBack = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180906T074630Z' name='Span_move' Bdg='SM[09320:09330]' index='0918-052-0933' num='3519' />"
        L"  <SpotEvent create_time='20180906T074700Z' name='Arrival' Bdg='P2[09330]' parknum='1' waynum='2' />"
        L"  <SpotEvent create_time='20180906T074900Z' name='Disform' Bdg='P2[09330]' parknum='1' waynum='2' />"
        L"</HemPath>");
    CPPUNIT_ASSERT( CreatePath( *m_chart, pathBack, 1 ) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 7 ) );
    CPPUNIT_ASSERT( hl.get_series( "Bdg" ) == "dA[09290] dD[09290] ?[09303] bT[09320] SM[09320:09330] P2[09330] P2[09330] " );
}


// достраивание до АСОУП: нарисовано в неконтролируемой середине не доходя до АСОУП
void TC_TrackedChart::DrawingUncontrolled_BeforeReachingAsoup()
{
    Uncontrolled_helper();
    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
	UtLayer<AsoupLayer>& al = UtLayer<AsoupLayer>::upgrade( m_chart->getAsoupLayer() );

    auto path1 = 
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T065601Z' name='Form' Bdg='6C[09180]' waynum='6' parknum='1' index='0918-031-0987' num='3526' >"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T080206Z' name='Departure' Bdg='6C[09180]' waynum='6' parknum='1' optCode='09180:09280' />"
        L"  <SpotEvent create_time='20180518T090000Z' name='Transition' Bdg='xx[09280]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180518T091000Z' name='Transition' Bdg='xx[09290]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180518T092000Z' name='Span_move'  Bdg='xx[09290:09303]' waynum='1' />"
        L"  <SpotEvent create_time='20180518T092000Z' name='Death'      Bdg='xx[09290:09303]' waynum='1' />"
        L"</HemPath>";
    auto path2 = 
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T121643Z' name='Form' Bdg='LT[09355:09860]' waynum='1' index='0918-031-0987' num='3526' />"
        L"  <SpotEvent create_time='20180518T122344Z' name='Arrival' Bdg='5C[09860]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20180518T135149Z' name='Disform' Bdg='5C[09860]' waynum='5' parknum='1' />"
        L"</HemPath>";

    // начальные 2 нити
	CPPUNIT_ASSERT(CreatePathWithMsgBorderQueue(path1, 1));
	CPPUNIT_ASSERT(CreatePathWithMsgBorderQueue(path2, 2));
    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size( 6 ) );
    CPPUNIT_ASSERT( hl.exist_path_size( 3 ) );

    time_t currentTime = time_from_iso("20180518T083030Z");
    // выставляем время - одна нить до, вторая после текущего времени (как бы нарисована вручную)
    m_chart->updateTime( currentTime, nullptr );

    // добавляем сразу все события АСОУП - одно запоздалое, остальные опережающие
    const TrainDescr td(L"3526", L"0918-031-0987");
    AsoupEvent midT20( HCode::TRANSITION,   BadgeE( L"T20", EsrKit(9320) ),
        ParkWayKit(1,1), time_from_iso("20180518T100000Z"), td, L"09303", L"09330" );
    AsoupEvent midT40( HCode::TRANSITION, BadgeE( L"T40", EsrKit(9340) ),
        ParkWayKit(1,3), time_from_iso("20180518T111100Z"), td, L"09330", L"09350" );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(midT20), currentTime ) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(midT40), currentTime ) );

    // никакие события АСОУП пока не использовались - время не наступило
    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size( 6 ) );
    CPPUNIT_ASSERT( hl.exist_path_size( 3 ) );

    // сдвигаем время за Т20 - попытка обработать T20 в обеих очередях
    // Очередь из прошлого до АСОУП обработает T20, но не Т40 (время не дошло)
    // Очередь от АСОУП до будущего вообще не сработает - время Т40 ещё не подошло, а Т20 ждет (т.к. для того же поезда)
    m_chart->updateTime( time_from_iso("20180518T100500Z"), nullptr );
    {
        hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }
    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] xx[09280] xx[09290] xx[09290:09303] ?[09303] T20[09320] T20[09320] "));
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "LT[09355:09860] 5C[09860] 5C[09860] "));

    // сдвигаем время за Т40 - попытка обработать T40 в обеих очередях
    // Очередь из прошлого до АСОУП обработает T40 (там будет на конце и Т20 и Т40)
    m_chart->updateTime( time_from_iso("20180518T112200Z"), nullptr );
    {
        hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] xx[09280] xx[09290] xx[09290:09303] ?[09303] T20[09320] ?[09330] T40[09340] T40[09340] "));
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "LT[09355:09860] 5C[09860] 5C[09860] "));

    //сдвигаем время за начало второй нити, 
    // Очередь от АСОУП до будущего обработает Т40
    //обе нити сольются т.к. приклеились к одному событию T40
    m_chart->updateTime( time_from_iso("20180518T121643"), nullptr );
    {
        hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 14 ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] xx[09280] xx[09290] xx[09290:09303] ?[09303] T20[09320] ?[09330] T40[09340] ?[09350] ?[09351] ?[09355] 5C[09860] 5C[09860] "));
}

bool TC_TrackedChart::CreatePathWithMsgBorderQueue(const std::wstring& text, const unsigned trainId)
{
	attic::a_document sourceDoc;
	CPPUNIT_ASSERT(sourceDoc.load_wide(text));

	HappenLayer::EventsList vEv;

	for (attic::a_node node : sourceDoc.document_element().children() )
	{
		HemHelpful::SpotEventPtr complexSpot = std::make_shared<SpotEvent>(node);
		vEv.push_back(complexSpot);
	}

	ConstTrainDescrPtr tdrLast;
	bool res = !vEv.empty();
	for ( auto it : vEv )
	{
		auto tdrCur = it->GetDetails() ? it->GetDetails()->tdptr : nullptr; 
		if ( tdrCur )
			tdrLast = tdrCur;
		res &= m_chart->acceptEvent(it, trainId );
	}

	if ( res )
	{
		auto& hl = m_chart->getHappenLayer();
		const auto& uncStation = m_chart->getUncontrolledStations();
		auto& queue = m_chart->getAsoupQueueMgr();
		const auto& borderStations = m_chart->getUncontrolledStations().getBorderControlledStations();
		auto ev = vEv.front();
		auto tdr = ev->GetDetails() ? ev->GetDetails()->tdptr : nullptr; 

		auto uncContain = [&uncStation, &borderStations ](const EsrKit& esr){
			if ( esr.span() )
			{
				auto pairEsr = esr.split_span();
				return uncStation.contains(pairEsr.first) || uncStation.contains(pairEsr.second);
			}
			else
				return uncStation.contains(esr) || borderStations.have( esr );
		};

		if ( tdr && uncContain(ev->GetBadge().num()) )
			queue.addBorderTrain( *tdr, ev, false, nullptr );

		ev = vEv.back();
		if ( tdrLast && uncContain(ev->GetBadge().num()) )
			queue.addBorderTrain( *tdrLast, ev, false, nullptr );
	}

	return res;
}


// достраивание до АСОУП: нарисовано в неконтролируемой середине с переходом за АСОУП
void TC_TrackedChart::DrawingUncontrolled_WithStepOverAsoup()
{
    Uncontrolled_helper();

	UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
	UtLayer<AsoupLayer>& al = UtLayer<AsoupLayer>::upgrade( m_chart->getAsoupLayer() );

	auto path1 = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T065601Z' name='Form' Bdg='6C[09180]' waynum='6' parknum='1' index='0918-031-0987' num='3526' >"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T080206Z' name='Departure' Bdg='6C[09180]' waynum='6' parknum='1' optCode='09180:09280' />"
        L"  <SpotEvent create_time='20180518T090000Z' name='Transition' Bdg='xx[09280]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180518T091000Z' name='Transition' Bdg='xx[09290]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180518T092000Z' name='Transition' Bdg='yy[09303]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180518T101111Z' name='Transition' Bdg='yy[09320]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180518T101200Z' name='Span_move'  Bdg='yy[09320:09330]' waynum='1' />"
        L"  <SpotEvent create_time='20180518T101200Z' name='Death'      Bdg='yy[09320:09330]' waynum='1' />"
        L"</HemPath>");
    auto path2 = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T121643Z' name='Form' Bdg='LT[09355:09860]' waynum='1' index='0918-031-0987' num='3526' />"
        L"  <SpotEvent create_time='20180518T122344Z' name='Arrival' Bdg='5C[09860]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20180518T135149Z' name='Disform' Bdg='5C[09860]' waynum='5' parknum='1' />"
        L"</HemPath>");

	CPPUNIT_ASSERT(CreatePath(*m_chart, path1, 1));

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 8 ) );

    time_t currentTime = time_from_iso("20180518T083030Z");
    // выставляем время - одна нить до, вторая после текущего времени (как бы нарисована вручную)
    m_chart->updateTime( currentTime, nullptr );

    // добавляем сразу все события АСОУП - одно запоздалое, остальные опережающие
    const TrainDescr td(L"3526", L"0918-031-0987");
    AsoupEvent midT20( HCode::TRANSITION,   BadgeE( L"T20", EsrKit(9320) ),
        ParkWayKit(1,1), time_from_iso("20180518T100500Z"), td, L"09303", L"09330" );
    AsoupEvent midT40( HCode::TRANSITION, BadgeE( L"T40", EsrKit(9340) ),
        ParkWayKit(1,3), time_from_iso("20180518T111100Z"), td, L"09330", L"09350" );
	auto asoupT20 = std::make_shared<AsoupEvent>(midT20);
	auto asoupT40 = std::make_shared<AsoupEvent>(midT40);
    CPPUNIT_ASSERT( PassAsoup( *m_chart, asoupT20, currentTime ) );
    CPPUNIT_ASSERT( PassAsoup( *m_chart,  asoupT40, currentTime ) );

    // никакие события АСОУП пока не использовались - время не наступило
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 8 ) );
	CPPUNIT_ASSERT( !al.IsServed(asoupT20) );
	CPPUNIT_ASSERT( !al.IsServed(asoupT40) );

    // сдвигаем время за Т20 - попытка обработать T20 в обеих очередях
    // Очередь из прошлого до АСОУП
    // - НЕ обработает T20 т.к. рисование прошло через станцию 09320,
    // - удалит из очереди Т20 как неприменимое
    // - не обработает Т40 т.к. время ещё не пришло
    // Очередь от АСОУП до будущего вообще не сработает - время Т40 ещё не подошло, а Т20 ждет (т.к. для того же поезда)
    m_chart->updateTime( time_from_iso("20180518T101200Z"), nullptr );
    {
        hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }
 	CPPUNIT_ASSERT( al.IsServed(asoupT20) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] xx[09280] xx[09290] yy[09303] yy[09320] yy[09320:09330] yy[09320:09330] "));

    // сдвигаем время за Т40 - попытка обработать T40 в обеих очередях
    // Очередь из прошлого до АСОУП обработает T40 (там будет и проход через 09320 нарисованный и Т40)
    m_chart->updateTime( time_from_iso("20180518T112200Z"), nullptr );
    {
        hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }
	CPPUNIT_ASSERT( hl.path_count() == 1 );
	CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] xx[09280] xx[09290] yy[09303] yy[09320] yy[09320:09330] ?[09330] T40[09340] T40[09340] "));

	CPPUNIT_ASSERT( CreatePath(*m_chart, path2, 2)	);

	CPPUNIT_ASSERT( hl.path_count() == 2 );
	CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] xx[09280] xx[09290] yy[09303] yy[09320] yy[09320:09330] ?[09330] T40[09340] T40[09340] "));
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "LT[09355:09860] 5C[09860] 5C[09860] "));

    //сдвигаем время за начало второй нити 
    // Очередь от АСОУП до будущего обработает Т40 
    //и обе нити сольются т.к. приклеились к одному событию T40
    m_chart->updateTime( time_from_iso("20180518T121643Z"), nullptr );
    {
        hl.path_count();
        std::string s1 = *hl.GetFlatContent();
        CPPUNIT_ASSERT( !s1.empty() );
    }

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 14 ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg", "6C[09180] 6C[09180] xx[09280] xx[09290] yy[09303] yy[09320] yy[09320:09330] ?[09330] T40[09340] ?[09350] ?[09351] ?[09355] 5C[09860] 5C[09860] "));
}

void TC_TrackedChart::UncontrolledEdge()
{
    Uncontrolled_helper();

    auto realPath = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20180518T115709Z' name='Form' Bdg='P3P[11002:11451]' waynum='1' index='8626-110-0983' num='J2233' length='56' weight='5471' net_weight='4131' through='Y'>"
        L"    <rwcoord picketing1_val='389~476' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='388~658' picketing1_comm='Вентспилс' />"
        L"    <feat_texts typeinfo='Т' />"
        L"    <Locomotive Series='2ТЭ116' NumLoc='1047' Consec='1' CarrierCode='2'>"
        L"      <Crew EngineDriver='RANCANS' Tim_Beg='2018-05-18 13:25' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T115855Z' name='Transition' Bdg='4/6SP:4+[11451]'>"
        L"    <rwcoord picketing1_val='387~399' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T120520Z' name='Transition' Bdg='5SP[11445]'>"
        L"    <rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20180518T121659Z' name='Transition' Bdg='1C[11443]' waynum='1' parknum='1' intervalSec='2' optCode='11442:11443' />"
        L"  <SpotEvent create_time='20180518T122944Z' name='Arrival' Bdg='1BC[11442]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180518T123352Z' name='Departure' Bdg='1AC[11442]' waynum='1' parknum='1' optCode='11442:11446' />"
        L"  <SpotEvent create_time='20180518T125607Z' name='Arrival' Bdg='1C[11446]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180518T130147Z' name='Departure' Bdg='1C[11446]' waynum='1' parknum='1' optCode='11432:11446' />"
        L"  <SpotEvent create_time='20180518T131153Z' name='Transition' Bdg='1C[11432]' waynum='1' parknum='1' intervalSec='19' optCode='11431:11432' />"
        L"  <SpotEvent create_time='20180518T132327Z' name='Transition' Bdg='1AC[11431]' waynum='1' parknum='2' optCode='11430:11431' />"
        L"  <SpotEvent create_time='20180518T132434Z' name='Transition' Bdg='1C[11431]' waynum='1' parknum='1' optCode='11430:11431' />"
        L"  <SpotEvent create_time='20180518T134055Z' name='Transition' Bdg='1C[11430]' waynum='1' parknum='1' intervalSec='19' optCode='11422:11430' />"
        L"  <SpotEvent create_time='20180518T135508Z' name='Transition' Bdg='1AC[11422]' waynum='1' parknum='1' intervalSec='3' optCode='11419:11422' />"
        L"  <SpotEvent create_time='20180518T140630Z' name='Transition' Bdg='1C[11419]' waynum='1' parknum='1' intervalSec='38' optCode='11419:11420' />"
        L"  <SpotEvent create_time='20180518T142213Z' name='Transition' Bdg='3C[11420]' waynum='3' parknum='1' intervalSec='10' optCode='09190:11420' />"
        L"  <SpotEvent create_time='20180518T143708Z' name='Transition' Bdg='1C[09190]' waynum='1' parknum='1' intervalSec='4' optCode='09190:09191' />"
        L"  <SpotEvent create_time='20180518T144600Z' name='Transition' Bdg='1C[09191]' waynum='1' parknum='1' intervalSec='21' optCode='09191:09193' />"
        L"  <SpotEvent create_time='20180518T145850Z' name='Span_move' Bdg='SS4C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='271~952' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='272~915' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    const TrainDescr td(L"2233", L"8626-110-0983");

    const time_t preTime = time_from_iso("20180518T115000Z");
    AsoupEvent preAsoup( HCode::DEPARTURE,  BadgeE( L"any", EsrKit(11001) ),
        ParkWayKit(1,0), preTime, td, L"",      L"11002" );

    CPPUNIT_ASSERT( CreatePath(*m_chart, realPath, 1) );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathOne = *pathSet.begin();
        CPPUNIT_ASSERT( pathOne->GetAllEvents().size() == 18 );
    }

    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(preAsoup) ) );
    m_chart->updateTime( time_from_iso("20180518T115709Z"), nullptr );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathOne = *pathSet.begin();
        CPPUNIT_ASSERT( pathOne->GetAllEvents().size() == 19 );
    }

}

void TC_TrackedChart::UncontrolledEdge_2()
{
    Uncontrolled_helper();

    auto realPath = loaderSpot(
        L"<HemPath>"
        L"  <SpotEvent create_time='20181016T154857Z' name='Form' Bdg='PN5P[09006:09430]' waynum='1' index='0001-599-0900' num='4891' reslocoutbnd='Y'>"
        L"    <rwcoord picketing1_val='7~' picketing1_comm='Шкиротава-Я' />"
        L"    <rwcoord picketing1_val='7~300' picketing1_comm='Шкиротава-Я' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20181016T155133Z' name='Arrival' Bdg='17JC[09006]' waynum='17' parknum='1' />"
        L"  <SpotEvent create_time='20181016T155559Z' name='Departure' Bdg='17JC[09006]' waynum='17' parknum='1' optCode='09000:09006' index='0001-599-0900' num='4892' reslocoutbnd='Y' />"
        L"  <SpotEvent create_time='20181016T160036Z' name='Transition' Bdg='19AC[09000]' waynum='19' parknum='1' intervalSec='74' optCode='09000' />"
        L"  <SpotEvent create_time='20181016T160226Z' name='Transition' Bdg='15AC[09000]' waynum='15' parknum='1' intervalSec='77' optCode='09000' />"
        L"  <SpotEvent create_time='20181016T160337Z' name='Arrival' Bdg='10AC[09000]' waynum='10' parknum='1' />"
        L"  <SpotEvent create_time='20181016T160337Z' name='Disform' Bdg='10AC[09000]' waynum='10' parknum='1' />"
        L"</HemPath>"
        );

// <AsoupEvent create_time="20181016T154500Z" name="Departure" Bdg="ASOUP 1042[09430]"
//     index="0001-599-0900" num="4891" reslocoutbnd="Y" waynum="1" parknum="1" dirTo="09000" adjTo="09000"><![CDATA[(:1042 909/000+09430 4891 0001 599 0900 03 09000 16 10 18 45 01/01 0 0/00 00 0
// Ю3 557 00062030 1 08 00 0000 00000 RODIONOVS   :)]]><Locomotive Series="ЧМЭ3" NumLoc="6203" Consec="1" CarrierCode="4">
// <Crew EngineDriver="RODIONOVS" Tim_Beg="2018-10-16 08:00" />
// </Locomotive>
// </AsoupEvent>

        const TrainDescr td(L"4891", L"0001-599-0900");

    const time_t preTime = time_from_iso("20181016T154500Z");
    AsoupEvent preAsoup( HCode::DEPARTURE,  BadgeE( L"ASOUP 1042", EsrKit(9430) ),
        ParkWayKit(1,1), preTime, td, L"",      L"09000" );

    CPPUNIT_ASSERT( CreatePath(*m_chart, realPath, 1) );
    {
        UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
        CPPUNIT_ASSERT( hl.path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size( 7 ) );
        CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "PN5P[09006:09430] 17JC[09006] 17JC[09006] 19AC[09000] 15AC[09000] 10AC[09000] 10AC[09000] "));
    }

    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(preAsoup) ) );
    m_chart->updateTime( preTime, nullptr );
    m_chart->updateTime( time_from_iso("20181016T160337Z"), nullptr );
    {
        UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
        CPPUNIT_ASSERT( hl.path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size( 7 ) );
        CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "ASOUP 1042[09430] 17JC[09006] 17JC[09006] 19AC[09000] 15AC[09000] 10AC[09000] 10AC[09000] "));
    }
}

void TC_TrackedChart::Uncontrolled_helper()
{
    attic::a_document uncDoc;
    uncDoc.load_wide(
        L"<Some>"
		//Северная дорога
		L"<UncontrolledSCB ESR='31151' />  <!--Маслово -->"
		L"<UncontrolledSCB ESR='31160' />  <!--Шестихино -->"
		L"<UncontrolledSCB ESR='31139' />  <!--Родионово -->"
		//Рижская дорога
        L"<UncontrolledSCB ESR='07790' />  <!--Пыталово -->"
        L"<UncontrolledSCB ESR='07792' />  <!--Скангали -->"
        L"<UncontrolledSCB ESR='09070' />  <!--Skulte -->"
        L"<UncontrolledSCB ESR='09280' />  <!--Глуда -->"
        L"<UncontrolledSCB ESR='09290' />  <!--Добеле -->"
        L"<UncontrolledSCB ESR='09303' />  <!--Биксти -->"
        L"<UncontrolledSCB ESR='09320' />  <!--Броцени -->"
        L"<UncontrolledSCB ESR='09330' />  <!--Салдус -->"
        L"<UncontrolledSCB ESR='09340' />  <!--Скрунда -->"
        L"<UncontrolledSCB ESR='09350' />  <!--Калвене -->"
        L"<UncontrolledSCB ESR='09351' />  <!--Илмая -->"
        L"<UncontrolledSCB ESR='09355' />  <!--Торе -->"
        L"<UncontrolledSCB ESR='09430' />  <!--Riga Precu -->"
        L"<UncontrolledSCB ESR='09600' />  <!--Saulkrasti -->"
        L"<UncontrolledSCB ESR='09604' />  <!--Incupe -->"
		L"<UncontrolledSCB ESR='09640' />  <!--Mangali -->"
		L"<UncontrolledSCB ESR='09650' />  <!--Саркандаугава -->"
        L"<UncontrolledSCB ESR='09820' />  <!--Вентпилс -->"
		L"<UncontrolledSCB ESR='09840' />  <!--Вентсп-а -->"
		L"<UncontrolledSCB ESR='09850' />  <!--Вентсп-н -->"
        L"<UncontrolledSCB ESR='09990' />  <!--Криевусала -->"
        L"<UncontrolledSCB ESR='11000' />  <!--Daugavpils Pas. -->"
        L"<UncontrolledSCB ESR='11001' />  <!--Daugavpils D -->"
        L"<UncontrolledSCB ESR='11002' />  <!--Daugavpils P(para) -->"
        L"<UncontrolledSCB ESR='11003' />  <!--Daugavpils N(nepara) -->"
        L"<UncontrolledSCB ESR='11010' />  <!--Daugavpils-Sk. -->"
        L"<UncontrolledSCB ESR='11272' />  <!--Резекне-1 -->"
        L"<UncontrolledSCB ESR='12550' />  <!--Радвилискис -->"
		L"<UncontrolledSCB ESR='16170' />  <!--Бигосово -->"
        L"</Some>"
        );
    UncontrolledStations uncontrolledEsr;
    uncontrolledEsr.deserialize(uncDoc.document_element());

	uncontrolledEsr.addBorderStation(EsrKit(11260));
	uncontrolledEsr.addBorderStation(EsrKit(9180));
	uncontrolledEsr.addBorderStation(EsrKit(9860));
	uncontrolledEsr.addBorderStation(EsrKit(9042));
	m_chart->setupUncontrolledStations(uncontrolledEsr);
}

//Перенесено из TC_GuessTransciever
void TC_TrackedChart::TestArrival()
{
    unsigned int const trainID = 64;
    BadgeE A(L"4p", EsrKit(100));
    BadgeE B(L"2p", EsrKit(200));
    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );

    time_t curtime = time( NULL );
    //имитируем прибытие ПЕ от пути "Q" до пути "C" (описание станции см. TrackerInfrastructure)

    SpotPtr s0_form(new TrackerEvent(trainID, HCode::FORM, A, ++curtime));
    SpotPtr s1_arrival(new TrackerEvent(trainID, HCode::ARRIVAL, B, ++curtime));

    // Событий с поездом 64 ещё нет
    CPPUNIT_ASSERT( hl.path_count() == 0 );

    CPPUNIT_ASSERT( m_chart->acceptTrackerEvent(s0_form, trainID, TrainCharacteristics::Source::Guess) );

    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "4p"));
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form" ) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );

    CPPUNIT_ASSERT( m_chart->acceptTrackerEvent(s1_arrival, trainID, TrainCharacteristics::Source::Guess) );

    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "4p 2p"));
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Arrival" ) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
}

//Перенесено из TC_GuessTransciever
void TC_TrackedChart::TestDisform()
{
    unsigned int const trainID = 64;
    time_t const curtime = 1234000;
    // имитируем формирование и расформирование двух поездов по очереди на одном пути
    const BadgeE bdg( L"1путь", EsrKit(11002) );

    SpotPtr s0_form( new TrackerEvent( trainID, HCode::FORM,    bdg, curtime+0 ) );
    SpotPtr s1_disform( new TrackerEvent( trainID, HCode::DISFORM, bdg, curtime+1 ) );
    //SpotPtr s2_form( new TrackerEvent( id, HCode::FORM,    bdg, curtime+2 ) );
    //SpotPtr s3_disform( new TrackerEvent( id, HCode::DISFORM, bdg, curtime+3 ) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );

    // Событий с поездом 64 ещё нет
    CPPUNIT_ASSERT( hl.path_count() == 0 );

    CPPUNIT_ASSERT( m_chart->acceptTrackerEvent(s0_form, trainID, TrainCharacteristics::Source::Guess) );

    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "1путь"));
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form" ) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );

    CPPUNIT_ASSERT( m_chart->acceptTrackerEvent(s1_disform, trainID, TrainCharacteristics::Source::Guess) );


    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "1путь 1путь"));
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Disform" ) );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
}

void TC_TrackedChart::IdentifyWrongPassagirArrivalMessage_5522()
{
    Uncontrolled_helper();

    auto path = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20190916T163654Z' name='Form' Bdg='7C[09710]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20190916T163654Z' name='Departure' Bdg='7C[09710]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20190916T164535Z' name='Transition' Bdg='1C[09680]' waynum='1' parknum='1' intervalSec='12' />"
        L"<SpotEvent create_time='20190916T165225Z' name='Transition' Bdg='1BC[09670]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190916T165853Z' name='Transition' Bdg='1C[09670]' waynum='1' parknum='2' />"
        L"<SpotEvent create_time='20190916T170106Z' name='Transition' Bdg='3AC[09670]' waynum='3' parknum='3' />"
        L"<SpotEvent create_time='20190916T170701Z' name='Transition' Bdg='3p[09100]' waynum='3' parknum='1' intervalSec='3' />"
        L"<SpotEvent create_time='20190916T171304Z' name='Transition' Bdg='9C[09010]' waynum='9' parknum='1' />"
        L"<SpotEvent create_time='20190916T172527Z' name='Transition' Bdg='3JC[09006]' waynum='3' parknum='1' intervalSec='56' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetEventsCount()==9 && path->GetFirstFragment() == nullptr);
    }

    std::wstring asoup9010 = L"<TrainAssignment operation='6' date='16-09-2019 20:16' esr='09010' location='24.121025,56.946324' number='6526' issue_moment='20190916T172009Z'>"
        L"<Unit number='7114-01--ER2T' driver='Robots' driverTime='16-09-2019 19:11' weight='363' length='6' specialConditions='M' carrier='24'>"
        L"<Wagon number='7114-01' axisCount='4' weight='51' length='1' type='1' owner='24' />"
        L"<Wagon number='7114-08' axisCount='4' weight='70' length='1' type='2' owner='24' />"
        L"<Wagon number='7116-08' axisCount='4' weight='70' length='1' type='2' owner='24' />"
        L"<Wagon number='7114-05' axisCount='4' weight='51' length='1' type='3' owner='24' />"
        L"<Wagon number='7116-02' axisCount='4' weight='70' length='1' type='2' owner='24' />"
        L"<Wagon number='7114-09' axisCount='4' weight='51' length='1' type='1' owner='24' />"
        L"</Unit>"
        L"</TrainAssignment>";

      CPPUNIT_ASSERT(m_chart->processNewPassengerLdzWstr(asoup9010, nullptr,  time_from_iso("20190916T171600Z")));

    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && !path->GetFirstFragment() );
    }
}

void TC_TrackedChart::IdentifyPassagirArrivalMessage_5522()
{
    Uncontrolled_helper();

    auto path = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20190916T153255Z' name='Form' Bdg='NKPU1[09100:09104]' waynum='2'>"
        L"<rwcoord picketing1_val='5~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='6~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190916T153851Z' name='Transition' Bdg='1p[09100]' waynum='1' parknum='1' intervalSec='106' />"
        L"<SpotEvent create_time='20190916T154140Z' name='Arrival' Bdg='7C[09010]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20190916T154140Z' name='Disform' Bdg='7C[09010]' waynum='7' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetEventsCount()==4 && path->GetFirstFragment() == nullptr);
    }

    std::wstring asoup9010 = L"<TrainAssignment operation='6' date='16-09-2019 18:41' esr='09010' location='24.121025,56.946324' number='6736' issue_moment='20190916T160009Z'>"
        L"<Unit number='8004-01--ER2' driver='Robots' driverTime='16-09-2019 17:59' weight='343' length='6' specialConditions='M' carrier='24'>"
        L"<Wagon number='8004-01' axisCount='4' weight='51' length='1' type='1' owner='24' />"
        L"<Wagon number='8004-02' axisCount='4' weight='64' length='1' type='2' owner='24' />"
        L"<Wagon number='8004-06' axisCount='4' weight='64' length='1' type='2' owner='24' />"
        L"<Wagon number='8004-05' axisCount='4' weight='50' length='1' type='3' owner='24' />"
        L"<Wagon number='2021-02' axisCount='4' weight='64' length='1' type='2' owner='24' />"
        L"<Wagon number='1317-01' axisCount='4' weight='50' length='1' type='1' owner='24' />"
        L"</Unit>"
        L"</TrainAssignment>";

    CPPUNIT_ASSERT( m_chart->processNewPassengerLdzWstr(asoup9010, nullptr, 0) );
	m_chart->updateTime( time_from_iso("20190916T154140Z"), nullptr );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetFirstFragment()!=nullptr );
    }

}

void TC_TrackedChart::IdentifyForLittleStation_5482()
{
    Uncontrolled_helper();

    auto path = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20190916T153255Z' name='Form' Bdg='NKPU1[09100:09104]' waynum='2'>"
        L"<rwcoord picketing1_val='5~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='6~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190916T153851Z' name='Transition' Bdg='1p[09100]' waynum='1' parknum='1' intervalSec='106' />"
        L"<SpotEvent create_time='20190916T154140Z' name='Transition' Bdg='7C[09010]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20190916T155543Z' name='Span_move' Bdg='R1P[09006:09010]' waynum='1'>"
        L"<rwcoord picketing1_val='3~880' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='3~665' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetEventsCount()==4 && path->GetFirstFragment() == nullptr);
    }

    const TrainDescr td(L"9509", L"0918-031-0987");
    AsoupEvent asoup( HCode::TRANSITION, BadgeE( L"any", EsrKit(9010) ),
        ParkWayKit(1,1), time_from_iso("20190916T153800Z"), td, L"09100", L"09006" );
    auto asoupPtr = std::make_shared<AsoupEvent>(asoup);

    CPPUNIT_ASSERT( PassAsoup( *m_chart, asoupPtr ) );
    AsoupLayer& al = m_chart->getAsoupLayer();

    {
        //проверяем отсутствие идентификации из-за несовпадения пути и парка
        CPPUNIT_ASSERT( !al.IsServed(asoupPtr) );
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetFirstFragment()==nullptr );
    }

    //удаляем сообщение
    CPPUNIT_ASSERT(m_chart->removePreviousMessage(asoup));

    //добавляем описание маленьких станций, не требующих проверки парка и пути
    NsiBasis_helper();

    CPPUNIT_ASSERT( PassAsoup( *m_chart, asoupPtr ) );

    {
        CPPUNIT_ASSERT( al.IsServed(asoupPtr) );
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetFirstFragment()!=nullptr );
    }

}

void TC_TrackedChart::NsiBasis_helper()
{
    const std::string nsiBaseStr = 
        "<Test>"
        "<Junctions>"
        "<Technodes>"
        "<Union ignoreOddness='Y'>"
        "<Station esrCode='09000'/>"
        "<Station esrCode='09008'/>"
        "<Station esrCode='09006'/>"
        "</Union>"
        "<Union>"   
        "<Station esrCode='11000'/>"
        "<Station esrCode='11001'/>"
        "<Station esrCode='11010'/>"
        "</Union>"
        "<Union ignoreOddness='Y'>" 
        "<Station esrCode='11310'/>"
        "<Station esrCode='11320'/>"
        "<Station esrCode='11272'/>"
        "</Union>"
        "</Technodes>"
        "<EqualAreas>"
		"<Union>"   
		"<Station esrCode='09820'/>"
		"<Station esrCode='09840'/>" 
		"<Station esrCode='09850'/>"
		"<Station esrCode='09880'/>"
		"</Union>"
        "<Union>"
        "<Station esrCode='09000'/>"
        "<Station esrCode='09008'/>"
        "<Station esrCode='09006'/>"
        "</Union>"
        "<Union>"
        "<Station esrCode='11310'/>"
        "<Station esrCode='11320'/>"
        "</Union>"
        "<Union>"
        "<Station esrCode='09180'/>"
        "<Station esrCode='09181'/>"
        "</Union>"
        "</EqualAreas>"
        "</Junctions>"
        "<StationsRegistry>"
        "<Station EsrCode='09010'>"
        "<StatWayInfo wayNum='1' parkNum='1'>"
        "<section even_capacity='45' odd_capacity='45' psng='Y' embark='Y' maj='Y' tpassOddness='0' nonStop='' bidCargo='' />"
        "</StatWayInfo>"
        "<StatWayInfo wayNum='2' parkNum='1'>"
        "<section even_capacity='48' odd_capacity='48' psng='' embark='' maj='Y' tpassOddness='0' nonStop='' bidCargo='' />"
        "</StatWayInfo>"
        "<StatWayInfo wayNum='3' parkNum='1'>"
        "<section even_capacity='25' odd_capacity='25' psng='Y' embark='Y' maj='Y' tpassOddness='0' nonStop='' bidCargo='' />"
        "</StatWayInfo>"
        "<StatWayInfo wayNum='4' parkNum='1'>"
        "<section even_capacity='30' odd_capacity='30' psng='Y' embark='Y' maj='' tpassOddness='0' nonStop='' bidCargo='' />"
        "</StatWayInfo>"
        "<PicketingInfo picketing1_val='0~1' picketing1_comm='Рига' picketing2_val='0~1' picketing2_comm='Рига-Земитани' picketing3_val='0~1' picketing3_comm='Рига-Зилупе' picketing4_val='0~1' picketing4_comm='Шкиротава-Я' />"
        "</Station>"
        "</StationsRegistry>"
        "</Test>";

    UtNsiBasis utNsi( nsiBaseStr );
    std::shared_ptr<const NsiBasis> nsiBasisPtr = std::make_shared<const NsiBasis>(utNsi);
    CPPUNIT_ASSERT( nsiBasisPtr );
    m_chart->setupByNsiBasis( nsiBasisPtr );
}

void TC_TrackedChart::IdentifyByDepartureWithoutArrivalNextStation_4689()
{
    NsiBasis_helper();
    const TrainDescr td(L"8232", L"0001-179-1109");
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20180511T062403Z' name='Form' Bdg='4B[11100]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20180511T062403Z' name='Departure' Bdg='4B[11100]' waynum='4' parknum='1' />"
        L"</HemPath>" );

    unsigned const trainID = 61;
    AsoupEvent asoup( HCode::DEPARTURE, BadgeE( L"any", EsrKit(11100) ),
        ParkWayKit(1,4), time_from_iso("20180511T062400Z"), td, L"", L"11091" );
    auto asoupPtr = std::make_shared<AsoupEvent>(asoup);
    CPPUNIT_ASSERT( PassAsoup( *m_chart, asoupPtr) );
    AsoupLayer& al = m_chart->getAsoupLayer();

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    m_chart->updateTime( time_from_iso("20180511T063603Z"), nullptr );

    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==2 && pathCreate->GetFirstFragment() == nullptr && !al.IsServed(asoupPtr) );
    }



//     L"<SpotEvent create_time='20180511T070136Z' name='Span_stopping_begin' Bdg='VB5[11091:11100]' waynum='1' intervalSec='601'>"
//         L"<rwcoord picketing1_val='124~' picketing1_comm='Земитани-Валга' />"
//         L"<rwcoord picketing1_val='125~' picketing1_comm='Земитани-Валга' />"
//         L"</SpotEvent>"

     CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
         std::make_shared<SpotEvent>(HCode::SPAN_STOPPING_BEGIN, BadgeE(L"VB5", EsrKit(11091,11100)), time_from_iso("20180511T070136Z")), trainID, TrainCharacteristics::Source::Guess));
    m_chart->updateTime( time_from_iso("20180511T070136Z"), nullptr );

    {
        //проверяем наличие идентификации
        CPPUNIT_ASSERT( al.IsServed(asoupPtr) );
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetFirstFragment()!=nullptr && *path->GetFirstFragment() == *asoup.GetDescr() );
    }

}

void TC_TrackedChart::IdentifyWithPassagirWithOtherNumber_5570()
{
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20191030T044956Z' name='Form' Bdg='6C[09010]' waynum='6' parknum='1' index='' num='6206' suburbreg='Y' />"
        L"<SpotEvent create_time='20191030T045409Z' name='Departure' Bdg='6C[09010]' waynum='6' parknum='1' />"
        L"<SpotEvent create_time='20191030T050101Z' name='Arrival' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T050205Z' name='Departure' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191030T050845Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='53' optCode='09000:09420' />"
        L"<SpotEvent create_time='20191030T051648Z' name='Arrival' Bdg='2C[09420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191030T051851Z' name='Departure' Bdg='2C[09420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191030T053211Z' name='Arrival' Bdg='1AC[09410]' waynum='1' parknum='1' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==8 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 6206 );
    }

    // до увязки с АСОУП на нити 1 фрагмент

   std::wstring asoup9410 = L"<TrainAssignment operation='6' date='30-10-2019 08:32' esr='09410' location='24.602487,56.815647' number='7732' issue_moment='20191030T054006Z'>"
        L"<Unit number='1307-01--ER2' driver='Kononovs' driverTime='30-10-2019 07:20' weight='344' length='6' specialConditions='M' carrier='24'>"
        L"<Wagon number='1307-01' axisCount='4' weight='51' length='1' type='1' owner='24' />"
        L"<Wagon number='1307-02' axisCount='4' weight='64' length='1' type='2' owner='24' />"
        L"<Wagon number='1307-06' axisCount='4' weight='64' length='1' type='2' owner='24' />"
        L"<Wagon number='1307-05' axisCount='4' weight='50' length='1' type='3' owner='24' />"
        L"<Wagon number='1307-10' axisCount='4' weight='64' length='1' type='2' owner='24' />"
        L"<Wagon number='1307-09' axisCount='4' weight='51' length='1' type='1' owner='24' />"
        L"</Unit>"
        L"</TrainAssignment>";

    CPPUNIT_ASSERT(m_chart->processNewPassengerLdzWstr(asoup9410, nullptr, 0));

    {
        HappenLayer& hl = m_chart->getHappenLayer();
        AsoupLayer& al = m_chart->getAsoupLayer();

        HappenLayer::ReadAccessor container =  m_chart->getHappenLayer().GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==8 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 6206 );

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

}

void TC_TrackedChart::GlueTwoPathesWithEqualNumber_5511()
{
    NsiBasis_helper();
    const TrainDescr td(L"8232", L"0001-179-1109");
    auto path1 = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20191105T135347Z' name='Form' Bdg='5C[09010]' waynum='5' parknum='1' index='' num='6138' length='4' weight='246' suburbreg='Y'>"
        L"<Locomotive NumLoc='7116'>"
        L"<Crew EngineDriver='Kolomojecs' Tim_Beg='2019-11-05 15:58' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191105T135843Z' name='Departure' Bdg='5C[09010]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20191105T140531Z' name='Transition' Bdg='2C[09500]' waynum='2' parknum='1' intervalSec='93' />"
        L"<SpotEvent create_time='20191105T140825Z' name='Transition' Bdg='211SP+[09042]' optCode='09042:09650'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191105T140834Z' name='Span_move' Bdg='2BP[09042:09650]' waynum='2'>"
        L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"<rwcoord picketing1_val='6~735' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191105T140936Z' name='Death' Bdg='2BP[09042:09650]' waynum='2'>"
        L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"<rwcoord picketing1_val='6~735' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    auto path2 = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20191105T140930Z' name='Span_move' Bdg='#6138[09042:09650]' index='' num='6138' suburbreg='Y'>"
        L"<rwcoord picketing1_val='7~125' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191105T141000Z' name='Arrival' Bdg='№6138[09650]' index='' num='6138' suburbreg='Y' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T141100Z' name='Departure' Bdg='№6138[09650]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T141400Z' name='Arrival' Bdg='№6138[09640]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T141430Z' name='Departure' Bdg='№6138[09640]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T141800Z' name='Arrival' Bdg='№6138[09630]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T141900Z' name='Departure' Bdg='№6138[09630]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T142330Z' name='Arrival' Bdg='№6138[09616]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T142400Z' name='Departure' Bdg='№6138[09616]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T143600Z' name='Arrival' Bdg='№6138[09612]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T143700Z' name='Departure' Bdg='№6138[09612]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T143900Z' name='Arrival' Bdg='№6138[09609]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T143930Z' name='Departure' Bdg='№6138[09609]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T144400Z' name='Arrival' Bdg='№6138[09603]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T144430Z' name='Departure' Bdg='№6138[09603]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T145000Z' name='Arrival' Bdg='№6138[09604]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T145030Z' name='Departure' Bdg='№6138[09604]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T145700Z' name='Arrival' Bdg='№6138[09600]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191105T145700Z' name='Death' Bdg='№6138[09600]' waynum='2' parknum='1' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path1, 1) );
    CPPUNIT_ASSERT( CreatePath(*m_chart, path2, 2) );

    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.begin();
        CPPUNIT_ASSERT( path->GetEventsCount()==24);
    }

}

void TC_TrackedChart::GlueTwoPathesWithEqualNumber_5587()
{
    Uncontrolled_helper();
    auto path1 = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20190212T140800Z' name='Departure' Bdg='ASOUP 1042[09820]' waynum='13' parknum='8' index='0982-013-1655' num='VV2432' length='57' weight='1724' through='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='973' Consec='1' CarrierCode='2'>"
        L"<Crew EngineDriver='BONDAREVS' Tim_Beg='2019-02-12 14:50' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190212T142105Z' name='Arrival' Bdg='1C[09813]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190212T144404Z' name='Departure' Bdg='1C[09813]' waynum='1' parknum='1' optCode='09812:09813' />"
        L"<SpotEvent create_time='20190212T145427Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' intervalSec='4' optCode='09810:09812' />"
        L"<SpotEvent create_time='20190212T151602Z' name='Arrival' Bdg='2C[09810]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190212T152306Z' name='Departure' Bdg='2C[09810]' waynum='2' parknum='1' optCode='09803:09810' />"
        L"<SpotEvent create_time='20190212T153450Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' intervalSec='2' optCode='09802:09803' />"
        L"<SpotEvent create_time='20190212T154354Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' intervalSec='3' optCode='09801:09802' />"
        L"<SpotEvent create_time='20190212T155508Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' intervalSec='7' optCode='09800:09801' />"
        L"<SpotEvent create_time='20190212T155915Z' name='Span_move' Bdg='SA6C[09800:09801]' waynum='1'>"
        L"<rwcoord picketing1_val='61~131' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='61~859' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190212T155956Z' name='Death' Bdg='SA6C[09800:09801]' waynum='1'>"
        L"<rwcoord picketing1_val='61~131' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='61~859' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    auto path2 = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20190212T160735Z' name='Form' Bdg='2C[09800]' waynum='2' parknum='1' index='' num='VV2432' through='Y' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path1, 1) );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.begin();
        CPPUNIT_ASSERT( path->GetLastEvent()->GetBadge().num()==EsrKit(9800, 9801) );
    }

    CPPUNIT_ASSERT( CreatePath(*m_chart, path2, 2) );

    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.begin();
        CPPUNIT_ASSERT( path->GetLastEvent()->GetBadge().num()==EsrKit(9800) );
    }

}

void TC_TrackedChart::GlueTwoPathesWithEqualNumber_5590()
{  
   Uncontrolled_helper();
//    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
   m_chart->clearAsoupQueue();
   CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 909/000+09820 2442 0982 027 1800 07 09790 10 10 20 30 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01392 00000 228 62712617 64313885 000 000 00 000 057.60 000 057 000\n"
        L"Ю4 11290 2 83 000 057.60 000 057 \n"
        L"Ю12 00 62712617 1 000 85270 16118 2738 300 00 00 00 00 00 0249 12 60 0600 04 100 18000 00000 11290 83 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        L"Ю12 00 62817507 255255255255 \n"
        L"Ю12 00 62822085 255239255255 0248\n"
        L"Ю12 00 54017116 203237255255 864701611740002405600\n"
        L"Ю12 00 54770508 255239255255 0236\n"
        L"Ю12 00 60517828 219237255255 8527050002460600\n"
        L"Ю12 00 62952478 255239255255 0249\n"
        L"Ю12 00 64039274 195239255255 861401611780230000238\n"
        L"Ю12 00 63854251 195239255255 862201611758603000245\n"
        L"Ю12 00 62639539 255255255255 \n"
        L"Ю12 00 63450647 255255255255 \n"
        L"Ю12 00 62981717 255255255255 \n"
        L"Ю12 00 62981519 255255255255 \n"
        L"Ю12 00 62842679 251255255255 400\n"
        L"Ю12 00 55183958 255253255255 5600\n"
        L"Ю12 00 61955415 219237255255 8621000002390600\n"
        L"Ю12 00 55184022 223237255255 8622002455600\n"
        L"Ю12 00 62305842 223237255255 8621002390600\n"
        L"Ю12 00 56147176 199237255255 8647016117273802345600\n"
        L"Ю12 00 63910236 195237255255 8622016117586050002450600\n"
        L"Ю12 00 60831682 255255255255 \n"
        L"Ю12 00 61484531 255255255255 \n"
        L"Ю12 00 61989620 195239255255 852701611827380000244\n"
        L"Ю12 00 61957817 235239255255 161173000247\n"
        L"Ю12 00 62710330 255239255255 0249\n"
        L"Ю12 00 62795133 255239255255 0247\n"
        L"Ю12 00 63172084 255239255255 0248\n"
        L"Ю12 00 62809629 255239255255 0250\n"
        L"Ю12 00 63636724 195239255255 862201611758600000245\n"
        L"Ю12 00 62296777 199239255255 852701611827380243\n"
        L"Ю12 00 63636914 199239255255 862201611758600245\n"
        L"Ю12 00 62813571 195239255255 852701611827384000248\n"
        L"Ю12 00 62954839 255239255255 0249\n"
        L"Ю12 00 64319031 195239255255 862201611758605000245\n"
        L"Ю12 00 64316425 255255255255 \n"
        L"Ю12 00 64319080 255255255255 \n"
        L"Ю12 00 60481116 195239255255 861401611780233000240\n"
        L"Ю12 00 64121148 255239255255 0238\n"
        L"Ю12 00 61756680 255239255255 0239\n"
        L"Ю12 00 61765160 255239255255 0240\n"
        L"Ю12 00 64061583 255239255255 0239\n"
        L"Ю12 00 60593787 251239255255 4000235\n"
        L"Ю12 00 64085616 255239255255 0237\n"
        L"Ю12 00 64084585 255239255255 0239\n"
        L"Ю12 00 61140638 195239255255 862101611758600000238\n"
        L"Ю12 00 63810659 199255255255 86140161178023\n"
        L"Ю12 00 62824826 195239255255 852701611727385000250\n"
        L"Ю12 00 64313448 255239255255 0247\n"
        L"Ю12 00 64313455 255255255255 \n"
        L"Ю12 00 64313463 255239255255 0248\n"
        L"Ю12 00 61934444 255239255255 0246\n"
        L"Ю12 00 62797048 235239255255 161183000247\n"
        L"Ю12 00 62772629 255239255255 0249\n"
        L"Ю12 00 63230320 255239255255 0248\n"
        L"Ю12 00 64313505 255239255255 0247\n"
        L"Ю12 00 64313513 255255255255 \n"
        L"Ю12 00 64313885 251255255255 000:)"
        ));

   CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:904 0982 2442 0982 27 1800 1 10 10 20 30 057 01392 0 0000 0 0\n"
        L"01 62712617 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"02 62817507 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"03 62822085 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"04 54017116 0202 000 86475  16117 2738 4 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"05 54770508 0202 000 86475  16117 2738 4 0 0 0 00/00 11020 000 09826  0236 0100 0700\n"
        L"06 60517828 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0246 0100 0750\n"
        L"07 62952478 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"08 64039274 0202 000 86146  16117 8023 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"09 63854251 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"10 62639539 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"11 63450647 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"12 62981717 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"13 62981519 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"14 62842679 0202 000 86221  16117 5860 4 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"15 55183958 0202 000 86221  16117 5860 4 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"16 61955415 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"17 55184022 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"18 62305842 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"19 56147176 0202 000 86475  16117 2738 0 0 0 0 00/00 11020 000 09826  0234 0100 0700\n"
        L"20 63910236 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"21 60831682 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"22 61484531 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"23 61989620 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0244 0100 0750\n"
        L"24 61957817 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"25 62710330 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"26 62795133 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"27 63172084 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"28 62809629 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0250 0100 0750\n"
        L"29 63636724 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"30 62296777 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0243 0100 0750\n"
        L"31 63636914 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"32 62813571 0202 000 85278  16118 2738 4 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"33 62954839 0202 000 85278  16118 2738 4 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"34 64319031 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"35 64316425 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"36 64319080 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"37 60481116 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"38 64121148 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"39 61756680 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"40 61765160 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"41 64061583 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"42 60593787 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0235 0100 0700\n"
        L"43 64085616 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0237 0100 0695\n"
        L"44 64084585 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"45 61140638 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"46 63810659 0202 000 86146  16117 8023 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"47 62824826 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0250 0100 0750\n"
        L"48 64313448 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"49 64313455 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"50 64313463 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"51 61934444 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0246 0100 0750\n"
        L"52 62797048 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"53 62772629 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"54 63230320 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"55 64313505 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"56 64313513 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"57 64313885 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0247 0100 0750:)"
        ));

CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 909/000+09820 2442 0982 027 1800 02 09750 10 10 21 21 08/13 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01392 00000 228 62712617 64313885 000 000 00 000 057.60 000 057 000\n"
        L"Ю4 11290 2 83 000 057.60 000 057 :)"
        ));

CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 909/000+09820 2442 0982 027 1800 03 09790 10 10 21 36 08/13 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01392 00000 228 62712617 64313885 000 000 00 000 057.60 000 057 000\n"
        L"Ю3 530 00010601 1 16 15 0000 00000 GRODIS      \n"
        L"Ю3 530 00010602 9\n"
        L"Ю4 11290 2 83 000 057.60 000 057 \n"
        L"Ю12 00 62712617 1 000 85270 16118 2738 300 00 00 00 00 00 0249 12 60 0600 04 100 18000 00000 11290 83 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        L"Ю12 00 62817507 255255255255 \n"
        L"Ю12 00 62822085 255239255255 0248\n"
        L"Ю12 00 54017116 203237255255 864701611740002405600\n"
        L"Ю12 00 54770508 255239255255 0236\n"
        L"Ю12 00 60517828 219237255255 8527050002460600\n"
        L"Ю12 00 62952478 255239255255 0249\n"
        L"Ю12 00 64039274 195239255255 861401611780230000238\n"
        L"Ю12 00 63854251 195239255255 862201611758603000245\n"
        L"Ю12 00 62639539 255255255255 \n"
        L"Ю12 00 63450647 255255255255 \n"
        L"Ю12 00 62981717 255255255255 \n"
        L"Ю12 00 62981519 255255255255 \n"
        L"Ю12 00 62842679 251255255255 400\n"
        L"Ю12 00 55183958 255253255255 5600\n"
        L"Ю12 00 61955415 219237255255 8621000002390600\n"
        L"Ю12 00 55184022 223237255255 8622002455600\n"
        L"Ю12 00 62305842 223237255255 8621002390600\n"
        L"Ю12 00 56147176 199237255255 8647016117273802345600\n"
        L"Ю12 00 63910236 195237255255 8622016117586050002450600\n"
        L"Ю12 00 60831682 255255255255 \n"
        L"Ю12 00 61484531 255255255255 \n"
        L"Ю12 00 61989620 195239255255 852701611827380000244\n"
        L"Ю12 00 61957817 235239255255 161173000247\n"
        L"Ю12 00 62710330 255239255255 0249\n"
        L"Ю12 00 62795133 255239255255 0247\n"
        L"Ю12 00 63172084 255239255255 0248\n"
        L"Ю12 00 62809629 255239255255 0250\n"
        L"Ю12 00 63636724 195239255255 862201611758600000245\n"
        L"Ю12 00 62296777 199239255255 852701611827380243\n"
        L"Ю12 00 63636914 199239255255 862201611758600245\n"
        L"Ю12 00 62813571 195239255255 852701611827384000248\n"
        L"Ю12 00 62954839 255239255255 0249\n"
        L"Ю12 00 64319031 195239255255 862201611758605000245\n"
        L"Ю12 00 64316425 255255255255 \n"
        L"Ю12 00 64319080 255255255255 \n"
        L"Ю12 00 60481116 195239255255 861401611780233000240\n"
        L"Ю12 00 64121148 255239255255 0238\n"
        L"Ю12 00 61756680 255239255255 0239\n"
        L"Ю12 00 61765160 255239255255 0240\n"
        L"Ю12 00 64061583 255239255255 0239\n"
        L"Ю12 00 60593787 251239255255 4000235\n"
        L"Ю12 00 64085616 255239255255 0237\n"
        L"Ю12 00 64084585 255239255255 0239\n"
        L"Ю12 00 61140638 195239255255 862101611758600000238\n"
        L"Ю12 00 63810659 199255255255 86140161178023\n"
        L"Ю12 00 62824826 195239255255 852701611727385000250\n"
        L"Ю12 00 64313448 255239255255 0247\n"
        L"Ю12 00 64313455 255255255255 \n"
        L"Ю12 00 64313463 255239255255 0248\n"
        L"Ю12 00 61934444 255239255255 0246\n"
        L"Ю12 00 62797048 235239255255 161183000247\n"
        L"Ю12 00 62772629 255239255255 0249\n"
        L"Ю12 00 63230320 255239255255 0248\n"
        L"Ю12 00 64313505 255239255255 0247\n"
        L"Ю12 00 64313513 255255255255 \n"
        L"Ю12 00 64313885 251255255255 000:)"
        ));

CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:904 0982 2442 0982 27 1800 1 10 10 21 36 057 01392 0 0000 0 0\n"
        L"01 62712617 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"02 62817507 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"03 62822085 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"04 54017116 0202 000 86475  16117 2738 4 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"05 54770508 0202 000 86475  16117 2738 4 0 0 0 00/00 11020 000 09826  0236 0100 0700\n"
        L"06 60517828 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0246 0100 0750\n"
        L"07 62952478 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"08 64039274 0202 000 86146  16117 8023 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"09 63854251 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"10 62639539 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"11 63450647 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"12 62981717 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"13 62981519 0202 000 86221  16117 5860 3 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"14 62842679 0202 000 86221  16117 5860 4 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"15 55183958 0202 000 86221  16117 5860 4 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"16 61955415 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"17 55184022 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"18 62305842 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"19 56147176 0202 000 86475  16117 2738 0 0 0 0 00/00 11020 000 09826  0234 0100 0700\n"
        L"20 63910236 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"21 60831682 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"22 61484531 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"23 61989620 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0244 0100 0750\n"
        L"24 61957817 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"25 62710330 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"26 62795133 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"27 63172084 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"28 62809629 0202 000 85278  16117 2738 3 0 0 0 00/00 11020 000 09826  0250 0100 0750\n"
        L"29 63636724 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"30 62296777 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0243 0100 0750\n"
        L"31 63636914 0202 000 86221  16117 5860 0 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"32 62813571 0202 000 85278  16118 2738 4 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"33 62954839 0202 000 85278  16118 2738 4 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"34 64319031 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"35 64316425 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"36 64319080 0202 000 86221  16117 5860 5 0 0 0 00/00 11020 000 09826  0245 0100 0750\n"
        L"37 60481116 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"38 64121148 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"39 61756680 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"40 61765160 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0240 0100 0695\n"
        L"41 64061583 0202 000 86146  16117 8023 3 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"42 60593787 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0235 0100 0700\n"
        L"43 64085616 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0237 0100 0695\n"
        L"44 64084585 0202 000 86146  16117 8023 4 0 0 0 00/00 11020 000 09826  0239 0100 0695\n"
        L"45 61140638 0202 000 86218  16117 5860 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"46 63810659 0202 000 86146  16117 8023 0 0 0 0 00/00 11020 000 09826  0238 0100 0695\n"
        L"47 62824826 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0250 0100 0750\n"
        L"48 64313448 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"49 64313455 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"50 64313463 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"51 61934444 0202 000 85278  16117 2738 5 0 0 0 00/00 11020 000 09826  0246 0100 0750\n"
        L"52 62797048 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"53 62772629 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0249 0100 0750\n"
        L"54 63230320 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0248 0100 0750\n"
        L"55 64313505 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"56 64313513 0202 000 85278  16118 2738 3 0 0 0 00/00 11020 000 09826  0247 0100 0750\n"
        L"57 64313885 0202 000 85278  16118 2738 0 0 0 0 00/00 11020 000 09826  0247 0100 0750:)"
        ));

	m_chart->addBorderControlled( EsrKit(9813) );

    // первая нить
    auto path1 =
        L"<HemPath>"
        L"<SpotEvent create_time='20151010T195851Z' name='Arrival' Bdg='1C[09813]' waynum='1' parknum='1' num='V2442' />"
        L"<SpotEvent create_time='20151010T195924Z' name='Departure' Bdg='1C[09813]' waynum='1' parknum='1' optCode='09812:09813' />"
        L"<SpotEvent create_time='20151010T200955Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' intervalSec='4' optCode='09810:09812' />"
        L"<SpotEvent create_time='20151010T202850Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' intervalSec='9' optCode='09803:09810' />"
        L"<SpotEvent create_time='20151010T204037Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' intervalSec='3' optCode='09802:09803' />"
        L"<SpotEvent create_time='20151010T205141Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' intervalSec='4' optCode='09801:09802' />"
        L"<SpotEvent create_time='20151010T210356Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' intervalSec='10' optCode='09800:09801' />"
        L"<SpotEvent create_time='20151010T211827Z' name='Transition' Bdg='2C[09800]' waynum='2' parknum='1' intervalSec='7' optCode='09790:09800' />"
        L"<SpotEvent create_time='20151010T212742Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' intervalSec='8' optCode='09780:09790' />"
        L"<SpotEvent create_time='20151010T213912Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' intervalSec='2' optCode='09751:09780' />"
        L"<SpotEvent create_time='20151010T215335Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' intervalSec='3' optCode='09750:09751' />"
        L"<SpotEvent create_time='20151010T220405Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' intervalSec='3' optCode='09750:09772' />"
         L"</HemPath>";

	CPPUNIT_ASSERT(CreatePathWithMsgBorderQueue(path1, 1));

    // Зато после заданного интервала времени производится повторная попытка идентификации,
    // которая должна завершиться успешно
    m_chart->updateTime(time_from_iso("20151010T200000Z"), nullptr);
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.begin();
        auto firstEvent = path->GetFirstEvent();
		CPPUNIT_ASSERT( firstEvent->GetBadge().num()==EsrKit(9820) && firstEvent->GetCode()==HCode::EXPLICIT_FORM);
    }

    m_chart->updateTime(time_from_iso("20151010T200001Z"), nullptr);

    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.begin();
        auto firstEvent = path->GetFirstEvent();
        CPPUNIT_ASSERT( firstEvent->GetBadge().num()==EsrKit(9820) && firstEvent->GetCode()==HCode::EXPLICIT_FORM);
    }
//	set_TimeZone("");
}

void TC_TrackedChart::IdentifyByArrival_5621()
{
    Uncontrolled_helper();
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20191204T095343Z' name='Form' Bdg='3JC[09006]' waynum='3' parknum='1' index='0001-031-0918' num='4901' joincrgloc='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='1259' Consec='1' CarrierCode='2'>"
        L"<Crew EngineDriver='RANCANS' Tim_Beg='2019-12-04 06:00' />"
        L"</Locomotive>"
        L"<Locomotive Series='2ТЭ116' NumLoc='973' Consec='4' CarrierCode='2'>"
        L"<Crew EngineDriver='PETROVS' Tim_Beg='2019-12-04 04:23' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191204T100525Z' name='Departure' Bdg='3JC[09006]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20191204T101427Z' name='Transition' Bdg='2C[09010]' waynum='2' parknum='1' intervalSec='59' />"
        L"<SpotEvent create_time='20191204T101906Z' name='Transition' Bdg='2p[09100]' waynum='2' parknum='1' intervalSec='55' />"
        L"<SpotEvent create_time='20191204T102613Z' name='Transition' Bdg='KPU13A/6A[09104]'>"
        L"<rwcoord picketing1_val='5~750' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='8~800' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191204T103718Z' name='Arrival' Bdg='4p[09150]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20191204T103802Z' name='Departure' Bdg='4Ap[09150]' waynum='4' parknum='1' optCode='09150:09160' />"
        L"<SpotEvent create_time='20191204T104854Z' name='Transition' Bdg='1p[09160]' waynum='1' parknum='1' intervalSec='102' optCode='09160:09162' />"
        L"<SpotEvent create_time='20191204T105610Z' name='Arrival' Bdg='AGP[09162]' waynum='1' parknum='1'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191204T105927Z' name='Departure' Bdg='AGP[09162]' waynum='1' parknum='1' optCode='09180'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==10 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 4901 );
    }

    std::wstring asoup9180 = L"(:1042 909/000+09180 4901 0001 031 0918 01 09150 04 12 14 01 02/45 1 0/00 00 0:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup9180,
        boost::gregorian::date(2019, 12, 04), nullptr, 0) );

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

//    m_chart->updateTime( time_from_iso("20191204T105927Z"), nullptr );

    //L"<SpotEvent create_time='20191204T110507Z' name='Transition' Bdg='14C[09180]' waynum='14' parknum='1' />"
    SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
        ParkWayKit(1, 14), std::make_pair(PicketingInfo(), PicketingInfo()));
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
        std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"14C", EsrKit(9180)), time_from_iso("20191204T110507Z"), spotDetails), trainID, TrainCharacteristics::Source::Guess));
    m_chart->updateTime( time_from_iso("20191204T110507Z"), nullptr );

    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==11 );

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
    }
}


void TC_TrackedChart::WrongGlueTwoPathesWithEqualNumber_5636()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    Uncontrolled_helper();
    CPPUNIT_ASSERT(m_chart->processNewAsoupText(L"(:1042 909/000+09000 2740 0970 053 8014 03 11760 09 01 13 50 04/06 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 050 04792 03585 200 60030525 53086575 000 000 00 050 000.60 050 000 000\n"
        L"Ю3 550 00002241 1 10 55 0000 00000 JERMAKOVS   \n"
        L"Ю3 550 00002242 9\n"
        L"Ю4 11290 2 80 050 000.60 050 000 \n"
        L"Ю12 00 60030525 1 074 80140 14206 5341 200 00 00 00 00 00 0245 20 60 0600 04 100 80140 00000 11290 80 09700 07 00000 0000 020 1 1280 097421 132 032 00000000\n"
        L"Ю12 00 64409089 255239255245 0248BЫKЛTO000\n"
        L"Ю12 00 54025788 191237255247 06902335600097421\n"
        L"Ю12 00 62599220 191237255255 07402450600\n"
        L"Ю12 00 61585600 191255255255 068\n"
        L"Ю12 00 60779774 191239255255 0690240\n"
        L"Ю12 00 68236231 191237255255 07402290608\n"
        L"Ю12 00 68048289 191239255255 0690224\n"
        L"Ю12 00 55026942 191237255247 06802355600BЫKЛTO\n"
        L"Ю12 00 64426828 191237255255 07202480600\n"
        L"Ю12 00 64409121 191255255247 069097421\n"
        L"Ю12 00 64409055 191239255255 0750247\n"
        L"Ю12 00 64409071 191239255255 0740248\n"
        L"Ю12 00 57636219 191237255247 06702355600BЫKЛTO\n"
        L"Ю12 00 60190717 191237255255 06902450600\n"
        L"Ю12 00 64409030 191239255247 0700248097421\n"
        L"Ю12 00 64409048 191255255255 074\n"
        L"Ю12 00 55998397 191237255247 06702335600BЫKЛTO\n"
        L"Ю12 00 62567136 191237255255 07502450600\n"
        L"Ю12 00 64409139 191239255247 0740248097421\n"
        L"Ю12 00 63753511 191239255255 0690247\n"
        L"Ю12 00 62476742 191239255255 0740245\n"
        L"Ю12 00 60958436 255255255255 \n"
        L"Ю12 00 63119754 191239255255 0750247\n"
        L"Ю12 00 68344225 191237255255 07402290608\n"
        L"Ю12 00 62575782 255237255255 02450600\n"
        L"Ю12 00 55327720 191237255255 06902355600\n"
        L"Ю12 00 62626544 191237255255 07402450600\n"
        L"Ю12 00 63706733 255239255255 0247\n"
        L"Ю12 00 55357529 191237255255 06902345600\n"
        L"Ю12 00 62495338 191237255255 07402450600\n"
        L"Ю12 00 55175848 191237255255 07002375600\n"
        L"Ю12 00 56388705 191239255255 0690243\n"
        L"Ю12 00 61585329 191237255255 06802450600\n"
        L"Ю12 00 62629845 191255255255 074\n"
        L"Ю12 00 63708382 191239255255 0750247\n"
        L"Ю12 00 52212768 191237255255 06902335600\n"
        L"Ю12 00 63289003 191237255255 07402470600\n"
        L"Ю12 00 63289680 255255255255 \n"
        L"Ю12 00 62598685 255239255255 0245\n"
        L"Ю12 00 63433122 255239255255 0247\n"
        L"Ю12 00 55347454 191237255255 06902355600\n"
        L"Ю12 00 62623111 191237255255 07502450600\n"
        L"Ю12 00 58761941 191237255255 07002355600\n"
        L"Ю12 00 61818175 191237255255 07502450600\n"
        L"Ю12 00 56261449 191237255255 07002335600\n"
        L"Ю12 00 56987951 255239255255 0236\n"
        L"Ю12 00 56240666 255239255255 0233\n"
        L"Ю12 00 59956664 255239255255 0237\n"
        L"Ю12 00 53086575 255239255255 0232:)",
        boost::gregorian::date(2020, 1, 9), nullptr, 0
        ));

    auto path1 = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20200109T071209Z' name='Form' Bdg='6JC[09006]' waynum='6' parknum='1' index='' num='2740' through='Y' />"
        L"<SpotEvent create_time='20200109T081021Z' name='Departure' Bdg='6JC[09006]' waynum='6' parknum='1' />"
        L"<SpotEvent create_time='20200109T114937Z' name='Death' Bdg='6JC[09006]' waynum='6' parknum='1' />"
        L"</HemPath>"
        );

    auto path2 = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20200109T115233Z' name='Station_exit' Bdg='4UP[09006]' />"
        L"<SpotEvent create_time='20200109T115721Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' intervalSec='46' />"
        L"<SpotEvent create_time='20200109T120026Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='37' optCode='09000:09420' />"
        L"<SpotEvent create_time='20200109T121444Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' intervalSec='21' />"
        L"<SpotEvent create_time='20200109T123309Z' name='Transition' Bdg='1C[09410]' waynum='1' parknum='1' intervalSec='14' />"
        L"<SpotEvent create_time='20200109T125558Z' name='Transition' Bdg='2C[09400]' waynum='2' parknum='1' intervalSec='23' optCode='09380:09400' />"
        L"<SpotEvent create_time='20200109T131843Z' name='Transition' Bdg='2C[09380]' waynum='2' parknum='1' intervalSec='25' optCode='09370:09380' />"
        L"<SpotEvent create_time='20200109T132810Z' name='Transition' Bdg='2C[09370]' waynum='2' parknum='1' intervalSec='19' />"
        L"<SpotEvent create_time='20200109T134031Z' name='Transition' Bdg='2C[09360]' waynum='2' parknum='1' intervalSec='4' />"
        L"<SpotEvent create_time='20200109T135725Z' name='Transition' Bdg='2AC[11760]' waynum='2' parknum='2' intervalSec='5' />"
        L"<SpotEvent create_time='20200109T135752Z' name='Transition' Bdg='2C[11760]' waynum='2' parknum='1' optCode='11420:11760' />"
        L"<SpotEvent create_time='20200109T141735Z' name='Arrival' Bdg='4C[11420]' waynum='4' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path1, 1) );
    CPPUNIT_ASSERT( CreatePath(*m_chart, path2, 2) );

    HappenLayer& hl = m_chart->getHappenLayer();
    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );

        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 2 );

        for ( auto it : pathSet )
        {
            const auto& path = *it;
            CPPUNIT_ASSERT ( path.GetLastEvent()->GetBadge().num()==EsrKit(9006) && path.GetFirstFragment() || 
                path.GetLastEvent()->GetBadge().num()==EsrKit(11420) &&  !path.GetFirstFragment());

        }
    }

    //    m_chart->updateTime(time_from_iso("20151111T220400Z"), nullptr);
    CPPUNIT_ASSERT(m_chart->processNewAsoupText(L"(:1042 909/000+11760 2740 0970 053 8014 04 09000+11420 09 01 15 57 01/02 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 050 04792 03585 200 60030525 53086575 000 000 00 050 000.60 050 000 000\n"
        L"Ю3 550 00002241 1 10 55 0000 00000 JERMAKOVS   \n"
        L"Ю3 550 00002242 9\n"
        L"Ю4 11290 2 80 050 000.60 050 000 \n"
        L"Ю12 00 60030525 1 074 80140 14206 5341 200 00 00 00 00 00 0245 20 60 0600 04 100 80140 00000 11290 80 09700 07 00000 0000 020 1 1280 097421 132 032 00000000\n"
        L"Ю12 00 64409089 255239255245 0248BЫKЛTO000\n"
        L"Ю12 00 54025788 191237255247 06902335600097421\n"
        L"Ю12 00 62599220 191237255255 07402450600\n"
        L"Ю12 00 61585600 191255255255 068\n"
        L"Ю12 00 60779774 191239255255 0690240\n"
        L"Ю12 00 68236231 191237255255 07402290608\n"
        L"Ю12 00 68048289 191239255255 0690224\n"
        L"Ю12 00 55026942 191237255247 06802355600BЫKЛTO\n"
        L"Ю12 00 64426828 191237255255 07202480600\n"
        L"Ю12 00 64409121 191255255247 069097421\n"
        L"Ю12 00 64409055 191239255255 0750247\n"
        L"Ю12 00 64409071 191239255255 0740248\n"
        L"Ю12 00 57636219 191237255247 06702355600BЫKЛTO\n"
        L"Ю12 00 60190717 191237255255 06902450600\n"
        L"Ю12 00 64409030 191239255247 0700248097421\n"
        L"Ю12 00 64409048 191255255255 074\n"
        L"Ю12 00 55998397 191237255247 06702335600BЫKЛTO\n"
        L"Ю12 00 62567136 191237255255 07502450600\n"
        L"Ю12 00 64409139 191239255247 0740248097421\n"
        L"Ю12 00 63753511 191239255255 0690247\n"
        L"Ю12 00 62476742 191239255255 0740245\n"
        L"Ю12 00 60958436 255255255255 \n"
        L"Ю12 00 63119754 191239255255 0750247\n"
        L"Ю12 00 68344225 191237255255 07402290608\n"
        L"Ю12 00 62575782 255237255255 02450600\n"
        L"Ю12 00 55327720 191237255255 06902355600\n"
        L"Ю12 00 62626544 191237255255 07402450600\n"
        L"Ю12 00 63706733 255239255255 0247\n"
        L"Ю12 00 55357529 191237255255 06902345600\n"
        L"Ю12 00 62495338 191237255255 07402450600\n"
        L"Ю12 00 55175848 191237255255 07002375600\n"
        L"Ю12 00 56388705 191239255255 0690243\n"
        L"Ю12 00 61585329 191237255255 06802450600\n"
        L"Ю12 00 62629845 191255255255 074\n"
        L"Ю12 00 63708382 191239255255 0750247\n"
        L"Ю12 00 52212768 191237255255 06902335600\n"
        L"Ю12 00 63289003 191237255255 07402470600\n"
        L"Ю12 00 63289680 255255255255 \n"
        L"Ю12 00 62598685 255239255255 0245\n"
        L"Ю12 00 63433122 255239255255 0247\n"
        L"Ю12 00 55347454 191237255255 06902355600\n"
        L"Ю12 00 62623111 191237255255 07502450600\n"
        L"Ю12 00 58761941 191237255255 07002355600\n"
        L"Ю12 00 61818175 191237255255 07502450600\n"
        L"Ю12 00 56261449 191237255255 07002335600\n"
        L"Ю12 00 56987951 255239255255 0236\n"
        L"Ю12 00 56240666 255239255255 0233\n"
        L"Ю12 00 59956664 255239255255 0237\n"
        L"Ю12 00 53086575 255239255255 0232:)",
        boost::gregorian::date(2020, 1, 9), nullptr, 0
        ));

    m_chart->updateTime( time_from_iso("20200109T135700Z"), nullptr );
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
        for ( auto it : pathAsoupList )
        {
            CPPUNIT_ASSERT ( it->GetBadge().num()==EsrKit(9000) && al.IsServed( it )  || 
                it->GetBadge().num()==EsrKit(11760) &&  al.IsServed( it ) );
        }

        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 2 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT ( path->GetFirstFragment() );
        path = *pathSet.crbegin();
        CPPUNIT_ASSERT ( path->GetFirstFragment() );
    }
    set_TimeZone("");
}


void TC_TrackedChart::AsoupWrongLinkInfoChanging_5652()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    Uncontrolled_helper();
    auto path = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20200129T130337Z' name='Form' Bdg='2C[11311]' waynum='2' parknum='1' index='0986-096-1800' num='2324' length='57' weight='1231' through='Y'>"
        L"<feat_texts typeinfo='Д' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200129T130337Z' name='Departure' Bdg='2C[11311]' waynum='2' parknum='1' optCode='11311:11321' />"
        L"<SpotEvent create_time='20200129T131642Z' name='Transition' Bdg='701SP:701+[11321]'>"
        L"<rwcoord picketing1_val='223~200' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200129T132406Z' name='Arrival' Bdg='5C[11310]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );

    {
        HappenLayer& hl = m_chart->getHappenLayer();

        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );

        const auto& path = **pathSet.begin();
        CPPUNIT_ASSERT ( path.GetFirstEvent()->GetBadge().num()==EsrKit(11311) && path.GetFirstFragment() && 
            path.GetLastEvent()->GetBadge().num()==EsrKit(11310) );
        auto gis = path.GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
    }

    //    m_chart->updateTime(time_from_iso("20151111T220400Z"));
    CPPUNIT_ASSERT(m_chart->processNewAsoupText(L"(:1042 909/000+11310 2601 1800 659 0906 10 11310 29 01 15 27 01/07 0 0/00 00 0\n"
        L"Ю1 2601 1800 659 1131 13\n"
        L"Ю2 0 00 00 00 00 0000 0 0 054 04862 03654 208 29115870 59233791 000 000 00 052 000.20 001 000 000 90 051 000 000 92 051 000 000\n"
        L"Ю4 00000 1 01 052 000.20 001 000 90 051 000 92 051 000 \n"
        L"Ю4 09050 1 01 001 000.20 001 000 \n"
        L"Ю4 09060 1 01 051 000.90 051 000 92 051 000 \n"
        L"Ю12 00 29115870 1 062 09050 46138 4481 005 04 00 00 00 00 0268 30 20 0211 04 135 09060 09000 00000 01 09050 18 73710 1422 029 0 0000 0      132 000 00000000\n"
        L"Ю12 00 59852442 129233120063 070090604340378465000302409259051001800017769504953020\n"
        L"Ю12 00 58796160 191237127255 06902305904095\n"
        L"Ю12 00 58891037 255239255255 0235\n"
        L"Ю12 00 59845396 255239127255 0238100\n"
        L"Ю12 00 58785577 191239127255 0700231095\n"
        L"Ю12 00 59863464 255237127255 02355905100\n"
        L"Ю12 00 59849034 191239127255 0690229095\n"
        L"Ю12 00 90233180 189237127255 0760202310901106\n"
        L"Ю12 00 53387452 189237255255 0710402235931\n"
        L"Ю12 00 58773771 189237127255 0690302325904095\n"
        L"Ю12 00 59861062 191237127255 07002355905100\n"
        L"Ю12 00 53247516 189237127255 0710402275931106\n"
        L"Ю12 00 58786385 189237127255 0690302315904095\n"
        L"Ю12 00 90229469 189237127255 0760202320901106\n"
        L"Ю12 00 90234717 255255255255 \n"
        L"Ю12 00 58866948 185237127255 0704000302365904095\n"
        L"Ю12 00 58889981 255239255255 0235\n"
        L"Ю12 00 53246930 189237127255 0710402275931106\n"
        L"Ю12 00 59860759 189237127255 0700302355905100\n"
        L"Ю12 00 90211848 189237127255 0760202290901106\n"
        L"Ю12 00 90249087 255255255255 \n"
        L"Ю12 00 90215153 255255255255 \n"
        L"Ю12 00 90219866 255239255255 0228\n"
        L"Ю12 00 59842252 189237127255 0700302395904095\n"
        L"Ю12 00 90283862 189237127255 0710402250901106\n"
        L"Ю12 00 58320706 191237255255 06902275931\n"
        L"Ю12 00 59042853 189237127255 0680202205904095\n"
        L"Ю12 00 90259680 191237127255 07402290901106\n"
        L"Ю12 00 59860650 189237127255 0700302355905100\n"
        L"Ю12 00 59851444 191239255255 0690240\n"
        L"Ю12 00 90110297 253237127255 0402260901106\n"
        L"Ю12 00 58798505 249237127255 3000302315904095\n"
        L"Ю12 00 58320904 253237127255 0402275931106\n"
        L"Ю12 00 58320169 255255255255 \n"
        L"Ю12 00 59817999 249237127255 4000302385904100\n"
        L"Ю12 00 90218835 253237127255 0202290901106\n"
        L"Ю12 00 58777434 253237127255 0302325904095\n"
        L"Ю12 00 90209479 253237127255 0402340901106\n"
        L"Ю12 00 90298548 255255255255 \n"
        L"Ю12 00 90293374 191255255255 070\n"
        L"Ю12 00 53248100 191237255255 06902275931\n"
        L"Ю12 00 59851527 189237127255 0700302405905100\n"
        L"Ю12 00 90246141 189237127255 0690402290901106\n"
        L"Ю12 00 53249207 255237255255 02275931\n"
        L"Ю12 00 59863076 253237127255 0302355905100\n"
        L"Ю12 00 90219619 253237127255 0202290901106\n"
        L"Ю12 00 53240149 189237255255 0700402275931\n"
        L"Ю12 00 90238361 185237255255 0695000202320901\n"
        L"Ю12 00 90213455 255239255255 0229\n"
        L"Ю12 00 59851337 253237127255 0302405905100\n"
        L"Ю12 00 59233791 255237127255 02355904095:)",
        boost::gregorian::date(2020, 1, 9), nullptr, 0
        ));

    {
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }
    set_TimeZone("");
}

void TC_TrackedChart::UncontrolledEdge_5689()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    Uncontrolled_helper();
	m_chart->addBorderControlled( EsrKit(11272) );

    auto realPath = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20200305T071732Z' name='Form' Bdg='1GP[11271:11272]' waynum='1' >"
        L"<rwcoord picketing1_val='223~500' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200305T072513Z' name='Transition' Bdg='1SP[11271]' />"
        L"<SpotEvent create_time='20200305T073040Z' name='Transition' Bdg='1C[11270]' waynum='1' parknum='1' intervalSec='2' />"
        L"<SpotEvent create_time='20200305T073657Z' name='Transition' Bdg='1C[11267]' waynum='1' parknum='2' />"
        L"<SpotEvent create_time='20200305T074741Z' name='Arrival' Bdg='1C[11266]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200305T074942Z' name='Departure' Bdg='1C[11266]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200305T075857Z' name='Arrival' Bdg='1C[11265]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200305T075922Z' name='Departure' Bdg='1C[11265]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200305T081503Z' name='Transition' Bdg='1C[11260]' waynum='1' parknum='1' intervalSec='52' />"
        L"</HemPath>"
        );

    // <AsoupEvent create_time="20181016T154500Z" name="Departure" Bdg="ASOUP 1042[09430]"
    //     index="0001-599-0900" num="4891" reslocoutbnd="Y" waynum="1" parknum="1" dirTo="09000" adjTo="09000"><![CDATA[(:1042 909/000+09430 4891 0001 599 0900 03 09000 16 10 18 45 01/01 0 0/00 00 0
    // Ю3 557 00062030 1 08 00 0000 00000 RODIONOVS   :)]]><Locomotive Series="ЧМЭ3" NumLoc="6203" Consec="1" CarrierCode="4">
    // <Crew EngineDriver="RODIONOVS" Tim_Beg="2018-10-16 08:00" />
    // </Locomotive>
    // </AsoupEvent>

    TrainDescr const td(std::to_wstring(3706), L"0990-050-0900");
    SpotDetailsCPtr spotDetails(new SpotDetails(td));

    const time_t preTime = time_from_iso("20200305T071300Z");
    AsoupEvent preAsoup( HCode::DEPARTURE,  BadgeE( L"ASOUP 1042", EsrKit(11272) ),
        ParkWayKit(1,1), preTime, td, L"",      L"11260" );

    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(preAsoup) ) );
    m_chart->updateTime( preTime, nullptr );

    m_chart->updateTime( time_from_iso("20200305T072000Z"), nullptr );
    m_chart->updateTime( time_from_iso("20200305T075000Z"), nullptr );

    unsigned const trainID = 64;
    CPPUNIT_ASSERT( CreatePath(*m_chart, realPath, trainID) );
    {
        UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
        CPPUNIT_ASSERT( hl.path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size( 9 ) );
        CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "1GP[11271:11272] 1SP[11271] 1C[11270] 1C[11267] 1C[11266] 1C[11266] 1C[11265] 1C[11265] 1C[11260] "));
    }
    
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
        std::make_shared<SpotEvent>(HCode::INFO_CHANGING, BadgeE( L"1C", EsrKit(11260)), time_from_iso("20200305T081155Z"), spotDetails), trainID, TrainCharacteristics::Source::Guess));

    m_chart->updateTime( time_from_iso("20200305T081155Z"), nullptr );
    {
        UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
        CPPUNIT_ASSERT( hl.path_count() == 1 );
        CPPUNIT_ASSERT( hl.exist_path_size( 9 ) );
        CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "ASOUP 1042[11272] 1SP[11271] 1C[11270] 1C[11267] 1C[11266] 1C[11266] 1C[11265] 1C[11265] 1C[11260] "));
    }
    set_TimeZone("");
}

void TC_TrackedChart::DisformReserveLocoByAsoupArrival_5653()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200129T012221Z' name='Form' Bdg='NRP#[11272]' index='0001-984-1131' num='4502' reslocfast='Y'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200129T012221Z' name='Departure' Bdg='NRP#[11272]'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200129T012322Z' name='Span_stopping_begin' Bdg='PRPC[11272:11310]' waynum='1'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200129T012857Z' name='Span_stopping_end' Bdg='PRPC[11272:11310]' waynum='1'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200129T013446Z' name='Arrival' Bdg='9C[11310]' waynum='9' parknum='1' />"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==5 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 4502 );
    }

    std::wstring asoup11310 =L"(:1042 909/000+11310 4502 0001 984 1131 01 11272 29 01 03 27 01/09 1 0/00 00 0:)";


    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11310,
        boost::gregorian::date(2020, 01, 29), nullptr, 0) );
	m_chart->updateTime( time_from_iso("20200129T013446Z"), nullptr );

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );

        attic::a_document doc("TC");
        attic::a_node root = doc.document_element();
        attic::a_node changes = root.append_child("Changes");
        attic::a_node analysis = root.append_child("Analysis");

        CPPUNIT_ASSERT(m_chart->takeChanges(changes, analysis, 0));
        CPPUNIT_ASSERT( analysis.child( "Disform" ).child("Train").attribute("id").as_int()==trainID);

//         CPPUNIT_ASSERT_EQUAL_MESSAGE("Changes existance", shouldBeChanges, pugi::xml_node_type::node_null != changes.first_child().type());
//         CPPUNIT_ASSERT_EQUAL_MESSAGE("Analysis existance", shouldBeAnalysis, pugi::xml_node_type::node_null != analysis.first_child().type());
    }
    set_TimeZone("");
}

void TC_TrackedChart::AttachSameNumberedPathes_5722()
{
    Uncontrolled_helper();
    auto path = 
        L"<HemPath>"
        L"<SpotEvent create_time='20191231T113757Z' name='Form' Bdg='2C[11310]' waynum='2' parknum='1' index='0001-004-1100' num='4511' reslocfast='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='96' Consec='1'>"
        L"<Crew EngineDriver='NAIDJONOVS' Tim_Beg='2019-12-31 09:22' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191231T113757Z' name='Departure' Bdg='2C[11310]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191231T113842Z' name='Span_stopping_begin' Bdg='PRPC[11272:11310]' waynum='1'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191231T114202Z' name='Span_stopping_end' Bdg='PRPC[11272:11310]' waynum='1'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191231T114202Z' name='Station_entry' Bdg='NRP#[11272]'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191231T114302Z' name='Disform' Bdg='NRP#[11272]'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>";

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    hl.createPath(path);
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size( 6 ) );

    auto vSpot = loaderSpot(            
        L"<HemPath>"
        L"<SpotEvent create_time='20191231T114710Z' name='Form' Bdg='N11P_RE1[11272:11273]' index='' num='4511M' reslocfast='Y' waynum='1' id='324' infsrc='disposition'>"
        L"<rwcoord picketing1_val='446~500' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='448~' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"</HemPath>");

        CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(vSpot.front(), 1, TrainCharacteristics::Source::Guess));
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );

}

void TC_TrackedChart::NotCreatePathByAsoupUncontrolledArea()
{
    Uncontrolled_helper();
    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    // исходные данные 
    const TrainDescr td(L"3519", L"0918-052-0933");

    const time_t t_doT( time_from_iso("20180906T044900Z") );
    const time_t t_doD( time_from_iso("20180906T054400Z") );

    AsoupEvent doT( HCode::TRANSITION,   BadgeE( L"dA", EsrKit(7790) ), ParkWayKit(1,3), t_doT, td, L"07050", L"07792" );
    AsoupEvent doD( HCode::DEPARTURE, BadgeE( L"dD", EsrKit(7792) ), ParkWayKit(1,3), t_doD, td,      L"", L"11260" );

    // добавляем все события АСОУП сразу т.к. поочередно их добавлять нет смысла
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(doT) ) );
    m_chart->updateTime( t_doT + 5*60, nullptr );
    CPPUNIT_ASSERT( hl.path_count() == 0 );
    CPPUNIT_ASSERT( PassAsoup( *m_chart, std::make_shared<AsoupEvent>(doD) ) );
    m_chart->updateTime( t_doD + 8*60, nullptr );
    //События не должны склеиться поскольку отсутствует прибытие на [7792]
    CPPUNIT_ASSERT( hl.path_count() == 0 );
}

void TC_TrackedChart::ExpanthFromSpanToStationByArrival()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    Uncontrolled_helper();
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200505T073444Z' name='Form' Bdg='1AC[11310]' waynum='1' parknum='1' index='0001-123-1127' num='4509' reslocfast='Y'>"
        L"<Locomotive Series='2ТЭ10У' NumLoc='222' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='SPILKOV' Tim_Beg='2020-05-05 00:07' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200505T073444Z' name='Departure' Bdg='1AC[11310]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200505T073549Z' name='Span_stopping_begin' Bdg='PRPC[11272:11310]' waynum='1'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200505T073945Z' name='Span_stopping_end' Bdg='PRPC[11272:11310]' waynum='1'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200505T073945Z' name='Station_entry' Bdg='NRP#[11272]'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200505T074041Z' name='Disform' Bdg='NRP#[11272]'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>" 
        );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==6 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 4509 );
    }

    std::wstring asoup11272 = L"(:1042 909/000+11272 4509 0001 123 1127 01 11290 05 05 10 42 02/01 1 0/00 00 0:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11272,
        boost::gregorian::date(2020, 05, 05), nullptr, time_from_iso("20200505T074041Z")) );

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

    m_chart->updateTime( time_from_iso("20200505T080000Z"), nullptr );
    m_chart->updateTime( time_from_iso("20200505T090000Z"), nullptr );

    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==7 );

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
    }
    set_TimeZone("");
}

void TC_TrackedChart::WrongChangeNumber_5597_1()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    Uncontrolled_helper();
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20191116T135100Z' name='Form' Bdg='ASOUP 1042[11050]' waynum='7' parknum='1' index='' num='3561' />"
        L"<SpotEvent create_time='20191116T145703Z' name='Departure' Bdg='12-20SP[11050]' waynum='7' parknum='1' />"
        L"</HemPath>" 
        );

    std::wstring asoup11050_1 = L"(:1042 909/000+11050 9999 1105 921 1100 07 11000 16 11 15 51 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 024 01024 00540 072 95064416 59591693 000 000 00 010 008.90 010 008 000 95 010 000 000\n"
        L"Ю4 16169 2 13 000 008.90 000 008 \n"
        L"Ю4 00000 1 01 010 000.90 010 000 95 010 000 \n"
        L"Ю4 09590 1 01 010 000.90 010 000 95 010 000 \n"
        L"Ю12 00 95064416 1 056 09590 10304 8239 000 04 00 00 00 00 0220 30 95 0950 04 106 11000 09000 00000 01 09630 24 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95128187 191255255255 051\n"
        L"Ю12 00 95038253 191255255255 052\n"
        L"Ю12 00 95063657 191255255255 056\n"
        L"Ю12 00 95574232 255255255255 \n"
        L"Ю12 00 95069712 191255255255 055\n"
        L"Ю12 00 95036125 189255255255 05105\n"
        L"Ю12 00 95548962 189239255255 056040216\n"
        L"Ю12 00 95684817 255239255255 0220\n"
        L"Ю12 00 95048880 191255255255 051\n"
        L"Ю12 00 59612598 129225067131 0001613010307224300500033012905917167161301616913110500261102011051 128\n"
        L"Ю12 00 59611095 255255255255 \n"
        L"Ю12 00 59433870 255255255255 \n"
        L"Ю12 00 59434282 255255255255 \n"
        L"Ю12 00 59433821 255255255255 \n"
        L"Ю12 00 53009304 199255255255 14770103043263\n"
        L"Ю12 00 53009957 255255255255 \n"
        L"Ю12 00 59591693 255255255255 :)";

    std::wstring asoup11050_2 = L"(:1042 909/000+11050 3560 1105 921 1100 07 11000 16 11 15 51 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 024 01024 00540 072 95064416 59591693 000 000 00 010 008.90 010 008 000 95 010 000 000\n"
        L"Ю4 16169 2 13 000 008.90 000 008 \n"
        L"Ю4 00000 1 01 010 000.90 010 000 95 010 000 \n"
        L"Ю4 09590 1 01 010 000.90 010 000 95 010 000 \n"
        L"Ю12 00 95064416 1 056 09590 10304 8239 000 04 00 00 00 00 0220 30 95 0950 04 106 11000 09000 00000 01 09630 24 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95128187 191255255255 051\n"
        L"Ю12 00 95038253 191255255255 052\n"
        L"Ю12 00 95063657 191255255255 056\n"
        L"Ю12 00 95574232 255255255255 \n"
        L"Ю12 00 95069712 191255255255 055\n"
        L"Ю12 00 95036125 189255255255 05105\n"
        L"Ю12 00 95548962 189239255255 056040216\n"
        L"Ю12 00 95684817 255239255255 0220\n"
        L"Ю12 00 95048880 191255255255 051\n"
        L"Ю12 00 59612598 129225067131 0001613010307224300500033012905917167161301616913110500261102011051 128\n"
        L"Ю12 00 59611095 255255255255 \n"
        L"Ю12 00 59433870 255255255255 \n"
        L"Ю12 00 59434282 255255255255 \n"
        L"Ю12 00 59433821 255255255255 \n"
        L"Ю12 00 53009304 199255255255 14770103043263\n"
        L"Ю12 00 53009957 255255255255 \n"
        L"Ю12 00 59591693 255255255255 :)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11050_1,
        boost::gregorian::date(2019, 11, 16), nullptr, 0) );
    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11050_2,
        boost::gregorian::date(2019, 11, 16), nullptr, 0) );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        CPPUNIT_ASSERT( !al.IsServed( pathAsoupList.front() ) );

        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==2 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3561 );
    }

        std::wstring asoup11050_3 =L"(:1042 909/000+11050 3561 1105 921 1100 03 11063 16 11 16 57 01/07 0 0/00 00 0\n"
            L"Ю2 0 00 00 00 00 0000 0 0 024 01024 00540 072 95064416 59591693 000 000 00 010 008.90 010 008 000 95 010 000 000\n"
            L"Ю3 531 00012910 1 08 58 0000 00000 PAVISKA     \n"
            L"Ю4 16169 2 13 000 008.90 000 008 \n"
            L"Ю4 00000 1 01 010 000.90 010 000 95 010 000 \n"
            L"Ю4 09590 1 01 010 000.90 010 000 95 010 000 \n"
            L"Ю12 00 95064416 1 056 09590 10304 8239 000 04 00 00 00 00 0220 30 95 0950 04 106 11000 09000 00000 01 09630 24 00000 0000 025 0 0000 0      132 000 00000000\n"
            L"Ю12 00 95128187 191255255255 051\n"
            L"Ю12 00 95038253 191255255255 052\n"
            L"Ю12 00 95063657 191255255255 056\n"
            L"Ю12 00 95574232 255255255255 \n"
            L"Ю12 00 95069712 191255255255 055\n"
            L"Ю12 00 95036125 189255255255 05105\n"
            L"Ю12 00 95548962 189239255255 056040216\n"
            L"Ю12 00 95684817 255239255255 0220\n"
            L"Ю12 00 95048880 191255255255 051\n"
            L"Ю12 00 59612598 129225067131 0001613010307224300500033012905917167161301616913110500261102011051 128\n"
            L"Ю12 00 59611095 255255255255 \n"
            L"Ю12 00 59433870 255255255255 \n"
            L"Ю12 00 59434282 255255255255 \n"
            L"Ю12 00 59433821 255255255255 \n"
            L"Ю12 00 53009304 199255255255 14770103043263\n"
            L"Ю12 00 53009957 255255255255 \n"
            L"Ю12 00 59591693 255255255255 :)\n";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11050_3,
        boost::gregorian::date(2019, 11, 16), nullptr, 0) );

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
        for ( auto it : pathAsoupList )
        {
            CPPUNIT_ASSERT( !al.IsServed( it ) );
        }
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==2 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3561 );
    }

    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
        std::make_shared<SpotEvent>(HCode::SPAN_MOVE, BadgeE(L"SK10C", EsrKit(11050,11051)), time_from_iso("20191116T145821Z")), trainID, TrainCharacteristics::Source::Guess));
    m_chart->updateTime( time_from_iso("20191116T145821Z"), nullptr );
    m_chart->updateTime( time_from_iso("20191116T150300Z"), nullptr );

    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==3 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3561 );

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
        for ( auto it : pathAsoupList )
            CPPUNIT_ASSERT( al.IsServed( it ) );
    }
        set_TimeZone("");
}

void TC_TrackedChart::WrongChangeNumber_5597_2()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    Uncontrolled_helper();
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20191116T135100Z' name='Form' Bdg='ASOUP 1042[11050]' waynum='7' parknum='1' index='' num='3561' />"
        L"<SpotEvent create_time='20191116T145703Z' name='Departure' Bdg='12-20SP[11050]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20191116T145821Z' name='Span_move' Bdg='SK10C[11050:11051]' waynum='1'>"
        L"<rwcoord picketing1_val='431~744' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='431~287' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>" 
        );

    std::wstring asoup11050_1 = L"(:1042 909/000+11050 3560 1105 921 1100 07 11000 16 11 15 51 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 024 01024 00540 072 95064416 59591693 000 000 00 010 008.90 010 008 000 95 010 000 000\n"
        L"Ю4 16169 2 13 000 008.90 000 008 \n"
        L"Ю4 00000 1 01 010 000.90 010 000 95 010 000 \n"
        L"Ю4 09590 1 01 010 000.90 010 000 95 010 000 \n"
        L"Ю12 00 95064416 1 056 09590 10304 8239 000 04 00 00 00 00 0220 30 95 0950 04 106 11000 09000 00000 01 09630 24 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95128187 191255255255 051\n"
        L"Ю12 00 95038253 191255255255 052\n"
        L"Ю12 00 95063657 191255255255 056\n"
        L"Ю12 00 95574232 255255255255 \n"
        L"Ю12 00 95069712 191255255255 055\n"
        L"Ю12 00 95036125 189255255255 05105\n"
        L"Ю12 00 95548962 189239255255 056040216\n"
        L"Ю12 00 95684817 255239255255 0220\n"
        L"Ю12 00 95048880 191255255255 051\n"
        L"Ю12 00 59612598 129225067131 0001613010307224300500033012905917167161301616913110500261102011051 128\n"
        L"Ю12 00 59611095 255255255255 \n"
        L"Ю12 00 59433870 255255255255 \n"
        L"Ю12 00 59434282 255255255255 \n"
        L"Ю12 00 59433821 255255255255 \n"
        L"Ю12 00 53009304 199255255255 14770103043263\n"
        L"Ю12 00 53009957 255255255255 \n"
        L"Ю12 00 59591693 255255255255 :)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11050_1,
        boost::gregorian::date(2019, 11, 16), nullptr, 0) );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        CPPUNIT_ASSERT( !al.IsServed( pathAsoupList.front() ) );

        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==3 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3561 );
    }

    std::wstring asoup11050_2 =L"(:1042 909/000+11050 3561 1105 921 1100 03 11063 16 11 16 57 01/07 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 024 01024 00540 072 95064416 59591693 000 000 00 010 008.90 010 008 000 95 010 000 000\n"
        L"Ю3 531 00012910 1 08 58 0000 00000 PAVISKA     \n"
        L"Ю4 16169 2 13 000 008.90 000 008 \n"
        L"Ю4 00000 1 01 010 000.90 010 000 95 010 000 \n"
        L"Ю4 09590 1 01 010 000.90 010 000 95 010 000 \n"
        L"Ю12 00 95064416 1 056 09590 10304 8239 000 04 00 00 00 00 0220 30 95 0950 04 106 11000 09000 00000 01 09630 24 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95128187 191255255255 051\n"
        L"Ю12 00 95038253 191255255255 052\n"
        L"Ю12 00 95063657 191255255255 056\n"
        L"Ю12 00 95574232 255255255255 \n"
        L"Ю12 00 95069712 191255255255 055\n"
        L"Ю12 00 95036125 189255255255 05105\n"
        L"Ю12 00 95548962 189239255255 056040216\n"
        L"Ю12 00 95684817 255239255255 0220\n"
        L"Ю12 00 95048880 191255255255 051\n"
        L"Ю12 00 59612598 129225067131 0001613010307224300500033012905917167161301616913110500261102011051 128\n"
        L"Ю12 00 59611095 255255255255 \n"
        L"Ю12 00 59433870 255255255255 \n"
        L"Ю12 00 59434282 255255255255 \n"
        L"Ю12 00 59433821 255255255255 \n"
        L"Ю12 00 53009304 199255255255 14770103043263\n"
        L"Ю12 00 53009957 255255255255 \n"
        L"Ю12 00 59591693 255255255255 :)\n";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11050_2,
        boost::gregorian::date(2019, 11, 16), nullptr, 0) );
	m_chart->updateTime( time_from_iso("20191116T145821"), nullptr );

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
        CPPUNIT_ASSERT( al.IsServed( pathAsoupList.front() ) );
        CPPUNIT_ASSERT( al.IsServed( pathAsoupList.back() ) );
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==3 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
        CPPUNIT_ASSERT( gis.front().second->GetNumber().getNumber() == 3560 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3561 );
    }
    set_TimeZone("");
}

void TC_TrackedChart::TwoAsoupOnTheSamePlace_4960()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    NsiBasis_helper();
    Uncontrolled_helper();
    {
        auto train_4918 = loaderSpot(
            L"<HemPath>"
            L"<SpotEvent create_time='20181108T021831Z' name='Arrival' Bdg='1C[11070]' waynum='1' parknum='1' index='0001-487-1600' num='D4918'/>"
            L"<SpotEvent create_time='20181108T022134Z' name='Departure' Bdg='1C[11070]' waynum='1' parknum='1' optCode='11063:11070' />"
            L"<SpotEvent create_time='20181108T022414Z' name='Transition' Bdg='1SP:1+[11063]' optCode='11062:11063'>"
            L"<rwcoord picketing1_val='400~732' picketing1_comm='Вентспилс' />"
            L"</SpotEvent>"
            L"<SpotEvent create_time='20181108T023603Z' name='Transition' Bdg='1C[11062]' waynum='1' parknum='1' intervalSec='102' optCode='11060:11062' />"
            L"</HemPath>"
            );
        CPPUNIT_ASSERT( CreatePath(*m_chart, train_4918, 1) );
        std::wstring str11001_4918 =     
            L"(:1042 909/000+11001 4918 0001 487 1600 03 11003 08 11 04 00 02/09 0 0/00 00 0\n"
            L"Ю3 572 00033511 1 02 50 1317 00502 ШAPAH       \n"
            L"Ю3 572 00033512 9\n"
            L"Ю3 575 00035541 4 02 50 0000 00000 ЮXHEBИЧ     \n"
            L"Ю3 575 00035542 9\n"
            L"Ю3 572 00029351 4 02 50 0000 00000 ЛУБЧOHOK    \n"
            L"Ю3 572 00029352 9:)";
        CPPUNIT_ASSERT(m_chart->processNewAsoupText(str11001_4918,
            boost::gregorian::date(2018, 11, 8), nullptr, 0 ));

        m_chart->updateTime( time_from_iso("20181108T033637Z"), nullptr );
        auto& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT ( !pathAsoupList.empty() && al.IsServed( pathAsoupList.front() ) );
        auto pathSet = m_chart->getHappenLayer().GetReadAccess()->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path->GetEventsCount()==6 && path->GetFirstEvent()->GetBadge().num()==EsrKit(11001) );
    }

    std::wstring str11001_2713 = L"(:1042 909/000+11001 2713 1664 817 0906 03 11420 08 11 04 00 02/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 055 04946 03632 220 61082368 63264378 000 000 00 055 000.60 055 000 000\n"
        L"Ю3 583 00000101 1 02 30 0000 00000 MARKOVS     \n"
        L"Ю3 583 00000102 9\n"
        L"Ю4 00000 1 01 055 000.60 055 000 \n"
        L"Ю4 09060 1 01 055 000.60 055 000 \n"
        L"Ю12 00 61082368 1 070 09060 16104 9915 000 00 00 00 00 00 0238 30 60 0600 04 100 09060 09000 00000 01 16640 01 25900 6302 020 0 0000 0      148 000 00000000\n"
        L"Ю12 00 63227144 175239254255 06616117024022000\n"
        L"Ю12 00 62992789 255239255255 0239\n"
        L"Ю12 00 53495586 191237254255 0670235560063000\n"
        L"Ю12 00 52487576 255239255255 0232\n"
        L"Ю12 00 63502470 187237254123 06530002450600862403126132\n"
        L"Ю12 00 68095884 191237255255 06702250608\n"
        L"Ю12 00 68139757 255255255255 \n"
        L"Ю12 00 68321652 255255255255 \n"
        L"Ю12 00 62869235 191237255255 06502450600\n"
        L"Ю12 00 62006085 255239255255 0246\n"
        L"Ю12 00 63185888 255239255255 0245\n"
        L"Ю12 00 60309424 255255255255 \n"
        L"Ю12 00 60513116 255255255255 \n"
        L"Ю12 00 63482285 255255255255 \n"
        L"Ю12 00 68178920 191237255255 06702250608\n"
        L"Ю12 00 63467880 191237255255 06502450600\n"
        L"Ю12 00 68142884 191237255255 06702250608\n"
        L"Ю12 00 68034230 191239255255 0680224\n"
        L"Ю12 00 63214571 187237255255 06540002450600\n"
        L"Ю12 00 63214464 255255255255 \n"
        L"Ю12 00 61398426 255255255255 \n"
        L"Ю12 00 63094015 255255255255 \n"
        L"Ю12 00 63088629 255255255255 \n"
        L"Ю12 00 63450753 255255255255 \n"
        L"Ю12 00 62620752 255255255255 \n"
        L"Ю12 00 63248108 255255255255 \n"
        L"Ю12 00 63269401 255255255255 \n"
        L"Ю12 00 63100366 255255255255 \n"
        L"Ю12 00 68321736 191237255255 06702250608\n"
        L"Ю12 00 60749918 191237255255 06502450600\n"
        L"Ю12 00 62768023 255255255255 \n"
        L"Ю12 00 63154942 255255255255 \n"
        L"Ю12 00 63125645 255255255255 \n"
        L"Ю12 00 59904979 251237255255 30002415600\n"
        L"Ю12 00 55937288 191239255255 0660235\n"
        L"Ю12 00 57953333 255239255255 0236\n"
        L"Ю12 00 60374816 255237255255 02370600\n"
        L"Ю12 00 60382371 255255255255 \n"
        L"Ю12 00 61082152 255255255255 \n"
        L"Ю12 00 53104295 255237255255 02355600\n"
        L"Ю12 00 58483868 255239255255 0236\n"
        L"Ю12 00 54134325 255239255255 0237\n"
        L"Ю12 00 52217221 255239255255 0235\n"
        L"Ю12 00 55116644 255239255255 0237\n"
        L"Ю12 00 59905471 255239255255 0235\n"
        L"Ю12 00 55627939 255255255255 \n"
        L"Ю12 00 55022685 191239255255 0650242\n"
        L"Ю12 00 61370409 191237255255 06602360600\n"
        L"Ю12 00 56158256 171237254255 068161184000238560086560\n"
        L"Ю12 00 55806632 255239255255 0240\n"
        L"Ю12 00 55554729 255239255255 0236\n"
        L"Ю12 00 55576656 255239255255 0240\n"
        L"Ю12 00 63349021 255253255255 0600\n"
        L"Ю12 00 63264378 255255255255 :)";

    CPPUNIT_ASSERT( m_chart->processNewAsoupText(str11001_2713,
        boost::gregorian::date(2018, 11, 8), nullptr, 0 ));

    auto train_2713 = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20181108T021322Z' name='Form' Bdg='P3P[11002:11451]' waynum='1' num='2713' >"
        L"<rwcoord picketing1_val='389~476' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='388~658' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20181108T021537Z' name='Transition' Bdg='4/6SP:4+[11451]'>"
        L"<rwcoord picketing1_val='387~399' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20181108T022333Z' name='Transition' Bdg='5SP[11445]'>"
        L"<rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20181108T023628Z' name='Arrival' Bdg='3BC[11443]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20181108T025032Z' name='Departure' Bdg='3AC[11443]' waynum='3' parknum='1' optCode='11442:11443' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( CreatePath(*m_chart, train_2713, 2) );

    HappenLayer& hl = m_chart->getHappenLayer();
    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 2 );
        for ( auto path : pathSet )
        {
            CPPUNIT_ASSERT( path->GetFirstFragment() );
            if ( path->GetFirstFragment()->GetNumber().getNumber()==4918 ) 
                CPPUNIT_ASSERT( path->GetEventsCount()==6 && path->GetFirstEvent()->GetBadge().num()==EsrKit(11001) );
            else if ( path->GetFirstFragment()->GetNumber().getNumber()==2713 ) 
                CPPUNIT_ASSERT( path->GetEventsCount()==5 && path->GetFirstEvent()->GetBadge().num()==EsrKit(11002,11451) );
            else 
                ASSERT("ошибка создания нитей");
        }

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        for ( auto it : pathAsoupList )
        {
            bool bServed = it->GetNumber()==4918;
            CPPUNIT_ASSERT( al.IsServed( it )==bServed );
        }
    }

    m_chart->updateTime( time_from_iso("20181108T050000Z"), nullptr );
    {   //проверим продление нити 2713 до сообщения АСОУП.
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 2 );
        for ( auto path : pathSet )
        {
            CPPUNIT_ASSERT( path->GetFirstFragment() );
            if ( path->GetFirstFragment()->GetNumber().getNumber()==4918 ) 
                CPPUNIT_ASSERT( path->GetEventsCount()==6 && path->GetFirstEvent()->GetBadge().num()==EsrKit(11001) );
            else if ( path->GetFirstFragment()->GetNumber().getNumber()==2713 ) 
                CPPUNIT_ASSERT( path->GetEventsCount()==6 && path->GetFirstEvent()->GetBadge().num()==EsrKit(11001) );
        }
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT ( pathAsoupList.size()==2 && al.IsServed( pathAsoupList.front() ) && al.IsServed( pathAsoupList.back() ) );
    }

    std::wstring messageWithdrawal = L"(:333 1042 909/000+11001 2713 1664 817 0906 03 11420 08 11 04 00 02/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 055 04946 03632 220 61082368 63264378 000 000 00 055 000.60 055 000 000\n:)";

    CPPUNIT_ASSERT( m_chart->processNewAsoupText(messageWithdrawal,
        boost::gregorian::date(2018, 11, 8), nullptr, 0 ));
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT ( pathAsoupList.size()==1 && pathAsoupList.front()->GetNumber()==4918 );


    set_TimeZone("");
}

void TC_TrackedChart::AttachToDisformOnUcontrolledStation()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    NsiBasis_helper();
    Uncontrolled_helper();

    auto train_2405 = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20200515T135049Z' name='Form' Bdg='LS6C[09801:09802]' waynum='1' index='5851-196-0983' num='V2405S' length='57' weight='5457' net_weight='4076' through='Y'>"
        L"<rwcoord picketing1_val='50~538' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='51~240' picketing1_comm='Вентспилс' />"
        L"<feat_texts typeinfo='Т' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200515T135537Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' intervalSec='2' optCode='09802:09803' />"
        L"<SpotEvent create_time='20200515T140529Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' intervalSec='2' optCode='09803:09810' />"
        L"<SpotEvent create_time='20200515T141828Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' intervalSec='10' optCode='09810:09812' />"
        L"<SpotEvent create_time='20200515T143721Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' intervalSec='2' optCode='09812:09813' />"
        L"<SpotEvent create_time='20200515T144534Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' intervalSec='17' optCode='09813:09820' />"
        L"<SpotEvent create_time='20200515T145200Z' name='Arrival' Bdg='ASOUP 1042[09820]' waynum='2' parknum='4' />"
        L"<SpotEvent create_time='20200515T145200Z' name='Death' Bdg='ASOUP 1042[09820]' waynum='2' parknum='4' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( CreatePath(*m_chart, train_2405, 1) );

    std::wstring strDisfom =     
        L"(:1042 909/000+09830 2405 5851 196 0983 05 00000 15 05 18 56 04/02 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05457 04076 236 95889911 58869728 000 000 00 059 000.90 059 000 000 92 055 000 000 95 004 000 000\n"
        L"Ю4 00000 1 01 059 000.90 059 000 92 055 000 95 004 000 \n"
        L"Ю4 09830 1 01 059 000.90 059 000 92 055 000 95 004 000 \n"
        L"Ю12 00 95889911 1 059 09830 54210 1379 500 02 00 00 00 00 0232 30 95 0950 04 106 09830 09820 00000 01 58510 38 62510 5739 020 0 0000 0      148 000 00000000\n"
        L"Ю12 00 95341202 191239255255 0540234\n"
        L"Ю12 00 95231676 191239255255 0590231\n"
        L"Ю12 00 95196960 187239254127 0600000233596708287\n"
        L"Ю12 00 58884198 161233124113 07043612210220001023592590409517585101419585105132032\n"
        L"Ю12 00 58871526 255255255253 000\n"
        L"Ю12 00 58870890 255239255255 0234\n"
        L"Ю12 00 59571117 191239255255 0640220\n"
        L"Ю12 00 58871021 191239255255 0700234\n"
        L"Ю12 00 58870452 255255255255 \n"
        L"Ю12 00 58878935 255239255255 0235\n"
        L"Ю12 00 58878612 255239255255 0234\n"
        L"Ю12 00 58871302 255255255255 \n"
        L"Ю12 00 58884966 255239255255 0235\n"
        L"Ю12 00 58884768 255255255255 \n"
        L"Ю12 00 58878810 255239255255 0234\n"
        L"Ю12 00 58874355 255255255255 \n"
        L"Ю12 00 58874314 255255255255 \n"
        L"Ю12 00 58884875 255239255255 0235\n"
        L"Ю12 00 58874256 255239255255 0234\n"
        L"Ю12 00 58884529 255239255255 0235\n"
        L"Ю12 00 58870189 255239255255 0234\n"
        L"Ю12 00 58884784 255255255255 \n"
        L"Ю12 00 58871393 255239255255 0235\n"
        L"Ю12 00 58884487 255255255255 \n"
        L"Ю12 00 58870445 255239255255 0234\n"
        L"Ю12 00 58871641 255239255255 0235\n"
        L"Ю12 00 59037192 255239255255 0230\n"
        L"Ю12 00 58869736 255239255255 0234\n"
        L"Ю12 00 58871039 255255255255 \n"
        L"Ю12 00 58870916 255239255255 0235\n"
        L"Ю12 00 58871666 255239255255 0234\n"
        L"Ю12 00 58884750 255239255255 0235\n"
        L"Ю12 00 58878638 255239255255 0234\n"
        L"Ю12 00 58871500 255239255255 0235\n"
        L"Ю12 00 58870817 255239255255 0234\n"
        L"Ю12 00 58871419 255255255255 \n"
        L"Ю12 00 58884685 255239255255 0235\n"
        L"Ю12 00 58884818 255255255255 \n"
        L"Ю12 00 58874272 255239255255 0234\n"
        L"Ю12 00 58870965 255255255255 \n"
        L"Ю12 00 58874165 255255255255 \n"
        L"Ю12 00 58874173 255239255255 0235\n"
        L"Ю12 00 58869926 255255255255 \n"
        L"Ю12 00 59036699 255239255255 0229\n"
        L"Ю12 00 58884271 255239255255 0234\n"
        L"Ю12 00 58870031 255239255255 0235\n"
        L"Ю12 00 58869892 255255255255 \n"
        L"Ю12 00 58884297 255255255255 \n"
        L"Ю12 00 58885005 255255255255 \n"
        L"Ю12 00 58879081 255239255255 0234\n"
        L"Ю12 00 58869785 255255255255 \n"
        L"Ю12 00 58884156 255255255255 \n"
        L"Ю12 00 58884909 255255255255 \n"
        L"Ю12 00 58870338 255239255255 0235\n"
        L"Ю12 00 58884388 255255255255 \n"
        L"Ю12 00 58869710 255239255255 0234\n"
        L"Ю12 00 58874421 255239254119 0235000000000ПPЧ05 \n"
        L"Ю12 00 58869728 255239255255 0234:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(strDisfom,
        boost::gregorian::date(2020, 5, 15), nullptr, 0 ));

    m_chart->updateTime( time_from_iso("20200515T160000Z") + 30, nullptr );
    
    auto& al = m_chart->getAsoupLayer();
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT ( !pathAsoupList.empty() && al.IsServed( pathAsoupList.front() ) );
    auto pathSet = m_chart->getHappenLayer().GetReadAccess()->getAllPathes();
    CPPUNIT_ASSERT( pathSet.size() == 1 );
    auto path = *pathSet.cbegin();
    CPPUNIT_ASSERT( path->GetEventsCount()==8 && path->GetLastEvent()->GetCode()==HCode::DISFORM );

    set_TimeZone("");
}

void TC_TrackedChart::AsoupGlueFromSpanToStation()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    NsiBasis_helper();
    Uncontrolled_helper();

    auto train = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20200609T103921Z' name='Form' Bdg='2C[9802]' waynum='2' parknum='1' index='0001-192-1100' num='8221' stone='Y'>"
        L"<Locomotive Series='ДГКу' NumLoc='2995' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='ZURAVLJOVS' Tim_Beg='2020-06-09 06:15' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200609T103921Z' name='Departure' Bdg='2C[9802]' waynum='2' parknum='1' optCode='11340:11342' />"
        L"<SpotEvent create_time='20200609T105023Z' name='Transition' Bdg='1C[9803]' waynum='1' parknum='1' intervalSec='61' optCode='11342:11343' index='0001-192-1100' num='8221' stone='Y'>"
        L"<Locomotive Series='ДГКу' NumLoc='2995' Consec='1'>"
        L"<Crew EngineDriver='ZURAVLJOVS' Tim_Beg='2020-06-09 06:15' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200609T105917Z' name='Transition' Bdg='1C[9810]' waynum='1' parknum='1' intervalSec='65' optCode='11343:11360' />"
        L"<SpotEvent create_time='20200609T111508Z' name='Transition' Bdg='1C[9812]' waynum='1' parknum='1' optCode='11360:11363' />"
        L"<SpotEvent create_time='20200609T112404Z' name='Transition' Bdg='7-9SP:9+[9813]'>"
        L"<rwcoord picketing1_val='523~947' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200609T112720Z' name='Span_move' Bdg='P8P[9813:9820]' waynum='1'>"
        L"<rwcoord picketing1_val='527~900' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='527~' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200609T112831Z' name='Death' Bdg='P8P[9813:9820]' waynum='1'>"
        L"<rwcoord picketing1_val='527~900' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='527~' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( CreatePath(*m_chart, train, 1) );
    auto pathSet = m_chart->getHappenLayer().GetReadAccess()->getAllPathes();
    auto path = *pathSet.cbegin();
    CPPUNIT_ASSERT( path->GetEventsCount()==8 && path->GetLastEvent()->GetBadge().num()==EsrKit(9813, 9820) );

    std::wstring strArrival = L"(:1042 909/000+09820 8221 0001 192 1100 01 09813 09 06 14 28 03/02 1 0/00 00 0:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(strArrival,
        boost::gregorian::date(2020, 6, 9), nullptr, 0 ));

    m_chart->updateTime( time_from_iso("20200609T120000Z"), nullptr );

    auto& al = m_chart->getAsoupLayer();
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT ( !pathAsoupList.empty() && al.IsServed( pathAsoupList.front() ) );
    pathSet = m_chart->getHappenLayer().GetReadAccess()->getAllPathes();
    CPPUNIT_ASSERT( pathSet.size() == 1 );
    path = *pathSet.cbegin();
    CPPUNIT_ASSERT( path->GetEventsCount()==9 && path->GetLastEvent()->GetBadge().num()==EsrKit(9820) );
    set_TimeZone("");
}

void TC_TrackedChart::setupTopology()
{
    std::shared_ptr<TopologyTest> tt( new TopologyTest );
    tt->Load(
        //C.p. Brasa,Skulte
        L"*09042,09070 {09042,09650,09640,09630,09616,09612,09609,09603,09604,09600,09070} \n"
        //Jelgava-Krustpils
        L"*09180,11420 {09180,09181,09251,09241,09240,09230,09220,09211,09210,09202,09200,09193,09191,09190,11420}\n"
        L"//_9.Zemitani-Skirotava J-Riga Precu                                             \n"
        L"=09500,09006,09430                                                               \n"
        L"//Gluda-Liepaja pas                                                              \n"
        L"*09180,09860 {09180,09280,09290,09303,09320,09330,09340,09350,09351,09355,09860} \n"
        L"//Zasulauks-Bolderaja2-Krievusala                                                \n"
        L"=09670,09710,09990                                                               \n"
        L"//Zasulauks,Bolderaja                                                            \n"
        L"*09670,09710 {09670,09680,09710}                                                 \n"
        L"//41.Plavinas-Krustpils-c.p. 383                                                 \n"
        L"=11760,11420,11445                                                               \n"
        L"//Krustpils,c.p. 383                                                             \n"
        L"*11420,11445 {11420,11419,11422,11430,11431,11432,11446,11442,11443,11445}       \n"
        L"//43.Jelgava-Krustpils-c.p. 383                                                  \n"
        L"=09180,11420,11445                                                               \n"
        L"//46.Krustpils-c.p. 383 km-c.p. 387 km (1)                                       \n"
        L"=11420,11445,11451                                                               \n"
        L"//48.c.p. 1 km-c.p. 383 km-c.p. 387 km (2)                                       \n"
        L"=11771,11445,11451                                                               \n"
        L"*11771,11445 {11771,11773,11445}                                                 \n"
        L"//50.Daugavpils.Sk-c.p. 387 km-c.p. 383 km (3)                                   \n"
        L"=11010,11451,11445                                                               \n"
        L"//53.c.p. 3 km-Daugavpils Para-Daugavpils Nepara (23)                            \n"
        L"=11071,11002,11003                                                               \n"
        L"//Daugavpils Para-Daugavpils Nepara                                              \n"
        L"*11002,11003 {11002,11000,11003}                                                 \n"
        L"//c.p. 401 km-Daugavpils Nepara-Daugavpils D (23)                                \n"
        L"=11063,11003,11001                                                               \n"
        L"//Daugavpils Nepara-c.p. 401 km                                                  \n"
        L"*11003,11063 {11003,11070,11063}                                                 \n"
        L"//54.Daugavpils Para-Daugavpils Nepara-c.p. 401 km (10)                          \n"
        L"=11002,11003,11063                                                               \n"
        L"//53.c.p. 383 km-c.p. 387 km-Daugavpils Para (4)                                 \n"
        L"=11445,11451,11002                                                               \n"
        L"//Daugavpils D-Daugavpils Para-c.p. 387 km (8)                                   \n"
        L"=11001,11002,11451                                                               \n"
        L"//53.c.p. 5 km-c.p. 3 km-Daugavpils Para (24)                                    \n"
        L"=11076,11071,11002                                                               \n"
        L"//56.c.p. 524 km-Daugavpils sk-c.p. 387 km (7)                                   \n"
        L"=11363,11010,11451                                                               \n"
        L"//c.p. 524 km-Daugavpils sk-Daugavpils Para (20)                                 \n"
        L"=11363,11010,11002                                                               \n"
        L"//c.p. 524 km-Daugavpils sk-c.p. 3 km (21)                                       \n"
        L"=11363,11010,11071                                                               \n"
        L"//Daugavpils sk-Daugavpils Para-Daugavpils Nepara (22)                           \n"
        L"=11010,11002,11003                                                               \n"
        L"//Daugavpils D-Daugavpils Para-c.p. 3 km (28)                                    \n"
        L"=11001,11002,11071                                                               \n"
        L"//c.p. 387 km-Daugavpils Para-Daugavpils Nepara (9)                              \n"
        L"=11451,11002,11003                                                               \n"
        L"//58.Daugavpils Nepara-c.p. 401 km-Polocka (25)                                  \n"
        L"=11003,11063,16130                                                               \n"
        L"//59.Daugavpils sk-c.p. 524 km-c.p. 401 km (11)                                  \n"
        L"=11010,11363,11063                                                               \n"
        L"//66.Daugavpils sk-Daugavpils Para-Daugavpils D (13)                             \n"
        L"=11010,11002,11001                                                               \n"
        L"//69.Daugavpils Para-c.p. 3 km-Griva (26)                                        \n"
        L"=11002,11071,11240                                                               \n"
        L"//80.Daugavpils Para-Daugavpils D-выход за пределы топологии                     \n"
        L"=11002,11001,00000                                                               \n"
        L"//80.Daugavpils Nepara-Daugavpils D-выход за пределы топологии                   \n"
        L"=11003,11001,00000                                                               \n"
        L"//60.c.p. 383 km-c.p. 1 km-c.p. 192 km (5)                                       \n"
        L"=11445,11771,11075                                                               \n"
        L"//.c.p. 383 km-c.p. 1 km-c.p. 191 km (6)                                         \n"
        L"=11445,11771,11074                                                               \n"
        L"//Kleperova-Pitalova                                                             \n"
        L"*11271,07790 {11271,11270,11267,11266,11265,11260,07792,07790}                   \n"
        L"// Riga - Plavinas                                                               \n"
        L"*09010,11760 {09010,09006,09000,09420,09410,09400,09380,09370,09360,11760}       \n"
        L"//Сабиле-Ventspils                                                               \n"
        L"*09790,09820 {09790,09800,09801,09802,09803,09810,09812,09813,09820}             \n"
        L"//Olaine-Jelgava                                                                 \n"
        L"*09150,09180 {09150,09160,09162,09180}                                           \n"
        L"//44.Sebeza-Rezekne 2-Rezekne 1 (3)                                              \n"
        L"=06610,11310,11272                                                               \n"
        L"//51.Rezekne 2-Rezekne 1-c.p. 524 km (17)                                        \n"
        L"=11310,11272,11363                                                               \n"
        L"//52.Kleperova-Rezekne 1-c.p. 524 km (18)                                        \n"
        L"=11271,11272,11363                                                               \n"
        L"//Sebeza-Rezekne 2                                                               \n"
        L"*06610,11310 {06610,11290,11291,11292,11300,11301,11302,11320,11310}             \n"
        L"//c.p. 401 km-Polocka                                                            \n"
        L"*11063,16130 {11063,11062,11060,11051,11050,11052,11041,11042,16169,16170,16180,16181,16190,16200,16210,16220,16130} \n"
		L"=09813,09840,09820															   \n"
        L"//Карта замен. *-строгое соответствие (станции экспорт)                          \n"
        L"//Ventspils                                                                      \n"
        L"$09820=*09830                                                                    \n"
        L"//Замена станций где оператор вводит 4 цифры                                     \n"
        L"$09990=*09900                                                                    \n"
        L"//Indra                                                                          \n"
        L"$11042=*11020                                                                    \n"
        );

    m_chart->setupTopology( tt );
}
void TC_TrackedChart::setupEsrGuide()
{
    {
        attic::a_document doc;
        EsrGuide eg;
        doc.load_wide(
            L"<EsrGuide>"
            L"<EsrGuide kit='09010' name='Riga-Pasagieru' picketing1_val='0~1' picketing1_comm='Рига' picketing2_val='0~1' picketing2_comm='Рига-Земитани' picketing3_val='0~1' picketing3_comm='Рига-Зилупе' picketing4_val='0~1' picketing4_comm='Шкиротава-Я'>"
            L"<Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
            L"<Way waynum='2' parknum='1' major='Y' />"
            L"<Way waynum='3' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
            L"<Way waynum='4' parknum='1' passenger='Y' embarkation='Y' />"
            L"<Way waynum='5' parknum='1' passenger='Y' embarkation='Y' />"
            L"<Way waynum='6' parknum='1' passenger='Y' embarkation='Y' />"
            L"<Way waynum='7' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
            L"<Way waynum='8' parknum='1' passenger='Y' embarkation='Y' />"
            L"<Way waynum='9' parknum='1' major='Y' />"
            L"<Way waynum='10' parknum='1' />"
            L"<Way waynum='11' parknum='1' />"
            L"<Way waynum='12' parknum='1' />"
            L"</EsrGuide>"
            L"</EsrGuide>");
        eg.load_from( doc.document_element() );
        m_chart->setupEsrGuide(eg);
    }
}

void TC_TrackedChart::IdentifyWithAsoupArrival()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();

    const std::string layerData = "<export>\n"
        "<ScheduledLayer timePeriod='[20200616T030000Z/20200618T025959Z)'>\n"
        "  <ScheduledPath>\n"
        "<SpotEvent create_time='20200616T183700Z' name='Info_changing' Bdg='№6743[09010]' index='' num='6743' suburbreg='Y' />\n"
        "<SpotEvent create_time='20200616T183700Z' name='Departure' Bdg='№6743[09010]' waynum='1' parknum='1' />\n"
        "<SpotEvent create_time='20200616T184100Z' name='Arrival' Bdg='№6743[09100]' waynum='1' parknum='1' />\n"
        "<SpotEvent create_time='20200616T184200Z' name='Departure' Bdg='№6743[09100]' waynum='1' parknum='1' />\n"
        "<SpotEvent create_time='20200616T184710Z' name='Transition' Bdg='№6743[09104]' parknum='1' />\n"
        "<SpotEvent create_time='20200616T190030Z' name='Arrival' Bdg='№6743[09150]' waynum='5' parknum='1' />\n"
        "<SpotEvent create_time='20200616T190130Z' name='Departure' Bdg='№6743[09150]' waynum='5' parknum='1' />\n"
        "<SpotEvent create_time='20200616T191030Z' name='Arrival' Bdg='№6743[09160]' waynum='3' parknum='1' />\n"
        "<SpotEvent create_time='20200616T191100Z' name='Departure' Bdg='№6743[09160]' waynum='3' parknum='1' />\n"
        "<SpotEvent create_time='20200616T191730Z' name='Arrival' Bdg='№6743[09162]' waynum='1' parknum='1' />\n"
        "<SpotEvent create_time='20200616T191830Z' name='Departure' Bdg='№6743[09162]' waynum='1' parknum='1' />\n"
        "<SpotEvent create_time='20200616T192200Z' name='Arrival' Bdg='№6743[09180]' waynum='1' parknum='1' />\n"
        "  </ScheduledPath>\n"
        "</ScheduledLayer>\n"
        "</export>\n";
    m_chart->deserializeRegulatoryLayer(layerData);

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200616T180903Z' name='Form' Bdg='1BP[09042:09650]' waynum='1' index='' num='6743' >"
        L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"<rwcoord picketing1_val='6~961' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T180908Z' name='Transition' Bdg='209SP+[09042]' optCode='09042:09500'>"
        L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T181345Z' name='Transition' Bdg='1C[09500]' waynum='1' parknum='1' intervalSec='116' />"
        L"<SpotEvent create_time='20200616T181837Z' name='Arrival' Bdg='7C[09010]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20200616T183754Z' name='Departure' Bdg='7C[09010]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20200616T184106Z' name='Arrival' Bdg='2p[09100]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200616T184300Z' name='Departure' Bdg='2p[09100]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200616T185119Z' name='Transition' Bdg='KPU13A/6A[09104]'>"
        L"<rwcoord picketing1_val='5~750' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='8~800' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T190143Z' name='Arrival' Bdg='4p[09150]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20200616T190353Z' name='Departure' Bdg='4Ap[09150]' waynum='4' parknum='1' optCode='09150:09160' />"
        L"<SpotEvent create_time='20200616T191140Z' name='Arrival' Bdg='2p[09160]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200616T191309Z' name='Departure' Bdg='2p[09160]' waynum='2' parknum='1' optCode='09160:09162' />"
        L"<SpotEvent create_time='20200616T191928Z' name='Station_entry' Bdg='AGP[09162]' waynum='1' parknum='1'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T192140Z' name='Departure' Bdg='AGP[09162]' waynum='1' parknum='1'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T192344Z' name='Arrival' Bdg='32C[09180]' waynum='32' parknum='1' />"
        L"<SpotEvent create_time='20200616T192344Z' name='Disform' Bdg='32C[09180]' waynum='32' parknum='1' />"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==16 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 6743 );
    }
    std::wstring asoup9010 = L"<TrainAssignment operation='5' date='16-06-2020 21:37' esr='09010' location='24.121025,56.946324' number='6743' issue_moment='20200616T183803Z'>"
        L"<Unit number='7118-09--ER2T' driver='Sulims' driverTime='16-06-2020 21:37' weight='242' length='6' specialConditions='M' carrier='24'>"
        L"<Wagon number='7118-09' axisCount='4' weight='51' length='1.41' type='1' owner='25' />"
        L"<Wagon number='7115-06' axisCount='4' weight='70' length='1.41' type='2' owner='25' />"
        L"<Wagon number='7118-10' axisCount='4' weight='70' length='1.41' type='2' owner='25' />"
        L"<Wagon number='7118-01' axisCount='4' weight='51' length='1.41' type='1' owner='25' />"
        L"</Unit>"
        L"</TrainAssignment>";

    CPPUNIT_ASSERT(m_chart->processNewPassengerLdzWstr(asoup9010, nullptr, 0));

    std::wstring asoup9180 = L"<TrainAssignment operation='6' date='16-06-2020 22:24' esr='09180' location='23.734509,56.641546' number='6743' issue_moment='20200616T192502Z'>"
        L"<Unit number='7118-09--ER2T' driver='Sulims' driverTime='16-06-2020 21:37' weight='242' length='6' specialConditions='M' carrier='24'>"
        L"<Wagon number='7118-09' axisCount='4' weight='51' length='1.41' type='1' owner='25' />"
        L"<Wagon number='7115-06' axisCount='4' weight='70' length='1.41' type='2' owner='25' />"
        L"<Wagon number='7118-10' axisCount='4' weight='70' length='1.41' type='2' owner='25' />"
        L"<Wagon number='7118-01' axisCount='4' weight='51' length='1.41' type='1' owner='25' />"
        L"</Unit>"
        L"</TrainAssignment>";

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

    CPPUNIT_ASSERT(m_chart->processNewPassengerLdzWstr(asoup9180, nullptr, 0));
	m_chart->updateTime( time_from_iso("20200616T192300Z"), nullptr);
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 2 );  //разрыв нити по нормативке
        
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
        for ( auto asoup  : pathAsoupList )
        {
            CPPUNIT_ASSERT( al.IsServed( asoup ) );
        }
    }

    set_TimeZone("");
}

void TC_TrackedChart::SecondCorrectAsoupOnUncontrolledStation()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    Uncontrolled_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200702T042400Z' name='ExplicitForm' Bdg='ASOUP 1042[09860]' waynum='1' parknum='1' index='0986-083-0935' num='8202' length='4' weight='94' stone='Y'>"
        L"<Locomotive Series='ЧМЭ3' NumLoc='4720' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='RAKOVS' Tim_Beg='2020-07-02 06:42' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200702T045122Z' name='Departure' Bdg='1C[09860]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200702T045606Z' name='Transition' Bdg='51C[09860]' waynum='51' parknum='2' intervalSec='112' />"
        L"<SpotEvent create_time='20200702T045842Z' name='Span_move' Bdg='LT-1C[09355:09860]' waynum='1'>"
        L"<rwcoord picketing1_val='207~700' picketing1_comm='Лиепая' />"
        L"<rwcoord picketing1_val='228~500' picketing1_comm='Лиепая' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200702T051555Z' name='Death' Bdg='LT-1C[09355:09860]' waynum='1'>"
        L"<rwcoord picketing1_val='207~700' picketing1_comm='Лиепая' />"
        L"<rwcoord picketing1_val='228~500' picketing1_comm='Лиепая' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==5 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 8202 );
    }
    const std::wstring arrival9355 = L"(:1042 909/000+09355 8202 0986 083 0935 01 09860 02 07 08 17 01/01 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 004 00094 00000 012 19185404 19151265 003 000 00 000 000\n"
        L"Ю12 00 19185404 1 000 09350 42100 7755 920 00 00 00 00 00 0240 50 10 0580 04 106 09350 00000 00000 01 09860 43 00000 0000 000 0 0000 ПEPECЛ 128 000 00000000\n"
        L"Ю12 00 19151273 251237127247 1000350055610594    \n"
        L"Ю12 00 19151265 255255255255 :)";
    const std::wstring departure9355_1 = L"(:1042 909/000+09355 8202 0986 083 0935 03 09180 02 07 08 21 01/01 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 004 00094 00000 012 19185404 19151265 003 000 00 000 000\n"
        L"Ю12 00 19185404 1 000 09350 42100 7755 920 00 00 00 00 00 0240 50 10 0580 04 106 09350 00000 00000 01 09860 43 00000 0000 000 0 0000 ПEPECЛ 128 000 00000000\n"
        L"Ю12 00 19151273 251237127247 1000350055610594    \n"
        L"Ю12 00 19151265 255255255255 :)";

        const std::wstring departure9355_2 = L"(:1042 909/000+09355 8202 0986 083 0935 03 09180 02 07 08 21 01/01 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 004 00094 00000 012 19185404 19151265 003 000 00 000 000\n"
        L"Ю3 557 00047200 1 06 42 0000 00000 RAKOVS      \n"
        L"Ю12 00 19185404 1 000 09350 42100 7755 920 00 00 00 00 00 0240 50 10 0580 04 106 09350 00000 00000 01 09860 43 00000 0000 000 0 0000 ПEPECЛ 128 000 00000000\n"
        L"Ю12 00 19151273 251237127247 1000350055610594    \n"
        L"Ю12 00 19151265 255255255255 :)";

        CPPUNIT_ASSERT(m_chart->processNewAsoupText( arrival9355, boost::gregorian::date(2020, 07, 02), nullptr, 0) );
        CPPUNIT_ASSERT(m_chart->processNewAsoupText( departure9355_1, boost::gregorian::date(2020, 07, 02), nullptr, 0) );

        m_chart->updateTime( time_from_iso("20200702T052100Z"), nullptr );

        AsoupLayer& al = m_chart->getAsoupLayer();

        {
            HappenLayer::ReadAccessor container = hl.GetReadAccess();
            auto pathSet = container->getAllPathes();
            CPPUNIT_ASSERT( pathSet.size() == 1 );
            auto pathCreate = *pathSet.cbegin();
            CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==7 );
            auto pLastEvent = pathCreate->GetLastAliveEvent();
            CPPUNIT_ASSERT( pLastEvent && pLastEvent->GetBadge().num()==EsrKit(9355) && 
                pLastEvent->GetTime()==time_from_iso("20200702T052100Z") );

            auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
            CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
            auto asoup = pathAsoupList.front();
            CPPUNIT_ASSERT( al.IsServed( asoup ) );
            asoup = pathAsoupList.back();
            CPPUNIT_ASSERT( al.IsServed( asoup ) );
        }

        CPPUNIT_ASSERT(m_chart->processNewAsoupText( departure9355_2, boost::gregorian::date(2020, 07, 02), nullptr, time_from_iso("20200702T052100Z")) );
        m_chart->updateTime( time_from_iso("20200702T053000Z"), nullptr );

        {
            auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
            CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
            for ( auto asoup : pathAsoupList )
            {
                CPPUNIT_ASSERT( al.IsServed( asoup ) );
                if ( asoup->GetCode()==HCode::DEPARTURE )
                {
                Locomotive loc;
                CPPUNIT_ASSERT( asoup->GetDescr() && asoup->GetDescr()->get_head_locomotive(loc) && !loc.get_crew().empty() );
                }
            }
        }

    set_TimeZone("");
}

void TC_TrackedChart::UncontrolledStationAfterDeathOnSpan_5792_1()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    Uncontrolled_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200713T035505Z' name='Form' Bdg='N7P_S[09410:09420]' waynum='1' index='8718-637-0990' num='2703' length='57' weight='5329' net_weight='3978' through='Y'>"
        L"<rwcoord picketing1_val='25~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='27~' picketing1_comm='Рига-Зилупе' />"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2М62УМ' NumLoc='93' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-SOLOVJEVS' Tim_Beg='2020-07-13 01:10' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200713T040343Z' name='Transition' Bdg='1C[09420]' waynum='1' parknum='1' intervalSec='23' optCode='09000:09420' />"
        L"<SpotEvent create_time='20200713T041419Z' name='Transition' Bdg='1AC[09000]' waynum='1' parknum='1' intervalSec='38' />"
        L"<SpotEvent create_time='20200713T041847Z' name='Transition' Bdg='1JC[09006]' waynum='1' parknum='1' intervalSec='7'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200713T042908Z' name='Transition' Bdg='2C[09010]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200713T043524Z' name='Transition' Bdg='4p[09100]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20200713T044052Z' name='Transition' Bdg='1AC[09670]' waynum='1' parknum='3' optCode='09670:09680' />"
        L"<SpotEvent create_time='20200713T044154Z' name='Transition' Bdg='1C[09670]' waynum='1' parknum='2' />"
        L"<SpotEvent create_time='20200713T044250Z' name='Transition' Bdg='1BC[09670]' waynum='1' parknum='1' optCode='09670:09680' />"
        L"<SpotEvent create_time='20200713T044842Z' name='Transition' Bdg='1C[09680]' waynum='1' parknum='1' intervalSec='2' optCode='09680:09710' />"
        L"<SpotEvent create_time='20200713T045438Z' name='Transition' Bdg='3C[09710]' waynum='3' parknum='1' intervalSec='7' />"
        L"<SpotEvent create_time='20200713T045839Z' name='Span_move' Bdg='NGP1[09710:09990]' />"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==12 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 2703 );
    }
    const std::wstring arrival9990 = L"(:1042 909/000+09900 2703 8718 637 0990 01 09670 13 07 08 03 06/04 1 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05329 03978 228 64344849 61185930 000 000 00 057 000.60 057 000 000\n"
        L"Ю4 00000 1 01 057 000.60 057 000 \n"
        L"Ю4 09900 1 01 057 000.60 057 000 \n"
        L"Ю12 00 64344849 1 075 09900 16117 2072 000 00 00 00 00 00 0248 30 60 0600 04 100 09900 00000 00000 01 87180 01 87180 5860 020 0 0000 0      148 000 00000000\n"
        L"Ю12 00 57564239 191237254127 07002335600161300021\n"
        L"Ю12 00 56081128 255239255255 0234\n"
        L"Ю12 00 55791891 251239254115 2000235871805860ПPЧ   132\n"
        L"Ю12 00 60084746 191237255255 06902420600\n"
        L"Ю12 00 60083524 255239255255 0244\n"
        L"Ю12 00 61492211 255239255255 0243\n"
        L"Ю12 00 57876682 191237255255 07002335600\n"
        L"Ю12 00 62124706 191237255255 06902370600\n"
        L"Ю12 00 53391660 191237255255 07002405600\n"
        L"Ю12 00 55014740 255239255255 0233\n"
        L"Ю12 00 53496428 191239255255 0690240\n"
        L"Ю12 00 61208054 191237255247 070023706000     \n"
        L"Ю12 00 61991600 255255255255 \n"
        L"Ю12 00 55808299 255237255255 02355600\n"
        L"Ю12 00 59379180 255255255255 \n"
        L"Ю12 00 59398941 191239255255 0690240\n"
        L"Ю12 00 53183505 191255255255 070\n"
        L"Ю12 00 59374819 191255255255 069\n"
        L"Ю12 00 62065156 255237255255 02450600\n"
        L"Ю12 00 61926739 255239255255 0243\n"
        L"Ю12 00 50022193 191237255255 07002405600\n"
        L"Ю12 00 54658745 255239255255 0236\n"
        L"Ю12 00 55577456 255239255255 0235\n"
        L"Ю12 00 56080963 255239255255 0233\n"
        L"Ю12 00 53181202 255239255255 0240\n"
        L"Ю12 00 55164669 255239255255 0236\n"
        L"Ю12 00 56105703 255239255255 0233\n"
        L"Ю12 00 65175689 191237255255 06902290600\n"
        L"Ю12 00 58717042 191237255255 07002365600\n"
        L"Ю12 00 55923890 191239255255 0690243\n"
        L"Ю12 00 56177157 191239255255 0700233\n"
        L"Ю12 00 58689522 255239255255 0236\n"
        L"Ю12 00 65181380 191237255255 06902280600\n"
        L"Ю12 00 55322754 255237255255 02435600\n"
        L"Ю12 00 61006078 191237255255 07002330600\n"
        L"Ю12 00 54065909 255237255255 02325600\n"
        L"Ю12 00 55068191 255239255255 0235\n"
        L"Ю12 00 67728642 191237255255 06902320600\n"
        L"Ю12 00 54318076 255237255255 02405600\n"
        L"Ю12 00 62141817 255237255255 02450600\n"
        L"Ю12 00 56219512 191237255255 07002335600\n"
        L"Ю12 00 61004578 255237255255 02320600\n"
        L"Ю12 00 52949831 191237255255 06902415600\n"
        L"Ю12 00 60687613 191237255255 07002400600\n"
        L"Ю12 00 52820859 255237255245 02335600871818032\n"
        L"Ю12 00 53744439 187237255245 071000022456080     000\n"
        L"Ю12 00 62431267 187237255255 07020002350600\n"
        L"Ю12 00 56077563 255237255255 02385600\n"
        L"Ю12 00 58492117 255239255255 0240\n"
        L"Ю12 00 55792998 255239255255 0235\n"
        L"Ю12 00 52935491 255239255255 0233\n"
        L"Ю12 00 50027697 255239255255 0240\n"
        L"Ю12 00 52174695 255255255255 \n"
        L"Ю12 00 57632754 255239255255 0235\n"
        L"Ю12 00 56391550 191239255255 0690243\n"
        L"Ю12 00 61185930 191237255255 07002350600:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText( arrival9990, boost::gregorian::date(2020, 07, 13), nullptr, 0) );
    m_chart->updateTime( time_from_iso("20200713T050400Z"), nullptr );
	AsoupLayer& al = m_chart->getAsoupLayer();
	{
		//проверяем свободность АСОУП (дотягивание не должно сработать так как базовая нить 2703 живая)
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT( !al.IsServed( asoup ) );
	}
    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
        std::make_shared<SpotEvent>(HCode::DEATH, BadgeE( L"NGP1", EsrKit(9710,9990)), time_from_iso("20200713T050314Z")), trainID, TrainCharacteristics::Source::Guess));

    m_chart->updateTime( time_from_iso("20200713T051000Z"), nullptr );


    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==14 );
        auto pLastEvent = pathCreate->GetLastAliveEvent();
        CPPUNIT_ASSERT( pLastEvent && pLastEvent->GetBadge().num()==EsrKit(9990) && 
            pLastEvent->GetTime()==time_from_iso("20200713T050300Z") );

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
    }

    set_TimeZone("");
}

void TC_TrackedChart::UncontrolledStationAfterDeathOnSpan_5792_2()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    Uncontrolled_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200722T161053Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' intervalSec='16' optCode='09810:09812' index='1655-007-0983' num='V1911' length='57' weight='5433' net_weight='3723' liqrr='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2ТЭ116' NumLoc='1042' Consec='1' CarrierCode='2'>"
        L"<Crew EngineDriver='SAPOVALS' Tim_Beg='2020-07-22 09:21' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200722T163325Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' intervalSec='4' optCode='09812:09813' />"
        L"<SpotEvent create_time='20200722T164249Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' intervalSec='20' optCode='09813:09820' />"
        L"<SpotEvent create_time='20200722T164321Z' name='Span_move' Bdg='PV1IP[09813:09820]' waynum='1'>"
        L"<rwcoord picketing1_val='4~200' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='2~' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200722T164321Z' name='Death' Bdg='PV1IP[09813:09820]' waynum='1'>"
        L"<rwcoord picketing1_val='4~200' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='2~' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==5 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 1911 );
    }
    const std::wstring disform9820 = L"(:1042 909/000+09820 1911 1655 007 0983 05 00000 22 07 21 14 04/02 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 05433 03723 264 74946187 73120602 000 000 00 066 000.70 066 000 000 73 066 000 000\n"
        L"Ю4 00000 1 01 066 000.70 066 000 73 066 000 \n"
        L"Ю4 09830 1 01 066 000.70 066 000 73 066 000 \n"
        L"Ю12 00 74946187 1 053 09830 22106 8320 480 01 00 00 00 00 0266 30 70 0732 04 086 09830 09820 00000 01 16550 03 16550 5906 021 0 0000 0      148 000 00000000\n"
        L"Ю12 00 74089376 191237255255 05102320730\n"
        L"Ю12 00 74133539 191255255255 052\n"
        L"Ю12 00 74812843 191237255255 05302700748\n"
        L"Ю12 00 74813668 255239255255 0260\n"
        L"Ю12 00 74743915 191237255255 05902700730\n"
        L"Ю12 00 74890476 187237255251 05338002630748132\n"
        L"Ю12 00 74873050 191255255255 054\n"
        L"Ю12 00 74939828 191237255255 05902690732\n"
        L"Ю12 00 74744137 191237255255 06102700730\n"
        L"Ю12 00 73037301 191239255255 0520232\n"
        L"Ю12 00 74753534 191239255255 0620270\n"
        L"Ю12 00 74812454 191237255255 05402690748\n"
        L"Ю12 00 74941535 191253255255 0610732\n"
        L"Ю12 00 74811761 191237255255 05502640748\n"
        L"Ю12 00 73046955 191237255255 05402320730\n"
        L"Ю12 00 74889288 191237255255 05102670748\n"
        L"Ю12 00 74050170 191237255255 05402270730\n"
        L"Ю12 00 74804964 255237255255 02700748\n"
        L"Ю12 00 74723388 191237255255 06002720730\n"
        L"Ю12 00 74722000 191239255255 0610273\n"
        L"Ю12 00 73060816 191239255255 0530230\n"
        L"Ю12 00 74743618 191239255255 0610272\n"
        L"Ю12 00 74743980 255239255255 0270\n"
        L"Ю12 00 74890740 191237255255 05402620748\n"
        L"Ю12 00 73070112 255237255255 02400730\n"
        L"Ю12 00 74812546 187237255255 05348002600748\n"
        L"Ю12 00 74869835 191239255255 0540265\n"
        L"Ю12 00 74746785 191237255255 06302710730\n"
        L"Ю12 00 72727167 191237255255 05602500720\n"
        L"Ю12 00 73060881 191253255255 0530730\n"
        L"Ю12 00 74746983 191239255255 0600270\n"
        L"Ю12 00 73256323 191239255255 0530232\n"
        L"Ю12 00 73925505 191237255255 06102690732\n"
        L"Ю12 00 74939240 191239255255 0600271\n"
        L"Ю12 00 74941865 191239255255 0610270\n"
        L"Ю12 00 74813346 191237255255 05202650748\n"
        L"Ю12 00 74813619 191239255255 0500268\n"
        L"Ю12 00 73058208 191237255255 05302320730\n"
        L"Ю12 00 73051401 191255255255 052\n"
        L"Ю12 00 72354145 191237255255 05402500720\n"
        L"Ю12 00 74802737 191237255255 05502650748\n"
        L"Ю12 00 74723537 191237255255 06202700730\n"
        L"Ю12 00 73043929 191239255255 0560232\n"
        L"Ю12 00 72046477 191237255255 05402410720\n"
        L"Ю12 00 74872730 191237255255 05302670748\n"
        L"Ю12 00 74723743 187237255255 06258002700730\n"
        L"Ю12 00 74889247 191237255255 05302670748\n"
        L"Ю12 00 72030711 191237255255 05502500720\n"
        L"Ю12 00 74755380 191237255255 06102700730\n"
        L"Ю12 00 74940214 191237255255 06202690732\n"
        L"Ю12 00 72039084 191237255255 05502470720\n"
        L"Ю12 00 74891334 191237255255 05702650748\n"
        L"Ю12 00 74753815 191237255255 06202720730\n"
        L"Ю12 00 73961815 191237255255 05802690732\n"
        L"Ю12 00 74753765 191237255255 06202720730\n"
        L"Ю12 00 73047037 191239255255 0520232\n"
        L"Ю12 00 74753088 191239255255 0610270\n"
        L"Ю12 00 74747353 191239255255 0620272\n"
        L"Ю12 00 74890534 191237255255 05302620748\n"
        L"Ю12 00 74890054 191239255255 0590266\n"
        L"Ю12 00 73052250 191237255255 05302590730\n"
        L"Ю12 00 74753062 191239255255 0630272\n"
        L"Ю12 00 74890542 191237255255 05302630748\n"
        L"Ю12 00 74747403 191237255255 06202700730\n"
        L"Ю12 00 73120602 191239255255 0540232:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText( disform9820, boost::gregorian::date(2020, 07, 22), nullptr, 0) );
    m_chart->updateTime( time_from_iso("20200722T181400Z"), nullptr );
    m_chart->updateTime( time_from_iso("20200722T182400Z"), nullptr );

    AsoupLayer& al = m_chart->getAsoupLayer();

    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==5 );
        auto pLastEvent = pathCreate->GetLastAliveEvent();
        CPPUNIT_ASSERT( pLastEvent && pLastEvent->GetBadge().num()==EsrKit(9813,9820) && 
            pLastEvent->GetTime()==time_from_iso("20200722T164321Z") );

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

    set_TimeZone("");
}

void TC_TrackedChart::SaveCarrierInPassagirArrivalMessage_5797()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    Uncontrolled_helper();

    auto path = loaderSpot(
        L"<HemPath>"
        L"<SpotEvent create_time='20200723T125459Z' name='Form' Bdg='10C[09010]' waynum='10' parknum='1' index='' num='704' length='12' weight='370' rapid='Y' />"
        L"<SpotEvent create_time='20200723T132037Z' name='Departure' Bdg='10C[09010]' waynum='10' parknum='1' />"
        L"<SpotEvent create_time='20200723T132656Z' name='Arrival' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200723T132746Z' name='Departure' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200723T133112Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='42' optCode='09000:09420' />"
        L"<SpotEvent create_time='20200723T133719Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' intervalSec='35' />"
        L"<SpotEvent create_time='20200723T134701Z' name='Arrival' Bdg='1AC[09410]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );
    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto path = *pathSet.cbegin();
        CPPUNIT_ASSERT( path && path->GetEventsCount()==7 && path->GetFirstFragment() &&
           path->GetFirstFragment()->GetNumber().getNumber() == 704 );
    }

    std::wstring asoup9010 = L"<TrainAssignment operation='5' date='23-07-2020 16:19' esr='09010' location='24.121025,56.946324' number='704' issue_moment='20200723T132008Z'>"
        L"<Unit number='290-1--DR1AC' driver='Vladimirovs' driverTime='23-07-2020 16:19' weight='185' length='6' specialConditions='M' carrier='24'>"
        L"<Wagon number='290-1' axisCount='4' weight='70' length='1.87' type='2' owner='25' />"
        L"<Wagon number='290-2' axisCount='4' weight='60' length='1.87' type='3' owner='25' />"
        L"<Wagon number='290-4' axisCount='4' weight='55' length='1.87' type='1' owner='25' />"
        L"</Unit>"
        L"<Unit number='185-3--DR1AC' driver='Vladimirovs' driverTime='23-07-2020 16:19' weight='185' length='6' specialConditions='M' carrier='24'>"
        L"<Wagon number='185-3' axisCount='4' weight='70' length='1.87' type='2' owner='25' />"
        L"<Wagon number='185-6' axisCount='4' weight='60' length='1.87' type='3' owner='25' />"
        L"<Wagon number='311-6' axisCount='4' weight='55' length='1.87' type='1' owner='25' />"
        L"</Unit>"
        L"</TrainAssignment>";

    CPPUNIT_ASSERT(m_chart->processNewPassengerLdzWstr(asoup9010, nullptr, 0));
	m_chart->updateTime( time_from_iso("20200723T132037Z"), nullptr);

    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto tdr = (*pathSet.cbegin())->GetFirstFragment();
        if ( tdr )
        {
            Locomotive loco;
            CPPUNIT_ASSERT ( tdr->get_head_locomotive(loco) );
            CPPUNIT_ASSERT( loco.getCarrier() == 24 );
        }
    }

    auto carrierStr = 
        L"<Carrier>"
        L"<Company>"
        L"<Company carrierCode='2' titleRu='БАЛТИЯС ЭКСПРЕСИС АО' />"
        L"<Company carrierCode='3' titleRu='БАЛТИЯС ТРАНЗИТА СЕРВИСС АО' />"
        L"<Company carrierCode='4' titleRu='ООО ЛДЗ КАРГО' />"
        L"<Company carrierCode='5' titleRu='ГАО &quot;Латвияс дзелзцельш&quot;' />"
        L"</Company>"
        L"<Leasing>"
        L"<Lease locNum='8' locSer='763' carrierCode='5' time='20171130T220000Z' />"
        L"<Lease locNum='13' locSer='618' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='267' locSer='579' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='670' locSer='530' carrierCode='3' time='20171130T220000Z' />"
        L"<Lease locNum='690' locSer='530' carrierCode='3' time='20171130T220000Z' />"
        L"<Lease locNum='933' locSer='530' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='1039' locSer='531' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='1134' locSer='543' carrierCode='5' time='20171130T220000Z' />"
        L"<Lease locNum='1227' locSer='531' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='1701' locSer='531' carrierCode='5' time='20180307T220000Z' />"
        L"<Lease locNum='1861' locSer='757' carrierCode='5' time='20171130T220000Z' />"
        L"<Lease locNum='3139' locSer='572' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='3350' locSer='557' carrierCode='2' time='20171130T220000Z' />"
        L"<Lease locNum='3368' locSer='572' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='3369' locSer='572' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='3425' locSer='575' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='3553' locSer='572' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='3967' locSer='557' carrierCode='4' time='20171130T220000Z' />"
        L"<Lease locNum='4987' locSer='757' carrierCode='5' time='20171130T220000Z' />"
        L"<Lease locNum='5963' locSer='557' carrierCode='2' time='20171130T220000Z' />"
        L"</Leasing>"
        L"</Carrier>";

    attic::a_document adoc;
    adoc.load_wide( carrierStr );
    Carrier carrier;
    carrier.deserialize(adoc.document_element());

    std::vector< LocoId > changed;
    m_chart->updateCarrier(changed, carrier);

    {
        HappenLayer::ReadAccessor container = m_chart->getHappenLayer().GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto tdr = (*pathSet.cbegin())->GetFirstFragment();
        if ( tdr )
        {
            Locomotive loco;
            CPPUNIT_ASSERT ( tdr->get_head_locomotive(loco) );
            CPPUNIT_ASSERT( loco.getCarrier() == 24 );
        }
    }

    set_TimeZone("");
}

// При попытке добавить в нить сигнал с нарушением хронологии
// закрываем исходную нить Death, отбрасываем некорректный сигнла БЕЗ ПОРОЖДЕНИЯ
// новой нити для трансивера по этому сигналу. 
// (обсудили с Потаповым А.М. - нет смысла по ошибочному сигналу порождать нить )
void TC_TrackedChart::RefuseGuessArrivalWrongTime()
{
    NsiBasis_helper();
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200803T071223Z' name='Form' Bdg='1C[09530]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200803T082709Z' name='Arrival' Bdg='1C[09530]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200803T082857Z' name='Departure' Bdg='1C[09530]' waynum='1' parknum='1' optCode='09530:09540' />"
        L"<SpotEvent create_time='20200803T083129Z' name='Station_entry' Bdg='NDP[09540]'>"
        L"<rwcoord picketing1_val='29~700' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200803T083244Z' name='Death' Bdg='1C[09540]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200803T083249Z' name='Form' Bdg='4-6SP:6+[09540]' />"
        L"</HemPath>" );

    unsigned const trainID = 61;

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );

    {
        HappenLayer& hl = m_chart->getHappenLayer();
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==6 );
    }

    SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
        ParkWayKit(1, 1), std::make_pair(PicketingInfo(), PicketingInfo()));
    auto spotPtr = std::make_shared<SpotEvent>(HCode::ARRIVAL, BadgeE(L"1C", EsrKit(9540)), time_from_iso("20200803T083212Z"), spotDetails);
    CPPUNIT_ASSERT( !m_chart->acceptTrackerEvent(spotPtr, trainID, TrainCharacteristics::Source::Guess) );

    {
        auto pathSet = m_chart->getHappenLayer().GetReadAccess()->getAllPathes();
        CPPUNIT_ASSERT(  pathSet.size() == 2 );
        for ( auto path : pathSet )
        {
            CPPUNIT_ASSERT( path->GetEventsCount()==7 || path->GetEventsCount()==1 );
            if ( path->GetEventsCount()==7 )
            {
                CPPUNIT_ASSERT ( path->GetLastEvent()->GetCode() == HCode::DEATH );
                CPPUNIT_ASSERT( m_chart->getGuessTransciever().getPathId(path) == 0 );
            }
            else if ( path->GetEventsCount()==1 )
            {
                CPPUNIT_ASSERT ( path->GetLastEvent()->GetCode() == HCode::ARRIVAL );
                CPPUNIT_ASSERT( m_chart->getGuessTransciever().getPathId(path) == trainID );
            }
        }
    }
}

// void TC_TrackedChart::GlueWithSpanStoppingBegin()
// {
//     set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
//     NsiBasis_helper();
//     Uncontrolled_helper();
// 
//     auto path1 = loaderSpot(
//         L"<HemPath>"
//         L"  <SpotEvent create_time='20180518T065601Z' name='Form' Bdg='6C[09180]' waynum='6' parknum='1' index='0918-031-0987' num='9509' length='57' weight='4946' net_weight='3624' heavy='Y'>"
//         L"    <Locomotive Series='2ТЭ10У' NumLoc='185' Consec='1' CarrierCode='4'>"
//         L"      <Crew EngineDriver='ABRAMOVS' Tim_Beg='2018-05-18 09:32' />"
//         L"    </Locomotive>"
//         L"  </SpotEvent>"
//         L"  <SpotEvent create_time='20180518T080206Z' name='Departure' Bdg='6C[09180]' waynum='6' parknum='1' optCode='09180:09280' />"
//         L"  <SpotEvent create_time='20180518T081000Z' name='Span_move' Bdg='2GPP[09180:09280]' waynum='1' >"
//         L"    <rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
//         L"    <rwcoord picketing1_val='54~' picketing1_comm='Реньге' />"
//         L"  </SpotEvent>"
//         L"  <SpotEvent create_time='20180518T081000Z' name='Death' Bdg='2GPP[09180:09280]' waynum='1' >"
//         L"    <rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
//         L"    <rwcoord picketing1_val='54~' picketing1_comm='Реньге' />"
//         L"  </SpotEvent>"
//         L"</HemPath>"
//         );
//     auto path2 = loaderSpot(
//         L"<HemPath>"
//         L"  <SpotEvent create_time='20180518T121643Z' name='Form' Bdg='LT-1C[09355:09860]' waynum='1' index='0918-031-0987' num='9509' length='57' weight='4946' net_weight='3624' heavy='Y'>"
//         L"    <rwcoord picketing1_val='226~' picketing1_comm='Лиепая' />"
//         L"    <rwcoord picketing1_val='227~700' picketing1_comm='Лиепая' />"
//         L"  </SpotEvent>"
//         L"  <SpotEvent create_time='20180518T122024Z' name='Transition' Bdg='55C[09860]' waynum='55' parknum='2' intervalSec='8' optCode='09860' />"
//         L"  <SpotEvent create_time='20180518T122344Z' name='Arrival' Bdg='5C[09860]' waynum='5' parknum='1' />"
//         L"  <SpotEvent create_time='20180518T135149Z' name='Disform' Bdg='5C[09860]' waynum='5' parknum='1' />"
//         L"</HemPath>"
//         );
// 
//     const TrainDescr td(L"9509", L"0918-031-0987");
// 
// 
//     CPPUNIT_ASSERT( CreatePath(*m_chart, train, 1) );
//     auto pathSet = m_chart->getHappenLayer().GetReadAccess()->getAllPathes();
//     auto path = *pathSet.cbegin();
//     CPPUNIT_ASSERT( path->GetEventsCount()==8 && path->GetLastEvent()->GetBadge().num()==EsrKit(9813, 9820) );
// 
//     std::wstring strArrival = L"(:1042 909/000+09820 8221 0001 192 1100 01 09813 09 06 14 28 03/02 1 0/00 00 0:)";
// 
//     CPPUNIT_ASSERT(m_chart->processNewAsoupText(strArrival,
//         boost::gregorian::date(2020, 6, 9), nullptr, 0 ));
// 
//     m_chart->updateTime( time_from_iso("20200609T120000Z"), nullptr );
// 
//     auto& al = m_chart->getAsoupLayer();
//     auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
//     CPPUNIT_ASSERT ( !pathAsoupList.empty() && al.IsServed( pathAsoupList.front() ) );
//     pathSet = m_chart->getHappenLayer().GetReadAccess()->getAllPathes();
//     CPPUNIT_ASSERT( pathSet.size() == 1 );
//     path = *pathSet.cbegin();
//     CPPUNIT_ASSERT( path->GetEventsCount()==9 && path->GetLastEvent()->GetBadge().num()==EsrKit(9820) );
//     set_TimeZone("");
// }


void TC_TrackedChart::IdentifyByIndex_5823()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200825T194629Z' name='Arrival' Bdg='2C[11390]' waynum='2' parknum='1' index='0982-028-3100' num='J1914' length='57' weight='2374' liqrr='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='CIBULSKIS' Tim_Beg='2020-08-25 14:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T195412Z' name='Departure' Bdg='2C[11390]' waynum='2' parknum='1' optCode='11381:11390' />"
        L"<SpotEvent create_time='20200825T200418Z' name='Transition' Bdg='1C[11381]' waynum='1' parknum='1' intervalSec='1' optCode='11380:11381' />"
        L"<SpotEvent create_time='20200825T201604Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' intervalSec='1' optCode='11311:11380' />"
        L"<SpotEvent create_time='20200825T203139Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' intervalSec='8' optCode='11311:11321' />"
        L"<SpotEvent create_time='20200825T204322Z' name='Transition' Bdg='701SP:701+[11321]'>"
        L"<rwcoord picketing1_val='223~200' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T205001Z' name='Arrival' Bdg='2C[11310]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200825T205028Z' name='Departure' Bdg='2C[11310]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200825T205616Z' name='Arrival' Bdg='3AC[11320]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200826T022342Z' name='Departure' Bdg='3AC[11320]' waynum='3' parknum='1' />"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==10 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 1914 );
    }

    const std::wstring departure11320 = L"(:1042 909/000+11320 2968 0982 028 3100 03 11310 26 08 05 22 01/03 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02374 00000 264 57878712 77996395 000 000 00 000 066.70 000 066 000\n"
        L"Ю3 530 00009281 1 03 45 0000 17996 OЛЬШEBCKИЙ  \n"
        L"Ю3 530 00009282 9\n"
        L"Ю3 531 00013580 2 03 45 0000 27668 PAXHO       \n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 57878712 1 000 27360 48816 6987 357 01 00 00 00 00 0368 12 70 5780 04 086 31000 00000 07792 24 09820 18 09830 7051 020 1 1280 09836  128 000 00000000\n"
        L"Ю12 00 77971190 255237255239 036107691250\n"
        L"Ю12 00 77935492 255239255255 0360\n"
        L"Ю12 00 77719797 255239255255 0373\n"
        L"Ю12 00 77632594 255255255255 \n"
        L"Ю12 00 77943090 255239255255 0361\n"
        L"Ю12 00 78084894 255239255239 03901280\n"
        L"Ю12 00 77966596 255239255239 03611250\n"
        L"Ю12 00 58237330 255237255255 03455080\n"
        L"Ю12 00 58242413 255239255255 0315\n"
        L"Ю12 00 77942092 255237255255 03610769\n"
        L"Ю12 00 58242298 255237255255 03165080\n"
        L"Ю12 00 58236993 255239255255 0345\n"
        L"Ю12 00 77965291 255237255255 03620769\n"
        L"Ю12 00 78080496 255239255239 03861280\n"
        L"Ю12 00 78078797 255239255255 0389\n"
        L"Ю12 00 58237793 255237255239 034650801250\n"
        L"Ю12 00 58237306 255239255255 0347\n"
        L"Ю12 00 76660380 251237255239 557035107691280\n"
        L"Ю12 00 77302396 255239255239 03721250\n"
        L"Ю12 00 76657741 255239255239 03741280\n"
        L"Ю12 00 57739336 255237255255 03775080\n"
        L"Ю12 00 77635498 255237255239 037307691250\n"
        L"Ю12 00 76657675 255255255239 1280\n"
        L"Ю12 00 77714293 255239255239 03721250\n"
        L"Ю12 00 57715617 255237255239 034950801280\n"
        L"Ю12 00 77936391 255237255239 036107691250\n"
        L"Ю12 00 77972099 255239255255 0360\n"
        L"Ю12 00 77998193 255239255255 0362\n"
        L"Ю12 00 58237116 255237255255 03455080\n"
        L"Ю12 00 77942894 251237255255 45703610769\n"
        L"Ю12 00 77721199 255239255255 0375\n"
        L"Ю12 00 77970895 255239255255 0362\n"
        L"Ю12 00 77988491 255239255255 0361\n"
        L"Ю12 00 77644995 255239255255 0371\n"
        L"Ю12 00 77705499 255239255255 0372\n"
        L"Ю12 00 76660653 255239255239 03781280\n"
        L"Ю12 00 58267931 255237255239 034650801250\n"
        L"Ю12 00 77936193 255237255255 03610769\n"
        L"Ю12 00 77988392 255255255255 \n"
        L"Ю12 00 78089992 255239255239 03881280\n"
        L"Ю12 00 77706398 255239255239 03721250\n"
        L"Ю12 00 77302693 255239255255 0373\n"
        L"Ю12 00 58260621 255237255255 03465080\n"
        L"Ю12 00 58237322 255239255255 0345\n"
        L"Ю12 00 76659184 255237255239 034007691280\n"
        L"Ю12 00 58237736 255237255239 034750801250\n"
        L"Ю12 00 50518489 255239255239 03461280\n"
        L"Ю12 00 76624659 251237255255 35703160769\n"
        L"Ю12 00 50832476 255237255255 03685080\n"
        L"Ю12 00 50823566 255239255255 0367\n"
        L"Ю12 00 58242470 255239255239 03151250\n"
        L"Ю12 00 76641943 255253255239 07691280\n"
        L"Ю12 00 58242629 255237255239 031650801250\n"
        L"Ю12 00 78077294 255237255239 038707691280\n"
        L"Ю12 00 78065893 255239255255 0386\n"
        L"Ю12 00 76660430 255239255255 0378\n"
        L"Ю12 00 58260977 255237255239 034650801250\n"
        L"Ю12 00 58260555 255255255255 \n"
        L"Ю12 00 58237389 255239255255 0349\n"
        L"Ю12 00 77631299 255237255255 03730769\n"
        L"Ю12 00 77293793 255239255255 0374\n"
        L"Ю12 00 78067394 255239255239 03871280\n"
        L"Ю12 00 77937696 255239255239 03611250\n"
        L"Ю12 00 77966794 255255255255 \n"
        L"Ю12 00 77996395 255239255255 0362:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText( departure11320, boost::gregorian::date(2020, 8, 26), nullptr, 0) );
    AsoupLayer& al = m_chart->getAsoupLayer();
    
    {
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    auto asoup = pathAsoupList.front();
    CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

    auto tDeparture = time_from_iso("20200826T022959Z");

    CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
        std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE( L"2C", EsrKit(11310)), tDeparture), trainID, TrainCharacteristics::Source::Guess));
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==11 );
        auto pLastEvent = pathCreate->GetLastEvent();
        CPPUNIT_ASSERT( pLastEvent && pLastEvent->GetBadge().num()==EsrKit(11310) );

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

    m_chart->updateTime(tDeparture + Hem::aeAsoupToSpotFind::getSecondaryProcessingDefaultDelaySec(), nullptr);

    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
    }

    set_TimeZone("");
}

void TC_TrackedChart::IdentifyByIndexAndLinkFreeStrangeTimeAsoup_5823()
{
    setupEsrGuide();
    Uncontrolled_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200825T194629Z' name='Arrival' Bdg='2C[11390]' waynum='2' parknum='1' index='0982-028-3100' num='J1914' length='57' weight='2374' liqrr='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='CIBULSKIS' Tim_Beg='2020-08-25 14:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T195412Z' name='Departure' Bdg='2C[11390]' waynum='2' parknum='1' optCode='11381:11390' />"
        L"<SpotEvent create_time='20200825T200418Z' name='Transition' Bdg='1C[11381]' waynum='1' parknum='1' intervalSec='1' optCode='11380:11381' />"
        L"<SpotEvent create_time='20200825T201604Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' intervalSec='1' optCode='11311:11380' />"
        L"<SpotEvent create_time='20200825T203139Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' intervalSec='8' optCode='11311:11321' />"
        L"<SpotEvent create_time='20200825T204322Z' name='Transition' Bdg='701SP:701+[11321]'>"
        L"<rwcoord picketing1_val='223~200' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T205001Z' name='Arrival' Bdg='2C[11310]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200825T205028Z' name='Departure' Bdg='2C[11310]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200825T205616Z' name='Arrival' Bdg='3AC[11320]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200826T022342Z' name='Departure' Bdg='3AC[11320]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200826T022959Z' name='Transition' Bdg='2C[11310]' waynum='2' parknum='1' intervalSec='16' />"
        L"<SpotEvent create_time='20200826T023543Z' name='Transition' Bdg='1SP[11271]'>"
        L"<rwcoord picketing1_val='440~780' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200826T024738Z' name='Transition' Bdg='1C[11270]' waynum='1' parknum='1' intervalSec='5' optCode='11267:11270' />"
        L"<SpotEvent create_time='20200826T025517Z' name='Transition' Bdg='1AC[11267]' waynum='1' parknum='1' intervalSec='34' optCode='11266:11267' />"
        L"<SpotEvent create_time='20200826T025632Z' name='Transition' Bdg='1C&apos;2[11267]' waynum='1' parknum='2' intervalSec='14' optCode='11266:11267' />"
        L"<SpotEvent create_time='20200826T030502Z' name='Transition' Bdg='1C[11266]' waynum='1' parknum='1' intervalSec='8' optCode='11265:11266' />"
        L"<SpotEvent create_time='20200826T031254Z' name='Transition' Bdg='1AC[11265]' waynum='1' parknum='1' intervalSec='4' optCode='11260:11265' />"
        L"<SpotEvent create_time='20200826T031429Z' name='Transition' Bdg='1C&apos;2[11265]' waynum='1' parknum='2' intervalSec='10' optCode='11260:11265' />"
        L"<SpotEvent create_time='20200826T032635Z' name='Arrival' Bdg='1C[11260]' waynum='1' parknum='1' />"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==19 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 1914 );
    }

    const std::wstring departure11320 = L"(:1042 909/000+11320 2968 0982 028 3100 03 11260 26 08 05 12 01/03 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02374 00000 264 57878712 77996395 000 000 00 000 066.70 000 066 000\n"
        L"Ю3 530 00009281 1 03 45 0000 17996 OЛЬШEBCKИЙ  \n"
        L"Ю3 530 00009282 9\n"
        L"Ю3 531 00013580 2 03 45 0000 27668 PAXHO       \n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 57878712 1 000 27360 48816 6987 357 01 00 00 00 00 0368 12 70 5780 04 086 31000 00000 07792 24 09820 18 09830 7051 020 1 1280 09836  128 000 00000000\n"
        L"Ю12 00 77971190 255237255239 036107691250\n"
        L"Ю12 00 77935492 255239255255 0360\n"
        L"Ю12 00 77719797 255239255255 0373\n"
        L"Ю12 00 77632594 255255255255 \n"
        L"Ю12 00 77943090 255239255255 0361\n"
        L"Ю12 00 78084894 255239255239 03901280\n"
        L"Ю12 00 77966596 255239255239 03611250\n"
        L"Ю12 00 58237330 255237255255 03455080\n"
        L"Ю12 00 58242413 255239255255 0315\n"
        L"Ю12 00 77942092 255237255255 03610769\n"
        L"Ю12 00 58242298 255237255255 03165080\n"
        L"Ю12 00 58236993 255239255255 0345\n"
        L"Ю12 00 77965291 255237255255 03620769\n"
        L"Ю12 00 78080496 255239255239 03861280\n"
        L"Ю12 00 78078797 255239255255 0389\n"
        L"Ю12 00 58237793 255237255239 034650801250\n"
        L"Ю12 00 58237306 255239255255 0347\n"
        L"Ю12 00 76660380 251237255239 557035107691280\n"
        L"Ю12 00 77302396 255239255239 03721250\n"
        L"Ю12 00 76657741 255239255239 03741280\n"
        L"Ю12 00 57739336 255237255255 03775080\n"
        L"Ю12 00 77635498 255237255239 037307691250\n"
        L"Ю12 00 76657675 255255255239 1280\n"
        L"Ю12 00 77714293 255239255239 03721250\n"
        L"Ю12 00 57715617 255237255239 034950801280\n"
        L"Ю12 00 77936391 255237255239 036107691250\n"
        L"Ю12 00 77972099 255239255255 0360\n"
        L"Ю12 00 77998193 255239255255 0362\n"
        L"Ю12 00 58237116 255237255255 03455080\n"
        L"Ю12 00 77942894 251237255255 45703610769\n"
        L"Ю12 00 77721199 255239255255 0375\n"
        L"Ю12 00 77970895 255239255255 0362\n"
        L"Ю12 00 77988491 255239255255 0361\n"
        L"Ю12 00 77644995 255239255255 0371\n"
        L"Ю12 00 77705499 255239255255 0372\n"
        L"Ю12 00 76660653 255239255239 03781280\n"
        L"Ю12 00 58267931 255237255239 034650801250\n"
        L"Ю12 00 77936193 255237255255 03610769\n"
        L"Ю12 00 77988392 255255255255 \n"
        L"Ю12 00 78089992 255239255239 03881280\n"
        L"Ю12 00 77706398 255239255239 03721250\n"
        L"Ю12 00 77302693 255239255255 0373\n"
        L"Ю12 00 58260621 255237255255 03465080\n"
        L"Ю12 00 58237322 255239255255 0345\n"
        L"Ю12 00 76659184 255237255239 034007691280\n"
        L"Ю12 00 58237736 255237255239 034750801250\n"
        L"Ю12 00 50518489 255239255239 03461280\n"
        L"Ю12 00 76624659 251237255255 35703160769\n"
        L"Ю12 00 50832476 255237255255 03685080\n"
        L"Ю12 00 50823566 255239255255 0367\n"
        L"Ю12 00 58242470 255239255239 03151250\n"
        L"Ю12 00 76641943 255253255239 07691280\n"
        L"Ю12 00 58242629 255237255239 031650801250\n"
        L"Ю12 00 78077294 255237255239 038707691280\n"
        L"Ю12 00 78065893 255239255255 0386\n"
        L"Ю12 00 76660430 255239255255 0378\n"
        L"Ю12 00 58260977 255237255239 034650801250\n"
        L"Ю12 00 58260555 255255255255 \n"
        L"Ю12 00 58237389 255239255255 0349\n"
        L"Ю12 00 77631299 255237255255 03730769\n"
        L"Ю12 00 77293793 255239255255 0374\n"
        L"Ю12 00 78067394 255239255239 03871280\n"
        L"Ю12 00 77937696 255239255239 03611250\n"
        L"Ю12 00 77966794 255255255255 \n"
        L"Ю12 00 77996395 255239255255 0362:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText( departure11320, boost::gregorian::date(2020, 8, 26), nullptr, 0) );
	m_chart->updateTime(time_from_iso("20200826T021200Z"), nullptr);
    AsoupLayer& al = m_chart->getAsoupLayer();

    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

    const std::wstring arrival11260 = 
        L"(:1042 909/000+11260 2968 0982 028 3100 01 11265 26 08 06 26 01/01 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02374 00000 264 57878712 77996395 000 000 00 000 066.70 000 066 000\n"
        L"Ю3 530 00009281 1 03 45 0000 17996 OЛЬШEBCKИЙ  \n"
        L"Ю3 530 00009282 9\n"
        L"Ю3 531 00013580 2 03 45 0000 27668 PAXHO       \n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 57878712 1 000 27360 48816 6987 357 01 00 00 00 00 0368 12 70 5780 04 086 31000 00000 07792 24 09820 18 09830 7051 020 1 1280 09836  128 000 00000000\n"
        L"Ю12 00 77971190 255237255239 036107691250\n"
        L"Ю12 00 77935492 255239255255 0360\n"
        L"Ю12 00 77719797 255239255255 0373\n"
        L"Ю12 00 77632594 255255255255 \n"
        L"Ю12 00 77943090 255239255255 0361\n"
        L"Ю12 00 78084894 255239255239 03901280\n"
        L"Ю12 00 77966596 255239255239 03611250\n"
        L"Ю12 00 58237330 255237255255 03455080\n"
        L"Ю12 00 58242413 255239255255 0315\n"
        L"Ю12 00 77942092 255237255255 03610769\n"
        L"Ю12 00 58242298 255237255255 03165080\n"
        L"Ю12 00 58236993 255239255255 0345\n"
        L"Ю12 00 77965291 255237255255 03620769\n"
        L"Ю12 00 78080496 255239255239 03861280\n"
        L"Ю12 00 78078797 255239255255 0389\n"
        L"Ю12 00 58237793 255237255239 034650801250\n"
        L"Ю12 00 58237306 255239255255 0347\n"
        L"Ю12 00 76660380 251237255239 557035107691280\n"
        L"Ю12 00 77302396 255239255239 03721250\n"
        L"Ю12 00 76657741 255239255239 03741280\n"
        L"Ю12 00 57739336 255237255255 03775080\n"
        L"Ю12 00 77635498 255237255239 037307691250\n"
        L"Ю12 00 76657675 255255255239 1280\n"
        L"Ю12 00 77714293 255239255239 03721250\n"
        L"Ю12 00 57715617 255237255239 034950801280\n"
        L"Ю12 00 77936391 255237255239 036107691250\n"
        L"Ю12 00 77972099 255239255255 0360\n"
        L"Ю12 00 77998193 255239255255 0362\n"
        L"Ю12 00 58237116 255237255255 03455080\n"
        L"Ю12 00 77942894 251237255255 45703610769\n"
        L"Ю12 00 77721199 255239255255 0375\n"
        L"Ю12 00 77970895 255239255255 0362\n"
        L"Ю12 00 77988491 255239255255 0361\n"
        L"Ю12 00 77644995 255239255255 0371\n"
        L"Ю12 00 77705499 255239255255 0372\n"
        L"Ю12 00 76660653 255239255239 03781280\n"
        L"Ю12 00 58267931 255237255239 034650801250\n"
        L"Ю12 00 77936193 255237255255 03610769\n"
        L"Ю12 00 77988392 255255255255 \n"
        L"Ю12 00 78089992 255239255239 03881280\n"
        L"Ю12 00 77706398 255239255239 03721250\n"
        L"Ю12 00 77302693 255239255255 0373\n"
        L"Ю12 00 58260621 255237255255 03465080\n"
        L"Ю12 00 58237322 255239255255 0345\n"
        L"Ю12 00 76659184 255237255239 034007691280\n"
        L"Ю12 00 58237736 255237255239 034750801250\n"
        L"Ю12 00 50518489 255239255239 03461280\n"
        L"Ю12 00 76624659 251237255255 35703160769\n"
        L"Ю12 00 50832476 255237255255 03685080\n"
        L"Ю12 00 50823566 255239255255 0367\n"
        L"Ю12 00 58242470 255239255239 03151250\n"
        L"Ю12 00 76641943 255253255239 07691280\n"
        L"Ю12 00 58242629 255237255239 031650801250\n"
        L"Ю12 00 78077294 255237255239 038707691280\n"
        L"Ю12 00 78065893 255239255255 0386\n"
        L"Ю12 00 76660430 255239255255 0378\n"
        L"Ю12 00 58260977 255237255239 034650801250\n"
        L"Ю12 00 58260555 255255255255 \n"
        L"Ю12 00 58237389 255239255255 0349\n"
        L"Ю12 00 77631299 255237255255 03730769\n"
        L"Ю12 00 77293793 255239255255 0374\n"
        L"Ю12 00 78067394 255239255239 03871280\n"
        L"Ю12 00 77937696 255239255239 03611250\n"
        L"Ю12 00 77966794 255255255255 \n"
        L"Ю12 00 77996395 255239255255 0362:)";


    CPPUNIT_ASSERT(m_chart->processNewAsoupText( arrival11260, boost::gregorian::date(2020, 8, 26), nullptr, 0) );
	m_chart->updateTime(time_from_iso("20200826T032700Z"), nullptr);

    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size()==2 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
        asoup = pathAsoupList.back();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
    }

}

void TC_TrackedChart::GlueFromUncontrolledStation()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    Uncontrolled_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200912T060900Z' name='ExplicitForm' Bdg='ASOUP 1042[9604]' waynum='2' parknum='1' index='0964-089-0900' num='' length='39' weight='1091' noinfo='Y' />"
        L"<SpotEvent create_time='20200912T062129Z' name='Departure' Bdg='?[9604]' waynum='2' parknum='1' index='' num='' length='39' weight='1091' noinfo='Y' />"
        L"<SpotEvent create_time='20200912T062817Z' name='Transition' Bdg='?[9603]' />"
        L"<SpotEvent create_time='20200912T063658Z' name='Transition' Bdg='?p1w1[9609]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200912T064741Z' name='Transition' Bdg='?p1w1[9612]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200912T065030Z' name='Transition' Bdg='5s[9616]' waynum='5' parknum='1' optCode='11100:11101' />"
		L"<SpotEvent create_time='20200912T065630Z' name='Arrival' Bdg='5s[9630]' waynum='5' parknum='1' optCode='11100:11101' />"
        L"<SpotEvent create_time='20200912T065630Z' name='Death' Bdg='5s[9630]' waynum='5' parknum='1' optCode='11100:11101' />"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==8 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
    }

    const std::wstring form9070 = L"(:1042 909/000+09070 9999 0907 240 0900 07 09060 12 09 07 33 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 015 00970 00659 056 64162258 95044863 000 000 00 012 002.60 000 002 000 90 012 000 000 95 012 000 000\n"
        L"Ю4 00000 0 00 000 002.60 000 002 \n"
        L"Ю4 00000 1 01 012 000.90 012 000 95 012 000 \n"
        L"Ю4 09870 1 01 012 000.90 012 000 95 012 000 \n"
        L"Ю12 00 64162258 1 000 00000 25409 5754 000 00 00 00 00 00 0231 11 60 0600 04 100 09000 00600 00000 00 09070 19 00000 0000 021 0 0000 09072  128 000 00000000\n"
        L"Ю12 00 63377121 255239255255 0239\n"
        L"Ю12 00 95806485 135225081179 05709870014005552022030950950106091800109870340250     132\n"
        L"Ю12 00 95039368 191255255255 053\n"
        L"Ю12 00 95048385 255255255255 \n"
        L"Ю12 00 95127221 191255255255 058\n"
        L"Ю12 00 95085163 255255255255 \n"
        L"Ю12 00 95574224 191255255255 059\n"
        L"Ю12 00 95032355 191255255255 054\n"
        L"Ю12 00 95043832 191255255255 053\n"
        L"Ю12 00 95044053 191255255255 054\n"
        L"Ю12 00 95000717 255255255255 \n"
        L"Ю12 00 95031241 191255255255 053\n"
        L"Ю12 00 95044863 255255255255 :)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText( form9070, boost::gregorian::date(2020, 9, 12), nullptr, 0) );
    AsoupLayer& al = m_chart->getAsoupLayer();
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    auto asoup = pathAsoupList.front();
    CPPUNIT_ASSERT( !al.IsServed( asoup ) );

    m_chart->updateTime( time_from_iso("20200912T073500Z"), nullptr );
    CPPUNIT_ASSERT( !al.IsServed( asoup ) );

    set_TimeZone("");
}

void TC_TrackedChart::GlueOnControlledStation_5955()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200919T171610Z' name='Form' Bdg='KPU5A/18A_O[09104:09150]' waynum='1' index='' num='4835' reslocoutbnd='Y'>"
        L"<rwcoord picketing1_val='15~' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200919T172010Z' name='Transition' Bdg='4Ap[09150]' waynum='4' parknum='1' intervalSec='46' optCode='09150:09160' />"
        L"<SpotEvent create_time='20200919T172858Z' name='Transition' Bdg='2p[09160]' waynum='2' parknum='1' intervalSec='41' optCode='09160:09162' />"
        L"<SpotEvent create_time='20200919T173720Z' name='Transition' Bdg='AGP[09162]' waynum='1' parknum='1' intervalSec='116' optCode='09180'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200919T173720Z' name='Station_entry' Bdg='NAP[09180]' />"
        L"<SpotEvent create_time='20200919T174330Z' name='Disform' Bdg='NMDP[09180]' />"
        L"</HemPath>"
        );

    unsigned const trainID = 64;
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    {
    HappenLayer::ReadAccessor container = hl.GetReadAccess();
    auto pathSet = container->getAllPathes();
    CPPUNIT_ASSERT( pathSet.size() == 1 );
    auto pathCreate = *pathSet.cbegin();
    CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==6 );
    auto gis = pathCreate->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    }

    const std::wstring form9070 = L"(:1042 909/000+09180 4835 0001 514 0918 01 09000 19 09 20 42 02/44 1 0/00 00 0:)";

    {
    CPPUNIT_ASSERT(m_chart->processNewAsoupText( form9070, boost::gregorian::date(2020, 9, 12), nullptr, 0) );
	m_chart->updateTime( time_from_iso("20200919T174400"), nullptr );
    AsoupLayer& al = m_chart->getAsoupLayer();
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    auto asoup = pathAsoupList.front();
    CPPUNIT_ASSERT( al.IsServed( asoup ) );
    auto pathSet = hl.GetReadAccess()->getAllPathes();
    CPPUNIT_ASSERT( pathSet.size() == 1 );
    auto pathCreate = *pathSet.cbegin();
    CPPUNIT_ASSERT( pathCreate->GetEventsCount()==7 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form     Transition Transition Transition Station_entry Arrival Death " ) );
    }
}

void TC_TrackedChart::IdentifyOnEndStation_KIG_784()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    NsiBasis_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20200920T043007Z' name='Station_exit' Bdg='6-8SP:6+[11310]'>"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200920T043328Z' name='Transition' Bdg='701SP[11321]'>"
        L"<rwcoord picketing1_val='222~474' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200920T044700Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' optCode='11311:11380' />"
        L"<SpotEvent create_time='20200920T045951Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' optCode='11380:11381' />"
        L"<SpotEvent create_time='20200920T051211Z' name='Transition' Bdg='1C[11381]' waynum='1' parknum='1' optCode='11381:11390' />"
        L"<SpotEvent create_time='20200920T052056Z' name='Transition' Bdg='1C[11390]' waynum='1' parknum='1' optCode='11390:11391' />"
        L"<SpotEvent create_time='20200920T053719Z' name='Transition' Bdg='1C[11391]' waynum='1' parknum='1' optCode='11391:11401' />"
        L"<SpotEvent create_time='20200920T054800Z' name='Transition' Bdg='1C[11401]' waynum='1' parknum='1' optCode='11401:11412' />"
        L"<SpotEvent create_time='20200920T054915Z' name='Span_move' Bdg='P4PC[11401:11412]' waynum='1'>"
        L"<rwcoord picketing1_val='152~833' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='152~433' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    unsigned const trainID = 64;
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==9 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.empty() );
    }

    const std::wstring departure11310 = 
    L"(:1042 909/000+11310 2407 1800 389 0900 03 11321 20 09 07 30 01/04 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04722 03357 228 52961505 51728194 000 000 00 054 003.20 003 001 000 40 002 000 000 60 023 000 000 70 015 002 000 73 000 002 000 90 011 000 000 95 001 000 000 96 010 000 000\n"
        L"Ю3 550 00002181 1 06 03 0000 00000 VANDISH     \n"
        L"Ю3 550 00002182 9\n"
        L"Ю4 00000 0 00 000 003.20 000 001 70 000 002 73 000 002 \n"
        L"Ю4 08620 2 08 006 000.70 004 000 90 002 000 96 002 000 \n"
        L"Ю4 00000 1 01 048 000.20 003 000 40 002 000 60 023 000 70 011 000 90 009 000 95 001 000 96 008 000 \n"
        L"Ю4 09480 1 01 003 000.20 003 000 \n"
        L"Ю4 09660 1 01 019 000.40 002 000 70 010 000 90 007 000 96 007 000 \n"
        L"Ю4 11100 1 01 023 000.60 023 000 \n"
        L"Ю4 09000 1 01 001 000.70 001 000 \n"
        L"Ю4 09700 1 01 001 000.90 001 000 96 001 000 \n"
        L"Ю4 09740 1 01 001 000.90 001 000 95 001 000 \n"
        L"Ю12 00 52961505 1 064 11100 43321 4175 360 00 00 00 00 00 0234 30 60 5600 04 100 09000 11100 00000 01 11100 17 27360 6987 020 0 0000 0      132 000 00000000\n"
        L"Ю12 00 53052320 255239255255 0240\n"
        L"Ю12 00 53112140 255239255255 0242\n"
        L"Ю12 00 56898992 255237255255 02155608\n"
        L"Ю12 00 61048542 255237255255 02390600\n"
        L"Ю12 00 59680041 255237255255 02355600\n"
        L"Ю12 00 55181069 255239255255 0237\n"
        L"Ю12 00 62141122 255237255255 02450600\n"
        L"Ю12 00 61050571 255239255255 0240\n"
        L"Ю12 00 62548383 255239255255 0235\n"
        L"Ю12 00 53496048 255237255255 02435600\n"
        L"Ю12 00 61942363 255237255255 02390600\n"
        L"Ю12 00 56975949 255237255255 02255608\n"
        L"Ю12 00 67714949 255237255255 02500600\n"
        L"Ю12 00 53017802 255237255255 02245608\n"
        L"Ю12 00 58717745 255237255255 02365600\n"
        L"Ю12 00 53606273 255239255255 0238\n"
        L"Ю12 00 52237963 255255255255 \n"
        L"Ю12 00 61526414 255237255255 02390600\n"
        L"Ю12 00 61051066 255239255255 0240\n"
        L"Ю12 00 68693845 255237255255 02170608\n"
        L"Ю12 00 63241228 255237255255 02450600\n"
        L"Ю12 00 53730503 255237255255 02245608\n"
        L"Ю12 00 29003076 131225080059 0000000042103999900502541120021113400200001800043000000000025128\n"
        L"Ю12 00 52653698 129229080059 0500948075344448130004026530520612209490010900018648205091020132\n"
        L"Ю12 00 28063014 255237255255 02600208\n"
        L"Ю12 00 52653995 251237255255 00002655206\n"
        L"Ю12 00 54349295 133169092127 06009660003005814000202109659421050964042806806823\n"
        L"Ю12 00 54953971 191239255255 0610220\n"
        L"Ю12 00 94386018 255237255255 01950942\n"
        L"Ю12 00 42300236 191233255255 0600215400404\n"
        L"Ю12 00 42345306 255239255255 0207\n"
        L"Ю12 00 54328778 191233255255 0620210965942\n"
        L"Ю12 00 54346739 191255255255 060\n"
        L"Ю12 00 54305040 255255255255 \n"
        L"Ю12 00 54344932 191255255255 061\n"
        L"Ю12 00 50014729 161169124115 0627115234770670100026870570008618796101684OXPAHA148\n"
        L"Ю12 00 95165957 129233088119 069097400180346940000202329509501060970018000348500063020     \n"
        L"Ю12 00 73389033 129225084051 000000002140373570600002441171079108600710000300000000002511315 144\n"
        L"Ю12 00 72132608 255255255255 \n"
        L"Ю12 00 51419158 133225196003 06508190754113089010267207057000862008620081811310801702011080OXP*  148\n"
        L"Ю12 00 54028766 255239255255 0266\n"
        L"Ю12 00 54028261 255255255255 \n"
        L"Ю12 00 54029111 255255255255 \n"
        L"Ю12 00 54325360 129169125119 069082900030015403800002019996594210542003211315 \n"
        L"Ю12 00 54326947 255255255255 \n"
        L"Ю12 00 54356209 195247198071 0970000300729008030097000000001269607006000000     \n"
        L"Ю12 00 51638674 129169092119 05709000214037263060010002657057000860900003314907366OXP   \n"
        L"Ю12 00 53887956 131239216119 063096607115234773670266096400900018796101684OXPAHA\n"
        L"Ю12 00 53895710 255255255255 \n"
        L"Ю12 00 50973858 191239255255 0620267\n"
        L"Ю12 00 51719391 191239255255 0560265\n"
        L"Ю12 00 51704492 191239255255 0550251\n"
        L"Ю12 00 50113299 191239255255 0630267\n"
        L"Ю12 00 53894689 191239255255 0620266\n"
        L"Ю12 00 51728251 191239255255 0550265\n"
        L"Ю12 00 51728194 191255255255 056:)";

    {
        CPPUNIT_ASSERT(m_chart->processNewAsoupText( departure11310, boost::gregorian::date(2020, 9, 20), nullptr, 0) );
        m_chart->updateTime( time_from_iso("20200920T044700Z"), nullptr );
        m_chart->updateTime( time_from_iso("20200920T052000Z"), nullptr );
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
        auto pathSet = hl.GetReadAccess()->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size()==1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate->GetEventsCount()==9 );
        CPPUNIT_ASSERT( hl.exist_series( "name", "Station_exit Transition Transition Transition Transition Transition Transition Transition Span_move " ) );
    }

    {
    std::wstring spotStr = L"<SpotEvent create_time='20200920T054919Z' name='Info_changing' Bdg='P2PC[11401:11412]' index='' num='2407' through='Y' waynum='1' id='195' infsrc='disposition'>"
        L"<rwcoord picketing1_val='153~716' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='152~833' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>";

    CPPUNIT_ASSERT( m_chart->acceptEventFromStr(spotStr, trainID, TrainCharacteristics::Source::Guess, 0) );
    AsoupLayer& al = m_chart->getAsoupLayer();
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    auto asoup = pathAsoupList.front();
    CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    auto pathSet = hl.GetReadAccess()->getAllPathes();
    CPPUNIT_ASSERT( pathSet.size()==1 );
    auto pathCreate = *pathSet.cbegin();
    auto gis = pathCreate->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 2407 );
    }

    {
    m_chart->updateTime( time_from_iso("20200920T055000Z"), nullptr );
    AsoupLayer& al = m_chart->getAsoupLayer();
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    auto asoup = pathAsoupList.front();
    //АСОУП не должно подхватиться, хотя в нити нет отправления со станции, поскольку станция входит в EqualAreas
    CPPUNIT_ASSERT( al.IsServed( asoup ) );
    }

}

void TC_TrackedChart::DisformReserveLocoByAsoupArrival_5653_2()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20201122T122957Z' name='Form' Bdg='NRP#[11272]' index='0001-071-1131' num='4902' joincrgloc='Y'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<Locomotive Series='М62' NumLoc='1206' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GORBUNOVS' Tim_Beg='2020-11-22 13:54' />"
        L"</Locomotive>"
        L"<Locomotive Series='М62' NumLoc='1358' Consec='4' CarrierCode='4'>"
        L"<Crew EngineDriver='GREBENJUKS' Tim_Beg='2020-11-22 13:54' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201122T122957Z' name='Departure' Bdg='NRP#[11272]'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201122T122957Z' name='Span_stopping_begin' Bdg='PRPC[11272:11310]' waynum='1'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201122T123428Z' name='Span_stopping_end' Bdg='PRPC[11272:11310]' waynum='1'>"
        L"<rwcoord picketing1_val='224~100' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='223~900' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201122T123814Z' name='Arrival' Bdg='6C[11310]' waynum='6' parknum='1' />"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==5 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 4902 );
    }

    std::wstring asoup11310 =L"(:1042 909/000+11310 4902 0001 071 1131 01 11272 22 11 14 33 01/06 1 0/00 00 0:)";


    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11310,
        boost::gregorian::date(2020, 11, 22), nullptr, 0) );
	m_chart->updateTime( time_from_iso("20201122T123814Z"), nullptr);

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );

        attic::a_document doc("TC");
        attic::a_node root = doc.document_element();
        attic::a_node changes = root.append_child("Changes");
        attic::a_node analysis = root.append_child("Analysis");

        CPPUNIT_ASSERT(m_chart->takeChanges(changes, analysis, 0));
        CPPUNIT_ASSERT( analysis.child( "Disform" ).child("Train").attribute("id").as_int()==trainID);

    }
    set_TimeZone("");
}

void TC_TrackedChart::IdentifyOnEqualArea_6044()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    NsiBasis_helper();

    const std::wstring arrival9180 = L"(:1042 909/000+09180 3528 0918 096 0924 03 09181 14 11 13 43 03/25 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 024 01879 01365 088 44172054 90031998 000 000 00 022 000.20 002 000 000 40 002 000 000 60 011 000 000 90 007 000 000 92 001 000 000 95 006 000 000\n"
        L"Ю3 561 00059640 1 07 00 0000 00000 ABAKUMOVS   \n"
        L"Ю4 00000 1 01 022 000.20 002 000 40 002 000 60 011 000 90 007 000 92 001 000 95 006 000 \n"
        L"Ю4 09240 1 01 022 000.20 002 000 40 002 000 60 011 000 90 007 000 92 001 000 95 006 000 \n"
        L"Ю12 00 44172054 1 068 09240 25404 8496 000 00 00 00 00 00 0214 30 40 0404 04 105 09240 00000 00000 01 09180 19 00000 0000 021 0 0000 ПЛИTA  132 000 00000000\n"
        L"Ю12 00 44695328 255255255255 \n"
        L"Ю12 00 60822004 167233127247 0672540457540234600600100ГCИCПP\n"
        L"Ю12 00 64254469 255239255247 0239138507\n"
        L"Ю12 00 61652988 255239255255 0233\n"
        L"Ю12 00 60496569 255239255255 0234\n"
        L"Ю12 00 64417348 163239255247 02923402751440002410     \n"
        L"Ю12 00 63745236 191239255255 0280237\n"
        L"Ю12 00 64330376 187239255255 0310000238\n"
        L"Ю12 00 56722838 163237253191 0644361219343000214560817020\n"
        L"Ю12 00 56722648 255255255255 \n"
        L"Ю12 00 56722663 255255255255 \n"
        L"Ю12 00 59881706 161233125191 07001500584800002023695593510634021\n"
        L"Ю12 00 59895953 253255255255 04\n"
        L"Ю12 00 95875241 253237255255 0202330950\n"
        L"Ю12 00 29146883 163233125191 066231082430005026820021113519020\n"
        L"Ю12 00 29258597 255239255191 0271024\n"
        L"Ю12 00 55435457 161233125183 0644330271830000002406056001001702018/11 \n"
        L"Ю12 00 95349635 161233125247 0665422383980600202349509501063858147-\n"
        L"Ю12 00 95998217 191239255247 072023251545-\n"
        L"Ю12 00 59899120 163237253183 07001500584800002365935340210     \n"
        L"Ю12 00 90031998 165233253183 06743304718304022592090117024EDOK  :)";

    {
        CPPUNIT_ASSERT(m_chart->processNewAsoupText( arrival9180, boost::gregorian::date(2020, 11, 14), nullptr, 0) );
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20201114T114621Z' name='Form' Bdg='25C[09180]' index='' num='J3528' outbnd='Y' />"
        L"<SpotEvent create_time='20201114T115218Z' name='Transition' Bdg='3AC[09181]' waynum='3' parknum='1' optCode='09181:09251' />"
        L"<SpotEvent create_time='20201114T121053Z' name='Station_entry' Bdg='PDP[09251]'>"
        L"<rwcoord picketing1_val='177~99' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    unsigned const trainID = 64;
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==3 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size()==1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3528 );
        CPPUNIT_ASSERT( hl.exist_series( "name", "Form Transition Station_entry" ) );
    }

    m_chart->updateTime( time_from_iso("20201114T121100Z"), nullptr );

    {
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        //АСОУП должно подклеить к нити поскольку станция входит в EqualAreas.

		CPPUNIT_ASSERT( al.IsServed( asoup ) );
        CPPUNIT_ASSERT( hl.exist_series( "name", "Departure Transition Station_entry" ) );
        CPPUNIT_ASSERT( hl.exist_series( "create_time", "20201114T114300Z 20201114T115218Z 20201114T121053Z" ) );
    }

}

void TC_TrackedChart::CreatePathRezevLocoByAsoupDeparture_6038()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    NsiBasis_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20201114T195933Z' name='Form' Bdg='11JC[09006]' waynum='11' parknum='1' index='0900-544-0950' num='3632' length='54' weight='1788' net_weight='798' transfer='Y'>"
        L"<Locomotive Series='2М62У' NumLoc='8' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GULJTJAJEVS' Tim_Beg='2020-11-14 20:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201114T195933Z' name='Departure' Bdg='11JC[09006]' waynum='11' parknum='1' />"
        L"<SpotEvent create_time='20201114T201431Z' name='Arrival' Bdg='11C[09500]' waynum='11' parknum='1' />"
        L"<SpotEvent create_time='20201114T202935Z' name='Departure' Bdg='11C[09500]' waynum='11' parknum='1' />"
        L"<SpotEvent create_time='20201114T203125Z' name='Span_move' Bdg='2BGP[09042:09500]' waynum='2'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='Земитани' />"
        L"<rwcoord picketing1_val='4~894' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    unsigned const trainID = 64;
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==5 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size()==1 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3632 );
        CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Arrival Departure Span_move" ) );
    }

    const std::wstring arrival9500 = L"(:1042 909/000+09500 4830 0001 060 0964 03 09042 14 11 22 27 03/11 0 0/00 00 0\n"
        L"Ю3 579 00000081 1 20 00 0000 00000 GULJTJAJEVS \n"
        L"Ю3 579 00000082 9:)";

    {
        CPPUNIT_ASSERT(m_chart->processNewAsoupText( arrival9500, boost::gregorian::date(2020, 11, 14), nullptr, 0) );
		m_chart->updateTime( time_from_iso( "20201114T203200Z"), nullptr  );
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 2 );
        CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Arrival Death ") );
        CPPUNIT_ASSERT( hl.exist_series( "num",  "3632    -         -     -    ") );
        CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure  Span_move" ) );
        CPPUNIT_ASSERT( hl.exist_series( "num",  "4830        -        -     ") );
    }
}

void TC_TrackedChart::ChangeNumberThroughEqualOrPaireNumber_6052()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    NsiBasis_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20201110T000000Z' name='ExplicitForm' Bdg='ASOUP 1042[09630]' waynum='1' parknum='1' index='0963-032-1668' num='3603' length='55' weight='1020' transfer='Y'>"
        L"<Locomotive Series='2М62У' NumLoc='282' Consec='1' CarrierCode='3'>"
        L"<Crew EngineDriver='VILCANS' Tim_Beg='2020-11-09 20:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201110T024300Z' name='Departure' Bdg='ASOUP 1042_1p1w[09630]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20201110T025053Z' name='Transition' Bdg='?p1w1[09640]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20201110T025825Z' name='Transition' Bdg='?p1w1[09650]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20201110T030204Z' name='Transition' Bdg='7SP[09042]'>"
        L"<rwcoord picketing1_val='5~857' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201110T030727Z' name='Transition' Bdg='4C[09500]' waynum='4' parknum='1' intervalSec='4' />"
        L"<SpotEvent create_time='20201110T031946Z' name='Arrival' Bdg='4JC[09006]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20201110T115310Z' name='Departure' Bdg='4JC[09006]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20201110T115832Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' intervalSec='23' />"
        L"<SpotEvent create_time='20201110T120109Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='21' optCode='09000:09420' index='0963-032-1668' num='3604' length='55' weight='1020' transfer='Y'>"
        L"<Locomotive Series='2М62У' NumLoc='282' Consec='1' CarrierCode='3'>"
        L"<Crew EngineDriver='VILCANS' Tim_Beg='2020-11-09 20:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201110T121332Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' intervalSec='17' />"
        L"<SpotEvent create_time='20201110T123209Z' name='Transition' Bdg='1C[09410]' waynum='1' parknum='1' intervalSec='3' />"
        L"</HemPath>"
        );

    unsigned const trainID = 64;
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==12 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size()==2 );
        CPPUNIT_ASSERT( gis.front().second->GetNumber().getNumber() == 3603 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3604 );
    }

    const std::wstring departure9006 = L"(:1042 909/000+09000 2730 0963 032 1668 03 11760 10 11 13 58 01/02 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01066 00000 144 65737488 91599019 000 000 00 000 036.60 000 002 000 90 000 034 000\n"
        L"Ю3 530 00006901 1 12 37 0000 00000 AUGSNE      \n"
        L"Ю3 530 00006902 9\n"
        L"Ю4 00000 0 00 000 002.60 000 002 \n"
        L"Ю4 16169 2 13 000 034.90 000 034 \n"
        L"Ю12 00 65737488 1 000 00000 10307 9944 000 00 00 00 00 00 0223 11 60 0600 04 100 16680 00600 00000 00 09630 24 00000 0000 021 0 0000 098704 144 000 00000000\n"
        L"Ю12 00 63185250 255239255255 0236\n"
        L"Ю12 00 98044167 195225071131 166801030706533050300129009341600000016169130261102009634 128\n"
        L"Ю12 00 98043805 255255255255 \n"
        L"Ю12 00 98043797 255255255255 \n"
        L"Ю12 00 98044217 255255255255 \n"
        L"Ю12 00 98043789 255255255255 \n"
        L"Ю12 00 98044316 255255255255 \n"
        L"Ю12 00 98044126 255255255255 \n"
        L"Ю12 00 98044159 255255255255 \n"
        L"Ю12 00 98044134 255255255255 \n"
        L"Ю12 00 98043755 255255255255 \n"
        L"Ю12 00 98044175 255255255255 \n"
        L"Ю12 00 98043698 255255255255 \n"
        L"Ю12 00 98044183 255255255255 \n"
        L"Ю12 00 98044001 255255255255 \n"
        L"Ю12 00 98044266 255255255255 \n"
        L"Ю12 00 91599035 255255255255 \n"
        L"Ю12 00 98044118 255255255255 \n"
        L"Ю12 00 98044035 255255255255 \n"
        L"Ю12 00 98044241 255255255255 \n"
        L"Ю12 00 91598342 255255255255 \n"
        L"Ю12 00 98044274 255255255255 \n"
        L"Ю12 00 91598367 255255255255 \n"
        L"Ю12 00 91599209 255255255255 \n"
        L"Ю12 00 91599316 255255255255 \n"
        L"Ю12 00 91599183 255255255255 \n"
        L"Ю12 00 91599027 255255255255 \n"
        L"Ю12 00 98043813 255255255255 \n"
        L"Ю12 00 98044142 255255255255 \n"
        L"Ю12 00 98044209 255255255255 \n"
        L"Ю12 00 98044233 255255255255 \n"
        L"Ю12 00 98043946 255255255255 \n"
        L"Ю12 00 91598763 255255255255 \n"
        L"Ю12 00 98043987 255255255255 \n"
        L"Ю12 00 91599019 255255255255 :)";

    {
        CPPUNIT_ASSERT(m_chart->processNewAsoupText( departure9006, boost::gregorian::date(2020, 11, 10), nullptr, 0) );
		m_chart->updateTime( time_from_iso( "20201110T120000Z"), nullptr  );
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size()==2 );
        CPPUNIT_ASSERT( gis.front().second->GetNumber().getNumber() == 3603 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 2730 );
    }
}


void TC_TrackedChart::ErrorIdentificationOnSkirotava_6058()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    NsiBasis_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20201206T021314Z' name='Form' Bdg='1BP[09042:09650]' waynum='1'>"
        L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"<rwcoord picketing1_val='6~961' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201206T021436Z' name='Transition' Bdg='7SP[09042]'>"
        L"<rwcoord picketing1_val='5~857' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201206T021928Z' name='Transition' Bdg='1C[09500]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20201206T023231Z' name='Transition' Bdg='3JC[09006]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );

    const std::wstring departure9006 =L"(:1042 909/000+09006 2710 0900 815 1131 03 09000 06 12 04 35 01/05 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 01417 00118 220 64303712 50186501 000 000 00 002 053.60 000 006 000 70 002 008 000 90 000 039 000 95 000 039 000\n"
        L"Ю3 583 00001101 1 02 58 0000 00000 HARUNS      \n"
        L"Ю3 583 00001102 9\n"
        L"Ю4 11290 2 28 000 006.60 000 006 \n"
        L"Ю4 11290 2 17 000 010.70 000 001 90 000 009 95 000 009 \n"
        L"Ю4 11290 2 24 000 006.70 000 004 90 000 002 95 000 002 \n"
        L"Ю4 11290 2 58 000 017.70 000 002 90 000 015 95 000 015 \n"
        L"Ю4 11290 2 83 000 003.70 000 001 90 000 002 95 000 002 \n"
        L"Ю4 11290 2 61 000 003.90 000 003 95 000 003 \n"
        L"Ю4 11290 2 63 000 004.90 000 004 95 000 004 \n"
        L"Ю4 00000 1 02 002 004.70 002 000 90 000 004 95 000 004 \n"
        L"Ю4 11310 1 02 002 004.70 002 000 90 000 004 95 000 004 \n"
        L"Ю12 00 64303712 1 000 30260 43619 4862 500 00 00 00 00 00 0245 12 60 0600 04 100 11310 18000 11290 28 09000 17 00000 0000 020 1 1280 09189  128 000 00000000\n"
        L"Ю12 00 64023716 255255255255 \n"
        L"Ю12 00 64336753 255255255255 \n"
        L"Ю12 00 63244420 255239255255 0240\n"
        L"Ю12 00 64235179 255239255255 0245\n"
        L"Ю12 00 63641617 255239255255 0238\n"
        L"Ю12 00 59094235 195233117247 613305422361010600235955935106613809247 \n"
        L"Ю12 00 95526109 199237247255 646205422374170232095063\n"
        L"Ю12 00 95346037 199239247255 25060542232885023424\n"
        L"Ю12 00 59891317 255237255255 02365935\n"
        L"Ю12 00 58566704 199239247255 58140542234504023558\n"
        L"Ю12 00 59826248 195239247247 63910542232885300023363ARENDA\n"
        L"Ю12 00 59501874 255255255255 \n"
        L"Ю12 00 59503706 255239255255 0234\n"
        L"Ю12 00 95232708 195237247255 600305420753804000231095058\n"
        L"Ю12 00 95454682 255239255255 0235\n"
        L"Ю12 00 95419636 255255255255 \n"
        L"Ю12 00 95321089 195239247255 60880542075739500023361\n"
        L"Ю12 00 95317582 255255255255 \n"
        L"Ю12 00 95904116 195239247255 59770542076302000023258\n"
        L"Ю12 00 95822649 223255247255 8462083\n"
        L"Ю12 00 95899050 223255247255 5977058\n"
        L"Ю12 00 95888657 223255247255 8462083\n"
        L"Ю12 00 95319687 195239247255 60030542185380500023358\n"
        L"Ю12 00 95227112 255255255255 \n"
        L"Ю12 00 95619094 251239255255 0000232\n"
        L"Ю12 00 50590074 193233117247 22000594020186060010230705700086173009500 \n"
        L"Ю12 00 51161255 199239247255 60230594029509023558\n"
        L"Ю12 00 51375244 255239255255 0278\n"
        L"Ю12 00 95466454 193233117247 21680011001552500000229950950106173409634 \n"
        L"Ю12 00 95348033 255239255255 0232\n"
        L"Ю12 00 95895165 195239255255 225300110063520000230\n"
        L"Ю12 00 95813374 223239255255 214700231\n"
        L"Ю12 00 95589610 195239255255 216800110015525000232\n"
        L"Ю12 00 95595039 255255255255 \n"
        L"Ю12 00 95498952 255255255255 \n"
        L"Ю12 00 95581633 255255255255 \n"
        L"Ю12 00 95604625 195239127255 217300110068410000236102\n"
        L"Ю12 00 95089629 199239117247 592505420760580235106583809487-\n"
        L"Ю12 00 95818175 195239255255 594605420746194000232\n"
        L"Ю12 00 95746509 255255255255 \n"
        L"Ю12 00 95464418 195255255255 43740542070218500\n"
        L"Ю12 00 95463741 255255255255 \n"
        L"Ю12 00 95464566 255255255255 \n"
        L"Ю12 00 50275353 193233117247 83150421005852000010273705700086834309665-\n"
        L"Ю12 00 76037183 195237245255 26960482147006360024607602418\n"
        L"Ю12 00 76036904 255255255255 \n"
        L"Ю12 00 76036920 255255255255 \n"
        L"Ю12 00 76037050 255239255255 0245\n"
        L"Ю12 00 95486460 193225069135 1131001100150200000022613950950106113100000002340250000009634 \n"
        L"Ю12 00 95459871 255239255247 0215REGUL_\n"
        L"Ю12 00 95048310 255239255247 022009634 \n"
        L"Ю12 00 95573960 255255255255 \n"
        L"Ю12 00 51629285 161225125243 0632140663966600102683070570008603OXP   132\n"
        L"Ю12 00 50186501 191239255191 0550232024:)";

    {
        CPPUNIT_ASSERT(m_chart->processNewAsoupText( departure9006, boost::gregorian::date(2020, 12, 4), nullptr, 0) );
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    }

    unsigned const trainID = 64;
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    {
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==4 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.empty() );
    }

    m_chart->updateTime(time_from_iso("20201206T025000Z"), nullptr );

    {
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==4 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.empty() );
    }
}

void TC_TrackedChart::RemoveLocomotivesByAsoupWithdrawal_6052()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    NsiBasis_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20210128T222922Z' name='Form' Bdg='5C[11310]' waynum='5' parknum='1' index='' num='J2001' />"
        L"<SpotEvent create_time='20210128T222922Z' name='Departure' Bdg='5C[11310]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20210128T223455Z' name='Transition' Bdg='701SP[11321]'>"
        L"<rwcoord picketing1_val='222~474' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210128T225020Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' optCode='11311:11380' />"
        L"<SpotEvent create_time='20210128T230447Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' optCode='11380:11381' />"
        L"<SpotEvent create_time='20210128T231546Z' name='Transition' Bdg='1C[11381]' waynum='1' parknum='1' optCode='11381:11390' />"
        L"<SpotEvent create_time='20210128T232354Z' name='Transition' Bdg='1C[11390]' waynum='1' parknum='1' optCode='11390:11391' />"
        L"<SpotEvent create_time='20210128T234248Z' name='Transition' Bdg='1C[11391]' waynum='1' parknum='1' optCode='11391:11401' />"
        L"<SpotEvent create_time='20210128T235517Z' name='Transition' Bdg='1C[11401]' waynum='1' parknum='1' optCode='11401:11412' />"
        L"<SpotEvent create_time='20210129T001142Z' name='Transition' Bdg='1C[11412]' waynum='1' parknum='1' optCode='11412:11420' />"
        L"<SpotEvent create_time='20210129T003221Z' name='Transition' Bdg='3C[11420]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20210129T004930Z' name='Transition' Bdg='1C[09190]' waynum='1' parknum='1' optCode='09190:09191' />"
        L"<SpotEvent create_time='20210129T005946Z' name='Transition' Bdg='1C[09191]' waynum='1' parknum='1' optCode='09191:09193' />"
        L"<SpotEvent create_time='20210129T011806Z' name='Transition' Bdg='1C[09193]' waynum='1' parknum='1' optCode='09193:09200' />"
        L"<SpotEvent create_time='20210129T012639Z' name='Transition' Bdg='1C[09200]' waynum='1' parknum='1' optCode='09200:09202' />"
        L"<SpotEvent create_time='20210129T013846Z' name='Transition' Bdg='1C[09202]' waynum='1' parknum='1' optCode='09202:09210' />"
        L"<SpotEvent create_time='20210129T015114Z' name='Transition' Bdg='1C[09210]' waynum='1' parknum='1' optCode='09210:09211' />"
        L"<SpotEvent create_time='20210129T020424Z' name='Transition' Bdg='1C[09211]' waynum='1' parknum='1' optCode='09211:09220' />"
        L"<SpotEvent create_time='20210129T021933Z' name='Transition' Bdg='1C[09220]' waynum='1' parknum='1' optCode='09220:09230' />"
        L"<SpotEvent create_time='20210129T023133Z' name='Transition' Bdg='1aC[09230]' waynum='1' parknum='1' optCode='09230:09240' />"
        L"<SpotEvent create_time='20210129T024910Z' name='Transition' Bdg='1C[09240]' waynum='1' parknum='1' optCode='09240:09241' />"
        L"<SpotEvent create_time='20210129T030142Z' name='Transition' Bdg='1C[09241]' waynum='1' parknum='1' optCode='09241:09251' />"
        L"<SpotEvent create_time='20210129T031933Z' name='Arrival' Bdg='2C[09251]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210129T032337Z' name='Departure' Bdg='2C[09251]' waynum='2' parknum='1' optCode='09181:09251' />"
        L"<SpotEvent create_time='20210129T034438Z' name='Transition' Bdg='3AC[09181]' waynum='3' parknum='1' optCode='09180:09764' />"
        L"<SpotEvent create_time='20210129T035040Z' name='Arrival' Bdg='5C[09180]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20210129T035109Z' name='Departure' Bdg='5C[09180]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20210129T041946Z' name='Transition' Bdg='1C[09764]' waynum='1' parknum='1' optCode='09764:09772' index='' num='V2001' />"
        L"<SpotEvent create_time='20210129T044758Z' name='Transition' Bdg='1C[09772]' waynum='1' parknum='1' optCode='09750:09772' />"
        L"<SpotEvent create_time='20210129T051508Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20210129T053124Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' optCode='09751:09780' />"
        L"<SpotEvent create_time='20210129T054732Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' optCode='09780:09790' />"
        L"<SpotEvent create_time='20210129T060650Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210129T062058Z' name='Transition' Bdg='2C[09800]' waynum='2' parknum='1' optCode='09800:09801' />"
        L"<SpotEvent create_time='20210129T063101Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' optCode='09801:09802' />"
        L"<SpotEvent create_time='20210129T064443Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' optCode='09802:09803' />"
        L"<SpotEvent create_time='20210129T065524Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' optCode='09803:09810' />"
        L"<SpotEvent create_time='20210129T070757Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' optCode='09810:09812' />"
        L"<SpotEvent create_time='20210129T073010Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' optCode='09812:09813' />"
        L"<SpotEvent create_time='20210129T073928Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' optCode='09813:09820' />"
        L"<SpotEvent create_time='20210129T075300Z' name='Arrival' Bdg='ASOUP 1042_3p2w[09820]' waynum='2' parknum='3' />"
        L"<SpotEvent create_time='20210129T075300Z' name='Death' Bdg='ASOUP 1042_3p2w[09820]' waynum='2' parknum='3' />"
        L"</HemPath>");

    CPPUNIT_ASSERT( CreatePath(*m_chart, path, 1) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==42 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
    }

    const std::wstring aInfo_11310 = L"(:1042 909/000+11310 2001 2119 949 0983 02 11420 29 01 00 01 01/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05336 04002 228 58885021 53018701 000 000 00 057 000.60 051 000 000 90 006 000 000 92 006 000 000\n"
        L"Ю4 00000 1 01 057 000.60 051 000 90 006 000 92 006 000 \n"
        L"Ю4 09830 1 01 057 000.60 051 000 90 006 000 92 006 000 :)";


    const std::wstring aDeparture_11310 = L"(:1042 909/000+11310 2001 2119 949 0983 03 11420 29 01 00 28 01/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05336 04002 228 58885021 53018701 000 000 00 057 000.60 051 000 000 90 006 000 000 92 006 000 000\n"
        L"Ю3 530 00010421 1 23 16 0000 00000 PAVLUKEVICS \n"
        L"Ю3 530 00010422 9\n"
        L"Ю4 00000 1 01 057 000.60 051 000 90 006 000 92 006 000 \n"
        L"Ю4 09830 1 01 057 000.60 051 000 90 006 000 92 006 000 \n"
        L"Ю12 00 58885021 1 069 09830 43612 2102 300 01 00 00 00 00 0234 30 92 5904 04 095 09830 09820 00000 01 21190 17 19760 6302 020 0 0000 0      148 000 00000000\n"
        L"Ю12 00 58878703 255239255255 0235\n"
        L"Ю12 00 58874231 255255255255 \n"
        L"Ю12 00 58885088 255239255255 0234\n"
        L"Ю12 00 58871377 255255255255 \n"
        L"Ю12 00 58884503 255255255255 \n"
        L"Ю12 00 61048328 161233124113 07031106137920000023960060010009211903437211927132032\n"
        L"Ю12 00 53741880 191237255245 07102245608Э     000\n"
        L"Ю12 00 53016747 255255255255 \n"
        L"Ю12 00 52994274 191237255255 06902385600\n"
        L"Ю12 00 56910615 191239255255 0700242\n"
        L"Ю12 00 55138127 255239255255 0237\n"
        L"Ю12 00 62409560 191237255255 07502470600\n"
        L"Ю12 00 57400640 191237255255 07002405600\n"
        L"Ю12 00 52758679 255239255255 0235\n"
        L"Ю12 00 68422468 191237255255 07102250608\n"
        L"Ю12 00 68192772 255255255255 \n"
        L"Ю12 00 53743761 255237255255 02245608\n"
        L"Ю12 00 53749370 255255255255 \n"
        L"Ю12 00 62206933 191237255255 07002360600\n"
        L"Ю12 00 53730339 191237255255 07102245608\n"
        L"Ю12 00 52011319 255255255255 \n"
        L"Ю12 00 55591390 191237255255 07002335600\n"
        L"Ю12 00 53749644 191237255255 07102245608\n"
        L"Ю12 00 57418709 191237255255 07002375600\n"
        L"Ю12 00 52900453 191239255255 0690238\n"
        L"Ю12 00 54123203 191239255255 0700237\n"
        L"Ю12 00 54007778 255239255255 0236\n"
        L"Ю12 00 55726269 255239255255 0237\n"
        L"Ю12 00 60084027 255237255255 02400600\n"
        L"Ю12 00 62314828 255239255255 0238\n"
        L"Ю12 00 60491040 255239255255 0243\n"
        L"Ю12 00 68015858 191237255255 07102240608\n"
        L"Ю12 00 61058202 191237255255 07002410600\n"
        L"Ю12 00 53095634 255237255255 02325600\n"
        L"Ю12 00 55038574 255239255255 0243\n"
        L"Ю12 00 53731279 191237255255 07102245608\n"
        L"Ю12 00 53015095 255255255255 \n"
        L"Ю12 00 61440822 191237255255 07002360600\n"
        L"Ю12 00 52344892 255237255255 02315600\n"
        L"Ю12 00 58656430 255239255255 0240\n"
        L"Ю12 00 56920101 255239255255 0243\n"
        L"Ю12 00 55938252 255239255255 0236\n"
        L"Ю12 00 56920572 255239255255 0243\n"
        L"Ю12 00 61394748 255237255255 02350600\n"
        L"Ю12 00 56967524 255253255255 5600\n"
        L"Ю12 00 53732632 191237255255 07102245608\n"
        L"Ю12 00 55594899 191237255255 07002335600\n"
        L"Ю12 00 53610358 255255255255 \n"
        L"Ю12 00 58753716 255239255255 0239\n"
        L"Ю12 00 54061452 255239255255 0232\n"
        L"Ю12 00 61325403 255237255255 02400600\n"
        L"Ю12 00 54904784 255237255255 02445600\n"
        L"Ю12 00 53084760 255239255255 0240\n"
        L"Ю12 00 52356219 255255255255 \n"
        L"Ю12 00 53730180 191237255255 07102245608\n"
        L"Ю12 00 53018701 255255255255 :)";

    const std::wstring aTransition_11420 = L"(:1042 909/000+11420 2001 2119 949 0983 04 11310+09180 29 01 02 32 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05336 04002 228 58885021 53018701 000 000 00 057 000.60 051 000 000 90 006 000 000 92 006 000 000\n"
        L"Ю3 530 00010421 1 23 16 0000 00000 PAVLUKEVICS \n"
        L"Ю3 530 00010422 9\n"
        L"Ю4 00000 1 01 057 000.60 051 000 90 006 000 92 006 000 \n"
        L"Ю4 09830 1 01 057 000.60 051 000 90 006 000 92 006 000 \n"
        L"Ю12 00 58885021 1 069 09830 43612 2102 300 01 00 00 00 00 0234 30 92 5904 04 095 09830 09820 00000 01 21190 17 19760 6302 020 0 0000 0      148 000 00000000\n"
        L"Ю12 00 58878703 255239255255 0235\n"
        L"Ю12 00 58874231 255255255255 \n"
        L"Ю12 00 58885088 255239255255 0234\n"
        L"Ю12 00 58871377 255255255255 \n"
        L"Ю12 00 58884503 255255255255 \n"
        L"Ю12 00 61048328 161233124113 07031106137920000023960060010009211903437211927132032\n"
        L"Ю12 00 53741880 191237255245 07102245608Э     000\n"
        L"Ю12 00 53016747 255255255255 \n"
        L"Ю12 00 52994274 191237255255 06902385600\n"
        L"Ю12 00 56910615 191239255255 0700242\n"
        L"Ю12 00 55138127 255239255255 0237\n"
        L"Ю12 00 62409560 191237255255 07502470600\n"
        L"Ю12 00 57400640 191237255255 07002405600\n"
        L"Ю12 00 52758679 255239255255 0235\n"
        L"Ю12 00 68422468 191237255255 07102250608\n"
        L"Ю12 00 68192772 255255255255 \n"
        L"Ю12 00 53743761 255237255255 02245608\n"
        L"Ю12 00 53749370 255255255255 \n"
        L"Ю12 00 62206933 191237255255 07002360600\n"
        L"Ю12 00 53730339 191237255255 07102245608\n"
        L"Ю12 00 52011319 255255255255 \n"
        L"Ю12 00 55591390 191237255255 07002335600\n"
        L"Ю12 00 53749644 191237255255 07102245608\n"
        L"Ю12 00 57418709 191237255255 07002375600\n"
        L"Ю12 00 52900453 191239255255 0690238\n"
        L"Ю12 00 54123203 191239255255 0700237\n"
        L"Ю12 00 54007778 255239255255 0236\n"
        L"Ю12 00 55726269 255239255255 0237\n"
        L"Ю12 00 60084027 255237255255 02400600\n"
        L"Ю12 00 62314828 255239255255 0238\n"
        L"Ю12 00 60491040 255239255255 0243\n"
        L"Ю12 00 68015858 191237255255 07102240608\n"
        L"Ю12 00 61058202 191237255255 07002410600\n"
        L"Ю12 00 53095634 255237255255 02325600\n"
        L"Ю12 00 55038574 255239255255 0243\n"
        L"Ю12 00 53731279 191237255255 07102245608\n"
        L"Ю12 00 53015095 255255255255 \n"
        L"Ю12 00 61440822 191237255255 07002360600\n"
        L"Ю12 00 52344892 255237255255 02315600\n"
        L"Ю12 00 58656430 255239255255 0240\n"
        L"Ю12 00 56920101 255239255255 0243\n"
        L"Ю12 00 55938252 255239255255 0236\n"
        L"Ю12 00 56920572 255239255255 0243\n"
        L"Ю12 00 61394748 255237255255 02350600\n"
        L"Ю12 00 56967524 255253255255 5600\n"
        L"Ю12 00 53732632 191237255255 07102245608\n"
        L"Ю12 00 55594899 191237255255 07002335600\n"
        L"Ю12 00 53610358 255255255255 \n"
        L"Ю12 00 58753716 255239255255 0239\n"
        L"Ю12 00 54061452 255239255255 0232\n"
        L"Ю12 00 61325403 255237255255 02400600\n"
        L"Ю12 00 54904784 255237255255 02445600\n"
        L"Ю12 00 53084760 255239255255 0240\n"
        L"Ю12 00 52356219 255255255255 \n"
        L"Ю12 00 53730180 191237255255 07102245608\n"
        L"Ю12 00 53018701 255255255255 :)";

    const std::wstring aTransition_9180 = L"(:1042 909/000+09180 2001 2119 949 0983 04 11420+09820 29 01 05 50 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05336 04002 228 58885021 53018701 000 000 00 057 000.60 051 000 000 90 006 000 000 92 006 000 000\n"
        L"Ю3 530 00010421 1 23 16 0000 00000 PAVLUKEVICS \n"
        L"Ю3 530 00010422 9\n"
        L"Ю4 00000 1 01 057 000.60 051 000 90 006 000 92 006 000 \n"
        L"Ю4 09830 1 01 057 000.60 051 000 90 006 000 92 006 000 \n"
        L"Ю12 00 58885021 1 069 09830 43612 2102 300 01 00 00 00 00 0234 30 92 5904 04 095 09830 09820 00000 01 21190 17 19760 6302 020 0 0000 0      148 000 00000000\n"
        L"Ю12 00 58878703 255239255255 0235\n"
        L"Ю12 00 58874231 255255255255 \n"
        L"Ю12 00 58885088 255239255255 0234\n"
        L"Ю12 00 58871377 255255255255 \n"
        L"Ю12 00 58884503 255255255255 \n"
        L"Ю12 00 61048328 161233124113 07031106137920000023960060010009211903437211927132032\n"
        L"Ю12 00 53741880 191237255245 07102245608Э     000\n"
        L"Ю12 00 53016747 255255255255 \n"
        L"Ю12 00 52994274 191237255255 06902385600\n"
        L"Ю12 00 56910615 191239255255 0700242\n"
        L"Ю12 00 55138127 255239255255 0237\n"
        L"Ю12 00 62409560 191237255255 07502470600\n"
        L"Ю12 00 57400640 191237255255 07002405600\n"
        L"Ю12 00 52758679 255239255255 0235\n"
        L"Ю12 00 68422468 191237255255 07102250608\n"
        L"Ю12 00 68192772 255255255255 \n"
        L"Ю12 00 53743761 255237255255 02245608\n"
        L"Ю12 00 53749370 255255255255 \n"
        L"Ю12 00 62206933 191237255255 07002360600\n"
        L"Ю12 00 53730339 191237255255 07102245608\n"
        L"Ю12 00 52011319 255255255255 \n"
        L"Ю12 00 55591390 191237255255 07002335600\n"
        L"Ю12 00 53749644 191237255255 07102245608\n"
        L"Ю12 00 57418709 191237255255 07002375600\n"
        L"Ю12 00 52900453 191239255255 0690238\n"
        L"Ю12 00 54123203 191239255255 0700237\n"
        L"Ю12 00 54007778 255239255255 0236\n"
        L"Ю12 00 55726269 255239255255 0237\n"
        L"Ю12 00 60084027 255237255255 02400600\n"
        L"Ю12 00 62314828 255239255255 0238\n"
        L"Ю12 00 60491040 255239255255 0243\n"
        L"Ю12 00 68015858 191237255255 07102240608\n"
        L"Ю12 00 61058202 191237255255 07002410600\n"
        L"Ю12 00 53095634 255237255255 02325600\n"
        L"Ю12 00 55038574 255239255255 0243\n"
        L"Ю12 00 53731279 191237255255 07102245608\n"
        L"Ю12 00 53015095 255255255255 \n"
        L"Ю12 00 61440822 191237255255 07002360600\n"
        L"Ю12 00 52344892 255237255255 02315600\n"
        L"Ю12 00 58656430 255239255255 0240\n"
        L"Ю12 00 56920101 255239255255 0243\n"
        L"Ю12 00 55938252 255239255255 0236\n"
        L"Ю12 00 56920572 255239255255 0243\n"
        L"Ю12 00 61394748 255237255255 02350600\n"
        L"Ю12 00 56967524 255253255255 5600\n"
        L"Ю12 00 53732632 191237255255 07102245608\n"
        L"Ю12 00 55594899 191237255255 07002335600\n"
        L"Ю12 00 53610358 255255255255 \n"
        L"Ю12 00 58753716 255239255255 0239\n"
        L"Ю12 00 54061452 255239255255 0232\n"
        L"Ю12 00 61325403 255237255255 02400600\n"
        L"Ю12 00 54904784 255237255255 02445600\n"
        L"Ю12 00 53084760 255239255255 0240\n"
        L"Ю12 00 52356219 255255255255 \n"
        L"Ю12 00 53730180 191237255255 07102245608\n"
        L"Ю12 00 53018701 255255255255 :)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText( aInfo_11310, boost::gregorian::date(2021, 01, 29), nullptr, 0) );
    CPPUNIT_ASSERT(m_chart->processNewAsoupText( aDeparture_11310, boost::gregorian::date(2021, 01, 29), nullptr, 0) );
    CPPUNIT_ASSERT(m_chart->processNewAsoupText( aTransition_11420, boost::gregorian::date(2021, 01, 29), nullptr, 0) );
    CPPUNIT_ASSERT(m_chart->processNewAsoupText( aTransition_9180, boost::gregorian::date(2021, 01, 29), nullptr, 0) );
	//первый временной шаг - для привязки сообщений по станциям 11310, 11420
	m_chart->updateTime( time_from_iso( "20210129T003000Z"), nullptr  );
	//второй временной шаг - для привязки сообщений по станциям 9180
	m_chart->updateTime( time_from_iso( "20210129T033500Z"), nullptr  );

    {
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 4 );
        for ( auto asoup : pathAsoupList )
        {
            CPPUNIT_ASSERT( al.IsServed( asoup ) );
        }

        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
        CPPUNIT_ASSERT( gis.front().second->get_loks_number(',')==L"1042" );
        CPPUNIT_ASSERT( gis.back().second->get_loks_number(',')==L"1042" );
    }

    const std::wstring aRemove_9180 = L"(:333 1042 909/000+09180 2001 2119 949 0983 04 11420+09820 29 01 05 50 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05336 04002 228 58885021 53018701 000 000 00 057 000.60 051 000 000 90 006 000 000 92 006 000 000:)";

    const std::wstring aRemove_11420 = L"(:333 1042 909/000+11420 2001 2119 949 0983 04 11310+09180 29 01 02 32 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05336 04002 228 58885021 53018701 000 000 00 057 000.60 051 000 000 90 006 000 000 92 006 000 000:)";

    const std::wstring aRemove_11310 = L"(:333 1042 909/000+11310 2001 2119 949 0983 03 11420 29 01 00 28 01/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 057 05336 04002 228 58885021 53018701 000 000 00 057 000.60 051 000 000 90 006 000 000 92 006 000 000:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText( aRemove_9180, boost::gregorian::date(2021, 01, 29), nullptr, 0) );
    CPPUNIT_ASSERT(m_chart->processNewAsoupText( aRemove_11420, boost::gregorian::date(2021, 01, 29), nullptr, 0) );
    CPPUNIT_ASSERT(m_chart->processNewAsoupText( aRemove_11310, boost::gregorian::date(2021, 01, 29), nullptr, 0) );

    {
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );

        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
        CPPUNIT_ASSERT( gis.front().second->get_loks_number(',')==L"" );
        CPPUNIT_ASSERT( gis.back().second->get_loks_number(',')==L"" );
    }

}

// void TC_TrackedChart::LoadHappenFromFileForStrangeError()
// {
// //    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
// //    setupEsrGuide();
//     Uncontrolled_helper();
//     attic::a_document adoc;
//     adoc.load_file(L"C:\\ERR\\Test\\222.hef");
// 
//     m_chart->deserialize( adoc.document_element() );
// 
//     UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
// 
// 	CPPUNIT_ASSERT(PassGuiRawText(*m_chart, L"(:0001 28000'BALAK_DSP':20 40317 :5 45059 17288 30013 2552 31330 10 31342 222178408 1 2 0 0 6525 71 П2 Z яв 09-25  S7630 F00232 "));
// 	{
// 		HappenLayer& hl = m_chart->getHappenLayer();
// 		HappenLayer::ReadAccessor container = hl.GetReadAccess();
// 		auto pathSet = container->getAllPathes();
// 		CPPUNIT_ASSERT( pathSet.size() == 1 );
// 		auto pathCreate = *pathSet.cbegin();
// 		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==1 );
// 		auto gis = pathCreate->GetInfoSpots();
// 		CPPUNIT_ASSERT( !gis.empty() );
// 		CPPUNIT_ASSERT(gis.front().second->GetNumber().getNumber() == 2552);
// 	}
// 
//     Hem::Bill bill = createBill(
//         L"<A2F_HINT Hint='cmd_edit' issue_moment='20201109T134458Z'>"
//         L"<PathEvent name='Transition' Bdg='15AC[09000]' create_time='20201109T134345Z' waynum='15' parknum='1' />"
//         L"<Action code='Identify' District='09000-11760' DistrictRight='Y'>"
//         L"<EventInfo layer='asoup' Bdg='ASOUP 1042_1p5w[09410]' name='Departure' create_time='20201109T131700Z' index='0001-018-0910' num='8215' stone='Y' waynum='5' parknum='1'>"
//         L"<![CDATA[(:1042 909/000+09410 8215 0001 018 0910 03 09000 09 11 15 17 01/05 0 0/00 00 0\n"
//         L"Ю3 927 00008250 1 08 00 0000 00000 CELEJS      :)]]>"
//         L"<Locomotive Series='USP2005' NumLoc='825' Consec='1' CarrierCode='5'>"
//         L"<Crew EngineDriver='CELEJS' Tim_Beg='2020-11-09 08:00' />"
//         L"</Locomotive>"
//         L"</EventInfo>"
//         L"</Action>"
//         L"</A2F_HINT>"
//         );
// 
//     std::function<bool(const EsrKit&, const ParkWayKit&)> fIsPassengerWay = [](const EsrKit&, const ParkWayKit&) -> bool {
//         return true;
//     };
//     Hem::AccessHappenAsoupRegulatory access(hl.base(), m_chart->getAsoupLayer(), m_chart->getRegulatoryLayer());
//     Context context(bill.getChance(), m_chart->getEsrGuide(), m_chart->getGuessTransciever(), m_chart->getTopology(), nullptr, 0); 
//     Hem::aeUserIdentify applier(std::move(access), context, bill, fIsPassengerWay);
//     CPPUNIT_ASSERT_NO_THROW(applier.Action());
// 
// }


void TC_TrackedChart::IdentifyByArrival_6182()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    Uncontrolled_helper();
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20210310T200008Z' name='Form' Bdg='P2P_ER[11470:12780]' waynum='1' index='1252-093-0734' num='2356' length='56' weight='2403' through='Y'>"
        L"<rwcoord picketing1_val='171~' picketing1_comm='Рокишки' />"
        L"<rwcoord picketing1_val='165~' picketing1_comm='Рокишки' />"
        L"<Locomotive Series='ER20CF' NumLoc='16' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='KAIRYS' Tim_Beg='2021-03-10 17:50' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210310T200115Z' name='Transition' Bdg='1C_[11470]' waynum='1' parknum='1' optCode='11460:11470' />"
        L"<SpotEvent create_time='20210310T200624Z' name='Transition' Bdg='1C[11460]' waynum='1' parknum='1' optCode='11074:11460' />"
        L"<SpotEvent create_time='20210310T201639Z' name='Transition' Bdg='2SP:2+[11074]'>"
        L"<rwcoord picketing1_val='190~159' picketing1_comm='Рокишки' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210310T201736Z' name='Transition' Bdg='1SP[11075]'>"
        L"<rwcoord picketing1_val='191~671' picketing1_comm='Рокишки' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210310T202339Z' name='Transition' Bdg='2SP:2+[11076]'>"
        L"<rwcoord picketing1_val='1~500' picketing1_comm='3км' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210310T202707Z' name='Transition' Bdg='2C[11071]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210310T203154Z' name='Span_move' Bdg='DPV[11010:11071]' waynum='1'>"
        L"<rwcoord picketing1_val='1~190' picketing1_comm='3-Д-СК' />"
        L"<rwcoord picketing1_val='3~500' picketing1_comm='3-Д-СК' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210310T204028Z' name='Death' Bdg='DPV[11010:11071]' waynum='1'>"
        L"<rwcoord picketing1_val='1~190' picketing1_comm='3-Д-СК' />"
        L"<rwcoord picketing1_val='3~500' picketing1_comm='3-Д-СК' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==9 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 1 );
        CPPUNIT_ASSERT( gis.front().second->GetNumber().getNumber() == 2356 );
    }
    
    std::wstring asoup12550 =L"(:1042 909/000+12550 2356 1252 093 0734 04 12503+12730 10 03 18 45 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 02403 00000 260 57814337 57880379 000 000 00 000 065.70 000 065 000\n"
        L"Ю3 618 00000160 1 17 50 0000 00000 KAIRYS      \n"
        L"Ю4 00000 2 01 000 065.70 000 065 \n"
        L"Ю12 00 57814337 1 000 07340 48816 0931 457 01 00 00 00 00 0362 12 70 5780 04 086 07340 00000 00000 01 12520 18 00000 0000 020 0 0000 E      128 000 00000000\n"
        L"Ю12 00 57880361 255239255255 0367\n"
        L"Ю12 00 50837772 255237255255 03825080\n"
        L"Ю12 00 57879876 255237255255 03645780\n"
        L"Ю12 00 50828722 255237255255 03635080\n"
        L"Ю12 00 57880684 255237255255 03725780\n"
        L"Ю12 00 77882397 255237255255 03710769\n"
        L"Ю12 00 77875797 255239255255 0368\n"
        L"Ю12 00 77877397 255239255255 0370\n"
        L"Ю12 00 77871994 255239255255 0371\n"
        L"Ю12 00 50805480 255237255255 03695080\n"
        L"Ю12 00 77873792 255253255255 0769\n"
        L"Ю12 00 77874592 251239255255 5570370\n"
        L"Ю12 00 50830959 255237255255 03645080\n"
        L"Ю12 00 50821909 255239255255 0368\n"
        L"Ю12 00 50832120 255239255255 0369\n"
        L"Ю12 00 57850349 255237255255 03675780\n"
        L"Ю12 00 77869592 255237255255 03710769\n"
        L"Ю12 00 57880437 255237255255 03605780\n"
        L"Ю12 00 57880932 255239255255 0369\n"
        L"Ю12 00 57880072 255255255255 \n"
        L"Ю12 00 57880965 255239255255 0365\n"
        L"Ю12 00 57880601 255255255255 \n"
        L"Ю12 00 77878890 255237255255 03700769\n"
        L"Ю12 00 77871598 251255255255 357\n"
        L"Ю12 00 77881498 255255255255 \n"
        L"Ю12 00 50829977 255237255255 03675080\n"
        L"Ю12 00 77869998 255237255255 03720769\n"
        L"Ю12 00 57880031 255237255255 03635780\n"
        L"Ю12 00 50837640 251237255255 45703805080\n"
        L"Ю12 00 77879997 255237255255 03690769\n"
        L"Ю12 00 77878593 255239255255 0370\n"
        L"Ю12 00 50828821 255237255255 03635080\n"
        L"Ю12 00 77877892 251237255255 55703680769\n"
        L"Ю12 00 77879393 255239255255 0370\n"
        L"Ю12 00 57880585 255237255255 03615780\n"
        L"Ю12 00 50829712 255237255255 03595080\n"
        L"Ю12 00 57880536 255237255255 03665780\n"
        L"Ю12 00 77872091 255237255255 03710769\n"
        L"Ю12 00 57880858 255237255255 03695780\n"
        L"Ю12 00 50829605 255237255255 03585080\n"
        L"Ю12 00 50804129 251239255255 3570370\n"
        L"Ю12 00 50833482 255239255255 0361\n"
        L"Ю12 00 50837525 255239255255 0380\n"
        L"Ю12 00 78093796 255237255255 03880769\n"
        L"Ю12 00 50803311 255237255255 03705080\n"
        L"Ю12 00 50829506 255239255255 0365\n"
        L"Ю12 00 50802560 255239255255 0370\n"
        L"Ю12 00 50806520 255255255255 \n"
        L"Ю12 00 78094497 255237255255 03880769\n"
        L"Ю12 00 50802818 255237255255 03705080\n"
        L"Ю12 00 50830637 255239255255 0365\n"
        L"Ю12 00 50830728 255239255255 0361\n"
        L"Ю12 00 78091899 255237255255 03890769\n"
        L"Ю12 00 78101797 255239255255 0388\n"
        L"Ю12 00 78093093 255255255255 \n"
        L"Ю12 00 57880452 255237255255 03675780\n"
        L"Ю12 00 77870897 255237255255 03720769\n"
        L"Ю12 00 77872398 255239255255 0370\n"
        L"Ю12 00 77876993 255239255255 0368\n"
        L"Ю12 00 77876696 255239255255 0369\n"
        L"Ю12 00 78096898 251239255255 4570388\n"
        L"Ю12 00 50830942 255237255255 03635080\n"
        L"Ю12 00 57814386 255237255255 03575780\n"
        L"Ю12 00 57880379 255239255255 0365:)";

    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup12550,
        boost::gregorian::date(2021, 03, 10), nullptr, time_from_iso("20210310T164400Z") ) );

    std::wstring asoup11010 = 
        L"(:1042 909/000+11010 2356 1252 093 0734 01 11071 10 03 22 45 03/04 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 02403 00000 260 57814337 57880379 000 000 00 000 065.70 000 065 000\n"
        L"Ю4 07792 2 01 000 065.70 000 065 \n"
        L"Ю12 00 57814337 1 000 07340 48816 0931 457 01 00 00 00 00 0362 12 70 5780 04 086 07340 00000 07792 01 12520 18 00000 0000 020 0 0000 E      128 000 00000000\n"
        L"Ю12 00 57880361 255239255255 0367\n"
        L"Ю12 00 50837772 255237255255 03825080\n"
        L"Ю12 00 57879876 255237255255 03645780\n"
        L"Ю12 00 50828722 255237255255 03635080\n"
        L"Ю12 00 57880684 255237255255 03725780\n"
        L"Ю12 00 77882397 255237255255 03710769\n"
        L"Ю12 00 77875797 255239255255 0368\n"
        L"Ю12 00 77877397 255239255255 0370\n"
        L"Ю12 00 77871994 255239255255 0371\n"
        L"Ю12 00 50805480 255237255255 03695080\n"
        L"Ю12 00 77873792 255253255255 0769\n"
        L"Ю12 00 77874592 251239255255 5570370\n"
        L"Ю12 00 50830959 255237255255 03645080\n"
        L"Ю12 00 50821909 255239255255 0368\n"
        L"Ю12 00 50832120 255239255255 0369\n"
        L"Ю12 00 57850349 255237255255 03675780\n"
        L"Ю12 00 77869592 255237255255 03710769\n"
        L"Ю12 00 57880437 255237255255 03605780\n"
        L"Ю12 00 57880932 255239255255 0369\n"
        L"Ю12 00 57880072 255255255255 \n"
        L"Ю12 00 57880965 255239255255 0365\n"
        L"Ю12 00 57880601 255255255255 \n"
        L"Ю12 00 77878890 255237255255 03700769\n"
        L"Ю12 00 77871598 251255255255 357\n"
        L"Ю12 00 77881498 255255255255 \n"
        L"Ю12 00 50829977 255237255255 03675080\n"
        L"Ю12 00 77869998 255237255255 03720769\n"
        L"Ю12 00 57880031 255237255255 03635780\n"
        L"Ю12 00 50837640 251237255255 45703805080\n"
        L"Ю12 00 77879997 255237255255 03690769\n"
        L"Ю12 00 77878593 255239255255 0370\n"
        L"Ю12 00 50828821 255237255255 03635080\n"
        L"Ю12 00 77877892 251237255255 55703680769\n"
        L"Ю12 00 77879393 255239255255 0370\n"
        L"Ю12 00 57880585 255237255255 03615780\n"
        L"Ю12 00 50829712 255237255255 03595080\n"
        L"Ю12 00 57880536 255237255255 03665780\n"
        L"Ю12 00 77872091 255237255255 03710769\n"
        L"Ю12 00 57880858 255237255255 03695780\n"
        L"Ю12 00 50829605 255237255255 03585080\n"
        L"Ю12 00 50804129 251239255255 3570370\n"
        L"Ю12 00 50833482 255239255255 0361\n"
        L"Ю12 00 50837525 255239255255 0380\n"
        L"Ю12 00 78093796 255237255255 03880769\n"
        L"Ю12 00 50803311 255237255255 03705080\n"
        L"Ю12 00 50829506 255239255255 0365\n"
        L"Ю12 00 50802560 255239255255 0370\n"
        L"Ю12 00 50806520 255255255255 \n"
        L"Ю12 00 78094497 255237255255 03880769\n"
        L"Ю12 00 50802818 255237255255 03705080\n"
        L"Ю12 00 50830637 255239255255 0365\n"
        L"Ю12 00 50830728 255239255255 0361\n"
        L"Ю12 00 78091899 255237255255 03890769\n"
        L"Ю12 00 78101797 255239255255 0388\n"
        L"Ю12 00 78093093 255255255255 \n"
        L"Ю12 00 57880452 255237255255 03675780\n"
        L"Ю12 00 77870897 255237255255 03720769\n"
        L"Ю12 00 77872398 255239255255 0370\n"
        L"Ю12 00 77876993 255239255255 0368\n"
        L"Ю12 00 77876696 255239255255 0369\n"
        L"Ю12 00 78096898 251239255255 4570388\n"
        L"Ю12 00 50830942 255237255255 03635080\n"
        L"Ю12 00 57814386 255237255255 03575780\n"
        L"Ю12 00 57880379 255239255255 0365:)";


    m_chart->updateTime( time_from_iso("20210310T210000Z"), nullptr );
    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11010,
        boost::gregorian::date(2021, 03, 10), nullptr, time_from_iso("20210310T210007Z")) );
	m_chart->updateTime( time_from_iso("20210310T210007Z"), nullptr );

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
        for ( auto asoup : pathAsoupList )
        {
            if ( asoup->GetBadge().num() == EsrKit(11010) )
                CPPUNIT_ASSERT( al.IsServed( asoup ) );
            else
                CPPUNIT_ASSERT( !al.IsServed( asoup ) );
        }
    }

}

void TC_TrackedChart::IdentifyWithWrongChangeNumber_6299()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    Uncontrolled_helper();
    NsiBasis_helper();
    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20210526T130028Z' name='Form' Bdg='11C[09010]' waynum='11' parknum='1' index='' num='704M' length='12' weight='370' rapid='Y'>"
        L"<Locomotive Series='ДР1АЦ' NumLoc='227' CarrierCode='24'>"
        L"<Crew EngineDriver='Borsukovskis' Tim_Beg='2021-05-26 16:31' PersonMode='1' />"
        L"</Locomotive>"
        L"<Locomotive Series='ДР1АЦ' NumLoc='290' CarrierCode='24'>"
        L"<Crew EngineDriver='Borsukovskis' Tim_Beg='2021-05-26 16:31' PersonMode='1' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T133244Z' name='Departure' Bdg='11C[09010]' waynum='11' parknum='1' />"
        L"<SpotEvent create_time='20210526T133946Z' name='Arrival' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T134030Z' name='Departure' Bdg='2JC[09006]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T134222Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' optCode='09000:09420' />"
        L"<SpotEvent create_time='20210526T134909Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T135857Z' name='Arrival' Bdg='1AC[09410]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20210526T140058Z' name='Departure' Bdg='1C[09410]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20210526T141138Z' name='Transition' Bdg='2C[09400]' waynum='2' parknum='1' optCode='09380:09400' />"
        L"<SpotEvent create_time='20210526T142401Z' name='Transition' Bdg='2C[09380]' waynum='2' parknum='1' optCode='09370:09380' />"
        L"<SpotEvent create_time='20210526T143024Z' name='Arrival' Bdg='2C[09370]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T143337Z' name='Departure' Bdg='2C[09370]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T144047Z' name='Transition' Bdg='2C[09360]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T145310Z' name='Transition' Bdg='2AC[11760]' waynum='2' parknum='2' />"
        L"<SpotEvent create_time='20210526T145424Z' name='Arrival' Bdg='2C[11760]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T145537Z' name='Departure' Bdg='2C[11760]' waynum='2' parknum='1' optCode='11420:11760' />"
        L"<SpotEvent create_time='20210526T150800Z' name='Arrival' Bdg='2C[11420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T151049Z' name='Departure' Bdg='2C[11420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T151735Z' name='Transition' Bdg='1C[11419]' waynum='1' parknum='1' optCode='11419:11422' />"
        L"<SpotEvent create_time='20210526T152209Z' name='Transition' Bdg='1C[11422]' waynum='1' parknum='1' optCode='11422:11430' />"
        L"<SpotEvent create_time='20210526T153028Z' name='Arrival' Bdg='4C[11430]' waynum='4' parknum='1' index='' num='D704M' length='12' weight='370' rapid='Y'>"
        L"<Locomotive Series='ДР1АЦ' NumLoc='227' CarrierCode='24'>"
        L"<Crew EngineDriver='Borsukovskis' Tim_Beg='2021-05-26 16:31' PersonMode='1' />"
        L"</Locomotive>"
        L"<Locomotive Series='ДР1АЦ' NumLoc='290' CarrierCode='24'>"
        L"<Crew EngineDriver='Borsukovskis' Tim_Beg='2021-05-26 16:31' PersonMode='1' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T153342Z' name='Departure' Bdg='4C[11430]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210526T154051Z' name='Transition' Bdg='1C[11431]' waynum='1' parknum='1' optCode='11431:11432' />"
        L"<SpotEvent create_time='20210526T154122Z' name='Transition' Bdg='1AC[11431]' waynum='1' parknum='2' optCode='11431:11432' />"
        L"<SpotEvent create_time='20210526T154629Z' name='Transition' Bdg='1C[11432]' waynum='1' parknum='1' optCode='11432:11446' />"
        L"<SpotEvent create_time='20210526T155117Z' name='Transition' Bdg='1C[11446]' waynum='1' parknum='1' optCode='11442:11446' />"
        L"<SpotEvent create_time='20210526T155806Z' name='Transition' Bdg='1BC[11442]' waynum='1' parknum='1' optCode='11442:11443' />"
        L"<SpotEvent create_time='20210526T160125Z' name='Transition' Bdg='1C[11443]' waynum='1' parknum='1' optCode='11443:11445' />"
        L"<SpotEvent create_time='20210526T160544Z' name='Transition' Bdg='5SP[11445]'>"
        L"<rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T160551Z' name='Wrong_way' Bdg='N1P_BP387[11445:11451]' waynum='1'>"
        L"<rwcoord picketing1_val='383~39' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='383~710' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T160855Z' name='Transition' Bdg='4/6SP:4+[11451]'>"
        L"<rwcoord picketing1_val='387~399' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T161040Z' name='Transition' Bdg='№704[11002]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T161300Z' name='Arrival' Bdg='№704[11000]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T161700Z' name='Departure' Bdg='№704[11000]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T162220Z' name='Transition' Bdg='№704[11003]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210526T162547Z' name='Transition' Bdg='1C[11070]' waynum='1' parknum='1' optCode='11063:11070' />"
        L"<SpotEvent create_time='20210526T162706Z' name='Transition' Bdg='1SP:1+[11063]' optCode='11062:11063'>"
        L"<rwcoord picketing1_val='400~732' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T163121Z' name='Transition' Bdg='1C[11062]' waynum='1' parknum='1' optCode='11060:11062' />"
        L"<SpotEvent create_time='20210526T163513Z' name='Arrival' Bdg='1C[11062]' waynum='1' parknum='1' index='' num='D3581' outbnd='Y' />"
        L"<SpotEvent create_time='20210526T172139Z' name='Departure' Bdg='1C[11062]' waynum='1' parknum='1' optCode='11062:11063' />"
        L"<SpotEvent create_time='20210526T173001Z' name='Transition' Bdg='1SP[11063]' optCode='11063:11790'>"
        L"<rwcoord picketing1_val='401~80' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T173622Z' name='Transition' Bdg='PAP[11790]'>"
        L"<rwcoord picketing1_val='14~3' picketing1_comm='524км' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T173815Z' name='Transition' Bdg='11SP[11363]'>"
        L"<rwcoord picketing1_val='13~190' picketing1_comm='524км' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T173907Z' name='Wrong_way' Bdg='PM2P[11010:11363]' waynum='2'>"
        L"<rwcoord picketing1_val='525~500' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='524~500' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T174347Z' name='Span_move' Bdg='PM8P[11010:11363]' waynum='2'>"
        L"<rwcoord picketing1_val='527~900' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='527~' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210526T174604Z' name='Death' Bdg='PM8P[11010:11363]' waynum='2'>"
        L"<rwcoord picketing1_val='527~900' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='527~' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    unsigned const trainID = 64;
    HappenLayer& hl = m_chart->getHappenLayer();
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==46 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 3 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 3581 );
        auto itGis = std::next(gis.begin(), 1);
        CPPUNIT_ASSERT( itGis!=gis.end() && itGis->second->GetNumber().getNumber() == 704 );
        auto vEvents = pathCreate->GetAllEvents();
        auto h_Arrival11430 = *std::next(vEvents.begin(), 20);
        CPPUNIT_ASSERT( h_Arrival11430 && h_Arrival11430->GetBadge().num()==EsrKit(11430) && itGis->first==h_Arrival11430->GetTime());
    }

    std::wstring asoup11000 = 
        L"(:1042 909/000+11010 3581 1105 710 1100 01 11363 26 05 20 49 03/09 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 017 01022 00689 056 95716569 59594291 000 000 00 012 002.90 012 002 000 95 012 000 000\n"
        L"Ю4 16169 2 13 000 002.90 000 002 \n"
        L"Ю4 00000 1 01 012 000.90 012 000 95 012 000 \n"
        L"Ю4 09590 1 01 012 000.90 012 000 95 012 000 \n"
        L"Ю12 00 95716569 1 055 09590 10304 8239 500 00 00 00 00 00 0220 30 95 0950 04 106 11000 09000 00000 01 09630 24 00000 0000 025 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95988713 191239255255 0710235\n"
        L"Ю12 00 95091278 191239255255 0550220\n"
        L"Ю12 00 95069761 255255255255 \n"
        L"Ю12 00 95700001 255255255255 \n"
        L"Ю12 00 95068169 255255255255 \n"
        L"Ю12 00 95685236 255255255255 \n"
        L"Ю12 00 95068987 255255255255 \n"
        L"Ю12 00 95047072 191255255255 051\n"
        L"Ю12 00 95684767 191255255255 055\n"
        L"Ю12 00 95548913 191239255255 0560215\n"
        L"Ю12 00 95003273 191239255255 0710234\n"
        L"Ю12 00 59592105 131225067131 00015170103047299505033012905917167161301616913110500261102011051 128\n"
        L"Ю12 00 59594291 255255255255 :)";


    CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11000,
    boost::gregorian::date(2021, 05, 26), nullptr, time_from_iso("20210526T174604Z")) );
    m_chart->updateTime( time_from_iso("20210526T181000Z"), nullptr );

    AsoupLayer& al = m_chart->getAsoupLayer();
    {
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==47 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 4 );
        auto vEvents = pathCreate->GetAllEvents();
        auto h_Arrival11062 = *std::next(vEvents.begin(), 38);
        auto gis3581 = *std::next(gis.begin(), 2);
        CPPUNIT_ASSERT( gis3581.second->GetNumber().getNumber() == 3581 );
        CPPUNIT_ASSERT ( gis3581.first == h_Arrival11062->GetTime() );
        CPPUNIT_ASSERT ( h_Arrival11062->GetBadge().num()==EsrKit(11062) );
        auto itGis = std::next(gis.begin(), 1);
        CPPUNIT_ASSERT( itGis!=gis.end() && itGis->second->GetNumber().getNumber() == 704 );
        auto h_Arrival11430 = *std::next(vEvents.begin(), 20);
        CPPUNIT_ASSERT( h_Arrival11430 && h_Arrival11430->GetBadge().num()==EsrKit(11430) && itGis->first==h_Arrival11430->GetTime());
    }

}

void TC_TrackedChart::AsoupOtherCategorie_KIG_1114()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    setupEsrGuide();
    NsiBasis_helper();

    auto path = loaderSpot( 
        L"<HemPath>"
        L"<SpotEvent create_time='20210806T212433Z' name='Form' Bdg='15AC[09000]' waynum='15' parknum='1' index='' num='10ja-026' fastyear='Y' />"
        L"<SpotEvent create_time='20210806T212433Z' name='Departure' Bdg='15AC[09000]' waynum='15' parknum='1' optCode='09000:09006' />"
        L"<SpotEvent create_time='20210806T212512Z' name='Transition' Bdg='19AC[09000]' waynum='19' parknum='1' optCode='09000:09006' />"
        L"<SpotEvent create_time='20210806T213005Z' name='Arrival' Bdg='17JC[09006]' waynum='17' parknum='1' />"
        L"<SpotEvent create_time='20210806T213228Z' name='Departure' Bdg='17JC[09006]' waynum='17' parknum='1' />"
        L"<SpotEvent create_time='20210806T213846Z' name='Arrival' Bdg='10JC[09006]' waynum='10' parknum='1' />"
        L"<SpotEvent create_time='20210806T222230Z' name='Departure' Bdg='10JC[09006]' waynum='10' parknum='1' index='' num='9ja-026' fastyear='Y' />"
        L"</HemPath>"
        );

    unsigned const trainID = 64;
    CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );

    UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
    {
        HappenLayer::ReadAccessor container = hl.GetReadAccess();
        auto pathSet = container->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size() == 1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==7 );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
        auto tdr = gis.back().second;
        CPPUNIT_ASSERT( tdr && tdr->GetNumber().getNumber() == 9 );
    }

    const std::wstring departure9006 = 
        L"(:1042 909/000+09006 3701 0900 193 0970 03 09100 07 08 01 20 01/10 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04266 02286 244 95772158 57892234 000 000 00 056 005.20 001 000 000 40 001 000 000 60 008 004 000 70 040 000 000 90 006 001 000 93 000 001 000 95 005 000 000\n"
        L"Ю3 579 00002681 1 19 00 0000 00000 M-URBANS    \n"
        L"Ю3 579 00002682 9\n"
        L"Ю4 00000 1 01 056 005.20 001 000 40 001 000 60 008 004 70 040 000 90 006 001 93 000 001 95 005 000 \n"
        L"Ю4 09740 1 01 045 005.60 004 004 70 040 000 90 001 001 93 000 001 \n"
        L"Ю4 09700 1 01 002 000.20 001 000 40 001 000 \n"
        L"Ю4 09900 1 01 004 000.60 004 000 \n"
        L"Ю4 09720 1 01 005 000.90 005 000 95 005 000 \n"
        L"Ю12 00 95772158 1 063 09720 54210 8237 300 02 00 00 00 00 0235 30 95 0950 04 106 09700 00000 00000 01 09000 38 00000 0000 020 0 0000 0      132 000 00000000\n"
        L"Ю12 00 95772182 191255255255 068\n"
        L"Ю12 00 95978136 189239255255 055040234\n"
        L"Ю12 00 54437835 129169125247 060097000030039140800002022040540410542CTPГ  \n"
        L"Ю12 00 95034286 129169125247 0560972054210823700004000233950950106380     \n"
        L"Ю12 00 95785911 189239255255 057020235\n"
        L"Ю12 00 91571026 133233125191 0460974009111469400026390091310524021\n"
        L"Ю12 00 62464375 255233127191 0241600600100026\n"
        L"Ю12 00 60663267 191239255191 0590243021\n"
        L"Ю12 00 64478001 255239255255 0240\n"
        L"Ю12 00 61962510 167231253179 0001711085170245130202009189-128\n"
        L"Ю12 00 62515531 239255253255 1611201\n"
        L"Ю12 00 52798477 255237255255 02345600\n"
        L"Ю12 00 62331228 255237255255 02450600\n"
        L"Ю12 00 59538710 239233125183 4210001919353300864302511003-\n"
        L"Ю12 00 61151510 167225125179 047091114694023130600600100240210     132\n"
        L"Ю12 00 54672837 135237253191 069099001611256590242560001020\n"
        L"Ю12 00 60498235 191237255255 07502470600\n"
        L"Ю12 00 62704283 255239255255 0249\n"
        L"Ю12 00 61308946 191239255255 0700240\n"
        L"Ю12 00 29172962 129233125247 0550970012202710200504026820021113524ПЛИTЫ \n"
        L"Ю12 00 50801240 129233125183 0330974022607163035701037070508008603024OXP   \n"
        L"Ю12 00 50803931 255255255255 \n"
        L"Ю12 00 50533801 255239255255 0371\n"
        L"Ю12 00 57892556 255237255255 03705780\n"
        L"Ю12 00 50530682 255237255255 03695080\n"
        L"Ю12 00 50805027 255255255255 \n"
        L"Ю12 00 50807056 255255255255 \n"
        L"Ю12 00 57892713 255237255255 03715780\n"
        L"Ю12 00 50536671 191237255255 03203705080\n"
        L"Ю12 00 50533959 191239255255 0330372\n"
        L"Ю12 00 50805936 255239255255 0369\n"
        L"Ю12 00 50531169 191255255255 035\n"
        L"Ю12 00 50803352 191239255255 0340368\n"
        L"Ю12 00 50803030 255239255255 0370\n"
        L"Ю12 00 57892473 191253255255 0335780\n"
        L"Ю12 00 57892259 255239255255 0371\n"
        L"Ю12 00 50890177 255237255191 03685080026\n"
        L"Ю12 00 50890870 255239255255 0367\n"
        L"Ю12 00 50533173 255239255191 0372024\n"
        L"Ю12 00 50808005 187239255255 0344570365\n"
        L"Ю12 00 50804061 191239255255 0330367\n"
        L"Ю12 00 57892564 191237255255 03403715780\n"
        L"Ю12 00 50807866 191237255255 03303695080\n"
        L"Ю12 00 57892747 255237255255 03705780\n"
        L"Ю12 00 57892606 255239255255 0372\n"
        L"Ю12 00 50807338 255237255255 03655080\n"
        L"Ю12 00 50802420 255239255255 0366\n"
        L"Ю12 00 50535517 255239255255 0372\n"
        L"Ю12 00 57892424 191237255255 03403705780\n"
        L"Ю12 00 50806272 191237255255 03303675080\n"
        L"Ю12 00 50801216 255239255255 0371\n"
        L"Ю12 00 50800713 255255255191 025\n"
        L"Ю12 00 50535400 255255255191 024\n"
        L"Ю12 00 50802354 255239255255 0370\n"
        L"Ю12 00 50532969 255239255255 0371\n"
        L"Ю12 00 50805704 191239255255 0340370\n"
        L"Ю12 00 50803923 191239255255 0330369\n"
        L"Ю12 00 57892762 255237255255 03705780\n"
        L"Ю12 00 50807502 191237255255 03203685080\n"
        L"Ю12 00 57892234 187237255255 03335703705780:)"
;

    {
        CPPUNIT_ASSERT(m_chart->processNewAsoupText( departure9006, boost::gregorian::date(2021, 8, 7), nullptr, 0) );
        m_chart->updateTime( time_from_iso("20210806T223000Z"), nullptr );
        m_chart->updateTime( time_from_iso("20200920T052000Z"), nullptr );
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
        auto pathSet = hl.GetReadAccess()->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size()==1 );
        auto pathCreate = *pathSet.cbegin();
        CPPUNIT_ASSERT( pathCreate->GetEventsCount()==7 );
        CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Transition Arrival Departure Arrival Departure" ) );
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
        auto tdr = gis.back().second;
        CPPUNIT_ASSERT( tdr && tdr->GetNumber().getNumber() == 9 );
    }

    {
        std::wstring spotStr = L"<SpotEvent create_time='20210806T222812Z' name='Span_move' Bdg='R5P[09006:09010]' waynum='3'>"
            L"<rwcoord picketing1_val='3~265' picketing1_comm='Рига-Зилупе' />"
            L"<rwcoord picketing1_val='2~865' picketing1_comm='Рига-Зилупе' />"
            L"</SpotEvent>";

        CPPUNIT_ASSERT( m_chart->acceptEventFromStr(spotStr, trainID, TrainCharacteristics::Source::Guess, 0) );
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
        auto pathSet = hl.GetReadAccess()->getAllPathes();
        CPPUNIT_ASSERT( pathSet.size()==1 );
        auto pathCreate = *pathSet.cbegin();
        auto gis = pathCreate->GetInfoSpots();
        CPPUNIT_ASSERT( gis.size() == 2 );
        CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 9 );
    }

    {
        m_chart->updateTime( time_from_iso("20200920T055000Z"), nullptr );
        AsoupLayer& al = m_chart->getAsoupLayer();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = pathAsoupList.front();
        //АСОУП не должно подхватиться, поскольку нить другой категории ПЕ
        CPPUNIT_ASSERT( !al.IsServed( asoup ) );
    }
    set_TimeZone("");
}

void TC_TrackedChart::AsoupNordWithdrawal()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    auto oldMode = m_chart->getAsoupOperationMode();
    m_chart->setupAsoupOperationMode(Asoup::AsoupOperationMode::NORD);
    // Отменяемое сообщение
    CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 928/400+30640 4702 3046 576 3064 01 30460 09 12 13 29 01/09 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 005 00417 00300 020 30865711 30868079 000 005 00 000 000.90 000 000 005\n"
        L"Ю3 572 00023121 1 06 10 2806 03279 ИCAEB        00000 041 2819 00035008 0 1 00 00 00 00\n"
        L"Ю3 572 00023122 9\n"
        L"Ю6 30865711 20211213\n"
        L"Ю6 30847008 20211213\n"
        L"Ю6 30894513 20211213\n"
        L"Ю6 30847339 20211213\n"
        L"Ю6 30868079 20211213\n"
        L"Ю8 3046 576 3064 30640 01 09 12 13 29 4702 304606 576 306404\n"
        L"Ю12 00 30865711 1 060 30640 23603 3641 900 00 00 00 00 00 0235 40 90 0304 04 082 30640 30640 00000 04 30460 19 30460 3781 020 0 0000 Э80/40 132 000 10000000\n"
        L"Ю12 00 30847008 255239255255 0232\n"
        L"Ю12 00 30894513 255239255255 0237\n"
        L"Ю12 00 30847339 255239255255 0232\n"
        L"Ю12 00 30868079 255255255255:)"));

    AsoupLayer& al = m_chart->getAsoupLayer();
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT ( pathAsoupList.size()==1 && pathAsoupList.front()->GetNumber()==4702 );
    // Отменяющее сообщение
    CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:333 1042 928/400+30640 4702 3046 576 3064 01:)"));

    pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT ( pathAsoupList.empty() );

    set_TimeZone("");
    m_chart->setupAsoupOperationMode(oldMode);
}

void TC_TrackedChart::AsoupDisform()
{
	auto pathSrc = loaderSpot(            
		L"<HemPath>"
		L"<SpotEvent create_time='20151010T144700Z' name='Departure' Bdg='ASOUP 1042[29610]' waynum='2' parknum='1' index='0000-012-6533' num='6534' length='4' weight='114' suburbreg='Y'>"
		L"<Locomotive Series='ТЭП70' NumLoc='49' Depo='2864' Consec='1'>"
		L"<Crew EngineDriver='ЗACOБA' TabNum='9603' Tim_Beg='2021-12-12 16:33' />"
		L"</Locomotive>"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20151010T152836Z' name='Transition' Bdg='ЧБП[29602]' index='0000-012-6533' num='6534' length='4' weight='114' suburbreg='Y'>"
		L"<Locomotive Series='ТЭП70' NumLoc='49' Depo='2864' Consec='1'>"
		L"<Crew EngineDriver='ЗACOБA' TabNum='9603' Tim_Beg='2021-12-12 16:33' />"
		L"</Locomotive>"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20151010T153841Z' name='Transition' Bdg='2Бп[29601]' waynum='2' parknum='1' optCode='29601:29681' />"
		L"<SpotEvent create_time='20151010T154036Z' name='Arrival' Bdg='2Ап[29601]' waynum='2' parknum='1' />"
		L"<SpotEvent create_time='20151010T154327Z' name='Departure' Bdg='2Ап[29601]' waynum='2' parknum='1' optCode='29601:29681' />"
		L"<SpotEvent create_time='20151010T155623Z' name='Arrival' Bdg='3п[29681]' waynum='3' parknum='1' />"
		L"<SpotEvent create_time='20151010T160227Z' name='Departure' Bdg='3п[29681]' waynum='3' parknum='1' optCode='29680:29681' />"
		L"<SpotEvent create_time='20151010T161302Z' name='Arrival' Bdg='1п[29680]' waynum='1' parknum='1'>"
		L"<rwcoord picketing1_val='207~' picketing1_comm='Мурманск' />"
		L"<rwcoord picketing1_val='208~' picketing1_comm='Мурманск' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20151010T161851Z' name='Departure' Bdg='1п[29680]' waynum='1' parknum='1' optCode='29590:29680'>"
		L"<rwcoord picketing1_val='207~' picketing1_comm='Мурманск' />"
		L"<rwcoord picketing1_val='208~' picketing1_comm='Мурманск' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20151010T163556Z' name='Arrival' Bdg='2п[29590]' waynum='2' parknum='1' />"
		L"<SpotEvent create_time='20151010T165100Z' name='Departure' Bdg='2п[29590]' waynum='2' parknum='1' optCode='29580:29590' />"
		L"<SpotEvent create_time='20151010T170137Z' name='Arrival' Bdg='2Ап[29580]' waynum='2' parknum='1' />"
// 		L"<SpotEvent create_time='20151010T170205Z' name='Departure' Bdg='2Ап[29580]' waynum='2' parknum='1' />"
// 		L"<SpotEvent create_time='20151010T170218Z' name='Arrival' Bdg='2Бп[29580]' waynum='2' parknum='1' />"
// 		L"<SpotEvent create_time='20151010T173350Z' name='Departure' Bdg='2Бп[29580]' waynum='2' parknum='1' />"
// 		L"<SpotEvent create_time='20151010T173510Z' name='Transition' Bdg='1Ап[29580]' waynum='1' parknum='1'/>"
		L"</HemPath>"
		);

	CPPUNIT_ASSERT( CreatePath(*m_chart, pathSrc, 1) );
	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.back()->GetCode()==HCode::ARRIVAL );
	}

	CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 928/400+29580 6534 0000 012 6533 05 29500 10 10 20 03 01/02 0 0/00 00 0\n"
		L"Ю8 0000 012 6533 29580 64 12 12 20 01 6534 296109 268 295801:)", boost::gregorian::date(2015, 10, 10), boost::posix_time::time_duration(20, 3, 0) ));

	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==6534 && al.IsServed(asoup) );

		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.back()->GetCode()==HCode::DISFORM );
	}
}

void TC_TrackedChart::AttachAsoupDisform()
{
	Uncontrolled_helper();
	{
		const std::wstring district_str = 
			L"<DistrictList>"
			L"<District code='31000-05110' name='Ярославль-Сонково' attachDepartureDisform='Y' >"
			L"<Involve esr='31160' />"  L"<!--Некоуз -->"          
			L"<Involve esr='31151' />"  L"<!--Маслово -->"         
			L"<Involve esr='31139' />"  L"<!--Родионово -->"       
			L"<Involve esr='31132' />"  L"<!--Пуршево -->"         
			L"<Involve esr='31130' />"  L"<!--Пищалкино -->"       
			L"<Involve esr='05110' />"  L"<!--Сонково -->"         
			L"</District>"
			L"</DistrictList>";

		attic::a_document x_doc;
		CPPUNIT_ASSERT( x_doc.load_wide( district_str ) );
		std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

		DistrictGuide dg;
		dg.deserialize( x_doc.document_element() );
		m_chart->cachingDistrictGuide(dg);
	}

	{
	std::shared_ptr<TopologyTest> tt( new TopologyTest );
	tt->Load(L"*05110,31262 {05110,31130,31132,31139,31151,31160,31262}");
	m_chart->setupTopology( tt );
	}


	CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 928/400+31151 6579 0000 011 6579 01 31210 11 12 13 44 01/02 0 0/00 00 0\n"
		L"Ю3 510 00001000 1 11 17 2853 02403 БAЛAШOB      00000 009 2805 00023308 0 1 00 00 00 00\n"
		L"Ю8 0000 011 6579 31151 01 11 12 13 44 6579 312104 671 311390:)", boost::gregorian::date(2021, 12, 11)));

	CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 928/400+31151 6579 0000 011 6579 03 05110 11 12 13 45 01/02 0 0/00 00 0\n"
		L"Ю3 510 00001000 1 11 17 2853 02403 БAЛAШOB      00000 009 2805 00023308 0 1 00 00 00 00\n"
		L"Ю8 0000 011 6579 31151 02 11 12 13 45 6579 312104 671 311390:)", boost::gregorian::date(2021, 12, 11)));
	m_chart->updateTime( time_from_iso("20211211T104500Z") + 200, nullptr );
{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==2 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( al.IsServed(asoup) );
		asoup = pathAsoupList.back();
		CPPUNIT_ASSERT ( al.IsServed(asoup) );
	}

	CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 928/400+31139 6579 0000 011 6579 05 31210 11 12 14 02 01/01 0 0/00 00 0\n"
		L"Ю8 0000 011 6579 31139 64 11 12 14 02 6579 312104 671 311390:)", boost::gregorian::date(2021, 12, 11)));
	m_chart->updateTime( time_from_iso("20211211T110200Z") + 100, nullptr );
	m_chart->updateTime( time_from_iso("20211211T110200Z") + 150, nullptr );
    m_chart->updateTime( time_from_iso("20211211T110200Z") + 200, nullptr );

	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==3 );
        for ( auto it : pathAsoupList )
            CPPUNIT_ASSERT( al.IsServed( it ));
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();

		CPPUNIT_ASSERT( vEvents.back()->GetCode()==HCode::DISFORM );
	}
}


void TC_TrackedChart::RegulatoryIdentificationErr()
{
	auto oldMode = m_chart->getAsoupOperationMode();
	m_chart->setupAsoupOperationMode(Asoup::AsoupOperationMode::NORD);
	const std::string scheduleData = "<export>\n"
		"<ScheduledLayer timePeriod='[20150830T100000Z/20160831T100000Z)'>\n"
		"  <ScheduledPath>\n"
		"<SpotEvent create_time='20211211T132900Z' name='Info_changing' Bdg='№6515[29580]' index='' num='6515' suburbreg='Y' />"
		"<SpotEvent create_time='20211211T132900Z' name='Departure' Bdg='№6515[29580]' waynum='1' parknum='1' />"
		"<SpotEvent create_time='20211211T134300Z' name='Arrival' Bdg='№6515[29590]' waynum='1' parknum='1' />"
		"<SpotEvent create_time='20211211T134400Z' name='Departure' Bdg='№6515[29590]' waynum='1' parknum='1' />"
		"<SpotEvent create_time='20211211T140100Z' name='Arrival' Bdg='№6515[29680]' waynum='1' parknum='1' />"
		"<SpotEvent create_time='20211211T140200Z' name='Departure' Bdg='№6515[29680]' waynum='1' parknum='1' />"
		"<SpotEvent create_time='20211211T141700Z' name='Arrival' Bdg='№6515[29681]' waynum='1' parknum='1' />"
		"  </ScheduledPath>\n"
		"</ScheduledLayer>\n"
		"</export>\n";

	m_chart->deserializeRegulatoryLayer(scheduleData);
	ProcessChanges(*m_chart);


	auto pathHappen = loaderSpot(
	L"<HemPath>"
	L"<SpotEvent create_time='20211211T133045Z' name='Form' Bdg='2Ап[29580]' waynum='2' parknum='1' />"
	L"<SpotEvent create_time='20211211T133045Z' name='Departure' Bdg='2Ап[29580]' waynum='2' parknum='1' />"
	L"</HemPath>");
	unsigned int trainId = 63;
	CPPUNIT_ASSERT( CreatePath( *m_chart, pathHappen, trainId ) );

	{
	HappenLayer& hl = m_chart->getHappenLayer();
	HappenLayer::ReadAccessor container = hl.GetReadAccess();
	auto pathSet = container->getAllPathes();
	CPPUNIT_ASSERT( pathSet.size() == 1 );
	auto pathCreate = *pathSet.cbegin();
	CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==2 );
	auto gis = pathCreate->GetInfoSpots();
	CPPUNIT_ASSERT( gis.empty() );
	}

	CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::STATION_EXIT,  BadgeE(L"ЧАП", EsrKit(29580)), time_from_iso("20211211T133151Z")), trainId, TrainCharacteristics::Source::Guess));
	CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( std::make_shared<SpotEvent>(HCode::SPAN_MOVE,  BadgeE(L"КПУ1Ч@12", EsrKit(29580, 29590)), time_from_iso("20211211T133151Z")), trainId, TrainCharacteristics::Source::Guess));

	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==4 );
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.empty() );
	}
	m_chart->setupAsoupOperationMode(oldMode);
}

void TC_TrackedChart::GuiError()
{
	CPPUNIT_ASSERT(PassGuiRawText(*m_chart, L"(:0001 POL02'GM-POL':20 40317 :2 25 04 22 1464\n"
		L"К 1650902495 -000036992 05 01 000 00 00 029305 029320 075 075 0222116634 0222116636 00000 00000 146 124 003 1 46173 22266 33060 2950 454 3000 2047\n"
		L"причина задерж.в продв. поезда: [94] Неприем порожних собственных полувагонов, подсылаемых|на Западно-Сибирскую железную дорогу\n"
		L"36864 219101760 $0"));
	CPPUNIT_ASSERT_EQUAL(size_t(1), GetGuiTrioCount());

	CPPUNIT_ASSERT(PassGuiRawText(*m_chart, L"(:0001 28002'IP0A.17.DD.29':20 40317 :2 11 01 22 1464\n"
		L"К 0000351553 0000000001 02 02 000 01 01 031481 000000 015 093 0221965850 0221965970 00004 00000 157 000 014 0\n"
		L"Со снятием напряжения. |Рук. раб.: Михайлов (ЭЧ-1 СЕВ)|Установка ПС(3шт), Установка эл.соединителей(3шт)|http://asapvo.gvc.oao.rzd/apvo2-op/app/cards/wnd/351553\n"
		L"34962 6 221965850 120 -6 157 0 157 0 0 0 0 0 0 0 way=2 3 12 $0"));

	CPPUNIT_ASSERT(!PassGuiRawText(*m_chart, L"(:0001 28002'GID-VOL2-DSP':20 40317 :2 11 01 22 1464\n"
		L"К 1641861946 0000130030 04 02 021 02 07 030031 000000 009 009 0221965883 0221965883 00068 00000 000 000 010 1 0 0 0 3003 580 2930 2222\n"
		L"366\n"
		L"00002 0 0 0 0 0 0 0 0 0 0 0 0 0 7 $0"));

	ProcessChanges(*m_chart);
	CPPUNIT_ASSERT_EQUAL(size_t(0), GetGuiTrioCount());
}

void TC_TrackedChart::AsoupToAsoup_WithErrorGlueOnTheSameStation_6646()
{
    setupEsrGuide();
    Uncontrolled_helper();
	{
		std::shared_ptr<TopologyTest> tt( new TopologyTest );
		tt->Load(L"*31262,05110 {31262,31268,31250,31232,31231,31230,31210,31202,31203,31191,31180,31170,31160,31151,31139,31132,31130,05110}");
		m_chart->setupTopology( tt );
	}

	CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 928/400+31151 5292 3115 251 3116 07 31210 11 12 19 10 01/02 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 006 00173 00000 012 03132735 02931053 000 000 03 000 000\n"
		L"Ю7 0 31160 0000 001.00000 001\n"
		L"Ю7 0 31160 0000 001.00 001\n"
		L"Ю8 3115 251 3116 31151 05 11 12 19 10 5292 311506 251 311600\n"
		L"Ю12 00 03132735 1 000 31160 00000 0000 000 00 00 00 00 00 0572 70 01 0010 04 183 31160 31160 00000 01 00000 00 31110 0000 020 0 0000 ПACC   128 000 00000000\n"
		L"Ю12 00 02933786 255239255255 0575\n"
		L"Ю12 00 02931053 255255255255:)", boost::gregorian::date(2021, 12, 11) ));

		CPPUNIT_ASSERT(PassAsoupRawText(*m_chart, L"(:1042 928/400+31151 5292 3115 251 3116 03 31210 11 12 19 52 01/02 0 0/00 00 0\n"
		L"Ю2 0 00 00 00 00 0000 0 0 006 00173 00000 012 03132735 02931053 000 000 03 000 000\n"
		L"Ю3 510 00001000 1 11 17 2853 02403 БAЛAШOB      00000 015 2805 00038808 0 1 00 00 00 00\n"
		L"Ю8 3115 251 3116 31151 02 11 12 19 52 5292 311506 251 311600\n"
		L"Ю12 00 03132735 1 000 31160 00000 0000 000 00 00 00 00 00 0572 70 01 0010 04 183 31160 31160 00000 01 00000 00 31110 0000 020 0 0000 ПACC   128 000 00000000\n"
		L"Ю12 00 02933786 255239255255 0575\n"
		L"Ю12 00 02931053 255255255255:)", boost::gregorian::date(2021, 12, 11)));

		auto path1 = loaderSpot(
			L"<HemPath>"
			L"<SpotEvent create_time='20211211T105830Z' name='Span_move' Bdg='#5292[31151:31160]' index='' num='5292' tepsng='Y'>"
			L"<rwcoord picketing1_val='59~150' picketing1_comm='Рыбинск' />"
			L"</SpotEvent>"
			L"<SpotEvent create_time='20211211T165500Z' name='Arrival' Bdg='N5292[31151]' waynum='1' parknum='1' />"
			L"<SpotEvent create_time='20211211T165500Z' name='Death' Bdg='N5292[31151]' waynum='1' parknum='1' />"
			L"</HemPath>"
			);

		CPPUNIT_ASSERT( CreatePath(*m_chart, path1, 1) );

		UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
		CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "#5292[31151:31160] N5292[31151] N5292[31151]"));
		CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move Arrival  Death" ) );

		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT ( pathAsoupList.size()==2 );
// 		auto timeFirst = pathAsoupList.front()->GetTime();
// 		auto timeSecond = pathAsoupList.back()->GetTime();
// 		if ( timeFirst>timeSecond )
// 			std::swap(timeFirst, timeSecond);
// 
// 		m_chart->updateTime( timeSecond+910/*time_from_iso("20211211T165500Z")*/, nullptr );
// 		m_chart->updateTime( timeSecond+910*2/*time_from_iso("20211211T165500Z")*/, nullptr );
}

void TC_TrackedChart::AsoupAttemptWithIndexChangeOnFirstDeparture_6829()
{
	time_t curtime = timeForDate(2016, 01, 01);
	time_t departure_time = curtime + 10;
	time_t transition_timeB = departure_time + 20;

	{
		const ParkWayKit c_parkWay(1, 1);

		// Строгая идентификация с первого раза
		SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
			c_parkWay, std::make_pair(PicketingInfo(), PicketingInfo()));
		
		const TrainDescr number(L"2176", L"0300-851-2300");
		SpotDetailsCPtr formDetails = std::make_shared<SpotDetails>(number);

		SpotPtr pathEvents[] = { std::make_shared<SpotEvent>( HCode::DEPARTURE, c_stationA, departure_time, formDetails ),
			std::make_shared<SpotEvent>( HCode::TRANSITION, c_stationB, transition_timeB, spotDetails ),
			std::make_shared<SpotEvent>( HCode::DEATH, c_stationB, transition_timeB, spotDetails ),
		};

		CPPUNIT_ASSERT(CreatePath(*m_chart, pathEvents, 64));
		CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
		CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::DEPARTURE, c_stationA,
			c_parkWay, departure_time+240, TrainDescr(L"8602", L"3121-042-3127"))));
		CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());

		CPPUNIT_ASSERT_EQUAL(size_t(0), GetHappenTrioCount());
	}
}

void TC_TrackedChart::GuiTrain()
{
	CPPUNIT_ASSERT(PassGuiRawText(*m_chart, L"(:0001 28000'BALAK_DSP':20 40317 :5 45059 17288 30013 2552 31330 10 31342 222178408 1 2 0 0 6525 71 П2 Z яв 09-25  S7630 F00232 "));
	CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==1 );
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( !gis.empty() );
		CPPUNIT_ASSERT(gis.front().second->GetNumber().getNumber() == 2552);
	}

}

void TC_TrackedChart::LinkFromBorderUncStatToControlZone()
{
	setupEsrGuide();
	Uncontrolled_helper();

	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::DEPARTURE, BadgeE(L"5C", EsrKit(16170)),
		ParkWayKit(1, 5), time_from_iso("20220726T051000Z"), TrainDescr(L"2811", L"8162-184-0983"))));

	auto pathSrc = loaderSpot(            
		L"<HemPath>"																				
		L"<SpotEvent create_time='20220726T053050Z' name='Form' Bdg='1N2IP[11042:16170]' waynum='1'>"
		L"<rwcoord picketing1_val='461~750' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='465~' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220726T053050Z' name='Span_move' Bdg='1N1IP[11042:16170]' waynum='1'>"
		L"<rwcoord picketing1_val='460~417' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='461~750' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220726T053259Z' name='Station_entry' Bdg='NDP[11042]'>"
		L"<rwcoord picketing1_val='460~417' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"</HemPath>"
		);

	for ( auto spot : pathSrc )
		CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(spot, 1, TrainCharacteristics::Source::Guess));

	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2811 && !al.IsServed(asoup) );

 		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.front()->GetBadge().num().span() );
	}

	TrainDescr const td(std::to_wstring(2811));
	SpotDetailsCPtr spotDetails(new SpotDetails(td));
	auto tEvent = time_from_iso("20220726T053301Z");
	CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
		std::make_shared<SpotEvent>(HCode::INFO_CHANGING, BadgeE(L"1N1IP", EsrKit(11042,16170)), tEvent, spotDetails ), 1, TrainCharacteristics::Source::Guess));
	m_chart->updateTime( tEvent, nullptr );


	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2811 && al.IsServed(asoup) );

		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.front()->GetBadge().num()==EsrKit(16170) );
	}
}


void TC_TrackedChart::CheckAsoup()
{
	set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");

	std::wstring asoup11420 = L"(:1042 909/000+11420 8221 0001 422 0918 03 09180 26 07 12 04 01/04 0 0/00 00 0\n"
		L"Ю3 935 00003800 1 07 00 0000 00000 CAKSA       \n"
		L"Ю3 934 00027330 2 04 00 0000 00000 TVARDOVSKIS :)";

	auto tEvent = time_from_iso("20220726T090400Z");
	CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11420, boost::gregorian::date(2022, 07, 26), nullptr,  tEvent));
	m_chart->updateTime(tEvent, nullptr);

	auto pathSrc = loaderSpot(            
		L"<HemPath>"																				
		L"<SpotEvent create_time='20220726T090345Z' name='Form' Bdg='4C[11420]' waynum='4' parknum='1' />"
		L"<SpotEvent create_time='20220726T090345Z' name='Departure' Bdg='4C[11420]' waynum='4' parknum='1' />"
		L"</HemPath>"
		);

	for ( auto spot : pathSrc )
		CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(spot, 1, TrainCharacteristics::Source::Guess));
	m_chart->updateTime(time_from_iso("20220726T090345Z"), nullptr);

	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		CPPUNIT_ASSERT( !path->GetFirstFragment() );

		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==8221 && !al.IsServed(asoup) );
	}

	tEvent = time_from_iso("20220726T090600Z");
	SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(ParkWayKit(1), std::make_pair(PicketingInfo(), PicketingInfo()));
	SpotPtr spot = std::make_shared<SpotEvent>(HCode::SPAN_MOVE, BadgeE(L"DK11C", EsrKit(9190,11420)), tEvent, spotDetails );

	CPPUNIT_ASSERT(m_chart->acceptTrackerEvent( spot, 1, TrainCharacteristics::Source::Guess));
	m_chart->updateTime(tEvent, nullptr);

	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==8221 && al.IsServed(asoup) );

		// 		HappenLayer& hl = m_chart->getHappenLayer();
		// 		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		// 		auto pathSet = container->getAllPathes();
		// 		CPPUNIT_ASSERT( pathSet.size() == 1 );
		// 		auto path = *pathSet.cbegin();
		// 		auto vEvents = path->GetAllEvents();
		// 		CPPUNIT_ASSERT( vEvents.front()->GetBadge().num()==EsrKit(16170) );
	}

	set_TimeZone("");
}


void TC_TrackedChart::AutoGlueFromUncontrolToNearSpan()
{
	set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
	Uncontrolled_helper();
	TrainDescr tdr(L"2438", L"0982-015-1800");
	auto departureTime = time_from_iso("20190212T164500Z");
	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::DEPARTURE, BadgeE(L"Bdg_A", EsrKit(9820)), ParkWayKit(8,14), departureTime, tdr), departureTime+30 ));

	auto pathSrc = loaderSpot(            
		L"<HemPath>"																				
		L"<SpotEvent create_time='20190212T165219Z' name='Form' Bdg='PV1IP[09813:09820]' waynum='1' />"
		L"<SpotEvent create_time='20190212T165219Z' name='Station_entry' Bdg='PVDP[09813]' />"
		L"</HemPath>"
		);

	for ( auto spot : pathSrc )
		CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(spot, 1, TrainCharacteristics::Source::Guess));
	m_chart->updateTime(time_from_iso("20190212T165219Z"), nullptr);

	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		CPPUNIT_ASSERT( !path->GetFirstFragment() );
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( (*vEvents.back()<*vEvents.front())==false );

		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2438 && !al.IsServed(asoup) );
	}

	SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>( tdr );
	auto spot = std::make_shared<SpotEvent>( HCode::INFO_CHANGING, BadgeE(L"PV1IP", EsrKit(9813,9820)), time_from_iso("20190212T165221Z"), spotDetails);



	m_chart->acceptTrackerEvent(spot, 1, TrainCharacteristics::Source::Guess);
	m_chart->updateTime(time_from_iso("20190212T165221Z"), nullptr);

	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		CPPUNIT_ASSERT( path->GetFirstFragment() );

		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2438 && al.IsServed(asoup) );
	}

	set_TimeZone("");
}


void TC_TrackedChart::AsoupAttemptWithIndexChangeOnFirstDeparture_ReservedLoco()
{
	time_t curtime = timeForDate(2016, 01, 01);
	time_t departure_time = curtime + 10;
	time_t transition_timeB = departure_time + 20;

	{
		const ParkWayKit c_parkWay(1, 1);

		// Строгая идентификация с первого раза
		SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
			c_parkWay, std::make_pair(PicketingInfo(), PicketingInfo()));

		const TrainDescr number(L"4404", L"0001-045-0918");
		SpotDetails dd(number);
		dd.parkway = c_parkWay;
		
		SpotDetailsCPtr formDetails = std::make_shared<SpotDetails>(dd);

		SpotPtr pathEvents[] = { std::make_shared<SpotEvent>( HCode::DEPARTURE, c_stationA, departure_time, formDetails ),
			std::make_shared<SpotEvent>( HCode::TRANSITION, c_stationB, transition_timeB, spotDetails ),
			std::make_shared<SpotEvent>( HCode::DEATH, c_stationB, transition_timeB, spotDetails ),
		};

		CPPUNIT_ASSERT(CreatePath(*m_chart, pathEvents, 64));
		CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
		CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::DEPARTURE, c_stationA,
			c_parkWay, departure_time-82, TrainDescr(L"4404", L"001-046-0918"))));
		CPPUNIT_ASSERT_EQUAL(size_t(1), GetAsoupTrioCount());

		CPPUNIT_ASSERT_EQUAL(size_t(1), GetHappenTrioCount());
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		CPPUNIT_ASSERT( path->GetFirstFragment() );
		auto new_number = *path->GetFirstFragment();
		CPPUNIT_ASSERT( number!=new_number);
	}
}

void TC_TrackedChart::GlueAfterIdentify()
{
	set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
	setupEsrGuide();
	Uncontrolled_helper();


	CPPUNIT_ASSERT( PassAsoupRawText(*m_chart, L"(:1042 909/000+09500 4882 0001 339 0964 03 09650 18 11 12 35 01/15 0 0/00 00 0\n"
		L"Ю3 557 00036720 1 11 00 0000 00000 DUSHACKIS   :)",  boost::gregorian::date(2022, 11, 18) ));

	auto pathSrc = loaderSpot(            
		L"<HemPath>"																				
		L"<SpotEvent create_time='20221118T103537Z' name='Form' Bdg='85-91SP:85+,87-[09500]' />"
		L"<SpotEvent create_time='20221118T103537Z' name='Departure' Bdg='85-91SP:85+,87-[09500]' />"
		L"<SpotEvent create_time='20221118T103903Z' name='Transition' Bdg='211SP+[09042]'>"
		L"<rwcoord picketing1_val='5~500' picketing1_comm='Земитани' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221118T103933Z' name='Disform' Bdg='207SP+[09042]'>"
		L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
		L"</SpotEvent>"
		L"</HemPath>"
		);

	CPPUNIT_ASSERT( CreatePath(*m_chart, pathSrc, 1) );

	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::ARRIVAL, BadgeE(L"5C", EsrKit(9640)),
		ParkWayKit(1, 1), time_from_iso("20221118T105000Z"), TrainDescr(L"4882", L"001-339-0964"))));

	HemHelpful::AsoupEventPtr asoup9500;
	HemHelpful::AsoupEventPtr asoup9640;
	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==2 );
		asoup9500 = pathAsoupList.front();
		asoup9640 = pathAsoupList.back();
		if ( asoup9500->GetBadge().num()!=EsrKit(9500) )
			swap(asoup9500, asoup9640);
		CPPUNIT_ASSERT ( asoup9500->GetNumber()==4882 && !al.IsServed(asoup9500) );
		CPPUNIT_ASSERT ( asoup9640->GetNumber()==4882 && !al.IsServed(asoup9640) );

		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.size()==4 && !path->GetFirstFragment() );
	}

	 {
		 Hem::Bill bill = createBill(
			 L"<A2F_HINT Hint='cmd_edit' issue_moment='20221118T120000Z'>"
			 L"<PathEvent name='Disform' Bdg='207SP+[09042]' create_time='20221118T103933Z' />"
			 L"<Action code='Identify' SaveLog='Y' District='09000-09640' DistrictRight='Y'>"
			 L"<EventInfo layer='asoup' Bdg='ASOUP 1042[09500]' name='Departure' create_time='20221118T103500Z' index='0001-339-0964' num='4882' reslocoutbnd='Y' waynum='15' parknum='1'>"
             L"<![CDATA[(:1042 909/000+09500 4882 0001 339 0964 03 09650 18 11 12 35 01/15 0 0/00 00 0\n"
			 L"Ю3 557 00036720 1 11 00 0000 00000 DUSHACKIS   :)]]><Locomotive Series='ЧМЭ3' NumLoc='3672' Consec='1' CarrierCode='4'>"
			 L"<Crew EngineDriver='DUSHACKIS' Tim_Beg='2022-11-18 11:00' />"
			 L"</Locomotive>"
			 L"</EventInfo>"
			 L"</Action>"
			 L"</A2F_HINT>"
			 );

		 std::function<bool(const EsrKit&, const ParkWayKit&)> fIsPassengerWay = [](const EsrKit&, const ParkWayKit&) -> bool {
			 return true;
		 };
		 HappenLayer& hl = m_chart->getHappenLayer();
		 Hem::AccessHappenAsoupRegulatory access(hl, m_chart->getAsoupLayer(), m_chart->getRegulatoryLayer());
		 Context context(bill.getChance(), m_chart->getEsrGuide(), m_chart->getGuessTransciever(), m_chart->getUncontrolledStations(), identifyCategory, m_chart->getNsiBasis(), m_chart->getTopology(), nullptr, 0); 
		 Hem::aeUserIdentify applier(std::move(access), m_chart->getAsoupQueueMgr(), context, bill, fIsPassengerWay);
		 CPPUNIT_ASSERT_NO_THROW(applier.Action());
	 }

	 {
		 HappenLayer& hl = m_chart->getHappenLayer();
		 HappenLayer::ReadAccessor container = hl.GetReadAccess();
		 auto pathSet = container->getAllPathes();
		 CPPUNIT_ASSERT( pathSet.size() == 1 );
		 auto path = *pathSet.cbegin();
		 auto vEvents = path->GetAllEvents();
		 CPPUNIT_ASSERT( vEvents.size()==4 && path->GetFirstFragment() );
	 }

	 m_chart->updateTime(Hem::aeAsoupToSpotFind::getSecondaryAttemptTime( time_from_iso("20221118T105000Z") ), nullptr);

	 {
		 AsoupLayer& al = m_chart->getAsoupLayer();
		 auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		 CPPUNIT_ASSERT( pathAsoupList.size()==2 );
		 auto asoupFirst = pathAsoupList.front();
		 auto asoupSecond = pathAsoupList.back();
		 CPPUNIT_ASSERT ( al.IsServed(asoupFirst) );
		 CPPUNIT_ASSERT ( al.IsServed(asoupSecond) );

		 HappenLayer& hl = m_chart->getHappenLayer();
		 HappenLayer::ReadAccessor container = hl.GetReadAccess();
		 auto pathSet = container->getAllPathes();
		 CPPUNIT_ASSERT( pathSet.size() == 1 );
		 auto path = *pathSet.cbegin();
		 auto vEvents = path->GetAllEvents();
		 CPPUNIT_ASSERT( vEvents.size()==6 );
		 auto spot = vEvents.back();
		 CPPUNIT_ASSERT( spot->GetBadge()==asoup9640->GetBadge() && spot->GetTime()==asoup9640->GetTime());
	 }

	 set_TimeZone("");
}

void TC_TrackedChart::IdentifyReserveLocoByEarlierAsoupDeparture()
{
	set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
	auto path = loaderSpot( 
		L"<HemPath>"
		L"<SpotEvent create_time='20221117T221200Z' name='Departure' Bdg='ASOUP 1042_1p2w[11272]' waynum='2' parknum='1' index='0001-094-1131' num='4512' reslocfast='Y'>"
		L"<Locomotive Series='2М62УМ' NumLoc='90' Consec='1' CarrierCode='4'>"
		L"<Crew EngineDriver='M-BOVKUNS' Tim_Beg='2022-11-17 23:20' />"
		L"</Locomotive>"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20221117T222420Z' name='Arrival' Bdg='9C[11310]' waynum='9' parknum='1' />"
		L"</HemPath>" );

	unsigned const trainID = 64;
	HappenLayer& hl = m_chart->getHappenLayer();
	CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==2 );
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 1 );
		CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 4512 );
	}

	//Asoup Departure
	std::wstring asoup11310 =L"(:1042 909/000+11310 2057 1131 765 0987 03 11321 18 11 01 30 01/09 0 0/00 00 0:)";


	CPPUNIT_ASSERT(m_chart->processNewAsoupText(asoup11310,
		boost::gregorian::date(2022, 11, 18), nullptr, 0) );
	m_chart->updateTime( time_from_iso("20221117T232000"), nullptr );

	AsoupLayer& al = m_chart->getAsoupLayer();
	{
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
		auto asoup = *pathAsoupList.cbegin();
		CPPUNIT_ASSERT( !al.IsServed( asoup ) );
	}

	SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
		ParkWayKit(1, 9), std::make_pair(PicketingInfo(), PicketingInfo()));
	auto spot = std::make_shared<SpotEvent>( HCode::DEPARTURE, BadgeE(L"9C", EsrKit(11310)), time_from_iso("20221117T232532Z"), spotDetails);

	m_chart->acceptTrackerEvent(spot, trainID, TrainCharacteristics::Source::Guess);
	spot = std::make_shared<SpotEvent>( HCode::SPAN_MOVE, BadgeE(L"6/701P", EsrKit(11310,11321)), time_from_iso("20221117T232837Z"), nullptr );
	m_chart->acceptTrackerEvent(spot, trainID, TrainCharacteristics::Source::Guess);

	m_chart->updateTime(time_from_iso("20221117T233032Z"), nullptr);

	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 2 );
		auto pathCreate = *pathSet.cbegin();
		auto pathSecond = *pathSet.rbegin();
		if ( pathCreate->GetFirstEvent()->GetCode()!=HCode::DEPARTURE )
			swap(pathCreate, pathSecond);
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 1 );
		CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 4512 );
		gis = pathSecond->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 1 );
		CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 2057 );
	}

	set_TimeZone("");
}

void TC_TrackedChart::GlueOnFromBorderSpanToUncontrol()
{
	Uncontrolled_helper();
	auto path = loaderSpot( 
		L"<HemPath>"
		L"<SpotEvent create_time='20220502T140007Z' name='Form' Bdg='UU2C[09803:09810]' waynum='1' index='' num='V2787' through='Y'>"
		L"<rwcoord picketing1_val='29~905' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='30~700' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220502T140233Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' optCode='09810:09812' />"
		L"<SpotEvent create_time='20220502T142203Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' optCode='09812:09813' />"
		L"<SpotEvent create_time='20220502T142959Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' />"
		L"</HemPath>" );

	unsigned const trainID = 64;
	HappenLayer& hl = m_chart->getHappenLayer();
	CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==4 );
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 1 );
		CPPUNIT_ASSERT( gis.back().second->GetNumber().getNumber() == 2787 );
	}

	SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
		ParkWayKit(1), std::make_pair(PicketingInfo(), PicketingInfo()));
	CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
		std::make_shared<SpotEvent>(HCode::SPAN_MOVE, BadgeE(L"PMN1IP", EsrKit(9813, 9840)), time_from_iso("20220502T143058Z"), spotDetails), trainID, TrainCharacteristics::Source::Guess));
	CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
		std::make_shared<SpotEvent>(HCode::DEATH, BadgeE(L"PMN1IP", EsrKit(9813, 9840)), time_from_iso("20220502T143517Z"), spotDetails), trainID, TrainCharacteristics::Source::Guess));
	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==6 );
	}

	auto asoup = std::make_shared<AsoupEvent>(HCode::ARRIVAL, BadgeE(L"xx", EsrKit(9840)),
		ParkWayKit(2,8), time_from_iso("20220502T144100Z"), TrainDescr(L"2787", L"1100-318-0983"));
	CPPUNIT_ASSERT( asoup );
	CPPUNIT_ASSERT(PassAsoup(*m_chart, asoup));
	 
	AsoupLayer& al = m_chart->getAsoupLayer();
	{
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
		auto asoup = *pathAsoupList.cbegin();
		CPPUNIT_ASSERT( al.IsServed( asoup ) );

		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==7 );
		CPPUNIT_ASSERT( pathCreate->GetLastEvent()->GetBadge().num()==EsrKit(9840));
	}
}


void TC_TrackedChart::IdentifyControlStatWithChangeIndex()
{
	set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
	Uncontrolled_helper();
	auto path = loaderSpot( 
		L"<HemPath>"
		L"<SpotEvent create_time='20221109T130500Z' name='Transition' Bdg='ASOUP 1042?p1w2[09280]' waynum='2' parknum='1' optCode='09180:09280' num='J8218' stone='Y' />"
		L"<SpotEvent create_time='20221109T132138Z' name='Arrival' Bdg='3C[09180]' waynum='3' parknum='1' />"
		L"</HemPath>" );

	unsigned const trainID = 64;
	HappenLayer& hl = m_chart->getHappenLayer();
	CPPUNIT_ASSERT( CreatePath(*m_chart, path, trainID) );
	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==2 );
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 1 );
		auto tdr = gis.back().second;
		CPPUNIT_ASSERT( tdr->GetNumber().getNumber() == 8218 && tdr->GetIndex().empty() );
	}

	auto asoupArrival = std::make_shared<AsoupEvent>(HCode::ARRIVAL, BadgeE(L"xx", EsrKit(9180)),
		ParkWayKit(1,3), time_from_iso("20221109T132100Z"), TrainDescr(L"8218", L"001-086-0918"));
	CPPUNIT_ASSERT( asoupArrival );
	CPPUNIT_ASSERT(PassAsoup(*m_chart, asoupArrival));

	AsoupLayer& al = m_chart->getAsoupLayer();
	{
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
		auto asoup = *pathAsoupList.cbegin();
		CPPUNIT_ASSERT( al.IsServed( asoup ) );

		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 1 );
		auto tdr = gis.back().second;
		CPPUNIT_ASSERT( tdr->GetNumber().getNumber() == 8218 && tdr->GetIndex()==Trindex(L"001-086-0918") );
	}


	auto asoupDeparture = std::make_shared<AsoupEvent>(HCode::DEPARTURE, BadgeE(L"xx", EsrKit(9180)),
		ParkWayKit(1,3), time_from_iso("20221109T132500Z"), TrainDescr(L"8218", L"001-089-1100"));
	CPPUNIT_ASSERT( asoupDeparture );
	CPPUNIT_ASSERT(PassAsoup(*m_chart, asoupDeparture));
	{
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
		auto asoup1 = *pathAsoupList.cbegin();
		auto asoup2 = *pathAsoupList.crbegin();
		CPPUNIT_ASSERT( al.IsServed( asoup1 )^al.IsServed( asoup2 ) );	//Arrival seved, Departure not served

		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 1 );
		auto tdr = gis.back().second;
		CPPUNIT_ASSERT( tdr->GetNumber().getNumber() == 8218 && tdr->GetIndex()==Trindex(L"001-086-0918") );
	}


	SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
		ParkWayKit(1,3), std::make_pair(PicketingInfo(), PicketingInfo()));
	CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
		std::make_shared<SpotEvent>(HCode::DEPARTURE, BadgeE(L"PMN1IP", EsrKit(9180)), time_from_iso("20221109T132646Z"), spotDetails), trainID, TrainCharacteristics::Source::Guess));
	{
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==3 );
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 1 );
		auto tdr = gis.back().second;
		CPPUNIT_ASSERT( tdr->GetNumber().getNumber() == 8218 && tdr->GetIndex()==Trindex(L"001-086-0918") );
	}

	CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(
		std::make_shared<SpotEvent>(HCode::SPAN_MOVE, BadgeE(L"xx", EsrKit(9180, 9181)), time_from_iso("20221109T132828Z"), spotDetails), trainID, TrainCharacteristics::Source::Guess));

	m_chart->updateTime(time_from_iso("20221109T132828Z"), nullptr);
	{
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
		for ( auto asoup : pathAsoupList )
			CPPUNIT_ASSERT( al.IsServed( asoup ) );

		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1  );
		auto pathCreate = *pathSet.cbegin();
		CPPUNIT_ASSERT( pathCreate && pathCreate->GetEventsCount()==4 );
		auto gis = pathCreate->GetInfoSpots();
		CPPUNIT_ASSERT( gis.size() == 2 );
		auto tdr = gis.back().second;
		CPPUNIT_ASSERT( tdr->GetNumber().getNumber() == 8218 && tdr->GetIndex()==Trindex(L"001-089-1100") );
	}
	set_TimeZone("");
}

void TC_TrackedChart::LinkFromBorderUncStatToControlZone_7081()
{
	setupEsrGuide();
	Uncontrolled_helper();

	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::EXPLICIT_FORM, BadgeE(L"5C", EsrKit(9820)),
		ParkWayKit(), time_from_iso("20221229T110300Z"), TrainDescr(L"2024", L"0982-249-1800"))));

	m_chart->updateTime(time_from_iso("20221229T110340Z"), nullptr);

	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::DEPARTURE, BadgeE(L"17C", EsrKit(9820)),
		ParkWayKit(8, 17), time_from_iso("20221229T181200Z"), TrainDescr(L"2024", L"0982-249-1800"))));

	m_chart->updateTime(time_from_iso("20221229T181240Z"), nullptr);
	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==2 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2024 && al.IsServed(asoup) );
		asoup = pathAsoupList.back();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2024 && al.IsServed(asoup) );

		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.size()==3 );
	}
 
	auto pathSrc = loaderSpot(            
		L"<HemPath>"																				
		L"<SpotEvent create_time='20221229T182426Z' name='Form' Bdg='PV1IP[09813:09820]' waynum='1' num='V2024'/>"
		L"<SpotEvent create_time='20221229T182426Z' name='Station_entry' Bdg='PVDP[09813]' waynum='1' />"
		L"</HemPath>"
		);

	for ( auto spot : pathSrc )
		CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(spot, 1, TrainCharacteristics::Source::Guess));

	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.front()->GetBadge().num().terminal() );
	}
}

void TC_TrackedChart::LinkFromBorderUncStatToControlZone_7081_1()
{
	setupEsrGuide();
	Uncontrolled_helper();

	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::EXPLICIT_FORM, BadgeE(L"5C", EsrKit(9820)),
		ParkWayKit(), time_from_iso("20221229T110300Z"), TrainDescr(L"2024", L"0982-249-1800"))));

	m_chart->updateTime(time_from_iso("20221229T110340Z"), nullptr);

	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2024 && !al.IsServed(asoup) );

		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.empty() );
	}

	auto pathSrc = loaderSpot(            
		L"<HemPath>"																				
		L"<SpotEvent create_time='20221229T182426Z' name='Form' Bdg='PV1IP[09813:09820]' waynum='1' num='V2024'/>"
		L"<SpotEvent create_time='20221229T182426Z' name='Station_entry' Bdg='PVDP[09813]' waynum='1' />"
		L"</HemPath>"
		);

	for ( auto spot : pathSrc )
		CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(spot, 1, TrainCharacteristics::Source::Guess));

	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.front()->GetBadge().num().span() );
	}

	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::DEPARTURE, BadgeE(L"17C", EsrKit(9820)),
		ParkWayKit(8, 17), time_from_iso("20221229T181200Z"), TrainDescr(L"2024", L"0982-249-1800"))));

	m_chart->updateTime(time_from_iso("20221229T182430Z"), nullptr);

	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==2 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2024 && al.IsServed(asoup) );
		asoup = pathAsoupList.back();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2024 && al.IsServed(asoup) );

		UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( m_chart->getHappenLayer() );
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );

		CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "5C[09820]   17C[09820] PVDP[09813] "));
		CPPUNIT_ASSERT( hl.exist_series( "name", "ExplicitForm Departure  Station_entry  " ) );
	}

}

void TC_TrackedChart::LinkFromBorderUncStatToControlZone_7081_2()
{
	setupEsrGuide();
	NsiBasis_helper();
	Uncontrolled_helper();

	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::EXPLICIT_FORM, BadgeE(L"5C", EsrKit(9820)),
		ParkWayKit(), time_from_iso("20230102T114600Z"), TrainDescr(L"3014", L"0983-227-0918"))));

	m_chart->updateTime(time_from_iso("20230102T114640Z"), nullptr);

	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==1 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==3014 && !al.IsServed(asoup) );

		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.empty() );
	}

	auto pathSrc = loaderSpot(            
		L"<HemPath>"																				
		L"<SpotEvent create_time='20230102T210853Z' name='Form' Bdg='PMN1IP[09813:09840]' waynum='1' />"
		L"<SpotEvent create_time='20230102T210853Z' name='Station_entry' Bdg='PMNDP[09813]' />"
		L"</HemPath>"
		);

	unsigned trainId = 1;
	for ( auto spot : pathSrc )
		CPPUNIT_ASSERT(m_chart->acceptTrackerEvent(spot, trainId, TrainCharacteristics::Source::Guess));

	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.size()==2);
		CPPUNIT_ASSERT( !path->GetFirstFragment() );
	}

		std::wstring spotStr = L"<SpotEvent create_time='20230102T210855Z' name='Info_changing' Bdg='PMN1IP[09813:09840]' index='' num='V3014' through='Y' waynum='1' id='195' infsrc='disposition' />";

	CPPUNIT_ASSERT( m_chart->acceptEventFromStr(spotStr, trainId, TrainCharacteristics::Source::Guess, 0) );
	m_chart->updateTime(time_from_iso("20230102T210855Z"), nullptr);

	{
		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.size()==2);
		CPPUNIT_ASSERT( vEvents.front()->GetBadge().num().span() );
	}
}

void TC_TrackedChart::LinkFromBorderUncTehnodeStatToDeparture_7081_3()
{
	setupEsrGuide();
	NsiBasis_helper();
	Uncontrolled_helper();

	time_t tForm = time_from_iso("20221229T110300Z");
	time_t tDeparture = time_from_iso("20221229T181200Z");
	BadgeE bdgDeparture = BadgeE(L"17C", EsrKit(9850));
	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::EXPLICIT_FORM, BadgeE(L"xxx", EsrKit(9820)),
		ParkWayKit(), tForm, TrainDescr(L"2024", L"0982-249-1800")), tForm + 40));

	CPPUNIT_ASSERT(PassAsoup(*m_chart, std::make_shared<AsoupEvent>(HCode::DEPARTURE, bdgDeparture,
		ParkWayKit(8, 17), tDeparture, TrainDescr(L"2024", L"0982-249-1800")), tDeparture+40));

	{
		AsoupLayer& al = m_chart->getAsoupLayer();
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size()==2 );
		auto asoup = pathAsoupList.front();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2024 && al.IsServed(asoup) );
		asoup = pathAsoupList.back();
		CPPUNIT_ASSERT ( asoup->GetNumber()==2024 && al.IsServed(asoup) );

		HappenLayer& hl = m_chart->getHappenLayer();
		HappenLayer::ReadAccessor container = hl.GetReadAccess();
		auto pathSet = container->getAllPathes();
		CPPUNIT_ASSERT( pathSet.size() == 1 );
		auto path = *pathSet.cbegin();
		auto vEvents = path->GetAllEvents();
		CPPUNIT_ASSERT( vEvents.size()==3 );
		auto firstEvent = *vEvents.front();
		CPPUNIT_ASSERT( firstEvent.GetBadge()==bdgDeparture && firstEvent.GetTime()==tForm );
	}
}
