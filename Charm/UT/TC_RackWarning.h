#ifndef TC_RACKWARNING_H
#define TC_RACKWARNING_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса RackWarning
 */

class TC_RackWarning : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_RackWarning );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

protected:
    void test();
private:
};


#endif // TC_RACKWARNING_H