#ifndef TC_LOS_STRIP_H
#define TC_LOS_STRIP_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса LOS_Strip
 */
class TC_LOS_Strip : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_Strip );
  CPPUNIT_TEST( signals );
  CPPUNIT_TEST_SUITE_END();

protected:
    void signals();
};


#endif // TC_LOS_STRIP_H