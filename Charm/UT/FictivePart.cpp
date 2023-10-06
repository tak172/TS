#include "stdafx.h"
#include "FictivePart.h"
#include "../StatBuilder/LogicJoint.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/LogicSemaleg.h"

using namespace std;

FictivePart::FictivePart( const std::wstring & _axis_name ) : axis_name( _axis_name )
{
	CreateFirstPart();
	CreateSecondPart();
	CreateThirdPart();
	CreateFourthPart();
	CreateFifthPart();
	CreateWrongWayPart();
	CreatePocketPart();
	CreateBlockPostPart();
	CreateTechnicalStationPart();
	CreateLongSpan();
	SetDetails();
}

void FictivePart::CreateFirstPart()
{
	const auto & spanCode1Out = EsrKit( 100, 99999 );
	const auto & spanCode12 = EsrKit( 101, 102 );
	const auto & spanCode23 = EsrKit( 102, 103 );
	const auto & statCode1 = EsrKit( 101 );
	const auto & statCode2 = EsrKit( 102 );
	const auto & statCode3 = EsrKit( 103 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"K", spanCode1Out ), BadgeE( L"H", spanCode1Out ), BadgeE( L"W", spanCode1Out ), BadgeE( L"Q", spanCode1Out ), 
		BadgeE( L"A", statCode1 ), BadgeE( L"B", statCode1 ), BadgeE( L"C", statCode1 ), BadgeE( L"D", statCode1 ), BadgeE( L"E", statCode1 ), 
		BadgeE( L"F", statCode1 ), BadgeE( L"G", statCode1 ), BadgeE( L"S", statCode1 ), BadgeE( L"T", statCode1 ),
		BadgeE( L"X", spanCode12 ), BadgeE( L"I", spanCode12 ), BadgeE( L"AF", spanCode12 ),
		BadgeE( L"J", statCode2 ), BadgeE( L"L", statCode2 ), BadgeE( L"M", statCode2 ), BadgeE( L"N", statCode2 ),
		BadgeE( L"O", statCode2 ), BadgeE( L"P", statCode2 ),
		BadgeE( L"R", spanCode23 ), BadgeE( L"V", spanCode23 ), BadgeE( L"Y", spanCode23 ), 
		BadgeE( L"Z", statCode3 ), BadgeE( L"AA", statCode3 ), BadgeE( L"AB", statCode3 ), BadgeE( L"AC", statCode3 ), BadgeE( L"AD", statCode3 ), 
		BadgeE( L"AE", statCode3 ), BadgeE( L"AG", statCode3 ), BadgeE( L"AH", statCode3 ), BadgeE( L"AI", statCode3 ), BadgeE( L"AK", statCode3 ),
		BadgeE( L"AL", statCode3 ), BadgeE( L"AM", statCode3 ), BadgeE( L"AN", spanCode23 ), BadgeE( L"AO", statCode3 )
	};
	BadgeE nots_strips_arr[] = { BadgeE( L"U", statCode1 ) };
	Insert( STRIP, strips_arr, size_array( strips_arr ) );
	Insert( STRIP, nots_strips_arr, size_array( nots_strips_arr ), false );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"H~K", spanCode1Out ), BadgeE( L"H~W", spanCode1Out ), BadgeE( L"Q~W", spanCode1Out ), BadgeE( L"A~Q", statCode1 ), 
		BadgeE( L"B~C", statCode1 ), BadgeE( L"C~F", statCode1 ), BadgeE( L"G~X", statCode1 ), BadgeE( L"I~X", spanCode12 ), BadgeE( L"I~J", spanCode12 ), 
		BadgeE( L"L~M", statCode2 ), BadgeE( L"M~O", statCode2 ), BadgeE( L"P~R", spanCode23 ), BadgeE( L"R~V", spanCode23 ), BadgeE( L"V~Y", spanCode23 ),
		BadgeE( L"Y~Z", spanCode23 ), BadgeE( L"AA~Z", statCode3 ), BadgeE( L"AB~AC", statCode3 ), BadgeE( L"AD~AE", statCode3 ), BadgeE( L"AC~AG", statCode3 ),
		BadgeE( L"AE~AH", statCode3 ), BadgeE( L"AI~AK", statCode3 ), BadgeE( L"AK~AL", statCode3 ), BadgeE( L"AL~AM", statCode3 ), BadgeE( L"AM~AN", statCode3 ),
		BadgeE( L"AH~AO", statCode3 )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"11", statCode1 ), BadgeE( L"13", statCode1 ), BadgeE( L"15", statCode2 ), BadgeE( L"16", statCode2 ), 
		BadgeE( L"12", statCode1 ), BadgeE( L"14", statCode1 ), BadgeE( L"17", statCode3 )
	};
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE heads_arr[] = { BadgeE( L"Head100", statCode1 ), BadgeE( L"Head101", statCode1 ), BadgeE( L"Head102", statCode1 ), BadgeE( L"Head103", statCode3 ),
		BadgeE( L"Head104", statCode3 ), BadgeE( L"Head105", statCode1 ), BadgeE( L"Head106", statCode2 ), BadgeE( L"Head107", statCode3 ), 
		BadgeE( L"Head108", statCode2 )
	};
	Insert( HEAD, heads_arr, size_array( heads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"Leg100", statCode1 ), BadgeE( L"Leg101", statCode1 ), BadgeE( L"Leg102", statCode1 ), BadgeE( L"Leg103", statCode3 ),
		BadgeE( L"Leg104", statCode3 ), BadgeE( L"Leg105", statCode1 ), BadgeE( L"Leg106", statCode2 ), BadgeE( L"Leg107", statCode3 ), 
		BadgeE( L"Leg108", statCode2 )
	};
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//маршруты
	CLink rtlinks[] = { CLink( HEAD, BadgeE( L"Head102", statCode1 ) ), CLink( STRIP, BadgeE( L"F", statCode1 ) ), CLink( STRIP, BadgeE( L"G", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head102->G", statCode1 ), vector<CLink>( begin( rtlinks ), end( rtlinks ) ) );
	CLink rtlinks2[] = { CLink( HEAD, BadgeE( L"Head106", statCode2 ) ), CLink( STRIP, BadgeE( L"O", statCode2 ) ), CLink( STRIP, BadgeE( L"P", statCode2 ) ) };
	InsertRoute( BadgeE( L"Head106->P", statCode2 ), vector<CLink>( begin( rtlinks2 ), end( rtlinks2 ) ) );
	CLink rtlinks3[] = { CLink( HEAD, BadgeE( L"Head101", statCode1 ) ), CLink( STRIP, BadgeE( L"B", statCode1 ) ), CLink( STRIP, BadgeE( L"A", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head101->A", statCode1 ), vector<CLink>( begin( rtlinks3 ), end( rtlinks3 ) ) );
	CLink rtlinks4[] = { CLink( HEAD, BadgeE( L"Head100", statCode1 ) ), CLink( STRIP, BadgeE( L"A", statCode1 ) ), CLink( STRIP, BadgeE( L"C", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head100->C", statCode1 ), vector<CLink>( begin( rtlinks4 ), end( rtlinks4 ) ) );
	CLink rtlinks5[] = { CLink( HEAD, BadgeE( L"Head107", statCode3 ) ), CLink( STRIP, BadgeE( L"AK", statCode3 ) ), CLink( STRIP, BadgeE( L"AM", statCode3 ) ) };
	InsertRoute( BadgeE( L"Head107->AM", statCode3 ), vector<CLink>( begin( rtlinks5 ), end( rtlinks5 ) ) );
	CLink rtlinks6[] = { CLink( HEAD, BadgeE( L"Head103", statCode3 ) ), CLink( STRIP, BadgeE( L"AB", statCode3 ) ), CLink( STRIP, BadgeE( L"AA", statCode3 ) ),
		CLink( STRIP, BadgeE( L"Z", statCode3 ) ),
	};
	InsertRoute( BadgeE( L"Head103->Z", statCode3 ), vector<CLink>( begin( rtlinks6 ), end( rtlinks6 ) ) );

	//связывание
	Link( L"K", FormLink( L"H~K" ) );
	Link( L"H", FormLink( L"H~W" ), FormLink( L"H~K" ) );
	Link( L"W", FormLink( L"Q~W" ), FormLink( L"H~W" ) );
	Link( L"Q", FormLink( L"A~Q" ), FormLink( L"Q~W" ) );
	Link( L"A", FormLink( L"11", CLink::BASE ), FormLink( L"A~Q" ) );
	Link( L"B", FormLink( L"11", CLink::MINUS ), FormLink( L"B~C" ) );
	Link( L"C", FormLink( L"B~C" ), FormLink( L"C~F" ) );
	Link( L"D", FormLink( L"11", CLink::PLUS ), FormLink( L"12", CLink::BASE ) );
	Link( L"E", FormLink( L"13", CLink::MINUS ), FormLink( L"12", CLink::MINUS ) );
	Link( L"S", FormLink( L"12", CLink::PLUS ), FormLink( L"14", CLink::MINUS ) );
	Link( L"F", FormLink( L"C~F" ), FormLink( L"13", CLink::PLUS ) );
	Link( L"G", FormLink( L"13", CLink::BASE ), FormLink( L"G~X" ) );
	Link( L"X", FormLink( L"G~X" ), FormLink( L"I~X" ) );
	Link( L"I", FormLink( L"I~X" ), FormLink( L"I~J" ) );
	Link( L"J", FormLink( L"I~J" ), FormLink( L"15", CLink::BASE ) );
	Link( L"L", FormLink( L"15", CLink::MINUS ), FormLink( L"L~M" ) );
	Link( L"M", FormLink( L"L~M" ), FormLink( L"M~O" ) );
	Link( L"N", FormLink( L"15", CLink::PLUS ), FormLink( L"16", CLink::MINUS ) );
	Link( L"O", FormLink( L"16", CLink::PLUS ), FormLink( L"M~O" ) );
	Link( L"P", FormLink( L"P~R" ), FormLink( L"16", CLink::BASE ) );
	Link( L"R", FormLink( L"P~R" ), FormLink( L"R~V" ) );
	Link( L"T", FormLink( L"14", CLink::BASE ) );
	Link( L"U", FormLink( L"14", CLink::PLUS ) );
	Link( L"V", FormLink( L"R~V" ), FormLink( L"V~Y" ) );
	Link( L"Y", FormLink( L"V~Y" ), FormLink( L"Y~Z" ) );
	Link( L"Z", FormLink( L"Y~Z" ), FormLink( L"AA~Z" ) );
	Link( L"AA", FormLink( L"AA~Z" ), FormLink( L"17", CLink::BASE ) );
	Link( L"AB", FormLink( L"17", CLink::MINUS ), FormLink( L"AB~AC" ) );
	Link( L"AC", FormLink( L"AB~AC" ), FormLink( L"AC~AG" ) );
	Link( L"AD", FormLink( L"17", CLink::PLUS ), FormLink( L"AD~AE" ) );
	Link( L"AE", FormLink( L"AD~AE" ), FormLink( L"AE~AH" ) );
	Link( L"AG", FormLink( L"AC~AG" ) );
	Link( L"AH", FormLink( L"AE~AH" ), FormLink( L"AH~AO" ) );
	Link( L"AI", FormLink( L"AI~AK" ) );
	Link( L"AK", FormLink( L"AI~AK" ), FormLink( L"AK~AL" ) );
	Link( L"AL", FormLink( L"AK~AL" ), FormLink( L"AL~AM" ) );
	Link( L"AM", FormLink( L"AL~AM" ), FormLink( L"AM~AN" ) );
	Link( L"AN", FormLink( L"AM~AN" ) );
	Link( L"AO", FormLink( L"AH~AO" ) );

	LinkSema( L"Leg100", L"Q", L"A", vector<wstring>( 1, L"Head100" ) );
	LinkSema( L"Leg101", L"C", L"B", vector<wstring>( 1, L"Head101" ) );
	LinkSema( L"Leg102", L"C", L"F", vector<wstring>( 1, L"Head102" ) );
	LinkSema( L"Leg103", L"AC", L"AB", vector<wstring>( 1, L"Head103" ) );
	LinkSema( L"Leg104", L"Y", L"Z", vector<wstring>( 1, L"Head104" ) );
	LinkSema( L"Leg105", L"X", L"G", vector<wstring>( 1, L"Head105" ) );
	LinkSema( L"Leg106", L"M", L"O", vector<wstring>( 1, L"Head106" ) );
	LinkSema( L"Leg107", L"AI", L"AK", vector<wstring>( 1, L"Head107" ) );
	LinkSema( L"Leg108", L"I", L"J", vector<wstring>( 1, L"Head108" ) );
}

void FictivePart::CreateSecondPart()
{
	const auto & statCode1 = EsrKit( 201 );
	const auto & statCode2 = EsrKit( 202 );
	const auto & statCode3 = EsrKit( 203 );
	const auto & statCode4 = EsrKit( 204 );
	const auto & spanCode34 = EsrKit( 203, 204 );

	//участки
	BadgeE strips_arr[] = { 
		BadgeE( L"I2", statCode1 ), BadgeE( L"T2", statCode1 ), BadgeE( L"U2", statCode1 ),
		BadgeE( L"A2", statCode2 ), BadgeE( L"B2", statCode2 ), BadgeE( L"C2", statCode2 ), BadgeE( L"D2", statCode2 ), BadgeE( L"E2", statCode2 ), 
		BadgeE( L"F2", statCode2 ), BadgeE( L"G2", statCode2 ), BadgeE( L"H2", statCode2 ), BadgeE( L"K2", statCode2 ), BadgeE( L"L2", statCode2 ), 
		BadgeE( L"M2", statCode2 ), BadgeE( L"N2", statCode2 ), BadgeE( L"O2", statCode2 ), BadgeE( L"P2", statCode2 ), BadgeE( L"R2", statCode2 ), 
		BadgeE( L"S2", statCode2 ),
		BadgeE( L"V2", statCode3 ), BadgeE( L"W2", statCode3 ), 
		BadgeE( L"X2", spanCode34 ), BadgeE( L"Y2", spanCode34 ), 
		BadgeE( L"Z2", statCode4 ), BadgeE( L"AA2", statCode4 )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"G2~H2", statCode2 ), BadgeE( L"A2~B2", statCode2 ), BadgeE( L"F2~L2", statCode2 ), BadgeE( L"D2~M2", statCode2 ),
		BadgeE( L"K2~N2", statCode2 ), BadgeE( L"M2~O2", statCode2 ), BadgeE( L"O2~P2", statCode2 ), BadgeE( L"I2~R2", statCode2 ), BadgeE( L"I2~T2", statCode1 ),
		BadgeE( L"T2~U2", statCode1 ), BadgeE( L"V2~W2", statCode3 ), BadgeE( L"W2~X2", statCode3 ), BadgeE( L"X2~Y2", spanCode34 ), BadgeE( L"Y2~Z2", statCode4 ),
		BadgeE( L"AA2~Z2", statCode4 )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"24", statCode2 ), BadgeE( L"22", statCode2 ), BadgeE( L"23", statCode2 ), BadgeE( L"21", statCode2 ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE shunt_heads_arr[] = { BadgeE( L"Head200", statCode2 ), BadgeE( L"Head201S", statCode2 ), BadgeE( L"Head203S", statCode2 ), BadgeE( L"Head205S", statCode1 ) };
	Insert( HEAD, shunt_heads_arr, size_array( shunt_heads_arr ), true, SHUNTING );
	BadgeE train_heads_arr[] = { BadgeE( L"Head201T", statCode2 ), BadgeE( L"Head203T", statCode2 ), BadgeE( L"Head205T", statCode1 ), BadgeE( L"Head202", statCode3 ),
		BadgeE( L"Head204", statCode4 )
	};
	Insert( HEAD, train_heads_arr, size_array( train_heads_arr ), true, TRAIN );

	BadgeE legs_arr[] = { BadgeE( L"Leg200", statCode2 ), BadgeE( L"Leg201", statCode2 ), BadgeE( L"Leg202", statCode3 ), BadgeE( L"Leg203", statCode2 ), 
		BadgeE( L"Leg204", statCode4 ), BadgeE( L"Leg205", statCode1 ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//маршруты
	CLink links1[] = { CLink( HEAD, BadgeE( L"Head203S", statCode2 ) ), CLink( STRIP, BadgeE( L"O2", statCode2 ) ), CLink( STRIP, BadgeE( L"R2", statCode2 ) ), 
		CLink( STRIP, BadgeE( L"T2", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head203S->T2", statCode2 ), vector <CLink>( begin( links1 ), end( links1 ) ) );
	CLink links2[] = { CLink( HEAD, BadgeE( L"Head203T", statCode2 ) ), CLink( STRIP, BadgeE( L"O2", statCode2 ) ), CLink( STRIP, BadgeE( L"R2", statCode2 ) ), 
		CLink( STRIP, BadgeE( L"T2", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head203T->T2", statCode2 ), vector <CLink>( begin( links2 ), end( links2 ) ) );

	//связывание
	Link( L"A2", FormLink( L"A2~B2" ) );
	Link( L"B2", FormLink( L"A2~B2" ), FormLink( L"24", CLink::BASE ) );
	Link( L"C2", FormLink( L"24", CLink::PLUS ) );
	Link( L"G2", FormLink( L"24", CLink::MINUS ), FormLink( L"G2~H2" ) );
	Link( L"H2", FormLink( L"G2~H2" ), FormLink( L"22", CLink::MINUS ) );
	Link( L"D2", FormLink( L"D2~M2" ), FormLink( L"22", CLink::PLUS ) );
	Link( L"E2", FormLink( L"22", CLink::BASE ), FormLink( L"23", CLink::BASE ) );
	Link( L"F2", FormLink( L"F2~L2" ), FormLink( L"23", CLink::PLUS ) );
	Link( L"I2", FormLink( L"I2~R2" ), FormLink( L"I2~T2" ) );
	Link( L"K2", FormLink( L"23", CLink::MINUS ), FormLink( L"K2~N2" ) );
	Link( L"L2", FormLink( L"F2~L2" ) );
	Link( L"M2", FormLink( L"D2~M2" ), FormLink( L"M2~O2" ) );
	Link( L"O2", FormLink( L"M2~O2" ), FormLink( L"O2~P2" ) );
	Link( L"P2", FormLink( L"O2~P2" ), FormLink( L"21", CLink::BASE ) );
	Link( L"R2", FormLink( L"21", CLink::PLUS ), FormLink( L"I2~R2" ) );
	Link( L"S2", FormLink( L"21", CLink::MINUS ) );
	Link( L"N2", FormLink( L"K2~N2" ) );
	Link( L"T2", FormLink( L"I2~T2" ), FormLink( L"T2~U2" ) );
	Link( L"U2", FormLink( L"T2~U2" ) );
	Link( L"V2", FormLink( L"V2~W2" ) );
	Link( L"W2", FormLink( L"V2~W2" ), FormLink( L"W2~X2" ) );
	Link( L"X2", FormLink( L"W2~X2" ), FormLink( L"X2~Y2" ) );
	Link( L"Y2", FormLink( L"X2~Y2" ), FormLink( L"Y2~Z2" ) );
	Link( L"Z2", FormLink( L"Y2~Z2" ), FormLink( L"AA2~Z2" ) );
	Link( L"AA2", FormLink( L"AA2~Z2" ) );

	LinkSema( L"Leg200", L"M2", L"D2", vector <wstring>( 1, L"Head200" ) );
	vector <wstring> heads201, heads203, heads205;
	heads201.push_back( L"Head201S" );
	heads201.push_back( L"Head201T" );
	LinkSema( L"Leg201", L"L2", L"F2", heads201 );
	heads203.push_back( L"Head203S" );
	heads203.push_back( L"Head203T" );
	LinkSema( L"Leg203", L"M2", L"O2", heads203 );
	heads205.push_back( L"Head205S" );
	heads205.push_back( L"Head205T" );
	LinkSema( L"Leg205", L"T2", L"U2", heads205 );
	LinkSema( L"Leg202", L"X2", L"W2", vector<wstring>( 1, L"Head202" ) );
	LinkSema( L"Leg204", L"Y2", L"Z2", vector<wstring>( 1, L"Head204" ) );
}

void FictivePart::CreateThirdPart()
{
	const auto & statCode1 = EsrKit( 301 );
	const auto & statCode2 = EsrKit( 302 );
	const auto & statCode3 = EsrKit( 303 );
	const auto & statCode4 = EsrKit( 304 );

	const auto spanCode1Out = EsrKit( 301, 99999 );
	const auto spanCode4Out = EsrKit( 304, 99998 );
	const auto & spanCode12 = EsrKit( 301, 302 );
	const auto & spanCode23 = EsrKit( 302, 303 );
	const auto & spanCode34 = EsrKit( 303, 304 );

	//участки
	BadgeE strips_arr[] = { 
		BadgeE( L"G3", spanCode1Out ),
		BadgeE( L"A3", statCode1 ), BadgeE( L"B3", statCode1 ), BadgeE( L"C3", statCode1 ), BadgeE( L"D3", statCode1 ), BadgeE( L"E3", statCode1 ), 
		BadgeE( L"F3", statCode1 ), 
		BadgeE( L"X3", spanCode12 ), BadgeE( L"Y3", spanCode12 ),
		BadgeE( L"H3", statCode2 ), BadgeE( L"I3", statCode2 ), BadgeE( L"J3", statCode2 ), BadgeE( L"K3", statCode2 ), BadgeE( L"L3", statCode2 ),
		BadgeE( L"M3", statCode2 ),
		BadgeE( L"N3", spanCode23 ),
		BadgeE( L"O3", statCode3 ), BadgeE( L"P3", statCode3 ), BadgeE( L"R3", statCode3 ), BadgeE( L"S3", statCode3 ), BadgeE( L"T3", statCode3 ), 
		BadgeE( L"U3", spanCode34 ), 
		BadgeE( L"Q3", statCode4 ), BadgeE( L"V3", statCode4 ), BadgeE( L"W3", statCode4 ), BadgeE( L"Z3", statCode4 ), 
		BadgeE( L"AA3", spanCode4Out )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"A3~G3", spanCode1Out ), BadgeE( L"D3~F3", statCode1 ), BadgeE( L"F3~X3", statCode1 ), BadgeE( L"X3~Y3", spanCode12 ),
		BadgeE( L"H3~Y3", spanCode12 ), BadgeE( L"H3~I3", statCode2 ), BadgeE( L"I3~J3", statCode2 ), BadgeE( L"J3~K3", statCode2 ), BadgeE( L"K3~L3", statCode2 ), 
		BadgeE( L"L3~M3", statCode2 ), BadgeE( L"M3~N3", spanCode23 ), BadgeE( L"N3~O3", spanCode23 ), BadgeE( L"O3~P3", statCode3 ), BadgeE( L"P3~R3", statCode3 ), 
		BadgeE( L"R3~S3", statCode3 ), BadgeE( L"S3~T3", statCode3 ), BadgeE( L"T3~U3", statCode3 ), BadgeE( L"Q3~U3", statCode4 ), BadgeE( L"Q3~V3", statCode4 ), 
		BadgeE( L"V3~W3", statCode4 ), BadgeE( L"W3~Z3", statCode4 ), BadgeE( L"AA3~Z3", statCode4 )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"31", statCode1 ), BadgeE( L"32", statCode1 ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE heads_arr[] = { BadgeE( L"Head312", statCode2 ), BadgeE( L"Head314", statCode2 ), BadgeE( L"Head300", statCode3 ), BadgeE( L"Head302", statCode3 ),
		BadgeE( L"Head301", statCode3 ), BadgeE( L"Head303", statCode3 ), BadgeE( L"Head305", statCode4 ), BadgeE( L"Head307", statCode4 ),
		BadgeE( L"Head304", statCode4 ), BadgeE( L"Head306", statCode3 ), BadgeE( L"Head308", statCode4 ), BadgeE( L"Head310", statCode2 )
	};
	Insert( HEAD, heads_arr, size_array( heads_arr ), true, TRAIN );
	BadgeE legs_arr[] = { BadgeE( L"Leg312", statCode2 ), BadgeE( L"Leg314", statCode2 ), BadgeE( L"Leg300", statCode3 ), BadgeE( L"Leg302", statCode3 ),
		BadgeE( L"Leg301", statCode3 ), BadgeE( L"Leg303", statCode3 ), BadgeE( L"Leg305", statCode4 ), BadgeE( L"Leg307", statCode4 ), BadgeE( L"Leg304", statCode4 ),
		BadgeE( L"Leg306", statCode3 ), BadgeE( L"Leg308", statCode4 ), BadgeE( L"Leg310", statCode2 )
	};
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//маршруты
	CLink rtlinks[] = { CLink( HEAD, BadgeE( L"Head314", statCode2 ) ), CLink( STRIP, BadgeE( L"M3", statCode2 ) ) };
	InsertRoute( BadgeE( L"Head314->M3", statCode2 ), vector<CLink>( begin( rtlinks ), end( rtlinks ) ) );
	CLink rtlinks2[] = { CLink( HEAD, BadgeE( L"Head312", statCode2 ) ), CLink( STRIP, BadgeE( L"J3", statCode2 ) ), CLink( STRIP, BadgeE( L"L3", statCode2 ) ) };
	InsertRoute( BadgeE( L"Head312->L3", statCode2 ), vector<CLink>( begin( rtlinks2 ), end( rtlinks2 ) ) );
	CLink rtlinks3[] = { CLink( HEAD, BadgeE( L"Head300", statCode3 ) ), CLink( STRIP, BadgeE( L"R3", statCode3 ) ), CLink( STRIP, BadgeE( L"S3", statCode3 ) ) };
	InsertRoute( BadgeE( L"Head300->S3", statCode3 ), vector<CLink>( begin( rtlinks3 ), end( rtlinks3 ) ) );
	CLink rtlinks4[] = { CLink( HEAD, BadgeE( L"Head302", statCode3 ) ), CLink( STRIP, BadgeE( L"T3", statCode3 ) ) };
	InsertRoute( BadgeE( L"Head302->T3", statCode3 ), vector<CLink>( begin( rtlinks4 ), end( rtlinks4 ) ) );
	CLink rtlinks5[] = { CLink( HEAD, BadgeE( L"Head301", statCode3 ) ), CLink( STRIP, BadgeE( L"O3", statCode3 ) ) };
	InsertRoute( BadgeE( L"Head301->O3", statCode3 ), vector<CLink>( begin( rtlinks5 ), end( rtlinks5 ) ) );
	CLink rtlinks6[] = { CLink( HEAD, BadgeE( L"Head303", statCode3 ) ), CLink( STRIP, BadgeE( L"R3", statCode3 ) ), CLink( STRIP, BadgeE( L"P3", statCode3 ) ) };
	InsertRoute( BadgeE( L"Head303->P3", statCode3 ), vector<CLink>( begin( rtlinks6 ), end( rtlinks6 ) ) );
	CLink rtlinks7[] = { CLink( HEAD, BadgeE( L"Head305", statCode4 ) ), CLink( STRIP, BadgeE( L"Q3", statCode4 ) ) };
	InsertRoute( BadgeE( L"Head305->Q3", statCode4 ), vector<CLink>( begin( rtlinks7 ), end( rtlinks7 ) ) );
	CLink rtlinks8[] = { CLink( HEAD, BadgeE( L"Head307", statCode4 ) ), CLink( STRIP, BadgeE( L"V3", statCode4 ) ) };
	InsertRoute( BadgeE( L"Head307->V3", statCode4 ), vector<CLink>( begin( rtlinks8 ), end( rtlinks8 ) ) );
	CLink rtlinks9[] = { CLink( HEAD, BadgeE( L"Head304", statCode4 ) ), CLink( STRIP, BadgeE( L"Z3", statCode4 ) ) };
	InsertRoute( BadgeE( L"Head304->Z3", statCode4 ), vector<CLink>( begin( rtlinks9 ), end( rtlinks9 ) ) );

	//связывание
	Link( L"G3", FormLink( L"A3~G3" ) );
	Link( L"A3", FormLink( L"31", CLink::PLUS ), FormLink( L"A3~G3" ) );
	Link( L"B3", FormLink( L"31", CLink::MINUS ) );
	Link( L"C3", FormLink( L"31", CLink::BASE ), FormLink( L"32", CLink::BASE ) );
	Link( L"D3", FormLink( L"32", CLink::PLUS ), FormLink( L"D3~F3" ) );
	Link( L"E3", FormLink( L"32", CLink::MINUS ) );
	Link( L"F3", FormLink( L"D3~F3" ), FormLink( L"F3~X3" ) );
	Link( L"X3", FormLink( L"F3~X3" ), FormLink( L"X3~Y3" ) );
	Link( L"Y3", FormLink( L"X3~Y3" ), FormLink( L"H3~Y3" ) );
	Link( L"H3", FormLink( L"H3~I3" ), FormLink( L"H3~Y3" ) );
	Link( L"I3", FormLink( L"H3~I3" ), FormLink( L"I3~J3" ) );
	Link( L"J3", FormLink( L"J3~K3" ), FormLink( L"I3~J3" ) );
	Link( L"K3", FormLink( L"J3~K3" ), FormLink( L"K3~L3" ) );
	Link( L"L3", FormLink( L"L3~M3" ), FormLink( L"K3~L3" ) );
	Link( L"M3", FormLink( L"L3~M3" ), FormLink( L"M3~N3" ) );
	Link( L"N3", FormLink( L"M3~N3" ), FormLink( L"N3~O3" ) );
	Link( L"O3", FormLink( L"N3~O3" ), FormLink( L"O3~P3" ) );
	Link( L"P3", FormLink( L"O3~P3" ), FormLink( L"P3~R3" ) );
	Link( L"R3", FormLink( L"P3~R3" ), FormLink( L"R3~S3" ) );
	Link( L"S3", FormLink( L"R3~S3" ), FormLink( L"S3~T3" ) );
	Link( L"T3", FormLink( L"S3~T3" ), FormLink( L"T3~U3" ) );
	Link( L"U3", FormLink( L"T3~U3" ), FormLink( L"Q3~U3" ) );
	Link( L"Q3", FormLink( L"Q3~V3" ), FormLink( L"Q3~U3" ) );
	Link( L"V3", FormLink( L"Q3~V3" ), FormLink( L"V3~W3" ) );
	Link( L"W3", FormLink( L"V3~W3" ), FormLink( L"W3~Z3" ) );
	Link( L"Z3", FormLink( L"W3~Z3" ), FormLink( L"AA3~Z3" ) );
	Link( L"AA3", FormLink( L"AA3~Z3" ) );

	LinkSema( L"Leg312", L"I3", L"J3", vector <wstring>( 1, L"Head312" ) );
	LinkSema( L"Leg314", L"L3", L"M3", vector <wstring>( 1, L"Head314" ) );
	LinkSema( L"Leg300", L"P3", L"R3", vector <wstring>( 1, L"Head300" ) );
	LinkSema( L"Leg302", L"S3", L"T3", vector <wstring>( 1, L"Head302" ) );
	LinkSema( L"Leg301", L"P3", L"O3", vector <wstring>( 1, L"Head301" ) );
	LinkSema( L"Leg303", L"S3", L"R3", vector <wstring>( 1, L"Head303" ) );
	LinkSema( L"Leg305", L"V3", L"Q3", vector <wstring>( 1, L"Head305" ) );
	LinkSema( L"Leg307", L"W3", L"V3", vector <wstring>( 1, L"Head307" ) );
	LinkSema( L"Leg304", L"W3", L"Z3", vector <wstring>( 1, L"Head304" ) );
	LinkSema( L"Leg306", L"N3", L"O3", vector <wstring>( 1, L"Head306" ) );
	LinkSema( L"Leg308", L"U3", L"Q3", vector <wstring>( 1, L"Head308" ) );
	LinkSema( L"Leg310", L"Y3", L"H3", vector <wstring>( 1, L"Head310" ) );
}

void FictivePart::CreateFourthPart()
{
	const auto & statCode1 = EsrKit( 401 );
	const auto & statCode2 = EsrKit( 402 );
	const auto & statCode3 = EsrKit( 403 );
	const auto & statCode4 = EsrKit( 404 );
	const auto & statCode5 = EsrKit( 405 );
	const auto & statCode6 = EsrKit( 406 );
	const auto & statCode7 = EsrKit( 407 );
	const auto & statCode8 = EsrKit( 408 );

	const auto & spanCode1Out1 = EsrKit( 401, 99999 );
	const auto & spanCode1Out2 = EsrKit( 401, 99998 );
	const auto & spanCode2Out = EsrKit( 402, 99999 );
	const auto & spanCode23 = EsrKit( 402, 403 );
	const auto & spanCode45 = EsrKit( 404, 405 );
	const auto & spanCode56 = EsrKit( 405, 406 );
	const auto & spanCode78 = EsrKit( 407, 408 );
	const auto & spanCodeFree = EsrKit( 99998, 99999 );

	//участки
	BadgeE strips_arr[] = { 
		BadgeE( L"Q4", spanCode1Out1 ),
		BadgeE( L"A4", statCode1 ), BadgeE( L"B4", statCode1 ), BadgeE( L"C4", statCode1 ), BadgeE( L"D4", statCode1 ), BadgeE( L"E4", statCode1 ), 
		BadgeE( L"F4", statCode1 ), BadgeE( L"G4", statCode1 ), BadgeE( L"H4", statCode1 ), BadgeE( L"K4", statCode1 ), BadgeE( L"L4", statCode1 ), 
		BadgeE( L"M4", statCode1 ), BadgeE( L"N4", statCode1 ), BadgeE( L"O4", statCode1 ), BadgeE( L"P4", statCode1 ), BadgeE( L"R4", statCode1 ), 
		BadgeE( L"S4", statCode1 ), BadgeE( L"U4", statCode1 ), BadgeE( L"V4", statCode1 ), BadgeE( L"W4", statCode1 ), BadgeE( L"Y4", statCode1 ), 
		BadgeE( L"Z4", statCode1 ), BadgeE( L"AI4", statCode1 ),
		BadgeE( L"I4", spanCode1Out2 ), BadgeE( L"AJ4", spanCode1Out2 ), BadgeE( L"AK4", spanCode1Out2 ),
		BadgeE( L"AA4", spanCode2Out ), BadgeE( L"AG4", spanCode2Out ), BadgeE( L"AH4", spanCode2Out ),
		BadgeE( L"AB4", statCode2 ), BadgeE( L"AC4", statCode2 ), BadgeE( L"AD4", statCode2 ), 
		BadgeE( L"AE4", spanCode23 ),
		BadgeE( L"AF4", statCode3 ),
		BadgeE( L"ST1", statCode4 ),
		BadgeE( L"ST2", statCode5 ),
		BadgeE( L"ST3", statCode6 ),
		BadgeE( L"ST4", statCode7 ),
		BadgeE( L"ST5", statCode8 ),
		BadgeE( L"Unlinked", spanCodeFree )
	};
	BadgeE nots_strips_arr[] = { BadgeE( L"T4", statCode1 ), BadgeE( L"X4", statCode1 ) };
	Insert( STRIP, strips_arr, size_array( strips_arr ) );
	Insert( STRIP, nots_strips_arr, size_array( nots_strips_arr ), false );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"A4~Q4", statCode1 ), BadgeE( L"C4~D4", statCode1 ), BadgeE( L"D4~E4", statCode1 ), BadgeE( L"O4~R4", statCode1 ), 
		BadgeE( L"R4~S4", statCode1 ), BadgeE( L"F4~S4", statCode1 ), BadgeE( L"P4~T4", statCode1 ), BadgeE( L"U4~V4", statCode1 ), BadgeE( L"V4~W4", statCode1 ),
		BadgeE( L"W4~X4", statCode1 ), BadgeE( L"X4~Y4", statCode1 ), BadgeE( L"Y4~Z4", statCode1 ), BadgeE( L"I4~K4", spanCode1Out2 ), BadgeE( L"AA4~AB4", statCode2 ), 
		BadgeE( L"AB4~AC4", statCode2 ), BadgeE( L"AC4~AD4", statCode2 ), BadgeE( L"AD4~AE4", statCode2 ), BadgeE( L"AE4~AF4", spanCode23 ), 
		BadgeE( L"AA4~AG4", spanCode2Out ), BadgeE( L"AG4~AH4", spanCode2Out ), BadgeE( L"AI4~N4", statCode1 ), BadgeE( L"AJ4~I4", spanCode1Out2 ), 
		BadgeE( L"AK4~M4", spanCode1Out2 ),
		BadgeE( L"ST1~ST2", spanCode45 ), BadgeE( L"ST2~ST3", spanCode56 ), BadgeE( L"ST4~ST5", spanCode78 )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"41", statCode1 ), BadgeE( L"42", statCode1 ), BadgeE( L"43", statCode1 ), BadgeE( L"44", statCode1 ), 
		BadgeE( L"45", statCode1 ), BadgeE( L"46", statCode1 )
	};
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"Head400", statCode1 ), BadgeE( L"Head401", statCode1 ), BadgeE( L"Head403T", statCode1 ), BadgeE( L"Head404", statCode2 ), 
		BadgeE( L"Head405", statCode2 ), BadgeE( L"Head406", statCode3 ), BadgeE( L"Head407", statCode1 ), BadgeE( L"Head408", spanCode2Out ), 
		BadgeE( L"Head410", spanCode2Out )
	};
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE shuheads_arr[] = { BadgeE( L"Head402", statCode1 ), BadgeE( L"Head403S", statCode1 ) };
	Insert( HEAD, shuheads_arr, size_array( shuheads_arr ), true, SHUNTING );
	BadgeE legs_arr[] = { BadgeE( L"Leg400", statCode1 ), BadgeE( L"Leg401", statCode1 ), BadgeE( L"Leg402", statCode1 ), BadgeE( L"Leg403", statCode1 ),
		BadgeE( L"Leg404", statCode2 ), BadgeE( L"Leg405", statCode2 ), BadgeE( L"Leg406", statCode3 ), BadgeE( L"Leg407", statCode1 ), 
		BadgeE( L"Leg408", spanCode2Out ), BadgeE( L"Leg410", spanCode2Out )
	};
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//маршруты
	CLink rtlinks[] = { CLink( HEAD, BadgeE( L"Head400", statCode1 ) ), CLink( STRIP, BadgeE( L"F4", statCode1 ) ), CLink( STRIP, BadgeE( L"K4", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head400->K4", statCode1 ), vector <CLink>( begin( rtlinks ), end( rtlinks ) ) );
	CLink rtlinks2[] = { CLink( HEAD, BadgeE( L"Head400", statCode1 ) ), CLink( STRIP, BadgeE( L"F4", statCode1 ) ), CLink( STRIP, BadgeE( L"M4", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head400->M4", statCode1 ), vector <CLink>( begin( rtlinks2 ), end( rtlinks2 ) ) );
	CLink rtlinks3[] = { CLink( HEAD, BadgeE( L"Head403S", statCode1 ) ), CLink( STRIP, BadgeE( L"O4", statCode1 ) ), CLink( STRIP, BadgeE( L"A4", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head403S->A4", statCode1 ), vector <CLink>( begin( rtlinks3 ), end( rtlinks3 ) ) );

	//четвертая часть
	Link( L"Q4", FormLink( L"A4~Q4" ) );
	Link( L"A4", FormLink( L"41", CLink::BASE ), FormLink( L"A4~Q4" ) );
	Link( L"O4", FormLink( L"41", CLink::MINUS ), FormLink( L"O4~R4" ) );
	Link( L"B4", FormLink( L"41", CLink::PLUS ), FormLink( L"42", CLink::BASE ) );
	Link( L"P4", FormLink( L"42", CLink::MINUS ), FormLink( L"P4~T4" ) );
	Link( L"C4", FormLink( L"42", CLink::PLUS ), FormLink( L"C4~D4" ) );
	Link( L"D4", FormLink( L"C4~D4" ), FormLink( L"D4~E4" ) );
	Link( L"E4", FormLink( L"D4~E4" ), FormLink( L"43", CLink::PLUS ) );
	Link( L"F4", FormLink( L"43", CLink::MINUS ), FormLink( L"F4~S4" ) );
	Link( L"G4", FormLink( L"43", CLink::BASE ), FormLink( L"44", CLink::MINUS ) );
	Link( L"H4", FormLink( L"44", CLink::BASE ), FormLink( L"45", CLink::BASE ) );
	Link( L"I4", FormLink( L"I4~K4" ), FormLink( L"AJ4~I4" ) );
	Link( L"K4", FormLink( L"45", CLink::PLUS ), FormLink( L"I4~K4" ) );
	Link( L"L4", FormLink( L"45", CLink::MINUS ), FormLink( L"46", CLink::BASE ) );
	Link( L"M4", FormLink( L"46", CLink::PLUS ), FormLink( L"AK4~M4" ) );
	Link( L"N4", FormLink( L"46", CLink::MINUS ), FormLink( L"AI4~N4" ) );
	Link( L"R4", FormLink( L"O4~R4" ), FormLink( L"R4~S4" ) );
	Link( L"S4", FormLink( L"R4~S4" ), FormLink( L"F4~S4" ) );
	Link( L"T4", FormLink( L"P4~T4" ) );
	Link( L"U4", FormLink( L"44", CLink::PLUS ), FormLink( L"U4~V4" ) );
	Link( L"V4", FormLink( L"U4~V4" ), FormLink( L"V4~W4" ) );
	Link( L"W4", FormLink( L"V4~W4" ), FormLink( L"W4~X4" ) );
	Link( L"X4", FormLink( L"W4~X4" ), FormLink( L"X4~Y4" ) );
	Link( L"Y4", FormLink( L"X4~Y4" ), FormLink( L"Y4~Z4" ) );
	Link( L"Z4", FormLink( L"Y4~Z4" ) );
	Link( L"AI4", FormLink( L"AI4~N4" ) );
	Link( L"AJ4", FormLink( L"AJ4~I4" ) );
	Link( L"AK4", FormLink( L"AK4~M4" ) );

	Link( L"AA4", FormLink( L"AA4~AB4" ), FormLink( L"AA4~AG4" ) );
	Link( L"AB4", FormLink( L"AA4~AB4" ), FormLink( L"AB4~AC4" ) );
	Link( L"AC4", FormLink( L"AB4~AC4" ), FormLink( L"AC4~AD4" ) );
	Link( L"AD4", FormLink( L"AC4~AD4" ), FormLink( L"AD4~AE4" ) );
	Link( L"AE4", FormLink( L"AD4~AE4" ), FormLink( L"AE4~AF4" ) );
	Link( L"AF4", FormLink( L"AE4~AF4" ) );
	Link( L"AG4", FormLink( L"AA4~AG4" ), FormLink( L"AG4~AH4" ) );
	Link( L"AH4", FormLink( L"AG4~AH4" ) );

	Link( L"ST1", FormLink( L"ST1~ST2" ) );
	Link( L"ST2", FormLink( L"ST1~ST2" ), FormLink( L"ST2~ST3" ) );
	Link( L"ST3", FormLink( L"ST2~ST3" ) );
	Link( L"ST4", FormLink( L"ST4~ST5" ) );
	Link( L"ST5", FormLink( L"ST4~ST5" ) );

	LinkSema( L"Leg400", L"S4", L"F4", vector <wstring>( 1, L"Head400" ) );
	LinkSema( L"Leg401", L"V4", L"W4", vector <wstring>( 1, L"Head401" ) );
	LinkSema( L"Leg402", L"C4", L"D4", vector <wstring>( 1, L"Head402" ) );

	vector <wstring> heads403;
	heads403.push_back( L"Head403S" );
	heads403.push_back( L"Head403T" );
	LinkSema( L"Leg403", L"R4", L"O4", heads403 );

	LinkSema( L"Leg404", L"AA4", L"AB4", vector <wstring>( 1, L"Head404" ) );
	LinkSema( L"Leg405", L"AE4", L"AD4", vector <wstring>( 1, L"Head405" ) );
	LinkSema( L"Leg406", L"AE4", L"AF4", vector <wstring>( 1, L"Head406" ) );
	LinkSema( L"Leg407", L"Q4", L"A4", vector <wstring>( 1, L"Head407" ) );
	LinkSema( L"Leg408", L"AG4", L"AA4", vector <wstring>( 1, L"Head408" ) );
	LinkSema( L"Leg410", L"AH4", L"AG4", vector <wstring>( 1, L"Head410" ) );
}

void FictivePart::CreateFifthPart()
{
	const auto & statCode1 = EsrKit( 501 );
	const auto & statCode2 = EsrKit( 502 );
	const auto & statCode3 = EsrKit( 503 );
	const auto & statCode4 = EsrKit( 504 );
	const auto & statCode5 = EsrKit( 505 );
	const auto & statCode6 = EsrKit( 506 );
	const auto & statCode7 = EsrKit( 507 );
	const auto & statCode8 = EsrKit( 508 );
	const auto & statCode9 = EsrKit( 509 );
	const auto & spanCode2Out = EsrKit( 502, 99999 );
	const auto & spanCode36 = EsrKit( 503, 506 );
	const auto & spanCode78 = EsrKit( 507, 508 );
	const auto & spanCode9Out = EsrKit( 509, 99999 );

	//участки
	BadgeE strips_arr[] = { BadgeE( L"A5", statCode1 ), BadgeE( L"B5", statCode1 ), BadgeE( L"C5", statCode1 ), BadgeE( L"D5", statCode1 ), BadgeE( L"E5", statCode1 ),
		BadgeE( L"F5", statCode1 ), BadgeE( L"G5", statCode1 ), BadgeE( L"H5", statCode1 ), BadgeE( L"I5", statCode1 ), BadgeE( L"J5", statCode1 ),
		BadgeE( L"K5", statCode1 ), BadgeE( L"V5", statCode1 ), BadgeE( L"W5", statCode1 ), BadgeE( L"X5", statCode1 ),
		BadgeE( L"L5", spanCode2Out ), BadgeE( L"M5", spanCode2Out ), BadgeE( L"N5", spanCode2Out ), BadgeE( L"O5", spanCode2Out ), 
		BadgeE( L"P5", spanCode2Out ), BadgeE( L"Q5", spanCode2Out ), BadgeE( L"S5", spanCode2Out ), BadgeE( L"T5", spanCode2Out ),
		BadgeE( L"U5", spanCode2Out ),
		BadgeE( L"R5", statCode2 ), BadgeE( L"AO5", statCode2 ), BadgeE( L"BI5", statCode2 ), BadgeE( L"BJ5", statCode2 ), BadgeE( L"BK5", statCode2 ),
		BadgeE( L"Y5", statCode3 ), BadgeE( L"Z5", statCode3 ), BadgeE( L"AA5", statCode3 ), BadgeE( L"AB5", statCode3 ), BadgeE( L"AC5", statCode3 ),
		BadgeE( L"AD5", statCode3 ), BadgeE( L"AE5", statCode3 ), BadgeE( L"AF5", statCode3 ), BadgeE( L"AG5", statCode3 ), BadgeE( L"AH5", statCode3 ),
		BadgeE( L"AI5", statCode3 ), BadgeE( L"AJ5", statCode3 ), BadgeE( L"AK5", statCode3 ), BadgeE( L"AL5", statCode3 ), BadgeE( L"AM5", statCode3 ),
		BadgeE( L"AN5", statCode3 ),
		BadgeE( L"AP5", statCode4 ), BadgeE( L"AQ5", statCode4 ), BadgeE( L"BL5", statCode4 ), BadgeE( L"BM5", statCode4 ), BadgeE( L"BN5", statCode4 ),
		BadgeE( L"AR5", statCode5 ), BadgeE( L"AS5", statCode5 ), BadgeE( L"BB5", statCode5 ), BadgeE( L"BC5", statCode5 ), BadgeE( L"BD5", statCode5 ),
		BadgeE( L"AV5", statCode6 ), BadgeE( L"AW5", statCode6 ), BadgeE( L"AX5", statCode6 ),
		BadgeE( L"AY5", statCode7 ), BadgeE( L"AZ5", statCode7 ),
		BadgeE( L"AT5", spanCode36 ), BadgeE( L"AU5", spanCode36 ),
		BadgeE( L"BA5", spanCode78 ), BadgeE( L"BE5", spanCode78 ),
		BadgeE( L"BF5", statCode8 ),
		BadgeE( L"BG5", statCode9 ),
		BadgeE( L"BH5", spanCode9Out ),
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"D5~F5", statCode1 ), BadgeE( L"F5~G5", statCode1 ), BadgeE( L"L5~M5", spanCode2Out ), BadgeE( L"M5~N5", spanCode2Out ),
		BadgeE( L"N5~O5", spanCode2Out ), BadgeE( L"O5~P5", spanCode2Out ), BadgeE( L"P5~Q5", spanCode2Out ), BadgeE( L"Q5~R5", spanCode2Out ), 
		BadgeE( L"S5~T5", spanCode2Out ), BadgeE( L"T5~U5", spanCode2Out ), BadgeE( L"L5~U5", spanCode2Out ), BadgeE( L"A5~V5", statCode1 ), 
		BadgeE( L"V5~W5", statCode1 ), BadgeE( L"W5~X5", statCode1 ), BadgeE( L"Y5~Z5", statCode3 ), BadgeE( L"AB5~AC5", statCode3 ), BadgeE( L"AC5~AD5", statCode3 ), 
		BadgeE( L"AF5~AG5", statCode3 ), BadgeE( L"AI5~AJ5", statCode3 ), BadgeE( L"AK5~AM5", statCode3 ), BadgeE( L"AM5~AN5", statCode3 ), 
		BadgeE( L"AO5~R5", statCode2 ), BadgeE( L"AO5~AP5", statCode2 ), BadgeE( L"BI5~BJ5", statCode2 ), BadgeE( L"BJ5~BK5", statCode2 ),
		BadgeE( L"BK5~BL5", statCode2 ), BadgeE( L"AP5~AQ5", statCode4 ), BadgeE( L"AQ5~AR5", statCode4 ), BadgeE( L"BL5~BM5", statCode4 ),
		BadgeE( L"BM5~BN5", statCode4 ), BadgeE( L"AR5~AS5", statCode5 ), BadgeE( L"AJ5~AT5", statCode3 ), BadgeE( L"AT5~AU5", spanCode36 ), 
		BadgeE( L"AU5~AV5", spanCode36 ), BadgeE( L"AV5~AW5", statCode6 ), BadgeE( L"AW5~AX5", statCode6 ), BadgeE( L"AS5~BB5", statCode5 ), 
		BadgeE( L"BB5~BC5", statCode5 ), BadgeE( L"BC5~BD5", statCode5 ), BadgeE( L"AY5~BD5", statCode5 ), BadgeE( L"AY5~AZ5", statCode7 ), 
		BadgeE( L"AZ5~BA5", statCode7 ), BadgeE( L"BA5~BE5", spanCode78 ), BadgeE( L"BE5~BF5", spanCode78 ), BadgeE( L"BG5~X5", statCode1 ), 
		BadgeE( L"BG5~BH5", statCode9 )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"51", statCode1 ), BadgeE( L"52", statCode1 ), BadgeE( L"53", statCode1 ), BadgeE( L"54", statCode1 ), 
		BadgeE( L"55", statCode3 ), BadgeE( L"56", statCode3 ), BadgeE( L"57", statCode3 ), BadgeE( L"58", statCode3 ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE heads_arr[] = { BadgeE( L"Head500", statCode1 ), BadgeE( L"Head501", statCode1 ), BadgeE( L"Head502", spanCode2Out ), BadgeE( L"Head503", spanCode2Out ),
		BadgeE( L"Head504", spanCode2Out ), BadgeE( L"Head505", spanCode2Out ), BadgeE( L"Head506", spanCode2Out ), BadgeE( L"Head507", spanCode2Out ), 
		BadgeE( L"Head509", spanCode2Out ), BadgeE( L"Head510", statCode3 ), BadgeE( L"Head511", spanCode2Out ), BadgeE( L"Head512", statCode2 ),
		BadgeE( L"Head513", statCode3 ), BadgeE( L"Head514", statCode4 ), BadgeE( L"Head515", spanCode36 ), BadgeE( L"Head516", statCode6 ),
		BadgeE( L"Head517", statCode6 ), BadgeE( L"Head518", statCode5 ), BadgeE( L"Head519", statCode7 ), BadgeE( L"Head520", statCode5 ),
		BadgeE( L"Head521", statCode5 ), BadgeE( L"Head522", statCode2 ), BadgeE( L"Head524", statCode4 )
	};
	Insert( HEAD, heads_arr, size_array( heads_arr ), true, TRAIN );
	BadgeE shuheads_arr[] = { BadgeE( L"Head508S", statCode1 ) };
	Insert( HEAD, shuheads_arr, size_array( shuheads_arr ), true, SHUNTING );
	BadgeE legs_arr[] = { BadgeE( L"Leg500", statCode1 ), BadgeE( L"Leg501", statCode1 ), BadgeE( L"Leg502", spanCode2Out ), BadgeE( L"Leg503", spanCode2Out ),
		BadgeE( L"Leg504", spanCode2Out ), BadgeE( L"Leg505", spanCode2Out ), BadgeE( L"Leg506", spanCode2Out ), BadgeE( L"Leg507", spanCode2Out ), 
		BadgeE( L"Leg508", statCode1 ), BadgeE( L"Leg509", spanCode2Out ), BadgeE( L"Leg510", statCode3 ), BadgeE( L"Leg511", spanCode2Out ),
		BadgeE( L"Leg512", statCode2 ), BadgeE( L"Leg513", statCode3 ), BadgeE( L"Leg514", statCode4 ), BadgeE( L"Leg515", spanCode36 ),
		BadgeE( L"Leg516", statCode6 ), BadgeE( L"Leg517", statCode6 ), BadgeE( L"Leg518", statCode5 ), BadgeE( L"Leg519", statCode5 ), BadgeE( L"Leg520", statCode5 ),
		BadgeE( L"Leg521", statCode5 ), BadgeE( L"Leg522", statCode2 ), BadgeE( L"Leg524", statCode4 )
	};
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//маршруты
	CLink rtlinks[] = { CLink( HEAD, BadgeE( L"Head510", statCode3 ) ), CLink( STRIP, BadgeE( L"AG5", statCode3 ) ), CLink( STRIP, BadgeE( L"AJ5", statCode3 ) ) };
	InsertRoute( BadgeE( L"Head510->AJ5", statCode3 ), vector<CLink>( begin( rtlinks ), end( rtlinks ) ) );
	CLink rtlinks2[] = { CLink( HEAD, BadgeE( L"Head512", statCode2 ) ), CLink( STRIP, BadgeE( L"R5", statCode2 ) ), CLink( STRIP, BadgeE( L"AO5", statCode2 ) ) };
	InsertRoute( BadgeE( L"Head512->AO5", statCode2 ), vector<CLink>( begin( rtlinks2 ), end( rtlinks2 ) ) );
	CLink rtlinks3[] = { CLink( HEAD, BadgeE( L"Head514", statCode4 ) ), CLink( STRIP, BadgeE( L"AP5", statCode4 ) ), CLink( STRIP, BadgeE( L"AQ5", statCode4 ) ) };
	InsertRoute( BadgeE( L"Head514->AQ5", statCode4 ), vector<CLink>( begin( rtlinks3 ), end( rtlinks3 ) ) );
	CLink rtlinks4[] = { CLink( HEAD, BadgeE( L"Head520", statCode5 ) ), CLink( STRIP, BadgeE( L"BD5", statCode5 ) ) };
	InsertRoute( BadgeE( L"Head520->BD5", statCode5 ), vector<CLink>( begin( rtlinks4 ), end( rtlinks4 ) ) );
	CLink rtlinks5[] = { CLink( HEAD, BadgeE( L"Head521", statCode5 ) ), CLink( STRIP, BadgeE( L"BB5", statCode5 ) ), CLink( STRIP, BadgeE( L"AR5", statCode5 ) ) };
	InsertRoute( BadgeE( L"Head521->AR5", statCode5 ), vector<CLink>( begin( rtlinks5 ), end( rtlinks5 ) ) );
	CLink rtlinks6[] = { CLink( HEAD, BadgeE( L"Head522", statCode2 ) ), CLink( STRIP, BadgeE( L"BK5", statCode2 ) ) };
	InsertRoute( BadgeE( L"Head522->BK5", statCode2 ), vector<CLink>( begin( rtlinks6 ), end( rtlinks6 ) ) );
	CLink rtlinks7[] = { CLink( HEAD, BadgeE( L"Head524", statCode4 ) ), CLink( STRIP, BadgeE( L"BN5", statCode4 ) ) };
	InsertRoute( BadgeE( L"Head524->BN5", statCode4 ), vector<CLink>( begin( rtlinks7 ), end( rtlinks7 ) ) );

	//связывание
	Link( L"A5", FormLink( L"51", CLink::BASE ), FormLink( L"A5~V5" ) );
	Link( L"B5", FormLink( L"51", CLink::PLUS ), FormLink( L"52", CLink::BASE ) );
	Link( L"C5", FormLink( L"51", CLink::MINUS ) );
	Link( L"D5", FormLink( L"52", CLink::MINUS ), FormLink( L"D5~F5" ) );
	Link( L"E5", FormLink( L"52", CLink::PLUS ) );
	Link( L"F5", FormLink( L"D5~F5" ), FormLink( L"F5~G5" ) );
	Link( L"G5", FormLink( L"F5~G5" ), FormLink( L"53", CLink::MINUS ) );
	Link( L"H5", FormLink( L"53", CLink::BASE ), FormLink( L"54", CLink::BASE ) );
	Link( L"I5", FormLink( L"53", CLink::PLUS ) );
	Link( L"J5", FormLink( L"54", CLink::MINUS ) );
	Link( L"K5", FormLink( L"54", CLink::PLUS ) );
	Link( L"L5", FormLink( L"L5~M5" ), FormLink( L"L5~U5" ) );
	Link( L"M5", FormLink( L"L5~M5" ), FormLink( L"M5~N5" ) );
	Link( L"N5", FormLink( L"M5~N5" ), FormLink( L"N5~O5" ) );
	Link( L"O5", FormLink( L"N5~O5" ), FormLink( L"O5~P5" ) );
	Link( L"P5", FormLink( L"O5~P5" ), FormLink( L"P5~Q5" ) );
	Link( L"Q5", FormLink( L"P5~Q5" ), FormLink( L"Q5~R5" ) );
	Link( L"R5", FormLink( L"Q5~R5" ), FormLink( L"AO5~R5" ) );
	Link( L"S5", FormLink( L"S5~T5" ) );
	Link( L"T5", FormLink( L"S5~T5" ), FormLink( L"T5~U5" ) );
	Link( L"U5", FormLink( L"T5~U5" ), FormLink( L"L5~U5" ) );
	Link( L"V5", FormLink( L"A5~V5" ), FormLink( L"V5~W5" ) );
	Link( L"W5", FormLink( L"V5~W5" ), FormLink( L"W5~X5" ) );
	Link( L"X5", FormLink( L"W5~X5" ), FormLink( L"BG5~X5" ) );
	Link( L"Y5", FormLink( L"Y5~Z5" ), FormLink( L"58", CLink::BASE ) );
	Link( L"Z5", FormLink( L"Y5~Z5" ), FormLink( L"55", CLink::MINUS ) );
	Link( L"AA5", FormLink( L"55", CLink::PLUS ) );
	Link( L"AB5", FormLink( L"AB5~AC5" ), FormLink( L"55", CLink::BASE ) );
	Link( L"AC5", FormLink( L"AB5~AC5" ), FormLink( L"AC5~AD5" ) );
	Link( L"AD5", FormLink( L"AC5~AD5" ), FormLink( L"56", CLink::MINUS ) );
	Link( L"AE5", FormLink( L"56", CLink::PLUS ) );
	Link( L"AF5", FormLink( L"AF5~AG5" ), FormLink( L"56", CLink::BASE ) );
	Link( L"AG5", FormLink( L"AF5~AG5" ), FormLink( L"57", CLink::MINUS ) );
	Link( L"AH5", FormLink( L"57", CLink::PLUS ) );
	Link( L"AI5", FormLink( L"57", CLink::BASE ), FormLink( L"AI5~AJ5" ) );
	Link( L"AJ5", FormLink( L"AI5~AJ5" ), FormLink( L"AJ5~AT5" ) );
	Link( L"AT5", FormLink( L"AJ5~AT5" ), FormLink( L"AT5~AU5" ) );
	Link( L"AU5", FormLink( L"AT5~AU5" ), FormLink( L"AU5~AV5" ) );
	Link( L"AV5", FormLink( L"AU5~AV5" ), FormLink( L"AV5~AW5" ) );
	Link( L"AW5", FormLink( L"AV5~AW5" ), FormLink( L"AW5~AX5" ) );
	Link( L"AX5", FormLink( L"AW5~AX5" ) );
	Link( L"AK5", FormLink( L"AK5~AM5" ), FormLink( L"58", CLink::MINUS ) );
	Link( L"AL5", FormLink( L"58", CLink::PLUS ) );
	Link( L"AM5", FormLink( L"AK5~AM5" ), FormLink( L"AM5~AN5" ) );
	Link( L"AN5", FormLink( L"AM5~AN5" ) );
	Link( L"AO5", FormLink( L"AO5~R5" ), FormLink( L"AO5~AP5" ) );
	Link( L"AP5", FormLink( L"AO5~AP5" ), FormLink( L"AP5~AQ5" ) );
	Link( L"AQ5", FormLink( L"AP5~AQ5" ), FormLink( L"AQ5~AR5" ) );
	Link( L"BI5", FormLink( L"BI5~BJ5" ) );
	Link( L"BJ5", FormLink( L"BI5~BJ5" ), FormLink( L"BJ5~BK5" ) );
	Link( L"BK5", FormLink( L"BK5~BL5" ), FormLink( L"BJ5~BK5" ) );
	Link( L"BL5", FormLink( L"BK5~BL5" ), FormLink( L"BL5~BM5" ) );
	Link( L"BM5", FormLink( L"BM5~BN5" ), FormLink( L"BL5~BM5" ) );
	Link( L"BN5", FormLink( L"BM5~BN5" ) );
	Link( L"AR5", FormLink( L"AQ5~AR5" ), FormLink( L"AR5~AS5" ) );
	Link( L"AS5", FormLink( L"AR5~AS5" ), FormLink( L"AS5~BB5" ) );
	Link( L"BB5", FormLink( L"AS5~BB5" ), FormLink( L"BB5~BC5" ) );
	Link( L"BC5", FormLink( L"BB5~BC5" ), FormLink( L"BC5~BD5" ) );
	Link( L"BD5", FormLink( L"BC5~BD5" ), FormLink( L"AY5~BD5" ) );
	Link( L"AY5", FormLink( L"AY5~BD5" ), FormLink( L"AY5~AZ5" ) );
	Link( L"AZ5", FormLink( L"AY5~AZ5" ), FormLink( L"AZ5~BA5" ) );
	Link( L"BA5", FormLink( L"AZ5~BA5" ), FormLink( L"BA5~BE5" ) );
	Link( L"BE5", FormLink( L"BA5~BE5" ), FormLink( L"BE5~BF5" ) );
	Link( L"BF5", FormLink( L"BE5~BF5" ) );
	Link( L"BG5", FormLink( L"BG5~X5" ), FormLink( L"BG5~BH5" ) );
	Link( L"BH5", FormLink( L"BG5~BH5" ) );

	//светофоры
	LinkSema( L"Leg500", L"F5", L"G5", vector <wstring>( 1, L"Head500" ) );
	LinkSema( L"Leg501", L"F5", L"D5", vector <wstring>( 1, L"Head501" ) );
	LinkSema( L"Leg502", L"L5", L"M5", vector <wstring>( 1, L"Head502" ) );
	LinkSema( L"Leg503", L"M5", L"L5", vector <wstring>( 1, L"Head503" ) );
	LinkSema( L"Leg504", L"M5", L"N5", vector <wstring>( 1, L"Head504" ) );
	LinkSema( L"Leg505", L"N5", L"M5", vector <wstring>( 1, L"Head505" ) );
	LinkSema( L"Leg506", L"O5", L"P5", vector <wstring>( 1, L"Head506" ) );
	LinkSema( L"Leg507", L"O5", L"N5", vector <wstring>( 1, L"Head507" ) );
	LinkSema( L"Leg508", L"V5", L"A5", vector <wstring>( 1, L"Head508S" ) );
	LinkSema( L"Leg509", L"Q5", L"P5", vector <wstring>( 1, L"Head509" ) );
	LinkSema( L"Leg510", L"AF5", L"AG5", vector <wstring>( 1, L"Head510" ) );
	LinkSema( L"Leg511", L"T5", L"S5", vector <wstring>( 1, L"Head511" ) );
	LinkSema( L"Leg512", L"Q5", L"R5", vector <wstring>( 1, L"Head512" ) );
	LinkSema( L"Leg513", L"Z5", L"Y5", vector <wstring>( 1, L"Head513" ) );
	LinkSema( L"Leg514", L"AO5", L"AR5", vector <wstring>( 1, L"Head514" ) );
	LinkSema( L"Leg515", L"AT5", L"AJ5", vector <wstring>( 1, L"Head515" ) );
	LinkSema( L"Leg516", L"AW5", L"AX5", vector <wstring>( 1, L"Head516" ) );
	LinkSema( L"Leg517", L"AW5", L"AV5", vector <wstring>( 1, L"Head517" ) );
	LinkSema( L"Leg518", L"AR5", L"AS5", vector <wstring>( 1, L"Head518" ) );
	LinkSema( L"Leg519", L"BA5", L"AZ5", vector <wstring>( 1, L"Head519" ) );
	LinkSema( L"Leg520", L"BC5", L"BD5", vector <wstring>( 1, L"Head520" ) );
	LinkSema( L"Leg521", L"BC5", L"BB5", vector <wstring>( 1, L"Head521" ) );
	LinkSema( L"Leg522", L"BJ5", L"BK5", vector <wstring>( 1, L"Head522" ) );
	LinkSema( L"Leg524", L"BM5", L"BN5", vector <wstring>( 1, L"Head524" ) );
}

void FictivePart::CreateTechnicalStationPart()
{
	const auto & statCode1 = EsrKit( 601 );
	const auto & spanCode6Out1 = EsrKit( 601, 99999 );
	const auto & spanCode6Out2 = EsrKit( 601, 99998 );

	//участки
	BadgeE strips_arr[] = { BadgeE ( L"A6", spanCode6Out1 ), BadgeE ( L"B6", statCode1 ), BadgeE( L"C6", statCode1 ), BadgeE( L"D6", statCode1 ), 
		BadgeE( L"E6", statCode1 ), BadgeE( L"F6", statCode1 ), BadgeE( L"G6", statCode1 ), BadgeE( L"H6", statCode1 ), BadgeE( L"I6", statCode1 ), 
		BadgeE( L"J6", spanCode6Out2 )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стыки
	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"A6~B6", statCode1 ), BadgeE( L"B6~C6", statCode1 ), BadgeE( L"D6~F6", statCode1 ), BadgeE( L"F6~G6", statCode1 ),
		BadgeE( L"H6~I6", statCode1 ), BadgeE( L"I6~J6", statCode1 )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"61", statCode1 ), BadgeE( L"62", statCode1 ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//светофоры
	BadgeE theads_arr[] = { BadgeE( L"Head600", statCode1 ), BadgeE( L"Head601", statCode1 ), BadgeE( L"Head602T", statCode1 ), BadgeE( L"Head603", statCode1 ), 
		BadgeE( L"Head604", statCode1 ), BadgeE( L"Head605", statCode1 )
	};
	Insert( HEAD, theads_arr, size_array( theads_arr ), true, TRAIN );
	BadgeE shuheads_arr[] = { BadgeE( L"Head602S", statCode1 ) };
	Insert( HEAD, shuheads_arr, size_array( shuheads_arr ), true, SHUNTING );
	BadgeE legs_arr[] = { BadgeE( L"Leg600", statCode1 ), BadgeE( L"Leg601", statCode1 ), BadgeE( L"Leg602", statCode1 ), BadgeE( L"Leg603", statCode1 ),
		BadgeE( L"Leg604", statCode1 ), BadgeE( L"Leg605", statCode1 )
	};
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//маршруты
	CLink rtlinks[] = { CLink( HEAD, BadgeE( L"Head602T", statCode1 ) ), CLink( STRIP, BadgeE( L"G6", statCode1 ) ), CLink( STRIP, BadgeE( L"H6", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head602T->H6", statCode1 ), vector <CLink>( begin( rtlinks ), end( rtlinks ) ) );
	CLink rtlinks2[] = { CLink( HEAD, BadgeE( L"Head604", statCode1 ) ), CLink( STRIP, BadgeE( L"I6", statCode1 ) ), CLink( STRIP, BadgeE( L"I6", statCode1 ) ) };
	InsertRoute( BadgeE( L"Head604->I6", statCode1 ), vector <CLink>( begin( rtlinks2 ), end( rtlinks2 ) ) );

	//связывание
	Link( L"A6", FormLink( L"A6~B6" ) );
	Link( L"B6", FormLink( L"B6~C6" ), FormLink( L"A6~B6" ) );
	Link( L"C6", FormLink( L"61", CLink::BASE ), FormLink( L"B6~C6" ) );
	Link( L"D6", FormLink( L"61", CLink::MINUS ), FormLink( L"D6~F6" ) );
	Link( L"E6", FormLink( L"61", CLink::PLUS ), FormLink( L"62", CLink::MINUS ) );
	Link( L"F6", FormLink( L"D6~F6" ), FormLink( L"F6~G6" ) );
	Link( L"G6", FormLink( L"62", CLink::PLUS ), FormLink( L"F6~G6" ) );
	Link( L"H6", FormLink( L"62", CLink::BASE ), FormLink( L"H6~I6" ) );
	Link( L"I6", FormLink( L"H6~I6" ), FormLink( L"I6~J6" ) );
	Link( L"J6", FormLink( L"I6~J6" ) );

	LinkSema( L"Leg600", L"B6", L"C6", vector <wstring>( 1, L"Head600" ) );
	LinkSema( L"Leg601", L"C6", L"B6", vector <wstring>( 1, L"Head601" ) );

	vector <wstring> heads602;
	heads602.push_back( L"Head602S" );
	heads602.push_back( L"Head602T" );
	LinkSema( L"Leg602", L"F6", L"G6", heads602 );
	LinkSema( L"Leg603", L"F6", L"D6", vector <wstring>( 1, L"Head603" ) );
	LinkSema( L"Leg604", L"H6", L"I6", vector <wstring>( 1, L"Head604" ) );
	LinkSema( L"Leg605", L"I6", L"H6", vector <wstring>( 1, L"Head605" ) );
}

void FictivePart::CreateWrongWayPart()
{
	const auto & statCode1 = EsrKit( 19000 );
	const auto & spanIWCode1 = EsrKit( 19000, 19001 );
	const auto & spanIWCode2 = EsrKit( 19000, 19002 );

	//генерация
	BadgeE strips_arr[] = { BadgeE (L"TIM1", statCode1 ), BadgeE( L"TIM2", spanIWCode1 ), BadgeE( L"TIM3", statCode1 ), BadgeE( L"TIM_E1", spanIWCode1 ),
		BadgeE( L"TIM4", statCode1 ), BadgeE( L"TIM_O1", spanIWCode1 ), BadgeE( L"TIM5", statCode1 ), BadgeE( L"TIM_E2", spanIWCode1 ), BadgeE( L"TIM6", statCode1 ),
		BadgeE( L"TIM7", spanIWCode1 ), BadgeE( L"TIM8", statCode1 ), BadgeE( L"TIM_E3", spanIWCode1 ), BadgeE( L"TIM9", statCode1 ), BadgeE( L"TIM_O2", spanIWCode1 ),
		BadgeE( L"TIM10", statCode1 ), BadgeE( L"TIM11", spanIWCode1 ), BadgeE( L"TIM12", statCode1 ), BadgeE( L"TIM_E4", spanIWCode1 ), BadgeE( L"TIM13", statCode1 ),
		BadgeE( L"TIM_O3", spanIWCode1 ), BadgeE( L"TIM14", statCode1 ), BadgeE( L"TIM15", statCode1 ), BadgeE( L"TIM16", statCode1 ), BadgeE( L"TIM_E5", spanIWCode1 ), 
		BadgeE( L"TIM_E6", spanIWCode1 )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	BadgeE one_way_strips_arr[] = { BadgeE( L"TIM_OOW", spanIWCode2 ) };
	Insert( STRIP, one_way_strips_arr, size_array( one_way_strips_arr ), true, 0, true );

	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"TIM1~TIM2", statCode1 ), BadgeE( L"TIM3~TIM_E1", statCode1 ), BadgeE( L"TIM4~TIM_O1", statCode1 ), 
		BadgeE( L"TIM5~TIM_E2", statCode1 ), BadgeE( L"TIM6~TIM7", statCode1 ), BadgeE( L"TIM8~TIM_E3", statCode1 ), BadgeE( L"TIM9~TIM_O2", statCode1 ), 
		BadgeE( L"TIM10~TIM11", statCode1 ), BadgeE( L"TIM12~TIM_E4", statCode1 ), BadgeE( L"TIM13~TIM_O3", statCode1 ), BadgeE( L"TIM14~TIM_OOW", statCode1 ),
		BadgeE( L"TIM15~TIM3", statCode1 ), BadgeE( L"TIM16~TIM5", statCode1 ), BadgeE( L"TIM_E1~TIM_E5", spanIWCode1 ), BadgeE( L"TIM_E5~TIM_E6", spanIWCode1 ),
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	BadgeE heads_arr[] = { BadgeE( L"TIM_Head2", statCode1 ) };
	Insert( HEAD, heads_arr, size_array( heads_arr ) );

	BadgeE legs_arr[] = { BadgeE( L"TIM_Leg2", statCode1 ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"TIM1", FormLink( L"TIM1~TIM2" ) );
	Link( L"TIM2", FormLink( L"TIM1~TIM2" ) );
	Link( L"TIM3", FormLink( L"TIM3~TIM_E1" ), FormLink( L"TIM15~TIM3" ) );
	Link( L"TIM_E1", FormLink( L"TIM3~TIM_E1" ), FormLink( L"TIM_E1~TIM_E5" ) );
	Link( L"TIM4", FormLink( L"TIM4~TIM_O1" ) );
	Link( L"TIM_O1", FormLink( L"TIM4~TIM_O1" ) );
	Link( L"TIM5", FormLink( L"TIM5~TIM_E2" ), FormLink( L"TIM16~TIM5" ) );
	Link( L"TIM_E2", FormLink( L"TIM5~TIM_E2" ) );
	Link( L"TIM6", FormLink( L"TIM6~TIM7" ) );
	Link( L"TIM7", FormLink( L"TIM6~TIM7" ) );
	Link( L"TIM8", FormLink( L"TIM8~TIM_E3" ) );
	Link( L"TIM_E3", FormLink( L"TIM8~TIM_E3" ) );
	Link( L"TIM9", FormLink( L"TIM9~TIM_O2" ) );
	Link( L"TIM_O2", FormLink( L"TIM9~TIM_O2" ) );
	Link( L"TIM10", FormLink( L"TIM10~TIM11" ) );
	Link( L"TIM11", FormLink( L"TIM10~TIM11" ) );
	Link( L"TIM12", FormLink( L"TIM12~TIM_E4" ) );
	Link( L"TIM_E4", FormLink( L"TIM12~TIM_E4" ) );
	Link( L"TIM13", FormLink( L"TIM13~TIM_O3" ) );
	Link( L"TIM_O3", FormLink( L"TIM13~TIM_O3" ) );
	Link( L"TIM14", FormLink( L"TIM14~TIM_OOW" ) );
	Link( L"TIM_OOW", FormLink( L"TIM14~TIM_OOW" ) );
	Link( L"TIM15", FormLink( L"TIM15~TIM3" ) );
	Link( L"TIM16", FormLink( L"TIM16~TIM5" ) );
	Link( L"TIM_E5", FormLink( L"TIM_E1~TIM_E5" ), FormLink( L"TIM_E5~TIM_E6" ) );
	Link( L"TIM_E6", FormLink( L"TIM_E5~TIM_E6" ) );

	LinkSema( L"TIM_Leg2", L"TIM_E2", L"TIM5", vector <wstring>( 1, L"TIM_Head2" ) );

	//установка атрибутов
	//направления стыков
	CLogicJoint * ljoint6_7 = static_cast <CLogicJoint *>( Get(L"TIM6~TIM7")->lePtr.get() );
	ljoint6_7->SetValidDirection( CLogicJoint::EXIT );
	CLogicJoint * ljoint8_e3 = static_cast <CLogicJoint *>( Get(L"TIM8~TIM_E3")->lePtr.get() );
	ljoint8_e3->SetValidDirection( CLogicJoint::EXIT );
	CLogicJoint * ljoint9_o2 = static_cast <CLogicJoint *>( Get(L"TIM9~TIM_O2")->lePtr.get() );
	ljoint9_o2->SetValidDirection( CLogicJoint::EXIT );
	CLogicJoint * ljoint10_11 = static_cast <CLogicJoint *>( Get(L"TIM10~TIM11")->lePtr.get() );
	ljoint10_11->SetValidDirection( CLogicJoint::ENTRY );
	CLogicJoint * ljoint12_e4 = static_cast <CLogicJoint *>( Get(L"TIM12~TIM_E4")->lePtr.get() );
	ljoint12_e4->SetValidDirection( CLogicJoint::ENTRY );
	CLogicJoint * ljoint13_o3 = static_cast <CLogicJoint *>( Get(L"TIM13~TIM_O3")->lePtr.get() );
	ljoint13_o3->SetValidDirection( CLogicJoint::ENTRY );

	//номера путей перегона
	CLogicStrip * lstrip_e1 = static_cast <CLogicStrip *>( Get(L"TIM_E1")->lePtr.get() );
	lstrip_e1->SetWaynum( 2 );
	CLogicStrip * lstrip_o1 = static_cast <CLogicStrip *>( Get(L"TIM_O1")->lePtr.get() );
	lstrip_o1->SetWaynum( 1 );
	CLogicStrip * lstrip_e2 = static_cast <CLogicStrip *>( Get(L"TIM_E2")->lePtr.get() );
	lstrip_e2->SetWaynum( 2 );
	CLogicStrip * lstrip_e3 = static_cast <CLogicStrip *>( Get(L"TIM_E3")->lePtr.get() );
	lstrip_e3->SetWaynum( 2 );
	CLogicStrip * lstrip_o2 = static_cast <CLogicStrip *>( Get(L"TIM_O2")->lePtr.get() );
	lstrip_o2->SetWaynum( 1 );
	CLogicStrip * lstrip_e4 = static_cast <CLogicStrip *>( Get(L"TIM_E4")->lePtr.get() );
	lstrip_e4->SetWaynum( 2 );
	CLogicStrip * lstrip_o3 = static_cast <CLogicStrip *>( Get(L"TIM_O3")->lePtr.get() );
	lstrip_o3->SetWaynum( 1 );
	CLogicStrip * lstrip_oow = static_cast <CLogicStrip *>( Get(L"TIM_OOW")->lePtr.get() );
	lstrip_oow->SetWaynum( 1 );
	CLogicStrip * lstrip_e5 = static_cast <CLogicStrip *>( Get(L"TIM_E5")->lePtr.get() );
	lstrip_e5->SetWaynum( 2 );
	CLogicStrip * lstrip_e6 = static_cast <CLogicStrip *>( Get(L"TIM_E6")->lePtr.get() );
	lstrip_e6->SetWaynum( 2 );

	//четность светофоров
	CLogicSemaleg * lsema2 = static_cast <CLogicSemaleg *>( Get(L"TIM_Leg2")->lePtr.get() );
	lsema2->SetOddness( Oddness::EVEN );
}

void FictivePart::CreatePocketPart()
{
	const auto & statCode1 = EsrKit( 28000 );
	const auto & statCode2 = EsrKit( 29000 );
	const auto & spanPKCode1 = EsrKit( 48001, 48002 );
	const auto & spanPKCode2 = EsrKit( 48002, 48003 );
	const auto & spanPKCode3 = EsrKit( 48003, 48004 );

	//генерация
	BadgeE strips_arr[] = { BadgeE ( L"PK1", spanPKCode1 ), BadgeE ( L"PK2", spanPKCode1 ), BadgeE( L"PK3", spanPKCode1 ), BadgeE( L"PK4", spanPKCode1 ), 
		BadgeE( L"PK5", spanPKCode1 ), BadgeE( L"PK6", spanPKCode1 ), BadgeE( L"PK7", spanPKCode1 ), BadgeE( L"PK8", spanPKCode1 ), BadgeE( L"PK9", spanPKCode1 ), 
		BadgeE( L"PK10", spanPKCode1 ), BadgeE( L"PK11", spanPKCode1 ), BadgeE( L"PK12", spanPKCode1 ), BadgeE( L"PK13", spanPKCode1 ), BadgeE( L"PK14", spanPKCode1 ), 
		BadgeE( L"PK15", spanPKCode1 ), BadgeE( L"PK16", spanPKCode1 ), 

		BadgeE( L"STPK1", statCode1 ), BadgeE( L"STPK2", statCode1 ), BadgeE( L"LPK2", spanPKCode2 ), BadgeE( L"LPK4", spanPKCode2 ), BadgeE( L"LPK6", spanPKCode2 ), 
		BadgeE( L"LPK8", spanPKCode2 ), BadgeE( L"LPK10", spanPKCode2 ), BadgeE( L"LPK12", spanPKCode2 ), BadgeE( L"LPK14", spanPKCode2 ),

		BadgeE( L"FST3", statCode1 ), BadgeE( L"FST1", statCode1 ), BadgeE( L"FPK1", spanPKCode3 ), BadgeE( L"FPK2", spanPKCode3 ), BadgeE( L"FPK3", spanPKCode3 ), 
		BadgeE( L"FST2", statCode2 ), BadgeE( L"FST4", statCode2 )
	};
	Insert( STRIP, strips_arr, 3 ); //PK1-PK3 - участки с ТС
	Insert( STRIP, strips_arr + 3, 3, false ); //PK4-PK6 - карман 1
	Insert( STRIP, strips_arr + 6, 2 ); //PK7-PK8 - участки с ТС
	Insert( STRIP, strips_arr + 8, 3, false ); //PK9-PK11 - карман 2
	Insert( STRIP, strips_arr + 11, 2 ); //PK12-PK13
	Insert( STRIP, strips_arr + 13, 1, false ); //PK14 - карман 3
	Insert( STRIP, strips_arr + 14, 2 ); //PK15-PK16

	Insert( STRIP, strips_arr + 16, 3 ); //STPK1-LPK2 - участки с ТС
	Insert( STRIP, strips_arr + 19, 5, false ); //LPK4 - LPK12 - карман 3
	Insert( STRIP, strips_arr + 24, 1 ); //LPK14 - участок с ТС

	Insert( STRIP, strips_arr + 25, 2 ); //FST3 и FST1 - участки с ТС
	Insert( STRIP, strips_arr + 27, 3, false ); //FPK1 - FPK3 - псевдокарман
	Insert( STRIP, strips_arr + 30, 2 ); //FST2 и FST4 - участки с ТС

	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"PK1~PK2", spanPKCode1 ), BadgeE( L"PK2~PK3", spanPKCode1 ), BadgeE( L"PK3~PK4", spanPKCode1 ), 
		BadgeE( L"PK4~PK5", spanPKCode1 ), BadgeE( L"PK5~PK6", spanPKCode1 ), BadgeE( L"PK6~PK7", spanPKCode1 ), BadgeE( L"PK7~PK8", spanPKCode1 ), 
		BadgeE( L"PK8~PK9", spanPKCode1 ), BadgeE( L"PK9~PK10", spanPKCode1 ), BadgeE( L"PK10~PK11", spanPKCode1 ), BadgeE( L"PK11~PK12", spanPKCode1 ), 
		BadgeE( L"PK12~PK13", spanPKCode1 ), BadgeE( L"PK13~PK14", spanPKCode1 ), BadgeE( L"PK14~PK15", spanPKCode1 ), BadgeE( L"PK15~PK16", spanPKCode1 ),
		BadgeE( L"STPK1~STPK2", statCode1 ), BadgeE( L"LPK2~STPK2", spanPKCode2 ), BadgeE( L"LPK2~LPK4", spanPKCode2 ), BadgeE( L"LPK4~LPK6", spanPKCode2 ), 
		BadgeE( L"LPK6~LPK8", spanPKCode2 ), BadgeE( L"LPK8~LPK10", spanPKCode2 ), BadgeE( L"LPK10~LPK12", spanPKCode2 ), BadgeE( L"LPK12~LPK14", spanPKCode2 ), 
		BadgeE( L"FST1~FST3", statCode1 ), BadgeE( L"FPK1~FST1", spanPKCode3 ), BadgeE( L"FPK1~FPK2", spanPKCode3 ), BadgeE( L"FPK2~FPK3", spanPKCode3 ), 
		BadgeE( L"FPK3~FST2", spanPKCode3 ), BadgeE( L"FST2~FST4", statCode2 )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );


	//светофоры
	BadgeE train_heads_arr[] = { BadgeE( L"PKHead1", statCode1 ) };
	Insert( HEAD, train_heads_arr, size_array( train_heads_arr ), true, TRAIN );

	BadgeE legs_arr[] = { BadgeE( L"PKLeg1", statCode1 ) };
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//связывание
	Link( L"PK1", FormLink( L"PK1~PK2" ) );
	Link( L"PK2", FormLink( L"PK1~PK2" ), FormLink( L"PK2~PK3" ) );
	Link( L"PK3", FormLink( L"PK2~PK3" ), FormLink( L"PK3~PK4" ) );
	Link( L"PK4", FormLink( L"PK3~PK4" ), FormLink( L"PK4~PK5" ) );
	Link( L"PK5", FormLink( L"PK4~PK5" ), FormLink( L"PK5~PK6" ) );
	Link( L"PK6", FormLink( L"PK5~PK6" ), FormLink( L"PK6~PK7" ) );
	Link( L"PK7", FormLink( L"PK6~PK7" ), FormLink( L"PK7~PK8" ) );
	Link( L"PK8", FormLink( L"PK7~PK8" ), FormLink( L"PK8~PK9" ) );
	Link( L"PK9", FormLink( L"PK8~PK9" ), FormLink( L"PK9~PK10" ) );
	Link( L"PK10", FormLink( L"PK9~PK10" ), FormLink( L"PK10~PK11" ) );
	Link( L"PK11", FormLink( L"PK10~PK11" ), FormLink( L"PK11~PK12" ) );
	Link( L"PK12", FormLink( L"PK11~PK12" ), FormLink( L"PK12~PK13" ) );
	Link( L"PK13", FormLink( L"PK12~PK13" ), FormLink( L"PK13~PK14" ) );
	Link( L"PK14", FormLink( L"PK13~PK14" ), FormLink( L"PK14~PK15" ) );
	Link( L"PK15", FormLink( L"PK14~PK15" ), FormLink( L"PK15~PK16" ) );
	Link( L"PK16", FormLink( L"PK15~PK16" ) );

	Link( L"STPK1", FormLink( L"STPK1~STPK2" ) );
	Link( L"STPK2", FormLink( L"STPK1~STPK2" ), FormLink( L"LPK2~STPK2" ) );
	Link( L"LPK2", FormLink( L"LPK2~STPK2" ), FormLink( L"LPK2~LPK4" ) );
	Link( L"LPK4", FormLink( L"LPK2~LPK4" ), FormLink( L"LPK4~LPK6" ) );
	Link( L"LPK6", FormLink( L"LPK4~LPK6" ), FormLink( L"LPK6~LPK8" ) );
	Link( L"LPK8", FormLink( L"LPK6~LPK8" ), FormLink( L"LPK8~LPK10" ) );
	Link( L"LPK10", FormLink( L"LPK8~LPK10" ), FormLink( L"LPK10~LPK12" ) );
	Link( L"LPK12", FormLink( L"LPK10~LPK12" ), FormLink( L"LPK12~LPK14" ) );
	Link( L"LPK14", FormLink( L"LPK12~LPK14" ) );

	Link( L"FST3", FormLink( L"FST1~FST3" ) );
	Link( L"FST1", FormLink( L"FST1~FST3" ), FormLink( L"FPK1~FST1" ) );
	Link( L"FPK1", FormLink( L"FPK1~FST1" ), FormLink( L"FPK1~FPK2" ) );
	Link( L"FPK2", FormLink( L"FPK1~FPK2" ), FormLink( L"FPK2~FPK3" ) );
	Link( L"FPK3", FormLink( L"FPK2~FPK3" ), FormLink( L"FPK3~FST2" ) );
	Link( L"FST2", FormLink( L"FPK3~FST2" ), FormLink( L"FST2~FST4" ) );
	Link( L"FST4", FormLink( L"FST2~FST4" ) );

	LinkSema( L"PKLeg1", L"LPK2", L"STPK2", vector <wstring>( 1, L"PKHead1" ) );
}

void FictivePart::CreateBlockPostPart()
{
	const auto & statCode1 = EsrKit( 38000 );
	const auto & statCode2 = EsrKit( 39000 );
	const auto & spanBPCode1 = EsrKit( 16001, 16002 );
	const auto & spanBPCode2 = EsrKit( 16002, 16003 );
	const auto & spanBPCode3 = EsrKit( 16003, 16004 );
	const auto & spanBPCode4 = EsrKit( 16004, 16005 );
	const auto & spanBPCode5 = EsrKit( 16005, 16006 );
	const auto & blockPostCode = EsrKit( 51000 );

	//генерация
	BadgeE strips_arr[] = { BadgeE ( L"SBP1", spanBPCode1 ), BadgeE( L"SBP2", spanBPCode1 ), BadgeE( L"SBP3", spanBPCode2 ), BadgeE( L"SBP4", spanBPCode2 ), 
		BadgeE( L"SBP5", spanBPCode3 ), BadgeE( L"SBP6", spanBPCode3 ), BadgeE( L"SBP7", statCode1 ), BadgeE( L"SBP8", spanBPCode4 ), BadgeE( L"SBP9", statCode2 ),
		BadgeE( L"SBP10", statCode2 ), BadgeE( L"SBP11", spanBPCode5 ), BadgeE( L"SBP12", statCode2 ), BadgeE( L"SBP13", statCode2 ), BadgeE( L"ADBP1", statCode1 ), 
		BadgeE( L"ADBP2", statCode2 ), BadgeE( L"BP1", blockPostCode ), BadgeE( L"BP2", blockPostCode ), BadgeE( L"BP3", blockPostCode ), 
		BadgeE( L"BP4", blockPostCode ), BadgeE( L"BP5", blockPostCode ), BadgeE( L"BP6", blockPostCode ), BadgeE( L"BP7", blockPostCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//стрелки
	BadgeE switches_arr[] = { BadgeE( L"BPSW", statCode2 ) };
	Insert( SWITCH, switches_arr, size_array( switches_arr ) );

	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"SBP1~SBP2", spanBPCode1 ), BadgeE( L"BP1~SBP2", spanBPCode1 ), BadgeE( L"BP1~BP2", blockPostCode ), 
		BadgeE( L"SBP3~SBP4", spanBPCode2 ), BadgeE( L"BP3~SBP4", spanBPCode2 ), BadgeE( L"BP3~BP4", blockPostCode ), BadgeE( L"SBP5~SBP6", spanBPCode3 ),
		BadgeE( L"BP5~SBP6", spanBPCode3 ), BadgeE( L"BP5~BP6", blockPostCode ), BadgeE( L"SBP5~SBP7", statCode1 ), BadgeE( L"ADBP1~SBP7", statCode1 ),
		BadgeE( L"BP6~SBP8", spanBPCode4 ), BadgeE( L"SBP8~SBP9", statCode2 ), BadgeE( L"ADBP2~SBP12", statCode2 ), BadgeE( L"SBP10~SBP11", statCode2 ),
		BadgeE( L"SBP12~SBP13", statCode2 ), BadgeE( L"BP7~SBP13", statCode2 )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//светофоры
	BadgeE heads_arr[] = { BadgeE( L"BPHead100", blockPostCode ), BadgeE( L"BPHead101", blockPostCode ), BadgeE( L"BPHead102", blockPostCode ), 
		BadgeE( L"BPHead104", statCode2 ), BadgeE( L"BPHead106", statCode2 )
	};
	Insert( HEAD, heads_arr, size_array( heads_arr ), true, TRAIN );

	BadgeE legs_arr[] = { BadgeE( L"BPLeg100", blockPostCode ), BadgeE( L"BPLeg101", blockPostCode ), BadgeE( L"BPLeg102", blockPostCode ), 
		BadgeE( L"BPLeg104", statCode2 ), BadgeE( L"BPLeg106", statCode2 ) 
	};
	Insert( SEMALEG, legs_arr, size_array( legs_arr ) );

	//маршруты
	CLink rtlinks[] = { CLink( HEAD, BadgeE( L"BPHead102", blockPostCode ) ), CLink( STRIP, BadgeE( L"BP5", blockPostCode ) ) };
	InsertRoute( BadgeE( L"BPHead102->BP6", blockPostCode ), vector <CLink>( begin( rtlinks ), end( rtlinks ) ) );
	CLink rtlinks2[] = { CLink( HEAD, BadgeE( L"BPHead100", blockPostCode ) ), CLink( STRIP, BadgeE( L"BP3", blockPostCode ) ) };
	InsertRoute( BadgeE( L"BPHead100->BP4", blockPostCode ), vector <CLink>( begin( rtlinks2 ), end( rtlinks2 ) ) );
	CLink rtlinks3[] = { CLink( HEAD, BadgeE( L"BPHead106", statCode2 ) ), CLink( STRIP, BadgeE( L"SBP13", statCode2 ) ) };
	InsertRoute( BadgeE( L"BPHead106->SBP13", statCode2 ), vector <CLink>( begin( rtlinks3 ), end( rtlinks3 ) ) );

	//связывание
	Link( L"SBP1", FormLink( L"SBP1~SBP2" ) );
	Link( L"SBP2", FormLink( L"SBP1~SBP2" ), FormLink( L"BP1~SBP2" ) );
	Link( L"BP1", FormLink( L"BP1~SBP2" ), FormLink( L"BP1~BP2" ) );
	Link( L"BP2", FormLink( L"BP1~BP2" ) );
	Link( L"SBP3", FormLink( L"SBP3~SBP4" ) );
	Link( L"SBP4", FormLink( L"SBP3~SBP4" ), FormLink( L"BP3~SBP4" ) );
	Link( L"BP3", FormLink( L"BP3~SBP4" ), FormLink( L"BP3~BP4" ) );
	Link( L"BP4", FormLink( L"BP3~BP4" ) );

	Link( L"ADBP1", FormLink( L"ADBP1~SBP7" ) );
	Link( L"SBP7", FormLink( L"ADBP1~SBP7" ), FormLink( L"SBP5~SBP7" ) );
	Link( L"SBP5", FormLink( L"SBP5~SBP6" ), FormLink( L"SBP5~SBP7" ) );
	Link( L"SBP6", FormLink( L"SBP5~SBP6" ), FormLink( L"BP5~SBP6" ) );
	Link( L"BP5", FormLink( L"BP5~SBP6" ), FormLink( L"BP5~BP6" ) );
	Link( L"BP6", FormLink( L"BP5~BP6" ), FormLink( L"BP6~SBP8" ) );
	Link( L"SBP8", FormLink( L"BP6~SBP8" ), FormLink( L"SBP8~SBP9" ) );
	Link( L"SBP9", FormLink( L"SBP8~SBP9" ), FormLink( L"BPSW", CLink::BASE ) );
	Link( L"ADBP2", FormLink( L"BPSW", CLink::PLUS ), FormLink( L"ADBP2~SBP12" ) );
	Link( L"SBP10", FormLink( L"BPSW", CLink::MINUS ), FormLink( L"SBP10~SBP11" ) );
	Link( L"SBP11", FormLink( L"SBP10~SBP11" ) );
	Link( L"SBP12", FormLink( L"ADBP2~SBP12" ), FormLink( L"SBP12~SBP13" ) );
	Link( L"SBP13", FormLink( L"SBP12~SBP13" ), FormLink( L"BP7~SBP13" ) );
	Link( L"BP7", FormLink( L"BP7~SBP13" ) );

	LinkSema( L"BPLeg100", L"SBP4", L"BP3", vector <wstring>( 1, L"BPHead100" ) );
	LinkSema( L"BPLeg101", L"SBP8", L"BP6", vector <wstring>( 1, L"BPHead101" ) );
	LinkSema( L"BPLeg102", L"SBP6", L"BP5", vector <wstring>( 1, L"BPHead102" ) );
	LinkSema( L"BPLeg104", L"SBP8", L"SBP9", vector <wstring>( 1, L"BPHead104" ) );
	LinkSema( L"BPLeg106", L"ADBP2", L"SBP12", vector <wstring>( 1, L"BPHead106" ) );
}

void FictivePart::CreateLongSpan()
{
	const auto & statCode = EsrKit( 3002 );
	const auto & spanCode = EsrKit( 3002, 3009);

	//генерация
	BadgeE strips_arr[] = { BadgeE ( L"LS1", spanCode ), BadgeE( L"LS2", spanCode ), BadgeE( L"LS3", spanCode ), BadgeE( L"LS4", spanCode ), 
		BadgeE( L"LS5", spanCode ), BadgeE( L"LS6", spanCode ), BadgeE( L"LS7", spanCode ), BadgeE( L"LS8", spanCode ), BadgeE( L"LS9", spanCode ),
		BadgeE( L"LS10", statCode )
	};
	Insert( STRIP, strips_arr, size_array( strips_arr ) );

	//именования стыков приняты в лексикографическом порядке
	BadgeE joints_arr[] = { BadgeE( L"LS1~LS2", spanCode ), BadgeE( L"LS2~LS3", spanCode ), BadgeE( L"LS3~LS4", spanCode ), BadgeE( L"LS4~LS5", spanCode ), 
		BadgeE( L"LS5~LS6", spanCode ), BadgeE( L"LS6~LS7", spanCode ), BadgeE( L"LS7~LS8", spanCode ), BadgeE( L"LS8~LS9", spanCode ), 
		BadgeE( L"LS9~LS10", statCode )
	};
	Insert( JOINT, joints_arr, size_array( joints_arr ) );

	//связывание
	Link( L"LS1", FormLink( L"LS1~LS2" ) );
	Link( L"LS2", FormLink( L"LS1~LS2" ), FormLink( L"LS2~LS3" ) );
	Link( L"LS3", FormLink( L"LS2~LS3" ), FormLink( L"LS3~LS4" ) );
	Link( L"LS4", FormLink( L"LS3~LS4" ), FormLink( L"LS4~LS5" ) );
	Link( L"LS5", FormLink( L"LS4~LS5" ), FormLink( L"LS5~LS6" ) );
	Link( L"LS6", FormLink( L"LS5~LS6" ), FormLink( L"LS6~LS7" ) );
	Link( L"LS7", FormLink( L"LS6~LS7" ), FormLink( L"LS7~LS8" ) );
	Link( L"LS8", FormLink( L"LS7~LS8" ), FormLink( L"LS8~LS9" ) );
	Link( L"LS9", FormLink( L"LS8~LS9" ), FormLink( L"LS9~LS10" ) );
	Link( L"LS10", FormLink( L"LS9~LS10" ) );
}

void FictivePart::SetStripDetails()
{
	//доп.атрибуты (номера п/о путей, парков и т.д.)
	list <CLogicStrip *> arrdep_strips;
	wstring adStripsNames[] = { L"C", L"D", L"E", L"M", L"AC", L"AH", L"AI", L"T2", L"I3", L"K3", L"P3", L"S3", L"R4", L"S4", L"ADBP1", L"ADBP2", L"V3", L"W3", 
		L"ST5", L"F5", L"F6", L"AC5", L"Z5", L"BC5", L"BG5", L"BJ5", L"BM5" };
	for_each( begin( adStripsNames ), end( adStripsNames ), [&arrdep_strips, this]( wstring arrdepName ){
		arrdep_strips.push_back( static_cast <CLogicStrip *>( Get( arrdepName )->lePtr.get() ) );
	} );
	for( CLogicStrip * arrdep_strip : arrdep_strips )
	{
		auto strip_name = arrdep_strip->GetName();
		if ( strip_name == L"D" || strip_name == L"AH" )
			arrdep_strip->SetWaynum( 2 );
		else
			arrdep_strip->SetWaynum( 1 );

		if ( strip_name == L"C" )
			arrdep_strip->SetParknum( 3 );
		else if ( strip_name == L"R4" || strip_name == L"S4" )
		{
			arrdep_strip->SetParknum( 4 );
			if ( strip_name == L"R4" )
			{
				ADProperties::AttrSet adAttributes;
				adAttributes.set( ADAttribute::EMBARKATION );
				arrdep_strip->SetADProperties( ADProperties( adAttributes, Oddness::UNDEF, WeightNorms() ) );
			}
		}
		else if ( strip_name == L"I3" )
			arrdep_strip->SetParknum( 1 );
		else if ( strip_name == L"K3" || strip_name == L"AC" || strip_name == L"AH" )
			arrdep_strip->SetParknum( 2 );
		//у путей D и E парки не определены
	}

	//номера перегонных путей
	wstring stripNames[] = { L"Q", L"X", L"I", L"N3", L"U3", L"X3", L"Y3", L"TIM_OOW" };
	for ( auto snIt = begin( stripNames ); snIt != end( stripNames ); ++snIt )
	{
		const auto & stripName = *snIt;
		CLogicStrip * lstrip = static_cast <CLogicStrip *>( Get( stripName )->lePtr.get() );
		if ( stripName == L"Q" )
			lstrip->SetWaynum( 2 );
		else
			lstrip->SetWaynum( 1 );
	}

	//длины путей
	auto wstrip = static_cast <CLogicStrip *>( Get(L"W")->lePtr.get() );
	wstrip->SetLength( 11000 );
	auto hstrip = static_cast <CLogicStrip *>( Get(L"H")->lePtr.get() );
	hstrip->SetLength( 7000 );
	auto pk9strip = static_cast <CLogicStrip *>( Get(L"PK10")->lePtr.get() );
	pk9strip->SetLength( 1250 );
}

void FictivePart::SetJointDetails()
{
	//стыки
	SetJointCoords(); //координаты
}

void FictivePart::SetDetails()
{
	SetStripDetails();
	SetJointDetails();
	SetSemaDetails();
	SetSpanDetails();
}

void FictivePart::SetSemaDetails()
{
	//светофоры
	//нечетные:
	wstring oddSemaNames[] = { L"Leg101", L"Leg103", L"Leg105", L"Leg107", L"Leg201", L"Leg203", L"Leg301", L"Leg303", L"Leg305", L"Leg307", L"Leg401", L"Leg403",
		L"Leg405", L"PKLeg1", L"Leg501", L"Leg503", L"Leg507", L"Leg509", L"Leg511", L"Leg513", L"Leg515", L"Leg517", L"Leg519", L"Leg521" 
	};
	for ( auto oddSemaName : oddSemaNames )
	{
		CLogicSemaleg * oddleg = static_cast <CLogicSemaleg *>( Get( oddSemaName )->lePtr.get() );
		oddleg->SetOddness( Oddness::ODD );
	}

	//четные:
	wstring evenSemaNames[] = { L"Leg100", L"Leg102", L"Leg104", L"Leg106", L"Leg108", L"Leg200", L"Leg202", L"Leg204", L"Leg300", L"Leg302", L"Leg306", 
		L"Leg308", L"Leg310", L"Leg312", L"Leg314", L"Leg400", L"Leg402", L"Leg404", L"Leg406", L"Leg408", L"Leg410", L"Leg508", L"Leg510", L"Leg512",
		L"Leg514", L"Leg516", L"Leg518", L"Leg520", L"Leg522", L"Leg524"
	};
	for ( auto evenSemaName : evenSemaNames )
	{
		CLogicSemaleg * evenleg = static_cast <CLogicSemaleg *>( Get( evenSemaName )->lePtr.get() );
		evenleg->SetOddness( Oddness::EVEN );
	}

	//неизвестные
	wstring unknownSemaNames[] = { L"Leg407", L"Leg500" };
	for ( auto unknownSemaName : unknownSemaNames )
	{
		CLogicSemaleg * unkleg = static_cast <CLogicSemaleg *>( Get( unknownSemaName )->lePtr.get() );
		unkleg->SetOddness( Oddness::UNDEF );
	}
}

void FictivePart::SetJointCoords()
{
	//координаты стыков
	PicketingInfo pinfo;
	auto wh_joint = static_cast <CLogicJoint *>( Get(L"H~W")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 51, 150 ) ) );
	wh_joint->SetPicketingInfo( pinfo );
	auto qw_joint = static_cast <CLogicJoint *>( Get(L"Q~W")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 56, 150 ) ) );
	qw_joint->SetPicketingInfo( pinfo );
	auto aq_joint = static_cast <CLogicJoint *>( Get(L"A~Q")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 57, 700 ) ) );
	aq_joint->SetPicketingInfo( pinfo );
	auto bc_joint = static_cast <CLogicJoint *>( Get(L"B~C")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 57, 850 ) ) );
	bc_joint->SetPicketingInfo( pinfo );
	auto cf_joint = static_cast <CLogicJoint *>( Get(L"C~F")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 57, 950 ) ) );
	cf_joint->SetPicketingInfo( pinfo );
	auto gx_joint = static_cast <CLogicJoint *>( Get(L"G~X")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 58, 300 ) ) );
	gx_joint->SetPicketingInfo( pinfo );
	auto m2o2_joint = static_cast <CLogicJoint *>( Get(L"M2~O2")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 74, 400 ) ) );
	m2o2_joint->SetPicketingInfo( pinfo );
	auto d2m2_joint = static_cast <CLogicJoint *>( Get(L"D2~M2")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 74, 850 ) ) );
	d2m2_joint->SetPicketingInfo( pinfo );

	//карманы
	auto pk1pk2_joint = static_cast <CLogicJoint *>( Get(L"PK2~PK3")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 10, 280 ) ) );
	pk1pk2_joint->SetPicketingInfo( pinfo );
	auto pk2pk3_joint = static_cast <CLogicJoint *>( Get(L"PK3~PK4")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 10, 720 ) ) );
	pk2pk3_joint->SetPicketingInfo( pinfo );
	auto pk3pk4_joint = static_cast <CLogicJoint *>( Get(L"PK4~PK5")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 11, 300 ) ) );
	pk3pk4_joint->SetPicketingInfo( pinfo );
	auto pk4pk5_joint = static_cast <CLogicJoint *>( Get(L"PK5~PK6")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 12, 50 ) ) );
	pk4pk5_joint->SetPicketingInfo( pinfo );
	auto pk5pk6_joint = static_cast <CLogicJoint *>( Get(L"PK6~PK7")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 12, 650 ) ) );
	pk5pk6_joint->SetPicketingInfo( pinfo );
	auto pk6pk7_joint = static_cast <CLogicJoint *>( Get(L"PK7~PK8")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 12, 860 ) ) );
	pk6pk7_joint->SetPicketingInfo( pinfo );
	auto pk7pk8_joint = static_cast <CLogicJoint *>( Get(L"PK8~PK9")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 13, 400 ) ) );
	pk7pk8_joint->SetPicketingInfo( pinfo );
	auto pk8pk9_joint = static_cast <CLogicJoint *>( Get(L"PK9~PK10")->lePtr.get() );
	pinfo.SetCoord( rwRuledCoord( axis_name, rwCoord( 13, 820 ) ) );
	pk8pk9_joint->SetPicketingInfo( pinfo );
}

void FictivePart::SetSpanDetails()
{
	SetMostSpanDetails();
	SetIWSpanDetails();
}

typedef LogicSpanWay::JointStrips JointStrips;

void FictivePart::SetMostSpanDetails()
{
	typedef LogicSpanWay::StripsLegs StripsLegs;
	typedef LogicSpanWay::JointsStrips JointsStrips;

	//первая часть:
	// - однопутный перегон с дополнительным фиктивным путем
	//кодировка номер раздела - две цифры индексов станций (1_12 = раздел 1, между станцией 101 и 102)
	const EsrKit & spanCode1_12 = EsrKit( 101, 102 );
	list<LogicSpanWay> spanWays1_12;
	list<const CLogicElement *> lstrips1_12;
	lstrips1_12.push_back( Get( L"X" )->lePtr.get() );
	lstrips1_12.push_back( Get( L"I" )->lePtr.get() );
	map<const CLogicElement *, StripsLegs> legs1_12;
	StripsLegs xstripLegs;
	set<const CLogicElement *> & legsFromX = xstripLegs.first;
	legsFromX.insert( Get( L"Leg105" )->lePtr.get() );
	legs1_12.insert( make_pair( Get( L"X" )->lePtr.get(), xstripLegs ) );

	LogicSpanWay spanWay1_12( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips1_12 ), move( legs1_12 ), JointStrips() );
	spanWays1_12.push_back( spanWay1_12 );
	lstrips1_12.clear();
	lstrips1_12.push_back( Get( L"AF" )->lePtr.get() );
	spanWay1_12 = LogicSpanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips1_12 ), map<const CLogicElement *, StripsLegs>(), JointStrips() );
	spanWays1_12.push_back( spanWay1_12 );

	TestSpanKitPtr spanKitPtr1_12( new TestSpanKit( spanCode1_12, spanWays1_12 ) );
	spanKits.insert( make_pair( spanCode1_12, spanKitPtr1_12 ) );

	//вторая часть:
	//нестандартный перегон с разнонаправленными светофорами одинаковой четности
	const EsrKit & spanCode2_34 = EsrKit( 203, 204 );
	list<LogicSpanWay> spanWays2_34;
	list<const CLogicElement *> lstrips2_34;
	lstrips2_34.push_back( Get( L"X2" )->lePtr.get() );
	lstrips2_34.push_back( Get( L"Y2" )->lePtr.get() );
	LogicSpanWay spanWay2_34( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips2_34 ), map<const CLogicElement *, StripsLegs>(), JointStrips() );
	spanWays2_34.push_back( spanWay2_34 );
	TestSpanKitPtr spanKitPtr2_34( new TestSpanKit( spanCode2_34, spanWays2_34, shared_ptr<Oddness>( new Oddness( Oddness::ODD ) ) ) );
	spanKits.insert( make_pair( spanCode2_34, spanKitPtr2_34 ) );

	//третья часть:
	// - протяженный перегонный путь с ПАБ
	const auto & spanCode3_12 = EsrKit( 301, 302 );
	list<LogicSpanWay> spanWays3_12;
	list<const CLogicElement *> lstrips3_12;
	lstrips3_12.push_back( Get( L"X3" )->lePtr.get() );
	lstrips3_12.push_back( Get( L"Y3" )->lePtr.get() );
	LogicSpanWay spanWay32( SpanLockType::SEMI_AUTOBLOCK, move( lstrips3_12 ), map<const CLogicElement *, StripsLegs>(), JointStrips() );
	spanWays3_12.push_back( spanWay32 );
	TestSpanKitPtr spanKitPtr3_12( new TestSpanKit( spanCode3_12, spanWays3_12 ) );
	spanKits.insert( make_pair( spanCode3_12, spanKitPtr3_12 ) );

	//- короткий перегонный путь с ПАБ, состоящий из единственного участка и аналогичный путь с АБ
	const auto & spanCode3_23 = EsrKit( 302, 303 );
	list<LogicSpanWay> spanWays3_23;
	list<const CLogicElement *> lstrips3_23;
	lstrips3_23.push_back( Get( L"N3" )->lePtr.get() );
	LogicSpanWay spanWay3_23( SpanLockType::SEMI_AUTOBLOCK, move( lstrips3_23 ), map<const CLogicElement *, StripsLegs>(), JointStrips() );
	spanWays3_23.push_back( spanWay3_23 );
	TestSpanKitPtr spanKitPtr3_23( new TestSpanKit( spanCode3_23, spanWays3_23 ) );
	spanKits.insert( make_pair( spanCode3_23, spanKitPtr3_23 ) );

	const auto & spanCode3_34 = EsrKit( 303, 304 );
	list<LogicSpanWay> spanWays3_34;
	list<const CLogicElement *> lstrips3_34;
	lstrips3_34.push_back( Get( L"U3" )->lePtr.get() );
	LogicSpanWay spanWay3_34( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips3_34 ), map<const CLogicElement *, StripsLegs>(), JointStrips() );
	spanWays3_34.push_back( spanWay3_34 );
	TestSpanKitPtr spanKitPtr3_34( new TestSpanKit( spanCode3_34, spanWays3_34 ) );
	spanKits.insert( make_pair( spanCode3_34, spanKitPtr3_34 ) );

	//пятая часть:
	const auto & spanCode5_78 = EsrKit( 507, 508 );
	list<LogicSpanWay> spanWays5_78;
	list<const CLogicElement *> lstrips5_78;
	lstrips5_78.push_back( Get( L"BA5" )->lePtr.get() );
	LogicSpanWay spanWay5_78( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips5_78 ), map<const CLogicElement *, StripsLegs>(), JointStrips() );
	spanWays5_78.push_back( spanWay5_78 );
	TestSpanKitPtr spanKitPtr5_78( new TestSpanKit( spanCode5_78, spanWays5_78, shared_ptr<Oddness>( new Oddness( Oddness::EVEN ) ) ) );
	spanKits.insert( make_pair( spanCode5_78, spanKitPtr5_78 ) );
}

void FictivePart::SetIWSpanDetails()
{
	typedef LogicSpanWay::StripsLegs StripsLegs;
	const auto & spanIWCode1 = EsrKit( 19000, 19001 );
	const auto & spanIWCode2 = EsrKit( 19000, 19002 );

	//часть про проверку корректного выезда на перегон:
	list<LogicSpanWay> spanIWWays1;

	wstring stripNames1[] = { L"TIM2", L"TIM_E1", L"TIM_O1", L"TIM_E2", L"TIM7", L"TIM_E3", L"TIM_O2", L"TIM11", L"TIM_E4", L"TIM_O3" };
	for ( auto snIt = begin( stripNames1 ); snIt != end( stripNames1 ); ++snIt )
	{
		wstring stripName = *snIt;
		list<const CLogicElement *> lstrips;
		lstrips.push_back( Get( stripName )->lePtr.get() );
		map<const CLogicElement *, StripsLegs> iwlegs;
		StripsLegs timE2stripLegs;
		set<const CLogicElement *> & legsFromTimE2 = timE2stripLegs.first;
		legsFromTimE2.insert( Get( L"TIM_Leg2" )->lePtr.get() );
		iwlegs.insert( make_pair( Get( L"TIM_E2" )->lePtr.get(), timE2stripLegs ) );
		LogicSpanWay spanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips ), move( iwlegs ), JointStrips() );
		spanIWWays1.push_back( spanWay );
	}

	TestSpanKitPtr spanKitPtr1( new TestSpanKit( spanIWCode1, spanIWWays1 ) );
	spanKits.insert( make_pair( spanIWCode1, spanKitPtr1 ) );

	list<LogicSpanWay> spanIWWays2;
	list<const CLogicElement *> lstrips2;
	lstrips2.push_back( Get( L"TIM_OOW" )->lePtr.get() );
	LogicSpanWay spanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips2 ), map<const CLogicElement *, StripsLegs>(), JointStrips() );
	spanIWWays2.push_back( spanWay );
	TestSpanKitPtr spanKitPtr2( new TestSpanKit( spanIWCode2, spanIWWays2 ) );
	spanKits.insert( make_pair( spanIWCode2, spanKitPtr2 ) );
}