#ifndef TC_ARAMISNOTIFY_H_
#define TC_ARAMISNOTIFY_H_

#include <cppunit/extensions/HelperMacros.h>
//#include "../Permission/ActorPermit.h"

/* 
 * Класс для проверки работы класса AramisNotify
 */
class TC_AramisNotify : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_AramisNotify );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp()    {}
    virtual void tearDown() {}
protected:
    void test();
};


#endif //TC_ARAMISNOTIFY_H_
