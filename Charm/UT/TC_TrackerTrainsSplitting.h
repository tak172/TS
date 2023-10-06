#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ���������� ����� ��� ���������� ��

class TC_TrackerTrainsSplitting : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainsSplitting );
	CPPUNIT_TEST( SwitchSplittingOfTrain );
	CPPUNIT_TEST( StripSplittingOfTrain );
	CPPUNIT_TEST( StripSplittingOfUnknown );
	CPPUNIT_TEST( LengthyDeclutching );
	CPPUNIT_TEST( MultiSeparation );
	CPPUNIT_TEST( SplittingAndRecovering );
	CPPUNIT_TEST( SplitTrainNearAnotherOne );
	CPPUNIT_TEST( OccupyNoTrainAfterADOut );
	CPPUNIT_TEST( MultiTrainRoutesOnADWayOut );
	CPPUNIT_TEST_SUITE_END();

	void SwitchSplittingOfTrain(); //���������� ������ ����� �������� �������
	void StripSplittingOfTrain(); //���������� ������ ����� ������� ������� � �������� �������
	void StripSplittingOfUnknown(); //���������� �� ������ ����� ������� ������� � �������� �������
	void LengthyDeclutching(); //����������� ��, ���������� ������� ���������� �������� (����� ������� ������ �����)
	void MultiSeparation(); //���������� ���� �� ������������
	void SplittingAndRecovering(); //������������� ���������� � �������������� ������
	void SplitTrainNearAnotherOne(); //���������� ������ �������� ��� ������� ���������
	void OccupyNoTrainAfterADOut(); //���������� �� ������ � ��-���� � ����������� ����� �� ����������
	void MultiTrainRoutesOnADWayOut(); //������������� �������� �������� ����� ��������� � ���������� ������� ������
};