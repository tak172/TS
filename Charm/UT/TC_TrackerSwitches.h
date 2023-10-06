#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� �� ���������

class TC_TrackerSwitches : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerSwitches );
	CPPUNIT_TEST( Teleport );
	CPPUNIT_TEST( Teleport2 );
	CPPUNIT_TEST( BidTeleport );
	CPPUNIT_TEST( BidTeleport2 );
	CPPUNIT_TEST( Occupation );
	CPPUNIT_TEST( Occupation2 );
	CPPUNIT_TEST_SUITE_END();

	void Teleport(); //�������� ����� ���������� ������� ������ ��������������, ��� ����� ���������
	void Teleport2();
	void BidTeleport();
	void BidTeleport2();
	void Occupation(); //������� ����������� ������� ������ �����������, ��� ������� ����������
	void Occupation2();
};