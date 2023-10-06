#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_ParkWayKit: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ParkWayKit );
  CPPUNIT_TEST( simple );
  CPPUNIT_TEST( create );
  CPPUNIT_TEST_SUITE_END();

protected:
    void simple();
    void create();
};
