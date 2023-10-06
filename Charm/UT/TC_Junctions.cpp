#include "stdafx.h"
#include "TC_Junctions.h"
#include "../helpful/Serialization.h"
#include "../helpful/Junctions.h"
#include "XmlEqual.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Junctions );

static const string junctionsTestStr = 
	"<Junctions>\n"\
		"<Technodes>"\
			"<Union>\n"\
				"<Station esrCode=\"10220\"/>\n"\
				"<Station esrCode=\"23560\"/>\n"\
				"<Station esrCode=\"47150\"/>\n"\
			"</Union>\n"\
			"<Union ignoreOddness=\"Y\">\n"\
				"<Station esrCode=\"38400\"/>\n"\
				"<Station esrCode=\"65120\"/>\n"\
			"</Union>\n"\
		"</Technodes>\n"\
		"<EqualAreas>"\
			"<Union>\n"\
				"<Station esrCode=\"7120\"/>\n"\
				"<Station esrCode=\"19240\"/>\n"\
				"<Station esrCode=\"37555\"/>\n"\
			"</Union>\n"\
			"<Union>\n"\
				"<Station esrCode=\"13444\"/>\n"\
				"<Station esrCode=\"29101\"/>\n"\
				"<Station esrCode=\"44763\"/>\n"\
				"<Station esrCode=\"77190\"/>\n"\
			"</Union>\n"\
		"</EqualAreas>\n"\
	"</Junctions>\n";

void TC_Junctions::CheckTechnodesAfterDeserialize()
{
	string serialStr = junctionsTestStr;
	auto junctionsPtr = deserialize<Junctions>( serialStr );
	CPPUNIT_ASSERT( junctionsPtr );
	CPPUNIT_ASSERT( junctionsPtr->sameTechnode( EsrKit( 23560 ), EsrKit( 47150 ) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameTechnode( EsrKit( 23560 ), EsrKit( 10220 ) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameTechnode( EsrKit( 47150 ), EsrKit( 10220 ) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameTechnode( EsrKit( 65120 ), EsrKit( 38400 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameTechnode( EsrKit( 23560 ), EsrKit( 65120 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameTechnode( EsrKit( 23560 ), EsrKit( 38400 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameTechnode( EsrKit( 47150 ), EsrKit( 65120 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameTechnode( EsrKit( 47150 ), EsrKit( 38400 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameTechnode( EsrKit( 10220 ), EsrKit( 65120 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameTechnode( EsrKit( 10220 ), EsrKit( 38400 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameTechnode( EsrKit( 23560 ), EsrKit( 88888 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameTechnode( EsrKit( 99999 ), EsrKit( 65120 ) ) );
}

void TC_Junctions::CheckOddnessAfterDeserialize()
{
	string serialStr = junctionsTestStr;
	auto junctionsPtr = deserialize<Junctions>( serialStr );
	CPPUNIT_ASSERT( junctionsPtr );
	CPPUNIT_ASSERT( junctionsPtr->oddnessIgnore( EsrKit( 65120 ) ) );
	CPPUNIT_ASSERT( junctionsPtr->oddnessIgnore( EsrKit( 38400 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->oddnessIgnore( EsrKit( 23560 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->oddnessIgnore( EsrKit( 47150 ) ) );
	CPPUNIT_ASSERT( !junctionsPtr->oddnessIgnore( EsrKit( 10220 ) ) );
}

void TC_Junctions::CheckEqualAreasAfterDeserialize()
{
	string serialStr = junctionsTestStr;
	auto junctionsPtr = deserialize<Junctions>( serialStr );
	CPPUNIT_ASSERT( junctionsPtr );
	CPPUNIT_ASSERT( junctionsPtr->sameArea( StationEsr(7120), StationEsr(19240) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameArea( StationEsr(7120), StationEsr(37555) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameArea( StationEsr(19240), StationEsr(37555) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameArea( StationEsr(13444), StationEsr(29101) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameArea( StationEsr(29101), StationEsr(77190) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameArea( StationEsr(44763), StationEsr(13444) ) );
	CPPUNIT_ASSERT( junctionsPtr->sameArea( StationEsr(77190), StationEsr(44763) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameArea( StationEsr(7120), StationEsr(13444) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameArea( StationEsr(19240), StationEsr(44763) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameArea( StationEsr(37555), StationEsr(77190) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameArea( StationEsr(13444), StationEsr(37555) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameArea( StationEsr(29101), StationEsr(19240) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameArea( StationEsr(44763), StationEsr(7120) ) );
	CPPUNIT_ASSERT( !junctionsPtr->sameArea( StationEsr(77190), StationEsr(37555) ) );
}

void TC_Junctions::Serialize()
{
	vector<Junctions::StationUnion> technodes;
	Junctions::StationUnion technodeUnion1( false );
	technodeUnion1.codes.insert( EsrKit( 23560 ) );
	technodeUnion1.codes.insert( EsrKit( 47150 ) );
	technodeUnion1.codes.insert( EsrKit( 10220 ) );
	technodes.push_back( technodeUnion1 );

	Junctions::StationUnion technodeUnion2( true );
	technodeUnion2.codes.insert( EsrKit( 65120 ) );
	technodeUnion2.codes.insert( EsrKit( 38400 ) );
	technodes.push_back( technodeUnion2 );

	vector<Junctions::StationCodes> equalAreas;
	Junctions::StationCodes areaCodeset1;
	areaCodeset1.insert( StationEsr(7120) );
	areaCodeset1.insert( StationEsr(19240) );
	areaCodeset1.insert( StationEsr(37555) );
	equalAreas.push_back( areaCodeset1 );

	Junctions::StationCodes areaCodeset2;
	areaCodeset2.insert( StationEsr(13444) );
	areaCodeset2.insert( StationEsr(29101) );
	areaCodeset2.insert( StationEsr(44763) );
	areaCodeset2.insert( StationEsr(77190) );
	equalAreas.push_back( areaCodeset2 );

	Junctions junctions( technodes, equalAreas );
	string serialStr = serialize( junctions );
	auto mismRes = mismatch( serialStr.cbegin(), serialStr.cend(), junctionsTestStr.cbegin(), junctionsTestStr.cend() );
	CPPUNIT_ASSERT( xmlEqual( serialStr, junctionsTestStr ) );
}