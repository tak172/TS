#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//������������� �������

class TC_TrackerTrainRecognition : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainRecognition );
	CPPUNIT_TEST( RouteSetExit );
	CPPUNIT_TEST( OpenAlienHeadExit );
	CPPUNIT_TEST( CloseHeadExit );
	CPPUNIT_TEST( FromStationToSpan );
	CPPUNIT_TEST( SpanMove );
	CPPUNIT_TEST( FormByInfoEvent );
	CPPUNIT_TEST( MoveTrainStationToStation );
	CPPUNIT_TEST( UpgradeNoTrainWhileGoToAlienStation );
	CPPUNIT_TEST( NoUpgradeNoTrainWhileGoToFriendStation );
	CPPUNIT_TEST( NoFormByTrainRoute );
	CPPUNIT_TEST( NoFormByTrainRoute2 );
	CPPUNIT_TEST( NoFormByShuntingRoute );
	CPPUNIT_TEST( NoFormByOnlyTrainHead );
	CPPUNIT_TEST_SUITE_END();

	void RouteSetExit(); //����� �� ��������� �������� -> ����� ���������
	void OpenAlienHeadExit(); //����� �� ��������� ������� ������ ��������� ����� �������� -> ����� �� ���������
	void CloseHeadExit(); //����� ���������� �� ��������� ������� ������ ��������� -> ����� �� ���������
	void FromStationToSpan();
	void SpanMove();
	void FormByInfoEvent();
	void MoveTrainStationToStation(); //����������� ������ �� ������� �� ������� �������
	void UpgradeNoTrainWhileGoToAlienStation(); //��������� ���������� �� ������ ��� �������� �� ������� �� ����� �������
	void NoUpgradeNoTrainWhileGoToFriendStation(); //���������� ������� ���������� ��� �������� �� ������������� �������, �������� � ������������ ��������
	void NoFormByTrainRoute(); //�� ��������� �������� ����� �� �����������
	void NoFormByTrainRoute2();
	void NoFormByShuntingRoute(); //�� ����������� �������� ����� �� �����������
	void NoFormByOnlyTrainHead(); //������ �� ��������� ��������� ��������� (��� ��������) ����� �� �����������
};