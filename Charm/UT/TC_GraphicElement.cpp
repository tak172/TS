#include "stdafx.h"
#include "TC_GraphicElement.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/GObjectManager.h"
#include "GraphicDocumentTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GraphicElement );

using namespace std;

void TC_GraphicElement::Move()
{
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * gstrip = gstrip_sample->Clone();
	gstrip->SetHotPoint( CGridPoint( 10, 10 ) );
	pgom->AddObject( gstrip, gdoc.get() );
	const CGridPoint & hp = gstrip->GetHotPoint();
	CGridSize shift( 5, -2 );
	gstrip->Move( shift, false );
	const CGridPoint & shifted_hp = gstrip->GetHotPoint();
	CPPUNIT_ASSERT( shifted_hp.x == ( GRID_COORD ) ( hp.x + shift.w ) && 
		shifted_hp.y == ( GRID_COORD ) ( hp.y + shift.h ) );
}

void TC_GraphicElement::LegOverlapping()
{
	//мачта 1
	CGraphicElement * gleg1 = BuildSemalegKit( CGridPoint( 10, 10 ), 1 );
	gleg1->SetAngle( -M_PI );
	const CGridPoint & gleg1_hp = gleg1->GetHotPoint();
	double leg_width, leg_height;
	gleg1->GetSizes( leg_width, leg_height );

	//мачта 2 (над мачтой 1)
	CGraphicElement * gleg2 = BuildSemalegKit( CGridPoint( gleg1_hp.x, gleg1_hp.y - 2 * leg_height ), 1 );
	gleg2->SetAngle( -M_PI );

	CPPUNIT_ASSERT( !gleg1->OverlappingBan( gleg2 ) );
}

void TC_GraphicElement::RevertedStripHit()
{
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * gstrip = gstrip_sample->Clone();
	gstrip->SetHotPoint( CGridPoint( 10, 10 ) );
	gstrip->SetSizes( -10, 0 );
	pgom->AddObject( gstrip, gdoc.get() );
	vector <const CGraphicElement *> hitted;
	pgom->HitPoint( CGridPoint( 5, 10 ), gdoc.get(), hitted );
	CPPUNIT_ASSERT( !hitted.empty() );
}