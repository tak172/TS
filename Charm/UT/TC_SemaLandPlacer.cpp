#include "stdafx.h"
#include "TC_SemaLandPlacer.h"
#include "LogicDocumentTest.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/StationManager.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LogicJoint.h"
#include "../StatBuilder/SemaLandPlacer.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicLandscape.h"
#include "../StatBuilder/SpanKit.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/GraphicLandscape.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/GraphicSemaleg.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/GraphicView.h"
#include "../StatBuilder/DrawElemInfo.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LogicSpanWay.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SemaLandPlacer );

void TC_SemaLandPlacer::setUp()
{
	TC_Project::setUp();
	axis_name = L"axis_name";
	projdoc->OpenAllPools();
	SetSpanEsr( logdoc.get(), EsrKit( 1, 2 ) );
	pair <CGraphicLandscape *, CLogicLandscape *> landscapes = CreateLandscapes( axis_name );
	gland = landscapes.first;
	lland = landscapes.second;
	double gland_hei, gland_wid;
	gland->GetSizes( gland_wid, gland_hei );
	SendLandscapeLos( landscapes.first, landscapes.second );
	projdoc->GetEditor()->StoreManipulations();
}

void TC_SemaLandPlacer::tearDown()
{
    TC_Project::tearDown();
}

pair <CGraphicLandscape *, CLogicLandscape *> TC_SemaLandPlacer::CreateLandscapes( wstring axis_name )
{
	//логический профиль
	CLogicElement * lelem = CLogicElementFactory().CreateObject( LANDSCAPE );
	lelem->SetName( L"landname" );
	plom->AddObject( lelem, logdoc.get() );
	CLogicLandscape * lland = static_cast <CLogicLandscape *>( lelem );

	lland->SetAxisName( axis_name );
	list <unsigned int> way_order_list;
	way_order_list.push_back( 1 );
	way_order_list.push_back( 2 );
	lland->SetOrderList( way_order_list );
	lland->SetBound( rwCoord( 16, 552 ), rwCoord( 21, 105 ) );
	const CLink & llandlink = lland->BuildLink( pstam->GetESR( logdoc.get() ) );

	//графический профиль
	CGraphicLandscape * gland = CGraphicEditableFactory().CreateLandscape( gdoc->GetSkin() );
	gland->SetSizes( 100, 20 );
	gland->SetLogicLink( llandlink );
	pgom->AddObject( gland, gdoc.get() );
	return make_pair( gland, lland );
}

vector <BadgeE> TC_SemaLandPlacer::CreateSpanWay( unsigned int way_num, const vector <JointInfo> & jvec )
{
	vector <BadgeE> legsBadges;
	const EsrKit & esrCode = pstam->GetESR( logdoc.get() );
	CLogicElement * last_strip = 0;
	for( auto It = jvec.cbegin(); It != jvec.cend(); ++It )
	{
		PicketingInfo pckinfo;
		pair <CLogicElement *, CLogicElement *> lstrips;
		if ( last_strip )
			lstrips.first = last_strip;
		CLogicElement * lleg = 0;
		BadgeE legBadge;
		if ( It->leg_exist )
		{
			lleg = BuildLeg( It->headTypes );
			legBadge = lleg->BuildLink( EsrCode() ).obj_badge;
		}
		legsBadges.push_back( legBadge );
		pair <CLogicElement *, CLogicElement *> legstrips = ( It->swap_leg ? make_pair( lstrips.second, lstrips.first ) : lstrips );
		CLogicElement * ljoint = BuildJoint( legstrips, lleg );
		static_cast <CLogicStrip *>( legstrips.first )->SetWaynum( way_num );
		static_cast <CLogicStrip *>( legstrips.second )->SetWaynum( way_num );
		last_strip = ( It->swap_leg ? legstrips.first : legstrips.second );
		pckinfo.SetCoord( rwRuledCoord(axis_name, It->pos) );
		static_cast <CLogicJoint *>( ljoint )->SetPicketingInfo( pckinfo );
	}
	return legsBadges;
}

void TC_SemaLandPlacer::TwoWaysThreeLegs()
{
	vector <JointInfo> jvec1, jvec2;
	jvec1.push_back( JointInfo( rwCoord( 18, 100 ), true ) );
	jvec1.push_back( JointInfo( rwCoord( 18, 500 ) ) );
	jvec1.push_back( JointInfo( rwCoord( 18, 970 ), true ) );
	auto spanLegs1 = CreateSpanWay( 1, jvec1 );
	CPPUNIT_ASSERT( spanLegs1.size() == 3 );

	jvec2.push_back( JointInfo( rwCoord( 17, 50 ), true, true ) );
	jvec2.push_back( JointInfo( rwCoord( 19, 100 ) ) );
	jvec2.push_back( JointInfo( rwCoord( 19, 110 ) ) );
	auto spanLegs2 = CreateSpanWay( 2, jvec2 );
	CPPUNIT_ASSERT( spanLegs2.size() == 3 );

	SemaLandPlacer slplacer( gland, gdoc.get(), projdoc.get() );
	set <const CGraphicElement *> glegs;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	CPPUNIT_ASSERT( glegs.size() == 3 );
	const CGraphicSemaleg * gleg11 = 0, * gleg13 = 0, * gleg21 = 0;
	for( const CGraphicElement * gelem : glegs )
	{
		const CGraphicSemaleg * gleg = static_cast <const CGraphicSemaleg *> ( gelem );
		const CLink & glink = gelem->GetLogicLink();
		auto semaBadge = glink.obj_badge;
		if (  semaBadge == spanLegs1[0] )
			gleg11 = gleg;
		else if ( semaBadge == spanLegs1[2] )
			gleg13 = gleg;
		else if ( semaBadge == spanLegs2[0] )
			gleg21 = gleg;
	}
	CPPUNIT_ASSERT( gleg11 && gleg13 && gleg21 );
	GRID_COORD way1_ord = gland->GetWayOrdinate( 0 );
	GRID_COORD way2_ord = gland->GetWayOrdinate( 1 );

	const CGridPoint & leg11_hp = gleg11->GetHotPoint();
	const CGridPoint & leg13_hp = gleg13->GetHotPoint();
	const CGridPoint & leg21_hp = gleg21->GetHotPoint();
	CPPUNIT_ASSERT( leg21_hp.x < leg11_hp.x && leg11_hp.x < leg13_hp.x );
	CPPUNIT_ASSERT( leg11_hp.y == leg13_hp.y );
	CPPUNIT_ASSERT( leg11_hp.y < way1_ord );
	CPPUNIT_ASSERT( leg21_hp.y > way2_ord );
	CPPUNIT_ASSERT( gleg21->GetAngle() == M_PI );
}

void TC_SemaLandPlacer::LegWOPicketage()
{
	vector <JointInfo> jvec;
	jvec.push_back( JointInfo( rwCoord(), true ) );
	auto spanLegs = CreateSpanWay( 1, jvec );
	SemaLandPlacer slplacer( gland, gdoc.get(), projdoc.get() );
	set <CGraphicElement *> glegs;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	CPPUNIT_ASSERT( glegs.size() == 1 );
	const CGraphicElement * gleg = *glegs.begin();
	const CGridPoint & gleg_hp = gleg->GetHotPoint();
	const CGridPoint & gland_hp = gland->GetHotPoint();
	CPPUNIT_ASSERT( gleg_hp.x == gland_hp.x );
}

void TC_SemaLandPlacer::SendLandscapeLos( CGraphicLandscape * gland, CLogicLandscape * lland )
{
	LosBasePtr losPtr( LOS_base::fabric_from_object( lland ) );
	LOS_landscape * land_los = static_cast <LOS_landscape *>( losPtr.get() );
	Landscape::Path lpath, lpath2;
	lpath.set_number( 1 );
	lpath.append_sample( Landscape::Measure( rwCoord( 10, 500 ), 2000 ), 
		Landscape::Measure( rwCoord( 20, 300 ), 3000 ), 0, "" );
	lpath2 = lpath;
	lpath2.set_number( 2 );
	land_los->insert_Path( lpath );
	land_los->insert_Path( lpath2 );
	auto gviewSkin = gdoc->GetSkin();
	DrawElemInfo deInfo( losPtr, false, UNSELECTED, false, gviewSkin.get(), nullptr );
	ScopedGraphics sgraphics( gdoc->GetView() );
	GraphicContext * gcont = const_cast <GraphicContext *>( sgraphics.Get() );
	gland->DrawElement( gcont, deInfo );
}

void TC_SemaLandPlacer::VerticalOddFirstDistribution()
{
	//пр€мой пор€док
	list <unsigned int> way_order_list;
	way_order_list.push_back( 1 );
	way_order_list.push_back( 2 );
	lland->SetOrderList( way_order_list );

	vector <JointInfo> jvec1, jvec2;
	jvec1.push_back( JointInfo( rwCoord( 18, 100 ), true ) );
	auto spanLegs1 = CreateSpanWay( 1, jvec1 );
	CPPUNIT_ASSERT( spanLegs1.size() == 1 );
	jvec2.push_back( JointInfo( rwCoord( 18, 100 ), true ) );
	auto spanLegs2 = CreateSpanWay( 2, jvec2 );
	CPPUNIT_ASSERT( spanLegs2.size() == 1 );
	GRID_COORD way_ord_1 = gland->GetWayOrdinate( 0 );
	GRID_COORD way_ord_2 = gland->GetWayOrdinate( 1 );

	SemaLandPlacer slplacer( gland, gdoc.get(), projdoc.get() );
	set <const CGraphicElement *> glegs;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	for( const CGraphicElement * gelem : glegs )
	{
		const CLink & glink = gelem->GetLogicLink();
		auto semaBadge = glink.obj_badge;
		CPPUNIT_ASSERT( semaBadge == spanLegs1[0] || semaBadge == spanLegs2[0] );
		CGridRect objrect;
		gelem->GetObjectRect( objrect );
		if ( semaBadge == spanLegs1[0] )
			CPPUNIT_ASSERT( objrect.topleft.y < way_ord_1 && objrect.rightbottom.y < way_ord_1 );
		else
			CPPUNIT_ASSERT( objrect.topleft.y > way_ord_2 && objrect.rightbottom.y > way_ord_2 );
	}
}

void TC_SemaLandPlacer::VerticalEvenFirstDistribution()
{
	//пр€мой пор€док
	list <unsigned int> way_order_list;
	way_order_list.push_back( 2 );
	way_order_list.push_back( 1 );
	lland->SetOrderList( way_order_list );

	vector <JointInfo> jvec1, jvec2;
	jvec1.push_back( JointInfo( rwCoord( 18, 100 ), true ) );
	auto spanLegs1 = CreateSpanWay( 1, jvec1 );
	jvec2.push_back( JointInfo( rwCoord( 18, 100 ), true ) );
	auto spanLegs2 = CreateSpanWay( 2, jvec2 );
	GRID_COORD way_ord_1 = gland->GetWayOrdinate( 0 );
	GRID_COORD way_ord_2 = gland->GetWayOrdinate( 1 );

	SemaLandPlacer slplacer( gland, gdoc.get(), projdoc.get() );
	set <const CGraphicElement *> glegs;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	for( const CGraphicElement * gelem : glegs )
	{
		const CLink & glink = gelem->GetLogicLink();
		auto semaBadge = glink.obj_badge;
		CPPUNIT_ASSERT( semaBadge == spanLegs1[0] || semaBadge == spanLegs2[0] );
		CGridRect objrect;
		gelem->GetObjectRect( objrect );
		if ( semaBadge == spanLegs1[0] )
			CPPUNIT_ASSERT( objrect.topleft.y > way_ord_2 && objrect.rightbottom.y > way_ord_2 );
		else
			CPPUNIT_ASSERT( objrect.topleft.y < way_ord_1 && objrect.rightbottom.y < way_ord_1 );
	}
}

void TC_SemaLandPlacer::OutsideLeg()
{
	JointInfo jarr[] = { JointInfo( rwCoord( 11, 0 ), true ), JointInfo( rwCoord( 12, 0 ), true ), JointInfo( rwCoord( 13, 0 ), true ) };
	CreateSpanWay( 1, vector <JointInfo>( &jarr[0], &jarr[sizeof(jarr) / sizeof(jarr[0])] ) );
	SpanKitPtr spanKitPtr = plom->GetSpanKit( logdoc.get() );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & span_ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( span_ways.size() == 1 );
	const LogicSpanWay & span_way = span_ways.front();
	list <const CLogicElement *> & strips = span_way.Strips();
	CPPUNIT_ASSERT( strips.size() == 4 );

	CLogicDocumentTest statdoc;
	projdoc->Include( &statdoc );
	projdoc->OpenAllPools();
	SetStationEsr( &statdoc, EsrKit( 100 ) );
	CLogicElement * last_strip = span_way.PicketageIsIncreasing( axis_name ) ? plom->Release( strips.back(), logdoc.get() )
		: plom->Release( strips.front(), logdoc.get() );

	//добавление в конец списка участков еще одного, ведущего на другую станцию
	CLogicElement * lleg = BuildLeg( TRAIN );
	plom->ChangeObjectName( lleg, L"border_leg", logdoc.get(), EsrCode() );
	pair <CLogicElement *, CLogicElement *> border_strips( nullptr, last_strip );
	BuildJoint( border_strips, lleg, &statdoc );
	projdoc->GetEditor()->StoreManipulations();

	SemaLandPlacer slplacer( gland, gdoc.get(), projdoc.get() );
	set <const CGraphicElement *> glegs;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	CPPUNIT_ASSERT( glegs.size() == 4 );
	for ( const CGraphicElement * gleg : glegs )
	{
		double angle = gleg->GetAngle();
		CPPUNIT_ASSERT( angle == 0 || angle == M_PI );
		const CLink & llink = gleg->GetLogicLink();
		if ( angle == M_PI )
			CPPUNIT_ASSERT( llink.obj_badge.str() == L"border_leg" );
	}
}

void TC_SemaLandPlacer::InvitationHeadSort()
{
	vector <JointInfo> jvec;
	vector <HEAD_TYPE> htypes;
	htypes.push_back( TRAIN );
	htypes.push_back( INVITATION );
	jvec.push_back( JointInfo( rwCoord( 18, 100 ), true, false, htypes ) );
	auto spanLegs = CreateSpanWay( 1, jvec );
	CPPUNIT_ASSERT( spanLegs.size() == 1 );

	SemaLandPlacer slplacer( gland, gdoc.get(), projdoc.get() );
	set <const CGraphicElement *> glegs;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	CPPUNIT_ASSERT( glegs.size() == 1 );
	auto gleg = *glegs.cbegin();
	auto gheads = gleg->GetLinks( HEAD );
	CPPUNIT_ASSERT( gheads.size() == 2 );
	auto gheadNear = ( gheads.front().glink_ptr->GetHotPoint() == gleg->GetHotPoint() ? gheads.front().glink_ptr : gheads.back().glink_ptr );
	auto gheadFar = ( gheadNear == gheads.front().glink_ptr ? gheads.back().glink_ptr : gheads.front().glink_ptr );
	CPPUNIT_ASSERT( gheadNear->GetType() == INVITATION );
	CPPUNIT_ASSERT( gheadFar->GetType() == TRAIN );
	auto linkNear = gheadNear->GetLogicLink();
	auto linkFar = gheadFar->GetLogicLink();
	CPPUNIT_ASSERT( linkNear.obj_badge != linkFar.obj_badge && !linkNear.obj_badge.empty() && !linkFar.obj_badge.empty() );
	auto lheadNear = plom->GetObjectByBadge<const CLogicElement *>( linkNear.obj_badge );
	CPPUNIT_ASSERT( lheadNear );
	auto lheadFar = plom->GetObjectByBadge<const CLogicElement *>( linkFar.obj_badge );
	CPPUNIT_ASSERT( lheadFar );
	CPPUNIT_ASSERT( lheadNear->GetType() == INVITATION );
	CPPUNIT_ASSERT( lheadFar->GetType() == TRAIN );
}