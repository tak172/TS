#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"
#include "TC_Logic.h"

class TC_ObsoleteLoader : 
	private TC_Graphic,
	private TC_Logic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_ObsoleteLoader );
	CPPUNIT_TEST( ObsoleteSingleBoardBranch );
	CPPUNIT_TEST( ObsoleteGBoardBranch );
	CPPUNIT_TEST( ObsoleteSemaforsBranch );
	CPPUNIT_TEST( ObsoleteFontsRemoving );
	CPPUNIT_TEST( ObsoleteSwitchRadius );
	CPPUNIT_TEST( ObsoleteStationAttributes );
	CPPUNIT_TEST( LStripArrdepToWaynumStation );
	CPPUNIT_TEST( LStripArrdepToWaynumSpan );
	CPPUNIT_TEST( DeleteOecLogicLegType );
	CPPUNIT_TEST( DeleteLogLinkType );
	CPPUNIT_TEST( LStripCapacityToTwiceCapacity );
	CPPUNIT_TEST( DeleteJointsParityChanging );
	CPPUNIT_TEST_SUITE_END();

    void setUp() override;
    void tearDown() override;
protected:
	void ObsoleteSingleBoardBranch(); //�������� ����� SINGLE_BOARDS � ������ �� ������ TABLE_BOARDS
	void ObsoleteGBoardBranch(); //�������� ����� BOARDS � ������ �� ������� TABLE_BOARDS � MULTI_BOARDS (� ����������� �� ���� �������)
	void ObsoleteSemaforsBranch(); //�������� ����� SEMAFORS ��� ����� SEMALEGS
	void ObsoleteFontsRemoving(); //�������� ��������� ���������� �������
	void ObsoleteSwitchRadius(); //�������� ������� �������
	void ObsoleteStationAttributes(); //�������� ����������� ���������
	void LStripArrdepToWaynumStation(); //������ �������� �/� ���� �� ����� �/� ���� (� ������, ���� �� �������) [�� �������]
	void LStripArrdepToWaynumSpan(); //�������� �������� �/� ����. �������������� ��������[�� ��������]
	void DeleteOecLogicLegType(); //�������� ���� ���������� ����� (�������� �������� ��� ������� � ����� oec)
	void DeleteLogLinkType(); //�������� �������� ���� �� ���� ��� ����������� ����� (CLink)
	void LStripCapacityToTwiceCapacity(); //������ �������������� ����������� �� ����������� � ����� ������������
	void DeleteJointsParityChanging(); //�������� �������� ����� �������� �� ����������� �����
};