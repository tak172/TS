#include "stdafx.h"
#include "RealPart.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/LogicSemaleg.h"

using namespace std;

RealPart::RealPart()
{
	CreateLigatne();
	CreateRiga();
	CreateLiepaja();
	CreateVangazi();
	CreateSloka();
	CreateKarsava();
	CreateKraslava();
	CreateKrauja();
	CreateMezvidi();
	CreateSkirotava();
	CreateKemeri();
	CreateRezekne2();
	CreateJelgava();
	CreateBale();
	SetDetails();
}

void RealPart::CreateLigatne()
{
	const auto & ligatneCode = EsrKit( 9510 );
	const auto & ligIerCode = EsrKit( 9510, 11130 );
	const auto & ligSigCode = EsrKit( 9510, 9511 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"LG_LI9", ligIerCode ), BadgeE( L"LG_LI7", ligIerCode ), BadgeE( L"LG_LI5", ligIerCode ), BadgeE( L"LG_LI3", ligIerCode ), 
		BadgeE( L"LG_LI1", ligIerCode ), BadgeE( L"LG_NDP", ligatneCode ), BadgeE( L"LG_1-3SP", ligatneCode ), BadgeE( L"LG_1-3SP:1+", ligatneCode ), 
		BadgeE( L"LG_1-3SP:1-", ligatneCode ), BadgeE( L"LG_1-3SP:1+,3+", ligatneCode ), BadgeE( L"LG_1-3SP:1+,3-", ligatneCode ), BadgeE( L"LG_5SP", ligatneCode ), 
		BadgeE( L"LG_5SP:5+", ligatneCode ), BadgeE( L"LG_5SP:5-", ligatneCode ), BadgeE( L"LG_1C", ligatneCode ), BadgeE( L"LG_2C", ligatneCode ), 
		BadgeE( L"LG_3C", ligatneCode ), BadgeE( L"LG_14SP", ligatneCode ), BadgeE( L"LG_14SP:14+", ligatneCode ), BadgeE( L"LG_14SP:14-", ligatneCode ), 
		BadgeE( L"LG_6-8SP:6+,8-", ligatneCode ), BadgeE( L"LG_6-8SP:6+,8+", ligatneCode ), BadgeE( L"LG_6-8SP:6+", ligatneCode ), 
		BadgeE( L"LG_6-8SP:6-", ligatneCode ), BadgeE( L"LG_6-8SP", ligatneCode ), BadgeE( L"LG_PDP", ligatneCode ), BadgeE( L"LG_SL9", ligSigCode ), 
		BadgeE( L"LG_SL7", ligSigCode ), BadgeE( L"LG_SL5", ligSigCode ), BadgeE( L"LG_SL3", ligSigCode ), BadgeE( L"LG_SL1", ligSigCode )

	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"LG_LI7~LG_LI9", ligIerCode ), BadgeE( L"LG_LI5~LG_LI7", ligIerCode ), BadgeE( L"LG_LI3~LG_LI5", ligIerCode ), 
		BadgeE( L"LG_LI1~LG_LI3", ligIerCode ), BadgeE( L"LG_LI1~LG_NDP", ligatneCode ), BadgeE( L"LG_1-3SP~LG_NDP", ligatneCode ), 
		BadgeE( L"LG_1-3SP:1+,3-~LG_5SP", ligatneCode ), BadgeE( L"LG_1-3SP:1+,3+~LG_1C", ligatneCode ), BadgeE( L"LG_1-3SP:1-~LG_2C", ligatneCode ), 
		BadgeE( L"LG_3C~LG_5SP:5+", ligatneCode ), BadgeE( L"LG_14SP~LG_3C", ligatneCode ), BadgeE( L"LG_14SP:14+~LG_6-8SP:6-", ligatneCode ), 
		BadgeE( L"LG_1C~LG_6-8SP:6+,8+", ligatneCode ), BadgeE( L"LG_2C~LG_6-8SP:6+,8-", ligatneCode ), BadgeE( L"LG_6-8SP~LG_PDP", ligatneCode ), 
		BadgeE( L"LG_PDP~LG_SL9", ligatneCode ), BadgeE( L"LG_SL7~LG_SL9", ligSigCode ), BadgeE( L"LG_SL5~LG_SL7", ligSigCode ),
		BadgeE( L"LG_SL3~LG_SL5", ligSigCode ), BadgeE( L"LG_SL1~LG_SL3", ligSigCode )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"LG1", ligatneCode ), BadgeE( L"LG3", ligatneCode ), BadgeE( L"LG5", ligatneCode ), BadgeE( L"LG6", ligatneCode ),
		BadgeE( L"LG8", ligatneCode ), BadgeE( L"LG14", ligatneCode )
	};
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"LG_HeadN", ligatneCode ), BadgeE( L"LG_HeadP1", ligatneCode ), BadgeE( L"LG_HeadP2", ligatneCode ), 
		BadgeE( L"LG_HeadP3", ligatneCode ), BadgeE( L"LG_HeadN1", ligatneCode ), BadgeE( L"LG_HeadN2", ligatneCode ), BadgeE( L"LG_HeadN3", ligatneCode ),
		BadgeE( L"LG_HeadP", ligatneCode )
	};
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"LG_LegN", ligatneCode ), BadgeE( L"LG_LegP1", ligatneCode ), BadgeE( L"LG_LegP2", ligatneCode ), 
		BadgeE( L"LG_LegP3", ligatneCode ), BadgeE( L"LG_LegN1", ligatneCode ), BadgeE( L"LG_LegN2", ligatneCode ), BadgeE( L"LG_LegN3", ligatneCode ),
		BadgeE( L"LG_LegP", ligatneCode )
	};
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//маршруты
	CLink rtlinks[] = { CLink( HEAD, BadgeE( L"LG_HeadP", ligatneCode ) ), CLink( STRIP, BadgeE( L"LG_PDP", ligatneCode ) ), 
		CLink( STRIP, BadgeE( L"LG_6-8SP:6+,8+", ligatneCode ) ), CLink( STRIP, BadgeE( L"LG_1C", ligatneCode ) ) };
	InsertRoute( BadgeE( L"LG_legP->LG_1C", ligatneCode ), vector<CLink>( begin( rtlinks ), end( rtlinks ) ) );

	//связывание
	Link( L"LG_LI9", FormLink( L"LG_LI7~LG_LI9" ) );
	Link( L"LG_LI7", FormLink( L"LG_LI5~LG_LI7" ), FormLink( L"LG_LI7~LG_LI9" ) );
	Link( L"LG_LI5", FormLink( L"LG_LI3~LG_LI5" ), FormLink( L"LG_LI5~LG_LI7" ) );
	Link( L"LG_LI3", FormLink( L"LG_LI1~LG_LI3" ), FormLink( L"LG_LI3~LG_LI5" ) );
	Link( L"LG_LI1", FormLink( L"LG_LI1~LG_NDP" ), FormLink( L"LG_LI1~LG_LI3" ) );
	Link( L"LG_NDP", FormLink( L"LG_1-3SP~LG_NDP" ), FormLink( L"LG_LI1~LG_NDP" ) );
	Link( L"LG_1-3SP", FormLink( L"LG1", CLink::BASE ), FormLink( L"LG_1-3SP~LG_NDP" ) );
	Link( L"LG_1-3SP:1+", FormLink( L"LG3", CLink::BASE ), FormLink( L"LG1", CLink::PLUS ) );
	Link( L"LG_1-3SP:1-", FormLink( L"LG_1-3SP:1-~LG_2C" ), FormLink( L"LG1", CLink::MINUS ) );
	Link( L"LG_2C", FormLink( L"LG_1-3SP:1-~LG_2C" ), FormLink( L"LG_2C~LG_6-8SP:6+,8-" ) );
	Link( L"LG_1-3SP:1+,3+", FormLink( L"LG_1-3SP:1+,3+~LG_1C" ), FormLink( L"LG3", CLink::PLUS ) );
	Link( L"LG_1C", FormLink( L"LG_1-3SP:1+,3+~LG_1C" ), FormLink( L"LG_1C~LG_6-8SP:6+,8+" ) );
	Link( L"LG_1-3SP:1+,3-", FormLink( L"LG_1-3SP:1+,3-~LG_5SP" ), FormLink( L"LG3", CLink::MINUS ) );
	Link( L"LG_5SP", FormLink( L"LG5", CLink::BASE ), FormLink( L"LG_1-3SP:1+,3-~LG_5SP" ) );
	Link( L"LG_5SP:5-", FormLink( L"LG5", CLink::MINUS ) );
	Link( L"LG_5SP:5+", FormLink( L"LG_3C~LG_5SP:5+" ), FormLink( L"LG5", CLink::PLUS ) );
	Link( L"LG_3C", FormLink( L"LG_3C~LG_5SP:5+" ), FormLink( L"LG_14SP~LG_3C" ) );
	Link( L"LG_14SP", FormLink( L"LG14", CLink::BASE ), FormLink( L"LG_14SP~LG_3C" ) );
	Link( L"LG_14SP:14-", FormLink( L"LG14", CLink::MINUS ) );
	Link( L"LG_14SP:14+", FormLink( L"LG14", CLink::PLUS ), FormLink( L"LG_14SP:14+~LG_6-8SP:6-" ) );
	Link( L"LG_6-8SP:6-", FormLink( L"LG_14SP:14+~LG_6-8SP:6-" ), FormLink( L"LG6", CLink::MINUS ) );
	Link( L"LG_6-8SP:6+,8+", FormLink( L"LG8", CLink::PLUS ), FormLink( L"LG_1C~LG_6-8SP:6+,8+" ) );
	Link( L"LG_6-8SP:6+,8-", FormLink( L"LG8", CLink::MINUS ), FormLink( L"LG_2C~LG_6-8SP:6+,8-" ) );
	Link( L"LG_6-8SP:6+", FormLink( L"LG8", CLink::BASE ), FormLink( L"LG6", CLink::PLUS ) );
	Link( L"LG_6-8SP", FormLink( L"LG6", CLink::BASE ), FormLink( L"LG_6-8SP~LG_PDP" ) );
	Link( L"LG_PDP", FormLink( L"LG_6-8SP~LG_PDP" ), FormLink( L"LG_PDP~LG_SL9" ) );
	Link( L"LG_SL9", FormLink( L"LG_PDP~LG_SL9" ), FormLink( L"LG_SL7~LG_SL9" ) );
	Link( L"LG_SL7", FormLink( L"LG_SL7~LG_SL9" ), FormLink( L"LG_SL5~LG_SL7" ) );
	Link( L"LG_SL5", FormLink( L"LG_SL5~LG_SL7" ), FormLink( L"LG_SL3~LG_SL5" ) );
	Link( L"LG_SL3", FormLink( L"LG_SL1~LG_SL3" ), FormLink( L"LG_SL3~LG_SL5" ) );
	Link( L"LG_SL1", FormLink( L"LG_SL1~LG_SL3" ) );

	LinkSema( L"LG_LegN", L"LG_LI1", L"LG_NDP", vector<wstring>( 1, L"LG_HeadN" ) );
	LinkSema( L"LG_LegP1", L"LG_1C", L"LG_1-3SP:1+,3+", vector<wstring>( 1, L"LG_HeadP1" ) );
	LinkSema( L"LG_LegP2", L"LG_2C", L"LG_1-3SP:1-", vector<wstring>( 1, L"LG_HeadP2" ) );
	LinkSema( L"LG_LegP3", L"LG_5SP", L"LG_1-3SP:1+,3-", vector<wstring>( 1, L"LG_HeadP3" ) );
	LinkSema( L"LG_LegP", L"LG_SL9", L"LG_PDP", vector<wstring>( 1, L"LG_HeadP" ) );
	LinkSema( L"LG_LegN1", L"LG_1C", L"LG_6-8SP:6+,8+", vector<wstring>( 1, L"LG_HeadN1" ) );
	LinkSema( L"LG_LegN2", L"LG_2C", L"LG_6-8SP:6+,8-", vector<wstring>( 1, L"LG_HeadN2" ) );
	LinkSema( L"LG_LegN3", L"LG_14SP:14+", L"LG_6-8SP:6-", vector<wstring>( 1, L"LG_HeadN3" ) );
}

void RealPart::CreateRiga()
{
	const auto & rigaCode = EsrKit( 9010 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"RG_10C", rigaCode ), BadgeE( L"RG_81SP", rigaCode ), BadgeE( L"RG_81SP+", rigaCode ), BadgeE( L"RG_81SP-", rigaCode ),
		BadgeE( L"RG_63SP", rigaCode ), BadgeE( L"RG_63SP+", rigaCode ), BadgeE( L"RG_63SP-", rigaCode ), BadgeE( L"RG_83SP", rigaCode ), 
		BadgeE( L"RG_83SP+", rigaCode ), BadgeE( L"RG_83SP-", rigaCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"RG_10C~RG_81SP", rigaCode ), BadgeE( L"RG_63SP-~RG_81SP+", rigaCode ), BadgeE( L"RG_63SP+-~RG_83SP", rigaCode ) };
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"RG81", rigaCode ), BadgeE( L"RG83", rigaCode ), BadgeE( L"RG63", rigaCode ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"RG_HeadP10", rigaCode ) };
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"RG_LegP10", rigaCode ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"RG_10C", FormLink( L"RG_10C~RG_81SP" ) );
	Link( L"RG_81SP", FormLink( L"RG_10C~RG_81SP" ), FormLink( L"RG81", CLink::BASE ) );
	Link( L"RG_81SP-", FormLink( L"RG81", CLink::MINUS ) );
	Link( L"RG_81SP+", FormLink( L"RG81", CLink::PLUS ), FormLink( L"RG_63SP-~RG_81SP+" ) );
	Link( L"RG_63SP-", FormLink( L"RG_63SP-~RG_81SP+" ), FormLink( L"RG63", CLink::MINUS ) );
	Link( L"RG_63SP", FormLink( L"RG63", CLink::BASE ) );
	Link( L"RG_63SP+", FormLink( L"RG_63SP+-~RG_83SP" ), FormLink( L"RG63", CLink::PLUS ) );
	Link( L"RG_83SP", FormLink( L"RG_63SP+-~RG_83SP" ), FormLink( L"RG83", CLink::BASE ) );
	Link( L"RG_83SP+", FormLink( L"RG83", CLink::PLUS ) );
	Link( L"RG_83SP-", FormLink( L"RG83", CLink::MINUS ) );

	LinkSema( L"RG_LegP10", L"RG_10C", L"RG_81SP", vector<wstring>( 1, L"RG_HeadP10" ) );
}

void RealPart::CreateLiepaja()
{
	const auto & liepCode = EsrKit( 9860 );
	const auto & liepToreCode = EsrKit( 9355, 9860 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"LP_12C", liepCode ), BadgeE( L"LP_75-77SP:75-", liepCode ), BadgeE( L"LP_75-77SP:75+", liepCode ), 
		BadgeE( L"LP_75-77SP", liepCode ), BadgeE( L"LP_69-73SP:69-,71-,73-", liepCode ), BadgeE( L"LP_69-73SP:69-,71-,73+", liepCode ),
		BadgeE( L"LP_69-73SP:69-,71-", liepCode ), BadgeE( L"LP_69-73SP:69-,71+", liepCode ), BadgeE( L"LP_69-73SP:69-", liepCode ),
		BadgeE( L"LP_69-73SP:69+", liepCode ), BadgeE( L"LP_69-73SP", liepCode ), BadgeE( L"LP_65SP", liepCode ), BadgeE( L"LP_65SP:65+", liepCode ),
		BadgeE( L"LP_65SP:65-", liepCode ), BadgeE( L"LP_9-13SP:13+", liepCode ), BadgeE( L"LP_9-13SP:13+,9+", liepCode ), BadgeE( L"LP_9-13SP:13+,9-", liepCode ),
		BadgeE( L"LP_NP", liepCode ), BadgeE( L"LP_NGP", liepToreCode ), BadgeE( L"LP_LT_1C", liepToreCode ),
		BadgeE( L"LP_30-32SP", liepCode ), BadgeE( L"LP_30-32SP:30+", liepCode ), BadgeE( L"LP_30-32SP:30-", liepCode ),
		BadgeE( L"LP_30-32SP:30-,32+", liepCode ), BadgeE( L"LP_30-32SP:30-,32-", liepCode ), BadgeE( L"LP_9C", liepCode ),
		BadgeE( L"LP_75-77SP:75-,77+", liepCode ), BadgeE( L"LP_75-77SP:75-,77-", liepCode ), BadgeE( L"LP_13C", liepCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"LP_12C~LP_75-77SP:75+", liepCode ), BadgeE( L"LP_69-73SP:69-,71-,73-~LP_75-77SP", liepCode ), 
		BadgeE( L"LP_65SP~LP_69-73SP", liepCode ), BadgeE( L"LP_9-13SP:13+,9+~LP_NP", liepCode ), BadgeE( L"LP_NGP~LP_NP", liepToreCode ),
		BadgeE( L"LP_NGP~LP_LT_1C", liepToreCode ), BadgeE( L"LP_30-32SP:30-,32+~LP_9C", liepCode ),
		BadgeE( L"LP_69-73SP:69+~LP_9C", liepCode ), BadgeE( L"LP_13C~LP_75-77SP:75-,77+", liepCode ),
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"LP77", liepCode ), BadgeE( L"LP75", liepCode ), BadgeE( L"LP73", liepCode ), 
		BadgeE( L"LP71", liepCode ), BadgeE( L"LP69", liepCode ), BadgeE( L"LP65", liepCode ), BadgeE( L"LP9", liepCode ), 
		BadgeE( L"LP32", liepCode ), BadgeE( L"LP30", liepCode )
	};
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE sheads_arr[] = { BadgeE( L"LP_HeadM1", liepCode ) };
	Insert( HEAD, sheads_arr, size_array( sheads_arr ), true, SHUNTING );
	BadgeE theads_arr[] = { BadgeE( L"LP_HeadN", liepCode ) };
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"LP_LegM1", liepCode ), BadgeE( L"LP_LegN", liepCode ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"LP_13C", FormLink( L"LP_13C~LP_75-77SP:75-,77+" ) );
	Link( L"LP_12C", FormLink( L"LP_12C~LP_75-77SP:75+" ) );
	Link( L"LP_9C", FormLink( L"LP_30-32SP:30-,32+~LP_9C" ), FormLink( L"LP_69-73SP:69+~LP_9C" ) );
	Link( L"LP_75-77SP:75-,77+", FormLink( L"LP_13C~LP_75-77SP:75-,77+" ), FormLink( L"LP77", CLink::PLUS ) );
	Link( L"LP_75-77SP:75-,77-", FormLink( L"LP77", CLink::MINUS ) );
	Link( L"LP_75-77SP:75+", FormLink( L"LP_12C~LP_75-77SP:75+" ), FormLink( L"LP75", CLink::PLUS ) );
	Link( L"LP_75-77SP:75-", FormLink( L"LP75", CLink::MINUS ), FormLink( L"LP77", CLink::BASE ) );
	Link( L"LP_75-77SP", FormLink( L"LP75", CLink::BASE ), FormLink( L"LP_69-73SP:69-,71-,73-~LP_75-77SP" ) );
	Link( L"LP_69-73SP:69-,71-,73-", FormLink( L"LP_69-73SP:69-,71-,73-~LP_75-77SP" ), FormLink( L"LP73", CLink::MINUS ) );
	Link( L"LP_69-73SP:69-,71-,73+", FormLink( L"LP73", CLink::PLUS ) );
	Link( L"LP_69-73SP:69-,71-", FormLink( L"LP73", CLink::BASE ), FormLink( L"LP71", CLink::MINUS ) );
	Link( L"LP_69-73SP:69-,71+", FormLink( L"LP71", CLink::PLUS ) );
	Link( L"LP_69-73SP:69-", FormLink( L"LP69", CLink::MINUS ), FormLink( L"LP71", CLink::BASE ) );
	Link( L"LP_69-73SP:69+", FormLink( L"LP69", CLink::PLUS ), FormLink( L"LP_69-73SP:69+~LP_9C" ) );
	Link( L"LP_69-73SP", FormLink( L"LP69", CLink::BASE ), FormLink( L"LP_65SP~LP_69-73SP" ) );
	Link( L"LP_65SP", FormLink( L"LP_65SP~LP_69-73SP" ), FormLink( L"LP65", CLink::BASE ) );
	Link( L"LP_65SP:65+", FormLink( L"LP65", CLink::PLUS ) );
	Link( L"LP_65SP:65-", FormLink( L"LP65", CLink::MINUS ) );
	Link( L"LP_9-13SP:13+", FormLink( L"LP9", CLink::BASE ) );
	Link( L"LP_9-13SP:13+,9-", FormLink( L"LP9", CLink::MINUS ) );
	Link( L"LP_9-13SP:13+,9+", FormLink( L"LP9", CLink::PLUS ), FormLink( L"LP_9-13SP:13+,9+~LP_NP" ) );
	Link( L"LP_NP", FormLink( L"LP_9-13SP:13+,9+~LP_NP" ), FormLink( L"LP_NGP~LP_NP" ) );
	Link( L"LP_NGP", FormLink( L"LP_NGP~LP_NP" ), FormLink( L"LP_NGP~LP_LT_1C" ) );
	Link( L"LP_LT_1C", FormLink( L"LP_NGP~LP_LT_1C" ) );
	Link( L"LP_30-32SP", FormLink( L"LP30", CLink::BASE ) );
	Link( L"LP_30-32SP:30+", FormLink( L"LP30", CLink::PLUS ) );
	Link( L"LP_30-32SP:30-", FormLink( L"LP30", CLink::MINUS ), FormLink( L"LP32", CLink::BASE ) );
	Link( L"LP_30-32SP:30-,32-", FormLink( L"LP32", CLink::MINUS ) );
	Link( L"LP_30-32SP:30-,32+", FormLink( L"LP32", CLink::PLUS ), FormLink( L"LP_30-32SP:30-,32+~LP_9C" ) );

	LinkSema( L"LP_LegN", L"LP_NGP", L"LP_NP", vector<wstring>( 1, L"LP_HeadN" ) );
	LinkSema( L"LP_LegM1", L"LP_NP", L"LP_9-13SP:13+,9+", vector<wstring>( 1, L"LP_HeadM1" ) );
}

void RealPart::CreateVangazi()
{
	const auto & vangaziCode = EsrKit( 9530 );
	const auto & vangIncCode = EsrKit( 9520, 9530 );
	const auto & vangKriCode = EsrKit( 9520, 9540 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"VG_2C", vangaziCode ), BadgeE( L"VG_3-9SP:5+,9+", vangaziCode ), BadgeE( L"VG_3-9SP:5+,9-", vangaziCode ), 
		BadgeE( L"VG_3-9SP:5+", vangaziCode ), BadgeE( L"VG_3-9SP:5-", vangaziCode ), BadgeE( L"VG_3-9SP", vangaziCode ), BadgeE( L"VG_3-9SP:3-", vangaziCode ),
		BadgeE( L"VG_3-9SP:3+", vangaziCode ), BadgeE( L"VG_NpDP", vangaziCode ), BadgeE( L"VG_VI6", vangIncCode ), BadgeE( L"VG_7-11SP:7-", vangaziCode ),
		BadgeE( L"VG_7-11SP", vangaziCode ), BadgeE( L"VG_7-11SP:11-", vangaziCode ), BadgeE( L"VG_7-11SP:11+", vangaziCode ), BadgeE( L"VG_1-11SP:1+,7+", vangaziCode ),
		BadgeE( L"VG_1SP:1-", vangaziCode ), BadgeE( L"VG_1SP", vangaziCode ), BadgeE( L"VG_NDP", vangaziCode ), BadgeE( L"VG_VI1", vangIncCode ),
		BadgeE( L"VG_1C", vangaziCode ), BadgeE( L"VG_3C", vangaziCode ), BadgeE( L"VG_2S-6SP:2S+,6+", vangaziCode ), BadgeE( L"VG_2S-6SP:2S+,6-", vangaziCode ), 
		BadgeE( L"VG_2S-6SP:2S+", vangaziCode ), BadgeE( L"VG_2S-6SP:2S-", vangaziCode ), BadgeE( L"VG_2S-6SP", vangaziCode ), BadgeE( L"VG_PDP", vangaziCode ), 
		BadgeE( L"VG_KV3", vangKriCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"VG_2C~VG_3-9SP:5+,9+", vangaziCode ), BadgeE( L"VG_3-9SP:3+~VG_NpDP", vangaziCode ), BadgeE( L"VG_NpDP~VG_VI6", vangaziCode ),
		BadgeE( L"VG_3-9SP:5-~VG_7-11SP:7-", vangaziCode ), BadgeE( L"VG_1SP:1-~VG_3-9SP:3-", vangaziCode ), BadgeE( L"VG_1SP-~VG_NDP", vangaziCode ),
		BadgeE( L"VG_NDP~VG_VI1", vangaziCode ), BadgeE( L"VG_2S-6SP:2S+,6-~VG_3C", vangaziCode ), BadgeE( L"VG_1C~VG_2S-6SP:2S+,6+", vangaziCode ),
		BadgeE( L"VG_2S-6SP:2S+,6-~VG_3C", vangaziCode ), BadgeE( L"VG_2S-6SP~VG_PDP", vangaziCode ), BadgeE( L"VG_KV3~VG_PDP", vangaziCode ),
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"VG1", vangaziCode ), BadgeE( L"VG3", vangaziCode ), BadgeE( L"VG5", vangaziCode ), BadgeE( L"VG7", vangaziCode ),
		BadgeE( L"VG9", vangaziCode ), BadgeE( L"VG11", vangaziCode ), BadgeE( L"VG6", vangaziCode ), BadgeE( L"VG2S", vangaziCode )
	};
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"VG_HeadP2", vangaziCode ), BadgeE( L"VG_HeadNp", vangaziCode ), BadgeE( L"VG_HeadN", vangaziCode ) };
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"VG_LegP2", vangaziCode ), BadgeE( L"VG_LegNp", vangaziCode ), BadgeE( L"VG_LegN", vangaziCode ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"VG_2C", FormLink( L"VG_2C~VG_3-9SP:5+,9+" ) );
	Link( L"VG_3-9SP:5+,9+", FormLink( L"VG_2C~VG_3-9SP:5+,9+" ), FormLink( L"VG9", CLink::PLUS ) );
	Link( L"VG_3-9SP:5+,9-", FormLink( L"VG9", CLink::MINUS ) );
	Link( L"VG_3-9SP:5+", FormLink( L"VG9", CLink::BASE ), FormLink( L"VG5", CLink::PLUS ) );
	Link( L"VG_3-9SP:5-", FormLink( L"VG5", CLink::MINUS ), FormLink( L"VG_3-9SP:5-~VG_7-11SP:7-" ) );
	Link( L"VG_7-11SP:7-", FormLink( L"VG7", CLink::MINUS ), FormLink( L"VG_3-9SP:5-~VG_7-11SP:7-" ) );
	Link( L"VG_7-11SP", FormLink( L"VG7", CLink::BASE ), FormLink( L"VG11", CLink::BASE ) );
	Link( L"VG_7-11SP:11-", FormLink( L"VG11", CLink::MINUS ) );
	Link( L"VG_7-11SP:11+", FormLink( L"VG11", CLink::PLUS ) );
	Link( L"VG_1-11SP:1+,7+", FormLink( L"VG7", CLink::PLUS ), FormLink( L"VG1", CLink::PLUS ) );
	Link( L"VG_1SP:1-", FormLink( L"VG1", CLink::MINUS ), FormLink( L"VG_1SP:1-~VG_3-9SP:3-" ) );
	Link( L"VG_1SP", FormLink( L"VG1", CLink::BASE ), FormLink( L"VG_1SP-~VG_NDP" ) );
	Link( L"VG_NDP", FormLink( L"VG_1SP-~VG_NDP" ), FormLink( L"VG_NDP~VG_VI1" ) );
	Link( L"VG_VI1", FormLink( L"VG_NDP~VG_VI1" ) );
	Link( L"VG_3-9SP", FormLink( L"VG5", CLink::BASE ), FormLink( L"VG3", CLink::BASE ) );
	Link( L"VG_3-9SP:3-", FormLink( L"VG3", CLink::MINUS ), FormLink( L"VG_1SP:1-~VG_3-9SP:3-" ) );
	Link( L"VG_3-9SP:3+", FormLink( L"VG_3-9SP:3+~VG_NpDP" ), FormLink( L"VG3", CLink::PLUS ) );
	Link( L"VG_NpDP", FormLink( L"VG_3-9SP:3+~VG_NpDP" ), FormLink( L"VG_NpDP~VG_VI6" ) );
	Link( L"VG_VI6", FormLink( L"VG_NpDP~VG_VI6" ) );
	Link( L"VG_1C", FormLink( L"VG_1C~VG_2S-6SP:2S+,6+" ) );
	Link( L"VG_3C", FormLink( L"VG_2S-6SP:2S+,6-~VG_3C" )  );
	Link( L"VG_2S-6SP:2S+,6+", FormLink( L"VG_1C~VG_2S-6SP:2S+,6+" ), FormLink( L"VG6", CLink::PLUS ) );
	Link( L"VG_2S-6SP:2S+,6-", FormLink( L"VG_2S-6SP:2S+,6-~VG_3C" ), FormLink( L"VG6", CLink::MINUS ) );
	Link( L"VG_2S-6SP:2S+", FormLink( L"VG6", CLink::BASE ), FormLink( L"VG2S", CLink::MINUS ) );
	Link( L"VG_2S-6SP:2S-", FormLink( L"VG2S", CLink::PLUS ) );
	Link( L"VG_2S-6SP", FormLink( L"VG2S", CLink::BASE ), FormLink( L"VG_2S-6SP~VG_PDP" ) );
	Link( L"VG_PDP", FormLink( L"VG_2S-6SP~VG_PDP" ), FormLink( L"VG_KV3~VG_PDP" ) );
	Link( L"VG_KV3", FormLink( L"VG_KV3~VG_PDP" ) );

	LinkSema( L"VG_LegP2", L"VG_2C", L"VG_3-9SP:5+,9+", vector<wstring>( 1, L"VG_HeadP2" ) );
	LinkSema( L"VG_LegNp", L"VG_VI6", L"VG_NpDP", vector<wstring>( 1, L"VG_HeadNp" ) );
	LinkSema( L"VG_LegN", L"VG_VI1", L"VG_NDP", vector<wstring>( 1, L"VG_HeadN" ) );
}

void RealPart::CreateSloka()
{
	const auto & slokaCode = EsrKit( 9530 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"SL_17sp:17/19-", slokaCode ), BadgeE( L"SL_17sp:17/19+", slokaCode ), BadgeE( L"SL_17sp", slokaCode ),
		BadgeE( L"SL_5/11sp:5/7+,9+", slokaCode ), BadgeE( L"SL_5/11sp:5/7+,9-", slokaCode ), BadgeE( L"SL_5/11sp:5/7+", slokaCode ),
		BadgeE( L"SL_5/11sp:5/7+,9-,11/13+", slokaCode ), BadgeE( L"SL_5/11sp:5/7+,9-,11/13-", slokaCode ), BadgeE( L"SL_5/11sp:5/7-", slokaCode ),
		BadgeE( L"SL_5/11sp", slokaCode ), BadgeE( L"SL_3sp", slokaCode ), BadgeE( L"SL_3sp:1/3-", slokaCode ), BadgeE( L"SL_3sp:1/3+", slokaCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"SL_17sp~SL_5/11sp:5/7+,9+", slokaCode ), BadgeE( L"SL_3sp~SL_5/11sp", slokaCode ) };
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"SL_3", slokaCode ), BadgeE( L"SL_5", slokaCode ), BadgeE( L"SL_9", slokaCode ), BadgeE( L"SL_11", slokaCode ),
		BadgeE( L"SL_17", slokaCode ) 
	};
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//связывание
	Link( L"SL_17sp:17/19-", FormLink( L"SL_17", CLink::MINUS ) );
	Link( L"SL_17sp:17/19+", FormLink( L"SL_17", CLink::PLUS ) );
	Link( L"SL_17sp", FormLink( L"SL_17", CLink::BASE ), FormLink( L"SL_17sp~SL_5/11sp:5/7+,9+" ) );
	Link( L"SL_5/11sp:5/7+,9+", FormLink( L"SL_9", CLink::PLUS ), FormLink( L"SL_17sp~SL_5/11sp:5/7+,9+" ) );
	Link( L"SL_5/11sp:5/7+", FormLink( L"SL_9", CLink::BASE ), FormLink( L"SL_5", CLink::PLUS ) );
	Link( L"SL_5/11sp:5/7+,9-", FormLink( L"SL_9", CLink::MINUS ), FormLink( L"SL_11", CLink::BASE ) );
	Link( L"SL_5/11sp:5/7+,9-,11/13+", FormLink( L"SL_11", CLink::PLUS ) );
	Link( L"SL_5/11sp:5/7+,9-,11/13-", FormLink( L"SL_11", CLink::MINUS ) );
	Link( L"SL_5/11sp:5/7-", FormLink( L"SL_5", CLink::PLUS ) );
	Link( L"SL_5/11sp", FormLink( L"SL_5", CLink::BASE ), FormLink( L"SL_3sp~SL_5/11sp" ) );
	Link( L"SL_3sp", FormLink( L"SL_3", CLink::BASE ), FormLink( L"SL_3sp~SL_5/11sp" ) );
	Link( L"SL_3sp:1/3-", FormLink( L"SL_3", CLink::MINUS ) );
	Link( L"SL_3sp:1/3+", FormLink( L"SL_3", CLink::PLUS ) );
}

void RealPart::CreateKarsava()
{
	const auto & karsavaCode = EsrKit( 11260 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"KR_5C", karsavaCode ), BadgeE( L"KR_13-17SP:15-,17-", karsavaCode ), BadgeE( L"KR_13-17SP:15-,17+", karsavaCode ),
		BadgeE( L"KR_13-17SP:15-", karsavaCode ), BadgeE( L"KR_3C", karsavaCode ), BadgeE( L"KR_13-17SP:15+", karsavaCode ), BadgeE( L"KR_13-17SP", karsavaCode ),
		BadgeE( L"KR_13-17SP:13-", karsavaCode ), BadgeE( L"KR_7-11SP:7+,11-", karsavaCode ), BadgeE( L"KR_13-17SP:13+", karsavaCode ),
		BadgeE( L"KR_1SP:1+", karsavaCode ), BadgeE( L"KR_1SP:1-", karsavaCode ), BadgeE( L"KR_1SP", karsavaCode ), BadgeE( L"KR_1C", karsavaCode ),
		BadgeE( L"KR_2-4SP:2+,4-", karsavaCode ), BadgeE( L"KR_2-4SP:2+,4+", karsavaCode ), BadgeE( L"KR_2-4SP:2+", karsavaCode ), 
		BadgeE( L"KR_2-4SP:2-", karsavaCode ), BadgeE( L"KR_2-4SP", karsavaCode ), BadgeE( L"KR_CP", karsavaCode ), BadgeE( L"KR_2C", karsavaCode ), 
		BadgeE( L"KR_19SP:19+", karsavaCode ), BadgeE( L"KR_19SP", karsavaCode ), BadgeE( L"KR_19SP:19-", karsavaCode ), BadgeE( L"KR_2AC", karsavaCode ), 
		BadgeE( L"KR_7-11SP:7-", karsavaCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"KR_13-17SP:15-,17-~KR_5C", karsavaCode ), BadgeE( L"KR_13-17SP:15+~KR_3C", karsavaCode ),
		BadgeE( L"KR_13-17SP:13+~KR_1SP:1+", karsavaCode ), BadgeE( L"KR_13-17SP:13-~KR_7-11SP:7+,11-", karsavaCode ),
		BadgeE( L"KR_1C~KR_2-4SP:2+,4-", karsavaCode ), BadgeE( L"KR_2-4SP~KR_CP", karsavaCode ), BadgeE( L"KR_2-4SP:2+,4+~KR_2C", karsavaCode ), 
		BadgeE( L"KR_19SP:19+~KR_2C", karsavaCode ), BadgeE( L"KR_19SP~KR_2AC", karsavaCode ), BadgeE( L"KR_2AC~KR_7-11SP:7-", karsavaCode )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"KR1", karsavaCode ), BadgeE( L"KR2", karsavaCode ), BadgeE( L"KR4", karsavaCode ), BadgeE( L"KR13", karsavaCode ), 
		BadgeE( L"KR15", karsavaCode ), BadgeE( L"KR17", karsavaCode ), BadgeE( L"KR19", karsavaCode )
	};
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"KR_HeadP3", karsavaCode ), BadgeE( L"KR_HeadP5", karsavaCode ) };
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"KR_LegP3", karsavaCode ), BadgeE( L"KR_LegP5", karsavaCode ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"KR_5C", FormLink( L"KR_13-17SP:15-,17-~KR_5C" ) );
	Link( L"KR_13-17SP:15-,17-", FormLink( L"KR17", CLink::MINUS ), FormLink( L"KR_13-17SP:15-,17-~KR_5C" ) );
	Link( L"KR_13-17SP:15-,17+", FormLink( L"KR17", CLink::PLUS ) );
	Link( L"KR_13-17SP:15-", FormLink( L"KR17", CLink::BASE ), FormLink( L"KR15", CLink::MINUS ) );
	Link( L"KR_3C", FormLink( L"KR_13-17SP:15+~KR_3C" ) );
	Link( L"KR_13-17SP:15+", FormLink( L"KR_13-17SP:15+~KR_3C" ), FormLink( L"KR15", CLink::PLUS ) );
	Link( L"KR_13-17SP", FormLink( L"KR15", CLink::BASE ), FormLink( L"KR13", CLink::BASE ) );
	Link( L"KR_13-17SP:13+", FormLink( L"KR13", CLink::PLUS ), FormLink( L"KR_13-17SP:13+~KR_1SP:1+" ) );
	Link( L"KR_1SP:1+", FormLink( L"KR_13-17SP:13+~KR_1SP:1+" ), FormLink( L"KR1", CLink::PLUS ) );
	Link( L"KR_1SP:1-", FormLink( L"KR1", CLink::MINUS ) );
	Link( L"KR_1SP", FormLink( L"KR1", CLink::BASE ) );
	Link( L"KR_13-17SP:13-", FormLink( L"KR13", CLink::MINUS ), FormLink( L"KR_13-17SP:13-~KR_7-11SP:7+,11-" ) );
	Link( L"KR_7-11SP:7+,11-", FormLink( L"KR_13-17SP:13-~KR_7-11SP:7+,11-" ) );
	Link( L"KR_1C", FormLink( L"KR_1C~KR_2-4SP:2+,4-" ) );
	Link( L"KR_2-4SP:2+,4-", FormLink( L"KR_1C~KR_2-4SP:2+,4-" ), FormLink( L"KR4", CLink::MINUS ) );
	Link( L"KR_2-4SP:2+,4+", FormLink( L"KR4", CLink::PLUS ), FormLink( L"KR_2-4SP:2+,4+~KR_2C" ) );
	Link( L"KR_2-4SP:2+", FormLink( L"KR4", CLink::BASE ), FormLink( L"KR2", CLink::PLUS ) );
	Link( L"KR_2-4SP:2-", FormLink( L"KR2", CLink::MINUS ) );
	Link( L"KR_2-4SP", FormLink( L"KR2", CLink::BASE ), FormLink( L"KR_2-4SP~KR_CP" ) );
	Link( L"KR_CP", FormLink( L"KR_2-4SP~KR_CP" ) );
	Link( L"KR_2C", FormLink( L"KR_2-4SP:2+,4+~KR_2C" ), FormLink( L"KR_19SP:19+~KR_2C" ) );
	Link( L"KR_19SP:19+", FormLink( L"KR_19SP:19+~KR_2C" ), FormLink( L"KR19", CLink::PLUS ) );
	Link( L"KR_19SP", FormLink( L"KR_19SP~KR_2AC" ), FormLink( L"KR19", CLink::BASE ) );
	Link( L"KR_19SP:19-", FormLink( L"KR19", CLink::MINUS ) );
	Link( L"KR_2AC", FormLink( L"KR_19SP~KR_2AC" ), FormLink( L"KR_2AC~KR_7-11SP:7-" ) );
	Link( L"KR_7-11SP:7-", FormLink( L"KR_2AC~KR_7-11SP:7-" ) );

	LinkSema( L"KR_LegP5", L"KR_5C", L"KR_13-17SP:15-,17-", vector<wstring>( 1, L"KR_HeadP5" ) );
	LinkSema( L"KR_LegP3", L"KR_3C", L"KR_13-17SP:15+", vector<wstring>( 1, L"KR_HeadP3" ) );
}

void RealPart::CreateKraslava()
{
	const auto & kraslavaCode = EsrKit( 11050 );
	//участки
	BadgeE strips_arr[] = { BadgeE( L"KS_7AC", kraslavaCode ), BadgeE( L"KS_12-20SP:12+", kraslavaCode ), BadgeE( L"KS_9C", kraslavaCode ),
		BadgeE( L"KS_12-20SP:12-,$", kraslavaCode ), BadgeE( L"KS_12-20SP:20+", kraslavaCode ), 
		BadgeE( L"KS_12-20SP:12-", kraslavaCode ), BadgeE( L"KS_12-20SP", kraslavaCode ), BadgeE( L"KS_10SP", kraslavaCode ), 
		BadgeE( L"KS_10SP:10+", kraslavaCode ), BadgeE( L"KS_10SP:10-", kraslavaCode ), BadgeE( L"KS_14SP:14-", kraslavaCode ), 
		BadgeE( L"KS_14SP:14+", kraslavaCode ), BadgeE( L"KS_14SP", kraslavaCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"KS_12-20SP:12-,$~KS_9C", kraslavaCode ), BadgeE( L"KS_12-20SP:12+~KS_7AC", kraslavaCode ),
		BadgeE( L"KS_10SP~KS_12-20SP", kraslavaCode ), BadgeE( L"KS_10SP:10+~KS_14SP:14-", kraslavaCode )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"KS10", kraslavaCode ), BadgeE( L"KS12", kraslavaCode ), BadgeE( L"KS14", kraslavaCode ), 
		BadgeE( L"KS20", kraslavaCode )
	};
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//связывание
	Link( L"KS_7AC", FormLink( L"KS_12-20SP:12+~KS_7AC" ) );
	Link( L"KS_12-20SP:12+", FormLink( L"KS12", CLink::PLUS ), FormLink( L"KS_12-20SP:12+~KS_7AC" ) );
	Link( L"KS_9C", FormLink( L"KS_12-20SP:12-,$~KS_9C" ) );
	Link( L"KS_12-20SP:12-,$", FormLink( L"KS_12-20SP:12-,$~KS_9C" ), FormLink( L"KS20", CLink::BASE ) );
	Link( L"KS_12-20SP:20+", FormLink( L"KS20", CLink::PLUS ) );
	Link( L"KS_12-20SP:12-", FormLink( L"KS20", CLink::MINUS ), FormLink( L"KS12", CLink::MINUS ) );
	Link( L"KS_12-20SP", FormLink( L"KS12", CLink::BASE ), FormLink( L"KS_10SP~KS_12-20SP" ) );
	Link( L"KS_10SP", FormLink( L"KS_10SP~KS_12-20SP" ), FormLink( L"KS10", CLink::BASE ) );
	Link( L"KS_10SP:10-", FormLink( L"KS10", CLink::MINUS ) );
	Link( L"KS_10SP:10+", FormLink( L"KS10", CLink::PLUS ), FormLink( L"KS_10SP:10+~KS_14SP:14-" ) );
	Link( L"KS_14SP:14-", FormLink( L"KS_10SP:10+~KS_14SP:14-" ), FormLink( L"KS14", CLink::MINUS ) );
	Link( L"KS_14SP:14+", FormLink( L"KS14", CLink::PLUS ) );
	Link( L"KS_14SP", FormLink( L"KS14", CLink::BASE ) );
}

void RealPart::CreateKrauja()
{
	const auto & kraujaCode = EsrKit( 11070 );
	const auto & kraujaCP401Code = EsrKit( 11063, 11070 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"KJ_N1P_KRA", kraujaCP401Code ), BadgeE( L"KJ_NAP", kraujaCode ), BadgeE( L"KJ_1-5SP", kraujaCode ), 
		BadgeE( L"KJ_1-5SP:1+", kraujaCode ), BadgeE( L"KJ_1-5SP:1-", kraujaCode ), BadgeE( L"KJ_3SP:3-", kraujaCode ), BadgeE( L"KJ_3SP:3+", kraujaCode ), 
		BadgeE( L"KJ_3SP", kraujaCode ), BadgeE( L"KJ_7-9SP", kraujaCode ), BadgeE( L"KJ_7-9SP:7-", kraujaCode ), BadgeE( L"KJ_7-9SP:7+", kraujaCode ), 
		BadgeE( L"KJ_3C", kraujaCode ), BadgeE( L"KJ_6-8SP:6+", kraujaCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"KJ_N1P_KRA~KJ_NAP", kraujaCode ), BadgeE( L"KJ_1-5SP~KJ_NAP", kraujaCode ), BadgeE( L"KJ_1-5SP:1-~KJ_3SP:3-", kraujaCode ), 
		BadgeE( L"KJ_3SP~KJ_7-9SP", kraujaCode ), BadgeE( L"KJ_3C~KJ_7-9SP:7+", kraujaCode ), BadgeE( L"KJ_3C~KJ_6-8SP:6+", kraujaCode ),
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"KJ1", kraujaCode ), BadgeE( L"KJ3", kraujaCode ), BadgeE( L"KJ7", kraujaCode ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"KJ_HeadP3", kraujaCode ), BadgeE( L"KJ_HeadN", kraujaCode ) };
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"KJ_LegP3", kraujaCode ), BadgeE( L"KJ_LegN", kraujaCode ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"KJ_N1P_KRA", FormLink( L"KJ_N1P_KRA~KJ_NAP" ) );
	Link( L"KJ_NAP", FormLink( L"KJ_1-5SP~KJ_NAP" ), FormLink( L"KJ_N1P_KRA~KJ_NAP" ) );
	Link( L"KJ_1-5SP", FormLink( L"KJ_1-5SP~KJ_NAP" ), FormLink( L"KJ1", CLink::BASE ) );
	Link( L"KJ_1-5SP:1+", FormLink( L"KJ1", CLink::PLUS ) );
	Link( L"KJ_1-5SP:1-", FormLink( L"KJ1", CLink::MINUS ), FormLink( L"KJ_1-5SP:1-~KJ_3SP:3-" ) );
	Link( L"KJ_3SP:3-", FormLink( L"KJ3", CLink::MINUS ), FormLink( L"KJ_1-5SP:1-~KJ_3SP:3-" ) );
	Link( L"KJ_3SP:3+", FormLink( L"KJ3", CLink::PLUS ) );
	Link( L"KJ_3SP", FormLink( L"KJ3", CLink::BASE ), FormLink( L"KJ_3SP~KJ_7-9SP" ) );
	Link( L"KJ_7-9SP", FormLink( L"KJ7", CLink::BASE ), FormLink( L"KJ_3SP~KJ_7-9SP" ) );
	Link( L"KJ_7-9SP:7-", FormLink( L"KJ7", CLink::MINUS ) );
	Link( L"KJ_7-9SP:7+", FormLink( L"KJ7", CLink::PLUS ), FormLink( L"KJ_3C~KJ_7-9SP:7+" ) );
	Link( L"KJ_3C", FormLink( L"KJ_3C~KJ_7-9SP:7+" ), FormLink( L"KJ_3C~KJ_6-8SP:6+" ) );
	Link( L"KJ_6-8SP:6+", FormLink( L"KJ_3C~KJ_6-8SP:6+" ) );

	LinkSema( L"KJ_LegP3", L"KJ_3C", L"KJ_7-9SP:7+", vector<wstring>( 1, L"KJ_HeadP3" ) );
	LinkSema( L"KJ_LegN", L"KJ_N1P_KRA", L"KJ_NAP", vector<wstring>( 1, L"KJ_HeadN" ) );
}

void RealPart::CreateMezvidi()
{
	const auto & mezvidiCode = EsrKit( 11266 );
	const auto & pureniCode = EsrKit( 11265 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"MZ_5C", mezvidiCode ), BadgeE( L"MZ_5C:6+", mezvidiCode ), BadgeE( L"MZ_5C:6-", mezvidiCode ), BadgeE( L"MZ_3C", mezvidiCode ), 
		BadgeE( L"MZ_1C", mezvidiCode ), BadgeE( L"MZ_1-3SP:1-,3-", mezvidiCode ), BadgeE( L"MZ_1-3SP:1-,3+", mezvidiCode ), BadgeE( L"MZ_1-3SP:1-", mezvidiCode ),
		BadgeE( L"MZ_1-3SP:1+", mezvidiCode ), BadgeE( L"MZ_1-3SP", mezvidiCode ), BadgeE( L"MZ_NP", mezvidiCode ), BadgeE( L"MZ_N1P_PUR", mezvidiCode ), 
		BadgeE( L"MZ_N3P", mezvidiCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"MZ_1-3SP:1-,3-~MZ_5C", mezvidiCode ), BadgeE( L"MZ_1-3SP:1-,3+~MZ_3C", mezvidiCode ),
		BadgeE( L"MZ_1C~MZ_1-3SP:1+", mezvidiCode ), BadgeE( L"MZ_1-3SP~MZ_NP", mezvidiCode ), BadgeE( L"MZ_N1P_PUR~MZ_NP", mezvidiCode ),
		BadgeE( L"MZ_N1P_PUR~MZ_N3P", EsrKit( mezvidiCode.getTerm(), pureniCode.getTerm() ) )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"MZ1", mezvidiCode ), BadgeE( L"MZ3", mezvidiCode ), BadgeE( L"MZ6", mezvidiCode ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"MZ_HeadP1", mezvidiCode ), BadgeE( L"MZ_HeadP3", mezvidiCode ), BadgeE( L"MZ_HeadP5", mezvidiCode ), 
		BadgeE( L"MZ_HeadN", mezvidiCode ) };
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"MZ_LegP1", mezvidiCode ), BadgeE( L"MZ_LegP3", mezvidiCode ), BadgeE( L"MZ_LegP5", mezvidiCode ),
		BadgeE( L"MZ_LegN", mezvidiCode ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"MZ_5C", FormLink( L"MZ_1-3SP:1-,3-~MZ_5C" ), FormLink( L"MZ6", CLink::BASE ) );
	Link( L"MZ_5C:6+", FormLink( L"MZ6", CLink::PLUS ) );
	Link( L"MZ_5C:6-", FormLink( L"MZ6", CLink::MINUS ) );
	Link( L"MZ_1-3SP:1-,3-", FormLink( L"MZ3", CLink::MINUS ), FormLink( L"MZ_1-3SP:1-,3-~MZ_5C" ) );
	Link( L"MZ_3C", FormLink( L"MZ_1-3SP:1-,3+~MZ_3C" ) );
	Link( L"MZ_1-3SP:1-,3+", FormLink( L"MZ3", CLink::PLUS ), FormLink( L"MZ_1-3SP:1-,3+~MZ_3C" ) );
	Link( L"MZ_1-3SP:1-", FormLink( L"MZ3", CLink::BASE ), FormLink( L"MZ1", CLink::MINUS ) );
	Link( L"MZ_1C", FormLink( L"MZ_1C~MZ_1-3SP:1+" ) );
	Link( L"MZ_1-3SP:1+", FormLink( L"MZ_1C~MZ_1-3SP:1+" ), FormLink( L"MZ1", CLink::PLUS ) );
	Link( L"MZ_1-3SP", FormLink( L"MZ1", CLink::BASE ), FormLink( L"MZ_1-3SP~MZ_NP" ) );
	Link( L"MZ_NP", FormLink( L"MZ_1-3SP~MZ_NP" ), FormLink( L"MZ_N1P_PUR~MZ_NP" ) );
	Link( L"MZ_N1P_PUR", FormLink( L"MZ_N1P_PUR~MZ_NP" ), FormLink( L"MZ_N1P_PUR~MZ_N3P" ) );
	Link( L"MZ_N3P", FormLink( L"MZ_N1P_PUR~MZ_N3P" ) );

	LinkSema( L"MZ_LegP1", L"MZ_1C", L"MZ_1-3SP:1+", vector<wstring>( 1, L"MZ_HeadP1" ) );
	LinkSema( L"MZ_LegP3", L"MZ_3C", L"MZ_1-3SP:1-,3+", vector<wstring>( 1, L"MZ_HeadP3" ) );
	LinkSema( L"MZ_LegP5", L"MZ_5C", L"MZ_1-3SP:1-,3-", vector<wstring>( 1, L"MZ_HeadP5" ) );
	LinkSema( L"MZ_LegN", L"MZ_N1P_PUR", L"MZ_NP", vector<wstring>( 1, L"MZ_HeadN" ) );
}

void RealPart::CreateSkirotava()
{
	const auto & skirotavaACode = EsrKit( 9000 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"SK_47-51SP:51-,49+,47+", skirotavaACode ), BadgeE( L"SK_4AAC", skirotavaACode ), BadgeE( L"SK_39SP:39+", skirotavaACode ),
		BadgeE( L"SK_39SP:39-", skirotavaACode ), BadgeE( L"SK_39SP", skirotavaACode ), BadgeE( L"SK_4ABC", skirotavaACode ), 
		BadgeE( L"SK_25SP:25-", skirotavaACode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"SK_47-51SP:51-,49+,47+~SK_4AAC", skirotavaACode ), BadgeE( L"SK_39SP:39+~SK_4AAC", skirotavaACode ),
		BadgeE( L"SK_39SP~SK_4ABC", skirotavaACode ), BadgeE( L"SK_25SP:25-~SK_4ABC", skirotavaACode )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"SK39", skirotavaACode ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"SK_HeadM27", skirotavaACode ), BadgeE( L"SK_HeadM29", skirotavaACode ) };
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"SK_LegM27", skirotavaACode ), BadgeE( L"SK_LegM29", skirotavaACode ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"SK_47-51SP:51-,49+,47+", FormLink( L"SK_47-51SP:51-,49+,47+~SK_4AAC" ) );
	Link( L"SK_4AAC", FormLink( L"SK_47-51SP:51-,49+,47+~SK_4AAC" ), FormLink( L"SK_39SP:39+~SK_4AAC" ) );
	Link( L"SK_39SP:39+", FormLink( L"SK_39SP:39+~SK_4AAC" ), FormLink( L"SK39", CLink::PLUS ) );
	Link( L"SK_39SP:39-", FormLink( L"SK39", CLink::MINUS ) );
	Link( L"SK_39SP", FormLink( L"SK_39SP~SK_4ABC" ), FormLink( L"SK39", CLink::BASE ) );
	Link( L"SK_4ABC", FormLink( L"SK_39SP~SK_4ABC" ), FormLink( L"SK_25SP:25-~SK_4ABC" ) );
	Link( L"SK_25SP:25-", FormLink( L"SK_25SP:25-~SK_4ABC" ) );

	LinkSema( L"SK_LegM27", L"SK_4ABC", L"SK_39SP", vector<wstring>( 1, L"SK_HeadM27" ) );
	LinkSema( L"SK_LegM29", L"SK_4AAC", L"SK_39SP:39+", vector<wstring>( 1, L"SK_HeadM29" ) );
}

void RealPart::CreateKemeri()
{
	const auto & kemeriCode = EsrKit( 9732 );
	const auto & tukumsCode = EsrKit( 9736 );
	const auto & kemtukCode = EsrKit( 9732, 9736 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"KM_CP", kemeriCode ), BadgeE( L"KM_CP:4+", kemeriCode ), BadgeE( L"KM_CP:4-", kemeriCode ), BadgeE( L"KM_3p", kemeriCode ),
		BadgeE( L"KM_CP:4+,8-", kemeriCode ), BadgeE( L"KM_CP:4+,8+", kemeriCode ), BadgeE( L"KM_1p", kemeriCode ), BadgeE( L"KM_2p", kemeriCode ),
		BadgeE( L"TK_NDP", tukumsCode ), BadgeE( L"KM_TK_CKPU1_T", kemtukCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"KM_1p~KM_CP:4+,8-", kemeriCode ), BadgeE( L"KM_2p~KM_CP:4+,8+", kemeriCode ), BadgeE( L"KM_3p~KM_CP:4-", kemeriCode ),
		BadgeE( L"KM_CP~KMTK_CKPU1_T", kemeriCode ), BadgeE( L"KM_TK_CKPU1_T~TK_NDP", kemtukCode )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"KM4", kemeriCode ), BadgeE( L"KM8", kemeriCode ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"KM_HeadN1", kemeriCode ), BadgeE( L"KM_HeadN2", kemeriCode ), BadgeE( L"KM_HeadN3", kemeriCode ), 
		BadgeE( L"KM_HeadP", kemeriCode ) 
	};
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"KM_LegN1", kemeriCode ), BadgeE( L"KM_LegN2", kemeriCode ), BadgeE( L"KM_LegN3", kemeriCode ),
		BadgeE( L"KM_LegP", kemeriCode )
	};
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"KM_CP", FormLink( L"KM4", CLink::BASE ), FormLink( L"KM_CP~KMTK_CKPU1_T" ) );
	Link( L"KM_CP:4-", FormLink( L"KM4", CLink::MINUS ), FormLink( L"KM_3p~KM_CP:4-" ) );
	Link( L"KM_3p", FormLink( L"KM_3p~KM_CP:4-" ) );
	Link( L"KM_CP:4+", FormLink( L"KM4", CLink::PLUS ), FormLink( L"KM8", CLink::BASE ) );
	Link( L"KM_CP:4+,8-", FormLink( L"KM8", CLink::MINUS ), FormLink( L"KM_1p~KM_CP:4+,8-" ) );
	Link( L"KM_1p", FormLink( L"KM_1p~KM_CP:4+,8-" ) );
	Link( L"KM_CP:4+,8+", FormLink( L"KM8", CLink::PLUS ), FormLink( L"KM_2p~KM_CP:4+,8+" ) );
	Link( L"KM_2p", FormLink( L"KM_2p~KM_CP:4+,8+" ) );
	Link( L"KM_TK_CKPU1_T", FormLink( L"KM_CP~KMTK_CKPU1_T" ), FormLink( L"KM_TK_CKPU1_T~TK_NDP" ) );
	Link( L"TK_NDP", FormLink( L"KM_TK_CKPU1_T~TK_NDP" ) );

	LinkSema( L"KM_LegN1", L"KM_1p", L"KM_CP:4+,8-", vector<wstring>( 1, L"KM_HeadN1" ) );
	LinkSema( L"KM_LegN2", L"KM_2p", L"KM_CP:4+,8+", vector<wstring>( 1, L"KM_HeadN2" ) );
	LinkSema( L"KM_LegN3", L"KM_3p", L"KM_CP:4-", vector<wstring>( 1, L"KM_HeadN3" ) );
	LinkSema( L"KM_LegP", L"KM_TK_CKPU1_T", L"KM_CP", vector<wstring>( 1, L"KM_HeadP" ) );
}

void RealPart::CreateRezekne2()
{
	const auto & rez2Code = EsrKit( 11310 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"RZ2_1C", rez2Code ), BadgeE( L"RZ2_43-45SP", rez2Code ), BadgeE( L"RZ2_43-45SP:43+", rez2Code ),
		BadgeE( L"RZ2_43-45SP:43-", rez2Code ), BadgeE( L"RZ2_43-45SP:43-,45+", rez2Code ), BadgeE( L"RZ2_43-45SP:43-,45-", rez2Code ),
		BadgeE( L"RZ2_47SP:47-", rez2Code ), BadgeE( L"RZ2_17-21SP:21-", rez2Code ), BadgeE( L"RZ2_17-21SP:21+", rez2Code ), BadgeE( L"RZ2_17-21SP", rez2Code ),
		BadgeE( L"RZ2_17-21SP:17-", rez2Code ), BadgeE( L"RZ2_17-21SP:17+", rez2Code ), BadgeE( L"RZ2_11SP:11+", rez2Code )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"RZ2_1C~RZ2_43-45SP", rez2Code ), BadgeE( L"RZ2_43-45SP:43-,45-~RZ2_47SP:47-", rez2Code ),
		BadgeE( L"RZ2_11SP:11+~RZ2_17-21SP:17+", rez2Code ), BadgeE( L"RZ2_17-21SP:21-~RZ2_1C", rez2Code ),
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"RZ2_17", rez2Code ), BadgeE( L"RZ2_21", rez2Code ), BadgeE( L"RZ2_43", rez2Code ), BadgeE( L"RZ2_45", rez2Code ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE sheads_arr[] = { BadgeE( L"RZ2_HeadM35", rez2Code ) };
	Insert( HEAD, sheads_arr, size_array( sheads_arr ), true, SHUNTING );
	BadgeE theads_arr[] = { BadgeE( L"RZ2_HeadPM1", rez2Code ) };
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"RZ2_LegM35", rez2Code ), BadgeE( L"RZ2_LegPM1", rez2Code ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"RZ2_1C", FormLink( L"RZ2_1C~RZ2_43-45SP" ), FormLink( L"RZ2_17-21SP:21-~RZ2_1C" ) );
	Link( L"RZ2_43-45SP", FormLink( L"RZ2_43", CLink::BASE ), FormLink( L"RZ2_1C~RZ2_43-45SP" ) );
	Link( L"RZ2_43-45SP:43+", FormLink( L"RZ2_43", CLink::PLUS ) );
	Link( L"RZ2_43-45SP:43-", FormLink( L"RZ2_43", CLink::MINUS ), FormLink( L"RZ2_45", CLink::BASE ) );
	Link( L"RZ2_43-45SP:43-,45+", FormLink( L"RZ2_45", CLink::PLUS ) );
	Link( L"RZ2_43-45SP:43-,45-", FormLink( L"RZ2_45", CLink::MINUS ), FormLink( L"RZ2_43-45SP:43-,45-~RZ2_47SP:47-" ) );
	Link( L"RZ2_47SP:47-", FormLink( L"RZ2_43-45SP:43-,45-~RZ2_47SP:47-" ) );
	Link( L"RZ2_17-21SP:21-", FormLink( L"RZ2_21", CLink::MINUS ), FormLink( L"RZ2_17-21SP:21-~RZ2_1C" ) );
	Link( L"RZ2_17-21SP:21+", FormLink( L"RZ2_21", CLink::PLUS ) );
	Link( L"RZ2_17-21SP", FormLink( L"RZ2_21", CLink::BASE ), FormLink( L"RZ2_17", CLink::BASE ) );
	Link( L"RZ2_17-21SP:17-", FormLink( L"RZ2_17", CLink::MINUS ) );
	Link( L"RZ2_17-21SP:17+", FormLink( L"RZ2_17", CLink::PLUS ), FormLink( L"RZ2_11SP:11+~RZ2_17-21SP:17+" ) );
	Link( L"RZ2_11SP:11+", FormLink( L"RZ2_11SP:11+~RZ2_17-21SP:17+" ) );

	LinkSema( L"RZ2_LegM35", L"RZ2_1C", L"RZ2_43-45SP", vector<wstring>( 1, L"RZ2_HeadM35" ) );
	LinkSema( L"RZ2_LegPM1", L"RZ2_1C", L"RZ2_17-21SP:21-", vector<wstring>( 1, L"RZ2_HeadPM1" ) );
}

void RealPart::CreateJelgava()
{
	const auto & jelCode = EsrKit( 9180 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"JE_12SP", jelCode ), BadgeE( L"JE_12SP:12-", jelCode ), BadgeE( L"JE_12SP:12+", jelCode ),
		BadgeE( L"JE_12/65P", jelCode ), BadgeE( L"JE_56SP:56+", jelCode ), BadgeE( L"JE_56SP:56-", jelCode ),
		BadgeE( L"JE_56SP", jelCode ), BadgeE( L"JE_58SP", jelCode ), BadgeE( L"JE_58SP:58+", jelCode ),
		BadgeE( L"JE_58SP:58-", jelCode ), BadgeE( L"JE_62SP:62+", jelCode ), BadgeE( L"JE_62SP:62-", jelCode ),
		BadgeE( L"JE_62SP", jelCode ), BadgeE( L"JE_76SP", jelCode ), BadgeE( L"JE_76SP:76+", jelCode ), 
		BadgeE( L"JE_76SP:76-", jelCode ), BadgeE( L"JE_17C", jelCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"JE_12/65P~JE_12SP:12+", jelCode ), BadgeE( L"JE_12/65P~JE_56SP:56+", jelCode ),
		BadgeE( L"JE_56SP~JE_58SP", jelCode ), BadgeE( L"JE_58SP:58-~JE_62SP:62+", jelCode ),
		BadgeE( L"JE_62SP~JE_76SP", jelCode ), BadgeE( L"JE_17C~JE_76SP:76-", jelCode ) };
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

		//стрелки
	BadgeE switches_arr[] = { BadgeE( L"JE12", jelCode ), BadgeE( L"JE56", jelCode ), BadgeE( L"JE58", jelCode ), 
		BadgeE( L"JE62", jelCode ), BadgeE( L"JE76", jelCode ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//связывание
	Link( L"JE_12SP", FormLink( L"JE12", CLink::BASE ) );
	Link( L"JE_12SP:12-", FormLink( L"JE12", CLink::MINUS ) );
	Link( L"JE_12SP:12+", FormLink( L"JE12", CLink::PLUS ), FormLink( L"JE_12/65P~JE_12SP:12+" ) );
	Link( L"JE_12/65P", FormLink( L"JE_12/65P~JE_12SP:12+" ), FormLink( L"JE_12/65P~JE_56SP:56+" ) );
	Link( L"JE_56SP:56+", FormLink( L"JE_12/65P~JE_56SP:56+" ), FormLink( L"JE56", CLink::PLUS ) );
	Link( L"JE_56SP:56-", FormLink( L"JE56", CLink::MINUS ) );
	Link( L"JE_56SP", FormLink( L"JE56", CLink::BASE ), FormLink( L"JE_56SP~JE_58SP" ) );
	Link( L"JE_58SP", FormLink( L"JE_56SP~JE_58SP" ), FormLink( L"JE58", CLink::BASE ) );
	Link( L"JE_58SP:58+", FormLink( L"JE58", CLink::PLUS ) );
	Link( L"JE_58SP:58-", FormLink( L"JE58", CLink::MINUS ), FormLink( L"JE_58SP:58-~JE_62SP:62+" ) );
	Link( L"JE_62SP:62+", FormLink( L"JE_58SP:58-~JE_62SP:62+" ), FormLink( L"JE62", CLink::PLUS ) );
	Link( L"JE_62SP:62-", FormLink( L"JE62", CLink::MINUS ) );
	Link( L"JE_62SP", FormLink( L"JE62", CLink::BASE ), FormLink( L"JE_62SP~JE_76SP" ) );
	Link( L"JE_76SP", FormLink( L"JE_62SP~JE_76SP" ), FormLink( L"JE76", CLink::BASE ) );
	Link( L"JE_76SP:76+", FormLink( L"JE76", CLink::PLUS ) );
	Link( L"JE_76SP:76-", FormLink( L"JE76", CLink::MINUS ), FormLink( L"JE_17C~JE_76SP:76-" ) );
	Link( L"JE_17C", FormLink( L"JE_17C~JE_76SP:76-" ) );
}

void RealPart::CreateBale()
{
	const auto & baleCode = EsrKit( 11101 );
	const auto & balevalmCode = EsrKit( 11101, 11100);
	const auto & balelodeCode = EsrKit( 11101, 11110);

	//участки
	BadgeE strips_arr[] = { BadgeE( L"BA_LB7", balelodeCode ), BadgeE( L"BA_LB9", balelodeCode ), 
		BadgeE( L"BA_LB11", balelodeCode ), BadgeE( L"BA_PDP", baleCode ), BadgeE( L"BA_4SP", baleCode ), 
		BadgeE( L"BA_4SP:4-", baleCode ), BadgeE( L"BA_4SP:4+", baleCode ), BadgeE( L"BA_1C", baleCode ), 
		BadgeE( L"BA_2C", baleCode ), BadgeE( L"BA_1SP:1+", baleCode ), BadgeE( L"BA_1SP:1-", baleCode ), 
		BadgeE( L"BA_1SP", baleCode ), BadgeE( L"BA_NDP", baleCode ), BadgeE( L"BA_BV1", balevalmCode ), 
		BadgeE( L"BA_BV3", balevalmCode ), BadgeE( L"BA_BV5", balevalmCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"BA_LB7~BA_LB9", balelodeCode ), BadgeE( L"BA_LB11~BA_LB9", balelodeCode ), 
		BadgeE( L"BA_LB11~BA_PDP", baleCode ), BadgeE( L"BA_4SP~BA_PDP", baleCode ), BadgeE( L"BA_1C~BA_4SP:4-", baleCode ), 
		BadgeE( L"BA_1C~BA_1SP:1+", baleCode ), BadgeE( L"BA_2C~BA_4SP:4+", baleCode ), BadgeE( L"BA_1SP:1-~BA_2C", baleCode ), 
		BadgeE( L"BA_1SP~BA_NDP", baleCode ), BadgeE( L"BA_BV1~BA_NDP", baleCode ), BadgeE( L"BA_BV1~BA_BV3", balevalmCode ), 
		BadgeE( L"BA_BV3~BA_BV5", balevalmCode )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"BA1", baleCode ), BadgeE( L"BA4", baleCode ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//связывание
	Link( L"BA_LB7", FormLink( L"BA_LB7~BA_LB9" ) );
	Link( L"BA_LB9", FormLink( L"BA_LB7~BA_LB9" ), FormLink( L"BA_LB11~BA_LB9" ) );
	Link( L"BA_LB11", FormLink( L"BA_LB11~BA_LB9" ), FormLink( L"BA_LB11~BA_PDP" ) );
	Link( L"BA_PDP", FormLink( L"BA_LB11~BA_PDP" ), FormLink( L"BA_4SP~BA_PDP" ) );
	Link( L"BA_4SP", FormLink( L"BA_4SP~BA_PDP" ), FormLink( L"BA4", CLink::BASE ) );
	Link( L"BA_4SP:4-", FormLink( L"BA_1C~BA_4SP:4-" ), FormLink( L"BA4", CLink::MINUS ) );
	Link( L"BA_4SP:4+", FormLink( L"BA_2C~BA_4SP:4+" ), FormLink( L"BA4", CLink::PLUS ) );
	Link( L"BA_1C", FormLink( L"BA_1C~BA_4SP:4-" ), FormLink( L"BA_1C~BA_1SP:1+" ) );
	Link( L"BA_2C", FormLink( L"BA_2C~BA_4SP:4+" ), FormLink( L"BA_1SP:1-~BA_2C" ) );
	Link( L"BA_1SP:1+", FormLink( L"BA_1C~BA_1SP:1+" ), FormLink( L"BA1", CLink::PLUS ) );
	Link( L"BA_1SP:1-", FormLink( L"BA_1SP:1-~BA_2C" ), FormLink( L"BA1", CLink::MINUS ) );
	Link( L"BA_1SP", FormLink( L"BA_1SP~BA_NDP" ), FormLink( L"BA1", CLink::BASE ) );
	Link( L"BA_NDP", FormLink( L"BA_1SP~BA_NDP" ), FormLink( L"BA_BV1~BA_NDP" ) );
	Link( L"BA_BV1", FormLink( L"BA_BV1~BA_NDP" ), FormLink( L"BA_BV1~BA_BV3" ) );
	Link( L"BA_BV3", FormLink( L"BA_BV1~BA_BV3" ), FormLink( L"BA_BV3~BA_BV5" ) );
	Link( L"BA_BV5", FormLink( L"BA_BV3~BA_BV5" ) );
}

void RealPart::SetDetails()
{
	SetStripDetails();
	SetSemaDetails();
}

void RealPart::SetStripDetails()
{
	//доп.атрибуты (номера п/о путей, парков и т.д.)
	list<CLogicStrip *> arrdep_strips;
	wstring adStripsNames[] = { L"LG_1C", L"LG_2C", L"LG_3C", L"VG_2C", L"KR_3C", L"KR_5C", L"SK_4AAC", L"SK_39SP:39+", L"SK_39SP", L"SK_4ABC", L"KM_1p",
		L"KM_2p", L"KM_3p", L"RZ2_1C", L"RZ2_43-45SP"
	};
	for_each( begin( adStripsNames ), end( adStripsNames ), [&arrdep_strips, this]( wstring arrdepName ){
		arrdep_strips.push_back( static_cast <CLogicStrip *>( Get( arrdepName )->lePtr.get() ) );
	} );
	for( CLogicStrip * arrdep_strip : arrdep_strips )
	{
		auto strip_name = arrdep_strip->GetName();
		if ( strip_name == L"LG_2C" || strip_name == L"KM_2p" )
			arrdep_strip->SetWaynum( 2 );
		else if ( strip_name == L"LG_3C" || strip_name == L"LG_5SP:5+" || strip_name == L"LG_5SP" || strip_name == L"KR_3C" || strip_name == L"MZ_3C" ||
				strip_name == L"KM_3p" )
			arrdep_strip->SetWaynum( 3 );
		else if ( strip_name == L"SK_4AAC" || strip_name == L"SK_39SP:39+" || strip_name == L"SK_39SP" || strip_name == L"SK_4ABC" )
			arrdep_strip->SetWaynum( 4 );
		else if ( strip_name == L"KR_5C" || strip_name == L"MZ_5C" )
			arrdep_strip->SetWaynum( 5 );
		else
			arrdep_strip->SetWaynum( 1 );

		if ( strip_name == L"LG_1C" || strip_name == L"LG_2C" || strip_name == L"LG_3C" || strip_name == L"LG_5SP:5+" )
			arrdep_strip->SetParknum( 1 );
	}
}

void RealPart::SetSemaDetails()
{
	//светофоры
	//нечетные:
	wstring oddSemaNames[] = { L"LG_LegN", L"LG_LegN1", L"LG_LegN2", L"LG_LegN3", L"LP_LegN", L"LP_LegM1", L"VG_LegNp", L"VG_LegN", L"MZ_LegN", L"SK_LegM27", 
		L"KM_LegN1", L"KM_LegN2", L"KM_LegN3", L"KJ_LegN"
	};
	for ( auto oddSemaName : oddSemaNames )
	{
		CLogicSemaleg * oddleg = static_cast <CLogicSemaleg *>( Get( oddSemaName )->lePtr.get() );
		oddleg->SetOddness( Oddness::ODD );
	}

	//четные:
	wstring evenSemaNames[] = { L"LG_LegP", L"LG_LegP1", L"LG_LegP2", L"LG_LegP3", L"RG_LegP10", L"VG_LegP2", L"KR_LegP3", L"KR_LegP5", L"MZ_LegP1", L"MZ_LegP3",
		L"MZ_LegP5", L"SK_LegM29", L"KM_LegP", L"KJ_LegP3"
	};
	for ( auto evenSemaName : evenSemaNames )
	{
		CLogicSemaleg * evenleg = static_cast <CLogicSemaleg *>( Get( evenSemaName )->lePtr.get() );
		evenleg->SetOddness( Oddness::EVEN );
	}
}
