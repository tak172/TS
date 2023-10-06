#include "stdafx.h"

#include "TC_GidUralMarkText.h"
#include "../helpful/Attic.h"
#include "../GUI/GuiMarkTextClassificator.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TC_GidUralMarkText);

void TC_GidUralMarkText::Load()
{
    const std::wstring str = 
	L"<List>"
		L"<Text name='���������� ���� ���������� �������' kind='5'>"
		 L"���������� ���� ���������� ������� �� __ � + __ ���., �������� %Name%, ��������� ����� %Tabel%\n"
		L"</Text>"
		L"<Text name='��������� ���������' kind='5'>"
		 L"��������� ��������� � %LkmNum%\n"
		L"</Text>"
		L"<Text name='�� ����� � ����� ��������' kind='5'>"
		 L"�� ����� � ����� �������� (�������� %Name%, ��������� ����� %Tabel%)\n"
		L"</Text>"
		L"<Text name='�� ������������ �' kind='2'>"
		 L"�� ������������ �\n"
		 L"������������ \n"
		 L"������������� �� ������������ �������� \n"
		 L"������\n"
		L"</Text>" 
		L"<Text name='������������ ������������� �����' kind='3, 5, 7'>"
		 L"������� ������� ������� ����� �������������� ������������� �����:\n"
		 L"��������� - ____ ���.,\n"
		 L"���� - ____ ���.\n"
		L"</Text>"
		L"<Text name='����� ������������ �������' kind='9'>"
		 L"����� ������������ �������\n"
		L"</Text>"
		L"<Text name='���������� ���� ���������� ����������� �� ���' kind='3, 5, 7'>"
		 L"������� ������� ������� ����� ���������� ���� ���������� ����������� �� ���:\n"
		 L"��� �����  - ____ ���.,\n"
		 L"���������� - ____ ���.\n"
		L"</Text>"
		L"<Text name='���������� ������������ �� ������� ����������' kind='3, 5, 7'>"
		 L"������� ������� ������� ��-�� ���������� ������������ �� ������� ���������� ������������ � ___________________ �.�.\n"
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

