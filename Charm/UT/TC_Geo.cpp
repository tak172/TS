#include "stdafx.h"
#include "TC_Geo.h"
#include "../StatBuilder/Geo.h"
#include "../StatBuilder/GraphicGrid.h"

using namespace std;
using namespace Geo;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Geo );

void TC_Geo::range_traditional()
{
    CPPUNIT_ASSERT( CoordRange(1.1, 2.2)==CoordRange(2.2,1.1) );

    CoordRange a(1.0, 3.0);
    CPPUNIT_ASSERT( !a.include(0.7) );
    CPPUNIT_ASSERT( a.include(1.0) );
    CPPUNIT_ASSERT( a.include(1.5) );
    CPPUNIT_ASSERT( a.include(3.0) );
    CPPUNIT_ASSERT( !a.include(3.1) );

    CPPUNIT_ASSERT( a.traditional() );
    CPPUNIT_ASSERT( !a.singular() );
    CPPUNIT_ASSERT( !a.invalid() );
}

void TC_Geo::range_singular()
{
    CoordRange a(4.0, 4.0);
    CPPUNIT_ASSERT( !a.include(3.99) );
    CPPUNIT_ASSERT( a.include(4.00) );
    CPPUNIT_ASSERT( !a.include(4.01) );

    CPPUNIT_ASSERT( !a.traditional() );
    CPPUNIT_ASSERT( a.singular() );
    CPPUNIT_ASSERT( !a.invalid() );
}

void TC_Geo::range_and()
{
	CPPUNIT_ASSERT( CoordRange::intersect( CoordRange(2.0, 5.0), CoordRange(3.0, 7.0) )==CoordRange(3.0,5.0) );
    CPPUNIT_ASSERT( CoordRange::intersect( CoordRange(3.0, 7.0), CoordRange(2.0, 5.0) )==CoordRange(3.0,5.0) );

    CPPUNIT_ASSERT( CoordRange::intersect( CoordRange(3.0, 7.0), CoordRange(7.0, 9.0) )==CoordRange(7.0,7.0) );
    CPPUNIT_ASSERT( CoordRange::intersect( CoordRange(7.0, 9.0), CoordRange(3.0, 7.0) )==CoordRange(7.0,7.0) );

    CoordRange inv = CoordRange::intersect( CoordRange(1.0, 2.0), CoordRange(7.0, 9.0) );
    CPPUNIT_ASSERT( inv.invalid() );
    CPPUNIT_ASSERT( !inv.singular() );
    CPPUNIT_ASSERT( !inv.traditional() );
}

void TC_Geo::intersectStrip_aslant_touch()
{
    // касание под углом
    CGridLine a( CGridPoint(1., 1.), CGridPoint(1., 5.) );
    CGridLine b( CGridPoint(1., 1.), CGridPoint(3., 1.) );
    CGridPoint pt;
	CPPUNIT_ASSERT( Geo::INTERSECTED == intersectStrips(a,b,pt) );
    CPPUNIT_ASSERT(pt==CGridPoint(1.,1.));
}
void TC_Geo::intersectStrip_aslant_inside()
{
    // пересечение под углом
    CGridLine a( CGridPoint(1., 1.), CGridPoint(3., 3.) );
    CGridLine b( CGridPoint(3., 1.), CGridPoint(1., 3.) );
    CGridPoint pt;
    CPPUNIT_ASSERT( Geo::INTERSECTED == intersectStrips(a,b,pt) );
    CPPUNIT_ASSERT(pt==CGridPoint(2.,2.));
}
void TC_Geo::intersectStrip_aslant_none()
{
    // нет пересечения под углом
    CGridLine a( CGridPoint(1., 1.), CGridPoint(3., 3.) );
    CGridLine b( CGridPoint(99., 1.), CGridPoint(1., 99.) );
    CGridPoint pt;
    CPPUNIT_ASSERT( Geo::OUTINTERSECTED == intersectStrips(a,b,pt) );
}
void TC_Geo::intersectStrip_same_touch()
{
    // касание на одной линии
    CGridLine a( CGridPoint(1., 1.), CGridPoint(1., 5.) );
    CGridLine b( CGridPoint(1., 5.), CGridPoint(1., 7.) );
    CGridPoint pt;
    CPPUNIT_ASSERT( Geo::LINED_INTERSECTED == intersectStrips(a,b,pt) );
    CPPUNIT_ASSERT(pt==CGridPoint(1.,5.));
}
void TC_Geo::intersectStrip_same_lap()
{
    // наложение коллинеарных
    CGridLine a( CGridPoint(1., 1.), CGridPoint(5., 1.) );
    CGridLine b( CGridPoint(2., 1.), CGridPoint(3., 1.) );
    CGridPoint pt;
	CPPUNIT_ASSERT( Geo::OVERLAPPED == intersectStrips(a,b,pt) );
}

void TC_Geo::intersectStrip_same_none()
{
    // на одной линии с промежутком 
    CGridLine a( CGridPoint(1., 1.), CGridPoint(2., 1.) );
    CGridLine b( CGridPoint(7., 1.), CGridPoint(8., 1.) );
    CGridPoint pt;
	CPPUNIT_ASSERT( Geo::LINED_NONOVERLAPPED == intersectStrips(a,b,pt) );
}
void TC_Geo::intersectStrip_parallel_none()
{
    // параллельные непересекающиеся
    CGridLine a( CGridPoint(1., 1.), CGridPoint(2., 1.) );
    CGridLine b( CGridPoint(1., 3.), CGridPoint(2., 3.) );
    CGridPoint pt;
	CPPUNIT_ASSERT( Geo::PARALLEL == intersectStrips(a,b,pt) );
}
void TC_Geo::onStrip_edge()
{
    CGridLine a( CGridPoint(1., 1.), CGridPoint(3., 3.) );
    CGridPoint pt(3., 3.);
    CPPUNIT_ASSERT( pointOnSegment( a, pt ) );
}
void TC_Geo::onStrip_inside()
{
    CGridLine a( CGridPoint(3., 3.), CGridPoint(1., 1.) );
    CGridPoint pt(2., 2.);
    CPPUNIT_ASSERT( pointOnSegment( a, pt ) );
}
void TC_Geo::onStrip_too_far()
{
    CGridLine a( CGridPoint(1., 1.), CGridPoint(3., 3.) );
    CGridPoint pt(0., 0.);
    CPPUNIT_ASSERT( !pointOnSegment( a, pt ) );
}
void TC_Geo::onStrip_far_side()
{
    CGridLine a( CGridPoint(3., 3.), CGridPoint(1., 1.) );
    CGridPoint pt(2., 1.);
    CPPUNIT_ASSERT( !pointOnSegment( a, pt ) );
}

void TC_Geo::LineEndPixelAlign()
{
	CGraphicGrid grid;
	grid.SetSize( 8 );
	CGridPoint gpt( 10.3, 10.3 );
	//толщина линии 2 пикселя (0.25)
	Geo::LineEndPixelAlign( &grid, gpt, 0.25 );
	//точка должна сместиться в позицию (81,5; 81,5) (в пикселях)
	CPPUNIT_ASSERT( round( gpt.x, 4 ) == 10.3125 );
	CPPUNIT_ASSERT( round( gpt.y, 4 ) == 10.3125 );
	gpt = CGridPoint( 10.3, 10.3 );
	//толщина линии 3 пикселя (0.375)
	Geo::LineEndPixelAlign( &grid, gpt, 0.375 );
	//точка должна сместиться в позицию (82; 82) (в пикселях)
	CPPUNIT_ASSERT( round( gpt.x, 2 ) == 10.25 );
	CPPUNIT_ASSERT( round( gpt.y, 2 ) == 10.25 );
}

void TC_Geo::AlmostVertLineEllipseIntersection()
{
	CGridLine tst_line( CGridPoint( 0.80000001, 4 ), CGridPoint( 0.8, 4.6 ) );
	CGridPoint respt1, respt2;
	int int_count = Geo::intersectLineNEllipse( tst_line, CGridPoint( 1, 4 ), 0.6, respt1, respt2 );
	CPPUNIT_ASSERT( int_count == 2 );
}

void TC_Geo::LineEllipseIntersection()
{
	CGridLine tst_line( CGridPoint( 3, 0 ), CGridPoint( 0, 3 ) );
	CGridPoint respt1, respt2;
	double elradius = sqrt( 2 ) / 2;
	Geo::intersectLineNEllipse( tst_line, CGridPoint( 1.5, 1.5 ), elradius, respt1, respt2 );
	if ( respt1 == CGridPoint( 1, 2 ) )
		CPPUNIT_ASSERT( respt2 == CGridPoint( 2, 1 ) );
	else
	{
		CPPUNIT_ASSERT( respt1 == CGridPoint( 2, 1 ) );
		CPPUNIT_ASSERT( respt2 == CGridPoint( 1, 2 ) );
	}
}