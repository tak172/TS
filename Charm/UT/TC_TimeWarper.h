#ifndef TC_TIMEWARPER_H
#define TC_TIMEWARPER_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка TimeMachine
 */

class TC_TimeWarper : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_TimeWarper );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

protected:
    void test();
private:
};


#endif // TC_TIMEWARPER_H