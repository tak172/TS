#include "stdafx.h"
#include "TC_RoadKit.h"
#include "../helpful/Serialization.h"
#include "../helpful/RoadKit.h"
#include "../helpful/RailroadClassifier.h"
#include "../helpful/Attic.h"
#include "XmlEqual.h"

using namespace std;

	/*
	//¬арианты возможной сериализации:
	// - все дороги с нац.кодом 21 кроме дороги с лок.кодом 9
	<root name = "optionalName">
	<National value="21" negative = "Y">
		<Local value = "9"/>
	</National>]
	</root>

	// - все дороги с нац.кодом 21
	<root name = "optionalName">
	<National value="25">
	</National>
	</root>

	// - дороги с нац.кодом 20 и лок.кодами 10 и 17
	<root name = "optionalName">
	<National value="20">
		<Local value = "10"/>
		<Local value = "17"/>
	</National>
	</root>

	// - все дороги кроме дороги с нац.кодом 20 и лок.кодом 10
	<root name = "optionalName" negative = "Y">
	<National value="20">
		<Local value = "10"/>
	</National>
	</root>

	// - все дороги, кроме дорог с нац.кодом 20 из которой исключены дороги с лок.кодами 10 и 17
	<root name = "optionalName" negative = "Y">
	<National value="20" negative = "Y">
		<Local value = "10"/>
		<Local value = "17"/>
	</National>
	</root>
	*/

CPPUNIT_TEST_SUITE_REGISTRATION( TC_RoadKit );

//без инверсии
const string anyRoadStr = "<RoadKit>"\
	"</RoadKit>";

const string rzdRoadStr = "<RoadKit>"\
	"<National value = \"20\"/>"\
	"</RoadKit>";

const string armenianRoadStr = "<RoadKit>"\
	"<National value = \"58\"/>"\
	"</RoadKit>";

const string armenianRoadStr2 = "<RoadKit>"\
	"<National value = \"58\">"\
		"<Local value = \"56\"/>"\
	"</National>"\
	"</RoadKit>";

const string kaliningradRoadStr = "<RoadKit name=\"user_klg_name\">"\
	"<National value = \"20\">"\
		"<Local value = \"10\"/>"\
	"</National>"\
	"</RoadKit>";

const string fewRzdRoadsStr = "<RoadKit name=\"few_road_set\">"\
	"<National value = \"20\">"\
		"<Local value = \"17\"/>"\
		"<Local value = \"24\"/>"\
	"</National>"\
	"</RoadKit>";

//с инверсией:
//вс€ составна€ дорога за исключением нескольких
const string manyRzdRoadsStr = "<RoadKit name=\"many_road_set\">"\
	"<National value = \"20\" negative = \"Y\">"\
		"<Local value = \"51\"/>"\
		"<Local value = \"58\"/>"\
	"</National>"\
	"</RoadKit>";

//все возможные дороги за исключением одной составной
const string allBesidesRzdStr = "<RoadKit name=\"user_name\" negative=\"Y\">"\
	"<National value = \"20\"/>"\
	"</RoadKit>";

//все возможные дороги за исключением нескольких составных
const string allBesidesSeveralNatStr = "<RoadKit name=\"user_name\" negative=\"Y\">"\
	"<National value = \"20\"/>"\
	"<National value = \"25\"/>"\
	"</RoadKit>";

//все возможные дороги за исключением одной поддороги определенной составной дороги
const string allBesidesOneLocalStr = "<RoadKit name=\"user_name\" negative=\"Y\">"\
	"<National value = \"20\">"\
		"<Local value = \"10\"/>"\
	"</National>"\
	"</RoadKit>";

//все возможные дороги за исключением нескольких поддорог разных составных дорог
const string allBesidesSeveralLocalsStr = "<RoadKit name=\"user_name\" negative=\"Y\">"\
	"<National value = \"20\">"\
		"<Local value = \"10\"/>"\
	"</National>"\
	"<National value = \"25\">"\
		"<Local value = \"1003\"/>"\
	"</National>"\
	"</RoadKit>";

//все возможные дороги и несколько поддорог разных составных дорог (проверка на двойное отрицание)
const string allBesidesManyLocalsStr = "<RoadKit name=\"user_name\" negative=\"Y\">"\
	"<National value = \"20\" negative=\"Y\">"\
		"<Local value = \"51\"/>"\
		"<Local value = \"58\"/>"\
	"</National>"\
	"<National value = \"25\" negative=\"Y\">"\
		"<Local value = \"1005\"/>"\
		"<Local value = \"1006\"/>"\
	"</National>"\
	"</RoadKit>";

//все возможные дороги за исключением нескольких дорог, описанных смешанных образом
const string allBesidesSeveralMixedStr = "<RoadKit name=\"user_name\" negative=\"Y\">"\
	"<National value = \"20\" negative=\"Y\">"\
		"<Local value = \"51\"/>"\
		"<Local value = \"58\"/>"\
	"</National>"\
	"<National value = \"25\">"\
		"<Local value = \"1003\"/>"\
		"<Local value = \"1005\"/>"\
	"</National>"\
	"</RoadKit>";

void TC_RoadKit::SerializeAny()
{
	RoadKit anyRoadKit;
	string serialStr = serialize( anyRoadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, anyRoadStr ) );
}

void TC_RoadKit::DeserializeAny()
{
	auto anyRoadKitPtr = deserialize<RoadKit>( anyRoadStr );
	CPPUNIT_ASSERT( anyRoadKitPtr && anyRoadKitPtr->Any() );
}

void TC_RoadKit::SerializeRichRoadFully()
{
	RoadKit rzdRoadKit( 20 );
	auto serialStr = serialize( rzdRoadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, rzdRoadStr ) );
}

void TC_RoadKit::DeserializeRichRoadFully()
{
	auto rzdRoadKitPtr = deserialize<RoadKit>( rzdRoadStr );
	CPPUNIT_ASSERT( rzdRoadKitPtr && !rzdRoadKitPtr->Any() );
}

void TC_RoadKit::SerializePoorRoad()
{
	RoadKit armenianRoadKit1( RoadCode( 58, 56 ) );
	RoadKit armenianRoadKit2( 58 );
	auto serialStr = serialize( armenianRoadKit1 );
	CPPUNIT_ASSERT( xmlEqual( serialStr, armenianRoadStr ) );
	serialStr = serialize( armenianRoadKit2 );
	CPPUNIT_ASSERT( xmlEqual( serialStr, armenianRoadStr ) );
}

void TC_RoadKit::DeserializePoorRoad()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto armRoadKitPtr1 = deserialize<RoadKit>( armenianRoadStr );
	CPPUNIT_ASSERT( armRoadKitPtr1 && !armRoadKitPtr1->Any() );
	auto armCodes = armRoadKitPtr1->AllCodes();
	set<RoadCode::Local> allArmLocalCodes = rrClassifier.getLocalCodes( RoadCode::armenianNationalCode );
	set<RoadCode> allArmCodes;
	for ( auto locCode : allArmLocalCodes )
		allArmCodes.insert( RoadCode( RoadCode::armenianNationalCode, locCode ) );
	CPPUNIT_ASSERT( armCodes == allArmCodes );
	auto armRoadKitPtr2 = deserialize<RoadKit>( armenianRoadStr );
	CPPUNIT_ASSERT( armRoadKitPtr2 );
	CPPUNIT_ASSERT( *armRoadKitPtr1 == *armRoadKitPtr2 );
}

void TC_RoadKit::SerializeRichRoadOneLocal()
{
	RoadKit kaliningradRoadKit( RoadCode( 20, 10 ), L"user_klg_name" );
	auto serialStr = serialize( kaliningradRoadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, kaliningradRoadStr ) );
}

void TC_RoadKit::DeserializeRichRoadOneLocal()
{
	auto klgRoadKitPtr = deserialize<RoadKit>( kaliningradRoadStr );
	CPPUNIT_ASSERT( klgRoadKitPtr && !klgRoadKitPtr->Any() );
	auto klgCodes = klgRoadKitPtr->AllCodes();
	CPPUNIT_ASSERT( klgCodes.size() == 1 && *klgCodes.cbegin() == RoadCode::kaliningrad );
	CPPUNIT_ASSERT( klgRoadKitPtr->GetName().first == L"user_klg_name" );
}

void TC_RoadKit::SerializeRichRoadSeveralLocals()
{
	set<RoadCode> codeset;
	codeset.insert( RoadCode( 20, 17 ) );
	codeset.insert( RoadCode( 20, 24 ) );
	RoadKit fewRzdRoadKit( codeset, L"few_road_set" );
	auto serialStr = serialize( fewRzdRoadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, fewRzdRoadsStr ) );
	CPPUNIT_ASSERT( fewRzdRoadKit.GetName().first == L"few_road_set" );
}

void TC_RoadKit::DeserializeRichRoadSeveralLocals()
{
	auto fewRzdRoadsKitPtr = deserialize<RoadKit>( fewRzdRoadsStr );
	CPPUNIT_ASSERT( fewRzdRoadsKitPtr && !fewRzdRoadsKitPtr->Any() );
	auto fewRzdRoadsCodes = fewRzdRoadsKitPtr->AllCodes();
	CPPUNIT_ASSERT( fewRzdRoadsCodes.size() == 2 );
	CPPUNIT_ASSERT( fewRzdRoadsCodes.find( RoadCode( 20, 17 ) ) != fewRzdRoadsCodes.cend() );
	CPPUNIT_ASSERT( fewRzdRoadsCodes.find( RoadCode( 20, 24 ) ) != fewRzdRoadsCodes.cend() );
}

void TC_RoadKit::SerializeRichRoadSeveralLocalsInverse()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	set<RoadCode::Local> manyLocalCodes = rrClassifier.getLocalCodes( 20 );
	manyLocalCodes.erase( 51 );
	manyLocalCodes.erase( 58 );
	set<RoadCode> codeset;
	for ( auto locCode : manyLocalCodes )
		codeset.insert( RoadCode( 20, locCode ) );
	RoadKit manyRzdRoadKit( codeset, L"many_road_set" );

	auto serialStr = serialize( manyRzdRoadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, manyRzdRoadsStr ) );
	CPPUNIT_ASSERT( manyRzdRoadKit.GetName().first == L"many_road_set" );
}

void TC_RoadKit::DeserializeRichRoadSeveralLocalsInverse()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	set<RoadCode::Local> allRzdLocalCodes = rrClassifier.getLocalCodes( RoadCode::rzdNationalCode );
	set<RoadCode> allRzdCodes;
	for ( auto locCode : allRzdLocalCodes )
		allRzdCodes.insert( RoadCode( RoadCode::rzdNationalCode, locCode ) );

	auto manyRzdRoadsKitPtr = deserialize<RoadKit>( manyRzdRoadsStr );
	auto manyRzdRoadsCodes = manyRzdRoadsKitPtr->AllCodes();
	CPPUNIT_ASSERT( manyRzdRoadsCodes.size() + 2 == allRzdLocalCodes.size() );
	set<RoadCode> forbiddenRzdCodes;
	set_difference( allRzdCodes.cbegin(), allRzdCodes.cend(), manyRzdRoadsCodes.cbegin(), manyRzdRoadsCodes.cend(),
		inserter( forbiddenRzdCodes, forbiddenRzdCodes.cend() ) );
	CPPUNIT_ASSERT( forbiddenRzdCodes.size() == 2 );
	CPPUNIT_ASSERT( forbiddenRzdCodes.find( RoadCode( 20, 51 ) ) != forbiddenRzdCodes.cend() );
	CPPUNIT_ASSERT( forbiddenRzdCodes.find( RoadCode( 20, 58 ) ) != forbiddenRzdCodes.cend() );
}

//все возможные дороги за исключением одной составной
void TC_RoadKit::SerializeInverseRichRoadFully()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	const auto & rzdLocalCodes = rrClassifier.getLocalCodes( RoadCode::rzdNationalCode );
	for ( const auto & localCode : rzdLocalCodes )
	{
		auto erasedNum = tstcodeset.erase( RoadCode( RoadCode::rzdNationalCode, localCode ) );
		CPPUNIT_ASSERT( erasedNum == 1 );
	}
	RoadKit roadKit( tstcodeset, L"user_name" );
	auto serialStr = serialize( roadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, allBesidesRzdStr ) );
	CPPUNIT_ASSERT( roadKit.GetName().first == L"user_name" );
}

void TC_RoadKit::DeserializeInverseRichRoadFully()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	const auto & rzdLocalCodes = rrClassifier.getLocalCodes( RoadCode::rzdNationalCode );
	for ( const auto & localCode : rzdLocalCodes )
	{
		auto erasedNum = tstcodeset.erase( RoadCode( RoadCode::rzdNationalCode, localCode ) );
		CPPUNIT_ASSERT( erasedNum == 1 );
	}
	auto roadKitPtr = deserialize<RoadKit>( allBesidesRzdStr );
	const auto & allRoadCodes = roadKitPtr->AllCodes();
	CPPUNIT_ASSERT( tstcodeset == allRoadCodes );
}

//все возможные дороги за исключением нескольких составных
void TC_RoadKit::SerializeInverseSeveralRichRoadFully()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	RoadCode::National removedNatCodes[] = { RoadCode::rzdNationalCode, RoadCode::latvianNationalCode };
	for ( auto natCode : removedNatCodes )
	{
		const auto & localCodes = rrClassifier.getLocalCodes( natCode );
		for ( const auto & localCode : localCodes )
		{
			auto erasedNum = tstcodeset.erase( RoadCode( natCode, localCode ) );
			CPPUNIT_ASSERT( erasedNum == 1 );
		}
	}

	RoadKit roadKit( tstcodeset, L"user_name" );
	auto serialStr = serialize( roadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, allBesidesSeveralNatStr ) );
	CPPUNIT_ASSERT( roadKit.GetName().first == L"user_name" );
}

void TC_RoadKit::DeserializeInverseSeveralRichRoadFully()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	RoadCode::National removedNatCodes[] = { RoadCode::rzdNationalCode, RoadCode::latvianNationalCode };
	for ( auto natCode : removedNatCodes )
	{
		const auto & localCodes = rrClassifier.getLocalCodes( natCode );
		for ( const auto & localCode : localCodes )
		{
			auto erasedNum = tstcodeset.erase( RoadCode( natCode, localCode ) );
			CPPUNIT_ASSERT( erasedNum == 1 );
		}
	}

	auto roadKitPtr = deserialize<RoadKit>( allBesidesSeveralNatStr );
	const auto & allRoadCodes = roadKitPtr->AllCodes();
	CPPUNIT_ASSERT( tstcodeset == allRoadCodes );
}

//все возможные дороги за исключением одной поддороги определенной составной дороги
void TC_RoadKit::SerializeInverseRichRoadOneLocal()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	auto erasedNum = tstcodeset.erase( RoadCode( RoadCode::rzdNationalCode, 10 ) );
	CPPUNIT_ASSERT( erasedNum == 1 );

	RoadKit roadKit( tstcodeset, L"user_name" );
	auto serialStr = serialize( roadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, allBesidesOneLocalStr ) );
	CPPUNIT_ASSERT( roadKit.GetName().first == L"user_name" );
}

void TC_RoadKit::DeserializeInverseRichRoadOneLocal()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	auto erasedNum = tstcodeset.erase( RoadCode( RoadCode::rzdNationalCode, 10 ) );
	CPPUNIT_ASSERT( erasedNum == 1 );

	auto roadKitPtr = deserialize<RoadKit>( allBesidesOneLocalStr );
	const auto & allRoadCodes = roadKitPtr->AllCodes();
	CPPUNIT_ASSERT( tstcodeset == allRoadCodes );
}

//все возможные дороги за исключением нескольких поддорог разных составных дорог
void TC_RoadKit::SerializeInverseSeveralRichRoadSeveralLocals()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	auto erasedNum = tstcodeset.erase( RoadCode( RoadCode::rzdNationalCode, 10 ) );
	CPPUNIT_ASSERT( erasedNum == 1 );
	erasedNum = tstcodeset.erase( RoadCode( RoadCode::latvianNationalCode, 1003 ) );
	CPPUNIT_ASSERT( erasedNum == 1 );

	RoadKit roadKit( tstcodeset, L"user_name" );
	auto serialStr = serialize( roadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, allBesidesSeveralLocalsStr ) );
	CPPUNIT_ASSERT( roadKit.GetName().first == L"user_name" );
}

void TC_RoadKit::DeserializeInverseSeveralRichRoadSeveralLocals()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	auto erasedNum = tstcodeset.erase( RoadCode( RoadCode::rzdNationalCode, 10 ) );
	CPPUNIT_ASSERT( erasedNum == 1 );
	erasedNum = tstcodeset.erase( RoadCode( RoadCode::latvianNationalCode, 1003 ) );
	CPPUNIT_ASSERT( erasedNum == 1 );

	auto roadKitPtr = deserialize<RoadKit>( allBesidesSeveralLocalsStr );
	const auto & allRoadCodes = roadKitPtr->AllCodes();
	CPPUNIT_ASSERT( tstcodeset == allRoadCodes );
}

//все возможные дороги и несколько поддорог разных составных дорог (проверка на двойное отрицание)
void TC_RoadKit::SerializeInverseSeveralRichRoadSeveralLocalsInverse()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	auto initsize = tstcodeset.size();
	for ( auto codeIt = tstcodeset.cbegin(); codeIt != tstcodeset.cend(); )
	{
		const RoadCode & roadCode = *codeIt;
		if ( roadCode.national == RoadCode::rzdNationalCode && roadCode.local != 51 && roadCode.local != 58  ||
			roadCode.national == RoadCode::latvianNationalCode && roadCode.local != 1005 && roadCode.local != 1006 )
			codeIt = tstcodeset.erase( codeIt );
		else
			++codeIt;
	}
	auto rzdLocalSize = rrClassifier.getLocalCodes( RoadCode::rzdNationalCode ).size();
	auto latLocalSize = rrClassifier.getLocalCodes( RoadCode::latvianNationalCode ).size();
	CPPUNIT_ASSERT( tstcodeset.size() + ( rzdLocalSize - 2 ) + ( latLocalSize - 2 ) == initsize );

	RoadKit roadKit( tstcodeset, L"user_name" );
	auto serialStr = serialize( roadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, allBesidesManyLocalsStr ) );
	CPPUNIT_ASSERT( roadKit.GetName().first == L"user_name" );
}

void TC_RoadKit::DeserializeInverseSeveralRichRoadSeveralLocalsInverse()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto tstcodeset = rrClassifier.getAllCodes();
	auto initsize = tstcodeset.size();
	auto allRzdLocalSize = rrClassifier.getLocalCodes( RoadCode::rzdNationalCode ).size();
	auto allLatLocalSize = rrClassifier.getLocalCodes( RoadCode::latvianNationalCode ).size();
	for ( auto codeIt = tstcodeset.cbegin(); codeIt != tstcodeset.cend(); )
	{
		const RoadCode & roadCode = *codeIt;
		if ( roadCode.national == RoadCode::rzdNationalCode && roadCode.local != 51 && roadCode.local != 58  ||
			roadCode.national == RoadCode::latvianNationalCode && roadCode.local != 1005 && roadCode.local != 1006 )
			codeIt = tstcodeset.erase( codeIt );
		else
			++codeIt;
	}
	CPPUNIT_ASSERT( tstcodeset.size() + allRzdLocalSize - 2 + allLatLocalSize - 2  == initsize );
	
	auto roadKitPtr = deserialize<RoadKit>( allBesidesManyLocalsStr );
	const auto & allRoadCodes = roadKitPtr->AllCodes();
	CPPUNIT_ASSERT( tstcodeset == allRoadCodes );
}

//все возможные дороги за исключением нескольких дорог, описанных смешанных образом
void TC_RoadKit::SerializeInverseSeveralRichRoadMixedLocals()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto allRzdCodesSize = rrClassifier.getLocalCodes( RoadCode::rzdNationalCode ).size();
	auto tstcodeset = rrClassifier.getAllCodes();
	auto initsize = tstcodeset.size();
	for ( auto codeIt = tstcodeset.cbegin(); codeIt != tstcodeset.cend(); )
	{
		const RoadCode & roadCode = *codeIt;
		if ( roadCode.national == RoadCode::rzdNationalCode && roadCode.local != 51 && roadCode.local != 58  ||
			roadCode.national == RoadCode::latvianNationalCode && ( roadCode.local == 1003 || roadCode.local == 1005 ) )
			codeIt = tstcodeset.erase( codeIt );
		else
			++codeIt;
	}
	CPPUNIT_ASSERT( tstcodeset.size() + 2 + ( allRzdCodesSize - 2 ) == initsize ); //д.б. оставлено две дороги от –∆ƒ и удалено две от Ћатвии

	RoadKit roadKit( tstcodeset, L"user_name" );
	auto serialStr = serialize( roadKit );
	CPPUNIT_ASSERT( xmlEqual( serialStr, allBesidesSeveralMixedStr ) );
	CPPUNIT_ASSERT( roadKit.GetName().first == L"user_name" );
}

void TC_RoadKit::DeserializeInverseSeveralRichRoadMixedLocals()
{
	const auto & rrClassifier = RailroadClassifier::getInstance();
	auto allRzdCodesSize = rrClassifier.getLocalCodes( RoadCode::rzdNationalCode ).size();
	auto tstcodeset = rrClassifier.getAllCodes();
	auto initsize = tstcodeset.size();
	for ( auto codeIt = tstcodeset.cbegin(); codeIt != tstcodeset.cend(); )
	{
		const RoadCode & roadCode = *codeIt;
		if ( roadCode.national == RoadCode::rzdNationalCode && roadCode.local != 51 && roadCode.local != 58  ||
			roadCode.national == RoadCode::latvianNationalCode && ( roadCode.local == 1003 || roadCode.local == 1005 ) )
			codeIt = tstcodeset.erase( codeIt );
		else
			++codeIt;
	}
	CPPUNIT_ASSERT( tstcodeset.size() + 2 + ( allRzdCodesSize - 2 ) == initsize ); //д.б. оставлено две дороги от –∆ƒ и удалено две от Ћатвии

	auto roadKitPtr = deserialize<RoadKit>( allBesidesSeveralMixedStr );
	const auto & allRoadCodes = roadKitPtr->AllCodes();
	CPPUNIT_ASSERT( tstcodeset == allRoadCodes );
}
