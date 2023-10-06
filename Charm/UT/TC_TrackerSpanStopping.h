#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ��������� ������� ������� �� ��������

class TC_TrackerSpanStopping : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerSpanStopping );
	CPPUNIT_TEST( CasualCaseWithLengthWithPicketage );
	CPPUNIT_TEST( CasualCaseWithLengthWOPicketage );
	CPPUNIT_TEST( CasualCaseWOLengthWithPicketage );
	CPPUNIT_TEST( CasualCaseWOLengthWOPicketage );
	CPPUNIT_TEST( SpanStopAfterReborn );
	CPPUNIT_TEST( SpanStopAfterReborn2 );
	CPPUNIT_TEST( SpanStopAfterWideReborn );
	CPPUNIT_TEST( NonSpanStopAfterWideReborn );
	CPPUNIT_TEST( NonSpanStopAfterRebornNear );
	CPPUNIT_TEST( SpanStopAfterRebornAndWaiting );
	CPPUNIT_TEST( NonSpanStopAfterRelight );
	CPPUNIT_TEST( NonSpanStopAfterRelightAndWaiting );
	CPPUNIT_TEST( ParallelActivity );
	CPPUNIT_TEST( SpanStoppingAndPocketEntry );
	CPPUNIT_TEST( SpanStoppingIntoPocket );
	CPPUNIT_TEST( SeveralStops );
	CPPUNIT_TEST( StopBeforeStation );
	CPPUNIT_TEST( SpanStoppingEarlierThanEntraStopping );
	CPPUNIT_TEST( EntraStoppingEarlierThanSpanStopping );
	CPPUNIT_TEST( EntraStoppingEarlierThanSpanStopping2 );
	CPPUNIT_TEST( NoTrainSpanStopping );
	CPPUNIT_TEST_SUITE_END();

	void CasualCaseWithLengthWithPicketage(); //������� �� ���������� ������� � ��������� ������ � ���������� ������
	void CasualCaseWithLengthWOPicketage(); //������� �� ���������� ������� � ��������� ������, �� ��� ��������� ������
	void CasualCaseWOLengthWithPicketage(); //������� �� ���������� ������� � ����������� ������, �� � ���������� ������
	void CasualCaseWOLengthWOPicketage(); //������� �� ���������� ������� � ����������� ������ � �� ��� ��������� ������
	void SpanStopAfterReborn();	//������� �� �������� �� ������� ������������ ������ � ��� �� ����� ����� ����������
	void SpanStopAfterReborn2();
	void SpanStopAfterWideReborn(); //������� ��� ������������ �� ����� � �������������� �� ���������� (�� ����) ��������
	void NonSpanStopAfterWideReborn(); //��� ������� ��� ������������ �� ����� � �������������� �� ���������� (�� ����) ��������
	void NonSpanStopAfterRebornNear(); //��� ������� �� �������� �� ������� ������������ ������ �� ��������� ����� ����� ����������
	void SpanStopAfterRebornAndWaiting(); //������� �� �������� �� ��������� ���������� �������� ����� ������������
	void NonSpanStopAfterRelight(); //���������� ������� �� �������� ����� ������������� ������� ��-�� ���������������� ������, � ��������� ������
	void NonSpanStopAfterRelightAndWaiting(); //���������� ������� �� �������� ����� ������������� ������� � �������� ��-�� ���������������� ������, � ��������� ������
	void ParallelActivity(); //������������ ���������� �� ������ ���������
	void SpanStoppingAndPocketEntry(); //������� �� �������� � ���� � ������
	void SpanStoppingIntoPocket(); //���� � ������ � ������� �� �������� ������ �������
	void SeveralStops(); //��������� ������� �� ��������
	void StopBeforeStation(); //������� �� ���������� ������� ����� �������� ��� �������� � ������� �������
	void SpanStoppingEarlierThanEntraStopping(); //������� ������� �� �������� �������� ��������� ������� ����� �������
	void EntraStoppingEarlierThanSpanStopping(); //������� ������� ����� ������� �������� ��������� ������� �� ��������
	void EntraStoppingEarlierThanSpanStopping2();
	void NoTrainSpanStopping(); //������ ������� �� �������� ���������� (��� �� �����). ������� ���� �� ������
};