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
	void RectifyWithMiss(); // �������� Rectify ��� �������������� ����/�������
    void RectifyDeathForm(); // �������� Rectify ��� �������� Death+Form
    void RectifyTransitionBetweenSpanMove(); // ������� ������������� ����� ���������� �� ������ ���������
    void Death_Form_Departure(); //
    void Death_Form_atEnd();
    void SpanMove_Entrastopping();
    void SpanMove_Death_Form_StationExit();
    void Arrival_Death_Form_Departure();
    void TermDeath_FormSpan();
    void MultyEntraStopping(); // ��������� ������� ����� ������� �� ����� ��������
    void WipeDeathForm(); // ������ ��� Death/Form
    void MisplacedDeath(); // ������ ���� Death/Form, ����� Death �� �� �����
    void DeathForm_HereAndThere(); // ������ ���� Death/Form, ����� Death ����� �� �� �����
    void DeathForm_Instant(); // ������ Death/Form ��������� � �� ����� �����
    void DeathForm_case9(); // ������ Death/Form - ������ ����� ������
    void DeathForm_caseA(); // ������ Death/Form - ������� �������������� ������
    void DeathForm_caseB(); // ������ Death/Form - ������� �������������� ������
    void DeathForm_caseC(); // ������ Death/Form + Departure
    void DeathForm_caseD(); // ������ Move/Death/Form ������ Move �� ��� �� ����� � ������� ��� � Death
    void DeathForm_caseE(); // ������ ������������� Death-Form ��� ����� �� �������
    void DeathForm_TermTerm(); // ������ Death/Form - ������� �����
    void DeathForm_Span();     // ������ Death/Form �� ��������
    void DeathForm_SpanSpan(); // ������ Death/Form �� ������� ���������
    void DeathForm_inside_SpanStopping(); // ������ Death/Form ������ ������� �� ��������
    void DeathForm_TermTransition(); // ������ Death+Form �� ��-���� � ��������� ������������� �� ����
    void DeathForm_TermSpanWrongWay(); // ������ Death/Form - �������, �������+�������� �� ������������� ����
    void DeathForm_IdentSpanMove(); // ������ Death/Form - ���������� �� ��������� ��������� �� ��������
    void DeathForm_IdentSpanMove2(); // ������ Death/Form - ���������� �� ��������� ��������� �� ��������
    void DeathForm_IdentSpanMove3(); // ������ Death/Form - ���������� �� ��������� ��������� �� ��������
    void RoundTime(); // ������ ��� ���������� �������� ������� ����� ���������
    void Move_Around_SpanStopping(); // ������ Span_move �� � ����� ������� �� ��������
    void WipePocketEntryExit(); // ������ ������� ����� ������
    void WipePocket_with_Stopping(); // ������ ������� ����� ������ �� �������� ������
    void WipePocketEntryDeath(); // ������ ����� � ������ � ������������ ���
    void RefineSpanstopping(); // ������ ������� �� �������� -�������� �������� �� ������� ������ #4198
    void RemoveSpanStoppingOnTheSamePlace(); //������ ������� �� �������� �� ����� � ��� �� �����
    void SpanStoppingBeginAndDeathForm();   //���������� ������� Form Death ����� ������ �������
    void Arrival_Departure_Death();
    void WrongReplace();
    void Check_Path();

    UtIdentifyCategoryList identifyCategory;
};
