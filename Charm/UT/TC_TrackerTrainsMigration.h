#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//����� �� �������� ������� � ����� �� �� ������

class TC_TrackerTrainsMigration : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainsMigration );
	//CPPUNIT_TEST( BidMigrationThrouBadDirectedSwitch );
	CPPUNIT_TEST( BidMigrationOverOther );
	CPPUNIT_TEST( MigrationVsOccupation );
	CPPUNIT_TEST( BidJumpToSpan );
	CPPUNIT_TEST_SUITE_END();

	void BidMigrationThrouBadDirectedSwitch(); //������ �������� ����� ������� (����� ���������� ������ � ������������ �� �������� ����� ���������� ������)
	void BidMigrationOverOther(); //������ �������� ����� ������ ��������� (����� ������ ��)
	void MigrationVsOccupation(); //������ �������� �� ����� �������� ������ � ���
	void BidJumpToSpan(); //������ ������� ������ ������ � ������ ������� (�������/�������) �� ������
};