#include "stdafx.h"
#include "TC_FutureTrainRoutes.h"
#include "../Augur/TrainRoutes.h"
#include "../helpful/Serialization.h"
#include "../Augur/AThread.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Hem/FixableEvent.h"
#include "../helpful/TrainDescr.h"
#include "../Guess/SpotDetails.h"

using namespace std;
using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_FutureTrainRoutes );

void TC_FutureTrainRoutes::Serialize()
{
	TrainRoutes<AEvent> futureLayer( TrainRoutes<AEvent>::Type::ForecastOut );

	vector <AEvent> aevents;
	AEvent stopStart1( 32000 );
	stopStart1.setType( AEvent::STOP_START );
	stopStart1.setObj( 3, 5 );
	tm tm1 = to_tm( from_iso_string("20170209T114300") );
	tm tm2 = to_tm( from_iso_string("20170209T114427") );
	stopStart1.diap = ADiap( _mkgmtime( &tm1 ), _mkgmtime( &tm2 ) );
	aevents.emplace_back( stopStart1 );

	AEvent transit( 22120 );
	transit.setType( AEvent::MOVE );
	transit.setObj( 2, 6 );
	tm tm3 = to_tm( from_iso_string("20170209T114543") );
	transit.diap = ADiap( _mkgmtime( &tm3 ) );
	aevents.emplace_back( transit );

	AEvent stopStart2( 11802 );
	stopStart2.setType( AEvent::STOP_START );
	stopStart2.setObj( 1, 7 );
	tm tm4 = to_tm( from_iso_string("20170209T115032") );
	tm tm5 = to_tm( from_iso_string("20170209T115241") );
	stopStart2.diap = ADiap( _mkgmtime( &tm4 ), _mkgmtime( &tm5 ) );
	aevents.emplace_back( stopStart2 );

	AEvent arrival( 55112 );
	arrival.setType( AEvent::STOP );
	arrival.setObj( 8, 4 );
	tm tm6 = to_tm( from_iso_string( "20170209T121345" ) );
	arrival.diap = ADiap( _mkgmtime( &tm6 ), _mkgmtime( &tm6 ) );
	aevents.emplace_back( arrival );

	AEvent departure( 55112 );
	departure.setType( AEvent::START );
	departure.setObj( 8, 4 );
	tm tm7 = to_tm( from_iso_string( "20170209T121714" ) );
	departure.diap = ADiap( _mkgmtime( &tm7 ), _mkgmtime( &tm7 ) );
	aevents.emplace_back( departure );

	futureLayer.addEventLine( make_pair( TrainDescr( L"1152" ), aevents ) );

	string serialStr = serialize( futureLayer );
	string testStr = "<FutureLayer>"\
						"<HemPath>"\
							"<SpotEvent create_time=\"20170209T114300\" name=\"Info_changing\">"\
								"<TrainDescr index=\"\" num=\"1152\" cont=\"Y\" />"\
							"</SpotEvent>"\
							"<SpotEvent create_time=\"20170209T114300\" name=\"Arrival\" waynum=\"3\" parknum=\"5\" Bdg=\"mock[32000]\" />"\
							"<SpotEvent create_time=\"20170209T114427\" name=\"Departure\" waynum=\"3\" parknum=\"5\" Bdg=\"mock[32000]\" />"\
							"<SpotEvent create_time=\"20170209T114543\" name=\"Transition\" waynum=\"2\" parknum=\"6\" Bdg=\"mock[22120]\" />"\
							"<SpotEvent create_time=\"20170209T115032\" name=\"Arrival\" waynum=\"1\" parknum=\"7\" Bdg=\"mock[11802]\" />"\
							"<SpotEvent create_time=\"20170209T115241\" name=\"Departure\" waynum=\"1\" parknum=\"7\" Bdg=\"mock[11802]\" />"\
							"<SpotEvent create_time=\"20170209T121345\" name=\"Arrival\" waynum=\"8\" parknum=\"4\" Bdg=\"mock[55112]\" />"\
							"<SpotEvent create_time=\"20170209T121714\" name=\"Departure\" waynum=\"8\" parknum=\"4\" Bdg=\"mock[55112]\" />"\
						"</HemPath>"\
					 "</FutureLayer>";
    
	auto mismRes = mismatch( serialStr.cbegin(), serialStr.cend(), testStr.cbegin(), testStr.cend() );
	CPPUNIT_ASSERT( serialStr == testStr );
}

void TC_FutureTrainRoutes::Deserialize()
{
	string serialStr = "<FutureLayer>"\
		"<HemPath>"\
		"<SpotEvent create_time=\"20170209T114300\" name=\"Info_changing\">"\
			"<TrainDescr index=\"\" num=\"1152\" cont=\"Y\" />"\
		"</SpotEvent>"\
		"<SpotEvent create_time=\"20170209T114300\" name=\"Arrival\" waynum=\"3\" parknum=\"5\" Bdg=\"mock[32000]\" />"\
		"<SpotEvent create_time=\"20170209T114427\" name=\"Departure\" waynum=\"3\" parknum=\"5\" Bdg=\"mock[32000]\" />"\
		"<SpotEvent create_time=\"20170209T114543\" name=\"Transition\" waynum=\"2\" parknum=\"6\" Bdg=\"mock[22120]\" />"\
		"<SpotEvent create_time=\"20170209T115032\" name=\"Arrival\" waynum=\"1\" parknum=\"7\" Bdg=\"mock[11802]\" />"\
		"<SpotEvent create_time=\"20170209T115241\" name=\"Departure\" waynum=\"1\" parknum=\"7\" Bdg=\"mock[11802]\" />"\
		"</HemPath>"\
		"</FutureLayer>";
	auto futlayerPtr = deserialize<TrainRoutes<FixableEvent>>( serialStr );
	CPPUNIT_ASSERT( futlayerPtr );
	auto spotLines = futlayerPtr->getSpotLines();
	CPPUNIT_ASSERT( spotLines.size() == 1);
	const auto & eventLine = spotLines.back();
	CPPUNIT_ASSERT( eventLine.first.GetNumber().getNumber() == 1152 );
	const auto & events = eventLine.second;
	CPPUNIT_ASSERT( events.size() == 5 );

	CPPUNIT_ASSERT( events[0].GetCode() == HCode::ARRIVAL );
	auto parkWay = events[0].ParkWay();
	CPPUNIT_ASSERT( parkWay.iWay() == 3 && parkWay.iPark() == 5 );
	tm tmval = to_tm( from_iso_string("20170209T114300") );
	CPPUNIT_ASSERT( events[0].GetTime() == _mkgmtime( &tmval ) );
	CPPUNIT_ASSERT( events[0].GetBadge().num() == EsrKit( 32000 ) );

	CPPUNIT_ASSERT( events[1].GetCode() == HCode::DEPARTURE );
	parkWay = events[1].ParkWay();
	CPPUNIT_ASSERT( parkWay.iWay() == 3 && parkWay.iPark() == 5 );
	tmval = to_tm( from_iso_string("20170209T114427") );
	CPPUNIT_ASSERT( events[1].GetTime() == _mkgmtime( &tmval ) );
	CPPUNIT_ASSERT( events[1].GetBadge().num() == EsrKit( 32000 ) );

	CPPUNIT_ASSERT( events[2].GetCode() == HCode::TRANSITION );
	parkWay = events[2].ParkWay();
	CPPUNIT_ASSERT( parkWay.iWay() == 2 && parkWay.iPark() == 6 );
	tmval = to_tm( from_iso_string("20170209T114543") );
	CPPUNIT_ASSERT( events[2].GetTime() == _mkgmtime( &tmval ) );
	CPPUNIT_ASSERT( events[2].GetBadge().num() == EsrKit( 22120 ) );

	CPPUNIT_ASSERT( events[3].GetCode() == HCode::ARRIVAL );
	parkWay = events[3].ParkWay();
	CPPUNIT_ASSERT( parkWay.iWay() == 1 && parkWay.iPark() == 7 );
	tmval = to_tm( from_iso_string("20170209T115032") );
	CPPUNIT_ASSERT( events[3].GetTime() == _mkgmtime( &tmval ) );
	CPPUNIT_ASSERT( events[3].GetBadge().num() == EsrKit( 11802 ) );

	CPPUNIT_ASSERT( events[4].GetCode() == HCode::DEPARTURE );
	parkWay = events[4].ParkWay();
	CPPUNIT_ASSERT( parkWay.iWay() == 1 && parkWay.iPark() == 7 );
	tmval = to_tm( from_iso_string("20170209T115241") );
	CPPUNIT_ASSERT( events[4].GetTime() == _mkgmtime( &tmval ) );
	CPPUNIT_ASSERT( events[4].GetBadge().num() == EsrKit( 11802 ) );
}