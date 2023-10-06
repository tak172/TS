#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/HemHelpful.h"
#include "../Hem/UncontrolledStations.h"
#include "../Hem/AsoupProcessingQueue.h"
#include "UtHemHelper.h"
#include "UtTearDown.h"

class TopologyTest;
class HappenLayer;
class EsrGuide;

namespace Hem
{
    class GuessTransciever;
}

class TC_Hem_aeAttach : public UtTearDowner
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeAttach );
    CPPUNIT_TEST( Normal_SimpleTopology );
    CPPUNIT_TEST( Normal_NoneTopology );
    CPPUNIT_TEST( Normal_and_Undo );
    CPPUNIT_TEST( AttachExistToNew );
    CPPUNIT_TEST( AttachExistToNew2 );
    CPPUNIT_TEST( AttachNewDepartureToExist );
    CPPUNIT_TEST( AttachNewArrivalToExist );
    CPPUNIT_TEST( AttachNewArrivalToExist_SomeWay );
    CPPUNIT_TEST( AttachNewToExist2 );
    CPPUNIT_TEST( AttachNewArrivalToNewDeparture );
    CPPUNIT_TEST( AttachNewMoveToNewMove_SomeSpan );
    CPPUNIT_TEST( AttachNewMoveToNewMove_Over1Terminal );
    CPPUNIT_TEST( AttachNewMoveToNewArrival_nearSpan );
    CPPUNIT_TEST( Death_Form );
    CPPUNIT_TEST( Overlap_Time_SpanMove );
    CPPUNIT_TEST( Transition_EntraStopping );
    CPPUNIT_TEST( FormAndStationexit );
    CPPUNIT_TEST( MoveDeathAndSome );
    CPPUNIT_TEST( SpanMoveAndSome );
    CPPUNIT_TEST( DisformAndSome );
    CPPUNIT_TEST( SentryDeath_Form );
    CPPUNIT_TEST( DepartureDeath_Form );
    CPPUNIT_TEST( TransitionDeath_More );
    CPPUNIT_TEST( TransitionDeath_More2 );
    CPPUNIT_TEST( Disform_FormStationexit );
    CPPUNIT_TEST( Some_FormArrivalOrTransition );
    CPPUNIT_TEST( EntraStopping_Some );
    CPPUNIT_TEST( Some_removableTerminalForm );
    CPPUNIT_TEST( Entry_FormExit );
    CPPUNIT_TEST( ArrDep_Form_MatchWay );
    CPPUNIT_TEST( Sentry_ToNew );
    CPPUNIT_TEST( SentrySexit_balance );
    CPPUNIT_TEST( Some_ToLive );
    CPPUNIT_TEST( Exist_to_IncompleteExist );
    CPPUNIT_TEST( GenerationNote_EntranceStopping );
    CPPUNIT_TEST( WrongwaySpanmoveDeath_To_Arrival );
    CPPUNIT_TEST( WrongwayDeath_To_Arrival );
    CPPUNIT_TEST( SpanStop_continue );
    CPPUNIT_TEST( SpanStop_continue2 );
    CPPUNIT_TEST( SpanStop_interval3 );
    CPPUNIT_TEST( PocketDeath_Form );
    CPPUNIT_TEST( DeathForm_onSpan );
    CPPUNIT_TEST( Wrong_and_Span_stopping );
    CPPUNIT_TEST( ManualExtendFront ); // ручное продление нити "вперёд" дает проследование
    CPPUNIT_TEST( ManualExtendBack ); // ручное продление нити "назад" дает проследование
    CPPUNIT_TEST( TimeFromDistance );
    CPPUNIT_TEST( GenerateTransitionWay );
    CPPUNIT_TEST( ArrivalTermDeathSpan_Extension );
    CPPUNIT_TEST( MoveDeathAndFormDepartureSamePoint );
    CPPUNIT_TEST( ArrivalTermDeathArrivalOtherWay_Extension );
    CPPUNIT_TEST( PushFrontWithSaveNumber );
    CPPUNIT_TEST( PushFrontWithSaveNumberOnTheSamePlace );
    CPPUNIT_TEST( AttachExistDeathAndFormOnTheSameStation );
    CPPUNIT_TEST( AttachSpanMoveWrongwayEvents );
    CPPUNIT_TEST ( AttachEntraStoppingToSpanMoveOnTheSamePlace );
    CPPUNIT_TEST ( Attach_dropDisform_4871 );
    CPPUNIT_TEST( Attach_form_4875 );
    CPPUNIT_TEST( Attach_disform_nowayform_spanmove );
    CPPUNIT_TEST( DeathSpanstoppingBeginAndClosedPathOnOneStation );
    CPPUNIT_TEST( Jelgava2Jelgava );
    CPPUNIT_TEST( PushFrontToPathWithLimit );
    CPPUNIT_TEST( AttachAsoupDepartureToFormSpan );
    CPPUNIT_TEST( AttachAsoupDepartureToPath );
    CPPUNIT_TEST( AsoupToAsoupDifferentCategories );
    CPPUNIT_TEST( Attach_DeathWithStationExit_5124 );
    CPPUNIT_TEST( Attach_DeathWithAsopupDisform_4807 );
    CPPUNIT_TEST( ArDep_ArDep_MatchWay );
    CPPUNIT_TEST( DisformToForm );
    CPPUNIT_TEST( AttachWithFormOnStation_5030 );
    CPPUNIT_TEST( AttachAsoupDepartureToPathOnBorderStation_5596 );
    CPPUNIT_TEST( ExtendCarrierCode );
    CPPUNIT_TEST( AttachWithPocketExit );
    CPPUNIT_TEST( AttachAsoupToAsoup );
    CPPUNIT_TEST( AsoupToAsoup_LinkNotBorderEvent );
    CPPUNIT_TEST( ContinuePathAndRelinkAsoup );
    CPPUNIT_TEST( AttachHapenToOwnLaterAsoup );
    CPPUNIT_TEST( AttachHapenToFreeAsoup );
    CPPUNIT_TEST( AttachAsoupToAsoup_2 );
    CPPUNIT_TEST( AttachAsoupDepartureToOwnLinkedPath_5849 );
    CPPUNIT_TEST( AttachHappenToAsoup );
    CPPUNIT_TEST ( FormArrivalDisformToForm_5947 );
    CPPUNIT_TEST ( SpanStop_OnTheSamePlace_3889 );
    CPPUNIT_TEST( SpanStopEntraStop_OnTheSamePlace_6155 );
    CPPUNIT_TEST( SaveDriver_6328 );
	CPPUNIT_TEST( AsoupToAsoup_WithThrow_6646 );
	CPPUNIT_TEST( AttachDiffNumbers );
	CPPUNIT_TEST( FindAndGluePathesByNumber_7077_1 );
	CPPUNIT_TEST( FindAndGluePathesByNumber_7077_2 );
	CPPUNIT_TEST( GlueExplicitFormWithSpanMove_7081_3 );
	CPPUNIT_TEST( GlueExplicitFormWithSpanMove_7081_4 );
    CPPUNIT_TEST( Attach_ManualExtentBack_7213 );
	
	CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void setTopologyPath(unsigned from, unsigned to, std::vector<unsigned> path = std::vector<unsigned>(), unsigned third = 1379u);

private:
    void Normal_SimpleTopology(); // корректная склейка при простой топологии
	void Normal_NoneTopology();   // корректная склейка без топологии
    void Normal_and_Undo();       // склейка и откат
    void AttachExistToNew();      // склейка существующей нити с новым событием
    void AttachExistToNew2();      // склейка существующей нити с новым событием
    void AttachNewDepartureToExist();    // склейка нового события DEPARTURE с существующей нитью
    void AttachNewArrivalToExist();      // склейка нового события ARRIVAL с существующей нитью
    void AttachNewArrivalToExist_SomeWay(); // склейка нового события ARRIVAL с существующей нитью на том же пути
    void AttachNewToExist2();      // склейка нового события с нитью по первому значимому событию
    void AttachNewArrivalToNewDeparture();      // склейка нового события ARRIVAL с новым DEPARTURE
    void AttachNewMoveToNewMove_SomeSpan();      // создать нить из двух SpanMove (на одном перегоне)
    void AttachNewMoveToNewMove_Over1Terminal(); // создать нить из двух SpanMove (через 1 станцию)
    void AttachNewMoveToNewArrival_nearSpan();
    void Death_Form();
    void Overlap_Time_SpanMove();
    void Transition_EntraStopping(); // склейка событий Transition-EntraStopping
    void FormAndStationexit();
    void MoveDeathAndSome();
    void SpanMoveAndSome();
    void DisformAndSome();
    void SentryDeath_Form();
    void DepartureDeath_Form();
    void TransitionDeath_More();
    void TransitionDeath_More2();
    void Disform_FormStationexit();
    void Some_FormArrivalOrTransition();
    void EntraStopping_Some();
    void Some_removableTerminalForm();
    void Entry_FormExit();
    void ArrDep_Form_MatchWay();
    void Sentry_ToNew();
    void SentrySexit_balance();
    void Some_ToLive();
    void Exist_to_IncompleteExist();
    void GenerationNote_EntranceStopping();
    void WrongwaySpanmoveDeath_To_Arrival();
    void WrongwayDeath_To_Arrival();
    void SpanStop_continue();
    void SpanStop_continue2();
    void SpanStop_interval3();
    void PocketDeath_Form();
    void DeathForm_onSpan(); //  некорректная нить с Death+Form на перегоне
    void Wrong_and_Span_stopping(); // 
    void ManualExtendFront(); // ручное продление нити вперёд дает проследование
    void ManualExtendBack(); // ручное продление нити назад дает проследование
    void TimeFromDistance(); // время событий пропорционально растоянию между станциями
    void GenerateTransitionWay(); // ПО-путь при генерации промежуточных событий идентифицированного поезда
    void ArrivalTermDeathSpan_Extension(); // Ошибка проявилась на версии 955
    void MoveDeathAndFormDepartureSamePoint();  //Form+Departure на одном и том же месте в одно и то же время меняем на проследование
    void ArrivalTermDeathArrivalOtherWay_Extension();
    void PushFrontWithSaveNumber();     //ручное продление идентифицированной нити назад сохраняет номер нити.
    void PushFrontWithSaveNumberOnTheSamePlace();
    void AttachExistDeathAndFormOnTheSameStation();
    void AttachSpanMoveWrongwayEvents();
    void AttachEntraStoppingToSpanMoveOnTheSamePlace();
    void Attach_dropDisform_4871();
    void Attach_form_4875();
    void Attach_disform_nowayform_spanmove();
    void DeathSpanstoppingBeginAndClosedPathOnOneStation();
    void Jelgava2Jelgava(); // [bug 553] - не склеиваются нити Elgava2-Elgava ( Form[elg2], Arriv[elg] )
    void PushFrontToPathWithLimit();
    void AttachAsoupDepartureToFormSpan();
    void AttachAsoupDepartureToPath();
    void AsoupToAsoupDifferentCategories();
    void Attach_DeathWithStationExit_5124();
    void Attach_DeathWithAsopupDisform_4807();
    void ArDep_ArDep_MatchWay();
    void DisformToForm();
    void AttachWithFormOnStation_5030();
    void AttachAsoupDepartureToPathOnBorderStation_5596();
    void CheckError();
    void ExtendCarrierCode();
    void AttachWithPocketExit();
    void AttachAsoupToAsoup();
    void AsoupToAsoup_LinkNotBorderEvent();
    void ContinuePathAndRelinkAsoup();
    void AttachHapenToOwnLaterAsoup();
    void AttachHapenToFreeAsoup();
    void AttachAsoupToAsoup_2();
    void AttachAsoupDepartureToOwnLinkedPath_5849();
    void AttachHappenToAsoup();
    void FormArrivalDisformToForm_5947();
    void SpanStop_OnTheSamePlace_3889();
    void SpanStopEntraStop_OnTheSamePlace_6155();
    void SaveDriver_6328();
	void AsoupToAsoup_WithThrow_6646();
	void AttachDiffNumbers();
	void FindAndGluePathesByNumber_7077_1();
	void FindAndGluePathesByNumber_7077_2();
	void GlueExplicitFormWithSpanMove_7081_3();
	void GlueExplicitFormWithSpanMove_7081_4();
    void Attach_ManualExtentBack_7213();

    void setTerminalCoord( int esr, const std::string& km );
    void collectUndo();
    bool implementUndo();

    static const time_t T=3600*(240+23);
    std::shared_ptr< UtLayer<HappenLayer> > happen_layer;
    std::shared_ptr< UtLayer<AsoupLayer> > asoup_layer;
    std::shared_ptr<EsrGuide> esrGuide;
    std::shared_ptr<TopologyTest> topology;
    std::shared_ptr<Hem::GuessTransciever> guessTransciever;
    UncontrolledStations uncontrolledStations;
	Hem::AsoupQueueMgr asoupQueue;
    attic::a_document undoer;
    UtIdentifyCategoryList identifyCategory;
};
