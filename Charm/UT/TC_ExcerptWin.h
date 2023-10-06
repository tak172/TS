#ifndef TC_EXCERPTWIN_H
#define TC_EXCERPTWIN_H

#include <cppunit/extensions/HelperMacros.h>

class WindowExcerpt;

/* 
 * Класс для проверки работы класса EsrKit
 */

class TC_ExcerptWin: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ExcerptWin );
  CPPUNIT_TEST( planning );
  CPPUNIT_TEST( factBE );
  CPPUNIT_TEST( factBm );
  CPPUNIT_TEST( fact_E );
  CPPUNIT_TEST( BadData );
  CPPUNIT_TEST( Wind );
  CPPUNIT_TEST( RackWindow_work );
  CPPUNIT_TEST_SUITE_END();

public:
    //virtual void setUp();
    //virtual void tearDown();
protected:
    void planning();
    void factBE();
    void factBm();
    void fact_E();
    void BadData();
    void Wind();
    void RackWindow_work();
private:
    WindowExcerpt makeWin( time_t mom_mentioned,
                           int Id,
                           const char * plan_start,
                           const char * plan_finish,
                           const char * fact_start = NULL,
                           const char * fact_finish = NULL
                           );
};


#endif // TC_EXCERPTWIN_H