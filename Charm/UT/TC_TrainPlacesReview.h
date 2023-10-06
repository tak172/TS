#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_TrainPlacesReview : public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_TrainPlacesReview );
	CPPUNIT_TEST( PlaceTrain );
	CPPUNIT_TEST( EraseTrain );
	CPPUNIT_TEST( PlaceShunting );
	CPPUNIT_TEST( EraseShunting );
	CPPUNIT_TEST( PlaceTrainOnShunting );
	CPPUNIT_TEST( NonrelevantEraseAfterReplace );
	CPPUNIT_TEST( MultiPlaceIntoSinglePacket );
	CPPUNIT_TEST_SUITE_END();

	void PlaceTrain(); //���������� ������
	void EraseTrain(); //������ ������ � �������
	void PlaceShunting(); //���������� ����������
	void EraseShunting(); //������ ���������� � �������
	void PlaceTrainOnShunting(); //���������� ������ ������ ����������
	void NonrelevantEraseAfterReplace(); //������ ���������� � ������� ��� ����� ���������� ���� ������ ������
	void MultiPlaceIntoSinglePacket(); //��������� ���������� ����, ��������� � ����� xml-����
};