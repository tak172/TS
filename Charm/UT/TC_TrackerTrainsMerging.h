#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ������� ����� ��� ���������� ��

class TC_TrackerTrainsMerging : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainsMerging );
	CPPUNIT_TEST( MergingTrainWithNearUnity );
	CPPUNIT_TEST( MergeTrainWithUnityOnSpan );
	CPPUNIT_TEST( MergeTrainWithUnityOnSema );
	CPPUNIT_TEST( TrainAbsorptionOfNotrain );
	CPPUNIT_TEST( TrainAbsorptionOfNotrain2 );
	CPPUNIT_TEST( TrainAbsorptionOfNotrain3 );
	CPPUNIT_TEST( NotrainEmbeddingIntoTrain );
	CPPUNIT_TEST( NotrainEmbeddingIntoTrain2 );
	CPPUNIT_TEST( NotrainEmbeddingIntoTrain3 );
	CPPUNIT_TEST( NotrainEmbeddingIntoTrain4 );
	CPPUNIT_TEST( NoMergingWithLeftTrain );
	CPPUNIT_TEST_SUITE_END();

	void MergingTrainWithNearUnity(); //������� ������ � ����������� ����� �� (�� �������)
	void MergeTrainWithUnityOnSpan(); //������� ������ � �� ������� �� ��������
	void MergeTrainWithUnityOnSema(); //������� ������ � ���������� ����� ���������
	void TrainAbsorptionOfNotrain(); //���������� ������� �� ��� �� �������� � ������ ������ �� ��-���� (����� �� ������ ���������)
	void TrainAbsorptionOfNotrain2();
	void TrainAbsorptionOfNotrain3();
	void NotrainEmbeddingIntoTrain(); //����������� ���������� � ����� ��� �� ��������
	void NotrainEmbeddingIntoTrain2(); //"�����" ���������� �� � ����� (������� ��� ����������� ���������� � ������)
	void NotrainEmbeddingIntoTrain3();
	void NotrainEmbeddingIntoTrain4();
	void NoMergingWithLeftTrain(); //������� � �������, ���������� ��, �������� � ������
};