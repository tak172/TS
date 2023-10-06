#pragma once

//�������� ��������� �������������� �������

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerRecovery : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerRecovery );
	CPPUNIT_TEST( DiscordantMoveRecovery ); //��������������� �������� ������ � ������� �� ������� (����� ��������� ������� ���������� ����� ������� �����). ������ ��������������
	CPPUNIT_TEST( DiscordantJumpRecovery ); //������ ������ ����� ������� ��������� ������� (������� ���������, ��������� ����� ���� � ��������� ���������)
	CPPUNIT_TEST( BoundDissapearanceUnrecovery ); //������ �������������� ��� ������������ ������ �� ��������� �������
	CPPUNIT_TEST( RecoveryCleanAfterSetInfo ); //������� ���������� � �������������� ������ ��� �������������� ��������� ����� �� ���������� �� ����������� �����
	CPPUNIT_TEST( SamePlaceRecovery ); //�������������� ��� ������� � ��������� ���� �� �������
	CPPUNIT_TEST( StripJump ); //������� ������ ����� �������
	CPPUNIT_TEST( RecoveryForbidness ); //������ �� �������������� ��� ������������ ��������
	CPPUNIT_TEST( RecoveringInsteadMoving ); //�������������� ������ ������ �������� ����������
	CPPUNIT_TEST( RecoveringInsteadMoving2 );
	CPPUNIT_TEST( RecoveringInsteadMoving3 );
	CPPUNIT_TEST( RecoveringAndCountraryMovingOnSingleStrip ); //�������������� � ��������� �������� �� ������������ ������� �������
	CPPUNIT_TEST( SameTrainFarPlacesRecovering ); //�������������� ������ �� ������ �� ������� ��������� ��������
	CPPUNIT_TEST( RecoveryOnThreeStrips ); //�������������� ������������ �� ���� ��������
	CPPUNIT_TEST( RecoveryOverBlockedStrips ); //�������������� ����� ��������� ����-�������
	CPPUNIT_TEST( InterferringOfEmptyInfoRecovering ); //������ ������������� �������������� ������� ��� ���������� �� ����������� ���������
	CPPUNIT_TEST( RecoveryThroughLive ); //�������������� �������������� ������ � ���������� ����� ����� ��-�� ��������� �� ������� ����������
	CPPUNIT_TEST( TopologicallyUnreachableRecover ); //������������� ����������� �������������� ����� �������
	CPPUNIT_TEST( RecoveringOnSinglePlaceUnity ); //�������������� ������ ��, ����������� ������������ �����
	CPPUNIT_TEST( RecoveryVersusOccupation ); //�������������� ������ � ������������� ������� ��� �� �������� ������ ��
	CPPUNIT_TEST( RecoveryVersusOccupation2 );
	CPPUNIT_TEST( PermittedRecoveryOverSwitch ); //��������� �������������� ������ �� ������� ����� �������
	CPPUNIT_TEST( ForbiddenRecoveryOverSwitch ); //������ �������������� ������ �� ������� ����� �������
	CPPUNIT_TEST( DeathVersusDisform ); //������������ ������ ������ ��������������� � ����������� ��� ��������������
	CPPUNIT_TEST( MultiRecovery ); //������������� ��������������
	CPPUNIT_TEST( MultiRecoveryWithinOneChain ); //������������� �������������� ������ ����� ��
	CPPUNIT_TEST( MultiRecoveryWithinOneChain2 );
	CPPUNIT_TEST( MultiRecoveryWithinOneChain3 );
	CPPUNIT_TEST( MultiRecoveryWithinOneChain4 );
	CPPUNIT_TEST( RecoveryWithTaking ); //������������� � ������� � ���������� ������ ������ �� ��������
	CPPUNIT_TEST( RecoveryWithTaking2 );
	CPPUNIT_TEST( RecoveryWithTaking3 );
	CPPUNIT_TEST( ForbidMultiPlacesRecovery ); //�������������� �� ���������� ��������� ������ ���������
	CPPUNIT_TEST( HeadPositioningAfterRecovery ); //������ ������ ������ �� ����������� ���������� ����� � ����� �������������� � �� �������� ������ � ������� ������ ����
	CPPUNIT_TEST( OddnessRecoveryForNoident ); //�������������� �������� ��������������������� ������
	CPPUNIT_TEST( DeathPlaceShouldBeRecordedInHistory ); //��� ������ ������ ��� ����� ������ ���� �������� � �������
	CPPUNIT_TEST( AppearanceVsRecovery ); //��������� �������������� ����� ���������
	CPPUNIT_TEST( NoTrainRecovery ); //�������������� ����������
	CPPUNIT_TEST( NoTrainRecovery2 );
	CPPUNIT_TEST( NoTrainRecoveryNearADWay ); //�������������� ���������� ����� ��-����
	CPPUNIT_TEST( NoRememberNoTrainOnSpan ); //��� ���� �� ��������� ���������� ���������� �� �������� - ��� �� ����������������� (��� ���������� ����������� ���������� ��������������)
	CPPUNIT_TEST( NoRememberSuddenSpanActivity ); //��������� ���������� �� �������� �� �������� � ����������� ����� �������� ������
	CPPUNIT_TEST( ForbidRecoveryAfterRouteSet ); //�������� ���������� � �������������� ����� ������� ��������
	CPPUNIT_TEST( FullStationExitAfterRecoveryOnADWay ); //��������� ������� ����� �������������� �� ��-����
	CPPUNIT_TEST_SUITE_END();

	void DiscordantMoveRecovery();
	void DiscordantJumpRecovery();
	void BoundDissapearanceUnrecovery();
	void RecoveryCleanAfterSetInfo();
	void SamePlaceRecovery();
	void StripJump();
	void RecoveryForbidness();
	void RecoveringInsteadMoving();
	void RecoveringInsteadMoving2();
	void RecoveringInsteadMoving3();
	void RecoveringAndCountraryMovingOnSingleStrip();
	void SameTrainFarPlacesRecovering();
	void RecoveryOnThreeStrips();
	void RecoveryOverBlockedStrips();
	void InterferringOfEmptyInfoRecovering();
	void RecoveryThroughLive();
	void TopologicallyUnreachableRecover();
	void RecoveringOnSinglePlaceUnity();
	void RecoveryVersusOccupation();
	void RecoveryVersusOccupation2();
	void PermittedRecoveryOverSwitch();
	void ForbiddenRecoveryOverSwitch();
	void DeathVersusDisform();
	void MultiRecovery();
	void MultiRecoveryWithinOneChain();
	void MultiRecoveryWithinOneChain2();
	void MultiRecoveryWithinOneChain3();
	void MultiRecoveryWithinOneChain4();
	void RecoveryWithTaking();
	void RecoveryWithTaking2();
	void RecoveryWithTaking3();
	void ForbidMultiPlacesRecovery();
	void HeadPositioningAfterRecovery();
	void OddnessRecoveryForNoident();
	void DeathPlaceShouldBeRecordedInHistory();
	void AppearanceVsRecovery();
	void NoTrainRecovery();
	void NoTrainRecovery2();
	void NoTrainRecoveryNearADWay();
	void NoRememberNoTrainOnSpan();
	void NoRememberSuddenSpanActivity();
	void ForbidRecoveryAfterRouteSet();
	void FullStationExitAfterRecoveryOnADWay();
};