#ifndef TC_LOCALE_H
#define TC_LOCALE_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка stream_badge<*>
 */

class TC_Locale : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Locale );
  CPPUNIT_TEST( checkRu );
  CPPUNIT_TEST( checkLv );
  CPPUNIT_TEST_SUITE_END();

protected:
    void checkRu();
    void checkLv();
private:
};


#endif // TC_LOCALE_H