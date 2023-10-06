#include "stdafx.h"
#include "TC_SpotToRegulatory.h"
#include "../Hem/SpotToRegulatory.h"
#include "../Hem/AutoIdentifyPolicy.h"
#include "../Hem/UserIdentifyPolicy.h"
#include "../Hem/HappenPath.h"
#include "../Guess/SpotDetails.h"
#include "../helpful/Badge.h"
#include "UtHelpfulDateTime.h"
#include "../Hem/UncontrolledStations.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SpotToRegulatory );

using namespace Hem;
typedef SpotToRegulatory::RegulatoryPathPtr RegulatoryPathPtr;
typedef Regulatory::Layer::RegulatoryPathManager RegulatoryPathManager;

// класс хранителя изменений (заглушка)
class UtChangesHolder
{
    void operator() () {};
};

TC_SpotToRegulatory::TC_SpotToRegulatory(void)
    : 
    m_time(0),
    // Пассажирский поезд
    m_appliableRoute(133), 
    // Транзитный грузовой
    m_disabledRoute(2001)
{
}

TC_SpotToRegulatory::~TC_SpotToRegulatory(void)
{
}

void TC_SpotToRegulatory::setUp()
{
    m_time = timeForDate(2015, 10, 10);
    m_uncontrolledStations.reset( new UncontrolledStations() );
}

TC_SpotToRegulatory::BadgesVec TC_SpotToRegulatory::createBadgesVec(size_t size) const
{
    unsigned const c_esrBase = 1000;

    BadgesVec result;
    result.reserve(size);

    for (size_t i = 0u; i < size; ++i)
        result.emplace_back(L"A", EsrKit(c_esrBase + static_cast<unsigned>(i) * 100u));

    return result;
}

void incrementTime(time_t& time, const IdentifyPolicy& policy)
{
    time += 4 * policy.getMaximumRegulatoryAdvanceSec();
}

template <class T>
TC_SpotToRegulatory::HappenPathPtr TC_SpotToRegulatory::createHappenPath(T begin, T end, const IdentifyPolicy& policy) const
{
    HappenPathPtr path = std::make_shared<Hem::HappenPath>(std::make_shared<SpotEvent>(HCode::FORM, *begin, m_time));
    time_t latestTime = m_time;
    for (auto it = begin; it != end; ++it)
    {
        incrementTime(latestTime, policy);
        path->DoAppend(std::make_shared<SpotEvent>(HCode::TRANSITION, *it, latestTime));
    }

    return path;
}

template <class T>
TC_SpotToRegulatory::RegPathPtr TC_SpotToRegulatory::createRegulatoryPath(
    T begin, T end, unsigned route, const IdentifyPolicy& policy) const
{
    auto path = std::make_shared<Regulatory::ScheduledPath>(route);
    time_t latestTime = m_time;
    for (auto it = begin; it != end; ++it)
    {
        incrementTime(latestTime, policy);
        path->PushBack(SpotEvent(HCode::TRANSITION, *it, latestTime));
    }
    return path;
}

template <class T>
void TC_SpotToRegulatory::generateRegulatoryPath(RegulatoryPathManager::WriteAccess&& layer, 
                                                 T begin, T end, unsigned route, const IdentifyPolicy& policy) const
{
    auto path = createRegulatoryPath(begin, end, route, policy);
    auto pathSpots = path->GetSpots();
    for (const auto& spot : pathSpots)
    {
        layer->registerEvent(spot, path);
    }
}

void TC_SpotToRegulatory::isAppliableRegulatoryRoute()
{
    // Passenger, uint_interval( 1, 998 )
    // Rapid, uint_interval( 701, 750 )
    // Highrapid, uint_interval( 751, 788 )
    // MVPS_Served, uint_interval( 801, 898 )
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(1));
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(998));
    // TechnicalPassenger, uint_interval( 5001, 5998 )
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(5001));
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(5998));
    // TechnicalMotorcar, uint_interval( 7631, 7998 ) );
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(7631));
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(7998));
    // Suburban, uint_interval( 6001, 7628 )
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(6001));
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(7624));

    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRoute(m_appliableRoute));
    CPPUNIT_ASSERT(!SpotToRegulatory::isAppliableRegulatoryRoute(m_disabledRoute));
}

void TC_SpotToRegulatory::findIdentification()
{
    size_t const regulatoryPathLength = 10;
    BadgesVec const badges = createBadgesVec(regulatoryPathLength);
    StrictIdentifyPolicy const policy;
    UserIdentifyPolicy const userPolicy;

    // Совпадение всех станций
    CPPUNIT_ASSERT(SpotToRegulatory::findIdentification(
        *createHappenPath(badges.cbegin(), badges.cend(), policy),
        createRegulatoryPath(badges.cbegin(), badges.cend(), m_appliableRoute, policy), true, policy));

    // Совпадение всех станций (неподходящий номер маршрута)
    // ToDo: Поведение системы для этого метода не описано в ТЗ,
    // поэтому проверка на номер не выполяется. Используется при ручной идентификации пользователем.
    CPPUNIT_ASSERT(/* ! */SpotToRegulatory::findIdentification(
        *createHappenPath(badges.cbegin(), badges.cend(), policy),
        createRegulatoryPath(badges.cbegin(), badges.cend(), m_disabledRoute, policy), true, policy));

    // Совпадение всех станций в обратном порядке
    CPPUNIT_ASSERT(!SpotToRegulatory::findIdentification(
        *createHappenPath(badges.crbegin(), badges.crend(), policy),
        createRegulatoryPath(badges.cbegin(), badges.cend(), m_appliableRoute, policy), true, policy));

    // Нитка исполненного графика короче нитки нормативки (минимальная длина для идентификации)
    CPPUNIT_ASSERT(SpotToRegulatory::findIdentification(
        *createHappenPath(badges.cbegin(), badges.cend() - regulatoryPathLength + policy.getMinimumRegulatoryStationsOverlap(), policy),
        createRegulatoryPath(badges.cbegin(), badges.cend(), m_appliableRoute, policy), true, policy));

    // Нитка исполненного графика короче нитки нормативки (меньше минимальной длины для идентификации)
    CPPUNIT_ASSERT(!SpotToRegulatory::findIdentification(
        *createHappenPath(badges.cbegin(), badges.cend() - regulatoryPathLength + policy.getMinimumRegulatoryStationsOverlap() - 1, policy),
        createRegulatoryPath(badges.cbegin(), badges.cend(), m_appliableRoute, policy), true, policy));
    CPPUNIT_ASSERT(!SpotToRegulatory::findIdentification(
        *createHappenPath(badges.cbegin(), badges.cend() - regulatoryPathLength + userPolicy.getMinimumRegulatoryStationsOverlap() - 1, userPolicy),
        createRegulatoryPath(badges.cbegin(), badges.cend(), m_appliableRoute, userPolicy), true, userPolicy));

    {
        // Нитка исполненного графика длиннее нитки нормативного графика (прошла станцию, которой нет в нормативке)
        HappenPathPtr happenPath = createHappenPath(badges.cbegin(), badges.cend(), policy);

        incrementTime(m_time, policy);
        RegPathPtr regulatoryPath = createRegulatoryPath(badges.cbegin() + 1, badges.cend(), m_appliableRoute, policy);
        CPPUNIT_ASSERT(!SpotToRegulatory::findIdentification(*happenPath, regulatoryPath, true, policy));
    }

    {
        // Нитка исполненного графика длиннее нитки нормативного графика (прошла станцию, которой нет в нормативке)
        HappenPathPtr happenPath = createHappenPath(badges.cbegin(), badges.cend(), userPolicy);

        incrementTime(m_time, userPolicy);
        RegPathPtr regulatoryPath = createRegulatoryPath(badges.cbegin() + 1, badges.cend(), m_appliableRoute, userPolicy);
        CPPUNIT_ASSERT(SpotToRegulatory::findIdentification(*happenPath, regulatoryPath, true, userPolicy));
    }

     {
         // Нитка исполненного графика длиннее нитки нормативного графика (прошла станцию, которой нет в нормативке)
         HappenPathPtr happenPath = createHappenPath(badges.cbegin(), badges.cend(), policy);
         RegPathPtr regulatoryPath = createRegulatoryPath(badges.cbegin(), badges.cend() - 1, m_appliableRoute, policy);
         CPPUNIT_ASSERT(!SpotToRegulatory::findIdentification(*happenPath, regulatoryPath, true, policy));
     }

    {
        // Нитка исполненного графика длиннее нитки нормативного графика (прошла станцию, которой нет в нормативке)
        HappenPathPtr happenPath = createHappenPath(badges.cbegin(), badges.cend(), userPolicy);
        RegPathPtr regulatoryPath = createRegulatoryPath(badges.cbegin(), badges.cend() - 1, m_appliableRoute, userPolicy);
        CPPUNIT_ASSERT(SpotToRegulatory::findIdentification(*happenPath, regulatoryPath, true, userPolicy));
    }
}

void TC_SpotToRegulatory::findIdentificationForStation()
{
    unsigned const m_appliableRoute = 733;
    CPPUNIT_ASSERT(SpotToRegulatory::isAppliableRegulatoryRouteForAutoIdentification(m_appliableRoute));
    size_t const regulatoryPathLength = 6;
    BadgesVec const badges = createBadgesVec(regulatoryPathLength);
    StrictIdentifyPolicy const policy;
    Regulatory::ChangesHolder regChangesHolder;

    {
        // Совпадающие станции на обеих нитях
        RegulatoryPathManager regulatoryPathManager("UTLayer", "UTPath",regChangesHolder);
        generateRegulatoryPath(regulatoryPathManager.getWriteAccess(),
            badges.cbegin(), badges.cend(), m_appliableRoute, policy);

        // Поиск идентификации для последней станции
        CPPUNIT_ASSERT(SpotToRegulatory::findIdentificationForStation(*m_uncontrolledStations, regulatoryPathManager.getReadAccess(),
            *createHappenPath(badges.cbegin(), badges.cend(), policy), badges.back().num(), true, policy));

        // Поиск идентификации для первой станции не должен возвращать вариантов,
        // так как пересечений ДО этой станции явно недостаточно
        CPPUNIT_ASSERT(!SpotToRegulatory::findIdentificationForStation(*m_uncontrolledStations, regulatoryPathManager.getReadAccess(),
            *createHappenPath(badges.cbegin(), badges.cend(), policy), badges.front().num(), true, policy));
    }

    {
        // Совпадающие станции на обеих нитях, грузовой поезд
        RegulatoryPathManager regulatoryPathManager("UTLayer", "UTPath",regChangesHolder);
        generateRegulatoryPath(regulatoryPathManager.getWriteAccess(),
            badges.cbegin(), badges.cend(), m_disabledRoute, policy);

        // Поиск идентификации для последней станции
        CPPUNIT_ASSERT(!SpotToRegulatory::findIdentificationForStation(*m_uncontrolledStations, regulatoryPathManager.getReadAccess(),
            *createHappenPath(badges.cbegin(), badges.cend(), policy), badges.back().num(), true, policy));
    }

    {
        // Нитка исполненного графика начинается на станции, которой нет в нитке нормативного графика
        HappenPathPtr happenPath = createHappenPath(badges.cbegin(), badges.cend(), policy);

        RegulatoryPathManager regulatoryPathManager("UTLayer", "UTPath",regChangesHolder);

        incrementTime(m_time, policy);
        generateRegulatoryPath(regulatoryPathManager.getWriteAccess(),
            badges.cbegin() + 1, badges.cend(), m_appliableRoute, policy);

        // Не должно быть вариантов из-за несовпадения станций 
        CPPUNIT_ASSERT(!SpotToRegulatory::findIdentificationForStation(*m_uncontrolledStations, regulatoryPathManager.getReadAccess(),
            *happenPath, badges.back().num(), true, policy));
    }
}

void TC_SpotToRegulatory::findIdentificationPossibilities()
{
    unsigned const m_appliableRoute = 133;
    size_t const regulatoryPathLength = 6;
    BadgesVec const badges = createBadgesVec(regulatoryPathLength);
    StrictIdentifyPolicy const policy;
    Regulatory::ChangesHolder regChangesHolder;

    {
        // Совпадение всех станций
        RegulatoryPathManager regulatoryPathManager("UTLayer", "UTPath",regChangesHolder);
        generateRegulatoryPath(regulatoryPathManager.getWriteAccess(),
            badges.cbegin(), badges.cend(), m_appliableRoute, policy);

        auto possibilites = SpotToRegulatory::findIdentificationPossibilites(
            regulatoryPathManager.getReadAccess(), 
            *createHappenPath(badges.cbegin(), badges.cend(), policy), true, policy);
        CPPUNIT_ASSERT_EQUAL(size_t(1), possibilites.size());
    }

    {
        // Совпадение всех станций, грузовой поезд
        RegulatoryPathManager regulatoryPathManager("UTLayer", "UTPath",regChangesHolder);
        generateRegulatoryPath(regulatoryPathManager.getWriteAccess(),
            badges.cbegin(), badges.cend(), m_disabledRoute, policy);

        auto possibilites = SpotToRegulatory::findIdentificationPossibilites(
            regulatoryPathManager.getReadAccess(), 
            *createHappenPath(badges.cbegin(), badges.cend(), policy), true, policy);
        CPPUNIT_ASSERT_EQUAL(size_t(1), possibilites.size());
    }

    {
        // Нить исполненного графика короче нитки нормативного, но начинается с той же станции, что и нормативка
        RegulatoryPathManager regulatoryPathManager("UTLayer", "UTPath",regChangesHolder);
        generateRegulatoryPath(regulatoryPathManager.getWriteAccess(),
            badges.cbegin(), badges.cend(), m_appliableRoute, policy);

        auto possibilites = SpotToRegulatory::findIdentificationPossibilites(
            regulatoryPathManager.getReadAccess(), 
            *createHappenPath(badges.cbegin(), badges.cend() - 1, policy), true, policy);
        CPPUNIT_ASSERT_EQUAL(size_t(1), possibilites.size());
    }

    {
        // Нить исполненного графика короче нитки нормативного, но начинается не с первой станции нормативной нити
        RegulatoryPathManager regulatoryPathManager("UTLayer", "UTPath",regChangesHolder);
        generateRegulatoryPath(regulatoryPathManager.getWriteAccess(),
            badges.cbegin(), badges.cend(), m_appliableRoute, policy);

        incrementTime(m_time, policy);

        auto possibilites = SpotToRegulatory::findIdentificationPossibilites(
            regulatoryPathManager.getReadAccess(), 
            *createHappenPath(badges.cbegin() + 1, badges.cend(), policy), true, policy);
        CPPUNIT_ASSERT(possibilites.empty());
    }

    {
        // Нитка нормативного графика короче нитки исполненного
        HappenPathPtr happenPath = createHappenPath(badges.cbegin(), badges.cend(), policy);

        incrementTime(m_time, policy);

        RegulatoryPathManager regulatoryPathManager("UTLayer", "UTPath",regChangesHolder);
        generateRegulatoryPath(regulatoryPathManager.getWriteAccess(),
            badges.cbegin() + 1, badges.cend(), m_appliableRoute, policy);

        // Если в нитке нормативного графика не хватает
        // станций, через которые прошла нитка исполненного графика, идентификация невозможна
        auto possibilites = SpotToRegulatory::findIdentificationPossibilites(
            regulatoryPathManager.getReadAccess(), *happenPath, true, policy);
        CPPUNIT_ASSERT(possibilites.empty());
    }
}
