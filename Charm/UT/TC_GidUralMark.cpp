#include "stdafx.h"

#include "TC_GidUralMark.h"
#include "../Hem/GuiMarkEvent.h"
#include "TopologyTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TC_GidUralMark);

void TC_GidUralMark::Parse_1()
{
    const std::wstring message = 
	L"(:0001 28002'TKIVER_REZ':20 40317 :2 11 01 22 1464\n"
	L"К 1641859349 0000129690 04 02 010 01 06 029690 000000 050 050 0221965922 0221965955 00068 00000 000 000 010 0\n"
	L"№6611 1кл ваг 4 оси одн 2т/б №001ВЛД/№002К-2 ДСП Пестерева\n"
	L"00134 0 0 1 4 1 1 0 0 0 0 0 0 0 7 $0\n";

    CPPUNIT_ASSERT(Asoup::MessageMarkGui::parse(message));
}

void TC_GidUralMark::CreateEvent()
{
	const std::wstring message = 
		L"(:0001 28002'SMSK-DS-DSP':20 40317 :2 11 01 22 1464\n"
		L"К 1641861547 0000128900 09 02 000 01 03 028900 000000 000 000 0221965937 0221965937 00004 00000 000 000 013 1 45316 19266 562 1 1 2890 4702\n"
		L"до Сивой под СДПМ\n"                                          
		L"00000 1 0 0 221965937 1600085766 6250335 $0\n";

	auto msgPtr = Asoup::MessageMarkGui::parse(message);
	CPPUNIT_ASSERT( msgPtr!=nullptr );
	std::shared_ptr<TopologyTest> topology(new TopologyTest);

	///\todo проверить поля!!!
	Gui::GuIdSaver guidSaver;
	auto markEvent = Gui::Event::Create( *msgPtr, guidSaver, topology.get(), L"0000128900" );
	CPPUNIT_ASSERT( markEvent );
	CPPUNIT_ASSERT( guidSaver.get(4702)==L"45316 19266 562" );
}

void TC_GidUralMark::Parse_2()
{
	const std::wstring message = 
		L"(:0001 28002'GID-CHER2-DSP':20 40317 :2 11 01 22 1464\n"
		L"К 1641909297 0000130250 01 02 000 02 16 030250 000000 086 086 0221966760 0221966760 00068 00000 000 000 013 0\n"
		L"3002-корнешов\n"
		L"00000 $0\n";


	auto msgPtr = Asoup::MessageMarkGui::parse(message);
	CPPUNIT_ASSERT( msgPtr!=nullptr );
	///\todo проверить поля!!!

}

void TC_GidUralMark::Parse_3()
{
	const std::wstring message = 
		L"(:0001 28002'IP0A.17.DD.29':20 40317 :2 11 01 22 1464\n"
		L"К 1641908313 -000007688 05 01 000 00 00 031060 031012 075 075 0221966725 0221966735 00068 00000 146 124 003 1 45105 19640 50922 158 625 6252 2037\n"
		L"\n"
		L"36864 219101760 $0\n";



	auto msgPtr = Asoup::MessageMarkGui::parse(message);
	CPPUNIT_ASSERT( msgPtr!=nullptr );
	///\todo проверить поля!!!

}

void TC_GidUralMark::Parse_4()
{
	const std::wstring message = 
		L"(:0001 28002'IP0A.17.DD.35':20 40317 :2 11 01 22 1465\n"
		L"К 1641908313 -000007688 05 01 000 00 00 031060 031012 075 075 0221966725 0221966735 00068 00000 146 124 003 1 45105 19640 50922 158 625 6252 2037\n"
		L"\n"
		L"00000 00004 $0\n";

	auto msgPtr = Asoup::MessageMarkGui::parse(message);
	CPPUNIT_ASSERT( msgPtr!=nullptr );
	///\todo проверить поля!!!

}

void TC_GidUralMark::Parse_5()
{
	const std::wstring message = 
		L"(:0001 28002'IP0A.17.DD.29':20 40317 :2 11 01 22 1464\n"
		L"К 0000351553 0000000001 02 02 000 01 01 031481 000000 015 093 0221965850 0221965970 00004 00000 157 000 014 0\n"
		L"Со снятием напряжения. |Рук. раб.: Михайлов (ЭЧ-1 СЕВ)|Установка ПС(3шт), Установка эл.соединителей(3шт)|http://asapvo.gvc.oao.rzd/apvo2-op/app/cards/wnd/351553\n"
		L"34962 6 221965850 120 -6 157 0 157 0 0 0 0 0 0 0 way=2 3 12 $0\n";

		auto msgPtr = Asoup::MessageMarkGui::parse(message);
	CPPUNIT_ASSERT( msgPtr!=nullptr );
	///\todo проверить поля!!!
	std::shared_ptr<TopologyTest> topology(new TopologyTest);
	auto markEvent = Gui::Event::Create( *msgPtr, Gui::GuIdSaver(), topology.get(), L"0000000001" );
	CPPUNIT_ASSERT( markEvent );
}

void TC_GidUralMark::LoadAndSaveMessage()
{
	const std::wstring message = 
		L"(:0001 28002'SMSK-DS-DSP':20 40317 :2 11 01 22 1464\r\n"
		L"К 1641861547 0000128900 09 02 000 01 03 028900 000000 000 000 0221965937 0221965937 00004 00000 000 000 013 1 0 0 0 1 1 2890 4702\r\n"
		L" до Сивой под СДПМ\r\n"
		L"00000 $0\n";                                          

	auto msgPtr = Asoup::MessageMarkGui::parse(message);
	CPPUNIT_ASSERT( msgPtr!=nullptr );

	auto str = FromUtf8(msgPtr->serialize());
	auto mismresult = std::mismatch( str.cbegin(), str.cend(), message.cbegin() );
	CPPUNIT_ASSERT( message==str );
}

void TC_GidUralMark::LoadToEventAndSaveToMessage_1()
{
	const std::wstring srcStr = 
		L"(:0001 28002'SMSK-DS-DSP':20 40317 :2 11 01 22 1464\r\n"
		L"К 1641849420 0000128900 09 02 000 01 03 028900 000000 000 000 0221965937 0221965937 00004 00000 000 000 013 1 0 0 0 1 1 2890 4702\r\n"
		L" до Сивой под СДПМ\r\n"                                          
		L"00000 $0\n";                                          

	auto msgSrc = Asoup::MessageMarkGui::parse(srcStr);
	CPPUNIT_ASSERT( msgSrc!=nullptr );

	//Gui::MarkStaticInfo msi(L"SMSK-DS-DSP", L"40317", L"0000128900");
	Gui::MarkStaticInfo msi(L"280", L"LOCAL", L"40317", L"0000423900");
	std::shared_ptr<TopologyTest> topology(new TopologyTest);
	Gui::GuIdSaver guidSaver;
	auto markEvent = Gui::Event::Create( *msgSrc, guidSaver, topology.get(), msi.workPlace );
	CPPUNIT_ASSERT( markEvent!=nullptr );
	CPPUNIT_ASSERT( guidSaver.empty() );

	time_t tEnter = 1641861547U;
	auto msgDst = Gui::Event::CreateMessage( *markEvent, msi, guidSaver, tEnter, false);
	CPPUNIT_ASSERT(msgDst);
	auto dstStr = FromUtf8(msgDst->serialize( &msi ));
	auto mismresult = std::mismatch( dstStr.cbegin(), dstStr.cend(), srcStr.cbegin() );
	CPPUNIT_ASSERT( srcStr==dstStr );
}

void TC_GidUralMark::LoadToEventAndSaveToMessage_2()
{
	const std::wstring srcStr = 
		L"(:0001 28002'TKI_CHIN2':20 40317 :2 11 01 22 1464\r\n"
		L"К 1641836460 0000128520 01 02 000 01 01 028520 000000 056 056 0221965721 0221965721 00068 00000 000 000 013 0\r\n"
		L" АО 'БОКСИТ ТИМАНА'|ТЭМ18 301 ПЕТРАШ/КРОТОВ|2ТЭ116 1605 КОТОВ/СЕЛИН\r\n"
		L"00000 $0\n";                                          

	auto msgSrc = Asoup::MessageMarkGui::parse(srcStr);
	CPPUNIT_ASSERT( msgSrc!=nullptr );

	Gui::MarkStaticInfo msi(L"280", L"TKI_CHIN2", L"40317", L"0000128520");
	std::shared_ptr<TopologyTest> topology(new TopologyTest);
	Gui::GuIdSaver guidSaver;
	auto markEvent = Gui::Event::Create( *msgSrc, guidSaver, topology.get(), msi.workPlace );
	CPPUNIT_ASSERT( markEvent!=nullptr );

	time_t tEnter = 1641848644;
	auto msgDst = Gui::Event::CreateMessage( *markEvent, msi, guidSaver, tEnter, false);
	CPPUNIT_ASSERT(msgDst);
	auto dstStr = FromUtf8(msgDst->serialize( &msi ));
    CPPUNIT_ASSERT( srcStr.size() == dstStr.size() );
    auto mismresult = std::mismatch( dstStr.cbegin(), dstStr.cend(), srcStr.cbegin() );
	CPPUNIT_ASSERT( srcStr==dstStr );
}

//ввод-вывод пометки с пустым комментарием
void TC_GidUralMark::LoadToEventAndSaveToMessage_3()
{
	const std::wstring srcStr = 
		L"(:0001 SEV02'VM-SEV':20 40317 :2 07 06 22 1464\r\n"
		L"К 1654618180 -000002286 05 01 000 00 00 028802 028800 025 025 0222178556 0222178559 00000 00000 146 124 015 1 0 0 0 2880 25 2899 3494\r\n"
		L"\r\r\n"
		L"00000 $0\n";

	auto msgSrc = Asoup::MessageMarkGui::parse(srcStr);
	CPPUNIT_ASSERT( msgSrc!=nullptr );

	Gui::MarkStaticInfo msi(L"SEV", L"VM-SEV", L"40317", L"-000002286");
	std::shared_ptr<TopologyTest> topology(new TopologyTest);
	Gui::GuIdSaver guidSaver;
	auto markEvent = Gui::Event::Create( *msgSrc, guidSaver, topology.get(), msi.workPlace );
	CPPUNIT_ASSERT( markEvent!=nullptr );

	time_t tEnter = 1654618180;
	auto msgDst = Gui::Event::CreateMessage( *markEvent, msi, guidSaver, tEnter, false);
	CPPUNIT_ASSERT(msgDst);
	auto dstStr = FromUtf8(msgDst->serialize( &msi ));
	auto mismresult = std::mismatch( dstStr.cbegin(), dstStr.cend(), srcStr.cbegin() );
	CPPUNIT_ASSERT( srcStr==dstStr );
}

//ввод-вывод пометки с комментарием со slash
void TC_GidUralMark::LoadToEventAndSaveToMessage_4()
{
	const std::wstring srcStr = 
		L"(:0001 SEV02'YARSON':20 40317 :2 07 06 22 1464\r\n"
		L"К 0000831908 0000000002 02 01 000 00 01 031262 031268 015 094 0222178290 0222178470 00005 00000 143 954 014 0\r\n"
		L" 307км3пк-307км6пк|Рук. раб.: Смирнов (ПЧ-4)|Смена плети(1600м), Перетяжка плетей(1600м)|http://asapvo.gvc.oao.rzd/apvo2-op/app/cards/wnd/831908\r\n"
	L"00000 $0\n";

	auto msgSrc = Asoup::MessageMarkGui::parse(srcStr);
	CPPUNIT_ASSERT( msgSrc!=nullptr );

	Gui::MarkStaticInfo msi(L"SEV", L"YARSON", L"40317", L"0000000002");
	std::shared_ptr<TopologyTest> topology(new TopologyTest);
	Gui::GuIdSaver guidSaver;
	auto markEvent = Gui::Event::Create( *msgSrc, guidSaver, topology.get(), msi.workPlace );
	CPPUNIT_ASSERT( markEvent!=nullptr );

	time_t tEnter = 1654618180;
	auto msgDst = Gui::Event::CreateMessage( *markEvent, msi, guidSaver, tEnter, false);
	CPPUNIT_ASSERT(msgDst);
	auto dstStr = FromUtf8(msgDst->serialize( &msi ));
    CPPUNIT_ASSERT( srcStr.size() == dstStr.size() );
	auto mismresult = std::mismatch( dstStr.cbegin(), dstStr.cend(), srcStr.cbegin() );
	CPPUNIT_ASSERT( srcStr==dstStr );
}
