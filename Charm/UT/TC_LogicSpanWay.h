#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"


class TC_LogicSpanWay : public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_LogicSpanWay );
	CPPUNIT_TEST( NonullLengthWay );
	CPPUNIT_TEST( UndefLengthWay );
	CPPUNIT_TEST( LengthMiniWayS );
	CPPUNIT_TEST( LengthMiniWayJ );
	CPPUNIT_TEST( LengthMiniWaySJ );
	CPPUNIT_TEST( LengthMiniWaySJS );
	CPPUNIT_TEST( LengthMiniWayJSJ );
	CPPUNIT_TEST_SUITE_END();


protected:
	void NonullLengthWay(); //���� �������� � ��������� ������
	void UndefLengthWay(); //���� �������� � �������������� ������
	//��������� ���������� ����:
	void LengthMiniWayS(); //�������
	void LengthMiniWayJ(); //����
	void LengthMiniWaySJ(); //������� - ����
	void LengthMiniWaySJS(); //� �.�.
	void LengthMiniWayJSJ();
};