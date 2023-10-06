#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//����� �� ������� � ����������� ��������� ��� ������������� ��

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerMultiTrain : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerMultiTrain );
	CPPUNIT_TEST( AppearanceOnSwitch );
	CPPUNIT_TEST( NamedMovingToNamed );
	CPPUNIT_TEST( SwitchMergingOfTrainAndNoTrain );
	CPPUNIT_TEST( SwitchNotMergingOfTrains );
	CPPUNIT_TEST( BusinessAtomicOverlay );
	CPPUNIT_TEST( GoingToObscurity );
	CPPUNIT_TEST( HeterogeneousMoving );
	CPPUNIT_TEST( TrainOccupyNoTrain );
	CPPUNIT_TEST( BidirCreate );
	CPPUNIT_TEST( Intersection );
	CPPUNIT_TEST( CloseWrongSemaPassing );
	CPPUNIT_TEST( NoMergeTrainWithUnityWhileDisforming );
	CPPUNIT_TEST( MultiConflict );
	CPPUNIT_TEST( MultiFreeNBusy );
	CPPUNIT_TEST( IndexIgnoringOnTrainPriority );
	CPPUNIT_TEST( TrainExitVsNoTrainMovingOnOpenHead );
	CPPUNIT_TEST( NoTrainsLessIdenOccupyPriority );
	CPPUNIT_TEST( ConflictBetweenNearAndFar );
	CPPUNIT_TEST( TailFadeNearADWayWithTrain );
	CPPUNIT_TEST( TailFadeNearADWayWithNoTrain );
	CPPUNIT_TEST( TailFadeNearTwoShortTrains );
	CPPUNIT_TEST( TailFadeNearTwoShortTrains2 );
	CPPUNIT_TEST( SwitchToggleBreakSequence );
	CPPUNIT_TEST( SwitchToggleBreakSequence2 );
	CPPUNIT_TEST_SUITE_END();

	void AppearanceOnSwitch(); //������������� �� ������� ���� ��
	void NamedMovingToNamed(); //����������� ������������ �� � ������������ ��
	void SwitchMergingOfTrainAndNoTrain(); //������� ������ � ���������� ����� �������� �������
	void SwitchNotMergingOfTrains(); //������ ������� ������� ����� �������� �������
	void BusinessAtomicOverlay(); //��������� �������� ���������� ���� ������� �� ������������ (�� ������ ��������� � ����������)
	void GoingToObscurity(); //���� ����� �� � ������������� (������������) � ������������� �������� �� ����� ������ ��
	void HeterogeneousMoving();
	void TrainOccupyNoTrain(); //������� ������� �����, ���������������� ��� ���������� �� (�� ��������� ��������� ���� � ������������� ������ ����������)
	void BidirCreate(); //������� ���� �������� �� ������ ������� �� �����, ����������� ������� (�������� �������� ������������ �������� �������� ������)
	void Intersection(); //����������� ���� ������� �� ����� �������
	void CloseWrongSemaPassing(); //������ ����� �������� ����� �������� ��� ���� ������ ������� �������
	void NoMergeTrainWithUnityWhileDisforming(); //������ ������� ������ � �� ����� �������� ����� ����� ��������
	void MultiConflict(); //�������� ��� ������� ������������� ������� �����, ��� ����� ��
	void MultiFreeNBusy(); //������������� ������������ � ������� ���������� �������� ��� ���������� ��
	void IndexIgnoringOnTrainPriority(); //������������� ������� ��� ����������� �������������� ������
	void TrainExitVsNoTrainMovingOnOpenHead(); //����� ����� ������� ������ � ��-���� �� �������� �������� � ��������� � ���� ���������� ������ ���������� ������
	void NoTrainsLessIdenOccupyPriority(); //��� �� ������� ��� ������ ������ ��� ������� ������������ �������� �� � ������� �������������� (��� ���������������� ���������)
	void ConflictBetweenNearAndFar(); //����������� ����������� ������� ������� �� ������� ������� ���������� ������
	void TailFadeNearADWayWithTrain(); //������� ������ �������� �� (�� ����� �����) ����� � ��-����� ��� ������� �� ��� ������
	void TailFadeNearADWayWithNoTrain(); //������� ������ �������� �� (�� ����� �����) ����� � ��-����� ��� ������� �� ��� ����������
	void TailFadeNearTwoShortTrains(); //������� ������ ������ ����� ���� ����������� �������
	void TailFadeNearTwoShortTrains2();
	void SwitchToggleBreakSequence(); //��������� ������� ���������� �������� ����� �������� �������
	void SwitchToggleBreakSequence2();
};