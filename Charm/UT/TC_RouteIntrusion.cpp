#include "stdafx.h"
#include "TC_RouteIntrusion.h"
#include "../helpful/Serialization.h"
#include "../Hem/RouteIntrusion.h"
#include "XmlEqual.h"

using namespace std;
namespace bt = boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_RouteIntrusion );

typedef RouteIntrusion::TimeImpact TimeImpact;
typedef RouteIntrusion::TimeImpact::TimePoint TimePoint;

static const string intrusTestStr =
    "<ArtificalRouteIntrusions>"
		"<RouteIntrusion>"
			"<TrainDescr index='0001-002-0003' num='7004' suburbfast='Y'/>"
			"<Station esrCode='09111'>"
				"<TimeImpact minstaying='660'/>"
			"</Station>"
			"<Station esrCode='09222'>"
				"<OrderPriority>"
					"<YieldFor>"
						"<TrainDescr index='0044-055-0066' num='D3992' docwag='Y'/>"
					"</YieldFor>"
				"</OrderPriority>"
			"</Station>"
			"<Station esrCode='09333'>"
				"<OrderPriority/>"
				"<TimeImpact mindeparture='20170813T062710Z'/>"
			"</Station>"
		"</RouteIntrusion>"
		"<RouteIntrusion>"
			"<TrainDescr index='0003-002-0001' num='7005' suburbfast='Y'/>"
			"<Station esrCode='08111'>"
				"<TimeImpact minstaying='660'/>"
			"</Station>"
			"<Station esrCode='08222'>"
				"<TimeImpact mindeparture='20170822T162839Z' minstaying='1320'/>"
			"</Station>"
			"<Station esrCode='08333'>"
				"<TimeImpact minstaying='180' />"
			"</Station>"
			"<Station esrCode='08444'>"
				"<TimeImpact mindeparture='20171026T024152Z' minstaying='240' />"
			"</Station>"
			"<Station esrCode='08555'>"
				"<TrainInfoAlteration>"
					"<TrainDescr index='' num='D2055' through='Y'/>"
				"</TrainInfoAlteration>"
			"</Station>"
			"<TransitThrough>"
				"<Station esrCode='07011'/>"
				"<Station esrCode='07022'/>"
				"<Station esrCode='07033'/>"
			"</TransitThrough>"
		"</RouteIntrusion>"
	"</ArtificalRouteIntrusions>";

typedef RouteIntrusion::StationIntrusion StationIntrusion;
typedef RouteIntrusion::PassingOrder PassingOrder;
typedef RouteIntrusion::PassingOrderPtr PassingOrderPtr;
void TC_RouteIntrusion::Serialize()
{
	vector<RouteIntrusion> routeIntrusions;

	//нить 7004
	map<EsrKit, StationIntrusion> statIntrusions7004;
	statIntrusions7004.insert( make_pair( EsrKit( 9111 ), 
		StationIntrusion( nullptr, nullptr, TimeImpact().Stay( bt::minutes( 11 ) ) ) ) );
	PassingOrderPtr passingOrderPtr( new PassingOrder( TrainDescrPtr( new TrainDescr( L"D3992", L"44-55-66" ) ) ) );
	statIntrusions7004.insert( make_pair( EsrKit( 9222 ), StationIntrusion( passingOrderPtr ) ) );
	statIntrusions7004.insert( make_pair( EsrKit( 9333 ), 
		StationIntrusion( PassingOrderPtr( new PassingOrder( nullptr ) ), nullptr, TimeImpact().Departure( TimeImpact::toTimePoint( "20170813T062710" ) ) ) ) );
	routeIntrusions.emplace_back( RouteIntrusion( TrainDescr( L"7004", L"0001-002-0003" ), vector<EsrKit>(), statIntrusions7004, map<EsrKit, RouteIntrusion::SpanIntrusion>() ) );

	//нить 7005
	map<EsrKit, StationIntrusion> statIntrusions7005;
	statIntrusions7005[ EsrKit( 8111 ) ] = 
		StationIntrusion( nullptr, nullptr, TimeImpact().Stay( bt::minutes( 11 ) ) );

	statIntrusions7005[ EsrKit( 8222 ) ] = 
		StationIntrusion( nullptr, nullptr, TimeImpact().Departure( TimeImpact::toTimePoint( "20170822T162839" ) ).Stay( bt::minutes( 22 ) ) );

	statIntrusions7005[ EsrKit( 8333 ) ] = 
		StationIntrusion( nullptr, nullptr, TimeImpact().Stay( bt::minutes( 3 ) ) );

	statIntrusions7005[ EsrKit( 8444 ) ] = 
		StationIntrusion( nullptr, nullptr, 
			TimeImpact()
            .Departure( TimeImpact::toTimePoint( "20171026T024152" ) )
            .Stay( bt::minutes( 4 ) ) );

	vector<EsrKit> visitingChain;
	visitingChain.emplace_back( EsrKit( 7011 ) );
	visitingChain.emplace_back( EsrKit( 7022 ) );
	visitingChain.emplace_back( EsrKit( 7033 ) );
	statIntrusions7005[ EsrKit( 8555 ) ] = StationIntrusion( nullptr, TrainDescrPtr( new TrainDescr( L"D2055" ) ) );
	routeIntrusions.emplace_back( RouteIntrusion( TrainDescr( L"7005", L"0003-002-0001" ), visitingChain, statIntrusions7005, map<EsrKit, RouteIntrusion::SpanIntrusion>() ) );

	string serialStr = serialize( routeIntrusions );
	CPPUNIT_ASSERT( xmlEqual( serialStr, intrusTestStr ) );
}

void TC_RouteIntrusion::Deserialize()
{
	auto routeIntensionsPtr = deserialize<vector<RouteIntrusion>>( intrusTestStr );
	CPPUNIT_ASSERT( routeIntensionsPtr );
	const auto & routeIntensions = *routeIntensionsPtr;
	CPPUNIT_ASSERT( routeIntensions.size() == 2 );
	const auto & rtInt1 = routeIntensions[0];
	const auto & tdescr1 = rtInt1.getTrainDescr();
	CPPUNIT_ASSERT( tdescr1.GetNumber().getNumber() == 7004 && tdescr1.GetIndex().str() == L"0001-002-0003" );

	auto impact91 = rtInt1.getTimeImpact( EsrKit( 9111 ) );
    CPPUNIT_ASSERT( !impact91.isEmpty() );
    CPPUNIT_ASSERT( !impact91.restrictDeparture() );
    CPPUNIT_ASSERT( impact91.Stay() == bt::minutes( 11 ) );
	CPPUNIT_ASSERT( !rtInt1.getInfoChange( EsrKit( 9111 ) ) );
	CPPUNIT_ASSERT( !rtInt1.dominantOrderOn( EsrKit( 9111 ) ) );
	CPPUNIT_ASSERT( !rtInt1.shouldYieldForOther( EsrKit( 9111 ) ) );

	auto impact92 = rtInt1.getTimeImpact( EsrKit( 9222 ) );
	CPPUNIT_ASSERT( impact92.isEmpty() );
	CPPUNIT_ASSERT( !rtInt1.getInfoChange( EsrKit( 9222 ) ) );
	CPPUNIT_ASSERT( !rtInt1.dominantOrderOn( EsrKit( 9222 ) ) );
	auto afterTrainPtr = rtInt1.shouldYieldForOther( EsrKit( 9222 ) );
	CPPUNIT_ASSERT( afterTrainPtr && *afterTrainPtr == TrainDescr( L"D3992", L"44-55-66" ) );

	auto impact93 = rtInt1.getTimeImpact( EsrKit( 9333 ) );
	CPPUNIT_ASSERT( !impact93.isEmpty() );
    CPPUNIT_ASSERT( !impact93.restrictStay() );
    CPPUNIT_ASSERT( impact93.Departure() == TimeImpact::toTimePoint( "20170813T062710" ) );
	CPPUNIT_ASSERT( !rtInt1.getInfoChange( EsrKit( 9333 ) ) );
	CPPUNIT_ASSERT( rtInt1.dominantOrderOn( EsrKit( 9333 ) ) );
	CPPUNIT_ASSERT( !rtInt1.shouldYieldForOther( EsrKit( 9333 ) ) );

    auto impact94 = rtInt1.getTimeImpact( EsrKit( 9444 ) );
    CPPUNIT_ASSERT( impact94.isEmpty() );

	CPPUNIT_ASSERT( rtInt1.getTransitStations().empty() );

	const auto rtInt2 = routeIntensions[1];
	const auto & tdescr2 = rtInt2.getTrainDescr();
	CPPUNIT_ASSERT( tdescr2.GetNumber().getNumber() == 7005 && tdescr2.GetIndex().str() == L"0003-002-0001" );
	auto impact81 = rtInt2.getTimeImpact( EsrKit( 8111 ) );
	CPPUNIT_ASSERT( !impact81.isEmpty() );
	CPPUNIT_ASSERT( !impact81.restrictDeparture() );
    CPPUNIT_ASSERT( impact81.Stay() == bt::minutes( 11 ) );

	auto impact82 = rtInt2.getTimeImpact( EsrKit( 8222 ) );
	CPPUNIT_ASSERT( !impact82.isEmpty() );
    CPPUNIT_ASSERT( impact82.Departure() == TimeImpact::toTimePoint( "20170822T162839" ) );
    CPPUNIT_ASSERT( impact82.Stay() == bt::minutes( 22 ) );

	auto impact83 = rtInt2.getTimeImpact( EsrKit( 8333 ) );
	CPPUNIT_ASSERT( !impact83.isEmpty() );
    CPPUNIT_ASSERT( !impact83.restrictDeparture() );
    CPPUNIT_ASSERT( impact83.Stay() == bt::minutes( 3 ) );

	auto impact84 = rtInt2.getTimeImpact( EsrKit( 8444 ) );
	CPPUNIT_ASSERT( !impact84.isEmpty() );
    CPPUNIT_ASSERT( impact84.Departure() == TimeImpact::toTimePoint( "20171026T024152" ) );
    CPPUNIT_ASSERT( impact84.Stay() == bt::minutes( 4 ) );

	CPPUNIT_ASSERT( !rtInt2.dominantOrderOn( EsrKit( 9999 ) ) );
	CPPUNIT_ASSERT( !rtInt2.shouldYieldForOther( EsrKit( 9999 ) ) );
	CPPUNIT_ASSERT( rtInt2.getTimeImpact( EsrKit( 9999 ) ).isEmpty() );

	const auto & walkthroughStations = rtInt2.getTransitStations();
	CPPUNIT_ASSERT( walkthroughStations.size() == 3 );
	CPPUNIT_ASSERT( walkthroughStations[0] == EsrKit( 7011 ) );
	CPPUNIT_ASSERT( walkthroughStations[1] == EsrKit( 7022 ) );
	CPPUNIT_ASSERT( walkthroughStations[2] == EsrKit( 7033 ) );

	const auto & ichangePtr1 = rtInt2.getInfoChange( EsrKit( 8555 ) );
	CPPUNIT_ASSERT( ichangePtr1 && *ichangePtr1 == TrainDescr( L"D2055" ) );
}
