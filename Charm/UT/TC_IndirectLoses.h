#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"
#include "LogicDocumentTest.h"

class IndirectLosesNotifier;
class Lexicon;
class LOS_base;
class TrainPlacesReview;
class TC_IndirectLoses : 
	public TC_Project
{

  CPPUNIT_TEST_SUITE( TC_IndirectLoses );
  CPPUNIT_TEST( SingleEntryOnStation );
  CPPUNIT_TEST( DoubleEntryOnStation );
  CPPUNIT_TEST( MultiEntryOnSpan );
  CPPUNIT_TEST( MultiEntryEverywhere );
  CPPUNIT_TEST( ReEntryDoesNotChange );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;

protected:
	void SingleEntryOnStation(); //��������� ������ �� ����� �������� ����� (�� �������)
	void DoubleEntryOnStation(); //��������� ������ �� ����� �������� ����� (�� �������)
	void MultiEntryOnSpan(); //��������� ������ �� ����� (���������) ������� ��������� ����� (�� ��������)
	void MultiEntryEverywhere(); //��������� ������ �� ���� ��������� ������ ���� �������� ����� (������� � ��������)
    void ReEntryDoesNotChange(); //��������� ��������� ������� �� ������� ���������

private:
	void MakeObjectsBasis();
	void MakeStationObjects();
	void MakeSpanObjects();
	void TestPresence( LOS_base * losBoard, unsigned int ) const;
	void TestAbsence( LOS_base * losBoard, unsigned int ) const;
	void TrainsPlacingNotify( attic::a_node trainsNode, const time_t * = nullptr );
	std::unique_ptr <IndirectLosesNotifier> indirLosesNotifier;
	std::unique_ptr <Lexicon> lexiconPtr;
	std::unique_ptr <TrainPlacesReview> tplcReviewer;
	CLogicDocumentTest spanDoc;
	CLogicElement * statStrip1, * statStrip2;
	std::vector <CLogicElement *> spanStrips;
	CLogicElement * statBoard1, * statBoard2, * spanBoard;
};