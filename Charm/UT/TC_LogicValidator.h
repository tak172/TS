#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"

class TC_LogicValidator :
	public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_LogicValidator );
	CPPUNIT_TEST( OneSidedLink );
	CPPUNIT_TEST( JointTransferring );
	CPPUNIT_TEST( RoutedJointTransferring );
	CPPUNIT_TEST_SUITE_END();

	void OneSidedLink();
	void JointTransferring();
	//���� ���������� �� ������� � ��������� ������� �������� � ������� ������ ������� (����� ��������� ��� ������ ����� �������):
	void RoutedJointTransferring();
};