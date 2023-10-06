#include "stdafx.h"
#include "TC_RouteIntrusionKeeper.h"
#include "../helpful/TrainDescr.h"
#include "../Hem/RouteIntrusionKeeper.h"
#include "../helpful/Serialization.h"
#include "XmlEqual.h"
#include "../Hem/RouteIntrusion.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_RouteIntrusionKeeper );

static const string intrusTestStr = "<ArtificalRouteIntrusions>"\
		"<RouteIntrusion>"\
			"<TrainDescr index=\"0001-002-0003\" num=\"7004\" suburbfast=\"Y\"/>"\
			"<Station esrCode=\"09326\">"\
				"<OrderPriority>"\
					"<YieldFor>"\
						"<TrainDescr index=\"0044-055-0066\" num=\"D3992\" docwag=\"Y\"/>"\
					"</YieldFor>"\
				"</OrderPriority>"\
			"</Station>"\
			"<Station esrCode=\"09420\">"\
				"<OrderPriority/>"\
			"</Station>"\
		"</RouteIntrusion>"\
		"<RouteIntrusion>"\
			"<TrainDescr index=\"0003-002-0001\" num=\"7005\" suburbfast=\"Y\"/>"\
			"<Station esrCode=\"12680\">"\
				"<TrainInfoAlteration>"\
					"<TrainDescr index=\"\" num=\"D2055\" through=\"Y\"/>"\
				"</TrainInfoAlteration>"\
			"</Station>"\
			"<TransitThrough>"\
				"<Station esrCode=\"07203\"/>"\
				"<Station esrCode=\"07142\"/>"\
				"<Station esrCode=\"07362\"/>"\
			"</TransitThrough>"\
		"</RouteIntrusion>"\
	"</ArtificalRouteIntrusions>";

void TC_RouteIntrusionKeeper::Serialize()
{
	TrainDescr tdescr1( L"7004", L"1-2-3" );
	RouteIntrusionKeeper riKeeper;
	riKeeper.setPassingAfter( tdescr1, EsrKit( 9326 ), TrainDescr( L"3344", L"77-22-77" ) );
	riKeeper.setPassingAfter( tdescr1, EsrKit( 9326 ), TrainDescr( L"D3992", L"44-55-66" ) ); //перезапись предыдущей
	riKeeper.setPassingAfter( tdescr1, EsrKit( 10580 ), TrainDescr( L"S8811", L"999-1-777" ) );
	riKeeper.discardPassingOrder( tdescr1, EsrKit( 10580 ) ); //удаление существующего ограничения
	riKeeper.setPriorPassing( tdescr1, EsrKit( 9420 ) );

	TrainDescr tdescr2( L"7005", L"3-2-1" );
	riKeeper.alterateInfo( tdescr2, EsrKit( 12680 ), TrainDescr( L"D2055" ) );
	EsrKit transCodes[] = { EsrKit( 7203 ), EsrKit( 7142 ), EsrKit( 7362 ) };
	riKeeper.shouldTransferAcross( tdescr2, vector<EsrKit>( begin( transCodes ), end( transCodes ) ) );
	auto serialStr = serialize<RouteIntrusionKeeper>( riKeeper );
	CPPUNIT_ASSERT( xmlEqual( serialStr, intrusTestStr ) );
}

void TC_RouteIntrusionKeeper::Deserialize()
{
	auto rikeeperPtr = deserialize<RouteIntrusionKeeper>( intrusTestStr );
	CPPUNIT_ASSERT( rikeeperPtr );
	const auto & rintrusions = rikeeperPtr->getIntrusions();
	CPPUNIT_ASSERT( rintrusions.size() == 2 );
	auto riIt1 = find_if( rintrusions.cbegin(), rintrusions.cend(), []( const RouteIntrusion & ri ){
		return ri.getTrainDescr() == TrainDescr( L"7004", L"1-2-3" );
	} );
	CPPUNIT_ASSERT( riIt1 != rintrusions.cend() );
	const auto & rintr1 = *riIt1;
	const auto & sintrus1 = rintr1.getStationIntrusions();
	CPPUNIT_ASSERT( sintrus1.size() == 2 );
	CPPUNIT_ASSERT( sintrus1.find( EsrKit( 9326 ) ) != sintrus1.cend() );
	CPPUNIT_ASSERT( sintrus1.find( EsrKit( 9420 ) ) != sintrus1.cend() );
	const auto & yieldForPtr = rintr1.shouldYieldForOther( EsrKit( 9326 ) );
	CPPUNIT_ASSERT( yieldForPtr && *yieldForPtr == TrainDescr( L"D3992", L"44-55-66" ) );
	CPPUNIT_ASSERT( !rintr1.shouldYieldForOther( EsrKit( 10580 ) ) );
	CPPUNIT_ASSERT( !rintr1.shouldYieldForOther( EsrKit( 9420 ) ) );
	CPPUNIT_ASSERT( rintr1.dominantOrderOn( EsrKit( 9420 ) ) );
	CPPUNIT_ASSERT( rintr1.getTransitStations().empty() );

	auto riIt2 = find_if( rintrusions.cbegin(), rintrusions.cend(), []( const RouteIntrusion & ri ){
		return ri.getTrainDescr() == TrainDescr( L"7005", L"3-2-1" );
	} );
	CPPUNIT_ASSERT( riIt2 != rintrusions.cend() );
	const auto & rintr2 = *riIt2;
	const auto & sintrus2 = rintr2.getStationIntrusions();
	CPPUNIT_ASSERT( sintrus2.size() == 1 );
	CPPUNIT_ASSERT( sintrus2.cbegin()->first == EsrKit( 12680 ) );
	auto tchangePtr = rintr2.getInfoChange( EsrKit( 12680 ) );
	CPPUNIT_ASSERT( tchangePtr && *tchangePtr == TrainDescr( L"D2055" ) );
	const auto & transCodes = rintr2.getTransitStations();
	CPPUNIT_ASSERT( transCodes.size() == 3 );
	CPPUNIT_ASSERT( transCodes[0] == EsrKit( 7203 ) );
	CPPUNIT_ASSERT( transCodes[1] == EsrKit( 7142 ) );
	CPPUNIT_ASSERT( transCodes[2] == EsrKit( 7362 ) );
	CPPUNIT_ASSERT( !rintr2.dominantOrderOn( EsrKit( 12680 ) ) );
	CPPUNIT_ASSERT( !rintr2.shouldYieldForOther( EsrKit( 12680 ) ) );
}
