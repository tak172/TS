#include "stdafx.h"

#include "TC_AsoupLayer.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/EventTrio.h"
#include "../Asoup/Message.h"
#include "../Hem/XlatEsrParkWay.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AsoupLayer );

using namespace HemHelpful;

const std::string c_asoup_0 = ToUtf8( L"<AsoupEvent name='Arrival' Bdg='ASOUP 333[07792]' create_time='' index='0900-856-0705' num='2976' fretran='Y'><![CDATA[(:333 1042 909/000+07792 2976 0900 856 0705 01 11260 23 07 07 52 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 056 03066 01841 224 94524436 54498068 000 001 00 030 025.20 002 000 000 40 000 002 000 60 000 013 001 90 028 010 000 92 001 008 000 93 023 000 000 96 004 000 000:)]]></AsoupEvent>");

enum AsoupCode
{
    Arrival = HCode::ARRIVAL,
    Departure = HCode::DEPARTURE,
    Transition = HCode::TRANSITION
};

AsoupEventPtr AsoupFromString(const std::string& serialized)
{
    attic::a_document doc;
    CPPUNIT_ASSERT(doc.load_utf8(serialized));
    AsoupEventPtr asoupPtr(new AsoupEvent(doc.document_element()));
    return asoupPtr;
}

Hem::EventAddress<SpotEvent> CreateSpotAddress(unsigned esr, time_t time, AsoupCode code)
{
    BadgeE badge(L"UT", EsrKit(esr));
    HCode eventCode = static_cast<HCode>(code);

    return Hem::EventAddress<SpotEvent>(badge, time, eventCode);
}

Hem::AsoupServedData CreateAsoup(unsigned esr, time_t time, AsoupCode code, std::wstring index, std::wstring number, bool linked, ParkWayKit parkWay = ParkWayKit())
{
    TrainDescr td;
    td.SetIndex(index);
    td.SetNumber(number);

    BadgeE badge(L"UT", EsrKit(esr));
    HCode eventCode = static_cast<HCode>(code);

    Hem::EventAddress<SpotEvent> linkedAddress;
    if (linked)
        linkedAddress = CreateSpotAddress(esr, time, code);

    return Hem::AsoupServedData(std::make_shared<AsoupEvent>(eventCode, badge, parkWay, time, td), linkedAddress);
}

void TC_AsoupLayer::Push()
{
    AsoupEventPtr asoup_0 = AsoupFromString(c_asoup_0);
    AsoupEventPtr asoup_0_copy = AsoupFromString(c_asoup_0);
    Hem::AsoupServedData asoupData_1 = CreateAsoup(7792u, 100, Arrival, L"0900-856-0705", L"2976", true);
    Hem::AsoupServedData asoupData_1_copy = CreateAsoup(7792u, 100, Arrival, L"0900-856-0705", L"2976", true);
    AsoupLayer layer;
    
    layer.AddAsoupEvent(asoup_0);
    CPPUNIT_ASSERT(layer.IsDuplicate(*asoup_0));
    CPPUNIT_ASSERT(!layer.IsDuplicate(*asoupData_1.GetAsoupEvent()));
    CPPUNIT_ASSERT(!layer.IsServed(asoup_0));
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    
    layer.AddAsoupEvent(asoupData_1.GetAsoupEvent());
    CPPUNIT_ASSERT(layer.IsDuplicate(*asoup_0));
    CPPUNIT_ASSERT(layer.IsDuplicate(*asoupData_1.GetAsoupEvent()));
    CPPUNIT_ASSERT(!layer.IsServed(asoupData_1.GetAsoupEvent()));
    CPPUNIT_ASSERT(!layer.IsServed(asoup_0));
    CPPUNIT_ASSERT_EQUAL((size_t)2u, layer.UT_GetEvents().size());

    layer.PushAsoupData(asoupData_1);
    CPPUNIT_ASSERT(layer.IsDuplicate(*asoup_0));
    CPPUNIT_ASSERT(layer.IsDuplicate(*asoupData_1.GetAsoupEvent()));
    CPPUNIT_ASSERT(layer.IsServed(asoupData_1.GetAsoupEvent()));
    // Привязки групп больше нет
    CPPUNIT_ASSERT(!layer.IsServed(asoup_0));
    CPPUNIT_ASSERT_EQUAL((size_t)2u, layer.UT_GetEvents().size());

    layer.AddAsoupEvent(asoup_0_copy);
    CPPUNIT_ASSERT_EQUAL((size_t)2u, layer.UT_GetEvents().size());

    layer.PushAsoupData(asoupData_1_copy);
    CPPUNIT_ASSERT_EQUAL((size_t)2u, layer.UT_GetEvents().size());
}

void TC_AsoupLayer::ParkWayKitUsage()
{
    // Чаще всего в АСОУП парк и путь не указываются (00/00 в шапке)
    const std::string c_asoupWithoutParkWay = ToUtf8(
        L"<AsoupEvent name='Arrival' Bdg='ASOUP 333[07792]' create_time='' index='0900-856-0705' num='2976' fretran='Y'><![CDATA[(:333 1042 909/000+07792 2976 0900 856 0705 01 11260 23 07 07 52 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 03066 01841 224 94524436 54498068 000 001 00 030 025.20 002 000 000 40 000 002 000 60 000 013 001 90 028 010 000 92 001 008 000 93 023 000 000 96 004 000 000:)]]></AsoupEvent>");
    // Тем не менее поля значимые и в дальнейшем должны указываться (02/02 в данном случае)
    const std::string c_asoupWithParkway = ToUtf8(
        L"<AsoupEvent name='Arrival' Bdg='ASOUP 333[07792]' create_time='' index='0900-856-0705' num='2976' fretran='Y'><![CDATA[(:333 1042 909/000+07792 2976 0900 856 0705 01 11260 23 07 07 52 02/02 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 03066 01841 224 94524436 54498068 000 001 00 030 025.20 002 000 000 40 000 002 000 60 000 013 001 90 028 010 000 92 001 008 000 93 023 000 000 96 004 000 000:)]]></AsoupEvent>");
    
    AsoupEventPtr asoup_0 = AsoupFromString(c_asoupWithoutParkWay);
    AsoupEventPtr asoup_1 = AsoupFromString(c_asoupWithParkway);

    AsoupLayer layer;

    layer.AddAsoupEvent(asoup_0);
    CPPUNIT_ASSERT(layer.IsDuplicate(*asoup_0));
    CPPUNIT_ASSERT(!layer.IsDuplicate(*asoup_1));
    CPPUNIT_ASSERT(!layer.IsServed(asoup_0));
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());

    layer.AddAsoupEvent(asoup_1);
    CPPUNIT_ASSERT(layer.IsDuplicate(*asoup_0));
    CPPUNIT_ASSERT(layer.IsDuplicate(*asoup_1));
    CPPUNIT_ASSERT(!layer.IsServed(asoup_1));
    CPPUNIT_ASSERT(!layer.IsServed(asoup_0));
    CPPUNIT_ASSERT_EQUAL((size_t)2u, layer.UT_GetEvents().size());

    CPPUNIT_ASSERT(layer.GetServedMessage(asoup_0).empty());
    CPPUNIT_ASSERT(layer.GetServedMessage(asoup_1).empty());
}

void TC_AsoupLayer::Served()
{
    AsoupEventPtr event_0 = CreateAsoup(7777u, 100, Departure, L"0900-856-0705", L"2976", false).GetAsoupEvent();
    AsoupEventPtr event_1 = CreateAsoup(7778u, 200, Transition, L"0900-856-0705", L"2976", false).GetAsoupEvent();
    AsoupEventPtr event_2 = CreateAsoup(7779u, 300, Arrival, L"0900-856-0705", L"2976", false).GetAsoupEvent();
    AsoupLayer::SpotEventAddress spotAddress = CreateSpotAddress(7778u, 210, Arrival);

    AsoupLayer layer;
    layer.AddAsoupEvent(event_0);
    layer.AddAsoupEvent(event_1);
    layer.AddAsoupEvent(event_2);

    CPPUNIT_ASSERT(!layer.IsServed(event_0));
    CPPUNIT_ASSERT(!layer.IsServed(event_1));
    CPPUNIT_ASSERT(!layer.IsServed(event_2));

    layer.SetServed(event_1, spotAddress);
    CPPUNIT_ASSERT(!layer.IsServed(event_0));
    CPPUNIT_ASSERT(layer.IsServed(event_1));
    CPPUNIT_ASSERT(!layer.IsServed(event_2));

    layer.SetServed(event_0, spotAddress);
    CPPUNIT_ASSERT(layer.IsServed(event_0));
    CPPUNIT_ASSERT(layer.IsServed(event_1));
    CPPUNIT_ASSERT(!layer.IsServed(event_2));

    layer.SetNotServed(event_2);
    CPPUNIT_ASSERT(layer.IsServed(event_0));
    CPPUNIT_ASSERT(layer.IsServed(event_1));
    CPPUNIT_ASSERT(!layer.IsServed(event_2));
}

void TC_AsoupLayer::Serialization()
{
    auto event_0 = CreateAsoup(7777u, 100, Departure, L"0900-856-0705", L"2976", true);
    auto event_1 = CreateAsoup(7778u, 200, Transition, L"0900-856-0705", L"2976", true);
    auto event_2 = CreateAsoup(7779u, 300, Arrival, L"0900-856-0705", L"2976", true);
    
    attic::a_document doc("UT");

    {
        AsoupLayer layer;
        layer.PushAsoupData(event_0);
        layer.PushAsoupData(event_1);
        layer.PushAsoupData(event_2);

        layer.TakeChanges(doc.document_element().append_child("__temp"));
        doc.document_element().remove_child("__temp");

        doc.document_element() << layer;
    }

    {
        AsoupLayer layer;
        doc.document_element() >> layer;

        CPPUNIT_ASSERT(layer.IsDuplicate(*event_0.GetAsoupEvent()));
        CPPUNIT_ASSERT(layer.IsDuplicate(*event_1.GetAsoupEvent()));
        CPPUNIT_ASSERT(layer.IsDuplicate(*event_2.GetAsoupEvent()));
        CPPUNIT_ASSERT(layer.IsServed(event_0.GetAsoupEvent()));
        CPPUNIT_ASSERT(layer.IsServed(event_1.GetAsoupEvent()));
        CPPUNIT_ASSERT(layer.IsServed(event_2.GetAsoupEvent()));
    }
}

void TC_AsoupLayer::DoubleSerialization()
{
    const unsigned c_enormouslyBigNumber = 42u;

    auto event_0 = CreateAsoup(7777u, 100, Departure, L"0900-856-0705", L"2976", true);
    auto event_1 = CreateAsoup(7778u, 200, Transition, L"0900-856-0705", L"2976", true);
    auto event_2 = CreateAsoup(7779u, 300, Arrival, L"0900-856-0705", L"2976", true);
    AsoupLayer layer;

    for (unsigned i = 0u; i < c_enormouslyBigNumber; ++i)
    {
        layer.PushAsoupData(event_0);
        layer.PushAsoupData(event_1);
        layer.PushAsoupData(event_2);
    }
    
    {
        attic::a_document doc("UT");
        CPPUNIT_ASSERT(layer.TakeChanges(doc.document_element()));
        const std::string representation = doc.pretty_str();
        CPPUNIT_ASSERT(!representation.empty());
        CPPUNIT_ASSERT_EQUAL((size_t)3u, doc.document_element().select_nodes(("//" + Trio_xTag).c_str()).size());
    }

    {
        attic::a_document doc("UT");
        doc.document_element() << layer;
        const std::string representation = doc.pretty_str();
        CPPUNIT_ASSERT(!representation.empty());
        CPPUNIT_ASSERT_EQUAL((size_t)3u, doc.document_element().select_nodes(("//" + AsoupEvent_xAttr).c_str()).size());
    }
}

void TC_AsoupLayer::RealExample()
{
    const std::string example = ToUtf8(L"<AsoupEvent name='Disform' Bdg='ASOUP 1042[09180]' create_time='20151207T100700Z' index='0986-040-0918' num='3122' fretran='Y' linkBadge='5C[09180]' linkTime='20151207T055434Z' linkCode='Arrival' linkTime='20151207T055434Z' linkCode='Arrival'><![CDATA[(:1042 909/000+09180 3122 0986 040 0918 05 00000 07 12 12 07 00/00 0 0/00 00 0\n"
        L"Ю2 0 00  Д 00 00 0000 0 0 051 01083 00000 192 95699351 95104733 000 000 00 000 048.60 000 002 000 90 000 046 000 95 000 046 000:)]]></AsoupEvent>");

    AsoupEventPtr exampleEvent_0 = AsoupFromString(example);
    AsoupEventPtr exampleEvent_1 = AsoupFromString(example);

    AsoupLayer layer;
    layer.AddAsoupEvent(exampleEvent_0);
    layer.AddAsoupEvent(exampleEvent_1);

    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());

    {
        attic::a_document doc("UT");
        CPPUNIT_ASSERT(layer.TakeChanges(doc.document_element()));
        const std::string representation = doc.pretty_str();
        CPPUNIT_ASSERT(!representation.empty());
        CPPUNIT_ASSERT_EQUAL((size_t)1u, doc.document_element().select_nodes(("//" + Trio_xTag).c_str()).size());
    }

    {
        attic::a_document doc("UT");
        doc.document_element() << layer;
        const std::string representation = doc.pretty_str();
        CPPUNIT_ASSERT(!representation.empty());
        CPPUNIT_ASSERT_EQUAL((size_t)1u, doc.document_element().select_nodes(("//" + AsoupEvent_xAttr).c_str()).size());
    }
}

unsigned GetChangesCount(AsoupLayer& layer)
{
    attic::a_document changeDoc("Tmp");
    layer.TakeChanges(changeDoc.document_element());
    return static_cast<unsigned>(changeDoc.document_element().select_nodes("//AsoupLayer/Trio").size());
}

const std::wstring c_linkedDepartureAsoup = 
    L"<AsoupEvent name='Departure' Bdg='ASOUP 1042[09180]' create_time='20160518T153000Z' index='0982-009-5851' num='2212' length='56' weight='1194' fretran='Y' dirTo='11420' linkBadge='8C[09180]' linkTime='20160518T152804Z' linkCode='Departure' ><![CDATA[(:1042 909/000+09180 2212 0982 009 5851 03 11420 18 05 18 30 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 056 01194 00000 212 95153169 95263745 000 000 00 000 053.90 000 053 000 92 000 005 000 95 000 048 000\n"
    L"Ю3 530 00009591 1 18 00 0000 00000 KOVALJONOKS \n"
    L"Ю3 530 00009592 9\n"
    L"Ю4 11290 2 58 000 053.90 000 053 92 000 005 95 000 048 \n"
    L"Ю12 00 95153169 1 000 58510 43612 2870 300 00 00 00 00 00 0220 12 95 0950 04 106 58510 00000 11290 58 09820 17 00000 0000 020 1 1280 09826  128 000 00000000\n"
    L"Ю12 00 95283487 255239255255 0215\n"
    L"Ю12 00 95082640 255255255255 \n"
    L"Ю12 00 95426458 255239255255 0225\n"
    L"Ю12 00 95173472 251255255255 400\n"
    L"Ю12 00 95171039 251239255255 3000222\n"
    L"Ю12 00 95102182 255239255255 0227\n"
    L"Ю12 00 95205985 255239255255 0223\n"
    L"Ю12 00 95136750 251239255255 4000220\n"
    L"Ю12 00 58523614 251237255255 30002305935\n"
    L"Ю12 00 53237046 251255255255 400\n"
    L"Ю12 00 58531922 251255255255 500\n"
    L"Ю12 00 95395430 255237255255 02200950\n"
    L"Ю12 00 90249533 251233255255 3000232920901\n"
    L"Ю12 00 90255159 255239255255 0230\n"
    L"Ю12 00 95301552 251249255255 500950950\n"
    L"Ю12 00 95304580 255239255255 0225\n"
    L"Ю12 00 95433504 255239255255 0226\n"
    L"Ю12 00 58548082 255237255255 02305935\n"
    L"Ю12 00 58528761 255255255255 \n"
    L"Ю12 00 95422028 255237255255 02200950\n"
    L"Ю12 00 95139374 251239255255 4000225\n"
    L"Ю12 00 95200432 255239255255 0223\n"
    L"Ю12 00 58559626 255237255255 02305935\n"
    L"Ю12 00 95515755 251237255255 50002200950\n"
    L"Ю12 00 95204814 255239255255 0222\n"
    L"Ю12 00 95293882 255239255255 0225\n"
    L"Ю12 00 58530502 251237255255 40002305935\n"
    L"Ю12 00 55002067 255239255255 0223\n"
    L"Ю12 00 53222238 255239255255 0224\n"
    L"Ю12 00 95380341 251237255255 50002200950\n"
    L"Ю12 00 95265260 251239255255 4000230\n"
    L"Ю12 00 58541921 255253255255 5935\n"
    L"Ю12 00 58550013 255255255255 \n"
    L"Ю12 00 95197430 255237255255 02130950\n"
    L"Ю12 00 58560848 255237255255 02305935\n"
    L"Ю12 00 59033969 251233127255 5000220925904095\n"
    L"Ю12 00 95294401 251249127255 400950950106\n"
    L"Ю12 00 95227187 255239255255 0230\n"
    L"Ю12 00 58542002 255253255255 5935\n"
    L"Ю12 00 58514779 255255255255 \n"
    L"Ю12 00 95356473 251237255255 50002260950\n"
    L"Ю12 00 59036178 255233127255 0223925904095\n"
    L"Ю12 00 58569385 251233127255 4000227955935106\n"
    L"Ю12 00 95420865 251237255255 50002200950\n"
    L"Ю12 00 58518135 255237255255 02305935\n"
    L"Ю12 00 95041125 255237255255 02200950\n"
    L"Ю12 00 55017388 255237255255 02235935\n"
    L"Ю12 00 58537929 255239255255 0230\n"
    L"Ю12 00 95252052 255237255255 02260950\n"
    L"Ю12 00 95323200 255239255255 0220\n"
    L"Ю12 00 59259291 251233127255 0000234925904095\n"
    L"Ю12 00 95263745 251233127255 4000230950950106:)]]></AsoupEvent>";

void TC_AsoupLayer::Doubles()
{
    attic::a_document doc;
    CPPUNIT_ASSERT(doc.load_wide(c_linkedDepartureAsoup));
    AsoupEventPtr asoupPtr(new AsoupEvent(doc.document_element()));
    AsoupLayer layer;

    // Добавление события без привязки в первый раз
    CPPUNIT_ASSERT(layer.UT_GetEvents().empty());
    layer.AddAsoupEvent(asoupPtr);
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(1u, GetChangesCount(layer));

    // Повторное добавление события без привязки
    layer.AddAsoupEvent(asoupPtr);
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(0u, GetChangesCount(layer));

    // Добавление этого же события, но уже с привязкой
    layer.PushAsoupData(Hem::AsoupServedData(doc.document_element()));
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(1u, GetChangesCount(layer));

    // Повторное добавление привязанного события
    layer.PushAsoupData(Hem::AsoupServedData(doc.document_element()));
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(0u, GetChangesCount(layer));

    {
        attic::a_document saveDoc("Tmp");
        saveDoc.document_element() << layer;
        CPPUNIT_ASSERT_EQUAL((size_t)1u, saveDoc.document_element().select_nodes("//Tmp/AsoupLayer/AsoupEvent").size());
    }
}

void TC_AsoupLayer::UnlinkEvents()
{
    attic::a_document doc;
    CPPUNIT_ASSERT(doc.load_wide(c_linkedDepartureAsoup));

    AsoupLayer layer;
    
    layer.PushAsoupData(Hem::AsoupServedData(doc.document_element()));
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(1u, GetChangesCount(layer));

    AsoupLayer::SpotEventAddress linkAddress(doc.document_element());
    CPPUNIT_ASSERT(!linkAddress.empty());
    CPPUNIT_ASSERT_EQUAL(size_t(1), layer.GetLinkedEvents(linkAddress).size());
    
    layer.UnlinkEvents(linkAddress);
    CPPUNIT_ASSERT_EQUAL(1u, GetChangesCount(layer));
    CPPUNIT_ASSERT_EQUAL(size_t(0), layer.GetLinkedEvents(linkAddress).size());

    layer.UnlinkEvents(linkAddress);
    CPPUNIT_ASSERT_EQUAL(0u, GetChangesCount(layer));
    CPPUNIT_ASSERT_EQUAL(size_t(0), layer.GetLinkedEvents(linkAddress).size());
}

void TC_AsoupLayer::StrangeTrindex()
{
    const std::string example = ToUtf8(L"<AsoupEvent create_time='20160831T013700Z' name='Arrival' Bdg='ASOUP 1042[11290]' index='0000-031-0001' num='0001' length='27' weight='854' psng='Y' dirFrom='6610' adjFrom='06612'><![CDATA[(:1042 909/000+11290 0001 0000 031 0001 01 06610 31 08 04 37 00/00 0 0/00 00 0\n"
        L"Ю2 0 00  Д 00 00 0000 0 0 051 01083 00000 192 95699351 95104733 000 000 00 000 048.60 000 002 000 90 000 046 000 95 000 046 000:)]]></AsoupEvent>");

    AsoupEventPtr exampleEvent_0 = AsoupFromString(example);
    AsoupEventPtr exampleEvent_1 = AsoupFromString(example);

    AsoupLayer layer;
    layer.AddAsoupEvent(exampleEvent_0);
    layer.AddAsoupEvent(exampleEvent_1);

    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());

    {
        attic::a_document doc("UT");
        CPPUNIT_ASSERT(layer.TakeChanges(doc.document_element()));
        const std::string representation = doc.pretty_str();
        CPPUNIT_ASSERT(!representation.empty());
        CPPUNIT_ASSERT_EQUAL((size_t)1u, doc.document_element().select_nodes(("//" + Trio_xTag).c_str()).size());
    }

    {
        attic::a_document doc("UT");
        doc.document_element() << layer;
        const std::string representation = doc.pretty_str();
        CPPUNIT_ASSERT(!representation.empty());
        CPPUNIT_ASSERT_EQUAL((size_t)1u, doc.document_element().select_nodes(("//" + AsoupEvent_xAttr).c_str()).size());
    }

    Trindex trindex(L"0000-031-0001");
    auto asoupEvents = layer.GetEventsByGroupAddress(trindex);
    CPPUNIT_ASSERT_EQUAL(size_t(1), asoupEvents.size());

    Trindex trindex2(L"0000-031-0002");
    asoupEvents = layer.GetEventsByGroupAddress(trindex2);
    CPPUNIT_ASSERT_EQUAL(size_t(0), asoupEvents.size());
}

bool AsoupContainsNAK(const std::wstring& asoup)
{
    std::shared_ptr<Asoup::Message> nakAsoup = Asoup::Message::parse(asoup, boost::gregorian::date(2015, 6, 6), Asoup::AsoupOperationMode::LDZ, false);
    CPPUNIT_ASSERT(nakAsoup);

    std::shared_ptr<AsoupEvent> nakEvent = AsoupEvent::Create(*nakAsoup, nullptr, nullptr, nullptr, BadgeE(), Asoup::AsoupOperationMode::LDZ);
    CPPUNIT_ASSERT(nakEvent);

    ConstTrainDescrPtr nakDescr = nakEvent->GetDescr();
    CPPUNIT_ASSERT(nakDescr);

    std::set<TrainCharacteristics::TrainFeature> features;
    nakDescr->GetFeatures(features);

    return (size_t(1) == features.count(TrainCharacteristics::TrainFeature::NAK));
}

void TC_AsoupLayer::NAKDetection()
{
    const std::wstring asoupWithoutNak = L"(:1042 909/000+09180 2212 0982 009 5851 03 11420 18 05 18 30 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 01194 00000 212 95153169 95263745 000 000 00 000 053.90 000 053 000 92 000 005 000 95 000 048 000\n"
        L"Ю3 530 00009591 1 18 00 0000 00000 KOVALJONOKS \n"
        L"Ю3 530 00009592 9\n"
        L"Ю4 11290 2 58 000 053.90 000 053 92 000 005 95 000 048 \n"
        L"Ю12 00 95153169 1 000 58510 43612 2870 300 00 00 00 00 00 0220 12 95 0950 04 106 58510 00000 11290 58 09820 17 00000 0000 020 1 1280 09826  128 000 00000000\n"
        L"Ю12 00 95283487 255239255255 0215:)";
    CPPUNIT_ASSERT(!AsoupContainsNAK(asoupWithoutNak));

    // Код груза равен 73217
    const std::wstring asoupCargoCode73217 = L"(:1042 909/000+09180 2212 0982 009 5851 03 11420 18 05 18 30 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 01194 00000 212 95153169 95263745 000 000 00 000 053.90 000 053 000 92 000 005 000 95 000 048 000\n"
        L"Ю3 530 00009591 1 18 00 0000 00000 KOVALJONOKS \n"
        L"Ю3 530 00009592 9\n"
        L"Ю4 11290 2 58 000 053.90 000 053 92 000 005 95 000 048 \n"
        L"Ю12 00 95153169 1 000 58510 73217 2870 300 00 00 00 00 00 0220 12 95 0950 04 106 58510 00000 11290 58 09820 17 00000 0000 020 1 1280 09826  128 000 00000000\n"
        L"Ю12 00 95283487 255239255255 0215:)";
    CPPUNIT_ASSERT(AsoupContainsNAK(asoupCargoCode73217));

    // Код груза 00300, станция назначения 09836, ПРИМЕЧАНИЕ “АКРИЛ”.
    const std::wstring asoupCargoCode00300station09836noteAKRIL = L"(:1042 909/000+09180 2212 0982 009 5851 03 11420 18 05 18 30 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 01194 00000 212 95153169 95263745 000 000 00 000 053.90 000 053 000 92 000 005 000 95 000 048 000\n"
        L"Ю3 530 00009591 1 18 00 0000 00000 KOVALJONOKS \n"
        L"Ю3 530 00009592 9\n"
        L"Ю4 11290 2 58 000 053.90 000 053 92 000 005 95 000 048 \n"
        L"Ю12 00 95153169 1 000 09836 00300 2870 300 00 00 00 00 00 0220 12 95 0950 04 106 58510 00000 11290 58 09820 17 00000 0000 020 1 1280 АКРИЛ  128 000 00000000\n"
        L"Ю12 00 95283487 255239255255 0215:)";
    CPPUNIT_ASSERT(AsoupContainsNAK(asoupCargoCode00300station09836noteAKRIL));
}

void TC_AsoupLayer::RemoveEvent()
{
    attic::a_document doc;
    CPPUNIT_ASSERT(doc.load_wide(c_linkedDepartureAsoup));

    AsoupLayer layer;

    Hem::AsoupServedData asoupData(doc.document_element());
    layer.PushAsoupData(asoupData);
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(1u, GetChangesCount(layer));

    CPPUNIT_ASSERT(layer.Remove(asoupData.GetAsoupEvent()));
    CPPUNIT_ASSERT_EQUAL((size_t)0u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(1u, GetChangesCount(layer));

    layer.AddAsoupEvent(asoupData.GetAsoupEvent());
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(1u, GetChangesCount(layer));

    CPPUNIT_ASSERT(layer.Remove(asoupData.GetAsoupEvent()));
    layer.AddAsoupEvent(asoupData.GetAsoupEvent());
    CPPUNIT_ASSERT_EQUAL((size_t)1u, layer.UT_GetEvents().size());
    CPPUNIT_ASSERT_EQUAL(1u, GetChangesCount(layer));
}

void TC_AsoupLayer::AsoupUniqueness()
{
    AsoupLayer layer;

    AsoupEventPtr asoup = AsoupFromString( ToUtf8(
        L"<AsoupEvent name='Arrival' Bdg='ASOUP 1042[07792]' create_time='' index='0900-856-0705' num='2976' fretran='Y'><![CDATA[(:333 1042 909/000+07792 2976 0900 856 0705 01 11260 23 07 07 52 02/02 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 03066 01841 224 94524436 54498068 000 001 00 030 025.20 002 000 000 40 000 002 000 60 000 013 001 90 028 010 000 92 001 008 000 93 023 000 000 96 004 000 000:)]]></AsoupEvent>") );
    CPPUNIT_ASSERT(asoup);

    layer.AddAsoupEvent(asoup);

    // Текст отличается только в бейдже
    AsoupEventPtr asoup2 = AsoupFromString( ToUtf8(
        L"<AsoupEvent name='Arrival' Bdg='ASOUP 333[07792]' create_time='' index='0900-856-0705' num='2976' fretran='Y'><![CDATA[(:333 1042 909/000+07792 2976 0900 856 0705 01 11260 23 07 07 52 02/02 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 03066 01841 224 94524436 54498068 000 001 00 030 025.20 002 000 000 40 000 002 000 60 000 013 001 90 028 010 000 92 001 008 000 93 023 000 000 96 004 000 000:)]]></AsoupEvent>") );
    CPPUNIT_ASSERT(asoup2);

    layer.AddAsoupEvent(asoup2);
    CPPUNIT_ASSERT_EQUAL((size_t)2u, layer.UT_GetEvents().size());

    CPPUNIT_ASSERT(layer.FindEvent(*asoup));
    CPPUNIT_ASSERT(layer.FindEvent(*asoup) == asoup);
    CPPUNIT_ASSERT(*layer.FindEvent(*asoup) == *asoup);

    CPPUNIT_ASSERT(layer.FindEvent(*asoup2));
    CPPUNIT_ASSERT(layer.FindEvent(*asoup2) == asoup2);
    CPPUNIT_ASSERT(*layer.FindEvent(*asoup2) == *asoup2);
}

AsoupEventPtr FindWithdrawalEvent(const AsoupLayer& layer, const AsoupEvent& withdrawalEvent, 
                                  const Asoup::Message& withdrawalMessage, boost::gregorian::date date,
                                  Asoup::AsoupOperationMode asoupMode)
{
    return layer.FindEventLike(withdrawalEvent, asoupMode,
        [&withdrawalMessage, date, asoupMode](const AsoupEvent& pretender) {
            return withdrawalMessage.isWithdrawalMessageFor(pretender.GetRawText(), date, asoupMode);
    });
}

void TC_AsoupLayer::WithdrawalSearch()
{
    boost::gregorian::date const date(2016, 10, 10);

    std::shared_ptr<Asoup::Message> message1042 = Asoup::Message::parse(
        L"(:1042 909/000+09180 3442 0982 044 0918 01 09820 29 11 09 43 03/17 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 036 01714 00908 140 95487088 95086161 000 001 00 016 018.20 000 008 000 70 000 010 000 73 000 010 000 90 016 000 001 95 016 000 001\n"
        L"Ю4 00000 0 00 000 018.20 000 008 70 000 010 73 000 010 \n"
        L"Ю4 00000 1 01 016 000.90 016 000 95 016 000 \n"
        L"Ю4 09870 1 01 016 000.90 016 000 95 016 000 \n"
        L"Ю12 00 95487088 1 000 09870 01100 3352 900 00 00 00 00 00 0220 40 95 0950 04 106 09180 09860 00000 01 09860 34 00000 0000 025 0 0000 ПEPECЛ 144 000 00000000\n"
        L"Ю12 00 74094467 195225081243 000002110359060600230117107910860071000098200309826 128\n"
        L"Ю12 00 74741232 255255255255 \n"
        L"Ю12 00 73519134 255255255255 \n"
        L"Ю12 00 74903451 255239255255 0245\n"
        L"Ю12 00 73117269 255255255255 \n"
        L"Ю12 00 74742651 255239255255 0232\n"
        L"Ю12 00 73132243 255239255255 0230\n"
        L"Ю12 00 73961872 255239255255 0244\n"
        L"Ю12 00 73086126 255239255255 0230\n"
        L"Ю12 00 73130213 255255255255 \n"
        L"Ю12 00 24478281 227233093255 42103630200002402002061060020043\n"
        L"Ю12 00 24478489 255255255255 \n"
        L"Ю12 00 26319194 255237127255 02500207110\n"
        L"Ю12 00 24115651 255237127255 02400206106\n"
        L"Ю12 00 24552168 255239255255 0244\n"
        L"Ю12 00 29567997 255237127255 02700211135\n"
        L"Ю12 00 24478463 255237127255 02400206106\n"
        L"Ю12 00 24552119 255255255255 \n"
        L"Ю12 00 95548822 135225208115 05709870014003352021630950950098600109860340979078880     148\n"
        L"Ю12 00 95408787 255239255255 0220\n"
        L"Ю12 00 95716437 191255255255 056\n"
        L"Ю12 00 95685228 191255255255 057\n"
        L"Ю12 00 95129086 255255255255 \n"
        L"Ю12 00 95068086 255255255255 \n"
        L"Ю12 00 95127205 255255255255 \n"
        L"Ю12 00 95845848 191239255255 0560236\n"
        L"Ю12 00 95127288 255239255255 0220\n"
        L"Ю12 00 95685053 191255255255 057\n"
        L"Ю12 00 95041844 191255255255 055\n"
        L"Ю12 00 95684429 191255255255 057\n"
        L"Ю12 00 95408647 255255255255 \n"
        L"Ю12 00 95128310 255255255255 \n"
        L"Ю12 00 95549044 191239255255 0580216\n"
        L"Ю12 00 95086161 191239255255 0570220:)", 
        date, Asoup::AsoupOperationMode::LDZ, true);
    CPPUNIT_ASSERT(message1042);
    CPPUNIT_ASSERT(!message1042->isWithdrawalMessage());

    std::shared_ptr<Asoup::Message> messageWithdrawal = Asoup::Message::parse(
        L"(:333 1042 909/000+09180 3442 0982 044 0918 01 09820 29 11 09 43 03/17 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 036 01714 00908 140 95487088 95086161 000 001 00 016 018.20 000 008 000 70 000 010 000 73 000 010 000 90 016 000 001 95 016 000 001:)",
        date, Asoup::AsoupOperationMode::LDZ, true);
    CPPUNIT_ASSERT(messageWithdrawal);
    CPPUNIT_ASSERT(messageWithdrawal->isWithdrawalMessage());

    AsoupEventPtr event1042 = AsoupEvent::Create(*message1042, nullptr, nullptr, nullptr, BadgeE(), Asoup::AsoupOperationMode::LDZ);
    CPPUNIT_ASSERT(event1042);
    AsoupEventPtr eventWithdrawal = AsoupEvent::Create(*messageWithdrawal, nullptr, nullptr, nullptr, BadgeE(), Asoup::AsoupOperationMode::LDZ);
    CPPUNIT_ASSERT(eventWithdrawal);
 
    AsoupLayer layer;

    layer.AddAsoupEvent(event1042);

    AsoupEventPtr found = FindWithdrawalEvent(layer, *eventWithdrawal, *messageWithdrawal, date, Asoup::AsoupOperationMode::LDZ);
    CPPUNIT_ASSERT(found);
    CPPUNIT_ASSERT(found == event1042);

    CPPUNIT_ASSERT(!FindWithdrawalEvent(layer, *event1042, *message1042, date, Asoup::AsoupOperationMode::LDZ));
}

void TC_AsoupLayer::TestCreateAsoup()
{
    boost::gregorian::date const date(2019, 05, 15);

    std::shared_ptr<Asoup::Message> message1042 = Asoup::Message::parse(
        L"(:1042 909/000+11000 8218 1100 193 1107 03 11020 15 05 13 02 03/09 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 013 00901 00435 048 19018274 19192830 012 000 00 000 000\n"
        L"Ю3 543 00064350 1 06 00 0000 00000 BOGDANS     \n"
        L"Ю12 00 19018274 1 000 11070 42100 7755 000 00 00 00 00 00 0443 50 10 0575 04 105 11070 00000 00000 02 11000 43 00000 0000 000 0 0000 60/25  128 000 00000000\n"
        L"Ю12 00 19174002 175237253251 045231000387058019132\n"
        L"Ю12 00 19642347 255237255255 02200577\n"
        L"Ю12 00 19174010 255237255255 03870580\n"
        L"Ю12 00 19192848 191255255255 050\n"
        L"Ю12 00 19192855 255255255255 \n"
        L"Ю12 00 19192863 255255255255 \n"
        L"Ю12 00 19345446 175239253251 00042100045043128\n"
        L"Ю12 00 19345438 255255255255 \n"
        L"Ю12 00 19192814 175239253251 05023100038719132\n"
        L"Ю12 00 19192822 255255255255 \n"
        L"Ю12 00 19192830 255255255255 :)\n",
        date, Asoup::AsoupOperationMode::LDZ, true);
    CPPUNIT_ASSERT(message1042);

    AsoupEventPtr event1042 = AsoupEvent::Create(*message1042, nullptr, nullptr, nullptr, BadgeE(), Asoup::AsoupOperationMode::LDZ);
    CPPUNIT_ASSERT(event1042);
}

void TC_AsoupLayer::CreateAsoupU3()
{
    boost::gregorian::date const date(2022, 06, 07);

    std::shared_ptr<Asoup::Message> message1042 = Asoup::Message::parse(
		L"(:1042 928/400+31151 6579 0000 007 6579 01 31210 07 06 13 40 01/02 0 0/00 00 0:\n"
		L"Ю3 853 00000621 1 08 25 2804 21010 ГУCEB        00000 769 2804 02839508 9 1 00 00 00 00:\n"
		L"Ю3 853 00000622 9:\n"
		L"Ю8 0000 007 6579 31151 01 07 06 13 40 6579 312104 358 311390:\n"
		L"Ю12 01 08530062 9 000 00000 00000 0000 000 00 00 00 00 00 0000 00 00 0000 00 000 00000 00000 00000 00 00000 00 00000 0000 000 0 0000 0      000 000 00000000:\n"
		L"Ю12 02 08530062 255255255255:\n"
		L"Ю12 03 08530062 255255255255:)",
        date, Asoup::AsoupOperationMode::LDZ, true);
    CPPUNIT_ASSERT(message1042);

    AsoupEventPtr event1042 = AsoupEvent::Create(*message1042, nullptr, nullptr, nullptr, BadgeE(), Asoup::AsoupOperationMode::LDZ);
    CPPUNIT_ASSERT(event1042);


}