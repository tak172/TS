#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"
#include "../Guess/TrackerEvent.h"
#include "../Guess/TrainUnity.h"

//�������� ����������� �� �� "��������" ��������
class TC_TrackerPockets : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerPockets );
	CPPUNIT_TEST( BusyInsensibility ); //"���������" ������� �� ������������� � �������� ���������
	CPPUNIT_TEST( FreeInsensibility ); //"���������" ������� �� ������������� � �������� �����������
	CPPUNIT_TEST( TrainKeeping ); //���������� �� �� �������� � ����� "���������"
	CPPUNIT_TEST( MultiTrains ); //�������� ����� "�������" ���������� ��
	CPPUNIT_TEST( ResetInPockets ); //����� ��������� (� �.�. �������� ���� ��) � ������ ���������� �� � "�������"
	CPPUNIT_TEST( LongTrainMoveToPocket ); //����� � "������" ��, ����������� ��������� ��������
	CPPUNIT_TEST( LongTrainMoveOutPocket ); //����� �� "�������" ��, ����� �� ��������� ��������
	CPPUNIT_TEST( SetInfo ); //���������� ���������� ������, ������������ � "�������"
	CPPUNIT_TEST( BidirectOut ); //"�����" �� ������� ����� � ����� ������������
	CPPUNIT_TEST( BecomeTrainOnEntry ); //������������ ������ ����� ������� � "������"
	CPPUNIT_TEST( DeleteTrain ); //�������� ������, ������������ � "�������"
	CPPUNIT_TEST( HeterogeneousMoving ); //��������� � ����������� �������� ������������ (����������� ����)
	CPPUNIT_TEST( DeleteAmongMany ); //�������� �� �������� ��������� ������� � "�������"

	//���� ������� ��-�� ������� ������ ������� ����� ��� ������������ (���������������� ������� ���������)
	//CPPUNIT_TEST( MultiTrainsPositioning ); //������������ ���������� �� � "�������"

	CPPUNIT_TEST( LongPocketMoving ); //������ ����� ������� "������". ������ ������� ������� ������������ ������� �����
    CPPUNIT_TEST( JointPicketage ); //�������� �������� ������ ��� ���������� �����
	CPPUNIT_TEST( FalsePocket ); //������������� ������-�������
	CPPUNIT_TEST( ForbidPocketJumping ); //������ �� "��������������" ����� ������
	CPPUNIT_TEST( OverflowShort ); //������������ ������� �� ������ �������
	CPPUNIT_TEST( OverflowLong ); //������������ �������� �������
	CPPUNIT_TEST( PocketEntryVsDisform ); //����� ����� ������ � ������ � ����������������
	CPPUNIT_TEST( JumpOutOfPocket ); //"������������" �� ������� ����� �������
	CPPUNIT_TEST( JumpOutVsExit ); //����� ����� "�������������" �� ������� ����� ������� � ������� �������
	CPPUNIT_TEST( PocketExitAsSoonAsPossible ); //����� �� ������� �� ����������� ���������
	CPPUNIT_TEST_SUITE_END();

	void BusyInsensibility();
	void FreeInsensibility();
	void TrainKeeping();
	void MultiTrains();
	void VerifyHemPath( const std::list <TrackerEventPtr> & );
	TrainUnityCPtr MoveThrow( std::wstring apprstrip1, std::wstring apprstrip2 ); //��������� �� �� 1 ��-��, ����� �� 2 ��-� � ����������
	TrainUnityCPtr MoveThrow( std::wstring apprstrip1, std::wstring apprstrip2, std::wstring apprstrip3 ); //���������� ����� ��� �������
	void ResetInPockets();
	void LongTrainMoveToPocket();
	void LongTrainMoveOutPocket();
	void SetInfo();
	void BidirectOut();
	void BecomeTrainOnEntry();
	void DeleteTrain();
	void HeterogeneousMoving();
	void DeleteAmongMany();
	void MultiTrainsPositioning();
	void LongPocketMoving();
    void JointPicketage();
	void FalsePocket();
	void ForbidPocketJumping();
	void OverflowShort();
	void OverflowLong();
	void PocketEntryVsDisform();
	void JumpOutOfPocket();
	void JumpOutVsExit();
	void PocketExitAsSoonAsPossible();
};