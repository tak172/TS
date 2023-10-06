#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_LOS_Route : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_Route );
  CPPUNIT_TEST( setTrainRouteByAllLocked );
  CPPUNIT_TEST( noTrainRouteByLockedNBusy );
  CPPUNIT_TEST( setShuntRouteByAllLocked );
  CPPUNIT_TEST( setShuntRouteByLockedNBusy );
  CPPUNIT_TEST_SUITE_END();

protected:
	void setTrainRouteByAllLocked(); //��������� ��������� �������� �� ���� ��������� ��������
	void noTrainRouteByLockedNBusy(); //��� ��������� ��������� �������� �� ��������� ���������� �������� � �������� ���������� ��-����
	void setShuntRouteByAllLocked(); //��������� ����������� �������� �� ���� ��������� ��������
	void setShuntRouteByLockedNBusy(); //��������� ����������� �������� �� ��������� ���������� �������� � �������� ���������� ��-����
};