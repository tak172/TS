#ifndef TC_TOWERAIM_H
#define TC_TOWERAIM_H

#include <cppunit/extensions/HelperMacros.h>


class TC_TowerAim : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_TowerAim );
  CPPUNIT_TEST( test_static );
  CPPUNIT_TEST( test_dynamic );
  CPPUNIT_TEST_SUITE_END();

protected:
    void test_static();
    void test_dynamic();
};


#endif // TC_TOWERAIM_H