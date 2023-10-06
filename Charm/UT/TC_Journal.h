#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Journal : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Journal );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

protected:
    void test();
};
