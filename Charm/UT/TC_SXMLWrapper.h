#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Signal.h"

class TC_SXMLWrapper : 
	private TC_Signal,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_SXMLWrapper );
	CPPUNIT_TEST( BasicSave );
	CPPUNIT_TEST( SaveLts );
	CPPUNIT_TEST( SaveVirtual );
	CPPUNIT_TEST( SaveFormula );
	CPPUNIT_TEST( BasicLoad );
	CPPUNIT_TEST( LoadLts );
	CPPUNIT_TEST( LoadVirtual );
	CPPUNIT_TEST( LoadFormula );

	CPPUNIT_TEST_SUITE_END();

protected:
	void BasicSave(); //���������� ����� ����� (�������, �������, ������)
	void SaveLts(); //���������� Lts-�������
	void SaveVirtual(); //���������� ������������ ������� (Ebiloc)
	void SaveFormula(); //���������� �������
	void BasicLoad();
	void LoadLts();
	void LoadVirtual();
	void LoadFormula();
};