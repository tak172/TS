#include "stdafx.h"

#include "TC_Hem_aePathRestrictor.h"
#include "../Hem/Appliers/aePathRestrictor.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/RegulatoryLayer.h"
#include "../Hem/AutoIdentifyPolicy.h"
#include "../helpful/PicketingInfo.h"
#include "../Guess/SpotDetails.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "UtHelpfulDateTime.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aePathRestrictor );

using Regulatory::SpotEventPtr;

#define TEST_CONSTANTS() \
    const time_t c_time1 = timeForDateTime(2015, 8, 30, 10, 10);\
    const time_t c_time2 = timeForDateTime(2015, 8, 30, 10, 20);\
    const time_t c_time3 = timeForDateTime(2015, 8, 30, 10, 30);\
    const time_t c_time4 = timeForDateTime(2015, 8, 30, 10, 40);\
    const time_t c_time5 = timeForDateTime(2015, 8, 30, 10, 50);\
    const BadgeE c_station1(L"1", EsrKit(100));\
    const BadgeE c_station2(L"2", EsrKit(200));\
    const BadgeE c_station3(L"3", EsrKit(300));\
    const BadgeE c_station4(L"4", EsrKit(400));\
    const BadgeE c_station5(L"5", EsrKit(500));

bool isPassengerFunc(const EsrKit& /* esr */, const ParkWayKit& /* parkWay */)
{
    return true;
}

void TC_Hem_aePathRestrictor::setUp()
{
    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[2015-Aug-30 10:00:00/2016-Aug-31 10:00:00]'>\n"
        L"  <ScheduledPath>\n"
        L"    <SpotEvent create_time='20150830T101000Z' name='Info_changing' Bdg='1p[00100]' index='' num='6024' suburbreg='Y' />\n" // c_time1
        L"    <SpotEvent create_time='20150830T101000Z' name='Departure' Bdg='1p[00100]' />\n" // c_time1
        L"    <SpotEvent create_time='20150830T102000Z' name='Transition' Bdg='2p[00200]' />\n" // c_time2
        L"    <SpotEvent create_time='20150830T103000Z' name='Arrival' Bdg='3p[00300]' />\n" // c_time3
        L"    <SpotEvent create_time='20150830T104000Z' name='Departure' Bdg='4p[00400]' />\n" // c_time4
        L"    <SpotEvent create_time='20150830T105000Z' name='Arrival' Bdg='5p[00500]' />\n" // c_time5
        L"  </ScheduledPath>\n"
        L"  <ScheduledPath>\n"
        L"    <SpotEvent create_time='20150830T101100Z' name='Info_changing' Bdg='1p[00100]' index='' num='2024' through='Y' />\n" // c_time1 (Номер 2024 не подходит)
        L"    <SpotEvent create_time='20150830T101100Z' name='Departure' Bdg='1p[00100]' />\n" // c_time1
        L"    <SpotEvent create_time='20150830T102100Z' name='Transition' Bdg='2p[00200]' />\n" // c_time2
        L"    <SpotEvent create_time='20150830T103100Z' name='Arrival' Bdg='3p[00300]' />\n" // c_time3
        L"    <SpotEvent create_time='20150830T104100Z' name='Departure' Bdg='4p[00400]' />\n" // c_time4
        L"    <SpotEvent create_time='20150830T105100Z' name='Arrival' Bdg='5p[00500]' />\n" // c_time5
        L"  </ScheduledPath>\n"
        L"  <ScheduledPath>\n"
        L"    <SpotEvent create_time='20150830T110000Z' name='Info_changing' Bdg='1p[00500]' index='' num='6048' suburbreg='Y' />\n" // c_time2_1
        L"    <SpotEvent create_time='20150830T110000Z' name='Departure' Bdg='1p[00500]' />\n" // c_time2_1
        L"    <SpotEvent create_time='20150830T120000Z' name='Transition' Bdg='1p[00400]' />\n" // c_time2_2
        L"    <SpotEvent create_time='20150830T130000Z' name='Arrival' Bdg='3p[00300]' />\n" // c_time2_3
        L"    <SpotEvent create_time='20150830T140000Z' name='Departure' Bdg='4p[00200]' />\n" // c_time2_4
        L"  </ScheduledPath>\n"
        L"</ScheduledLayer>\n"
        L"</export>\n";

    m_regulatoryLayer.reset(new Regulatory::Layer());

    {
        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(layerData));
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(attic::a_node());
    }
}

void TC_Hem_aePathRestrictor::tearDown()
{
    m_regulatoryLayer.reset();
}

void TC_Hem_aePathRestrictor::Creation()
{
    HappenLayer happenLayer;

    // Нельзя передавать пустой указатель на событие
    CPPUNIT_ASSERT_THROW(Hem::aePathRestrictor(Hem::AccessHappenRegulatory(happenLayer, *m_regulatoryLayer),
        std::shared_ptr<const SpotEvent>(), &isPassengerFunc), HemHelpful::HemException);

    CPPUNIT_ASSERT_NO_THROW(Hem::aePathRestrictor(Hem::AccessHappenRegulatory(happenLayer, *m_regulatoryLayer),
        std::make_shared<const SpotEvent>(HCode::FORM, BadgeE(L"bdg", EsrKit(11111)), 1), &isPassengerFunc) );
}

void TC_Hem_aePathRestrictor::ClearRun()
{
    UtLayer<HappenLayer> happenLayer;
    Hem::Chance chance = Hem::Chance::System();
    UtIdentifyCategoryList identifyCategory;

    TEST_CONSTANTS()

    // Совпадает с ниткой нормативки с номером 6024
    SpotEventPtr pathEvents[] = { 
        std::make_shared<SpotEvent>( HCode::FORM, c_station1, c_time1 ),
        std::make_shared<SpotEvent>( HCode::DEPARTURE, c_station1, c_time1 ),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station2, c_time2 ),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station3, c_time3 ),
        std::make_shared<SpotEvent>( HCode::ARRIVAL, c_station4, c_time4 )
    };

    happenLayer.createPath( pathEvents);
    {
        auto path = happenLayer.GetWriteAccess()->getPath(pathEvents[0]);
        path->IdentifyWith(pathEvents[0], TrainDescr(L"6024"), identifyCategory, chance, Hem::SoftIdentifyPolicy());
        CPPUNIT_ASSERT(path);
        CPPUNIT_ASSERT_EQUAL(size_t(5), path->GetEventsCount());
    }

    SpotEventPtr anchor;
    Hem::aePathRestrictor engine(Hem::AccessHappenRegulatory(happenLayer.base(), *m_regulatoryLayer), pathEvents[1], &isPassengerFunc);

    engine.Action();

    {
        auto path = happenLayer.GetReadAccess()->getPath(pathEvents[0]);
        CPPUNIT_ASSERT(path);
        CPPUNIT_ASSERT_EQUAL(size_t(5), path->GetEventsCount());
    }
}

void TC_Hem_aePathRestrictor::Departure()
{
    UtLayer<HappenLayer> happenLayer;
    Hem::Chance chance = Hem::Chance::System();
    UtIdentifyCategoryList identifyCategory;
    TEST_CONSTANTS()

    const BadgeE c_station0(L"11", EsrKit(1100));

    std::shared_ptr<SpotDetails> details = std::make_shared<SpotDetails>(ParkWayKit(1, 1), 
        std::make_pair(PicketingInfo(), PicketingInfo()));
   
    // Выходит за рамки нормативки с номером 6024 в начале
    SpotEventPtr pathEvents[] = { 
        std::make_shared<SpotEvent>( HCode::FORM, c_station0, c_time1 - 20 ),
        std::make_shared<SpotEvent>( HCode::DEPARTURE, c_station0, c_time1 - 18 ),
        std::make_shared<SpotEvent>( HCode::ARRIVAL, c_station1, c_time1 - 1, details ),
        std::make_shared<SpotEvent>( HCode::DEPARTURE, c_station1, c_time1, details ),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station2, c_time2 ),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station3, c_time3 ),
        std::make_shared<SpotEvent>( HCode::ARRIVAL, c_station4, c_time4 )
    };

    happenLayer.createPath( pathEvents);
    {
        auto path = happenLayer.GetWriteAccess()->getPath(pathEvents[0]);
        path->IdentifyWith(pathEvents[0], TrainDescr(L"6024"), identifyCategory, chance, Hem::SoftIdentifyPolicy());
        CPPUNIT_ASSERT(path);
        CPPUNIT_ASSERT_EQUAL(size_t(7), path->GetEventsCount());
    }

    SpotEventPtr anchor;
    Hem::aePathRestrictor engine(Hem::AccessHappenRegulatory(happenLayer.base(), *m_regulatoryLayer), pathEvents[1], &isPassengerFunc);

    engine.Action();

    {
        auto path0 = happenLayer.GetReadAccess()->getPath(pathEvents[0]);
        CPPUNIT_ASSERT(path0);

        auto path1 = happenLayer.GetReadAccess()->getPath(pathEvents[6]);
        CPPUNIT_ASSERT(path1);
        CPPUNIT_ASSERT(path0 != path1);

        CPPUNIT_ASSERT_EQUAL(size_t(4), path0->GetEventsCount());
        CPPUNIT_ASSERT_EQUAL(size_t(5), path1->GetEventsCount());
    }
}

void TC_Hem_aePathRestrictor::Transition()
{
    UtLayer<HappenLayer> happenLayer;
    Hem::Chance chance = Hem::Chance::System();
    UtIdentifyCategoryList identifyCategory;
    TEST_CONSTANTS()

        const BadgeE c_station0(L"11", EsrKit(1100));

    std::shared_ptr<SpotDetails> details = std::make_shared<SpotDetails>(ParkWayKit(1, 1), 
        std::make_pair(PicketingInfo(), PicketingInfo()));

    // Выходит за рамки нормативки с номером 6024 в начале
    SpotEventPtr pathEvents[] = { 
        std::make_shared<SpotEvent>( HCode::FORM, c_station0, c_time1 - 20 ),
        std::make_shared<SpotEvent>( HCode::DEPARTURE, c_station0, c_time1 - 18 ),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station1, c_time1, details ),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station2, c_time2 ),
        std::make_shared<SpotEvent>( HCode::TRANSITION, c_station3, c_time3 ),
        std::make_shared<SpotEvent>( HCode::ARRIVAL, c_station4, c_time4 )
    };

    happenLayer.createPath( pathEvents);
    {
        auto path = happenLayer.GetWriteAccess()->getPath(pathEvents[0]);
        path->IdentifyWith(pathEvents[0], TrainDescr(L"6024"), identifyCategory, chance, Hem::SoftIdentifyPolicy());
        CPPUNIT_ASSERT(path);
        CPPUNIT_ASSERT_EQUAL(size_t(6), path->GetEventsCount());
    }

    SpotEventPtr anchor;
    Hem::aePathRestrictor engine(Hem::AccessHappenRegulatory(happenLayer.base(), *m_regulatoryLayer), pathEvents[1], &isPassengerFunc);

    engine.Action();

    {
        auto path0 = happenLayer.GetReadAccess()->getPath(pathEvents[0]);
        CPPUNIT_ASSERT(path0);

        auto path1 = happenLayer.GetReadAccess()->getPath(pathEvents[5]);
        CPPUNIT_ASSERT(path1);
        CPPUNIT_ASSERT(path0 != path1);

        CPPUNIT_ASSERT_EQUAL(size_t(4), path0->GetEventsCount());
        CPPUNIT_ASSERT_EQUAL(size_t(5), path1->GetEventsCount());
    }
}