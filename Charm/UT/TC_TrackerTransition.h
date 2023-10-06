#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ��������� ������� �����������

class TC_TrackerTransition : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTransition );
	CPPUNIT_TEST( UncontainedTransition );
	CPPUNIT_TEST( AsyncTransitionCargo );
	CPPUNIT_TEST( AsyncTransitionPassengerSpec );
	CPPUNIT_TEST( AsyncTransitionPassengerNoSpec );
	CPPUNIT_TEST( StandardTransition );
	CPPUNIT_TEST( WOWaysTransition );
	CPPUNIT_TEST( WOWaysPassing );
	CPPUNIT_TEST( OpenedBlockpostTransition );
	CPPUNIT_TEST( ClosedBlockpostTransition );
	CPPUNIT_TEST( DeparturingFormAndBPTransition );
	CPPUNIT_TEST( TransitionWhileMultiWaysLiberation );
	CPPUNIT_TEST( LongTransitionThroughEmbarkWay );
	CPPUNIT_TEST( FastTransitionThroughEmbarkWay );
	CPPUNIT_TEST( MoveThroughSeveralBlockposts );
	CPPUNIT_TEST( HistoryLimitationOnTransitionGeneration );
	CPPUNIT_TEST_SUITE_END();

	void UncontainedTransition(); //������������� ������������ ������, ������� �� ���������� �� �/� ���� �������
	void AsyncTransitionCargo(); //������������� ��������� ������
	void AsyncTransitionPassengerSpec(); //������������� ������������� ������ (�� ����, ������������������� ��� �������/������� ����������)
	void AsyncTransitionPassengerNoSpec(); //������������� ������������� ������ (�� ����, �� ������������������� ��� �������/������� ����������)
	void StandardTransition(); //������������ ��� ������ �� ������� � ����������� ����������� �������������
	void WOWaysTransition(); //������������� �������, ����� �/� ���� (������� ������������� ����)
	void WOWaysPassing(); //������ �������, ����� �/� ���� (�� �������������)
	void OpenedBlockpostTransition(); //������������� ����� ����-���� (�������� ������)
	void ClosedBlockpostTransition(); //������������� ����� ����-���� (�������� ������)
	void BlockpostTransition(); //������������� ����� ����-����
	void DeparturingFormAndBPTransition(); //�������� ������ ����� ��������� �/� ���� � ����������� ������������� ����-�����
	void TransitionWhileMultiWaysLiberation(); //������������� ��� ������������ ���������� �������� (�������� �� ���������� ������������ ������������� ��������)
	//���������� ��� ������������� ������� ������������� ���������� ����� ������ ������� �� ���� � �������� ������� �������������
	void LongTransitionThroughEmbarkWay();
	void FastTransitionThroughEmbarkWay();
	void MoveThroughSeveralBlockposts(); //������ "�����������" ���������� � ������ � ��-���� ����� �������� �������
	void HistoryLimitationOnTransitionGeneration(); //����������� ������� ��������� ������� ��� ��������� �������������
};