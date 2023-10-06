#ifndef TC_CONVERSION_LOS_H
#define TC_CONVERSION_LOS_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса LOS_Strip
 */
class TC_Conversion_LOS : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Conversion_LOS );
  CPPUNIT_TEST( board );
  CPPUNIT_TEST( commutator );
  CPPUNIT_TEST( landscape );
  CPPUNIT_TEST( strip );
  CPPUNIT_TEST_SUITE_END();

protected:
    void board();
    void commutator();
    void landscape();
    void strip();
};


#endif // TC_CONVERSION_LOS_H