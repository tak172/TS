#pragma once

#include "TC_Project.h"

class TC_LinkEditing : public TC_Project
{
public:
    void tearDown() override;
private:
	CPPUNIT_TEST_SUITE( TC_LinkEditing );
	CPPUNIT_TEST( LStripUnlinking );
	CPPUNIT_TEST( LHeadUnlinking );
	CPPUNIT_TEST( LCommutatorUnlinking );
	CPPUNIT_TEST( LJointUnlinking );
	CPPUNIT_TEST( LStripLSwitchBreaking );
	CPPUNIT_TEST_SUITE_END();

	//������ � �������
	void LStripUnlinking(); //������������ ����������� �������
	void LHeadUnlinking(); //������������ ���������� ������
	void LCommutatorUnlinking(); //������������ ����������� �����������
	void LJointUnlinking(); //������������ ����������� �����
	void LStripLSwitchBreaking(); //������������ ����������� ������� � �������
};