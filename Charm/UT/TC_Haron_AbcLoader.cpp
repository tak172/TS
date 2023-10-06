#include "stdafx.h"
#include "../helpful/Dictum.h"
#include "../Haron/HaronAbcLoader.h"
#include "TC_Haron_AbcLoader.h"
#include "../helpful/Attic.h"
#include "../Haron/LTUCommands.h"
#include "../Haron/ImpulsTU.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Haron_AbcLoader );

//загрузка элемента T из first_child xmlDoc
//
template< class T >
T LoadItemFromXmlDoc( const std::wstring& strDoc)
{
	attic::a_document adoc;
	adoc.load_wide( strDoc );

	return T(adoc.document_element().first_child());
}

void TC_Haron_AbcLoader::LoadSteeringNevaUnit()
{
	std::wstring steeringStr = 
	L"<Test >"
		L"<Steering ModeDC='NEVA' Gap='1600' ESR_code='12345'>"
		//command 1
		L"<LTU Name='s1' Keyboard='КТест1' Comment='строка 1'>"
		L"<FTU Obsolete='0C 03 0A 0F' HexData='43 96' SpellCmd='•11•1• •11• 1••••1••' Abonent='313'/>"
		L"<Inset Name='s2' ESR_code='11111'/>"
		L"</LTU>"
		//command 2
		L"<LTU Name='s2' Keyboard='КТест2' Comment='строка 2'>"
		L"<FTU Obsolete='0C 03 0B 0F' HexData='45 96' SpellCmd='•11•1• •11• •1•••1••' Abonent='313'/>"
		L"<Inset Name='s1' ESR_code='11111'/>"
		L"</LTU>"
		//command 3
		L"<LTU Name='s3' Keyboard='КТест3' Comment='строка 3'>"
		L"<FTU Obsolete='0C 03 0C 10' HexData='89 96' SpellCmd='•11•1• •11• ••1•••1•' Abonent='313'/>"
		L"<Inset Name='s4' ESR_code='11111'/>"
		L"</LTU>"
		//command 4
		L"<LTU Name='s4' Keyboard='КТест4' Comment='строка 4'>"
		L"<FTU Obsolete='0C 03 0D 10' HexData='91 96' SpellCmd='•11•1• •11• •••1••1•' Abonent='313'/>"
		L"<Inset Name='s3' ESR_code='11111'/>"
		L"</LTU>"
		L"</Steering>"
	L"</Test>";

	attic::a_document adoc;
	adoc.load_wide( steeringStr );
	attic::a_node steeringNode = adoc.document_element().first_child();

	Haron::AbcLoader abcLoader;
	LTU_ABC_TYPE commandsMap;
	int steeringCounter = 0;

	CPPUNIT_ASSERT( abcLoader.LoadSteeringUnit(steeringNode, &commandsMap, &steeringCounter) );
	CPPUNIT_ASSERT(  steeringCounter==4 );
	CPPUNIT_ASSERT( commandsMap.size()==1 );

	auto steering = *commandsMap.begin();
	
	//check steering station esr
	CPPUNIT_ASSERT ( steering.first==12345 );
	
	//check LTUSteering fields
	LTUSteering ltus = steering.second;
	CPPUNIT_ASSERT( ltus.m_esrCode==12345 && ltus.m_nGlobalGap==1600  );
	CPPUNIT_ASSERT( ltus.m_nModeDC==NEVA_fmt && ltus.m_allCommand.size()==4 );

	const auto& vCommands = steering.second.m_allCommand;
	//check command 1
	auto cmd = vCommands[0];
	CPPUNIT_ASSERT( cmd.m_Name==L"s1" );
	CPPUNIT_ASSERT( cmd.m_Keyboard==L"КТест1" );
	CPPUNIT_ASSERT( cmd.m_Comment==L"строка 1" );
	CPPUNIT_ASSERT( cmd.m_GapSet==0 );
	CPPUNIT_ASSERT( cmd.m_command_chain.size()==2 );
	//check FTU
	auto ftu = cmd.m_command_chain.front().m_ftu;
	CPPUNIT_ASSERT( ftu.m_Abonent==313 && ftu.is_active==true && ftu.m_GapBefore==0  );
	CPPUNIT_ASSERT(  ftu.m_sHexDataInStrView == L"43 96" );	
	// hexData_to_User_Cmd_View(ftu.m_HexData, DC_fmt_t::NEVA_fmt)==L"0C 03 0A 0F" 
	// HexData_to_SpellCmd(ftu.m_HexData, DC_fmt_t::NEVA_fmt)==L"•11•1• •11• 1••••1••" );
	CPPUNIT_ASSERT( ftu.m_HexData== 0x4396 );
	//check Inset
	auto inset = cmd.m_command_chain.back().m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==11111 && inset.m_Name==L"s2");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );

	//check command 2
	cmd = vCommands[1];
	CPPUNIT_ASSERT( cmd.m_Name==L"s2" );
	CPPUNIT_ASSERT( cmd.m_Keyboard==L"КТест2" );
	CPPUNIT_ASSERT( cmd.m_Comment==L"строка 2" );
	CPPUNIT_ASSERT( cmd.m_GapSet==0 );
	CPPUNIT_ASSERT( cmd.m_command_chain.size()==2 );
	//check FTU
	ftu = cmd.m_command_chain.front().m_ftu;
	CPPUNIT_ASSERT( ftu.m_Abonent==313 && ftu.is_active==true && ftu.m_GapBefore==0  );
	CPPUNIT_ASSERT( ftu.m_sHexDataInStrView == L"45 96" );
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::NEVA_fmt ) == L"0C 03 0B 0F";
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::NEVA_fmt ) == L"•11•1• •11• •1•••1••";
	CPPUNIT_ASSERT( ftu.m_HexData== 0x4596 );
	//check Inset
	inset = cmd.m_command_chain.back().m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==11111 && inset.m_Name==L"s1");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );

	//check command 3
	cmd = vCommands[2];
	CPPUNIT_ASSERT( cmd.m_Name==L"s3" );
	CPPUNIT_ASSERT( cmd.m_Keyboard==L"КТест3" );
	CPPUNIT_ASSERT( cmd.m_Comment==L"строка 3" );
	CPPUNIT_ASSERT( cmd.m_GapSet==0 );
	CPPUNIT_ASSERT( cmd.m_command_chain.size()==2 );
	//check FTU
	ftu = cmd.m_command_chain.front().m_ftu;
	CPPUNIT_ASSERT( ftu.m_Abonent==313 && ftu.is_active==true && ftu.m_GapBefore==0  );
	CPPUNIT_ASSERT( ftu.m_sHexDataInStrView == L"89 96" );
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::NEVA_fmt ) == L"0C 03 0C 10";
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::NEVA_fmt) == L"•11•1• •11• ••1•••1•";
	CPPUNIT_ASSERT( ftu.m_HexData== 0x8996 );
	//check Inset
	inset = cmd.m_command_chain.back().m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==11111 && inset.m_Name==L"s4");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );

	//check command 4
	cmd = vCommands[3];
	CPPUNIT_ASSERT( cmd.m_Name==L"s4" );
	CPPUNIT_ASSERT( cmd.m_Keyboard==L"КТест4" );
	CPPUNIT_ASSERT( cmd.m_Comment==L"строка 4" );
	CPPUNIT_ASSERT( cmd.m_GapSet==0 );
	CPPUNIT_ASSERT( cmd.m_command_chain.size()==2 );
	//check FTU
	ftu = cmd.m_command_chain.front().m_ftu;
	CPPUNIT_ASSERT( ftu.m_Abonent==313 && ftu.is_active==true && ftu.m_GapBefore==0  );
	CPPUNIT_ASSERT( ftu.m_sHexDataInStrView == L"91 96");
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::NEVA_fmt ) == L"0C 03 0D 10";
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::NEVA_fmt) == L"•11•1• •11• •••1••1•";
	CPPUNIT_ASSERT( ftu.m_HexData== 0x9196 );
	//check Inset
	inset = cmd.m_command_chain.back().m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==11111 && inset.m_Name==L"s3");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );
}

void TC_Haron_AbcLoader::LoadSteeringTraktUnit()
{
	std::wstring steeringStr1 = 
		L"<Test >"
		L"<Steering ModeDC='TRAKT' Gap='100' ESR_code='43211'>"
		//command 1
		L"<LTU Name='s1' Keyboard='КТест1' Comment='строка 1'>"
		L"<Inset Name='s7' ESR_code='43210' />"
		L"<Inset Name='s1' ESR_code='43211' />"
		L"<Inset Name='s7' ESR_code='43210' />"
		L"<FTU Obsolete='00 25' HexData='00 25' Abonent='246' />"
		L"</LTU>"
		L"</LTU>"
		L"</Steering>"
		L"</Test>";

	std::wstring steeringStr2 = 
		L"<Test >"
		L"<Steering ModeDC='TRAKT' Gap='60' ESR_code='43210'>"
		//command 7
		L"<LTU Name='s7' Keyboard='КТест7' Comment='строка 7'>"
		L"<FTU Obsolete='0C 15' HexData='0C 15' Abonent='264' />"
		L"<Inset Name='s1' ESR_code='43211' />"
		L"<Inset Name='s7' ESR_code='43210' />"
		L"<Inset Name='s1' ESR_code='43211' />"
		L"</Steering>"
		L"</Test>";

	attic::a_document adoc;
	adoc.load_wide( steeringStr1 );
	attic::a_node steeringNode = adoc.document_element().first_child();

	Haron::AbcLoader abcLoader;
	LTU_ABC_TYPE commandsMap;
	int steeringCounter = 0;

	//имитируем загрузку команд для двух станций
	CPPUNIT_ASSERT( abcLoader.LoadSteeringUnit(steeringNode, &commandsMap, &steeringCounter) );
	adoc.reset();
	adoc.load_wide(steeringStr2);
	steeringNode = adoc.document_element().first_child();
	CPPUNIT_ASSERT( abcLoader.LoadSteeringUnit(steeringNode, &commandsMap, &steeringCounter) );
	CPPUNIT_ASSERT(  steeringCounter==2 );
	CPPUNIT_ASSERT( commandsMap.size()==2 );

	auto it = commandsMap.begin();
	CPPUNIT_ASSERT ( it->first==43210 );
	LTUSteering ltus = it->second;
	CPPUNIT_ASSERT( ltus.m_esrCode==43210 && ltus.m_nGlobalGap==60  );
	CPPUNIT_ASSERT( ltus.m_nModeDC==TRAKT_fmt && ltus.m_allCommand.size()==1 );
	auto vCommands = it->second.m_allCommand;

	//check command 7
	auto cmd = vCommands.front();
	CPPUNIT_ASSERT( cmd.m_Name==L"s7" );
	CPPUNIT_ASSERT( cmd.m_Keyboard==L"КТест7" );
	CPPUNIT_ASSERT( cmd.m_Comment==L"строка 7" );
	CPPUNIT_ASSERT( cmd.m_GapSet==0 );
	CPPUNIT_ASSERT( cmd.m_command_chain.size()==4 );
	//check FTU
	auto ftu = cmd.getCommand(0).m_ftu;
	CPPUNIT_ASSERT( ftu.m_Abonent==264 && ftu.is_active==true && ftu.m_GapBefore==0  );
	CPPUNIT_ASSERT( ftu.m_sHexDataInStrView == L"0C 15" );
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::Trakt ) == L"0C 15";
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::Trakt).empty();
	CPPUNIT_ASSERT( ftu.m_HexData== 0x0C15 );

	//check inset 1
	auto inset = cmd.getCommand(1).m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==43211 && inset.m_Name==L"s1");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );
	
	//check inset 2
	inset = cmd.getCommand(2).m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==43210 && inset.m_Name==L"s7");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );

	//check inset3
	inset = cmd.getCommand(3).m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==43211 && inset.m_Name==L"s1");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );

	it++;
	//check steering station esr
	CPPUNIT_ASSERT ( it->first==43211 );

	//check LTUSteering fields
	ltus = it->second;
	vCommands = it->second.m_allCommand;
	CPPUNIT_ASSERT( ltus.m_esrCode==43211 && ltus.m_nGlobalGap==100  );
	CPPUNIT_ASSERT( ltus.m_nModeDC==TRAKT_fmt && ltus.m_allCommand.size()==1 );
	//check command 1
	cmd = vCommands.front();
	CPPUNIT_ASSERT( cmd.m_Name==L"s1" );
	CPPUNIT_ASSERT( cmd.m_Keyboard==L"КТест1" );
	CPPUNIT_ASSERT( cmd.m_Comment==L"строка 1" );
	CPPUNIT_ASSERT( cmd.m_GapSet==0 );
	CPPUNIT_ASSERT( cmd.m_command_chain.size()==4 );
	//check inset1
	inset = cmd.getCommand(0).m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==43210 && inset.m_Name==L"s7");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );
	//check inset2
	inset = cmd.getCommand(1).m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==43211 && inset.m_Name==L"s1");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );
	//check inset3
	inset = cmd.getCommand(2).m_inset;
	CPPUNIT_ASSERT( inset.m_ESR_Code==43210 && inset.m_Name==L"s7");
	CPPUNIT_ASSERT( inset.is_active==true && inset.m_GapBefore==0 );

	//check FTU
	ftu = cmd.getCommand(3).m_ftu;
	CPPUNIT_ASSERT( ftu.m_Abonent==246 && ftu.is_active==true && ftu.m_GapBefore==0  );
	CPPUNIT_ASSERT( ftu.m_sHexDataInStrView == L"00 25" );
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::Trakt ) == L"0C 09";
	// hexData_to_User_Cmd_View( ftu.m_HexData, DC_fmt_t::Trakt).empty();
	CPPUNIT_ASSERT( ftu.m_HexData== 0x25 );

}

void TC_Haron_AbcLoader::ConvertHexData()
{
	std::wstring srcStr = L"0C 09";
	DWORD hexData = wstrHexData_to_Dword(srcStr);
	CPPUNIT_ASSERT( hexData==0x0C09 );
	std::wstring dstStr = dwordHexData_to_wstr( hexData );
	CPPUNIT_ASSERT( srcStr==dstStr );

	srcStr = L"02 00 40";
	hexData = wstrHexData_to_Dword(srcStr);
	CPPUNIT_ASSERT( hexData==0x020040 );
	dstStr = dwordHexData_to_wstr( hexData );
	CPPUNIT_ASSERT( srcStr==dstStr );
}

void TC_Haron_AbcLoader::GenerationSpellCmd()
{
	{
	std::wstring ftuStr = 
	L"<Test >"
		L"<FTU Obsolete='0C 03 0A 0F' HexData='43 96' SpellCmd='•11•1• •11• 1••••1••' Abonent='313'/>"
	L"</Test>";

	FTU ftu = LoadItemFromXmlDoc<FTU>(ftuStr);

	CPPUNIT_ASSERT( ftu.m_HexData!=0 );
	CPPUNIT_ASSERT( ftu.getErrSourceSpellCmd(L"UTcmd", L"•11•1• •11• 1••••1••").empty() );
	}

	{
		std::wstring ftuErrStr = 
			L"<Test >"
			L"<FTU Obsolete='0C 03 0A 0F' HexData='43 96' SpellCmd='•11•1• 1••• 1••••1••' Abonent='313'/>"
			L"</Test>";

		FTU ftu = LoadItemFromXmlDoc<FTU>(ftuErrStr);
		CPPUNIT_ASSERT( ftu.m_HexData!=0 );
		CPPUNIT_ASSERT( !ftu.getErrSourceSpellCmd(L"UTcmd", L"•11•1• 1••• 1••••1••").empty() );
	}
}

void TC_Haron_AbcLoader::GenerationObsolete()
{
	{
		std::wstring ftuStr = 
			L"<Test >"
			L"<FTU Obsolete='07 04 0D 0F' HexData='02 50 65' SpellCmd='1•1••1 1••1 •••1•1••' Abonent='302' />"
			L"</Test>";

		FTU ftu = LoadItemFromXmlDoc<FTU>(ftuStr);

		CPPUNIT_ASSERT( ftu.m_HexData!=0 );
		CPPUNIT_ASSERT( ftu.getErrSourceObsolete(L"UTcmd", L"07 04 0D 0F").empty() );

	}

	{
		std::wstring ftuStr = 
			L"<Test >"
			L"<FTU Obsolete='0C 03 0A 0F' HexData='43 96' SpellCmd='•11•1• •11• 1••••1••' Abonent='313'/>"
			L"</Test>";

		FTU ftu = LoadItemFromXmlDoc<FTU>(ftuStr);

		CPPUNIT_ASSERT( ftu.m_HexData!=0 );
		CPPUNIT_ASSERT( ftu.getErrSourceObsolete(L"UTcmd", L"0C 03 0A 0F").empty() );
	}

	{
		std::wstring ftuErrStr = 
			L"<Test >"
			L"<FTU Obsolete='0C 03 0A F0' HexData='43 96' SpellCmd='•11•1• •11• 1••••1••' Abonent='313'/>"
			L"</Test>";

		FTU ftu = LoadItemFromXmlDoc<FTU>(ftuErrStr);
		CPPUNIT_ASSERT( ftu.m_HexData!=0 );
		CPPUNIT_ASSERT( !ftu.getErrSourceObsolete(L"UTcmd", L"0C 03 0A F0").empty() );
	}


	{
		std::wstring ftuErrStr = 
			L"<Test >"
			L"<FTU Obsolete='0C 07 11' HexData='03 01 D6' SpellCmd='•11•1• 1111 •••••••1' Abonent='313' />"
			L"</Test>";
		FTU ftu = LoadItemFromXmlDoc<FTU>(ftuErrStr);
		CPPUNIT_ASSERT( ftu.m_HexData!=0 );
		CPPUNIT_ASSERT( ftu.getErrSourceObsolete(L"UTcmd", L"0C 07 11").empty() );

	}
}
