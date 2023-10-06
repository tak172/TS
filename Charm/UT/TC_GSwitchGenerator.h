#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_GSwitchGenerator :
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_GSwitchGenerator );
	CPPUNIT_TEST( SwitchCreating );
	CPPUNIT_TEST( SwitchRecovering );
	CPPUNIT_TEST( SwitchNotRecovering );
	CPPUNIT_TEST( OrthoSwitchNotRecreating );
	CPPUNIT_TEST( OrthoSwitchRecreating );
	CPPUNIT_TEST( TriangleJunction );
	CPPUNIT_TEST_SUITE_END();

	void SwitchCreating(); //�������� �������
	void SwitchRecovering(); //�������������� ������� ����� ��������
	void SwitchNotRecovering(); //�� �������������� ������� ����� �������� ��������
	void OrthoSwitchNotRecreating(); //������ ������������ ������� ����� ����������� �������� (������ ����������������� ��������)
	void OrthoSwitchRecreating(); //������������ ������� ����� �������� (������ ����������������� ��������)
	void TriangleJunction(); //���������� �������� �������������
};