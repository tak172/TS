#ifndef TC_CARRIER_H_
#define TC_CARRIER_H_

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класс Carrier
 */
class TC_Carrier : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Carrier );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST( request );
  CPPUNIT_TEST( answer );
  CPPUNIT_TEST_SUITE_END();

public:
    void tearDown() override;
protected:
    void test();
    void request();
    void answer();
};


#endif //TC_CARRIER_H_
