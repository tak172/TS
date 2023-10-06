#pragma once

//������ �� �������� ��������

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerRealStations : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerRealStations );
	CPPUNIT_TEST( JelgavaSequencyIntegrity );
	CPPUNIT_TEST( KarsavaRecoveryIntegrity );
	CPPUNIT_TEST( KarsavaRecoveryIntegrity2 );
	CPPUNIT_TEST( KarsavaRecoveryIntegrity3 );
	CPPUNIT_TEST( KraslavaSequencyIntegrity );
	CPPUNIT_TEST( KemeriOccupancyIntegrity );
	CPPUNIT_TEST( LiepajaSequencyIntegrity );
	CPPUNIT_TEST( LiepajaChainsIntegrity );
	CPPUNIT_TEST( LiepajaChainsIntegrity2 );
	CPPUNIT_TEST( LigatneChainsIntegrity ); //��������� ����������� ��� ��
	CPPUNIT_TEST( LigatneChainsIntegrity2 );
	CPPUNIT_TEST( LigatneChainsIntegrity3 );
	CPPUNIT_TEST( LigatneRecoveryIntegrity ); //��������� ������� ��� �������������� � ������ ��������
	CPPUNIT_TEST( LigatneRecoveryIntegrity2 );
	CPPUNIT_TEST( LigatneRecoveryIntegrity3 );
	CPPUNIT_TEST( LigatneRecoveryIntegrity4 );
	CPPUNIT_TEST( LigatneRecoveryIntegrity5 );
	CPPUNIT_TEST( LigatneRecoveryIntegrity6 );
	CPPUNIT_TEST( LigatneStripUniquenessIntegrity ); //������������ ��������� ������� � ������������ �����
	CPPUNIT_TEST( LigatneStripUniquenessIntegrity2 );
	CPPUNIT_TEST( LigatneCriticalIntentions ); //������ ��������� "�����������" ���������
	CPPUNIT_TEST( LigatneEmptyRecoveryPlace ); //������� �������������� �� ������ �����
	CPPUNIT_TEST( LigatneStripSequenceCorrectness ); //������ ������� ���������� �������� � ������
	CPPUNIT_TEST( LigatneStripSequenceCorrectness2 );
	CPPUNIT_TEST( LigatneInvalidTrainsExchange ); //��������� ����� ������� �������� �������
	CPPUNIT_TEST( LigatneOccupancyIntegrity );
	CPPUNIT_TEST( LigatneOccupancyIntegrity2 );
	CPPUNIT_TEST( LigatneOccupancyIntegrity3 );
	CPPUNIT_TEST( MezvidiRecoveryIntegrity );
	CPPUNIT_TEST( RigaLastPlaceLiberation ); //������������ ������� ���������� ����������� �������
	CPPUNIT_TEST( SlokaSequencyIntegrity );
	CPPUNIT_TEST( VangaziSequencyIntegrity );
	CPPUNIT_TEST( VangaziSequencyIntegrity2 );
	CPPUNIT_TEST( VangaziSequencyIntegrity3 );
	CPPUNIT_TEST( VangaziChainsIntegrity );
	CPPUNIT_TEST( VangaziChainsIntegrity2 );
	CPPUNIT_TEST( Rezekne2OccupancyIntegrity );
	CPPUNIT_TEST( Rezekne2OccupancyIntegrity2 );
	CPPUNIT_TEST( BaleException );
	CPPUNIT_TEST( BaleSequencyIntegrity );
	CPPUNIT_TEST_SUITE_END();

	void JelgavaSequencyIntegrity();
	void KarsavaRecoveryIntegrity();
	void KarsavaRecoveryIntegrity2();
	void KarsavaRecoveryIntegrity3();
	void KraslavaSequencyIntegrity ();
	void KemeriOccupancyIntegrity();
	void LiepajaSequencyIntegrity();
	void LiepajaChainsIntegrity();
	void LiepajaChainsIntegrity2();
	void LigatneChainsIntegrity();
	void LigatneChainsIntegrity2();
	void LigatneChainsIntegrity3();
	void LigatneRecoveryIntegrity();
	void LigatneRecoveryIntegrity2();
	void LigatneRecoveryIntegrity3();
	void LigatneRecoveryIntegrity4();
	void LigatneRecoveryIntegrity5();
	void LigatneRecoveryIntegrity6();
	void LigatneStripUniquenessIntegrity();
	void LigatneStripUniquenessIntegrity2();
	void LigatneCriticalIntentions();
	void LigatneEmptyRecoveryPlace();
	void LigatneStripSequenceCorrectness();
	void LigatneStripSequenceCorrectness2();
	void LigatneInvalidTrainsExchange();
	void LigatneOccupancyIntegrity();
	void LigatneOccupancyIntegrity2();
	void LigatneOccupancyIntegrity3();
	void MezvidiRecoveryIntegrity();
	void RigaLastPlaceLiberation();
	void SlokaSequencyIntegrity();
	void VangaziSequencyIntegrity();
	void VangaziSequencyIntegrity2();
	void VangaziSequencyIntegrity3();
	void VangaziChainsIntegrity2();
	void VangaziChainsIntegrity();
	void Rezekne2OccupancyIntegrity();
	void Rezekne2OccupancyIntegrity2();
	void BaleException();
	void BaleSequencyIntegrity();
};