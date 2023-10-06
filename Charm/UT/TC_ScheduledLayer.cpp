#include "stdafx.h"
#include "TC_ScheduledLayer.h"

#include "UtHelpfulDateTime.h"

#include "../Guess/Msg.h"
#include "../Hem/RegulatoryXml.h"
#include "../Hem/DailyCalendar.h"
#include "../Hem/DailyEvent.h"
#include "../Hem/DailyPath.h"
#include "../Hem/ScheduledPath.h"
#include "../Hem/ScheduledLayer.h"
#include "../Hem/DailySchedule.h"
#include "../Hem/RegulatoryLayer.h"
#include "../Hem/Platforms.h"
#include "../helpful/StrToTime.h"
#include "../helpful/EsrGuide.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ScheduledLayer );

void TC_ScheduledLayer::Serialization()
{
    Regulatory::ScheduledLayer layer;

    std::string source;
    {
        const std::wstring c_source1 = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T030000Z'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='230700' />\n"
            L"        <Event esr='90100' type='departure' time='030100' />\n"
            L"        <Event esr='90064' type='arrival' time='031100' />\n"
            L"        <Event esr='90064' type='departure' time='031100' />\n"
            L"        <Event esr='90007' type='departure' time='032100' />\n"
            L"      </Thread>\n"
            L"      <Thread train='2031'>\n"
            L"        <Event esr='90064' type='arrival' time='135200' />\n"
            L"        <Event esr='90064' type='departure' time='135200' />\n"
            L"        <Event esr='90100' type='departure' time='140600' />\n"
            L"        <Event esr='91809' type='departure' time='180100' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";
        attic::a_document doc;
        doc.load_wide(c_source1);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", layer.deserialize(doc.document_element(), time_t(0)));

        source = doc.document_element().pretty_str();
    }

    {
        attic::a_document doc("Root");
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to serialize into xml", layer.serialize(doc.document_element()));
        const std::string result = doc.document_element().pretty_str();

        CPPUNIT_ASSERT_EQUAL(source, result);
    }
}

typedef std::tuple<unsigned, std::string, HCode> ExpectedEvent;

template <size_t EXPECTED_COUNT>
void ensureEvents(attic::a_node pathNode, const std::array<ExpectedEvent, EXPECTED_COUNT> expectedEventsLocal)
{
    size_t eventNumber = 0u;
    for (attic::a_node eventNode = pathNode.child(SpotEvent_xAttr);
        eventNode; eventNode = eventNode.next_sibling(SpotEvent_xAttr))
    {
        CPPUNIT_ASSERT_MESSAGE("Already too much events found", eventNumber < EXPECTED_COUNT);
        ExpectedEvent expectedEvent = expectedEventsLocal[eventNumber];

        attic::a_attribute badgeAttr = eventNode.attribute(bdg_xAttr);
        CPPUNIT_ASSERT_MESSAGE("Badge attribute was not found", badgeAttr);
        {
            std::string badgeValue = badgeAttr.as_string();
            std::string stationValue(std::find(badgeValue.begin(), badgeValue.end(), '[') + 1,
                std::find(badgeValue.begin(), badgeValue.end(), ']'));
            CPPUNIT_ASSERT_EQUAL(std::to_string(std::get<0>(expectedEvent)), stationValue);
        }

        attic::a_attribute stampAttr = eventNode.attribute(create_time_xAttr);
        CPPUNIT_ASSERT_MESSAGE("Stamp attribute was not found", stampAttr);
        std::string expectedStampLocal = std::get<1>(expectedEvent);
        boost::posix_time::ptime expectedPtime = boost::posix_time::from_iso_string(expectedStampLocal);
        expectedPtime = boost::posix_time::from_time_t(UTC_From_Local(expectedPtime));
        std::string expectedStampUtc = boost::posix_time::to_iso_string(expectedPtime) + "Z";
        CPPUNIT_ASSERT_EQUAL(expectedStampUtc, std::string(stampAttr.as_string()));

        attic::a_attribute nameAttr = eventNode.attribute(name_xAttr);
        CPPUNIT_ASSERT_MESSAGE("Name attribute was not found", nameAttr);
        CPPUNIT_ASSERT_EQUAL(HemEventRegistry::instance()->GetXMLName(std::get<2>(expectedEvent)), std::string(nameAttr.as_string()));
        ++eventNumber;
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Not enough events found", EXPECTED_COUNT, eventNumber);
}


static std::shared_ptr<attic::a_document> request2doc( Regulatory::ScheduledLayer::ScheduleSelectionMap request )
{
    std::shared_ptr<attic::a_document> pDoc;
    try
    {
        EsrGuide eg;
        pDoc = Regulatory::Layer::generatePathesDoc( request, eg );
        CPPUNIT_ASSERT_MESSAGE( "Layer failed to generate pathes", pDoc );
    }
    catch(...)
    {
        CPPUNIT_FAIL( "Layer failed to generate pathes" );
    }
    return pDoc;
}

void TC_ScheduledLayer::Generation()
{
    Regulatory::ScheduledLayer schedule;
    {
        const std::wstring c_source2 = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T060000' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='230700' />\n"
            L"        <Event esr='90100' type='departure' time='030100' />\n"
            L"        <Event esr='90064' type='arrival' time='031100' />\n"
            L"        <Event esr='90064' type='departure' time='031100' />\n"
            L"        <Event esr='90007' type='departure' time='032100' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";

        attic::a_document doc;
        doc.load_wide(c_source2);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));
    }

    auto request = schedule.selectSchedulesFor(periodForTimes(
        timeForDateTime(2015, 10, 13, 23, 00), timeForDateTime(2015, 10, 14, 04, 00)));


    {
        std::shared_ptr<attic::a_document> pDoc = request2doc( request );
        attic::a_document& doc = *pDoc;

        const std::string result = doc.document_element().pretty_str();
        CPPUNIT_ASSERT(!result.empty());

        attic::a_node layerNode = doc.document_element().child(ScheduledLayer_xAttr);
        CPPUNIT_ASSERT(layerNode);

        attic::a_node pathNode = layerNode.child(ScheduledPath_xAttr);
        CPPUNIT_ASSERT_MESSAGE("1st generated path", pathNode);

        std::array<ExpectedEvent, 6u> expectedEvents = {
            std::make_tuple(91809u, std::string("20151013T230700Z"), HCode::INFO_CHANGING),
            std::make_tuple(91809u, std::string("20151013T230700Z"), HCode::ARRIVAL),
            std::make_tuple(90100u, std::string("20151014T030100Z"), HCode::DEPARTURE),
            std::make_tuple(90064u, std::string("20151014T031100Z"), HCode::ARRIVAL),
            std::make_tuple(90064u, std::string("20151014T031100Z"), HCode::DEPARTURE),
            std::make_tuple(90007u, std::string("20151014T032100Z"), HCode::DEPARTURE)
        };

        ensureEvents(pathNode, expectedEvents);

        CPPUNIT_ASSERT_MESSAGE("2nd generated path shouldn't exist", !pathNode.next_sibling(ScheduledPath_xAttr));
    }
}

void TC_ScheduledLayer::GenerationCutting()
{
    Regulatory::ScheduledLayer schedule;
    {
        const std::wstring c_source3 = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T060000' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='100000' />\n"
            L"        <Event esr='90100' type='departure' time='110000' />\n"
            L"        <Event esr='90064' type='arrival' time='120000' />\n"
            L"        <Event esr='90064' type='departure' time='120000' />\n"
            L"        <Event esr='90007' type='departure' time='130000' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";

        attic::a_document doc;
        doc.load_wide(c_source3);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));
    }

    auto request = schedule.selectSchedulesFor(periodForTimes(
        timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 10, 13, 11, 30)));

    {
        std::shared_ptr<attic::a_document> pDoc = request2doc( request );
        attic::a_document& doc = *pDoc;

        const std::string result = doc.document_element().pretty_str();
        CPPUNIT_ASSERT(!result.empty());

        attic::a_node layerNode = doc.document_element().child(ScheduledLayer_xAttr);
        CPPUNIT_ASSERT(layerNode);

        attic::a_node pathNode = layerNode.child(ScheduledPath_xAttr);
        CPPUNIT_ASSERT_MESSAGE("1st generated path", pathNode);

        {
            std::array<ExpectedEvent, 4u> expectedEvents = {
                std::make_tuple(91809u, std::string("20151013T100000Z"), HCode::INFO_CHANGING),
                std::make_tuple(91809u, std::string("20151013T100000Z"), HCode::ARRIVAL),
                std::make_tuple(90100u, std::string("20151013T110000Z"), HCode::DEPARTURE),
                // —ледующее событие не входит в запрошенный интервал, но попадает в "нахлест" в 1 событие
                std::make_tuple(90064u, std::string("20151013T120000Z"), HCode::ARRIVAL) /*,
                // —ледующее событие должно быть обрезано, так как заказано расписание по 11.30, нахлест в 1 событие
                std::make_tuple(90064u, std::string("20151013T120000Z"), false),
                std::make_tuple(90007u, std::string("20151013T130000Z"), false)*/
            };

            ensureEvents(pathNode, expectedEvents);
        }

        CPPUNIT_ASSERT_MESSAGE("2nd generated path shouldn't exist", !pathNode.next_sibling(ScheduledPath_xAttr));
    }
}

void TC_ScheduledLayer::GenerationMerging()
{
    Regulatory::ScheduledLayer schedule;
    {
        // ƒва расписани€ с пересекающимис€ нитками одного маршрута
        const std::wstring c_source4 = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T030000Z' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='100000' />\n"
            L"        <Event esr='90100' type='departure' time='110000' />\n"
            L"        <Event esr='90064' type='arrival' time='120000' />\n"
            L"        <Event esr='90064' type='departure' time='120000' />\n"
            L"        <Event esr='90007' type='departure' time='130000' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"    <RegulatorySchedule startTime='20151013T083000Z' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='100000' />\n"
            L"        <Event esr='90100' type='departure' time='110000' />\n"
            L"        <Event esr='90064' type='arrival' time='120500' />\n"
            L"        <Event esr='90064' type='departure' time='120500' />\n"
            L"        <Event esr='90007' type='departure' time='130500' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";

        attic::a_document doc;
        doc.load_wide(c_source4);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));
    }

    auto request = schedule.selectSchedulesFor(periodForTimes(
        timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 10, 13, 14, 00)));

    {
        std::shared_ptr<attic::a_document> pDoc = request2doc( request );
        attic::a_document& doc = *pDoc;

        const std::string result = doc.document_element().pretty_str();
        CPPUNIT_ASSERT(!result.empty());

        attic::a_node layerNode = doc.document_element().child(ScheduledLayer_xAttr);
        CPPUNIT_ASSERT(layerNode);

        attic::a_node pathNode = layerNode.child(ScheduledPath_xAttr);
        CPPUNIT_ASSERT_MESSAGE("1st generated path", pathNode);

        std::array<ExpectedEvent, 6u> expectedEvents = {
            // Ќитка начинаетс€ сгенерированными на основании первого расписани€ событи€ми
            std::make_tuple(91809u, std::string("20151013T100000Z"), HCode::INFO_CHANGING),
            std::make_tuple(91809u, std::string("20151013T100000Z"), HCode::ARRIVAL),
            std::make_tuple(90100u, std::string("20151013T110000Z"), HCode::DEPARTURE),
            // Ќитка продолжаетс€ сгенерированными на основании второго расписани€ событи€ми
            std::make_tuple(90064u, std::string("20151013T120500Z"), HCode::ARRIVAL),
            std::make_tuple(90064u, std::string("20151013T120500Z"), HCode::DEPARTURE),
            std::make_tuple(90007u, std::string("20151013T130500Z"), HCode::DEPARTURE)
        };

        ensureEvents(pathNode, expectedEvents);

        CPPUNIT_ASSERT_MESSAGE("2nd generated path shouldn't exist", !pathNode.next_sibling(ScheduledPath_xAttr));
    }

}


void TC_ScheduledLayer::GenerationAdding()
{
    Regulatory::ScheduledLayer schedule;
    {
        // ƒва расписани€ с пересекающимис€ событи€ми в разных маршрутах
        const std::wstring c_source5 = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T030000Z' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='100000' />\n"
            L"        <Event esr='90100' type='departure' time='110000' />\n"
            L"        <Event esr='90064' type='arrival' time='120000' />\n"
            L"        <Event esr='90064' type='departure' time='120000' />\n"
            L"        <Event esr='90007' type='departure' time='130000' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"    <RegulatorySchedule startTime='20151013T083000Z' creationDate='20140605'>\n"
            L"      <Thread train='1048'>\n"
            L"        <Event esr='91809' type='arrival' time='100000' />\n"
            L"        <Event esr='90100' type='departure' time='110000' />\n"
            L"        <Event esr='90064' type='arrival' time='120500' />\n"
            L"        <Event esr='90064' type='departure' time='120500' />\n"
            L"        <Event esr='90007' type='departure' time='130500' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";

        attic::a_document doc;
        doc.load_wide(c_source5);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));
    }

    auto request = schedule.selectSchedulesFor(periodForTimes(
        timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 10, 13, 14, 00)));

    {
        std::shared_ptr<attic::a_document> pDoc = request2doc( request );
        attic::a_document& doc = *pDoc;

        const std::string result = doc.document_element().pretty_str();
        CPPUNIT_ASSERT(!result.empty());

        attic::a_node layerNode = doc.document_element().child(ScheduledLayer_xAttr);
        CPPUNIT_ASSERT(layerNode);

        attic::a_node pathNode = layerNode.child(ScheduledPath_xAttr);
        CPPUNIT_ASSERT_MESSAGE("1st generated path", pathNode);

        pathNode = pathNode.next_sibling(ScheduledPath_xAttr);
        CPPUNIT_ASSERT_MESSAGE("2nd generated path", pathNode);

        CPPUNIT_ASSERT_MESSAGE("3rd generated path shouldn't exist", !pathNode.next_sibling(ScheduledPath_xAttr));
    }

}

void TC_ScheduledLayer::SerializationAfterGeneration()
{
    Regulatory::ScheduledLayer schedule;
    {
        // ƒва расписани€ с пересекающимис€ событи€ми в разных маршрутах
        const std::wstring c_source5 = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T060000' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='100000' />\n"
            L"        <Event esr='90100' type='departure' time='110000' />\n"
            L"        <Event esr='90064' type='arrival' time='120000' />\n"
            L"        <Event esr='90064' type='departure' time='120000' />\n"
            L"        <Event esr='90007' type='departure' time='130000' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";

        attic::a_document doc;
        doc.load_wide(c_source5);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));
    }

    auto request = schedule.selectSchedulesFor(periodForTimes(
        timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 10, 13, 14, 00)));

    Regulatory::Layer layer;
    {
        std::shared_ptr<attic::a_document> pDoc = request2doc( request );
        layer.deserialize( pDoc->document_element() );
    }

    {
        attic::a_document doc("Changes");
        CPPUNIT_ASSERT(layer.takeChanges(doc.document_element()));
        const std::string result = doc.document_element().pretty_str();
        CPPUNIT_ASSERT(!result.empty());
    }

    {
        attic::a_document doc("Root");
        
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to serialize into xml", layer.serialize(doc.document_element()));
        const std::string result = doc.document_element().pretty_str();

        CPPUNIT_ASSERT(!result.empty());
        //CPPUNIT_ASSERT_EQUAL(source, result);
    }   
}

void TC_ScheduledLayer::Serialization2()
{
    Regulatory::ScheduledLayer schedule;

    {
        const std::wstring c_source = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20150101T060000' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='departure' time='230700' />\n"
            L"        <Event esr='90100' type='transition' time='030100' />\n"
            L"        <Event esr='90064' type='arrival' time='031100' />\n"
            L"        <Event esr='90064' type='departure' time='031100' />\n"
            L"        <Event esr='90007' type='arrival' time='032100' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";

        attic::a_document doc;
        doc.load_wide(c_source);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));
    }

    auto request = schedule.selectSchedulesFor(periodForTimes(timeForDateTime(2015, 8, 30, 10, 00), timeForDateTime(2015, 8, 31, 10, 00)));
    Regulatory::Layer layer;

    {
        std::shared_ptr<attic::a_document> pDoc = request2doc( request );
        layer.deserialize( pDoc->document_element() );
    }

    {
        attic::a_document doc("tmp");
        layer.takeChanges(doc.document_element());
    }

    {
        attic::a_document doc("export");
        layer.serialize(doc.document_element());

        std::string docStr = doc.pretty_str();
        CPPUNIT_ASSERT(!docStr.empty());

        attic::a_node layerNode = doc.document_element().child(ScheduledLayer_xAttr);
        CPPUNIT_ASSERT(layerNode);

        attic::a_node pathNode = layerNode.child(ScheduledPath_xAttr);
        CPPUNIT_ASSERT_MESSAGE("1st generated path", pathNode);

        std::array<ExpectedEvent, 6u> expectedEvents = {
            // Ќитка начинаетс€ сгенерированными на основании первого расписани€ событи€ми
            std::make_tuple(91809u, std::string("20150830T230700Z"), HCode::INFO_CHANGING),
            std::make_tuple(91809u, std::string("20150830T230700Z"), HCode::DEPARTURE),
            std::make_tuple(90100u, std::string("20150831T030100Z"), HCode::TRANSITION),
            std::make_tuple(90064u, std::string("20150831T031100Z"), HCode::ARRIVAL),
            std::make_tuple(90064u, std::string("20150831T031100Z"), HCode::DEPARTURE),
            std::make_tuple(90007u, std::string("20150831T032100Z"), HCode::ARRIVAL)
        };

        ensureEvents(pathNode, expectedEvents);
    }    

    std::string firstIteration = "";
    {
        attic::a_document doc("Root");
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to serialize into xml", layer.serialize(doc.document_element()));
        firstIteration = doc.document_element().pretty_str();

        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", layer.deserialize(doc.document_element()));
    }

    {
        attic::a_document doc("tmp");
        layer.takeChanges(doc.document_element());
    }

    std::string secondIteration = "";
    {
        attic::a_document doc("Root");
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to serialize into xml", layer.serialize(doc.document_element()));
        secondIteration = doc.document_element().pretty_str();
    }

    CPPUNIT_ASSERT_EQUAL(firstIteration, secondIteration);

    {
        attic::a_document doc("export");
        layer.serialize(doc.document_element());

        std::string docStr = doc.pretty_str();
        CPPUNIT_ASSERT(!docStr.empty());

        attic::a_node layerNode = doc.document_element().child(ScheduledLayer_xAttr);
        CPPUNIT_ASSERT(layerNode);

        attic::a_node pathNode = layerNode.child(ScheduledPath_xAttr);
        CPPUNIT_ASSERT_MESSAGE("1st generated path", pathNode);

        std::array<ExpectedEvent, 6u> expectedEvents = {
            // Ќитка начинаетс€ сгенерированными на основании первого расписани€ событи€ми
            std::make_tuple(91809u, std::string("20150830T230700Z"), HCode::INFO_CHANGING),
            std::make_tuple(91809u, std::string("20150830T230700Z"), HCode::DEPARTURE),
            std::make_tuple(90100u, std::string("20150831T030100Z"), HCode::TRANSITION),
            std::make_tuple(90064u, std::string("20150831T031100Z"), HCode::ARRIVAL),
            std::make_tuple(90064u, std::string("20150831T031100Z"), HCode::DEPARTURE),
            std::make_tuple(90007u, std::string("20150831T032100Z"), HCode::ARRIVAL)
        };

        ensureEvents(pathNode, expectedEvents);
    } 
}

std::shared_ptr<Regulatory::DailySchedule> CreateSimpleSchedule()
{  
    const std::wstring c_source = 
        L"    <Tmp>\n"
        L"      <Thread train='2024'>\n"
        L"        <Event esr='91809' type='arrival' time='100000' />\n"
        L"        <Event esr='90100' type='departure' time='110000' />\n"
        L"        <Event esr='90064' type='arrival' time='120000' />\n"
        L"        <Event esr='90064' type='departure' time='120000' />\n"
        L"        <Event esr='90007' type='departure' time='130000' />\n"
        L"      </Thread>\n"
        L"    </Tmp>\n";
    attic::a_document doc;
    doc.load_wide(c_source);
    Regulatory::DailyPath dailyPath(doc.document_element().first_child());

    auto schedule = std::make_shared<Regulatory::DailySchedule>();
    schedule->addPath(2024, std::move(dailyPath));
    return schedule;
}

void TC_ScheduledLayer::TrimPlanned()
{
    using namespace Regulatory;

    ScheduledLayer schedule;

    {
        // TODO: ѕосле реализаци€ вариантных расписаний следует добавить новый тест с ними
        const std::wstring c_source = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T060000' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='100000' />\n"
            L"        <Event esr='90100' type='departure' time='110000' />\n"
            L"        <Event esr='90064' type='arrival' time='120000' />\n"
            L"        <Event esr='90064' type='departure' time='120000' />\n"
            L"        <Event esr='90007' type='departure' time='130000' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"    <RegulatorySchedule startTime='20151113T113000' creationDate='20151013'>\n"
            L"      <Thread train='1048'>\n"
            L"        <Event esr='91809' type='arrival' time='100000' />\n"
            L"        <Event esr='90100' type='departure' time='110000' />\n"
            L"        <Event esr='90064' type='arrival' time='120500' />\n"
            L"        <Event esr='90064' type='departure' time='120500' />\n"
            L"        <Event esr='90007' type='departure' time='130500' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";

        attic::a_document doc;
        doc.load_wide(c_source);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));

        auto request = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 11, 23, 14, 00)));
        CPPUNIT_ASSERT_EQUAL(size_t(2), request.size());
    }

    // ѕопытка удалить расписани€ тогда, когда ни одно из них ещЄ не стало действующим
    // (Ѕлижайшее начнЄт действовать с 13 окт.)
    size_t trimmedCount = schedule.trim(ptimeForDate(2015, 10, 11));
    CPPUNIT_ASSERT_EQUAL(size_t(0), trimmedCount);
    {
        auto request = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 11, 23, 14, 00)));
        CPPUNIT_ASSERT_EQUAL(size_t(2), request.size());
    }

    // ѕопытка удалени€ действующего расписани€ (с 13 окт. по 13 но€.)
    trimmedCount = schedule.trim(ptimeForDate(2015, 11, 10));
    CPPUNIT_ASSERT_EQUAL(size_t(0), trimmedCount);
    {
        auto request = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 11, 23, 14, 00)));
        CPPUNIT_ASSERT_EQUAL(size_t(2), request.size());
    }

    // ”даление устаревшего расписани€ (с 13 окт. по 13 но€.)
    trimmedCount = schedule.trim(ptimeForDate(2015, 11, 14));
    CPPUNIT_ASSERT_EQUAL(size_t(1), trimmedCount);
    {
        auto request = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 11, 23, 14, 00)));
        CPPUNIT_ASSERT_EQUAL(size_t(1), request.size());
    }

    // ѕовтор€ем процедуру удалени€ расписани€ на ту же дату (ничего не должно изменитьс€)
    trimmedCount = schedule.trim(ptimeForDate(2015, 11, 14));
    CPPUNIT_ASSERT_EQUAL(size_t(0), trimmedCount);
    {
        auto request = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 11, 23, 14, 00)));
        CPPUNIT_ASSERT_EQUAL(size_t(1), request.size());
    }

    // ѕопытка удалени€ действующего расписани€ (с 13 но€.)
    trimmedCount = schedule.trim(ptimeForDate(2015, 12, 10));
    CPPUNIT_ASSERT_EQUAL(size_t(0), trimmedCount);
    {
        auto request = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime(2015, 10, 13, 9, 30), timeForDateTime(2015, 11, 23, 14, 00)));
        CPPUNIT_ASSERT_EQUAL(size_t(1), request.size());
    }
}

void TC_ScheduledLayer::EliminatePlatforms()
{
    Regulatory::ScheduledLayer schedule;
    {
        const std::wstring c_source2 = 
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T060000' creationDate='20140605'>\n"
            L"      <Thread train='2024'>\n"
            L"        <Event esr='91809' type='arrival' time='230700' />\n"
            L"        <Event esr='90100' type='departure' time='030100' />\n"
            L"        <Event esr='90064' type='arrival' time='031100' />\n"
            L"        <Event esr='90064' type='departure' time='031100' />\n"
            L"        <Event esr='90007' type='departure' time='032100' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n";

        attic::a_document doc;
        doc.load_wide(c_source2);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));
    }

    auto request = schedule.selectSchedulesFor(periodForTimes(
        timeForDateTime(2015, 10, 13, 23, 00), timeForDateTime(2015, 10, 14, 04, 00)));

    Regulatory::Layer layer;

    {
        std::shared_ptr<attic::a_document> pDoc = request2doc( request );
        layer.deserialize( pDoc->document_element() );
    }

    {
        Platforms platforms;
        attic::a_document dd;
        dd.load_wide(
            L"<Platforms>"
            L"  <Span from='901000' to='900070'>"
            L"    <Platform esr='900640' name='Depo'/>"
            L"  </Span>"
            L"</Platforms>"
            );
        platforms.deserialize( dd.document_element() );
        layer.eliminate(platforms);
    }

    {
        attic::a_document doc("Export");
        layer.takeChanges( attic::a_node() );
        layer.serialize(doc.document_element());

        const std::string result = doc.document_element().pretty_str();
        CPPUNIT_ASSERT(!result.empty());

        attic::a_node layerNode = doc.document_element().child(ScheduledLayer_xAttr);
        CPPUNIT_ASSERT(layerNode);

        attic::a_node pathNode = layerNode.child(ScheduledPath_xAttr);
        CPPUNIT_ASSERT_MESSAGE("1st generated path", pathNode);

        std::array<ExpectedEvent, 4u> expectedEvents = {
            std::make_tuple(91809u, std::string("20151013T230700Z"), HCode::INFO_CHANGING),
            std::make_tuple(91809u, std::string("20151013T230700Z"), HCode::ARRIVAL),
            std::make_tuple(90100u, std::string("20151014T030100Z"), HCode::DEPARTURE),
//платформа!   std::make_tuple(90064u, std::string("20151014T031100Z"), HCode::ARRIVAL),
//платформа!   std::make_tuple(90064u, std::string("20151014T031100Z"), HCode::DEPARTURE),
            std::make_tuple(90007u, std::string("20151014T032100Z"), HCode::DEPARTURE)
        };

        ensureEvents(pathNode, expectedEvents);

        CPPUNIT_ASSERT_MESSAGE("2nd generated path shouldn't exist", !pathNode.next_sibling(ScheduledPath_xAttr));
    }
}

void TC_ScheduledLayer::ParkWaySerialization()
{
    Regulatory::ScheduledLayer layer;

    std::string source;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T030000Z'>\n"
            L"      <Thread train='222'>\n"
            L"        <Event esr='91809' type='arrival'   time='230700' parknum='25' waynum='2' />\n"
            L"        <Event esr='90100' type='departure' time='030100' parknum='25' waynum='2' />\n"
            L"        <Event esr='90064' type='arrival'   time='031100' parknum='7'  waynum='4' />\n"
            L"        <Event esr='90064' type='departure' time='031100' parknum='7'  waynum='4' />\n"
            L"        <Event esr='90007' type='departure' time='032100' parknum='11' waynum='1' />\n"
            L"      </Thread>\n"
            L"      <Thread train='333'>\n"
            L"        <Event esr='90064' type='arrival'   time='135200' waynum='3' />\n"
            L"        <Event esr='90064' type='departure' time='135200' waynum='3' />\n"
            L"        <Event esr='90100' type='departure' time='140600' />\n"
            L"        <Event esr='91809' type='departure' time='180100' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n"
            );
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", layer.deserialize(doc.document_element(), time_t(0)));
        source = doc.document_element().pretty_str();
    }

    {
        attic::a_document doc("Root");
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to serialize into xml", layer.serialize(doc.document_element()));
        const std::string result = doc.document_element().pretty_str();

        CPPUNIT_ASSERT_EQUAL(source, result);
    }
}

void TC_ScheduledLayer::ParkWayGeneration()
{
    Regulatory::ScheduledLayer schedule;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<Root>\n"
            L"  <ScheduledLayer>\n"
            L"    <RegulatorySchedule startTime='20151013T030000Z'>\n"
            L"      <Thread train='222'>\n"
            L"        <Event esr='91809' type='arrival'   time='230700' parknum='25' waynum='2' />\n"
            L"        <Event esr='90100' type='departure' time='030100' parknum='25' waynum='2' />\n"
            L"        <Event esr='90064' type='arrival'   time='031100' parknum='7'  waynum='4' />\n"
            L"        <Event esr='90064' type='departure' time='031100' parknum='7'  waynum='4' />\n"
            L"        <Event esr='90007' type='departure' time='032100' parknum='11' waynum='1' />\n"
            L"      </Thread>\n"
            L"      <Thread train='333'>\n"
            L"        <Event esr='90064' type='arrival'   time='135200' waynum='3' />\n"
            L"        <Event esr='90064' type='departure' time='135200' waynum='3' />\n"
            L"        <Event esr='90100' type='departure' time='140600' />\n"
            L"        <Event esr='91809' type='departure' time='180100' />\n"
            L"      </Thread>\n"
            L"    </RegulatorySchedule>\n"
            L"  </ScheduledLayer>\n"
            L"</Root>\n"
            );

        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to deserialize from xml", schedule.deserialize(doc.document_element(),time_t(0)));
    }

    auto request = schedule.selectSchedulesFor(periodForTimes(
        timeForDateTime(2015, 10, 13, 23, 00), timeForDateTime(2015, 10, 14, 22, 00)));


    {
        std::shared_ptr<attic::a_document> pDoc = request2doc( request );
        attic::a_document& doc = *pDoc;

        const std::string result = doc.document_element().pretty_str();
        CPPUNIT_ASSERT(!result.empty());

        attic::a_node a = doc.select_node("//SpotEvent[@name='Arrival' and @waynum=2 and @parknum=25]").node();
        CPPUNIT_ASSERT( a );

        attic::a_node d = doc.select_node("//SpotEvent[@name='Departure' and @waynum=3]").node();
        CPPUNIT_ASSERT( d );
    }
}

void TC_ScheduledLayer::CalendarSerialization()
{
    Regulatory::DailyCalendar dc_concrete;
    std::string source;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<Test>"
            L"  <OnDays Y='2022' M='04' D='00110000000000000000000000000000' />"
            L"  <OnDays Y='2022' M='05' D='10100000000000000000000000000000' />"
            L"</Test>"
            );
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Calendar failed to deserialize from xml", dc_concrete.deserialize( doc.document_element() ) );
        source = doc.document_element().pretty_str();
    }
    CPPUNIT_ASSERT( !dc_concrete.contains( 1995, 12, 1 ) );

    CPPUNIT_ASSERT( !dc_concrete.contains( 2022, 04, 1 ) );
    CPPUNIT_ASSERT( !dc_concrete.contains( 2022, 04, 2 ) );
    CPPUNIT_ASSERT( dc_concrete.contains( 2022, 04, 3 ) );
    CPPUNIT_ASSERT( dc_concrete.contains( 2022, 04, 4 ) );

    CPPUNIT_ASSERT( dc_concrete.contains( 2022, 05, 1 ) );
    CPPUNIT_ASSERT( !dc_concrete.contains( 2022, 05, 2 ) );
    CPPUNIT_ASSERT( dc_concrete.contains( 2022, 05, 3 ) );
    CPPUNIT_ASSERT( !dc_concrete.contains( 2022, 05, 4 ) );

    CPPUNIT_ASSERT( !dc_concrete.contains( 2077, 01, 1 ) );
    {
        attic::a_document doc("Test");
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Layer failed to serialize into xml", dc_concrete.serialize( doc.document_element() ) );
        const std::string result = doc.document_element().pretty_str();
        CPPUNIT_ASSERT_EQUAL(result,source );
    }

    Regulatory::DailyCalendar dc_empty;
    CPPUNIT_ASSERT( dc_empty.contains( 1995, 12, 1 ) );
    CPPUNIT_ASSERT( dc_empty.contains( 2077, 01, 1 ) );
}

void TC_ScheduledLayer::GenerationByCalendar()
{
    attic::a_document doc;
    doc.load_wide(
        L"<Root>\n"
        L"  <ScheduledLayer>\n"
        L"    <RegulatorySchedule startTime='20220322T030000Z'>\n"
        L"      <Thread train='33'>\n"
        L"        <Event esr='77777' type='arrival'   time='110000' />\n"
        L"        <Event esr='77777' type='departure' time='113000' />\n"
        L"        <Event esr='66666' type='arrival'   time='220000' />\n"
        L"        <Event esr='66666' type='departure' time='223000' />\n"
        L"      </Thread>\n"
        L"      <Thread train='55'>\n"
        L"        <OnDays Y='2022' M='04' D='11000000000000000000000000000000' />"
        L"        <Event esr='11111' type='arrival'   time='111000' />\n"
        L"        <Event esr='11111' type='departure' time='111500' />\n"
        L"        <Event esr='22222' type='arrival'   time='222000' />\n"
        L"        <Event esr='22222' type='departure' time='222500' />\n"
        L"      </Thread>\n"
        L"      <Thread train='55'>\n"
        L"        <OnDays Y='2022' M='04' D='00110000000000000000000000000000' />"
        L"        <Event esr='11111' type='arrival'   time='111000' />\n"
        L"        <Event esr='11111' type='departure' time='111500' />\n"
        L"        <Event esr='44444' type='arrival'   time='144000' />\n"
        L"        <Event esr='44444' type='departure' time='144500' />\n"
        L"      </Thread>\n"
        L"    </RegulatorySchedule>\n"
        L"  </ScheduledLayer>\n"
        L"</Root>\n"
        );

    Regulatory::ScheduledLayer schedule;
    CPPUNIT_ASSERT_NO_THROW( schedule.deserialize( doc.document_element(), time_t(0) ) );

    {
        // в расписании 2022-03-20 генерируетс€ только 33 поезд
        auto request30mar = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime( 2022, 03, 30,  5, 00 ),
            timeForDateTime( 2022, 03, 30, 23, 59 ) ) );
        CPPUNIT_ASSERT( !request30mar.empty() );

        auto doc30mar = request2doc( request30mar );
        CPPUNIT_ASSERT( doc30mar );

        std::wstring ws = doc30mar->pretty_wstr();
        CPPUNIT_ASSERT( !ws.empty() );

        attic::a_node train33 = doc30mar->select_node("//SpotEvent[@num=33]").node();
        CPPUNIT_ASSERT( train33 );
        attic::a_node train55 = doc30mar->select_node("//SpotEvent[@num=55]").node();
        CPPUNIT_ASSERT( !train55 );
    }

    {
        // в расписании 2022-04-01 генерируютс€ поезда 33 и 55(через 22222)
        auto request01apr = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime( 2022, 04, 01,  5, 00 ),
            timeForDateTime( 2022, 04, 01, 23, 59 ) ) );
        CPPUNIT_ASSERT( !request01apr.empty() );

        auto doc01apr = request2doc( request01apr );
        CPPUNIT_ASSERT( doc01apr );

        std::wstring ws = doc01apr->pretty_wstr();
        CPPUNIT_ASSERT( !ws.empty() );

        attic::a_node train33 = doc01apr->select_node("//SpotEvent[@num=33]").node();
        CPPUNIT_ASSERT( train33 );
        attic::a_node train55 = doc01apr->select_node("//SpotEvent[@num=55]").node();
        CPPUNIT_ASSERT( train55 );
        attic::a_node spot2 = doc01apr->select_node("//SpotEvent[contains(@Bdg,'[22222]')]").node();
        CPPUNIT_ASSERT( spot2 );
        attic::a_node spot4 = doc01apr->select_node("//SpotEvent[contains(@Bdg,'[44444]')]").node();
        CPPUNIT_ASSERT( !spot4 );
    }

    {
        // в расписании 2022-04-03 генерируютс€ поезда 33 и 55(через 44444)
        auto request03apr = schedule.selectSchedulesFor(periodForTimes(
            timeForDateTime( 2022, 04, 03,  5, 00 ),
            timeForDateTime( 2022, 04, 03, 23, 59 ) ) );
        CPPUNIT_ASSERT( !request03apr.empty() );

        auto doc03apr = request2doc( request03apr );
        CPPUNIT_ASSERT( doc03apr );

        std::wstring ws = doc03apr->pretty_wstr();
        CPPUNIT_ASSERT( !ws.empty() );

        attic::a_node train33 = doc03apr->select_node("//SpotEvent[@num=33]").node();
        CPPUNIT_ASSERT( train33 );
        attic::a_node train55 = doc03apr->select_node("//SpotEvent[@num=55]").node();
        CPPUNIT_ASSERT( train55 );
        attic::a_node spot2 = doc03apr->select_node("//SpotEvent[contains(@Bdg,'[22222]')]").node();
        CPPUNIT_ASSERT( !spot2 );
        attic::a_node spot4 = doc03apr->select_node("//SpotEvent[contains(@Bdg,'[44444]')]").node();
        CPPUNIT_ASSERT( spot4 );
    }
}
