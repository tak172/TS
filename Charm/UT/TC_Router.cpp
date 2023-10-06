#include "stdafx.h"
#include "TC_Router.h"
#include "ProjectDocumentTest.h"
#include "LogicDocumentTest.h"
#include "../helpful/EsrKit.h"
#include "../StatBuilder/Router.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/LogicStrip.h"
#include "../helpful/Informator.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LogicView.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Router );

using namespace std;

void TC_Router::SimpleBlindRoute()
{
	//стартова€ мачта
	CLogicElement * lleg = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> strips_near_joint;
	BuildJoint( strips_near_joint, lleg );

	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	RouteContentPtr route = routes.front();
	CPPUNIT_ASSERT( route->GetKind() == BLINDROUTE );
	CPPUNIT_ASSERT( route->Size() == 3 );
	const list <const CLogicElement *> & route_path = route->GetPath();
	CPPUNIT_ASSERT( route_path.back() == strips_near_joint.second );
}

void TC_Router::SimpleSemaRoute()
{
	//стартова€ мачта
	CLogicElement * lleg = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> strips_near_joint1;
	CLogicElement * ljoint = BuildJoint( strips_near_joint1, lleg );

	//оконечна€ мачта
	CLogicElement * lleg2 = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> strips_near_joint2( strips_near_joint1.second, 0 );
	CLogicElement * ljoint2 = BuildJoint( strips_near_joint2, lleg2 );

	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 2 );
	for( RouteContentPtr routePtr : routes )
	{
		CPPUNIT_ASSERT( routePtr->GetKind() == SEMAROUTE || routePtr->GetKind() == BLINDROUTE );
		if ( routePtr->GetKind() == SEMAROUTE )
		{
			CPPUNIT_ASSERT( routePtr->Size() == 4 );
			CPPUNIT_ASSERT( routePtr->PostHead() == ljoint );
			CPPUNIT_ASSERT( routePtr->PrevTail() == strips_near_joint1.second );
			CPPUNIT_ASSERT( routePtr->Tail() == ljoint2 );
		}
		else
		{
			CPPUNIT_ASSERT( routePtr->Size() == 3 );
			CPPUNIT_ASSERT( routePtr->PostHead() == ljoint2 );
			CPPUNIT_ASSERT( routePtr->Tail() == strips_near_joint2.second );
		}
	}
}

void TC_Router::OutrouteAlienEsr()
{
	//стартова€ мачта
	CLogicElement * lleg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> strips_near_joint1;
	CLogicElement * ljoint = BuildJoint( strips_near_joint1, lleg );

	//чужеродный участок
	LogElemPtr alien_strip( BuildObject( STRIP, false ) );
	EsrKit alienEsr( 111, 222 );
	CPPUNIT_ASSERT( alienEsr != EsrCode() );	

	//оконечна€ мачта
	CLogicElement * lleg2 = BuildLeg( TRAIN );
	CLogicElement * ljoint2 = BuildObject( JOINT );

	const CLink & finalStripLink = strips_near_joint1.second->BuildLink( EsrCode(), CLink::FROM );
	const CLink & alienStripLink = alien_strip->BuildLink( alienEsr, CLink::TO );
	plim->Join( ljoint2, finalStripLink );
	plim->Join( ljoint2, alienStripLink );
	plim->Join( lleg2, finalStripLink );
	plim->Join( lleg2, alienStripLink );

	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	RouteContentPtr routePtr = routes.front();
	CPPUNIT_ASSERT( routePtr->GetKind() == OUTROUTE );
	CPPUNIT_ASSERT( routePtr->Size() == 4 );
	CPPUNIT_ASSERT( routePtr->PostHead() == ljoint );
	CPPUNIT_ASSERT( routePtr->PrevTail() == strips_near_joint1.second );
	CPPUNIT_ASSERT( routePtr->Tail() == ljoint2 );
}

void TC_Router::OutrouteApproachingType()
{
	//стартова€ мачта
	CLogicElement * lleg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> strips_near_joint1;
	CLogicElement * ljoint = BuildJoint( strips_near_joint1, lleg );
	pair <CLogicElement *, CLogicElement *> strips_near_joint2( strips_near_joint1.second, 0 );
	CLogicElement * ljoint2 = BuildJoint( strips_near_joint2 );
	//последний участок входит в тот же документ, но имеет тип приближени€
	strips_near_joint2.second->SetType( APPROACHING );

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	RouteContentPtr routePtr = routes.front();
	CPPUNIT_ASSERT( routePtr->GetKind() == OUTROUTE );
	CPPUNIT_ASSERT( routePtr->Size() == 4 );
	CPPUNIT_ASSERT( routePtr->PostHead() == ljoint );
	CPPUNIT_ASSERT( routePtr->PrevTail() == strips_near_joint1.second );
	CPPUNIT_ASSERT( routePtr->Tail() == ljoint2 );
}

void TC_Router::OutrouteApproachingTypeTwice()
{
	//стартова€ мачта
	CLogicElement * lleg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> strips_near_joint1;
	CLogicElement * ljoint = BuildJoint( strips_near_joint1, lleg );
	pair <CLogicElement *, CLogicElement *> strips_near_joint2( strips_near_joint1.second, 0 );
	CLogicElement * ljoint2 = BuildJoint( strips_near_joint2 );
	pair <CLogicElement *, CLogicElement *> strips_near_joint3( strips_near_joint2.second, 0 );
	CLogicElement * ljoint3 = BuildJoint( strips_near_joint3 );

	//последние два участка вход€т в тот же документ, но имеют тип приближени€
	strips_near_joint3.first->SetType( APPROACHING );
	strips_near_joint3.second->SetType( APPROACHING );

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	RouteContentPtr routePtr = routes.front();
	CPPUNIT_ASSERT( routePtr->GetKind() == OUTROUTE );
	CPPUNIT_ASSERT( routePtr->Size() == 4 );
	CPPUNIT_ASSERT( routePtr->PostHead() == ljoint );
	CPPUNIT_ASSERT( routePtr->PrevTail() == strips_near_joint1.second );
	CPPUNIT_ASSERT( routePtr->Tail() == ljoint2 );
}

void TC_Router::DoubleLegsInJointRoute()
{
	CLogicElement * lleg = BuildLeg( TRAIN );
	CLogicElement * lleg2 = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> strips_near_joint;
	/*CLogicElement * ljoint =*/ BuildJoint( strips_near_joint, lleg );
	LinkLeg( lleg2, make_pair( strips_near_joint.second, strips_near_joint.first ) );

	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	CPPUNIT_ASSERT( routes.front()->GetKind() == BLINDROUTE );
	CPPUNIT_ASSERT( routes.front()->Size() == 3 );
}

void TC_Router::BlindRouteToOppositeLeg()
{
	//создаем две противонаправленные мачты
	CLogicElement * leg1 = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> strips_near_joint1;
	/*CLogicElement * joint1 =*/ BuildJoint( strips_near_joint1, leg1 );
	CLogicElement * leg2 = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> strips_near_joint2;
	/*CLogicElement * joint2 =*/ BuildJoint( strips_near_joint2, leg2 );

	//помещаем между ними стык
	/*CLogicElement * middle_joint =*/ BuildJoint( make_pair( strips_near_joint1.second, strips_near_joint2.second ) );

	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 2 );
	CPPUNIT_ASSERT( routes.front()->GetKind() == BLINDROUTE );
	CPPUNIT_ASSERT( routes.back()->GetKind() == BLINDROUTE );
	CPPUNIT_ASSERT( routes.front()->Size() == 7 && routes.back()->Size() == 7 );
}

void TC_Router::ArrDepPermission()
{
	//мачты:
	CLogicElement * startleg = BuildLeg( TRAIN );
	CLogicElement * endleg = BuildLeg( TRAIN );

	//участки:
	pair <CLogicElement *, CLogicElement *> strips12( BuildObject( STRIP ), BuildObject( STRIP ) );
	pair <CLogicElement *, CLogicElement *> strips34( BuildObject( STRIP ), BuildObject( STRIP ) );
	pair <CLogicElement *, CLogicElement *> strips56( BuildObject( STRIP ), BuildObject( STRIP ) );
	pair <CLogicElement *, CLogicElement *> strips78( BuildObject( STRIP ), BuildObject( STRIP ) );

	//стыки:
	BuildJoint( strips12, startleg );
	BuildJoint( make_pair( strips12.second, strips34.first ) );
	BuildJoint( strips34 );
	BuildJoint( make_pair( strips34.second, strips56.first ) );
	BuildJoint( strips56 );
	BuildJoint( make_pair( strips56.second, strips78.first ) );
	BuildJoint( strips78, endleg );

	//назначение признака ѕќ пути
	static_cast <CLogicStrip *>( strips34.first )->SetWaynum( 1 );
	static_cast <CLogicStrip *>( strips56.first )->SetWaynum( 1 );

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
}

void TC_Router::ArrDepForbidness()
{
	//мачты:
	CLogicElement * startleg = BuildLeg( TRAIN );
	CLogicElement * endleg = BuildLeg( TRAIN );

	//участки:
	vector <CLogicElement *> strips;
	strips.reserve( 13 );
	for ( auto i = 0; i < 13; ++i )
		strips.push_back( BuildObject( STRIP ) );
	//назначение признака ѕќ пути
	static_cast <CLogicStrip *>( strips[2] )->SetWaynum( 1 );
	static_cast <CLogicStrip *>( strips[10] )->SetWaynum( 1 );

	//стыки:
	BuildJoint( make_pair( strips[0], strips[1] ), startleg );
	BuildJoint( make_pair( strips[1], strips[2] ) );
	BuildJoint( make_pair( strips[2], strips[3] ) );
	BuildJoint( make_pair( strips[3], strips[4] ) );
	BuildJoint( make_pair( strips[5], strips[7] ) );
	BuildJoint( make_pair( strips[9], strips[10] ) );
	BuildJoint( make_pair( strips[10], strips[11] ) );
	BuildJoint( make_pair( strips[11], strips[12] ), endleg );

	//стрелки
	TC_Logic::BuildSwitchKit( strips[4], strips[6], strips[5] );
	TC_Logic::BuildSwitchKit( strips[9], strips[8], strips[7] );

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.empty() );
}

void TC_Router::RouteThroughSwitch()
{
	//мачты:
	CLogicElement * startleg = BuildLeg( TRAIN );
	CLogicElement * endleg_valid = BuildLeg( TRAIN );
	CLogicElement * endleg_invalid = BuildLeg( TRAIN );

	//участки:
	vector <CLogicElement *> strips;
	strips.reserve( 6 );
	for ( auto i = 0; i < 6; ++i )
		strips.push_back( BuildObject( STRIP ) );

	//св€зываем один из участков со множеством линкуемых объектов (кроме стыка и стрелки)
	vector <ELEMENT_CLASS> strip_linkable;
	CInformator * pI = CInformator::Instance();
	pI->GetLinkableClasses( STRIP, strip_linkable );
	const CLink & noised_strip_link = strips[4]->BuildLink( EsrCode() );
	for( ELEMENT_CLASS elclass : strip_linkable )
	{
		if ( elclass != SWITCH && elclass != JOINT )
		{
			CLogicElement * noise_object = BuildObject( elclass );
			plim->Join( noise_object, noised_strip_link );
		}
	}

	//стыки:
	BuildJoint( make_pair( strips[0], strips[1] ), startleg );
	BuildJoint( make_pair( strips[2], strips[3] ), endleg_valid );
	BuildJoint( make_pair( strips[4], strips[5] ), endleg_invalid );

	//стрелки
	TC_Logic::BuildSwitchKit( strips[2], strips[1], strips[4] );

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
}

void TC_Router::InterstationsSemaroute()
{
	CLogicDocumentTest aliendoc;
	projdoc->Include( &aliendoc );
	projdoc->OpenAllPools();
	auto srcLeg = BuildLeg( TRAIN );

	const EsrKit & native_code = EsrCode();
	EsrKit alienCode( native_code.getTerm() + 1 );
	SetStationEsr( &aliendoc, alienCode );
	vector <HEAD_TYPE> destHeads;
	destHeads.push_back( TRAIN );
	destHeads.push_back( SHUNTING );
	auto destLeg = BuildLeg( destHeads, &aliendoc );

	pair <CLogicElement *, CLogicElement *> strips0;
	BuildJoint( strips0, srcLeg ); //стык в начале
	pair <CLogicElement *, CLogicElement *> strips1;
	BuildJoint( strips1, destLeg, &aliendoc ); //стык на другой станции
	pair <CLogicElement *, CLogicElement *> midstrips( strips0.second, strips1.first );
	BuildJoint( midstrips );

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	auto routeContent = routes.front();
	CPPUNIT_ASSERT( routeContent->GetKind() == ROUTE_KIND::SEMAROUTE );
	const auto & routepath = routeContent->GetPath();
	vector <const CLogicElement *> lstrips;
	for ( auto lelem : routepath )
	{
		if ( lelem->GetClass() == STRIP )
			lstrips.push_back( lelem );
	}
	CPPUNIT_ASSERT( lstrips.size() == 2 );
	CPPUNIT_ASSERT( lstrips[0] == strips0.second );
	CPPUNIT_ASSERT( lstrips[1] == midstrips.second );
}

void TC_Router::WOSemaStationRouteForbid()
{
	//строим схему станци€ ј - станци€ Ѕ (без светофоров)...
	//...и пытаемс€ построить маршрут сквозь обе станции.
	//маршрут должен остановитьс€ на границе
	CLogicDocumentTest aliendoc;
	projdoc->Include( &aliendoc );
	projdoc->OpenAllPools();

	const EsrKit & native_code = EsrCode();
	EsrKit alienCode( native_code.getTerm() + 1 );
	SetStationEsr( &aliendoc, alienCode );

	auto srcLeg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> stripsA;
	BuildJoint( stripsA, srcLeg ); //стык в начале
	pair <CLogicElement *, CLogicElement *> stripsB;
	BuildJoint( stripsB, nullptr, &aliendoc );
	pair <CLogicElement *, CLogicElement *> midstrips( stripsA.second, stripsB.first );
	BuildJoint( midstrips ); //стык между станци€ми

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	auto routeContent = routes.front();
	CPPUNIT_ASSERT( routeContent->GetKind() == ROUTE_KIND::OUTROUTE );
	const auto & routepath = routeContent->GetPath();
	vector <const CLogicElement *> lstrips;
	for ( auto lelem : routepath )
	{
		if ( lelem->GetClass() == STRIP )
			lstrips.push_back( lelem );
	}
	CPPUNIT_ASSERT( lstrips.size() == 1 );
	CPPUNIT_ASSERT( lstrips[0] == stripsA.second );
}

void TC_Router::WOSemaStationRouteForbid2()
{
	//строим схему станци€ ј - станци€ Ѕ (без светофоров) со встречным светофором на стыке...
	//...и пытаемс€ построить маршрут сквозь обе станции.
	//маршрут должен остановитьс€ на границе
	CLogicDocumentTest aliendoc;
	projdoc->Include( &aliendoc );
	projdoc->OpenAllPools();

	const EsrKit & native_code = EsrCode();
	EsrKit alienCode( native_code.getTerm() + 1 );
	SetStationEsr( &aliendoc, alienCode );

	auto srcLeg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> stripsA;
	BuildJoint( stripsA, srcLeg ); //стык в начале
	pair <CLogicElement *, CLogicElement *> stripsB;
	BuildJoint( stripsB, nullptr, &aliendoc );
	auto midLeg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> midstrips( stripsB.first, stripsA.second );
	BuildJoint( midstrips, midLeg ); //стык между станци€ми со встречным светофором

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	auto routeContent = routes.front();
	CPPUNIT_ASSERT( routeContent->GetKind() == ROUTE_KIND::OUTROUTE );
	const auto & routepath = routeContent->GetPath();
	vector <const CLogicElement *> lstrips;
	for ( auto lelem : routepath )
	{
		if ( lelem->GetClass() == STRIP )
			lstrips.push_back( lelem );
	}
	CPPUNIT_ASSERT( lstrips.size() == 1 );
	CPPUNIT_ASSERT( lstrips[0] == stripsA.second );
}

void TC_Router::WOSemaStationRouteUnforbid()
{
	//строим схему станци€ ј - станци€ Ѕ (без светофоров) и строим тупиковый маршрут с границы станций вглубь станции ј
	CLogicDocumentTest aliendoc;
	projdoc->Include( &aliendoc );
	projdoc->OpenAllPools();

	const EsrKit & native_code = EsrCode();
	EsrKit alienCode( native_code.getTerm() + 1 );
	SetStationEsr( &aliendoc, alienCode );

	auto srcLeg = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> stripsBA;
	BuildJoint( stripsBA, srcLeg ); //стык на границе станций
	pair <CLogicElement *, CLogicElement *> stripsA( nullptr, stripsBA.second );
	pair <CLogicElement *, CLogicElement *> stripsB( stripsBA.first, nullptr );
	BuildJoint( stripsA );
	BuildJoint( stripsB, nullptr, &aliendoc );

	//проверка
	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( routes.size() == 1 );
	auto routeContent = routes.front();
	CPPUNIT_ASSERT( routeContent->GetKind() == ROUTE_KIND::BLINDROUTE );
	const auto & routepath = routeContent->GetPath();
	vector <const CLogicElement *> lstrips;
	for ( auto lelem : routepath )
	{
		if ( lelem->GetClass() == STRIP )
			lstrips.push_back( lelem );
	}
	CPPUNIT_ASSERT( lstrips.size() == 2 );
	CPPUNIT_ASSERT( lstrips[0] == stripsA.second );
	CPPUNIT_ASSERT( lstrips[1] == stripsA.first );
}

void TC_Router::InterstatBoundSemaRoutes()
{
	CLogicDocumentTest aliendoc;
	projdoc->Include( &aliendoc );
	projdoc->OpenAllPools();

	const EsrKit & native_code = EsrCode();
	EsrKit alienCode( native_code.getTerm() + 1 );
	SetStationEsr( &aliendoc, alienCode );

	vector<HEAD_TYPE> htypes;
	htypes.push_back( TRAIN );
	htypes.push_back( SHUNTING );
	auto srcLeg = BuildLeg( htypes );
	pair <CLogicElement *, CLogicElement *> strips12( BuildObject( STRIP, true, &aliendoc ), BuildObject( STRIP ) );
	BuildJoint( strips12, srcLeg ); //стык на границе станций

	auto destLeg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> strips34;
	BuildJoint( strips34, destLeg );
	auto lswitch = BuildObject( SWITCH );

	const CLink & switchBaseLink = lswitch->BuildLink( native_code, CLink::BASE );
	const CLink & switchPlusLink = lswitch->BuildLink( native_code, CLink::PLUS );
	plim->Join( strips12.second, switchBaseLink );
	plim->Join( strips34.first, switchPlusLink );

	list <RouteContentPtr> routes;
	router.GetAllRoutes( routes, logdoc.get() );
	CPPUNIT_ASSERT( !routes.empty() );
	CPPUNIT_ASSERT( routes.size() == 2 );
	CPPUNIT_ASSERT( any_of( routes.cbegin(), routes.cend(), []( RouteContentPtr routePtr ){ return routePtr->GetKind() == BLINDROUTE; } ) );
	CPPUNIT_ASSERT( any_of( routes.cbegin(), routes.cend(), []( RouteContentPtr routePtr ){ return routePtr->GetKind() == SEMAROUTE; } ) );
	for ( auto routePtr : routes )
	{
		const auto & rpath = routePtr->GetPath();
		auto lastStripIt = find_if( rpath.crbegin(), rpath.crend(), []( const CLogicElement * lelem ){
			return lelem->GetClass() == STRIP;
		} );
		CPPUNIT_ASSERT( lastStripIt != rpath.crend() );
		auto lastStrip = *lastStripIt;
		CPPUNIT_ASSERT( lastStrip->GetClass() == STRIP );
		if ( routePtr->GetKind() == BLINDROUTE )
			CPPUNIT_ASSERT( lastStrip == strips34.second );
		else
			CPPUNIT_ASSERT( lastStrip == strips34.first );
	}
}