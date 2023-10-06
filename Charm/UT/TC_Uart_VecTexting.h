#ifndef TC_UART_VECTEXTING_H_
#define TC_UART_VECTEXTING_H_

#include <cppunit/extensions/HelperMacros.h>

/* 
* Проверка attic::a_attribute и attic::a_document
 */

class TC_UartVecTexting : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_UartVecTexting );
  CPPUNIT_TEST( Neva );
  CPPUNIT_TEST( Tk_type0 );
  CPPUNIT_TEST( Tk_type1 );
  CPPUNIT_TEST( Tk_type2 );
  CPPUNIT_TEST( Tk_type3 );
  CPPUNIT_TEST( Trakt );
  CPPUNIT_TEST( DialogTs );
  CPPUNIT_TEST_SUITE_END();

protected:
    void Neva();
    void Tk_type0();
    void Tk_type1();
    void Tk_type2();
    void Tk_type3();
    void Trakt();
    void DialogTs();

private:
};


#endif // TC_UART_VECTEXTING_H_