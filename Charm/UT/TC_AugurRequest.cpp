#include "stdafx.h"
#include "TC_AugurRequest.h"
#include "../Augur/AugurRequest.h"
#include "../Augur/TrainRoutes.h"
#include "../helpful/Serialization.h"
#include "../Augur/AThread.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Hem/FixableEvent.h"
#include "../Augur/LimitationInfo.h"
#include "../helpful/TrainDescr.h"
#include "../helpful/RestrictSpeed.h"
#include "../helpful/ConjugateWaysLimit.h"
#include "../helpful/ExcerptLot.h"
#include "XmlEqual.h"
#include "../Hem/RouteIntrusion.h"
#include "../Guess/SpotDetails.h"
#include "../helpful/TrainsPriorityOffsets.h"
#include "../helpful/SpanTravelTime.h"

using namespace std;
namespace bt = boost::posix_time;
using namespace Excerpt;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AugurRequest );

const string areqTestStr = "<AugurRequest type = \"casual\">"\
	"<LiveMoment value=\"20010911T183041Z\" />"\
	"<HappenLayer>"\
		"<HemPath>"\
		"<SpotEvent create_time=\"20170630T191000\" name=\"Info_changing\">"\
			"<TrainDescr index=\"\" num=\"1152\" cont=\"Y\" />"\
		"</SpotEvent>"\
		"<SpotEvent create_time=\"20170630T191000\" name=\"Arrival\" Bdg=\"XXX[00100]\" waynum=\"5\" parknum=\"2\" />"\
		"<SpotEvent create_time=\"20170630T191500\" name=\"Departure\" Bdg=\"XXX[00100]\" waynum=\"5\" parknum=\"2\" />"\
		"</HemPath>"\
		"<HemPath>"\
		"<SpotEvent create_time=\"20170715T151900\" name=\"Info_changing\">"\
			"<TrainDescr index=\"\" num=\"3520\" outbnd=\"Y\" />"\
		"</SpotEvent>"\
		"<SpotEvent create_time=\"20170715T151900\" name=\"Transition\" Bdg=\"XXX[00100]\" waynum=\"4\" parknum=\"1\" />"\
		"</HemPath>"\
		"<HemPath>"\
		"<SpotEvent create_time=\"20170812T115020\" name=\"Info_changing\">"\
			"<TrainDescr index=\"\" num=\"\" noinfo=\"Y\" />"\
		"</SpotEvent>"\
		"<SpotEvent create_time=\"20170812T115020\" name=\"Transition\" Bdg=\"XXX[00100]\" waynum=\"3\" parknum=\"3\" />"\
		"</HemPath>"\
		"<HemPath>"\
		"<SpotEvent create_time=\"20170915T072055\" name=\"Info_changing\">"\
			"<TrainDescr index=\"\" num=\"6010\" suburbreg=\"Y\" />"\
		"</SpotEvent>"\
		"<SpotEvent create_time=\"20170915T072055\" name=\"Transition\" Bdg=\"XXX[00400]\" waynum=\"4\" parknum=\"1\" />"\
		"<SpotEvent create_time=\"20170915T081037\" name=\"Arrival\" Bdg=\"XXX[00300]\" waynum=\"1\" parknum=\"2\" sourcetype=\"fix\" />"\
		"<SpotEvent create_time=\"20170915T081500\" name=\"Departure\" Bdg=\"XXX[00300]\" waynum=\"1\" parknum=\"2\" sourcetype=\"fix\" />"\
		"</HemPath>"\
	"</HappenLayer>"\
	"<LimitationInfos>"\
		"<LimitInfo start=\"20171215T072511Z\" end=\"20171215T094511Z\">"\
			"<location esr=\"04411:84547\" waynum=\"3\">"\
				"<picketing_start picketing1_val=\"1~\" picketing1_comm=\"mockaxis\" />"\
				"<picketing_stop picketing1_val=\"2~\" picketing1_comm=\"mockaxis\" />"\
			"</location>"\
			"<velocity SpeedAll=\"9\" />" \
		"</LimitInfo>"\
	"</LimitationInfos>"\
	"<ScheduledLayer>"\
		"<HemPath>"\
		"<SpotEvent create_time=\"20170715T014145\" name=\"Info_changing\">"\
			"<TrainDescr index=\"\" num=\"7845\" temot=\"Y\" />"\
		"</SpotEvent>"\
		"<SpotEvent create_time=\"20170715T014145\" name=\"Transition\" Bdg=\"XXX[00100]\" waynum=\"3\" parknum=\"10\" />"\
		"</HemPath>"\
		"<HemPath>"\
		"<SpotEvent create_time=\"20170715T052417\" name=\"Info_changing\">"\
			"<TrainDescr index=\"\" num=\"9841\" noinfo=\"Y\" />"\
		"</SpotEvent>"\
		"<SpotEvent create_time=\"20170715T052417\" name=\"Arrival\" Bdg=\"XXX[00100]\" waynum=\"1\" parknum=\"4\" />"\
		"<SpotEvent create_time=\"20170715T052706\" name=\"Departure\" Bdg=\"XXX[00100]\" waynum=\"1\" parknum=\"4\" />"\
		"</HemPath>"\
	"</ScheduledLayer>"\
	"<ArtificalRouteIntrusions>"\
		"<RouteIntrusion>"
			"<TrainDescr index=\"0001-002-0003\" num=\"7004\" suburbfast=\"Y\"/>"\
			"<Station esrCode=\"11250\">"\
				"<TimeImpact minstaying=\"1380\"/>"\
			"</Station>"\
		"</RouteIntrusion>"\
	"</ArtificalRouteIntrusions>"\
	"<PriorityOffsets>"\
		"<Train number=\"1197\" type=\"outerDec\" shift=\"8\"/>\n"\
	"</PriorityOffsets>"\
    "<PvxList>"
    "  <Pvx Span='(12345,54321)'>"
    "    <Gruz start='1' full='2' reserv='3' stop='4'/>"
    "    <Pass start='5' full='6' reserv='7' stop='8'/>"
    "  </Pvx>"
    "</PvxList>"
	"</AugurRequest>";

const string areqTestStr2 = "<AugurRequest type = \"casual\">"\
		"<LiveMoment value=\"\" />"\
	"</AugurRequest>";

const string areqTestStr3 = "<AugurRequest type = \"schedule\">"\
		"<LiveMoment value=\"\" />"\
	"</AugurRequest>";

const string areqTestStr4 = "<AugurRequest type = \"schedule\">"\
		"<LiveMoment value=\"\" />"\
		"<PriorityOffsets>"\
			"<Train number=\"1197\" type=\"outerDec\" shift=\"8\"/>\n"\
		"</PriorityOffsets>"\
	"</AugurRequest>";

const string areqTestStr5 = 
    "<AugurRequest type='schedule'>"
    "  <LiveMoment value='' />"
    "  <PvxList>"
    "    <Pvx Span='(12345,54321)'>"
    "      <Gruz start='1' full='1' reserv='2'  stop='3'/>"
    "      <Pass start='5' full='8' reserv='13' stop='21'/>"
    "    </Pvx>"
    "  </PvxList>"
    "</AugurRequest>";

void TC_AugurRequest::Serialize()
{
	AugurRequest<FixableEvent, SpotEvent> augurRequest( AugurRequest<FixableEvent, SpotEvent>::Type::Casual );
	tm livetm = to_tm( bt::from_iso_string( "20010911T183041" ) );
	augurRequest.liveMoment = _mkgmtime( &livetm );
	augurRequest.inputPathsPtr = MakeInputPaths();
	augurRequest.limitations = MakeLimitations();
	augurRequest.trainsSchedulePtr = MakeSchedulePaths();
	augurRequest.routeIntrusions = MakeIntrusions(); 
	augurRequest.priorityOffsetsPtr = MakePriorityOffsets();
    augurRequest.userSpanTravelTime = MakeUserSpanTravelTime();
	string serialStr = serialize( augurRequest );
	CPPUNIT_ASSERT( xmlEqual( serialStr, areqTestStr ) );
}

TC_AugurRequest::TrainFixablePathsPtr TC_AugurRequest::MakeInputPaths() const
{
	TrainFixablePathsPtr inputPathsPtr( new TrainFixablePaths( TrainFixablePaths::Type::ForecastIn ) );

	//поезд 1152
	vector <FixableEvent> spevents1;
	tm arrivMom = to_tm( bt::from_iso_string("20170630T191000") );
	SpotDetailsPtr spotDetPtr( new SpotDetails() );
	spotDetPtr->parkway = ParkWayKit( 2, 5 );
	FixableEvent arrivEvent( SpotEvent( HCode::ARRIVAL, BadgeE(L"XXX",EsrKit(100)), _mkgmtime( &arrivMom ), spotDetPtr ), FixableEvent::FixatingKind::MOVEABLE );
	spevents1.emplace_back( arrivEvent );
	tm departMom = to_tm( bt::from_iso_string("20170630T191500") );
	FixableEvent departEvent( SpotEvent( HCode::DEPARTURE, BadgeE(L"XXX",EsrKit(100)), _mkgmtime( &departMom ), spotDetPtr ), FixableEvent::FixatingKind::MOVEABLE );
	spevents1.emplace_back( departEvent );
	inputPathsPtr->addEventLine( make_pair( TrainDescr( L"1152" ), spevents1 ) );

	//поезд 3520
	vector <FixableEvent> spevents2;
	tm transitMom = to_tm( bt::from_iso_string("20170715T151900") );
	SpotDetailsPtr spotDetPtr2( new SpotDetails() );
	spotDetPtr2->parkway = ParkWayKit( 1, 4 );
	FixableEvent transitEvent( SpotEvent( HCode::TRANSITION, BadgeE(L"XXX",EsrKit(100)), _mkgmtime( &transitMom ), spotDetPtr2 ), FixableEvent::FixatingKind::MOVEABLE );
	spevents2.emplace_back( transitEvent );
	inputPathsPtr->addEventLine( make_pair( TrainDescr( L"3520" ), spevents2 ) );

	//неидентифицированный поезд
	tm transitMom2 = to_tm( bt::from_iso_string("20170812T115020") );
	SpotDetailsPtr spotDetPtr3( new SpotDetails() );
	spotDetPtr3->parkway = ParkWayKit( 3, 3 );
	FixableEvent transitEvent2( SpotEvent( HCode::TRANSITION, BadgeE(L"XXX",EsrKit(100)), _mkgmtime( &transitMom2 ), spotDetPtr3 ), FixableEvent::FixatingKind::MOVEABLE );
	vector<FixableEvent> spevents3( 1, transitEvent2 );
	inputPathsPtr->addEventLine( make_pair( TrainDescr(), spevents3 ) );

	//зафиксированная часть
	vector <FixableEvent> fixableEvents;
	tm transitMom3 = to_tm( bt::from_iso_string("20170915T072055") );
	SpotDetailsPtr spotDetPtr4( new SpotDetails() );
	spotDetPtr4->parkway = ParkWayKit( 1, 4 );
	FixableEvent transitEvent3( SpotEvent( HCode::TRANSITION, BadgeE(L"XXX",EsrKit(400)), _mkgmtime( &transitMom3 ), spotDetPtr4 ), FixableEvent::FixatingKind::MOVEABLE );
	fixableEvents.emplace_back( transitEvent3 );
	tm fixedArrMom = to_tm( bt::from_iso_string("20170915T081037") );
	SpotDetailsPtr spotDetPtr5( new SpotDetails() );
	spotDetPtr5->parkway = ParkWayKit( 2, 1 );
	FixableEvent fixedArrEvent( SpotEvent( HCode::ARRIVAL, BadgeE(L"XXX",EsrKit(300)), _mkgmtime( &fixedArrMom ), spotDetPtr5 ), FixableEvent::FixatingKind::FIXATED );
	fixableEvents.emplace_back( fixedArrEvent );
	tm fixedDepMom = to_tm( bt::from_iso_string("20170915T081500") );
	FixableEvent fixedDepEvent( SpotEvent( HCode::DEPARTURE, BadgeE(L"XXX",EsrKit(300)), _mkgmtime( &fixedDepMom ), spotDetPtr5 ), FixableEvent::FixatingKind::FIXATED );
	fixableEvents.emplace_back( fixedDepEvent );
	inputPathsPtr->addEventLine( make_pair( TrainDescr( L"6010" ), fixableEvents ) );
	return inputPathsPtr;
}

vector<LimitationInfo> TC_AugurRequest::MakeLimitations() const
{
	vector<LimitationInfo> liminfos;
	tm startMom = to_tm( bt::from_iso_string("20171215T072511") );
	tm endMom = to_tm( bt::from_iso_string("20171215T094511") );
	auto restrictSpeedPtr = make_shared<RestrictSpeed>();
	(*restrictSpeedPtr)[RestrictSpeed::ALL] = 9;
	rwRuledInterval mockRuler( L"mockaxis", rwCoord( 1, 0 ), rwCoord( 2, 0 ) );
	SpanLot spanLot( EsrKit( 4411, 84547 ), 3, mockRuler );
	liminfos.emplace_back( LimitationInfo( spanLot, _mkgmtime( &startMom ), _mkgmtime( &endMom ), restrictSpeedPtr, nullptr ) );
	return liminfos;
}

TC_AugurRequest::TrainSpotPathsPtr TC_AugurRequest::MakeSchedulePaths() const
{
	TrainSpotPathsPtr schedulePathsPtr( new TrainSpotPaths( TrainSpotPaths::Type::Schedule ) );

	//поезд 7845
	vector <SpotEvent> spevents1;
	tm transitMom = to_tm( bt::from_iso_string("20170715T014145") );
	SpotDetailsPtr spotDetPtr( new SpotDetails() );
	spotDetPtr->parkway = ParkWayKit( 10, 3 );
	SpotEvent transitEvent( HCode::TRANSITION, BadgeE(L"XXX",EsrKit(100)), _mkgmtime( &transitMom ), spotDetPtr );
	spevents1.emplace_back( transitEvent );
	schedulePathsPtr->addEventLine( make_pair( TrainDescr( L"7845" ), spevents1 ) );

	//поезд 9841
	vector <SpotEvent> spevents2;
	tm arrivMom = to_tm( bt::from_iso_string("20170715T052417") );
	SpotDetailsPtr spotDetPtr2( new SpotDetails() );
	spotDetPtr2->parkway = ParkWayKit( 4, 1 );
	SpotEvent arrivEvent( HCode::ARRIVAL, BadgeE(L"XXX",EsrKit(100)), _mkgmtime( &arrivMom ), spotDetPtr2 );
	spevents2.emplace_back( arrivEvent );
	tm departMom = to_tm( bt::from_iso_string("20170715T052706") );
	SpotEvent departEvent( HCode::DEPARTURE, BadgeE(L"XXX",EsrKit(100)), _mkgmtime( &departMom ), spotDetPtr2 );
	spevents2.emplace_back( departEvent );
	schedulePathsPtr->addEventLine( make_pair( TrainDescr( L"9841" ), spevents2 ) );

	return schedulePathsPtr;
}

vector<RouteIntrusion> TC_AugurRequest::MakeIntrusions() const
{
	vector<RouteIntrusion> routeIntrusions;
	typedef RouteIntrusion::StationIntrusion StationIntrusion;
	typedef RouteIntrusion::TimeImpact TimeImpact;
    typedef RouteIntrusion::TimeImpact::TimePoint TimePoint;
	typedef TimeImpact::TimeStay TimeStay;
	map<EsrKit, StationIntrusion> statIntrusions;
	statIntrusions.insert( make_pair( EsrKit( 11250 ), StationIntrusion( nullptr, nullptr, TimeImpact().Stay( bt::minutes( 23 ) ) ) ) );
	RouteIntrusion rint( TrainDescr( L"7004", L"1-2-3" ), vector<EsrKit>(), statIntrusions, map<EsrKit, RouteIntrusion::SpanIntrusion>() );
	routeIntrusions.push_back( rint );
	return routeIntrusions;
}

TC_AugurRequest::TrainsPriorityOffsetsPtr TC_AugurRequest::MakePriorityOffsets() const
{
	TrainsPriorityOffsetsPtr offsetsPtr( new TrainsPriorityOffsets() );
	offsetsPtr->GroupDecrease( TrainNumber( L"1197" ), 8 );
	return offsetsPtr;
}

SpanTravelTimePtr TC_AugurRequest::MakeUserSpanTravelTime() const
{
    EsrKit from(12345);
    EsrKit to(54321);

    STT_Val sttVal;
    sttVal.set_gruz( 1*60, 2*60, 3*60, 4*60 );
    sttVal.set_pass( 5*60, 6*60, 7*60, 8*60 );

    SpanTravelTimePtr userPvxPtr = std::make_shared<SpanTravelTime>();
    userPvxPtr->SetSpanTime( SpanTravelTimeInfo( from, to ), sttVal );

    return userPvxPtr;
}

void TC_AugurRequest::TestInputPaths( TrainAEventPathsPtr inputPathsPtr ) const
{
	CPPUNIT_ASSERT( inputPathsPtr );
	const auto & spotLines = inputPathsPtr->getSpotLines();
	CPPUNIT_ASSERT( spotLines.size() == 4 );
	for( unsigned int k = 0; k < spotLines.size(); ++k )
	{
		const auto & spotLine = spotLines[k];
		const auto & evvec = spotLine.second;
		switch( k )
		{
		case 0:
			{
				const auto & aevent = evvec.front();
				CPPUNIT_ASSERT( evvec.size() == 1 );
				CPPUNIT_ASSERT( spotLine.first.GetNumber().getNumber() == 1152 );
				CPPUNIT_ASSERT( aevent.getType() == AEvent::STOP_START );
				CPPUNIT_ASSERT( aevent.Num() == 5 && aevent.Park() == 2 );
				tm arrMom = to_tm( bt::from_iso_string("20170630T191000") );
				tm depMom = to_tm( bt::from_iso_string("20170630T191500") );
				CPPUNIT_ASSERT( aevent.diap.getOrig() == _mkgmtime( &arrMom ) );
				CPPUNIT_ASSERT( aevent.diap.getEnd() == _mkgmtime( &depMom ) );
				CPPUNIT_ASSERT( aevent.getSource() == AEvent::Source::REAL );
				break;
			}
		case 1:
			{
				const auto & aevent = evvec.front();
				CPPUNIT_ASSERT( evvec.size() == 1 );
				CPPUNIT_ASSERT( spotLine.first.GetNumber().getNumber() == 3520 );
				CPPUNIT_ASSERT( aevent.getType() == AEvent::MOVE );
				CPPUNIT_ASSERT( aevent.Num() == 4 && aevent.Park() == 1 );
				tm transMom = to_tm( bt::from_iso_string("20170715T151900") );
				CPPUNIT_ASSERT( aevent.diap.getOrig() == _mkgmtime( &transMom ) );
				CPPUNIT_ASSERT( aevent.diap.getEnd() == _mkgmtime( &transMom ) );
				CPPUNIT_ASSERT( aevent.getSource() == AEvent::Source::REAL );
				break;
			}
		case 2:
			{
				const auto & aevent = evvec.front();
				CPPUNIT_ASSERT( evvec.size() == 1 );
				CPPUNIT_ASSERT( spotLine.first.empty() );
				CPPUNIT_ASSERT( aevent.getType() == AEvent::MOVE );
				CPPUNIT_ASSERT( aevent.Num() == 3 && aevent.Park() == 3 );
				tm transMom = to_tm( bt::from_iso_string("20170812T115020") );
				CPPUNIT_ASSERT( aevent.diap.getOrig() == _mkgmtime( &transMom ) );
				CPPUNIT_ASSERT( aevent.diap.getEnd() == _mkgmtime( &transMom ) );
				CPPUNIT_ASSERT( aevent.getSource() == AEvent::Source::REAL );
				break;
			}
		case 3:
			{
				CPPUNIT_ASSERT( evvec.size() == 2 );
				CPPUNIT_ASSERT( spotLine.first.GetNumber().getNumber() == 6010 );
				for ( unsigned int i = 0; i < evvec.size(); ++i )
				{
					const auto & aevent = evvec[i];
					if ( i == 0 )
					{
						CPPUNIT_ASSERT( evvec[i].getSource() == AEvent::Source::REAL );
						CPPUNIT_ASSERT( aevent.Num() == 4 && aevent.Park() == 1 );
						tm transMom = to_tm( bt::from_iso_string("20170915T072055") );
						CPPUNIT_ASSERT( aevent.diap.getOrig() == _mkgmtime( &transMom ) );
						CPPUNIT_ASSERT( aevent.diap.getEnd() == _mkgmtime( &transMom ) );
					}
					else
					{
						CPPUNIT_ASSERT( evvec[i].getSource() == AEvent::Source::FIXED );
						CPPUNIT_ASSERT( aevent.Num() == 1 && aevent.Park() == 2 );
						tm arrMom = to_tm( bt::from_iso_string("20170915T081037") );
						tm depMom = to_tm( bt::from_iso_string("20170915T081500") );
						CPPUNIT_ASSERT( aevent.diap.getOrig() == _mkgmtime( &arrMom ) );
						CPPUNIT_ASSERT( aevent.diap.getEnd() == _mkgmtime( &depMom ) );
					}
				}
			}
			break;
		}
	}
}

void TC_AugurRequest::TestLimitations( const vector<LimitationInfo> & liminfosVec ) const
{
	CPPUNIT_ASSERT( liminfosVec.size() == 1 );
	const auto & limInfo = liminfosVec[0];
	CPPUNIT_ASSERT( limInfo.location.getParkWay().iWay() == 3 );
	CPPUNIT_ASSERT( limInfo.location.getEsr() == EsrKit( 4411, 84547 ) );
	tm startMom = to_tm( bt::from_iso_string("20171215T072511") );
	CPPUNIT_ASSERT( limInfo.startMoment == _mkgmtime( &startMom ) );
	tm endMom = to_tm( bt::from_iso_string("20171215T094511") );
	CPPUNIT_ASSERT( limInfo.endMoment == _mkgmtime( &endMom ) );
	auto restrictSpeedPtr = limInfo.speedRestrictPtr;
	CPPUNIT_ASSERT( restrictSpeedPtr && ( *restrictSpeedPtr )[RestrictSpeed::ALL] == 9 );
}

void TC_AugurRequest::TestSchedulePaths( TrainAEventPathsPtr trainsSchedulePtr ) const
{
	CPPUNIT_ASSERT( trainsSchedulePtr );
	const auto & spotLines = trainsSchedulePtr->getSpotLines();
	CPPUNIT_ASSERT( spotLines.size() == 2 );
	for( unsigned int k = 0; k < spotLines.size(); ++k )
	{
		const auto & spotLine = spotLines[k];
		const auto & evvec = spotLine.second;
		const auto & aevent = evvec.front();
		CPPUNIT_ASSERT( evvec.size() == 1 );
		switch( k )
		{
		case 0:
			{
				CPPUNIT_ASSERT( spotLine.first.GetNumber().getNumber() == 7845 );
				CPPUNIT_ASSERT( aevent.getType() == AEvent::MOVE );
				CPPUNIT_ASSERT( aevent.Num() == 3 && aevent.Park() == 10 );
				tm transMom = to_tm( bt::from_iso_string("20170715T014145") );
				CPPUNIT_ASSERT( aevent.diap.getOrig() == _mkgmtime( &transMom ) );
				CPPUNIT_ASSERT( aevent.diap.getEnd() == _mkgmtime( &transMom ) );
				break;
			}
		case 1:
			{
				CPPUNIT_ASSERT( spotLine.first.GetNumber().getNumber() == 9841 );
				CPPUNIT_ASSERT( aevent.getType() == AEvent::STOP_START );
				CPPUNIT_ASSERT( aevent.Num() == 1 && aevent.Park() == 4 );
				tm arrMom = to_tm( bt::from_iso_string("20170715T052417") );
				tm depMom = to_tm( bt::from_iso_string("20170715T052706") );
				CPPUNIT_ASSERT( aevent.diap.getOrig() == _mkgmtime( &arrMom ) );
				CPPUNIT_ASSERT( aevent.diap.getEnd() == _mkgmtime( &depMom ) );
				break;
			}
		}
		for ( const auto & aevent : evvec )
			CPPUNIT_ASSERT( aevent.getSource() == AEvent::Source::NONE );
	}
}

void TC_AugurRequest::TestCurrentMoment( time_t liveMoment ) const
{
	tm tstMom = to_tm( bt::from_iso_string("20010911T183041") );
	CPPUNIT_ASSERT( liveMoment == _mkgmtime( &tstMom ) );
}

void TC_AugurRequest::TestIntrusions( const vector<RouteIntrusion> & routeIntrusions ) const
{
	CPPUNIT_ASSERT( routeIntrusions.size() == 1 );
	const auto & routeIntrusion = routeIntrusions[0];
	const auto & tdescr = routeIntrusion.getTrainDescr();
	CPPUNIT_ASSERT( tdescr.GetNumber().getNumber() == 7004 && tdescr.GetIndex().str() == L"0001-002-0003" );
	const auto & impact = routeIntrusion.getTimeImpact( EsrKit( 11250 ) );
	CPPUNIT_ASSERT( !impact.isEmpty() );
    CPPUNIT_ASSERT( !impact.restrictDeparture() );
    CPPUNIT_ASSERT( impact.Stay() == bt::minutes(23) );
	CPPUNIT_ASSERT( routeIntrusion.getTransitStations().empty() );
}

void TC_AugurRequest::TestPriorityOffsets( TrainsPriorityOffsetsPtr priorityOffsetsPtr ) const
{
	CPPUNIT_ASSERT( priorityOffsetsPtr );
	const auto & affNumbers = priorityOffsetsPtr->AffectedNumbers();
	CPPUNIT_ASSERT( affNumbers.size() == 1 );
	const auto & tnum = affNumbers[0];
	CPPUNIT_ASSERT( tnum == TrainNumber( L"1197" ) );
	auto offsetPtr = priorityOffsetsPtr->GetOffset( tnum );
	CPPUNIT_ASSERT( offsetPtr && offsetPtr->type == TrainsPriorityOffsets::OuterDecreased && offsetPtr->shift == 8 );
}

void TC_AugurRequest::TestUserSpanTravelTime( SpanTravelTimePtr userPvx ) const
{
    CPPUNIT_ASSERT( userPvx );

    const EsrKit from(12345);
    const EsrKit to(54321);

    STT_Val sttVal;
    sttVal.set_gruz( 1*60, 2*60, 3*60, 4*60 );
    sttVal.set_pass( 5*60, 6*60, 7*60, 8*60 );

    const SpanTravelTimeInfo sttInfo( from, to );

    auto spans = userPvx->GetSpans();
    CPPUNIT_ASSERT( spans.size() == 1 );
    CPPUNIT_ASSERT( spans.front() == sttInfo );

    CPPUNIT_ASSERT( userPvx->existData( sttInfo ) );
    auto stt = userPvx->GetSpanTime( sttInfo );
    CPPUNIT_ASSERT( sttVal == stt );
}

void TC_AugurRequest::Deserialize()
{
	string serialStr = areqTestStr;
	auto augurReqPtr = deserialize<AugurRequest<AEvent, AEvent>>( serialStr );
	CPPUNIT_ASSERT( augurReqPtr );
	typedef AugurRequest<AEvent, AEvent>::Type AugurRequestType;
	CPPUNIT_ASSERT( augurReqPtr->reqtype == AugurRequestType::Casual );
	TestCurrentMoment( augurReqPtr->liveMoment );
	TestInputPaths( augurReqPtr->inputPathsPtr );
	TestLimitations( augurReqPtr->limitations );
	TestSchedulePaths( augurReqPtr->trainsSchedulePtr );
	TestIntrusions( augurReqPtr->routeIntrusions );
	TestPriorityOffsets( augurReqPtr->priorityOffsetsPtr );
    TestUserSpanTravelTime( augurReqPtr->userSpanTravelTime );
}

void TC_AugurRequest::SerializeType()
{
	AugurRequest<FixableEvent, SpotEvent> augurRequest( AugurRequest<FixableEvent, SpotEvent>::Type::Casual );
	TrainFixablePathsPtr inputPathsPtr( new TrainFixablePaths( TrainFixablePaths::Type::ForecastIn ) );
	augurRequest.inputPathsPtr = inputPathsPtr;
	string serialStr = serialize( augurRequest );
	CPPUNIT_ASSERT( xmlEqual( serialStr, areqTestStr2 ) );

	AugurRequest<FixableEvent, SpotEvent> augurRequest2( AugurRequest<FixableEvent, SpotEvent>::Type::Schedule );
	augurRequest2.inputPathsPtr = inputPathsPtr;
	string serialStr2 = serialize( augurRequest2 );
	CPPUNIT_ASSERT( xmlEqual( serialStr2, areqTestStr3 ) );
}

void TC_AugurRequest::DeserializeType()
{
	string serialStr = areqTestStr2;
	auto augurReqPtr = deserialize<AugurRequest<AEvent, AEvent>>( serialStr );
	typedef AugurRequest<AEvent, AEvent>::Type AugurRequestType;
	CPPUNIT_ASSERT( augurReqPtr && augurReqPtr->reqtype == AugurRequestType::Casual );

	string serialStr2 = areqTestStr3;
	auto augurReqPtr2 = deserialize<AugurRequest<AEvent, AEvent>>( serialStr2 );
	CPPUNIT_ASSERT( augurReqPtr2 && augurReqPtr2->reqtype == AugurRequestType::Schedule );
}

void TC_AugurRequest::DeserializePrioritiesWOIntrusions()
{
	string serialStr = areqTestStr4;
	auto augurReqPtr = deserialize<AugurRequest<AEvent, AEvent>>( serialStr );
	TestPriorityOffsets( augurReqPtr->priorityOffsetsPtr );
}

void TC_AugurRequest::DeserializeSpanTravelTime()
{
    string serialStr = areqTestStr5;
    auto augurReqPtr = deserialize<AugurRequest<AEvent, AEvent>>( serialStr );
    TestUserSpanTravelTime( augurReqPtr->userSpanTravelTime );
}