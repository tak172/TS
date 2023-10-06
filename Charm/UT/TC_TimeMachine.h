#ifndef TC_TIMEMACHINE_H
#define TC_TIMEMACHINE_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка TimeMachine
 */

class TC_TimeMachine : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_TimeMachine );
  CPPUNIT_TEST( start_stop );
  CPPUNIT_TEST( speed );
  CPPUNIT_TEST( jump );
  CPPUNIT_TEST_SUITE_END();

protected:
    void start_stop();
    void speed();
    void jump();
private:
};


#endif // TC_TIMEMACHINE_H