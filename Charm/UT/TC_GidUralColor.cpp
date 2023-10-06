#include "stdafx.h"

#include "TC_GidUralColor.h"
#include "../helpful/Attic.h"
#include "../GUI/InfoTypesClassificator.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TC_GidUralColor);

void TC_GidUralColor::Load()
{
    const std::wstring str = 
		L"<ColorList>"
		L"<Color code='0' name='черный' rgb='0x00000000' />"
		L"<Color code='1' name='синий' rgb='0x000000FF' />"
		L"<Color code='2' name='зеленый' rgb='0x00008000' />"
		L"<Color code='3' name='голубой' rgb='0x0000FFFF' />"
		L"<Color code='4' name='красный' rgb='0x00FF0000' />"
		L"<Color code='5' name='фиолетовый' rgb='0x00FF00FF' />"
		L"<Color code='6' name='коричневый' rgb='0x00A52A2A' />"
		L"<Color code='7' name='светло-серый' rgb='0x00D3D3D3' />"
		L"<Color code='8' name='темно-серый' rgb='0x00A9A9A9' />"
		L"<Color code='9' name='€рко-синий' rgb='0x00ADD8E6' />"
		L"<Color code='10' name='€рко-зеленый' rgb='0x0090EE90' />"
		L"<Color code='11' name='€рко-голубой' rgb='0x00E0FFFF' />"
		L"<Color code='12' name='€рко-красный' rgb='0x00FF3333' />"
		L"<Color code='13' name='€рко-фиолетовый' rgb='0xFFFA5FF7' />"
		L"<Color code='14' name='желтый' rgb='0x00FFFF00' />"
		L"<Color code='15' name='белый' rgb='0x00FFFFFF' />"
		L"</ColorList>";

	attic::a_document adoc;
	adoc.load_wide( str );
	auto root = adoc.document_element();

	Gui::ColorClassificator colorClassificator( root );

	auto colorItem = colorClassificator.getItemByCode(13); 
	CPPUNIT_ASSERT( colorItem.getCode()==13 );
	CPPUNIT_ASSERT ( colorItem.getName()==L"€рко-фиолетовый" );
	CPPUNIT_ASSERT ( colorItem.getColor()==0xFFFA5FF7 );	
}

