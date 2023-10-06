#ifndef TC_CONS_H
#define TC_CONS_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка списка в стиле LISP
 */

class TC_Cons : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Cons );
  CPPUNIT_TEST( single );
  CPPUNIT_TEST( multi );
  CPPUNIT_TEST( sharing );
  CPPUNIT_TEST( esr );
  CPPUNIT_TEST_SUITE_END();

protected:
    void single();
    void multi();
    void sharing();
    void esr();
private:
};


#endif // TC_CONS_H
