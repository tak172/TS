#include "stdafx.h"
#include "TC_SpansRegistry.h"
#include "../helpful/Serialization.h"
#include "../helpful/SpansRegistry.h"
#include "XmlEqual.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SpansRegistry );

static const string spanRegistryTestStr = 
	"<SpansRegistry>\n  "\
		"<Span EsrCode=\"23000:23057\" incoddness=\"odd\" crossesNum=\"8\">\n"\
			"    <SpanWayInfo wayNum=\"2\" length=\"8.52\" spanLock=\"3\" parity=\"even\" internSemas=\"Y\" asymmSema=\"Y\" />\n"\
			"    <SpanWayInfo wayNum=\"3\" length=\"17.23\" spanLock=\"5\" parity=\"odd\" internSemas=\"\" asymmSema=\"\" />\n  "\
		"</Span>\n  "\
		"<Span EsrCode=\"33214:41220\" incoddness=\"even\" crossesNum=\"0\">\n"\
			"    <SpanWayInfo wayNum=\"1\" length=\"3.99\" spanLock=\"0\" parity=\"any\" internSemas=\"\" asymmSema=\"\" />\n"\
			"    <SpanWayInfo wayNum=\"2\" length=\"2\" spanLock=\"1\" parity=\"odd\" internSemas=\"\" asymmSema=\"Y\" />\n"\
			"    <SpanWayInfo wayNum=\"4\" length=\"5.41\" spanLock=\"4\" parity=\"even\" internSemas=\"Y\" asymmSema=\"\" />\n  "\
		"</Span>\n"\
	"</SpansRegistry>\n";

void TC_SpansRegistry::Serialize()
{
	map <EsrKit, SpansRegistry::SpanInfo> spanInfos;

	SpansRegistry::SpanInfo spanInfo1;
	spanInfo1.spanWays.emplace_back( new SpanWayInfo( 2, SpanLockType::SEMI_AUTOBLOCK, 8.52, PERPARITY::EVEN_PARITY, true, true ) );
	spanInfo1.spanWays.emplace_back( new SpanWayInfo( 3, SpanLockType::TELEPHONE_CONNECTION, 17.23, PERPARITY::ODD_PARITY, false ) );
	spanInfo1.incOddness = Oddness::ODD;
	spanInfo1.crossesNum = 8;
	spanInfos.insert( make_pair( EsrKit( 23000, 23057 ), spanInfo1 ) );

	SpansRegistry::SpanInfo spanInfo2;
	spanInfo2.spanWays.emplace_back( new SpanWayInfo( 1, SpanLockType::BIDIR_AUTOBLOCK, 3.99, PERPARITY::ANY_PARITY, false ) );
	spanInfo2.spanWays.emplace_back( new SpanWayInfo( 2, SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS, 2, PERPARITY::ODD_PARITY, false, true ) );
	spanInfo2.spanWays.emplace_back( new SpanWayInfo( 4, SpanLockType::ELECTRIC_TOKEN, 5.41, PERPARITY::EVEN_PARITY, true ) );
	spanInfo2.incOddness = Oddness::EVEN;
	spanInfos.insert( make_pair( EsrKit( 33214, 41220 ), spanInfo2 ) );

	SpansRegistry spansRegistry( spanInfos );
	string serialStr = serialize( spansRegistry );
	//auto mismRes = mismatch( serialStr.cbegin(), serialStr.cend(), spanRegistryTestStr.cbegin(), spanRegistryTestStr.cend() );
	CPPUNIT_ASSERT( xmlEqual( serialStr, spanRegistryTestStr ) );
}

void TC_SpansRegistry::Deserialize()
{
	string serialStr = spanRegistryTestStr;
	auto spansRegistryPtr = deserialize<SpansRegistry>( serialStr );
	CPPUNIT_ASSERT( spansRegistryPtr );
	const SpansRegistry & spansRegistry = *spansRegistryPtr;
	const auto & ecodes = spansRegistry.getEsrCodes();
	CPPUNIT_ASSERT( ecodes.size() == 2 );
	CPPUNIT_ASSERT( ecodes.find( EsrKit( 23000, 23057 ) ) != ecodes.cend() );
	CPPUNIT_ASSERT( ecodes.find( EsrKit( 33214, 41220 ) ) != ecodes.cend() );

	const auto & spanWays1 = spansRegistry.getWays( EsrKit( 23000, 23057 ) );
	CPPUNIT_ASSERT( spanWays1.size() == 2 );
	CPPUNIT_ASSERT( find_if( spanWays1.cbegin(), spanWays1.cend(), []( SpanWayInfoCPtr spanWayPtr ){ return spanWayPtr->Num() == 2; } ) != spanWays1.cend() );
	CPPUNIT_ASSERT( find_if( spanWays1.cbegin(), spanWays1.cend(), []( SpanWayInfoCPtr spanWayPtr ){ return spanWayPtr->Num() == 3; } ) != spanWays1.cend() );
	for( const auto & spanWayPtr : spanWays1 )
	{
		if ( spanWayPtr->Num() == 2 )
		{
			CPPUNIT_ASSERT( spanWayPtr->getLock() == SpanLockType::SEMI_AUTOBLOCK );
			CPPUNIT_ASSERT( spanWayPtr->Length() == 8.52 );
			CPPUNIT_ASSERT( spanWayPtr->getParity() == PERPARITY::EVEN_PARITY );
			CPPUNIT_ASSERT( spanWayPtr->haveInternalSemas() );
			CPPUNIT_ASSERT( spanWayPtr->haveAsymmetricSema() );
		}
		else
		{
			CPPUNIT_ASSERT( spanWayPtr->Num() == 3 );
			CPPUNIT_ASSERT( spanWayPtr->getLock() == SpanLockType::TELEPHONE_CONNECTION );
			CPPUNIT_ASSERT( spanWayPtr->Length() == 17.23 );
			CPPUNIT_ASSERT( spanWayPtr->getParity() == PERPARITY::ODD_PARITY );
			CPPUNIT_ASSERT( !spanWayPtr->haveInternalSemas() );
			CPPUNIT_ASSERT( !spanWayPtr->haveAsymmetricSema() );
		}
	}
	CPPUNIT_ASSERT( spansRegistry.getOddness( StationEsr(23000), StationEsr(23057) ) == Oddness::ODD );
	CPPUNIT_ASSERT( spansRegistry.getOddness( StationEsr(23057), StationEsr(23000) ) == Oddness::EVEN );
	CPPUNIT_ASSERT( spansRegistry.crossesNum( EsrKit( 23000, 23057 ) ) == 8 );

	const auto & spanWays2 = spansRegistry.getWays( EsrKit( 33214, 41220 ) );
	CPPUNIT_ASSERT( spanWays2.size() == 3 );
	CPPUNIT_ASSERT( find_if( spanWays2.cbegin(), spanWays2.cend(), []( SpanWayInfoCPtr spanWayPtr ){ return spanWayPtr->Num() == 1; } ) != spanWays2.cend() );
	CPPUNIT_ASSERT( find_if( spanWays2.cbegin(), spanWays2.cend(), []( SpanWayInfoCPtr spanWayPtr ){ return spanWayPtr->Num() == 4; } ) != spanWays2.cend() );
	for( const auto & spanWayPtr : spanWays2 )
	{
		if ( spanWayPtr->Num() == 1 )
		{
			CPPUNIT_ASSERT( spanWayPtr->getLock() == SpanLockType::BIDIR_AUTOBLOCK );
			CPPUNIT_ASSERT( spanWayPtr->Length() == 3.99 );
			CPPUNIT_ASSERT( spanWayPtr->getParity() == PERPARITY::ANY_PARITY );
			CPPUNIT_ASSERT( !spanWayPtr->haveInternalSemas() );
			CPPUNIT_ASSERT( !spanWayPtr->haveAsymmetricSema() );
		}
		else if ( spanWayPtr->Num() == 2 )
		{
			CPPUNIT_ASSERT( spanWayPtr->getLock() == SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS );
			CPPUNIT_ASSERT( spanWayPtr->Length() == 2 );
			CPPUNIT_ASSERT( spanWayPtr->getParity() == PERPARITY::ODD_PARITY );
			CPPUNIT_ASSERT( !spanWayPtr->haveInternalSemas() );
			CPPUNIT_ASSERT( spanWayPtr->haveAsymmetricSema() );
		}
		else
		{
			CPPUNIT_ASSERT( spanWayPtr->Num() == 4 );
			CPPUNIT_ASSERT( spanWayPtr->getLock() == SpanLockType::ELECTRIC_TOKEN );
			CPPUNIT_ASSERT( spanWayPtr->Length() == 5.41 );
			CPPUNIT_ASSERT( spanWayPtr->getParity() == PERPARITY::EVEN_PARITY );
			CPPUNIT_ASSERT( spanWayPtr->haveInternalSemas() );
			CPPUNIT_ASSERT( !spanWayPtr->haveAsymmetricSema() );
		}
	}
	CPPUNIT_ASSERT( spansRegistry.getOddness( StationEsr(33214), StationEsr(41220) ) == Oddness::EVEN );
	CPPUNIT_ASSERT( spansRegistry.getOddness( StationEsr(41220), StationEsr(33214) ) == Oddness::ODD );
	CPPUNIT_ASSERT( spansRegistry.crossesNum( EsrKit( 33214, 33214 ) ) == 0 );
}