#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ��������� ������� ������/������ �� �������

class TC_TrackerStationEntryExit : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerStationEntryExit );
	CPPUNIT_TEST( NoWayEntry );
	CPPUNIT_TEST( ProlongedEntryAndArriving );
	CPPUNIT_TEST( FastTransit );
	CPPUNIT_TEST( FastEntryAndArriving );
	CPPUNIT_TEST( NoADPassingAfterBlockpost );
	CPPUNIT_TEST( StationOutOnEqualStationsWithRoute );
	CPPUNIT_TEST( StationOutOnEqualStationsWithRoute2 );
	CPPUNIT_TEST( StationOutOnEqualStationsWORoute );
	CPPUNIT_TEST( StationOutOnEqualStationsWORoute2 );
	CPPUNIT_TEST( StationOutOnUnequalStationsWithRoute );
	CPPUNIT_TEST( StationOutOnUnequalStationsWORoute );
	CPPUNIT_TEST( StationOutOnUnequalStationsWORoute2 );
	CPPUNIT_TEST_SUITE_END();

	void NoWayEntry(); //����� �� ������� �� �� �/� ����
	void ProlongedEntryAndArriving(); //��������������� ����� �� ������� � ����������� ��������
	void FastTransit(); //������� ������ ����� ������� ��� ������� �/� �����
	void FastEntryAndArriving(); //������� ����� �� ������� � ����������� ��������
	void NoADPassingAfterBlockpost(); //������ ������ ������� ��� ������� �/� ����� � ���������� �������� ����-�����
	void StationOutOnEqualStationsWithRoute(); //����� �� ������� �� ������������� ������� �� ��������� ��������
	void StationOutOnEqualStationsWithRoute2();
	void StationOutOnEqualStationsWORoute(); //����� �� ������� �� ������������� ������� ��� ��������� ��������
	void StationOutOnEqualStationsWORoute2();
	void StationOutOnUnequalStationsWithRoute(); //����� �� ������� �� ��������������� ������� �� ��������� ��������
	void StationOutOnUnequalStationsWORoute(); //����� �� ������� �� ��������������� ������� ��� ��������� ��������
	void StationOutOnUnequalStationsWORoute2();
};