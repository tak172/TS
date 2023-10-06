#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"
#include "../Guess/TrackerEvent.h"

class TC_TrackerWrongWay : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerWrongWay );
	CPPUNIT_TEST( UnorientedNoNumber );
	CPPUNIT_TEST( UnorientedEvenNumber );
	CPPUNIT_TEST( UnorientedOddNumber );
	CPPUNIT_TEST( UnorientedEvenNumberWithSema );
	CPPUNIT_TEST( SpanOrientedNoNumber );
	CPPUNIT_TEST( SpanOrientedEvenNumber );
	CPPUNIT_TEST( SpanOrientedOddNumber );
	CPPUNIT_TEST( StatOrientedNoNumber );
	CPPUNIT_TEST( StatOrientedEvenNumber );
	CPPUNIT_TEST( StatOrientedOddNumber );
	CPPUNIT_TEST( CheckOrder );
	CPPUNIT_TEST( OneWaySpan );
	CPPUNIT_TEST( NoWrongWayWithFictiveStrip );
	CPPUNIT_TEST( DeathThenRecoveryOnWrongSpanWithEvent );
	CPPUNIT_TEST( DeathThenRecoveryOnWrongSpanWOEvent );
	CPPUNIT_TEST( DeathThenRecoveryOnWrongSpanUniqueEvent );
	CPPUNIT_TEST( BadHeadOrientationWhileWrongWayWithRecovery );
	CPPUNIT_TEST( BadHeadOrientationWhileWrongWayWithRecovery2 );
	CPPUNIT_TEST_SUITE_END();

	void UnorientedNoNumber(); //���� �� ������������, ���� �������� ��� ������
	void UnorientedEvenNumber(); //���� �� ������������, ���� �������� ������
	void UnorientedOddNumber(); //���� �� ������������, ���� �������� ��������
	void UnorientedEvenNumberWithSema(); //���� �� ������������, ���� �������� ������, �� ������ ���������� ������ ��������
	void SpanOrientedNoNumber(); //���� ������������ �� �������, ���� �������� ��� ������
	void SpanOrientedEvenNumber(); //���� ������������ �� �������, ���� �������� ������
	void SpanOrientedOddNumber(); //���� ������������ �� �������, ���� �������� ��������
	void StatOrientedNoNumber(); //���� ������������ �� �������, ���� �������� ��� ������
	void StatOrientedEvenNumber(); //���� ������������ �� �������, ���� �������� ������
	void StatOrientedOddNumber(); //���� ������������ �� �������, ���� �������� ��������
	void CheckOrder(); //������� ���������� �������: ������� ����� �� ������������ ���� ��������, ����� �������� �� ��������
	void OneWaySpan(); //�������� �� ����������� �������� (��������� � ����� ������)
	void NoWrongWayWithFictiveStrip();
	void DeathThenRecoveryOnWrongSpanWithEvent(); //��������� ������� ������ �� ������������ ���� �������� ��� �������������� �� ������� �������-�������
	void DeathThenRecoveryOnWrongSpanWOEvent(); //��� ��������� ������� ������ �� ������������ ���� �������� ��� �������������� �� ������� �������-�������
	void DeathThenRecoveryOnWrongSpanUniqueEvent(); //������� ������ �� ������������ ���� �������� ��� �������������� �� ������� �������-������� ������ ���� ������������
	bool GoodMove( const std::wstring & strip1, const std::wstring & strip2, Oddness trainOddness ); //��������� - ���� �� ������� ������ �� ������������ ����
	void BadHeadOrientationWhileWrongWayWithRecovery(); //��������� ���������� ������ �� ����� �������������� � ������������� ������� �� ������������ ���� ��������
	void BadHeadOrientationWhileWrongWayWithRecovery2();
};