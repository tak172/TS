#pragma once

//�������� ��������� ������� ������������/��������������� �������

//���������������, ��� ��� ���������� ��������� � ����������� ���������, 
//���� �� ������� ����

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerFormDisform : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerFormDisform );
	CPPUNIT_TEST( NoDisformCauseLocoMove );
	CPPUNIT_TEST( NoDisformEmptyNumber );
	CPPUNIT_TEST( FormDisform );
	CPPUNIT_TEST( MultiForm );
	CPPUNIT_TEST( DisformAndDeath );
	CPPUNIT_TEST( NoFormOnOnlyOpenSema );
	CPPUNIT_TEST( FormOnOpenRoute );
	CPPUNIT_TEST( LeaveADWayOnOppositeOpenedRoute );
	CPPUNIT_TEST( LeaveADWayOnOppositeClosedRoute );
	CPPUNIT_TEST( LeaveADWayOnTrainSemaAndDisformOnSema );
	CPPUNIT_TEST( LeaveADWayOnShuntSemaAndDisformOnSema );
	CPPUNIT_TEST( LeaveADWayWOSemaAndDisformOnSema );
	CPPUNIT_TEST( FromADWayToSpanOnClosedTrainSemaAndGoToObscurity );
	CPPUNIT_TEST( FromADWayToSpanOnOpenedTrainSemaAndGoToObscurity );
	CPPUNIT_TEST( LeaveADWayWithSemaAndGoToObscurity );
	CPPUNIT_TEST( LeaveADWayWOSemaAndGoToObscurity );
	CPPUNIT_TEST( StatEntryWOADWayDisformOnSema );
	CPPUNIT_TEST( StatEntryAndDeath );
	CPPUNIT_TEST( StatEntryWOADWayGoToNoTSStrip );
	CPPUNIT_TEST( StatEntryWOADWayGoToObscurity );
	CPPUNIT_TEST( StatEntryWOADWayGoToSpanObscurity );
	CPPUNIT_TEST( MoveADWayToADWayAndSemaDisform );
	CPPUNIT_TEST( NoDisformOnSpan );
	CPPUNIT_TEST( StationMigrationFromADWay );
	CPPUNIT_TEST( PassingTwoAlienSemasWithNumForwarding );
	CPPUNIT_TEST( RecreateOnStationFromSpanEntryThroughAlienSema );
	CPPUNIT_TEST( RecreateOnStationFromSpanEntryThroughAlienSema2 );
	CPPUNIT_TEST( MultiTrainsThroughAlienSema );
	CPPUNIT_TEST( MultiTrainsThroughAlienSema2 );
	CPPUNIT_TEST( CompeteForPlaceWhileRecreating );
	CPPUNIT_TEST( LosingOddnesOnDisform );
	CPPUNIT_TEST( IgnoringUndefSemaFromSpanToStat );
	CPPUNIT_TEST( LongtimeDisformLosing );
	CPPUNIT_TEST( RebornInsteadOutThenDeath );
	CPPUNIT_TEST( ADFormNearObscurity );
	CPPUNIT_TEST( ADFormNearObscurity2 );
	CPPUNIT_TEST( ADNoFormNearObscurity );
	CPPUNIT_TEST( ADNoFormNearObscurity2 );
	CPPUNIT_TEST( EventMomentOnDispositionDisform );
	CPPUNIT_TEST( ClosedWayOutAndGoToObscurity );
	CPPUNIT_TEST( OpenedWayOutAndGoToObscurity );
	CPPUNIT_TEST( HistoryLimitationObserve );
	CPPUNIT_TEST( ExtFormNSemaDisform );
	CPPUNIT_TEST_SUITE_END();

	void NoDisformCauseLocoMove(); //���������� ��������������� ����� ������� ���������� �� ������
	void NoDisformEmptyNumber(); //���������� ��������������� ��� ������ ��� ������
	void FormDisform(); //������������ � ��������������� ������
	void MultiForm(); //������������ ������ ��������� ���
	void DisformAndDeath(); //��������������� ������ ����� ������� ��������� ������������ �������� � ��� ��������
	void NoFormOnOnlyOpenSema(); //���������� ������������ ��� ������ � �� ���� ������ �� �������� �������� �������� (��� ��������)
	void FormOnOpenRoute(); //������������ ��� �������� ��� ������ � �� ���� �� �������� �������

	void LeaveADWayOnOppositeOpenedRoute(); //����� � �/� ���� �� ��������� �������� � ��������������� �������
	void LeaveADWayOnOppositeClosedRoute(); //����� � �/� ���� �� ��������� �������� � ��������������� �������
	//����� � �/� ���� �� �������� �������� ��������������� �������� � ����������� ���������������� ��� �������� �� ������� ����� �������� ����� ��������:
	void LeaveADWayOnTrainSemaAndDisformOnSema();
	//����� � �/� ���� �� ���������� �������� ��������������� �������� � ����������� ���������������� ��� �������� �� ������� ����� �������� ����� ��������:
	void LeaveADWayOnShuntSemaAndDisformOnSema();
	//����� � �/� ���� ��� ��������� � ����������� ���������������� ��� �������� �� ������� ����� �������� ����� ��������:
	void LeaveADWayWOSemaAndDisformOnSema();
	//����� � �/� ���� �� �������� �������� �������� ��������������� �������� (������� �� �����) � ����������� ���������������� ��� ������ �� ���� ������������:
	void FromADWayToSpanOnClosedTrainSemaAndGoToObscurity();
	//����� � �/� ���� �� �������� �������� �������� ��������������� �������� (�������� �������) � ����������� ���������������� ��� ������ �� ���� ������������:
	void FromADWayToSpanOnOpenedTrainSemaAndGoToObscurity();
	//����� � �/� ���� ����� �������� ��������������� �������� � ����������� ���������������� ��� ������ �� ���� ������������:
	void LeaveADWayWithSemaAndGoToObscurity();
	//����� � �/� ���� ��� ��������� � ����������� ���������������� ��� ������ �� ���� ������������:
	void LeaveADWayWOSemaAndGoToObscurity();
	//����� �� ������� ��� ������� �/� ����
	//� ����������� ��������������� ��� �������� �� ������� ����� �������� ����� ��������:
	void StatEntryWOADWayDisformOnSema();
	//����� �� ������� ��� ������� �/� ���� � ����������� ������ � ���������������� ����
	void StatEntryWOADWayGoToObscurity();
	//����� �� ������� ��� ������� �/� ���� � ����������� ������� �� ������� � ������ � ���������������� ����
	void StatEntryWOADWayGoToSpanObscurity();
	//����� �� ������� ��� ������� �/� ���� � ����������� ������ �� ������� ��� ��:
	void StatEntryWOADWayGoToNoTSStrip();
	//����� �� ������� � ����������� ������ ������ (������������ ��� ������� ������):
	void StatEntryAndDeath();
	//������� � �/� ���� �� �/� ���� � ����������� ���������������� �� ��������� ����� ��������
	void MoveADWayToADWayAndSemaDisform();
	//���������� ��������������� ��� �������� �� ��������
	void NoDisformOnSpan();
	//������� ������ �� ������� �� ������� ����� � �� ����
	void StationMigrationFromADWay();
	//������ ������� ������ ����� ��� ��������� ����� �������� � ������������� ������������� ������ ������
	void PassingTwoAlienSemasWithNumForwarding();
	//������������ ������ � ����� ������� ��� ������ � �������� �� ������� ����� ����� ��������
	void RecreateOnStationFromSpanEntryThroughAlienSema();
	void RecreateOnStationFromSpanEntryThroughAlienSema2();
	//������������ ������ � ����� ������� ��� ������ ������ � �������� �� ������� ����� ����� ��������
	void RecreateOnFullSpanLeaveThroughAlienSema();
	//��������� ������� ����� ���������� ����� �������� � ��������� ��������� �� ����������
	void MultiTrainsThroughAlienSema();
	void MultiTrainsThroughAlienSema2();
	//����������� ����� ����� �������� �� ������� ���� � ������������� ������������� ������ �� ��� �� ����� ���������
	void CompeteForPlaceWhileRecreating();
	void LosingOddnesOnDisform(); //��������������� ������ ������ ��������� � ������ ��������
	void IgnoringUndefSemaFromSpanToStat(); //��� ������� ��������� � �������������� ��������� � �������� �� ������� ����� ��������� ��������
	//����� � �/� ���� �� �������� ���������� �������� � ����������� ��������� �������� ������� ��������������� �� ��������� ������� �������� (��� �������� �� ���)
	void LongtimeDisformLosing();
	//������������ ������ �� ������� ��-���� ������ ����������� ������ � ����
	void RebornInsteadOutThenDeath();
	void ADFormNearObscurity(); //������������ ������ �� ��-���� ����� � ���������������� �����
	void ADFormNearObscurity2();
	void ADNoFormNearObscurity(); //��� ������������ ������ �� ��-���� ����� � ���������������� �����
	void ADNoFormNearObscurity2();
	void EventMomentOnDispositionDisform(); //������ ������� ��������������� ������ ��������� �� �������� ������� ����������� � ��������������� � ������ �������� ���������
	void ClosedWayOutAndGoToObscurity(); //����� ������ �� �������� �������� �������� � ������ �� ���������� ������������ �������
	void OpenedWayOutAndGoToObscurity(); //����� ������ �� �������� �������� �������� � ������ �� ���������� ������������ �������
	void HistoryLimitationObserve(); //����� ������� ��� ����������� ������� �������������� ������� ��������
	void ExtFormNSemaDisform(); //������������ ������ �� ������ � ��������������� �� �������� ����� ������� ����� ��������
};