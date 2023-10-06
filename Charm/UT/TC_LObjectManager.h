#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"

class TC_LObjectManager : public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_LObjectManager );
	CPPUNIT_TEST( GenerateRouteAlienLastJoint );
	CPPUNIT_TEST( JointNameChanging );
	CPPUNIT_TEST( JointConflictNameChanging );
	CPPUNIT_TEST( ChangeStripNameToSame );
	CPPUNIT_TEST( RouteUndependencyToCommutatorRemove );
	CPPUNIT_TEST( RouteDependencyToAlienStrip );
	CPPUNIT_TEST( RenameHeadIntoRoute );
	CPPUNIT_TEST( RenameRoutesCommonFinalStrip );
	CPPUNIT_TEST( InterstationRouteAndCommutator );
	CPPUNIT_TEST_SUITE_END();

	void GenerateRouteAlienLastJoint(); //��������� �������� � ��������� ������ �� �������� �������
	void JointNameChanging(); //�������� ����� ����� �����
	void JointConflictNameChanging(); //�������� ����� ����� ����� ��� ��������� ����
	void ChangeStripNameToSame(); //��������� ����� ������� �� ������ �� �� ��
	void RouteUndependencyToCommutatorRemove(); //�������� ����������� �� ������ ����������� ������������� ��������
	void RouteDependencyToAlienStrip(); //�������� �������, ��������� � ����� �������, ������ ��������� �������
	void RenameHeadIntoRoute(); //�������������� ������� ���������, ��������� � �������
	void RenameRoutesCommonFinalStrip(); //�������������� ���������� �������, ��������� � ��������� ���������
	void InterstationRouteAndCommutator(); //�������������� ������� �� ��������� ������������ �� ����� �������
};