#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//������������ ��������� ��������� ��

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerSingleTrain : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerSingleTrain );
	CPPUNIT_TEST( Appearance );
	CPPUNIT_TEST( AppearanceOnSwitch );
	CPPUNIT_TEST( AtomicMultiSwitchMultiAppearance );
	CPPUNIT_TEST( NamedMoving );
	CPPUNIT_TEST( NamedDoubling );
	CPPUNIT_TEST( AtomicMoving );
	CPPUNIT_TEST( AtomicMoving2 );
	CPPUNIT_TEST( MultiSwitchMoving );
	CPPUNIT_TEST( MultiSwitchMoving2 );
	CPPUNIT_TEST( RefSameTrain );
	CPPUNIT_TEST( RefDiffTrain );
	CPPUNIT_TEST( WrongSortingSideEffect );
	CPPUNIT_TEST( BidirectionalMovingForbid );
	CPPUNIT_TEST( MultiADWaysDiffStations );
	CPPUNIT_TEST( SimultaneousBusyAndFree );
	CPPUNIT_TEST( MicrostationOut );
	CPPUNIT_TEST( SaveTrainWithinComplexADWay );
	CPPUNIT_TEST( NoSaveTrainWithinComplexADWay );
	CPPUNIT_TEST_SUITE_END();

	void Appearance(); //������ ������������� ��
	void AppearanceOnSwitch(); //������ ������������� �� ������� ��������� ��
	void AtomicMultiSwitchMultiAppearance(); //��������� ��������� ���������� �� �� ���������� ��������
	void NamedMoving(); //������ ������������� ����������� ���������� �� (�� ���� �������)
	void NamedDoubling(); //������������ ��������� � ��������� ��� ������������� ��
	void AtomicMoving(); //��������� (��������� ����� ������������ ��������) �������� ��
	void AtomicMoving2(); //��������� �������� �� (� ������ ������������������ �������)
	void MultiSwitchMoving(); //������ �������� �� ����� ��������� �������
	void MultiSwitchMoving2(); //������ �������� �� ����� ��������� ������� (������� 2: �������)
	void RefSameTrain(); //������������ ��������� �� reference-���������
	void RefDiffTrain(); //�������� �� �� ����������� reference-�������
	void WrongSortingSideEffect(); //�������� ������ ���������� ���� �������� �� (�������� �� ��������� ������������� �� �� ��������������� �������)
	void BidirectionalMovingForbid(); //������ �� �������� � ��� �������
	void MultiADWaysDiffStations(); //������� ������� ���������� �/� ����� �� ������ ��������
	void SimultaneousBusyAndFree(); //������������� ������� � ������������ ���������� ��������
	void MicrostationOut(); //����� � ������������ (�������, ��������� �� ������������� ��-�������)
	void SaveTrainWithinComplexADWay(); //���������� ������ ��� �������� ������ ���������� ��-����
	void NoSaveTrainWithinComplexADWay(); //��� ���������� ������ ��� �������� ������ ���������� ��-����
};