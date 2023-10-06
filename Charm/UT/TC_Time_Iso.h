#ifndef TC_TIME_ISO_H
#define TC_TIME_ISO_H

#include <cppunit/extensions/HelperMacros.h>

/* 
* Проверка time conversion
 */

class TC_Time_Iso : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Time_Iso );
  CPPUNIT_TEST( big_time );
  CPPUNIT_TEST_SUITE_END();

protected:
    void big_time();
};


#endif // TC_TIME_ISO_H