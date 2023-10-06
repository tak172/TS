#include "stdafx.h"

#include "TC_GidUralMarkText.h"
#include "../helpful/Attic.h"
#include "../GUI/GuiMarkTextClassificator.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TC_GidUralMarkText);

void TC_GidUralMarkText::Load()
{
    const std::wstring str = 
	L"<List>"
		L"<Text name='Привышение норм накладного времени' kind='5'>"
		 L"Привышение норм накладного времени на __ ч + __ мин., машинист %Name%, табельный номер %Tabel%\n"
		L"</Text>"
		L"<Text name='Снабжение тепловоза' kind='5'>"
		 L"Снабжение тепловоза № %LkmNum%\n"
		L"</Text>"
		L"<Text name='Не готов к пробе тормозов' kind='5'>"
		 L"Не готов к пробе тормозов (машинист %Name%, табельный номер %Tabel%)\n"
		L"</Text>"
		L"<Text name='По согласованию с' kind='2'>"
		 L"По согласованию с\n"
		 L"Руководитель \n"
		 L"Ответственный за безопасность движения \n"
		 L"Работа\n"
		L"</Text>" 
		L"<Text name='Несодержание локомотивного парка' kind='3, 5, 7'>"
		 L"Невывоз готовых поездов ввиду недосодержания локомотивного парка:\n"
		 L"потребное - ____ лок.,\n"
		 L"факт - ____ лок.\n"
		L"</Text>"
		L"<Text name='Смена локомотивной бригады' kind='9'>"
		 L"Смена локомотивной бригады\n"
		L"</Text>"
		L"<Text name='Превышение норм содержания локомотивов на НЭП' kind='3, 5, 7'>"
		 L"Невывоз готовых поездов ввиду превышения норм содержания локомотивов на НЭП:\n"
		 L"при норме  - ____ лок.,\n"
		 L"содержание - ____ лок.\n"
		L"</Text>"
		L"<Text name='Отсутствие электровозов по причине невозврата' kind='3, 5, 7'>"
		 L"Невывоз готовых поездов из-за отсутствия электровозов по причине невозврата электровозов с ___________________ ж.д.\n"
		L"</Text>"
	L"</List>";

	attic::a_document adoc;
	adoc.load_wide( str );
	auto root = adoc.document_element();

	Gui::MarkTextClassificator markTextClassificator( root );

	auto vTexts = markTextClassificator.geTextByMarkType(2); 
	CPPUNIT_ASSERT( vTexts.size()==1 );
	CPPUNIT_ASSERT( vTexts.front().getText().size()==4 );
	vTexts = markTextClassificator.geTextByMarkType(5); 
	CPPUNIT_ASSERT( vTexts.size()==6 );
}

