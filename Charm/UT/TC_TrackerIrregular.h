#pragma once

//�������� ��������� ������������� �������� � ������������ ��������

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerIrregular : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerIrregular );
	CPPUNIT_TEST( UnlinkedSpanStrip ); //���������� �������, �� ��������� �� � ���, �� �������������
	CPPUNIT_TEST( DiscordantMoveMerging ); //������� ������ � �� ��� ��������������� �������� �� ��������
	CPPUNIT_TEST( AbnormalLightningOnSwitch ); //������������ ��������� �� �������
	CPPUNIT_TEST( AbnormalLightningOnSwitch2 ); //������������ ��������� �� �������
	CPPUNIT_TEST( TrainVsNotrainDisappearance ); //������� ������ ������� ����� �� (����� ����� ������� � �� �������)
	CPPUNIT_TEST( ChangesAfterCrush ); //�������� ��������� ����� ������ (������) ������ ������������
	CPPUNIT_TEST( TrainContentVsDirectionDisappearance ); //������� ������ ������� ����� �� (��������� ����� ������������ �������� ����� ������� ���������� � ������)
    CPPUNIT_TEST( DoubleArrivingByReturn ); //������������ ������� �������� ��� "�����������" �� �� �/� ���� (�� ���� ������� �/� ����)
	CPPUNIT_TEST( WreckingADPassing ); //������������ ������ ����� �/� ���� (�����, ������ �� �������� ��������, ���������� � ���������� �����������)
	CPPUNIT_TEST( DeathRecoverAndErrorStopping ); //�������������� ������ ����� ������������ � ��������� ��������� ������� ����� �������
	CPPUNIT_TEST( AsyncEraseDisappeared ); //����������� �������� ������������ ������ �� ��������� ����������� ������ �������
	CPPUNIT_TEST( TransitionReturnExit ); //������������� �/� ����, ����������� � ����� �� �������
	CPPUNIT_TEST( BlinkSeveralEntranceAfterArrival ); //������� ���������� �������� �������� ����� �������� ������ �� �/� ����
	CPPUNIT_TEST( InternalUnlightWithNeighbour ); //������� �������� ������ ��� ������� �� ��������� ��� ����� ��
	CPPUNIT_TEST( TrainPassThroughFakeBusy ); //������ ������ ����� ������� � ������ ����������
	CPPUNIT_TEST( ReversiveMoveOnSpan ); //�������� �������� ������ �� ��������
	CPPUNIT_TEST( ReversiveMoveOnSema ); //�������� �������� ������ ����� ��������
	CPPUNIT_TEST( MultiSectionFaultBusy ); //������ ��������� �� ���� ���������� ������ (��� ����������)
	CPPUNIT_TEST( TrainThroughFaultFreeOnSpan ); //������ ������ ������ ����� ������ ����������� (�� ��������)
	CPPUNIT_TEST( TrainThroughFaultFreeOnADOut ); //������ ������ ������ ����� ������ ����������� (����� � �� ����)
	CPPUNIT_TEST( TrainThroughFaultFreeOnShuntSema ); //������ ������ ������ ����� ������ ����������� (������ ����� ���������� ��������)
	CPPUNIT_TEST( TrainThroughFaultFreeOnShuntSema2 ); //������ ������ ������ ����� ������ ����������� (������ ����� ���������� ��������)
	CPPUNIT_TEST( TwoTrainsThroughFaultFree ); //������ ���� ������� ����� ������ �����������
	CPPUNIT_TEST( InsensitivityToInsignificantFree ); //������ �� �������� ����������� �� ������ ��������� � ��������� ����� ��
	CPPUNIT_TEST( BlinkingOnActiveRoute ); //�������� ��������� �� ����� ��������� �������� (����� ����� ���������)
	CPPUNIT_TEST_SUITE_END();

	void UnlinkedSpanStrip();
	void DiscordantMoveMerging();
	void AbnormalLightningOnSwitch();
	void AbnormalLightningOnSwitch2();
	void TrainVsNotrainDisappearance();
	void ChangesAfterCrush();
	void TrainContentVsDirectionDisappearance();
    void DoubleArrivingByReturn();
	void WreckingADPassing();
	void DeathRecoverAndErrorStopping();
	void AsyncEraseDisappeared();
	void TransitionReturnExit();
	void BlinkSeveralEntranceAfterArrival();
	void InternalUnlightWithNeighbour();
	void TrainPassThroughFakeBusy();
	void ReversiveMoveOnSpan();
	void ReversiveMoveOnSema();
	void MultiSectionFaultBusy();
	void TrainThroughFaultFreeOnSpan();
	void TrainThroughFaultFreeOnADOut();
	void TrainThroughFaultFreeOnShuntSema();
	void TrainThroughFaultFreeOnShuntSema2();
	void TwoTrainsThroughFaultFree();
	void InsensitivityToInsignificantFree();
	void BlinkingOnActiveRoute();
};