#include "stdafx.h"

#include "TC_Hem_aeUserIdentify.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/HappenPath.h"
#include "../Hem/Appliers/aeCut.h"
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Hem/Appliers/aeUserIdentify.h"
#include "../Hem/Appliers/Context.h"
#include "../Hem/Bill.h"
#include "../Hem/UserEditInventory.h"
#include "../helpful/DistrictGuide.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/TrainSpecialNotes.h"
#include "../Asoup/Message.h"
#include "TopologyTest.h"
#include "UtHelpfulDateTime.h"
#include "../Hem/Appliers/aeGetPossibleIdentification.h"
#include "../Hem/Demand.h"
#include "../Hem/GuessTransciever.h"
#include "../helpful/NsiBasis.h"
#include "../Hem/UserChart.h"
#include "../helpful/Crew.h"
#include "../Hem/XlatEsrParkWay.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "../helpful/Badge.h"

using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeUserIdentify );

using namespace HemHelpful;

static void VerifyEmptyDescriptions(const HappenLayer::EventWithDescrList& path)
{
    for (auto it = path.begin(); it != path.end(); ++it)
    {
        CPPUNIT_ASSERT(!it->second);
    }
}

class UtUserChart : public UserChart
{
public:
	UserChart::getHappenLayer;
	UserChart::getAsoupLayer;
};

void TC_Hem_aeUserIdentify::setUp()
{
    topology.reset( new TopologyTest );
    esrGuide.reset( new EsrGuide );
    guessTransciever.reset( new Hem::GuessTransciever );
    m_isPassengerWay = [](const EsrKit&, const ParkWayKit&) -> bool {
        return true;
    };

    SpotEvent path1[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T + 10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T + 15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T + 20 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T + 30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T + 40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T + 50 ),
    };

    SpotEvent path2[] = {
        SpotEvent( HCode::FORM, BadgeE(L"1p", EsrKit(100)), T + 10 + 60 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T + 15 + 60 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T + 20 + 60 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T + 30 + 60 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T + 40 + 60 ),
    };
    
    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    m_asoupLayer = std::make_shared<AsoupLayer>();
    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();
    m_happenLayer->createPath(path1);
    m_happenLayer->createPath(path2);
    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)m_happenLayer->path_count());

    CPPUNIT_ASSERT(!GetPath_1().empty());
    CPPUNIT_ASSERT(!GetPath_2().empty());
    Hem::DistrictSection::Shutdowner();
}

HappenLayer::EventWithDescrList TC_Hem_aeUserIdentify::GetPath_1()
{
    std::list<HappenLayer::EventWithDescrList> pathes = m_happenLayer->GetPaths_EventAndDescr(T, T + 50);
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)pathes.size());
    CPPUNIT_ASSERT_EQUAL(6u, (unsigned)pathes.front().size());
    return pathes.front();
}

HappenLayer::EventWithDescrList TC_Hem_aeUserIdentify::GetPath_2()
{
    std::list<HappenLayer::EventWithDescrList> pathes = m_happenLayer->GetPaths_EventAndDescr(T + 60, T + 50 + 60);
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)pathes.size());
    CPPUNIT_ASSERT_EQUAL(5u, (unsigned)pathes.front().size());
    return pathes.front();
}


template <size_t L>
void VerifyTrainNumbers(const HappenLayer::EventWithDescrList& path, unsigned numbers[L])
{
    auto it = path.begin();
    for (size_t i = 0; i < L; ++i)
    {
        std::wstringstream ss;
        ss << L"При сравнении " << i + 1 << L" события в нити";
        std::string msg = To1251(ss.str());
        if (numbers[i])
        {
            CPPUNIT_ASSERT_MESSAGE(msg, it->second);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, numbers[i], it->second->GetNumber().getNumber());
        }
        else
        {
            if (it->second)
            {
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, numbers[i], it->second->GetNumber().getNumber());
            }
        }
        ++it;
    }
}

void TC_Hem_aeUserIdentify::Asoup()
{
    Hem::Bill bill;
    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PathEvent name='Transition' Bdg='2p[200]' create_time='19700101T010020Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='2p[200]' num='1379' index='' name='Transition' waynum='1' parknum='5' create_time='19700101T010022Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));
        bill.read(doc.document_element());
    }

    AsoupEventPtr asoupPtr;
    {
        TrainDescr trainDescr(L"1379", L"", TrainSpecialNotes());
        asoupPtr.reset(new AsoupEvent(HCode::TRANSITION, 
            BadgeE(L"2p", EsrKit(200)), ParkWayKit(5, 1), T + 22, trainDescr));
    }
        
    CPPUNIT_ASSERT(m_asoupLayer->UT_GetEvents().empty());
    m_asoupLayer->AddAsoupEvent(asoupPtr);
    CPPUNIT_ASSERT(!m_asoupLayer->UT_GetEvents().empty());
    CPPUNIT_ASSERT(m_asoupLayer->FindEvent(*asoupPtr));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(asoupPtr));
    
    {
        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0 );
		Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT_MESSAGE("Failed to apply asoup identification to hem path", applier.UT_WasCompleted());
    }

    {
        // Проверка "привязанности" АСОУП события к нужному SpotEvent'у
        CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));
        AsoupLayer::SpotEventAddress servedAddress = m_asoupLayer->GetServedMessage(asoupPtr);
        CPPUNIT_ASSERT(!servedAddress.empty());

        HemHelpful::SpotEventPtr spotPtr(new SpotEvent(HCode::TRANSITION, BadgeE(L"2p", EsrKit(200)), T + 20));
        AsoupLayer::SpotEventAddress spotAddress(*spotPtr);
        CPPUNIT_ASSERT(spotAddress == servedAddress);
    }

    unsigned numbers1[] = { 1379, 1379, 1379, 1379, 1379, 1379 };
    VerifyTrainNumbers<6>(GetPath_1(), numbers1);
    VerifyEmptyDescriptions(GetPath_2());

    std::wstring res = m_happenLayer->result()->pretty_wstr();
    CPPUNIT_ASSERT(!res.empty());

    // TODO: Check correctness of happen layer descriptions
}

void TC_Hem_aeUserIdentify::UserDataWithPostPosition()
{
    Hem::Bill bill;
    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PrePoint name='Transition' Bdg='2p[200]' create_time='19700101T010020Z' cover='y' />\n"
            L"    <Action code='Identify'>\n"
            L"        <TrainInfo num='1379' index='' long='Y' heavy='Y' explosive='N' up='5' side='2' down='' super='Y' machinist='Иванов'>\n"
            L"            <lok num='625' />\n"
            L"            <lok num='121' />\n"
            L"        </TrainInfo>\n"
            L"    </Action>\n"
            L"    <PostPoint name='Transition' cover='y' Bdg='4p[400]' waynum='1' parknum='5' create_time='19700101T010040Z' />\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));
        bill.read(doc.document_element());
    }

    {
        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT_MESSAGE("Failed to apply asoup identification to hem path", applier.UT_WasCompleted());
    }

    unsigned numbers1[] = { 0, 0, 1379, 1379, 0, 0 };
    VerifyTrainNumbers<6>(GetPath_1(), numbers1);
    VerifyEmptyDescriptions(GetPath_2());

    std::wstring res = m_happenLayer->result()->pretty_wstr();
    CPPUNIT_ASSERT(!res.empty());
}

void TC_Hem_aeUserIdentify::UserData_0()
{
    Hem::Bill bill;
    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PrePoint name='Transition' cover='y' Bdg='2p[200]' create_time='19700101T010020Z'/>\n"
            L"    <Action code='Identify'>\n"
            L"        <TrainInfo num='1379' index='' long='Y' heavy='Y' explosive='N' up='5' side='2' down='' super='Y' machinist='Иванов'>\n"
            L"            <lok num='625' />\n"
            L"            <lok num='121' />\n"
            L"        </TrainInfo>\n"
            L"    </Action>\n"
            L"    <PostPoint name='Transition' Bdg='5p[500]' waynum='1' parknum='5' create_time='19700101T010050Z' />\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));
        bill.read(doc.document_element());
    }

    {
        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT_MESSAGE("Failed to apply user data identification to hem path", applier.UT_WasCompleted());
    }

    unsigned numbers1[] = { 0, 0, 1379, 1379, 0, 0 };
    VerifyTrainNumbers<6>(GetPath_1(), numbers1);
    VerifyEmptyDescriptions(GetPath_2());
    
    std::wstring res = m_happenLayer->result()->pretty_wstr();
    CPPUNIT_ASSERT(!res.empty());

    // TODO: Check correctness of happen layer descriptions
}

void TC_Hem_aeUserIdentify::UserData_1()
{
    Hem::Bill bill;
    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PrePoint name='Transition' cover='y' Bdg='2p[200]' create_time='19700101T010020Z'/>\n"
            L"    <Action code='Identify'>\n"
            L"        <TrainInfo num='1379' index='' long='Y' heavy='Y' explosive='N' up='5' side='2' down='' super='Y' machinist='Иванов'>\n"
            L"            <lok num='625' />\n"
            L"            <lok num='121' />\n"
            L"        </TrainInfo>\n"
            L"    </Action>\n"
            L"    <PostPoint name='Transition' cover='y' Bdg='5p[500]' waynum='1' parknum='5' create_time='19700101T010050Z' />\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));
        bill.read(doc.document_element());
    }

    {
        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT_MESSAGE("Failed to apply user data identification to hem path", applier.UT_WasCompleted());
    }

    unsigned numbers1[] = { 0, 0, 1379, 1379, 1379, 1379 };
    VerifyTrainNumbers<6>(GetPath_1(), numbers1);
    VerifyEmptyDescriptions(GetPath_2());

    std::wstring res = m_happenLayer->result()->pretty_wstr();
    CPPUNIT_ASSERT(!res.empty());

    // TODO: Check correctness of happen layer descriptions
}


class UtDistrictGuide : public DistrictGuide
{
public:
    UtDistrictGuide()
    {}
    void Generate(unsigned from, unsigned to);
};

void UtDistrictGuide::Generate(unsigned from, unsigned to )
{
    std::wstring name = L"N"+std::to_wstring(from)+std::to_wstring(to);
    std::wstring code = L"D"+std::to_wstring(from)+std::to_wstring(to);
    std::vector<EsrKit> vec;
    while( from<=to )
    {
        vec.emplace_back( EsrKit( from ) );
        from += 100;
    }
    Append( CDistrict(name,code, false, vec, std::vector<EsrKit>(), std::vector<EsrKit>(), std::vector<EsrKit>()) );
}

void TC_Hem_aeUserIdentify::UserData_2()
{
    // задаем номер на всю нить, поскольку нить без номера, номер устанавливается на всю нить, 
    // независимо от текущих прав
    {
        UtDistrictGuide dg;
        dg.Generate(100,300);
        dg.Generate(400,500);

        Hem::Bill bill = createBill(
            L"<Changing>\n"
            L"    <PrePoint name='Form' cover='y' Bdg='1p[100]' create_time='19700101T010010Z'/>\n"
            L"    <Action code='Identify' District='D100300' DistrictRight='Y'>\n"
            L"        <TrainInfo num='1379' index='1111-222-3333' />\n"
            L"    </Action>\n"
            L"</Changing>"
            );

        Hem::DistrictSection::instance()->Set( dg );
        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT(applier.UT_WasCompleted());

        unsigned numbers[] = { 1379, 1379, 1379, 1379, 1379, 1379 };
        VerifyTrainNumbers<6>(GetPath_1(), numbers);
        VerifyEmptyDescriptions(GetPath_2());
    }
    // снова на всю нить, но права шире и номер попадает везде
    {
        UtDistrictGuide dg;
        dg.Generate(100,700);

        Hem::Bill bill = createBill(
            L"<Changing>\n"
            L"    <PrePoint name='Form' cover='y' Bdg='1p[100]' create_time='19700101T010010Z'/>\n"
            L"    <Action code='Identify' District='D100700' DistrictRight='Y'>\n"
            L"        <TrainInfo num='3311' index='3333-555-2222' />\n"
            L"    </Action>\n"
            L"</Changing>"
            );

        Hem::DistrictSection::instance()->Set( dg );
        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT(applier.UT_WasCompleted());

        unsigned numbers[] = { 3311, 3311, 3311, 3311, 3311, 3311 };
        VerifyTrainNumbers<6>(GetPath_1(), numbers);
        VerifyEmptyDescriptions(GetPath_2());
    }
}

void TC_Hem_aeUserIdentify::AsoupWithoutLayerEvent()
{
    Hem::Bill bill;
    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PathEvent name='Transition' Bdg='2p[200]' create_time='19700101T010020Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='2p[200]' num='1379' index='' name='Transition' waynum='1' parknum='5' create_time='19700101T010022Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));
        bill.read(doc.document_element());
    }
    
    {
        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
		applier.Action();
    }

    VerifyEmptyDescriptions(GetPath_1());
    VerifyEmptyDescriptions(GetPath_2());
}

void TC_Hem_aeUserIdentify::AsoupDisjoint()
{
    AsoupEventPtr asoupPtr;
    {
        TrainDescr trainDescr;
        trainDescr.SetNumber(L"1379");
        asoupPtr.reset(new AsoupEvent(HCode::TRANSITION, 
            BadgeE(L"2p", EsrKit(200)), ParkWayKit(5, 1), T + 22, trainDescr));
    }

    CPPUNIT_ASSERT(m_asoupLayer->UT_GetEvents().empty());
    m_asoupLayer->AddAsoupEvent(asoupPtr);
    CPPUNIT_ASSERT(!m_asoupLayer->UT_GetEvents().empty());
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(asoupPtr));
    
    VerifyEmptyDescriptions(GetPath_1());
    VerifyEmptyDescriptions(GetPath_2());

    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PathEvent name='Transition' Bdg='2p[200]' create_time='19700101T010020Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='2p[200]' num='1379' index='' name='Transition' waynum='1' parknum='5' create_time='19700101T010022Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());

        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT_MESSAGE("Failed to apply first asoup identification to hem path", applier.UT_WasCompleted());

        CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));

        unsigned numbers1[] = { 1379, 1379, 1379, 1379, 1379, 1379 };
        VerifyTrainNumbers<6>(GetPath_1(), numbers1);
        
        unsigned numbers2[] = { 0, 0, 0, 0, 0 };
        VerifyTrainNumbers<5>(GetPath_2(), numbers2);
    }

    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PathEvent name='Transition' Bdg='2p[200]' create_time='19700101T010120Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='2p[200]' num='1379' index='' name='Transition' waynum='1' parknum='5' create_time='19700101T010022Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());

        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT_MESSAGE("Failed to apply second asoup identification to hem path", applier.UT_WasCompleted());

        CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));

        unsigned numbers2[] = { 1379, 1379, 1379, 1379, 1379 };
        VerifyTrainNumbers<5>(GetPath_2(), numbers2);

        unsigned numbers1[] = { 0, 0, 0, 0, 0, 0 };
        // Тут нельзя проверять на отсутствие описаний, так как по ТЗ вставляются пустые описания
        VerifyTrainNumbers<6>(GetPath_1(), numbers1);
    }
}

void TC_Hem_aeUserIdentify::AsoupDoubleLink()
{
    AsoupEventPtr asoupPtr;
    {
        TrainDescr trainDescr;
        trainDescr.SetNumber(L"1379");
        asoupPtr.reset(new AsoupEvent(HCode::TRANSITION, 
            BadgeE(L"2p", EsrKit(200)), ParkWayKit(5, 1), T + 22, trainDescr));
    }

    CPPUNIT_ASSERT(m_asoupLayer->UT_GetEvents().empty());
    m_asoupLayer->AddAsoupEvent(asoupPtr);
    CPPUNIT_ASSERT(!m_asoupLayer->UT_GetEvents().empty());
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(asoupPtr));

    VerifyEmptyDescriptions(GetPath_1());

    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PathEvent name='Transition' Bdg='2p[200]' create_time='19700101T010020Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='2p[200]' num='1379' index='' name='Transition' waynum='1' parknum='5' create_time='19700101T010022Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());

        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT_MESSAGE("Failed to apply first asoup identification to hem path", applier.UT_WasCompleted());

        CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));

        unsigned numbers1[] = { 1379, 1379, 1379, 1379, 1379, 1379 };
        VerifyTrainNumbers<6>(GetPath_1(), numbers1);

        VerifyEmptyDescriptions(GetPath_2());
    }

    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PrePoint name='Transition' Bdg='3p[300]' create_time='19700101T010030Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='2p[200]' num='1379' index='' name='Transition' waynum='1' parknum='5' create_time='19700101T010022Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());

        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        applier.Action();
        CPPUNIT_ASSERT_MESSAGE("Failed to apply first asoup identification to hem path", applier.UT_WasCompleted());

        CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));

        unsigned numbers1[] = { 1379, 1379, 1379, 1379, 1379, 1379 };
        VerifyTrainNumbers<6>(GetPath_1(), numbers1);

        VerifyEmptyDescriptions(GetPath_2());
    }
}

void TC_Hem_aeUserIdentify::GroupedAsoupLink()
{
    TrainDescr trainDescr;
    trainDescr.SetNumber(L"1379");
    trainDescr.SetIndex(L"1111-222-333");

    AsoupEventPtr asoupPtr(new AsoupEvent(HCode::TRANSITION, 
        BadgeE(L"2p", EsrKit(200)), ParkWayKit(5, 1), T + 22, trainDescr));
    CPPUNIT_ASSERT(m_asoupLayer->UT_GetEvents().empty());
    m_asoupLayer->AddAsoupEvent(asoupPtr);
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_asoupLayer->UT_GetEvents().size());
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(asoupPtr));

    trainDescr.SetRelLength(44);
    AsoupEventPtr asoupPtr2(new AsoupEvent(HCode::ARRIVAL, 
        BadgeE(L"6p", EsrKit(600)), ParkWayKit(1, 1), T + 55, trainDescr));
    m_asoupLayer->AddAsoupEvent(asoupPtr2);
    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)m_asoupLayer->UT_GetEvents().size());
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(asoupPtr2));

    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PathEvent name='Transition' Bdg='2p[200]' create_time='19700101T010020Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='2p[200]' num='1379' index='' name='Transition' waynum='1' parknum='5' create_time='19700101T010022Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());

        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations,identifyCategory,  nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        CPPUNIT_ASSERT_NO_THROW(applier.Action());
        CPPUNIT_ASSERT_MESSAGE("Failed to apply first asoup identification to hem path", applier.UT_WasCompleted());

        CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));

        unsigned numbers1[] = { 1379, 1379, 1379, 1379, 1379, 1379 };
        VerifyTrainNumbers<6>(GetPath_1(), numbers1);

        VerifyEmptyDescriptions(GetPath_2());
    }

    CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));
    
    // Поведение изменилось: более привязывать АСОУП группами нельзя,
    // каждое АСОУП должно быть привязано явно
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(asoupPtr2));
}

void TC_Hem_aeUserIdentify::AsoupChainLinkAndUnlink()
{
    TrainDescr trainDescr;
    trainDescr.SetNumber(L"1379");
    trainDescr.SetIndex(L"1111-222-333");

    AsoupEventPtr asoupPtr(new AsoupEvent(HCode::TRANSITION, 
        BadgeE(L"5p", EsrKit(500)), ParkWayKit(5, 1), T + 52, trainDescr));
    CPPUNIT_ASSERT(m_asoupLayer->UT_GetEvents().empty());
    m_asoupLayer->AddAsoupEvent(asoupPtr);
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)m_asoupLayer->UT_GetEvents().size());
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(asoupPtr));

    trainDescr.SetRelLength(44);
    AsoupEventPtr asoupPtr2(new AsoupEvent(HCode::ARRIVAL, 
        BadgeE(L"2p", EsrKit(200)), ParkWayKit(1, 1), T + 25, trainDescr));
    m_asoupLayer->AddAsoupEvent(asoupPtr2);
    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)m_asoupLayer->UT_GetEvents().size());
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(asoupPtr2));

    {
        // Привязываем АСОУП к первой нити (оба должны подхватиться)
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PathEvent name='Transition' Bdg='4p[400]' create_time='19700101T010040Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='5p[500]' num='1379' index='1111-222-333' name='Transition' waynum='1' parknum='5' create_time='19700101T010052Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());

        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations,identifyCategory,  nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        CPPUNIT_ASSERT_NO_THROW(applier.Action());
        CPPUNIT_ASSERT_MESSAGE("Failed to apply first asoup identification to hem path", applier.UT_WasCompleted());

        //CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));
        //CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr2));
		CPPUNIT_ASSERT( m_asoupQMgr.haveUpdate(trainDescr) );

        unsigned numbers1[] = { 1379, 1379, 1379, 1379, 1379, 1379 };
        VerifyTrainNumbers<6>(GetPath_1(), numbers1);

        VerifyEmptyDescriptions(GetPath_2());
    }

    {
		m_asoupQMgr.clear();
		CPPUNIT_ASSERT( !m_asoupQMgr.haveUpdate(trainDescr) );
        // Привязываем АСОУП ко второй нити (отвязываем от первой)
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PathEvent name='Transition' Bdg='3p[300]' create_time='19700101T010130Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='5p[500]' num='1379' index='1111-222-333' name='Transition' waynum='1' parknum='5' create_time='19700101T010052Z' />\n"
            L"    </Action>\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());

        Hem::AccessHappenAsoupRegulatory access(m_happenLayer->base(), *m_asoupLayer, *m_regulatoryLayer);
        Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory,  nullptr, topology, nullptr, 0); 
        Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
        CPPUNIT_ASSERT_NO_THROW(applier.Action());
        CPPUNIT_ASSERT_MESSAGE("Failed to apply first asoup identification to hem path", applier.UT_WasCompleted());

//         CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr));
//         CPPUNIT_ASSERT(m_asoupLayer->IsServed(asoupPtr2));
		CPPUNIT_ASSERT( m_asoupQMgr.haveUpdate(trainDescr) );

        unsigned numbers1[] = { 1379, 1379, 1379, 1379, 1379 };
        VerifyTrainNumbers<5>(GetPath_2(), numbers1);

        VerifyEmptyDescriptions(GetPath_1());
    }
}

static std::shared_ptr<AsoupEvent> createAsoupEventPtr( const std::wstring& text, boost::gregorian::date receivedDate )
{
    std::shared_ptr<Asoup::Message> asoupMessagePtr = Asoup::Message::parse(text, receivedDate, Asoup::AsoupOperationMode::LDZ, true);
    CPPUNIT_ASSERT(asoupMessagePtr);
    if (!asoupMessagePtr)
        throw std::logic_error("Can't create ASOUP message");

    std::shared_ptr<AsoupEvent> asoupEventPtr = AsoupEvent::Create( *asoupMessagePtr, nullptr, nullptr, nullptr, BadgeE(), Asoup::AsoupOperationMode::LDZ );
    CPPUNIT_ASSERT(asoupEventPtr);
    if (!asoupEventPtr)
        throw std::logic_error("Can't create ASOUP event");
    return asoupEventPtr;
}

void TC_Hem_aeUserIdentify::UserIdentifyNearEnd()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20180124T101422Z' name='Form' Bdg='6C[11420]' waynum='6' parknum='1' />"
        L"<SpotEvent create_time='20180124T101422Z' name='Departure' Bdg='6C[11420]' waynum='6' parknum='1' />"
        L"<SpotEvent create_time='20180124T103910Z' name='Arrival' Bdg='5C[11760]' waynum='5' parknum='2' />"
        L"<SpotEvent create_time='20180124T105651Z' name='Departure' Bdg='5C[11760]' waynum='5' parknum='2' />"
        L"<SpotEvent create_time='20180124T105651Z' name='Arrival' Bdg='5AC[11760]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );
    AsoupEventPtr asoupEventPtr = createAsoupEventPtr( 
        L"(:1042 909/000+11760 3551 1142 012 1176 01 11420 24 01 12 39 01/05 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 011 00220 00000 040 95038253 95083077 000 000 00 000 010.90 000 010 000 95 000 010 000\n"
        L"Ю4 00000 1 02 000 010.90 000 010 95 000 010 \n"
        L"Ю4 11710 1 02 000 010.90 000 010 95 000 010 \n"
        L"Ю12 00 95038253 1 000 11710 01400 5552 000 00 00 00 00 00 0220 13 95 0950 04 106 11760 11710 00000 02 11760 34 00000 0000 025 0 0000 098704 128 000 00000000\n"
        L"Ю12 00 95045589 255255255255 \n"
        L"Ю12 00 95487005 255255255255 \n"
        L"Ю12 00 95070660 255255255255 \n"
        L"Ю12 00 95128070 231255255255 014008237\n"
        L"Ю12 00 95743423 255255255255 \n"
        L"Ю12 00 95716577 255255255255 \n"
        L"Ю12 00 95486403 231255255247 01400555209874 \n"
        L"Ю12 00 95031571 255255255255 \n"
        L"Ю12 00 95083077 255255255255 :)\n",
        boost::gregorian::date(2018,01,24)
        );
    al.AddAsoupEvent(asoupEventPtr);

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180124T114537Z'>"
        L"<PathEvent name='Arrival' Bdg='5AC[11760]' create_time='20180124T105651Z' waynum='5' parknum='1' />"
        L"<Action code='Identify' District='11420-11000-11310'>"
        L"<EventInfo layer='asoup' Bdg='ASOUP 1042[11760]' name='Arrival' create_time='20180124T103900Z' index='1142-012-1176' num='3551' length='11' weight='220' outbnd='Y' waynum='5' parknum='1'><![CDATA[(:1042 909/000+11760 3551 1142 012 1176 01 11420 24 01 12 39 01/05 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 011 00220 00000 040 95038253 95083077 000 000 00 000 010.90 000 010 000 95 000 010 000\n"
        L"Ю4 00000 1 02 000 010.90 000 010 95 000 010 \n"
        L"Ю4 11710 1 02 000 010.90 000 010 95 000 010 \n"
        L"Ю12 00 95038253 1 000 11710 01400 5552 000 00 00 00 00 00 0220 13 95 0950 04 106 11760 11710 00000 02 11760 34 00000 0000 025 0 0000 098704 128 000 00000000\n"
        L"Ю12 00 95045589 255255255255 \n"
        L"Ю12 00 95487005 255255255255 \n"
        L"Ю12 00 95070660 255255255255 \n"
        L"Ю12 00 95128070 231255255255 014008237\n"
        L"Ю12 00 95743423 255255255255 \n"
        L"Ю12 00 95716577 255255255255 \n"
        L"Ю12 00 95486403 231255255247 01400555209874 \n"
        L"Ю12 00 95031571 255255255255 \n"
        L"Ю12 00 95083077 255255255255 :)]]></EventInfo>\n"
        L"</Action>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",  "Form          Departure Arrival Departure Arrival " ) );
    CPPUNIT_ASSERT( hl.exist_series( "num",   "-             -         -       -         -       " ) );
    CPPUNIT_ASSERT( hl.exist_series( "index", "-             -         -       -         -       " ) );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",  "Form          Departure Arrival Departure Arrival " ) );
    CPPUNIT_ASSERT( hl.exist_series( "num",   "3551          -         -       -         -       " ) );
    CPPUNIT_ASSERT( hl.exist_series( "index", "1142-012-1176 -         -       -         -       " ) );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::UserIdentifyOnBoardStation()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<DistrictList>"
            L"<District code='09180-11420' name='Jelgava-Krustpils'>"   L"<!-- Елгава-Крустпилс- вкладка елгава- крустпилс -->"
            L"<Involve esr='09190:11420'/>"  L"<!-- даугава-крустпилс -->"
            L"<Involve esr='09190'/>"    L"<!-- даугава -->"
            L"<Involve esr='09191'/>"    L"<!-- селспилс -->"
            L"<Involve esr='09193'/>"    L"<!-- сеце -->"
            L"<Involve esr='09200'/>"    L"<!-- даудзева -->"
            L"<Involve esr='09202'/>"    L"<!-- мента -->"
            L"<Involve esr='09210'/>"    L"<!-- тауркалне -->"
            L"<Involve esr='09211'/>"    L"<!-- лачплесис -->"
            L"<Involve esr='09220'/>"    L"<!-- вецумниеки -->"
            L"<Involve esr='09230'/>"     L"<!--миса -->"
            L"<Involve esr='09240'/>"     L"<!--иецава -->"
            L"<Involve esr='09241'/>"      L"<!--залите -->"
            L"<Involve esr='09251'/>"     L"<!-- гароза -->"
            L"<Involve esr='09181'/>"    L"<!-- елгава-2 -->"
            L"<Involve esr='09180' border='Y' target='Y'/>"    L"<!-- елгава  -->"
            L"</District>"
            L"<District code='09000-11760' name='xxx'>"
            L"<Involve esr='11720'/>"
            L"<Involve esr='11760'/>"
            L"<Involve esr='09360'/>"
            L"<Involve esr='09370'/>"
            L"<Involve esr='09380'/>"
            L"<Involve esr='09400'/>"
            L"<Involve esr='09410'/>"
            L"<Involve esr='09420'/>"
            L"<Involve esr='09000:09420'/>"
            L"</District>"
            L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }
    Hem::DistrictSection::instance()->Set( dg );

    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190614T014323Z' name='Form' Bdg='TM6C[09202:09210]' waynum='1' index='' num='J2442' through='Y'>"
        L"<rwcoord picketing1_val='245~400' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='246~166' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190614T015019Z' name='Transition' Bdg='1C[09202]' optCode='09200:09202' />"
        L"<SpotEvent create_time='20190614T020244Z' name='Transition' Bdg='1C[09200]' waynum='1' parknum='1' intervalSec='2' optCode='09193:09200' />"
        L"<SpotEvent create_time='20190614T021137Z' name='Transition' Bdg='1C[09193]' waynum='1' parknum='1' intervalSec='2' optCode='09191:09193' />"
        L"<SpotEvent create_time='20190614T023258Z' name='Transition' Bdg='1C[09191]' waynum='1' parknum='1' intervalSec='27' optCode='09190:09191' />"
        L"<SpotEvent create_time='20190614T024213Z' name='Transition' Bdg='?p1w1[09190]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T025214Z' name='Transition' Bdg='16C[11420]' waynum='16' parknum='1' />"
        L"<SpotEvent create_time='20190614T030635Z' name='Arrival' Bdg='1C[11760]' waynum='1' parknum='1' index='' num='819' length='3' weight='176' mvps='Y' />"
        L"<SpotEvent create_time='20190614T030742Z' name='Departure' Bdg='1C[11760]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T030917Z' name='Transition' Bdg='1AC[11760]' waynum='1' parknum='2' intervalSec='63' />"
        L"<SpotEvent create_time='20190614T032259Z' name='Arrival' Bdg='1C[09360]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T032501Z' name='Departure' Bdg='1C[09360]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T033304Z' name='Arrival' Bdg='1C[09370]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T033514Z' name='Departure' Bdg='1C[09370]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T034218Z' name='Arrival' Bdg='2C[09380]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190614T034537Z' name='Departure' Bdg='2C[09380]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190614T035924Z' name='Arrival' Bdg='1C[09400]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T040238Z' name='Departure' Bdg='1C[09400]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T041444Z' name='Arrival' Bdg='3C[09410]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20190614T041746Z' name='Departure' Bdg='3C[09410]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20190614T042945Z' name='Transition' Bdg='1C[09420]' waynum='1' parknum='1' intervalSec='52' optCode='09000:09420' />"
        L"<SpotEvent create_time='20190614T043854Z' name='Arrival' Bdg='1AC[09000]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T044014Z' name='Departure' Bdg='1AC[09000]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190614T044517Z' name='Transition' Bdg='1JC[09006]' waynum='1' parknum='1' intervalSec='59'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190614T044951Z' name='Arrival' Bdg='10C[09010]' waynum='10' parknum='1' />"
        L"<SpotEvent create_time='20190614T044951Z' name='Disform' Bdg='10C[09010]' waynum='10' parknum='1' />"
        L"</HemPath>"
        );
    time_t tTransition9202 = (from_iso_string("20190614T015019Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"1C", EsrKit(9202)), tTransition9202);
    CPPUNIT_ASSERT(spotPtr);
    time_t tArrival11760 = (from_iso_string("20190614T030635Z") - from_time_t(0)).total_seconds();
    time_t tForm = (from_iso_string("20190614T014323Z") - from_time_t(0)).total_seconds();

    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );

    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );

    CPPUNIT_ASSERT( gis.front().first == tForm && gis.front().second && gis.front().second->GetNumber().getNumber()==2442 );
    CPPUNIT_ASSERT( gis.back().first == tArrival11760 && gis.back().second && gis.back().second->GetNumber().getNumber()==819 );


    AsoupEventPtr asoupEventPtr = createAsoupEventPtr( 
        L"(:1042 909/000+11420 2442 0982 024 1800 01 09180 14 06 06 18 01/06 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01376 00000 228 63134977 62747159 000 000 00 000 057.60 000 057 000\n"
        L"Ю3 530 00009661 1 01 57 0000 00000 MINJCHONOKS \n"
        L"Ю3 530 00009662 9\n"
        L"Ю4 11290 2 76 000 001.60 000 001 \n"
        L"Ю4 11290 2 83 000 056.60 000 056 \n"
        L"Ю12 00 63134977 1 000 86230 16101 3126 000 00 00 00 00 00 0247 12 60 0600 04 100 18000 00000 11290 83 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        L"Ю12 00 53159349 203237255255 862801611230002405600\n"
        L"Ю12 00 63736656 255253255255 0600\n"
        L"Ю12 00 62099148 255239255255 0244\n"
        L"Ю12 00 62385752 255239255255 0243\n"
        L"Ю12 00 63527576 255239255255 0238\n"
        L"Ю12 00 61392346 255239255255 0240\n"
        L"Ю12 00 61392239 251255255255 400\n"
        L"Ю12 00 62607437 255239255255 0242\n"
        L"Ю12 00 63225429 219239255255 862305000247\n"
        L"Ю12 00 62774153 255239255255 0249\n"
        L"Ю12 00 62774005 255239255255 0250\n"
        L"Ю12 00 63137038 255239255255 0247\n"
        L"Ю12 00 62773213 255239255255 0250\n"
        L"Ю12 00 60182037 219239255255 862803000241\n"
        L"Ю12 00 61564787 255239255255 0243\n"
        L"Ю12 00 62774914 219239255255 862300000249\n"
        L"Ю12 00 63521843 219239255255 862804000237\n"
        L"Ю12 00 52011574 255237255255 02245608\n"
        L"Ю12 00 60865706 255237255255 02430600\n"
        L"Ю12 00 61409967 195239247255 78220161122006000023976\n"
        L"Ю12 00 60887064 195239247255 86280161123126500024383\n"
        L"Ю12 00 61262903 255239255255 0240\n"
        L"Ю12 00 61458972 255255255255 \n"
        L"Ю12 00 61410627 251255255255 300\n"
        L"Ю12 00 61172821 255239255255 0238\n"
        L"Ю12 00 62635446 255239255255 0241\n"
        L"Ю12 00 60127438 255239255255 0242\n"
        L"Ю12 00 54181813 255237255255 02445600\n"
        L"Ю12 00 62710546 251237255255 00002410600\n"
        L"Ю12 00 60158540 251239255255 4000244\n"
        L"Ю12 00 60105020 255255255255 \n"
        L"Ю12 00 60930732 255255255255 \n"
        L"Ю12 00 63119739 255239255255 0241\n"
        L"Ю12 00 63526958 255239255255 0237\n"
        L"Ю12 00 60029048 251239255255 5000242\n"
        L"Ю12 00 62023817 255239255255 0238\n"
        L"Ю12 00 64540180 255239255255 0242\n"
        L"Ю12 00 62027115 255239255255 0243\n"
        L"Ю12 00 63205736 255239255255 0242\n"
        L"Ю12 00 53767786 251237255255 30002335600\n"
        L"Ю12 00 62188503 255237255255 02450600\n"
        L"Ю12 00 55293567 255237255255 02365600\n"
        L"Ю12 00 63526743 255237255255 02370600\n"
        L"Ю12 00 60158201 255239255255 0240\n"
        L"Ю12 00 62416680 251239255255 4000243\n"
        L"Ю12 00 61971545 255239255255 0240\n"
        L"Ю12 00 61149803 255239255255 0244\n"
        L"Ю12 00 62121264 255239255255 0241\n"
        L"Ю12 00 55995294 255237255255 02335600\n"
        L"Ю12 00 63335673 251237255255 50002430600\n"
        L"Ю12 00 59082073 255237255255 02335600\n"
        L"Ю12 00 62712351 255237255255 02400600\n"
        L"Ю12 00 62303714 255239255255 0243\n"
        L"Ю12 00 55581847 255237255255 02355600\n"
        L"Ю12 00 62263751 219237255255 8623000002480600\n"
        L"Ю12 00 62747159 223239255255 862800244:)\n",
        boost::gregorian::date(2019,06,14)
        );
    al.AddAsoupEvent(asoupEventPtr);

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20190614T054027Z'>"
        L"<PathEvent name='Transition' Bdg='?p1w1[09190]' create_time='20190614T024213Z' waynum='1' parknum='1' />"
        L"<Action code='Identify' SaveLog='Y' District='09180-11420' DistrictRight='Y'>"
        L"<EventInfo layer='asoup' Bdg='ASOUP 1042[11420]' name='Arrival' create_time='20190614T031800Z' index='0982-024-1800' num='2442' length='57' weight='1376' through='Y' waynum='6' parknum='1'><![CDATA[(:1042 909/000+11420 2442 0982 024 1800 01 09180 14 06 06 18 01/06 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01376 00000 228 63134977 62747159 000 000 00 000 057.60 000 057 000\n"
        L"Ю3 530 00009661 1 01 57 0000 00000 MINJCHONOKS \n"
        L"Ю3 530 00009662 9\n"
        L"Ю4 11290 2 76 000 001.60 000 001 \n"
        L"Ю4 11290 2 83 000 056.60 000 056 \n"
        L"Ю12 00 63134977 1 000 86230 16101 3126 000 00 00 00 00 00 0247 12 60 0600 04 100 18000 00000 11290 83 09820 01 00000 0000 020 1 1020 09826  128 000 00000000\n"
        L"Ю12 00 53159349 203237255255 862801611230002405600\n"
        L"Ю12 00 63736656 255253255255 0600\n"
        L"Ю12 00 62099148 255239255255 0244\n"
        L"Ю12 00 62385752 255239255255 0243\n"
        L"Ю12 00 63527576 255239255255 0238\n"
        L"Ю12 00 61392346 255239255255 0240\n"
        L"Ю12 00 61392239 251255255255 400\n"
        L"Ю12 00 62607437 255239255255 0242\n"
        L"Ю12 00 63225429 219239255255 862305000247\n"
        L"Ю12 00 62774153 255239255255 0249\n"
        L"Ю12 00 62774005 255239255255 0250\n"
        L"Ю12 00 63137038 255239255255 0247\n"
        L"Ю12 00 62773213 255239255255 0250\n"
        L"Ю12 00 60182037 219239255255 862803000241\n"
        L"Ю12 00 61564787 255239255255 0243\n"
        L"Ю12 00 62774914 219239255255 862300000249\n"
        L"Ю12 00 63521843 219239255255 862804000237\n"
        L"Ю12 00 52011574 255237255255 02245608\n"
        L"Ю12 00 60865706 255237255255 02430600\n"
        L"Ю12 00 61409967 195239247255 78220161122006000023976\n"
        L"Ю12 00 60887064 195239247255 86280161123126500024383\n"
        L"Ю12 00 61262903 255239255255 0240\n"
        L"Ю12 00 61458972 255255255255 \n"
        L"Ю12 00 61410627 251255255255 300\n"
        L"Ю12 00 61172821 255239255255 0238\n"
        L"Ю12 00 62635446 255239255255 0241\n"
        L"Ю12 00 60127438 255239255255 0242\n"
        L"Ю12 00 54181813 255237255255 02445600\n"
        L"Ю12 00 62710546 251237255255 00002410600\n"
        L"Ю12 00 60158540 251239255255 4000244\n"
        L"Ю12 00 60105020 255255255255 \n"
        L"Ю12 00 60930732 255255255255 \n"
        L"Ю12 00 63119739 255239255255 0241\n"
        L"Ю12 00 63526958 255239255255 0237\n"
        L"Ю12 00 60029048 251239255255 5000242\n"
        L"Ю12 00 62023817 255239255255 0238\n"
        L"Ю12 00 64540180 255239255255 0242\n"
        L"Ю12 00 62027115 255239255255 0243\n"
        L"Ю12 00 63205736 255239255255 0242\n"
        L"Ю12 00 53767786 251237255255 30002335600\n"
        L"Ю12 00 62188503 255237255255 02450600\n"
        L"Ю12 00 55293567 255237255255 02365600\n"
        L"Ю12 00 63526743 255237255255 02370600\n"
        L"Ю12 00 60158201 255239255255 0240\n"
        L"Ю12 00 62416680 251239255255 4000243\n"
        L"Ю12 00 61971545 255239255255 0240\n"
        L"Ю12 00 61149803 255239255255 0244\n"
        L"Ю12 00 62121264 255239255255 0241\n"
        L"Ю12 00 55995294 255237255255 02335600\n"
        L"Ю12 00 63335673 251237255255 50002430600\n"
        L"Ю12 00 59082073 255237255255 02335600\n"
        L"Ю12 00 62712351 255237255255 02400600\n"
        L"Ю12 00 62303714 255239255255 0243\n"
        L"Ю12 00 55581847 255237255255 02355600\n"
        L"Ю12 00 62263751 219237255255 8623000002480600\n"
        L"Ю12 00 62747159 223239255255 862800244:)]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='966' Consec='1' CarrierCode='2'>"
        L"<Crew EngineDriver='MINJCHONOKS' Tim_Beg='2019-06-14 01:57' />"
        L"</Locomotive>"
        L"</EventInfo>"
        L"</Action>"
        L"</A2F_HINT>"
        );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    CPPUNIT_ASSERT( gis.front().first == tForm && gis.front().second && gis.front().second->GetNumber().getNumber()==2442 );
    CPPUNIT_ASSERT( gis.back().first == tArrival11760 && gis.back().second && gis.back().second->GetNumber().getNumber()==819 );

    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::UserIdentifyAndLinkFreeAsoup()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20191003T020854Z' name='Form' Bdg='48SP[11310]' />"
        L"<SpotEvent create_time='20191003T020854Z' name='Departure' Bdg='48SP[11310]' />"
        L"<SpotEvent create_time='20191003T021200Z' name='Transition' Bdg='701SP[11321]'>"
        L"<rwcoord picketing1_val='222~474' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191003T022558Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' intervalSec='8' optCode='11311:11380' />"
        L"<SpotEvent create_time='20191003T023822Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' intervalSec='2' optCode='11380:11381' />"
        L"<SpotEvent create_time='20191003T025203Z' name='Transition' Bdg='1C[11381]' waynum='1' parknum='1' intervalSec='1' optCode='11381:11390' />"
        L"<SpotEvent create_time='20191003T030042Z' name='Transition' Bdg='1C[11390]' waynum='1' parknum='1' intervalSec='4' optCode='11390:11391' />"
        L"<SpotEvent create_time='20191003T031843Z' name='Transition' Bdg='1C[11391]' waynum='1' parknum='1' intervalSec='1' optCode='11391:11401' />"
        L"<SpotEvent create_time='20191003T033103Z' name='Transition' Bdg='1C[11401]' waynum='1' parknum='1' intervalSec='9' optCode='11401:11412' />"
        L"<SpotEvent create_time='20191003T034437Z' name='Transition' Bdg='1C[11412]' waynum='1' parknum='1' intervalSec='15' optCode='11412:11420' />"
        L"<SpotEvent create_time='20191003T040916Z' name='Transition' Bdg='3C[11420]' waynum='3' parknum='1' intervalSec='16' index='' num='' noinfo='Y' />"
        L"<SpotEvent create_time='20191003T042351Z' name='Transition' Bdg='1C[09190]' waynum='1' parknum='1' intervalSec='14' optCode='09190:09191' />"
        L"<SpotEvent create_time='20191003T043332Z' name='Transition' Bdg='1C[09191]' waynum='1' parknum='1' intervalSec='8' optCode='09191:09193' />"
        L"<SpotEvent create_time='20191003T045614Z' name='Transition' Bdg='1C[09193]' waynum='1' parknum='1' intervalSec='2' optCode='09193:09200' />"
        L"<SpotEvent create_time='20191003T050340Z' name='Transition' Bdg='1C[09200]' waynum='1' parknum='1' intervalSec='3' optCode='09200:09202' />"
        L"<SpotEvent create_time='20191003T051318Z' name='Station_entry' Bdg='NDP[09202]'>"
        L"<rwcoord picketing1_val='252~' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    AsoupEventPtr adeparture11310Ptr = createAsoupEventPtr( 
        L"(:1042 909/000+11310 2411 1131 095 0987 03 11420 03 10 05 02 03/13 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04988 03756 216 59054213 95904249 000 000 00 054 000.70 001 000 000 90 053 000 000 92 009 000 000 95 044 000 000\n"
        L"Ю3 550 00002181 1 03 35 0000 00000 ANISIMOV    \n"
        L"Ю3 550 00002182 9\n"
        L"Ю4 00000 1 01 054 000.70 001 000 90 053 000 92 009 000 95 044 000 \n"
        L"Ю4 09870 1 01 045 000.70 001 000 90 044 000 95 044 000 \n"
        L"Ю4 09860 1 01 009 000.90 009 000 92 009 000 \n"
        L"Ю12 00 59054213 1 069 09860 43619 7533 400 02 00 00 00 00 0224 30 92 5904 04 095 09870 09860 00000 01 09870 17 00000 0000 020 0 0000 0      132 000 00000000\n"
        L"Ю12 00 59041871 255239255255 0220\n"
        L"Ю12 00 59251959 255239255255 0223\n"
        L"Ю12 00 58899774 255239255255 0219\n"
        L"Ю12 00 58653304 255239255255 0222\n"
        L"Ю12 00 59038885 255239255255 0219\n"
        L"Ю12 00 59037408 255239255255 0218\n"
        L"Ю12 00 53384012 189237127255 0700402265931106\n"
        L"Ю12 00 77201481 129233121245 066098705562215280000102757007740861131026000   064\n"
        L"Ю12 00 95872032 161233125181 0700110055525000602319509501063402511315Э000\n"
        L"Ю12 00 95004230 253239255255 020233\n"
        L"Ю12 00 95169330 255239255255 0231\n"
        L"Ю12 00 95872040 189255255255 06906\n"
        L"Ю12 00 95657573 249239255247 00004021511315 \n"
        L"Ю12 00 95846069 191239255255 0700237\n"
        L"Ю12 00 95806717 191239255255 0680220\n"
        L"Ю12 00 95700035 191255255255 069\n"
        L"Ю12 00 95872354 189239255255 070060231\n"
        L"Ю12 00 95459871 189239255255 069040215\n"
        L"Ю12 00 95716635 191239255255 0670220\n"
        L"Ю12 00 95698833 191239255255 0700235\n"
        L"Ю12 00 95716759 191239255255 0680220\n"
        L"Ю12 00 95032140 191255255255 065\n"
        L"Ю12 00 95998951 189239255255 070020234\n"
        L"Ю12 00 95573945 253239255255 040220\n"
        L"Ю12 00 95038576 191255255255 065\n"
        L"Ю12 00 95716247 191255255255 068\n"
        L"Ю12 00 95085437 255255255255 \n"
        L"Ю12 00 95335089 189239255255 070020232\n"
        L"Ю12 00 95685228 189239255255 068040220\n"
        L"Ю12 00 95032124 191255255255 065\n"
        L"Ю12 00 95698643 191239255255 0700235\n"
        L"Ю12 00 95181210 191239255255 0680220\n"
        L"Ю12 00 95927620 235239125183 542073000234102380200     \n"
        L"Ю12 00 95891875 189239127255 067020229106\n"
        L"Ю12 00 95812269 191255255255 070\n"
        L"Ю12 00 95916060 191239255255 0690230\n"
        L"Ю12 00 95854121 191239255255 0700229\n"
        L"Ю12 00 95896023 191255255255 069\n"
        L"Ю12 00 95898664 255239255255 0230\n"
        L"Ю12 00 95190625 191239255255 0700233\n"
        L"Ю12 00 95692000 249239255183 00004023602503-10 \n"
        L"Ю12 00 53371357 199233249183 098604330284980230925931098701702063728-\n"
        L"Ю12 00 95369153 129233249247 0710987054207555250002023595095011310380     \n"
        L"Ю12 00 95233086 191239255247 067023113570 \n"
        L"Ю12 00 95376067 191239255247 06802350     \n"
        L"Ю12 00 95163408 189239127255 063030237104\n"
        L"Ю12 00 95809224 189239127255 075020232106\n"
        L"Ю12 00 95818704 255255255255 \n"
        L"Ю12 00 95430559 191255255255 076\n"
        L"Ю12 00 95903738 255255255255 \n"
        L"Ю12 00 95904108 255255255255 \n"
        L"Ю12 00 95904157 255255255255 \n"
        L"Ю12 00 95904249 255255255255 :)",
        boost::gregorian::date(2019,10,03)
        );
    al.AddAsoupEvent(adeparture11310Ptr);

    AsoupEventPtr atransition11420Ptr = createAsoupEventPtr( 
        L"(:1042 909/000+11420 2411 1131 095 0987 04 11310+09180 03 10 07 08 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04988 03756 216 59054213 95904249 000 000 00 054 000.70 001 000 000 90 053 000 000 92 009 000 000 95 044 000 000\n"
        L"Ю3 550 00002181 1 03 35 0000 00000 ANISIMOV    \n"
        L"Ю3 550 00002182 9\n"
        L"Ю4 00000 1 01 054 000.70 001 000 90 053 000 92 009 000 95 044 000 \n"
        L"Ю4 09870 1 01 045 000.70 001 000 90 044 000 95 044 000 \n"
        L"Ю4 09860 1 01 009 000.90 009 000 92 009 000 \n"
        L"Ю12 00 59054213 1 069 09860 43619 7533 400 02 00 00 00 00 0224 30 92 5904 04 095 09870 09860 00000 01 09870 17 00000 0000 020 0 0000 0      132 000 00000000\n"
        L"Ю12 00 59041871 255239255255 0220\n"
        L"Ю12 00 59251959 255239255255 0223\n"
        L"Ю12 00 58899774 255239255255 0219\n"
        L"Ю12 00 58653304 255239255255 0222\n"
        L"Ю12 00 59038885 255239255255 0219\n"
        L"Ю12 00 59037408 255239255255 0218\n"
        L"Ю12 00 53384012 189237127255 0700402265931106\n"
        L"Ю12 00 77201481 129233121245 066098705562215280000102757007740861131026000   064\n"
        L"Ю12 00 95872032 161233125181 0700110055525000602319509501063402511315Э000\n"
        L"Ю12 00 95004230 253239255255 020233\n"
        L"Ю12 00 95169330 255239255255 0231\n"
        L"Ю12 00 95872040 189255255255 06906\n"
        L"Ю12 00 95657573 249239255247 00004021511315 \n"
        L"Ю12 00 95846069 191239255255 0700237\n"
        L"Ю12 00 95806717 191239255255 0680220\n"
        L"Ю12 00 95700035 191255255255 069\n"
        L"Ю12 00 95872354 189239255255 070060231\n"
        L"Ю12 00 95459871 189239255255 069040215\n"
        L"Ю12 00 95716635 191239255255 0670220\n"
        L"Ю12 00 95698833 191239255255 0700235\n"
        L"Ю12 00 95716759 191239255255 0680220\n"
        L"Ю12 00 95032140 191255255255 065\n"
        L"Ю12 00 95998951 189239255255 070020234\n"
        L"Ю12 00 95573945 253239255255 040220\n"
        L"Ю12 00 95038576 191255255255 065\n"
        L"Ю12 00 95716247 191255255255 068\n"
        L"Ю12 00 95085437 255255255255 \n"
        L"Ю12 00 95335089 189239255255 070020232\n"
        L"Ю12 00 95685228 189239255255 068040220\n"
        L"Ю12 00 95032124 191255255255 065\n"
        L"Ю12 00 95698643 191239255255 0700235\n"
        L"Ю12 00 95181210 191239255255 0680220\n"
        L"Ю12 00 95927620 235239125183 542073000234102380200     \n"
        L"Ю12 00 95891875 189239127255 067020229106\n"
        L"Ю12 00 95812269 191255255255 070\n"
        L"Ю12 00 95916060 191239255255 0690230\n"
        L"Ю12 00 95854121 191239255255 0700229\n"
        L"Ю12 00 95896023 191255255255 069\n"
        L"Ю12 00 95898664 255239255255 0230\n"
        L"Ю12 00 95190625 191239255255 0700233\n"
        L"Ю12 00 95692000 249239255183 00004023602503-10 \n"
        L"Ю12 00 53371357 199233249183 098604330284980230925931098701702063728-\n"
        L"Ю12 00 95369153 129233249247 0710987054207555250002023595095011310380     \n"
        L"Ю12 00 95233086 191239255247 067023113570 \n"
        L"Ю12 00 95376067 191239255247 06802350     \n"
        L"Ю12 00 95163408 189239127255 063030237104\n"
        L"Ю12 00 95809224 189239127255 075020232106\n"
        L"Ю12 00 95818704 255255255255 \n"
        L"Ю12 00 95430559 191255255255 076\n"
        L"Ю12 00 95903738 255255255255 \n"
        L"Ю12 00 95904108 255255255255 \n"
        L"Ю12 00 95904157 255255255255 \n"
        L"Ю12 00 95904249 255255255255 :)",
        boost::gregorian::date(2019,10,03)
        );
    al.AddAsoupEvent(atransition11420Ptr);

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20191003T051318Z'>"
        L"<PathEvent name='Transition' Bdg='3C[11420]' create_time='20191003T040916Z' index='' num='' noinfo='Y' waynum='3' parknum='1' intervalSec='16' />"
        L"<Action code='Identify' SaveLog='Y' District='11420-11310-11260-11290' DistrictRight='Y'>"
        L"<EventInfo layer='asoup' Bdg='ASOUP 1042[11420]' name='Transition' create_time='20191003T040800Z' index='1131-095-0987' num='2411' length='57' weight='4988' net_weight='3756' through='Y'><![CDATA[(:1042 909/000+11420 2411 1131 095 0987 04 11310+09180 03 10 07 08 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04988 03756 216 59054213 95904249 000 000 00 054 000.70 001 000 000 90 053 000 000 92 009 000 000 95 044 000 000\n"
        L"Ю3 550 00002181 1 03 35 0000 00000 ANISIMOV    \n"
        L"Ю3 550 00002182 9\n"
        L"Ю4 00000 1 01 054 000.70 001 000 90 053 000 92 009 000 95 044 000 \n"
        L"Ю4 09870 1 01 045 000.70 001 000 90 044 000 95 044 000 \n"
        L"Ю4 09860 1 01 009 000.90 009 000 92 009 000 \n"
        L"Ю12 00 59054213 1 069 09860 43619 7533 400 02 00 00 00 00 0224 30 92 5904 04 095 09870 09860 00000 01 09870 17 00000 0000 020 0 0000 0      132 000 00000000\n"
        L"Ю12 00 59041871 255239255255 0220\n"
        L"Ю12 00 59251959 255239255255 0223\n"
        L"Ю12 00 58899774 255239255255 0219\n"
        L"Ю12 00 58653304 255239255255 0222\n"
        L"Ю12 00 59038885 255239255255 0219\n"
        L"Ю12 00 59037408 255239255255 0218\n"
        L"Ю12 00 53384012 189237127255 0700402265931106\n"
        L"Ю12 00 77201481 129233121245 066098705562215280000102757007740861131026000   064\n"
        L"Ю12 00 95872032 161233125181 0700110055525000602319509501063402511315Э000\n"
        L"Ю12 00 95004230 253239255255 020233\n"
        L"Ю12 00 95169330 255239255255 0231\n"
        L"Ю12 00 95872040 189255255255 06906\n"
        L"Ю12 00 95657573 249239255247 00004021511315 \n"
        L"Ю12 00 95846069 191239255255 0700237\n"
        L"Ю12 00 95806717 191239255255 0680220\n"
        L"Ю12 00 95700035 191255255255 069\n"
        L"Ю12 00 95872354 189239255255 070060231\n"
        L"Ю12 00 95459871 189239255255 069040215\n"
        L"Ю12 00 95716635 191239255255 0670220\n"
        L"Ю12 00 95698833 191239255255 0700235\n"
        L"Ю12 00 95716759 191239255255 0680220\n"
        L"Ю12 00 95032140 191255255255 065\n"
        L"Ю12 00 95998951 189239255255 070020234\n"
        L"Ю12 00 95573945 253239255255 040220\n"
        L"Ю12 00 95038576 191255255255 065\n"
        L"Ю12 00 95716247 191255255255 068\n"
        L"Ю12 00 95085437 255255255255 \n"
        L"Ю12 00 95335089 189239255255 070020232\n"
        L"Ю12 00 95685228 189239255255 068040220\n"
        L"Ю12 00 95032124 191255255255 065\n"
        L"Ю12 00 95698643 191239255255 0700235\n"
        L"Ю12 00 95181210 191239255255 0680220\n"
        L"Ю12 00 95927620 235239125183 542073000234102380200     \n"
        L"Ю12 00 95891875 189239127255 067020229106\n"
        L"Ю12 00 95812269 191255255255 070\n"
        L"Ю12 00 95916060 191239255255 0690230\n"
        L"Ю12 00 95854121 191239255255 0700229\n"
        L"Ю12 00 95896023 191255255255 069\n"
        L"Ю12 00 95898664 255239255255 0230\n"
        L"Ю12 00 95190625 191239255255 0700233\n"
        L"Ю12 00 95692000 249239255183 00004023602503-10 \n"
        L"Ю12 00 53371357 199233249183 098604330284980230925931098701702063728-\n"
        L"Ю12 00 95369153 129233249247 0710987054207555250002023595095011310380     \n"
        L"Ю12 00 95233086 191239255247 067023113570 \n"
        L"Ю12 00 95376067 191239255247 06802350     \n"
        L"Ю12 00 95163408 189239127255 063030237104\n"
        L"Ю12 00 95809224 189239127255 075020232106\n"
        L"Ю12 00 95818704 255255255255 \n"
        L"Ю12 00 95430559 191255255255 076\n"
        L"Ю12 00 95903738 255255255255 \n"
        L"Ю12 00 95904108 255255255255 \n"
        L"Ю12 00 95904157 255255255255 \n"
        L"Ю12 00 95904249 255255255255 :)]]>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2ТЭ10У' NumLoc='218' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ANISIMOV' Tim_Beg='2019-10-03 03:35' />"
        L"</Locomotive>"
        L"</EventInfo>"
        L"</Action>"
        L"</A2F_HINT>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",  "Form  Departure Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition Station_entry" ) );
    CPPUNIT_ASSERT( hl.exist_series( "num",   "-        -          -         -            -         -          -          -          -          -          -          -          -          -          -          -        " ) );
    CPPUNIT_ASSERT( hl.exist_series( "index", "-        -          -         -            -         -          -          -          -          -          -          -          -          -          -          -        " ) );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",  "Form  Departure Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition Transition Station_entry" ) );
    CPPUNIT_ASSERT( hl.exist_series( "num",   "2411       -          -         -            -         -          -          -          -          -          -          -          -          -          -          -         " ) );
    CPPUNIT_ASSERT( hl.exist_series( "index", "1131-095-0987 -       -         -            -         -          -          -          -          -          -          -          -          -          -          -         " ) );
    CPPUNIT_ASSERT( al.IsServed(atransition11420Ptr) );
    CPPUNIT_ASSERT( al.IsServed(adeparture11310Ptr) );
    set_TimeZone("");
}


void TC_Hem_aeUserIdentify::UserIdentifyWithChangeOnlyNotes()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
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
        L"<SpotEvent create_time='20191204T110507Z' name='Transition' Bdg='1C[09180]' />"
        L"<SpotEvent create_time='20191204T113239Z' name='Transition' Bdg='?p1w1[09764]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20191204T114337Z' name='Transition' Bdg='?p1w1[09772]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20191204T115337Z' name='Transition' Bdg='?p1w3[09750]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20191204T115936Z' name='Transition' Bdg='?p1w1[09751]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20191204T120721Z' name='Transition' Bdg='?p1w1[09780]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20191204T121405Z' name='Transition' Bdg='?p1w2[09790]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191204T121820Z' name='Transition' Bdg='?p1w2[09800]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20191204T122300Z' name='Arrival' Bdg='?p1w1[09801]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20191204T122300Z' name='Death' Bdg='?p1w1[09801]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    time_t tTransition9180 = (from_iso_string("20191204T110507Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"1C", EsrKit(9180)), tTransition9180);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );

    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );

    Hem::Bill bill = createBill(
        L"<Changing>\n"
              L"<PrePoint name='Transition' cover='Y' Bdg='1C[09180]' create_time='20191204T110507Z' />"
              L"<PostPoint name='Death' cover='Y' Bdg='?p1w1[09801]' create_time='20191204T122300Z' waynum='1' parknum='1' />"
              L"<Action code='Identify' District='09180-09820' DistrictRight='Y'>"
                L"<TrainInfo index='0001-031-0918' num='4901' joincrgloc='Y'>"
                  L"<Locomotive Series='2ТЭ116' NumLoc='1259' Consec='1' CarrierCode='2'>"
                    L"<Crew EngineDriver='RANCANS' Tim_Beg='2019-12-04 06:00' PersonMode='1' />"
                  L"</Locomotive>"
                  L"<Locomotive Series='2ТЭ116' NumLoc='973' Consec='4' CarrierCode='2'>"
                    L"<Crew EngineDriver='PETROVS' Tim_Beg='2019-12-04 04:23' />"
                  L"</Locomotive>"
                L"</TrainInfo>"
              L"</Action>"
        L"</Changing>"
        );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever,uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::UserGetPossibleIdentification()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200119T135700Z' name='Span_move' Bdg='?[09220:09230]'>"
        L"<rwcoord picketing1_val='213~458' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200119T140100Z' name='Transition' Bdg='1C[09220]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200119T140500Z' name='Death' Bdg='1C[09220]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    time_t tEvent9220 = (from_iso_string("20200119T140100Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"1C", EsrKit(9220)), tEvent9220);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );

    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 0 );

    {
    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[20200118T160000Z/20200120T155959Z)'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20200119T072700Z' name='Info_changing' Bdg='№2418[09850]' index='' num='2418' through='Y' />"
        L"<SpotEvent create_time='20200119T072700Z' name='Departure' Bdg='№2418[09850]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T073200Z' name='Transition' Bdg='№2418[09859]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T073600Z' name='Transition' Bdg='№2418[09840]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T074100Z' name='Transition' Bdg='№2418[09813]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T075000Z' name='Transition' Bdg='№2418[09812]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T080300Z' name='Transition' Bdg='№2418[09810]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T081200Z' name='Transition' Bdg='№2418[09803]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T082200Z' name='Arrival' Bdg='№2418[09802]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T083500Z' name='Departure' Bdg='№2418[09802]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T084800Z' name='Transition' Bdg='№2418[09801]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T085700Z' name='Transition' Bdg='№2418[09800]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T090700Z' name='Arrival' Bdg='№2418[09790]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T092100Z' name='Departure' Bdg='№2418[09790]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T093300Z' name='Transition' Bdg='№2418[09780]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T094800Z' name='Arrival' Bdg='№2418[09751]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T100200Z' name='Departure' Bdg='№2418[09751]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T101400Z' name='Transition' Bdg='№2418[09750]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T103100Z' name='Arrival' Bdg='№2418[09772]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T105200Z' name='Departure' Bdg='№2418[09772]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T111100Z' name='Arrival' Bdg='№2418[09764]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T113800Z' name='Departure' Bdg='№2418[09764]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T120400Z' name='Arrival' Bdg='№2418[09180]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T130200Z' name='Departure' Bdg='№2418[09180]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T131200Z' name='Arrival' Bdg='№2418[09181]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T131700Z' name='Departure' Bdg='№2418[09181]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T133400Z' name='Transition' Bdg='№2418[09251]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T134200Z' name='Transition' Bdg='№2418[09241]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T135000Z' name='Transition' Bdg='№2418[09240]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T135900Z' name='Transition' Bdg='№2418[09230]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T140800Z' name='Transition' Bdg='№2418[09220]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T142200Z' name='Transition' Bdg='№2418[09211]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T143200Z' name='Arrival' Bdg='№2418[09210]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T144800Z' name='Departure' Bdg='№2418[09210]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T145900Z' name='Transition' Bdg='№2418[09202]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T150700Z' name='Transition' Bdg='№2418[09200]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T151600Z' name='Arrival' Bdg='№2418[09193]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T152100Z' name='Departure' Bdg='№2418[09193]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T154300Z' name='Arrival' Bdg='№2418[09191]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T155000Z' name='Departure' Bdg='№2418[09191]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T155800Z' name='Transition' Bdg='№2418[09190]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T161000Z' name='Arrival' Bdg='№2418[11420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T162000Z' name='Departure' Bdg='№2418[11420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T163300Z' name='Transition' Bdg='№2418[11419]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T164300Z' name='Arrival' Bdg='№2418[11422]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T165200Z' name='Departure' Bdg='№2418[11422]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T170800Z' name='Arrival' Bdg='№2418[11430]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T171300Z' name='Departure' Bdg='№2418[11430]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T172900Z' name='Arrival' Bdg='№2418[11431]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T173900Z' name='Departure' Bdg='№2418[11431]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T175500Z' name='Arrival' Bdg='№2418[11432]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T180000Z' name='Departure' Bdg='№2418[11432]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T181300Z' name='Arrival' Bdg='№2418[11446]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T183500Z' name='Departure' Bdg='№2418[11446]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T184900Z' name='Transition' Bdg='№2418[11442]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T185600Z' name='Transition' Bdg='№2418[11443]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T190500Z' name='Transition' Bdg='№2418[11445]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T191300Z' name='Transition' Bdg='№2418[11451]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T191620Z' name='Transition' Bdg='№2418[11002]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200119T192300Z' name='Arrival' Bdg='№2418[11000]' waynum='2' parknum='1' />"
        L"</ScheduledPath>"
        L"</ScheduledLayer>"
        L"</export>\n";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(layerData));
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(attic::a_node());
    }


    std::wstring cmdStr = 
        L"<A2F_HINT Hint='cmd_demand'>"
        L"<Action code='GetPossibleIdentification' District='09180-11420' DistrictRight='Y' />"
        L"<PrePoint name='Transition' Bdg='1C[09220]' create_time='20200119T140100Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>";

    attic::a_document doc;
    CPPUNIT_ASSERT(doc.load_wide(cmdStr));

    Hem::Demand demand(doc.document_element());

    Hem::aeGetPossibleIdentification engine(hl.base().GetReadAccess(), al.GetReadAccess(), *m_regulatoryLayer, 
		demand.GetRelatedEventNode(), demand.GetRights(), nullptr, true );
    engine.Action();

    CPPUNIT_ASSERT( engine.GetTrainsCountForIdentify().second == 1  );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::UserIdentifyByLinkedAsoup_6057()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtUserChart userChart;
    auto& pure_happen = userChart.getHappenLayer();
    UtLayer<HappenLayer>& hl = reinterpret_cast< UtLayer<HappenLayer>& >(pure_happen);
    auto& pure_asoup = userChart.getAsoupLayer();
    UtLayer<AsoupLayer>& al = reinterpret_cast< UtLayer<AsoupLayer>& >(pure_asoup);
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20201203T202831Z' name='Form' Bdg='8JC[09006]' waynum='8' parknum='1' index='0900-822-1400' num='1464' length='57' weight='2635' net_weight='1350' drirr='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='10' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-KUZNECOVS' Tim_Beg='2020-12-03 21:03' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201203T204711Z' name='Departure' Bdg='8JC[09006]' waynum='8' parknum='1' />"
        L"<SpotEvent create_time='20201203T205220Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20201203T205521Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' optCode='09000:09420' />"
        L"<SpotEvent create_time='20201203T210852Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20201203T212646Z' name='Transition' Bdg='1C[09410]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20201203T214716Z' name='Transition' Bdg='2C[09400]' waynum='2' parknum='1' optCode='09380:09400' />"
        L"<SpotEvent create_time='20201203T221058Z' name='Transition' Bdg='2C[09380]' waynum='2' parknum='1' optCode='09370:09380' />"
        L"<SpotEvent create_time='20201203T222149Z' name='Transition' Bdg='2C[09370]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20201203T223213Z' name='Transition' Bdg='2C[09360]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20201203T224931Z' name='Transition' Bdg='2AC[11760]' waynum='2' parknum='2' />"
        L"<SpotEvent create_time='20201203T224958Z' name='Transition' Bdg='2C[11760]' waynum='2' parknum='1' optCode='11420:11760' />"
        L"<SpotEvent create_time='20201203T231053Z' name='Arrival' Bdg='3C[11420]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20201203T231948Z' name='Departure' Bdg='3C[11420]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20201203T233415Z' name='Transition' Bdg='1C[11419]' waynum='1' parknum='1' optCode='11419:11422' />"
        L"<SpotEvent create_time='20201203T234201Z' name='Transition' Bdg='1C[11422]' waynum='1' parknum='1' optCode='11422:11430' />"
        L"<SpotEvent create_time='20201203T235659Z' name='Transition' Bdg='1C[11430]' waynum='1' parknum='1' optCode='11430:11431' />"
        L"<SpotEvent create_time='20201204T000821Z' name='Transition' Bdg='1C[11431]' waynum='1' parknum='1' optCode='11431:11432' />"
        L"<SpotEvent create_time='20201204T000853Z' name='Transition' Bdg='1AC[11431]' waynum='1' parknum='2' optCode='11431:11432' />"
        L"<SpotEvent create_time='20201204T001755Z' name='Transition' Bdg='1C[11432]' waynum='1' parknum='1' optCode='11432:11446' />"
        L"<SpotEvent create_time='20201204T003011Z' name='Arrival' Bdg='3C[11446]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20201204T003151Z' name='Departure' Bdg='3C[11446]' waynum='3' parknum='1' optCode='11442:11446' />"
        L"<SpotEvent create_time='20201204T004610Z' name='Transition' Bdg='1BC[11442]' waynum='1' parknum='1' optCode='11442:11443' />"
        L"<SpotEvent create_time='20201204T005238Z' name='Transition' Bdg='1C[11443]' waynum='1' parknum='1' optCode='11443:11445' />"
        L"<SpotEvent create_time='20201204T005934Z' name='Transition' Bdg='5SP:5-[11445]'>"
        L"<rwcoord picketing1_val='383~39' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201204T010711Z' name='Span_stopping_begin' Bdg='MP2P[11445:11451]' waynum='2'>"
        L"<rwcoord picketing1_val='387~' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='387~395' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201204T011739Z' name='Span_stopping_end' Bdg='MP2P[11445:11451]' waynum='2'>"
        L"<rwcoord picketing1_val='387~' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='387~395' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201204T011837Z' name='Transition' Bdg='10SP[11451]'>"
        L"<rwcoord picketing1_val='387~840' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201204T012300Z' name='Transition' Bdg='?[11002]' />"
        L"<SpotEvent create_time='20201204T013000Z' name='Arrival' Bdg='ASOUP 1042_1p[11001]' parknum='1' />"
        L"<SpotEvent create_time='20201204T043000Z' name='Departure' Bdg='ASOUP 1042_1p[11001]' parknum='1' index='0900-822-1400' num='1464' length='57' weight='2635' net_weight='1350' drirr='Y'>"
        L"<Locomotive Series='2ТЭ10МК' NumLoc='3371' Depo='1317' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ШИTOB' TabNum='3111' Tim_Beg='2020-12-04 04:50' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201204T043000Z' name='Death' Bdg='ASOUP 1042_1p[11001]' parknum='1' />"
        L"</HemPath>"
        );

    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20201204T043531Z' name='Form' Bdg='N1P[11003:11070]' waynum='1'>"
        L"<rwcoord picketing1_val='390~800' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='391~220' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201204T044706Z' name='Transition' Bdg='1C[11070]' waynum='1' parknum='1' optCode='11063:11070' />"
        L"<SpotEvent create_time='20201204T044846Z' name='Transition' Bdg='1SP[11063]'>"
        L"<rwcoord picketing1_val='401~80' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201204T044846Z' name='Span_move' Bdg='NB1C[11062:11063]' waynum='1'>"
        L"<rwcoord picketing1_val='402~200' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='401~80' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

 al.createPath(
     L"<AsoupEvent create_time='20201204T043000Z' name='Departure' Bdg='ASOUP 1042_1p[11001]' index='0900-822-1400' num='1464' length='57' weight='2635' net_weight='1350' drirr='Y' parknum='1' dirTo='16169' adjTo='11003' linkBadge='ASOUP 1042_1p[11001]' linkTime='20201204T043000Z' linkCode='Departure'><![CDATA[(:1042 909/000+11000 1464 0900 822 1400 03 16169 04 12 06 30 02/06 0 0/00 00 0\n"
     L"Ю2 0 00 00 00 00 0000 0 0 057 02635 01350 220 95876512 95846465 000 000 00 019 036.20 000 001 000 60 000 027 000 90 019 008 000 92 000 004 000 95 019 004 000\n"
     L"Ю3 572 00033711 1 04 50 1317 03111 ШИTOB       \n"
     L"Ю3 572 00033712 9\n"
     L"Ю4 00000 0 00 000 035.60 000 027 90 000 008 92 000 004 95 000 004 \n"
     L"Ю4 16169 2 13 019 001.20 000 001 90 019 000 95 019 000 \n"
     L"Ю12 00 95876512 1 070 15490 43107 3942 300 02 00 00 00 00 0232 20 95 0950 04 106 14000 00000 16169 13 09000 17 09740 8517 021 1 1020 09740  132 000 00000000\n"
     L"Ю12 00 95983920 255255255255 \n"
     L"Ю12 00 95993085 255239255255 0233\n"
     L"Ю12 00 95992822 255239255255 0232\n"
     L"Ю12 00 59941864 255237255255 02345935\n"
     L"Ю12 00 95896460 255237255255 02320950\n"
     L"Ю12 00 95992533 255255255255 \n"
     L"Ю12 00 95988887 255255255255 \n"
     L"Ю12 00 95896338 255239255255 0233\n"
     L"Ю12 00 95993242 255255255255 \n"
     L"Ю12 00 59886143 255237255255 02365935\n"
     L"Ю12 00 95993010 255237255255 02330950\n"
     L"Ю12 00 59885681 255237255255 02375935\n"
     L"Ю12 00 95988275 255237255255 02330950\n"
     L"Ю12 00 95489639 191239255191 0760238020\n"
     L"Ю12 00 95520128 255255255255 \n"
     L"Ю12 00 95514212 255255255255 \n"
     L"Ю12 00 59946772 189237255255 0710402235935\n"
     L"Ю12 00 55000848 255239255255 0224\n"
     L"Ю12 00 62665062 129225069011 000000000911146940000002401160060010000600000000024469402100000128\n"
     L"Ю12 00 60668167 231239254119 1030799440244098709944098704\n"
     L"Ю12 00 66312711 255239255255 0230\n"
     L"Ю12 00 63745608 255239255255 0238\n"
     L"Ю12 00 64487424 255239255255 0240\n"
     L"Ю12 00 63711303 255239255255 0232\n"
     L"Ю12 00 63647432 255239255255 0238\n"
     L"Ю12 00 65362279 255239255255 0234\n"
     L"Ю12 00 65266835 255239255255 0225\n"
     L"Ю12 00 60201571 255239255255 0234\n"
     L"Ю12 00 61538625 255255255255 \n"
     L"Ю12 00 62868450 255239255255 0233\n"
     L"Ю12 00 62133723 255239255255 0234\n"
     L"Ю12 00 64255185 255239255255 0239\n"
     L"Ю12 00 64067986 255255255255 \n"
     L"Ю12 00 66234188 255239255255 0234\n"
     L"Ю12 00 63898621 255239255255 0238\n"
     L"Ю12 00 63899033 255239255255 0239\n"
     L"Ю12 00 28081289 199225070007 130200940715170259122002081220000016169130968059260201102009108 \n"
     L"Ю12 00 63745681 199225070007 000001030799440240116006001000060000000000987099440210000009874 \n"
     L"Ю12 00 61801502 255239255255 0232\n"
     L"Ю12 00 61109922 255239255255 0231\n"
     L"Ю12 00 64470313 255239255255 0242\n"
     L"Ю12 00 64469885 255239255255 0235\n"
     L"Ю12 00 63637375 255239255255 0236\n"
     L"Ю12 00 64420029 255239255255 0240\n"
     L"Ю12 00 64255904 255255255255 \n"
     L"Ю12 00 63898225 255239255255 0238\n"
     L"Ю12 00 90210097 231233095079 103047610022592090110600920761011020\n"
     L"Ю12 00 95708111 255233223255 022695095000950\n"
     L"Ю12 00 90297177 255233223255 022792090100920\n"
     L"Ю12 00 90204991 255239255255 0217\n"
     L"Ю12 00 90214297 255239255255 0225\n"
     L"Ю12 00 95846622 255233223255 022695095000950\n"
     L"Ю12 00 95456562 255239255255 0214\n"
     L"Ю12 00 95846465 255239255255 0226:)]]><Locomotive Series='2ТЭ10МК' NumLoc='3371' Depo='1317' Consec='1' CarrierCode='4'>"
     L"<Crew EngineDriver='ШИTOB' TabNum='3111' Tim_Beg='2020-12-04 04:50' />"
     L"</Locomotive>"
     L"</AsoupEvent>"
     );
    

     Hem::Bill bill = createBill(
     L"<A2F_HINT Hint='cmd_edit' parentPath='Y' issue_moment='20201204T044934Z'>"
     L"<PathEvent name='Form' Bdg='N1P[11003:11070]' create_time='20201204T043531Z' waynum='1'>"
     L"<rwcoord picketing1_val='391~10' picketing1_comm='Вентспилс' />"
     L"</PathEvent>"
     L"<Action code='Identify' SaveLog='Y' District='11000-11042' DistrictRight='Y'>"
     L"<EventInfo layer='asoup' Bdg='ASOUP 1042_1p[11001]' name='Departure' create_time='20201204T043000Z' index='0900-822-1400' num='1464' length='57' weight='2635' net_weight='1350' drirr='Y' parknum='1'>"
     L"<![CDATA[(:1042 909/000+11000 1464 0900 822 1400 03 16169 04 12 06 30 02/06 0 0/00 00 0\n"
     L"Ю2 0 00 00 00 00 0000 0 0 057 02635 01350 220 95876512 95846465 000 000 00 019 036.20 000 001 000 60 000 027 000 90 019 008 000 92 000 004 000 95 019 004 000\n"
     L"Ю3 572 00033711 1 04 50 1317 03111 ШИTOB       \n"
     L"Ю3 572 00033712 9\n"
     L"Ю4 00000 0 00 000 035.60 000 027 90 000 008 92 000 004 95 000 004 \n"
     L"Ю4 16169 2 13 019 001.20 000 001 90 019 000 95 019 000 \n"
     L"Ю12 00 95876512 1 070 15490 43107 3942 300 02 00 00 00 00 0232 20 95 0950 04 106 14000 00000 16169 13 09000 17 09740 8517 021 1 1020 09740  132 000 00000000\n"
     L"Ю12 00 95983920 255255255255 \n"
     L"Ю12 00 95993085 255239255255 0233\n"
     L"Ю12 00 95992822 255239255255 0232\n"
     L"Ю12 00 59941864 255237255255 02345935\n"
     L"Ю12 00 95896460 255237255255 02320950\n"
     L"Ю12 00 95992533 255255255255 \n"
     L"Ю12 00 95988887 255255255255 \n"
     L"Ю12 00 95896338 255239255255 0233\n"
     L"Ю12 00 95993242 255255255255 \n"
     L"Ю12 00 59886143 255237255255 02365935\n"
     L"Ю12 00 95993010 255237255255 02330950\n"
     L"Ю12 00 59885681 255237255255 02375935\n"
     L"Ю12 00 95988275 255237255255 02330950\n"
     L"Ю12 00 95489639 191239255191 0760238020\n"
     L"Ю12 00 95520128 255255255255 \n"
     L"Ю12 00 95514212 255255255255 \n"
     L"Ю12 00 59946772 189237255255 0710402235935\n"
     L"Ю12 00 55000848 255239255255 0224\n"
     L"Ю12 00 62665062 129225069011 000000000911146940000002401160060010000600000000024469402100000128\n"
     L"Ю12 00 60668167 231239254119 1030799440244098709944098704\n"
     L"Ю12 00 66312711 255239255255 0230\n"
     L"Ю12 00 63745608 255239255255 0238\n"
     L"Ю12 00 64487424 255239255255 0240\n"
     L"Ю12 00 63711303 255239255255 0232\n"
     L"Ю12 00 63647432 255239255255 0238\n"
     L"Ю12 00 65362279 255239255255 0234\n"
     L"Ю12 00 65266835 255239255255 0225\n"
     L"Ю12 00 60201571 255239255255 0234\n"
     L"Ю12 00 61538625 255255255255 \n"
     L"Ю12 00 62868450 255239255255 0233\n"
     L"Ю12 00 62133723 255239255255 0234\n"
     L"Ю12 00 64255185 255239255255 0239\n"
     L"Ю12 00 64067986 255255255255 \n"
     L"Ю12 00 66234188 255239255255 0234\n"
     L"Ю12 00 63898621 255239255255 0238\n"
     L"Ю12 00 63899033 255239255255 0239\n"
     L"Ю12 00 28081289 199225070007 130200940715170259122002081220000016169130968059260201102009108 \n"
     L"Ю12 00 63745681 199225070007 000001030799440240116006001000060000000000987099440210000009874 \n"
     L"Ю12 00 61801502 255239255255 0232\n"
     L"Ю12 00 61109922 255239255255 0231\n"
     L"Ю12 00 64470313 255239255255 0242\n"
     L"Ю12 00 64469885 255239255255 0235\n"
     L"Ю12 00 63637375 255239255255 0236\n"
     L"Ю12 00 64420029 255239255255 0240\n"
     L"Ю12 00 64255904 255255255255 \n"
     L"Ю12 00 63898225 255239255255 0238\n"
     L"Ю12 00 90210097 231233095079 103047610022592090110600920761011020\n"
     L"Ю12 00 95708111 255233223255 022695095000950\n"
     L"Ю12 00 90297177 255233223255 022792090100920\n"
     L"Ю12 00 90204991 255239255255 0217\n"
     L"Ю12 00 90214297 255239255255 0225\n"
     L"Ю12 00 95846622 255233223255 022695095000950\n"
     L"Ю12 00 95456562 255239255255 0214\n"
     L"Ю12 00 95846465 255239255255 0226:)]]>"
     L"<Locomotive Series='2ТЭ10МК' NumLoc='3371' Depo='1317' Consec='1' CarrierCode='4'>"
     L"<Crew EngineDriver='ШИTOB' TabNum='3111' Tim_Beg='2020-12-04 04:50' />"
     L"</Locomotive>"
     L"</EventInfo>"
     L"</Action>"
     L"</A2F_HINT>"
     );

    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_path_size(32) );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    auto asoup = pathAsoupList.front();
    CPPUNIT_ASSERT( al.IsServed( asoup ) );

    std::shared_ptr<TopologyTest> tt( new TopologyTest );
    tt->Load(L"*11001,11070 {11001,11003,11070}");
    userChart.setupTopology(tt);
    griffin::Sender gf = []( const griffin::Utility* ){};
    time_t t = (from_iso_string("20201204T044934Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, gf );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(35) );
    CPPUNIT_ASSERT( al.IsServed( asoup ) );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::ClearOnePersonMode()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20210329T202537Z' name='Form' Bdg='5C[09010]' waynum='5' parknum='1' index='' num='6751' length='6' weight='228' suburbreg='Y'>"
        L"<Locomotive Series='ЭР2' NumLoc='964' CarrierCode='24'>"
        L"<Crew EngineDriver='Gorsakovs' Tim_Beg='2021-03-29 23:35' PersonMode='1' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210329T203533Z' name='Departure' Bdg='5C[09010]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20210329T203933Z' name='Arrival' Bdg='2p[09100]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210329T204123Z' name='Departure' Bdg='2p[09100]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210329T204956Z' name='Transition' Bdg='KPU13A/6A[09104]'>"
        L"<rwcoord picketing1_val='5~750' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='8~800' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210329T210005Z' name='Arrival' Bdg='4p[09150]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210329T210145Z' name='Departure' Bdg='4Ap[09150]' waynum='4' parknum='1' optCode='09150:09160' />"
        L"<SpotEvent create_time='20210329T210918Z' name='Arrival' Bdg='2p[09160]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210329T211048Z' name='Departure' Bdg='2p[09160]' waynum='2' parknum='1' optCode='09160:09162' />"
        L"<SpotEvent create_time='20210329T211613Z' name='Transition' Bdg='AGP[09162]' waynum='1' parknum='1'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210329T211950Z' name='Arrival' Bdg='33C[09180]' waynum='33' parknum='1' />"
        L"<SpotEvent create_time='20210329T211950Z' name='Disform' Bdg='33C[09180]' waynum='33' parknum='1' />"
        L"</HemPath>");

    time_t tEvent9010 = (from_iso_string("20210329T202537Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::FORM, BadgeE(L"5C", EsrKit(9010)), tEvent9010);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );
    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    auto tdr = gis.front().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==6751 && tdr->isOnePersonMode() );

    Hem::Bill bill = createBill(
    L"<A2F_HINT Hint='cmd_edit' issue_moment='20210329T212944Z'>"
        L"<PrePoint name='Form' cover='Y' Bdg='5C[09010]' create_time='20210329T202537Z' index='' num='6751' length='6' weight='228' suburbreg='Y' waynum='5' parknum='1'>"
        L"<Locomotive Series='ЭР2' NumLoc='964' CarrierCode='24'>"
        L"<Crew EngineDriver='Gorsakovs' Tim_Beg='2021-03-29 23:35' PersonMode='1' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<PostPoint name='Disform' cover='Y' Bdg='33C[09180]' create_time='20210329T211950Z' waynum='33' parknum='1' />"
        L"<Action code='Identify' SaveLog='Y' District='09000-09640' DistrictRight='Y'>"
        L"<TrainInfo index='' num='6751' length='6' weight='228' suburbreg='Y'>"
        L"<Locomotive Series='ЭР2' NumLoc='964' CarrierCode='24'>"
        L"<Crew EngineDriver='Gorsakovs' Tim_Beg='2021-03-29 23:35' />"
        L"</Locomotive>"
        L"</TrainInfo>"
        L"</Action>"
        L"</A2F_HINT>");

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    tdr = gis.front().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==6751 && !tdr->isOnePersonMode() );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::SetOnePersonMode()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20210329T195740Z' name='Form' Bdg='SV6C[09764:09772]' waynum='1' index='' num='6257' suburbreg='Y'>"
        L"<rwcoord picketing1_val='130~280' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='131~210' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210329T200532Z' name='Transition' Bdg='1C[09772]' waynum='1' parknum='1' optCode='09750:09772' />"
        L"<SpotEvent create_time='20210329T203142Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20210329T204840Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' optCode='09751:09780' />"
        L"<SpotEvent create_time='20210329T210357Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' optCode='09780:09790' />"
        L"<SpotEvent create_time='20210329T212257Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210329T212843Z' name='Span_move' Bdg='SN3C[09790:09800]' waynum='1'>"
        L"<rwcoord picketing1_val='68~438' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='69~411' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>");

    time_t tEvent9772 = (from_iso_string("20210329T200532") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"1C", EsrKit(9772)), tEvent9772);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );
    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    auto tdr = gis.front().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==6257 && !tdr->isOnePersonMode() );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20210329T212944Z'>"
        L"<PrePoint name='Form' cover='Y' Bdg='SV6C[09764:09772]' create_time='20210329T195740Z' index='' num='6257' suburbreg='Y' waynum='1'>"
        L"<rwcoord picketing1_val='130~745' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<PostPoint name='Span_move' cover='Y' Bdg='SN3C[09790:09800]' create_time='20210329T212843Z' waynum='1'>"
        L"<rwcoord picketing1_val='68~924' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"<Action code='Identify' SaveLog='Y' District='09180-09820' DistrictRight='Y'>"
        L"<TrainInfo index='' num='6257' suburbreg='Y'>"
        L"<feat_texts typeinfo='М' />"
        L"</TrainInfo>"
        L"</Action>"
        L"</A2F_HINT>");

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    tdr = gis.front().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==6257 && tdr->isOnePersonMode() );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::WrongLocomotiveAfterIdentify_6334()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20210617T194627Z' name='Form' Bdg='4C[09180]' waynum='4' parknum='1' index='1263-090-0821' num='2102' length='52' weight='5006' net_weight='3485' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2ТЭ116' NumLoc='933' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='SAKNELS' Tim_Beg='2021-06-17 21:34' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210617T194627Z' name='Departure' Bdg='4C[09180]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210617T195204Z' name='Arrival' Bdg='BGP[09162]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210617T195238Z' name='Departure' Bdg='BGP[09162]' waynum='2' parknum='1'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210617T200456Z' name='Transition' Bdg='3p[09160]' waynum='3' parknum='1' optCode='09150:09160' />"
        L"<SpotEvent create_time='20210617T201721Z' name='Transition' Bdg='5p[09150]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20210617T203427Z' name='Transition' Bdg='NBP[09104]'>"
        L"<rwcoord picketing1_val='6~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210617T204107Z' name='Transition' Bdg='1p[09100]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20210617T204700Z' name='Transition' Bdg='4C[09010]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210617T210131Z' name='Arrival' Bdg='10JC[09006]' waynum='10' parknum='1' />"
        L"<SpotEvent create_time='20210618T054623Z' name='Departure' Bdg='10JC[09006]' waynum='10' parknum='1' index='1263-090-0821' num='2526' length='52' weight='5006' net_weight='3485' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2М62УМ' NumLoc='10' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-ZOMMERS' Tim_Beg='2021-06-18 07:13' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210618T060249Z' name='Transition' Bdg='11C[09500]' waynum='11' parknum='1' />"
        L"<SpotEvent create_time='20210618T060844Z' name='Transition' Bdg='2C[09580]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210618T061616Z' name='Transition' Bdg='2C[09570]' waynum='2' parknum='1' optCode='09550:09570' />"
        L"<SpotEvent create_time='20210618T062951Z' name='Transition' Bdg='2C[09550]' waynum='2' parknum='1' optCode='09540:09550' />"
        L"<SpotEvent create_time='20210618T063806Z' name='Transition' Bdg='2C[09540]' waynum='2' parknum='1' optCode='09530:09540' />"
        L"<SpotEvent create_time='20210618T064707Z' name='Transition' Bdg='2C[09530]' waynum='2' parknum='1' optCode='09520:09530' />"
        L"<SpotEvent create_time='20210618T065641Z' name='Transition' Bdg='2C[09520]' waynum='2' parknum='1' optCode='09511:09520' />"
        L"<SpotEvent create_time='20210618T071507Z' name='Transition' Bdg='4C[09511]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210618T073036Z' name='Transition' Bdg='1C[09510]' waynum='1' parknum='1' optCode='09510:11130' />"
        L"<SpotEvent create_time='20210618T073924Z' name='Transition' Bdg='1C[11130]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20210618T074018Z' name='Span_move' Bdg='IA3[11121:11130]' waynum='1'>"
        L"<rwcoord picketing1_val='75~' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='76~800' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"</HemPath>");

    time_t tEvent11130 = (from_iso_string("20210618T073924Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"1C", EsrKit(11130)), tEvent11130);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );
    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    auto tdr = gis.back().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==2526 && !tdr->isOnePersonMode() );
    Crew crew;
    tdr->get_crew( crew );
    CPPUNIT_ASSERT( crew.get_engineDriver()==L"M-ZOMMERS" );


    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20210618T074235Z'>"
        L"<PrePoint name='Arrival' cover='Y' Bdg='10JC[09006]' create_time='20210617T210131Z' waynum='10' parknum='1' />"
        L"<PostPoint name='Transition' cover='Y' Bdg='1C[11091]' create_time='20210618T084100Z' waynum='1' parknum='1' />"
        L"<Action code='Identify' SaveLog='Y' District='09580-08620' DistrictRight='Y'>"
        L"<TrainInfo index='1263-090-0821' num='2526' length='52' weight='5006' net_weight='3485' through='Y'>"
        L"<feat_texts typeinfo='ТМ' />"
        L"<Locomotive Series='2М62УМ' NumLoc='10' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-ZOMMERS' Tim_Beg='2021-06-18 07:13' PersonMode='1' />"
        L"</Locomotive>"
        L"</TrainInfo>"
        L"</Action>"
        L"</A2F_HINT>"
        );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );
    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    tdr = gis.back().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==2526 && !tdr->isOnePersonMode() );
    tdr->get_crew( crew );
    CPPUNIT_ASSERT( crew.get_engineDriver()==L"M-ZOMMERS" );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::EditLiterM_6357()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");
    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20210705T100500Z' name='ExplicitForm' Bdg='ASOUP 1042[09290]' waynum='3' parknum='1' index='0929-156-0983' num='3010' length='57' weight='4163' net_weight='2984' divisional='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1'>"
        L"<Crew EngineDriver='STEPANOVS' Tim_Beg='2021-07-05 14:01' />"
        L"</Locomotive>"
        L"<Locomotive Series='2М62УМ' NumLoc='93' Consec='9'>"
        L"<Crew EngineDriver='STEPANOVS' Tim_Beg='2021-07-05 14:01' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210705T132500Z' name='Departure' Bdg='ASOUP 1042_1p3w[09290]' waynum='3' parknum='1' index='0929-156-0983' num='3010' length='57' weight='4163' net_weight='2984' divisional='Y'>"
        L"<feat_texts typeinfo='ДТ' />"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1'>"
        L"<Crew EngineDriver='STEPANOVS' Tim_Beg='2021-07-05 14:01' />"
        L"</Locomotive>"
        L"<Locomotive Series='2М62УМ' NumLoc='93' Consec='9'>"
        L"<Crew EngineDriver='STEPANOVS' Tim_Beg='2021-07-05 14:01' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210705T134800Z' name='Transition' Bdg='ASOUP 1042_1p2w[09280]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210705T141056Z' name='Arrival' Bdg='9C[09180]' waynum='9' parknum='1' index='0929-156-0983' num='3009' length='57' weight='4163' net_weight='2984' divisional='Y'>"
        L"<feat_texts typeinfo='М' />"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1'>"
        L"<Crew EngineDriver='M-STEPANOVS' Tim_Beg='2021-07-05 13:05' PersonMode='1' />"
        L"</Locomotive>"
        L"<Locomotive Series='2М62УМ' NumLoc='93' Consec='9'>"
        L"<Crew EngineDriver='M-STEPANOVS' Tim_Beg='2021-07-05 13:05' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210705T150706Z' name='Departure' Bdg='9C[09180]' waynum='9' parknum='1' index='0929-156-0983' num='V3009' length='57' weight='4163' net_weight='2984' divisional='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1'>"
        L"<Crew EngineDriver='M-STEPANOVS' Tim_Beg='2021-07-05 13:05' />"
        L"</Locomotive>"
        L"<Locomotive Series='2М62УМ' NumLoc='93' Consec='9'>"
        L"<Crew EngineDriver='M-STEPANOVS' Tim_Beg='2021-07-05 13:05' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210705T153620Z' name='Transition' Bdg='1C[09764]' waynum='1' parknum='1' optCode='09764:09772' />"
        L"<SpotEvent create_time='20210705T155847Z' name='Transition' Bdg='1C[09772]' waynum='1' parknum='1' optCode='09750:09772' />"
        L"<SpotEvent create_time='20210705T161915Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20210705T163305Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' optCode='09751:09780' />"
        L"<SpotEvent create_time='20210705T164802Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' optCode='09780:09790' />"
        L"<SpotEvent create_time='20210705T170451Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210705T171541Z' name='Transition' Bdg='2C[09800]' waynum='2' parknum='1' optCode='09800:09801' />"
        L"<SpotEvent create_time='20210705T172344Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' optCode='09801:09802' />"
        L"<SpotEvent create_time='20210705T173700Z' name='Arrival' Bdg='3C[09802]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20210705T173728Z' name='Departure' Bdg='3C[09802]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20210705T174835Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' optCode='09803:09810' />"
        L"<SpotEvent create_time='20210705T180157Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' optCode='09810:09812' />"
        L"<SpotEvent create_time='20210705T181833Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' optCode='09812:09813' />"
        L"<SpotEvent create_time='20210705T182558Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' optCode='09813:09820' />"
        L"<SpotEvent create_time='20210705T183300Z' name='Arrival' Bdg='ASOUP 1042_4p2w[09820]' waynum='2' parknum='4' />"
        L"<SpotEvent create_time='20210705T200500Z' name='Disform' Bdg='ASOUP 1042_4p2w[09820]' waynum='2' parknum='4' />"
        L"</HemPath>");

    time_t tTransition9764 = (from_iso_string("20210705T153620Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"1C", EsrKit(9764)), tTransition9764);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );
    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 4 );
    auto tdr = gis.back().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==3009 && !tdr->isOnePersonMode() );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20210706T025955Z'>"
        L"<PrePoint name='Arrival' cover='Y' Bdg='9C[09180]' create_time='20210705T141056Z' index='0929-156-0983' num='3009' length='57' weight='4163' net_weight='2984' divisional='Y' waynum='9' parknum='1'>"
        L"<feat_texts typeinfo='М' />"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1'>"
        L"<Crew EngineDriver='M-STEPANOVS' Tim_Beg='2021-07-05 13:05' PersonMode='1' />"
        L"</Locomotive>"
        L"<Locomotive Series='2М62УМ' NumLoc='93' Consec='9'>"
        L"<Crew EngineDriver='M-STEPANOVS' Tim_Beg='2021-07-05 13:05' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<PostPoint name='Disform' cover='Y' Bdg='ASOUP 1042_4p2w[09820]' create_time='20210705T200500Z' waynum='2' parknum='4' />"
        L"<Action code='Identify' SaveLog='Y' District='09180-09820' DistrictRight='Y' FixBound='20210324T052000Z'>"
        L"<TrainInfo index='0929-156-0983' num='3009' length='57' weight='4163' net_weight='2984' divisional='Y'>"
        L"<feat_texts typeinfo='М' />"
        L"<Locomotive Series='2М62УМ' NumLoc='86' Consec='1'>"
        L"<Crew EngineDriver='M-STEPANOVS' Tim_Beg='2021-07-05 13:05' PersonMode='1' />"
        L"</Locomotive>"
        L"<Locomotive Series='2М62УМ' NumLoc='93' Consec='9'>"
        L"<Crew EngineDriver='M-STEPANOVS' Tim_Beg='2021-07-05 13:05' />"
        L"</Locomotive>"
        L"</TrainInfo>"
        L"</Action>"
        L"</A2F_HINT>"
        );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    happenPath = hl.GetWriteAccess()->getPath(spotPtr);
    CPPUNIT_ASSERT( happenPath );
    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 4 );
    tdr = gis.back().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==3009 && tdr->isOnePersonMode() );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::CheckRightOverstock_6500()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");

    FillDistrictGuideForRigamezgl_Zasulauks();

    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20210813T003804Z' name='Form' Bdg='10JC[09006]' waynum='10' parknum='1' index='0900-200-0970' num='3703' length='57' weight='2097' net_weight='909' transfer='Y'>"
        L"<feat_texts typeinfo='М' />"
        L"<Locomotive Series='2М62УМ' NumLoc='94' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-EJSAKS' Tim_Beg='2021-08-13 02:00' PersonMode='1' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210813T004040Z' name='Departure' Bdg='10JC[09006]' waynum='10' parknum='1'>"
        L"<Acceptance acceptance_type='enter' user_act='remove' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210813T005741Z' name='Transition' Bdg='1C[09010]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20210813T010347Z' name='Transition' Bdg='4p[09100]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210813T010957Z' name='Transition' Bdg='2AC[09670]' waynum='2' parknum='3' index='0900-200-0970' num='3703m' length='57' weight='2097' net_weight='909' transfer='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='94' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-EJSAKS' Tim_Beg='2021-08-13 02:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210813T011125Z' name='Transition' Bdg='1C[09670]' waynum='1' parknum='2' />"
        L"<SpotEvent create_time='20210813T011238Z' name='Transition' Bdg='1BC[09670]' waynum='1' parknum='1' optCode='09670:09680' />"
        L"<SpotEvent create_time='20210813T011946Z' name='Transition' Bdg='1C[09680]' waynum='1' parknum='1' optCode='09680:09710' />"
        L"<SpotEvent create_time='20210813T012532Z' name='Transition' Bdg='1C[09710]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20210813T013311Z' name='Arrival' Bdg='5C[09700]' waynum='5' parknum='1'>"
        L"<Acceptance acceptance_type='exit' user_act='remove' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210813T013600Z' name='Disform' Bdg='ASOUP 1042_1p5w[09700]' waynum='5' parknum='1' />"
        L"</HemPath>");

    time_t tTransition9680 = (from_iso_string("20210813T011946Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"1C", EsrKit(9680)), tTransition9680);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );
    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    auto tdr = gis.back().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==3703 && !tdr->isOnePersonMode() );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20210813T025953Z'>"
        L"<PrePoint name='Form' cover='Y' Bdg='10JC[09006]' create_time='20210813T003804Z' index='0900-200-0970' num='3703' length='57' weight='2097' net_weight='909' transfer='Y' waynum='10' parknum='1'>"
        L"<feat_texts typeinfo='М' />"
        L"<Locomotive Series='2М62УМ' NumLoc='94' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-EJSAKS' Tim_Beg='2021-08-13 02:00' PersonMode='1' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<PostPoint name='Disform' cover='Y' Bdg='ASOUP 1042_1p5w[09700]' create_time='20210813T013600Z' waynum='5' parknum='1' />"
        L"<Action code='Identify' SaveLog='Y' District='09000-09640' DistrictRight='Y' FixBound='20190724T164000Z'>"
        L"<TrainInfo index='0900-200-0970' num='3703' length='57' weight='2097' net_weight='909' transfer='Y'>"
        L"<feat_texts typeinfo='М' />"
        L"<Locomotive Series='2М62УМ' NumLoc='94' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-EJSAKS' Tim_Beg='2021-08-13 02:00' PersonMode='1' />"
        L"</Locomotive>"
        L"</TrainInfo>"
        L"</Action>"
        L"</A2F_HINT>"
        );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    happenPath = hl.GetWriteAccess()->getPath(spotPtr);
    CPPUNIT_ASSERT( happenPath );
    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    tdr = gis.back().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==3703 && tdr->isOnePersonMode() );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::CheckRightOverstockOnEndBorderStation_6500()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");

    FillDistrictGuideForRigamezgl_Zasulauks();

    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20210813T011926Z' name='Form' Bdg='5C[09010]' waynum='5' parknum='1' index='0001-010-0967' num='8213' stone='Y'>"
        L"<Locomotive Series='АДМ' NumLoc='1506' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='DJAKOVS' Tim_Beg='2021-08-12 20:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210813T012240Z' name='Departure' Bdg='5C[09010]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20210813T012705Z' name='Transition' Bdg='4p[09100]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20210813T013141Z' name='Transition' Bdg='2AC[09670]' waynum='2' parknum='3' optCode='09670' />"
        L"<SpotEvent create_time='20210813T013314Z' name='Arrival' Bdg='2C[09670]' waynum='2' parknum='2' />"
        L"<SpotEvent create_time='20210813T013314Z' name='Departure' Bdg='2C[09670]' waynum='2' parknum='2' />"
        L"<SpotEvent create_time='20210813T013941Z' name='Death' Bdg='11C[09670]' />"
        L"</HemPath>");

    time_t tTransition9100 = (from_iso_string("20210813T012705Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"4p", EsrKit(9100)), tTransition9100);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );
    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    auto tdr = gis.front().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==8213 && !tdr->isOnePersonMode() );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20210813T025953Z'>"
        L"<PrePoint name='Form' cover='Y' Bdg='5C[09010]' create_time='20210813T011926Z' index='0001-010-0967' num='8213' stone='Y' waynum='5' parknum='1'>"
        L"<Locomotive Series='АДМ' NumLoc='1506' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='DJAKOVS' Tim_Beg='2021-08-12 20:00' />"
        L"</Locomotive>"
        L"</PrePoint>"
        L"<PostPoint name='Death' cover='Y' Bdg='11C[09670]' create_time='20210813T013941Z' />"
        L"<Action code='Identify' SaveLog='Y' District='09000-09640' DistrictRight='Y' FixBound='20190724T164000Z'>"
        L"<TrainInfo index='0001-010-0967' num='8213' stone='Y'>"
        L"<feat_texts typeinfo='М' />"
        L"<Locomotive Series='АДМ' NumLoc='1506' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='DJAKOVS' Tim_Beg='2021-08-12 20:00' PersonMode='1' />"
        L"</Locomotive>"
        L"</TrainInfo>"
        L"</Action>"
        L"</A2F_HINT>"
        );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    happenPath = hl.GetWriteAccess()->getPath(spotPtr);
    CPPUNIT_ASSERT( happenPath );
    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    tdr = gis.back().second;
    CPPUNIT_ASSERT ( tdr && tdr->GetNumber().getNumber()==8213 && tdr->isOnePersonMode() );
    set_TimeZone("");
}

void TC_Hem_aeUserIdentify::FillDistrictGuideForRigamezgl_Zasulauks()
{
    DistrictGuide dg;

    attic::a_document doc;
    doc.load_wide(
        L"<DistrictList>"
        L"<District code='09000-09640' name='Rigas mezgls'>"
        L"<Involve esr='09690' />"
        L"<Involve esr='09502' />"
        L"<Involve esr='09430' />"
        L"<Involve esr='09990' />"
        L"<Involve esr='09700' />"
        L"<Involve esr='09710' />"
        L"<Involve esr='09680' />"
        L"<Involve esr='09670:09680' />"
        L"<Involve esr='09010' />"
        L"<Involve esr='09100' />"
        L"<Involve esr='09630' border='Y' />"
        L"<Involve esr='09580' border='Y' />"
        L"<Involve esr='09640' />"
        L"<Involve esr='09650' />"
        L"<Involve esr='09042' />"
        L"<Involve esr='09006' target='Y' />"
        L"<Involve esr='09008' target='Y' />"
        L"<Involve esr='09000' border='Y' target='Y' />"
        L"<Involve esr='09500' />"
        L"<Involve esr='09050' />"
        L"<Involve esr='09490' />"
        L"</District>"
        L"<District code='09010-09180' name='Jurmalas loks'>"
        L"<Involve esr='09670' />"
        L"<Involve esr='09100:09670' />"
        L"<Involve esr='09100:09104' />"
        L"<Involve esr='09104' />"
        L"<Involve esr='09150' />"
        L"<Involve esr='09160' />"
        L"<Involve esr='09162' />"
        L"<Involve esr='09162:09180' />"
        L"<Involve esr='09676' />"
        L"<Involve esr='09715' />"
        L"<Involve esr='09730' />"
        L"<Involve esr='09732' />"
        L"<Involve esr='09736' />"
        L"<Involve esr='09736:09750' />"
        L"</District>"
        L"</DistrictList>" );

    dg.deserialize( doc.document_element() );

    Hem::DistrictSection::instance()->Set( dg );
}

void TC_Hem_aeUserIdentify::CheckRightOverstockOnStartBorderStation_6500()
{
    set_TimeZone("NNN+02DDD+01,M3.5.0/03:00,M10.5.0/04:00");

    FillDistrictGuideForRigamezgl_Zasulauks();

    UtLayer<HappenLayer> hl;
    AsoupLayer al;
    Regulatory::Layer rl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20210813T020600Z' name='Arrival' Bdg='?p3w2[09670]' waynum='2' parknum='3' />"
        L"<SpotEvent create_time='20210813T021700Z' name='Departure' Bdg='?p3w2[09670]' waynum='2' parknum='3' />"
        L"<SpotEvent create_time='20210813T021909Z' name='Transition' Bdg='?p1w2[09100]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210813T022626Z' name='Transition' Bdg='?p1w2[09010]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210813T023400Z' name='Span_move' Bdg='?[09006:09010]'>"
        L"<rwcoord picketing1_val='2~805' picketing1_comm='Рига-Зилупе' picketing2_val='2~805' picketing2_comm='Шкиротава-Я' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210813T023400Z' name='Death' Bdg='?[09006:09010]'>"
        L"<rwcoord picketing1_val='2~805' picketing1_comm='Рига-Зилупе' picketing2_val='2~805' picketing2_comm='Шкиротава-Я' />"
        L"</SpotEvent>"
        L"</HemPath>");

    time_t tArrival9670 = (from_iso_string("20210813T020600Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::ARRIVAL, BadgeE(L"?p3w2", EsrKit(9670)), tArrival9670);
    CPPUNIT_ASSERT(spotPtr);
    auto happenPath = hl.GetWriteAccess()->getPath(spotPtr);

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( happenPath );
    auto gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.empty() );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20210813T025953Z'>"
        L"<PrePoint name='Arrival' cover='Y' Bdg='?p3w2[09670]' create_time='20210813T020600Z' waynum='2' parknum='3' />"
        L"<PostPoint name='Death' cover='Y' Bdg='?[09006:09010]' create_time='20210813T023400Z'>"
        L"<rwcoord picketing1_val='2~805' picketing1_comm='Рига-Зилупе' picketing2_val='2~805' picketing2_comm='Шкиротава-Я' />"
        L"</PostPoint>"
        L"<Action code='Identify' SaveLog='Y' District='09000-09640' DistrictRight='Y' FixBound='20190724T164000Z'>"
        L"<TrainInfo index='' num='1111' cont='Y' />"
        L"</Action>"
        L"</A2F_HINT>"
        );

    Hem::AccessHappenAsoupRegulatory access(hl.base(), al, rl);
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0); 
    Hem::aeUserIdentify applier(std::move(access), m_asoupQMgr, context, bill, m_isPassengerWay);
    CPPUNIT_ASSERT_NO_THROW(applier.Action());

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    happenPath = hl.GetWriteAccess()->getPath(spotPtr);
    CPPUNIT_ASSERT( happenPath );
    gis = happenPath->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    auto tdrCheck = gis.front().second;
    auto tCheck = gis.front().first;
    CPPUNIT_ASSERT ( tdrCheck && tdrCheck->GetNumber().getNumber()==1111 && tCheck==tArrival9670 );
    set_TimeZone("");
}


