#ifndef TC_BADGE_H
#define TC_BADGE_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса BadgeE
 */

class TC_Badge : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Badge );
  CPPUNIT_TEST( compare );
  CPPUNIT_TEST_SUITE_END();

protected:
    void compare();
};


#endif // TC_BADGE_H