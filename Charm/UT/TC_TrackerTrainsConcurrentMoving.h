#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//����� �� ������������� �������� ������������� ��

class TC_TrackerTrainsConcurrentMoving : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainsConcurrentMoving );
	CPPUNIT_TEST( UnknownMoving );
	CPPUNIT_TEST( RacingMovingSinglePlaces );
	CPPUNIT_TEST( RacingMovingMultiPlaces );
	CPPUNIT_TEST( CloseMoving );
	CPPUNIT_TEST( CloseMoving2 );
	CPPUNIT_TEST( CloseMoving3 );
	CPPUNIT_TEST( CloseMoving4 );
	CPPUNIT_TEST( ChainShortening );
	CPPUNIT_TEST( ChainShortening2 );
	CPPUNIT_TEST( ChainShortening3 );
	CPPUNIT_TEST( ChainShortening4 );
	CPPUNIT_TEST( ChainShortening5 );
	CPPUNIT_TEST( ChainShortening6 );
	CPPUNIT_TEST( ChainShortening7 );
	CPPUNIT_TEST( SpanChainShortening );
	CPPUNIT_TEST( ChainBreaking );
	CPPUNIT_TEST( ParallelMovingNearWrongSema );
	CPPUNIT_TEST( TrainChaseNoTrainThroughWrongSema );
	CPPUNIT_TEST( TrainChaseNoTrainThroughGoodSema );
	CPPUNIT_TEST( TrainChaseTwoNoTrainsThroughWrongSema );
	CPPUNIT_TEST( TrainChaseTwoNoTrainsThroughGoodSema );
	CPPUNIT_TEST( TrainChaseTwoNoTrainsThroughGoodSema2 );
	CPPUNIT_TEST( TrainAbsorbsNoTrainThroughGoodSema );
	CPPUNIT_TEST( PushByDyingThroughStripFast );
	CPPUNIT_TEST( PushByDyingThroughStripSlow );
	CPPUNIT_TEST( ApproachingSpanTrainVsExitingStatTrain );
	CPPUNIT_TEST( LeavingSpanTrainVsExitingStatTrain );
	CPPUNIT_TEST( LeavingSpanTrainVsExitingStatTrain2 );
	CPPUNIT_TEST_SUITE_END();

	void UnknownMoving(); //����������� ���� ����������� �� (�� ����� ��������� �������� ����� �����) � ���� ������� �� ���� �������
	void RacingMovingSinglePlaces(); //������������� �������� ���� ������� � ������� ������� ������ �� ����� ���������. ������ �� ������� �������� ���� �����
	//������������� �������� ���� ������� � ������� ������� ������ �� ����� ���������. ������� ����� �������� ����� ������ �����
	void RacingMovingMultiPlaces();

	void CloseMoving(); //�������� ��������
	void CloseMoving2(); //�������� �������� (�������� 2)
	void CloseMoving3(); //�������� �������� (�������� 3)
	void CloseMoving4(); //�������� �������� (�������� 4, ������� ���������� � �������� �����)
	void ChainShortening(); //������������ �� (�������� 1)
	void ChainShortening2(); //������������ �� (�������� 2)
	void ChainShortening3(); //������������ �� (�������� 3)
	void ChainShortening4(); //������������ �� (�������� 4)
	void ChainShortening5(); //������������ �� (�������� 5)
	void ChainShortening6(); //������������ �� (�������� 6)
	void ChainShortening7();
	void SpanChainShortening(); //������������ �� �� ��������
	void ChainBreaking(); //������� �������� �� � ������ ���� �������, ������� ��������
	void ParallelMovingNearWrongSema(); //������������� �������� ������� ����� ��������� ����� ��������
	void TrainChaseNoTrainThroughWrongSema(); //����� �������� ����������, �������� ����� �������� ����� ��������
	void TrainChaseNoTrainThroughGoodSema(); //����� �������� ����������, �������� ����� �������� ����� ��������
	void TrainChaseTwoNoTrainsThroughWrongSema(); //����� �������� ��� ����������, �������� ����� �������� ����� ��������
	void TrainChaseTwoNoTrainsThroughGoodSema(); //����� �������� ��� ����������, �������� ����� �������� ����� ��������
	void TrainChaseTwoNoTrainsThroughGoodSema2();
	void TrainAbsorbsNoTrainThroughGoodSema(); //������� ���������� � ������� ����� ��� ������ �� ���������� ����� �������� ����� ��������
	void PushByDyingThroughStripFast(); //������������� ��������� ������� ������ ����� ������� �� ����� ����������
	void PushByDyingThroughStripSlow();
	void PushByDyingThroughStrip( bool slowly );
	void ApproachingSpanTrainVsExitingStatTrain(); //��� ������ ������ ������������ �������� ������������� ����������� ������ ����� ��������� �� �������
	void LeavingSpanTrainVsExitingStatTrain(); //��� ������ ������ ������������ �������� ������, ���������� �� �������, ����� ����������� ����������
	void LeavingSpanTrainVsExitingStatTrain2();
};