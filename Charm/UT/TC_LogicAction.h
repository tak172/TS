#pragma once

#include "TC_Project.h"

class TC_LogicAction : public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_LogicAction );
	CPPUNIT_TEST( JointedStripChanging );
	CPPUNIT_TEST( GLinkedStripChanging );
	CPPUNIT_TEST( JointedStripRemoving );
	CPPUNIT_TEST( ViewChangesOnHeadModify );
	CPPUNIT_TEST_SUITE_END();

	//��������� ���. �������, ���������� �� ������:
	void JointedStripChanging();
	//��������� ���. �������, �� ������� ���������� ������ �� ������������ ���������:
	void GLinkedStripChanging();
	//�������� ������� ���������� �� ������
	void JointedStripRemoving();
	//�������� ��������� ��� ����������� ������ ���������� �������� ��� ����������� ������
	void ViewChangesOnHeadModify();
};