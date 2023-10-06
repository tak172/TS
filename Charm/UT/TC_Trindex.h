#ifndef TC_TRINDEX_H
#define TC_TRINDEX_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка Trindex
 */

class TC_Trindex : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Trindex );
  CPPUNIT_TEST( normal );
  CPPUNIT_TEST( empty );
  CPPUNIT_TEST( bad );
  CPPUNIT_TEST_SUITE_END();

protected:
    void normal();
    void empty();
    void bad();
private:
};


#endif // TC_TRINDEX_H