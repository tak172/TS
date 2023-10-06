#pragma once

#include <vector>
#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Badge_fwd.h"
#include "../Hem/SpotToRegulatory.h"

namespace Hem
{
	template<typename T> class EventsPath;
	typedef EventsPath<SpotEvent> HappenPath;
    class IdentifyPolicy;
}

class TC_SpotToRegulatory : public CPPUNIT_NS::TestFixture
{
public:
    TC_SpotToRegulatory(void);
    ~TC_SpotToRegulatory(void);
    
    CPPUNIT_TEST_SUITE( TC_SpotToRegulatory );
    CPPUNIT_TEST( isAppliableRegulatoryRoute );
    CPPUNIT_TEST( findIdentification );
    CPPUNIT_TEST( findIdentificationForStation );
    CPPUNIT_TEST( findIdentificationPossibilities );
    CPPUNIT_TEST_SUITE_END();

private:
    typedef std::shared_ptr<Regulatory::ScheduledPath> RegPathPtr;
    typedef std::shared_ptr<Hem::HappenPath> HappenPathPtr;
    typedef Regulatory::Layer::RegulatoryPathManager RegulatoryPathManager;
    typedef std::vector<BadgeE> BadgesVec;
    time_t m_time;
    unsigned const m_appliableRoute;
    unsigned const m_disabledRoute;
    std::unique_ptr<UncontrolledStations> m_uncontrolledStations;

    virtual void setUp() override;

    BadgesVec createBadgesVec(size_t size) const;
    template <class T>
    HappenPathPtr createHappenPath(T begin, T end, const Hem::IdentifyPolicy& policy) const;
    template <class T>
    RegPathPtr createRegulatoryPath(T begin, T end, unsigned route, const Hem::IdentifyPolicy& policy) const;
    template <class T>
    void generateRegulatoryPath(RegulatoryPathManager::WriteAccess&& layer, T begin, T end, unsigned route, const Hem::IdentifyPolicy& policy) const;

    // ѕроверка номеров нормативных нитей
    void isAppliableRegulatoryRoute();

    // “естирование метода проверки возможности идентификации 
    // данной нити исполенного графика нитью нормативного графика
    void findIdentification();

    // “естирование метода поиска вариантов идентификации
    // данной нити исполненного графика дл€ станционного событи€
    // (предполагаетс€ его использование при отслеживании нового событи€)
    void findIdentificationForStation();

    // “естирование метода поиска всех вариантов идентификации
    // данной нити исполненного графика
    void findIdentificationPossibilities();
};

