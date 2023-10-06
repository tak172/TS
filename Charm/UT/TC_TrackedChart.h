#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_ChartImpl.h"
#include "UtHemHelper.h"
#include "UtTearDown.h"

class UtTrackedChart;

class TC_TrackedChart: 
    private TC_ChartImpl,
    public UtTearDowner
{
public:
    TC_TrackedChart();
    virtual ~TC_TrackedChart();

private:
    std::unique_ptr<UtTrackedChart> m_chart;

    CPPUNIT_TEST_SUITE( TC_TrackedChart );
    CPPUNIT_TEST( Serialization );
    CPPUNIT_TEST( AsoupIdentification_OnePass );
    CPPUNIT_TEST( AsoupIdentification_TwoPass );
    CPPUNIT_TEST( AsoupIdentificationWithIndexChange );
    CPPUNIT_TEST( RegulatoryIdentification );
    CPPUNIT_TEST( Changes );
    CPPUNIT_TEST( InfoChanging );
    CPPUNIT_TEST( AsoupWithdrawal );
    CPPUNIT_TEST( SafelyReject_Live );
    CPPUNIT_TEST( SafelyReject_Dead );
    CPPUNIT_TEST( SafelyReject_NotExist );
    CPPUNIT_TEST( SafelyReject_Obstacle );
    CPPUNIT_TEST( UncontrolledMiddle );
    CPPUNIT_TEST( UncontrolledDepartureWithHappenDeparture );
    CPPUNIT_TEST( UncontrolledWithGroupNumber );
    CPPUNIT_TEST( UncontrolledEdge );
    CPPUNIT_TEST( UncontrolledEdge_2 );
    CPPUNIT_TEST( MultipleAsoupUncontrolledArea );
    CPPUNIT_TEST( MultipleAsoupUncontrolledArea_4818_fwd );
    CPPUNIT_TEST( MultipleAsoupUncontrolledArea_4818_bkw );
    //CPPUNIT_TEST( DrawingUncontrolled_BeforeReachingAsoup ); //пока закрываю (очередь пока обрабатывает реальные события тут же при их появлении, не дожидаясь времени наступления событий)
    //CPPUNIT_TEST( DrawingUncontrolled_WithStepOverAsoup );
    CPPUNIT_TEST( IdentifyWrongPassagirArrivalMessage_5522 );
    CPPUNIT_TEST( IdentifyPassagirArrivalMessage_5522 );
    CPPUNIT_TEST( IdentifyForLittleStation_5482 );
    CPPUNIT_TEST( IdentifyByDepartureWithoutArrivalNextStation_4689 );
    CPPUNIT_TEST( IdentifyWithPassagirWithOtherNumber_5570 );
    CPPUNIT_TEST( IdentifyAndWrongAttachSameNumberedPathes_5570 );
    CPPUNIT_TEST( GlueTwoPathesWithEqualNumber_5511 );
    CPPUNIT_TEST( GlueTwoPathesWithEqualNumber_5587 );
    CPPUNIT_TEST( GlueTwoPathesWithEqualNumber_5590 );
    CPPUNIT_TEST( IdentifyByArrival_5621 );
    CPPUNIT_TEST( WrongGlueTwoPathesWithEqualNumber_5636 );
    CPPUNIT_TEST( AsoupWrongLinkInfoChanging_5652 );
    CPPUNIT_TEST( UncontrolledEdge_5689 );
    CPPUNIT_TEST( AttachSameNumberedPathes_5722 );
    CPPUNIT_TEST( NotCreatePathByAsoupUncontrolledArea );
    CPPUNIT_TEST( ExpanthFromSpanToStationByArrival );
    CPPUNIT_TEST( WrongChangeNumber_5597_1 );
    CPPUNIT_TEST( WrongChangeNumber_5597_2 );
    CPPUNIT_TEST( TwoAsoupOnTheSamePlace_4960 );
    CPPUNIT_TEST( AttachToDisformOnUcontrolledStation );
    CPPUNIT_TEST( AsoupGlueFromSpanToStation );
    CPPUNIT_TEST( IdentifyWithAsoupArrival );
    CPPUNIT_TEST( SecondCorrectAsoupOnUncontrolledStation );
    CPPUNIT_TEST( UncontrolledStationAfterDeathOnSpan_5792_1 );
    CPPUNIT_TEST( UncontrolledStationAfterDeathOnSpan_5792_2 );
    CPPUNIT_TEST( SaveCarrierInPassagirArrivalMessage_5797 );
    CPPUNIT_TEST( RefuseGuessArrivalWrongTime );
    CPPUNIT_TEST( IdentifyByIndex_5823 );
    CPPUNIT_TEST( IdentifyByIndexAndLinkFreeStrangeTimeAsoup_5823 );
    CPPUNIT_TEST( GlueFromUncontrolledStation );
    CPPUNIT_TEST( GlueOnControlledStation_5955 );
    CPPUNIT_TEST( IdentifyOnEndStation_KIG_784 );
    CPPUNIT_TEST( ErrorIdentificationOnSkirotava_6058 );
    CPPUNIT_TEST( AsoupOtherCategorie_KIG_1114 );
	CPPUNIT_TEST( AsoupDisform );
	CPPUNIT_TEST( RegulatoryIdentificationErr );

//     CPPUNIT_TEST( LoadHappenFromFileForStrangeError );


    CPPUNIT_TEST( DisformReserveLocoByAsoupArrival_5653 );
    CPPUNIT_TEST( DisformReserveLocoByAsoupArrival_5653_2 );
    CPPUNIT_TEST( IdentifyOnEqualArea_6044 );
    CPPUNIT_TEST( CreatePathRezevLocoByAsoupDeparture_6038 );
    CPPUNIT_TEST( ChangeNumberThroughEqualOrPaireNumber_6052 );
    CPPUNIT_TEST( ErrorIdentificationOnSkirotava_6058 );
    CPPUNIT_TEST( RemoveLocomotivesByAsoupWithdrawal_6052 );
    CPPUNIT_TEST( IdentifyByArrival_6182 );
    CPPUNIT_TEST( IdentifyWithWrongChangeNumber_6299 );
    CPPUNIT_TEST( AsoupNordWithdrawal );
	CPPUNIT_TEST( GuiError );
	CPPUNIT_TEST( AttachAsoupDisform );
	CPPUNIT_TEST( AsoupToAsoup_WithErrorGlueOnTheSameStation_6646 );
	CPPUNIT_TEST( AsoupAttemptWithIndexChangeOnFirstDeparture_6829 );
	CPPUNIT_TEST( GuiTrain );
	CPPUNIT_TEST( LinkFromBorderUncStatToControlZone );
	CPPUNIT_TEST(AutoGlueFromUncontrolToNearSpan);
// 	CPPUNIT_TEST( CheckAsoup );
	CPPUNIT_TEST( AsoupAttemptWithIndexChangeOnFirstDeparture_ReservedLoco );
	CPPUNIT_TEST( GlueAfterIdentify );
	CPPUNIT_TEST( IdentifyReserveLocoByEarlierAsoupDeparture );
	CPPUNIT_TEST( GlueOnFromBorderSpanToUncontrol );
	CPPUNIT_TEST( IdentifyControlStatWithChangeIndex );
	CPPUNIT_TEST( LinkFromBorderUncStatToControlZone_7081 );
	CPPUNIT_TEST( LinkFromBorderUncStatToControlZone_7081_1 );
	CPPUNIT_TEST( LinkFromBorderUncStatToControlZone_7081_2 );
	CPPUNIT_TEST( LinkFromBorderUncTehnodeStatToDeparture_7081_3 );

    CPPUNIT_TEST_SUITE_END();

private:
    void setUp() override;

    void Serialization();
    void AsoupIdentification_OnePass(); // идентификация с первого раза
    void AsoupIdentification_TwoPass(); // идентификация только со второго раза
    void AsoupIdentificationWithIndexChange();
    void RegulatoryIdentification();
    void Changes();
    void InfoChanging();
    void AsoupWithdrawal();
    void SafelyReject_Live(); // сброс нити из-за неприемлемого события (нить живая)
    void SafelyReject_Dead(); // сброс нити из-за неприемлемого события (нить уже закрыта)
    void SafelyReject_NotExist(); // сброс нити из-за неприемлемого события (нить не существует)
    void SafelyReject_Obstacle(); // сброс нити из-за неприемлемого события (закрытию мешает другая нить)
    // достраивание до АСОУП
    void UncontrolledMiddle(); // неконтролируемая середина, края под контролем
    void UncontrolledDepartureWithHappenDeparture();   //Отправление на неконтролируемой и  отправление на контролируемой (пропущено прибытие на станцию, склейи не будет)
    void UncontrolledWithGroupNumber();
    void UncontrolledEdge();   // край неконтролируем, а середина под контролем
    void UncontrolledEdge_2();   // край неконтролируем, а середина под контролем (объединение в узел)
    void MultipleAsoupUncontrolledArea(); // несколько сообщений в неконтролируемой середине
    void MultipleAsoupUncontrolledArea_4818_fwd();  // несколько сообщений в неконтролируемой середине и даже на одной станции ARRIVAL+DEPARTURE
    void MultipleAsoupUncontrolledArea_4818_bkw(); // несколько сообщений в неконтролируемой середине и даже на одной станции ARRIVAL+DEPARTURE
    void DrawingUncontrolled_BeforeReachingAsoup(); // нарисовано в неконтролируемой середине не доходя до АСОУП
    void DrawingUncontrolled_WithStepOverAsoup(); // нарисовано в неконтролируемой середине с переходом за АСОУП
    void TestArrival(); //имитация прибытия
    void TestDisform(); //Form-Disform 
    void IdentifyWrongPassagirArrivalMessage_5522();
    void IdentifyPassagirArrivalMessage_5522();
    void IdentifyForLittleStation_5482();
    void IdentifyByDepartureWithoutArrivalNextStation_4689();
    void IdentifyWithPassagirWithOtherNumber_5570();
    void IdentifyAndWrongAttachSameNumberedPathes_5570();
    void GlueTwoPathesWithEqualNumber_5511();
    void GlueTwoPathesWithEqualNumber_5587();
    void GlueTwoPathesWithEqualNumber_5590();
    void IdentifyByArrival_5621();
    void WrongGlueTwoPathesWithEqualNumber_5636();
    void AsoupWrongLinkInfoChanging_5652();
    void UncontrolledEdge_5689();
    void DisformReserveLocoByAsoupArrival_5653();
    void DisformReserveLocoByAsoupArrival_5653_2();
    void AttachSameNumberedPathes_5722();
    void NotCreatePathByAsoupUncontrolledArea();
    void ExpanthFromSpanToStationByArrival();
    void WrongChangeNumber_5597_1();
    void WrongChangeNumber_5597_2();
    void TwoAsoupOnTheSamePlace_4960();
    void AttachToDisformOnUcontrolledStation();
    void AsoupGlueFromSpanToStation();
    void IdentifyWithAsoupArrival();
    void SecondCorrectAsoupOnUncontrolledStation();
    void UncontrolledStationAfterDeathOnSpan_5792_1();
    void UncontrolledStationAfterDeathOnSpan_5792_2();
    void SaveCarrierInPassagirArrivalMessage_5797();
    void IdentifyByIndex_5823();
    void IdentifyByIndexAndLinkFreeStrangeTimeAsoup_5823();
    void GlueFromUncontrolledStation();
    void IdentifyOnEndStation_KIG_784();
    void AsoupOtherCategorie_KIG_1114();

    void Uncontrolled_helper(); 
    void NsiBasis_helper();
    void setupEsrGuide();
    void setupTopology();
    void RefuseGuessArrivalWrongTime();
    void GlueOnControlledStation_5955();
    void IdentifyOnEqualArea_6044();
    void CreatePathRezevLocoByAsoupDeparture_6038();
    void ChangeNumberThroughEqualOrPaireNumber_6052();
    void ErrorIdentificationOnSkirotava_6058();
    void RemoveLocomotivesByAsoupWithdrawal_6052();
    void IdentifyByArrival_6182();
    void IdentifyWithWrongChangeNumber_6299();
    void AsoupNordWithdrawal();
	void AsoupDisform();
	void RegulatoryIdentificationErr();
	void AttachAsoupDisform();
	void GuiError();
	void AsoupToAsoup_WithErrorGlueOnTheSameStation_6646();
	void AsoupAttemptWithIndexChangeOnFirstDeparture_6829();
	bool CreatePathWithMsgBorderQueue(const std::wstring& text, const unsigned trainId);
	void GuiTrain();
	void CheckAsoup();
	void LinkFromBorderUncStatToControlZone();
	void AsoupAttemptWithIndexChangeOnFirstDeparture_ReservedLoco();
	void AutoGlueFromUncontrolToNearSpan();
	void GlueAfterIdentify();
	void IdentifyReserveLocoByEarlierAsoupDeparture();
	void GlueOnFromBorderSpanToUncontrol();
	void IdentifyControlStatWithChangeIndex();
	void LinkFromBorderUncStatToControlZone_7081();
	void LinkFromBorderUncStatToControlZone_7081_1();
	void LinkFromBorderUncStatToControlZone_7081_2();
	void LinkFromBorderUncTehnodeStatToDeparture_7081_3();

//     void LoadHappenFromFileForStrangeError();

    UtIdentifyCategoryList identifyCategory;

};
