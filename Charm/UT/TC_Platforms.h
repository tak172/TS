#ifndef TC_PLATFORMS_H_
#define TC_PLATFORMS_H_

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса Platforms
 */
class TC_Platforms : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Platforms );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp()    {}
    virtual void tearDown() {}
protected:
    void test();
};


#endif //TC_PLATFORMS_H_
