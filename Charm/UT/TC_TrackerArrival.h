#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ��������� ������� ��������

class TC_TrackerArrival : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerArrival );
	CPPUNIT_TEST( CoupledSlowArriving );
	CPPUNIT_TEST( OppositeArriving );
    CPPUNIT_TEST( ArrivingToOpenWay );
    CPPUNIT_TEST( ArrivingToClosedWay );
    CPPUNIT_TEST( ArrivingToWayWOSema );
	CPPUNIT_TEST( FastArrivingToWayWOSema );
	CPPUNIT_TEST( CloseWayUncontainedArriving );
	CPPUNIT_TEST( OpenWayAsyncArrivingPassenger );
	CPPUNIT_TEST( OpenWayAsyncArrivingCargo );
	CPPUNIT_TEST( ArrivingToOpenWayAndCloseSema );
	CPPUNIT_TEST( ArrivingToOpenWayAndCloseSema2 );
	CPPUNIT_TEST( ArrivingToClosedWayAndReversiveExit );
	CPPUNIT_TEST( UncontainedArrivingPassAndBack );
	CPPUNIT_TEST( OpenWayAsyncArrivingMomentFullEntry );
	CPPUNIT_TEST( OpenWayAsyncArrivingMomentFullEntry2 );
	CPPUNIT_TEST( OpenWayAsyncArrivingMomentPartialEntry );
	CPPUNIT_TEST( ForbidArrivingAfterFormOnSameADWay );
	CPPUNIT_TEST( PermitArrivingAfterForm );
	CPPUNIT_TEST( ForbidArrivingAfterFormOnSameADWay2 );
	CPPUNIT_TEST( ArrivingToBusyWay );
	CPPUNIT_TEST( NoArrivingOnHeadCloseWithNoTrain );
	CPPUNIT_TEST( NoAsyncArrivalForNoTrain );
	CPPUNIT_TEST_SUITE_END();

	void CoupledSlowArriving(); //����������� �������� �� ��� �/� ����
	void OppositeArriving(); //�������� �� �/� ���� � ������ ������
    void ArrivingToOpenWay(); //�������� �� �/� ���� ��� �������� �������� ���������
    void ArrivingToClosedWay(); //�������� �� �/� ���� ��� �������� �������� ���������
    void ArrivingToWayWOSema(); //�������� �� �/� ���� ��� ���������� ��������� ���������
	void FastArrivingToWayWOSema(); //������� �������� �� �/� ���� ��� ���������� ��������� ���������
	void CloseWayUncontainedArriving(); //�������� ������������ ������, ������� �� ���������� �� �/� ���� �������
	void OpenWayAsyncArrivingPassenger(); //����������� ��������� ������� �������� ��� ������� �/� ���� � �������� ���������� ������������� ������
	void OpenWayAsyncArrivingCargo(); //����������� ��������� ������� �������� ��� ������� �/� ���� � �������� ���������� ��������� ������
	void ArrivingToOpenWayAndCloseSema(); //�������� �� �/� ���� ��� �������� �������� ��������� � ����������� ��� �����������
	void ArrivingToOpenWayAndCloseSema2(); //��������� �������� �� �/� ���� ��� �������� �������� ��������� � ����������� ��� �����������
	void ArrivingToClosedWayAndReversiveExit(); //����������� �������� �� �/� ���� � �������� ���������� � ����������� �� ��� �� �������
	void UncontainedArrivingPassAndBack(); //�������� ������������ ������ (����� �� ��������), ������ ��������� �� ������� � ����������
	void OpenWayAsyncArrivingMomentFullEntry(); //���� �� ���������� ������ ������� �������� �� ����� ������� ������� ���������� �� ���� (������ �����)
	void OpenWayAsyncArrivingMomentFullEntry2();
	void OpenWayAsyncArrivingMomentPartialEntry(); //���� �� ���������� ������ ������� �������� �� ����� ������� ������� ���������� �� ���� (��������� �����)
	void ForbidArrivingAfterFormOnSameADWay(); //����� �� ����� ������������ �������������� � ������� �� ��� ��-���� � ��� �� ������
	void ForbidArrivingAfterFormOnSameADWay2(); //������ ���������� ������ ���������
	void PermitArrivingAfterForm(); //������������ �������� ����� ������������, ���� ����� ������������� �� �� ��-����
	void ArrivingToBusyWay(); //�������� �� ������� ���� � ����������� ������������
	void NoArrivingOnHeadCloseWithNoTrain(); //�������� ��������� �� ������� �������� ��� ����������
	void NoAsyncArrivalForNoTrain(); //��� ������������ ������� ��� ������������ �������� �� ���������� ��������� ��� ����������
};