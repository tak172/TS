#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ������������ ��������� ������� ������������ ��� �������� �� Hem

class TC_TrackerEvents : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerEvents );
	CPPUNIT_TEST( SpanToSpanByOrdInc );
	CPPUNIT_TEST( TwoTrainsByInc );
	CPPUNIT_TEST( OneTrainByUnordInc );
	CPPUNIT_TEST( OccupationOwnStrip );
	CPPUNIT_TEST( StationToStation );
	CPPUNIT_TEST( PocketMove );
	CPPUNIT_TEST( JointCoordinates );
	CPPUNIT_TEST( ParkWayNumbers );
	CPPUNIT_TEST( NoTrainDeath );
    CPPUNIT_TEST( BanCoordinatesAtStaion );
	CPPUNIT_TEST( YieldNumber );
    CPPUNIT_TEST( RepeatInfoChanging );
	CPPUNIT_TEST( LightFadeArrdep );
	CPPUNIT_TEST( EntryInfoChangeExit );
	CPPUNIT_TEST( UTurnAfterStationPartialEntryVar1 );
	CPPUNIT_TEST( UTurnAfterStationPartialEntryVar2 );
	CPPUNIT_TEST( HeadPlaceInDeath );
	CPPUNIT_TEST( FlashLeaving );
	CPPUNIT_TEST( PicketageOnForm );
	CPPUNIT_TEST( FaultStickingTrainKnowledge );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST( DisformTimeOnReachingEndOfStation );
	CPPUNIT_TEST( SameTimeEvents );
	CPPUNIT_TEST_SUITE_END();

	void SpanToSpanByOrdInc(); //����������� �� �� �������� � �������� �� ������������� ������������ ��������
	void TwoTrainsByInc(); //������������� ���� �� �� ������������ ��������
	void OneTrainByUnordInc(); //������������� ����� �� �� ��������������� ������������ ��������
	void OccupationOwnStrip(); //������� �� ��������, ��� �������� � �� ������
	void StationToStation(); //������� �� ������� �� �������
	void PocketMove(); //����/����� �� "�������"
	void JointCoordinates(); //������� ��������� � �������� ��� ������� ����� ����� � ������������
	void ParkWayNumbers(); //������� ������� ����� � ������ ��� �/� �����
	void NoTrainDeath(); //��� ������������ "�� ������" ������� DEATH �� ������������
    void BanCoordinatesAtStaion(); //��� ����������� ������� �� ���������� ���������� (���� ���� ��� ����)
	void YieldNumber(); //���������� ��������� ������� ��� ������� ������ (���������� ������������� ������ ������� ������)
    void RepeatInfoChanging(); //���������� ��������� ������� ��� ��������� ��������� ���� �� ������ (���������� ���� ���. �����)
	void LightFadeArrdep(); //��������� � ������� �/� ����
	void EntryInfoChangeExit(); //����� ���������� �� ������ ����� ������ � ������� �� �������
	void UTurnAfterStationPartialEntryVar1(); //�������� ������ ����� ���������� ������ �� ������� (������� 1)
	void UTurnAfterStationPartialEntryVar2(); //�������� ������ ����� ���������� ������ �� ������� (������� 2)
	void HeadPlaceInDeath(); //� ������� ������ ������ ������ ����������� ����� ������ ������
	void FlashLeaving(); //��������� ������ �/� ���� � ������������� �������� ���� ���������� ��������
	void CheckUTurnEvents();
	void PicketageOnForm(); //������� � ������� ������������ �� ����� ������� ����
	void FaultStickingTrainKnowledge(); //���������� � ������� ������ �� ������� �������� �������� ��� ������������ ������ �� �������� ����� ������� �� ��������
	void Serialize();
	void Deserialize();
	void DisformTimeOnReachingEndOfStation(); //���������� ����������� �������� ��������������� ��� ���������� ������� �������
	void SameTimeEvents(); //��-�� ���������� �������� ������� �� ������ �������������� ������� � ����������� ���������
};