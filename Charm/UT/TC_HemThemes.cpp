#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_HemThemes.h"
#include "../Actor/HemDraw/HemThemes.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_HemThemes );

void TC_HemThemes::LoadSave()
{
	const std::wstring themes_txt = 
		L"<ThemesAll>"
		L"<Screen ThemeDefault='ЖК Монитор 1280х1024'>"
		L"<Theme name='ЖК Монитор 1280х1024'>"
		L"<Item name='Минуты поездных операций' font_name='Times New Roman CYR' bold='1' italic='1' size='20' />"
		L"<Item name='Название закрытой станции' font_name='Times New Roman CYR' bold='1' italic='1' color='0x00000000' size='24' />"
		L"<Item name='Название раскрытой станции' font_name='Times New Roman CYR' bold='1' italic='1' color='0x00AA0000' size='24' />"
		L"</Theme>"
		L"<Theme name='Оформление ГИД &quot;Урал-ВНИИЖТ&quot;'>"
		L"<Item name='Номера поездов' font_name='Arial Cyr' bold='1' italic='1' size='22' />"
		L"<Item name='Номер парка' font_name='Tahoma' bold='1' italic='1' color='0x00FF0000' size='11' />"
		L"<Item name='Номер приемоотправочного пути' font_name='Tahoma' bold='1' italic='1' color='0x00FFFFFF' size='11' />"
		L"<Item name='Номер главного приемоотправочного пути' font_name='Tahoma' bold='1' italic='1' color='0x00FFFFFF' size='11' />"
		L"</Theme>"
		L"</Screen>"
// 		L"<Print ThemeDefault='Стандартная А1'>"
// 		L"<Theme name='Стандартная А1'>"
// 		L"<Item name='Минуты поездных операций' font_name='Times New Roman CYR' bold='1' italic='1' size='22' />"
// 		L"<Item name='Название закрытой станции' font_name='Times New Roman CYR' bold='1' italic='1' size='26' />"
// 		L"<Item name='Название раскрытой станции' font_name='Times New Roman CYR' bold='1' italic='1' size='26' />"
// 		L"</Theme>"
// 		L"<Theme name='Стандартная А2'>"
//         L"<Item name='Минуты поездных операций' font_name='Times New Roman CYR' bold='1' italic='1' size='22' />"
//         L"<Item name='Название закрытой станции' font_name='Times New Roman CYR' bold='1' italic='1' size='26' />"
//         L"<Item name='Название раскрытой станции' font_name='Times New Roman CYR' bold='1' italic='1' size='26' />"
// 		L"<Item name='Отчет - заголовок страницы' font_name='Times New Roman CYR' bold='1' italic='1' size='16' />"
// 		L"<Item name='Отчет - заголовок таблицы' font_name='Times New Roman CYR' bold='1' italic='1' size='16' />"
// 		L"<Item name='Отчет - таблица' font_name='Times New Roman CYR' bold='1' italic='1' size='14' />"
// 		L"<Item name='Отчет - просвет' size=' 10' />"
// 		L"<Item name='Учёт приёма' font_name='Times New Roman CYR' bold='1' italic='1' color='0x00004080' size='14' />"
// 		L"</Theme>"
// 		L"</Print>"
		L"</ThemesAll>";

	attic::a_document x_doc;
	x_doc.load_wide( themes_txt );
	std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

	HemThemes Themes( x_doc );

	attic::a_document y_doc;
	Themes.SaveToXML( y_doc );
	std::wstring str_dst = FromUtf8(y_doc.document_element().to_str());

    //auto mismres = std::mismatch( str_src.cbegin(), str_src.cend(), str_dst.cbegin(), str_dst.cend() );
	CPPUNIT_ASSERT( str_src == str_dst );
}

void TC_HemThemes::setUp()
{
}

void TC_HemThemes::tearDown()
{
}
