#pragma once

//������ ��������� ����������� �������� �������

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerIntegrity : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();


private:
	CPPUNIT_TEST_SUITE( TC_TrackerIntegrity );
	CPPUNIT_TEST( RecoveringIntegrity );
	CPPUNIT_TEST( RecoveringIntegrity2 );
	CPPUNIT_TEST( RecoveringIntegrity3 );
	CPPUNIT_TEST( RecoveringIntegrity4 );
	CPPUNIT_TEST( RecoveringIntegrity5 );
	CPPUNIT_TEST( RecoveringIntegrity6 );
	CPPUNIT_TEST( RecoveringIntegrity7 );
	CPPUNIT_TEST( RecoveringIntegrity8 );
	CPPUNIT_TEST( RecoveringIntegrity9 );
	CPPUNIT_TEST( BreakingIntegrityByIdensOrder );
	CPPUNIT_TEST( SwitchToggleBreakIntegrity );
	CPPUNIT_TEST( BreakIntegrityOnMerging );
	CPPUNIT_TEST( BreakIntegrityOnMerging2 );
	CPPUNIT_TEST( ChainSplittingCausingUnintegrity );
	CPPUNIT_TEST( WrongCheckingLiveTrainWhileRecovery );
	CPPUNIT_TEST( BreakIntegrityOnMergingCausedByInfoChange );
	CPPUNIT_TEST( WrongTeleportOverBlockedStrips );
	CPPUNIT_TEST( FarTeleportWhilePathDeficite );
	CPPUNIT_TEST( TeleportIntegrity );
	CPPUNIT_TEST( BreakSequenceIntegrityDueToMultipath );
	CPPUNIT_TEST_SUITE_END();

	void RecoveringIntegrity();
	void RecoveringIntegrity2();
	void RecoveringIntegrity3();
	void RecoveringIntegrity4();
	void RecoveringIntegrity5();
	void RecoveringIntegrity6();
	void RecoveringIntegrity7();
	void RecoveringIntegrity8();
	void RecoveringIntegrity9();
	void BreakingIntegrityByIdensOrder(); //�������� ������� �������� ������� (������������� ���������������) �� ����������� ������ ������������
	void SwitchToggleBreakIntegrity(); //��������� ����������� ������ ����� �������� �������
	void BreakIntegrityOnMerging();
	void BreakIntegrityOnMerging2();
	void BreakIntegrityOnMergingCausedByInfoChange(); //��������� ����������� ��-�� ������� ������� � ���������� ���������� ����������
	void ChainSplittingCausingUnintegrity(); //������ ��������, ���������� � ����������� ������ � ��������� �����������
	void WrongCheckingLiveTrainWhileRecovery(); //��������� ����������� ������ ������ � �������� �������������� � ������� ��
	void WrongTeleportOverBlockedStrips();
	void FarTeleportWhilePathDeficite(); //��������� ������� ������ ��� ���������� � ������ ������������� ����� �������
	void TeleportIntegrity();
	void BreakSequenceIntegrityDueToMultipath(); //��������� ����������� ��� ����� � �������
};