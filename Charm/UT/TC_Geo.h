#ifndef TC_GEO_H
#define TC_GEO_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка классов пространства Geo
 */

class TC_Geo : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Geo );
  CPPUNIT_TEST( range_traditional );
  CPPUNIT_TEST( range_singular );
  CPPUNIT_TEST( range_and );
  CPPUNIT_TEST( intersectStrip_aslant_touch );
  CPPUNIT_TEST( intersectStrip_aslant_inside );
  CPPUNIT_TEST( intersectStrip_aslant_none );
  CPPUNIT_TEST( intersectStrip_same_touch );
  CPPUNIT_TEST( intersectStrip_same_lap );
  CPPUNIT_TEST( intersectStrip_same_none );
  CPPUNIT_TEST( intersectStrip_parallel_none );
  CPPUNIT_TEST( onStrip_edge );
  CPPUNIT_TEST( onStrip_inside );
  CPPUNIT_TEST( onStrip_too_far );
  CPPUNIT_TEST( onStrip_far_side );
  CPPUNIT_TEST( LineEndPixelAlign );
  CPPUNIT_TEST( AlmostVertLineEllipseIntersection );
  CPPUNIT_TEST( LineEllipseIntersection );
  CPPUNIT_TEST_SUITE_END();

protected:
    void range_traditional();
    void range_singular();
    void range_and();
    void intersectStrip_aslant_touch();
    void intersectStrip_aslant_inside();
    void intersectStrip_aslant_none();
    void intersectStrip_same_touch();
    void intersectStrip_same_lap();
    void intersectStrip_same_none();
    void intersectStrip_parallel_none();
    void onStrip_edge();
    void onStrip_inside();
    void onStrip_too_far();
    void onStrip_far_side();
	void LineEndPixelAlign();
	void AlmostVertLineEllipseIntersection(); //пересечение эллипса с "почти" вертикальной прямой
	void LineEllipseIntersection(); //пересечение эллипса и прямой
private:
};


#endif // TC_GEO_H