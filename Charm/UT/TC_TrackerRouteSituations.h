#pragma once

//������ ������������ �� �������/������ ���������

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerRouteSituations : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerRouteSituations );
	CPPUNIT_TEST( ParkMigrationByShortShuntRoute );
	CPPUNIT_TEST( ParkMigrationByShortTrainRoute );
	CPPUNIT_TEST( ParkMigrationWithoutRoutes );
	CPPUNIT_TEST( ParkMigrationByOnOffTrainRoute );
	CPPUNIT_TEST( SplitIntoSettledRoute );
	CPPUNIT_TEST( ParkMigrationOnCuttingRoute );
	CPPUNIT_TEST( UnsetRouteWithBPMigrations );
	CPPUNIT_TEST( UnsetRouteWithBPMigrations2 );
	CPPUNIT_TEST( SimultHeadCloseAndADOut );
	CPPUNIT_TEST( DepartureWithIrregularBusyOnRoute );
	CPPUNIT_TEST_SUITE_END();

	//������� ������ � ���� �� �������� ����������� ��������
	void ParkMigrationByShortShuntRoute();
	//������� ������ � ���� �� �������� ��������� ��������
	void ParkMigrationByShortTrainRoute();
	//������� ������ � ���� ��� ���������
	void ParkMigrationWithoutRoutes();
	//������� ������ � ���� ����� ��������� � ������ ��������
	void ParkMigrationByOnOffTrainRoute();
	//����������� ������ � ������� ������� �� ��������� ��������
	void SplitIntoSettledRoute();
	//������� ������ � ���� �� ������������� ��������
	void ParkMigrationOnCuttingRoute();
	//������ �������� ��� �������� �� ������� �� ����-����
	void UnsetRouteWithBPMigrations(); //�������� �������� (������ �������� � ������� ��������� ������������)
	void UnsetRouteWithBPMigrations2(); //�� �������� ��������
	void ParkMigration( const std::vector <std::wstring> & routeNames, const std::vector <std::wstring> & throughPlaces, bool mustBeTrain );
	void SimultHeadCloseAndADOut(); //������������� �������� ��������� � ������� � ��-����
	void DepartureWithIrregularBusyOnRoute(); //����������� �� �������� � ������������� ���������� ��������
};