#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"
#include "UtTearDown.h"
#include "../helpful/TrainDescr.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/RegulatoryLayer.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/UncontrolledStations.h"
#include "../Hem/AsoupProcessingQueue.h"

class TopologyTest;
class EsrGuide;
namespace Hem
{
    class GuessTransciever;
};

class TC_Hem_aeUserIdentify : public UtTearDowner
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeUserIdentify );
    CPPUNIT_TEST( Asoup );                 // Простой сценарий идентификации по АСОУП
    CPPUNIT_TEST( UserDataWithPostPosition ); // Идентификация по АСОУП с постпозицией
    CPPUNIT_TEST( UserData_0 );            // Простой сценарий идентификации по TrainDescr 
    CPPUNIT_TEST( UserData_1 );
    CPPUNIT_TEST( UserData_2 );            // Идентификация по TrainDescr с ограничением по участку
    CPPUNIT_TEST( AsoupWithoutLayerEvent );
    // CPPUNIT_TEST( AsoupDisjoint );         // Идентификация по уже занятому АСОУП
    CPPUNIT_TEST( AsoupDoubleLink );       // Повторная идентификации нитки АСОУП сообщением
    CPPUNIT_TEST( GroupedAsoupLink );      // Идентификация по связанным событиям
    CPPUNIT_TEST( AsoupChainLinkAndUnlink );
    CPPUNIT_TEST( UserIdentifyNearEnd ); // идентификация около конца нити
    CPPUNIT_TEST( UserIdentifyOnBoardStation );
    CPPUNIT_TEST( UserIdentifyAndLinkFreeAsoup );
    CPPUNIT_TEST( UserIdentifyWithChangeOnlyNotes );
    CPPUNIT_TEST( UserGetPossibleIdentification );
    CPPUNIT_TEST( UserIdentifyByLinkedAsoup_6057 );
    CPPUNIT_TEST( ClearOnePersonMode );
    CPPUNIT_TEST( SetOnePersonMode );
    CPPUNIT_TEST( WrongLocomotiveAfterIdentify_6334 );
    CPPUNIT_TEST( EditLiterM_6357 );
    CPPUNIT_TEST( CheckRightOverstock_6500 );
    CPPUNIT_TEST( CheckRightOverstockOnEndBorderStation_6500 );
    CPPUNIT_TEST( CheckRightOverstockOnStartBorderStation_6500 );

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;

private:
    static const time_t T = 3600;
    std::shared_ptr< UtLayer<HappenLayer> > m_happenLayer;
    std::shared_ptr<AsoupLayer> m_asoupLayer;
    std::shared_ptr<Regulatory::Layer> m_regulatoryLayer;
    std::function<bool(const EsrKit&, const ParkWayKit&)> m_isPassengerWay;
    std::shared_ptr<EsrGuide> esrGuide;
    std::shared_ptr<TopologyTest> topology;
    std::shared_ptr<Hem::GuessTransciever> guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;
	Hem::AsoupQueueMgr m_asoupQMgr;
	std::set<EsrKit> vBorderEsr;


    void Asoup();
    void UserDataWithPostPosition();
    void UserData_0();
    void UserData_1();
    void UserData_2();
    void AsoupWithoutLayerEvent();
    void AsoupDisjoint();
    void AsoupDoubleLink();
    void GroupedAsoupLink();
    void AsoupChainLinkAndUnlink();
    void UserIdentifyNearEnd();
    void UserIdentifyOnBoardStation();
    void UserIdentifyAndLinkFreeAsoup();
    void UserIdentifyWithChangeOnlyNotes();
    void UserGetPossibleIdentification();
    void UserIdentifyByLinkedAsoup_6057();
    void ClearOnePersonMode();
    void SetOnePersonMode();
    void WrongLocomotiveAfterIdentify_6334();
    void EditLiterM_6357();
    void CheckRightOverstock_6500();
    void CheckRightOverstockOnEndBorderStation_6500();

    void FillDistrictGuideForRigamezgl_Zasulauks();
    void CheckRightOverstockOnStartBorderStation_6500();


    HappenLayer::EventWithDescrList GetPath_1();
    HappenLayer::EventWithDescrList GetPath_2();
};
