#ifndef TC_HLOG_H
#define TC_HLOG_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы hlog
 */
class TC_HLog : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_HLog );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
protected:
    void test();
};


#endif // TC_HLOG_H