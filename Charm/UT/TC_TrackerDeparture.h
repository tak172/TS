#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ��������� ������� �����������

class TC_TrackerDeparture : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerDeparture );
	CPPUNIT_TEST( Departure );
	CPPUNIT_TEST( OpenSemaDepartureWithRoute );
	CPPUNIT_TEST( OpenSemaDepartureWORoute );
	CPPUNIT_TEST( OpenMultiWayDeparture );
	CPPUNIT_TEST( CloseWayExit );
	CPPUNIT_TEST( CloseWayExitAndStationExit );
	CPPUNIT_TEST( WOSemaWayExit );
	CPPUNIT_TEST( WOSemaWayExitAndStationExit );
	CPPUNIT_TEST( BanTransitionAfterStart );
	CPPUNIT_TEST( FormDepartureAndSpanMove );
	CPPUNIT_TEST( FormSpanMoveAndLeaveAD );
	CPPUNIT_TEST( CompositeRouteDeparture );
	CPPUNIT_TEST( DuplicateEventOnReborn );
	CPPUNIT_TEST( DelayedDepartureForExitingNoTrain );
	CPPUNIT_TEST( DelayedDepartureForExitingNoTrain2 );
	CPPUNIT_TEST( DelayedDepartureForExitingNoTrain3 );
	CPPUNIT_TEST( DelayedDepartureForExitingNoTrain4 );
	CPPUNIT_TEST( DepartureWithRecoveryForExitingNoTrain );
	CPPUNIT_TEST( DepartureWithRecoveryForExitingNoTrain2 );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain2 );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain3 );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain4 );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain5 );
	CPPUNIT_TEST( DelayedDepartureADOutUpdate );
	CPPUNIT_TEST( DelayedDepartureADOutUpdate2 );
	CPPUNIT_TEST( ADOutThenReturnThenUpgradeThenExit );
	CPPUNIT_TEST( DelayedDepartureForLongTrain );
	CPPUNIT_TEST( MixingUpDepartureFactOnRecovering );
	CPPUNIT_TEST_SUITE_END();

	void Departure(); //����������� � �/� ����
	void OpenSemaDepartureWithRoute(); //����������� � �/� ���� �� ����������� ������ ��������� � �������� �������� ���������
	void OpenSemaDepartureWORoute(); //����������� � �/� ���� �� ����������� ������ ��������� ��� ��������� ��������� ��������
	void OpenMultiWayDeparture(); //����������� � �/� ���� (�� ���������� ��������) �� ����������� ������ ���������
	void CloseWayExit(); //����� � �/� ���� �� ����������� ������ ���������
	void CloseWayExitAndStationExit(); //����� � �/� ���� �� ����������� ������ ��������� � ����������� ������� �� �������
	void WOSemaWayExit(); //����� � �/� ���� ��� ���������� ���������
	void WOSemaWayExitAndStationExit(); //����� � �/� ���� ��� ���������� ��������� � ����������� ������� �� �������
	void BanTransitionAfterStart(); //������ �������� ����� ������ ������ ������������ �� ����� ���� �������������
	void FormDepartureAndSpanMove(); //������������ ������ ����� ����������� � ����� �� �������
	void FormSpanMoveAndLeaveAD(); //������������ ������, ����� �� ������� � ����� � �/� ����
	void CompositeRouteDeparture(); //����������� �� ������� �� ���� �������� ���������
	void DuplicateEventOnReborn(); //������������ ������� ����� ������������ ������
	void DelayedDepartureForExitingNoTrain(); //��� ����������� ���������� � ����� �� ������ �� ������� ������������ ����� � ����������� � ���������� ��-����
	void DelayedDepartureForExitingNoTrain2();
	void DelayedDepartureForExitingNoTrain3();
	void DelayedDepartureForExitingNoTrain4();
	void DelayedDepartureForFormingTrain(); //���������� ����������� � ���������� ��-���� ��� ������������ ������ �� ����������
	void DelayedDepartureForFormingTrain2();
	void DelayedDepartureForFormingTrain3();
	void DelayedDepartureForFormingTrain4();
	void DelayedDepartureForFormingTrain5();
	void DelayedDepartureADOutUpdate(); //���������� ���������� � ������ � ��-���� ��� ���������� ��� �� �������� �� ������ ��-����
	void DelayedDepartureADOutUpdate2();
	void ADOutThenReturnThenUpgradeThenExit(); //����� ���������� � ��-���� � ����������� ������������, ��������� � ������� �� ������� ��� ������
	void DelayedDepartureForLongTrain(); //����������� �������� ������ �� ��������� ��������
	void DepartureWithRecoveryForExitingNoTrain(); //�����������, ����������� � ������������� � ��������������� ����������
	void DepartureWithRecoveryForExitingNoTrain2();
	void MixingUpDepartureFactOnRecovering(); //������������� ������ ����������� � ������ ��-����� ��-�� ��������������
};