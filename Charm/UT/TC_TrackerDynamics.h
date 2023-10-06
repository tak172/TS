#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ������������ �� � ������ �������� �������� (������� � ��������� �������������)

class TC_TrackerDynamics : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerDynamics );
	CPPUNIT_TEST( StationEntry );
	CPPUNIT_TEST( HistoryDepth );
	CPPUNIT_TEST( Declutching );
	CPPUNIT_TEST( OpenWayDeclutching );
	CPPUNIT_TEST( OpenWayDeclutchingWithDelayedSema );
	CPPUNIT_TEST( OpenWayExitAndDeclutchingCloseSema );
	CPPUNIT_TEST( CloseWayDeclutching );
	CPPUNIT_TEST( OpenShuntingDeclutching );
	CPPUNIT_TEST( WOSemaWayDeclutching );
	CPPUNIT_TEST( StationExitMissingArrdep );
	CPPUNIT_TEST( SpanDeclutching );
	CPPUNIT_TEST( SpanDeclutching2 );
	CPPUNIT_TEST( AnotherOddnessCoSemaTransit );
	CPPUNIT_TEST( AnotherOddnessCoSemaFastTransit );
	CPPUNIT_TEST( AnotherOddnessOppSemaTransit );
	CPPUNIT_TEST( ReversiveMoveAfterPartialArrive );
	CPPUNIT_TEST( LongTrainAtypicalStationExit );
	CPPUNIT_TEST( MovingPriority );
	CPPUNIT_TEST( DeclutchingAfterTurn );
	//CPPUNIT_TEST( InstantDoubleTrainsSpanMove );
	CPPUNIT_TEST( InstantMixedUnitiesTightMove );
	CPPUNIT_TEST( TrainJumpToNoTrain );
	CPPUNIT_TEST( TrainJumpToNoTrainOverBlockedPart );
	CPPUNIT_TEST( TrainJumpBidToNoTrainOverBlockedPart );
	//CPPUNIT_TEST( TrainJumpToNoTrainOverSwitchSection );
	//CPPUNIT_TEST( TrainJumpBidToNoTrainOverSwitchSection );
	CPPUNIT_TEST( SpanMotionVsArrdep );
	CPPUNIT_TEST( StatMotionVsPresence );
	CPPUNIT_TEST( StatMotionVsArrdep );
	//CPPUNIT_TEST( HeadMovingsOnSpanOnlyForward );
	CPPUNIT_TEST( SpanMotionVsKeepingLastPlace );
	CPPUNIT_TEST( ChoosingTrainByMoveDirection );
	CPPUNIT_TEST( TwoSideTrainObstacles );
	CPPUNIT_TEST( FormAndDepartureFromObscurity );
	CPPUNIT_TEST( TailBusyOnSpan );
	CPPUNIT_TEST( TailBusyOnSpan2 );
	//CPPUNIT_TEST( TailBusyOnSpan3 );
	CPPUNIT_TEST( GoOutFromStationAfterRecover );
	CPPUNIT_TEST_SUITE_END();

	void StationEntry(); //����� �� �������
	void HistoryDepth(); //�������� �� ������� ������� �������� �� ����������� �������
	void Declutching(); //������� ���������� ��
	void OpenWayDeclutching(); //���������� ������ ����� ������� � �/� ���� �� �������� ��������
	void OpenWayDeclutchingWithDelayedSema(); //���������� ������ ����� ������� � �/� ���� �� �������� �������� � ��������� ������� �� ��� ����������� ��������
	void OpenWayExitAndDeclutchingCloseSema(); //����� ������ � �� ���� �� �������� ��������, �������� ��������� � ��������� ������
	void CloseWayDeclutching(); //���������� ������ ����� ������� � �/� ���� �� �������� ��������
	void OpenShuntingDeclutching(); //������ ���������� �� ������ �� �� ���� �� �������� ���������� ��������
	void WOSemaWayDeclutching(); //���������� ������ ����� ������� � �/� ���� ��� ���������� ���������
	void StationExitMissingArrdep(); //����� �� ������� (��� ������� �/� �����)
	void SpanDeclutching(); //����� ������ �� ������� � ����������� ������� ���������� �� �������
	void SpanDeclutching2();
	void AnotherOddnessCoSemaTransit(); //������ ������ ����� �������� �������� �� ���������� ��������
	void AnotherOddnessCoSemaFastTransit(); //������� ������ (� �������� ����� ���� ��������) ����� �������� �������� �� ���������� ��������
	void AnotherOddnessOppSemaTransit(); //������ ������ ����� ��������� �������� �� ���������� ��������
	void ReversiveMoveAfterPartialArrive(); //�������� �������� ����� ���������� ������� �/� ����
	void LongTrainAtypicalStationExit(); //��������������� ����� �������� ������ � �/� ���� �� ������� (� ����������� ��������� �� �/� ����)
	void MovingPriority(); //��������� ���������� ������ �� �������� ��������
	void DeclutchingAfterTurn(); //���������� ������ ����� ��� ��������� (��� ���������� �� �/� ���� � ������� ��������� ���������)
	void InstantDoubleTrainsSpanMove(); //������ ������� ����� ���� ������� �������� ���� ������� � ���� ������ �������
	void InstantMixedUnitiesTightMove(); //������ ����� ���� ������� ������ � �� ������ � ���� ������ ������� (����� ���� ���������). �������� ���� ������� �������
	void TrainJumpToNoTrain(); //������ ������ ����� ������� � ���������� (���������� �� �����������) 
	void TrainJumpToNoTrainOverBlockedPart(); //������ ������ ����� ������� � ���������� (���������� �� �����������) ����� ����-�������
	void TrainJumpBidToNoTrainOverBlockedPart(); //������ ������ ������ ����� ������� � ���������� ����� ����-�������
	void TrainJumpToNoTrainOverSwitchSection(); //������ ������ ����� ������� � ���������� ����� ���������� ������
	void TrainJumpBidToNoTrainOverSwitchSection(); //������ ������ ������ ����� ������� � ���������� ����� ���������� ������
	void SpanMotionVsArrdep(); //����� ����� ��� ������ � ����� ������ �������� �������� �� ������� � ����������� �� ��-����� (��������� - ��������)
	void StatMotionVsPresence(); //����� ����� ��� ������ � ����� ������ �������� �������� �� ������� � �������� ����������� (��������� - ��������)
	void StatMotionVsArrdep(); //����� ����� ��� ������ � ����� ������ �������� �������� �� ������� � ����������� �� ��-����� (��������� - ��-�����)
	void HeadMovingsOnSpanOnlyForward(); //������ ������ ��� �������� �� �������� ����� ��������� ������ ������
	void SpanMotionVsKeepingLastPlace(); //����� ����� ��� ������ � ����� ������ �������� �������� �� �������� � ������ �� �����, ������������ ��� ������� ������ (��������� - ���������� ����� �������)
	void ChoosingTrainByMoveDirection(); //���������� ����� ������ ��� ������� � ����� ������ ����������� ��������
	void TwoSideTrainObstacles(); //������������ ����������� ��� ������ (����� �������� � ��������� �������� ����������� ������)
	void FormAndDepartureFromObscurity(); //������������ � ����������� ������ �� ���������������� ���� � ����� �� �������

	void TailBusyOnSpan(); //��������� �� �������� � ������ ���������� ���������� (� �������� ������������ ��������� ������� �� �������)
	void TailBusyOnSpan2(); //���� ��������� � ������ ����� �������, ��� �� ����� ��������� ����� �� ������� (� �������� ������������ ��������� ������� �� �������)
	void TailBusyOnSpan3(); //������ ��������� � ������� ������, ��������� �� ��������� ���������� (� �������� ������������ ��������� ������� �� �������)
	void GoOutFromStationAfterRecover(); //����� �� ������� ����� ��������������
};