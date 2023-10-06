#include "stdafx.h"
#include "TC_HappenPath.h"
#include "../helpful/Badge.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/HappenPath.h"
#include "../Hem/AutoIdentifyPolicy.h"
#include "../Hem/UserIdentifyPolicy.h"
#include "../Guess/SpotDetails.h"
#include "../helpful/TrainDescr.h"
#include "UtHelpfulDateTime.h"
#include "../helpful/DistrictGuide.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_HappenPath );

const time_t c_start = 60u * 60u * 1u;

const HCode c_codeForm = HCode::FORM;
const HCode c_codeArrival = HCode::ARRIVAL;
const HCode c_codeDeparture = HCode::DEPARTURE;
const HCode c_codeTransition = HCode::TRANSITION;
const HCode c_codeSpanmove = HCode::SPAN_MOVE;
const HCode c_codeDeath = HCode::DEATH;
const HCode c_codeDisform = HCode::DISFORM;

void TC_HappenPath::tearDown()
{
    Hem::DistrictSection::Shutdowner();
}

void TC_HappenPath::Create()
{
    const std::wstring c_data = 
        L"<HappenPath>\n"
        L"  <SpotEvent create_time='19700101T000100Z' name='Arrival' Bdg='Badge[10000]' index='1111-111-1111' num='N1379S' cont='Y' />\n"
        L"  <SpotEvent create_time='19700101T000200Z' name='Departure' Bdg='Badge[10000]' />\n"
        L"  <SpotEvent create_time='19700101T010000Z' name='Transition' Bdg='Badge[11000]' />\n"
        L"  <SpotEvent create_time='19700101T020000Z' name='Transition' Bdg='Badge[12000]' index='2222-222-2222' num='N1379S' cont='Y' />\n"
        L"</HappenPath>\n";

    attic::a_document sourceDoc;
    CPPUNIT_ASSERT(sourceDoc.load_wide(c_data));

    Hem::HappenPath path(sourceDoc.document_element());
    CPPUNIT_ASSERT_EQUAL(size_t(4), path.GetEventsCount());
    CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());

    // Десериализация не поддерживается, если нитка уже заполнена
    CPPUNIT_ASSERT_THROW(path << sourceDoc.document_element(), HemHelpful::HemException);

    attic::a_document serializeDoc("HappenPath");
    path >> serializeDoc.document_element();

    {
        const std::string source = sourceDoc.pretty_str();
        const std::string target = serializeDoc.pretty_str();
        CPPUNIT_ASSERT_EQUAL(source, target);
    }
}

void TC_HappenPath::Create2()
{
    const std::wstring c_data = 
        L"<HappenPath>\n"
        L"  <SpotEvent create_time='19700101T000100Z' name='Arrival' Bdg='Badge[10000]' index='1111-111-1111' num='N1379S' cont='Y' />\n"
        L"  <SpotEvent create_time='19700101T000200Z' name='Departure' Bdg='Badge[10000]' index='' num='' />\n"
        L"  <SpotEvent create_time='19700101T010000Z' name='Transition' Bdg='Badge[11000]' />\n"
        L"  <SpotEvent create_time='19700101T020000Z' name='Transition' Bdg='Badge[12000]' index='2222-222-2222' num='NUM_1380'  cont='Y' />\n"
        L"</HappenPath>\n";

    attic::a_document sourceDoc;
    CPPUNIT_ASSERT(sourceDoc.load_wide(c_data));

    Hem::HappenPath path(sourceDoc.document_element());
    CPPUNIT_ASSERT_EQUAL(size_t(4), path.GetEventsCount());
    CPPUNIT_ASSERT_EQUAL(size_t(3), path.GetInfoSpots().size());

    // Десериализация не поддерживается, если нитка уже заполнена
    CPPUNIT_ASSERT_THROW(path << sourceDoc.document_element(), HemHelpful::HemException);

    attic::a_document serializeDoc("HappenPath");
    path >> serializeDoc.document_element();

    {
        const std::string source = sourceDoc.pretty_str();
        const std::string target = serializeDoc.pretty_str();
        CPPUNIT_ASSERT_EQUAL(source, target);
    }
}

void TC_HappenPath::BasicOperations()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeArrival, BadgeE(L"Badge", EsrKit(1379)), c_start));
    Hem::HappenPath path(firstSpot);
    CPPUNIT_ASSERT(!path.GetFragment(firstSpot));

    std::wstring error;
    {
        SpotEvent tmpSpot(c_codeTransition, BadgeE(L"Badge2", EsrKit(10000)), c_start + 5);
        CPPUNIT_ASSERT(!path.CanAppend(tmpSpot, error));
    }
    
    {
        SpotEvent tmpSpot(c_codeDeparture, BadgeE(L"Badge2", EsrKit(10000)), c_start - 5);
        CPPUNIT_ASSERT(!path.CanAppend(tmpSpot, error));
    }

    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"Badge2", EsrKit(10000)), c_start + 5));
    CPPUNIT_ASSERT(path.CanAppend(*secondSpot, error));
    path.DoAppendCopy(*secondSpot);

    CPPUNIT_ASSERT(path.RemoveEvent(secondSpot));
    CPPUNIT_ASSERT(!path.RemoveEvent(secondSpot));

    path.DoAppendCopy(*secondSpot);

    TrainDescr trainDescr(L"N2222", L"1-2-3");
    path.SetInfoToSpot(firstSpot, trainDescr);
    {
        auto resultPtr = path.GetFragment(firstSpot);
        CPPUNIT_ASSERT(resultPtr);
        CPPUNIT_ASSERT(trainDescr == *resultPtr);

        resultPtr = path.GetFragment(secondSpot);
        CPPUNIT_ASSERT(resultPtr);
        CPPUNIT_ASSERT(trainDescr == *resultPtr);
    }

    CPPUNIT_ASSERT(path.RemoveEvent(firstSpot));
    {
        auto resultPtr = path.GetFragment(secondSpot);
        CPPUNIT_ASSERT(resultPtr);
        CPPUNIT_ASSERT(trainDescr == *resultPtr);
    }

    CPPUNIT_ASSERT(!path.RemoveEvent(secondSpot));
    CPPUNIT_ASSERT_EQUAL(c_start + 5, path.GetLatestTime());
}


void TC_HappenPath::SetInfoToSpot()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);

    const TrainDescr info(L"N1379", L"1234-456-678");
    path.SetInfoToSpot(firstSpot, info);
    auto infoPtr = path.GetFragment(firstSpot);
    CPPUNIT_ASSERT(infoPtr && *infoPtr == info);
}

void TC_HappenPath::GetFragments()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);

    auto fragmentsVec = path.GetInfoSpots();
    CPPUNIT_ASSERT_EQUAL(size_t(0), fragmentsVec.size());

    const TrainDescr info(L"N1379", L"1234-456-678");
    path.SetInfoToSpot(firstSpot, info);

    fragmentsVec = path.GetInfoSpots();
    CPPUNIT_ASSERT_EQUAL(size_t(1), fragmentsVec.size());
    CPPUNIT_ASSERT_EQUAL(c_start, fragmentsVec.front().first);
    CPPUNIT_ASSERT(info == *fragmentsVec.front().second);
}

void TC_HappenPath::RemoveInfoFromSpot()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    const TrainDescr info(L"N1379", L"1234-456-678");

    // Удаление единственного описания в начале нитки
    path.SetInfoToSpot(firstSpot, info);
    path.RemoveInfoFromSpot(*firstSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    // Удаление единственного описания в середине нитки
    path.SetInfoToSpot(secondSpot, info);
    path.RemoveInfoFromSpot(*secondSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    // Удаление единственного описания в конце нитки
    path.SetInfoToSpot(thirdSpot, info);
    path.RemoveInfoFromSpot(*thirdSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    // Удаление описания со второй точки, на которую действует описание первой точки
    // должно создать пустое описание на второй точке
    path.SetInfoToSpot(firstSpot, info);
    path.RemoveInfoFromSpot(*secondSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == info);
    CPPUNIT_ASSERT(path.GetFragment(secondSpot)->empty());
    CPPUNIT_ASSERT(path.GetFragment(thirdSpot)->empty());

    // Удаление описания с точки, за которой следует пустое,
    // созданное для очистки воздействия описания указанной точки
    path.RemoveInfoFromSpot(*firstSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());
}

void TC_HappenPath::TrainDescriptions()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);

    TrainDescr trainDescr(L"NU13BER", L"1-2-3");
    path.SetInfoToSpot(firstSpot, trainDescr);
    path.SetInfoToSpot(secondSpot, trainDescr);
    {
        auto firstDescr = path.GetFragment(firstSpot);
        auto secondDescr = path.GetFragment(secondSpot);
        auto thirdDescr = path.GetFragment(thirdSpot);
        CPPUNIT_ASSERT(firstDescr && *firstDescr == trainDescr);
        CPPUNIT_ASSERT(secondDescr && *secondDescr == trainDescr);
        CPPUNIT_ASSERT(thirdDescr && *thirdDescr == trainDescr);
    }

    path.RemoveInfoFromSpot(*firstSpot);
    {
        auto firstDescr = path.GetFragment(firstSpot);
        auto secondDescr = path.GetFragment(secondSpot);
        auto thirdDescr = path.GetFragment(thirdSpot);
        CPPUNIT_ASSERT(!firstDescr);
        CPPUNIT_ASSERT(!secondDescr);
        CPPUNIT_ASSERT(!thirdDescr);
    }

    path.SetInfoToSpot(firstSpot, trainDescr);
    path.RemoveInfoFromSpot(*secondSpot);
    {
        auto firstDescr = path.GetFragment(firstSpot);
        auto secondDescr = path.GetFragment(secondSpot);
        auto thirdDescr = path.GetFragment(thirdSpot);
        CPPUNIT_ASSERT(firstDescr && *firstDescr == trainDescr);
        CPPUNIT_ASSERT(secondDescr && secondDescr->empty());
        CPPUNIT_ASSERT(thirdDescr && thirdDescr->empty());
    }
}

void TC_HappenPath::Duplicates()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    const TrainDescr info(L"N1379", L"1234-456-678");
    const TrainDescr info2(L"K1000", L"9876-654-432");

    // Добавление описания в третью точку произойти не должно, ведь предыдущее описание точно такое же
    path.SetInfoToSpot(firstSpot, info);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    path.SetInfoToSpot(thirdSpot, info);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(path.GetFragment(firstSpot));

    path.RemoveInfoFromSpot(*firstSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    // При добавлении описания в первую точку должно удаляться описание из третьей
    path.SetInfoToSpot(thirdSpot, info);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    path.SetInfoToSpot(firstSpot, info);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());

    path.RemoveInfoFromSpot(*firstSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    path.SetInfoToSpot(thirdSpot, info);
    path.SetInfoToSpot(secondSpot, info2);
    path.SetInfoToSpot(firstSpot, info);
    CPPUNIT_ASSERT_EQUAL(size_t(3), path.GetInfoSpots().size());
}

void TC_HappenPath::CanReplace()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));
    HemHelpful::SpotEventPtr secondSpot2(new SpotEvent(c_codeDeparture, BadgeE(L"BDG 2", EsrKit(1000)), c_start + 30));
    HemHelpful::SpotEventPtr thirdSpot2(new SpotEvent(c_codeArrival, BadgeE(L"BDG 2", EsrKit(2000)), c_start + 50));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);

    CPPUNIT_ASSERT_EQUAL(size_t(3), path.GetAllEvents().size());

    std::wstring message;
    CPPUNIT_ASSERT(path.CanReplace(*secondSpot, *thirdSpot, *secondSpot2, *thirdSpot2, message));
    CPPUNIT_ASSERT(message.empty());

    // Отказ из-за того, что события не находятся в нити
    CPPUNIT_ASSERT(!path.CanReplace(*secondSpot2, *thirdSpot, *secondSpot2, *thirdSpot2, message));
    CPPUNIT_ASSERT(!path.CanReplace(*secondSpot, *thirdSpot2, *secondSpot2, *thirdSpot2, message));

    // Отказ из-за нарушения временной последовательности после замены
    CPPUNIT_ASSERT(!path.CanReplace(*firstSpot, *firstSpot, *thirdSpot2, *thirdSpot2, message));
    CPPUNIT_ASSERT(!path.CanReplace(*secondSpot, *secondSpot, *thirdSpot2, *thirdSpot2, message));

    CPPUNIT_ASSERT(path.CanReplace(*firstSpot, *firstSpot, *firstSpot, *firstSpot, message));
    CPPUNIT_ASSERT(path.CanReplace(*thirdSpot, *thirdSpot, *thirdSpot2, *thirdSpot2, message));
}

void TC_HappenPath::ReplaceEvents()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    SpotEvent secondSpot(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start);
    SpotEvent thirdSpot(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60);
    std::vector<SpotEvent> replaceWith;
    replaceWith.emplace_back(c_codeDeparture, BadgeE(L"BDG 2", EsrKit(1000)), c_start + 30);
    replaceWith.emplace_back(c_codeArrival, BadgeE(L"BDG 2", EsrKit(2000)), c_start + 50);

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(secondSpot);
    path.DoAppendCopy(thirdSpot);

    auto result = path.Replace(secondSpot, thirdSpot, replaceWith);
    CPPUNIT_ASSERT_EQUAL(size_t(2), result.first.size());
    CPPUNIT_ASSERT_EQUAL(size_t(2), result.second.size());
    CPPUNIT_ASSERT(*result.first.front() == secondSpot);
    CPPUNIT_ASSERT(*result.first.back() == thirdSpot);
    CPPUNIT_ASSERT(*result.second.front() == replaceWith[0]);
    CPPUNIT_ASSERT(*result.second.back() == replaceWith[1]);

    CPPUNIT_ASSERT_EQUAL(size_t(3), path.GetAllEvents().size());
}

void TC_HappenPath::ReplaceEventsFragments()
{
    const BadgeE badge1(L"BDG", EsrKit(1000));
    const BadgeE badge2(L"B2G", EsrKit(2000));
    TrainDescr descr1(L"1379", L"1379-973-1375");
    TrainDescr descr2(L"2222", L"2222-222-2222");
    SpotDetailsCPtr details1(new SpotDetails(descr1));
    SpotDetailsCPtr details2(new SpotDetails(descr2));
    HemHelpful::SpotEventPtr formEvent(new SpotEvent(c_codeForm, badge1, c_start, details1));
    Hem::HappenPath path(formEvent);
    
    std::vector<SpotEvent> body1;
    body1.emplace_back(c_codeDeparture, badge1, c_start + 1, details2);
    body1.emplace_back(c_codeArrival, badge2, c_start + 61);
    body1.emplace_back(c_codeDeparture, badge2, c_start + 121, details1);
    for (const auto& tmp : body1)
        path.DoAppendCopy(tmp);
    
    std::vector<SpotEvent> body2;
    body2.emplace_back(c_codeDeparture, badge1, c_start + 2);
    body2.emplace_back(c_codeArrival, badge2, c_start + 63, details2);

    auto removedAndInserted = path.Replace(body1[0], body1[1], body2);
    CPPUNIT_ASSERT_EQUAL(size_t(2), removedAndInserted.first.size());
    CPPUNIT_ASSERT_EQUAL(body2.size(), removedAndInserted.second.size());

    CPPUNIT_ASSERT(*path.GetFragment(formEvent) == descr1);
    CPPUNIT_ASSERT(*path.GetFragment(removedAndInserted.second.front()) == descr1);
    CPPUNIT_ASSERT(*path.GetFragment(removedAndInserted.second.back()) == descr2);
    CPPUNIT_ASSERT(*path.GetFragment(path.GetAllEvents().back()) == descr1);
}

void TC_HappenPath::Concatenate()
{
    BadgeE badge0(L"UT", EsrKit(1379));
    time_t time0(100);
    TrainDescr trainDescr0;
    unsigned number0 = 32167;
    trainDescr0.SetNumber(std::to_wstring(number0));
    SpotDetailsPtr spotDetails0(new SpotDetails(trainDescr0));

    using namespace HemHelpful;
    using namespace Hem;
    SpotEventPtr p0s0(new SpotEvent(HCode::FORM, badge0, time0, spotDetails0));
    SpotEventPtr p0s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 1));
    HappenPath path0(p0s0);
    path0.DoAppendCopy(*p0s1);
    CPPUNIT_ASSERT_EQUAL(number0, path0.GetFragment(p0s0)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number0, path0.GetFragment(p0s1)->GetNumber().getNumber());
    
    TrainDescr trainDescr1;
    unsigned number1 = 76123;
    trainDescr1.SetNumber(std::to_wstring(number1));
    SpotDetailsPtr spotDetails1(new SpotDetails(trainDescr1));
    SpotEventPtr p1s0(new SpotEvent(HCode::ARRIVAL, badge0, time0 + 2));
    SpotEventPtr p1s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 3, spotDetails1));
    HappenPath path1(p1s0);
    path1.DoAppendCopy(*p1s1);
    CPPUNIT_ASSERT(nullptr == path1.GetFragment(p1s0));
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p1s1)->GetNumber().getNumber());

    {
        auto eventsAddedRemoved = path1.Concatenate(path0, *p0s1, std::vector<SpotEventPtr>(), *p0s1, identifyCategory);
        CPPUNIT_ASSERT(eventsAddedRemoved.first.empty());
        CPPUNIT_ASSERT(eventsAddedRemoved.second.empty());
    }

    std::vector<SpotEventPtr> bridge;
    {
        auto eventsAddedRemoved = path1.Concatenate(path0, *p0s1, bridge, *p1s0, identifyCategory);
        CPPUNIT_ASSERT_EQUAL(path0.GetAllEvents().size(), eventsAddedRemoved.first.size());
        CPPUNIT_ASSERT(eventsAddedRemoved.second.empty());
    }

    // В составе нитки path0 события должны получить 
    CPPUNIT_ASSERT_EQUAL(number0, path1.GetFragment(p0s0)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number0, path1.GetFragment(p0s1)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number0, path1.GetFragment(p1s0)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p1s1)->GetNumber().getNumber());
}


void TC_HappenPath::Concatenate2()
{
    BadgeE badge0(L"UT", EsrKit(1379));
    time_t time0(100);
    TrainDescr trainDescr0;
    SpotDetailsPtr spotDetails0(new SpotDetails(trainDescr0));

    using namespace HemHelpful;
    using namespace Hem;
    SpotEventPtr p0s0(new SpotEvent(HCode::FORM, badge0, time0, spotDetails0));
    SpotEventPtr p0s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 1));
    HappenPath path0(p0s0);
    path0.DoAppendCopy(*p0s1);
    CPPUNIT_ASSERT(!path0.GetFragment(p0s0));
    CPPUNIT_ASSERT(!path0.GetFragment(p0s1));

    TrainDescr trainDescr1;
    unsigned number1 = 76123;
    trainDescr1.SetNumber(std::to_wstring(number1));
    SpotDetailsPtr spotDetails1(new SpotDetails(trainDescr1));
    SpotEventPtr p1s0(new SpotEvent(HCode::ARRIVAL, badge0, time0 + 2));
    SpotEventPtr p1s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 3, spotDetails1));
    HappenPath path1(p1s0);
    path1.DoAppendCopy(*p1s1);
    CPPUNIT_ASSERT(nullptr == path1.GetFragment(p1s0));
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p1s1)->GetNumber().getNumber());

    std::vector<SpotEventPtr> bridge;
    {
        auto eventsAddedRemoved = path1.Concatenate(path0, *p0s1, bridge, *p1s0, identifyCategory);
        CPPUNIT_ASSERT_EQUAL(path0.GetAllEvents().size(), eventsAddedRemoved.first.size());
        CPPUNIT_ASSERT(eventsAddedRemoved.second.empty());
    }

    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p0s0)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p0s1)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p1s0)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p1s1)->GetNumber().getNumber());
}

void TC_HappenPath::ConcatenateEmptyTailFragment()
{
    BadgeE badge0(L"UT", EsrKit(1379));
    time_t time0(100);
    TrainDescr trainDescr0;
    unsigned number0 = 32167;
    trainDescr0.SetNumber(std::to_wstring(number0));
    SpotDetailsPtr spotDetails0(new SpotDetails(trainDescr0));

    TrainDescr trainDescrEmpty;
    SpotDetailsPtr spotDetailsEmpty(new SpotDetails(trainDescrEmpty));

    using HemHelpful::SpotEventPtr;
    SpotEventPtr p0s0(new SpotEvent(HCode::FORM, badge0, time0, spotDetails0));
    SpotEventPtr p0s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 1, spotDetailsEmpty));
    Hem::HappenPath path0(p0s0);
    path0.DoAppendCopy(*p0s1);
    CPPUNIT_ASSERT_EQUAL(number0, path0.GetFragment(p0s0)->GetNumber().getNumber());
    CPPUNIT_ASSERT(path0.GetFragment(p0s1));
    CPPUNIT_ASSERT(path0.GetFragment(p0s1)->empty());

    TrainDescr trainDescr1;
    unsigned number1 = 76123;
    trainDescr1.SetNumber(std::to_wstring(number1));
    SpotDetailsPtr spotDetails1(new SpotDetails(trainDescr1));
    SpotEventPtr p1s0(new SpotEvent(HCode::ARRIVAL, badge0, time0 + 2));
    SpotEventPtr p1s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 3, spotDetails1));
    Hem::HappenPath path1(p1s0);
    path1.DoAppendCopy(*p1s1);
    CPPUNIT_ASSERT(nullptr == path1.GetFragment(p1s0));
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p1s1)->GetNumber().getNumber());

    std::vector<SpotEventPtr> bridge;
    {
        auto eventsAddedRemoved = path1.Concatenate(path0, *p0s1, bridge, *p1s0, identifyCategory);
        CPPUNIT_ASSERT_EQUAL(path0.GetAllEvents().size(), eventsAddedRemoved.first.size());
        CPPUNIT_ASSERT(eventsAddedRemoved.second.empty());
    }

    CPPUNIT_ASSERT_EQUAL(number0, path1.GetFragment(p0s0)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p0s1)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p1s0)->GetNumber().getNumber());
    CPPUNIT_ASSERT_EQUAL(number1, path1.GetFragment(p1s1)->GetNumber().getNumber());
}

void TC_HappenPath::ReplaceLastEventFragment()
{
    const BadgeE badge1(L"iu1", EsrKit(100,200));
    const BadgeE badge2(L"iu2", EsrKit(100,200));
    const BadgeE badge3(L"iu3", EsrKit(100,200));
    const BadgeE badge4(L"iu4", EsrKit(100,200));
    TrainDescr descr1(L"1001", L"1001-101-101");
    TrainDescr descr2(L"1111", L"1111-111-111");
    TrainDescr descr4(L"4444", L"1111-111-111");
    SpotDetailsCPtr details1(new SpotDetails(descr1));
    SpotDetailsCPtr details2(new SpotDetails(descr2));
    SpotDetailsCPtr details4(new SpotDetails(descr4));
    HemHelpful::SpotEventPtr moveEvent1(     new SpotEvent(c_codeSpanmove, badge1, c_start+10, details1));
    HemHelpful::SpotEventPtr moveEvent2_pure(new SpotEvent(c_codeSpanmove, badge2, c_start+20));
    HemHelpful::SpotEventPtr moveEvent2_frag(new SpotEvent(c_codeSpanmove, badge2, c_start+20, details2));
    HemHelpful::SpotEventPtr moveEvent3(     new SpotEvent(c_codeSpanmove, badge3, c_start+30));
    HemHelpful::SpotEventPtr moveEvent4_pure(new SpotEvent(c_codeSpanmove, badge4, c_start+40));
    HemHelpful::SpotEventPtr moveEvent4_frag(new SpotEvent(c_codeSpanmove, badge4, c_start+40, details4));

    Hem::HappenPath path(moveEvent1);
    path.DoAppendCopy(*moveEvent2_pure);
    path.DoAppendCopy(*moveEvent3);
    path.DoAppendCopy(*moveEvent4_pure);
    auto frags_before = path.GetInfoSpots();
    CPPUNIT_ASSERT_EQUAL(size_t(1), frags_before.size());
    CPPUNIT_ASSERT_EQUAL(c_start+10, frags_before.front().first );

    std::vector<SpotEvent> reEv;
    reEv.push_back(*moveEvent2_frag);
    reEv.push_back(*moveEvent3);
    reEv.push_back(*moveEvent4_frag);
    path.Replace( *moveEvent2_pure, *moveEvent4_pure, reEv );
    auto frags_after = path.GetInfoSpots();
    CPPUNIT_ASSERT_EQUAL(size_t(3), frags_after.size());
    CPPUNIT_ASSERT_EQUAL(c_start+10, frags_after.front().first );
    frags_after.pop_front();
    CPPUNIT_ASSERT_EQUAL(c_start+20, frags_after.front().first );
    frags_after.pop_front();
    CPPUNIT_ASSERT_EQUAL(c_start+40, frags_after.front().first );
}

void TC_HappenPath::InsertSpots()
{
    const BadgeE badge1(L"BDG", EsrKit(1000));
    const BadgeE badge2(L"B2G", EsrKit(2000));
    TrainDescr descr1(L"1379", L"1379-973-1375");
    TrainDescr descr2(L"2222", L"2222-222-2222");
    SpotDetailsCPtr details1(new SpotDetails(descr1));
    SpotDetailsCPtr details2(new SpotDetails(descr2));
    HemHelpful::SpotEventPtr formEvent(new SpotEvent(c_codeForm, badge1, c_start, details1));
    Hem::HappenPath path(formEvent);

    std::vector<SpotEvent> body1;
    body1.emplace_back(c_codeDeparture, badge1, c_start + 64);
    body1.emplace_back(c_codeArrival, badge2, c_start + 65, details2);
    body1.emplace_back(c_codeDeparture, badge2, c_start + 121, details1);
    path.DoInsert(body1, formEvent);
    CPPUNIT_ASSERT_EQUAL(body1.size() + 1u, path.GetAllEvents().size());
    CPPUNIT_ASSERT_EQUAL(size_t(3), path.GetInfoSpots().size());

    std::vector<SpotEvent> body2;
    body2.emplace_back(c_codeDeparture, badge1, c_start + 2);
    body2.emplace_back(c_codeArrival, badge2, c_start + 63, details2);

    path.DoInsert(body2, formEvent);
    CPPUNIT_ASSERT_EQUAL(body1.size() + body2.size() + 1u, path.GetAllEvents().size());
    const auto fragments = path.GetInfoSpots();
    CPPUNIT_ASSERT_EQUAL(size_t(3), fragments.size());
    auto fragIt = fragments.cbegin();
    CPPUNIT_ASSERT_EQUAL(c_start, fragIt->first);
    CPPUNIT_ASSERT(std::wstring(L"1379-973-1375") == fragIt->second->GetIndex().str());
    ++fragIt;
    CPPUNIT_ASSERT_EQUAL(c_start + 63, fragIt->first);
    CPPUNIT_ASSERT(std::wstring(L"2222-222-2222") == fragIt->second->GetIndex().str());
    ++fragIt;
    CPPUNIT_ASSERT_EQUAL(c_start + 121, fragIt->first);
    CPPUNIT_ASSERT(std::wstring(L"1379-973-1375") == fragIt->second->GetIndex().str());
}

void TC_HappenPath::SelectStationEvent()
{
    attic::a_document sourceDoc;
    {
        const std::wstring c_data = 
            L"<HappenPath>\n"
            L"  <SpotEvent name='Arrival' Bdg='Badge[10000]' create_time='19700101T000100Z' index='1379-137-1379' num='1379' />\n"
            L"  <SpotEvent name='Departure' Bdg='Badge[10000]' create_time='19700101T000200Z' />\n"
            L"  <SpotEvent name='Transition' Bdg='Badge[11000]' create_time='19700101T010000Z' />\n"
            L"  <SpotEvent name='Transition' Bdg='Badge[12000]' create_time='19700101T020000Z' index='1-1-1' num='1379' psng='Y' />\n"
            L"</HappenPath>\n";
        CPPUNIT_ASSERT(sourceDoc.load_wide(c_data));
    }
    Hem::HappenPath path(sourceDoc.document_element());

    const EsrKit esr(10000);
    CPPUNIT_ASSERT(esr.terminal());
    interval_time_t requestInterval(100, 140);
    auto spot2 = path.SelectStationEvent(esr, requestInterval, TrainDescr(L"1379", L"1379-137-1379"), HCode::DEPARTURE, false);
    CPPUNIT_ASSERT(spot2);
    CPPUNIT_ASSERT(spot2->GetTime() == 120);

    requestInterval = interval_time_t(0, 200);
    auto withoutNumberSpot = path.SelectStationEvent(esr, requestInterval, TrainDescr(L"", L"1379-137-1379"), HCode::ARRIVAL, false);
    CPPUNIT_ASSERT(withoutNumberSpot);
    CPPUNIT_ASSERT(withoutNumberSpot->GetTime() == 60);

    auto withoutIndexSpot = path.SelectStationEvent(esr, requestInterval, TrainDescr(L"1379", L""), HCode::ARRIVAL, false);
    CPPUNIT_ASSERT(withoutIndexSpot);
    CPPUNIT_ASSERT(withoutIndexSpot == withoutNumberSpot);

	//подходит по индексу!
    CPPUNIT_ASSERT(path.SelectStationEvent(esr, requestInterval, TrainDescr(L"7777", L"1379-137-1379"), HCode::ARRIVAL, false));
    //не подходит
	CPPUNIT_ASSERT(!path.SelectStationEvent(esr, requestInterval, TrainDescr(L"1379", L"13-137-1379"), HCode::ARRIVAL, false));
    CPPUNIT_ASSERT(!path.SelectStationEvent(esr, requestInterval, TrainDescr(), HCode::ARRIVAL, false));
}

static std::set<EsrKit> GetSetEsr(const EsrKit& esr)
{
    std::set<EsrKit> vRes;
    vRes.insert(esr);
    return vRes;
}

void TC_HappenPath::Directions()
{
    attic::a_document sourceDoc;
    {
        const std::wstring c_data = 
            L"<HappenPath>\n"
            L"  <SpotEvent name='Arrival' Bdg='Badge[10000]' create_time='19700101T000100Z' index='1379-137-1379' num='1379' />\n"
            L"  <SpotEvent name='Departure' Bdg='Badge[10000]' create_time='19700101T000200Z' />\n"
            L"  <SpotEvent name='Transition' Bdg='Badge[11000]' create_time='19700101T010000Z' />\n"
            L"  <SpotEvent name='Transition' Bdg='Badge[12000]' create_time='19700101T020000Z' index='1-1-1' num='1379' psng='Y' />\n"
            L"</HappenPath>\n";
        CPPUNIT_ASSERT(sourceDoc.load_wide(c_data));
    }
    Hem::HappenPath path(sourceDoc.document_element());
    auto spotPtr = path.GetLastEvent();
    CPPUNIT_ASSERT(spotPtr);

    CPPUNIT_ASSERT(path.PassedStations(EsrKit(10000), GetSetEsr(EsrKit(11000)), *spotPtr));
    CPPUNIT_ASSERT(path.PassedStations(EsrKit(11000), GetSetEsr(EsrKit(11000)), *spotPtr));
    CPPUNIT_ASSERT(!path.PassedStations(EsrKit(11000), GetSetEsr(EsrKit(10000)), *spotPtr));
    CPPUNIT_ASSERT(!path.PassedStations(EsrKit(1313), GetSetEsr(EsrKit(3344)), *spotPtr));
    CPPUNIT_ASSERT(!path.PassedStations(EsrKit(10000), GetSetEsr(EsrKit(11000)), SpotEvent(c_codeDeparture, BadgeE(L"qq",EsrKit(666)), 0)));

    spotPtr = path.GetAllEvents().front();
    CPPUNIT_ASSERT(spotPtr);
    CPPUNIT_ASSERT(path.IsMovingTowardsStations(*spotPtr, GetSetEsr(EsrKit(11000)), EsrKit(12000)));
    CPPUNIT_ASSERT(path.IsMovingTowardsStations(*spotPtr, GetSetEsr(EsrKit(11000)), EsrKit(11000)));
    CPPUNIT_ASSERT(!path.IsMovingTowardsStations(*spotPtr, GetSetEsr(EsrKit(12000)), EsrKit(12000)));
    CPPUNIT_ASSERT(!path.IsMovingTowardsStations(SpotEvent(c_codeDeparture, BadgeE(L"qq",EsrKit(666)), 0), GetSetEsr(EsrKit(11000)), EsrKit(12000)));
}

void TC_HappenPath::SpanDirections()
{
    attic::a_document sourceDoc;
    {
        const std::wstring c_data = 
            L"<HappenPath>\n"
            //L"  <SpotEvent name='Arrival' Bdg='Badge[10000]' create_time='19700101T000100Z' index='1379-137-1379' num='1379' />\n"
            //L"  <SpotEvent name='Departure' Bdg='Badge[10000]' create_time='19700101T000200Z' />\n"
            //L"  <SpotEvent name='Span_move' Bdg='Badge[10000:11000]' create_time='19700101T000300Z' />\n"
            //L"  <SpotEvent name='Transition' Bdg='Badge[11000]' create_time='19700101T010000Z' />\n"
            L"  <SpotEvent name='Span_move' Bdg='Badge[11000:12000]' create_time='19700101T015000Z' />\n"
            L"  <SpotEvent name='Transition' Bdg='Badge[12000]' create_time='19700101T020000Z' index='1-1-1' num='1379' psng='Y' />\n"
            L"</HappenPath>\n";
        CPPUNIT_ASSERT(sourceDoc.load_wide(c_data));
    }
    Hem::HappenPath path(sourceDoc.document_element());
    auto spotPtr = path.GetLastEvent();
    CPPUNIT_ASSERT(spotPtr);

    CPPUNIT_ASSERT(path.PassedStations(EsrKit(10000), GetSetEsr(EsrKit(11000)), *spotPtr));
}

void TC_HappenPath::SeparateBeginning()
{
    TrainDescr descr1(L"1379", L"1379-973-1375");
    TrainDescr descr4(L"2222", L"1379-973-1375");
    SpotDetailsCPtr details1(new SpotDetails(descr1));
    SpotDetailsCPtr details4(new SpotDetails(descr4));

    HemHelpful::SpotEventPtr spot1(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start, details1));
    HemHelpful::SpotEventPtr spot2(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr spot3(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));
    HemHelpful::SpotEventPtr spot4(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(2000)), c_start + 120, details4));
    HemHelpful::SpotEventPtr spot5(new SpotEvent(c_codeTransition, BadgeE(L"BDG", EsrKit(3000)), c_start + 180));

    Hem::HappenPath sourcePath(spot1);
    sourcePath.DoAppendCopy(*spot2);
    sourcePath.DoAppendCopy(*spot3);
    sourcePath.DoAppendCopy(*spot4);
    sourcePath.DoAppendCopy(*spot5);

    SpotEvent spotPrefix1(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start + 10);
    SpotEvent spotPrefix2(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start + 10);
    std::vector<SpotEvent> prefixSpots;
    prefixSpots.push_back(spotPrefix1);
    prefixSpots.push_back(spotPrefix2);

    // Если после разделения нить не меняется, отделять нечего
    CPPUNIT_ASSERT(!sourcePath.SeparateBeginning(*spot1, prefixSpots, true).first);
    // Не должно быть возможным указывать отсутствующие в нитке события
    CPPUNIT_ASSERT_THROW(sourcePath.SeparateBeginning(spotPrefix2, prefixSpots, true).first, HemHelpful::HemException);
    // Не должно быть возможным указывать отсутствующие в нитке события
    CPPUNIT_ASSERT_THROW(sourcePath.SeparateBeginning(spotPrefix1, prefixSpots, true).first, HemHelpful::HemException);

    std::shared_ptr<Hem::HappenPath> headPath = sourcePath.SeparateBeginning(*spot3, prefixSpots, true).first;
    CPPUNIT_ASSERT(headPath);

    // Старая нитка должна содержать префикс и оставшиеся после отделения события
    auto oldPathEvents = sourcePath.GetAllEvents();
    CPPUNIT_ASSERT_EQUAL(5u, static_cast<unsigned>(oldPathEvents.size()));
    CPPUNIT_ASSERT(*oldPathEvents.front() == spotPrefix1);
    CPPUNIT_ASSERT(*oldPathEvents.front() == spotPrefix1);
    HemHelpful::SpotEventPtr spot = *std::next(oldPathEvents.begin());
    CPPUNIT_ASSERT(*spot == spotPrefix2); // События префиксов передаются по указателю
    CPPUNIT_ASSERT(*spot == spotPrefix2);

    // В начале старой нитки должен быть хвост фрагмента, оставшийся от её начала до отделения
    auto descr = sourcePath.GetFragment(oldPathEvents.front());
    CPPUNIT_ASSERT(descr);
    CPPUNIT_ASSERT(*descr == descr1);

    // Фрагмент в середине старой нитки никуда не должен был деться
    descr = sourcePath.GetFragment(spot4);
    CPPUNIT_ASSERT(descr);
    CPPUNIT_ASSERT(*descr == descr4);

    // Новая нитка должна содержать события начала старой
    auto newPathEvents = headPath->GetAllEvents();
    CPPUNIT_ASSERT_EQUAL(2u, static_cast<unsigned>(newPathEvents.size()));
    CPPUNIT_ASSERT(*newPathEvents.front() == *spot1);

    // Новая нитка должна содержать фрагменты начала старой
    descr = headPath->GetFragment(spot1);
    CPPUNIT_ASSERT(descr);
    CPPUNIT_ASSERT(*descr == descr1);
}

void TestIdentifyWith(const Hem::IdentifyPolicy& policy, const UtIdentifyCategoryList& identifyCategory)
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    const TrainDescr info(L"N1379", L"1234-456-678");
    const TrainDescr info2(L"K1001", L"9876-654-432");

    // Добавление описания в третью точку произойти не должно, ведь предыдущее описание точно такое же
    path.IdentifyWith(firstSpot, info, identifyCategory, Hem::Chance::System(), policy);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    path.SetInfoToSpot(thirdSpot, info);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(path.GetFragment(firstSpot));

    path.RemoveInfoFromSpot(*firstSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    // При добавлении описания в первую точку должно удаляться описание из третьей
    path.SetInfoToSpot(thirdSpot, info);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    path.IdentifyWith(firstSpot, info, identifyCategory, Hem::Chance::System(), policy);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());

    path.RemoveInfoFromSpot(*firstSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    path.IdentifyWith(thirdSpot, info, identifyCategory, Hem::Chance::System(), policy);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(path.GetFragment(firstSpot));
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == info);

    path.IdentifyWith(secondSpot, info, identifyCategory, Hem::Chance::System(), policy);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(path.GetFragment(firstSpot));
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == info);

    // Поведение при идентификации нити другим индексом:
    path.IdentifyWith(secondSpot, info2, identifyCategory, Hem::Chance::System(), policy);
    if (policy.overrideMismatchedPastFragments())
    {
        CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());
        CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == info);
        CPPUNIT_ASSERT(*path.GetFragment(secondSpot) == info2);
    }
    else
    {
        // сохранить противоречащие фрагменты
        CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());
        CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == info);
        CPPUNIT_ASSERT(*path.GetFragment(path.GetNextEvent(*firstSpot)) == info2);
    }
}

void TC_HappenPath::IdentifyWith()
{
    TestIdentifyWith(Hem::SoftIdentifyPolicy(), identifyCategory);
    TestIdentifyWith(Hem::StrictIdentifyPolicy(), identifyCategory);
    TestIdentifyWith(Hem::UserIdentifyPolicy(), identifyCategory);
}

void TC_HappenPath::IdentifyDublicates()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    const TrainDescr infoNum(L"1379", L"");
    const TrainDescr infoNumLit(L"J1379", L"");
    const TrainDescr infoNumWithIndex(L"1379", L"1234-456-678");
    const TrainDescr infoNumLitWithIndex(L"J1379", L"1234-456-678");

    const TrainDescr infoNumWrong(L"4444");

    // Устанавливаем правильный номер с литерой в середину нити
    path.SetInfoToSpot(secondSpot, infoNumLit);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(!path.GetFragment(firstSpot));
    CPPUNIT_ASSERT(*path.GetFragment(secondSpot) == infoNumLit);

    // Идентифицируем нить в конце (информация распространится на предыдущее событие
    // и перенесёт его в начало нити, не потеряв литеру)
    path.IdentifyWith(thirdSpot, infoNumWithIndex, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(path.GetFragment(firstSpot));
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == infoNumLitWithIndex);

    // Устанавливаем вручную правильный номер - он пополяется данными из первого события
    // и удаляется как дубликат
    path.SetInfoToSpot(secondSpot, infoNum);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == infoNumLitWithIndex);

    // Идентифицируем нить правильной информацией без литеры - она удаляется как дублирующая
    path.IdentifyWith(secondSpot, infoNum, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == infoNumLitWithIndex);

    path.IdentifyWith(thirdSpot, infoNumLit, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == infoNumLitWithIndex);

    // Пытаемся идентифицировать нить, перезаписав первый фрагмент фрагментом с правильным номером,
    // он дополняется данными из старого фрагмента и не меняется
    path.IdentifyWith(firstSpot, infoNum, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == infoNumLitWithIndex);

    // Очистка нити
    path.RemoveInfoFromSpot(*firstSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    // Устанавливаем ошибочный номер на середину
    path.SetInfoToSpot(secondSpot, infoNumWrong);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(secondSpot) == infoNumWrong);

//не стираем неправильные номера автоматически!!
     path.IdentifyWith(thirdSpot, infoNumWithIndex, identifyCategory, Hem::Chance::System(), Hem::UserIdentifyPolicy());
     CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());
     CPPUNIT_ASSERT(*path.GetFragment(secondSpot) == infoNumWrong);
     CPPUNIT_ASSERT(*path.GetFragment(thirdSpot) == infoNumWithIndex);
// Идентификация после этого должна стереть "неправильный" номер
//     path.IdentifyWith(thirdSpot, infoNumWithIndex, Hem::Chance::System(), Hem::UserIdentifyPolicy());
//     CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
//     CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == infoNumWithIndex);

     // Идентификация поверх этого номера отбрасывает его и распространяет 
     // информацию на всю нить
     path.IdentifyWith(secondSpot, infoNumWithIndex, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
     CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
     CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == infoNumWithIndex);

    // Очистка нити
    path.RemoveInfoFromSpot(*firstSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    // Устанавливаем ошибочный номер на середину
    path.SetInfoToSpot(secondSpot, infoNumWrong);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(secondSpot) == infoNumWrong);

    // Идентификация без перезаписи неправильных номеров
    path.IdentifyWith(thirdSpot, infoNumWithIndex, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(secondSpot) == infoNumWrong);
    CPPUNIT_ASSERT(*path.GetFragment(thirdSpot) == infoNumWithIndex);

    path.IdentifyWith(secondSpot, infoNumWithIndex, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(*path.GetFragment(firstSpot) == infoNumWithIndex);
}

void TC_HappenPath::RemoveFragmentsIf()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppendCopy(*secondSpot);
    path.DoAppendCopy(*thirdSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    std::wstring const number = L"N1379";
    int const numberInt = 1379;
    std::wstring const index = L"1111-222-3333";
    std::wstring const otherIndex = L"9999-999-9999";
    
    TrainDescr const tdNumIndex(number, index);
    TrainDescr const tdNum(number, L"");
    TrainDescr const tdIndex(L"", index);

    TrainDescr const tdOtherIndex(number, otherIndex);

    auto isIndex = [index](const TrainDescr& trainDescr) {
        return trainDescr.GetIndex().str() == index;
    };
    auto isNumIndex = [number, index](const TrainDescr& trainDescr) {
        return trainDescr.GetNumber().getString() == number
            && trainDescr.GetIndex().str() == index;
    };
    auto isNumOrIndex = [numberInt, index](const TrainDescr& trainDescr) {
        return trainDescr.GetNumber().getNumber() == numberInt
            || trainDescr.GetIndex().str() == index;
    };
    auto isOtherIndex = [otherIndex](const TrainDescr& trainDescr) {
        return trainDescr.GetIndex().str() == otherIndex;
    };

    path.SetInfoToSpot(firstSpot, tdNumIndex);
    path.RemoveFragmentsIf(isNumIndex);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    path.IdentifyWith(firstSpot, tdIndex, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    path.RemoveFragmentsIf(isNumIndex);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    path.RemoveFragmentsIf(isIndex);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    path.IdentifyWith(firstSpot, tdIndex, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    path.RemoveInfoFromSpot(*secondSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());
    path.RemoveFragmentsIf(isIndex);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    path.IdentifyWith(firstSpot, tdNumIndex, identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    path.IdentifyWith(secondSpot, tdOtherIndex, identifyCategory, Hem::Chance::System(), Hem::UserIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());
    path.RemoveFragmentsIf(isIndex);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    path.RemoveFragmentsIf(isOtherIndex);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    path.SetInfoToSpot(firstSpot, tdNumIndex);
    path.SetInfoToSpot(secondSpot, tdOtherIndex);
    path.SetInfoToSpot(thirdSpot, tdNumIndex);
    CPPUNIT_ASSERT_EQUAL(size_t(3), path.GetInfoSpots().size());
    path.RemoveFragmentsIf(isNumIndex);
    CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetInfoSpots().size());
    CPPUNIT_ASSERT(!path.GetFragment(firstSpot));
    CPPUNIT_ASSERT(*path.GetFragment(secondSpot) == tdOtherIndex);
    CPPUNIT_ASSERT(path.GetFragment(thirdSpot));
    CPPUNIT_ASSERT(path.GetFragment(thirdSpot)->empty());
}

static CDistrict TC_District( const std::vector<EsrKit>& esrs )
{
   return CDistrict(L"TC_District", L"1000", false, esrs, std::vector<EsrKit>(), std::vector<EsrKit>(), std::vector<EsrKit>());
}


CDistrict PrepareElgavaVentspilsDistrict()
{
    /*
    <District code="09180-09820" name="Jelgava-Ventspils">   <!-- елгава-вентспилс - вкладка вентспилс-елгава-->
        <Involve esr="09820"/>    <!-- вентспилс -->
        <Involve esr="09813"/>    <!-- вентспилс-2 -->
        <Involve esr="09840"/>    <!-- вентспилс -->
        <Involve esr="09850"/>    <!-- вентспилс -->
        <Involve esr="09880"/>    <!-- вентспилс -->
        <Involve esr="09851"/>    <!-- вентспилс -->
        <Involve esr="09812"/>    <!-- элшкене -->
        <Involve esr="09810"/>    <!-- угале -->
        <Involve esr="09803"/>    <!-- усма -->
        <Involve esr="09802"/>    <!-- спаре -->
        <Involve esr="09801"/>    <!-- личи -->
        <Involve esr="09800"/>    <!-- стенде -->
        <Involve esr="09790"/>    <!-- сабиле -->
        <Involve esr="09780"/>    <!-- кандава -->
        <Involve esr="09751"/>    <!-- зваре -->
        <Involve esr="09750"/>    <!-- тукумс-2 -->
        <Involve esr="09772"/>     <!-- слампе -->
        <Involve esr="09764"/>     <!-- ливберзе -->
        <Involve esr="09764:09180"/>  <!-- ливберзе-елгава -->
        </District>
    */
    std::vector<EsrKit> elgavaVentspils;
    elgavaVentspils.emplace_back(9820);
    elgavaVentspils.emplace_back(9813);
    elgavaVentspils.emplace_back(9840);
    elgavaVentspils.emplace_back(9850);
    elgavaVentspils.emplace_back(9880);
    elgavaVentspils.emplace_back(9851);
    elgavaVentspils.emplace_back(9812);
    elgavaVentspils.emplace_back(9810);
    elgavaVentspils.emplace_back(9803);
    elgavaVentspils.emplace_back(9802);
    elgavaVentspils.emplace_back(9801);
    elgavaVentspils.emplace_back(9800);
    elgavaVentspils.emplace_back(9790);
    elgavaVentspils.emplace_back(9780);
    elgavaVentspils.emplace_back(9751);
    elgavaVentspils.emplace_back(9750);
    elgavaVentspils.emplace_back(9772);
    elgavaVentspils.emplace_back(9764);
    elgavaVentspils.emplace_back(9764, 9180);

    return CDistrict(L"Jelgava-Ventspils", L"09180-09820", false, elgavaVentspils, std::vector<EsrKit>(), std::vector<EsrKit>(), std::vector<EsrKit>() );
};


void TC_HappenPath::FindDistrictBorders()
{
    using HemHelpful::SpotEventPtr;

    SpotEventPtr spot0formPtr(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    SpotEvent const spot1dep(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start);
    SpotEvent const spot2span(c_codeSpanmove, BadgeE(L"BDG", EsrKit(1000, 1500)), c_start + 20);
    SpotEvent const spot3tr(c_codeTransition, BadgeE(L"BDG", EsrKit(1500)), c_start + 40);
    SpotEvent const spot4arr(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60);
    SpotEvent const spot5d(c_codeDeath, BadgeE(L"BDG", EsrKit(2000)), c_start + 65);

    Hem::HappenPath path(spot0formPtr);
    path.DoAppendCopy(spot1dep);
    path.DoAppendCopy(spot2span);
    path.DoAppendCopy(spot3tr);
    path.DoAppendCopy(spot4arr);
    path.DoAppendCopy(spot5d);

    std::vector<EsrKit> districtEsrs;
    districtEsrs.emplace_back(1000);
    districtEsrs.emplace_back(1000, 1500);
    districtEsrs.emplace_back(1500);
    districtEsrs.emplace_back(2000);

    // В участок попали все события нити, поэтому выбирается формирование
    // и последнее событие нити
    std::pair<SpotEventPtr, SpotEventPtr> borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == *spot0formPtr);
    CPPUNIT_ASSERT(*borders.second == spot5d);

    districtEsrs.clear();
    // В участок не попало ни одно событие, не должно быть и пересечений
    borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(!borders.first);
    CPPUNIT_ASSERT(!borders.second);

    // Имитируем ситуацию, когда в участок попадают события с 2 по 3,
    // мы должны получить отправление до участка и проследование последней станции участка
    districtEsrs.clear();
    districtEsrs.emplace_back(1000, 1500);
    districtEsrs.emplace_back(1500);
    borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == spot1dep);
    CPPUNIT_ASSERT(*borders.second == spot3tr);
}

void TC_HappenPath::FindDistrictBordersWithoutSpan()
{
    using HemHelpful::SpotEventPtr;

    SpotEventPtr spot0formPtr(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    SpotEvent const spot1dep(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start);
    SpotEvent const spot3tr(c_codeTransition, BadgeE(L"BDG", EsrKit(1500)), c_start + 40);
    SpotEvent const spot4arr(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60);
    SpotEvent const spot5d(c_codeDeath, BadgeE(L"BDG", EsrKit(2000)), c_start + 65);

    Hem::HappenPath path(spot0formPtr);
    path.DoAppendCopy(spot1dep);
    path.DoAppendCopy(spot3tr);
    path.DoAppendCopy(spot4arr);
    path.DoAppendCopy(spot5d);

    std::vector<EsrKit> districtEsrs;
    districtEsrs.emplace_back(1000);
    districtEsrs.emplace_back(1000, 1500);
    districtEsrs.emplace_back(1500);
    districtEsrs.emplace_back(2000);

    // В участок попали все события нити, поэтому выбирается первое и последнее событие нити
    std::pair<SpotEventPtr, SpotEventPtr> borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == *spot0formPtr);
    CPPUNIT_ASSERT(*borders.second == spot5d);

    districtEsrs.clear();
    districtEsrs.emplace_back(1000, 1500);
    districtEsrs.emplace_back(1500);
    districtEsrs.emplace_back(2000);
    borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == spot1dep);
    CPPUNIT_ASSERT(*borders.second == spot5d);
}

void TC_HappenPath::FindDistrictBordersLeavingDistrict()
{
    using HemHelpful::SpotEventPtr;

    SpotEventPtr spot0formPtr(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(9180)), c_start)); // Елгава
    SpotEventPtr spot1depPtr(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(9180)), c_start));
    SpotEvent const spot2tr(c_codeTransition, BadgeE(L"BDG", EsrKit(9764)), c_start + 60); // Либверзе
    SpotEvent const spot3tr(c_codeTransition, BadgeE(L"BDG", EsrKit(9772)), c_start + 120); // Слампе
    SpotEvent const spot4tr(c_codeTransition, BadgeE(L"BDG", EsrKit(9750)), c_start + 180); // Тукумс-2
    SpotEvent const spot5sp(c_codeSpanmove, BadgeE(L"BDG", EsrKit(9750, 9751)), c_start + 200); // Тукумс-2 : Звере
    SpotEvent const spot6d(c_codeDeath, BadgeE(L"BDG", EsrKit(9750, 9751)), c_start + 240);
    Hem::HappenPath path(spot0formPtr);
    path.DoAppendCopy(*spot1depPtr);
    path.DoAppendCopy(spot2tr);
    path.DoAppendCopy(spot3tr);
    path.DoAppendCopy(spot4tr);
    path.DoAppendCopy(spot5sp);
    path.DoAppendCopy(spot6d);
    CPPUNIT_ASSERT_EQUAL(size_t(7), path.GetEventsCount());

    std::vector<EsrKit> districtEsrs;
    districtEsrs.emplace_back(9180);

    // Имитируем участок, в который входит только Елгава
    std::pair<SpotEventPtr, SpotEventPtr> borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(*borders.first == *spot0formPtr);
    CPPUNIT_ASSERT(*borders.second == *spot1depPtr);
    //CPPUNIT_ASSERT(!borders.second);

    // Имитируем участок, захватывающий Елгаву и Либверзе
    districtEsrs.clear();
    districtEsrs.emplace_back(9180);
    districtEsrs.emplace_back(9764);
    borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == *spot0formPtr);
    CPPUNIT_ASSERT(*borders.second == spot2tr);

    // Добавляем движение по перегону Елгава-Либверзе
    SpotEvent spanEvent(c_codeSpanmove, BadgeE(L"BDG", EsrKit(9180, 9764)), c_start + 30);
    path.DoInsert(spanEvent, spot1depPtr);
    CPPUNIT_ASSERT_EQUAL(size_t(8), path.GetEventsCount());

    // Имитируем участок, в который входит Елгава и перегон Елгава-Либверзе
    districtEsrs.clear();
    districtEsrs.emplace_back(9180);
    districtEsrs.emplace_back(9180, 9764);
    borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == *spot0formPtr);
    CPPUNIT_ASSERT(*borders.second == spanEvent);
}

void TC_HappenPath::FindDistrictBordersEntrySpot()
{
    using HemHelpful::SpotEventPtr;

    SpotEventPtr spot0formPtr(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(9180)), c_start)); // Елгава
    SpotEvent const spot1dep(c_codeDeparture, BadgeE(L"BDG", EsrKit(9180)), c_start);
    SpotEvent const spot2span(c_codeSpanmove, BadgeE(L"BDG", EsrKit(9180, 9764)), c_start + 30);
    SpotEvent const spot3tr(c_codeTransition, BadgeE(L"BDG", EsrKit(9764)), c_start + 60); // Либверзе
    SpotEvent const spot4span(c_codeSpanmove, BadgeE(L"BDG", EsrKit(9764, 9772)), c_start + 90);
    SpotEvent const spot5tr(c_codeTransition, BadgeE(L"BDG", EsrKit(9772)), c_start + 120); // Слампе
    SpotEvent const spot6ar(c_codeArrival, BadgeE(L"BDG", EsrKit(9750)), c_start + 130); // Тукумс-2
    SpotEvent const spot7dp(c_codeDeparture, BadgeE(L"BDG", EsrKit(9750)), c_start + 180); // Тукумс-2
    SpotEvent const spot8sp(c_codeSpanmove, BadgeE(L"BDG", EsrKit(9750, 9751)), c_start + 200); // Тукумс-2 : Звере
    SpotEvent const spot9d(c_codeDeath, BadgeE(L"BDG", EsrKit(9750, 9751)), c_start + 240);
    Hem::HappenPath path(spot0formPtr);
    path.DoAppendCopy(spot1dep);
    path.DoAppendCopy(spot2span);
    path.DoAppendCopy(spot3tr);
    path.DoAppendCopy(spot4span);
    path.DoAppendCopy(spot5tr);
    path.DoAppendCopy(spot6ar);
    path.DoAppendCopy(spot7dp);
    path.DoAppendCopy(spot8sp);
    path.DoAppendCopy(spot9d);
    CPPUNIT_ASSERT_EQUAL(size_t(10), path.GetEventsCount());

    std::vector<EsrKit> districtEsrs;
    districtEsrs.emplace_back(9180);
    districtEsrs.emplace_back(9764);
    districtEsrs.emplace_back(9772);
    districtEsrs.emplace_back(9772, 9750);

    std::pair<SpotEventPtr, SpotEventPtr> borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == *spot0formPtr);
    CPPUNIT_ASSERT(*borders.second == spot6ar);

    districtEsrs.clear();
    districtEsrs.emplace_back(9180, 9764);
    districtEsrs.emplace_back(9764);
    districtEsrs.emplace_back(9772);
    districtEsrs.emplace_back(9772, 9750);
    borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == spot1dep);
    CPPUNIT_ASSERT(*borders.second == spot6ar);

    districtEsrs.clear();
    districtEsrs.emplace_back(9764);
    districtEsrs.emplace_back(9772);
    districtEsrs.emplace_back(9772, 9750);
    borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == spot3tr);
    CPPUNIT_ASSERT(*borders.second == spot6ar);

    districtEsrs.clear();
    districtEsrs.emplace_back(9764, 9772);
    districtEsrs.emplace_back(9772);
    districtEsrs.emplace_back(9772, 9750);
    borders = path.FindDistrictBorders(TC_District(districtEsrs));
    CPPUNIT_ASSERT(borders.first);
    CPPUNIT_ASSERT(borders.second);
    CPPUNIT_ASSERT(*borders.first == spot3tr);
    CPPUNIT_ASSERT(*borders.second == spot6ar);
}

void TC_HappenPath::FindDistrictBorders_3997()
{
    using HemHelpful::SpotEventPtr;

    CDistrict district = PrepareElgavaVentspilsDistrict();

    SpotEventPtr spot0Form(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(9180)), c_start));
    SpotEventPtr spot1Arr(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(9180)), c_start + 10));
    SpotEventPtr spot2Dep(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(9180)), c_start + 20));
    SpotEventPtr spot3Tr(new SpotEvent(c_codeTransition, BadgeE(L"BDG", EsrKit(9764)), c_start + 30));
    SpotEventPtr spot4Tr(new SpotEvent(c_codeTransition, BadgeE(L"BDG", EsrKit(9772)), c_start + 40));
    SpotEventPtr spot5Arr(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(9750)), c_start + 50));
    SpotEventPtr spot6Disform(new SpotEvent(c_codeDisform, BadgeE(L"BDG", EsrKit(9750)), c_start + 60));
    
    {
        Hem::HappenPath path(spot0Form);
        path.DoAppend(spot1Arr);
        path.DoAppend(spot2Dep);
        path.DoAppend(spot3Tr);
        path.DoAppend(spot4Tr);
        path.DoAppend(spot5Arr);
        path.DoAppend(spot6Disform);

        auto borders = path.FindDistrictBorders(district);
        CPPUNIT_ASSERT(borders.first);
        CPPUNIT_ASSERT(borders.second);
        CPPUNIT_ASSERT(*borders.first == *spot2Dep);
        CPPUNIT_ASSERT(*borders.second == *spot6Disform);
    }

    {
        Hem::HappenPath path(spot0Form);
        path.DoAppend(spot1Arr);
        path.DoAppend(spot2Dep);
        path.DoAppend(spot3Tr);

        auto borders = path.FindDistrictBorders(district);
        CPPUNIT_ASSERT(borders.first);
        CPPUNIT_ASSERT(borders.second);
        CPPUNIT_ASSERT(*borders.first == *spot2Dep);
        CPPUNIT_ASSERT(*borders.second == *spot3Tr);
    }

    {
        SpotEventPtr spot3Span(new SpotEvent(c_codeSpanmove, BadgeE(L"BDG", EsrKit(9180, 9764)), c_start + 30));
        Hem::HappenPath path(spot0Form);
        path.DoAppend(spot1Arr);
        path.DoAppend(spot2Dep);
        path.DoAppend(spot3Span);

        auto borders = path.FindDistrictBorders(district);
        CPPUNIT_ASSERT(borders.first);
        CPPUNIT_ASSERT(borders.second);
        CPPUNIT_ASSERT(*borders.first == *spot2Dep);
        CPPUNIT_ASSERT(*borders.second == *spot3Span);
    }

    {
        Hem::HappenPath path(spot0Form);
        path.DoAppend(spot1Arr);
        path.DoAppend(spot2Dep);

        auto borders = path.FindDistrictBorders(district);
        CPPUNIT_ASSERT(!borders.first);
        CPPUNIT_ASSERT(!borders.second);
    }

}

void TC_HappenPath::IdentifyWithOtherLiteral()
{
    using HemHelpful::SpotEventPtr;

    SpotEventPtr spot0formPtr(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    SpotEventPtr spot1depPtr(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));
    SpotEventPtr spot2spanPtr(new SpotEvent(c_codeSpanmove, BadgeE(L"BDG", EsrKit(1000, 1500)), c_start + 20));
    SpotEvent const spot3tr(c_codeTransition, BadgeE(L"BDG", EsrKit(1500)), c_start + 40);
    SpotEvent const spot4arr(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60);

    Hem::HappenPath path(spot0formPtr);
    path.DoAppendCopy(*spot1depPtr);
    path.DoAppendCopy(*spot2spanPtr);
    path.DoAppendCopy(spot3tr);
    path.DoAppendCopy(spot4arr);

    path.IdentifyWith(spot1depPtr, TrainDescr(L"J2211", L"1111-111-1111"), identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    path.IdentifyWith(spot2spanPtr, TrainDescr(L"V2211"), identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT_EQUAL(size_t(5), path.GetEventsCount());
    CPPUNIT_ASSERT(path.GetFragment(spot0formPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"J2211"), path.GetFragment(spot0formPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(spot0formPtr)->GetIndex().str());
    CPPUNIT_ASSERT(path.GetFragment(spot2spanPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"V2211"), path.GetFragment(spot2spanPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(spot2spanPtr)->GetIndex().str());

    path.IdentifyWith(spot2spanPtr, TrainDescr(L"T2211"), identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT(path.GetFragment(spot0formPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"J2211"), path.GetFragment(spot0formPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(spot0formPtr)->GetIndex().str());
    CPPUNIT_ASSERT(path.GetFragment(spot2spanPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"T2211"), path.GetFragment(spot2spanPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(spot2spanPtr)->GetIndex().str());

    TrainDescr const td5(L"D2211");
    SpotEvent spot5dep(c_codeDeparture, BadgeE(L"BDG", EsrKit(2000)), c_start + 80,
        std::make_shared<const SpotDetails>(td5));
    path.DoAppendCopy(spot5dep);
    CPPUNIT_ASSERT_EQUAL(size_t(6), path.GetEventsCount());
    CPPUNIT_ASSERT(path.GetFragment(spot0formPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"J2211"), path.GetFragment(spot0formPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(spot0formPtr)->GetIndex().str());
    CPPUNIT_ASSERT(path.GetFragment(spot2spanPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"T2211"), path.GetFragment(spot2spanPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(spot2spanPtr)->GetIndex().str());
    CPPUNIT_ASSERT(path.GetFragment(path.GetLastEvent()));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"D2211"), path.GetFragment(path.GetLastEvent())->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(path.GetLastEvent())->GetIndex().str());

    path.DoAppendCopy(SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(3000)), c_start + 100));
    CPPUNIT_ASSERT_EQUAL(size_t(7), path.GetEventsCount());
    CPPUNIT_ASSERT(path.GetFragment(spot0formPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"J2211"), path.GetFragment(spot0formPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(spot0formPtr)->GetIndex().str());
    CPPUNIT_ASSERT(path.GetFragment(spot2spanPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"T2211"), path.GetFragment(spot2spanPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(spot2spanPtr)->GetIndex().str());
    CPPUNIT_ASSERT(path.GetFragment(path.GetLastEvent()));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"D2211"), path.GetFragment(path.GetLastEvent())->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"1111-111-1111"), path.GetFragment(path.GetLastEvent())->GetIndex().str());
}

void TC_HappenPath::IdentifyThroughTwoLiterals()
{
    using HemHelpful::SpotEventPtr;

    SpotEventPtr spot0formPtr(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    SpotEventPtr spot1depPtr(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start));

    TrainDescr const td2(L"V2203");
    SpotEventPtr spot2spanPtr(new SpotEvent(c_codeSpanmove, BadgeE(L"BDG", EsrKit(1000, 1500)), c_start + 20,
        std::make_shared<const SpotDetails>(td2)));
    TrainDescr const td3(L"T2203");
    SpotEventPtr const spot3trPtr(new SpotEvent(c_codeTransition, BadgeE(L"BDG", EsrKit(1500)), c_start + 40,
        std::make_shared<const SpotDetails>(td3)));
    SpotEvent const spot4arr(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60);

    Hem::HappenPath path(spot0formPtr);
    path.DoAppendCopy(*spot1depPtr);
    path.DoAppendCopy(*spot2spanPtr);
    path.DoAppendCopy(*spot3trPtr);
    path.DoAppendCopy(spot4arr);

    CPPUNIT_ASSERT_EQUAL(size_t(5), path.GetEventsCount());
    CPPUNIT_ASSERT(!path.GetFragment(spot0formPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"V2203"), path.GetFragment(spot2spanPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"T2203"), path.GetFragment(spot3trPtr)->GetNumber().getString());

    path.IdentifyWith(spot1depPtr, TrainDescr(L"2203", L"2203-222-0003"), identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT(path.GetFragment(spot0formPtr));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"2203"), path.GetFragment(spot0formPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"2203-222-0003"), path.GetFragment(spot0formPtr)->GetIndex().str());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"V2203"), path.GetFragment(spot2spanPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"T2203"), path.GetFragment(spot3trPtr)->GetNumber().getString());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"2203-222-0003"), path.GetFragment(spot2spanPtr)->GetIndex().str());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"2203-222-0003"), path.GetFragment(spot3trPtr)->GetIndex().str());
}

std::shared_ptr<const SpotDetails> makeSpotDetails(const std::wstring& _Number = L"", const std::wstring& _Index = L"")
{
    return std::make_shared<const SpotDetails>(TrainDescr(_Number, _Index));
}

void TC_HappenPath::EmptyTrainDescriptions()
{
    using HemHelpful::SpotEventPtr;
    BadgeE const badge1(L"BDG1", EsrKit(1000));
    BadgeE const badge2(L"BDG2", EsrKit(2000));

    Hem::HappenPath path(SpotEventPtr(new SpotEvent(c_codeForm, badge1, c_start)));
    path.DoAppendCopy(SpotEvent(c_codeDeparture, badge1, c_start + 1, makeSpotDetails(L"", L"")));
    CPPUNIT_ASSERT_EQUAL(size_t(2), path.GetEventsCount());
    // TODO: Поведение под вопросом: терять пустые описания нельзя, но они приходят
    // слишком часто. Пусть пустые описания отбрасываются, если до них других фрагментов нет.
    CPPUNIT_ASSERT(!path.GetFragment(path.GetLastEvent()));
    CPPUNIT_ASSERT(!path.IsIdentified());

    path.DoAppendCopy(SpotEvent(c_codeArrival, badge2, c_start + 2, makeSpotDetails(L"V2012", L"")));
    CPPUNIT_ASSERT_EQUAL(size_t(3), path.GetEventsCount());
    CPPUNIT_ASSERT(path.GetFragment(path.GetLastEvent()));
    CPPUNIT_ASSERT(path.IsIdentified());

    path.DoAppendCopy(SpotEvent(c_codeDeparture, badge2, c_start + 3, makeSpotDetails(L"", L"")));
    CPPUNIT_ASSERT_EQUAL(size_t(4), path.GetEventsCount());
    CPPUNIT_ASSERT(path.GetFragment(path.GetLastEvent()));
    CPPUNIT_ASSERT(path.GetFragment(path.GetLastEvent())->empty());
    
    // ToDo: Последний фрагмент пустой, но в нити сохраняется фрагмент с номером перед ним
    CPPUNIT_ASSERT(path.IsIdentified());
}

void TC_HappenPath::SetInfoToSpotsRange()
{
    HemHelpful::SpotEventPtr firstSpot(new SpotEvent(c_codeForm, BadgeE(L"BDG", EsrKit(1000)), c_start));
    HemHelpful::SpotEventPtr secondSpot(new SpotEvent(c_codeDeparture, BadgeE(L"BDG", EsrKit(1000)), c_start + 10));
    HemHelpful::SpotEventPtr thirdSpot(new SpotEvent(c_codeArrival, BadgeE(L"BDG", EsrKit(2000)), c_start + 60));

    Hem::HappenPath path(firstSpot);
    path.DoAppend(secondSpot);
    path.DoAppend(thirdSpot);
    CPPUNIT_ASSERT_EQUAL(size_t(0), path.GetInfoSpots().size());

    const TrainDescr infoEmpty(L"", L"");
    const TrainDescr infoNumLit(L"J1379", L"");
    
    // Устанавливаем правильный номер с литерой в середину нити
    path.SetInfoToSpot(firstSpot, infoNumLit);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());

    path.SetInfoToSpotsRangeStartingAt(*firstSpot, infoEmpty, identifyCategory, Hem::Chance::System());
    CPPUNIT_ASSERT(path.GetInfoSpots().empty());

    path.SetInfoToSpot(firstSpot, infoNumLit);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());

    path.SetInfoToSpotsRange(*firstSpot, *secondSpot, infoEmpty, Hem::Chance::System());
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT_EQUAL(secondSpot->GetTime(), path.GetInfoSpots().front().first);
    CPPUNIT_ASSERT(infoNumLit == *path.GetInfoSpots().front().second);

    path.SetInfoToSpotsRange(*firstSpot, *thirdSpot, infoEmpty, Hem::Chance::System());
    CPPUNIT_ASSERT_EQUAL(size_t(1), path.GetInfoSpots().size());
    CPPUNIT_ASSERT_EQUAL(thirdSpot->GetTime(), path.GetInfoSpots().front().first);
    CPPUNIT_ASSERT(infoNumLit == *path.GetInfoSpots().front().second);
}