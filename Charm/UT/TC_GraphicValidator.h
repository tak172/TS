#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"

class TC_GraphicValidator :
	public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_GraphicValidator );
	CPPUNIT_TEST( CorrectUnmatchedSwitchesBDef );
	CPPUNIT_TEST( CorrectUnmatchedSwitchesBUndef );
	CPPUNIT_TEST( NoCorrectLSwitchWOLinks );
	CPPUNIT_TEST( SemaWithLegNAlienHeadsLogic );
	CPPUNIT_TEST( SemaWOLegOneHeadLogic );
	CPPUNIT_TEST( SemaWOLegManyHeadLogic );
	CPPUNIT_TEST( SemaWOLegManyTrainHeadLogic );
	CPPUNIT_TEST_SUITE_END();

	void CorrectUnmatchedSwitchesBDef(); //�� ����������� ������� ���������� ���� � ����� (��� ������������ ������� �������)
	void CorrectUnmatchedSwitchesBUndef(); //�� ����������� ������� ���������� ���� � ����� (��� �������������� ������� �������)
	void NoCorrectLSwitchWOLinks(); //���������� ��������� ��� ���������� ������� ��� ������
	void CorrectUnmatchedSwitches( bool base_defined );

	//�� ����������� ��������� ��������� �����, � ���� ��� ��������� ����� ��������� ��������, �������� � ������ ���������� �������� (����������� ����� �����):
	void SemaWithLegNAlienHeadsLogic();
	//�� ����������� ��������� ��������� ������ ���� ������:
	void SemaWOLegOneHeadLogic();
	//�� ����������� ��������� ��������� ��������� ����������� ����� �� ������ ���������� ����������:
	void SemaWOLegManyHeadLogic();
	//�� ����������� ��������� ��������� ��������� �������� ����� �� ������ ���������� ����������:
	void SemaWOLegManyTrainHeadLogic();
};