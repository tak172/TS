#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

class TC_Hem_aeRectify : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeRectify );
    CPPUNIT_TEST( RectifyWithMiss );
    CPPUNIT_TEST( RectifyDeathForm );
    CPPUNIT_TEST( RectifyTransitionBetweenSpanMove );
    CPPUNIT_TEST( Death_Form_Departure );
    CPPUNIT_TEST( Death_Form_atEnd );
    CPPUNIT_TEST( SpanMove_Entrastopping );
    CPPUNIT_TEST( SpanMove_Death_Form_StationExit );
    CPPUNIT_TEST( Arrival_Death_Form_Departure );
    CPPUNIT_TEST( TermDeath_FormSpan );
    CPPUNIT_TEST( MultyEntraStopping );
    CPPUNIT_TEST( WipeDeathForm );
    CPPUNIT_TEST( MisplacedDeath );
    CPPUNIT_TEST( DeathForm_HereAndThere );
    CPPUNIT_TEST( DeathForm_Instant );
    CPPUNIT_TEST( DeathForm_case9 );
    CPPUNIT_TEST( DeathForm_caseA );
    CPPUNIT_TEST( DeathForm_caseB );
    CPPUNIT_TEST( DeathForm_caseC );
    CPPUNIT_TEST( DeathForm_caseD );
    CPPUNIT_TEST( DeathForm_caseE );
    CPPUNIT_TEST( DeathForm_TermTerm );
    CPPUNIT_TEST( DeathForm_Span );
    CPPUNIT_TEST( DeathForm_SpanSpan );
    CPPUNIT_TEST( DeathForm_inside_SpanStopping );
    CPPUNIT_TEST( DeathForm_TermTransition );
    CPPUNIT_TEST( DeathForm_TermSpanWrongWay );
    CPPUNIT_TEST( DeathForm_IdentSpanMove );
    CPPUNIT_TEST( DeathForm_IdentSpanMove2 );
    CPPUNIT_TEST( DeathForm_IdentSpanMove3 );
    CPPUNIT_TEST( RoundTime );
    CPPUNIT_TEST( Move_Around_SpanStopping );
    CPPUNIT_TEST( WipePocketEntryExit );
    CPPUNIT_TEST( WipePocket_with_Stopping );
    CPPUNIT_TEST( WipePocketEntryDeath );
    CPPUNIT_TEST( RefineSpanstopping );
    CPPUNIT_TEST( RemoveSpanStoppingOnTheSamePlace );
    CPPUNIT_TEST( SpanStoppingBeginAndDeathForm );
    CPPUNIT_TEST( Arrival_Departure_Death );
    CPPUNIT_TEST( WrongReplace );

//    CPPUNIT_TEST( Check_Path );
	CPPUNIT_TEST_SUITE_END();

private:
    void tearDown() override;
	void RectifyWithMiss(); // операции Rectify для несуществующей нити/события
    void RectifyDeathForm(); // операции Rectify для удаления Death+Form
    void RectifyTransitionBetweenSpanMove(); // вставка проследования между движениями по разным перегонам
    void Death_Form_Departure(); //
    void Death_Form_atEnd();
    void SpanMove_Entrastopping();
    void SpanMove_Death_Form_StationExit();
    void Arrival_Death_Form_Departure();
    void TermDeath_FormSpan();
    void MultyEntraStopping(); // повторные стоянки перед входным на одном перегоне
    void WipeDeathForm(); // чистка пар Death/Form
    void MisplacedDeath(); // чистка пары Death/Form, когда Death не на месте
    void DeathForm_HereAndThere(); // чистка пары Death/Form, когда Death иначе не на месте
    void DeathForm_Instant(); // чистка Death/Form мгновенно и на одном месте
    void DeathForm_case9(); // чистка Death/Form - случай номер девять
    void DeathForm_caseA(); // чистка Death/Form - десятый необработанный случай
    void DeathForm_caseB(); // чистка Death/Form - десятый необработанный случай
    void DeathForm_caseC(); // чистка Death/Form + Departure
    void DeathForm_caseD(); // чистка Move/Death/Form причем Move на том же месте и времени что и Death
    void DeathForm_caseE(); // чистка множественных Death-Form при входе на станцию
    void DeathForm_TermTerm(); // чистка Death/Form - станции встык
    void DeathForm_Span();     // чистка Death/Form на перегоне
    void DeathForm_SpanSpan(); // чистка Death/Form на смежных перегонах
    void DeathForm_inside_SpanStopping(); // чистка Death/Form внутри стоянки на перегоне
    void DeathForm_TermTransition(); // чистка Death+Form на ПО-пути и генерация проследования по нему
    void DeathForm_TermSpanWrongWay(); // чистка Death/Form - станция, перегон+движение по неправильному пути
    void DeathForm_IdentSpanMove(); // чистка Death/Form - совпадение со следующим движением по перегону
    void DeathForm_IdentSpanMove2(); // чистка Death/Form - совпадение со следующим движением по перегону
    void DeathForm_IdentSpanMove3(); // чистка Death/Form - совпадение со следующим движением по перегону
    void RoundTime(); // ошибка при вычислении среднего времени между событиями
    void Move_Around_SpanStopping(); // чистка Span_move до и после стоянок на перегоне
    void WipePocketEntryExit(); // чистка прохода через карман
    void WipePocket_with_Stopping(); // чистка прохода через карман со стоянкой внутри
    void WipePocketEntryDeath(); // чистка входа в карман и исчезновения там
    void RefineSpanstopping(); // чистка стоянки на перегоне -удаление смещения из события начала #4198
    void RemoveSpanStoppingOnTheSamePlace(); //чистка стоянки на перегоне на одном и том же месте
    void SpanStoppingBeginAndDeathForm();   //корректная подмена Form Death после начала стоянки
    void Arrival_Departure_Death();
    void WrongReplace();
    void Check_Path();

    UtIdentifyCategoryList identifyCategory;
};
