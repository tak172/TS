#include "stdafx.h"
#include "TC_Planned.h"
#include "UtHelpfulDateTime.h"
#include "../Hem/RegulatoryXml.h"
#include "../Hem/DailyEvent.h"
#include "../Hem/DailyPath.h"
#include "../Hem/RegulatoryTypes.h"
#include "../Hem/ScheduledPath.h"
#include "../Hem/ScheduledLayer.h"
#include "../Hem/TerraChart.h"
#include "../helpful/StrToTime.h"
#include "../helpful/EsrGuide.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Planned );

using namespace Regulatory;

static void addArrival( attic::a_node parentNode, std::string timeOfDay, EsrKit stationEsr )
{
    auto eventNode = parentNode.append_child(XmlDecl::nodeEvent);
    eventNode.brief_attribute(XmlDecl::attrType, XmlDecl::valueTypeArrival);
    eventNode.brief_attribute(XmlDecl::attrTime, timeOfDay);
    eventNode.brief_attribute(XmlDecl::attrEsr,  stationEsr);
}

static void addDeparture( attic::a_node parentNode, std::string timeOfDay, EsrKit stationEsr )
{
    auto eventNode = parentNode.append_child(XmlDecl::nodeEvent);
    eventNode.brief_attribute(XmlDecl::attrType, XmlDecl::valueTypeDeparture);
    eventNode.brief_attribute(XmlDecl::attrTime, timeOfDay);
    eventNode.brief_attribute(XmlDecl::attrEsr,  stationEsr);
}

void TC_Planned::EventFromXml()
{
	EsrKit stationEsr(100u);

	attic::a_document doc("EventLoading");
	auto root = doc.document_element();
	auto eventNode = root.ensure_child(XmlDecl::nodeEvent);
	eventNode.ensure_attribute(XmlDecl::attrType).set_value(XmlDecl::valueTypeArrival);
	eventNode.ensure_attribute(XmlDecl::attrTime).set_value("120030Z and something that doesn't make any sense");
	eventNode.ensure_attribute(XmlDecl::attrEsr).set_value(stationEsr.to_string());

	boost::posix_time::time_duration timeDuration = boost::posix_time::hours(23);
	DailyEvent event = DailyEvent::create(eventNode, timeDuration);

	assertTimeOfDateEqual(24u + 12u, 0u, 30u, timeDuration);
	assertTimeOfDateEqual(24u + 12u, 0u, 30u, event.getLocalTimeOfDay());
	CPPUNIT_ASSERT_EQUAL(1u, (unsigned)event.getDateOffset());
	CPPUNIT_ASSERT(DailyEvent::Type::Arrival == event.getType());
	
	const boost::gregorian::date date(2014, 01, 01);
	const time_t expectedTime = UTC_From_Local(boost::posix_time::ptime(date, event.getLocalTimeOfDay()));

	const unsigned c_routeNumber = 32167u;

    EsrGuide eg;
	SpotEvent spotEvent = event.generateEventFor(date, c_routeNumber, eg);

	// TODO: Badge string representation should be reworked
	CPPUNIT_ASSERT(spotEvent.GetBadge() == BadgeE(DailyEvent::badgeString(c_routeNumber), stationEsr));
	CPPUNIT_ASSERT_EQUAL(expectedTime, spotEvent.GetTime());
	CPPUNIT_ASSERT(spotEvent.GetCode() == HCode::ARRIVAL);
}

void TC_Planned::PathFromXml()
{
	attic::a_document doc("PathLoading");
	auto pathNode = doc.document_element().ensure_child(XmlDecl::nodePath);
	pathNode.ensure_attribute(XmlDecl::attrTrain).set_value(1042u);
	addArrival(  pathNode, "163030", EsrKit(100) );
	addDeparture(pathNode, "164000", EsrKit(100) );
	addArrival(  pathNode, "140030", EsrKit(110) );
	addDeparture(pathNode, "142000", EsrKit(110) );

	DailyPath path(pathNode);
	CPPUNIT_ASSERT_EQUAL(2u, path.getInvolvedDaysCount());
	CPPUNIT_ASSERT_EQUAL(1042u, path.getRouteNumber());

    EsrGuide eg;
	auto scheduledPath = path.generatePathForDate(boost::gregorian::date(2014, 1, 1), eg);
	CPPUNIT_ASSERT(scheduledPath);
}

void TC_Planned::Serialization()
{
	attic::a_document sourceDoc("Source");
	auto scheduleNode = sourceDoc.document_element().append_child(XmlDecl::nodeRegulatorySchedule);
	scheduleNode.brief_attribute( XmlDecl::attrStartTime, time_from_iso("20141013T060000Z") );
	auto pathNode = scheduleNode.append_child(XmlDecl::nodePath);
	pathNode.append_attribute(XmlDecl::attrTrain).set_value(32167u);
	addDeparture(pathNode, "164010", EsrKit(701));
	addArrival(  pathNode, "130000", EsrKit(721));
	addDeparture(pathNode, "170000", EsrKit(721));
	addArrival(  pathNode, "070000", EsrKit(741));

	pathNode = scheduleNode.append_child(XmlDecl::nodePath);
	pathNode.append_attribute(XmlDecl::attrTrain).set_value(32168u);
	addDeparture(pathNode, "104010", EsrKit(601));
	addArrival(  pathNode, "110000", EsrKit(621));
	addDeparture(pathNode, "120000", EsrKit(621));
	addArrival(  pathNode, "130000", EsrKit(641));

    const time_t currentMoment = time_from_iso("20141013T020000Z");
	Regulatory::ScheduledLayer layer;
    time_t startMoment = Regulatory::ScheduledLayer::getScheduleStart( scheduleNode );
	CPPUNIT_ASSERT(layer.loadableSchedule(startMoment, currentMoment, scheduleNode));
    CPPUNIT_ASSERT(layer.loadSchedule(scheduleNode));

	attic::a_document serializeDoc("Serialize");
	layer.serialize(serializeDoc.document_element());
	std::string serializedValue = serializeDoc.to_str();

	Regulatory::ScheduledLayer deserializedLayer;
	deserializedLayer.deserialize(serializeDoc.document_element(),currentMoment);

	CPPUNIT_ASSERT(layer.hasEqualSchedules(deserializedLayer));
}

void TC_Planned::GuardZone()
{
    attic::a_document sourceDoc("Source");
    auto scheduleNode = sourceDoc.document_element().append_child(XmlDecl::nodeRegulatorySchedule);
    scheduleNode.append_attribute(XmlDecl::attrStartTime).set_value("20141013T060000");
    auto pathNode = scheduleNode.append_child(XmlDecl::nodePath);
    pathNode.append_attribute(XmlDecl::attrTrain).set_value(32167u);
    addDeparture(pathNode, "164010", EsrKit(701));
    addArrival(  pathNode, "130000", EsrKit(721));
    addDeparture(pathNode, "170000", EsrKit(721));
    addArrival(  pathNode, "070000", EsrKit(741));

    TerraChart terra;
    time_t startMoment1 = Regulatory::ScheduledLayer::getScheduleStart( scheduleNode );
    CPPUNIT_ASSERT(terra.SetupRegulatorySchedule( startMoment1, time_t(0), scheduleNode ) );

    // новое расписание начинается на пару дней позже
    scheduleNode.attribute(XmlDecl::attrStartTime).set_value("20141015T060000");
    Regulatory::TimeStamp fixTime = Local_From_UTC( scheduleNode.attribute(XmlDecl::attrStartTime).as_time() );

    // если охраняемая область начинается позже начала нового расписания
    // то загрузить такое расписание невозможно
    time_t startMoment2 = Regulatory::ScheduledLayer::getScheduleStart( scheduleNode );
    CPPUNIT_ASSERT(!terra.SetupRegulatorySchedule( startMoment2, UTC_From_Local(fixTime + boost::posix_time::hours(7)),scheduleNode));

    // если охраняемая область начинается раньше начала нового расписания
    // то загрузка допускается
    time_t startMoment3 = Regulatory::ScheduledLayer::getScheduleStart( scheduleNode );
    CPPUNIT_ASSERT(terra.SetupRegulatorySchedule( startMoment3, UTC_From_Local(fixTime - boost::posix_time::hours(5)),scheduleNode));
}
