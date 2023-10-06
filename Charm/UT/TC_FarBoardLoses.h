#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"
#include "LogicDocumentTest.h"
#include "../helpful/Oddness.h"

class IndirectLosesNotifier;
class Lexicon;
class TrainDescr;
class TrainPlacesReview;
class RoutesChangesDetector;
class TC_FarBoardLoses : 
	public TC_Project
{

	CPPUNIT_TEST_SUITE( TC_FarBoardLoses );
	CPPUNIT_TEST( SHO_SpanToStatByRouteOddNumOddSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteOddNumEvenSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteOddNumUndefSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteEvenNumOddSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteEvenNumEvenSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteEvenNumUndefSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripOddNumOddSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripOddNumEvenSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripOddNumUndefSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripEvenNumOddSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripEvenNumEvenSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripEvenNumUndefSema );
	CPPUNIT_TEST( SHO_StatToSpanByRoute );
	CPPUNIT_TEST( SHO_StatToSpanByStrip );
	CPPUNIT_TEST( SHO_StatToSpanErrorCase );
	CPPUNIT_TEST( LEN_SpanToStatByRoute );
	CPPUNIT_TEST( LEN_SpanToStatByStrip );
	CPPUNIT_TEST( LEN_IntoStatByRoute );
	CPPUNIT_TEST( LEN_IntoStatByStrip );
	CPPUNIT_TEST( LEN_StatToSpanByRoute );
	CPPUNIT_TEST( LEN_StatToSpanByStrip );
	CPPUNIT_TEST( BP_AmbivalentByRoute );
	CPPUNIT_TEST( BP_AmbivalentByStrip );
	CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp();

private:
	void MakeShortSetObjects();
	void MakeShortSetStationObjects();
	void MakeLengthySetObjects();
	void MakeLengthySetStatObjects();
	void MakeBPostSetObjects();
	CLogicElement * MakeSpanObjects( CLogicDocumentTest *, std::vector <CLogicElement *> & lstrips ); //��������� - ������ ��������� �����
	void MakeStripLine( CLogicDocumentTest *, std::vector <CLogicElement *> & lstrips ); //��������� ������� �������� � �� ����������
	void InitLexicon();

	void SHO_SpanToStatByRouteOddNumOddSema(); //������� �������. ������� �� ������� �������� � �������� �� ������� ��� ���������� ������ �� ��������. �������� �����. �������� ��������
	void SHO_SpanToStatByRouteOddNumEvenSema(); //������� �������. ������� �� ������� �������� � �������� �� ������� ��� ���������� ������ �� ��������. �������� �����. ������ ��������
	void SHO_SpanToStatByRouteOddNumUndefSema(); //������� �������. ������� �� ������� �������� � �������� �� ������� ��� ���������� ������ �� ��������. �������� �����. �������������� ��������
	void SHO_SpanToStatByRouteEvenNumOddSema(); //������� �������. ������� �� ������� �������� � �������� �� ������� ��� ���������� ������ �� ��������. ������ �����. �������� ��������
	void SHO_SpanToStatByRouteEvenNumEvenSema(); //������� �������. ������� �� ������� �������� � �������� �� ������� ��� ���������� ������ �� ��������. ������ �����. ������ ��������
	void SHO_SpanToStatByRouteEvenNumUndefSema(); //������� �������. ������� �� ������� �������� � �������� �� ������� ��� ���������� ������ �� ��������. ������ �����. �������������� ��������
	void SHO_SpanToStatByRoute( bool numIsOdd, Oddness semaOddness, bool permitted /*�������� �� ������� ������*/ );

	void SHO_SpanToStatByStripOddNumOddSema(); //������� �������. ������� �� ��������� ������ �� �������� ��� �������� �������� � �������� �� �������. �������� �����. �������� ��������
	void SHO_SpanToStatByStripOddNumEvenSema(); //������� �������. ������� �� ��������� ������ �� �������� ��� �������� �������� � �������� �� �������. �������� �����. ������ ��������
	void SHO_SpanToStatByStripOddNumUndefSema(); //������� �������. ������� �� ��������� ������ �� �������� ��� �������� �������� � �������� �� �������. �������� �����. �������������� ��������
	void SHO_SpanToStatByStripEvenNumOddSema(); //������� �������. ������� �� ��������� ������ �� �������� ��� �������� �������� � �������� �� �������. ������ �����. �������� ��������
	void SHO_SpanToStatByStripEvenNumEvenSema(); //������� �������. ������� �� ��������� ������ �� �������� ��� �������� �������� � �������� �� �������. ������ �����. ������ ��������
	void SHO_SpanToStatByStripEvenNumUndefSema(); //������� �������. ������� �� ��������� ������ �� �������� ��� �������� �������� � �������� �� �������. ������ �����. �������������� ��������
	void SHO_SpanToStatByStrip( bool numIsOdd, Oddness semaOddness, bool permitted /*�������� �� ������� ������*/ );

	void SHO_StatToSpanByRoute(); //������� �������. ������� �� ������� �������� �� ������� �� ������� ��� ���������� ������ ����� ����������
	void SHO_StatToSpanByStrip(); //������� �������. ������� �� ��������� ������ ����� ���������� ��� �������� �������� �� ������� �� �������
	void SHO_StatToSpanErrorCase(); //������� �������. ���������� ������ ��� ������� �������� �� ������� �� �������

	void LEN_SpanToStatByRoute(); //��������� �������. ������� �� ������� �������� � �������� �� ������� ��� ���������� ������ �� ��������
	void LEN_SpanToStatByStrip(); //��������� �������. ������� �� ��������� ������ �� �������� ��� �������� �������� � �������� �� �������
	void LEN_IntoStatByRoute(); //��������� �������. ������� �� ������� �������� � ������ ����� ������� ��� ���������� ������ ����� ����������
	void LEN_IntoStatByStrip(); //��������� �������. ������� �� ��������� ������ ����� ���������� ��� �������� �������� � �������� �� �������
	void LEN_StatToSpanByRoute(); //��������� �������. ������� �� ������� �������� �� ������� �� ������� ��� ���������� ������ ����� ����������
	void LEN_StatToSpanByStrip(); //��������� �������. ������� �� ��������� ������ ����� ���������� ��� �������� �������� �� ������� �� �������

	void BP_AmbivalentByRoute(); //������� ������ ����-����, ������� ������ ������������������ ��� ������� ������ � ����������� ������������. ������� �� ������� ��������
	void BP_AmbivalentByStrip(); //������� ������ ����-����, ������� ������ ������������������ ��� ������� ������ � ����������� ������������. ������� �� ��������� ������

	void SetRouteByLeg( CLogicElement * lleg );
	TrainDescr SetTrainOnStrip( CLogicElement * lstrip, bool oddNumber = true /*�������� ������ ������*/ ); //��������� ������ �� �������
	void CleanTrainOnStrip( bool only_number, CLogicElement * lstrip ); //����� ������ ������ �� ��������� ������� �������� (only_number = ��������� ��������)
	void TrainsPlacingNotify( attic::a_node trainsNode );

	std::unique_ptr <IndirectLosesNotifier> indirLosesNotifier;
	std::unique_ptr <Lexicon> lexiconPtr;
	std::unique_ptr <TrainPlacesReview> tplcReviewer;

	RoutesChangesDetector MakeRouteChangesDetector( const std::set<const CLogicElement *> & lroutes ) const;

	//������ ��� ��������� �������� (�� ���������) ���������
	struct ShortSet
	{
		//����� ��������, ����������� ������� � ����������� � ��� ������� (��� ��������� ������� ������������� ���������)
		CLogicDocumentTest spanDoc, anotherStatDoc;
		CLogicElement * statBoard, * spanBoard;
		std::vector <CLogicElement *> statStrips;
		std::vector <CLogicElement *> spanStrips;
		CLogicElement * statLegFromSpanToStat; //����������� �������� ������������ �� �������� head
		CLogicElement * bndLegFromSpanToStat; //��������� �������� ������������ �� �������� head3
		CLogicElement * statLegFromStatToSpan; //����������� �������� ������������ �� ������� head2
	} sset;

	//������ ��� ��������� ������� (���������) ���������
	struct LengthySet
	{
		CLogicDocumentTest lspanDoc, rspanDoc;
		CLogicElement * lspanBoard, * rspanBoard;
		std::vector <CLogicElement *> statBoards;
		std::vector <CLogicElement *> semalegs;
		std::vector <CLogicElement *> lspanStrips, statStrips, rspanStrips;
	} lset;

	//������ ��� ��������� �������� ����� ����-����
	struct BPostSet
	{
		CLogicDocumentTest lspanDoc, bpostDoc, rspanDoc;
		CLogicElement * rspanBoard;
		CLogicElement * semaleg;
		std::vector <CLogicElement *> lspanStrips, statStrips, rspanStrips;
	} bpset;
};