#include "stdafx.h"
#include "TC_TrackerTrainImpl.h"
#include "../UT/TestTracker.h"
#include "../Guess/Msg.h"
#include "../Guess/GLog.h"
#include "../Hem/HLog.h"
#include "../Guess/TrainContainer.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Guess/FieldGraph.h"
#include "../Guess/TrainInfoPackage.h"
#include "../Guess/ChainContainer.h"
#include "../Actor/Latch_LOSHead.h"

using namespace std;
using namespace boost::posix_time;

TC_TrackerTrainImpl::TC_TrackerTrainImpl() : TI( *TrackerInfrastructure::instance() )
{
	tracker = TI.Tracker();
	const auto & tcontRef = tracker->GetTrainContainer();
	trainCont = &tcontRef;
}

TC_TrackerTrainImpl::~TC_TrackerTrainImpl()
{
	HemEventRegistry::Shutdowner();
    Latch_LOS_Head::Shutdowner();
    TrackerInfrastructure::Shutdowner();
}

TrainInfoPackage TC_TrackerTrainImpl::CreateInfoPackage( bool disform, unsigned int trainIden, const TrainDescr & tdescr, const BadgeE & tplace,
						const TrainCharacteristics::Source & source, time_t * timptr ) const
{
	time_t timmom = time( NULL );
	if ( timptr )
	{
		timmom = *timptr;
	}
	else
	{
		timmom = TI.GetTimeMoment();
	}
	tracker->SetTime( timmom ); //проверить - надо ли делать вызов
	const TrainCharacteristics::SourceInfo srcInfo( source, L"", "" );
	return TrainInfoPackage( tdescr, srcInfo, timmom, tplace, trainIden, disform ? TrainInfoPackage::TrainAction::DISFORM : TrainInfoPackage::TrainAction::ASSIGN );
}

TrainDescr TC_TrackerTrainImpl::RandomTrainDescr( bool isOdd )
{
	TrainDescr RandomDescr;
	static unsigned int oddNum = 1;
	static unsigned int evenNum = 2;
	auto & tnum = isOdd ? oddNum : evenNum;
    wstring buf = to_wstring(tnum);
	tnum += 2;
	RandomDescr.SetIndex( L"1-2-3" + buf );
	RandomDescr.SetNumber( buf );
	RandomDescr.SetRelLength( 55 + tnum * 2 );
	RandomDescr.SetNetto( 200 );
	RandomDescr.SetFullCarsCount( 84 );
	return RandomDescr;
}

void TC_TrackerTrainImpl::SetRandomOddInfo( TrainUnityCPtr tunity, TestTracker * sometracker )
{
	TestTracker * acttracker = ( sometracker ? sometracker : tracker );
	SetInfo( tunity, RandomTrainDescr( true ), acttracker );
}

void TC_TrackerTrainImpl::SetRandomEvenInfo( TrainUnityCPtr tunity, TestTracker * sometracker )
{
	TestTracker * acttracker = ( sometracker ? sometracker : tracker );
	SetInfo( tunity, RandomTrainDescr( false ), acttracker );
}

void TC_TrackerTrainImpl::SetInfo( TrainUnityCPtr train, const TrainDescr & tdescr, TestTracker * sometracker )
{
	TestTracker * acttracker = ( sometracker ? sometracker : tracker );
	acttracker->SetTrainsInfo( CreateInfoPackage( false, train->GetId(), tdescr, BadgeE() ) );
}

TrainUnityCPtr TC_TrackerTrainImpl::SetInfo( const BadgeE & placeBdg, const TrainDescr & tdescr, const TrainCharacteristics::Source & defsource )
{
	tracker->SetTrainsInfo( CreateInfoPackage( false, 0, tdescr, placeBdg, defsource ) );
	TrainUnityCPtr retUnity;
	const auto & tunities = trainCont->GetUnities();
	auto tIt = find_if( tunities.cbegin(), tunities.cend(), [&placeBdg]( TrainUnityCPtr tptr ){
		return tptr->Have( placeBdg );
	} );
	if ( tIt != tunities.cend() )
		retUnity = *tIt;
	return retUnity;
}

TrainUnityCPtr TC_TrackerTrainImpl::SetRandomOddInfo( const BadgeE & onPlace )
{
	return SetInfo( onPlace, RandomTrainDescr( true ), TrainCharacteristics::Source::User );
}

TrainUnityCPtr TC_TrackerTrainImpl::SetRandomEvenInfo( const BadgeE & onPlace )
{
	return SetInfo( onPlace, RandomTrainDescr( false ), TrainCharacteristics::Source::User );
}

string TC_TrackerTrainImpl::DisformTrain( unsigned int trainId, const TrainCharacteristics::Source & disfsource, TestTracker * sometracker )
{
	TestTracker * acttracker = ( sometracker ? sometracker : tracker );
	TrainDescr tdescr;
	const auto & tcont = acttracker->GetTrainContainer();
	auto trainPtr = tcont.GetUnity( trainId );
	ASSERT( trainPtr );
	if ( trainPtr && trainPtr->GetDescrPtr() )
		tdescr = *trainPtr->GetDescrPtr();
	return acttracker->SetTrainsInfo( CreateInfoPackage( true, trainId, tdescr, BadgeE(), disfsource ) );
}

string TC_TrackerTrainImpl::DisformTrain( unsigned int trainId, const BadgeE & place, const TrainDescr & tdescr, const TrainCharacteristics::Source & disfsource, 
			TestTracker * sometracker )
{
	TestTracker * acttracker = ( sometracker ? sometracker : tracker );
	return acttracker->SetTrainsInfo( CreateInfoPackage( true, trainId, tdescr, place, disfsource ) );
}

bool TC_TrackerTrainImpl::PullingTailToCWay( TrainCharacteristics::TrainFeature tfeature )
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"W", *tracker ); //W

	//установка соответствующего свойства:
	auto tlist = trainCont->GetUnities();
	CPPUNIT_ASSERT( tlist.size() == 1 );
	auto train = tlist.back();
	CPPUNIT_ASSERT( !train->IsTrain() );
	TrainDescr tdescr;
	tdescr.SetFeature( tfeature );
	SetInfo( train, tdescr );

	TI.DetermineStripBusy( L"Q", *tracker ); //W-Q
	TI.DetermineStripBusy( L"A", *tracker ); //W-Q-A
	TI.DetermineStripFree( L"W", *tracker ); //Q-A
	TI.DetermineStripBusy( L"B", *tracker ); //Q-A-B
	TI.DetermineStripBusy( L"C", *tracker ); //Q-A-B-C (здесь прибытия еще нет)
	TI.DetermineStripFree( L"Q", *tracker ); //A-B-C
	TI.DetermineStripFree( L"A", *tracker ); //B-C
	auto trackerEvents = tracker->GetTrainEvents();
	auto arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
	CPPUNIT_ASSERT( arrEvIt == trackerEvents.cend() );
	TI.DetermineStripFree( L"B", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	arrEvIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr tevPtr ){
		return tevPtr->GetCode() == HCode::ARRIVAL;
	} );
	return ( arrEvIt != trackerEvents.cend() );
}

void TC_TrackerTrainImpl::PullingTailToEWay()
{
	TI.DetermineSwitchPlus( L"11", *tracker );
	TI.DetermineSwitchMinus( L"13", *tracker );
	TI.DetermineSwitchMinus( L"12", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"D", *tracker );
	TI.DetermineStripBusy( L"E", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"D", *tracker );
}

void TC_TrackerTrainImpl::OpenWayAsyncArriving( TrainCharacteristics::TrainFeature tfeature, DWORD waitingTimeMs )
{
	//въезд поезда на путь с открытым светофором:
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	PullingTailToCWay( tfeature );
	list <TrackerEventPtr> trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	TI.IncreaseTime( waitingTimeMs );
	TI.DetermineStripBusy( L"F", *tracker ); //выезд с п/о пути
}

bool TC_TrackerTrainImpl::CheckTimeSequence( const list <TrackerEventPtr> & tevents ) const
{
	map <unsigned int, list <TrackerEventPtr> > iden2Events;
	for_each( tevents.cbegin(), tevents.cend(), [&iden2Events]( TrackerEventPtr tevptr ){
		iden2Events[tevptr->GetId()].push_back( tevptr );
	} );
	bool notDecreasing = true;
	for ( auto idevIt = iden2Events.cbegin(); idevIt != iden2Events.cend(); ++idevIt )
	{
		const auto & tevents = idevIt->second;
		if ( tevents.size() > 1 )
		{
			for ( auto teIt1 = tevents.cbegin(), teIt2 = ++tevents.cbegin(); teIt1 != tevents.cend() && teIt2 != tevents.cend(); ++teIt1, ++teIt2 )
			{
				if ( ( *teIt1 )->GetTime() > ( *teIt2 )->GetTime() )
				{
					notDecreasing = false;
					break;
				}
			}
		}
		if ( !notDecreasing )
			break;
	}
	return notDecreasing;
}


bool TC_TrackerTrainImpl::LocateUnityByForce( TrainDescrPtr tdescrPtr, const list<wstring> & place, TestTracker * customTracker )
{
	auto curtracker = customTracker ? customTracker : tracker;
	auto & trainCont = curtracker->GetTrainContainer();
	auto & chainCont = curtracker->GetChainContainer();
	auto & losKeeper = curtracker->GetLosKeeper();
	list<BadgeE> bornPlace;
	for( const auto & plcName : place )
	{
		const auto & plcBdg = TI.Get( plcName ).bdg;
		TestElem & telem = TI.Get( plcName );
		telem.Los()->setUsed( StateLTS::ACTIVE );
		unordered_map<BadgeE, LosInfo> bdgToInfos;
		bdgToInfos.insert( make_pair( plcBdg, LosInfo( telem.Los(), STRIP, curtracker->GetTrackTime().GetInternalTickMoment() ) ) );
		losKeeper.Set( bdgToInfos );
		chainCont.EnsureChain( plcBdg );
		bornPlace.push_back( plcBdg );
	}
	for ( auto plcIt1 = bornPlace.cbegin(), plcIt2 = ++bornPlace.cbegin(); plcIt1 != bornPlace.cend() && plcIt2 != bornPlace.cend(); ++plcIt1, ++plcIt2 )
		chainCont.Bind( *plcIt1, *plcIt2 );

	auto tfieldPtr = curtracker->GetTrainField();
	const auto & fldgraph = tfieldPtr->GetComponent<TrainField::FieldGraph>();
	
	//слияние с соседними ЦЗ
	const auto & frontNeighbs = fldgraph.EdgesNearEdge( bornPlace.front() );
	for ( const auto & neighBdg : frontNeighbs )
	{
		if ( tfieldPtr->ReachableNear( losKeeper, neighBdg, bornPlace.front() ) && chainCont.GetConstChain( neighBdg ) &&
			find( bornPlace.cbegin(), bornPlace.cend(), neighBdg ) == bornPlace.cend() )
			chainCont.Bind( neighBdg, bornPlace.front() );
	}
	if ( bornPlace.size() > 1 )
	{
		const auto & backNeighbs = fldgraph.EdgesNearEdge( bornPlace.back() );
		for ( const auto & neighBdg : backNeighbs )
		{
			if ( fldgraph.Moveable2( neighBdg, bornPlace.back() ) && chainCont.GetConstChain( neighBdg ) && 
				find( bornPlace.cbegin(), bornPlace.cend(), neighBdg ) == bornPlace.cend() )
			{
				chainCont.Bind( neighBdg, bornPlace.back() );
				break;
			}
		}
	}

	//назначение информации
	auto tunityPtr = trainCont.BornUnity( bornPlace, 0, BadgeE() );
	if ( tdescrPtr )
		SetInfo( tunityPtr, *tdescrPtr, curtracker );

	bool placesMatched = false;
	auto place1 = place;
	list<wstring> place2;
	for_each( bornPlace.cbegin(), bornPlace.cend(), [&place2]( const BadgeE & bdg ){
		place2.push_back( bdg.str() );
	} );
	place1.sort();
	place2.sort();
	return place1 == place2;
}

bool TC_TrackerTrainImpl::chk_eq( const TrackerEventPtr ev, HCode hcode, const wchar_t* bdg_name )
{
    return ev && ev->GetCode() == hcode && ev->GetBadge() == TI.Get( bdg_name ).bdg;
}