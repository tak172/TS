#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"
#include "../StatBuilder/Router.h"

class CRouter;
class TC_Router : 
	public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_Router );
	CPPUNIT_TEST( SimpleBlindRoute ); 
	CPPUNIT_TEST( SimpleSemaRoute );
	CPPUNIT_TEST( OutrouteAlienEsr );
	CPPUNIT_TEST( OutrouteApproachingType );
	CPPUNIT_TEST( OutrouteApproachingTypeTwice );
	CPPUNIT_TEST( DoubleLegsInJointRoute );
	CPPUNIT_TEST( BlindRouteToOppositeLeg );
	CPPUNIT_TEST( ArrDepPermission );
	CPPUNIT_TEST( ArrDepForbidness );
	CPPUNIT_TEST( RouteThroughSwitch );
	CPPUNIT_TEST( InterstationsSemaroute );
	CPPUNIT_TEST( WOSemaStationRouteForbid );
	CPPUNIT_TEST( WOSemaStationRouteForbid2 );
	CPPUNIT_TEST( WOSemaStationRouteUnforbid );
	CPPUNIT_TEST( InterstatBoundSemaRoutes );
	CPPUNIT_TEST_SUITE_END();

protected:
	void SimpleBlindRoute(); //��������� ������� � ����� ��������
	void SimpleSemaRoute(); //����������� ������� � ����� ��������
	void OutrouteAlienEsr(); //�������� ������� (�� ������� � ������ ���-�����)
	void OutrouteApproachingType(); //�������� ������� �� ������� � ����� "�����������"
	void OutrouteApproachingTypeTwice(); //�������� ������� �� ��� ������� � ����� "�����������"
	void DoubleLegsInJointRoute(); //��� ��������� �������� �� ���� ���� �� ����� �����
	void BlindRouteToOppositeLeg(); //��������� ������� � ���������� �� ��������� �����
	void ArrDepPermission(); //�������� ��������� �� ������� � ������� �� �����
	void ArrDepForbidness(); //�������� ��������� �� ������� � ������� �� �����
	void RouteThroughSwitch(); //������� ����� �������
	void InterstationsSemaroute(); //����������� ������� ����� ���������
	void WOSemaStationRouteForbid(); //������ ����������� �������� ����� ������� ��� ����������
	void WOSemaStationRouteForbid2();
	void WOSemaStationRouteUnforbid(); //���������� �������� �� ������������� �����, ����������� �� �������� ��� ����������, �� �������� � ������� ����������� �������
	void InterstatBoundSemaRoutes(); //�������� �� ��������� �� ������� ���� �������

private:
	CRouter router;
};
