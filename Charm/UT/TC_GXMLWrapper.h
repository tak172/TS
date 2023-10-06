#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_GXMLWrapper : 
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_GXMLWrapper );
	CPPUNIT_TEST( SaveLoadOrthoSwitches );
	CPPUNIT_TEST( SaveLoadOrthoTwoStrips );
	CPPUNIT_TEST( SaveLoadOrthoThreeStrips );
	CPPUNIT_TEST( NullLengthStripLoad );
	CPPUNIT_TEST( SaveLoadSofAttributes );
	CPPUNIT_TEST( CleanRootAttributeDuplicates );
	CPPUNIT_TEST_SUITE_END();

protected:
	void SaveLoadOrthoSwitches(); //����������/�������� ������� ��� ������ �����
	void SaveLoadOrthoTwoStrips(); //����������/�������� ���� �������� ��� ������ �����
	void SaveLoadOrthoThreeStrips(); //����������/�������� ���� �������� ��� ������ �����
	void NullLengthStripLoad(); //�������� �������� ������� ������� �����
	void SaveLoadSofAttributes(); //����������/�������� ���������� �����
	void CleanRootAttributeDuplicates(); //�������� ����������� ���������

	void CleanAll(); //�������� ��������� CGraphicDocument
};
