#pragma once

#include <cppunit/extensions/HelperMacros.h>

/* 
 * �������� �������� ������
 */
class TC_ActorPeakMemory : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ActorPeakMemory );
  CPPUNIT_TEST( check );
  CPPUNIT_TEST_SUITE_END();

protected:
    void check();
};
