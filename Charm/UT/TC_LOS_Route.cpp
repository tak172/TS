#include "stdafx.h"
#include "TC_LOS_Route.h"
#include "../Actor/LOSBase.h"
#include "../Fund/RouteElementState.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LOS_Route );

void TC_LOS_Route::setTrainRouteByAllLocked()
{
	unique_ptr<LOS_base> losRoute( LOS_base::fabric_from_object( ROUTE ) );

	//поездной маршрут
	losRoute->setRouteType( true );

	//открытая голова
	RouteElementState headState( HEAD );
	unique_ptr<LOS_base> losHead( LOS_base::fabric_from_object( HEAD ) );
	losHead->setOpenSignal( StateLTS::ACTIVE );
	headState.saveLOS( losHead.get() );
	losRoute->addRouteElementState( BadgeE( L"head", EsrKit( 1 ) ), headState, 0 );

	//два замкнутых участка
	RouteElementState stripState1( STRIP );
	unique_ptr<LOS_base> losStrip1( LOS_base::fabric_from_object( STRIP ) );
	losStrip1->setLock( StateLTS::ACTIVE );
	stripState1.saveLOS( losStrip1.get() );
	losRoute->addRouteElementState( BadgeE( L"strip1", EsrKit( 1 ) ), stripState1, 1 );
	RouteElementState stripState2( STRIP );
	unique_ptr<LOS_base> losStrip2( LOS_base::fabric_from_object( STRIP ) );
	losStrip2->setLock( StateLTS::ACTIVE );
	stripState2.saveLOS( losStrip2.get() );
	losRoute->addRouteElementState( BadgeE( L"strip2", EsrKit( 1 ) ), stripState2, 2 );

	CPPUNIT_ASSERT( !losRoute->isRouteSet() );
	losRoute->processRouteEvents( set<BadgeE>() );
	CPPUNIT_ASSERT( losRoute->isRouteSet() );
}

void TC_LOS_Route::noTrainRouteByLockedNBusy()
{
	unique_ptr<LOS_base> losRoute( LOS_base::fabric_from_object( ROUTE ) );

	//поездной маршрут
	losRoute->setRouteType( true );

	//открытая голова
	RouteElementState headState( HEAD );
	unique_ptr<LOS_base> losHead( LOS_base::fabric_from_object( HEAD ) );
	losHead->setOpenSignal( StateLTS::ACTIVE );
	headState.saveLOS( losHead.get() );
	losRoute->addRouteElementState( BadgeE( L"head", EsrKit( 1 ) ), headState, 0 );

	//замкнутый участок 1
	RouteElementState stripState1( STRIP );
	unique_ptr<LOS_base> losStrip1( LOS_base::fabric_from_object( STRIP ) );
	losStrip1->setLock( StateLTS::ACTIVE );
	stripState1.saveLOS( losStrip1.get() );
	losRoute->addRouteElementState( BadgeE( L"strip1", EsrKit( 1 ) ), stripState1, 1 );

	//замкнутый участок 2
	RouteElementState stripState2( STRIP );
	unique_ptr<LOS_base> losStrip2( LOS_base::fabric_from_object( STRIP ) );
	losStrip2->setLock( StateLTS::ACTIVE );
	stripState2.saveLOS( losStrip2.get() );
	losRoute->addRouteElementState( BadgeE( L"strip2", EsrKit( 1 ) ), stripState2, 2 );

	//занятый ПО-путь
	RouteElementState adStripState( STRIP );
	unique_ptr<LOS_base> adlosStrip( LOS_base::fabric_from_object( STRIP ) );
	adlosStrip->setUsed( StateLTS::ACTIVE );
	adStripState.saveLOS( adlosStrip.get() );
	BadgeE adstripBdg( L"adstrip", EsrKit( 1 ) );
	losRoute->addRouteElementState( adstripBdg, adStripState, 3 );

	CPPUNIT_ASSERT( !losRoute->isRouteSet() );
	set<BadgeE> adStripsSet;
	adStripsSet.insert( adstripBdg );
	losRoute->processRouteEvents( adStripsSet );
	CPPUNIT_ASSERT( !losRoute->isRouteSet() && !losRoute->isRouteCutting() );
}

void TC_LOS_Route::setShuntRouteByAllLocked()
{
	unique_ptr<LOS_base> losRoute( LOS_base::fabric_from_object( ROUTE ) );

	//маневровый маршрут
	losRoute->setRouteType( false );

	//открытая голова
	RouteElementState headState( HEAD );
	unique_ptr<LOS_base> losHead( LOS_base::fabric_from_object( HEAD ) );
	losHead->setOpenSignal( StateLTS::ACTIVE );
	headState.saveLOS( losHead.get() );
	losRoute->addRouteElementState( BadgeE( L"head", EsrKit( 1 ) ), headState, 0 );

	//два замкнутых участка
	RouteElementState stripState1( STRIP );
	unique_ptr<LOS_base> losStrip1( LOS_base::fabric_from_object( STRIP ) );
	losStrip1->setLock( StateLTS::ACTIVE );
	stripState1.saveLOS( losStrip1.get() );
	losRoute->addRouteElementState( BadgeE( L"strip1", EsrKit( 1 ) ), stripState1, 1 );
	RouteElementState stripState2( STRIP );
	unique_ptr<LOS_base> losStrip2( LOS_base::fabric_from_object( STRIP ) );
	losStrip2->setLock( StateLTS::ACTIVE );
	stripState2.saveLOS( losStrip2.get() );
	losRoute->addRouteElementState( BadgeE( L"strip2", EsrKit( 1 ) ), stripState2, 2 );

	CPPUNIT_ASSERT( !losRoute->isRouteSet() );
	losRoute->processRouteEvents( set<BadgeE>() );
	CPPUNIT_ASSERT( losRoute->isRouteSet() );
}

void TC_LOS_Route::setShuntRouteByLockedNBusy()
{
	unique_ptr<LOS_base> losRoute( LOS_base::fabric_from_object( ROUTE ) );

	//маневровый маршрут
	losRoute->setRouteType( false );

	//открытая голова
	RouteElementState headState( HEAD );
	unique_ptr<LOS_base> losHead( LOS_base::fabric_from_object( HEAD ) );
	losHead->setOpenSignal( StateLTS::ACTIVE );
	headState.saveLOS( losHead.get() );
	losRoute->addRouteElementState( BadgeE( L"head", EsrKit( 1 ) ), headState, 0 );

	//замкнутый участок
	RouteElementState stripState1( STRIP );
	unique_ptr<LOS_base> losStrip1( LOS_base::fabric_from_object( STRIP ) );
	losStrip1->setLock( StateLTS::ACTIVE );
	stripState1.saveLOS( losStrip1.get() );
	losRoute->addRouteElementState( BadgeE( L"strip1", EsrKit( 1 ) ), stripState1, 1 );

	//занятый ПО-путь
	RouteElementState stripState2( STRIP );
	unique_ptr<LOS_base> losStrip2( LOS_base::fabric_from_object( STRIP ) );
	losStrip2->setUsed( StateLTS::ACTIVE );
	stripState2.saveLOS( losStrip2.get() );
	BadgeE adstripBdg( L"strip2", EsrKit( 1 ) );
	losRoute->addRouteElementState( adstripBdg, stripState2, 2 );

	CPPUNIT_ASSERT( !losRoute->isRouteSet() );
	set<BadgeE> adStripsSet;
	adStripsSet.insert( adstripBdg );
	losRoute->processRouteEvents( adStripsSet );
	CPPUNIT_ASSERT( losRoute->isRouteSet() );
}