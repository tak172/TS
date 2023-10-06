#include "stdafx.h"

#include <sstream>

#include "TC_TopologyBlacklist.h"
#include "../helpful/TopologyBlacklist.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(TC_TopologyBlacklist);

TC_TopologyBlacklist::TC_TopologyBlacklist()
{
	for ( auto i = 1; i <= 10; ++i )
		nodeCodes.insert( StationEsr(i * 100) );
}

void TC_TopologyBlacklist::Infix()
{
	const std::string data = "// This is a comment line\r\n"
		"		# This is comment too\n  "
		"	500..600..700\n"
		"300..400..500..800\n"
		"100..900\n";

	std::istringstream stream( data );
	TopologyBlacklist topologyBlacklist( stream, nodeCodes );
	int arr1[] = { 409, 410, 500, 200, 501, 502, 600, 601, 700 };
	CPPUNIT_ASSERT( topologyBlacklist.RouteIsDesired( list<EsrKit>( begin( arr1 ), end( arr1 ) ) ) ); //проходит первое правило
	int arr2[] = { 405, 500, 501, 502, 600, 601, 700, 703 };
	CPPUNIT_ASSERT( !topologyBlacklist.RouteIsDesired( list<EsrKit>( begin( arr2 ), end( arr2 ) ) ) ); //не проходит по первому правилу
	int arr3[] = { 255, 300, 301, 302, 1000, 303, 304, 400, 401, 500, 501, 800 };
	CPPUNIT_ASSERT( topologyBlacklist.RouteIsDesired( list<EsrKit>( begin( arr3 ), end( arr3 ) ) ) ); //проходит второе правило
	int arr4[] = { 300, 301, 302, 400, 401, 500, 501, 800, 810 };
	CPPUNIT_ASSERT( !topologyBlacklist.RouteIsDesired( list<EsrKit>( begin( arr4 ), end( arr4 ) ) ) ); //не проходит по второму правилу
	int arr5[] = { 100, 101, 700, 102, 900, 901, 902, 903 };
	CPPUNIT_ASSERT( topologyBlacklist.RouteIsDesired( list<EsrKit>( begin( arr5 ), end( arr5 ) ) ) ); //проходит третье правило
	int arr6[] = { 100, 101, 102, 103, 900, 901, 902, 903 };
	CPPUNIT_ASSERT( !topologyBlacklist.RouteIsDesired( list<EsrKit>( begin( arr6 ), end( arr6 ) ) ) ); //не проходит третье правило
	int arr7[] = { 100, 101, 102, 200, 201, 205, 300, 302, 304, 400, 405, 406, 500, 800, 900, };
	CPPUNIT_ASSERT( !topologyBlacklist.RouteIsDesired( list<EsrKit>( begin( arr7 ), end( arr7 ) ) ) ); //не проходит второе правило
}