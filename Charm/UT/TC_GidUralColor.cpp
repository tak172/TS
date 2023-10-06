#include "stdafx.h"

#include "TC_GidUralColor.h"
#include "../helpful/Attic.h"
#include "../GUI/InfoTypesClassificator.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TC_GidUralColor);

void TC_GidUralColor::Load()
{
    const std::wstring str = 
		L"<ColorList>"
		L"<Color code='0' name='������' rgb='0x00000000' />"
		L"<Color code='1' name='�����' rgb='0x000000FF' />"
		L"<Color code='2' name='�������' rgb='0x00008000' />"
		L"<Color code='3' name='�������' rgb='0x0000FFFF' />"
		L"<Color code='4' name='�������' rgb='0x00FF0000' />"
		L"<Color code='5' name='����������' rgb='0x00FF00FF' />"
		L"<Color code='6' name='����������' rgb='0x00A52A2A' />"
		L"<Color code='7' name='������-�����' rgb='0x00D3D3D3' />"
		L"<Color code='8' name='�����-�����' rgb='0x00A9A9A9' />"
		L"<Color code='9' name='����-�����' rgb='0x00ADD8E6' />"
		L"<Color code='10' name='����-�������' rgb='0x0090EE90' />"
		L"<Color code='11' name='����-�������' rgb='0x00E0FFFF' />"
		L"<Color code='12' name='����-�������' rgb='0x00FF3333' />"
		L"<Color code='13' name='����-����������' rgb='0xFFFA5FF7' />"
		L"<Color code='14' name='������' rgb='0x00FFFF00' />"
		L"<Color code='15' name='�����' rgb='0x00FFFFFF' />"
		L"</ColorList>";

	attic::a_document adoc;
	adoc.load_wide( str );
	auto root = adoc.document_element();

	Gui::ColorClassificator colorClassificator( root );

	auto colorItem = colorClassificator.getItemByCode(13); 
	CPPUNIT_ASSERT( colorItem.getCode()==13 );
	CPPUNIT_ASSERT ( colorItem.getName()==L"����-����������" );
	CPPUNIT_ASSERT ( colorItem.getColor()==0xFFFA5FF7 );	
}

