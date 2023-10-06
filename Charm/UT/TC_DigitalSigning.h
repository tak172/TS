#ifndef TC_DIGITALSIGN_H
#define TC_DIGITALSIGN_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса BadgeE
 */

class TC_DigitalSign : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_DigitalSign );
  CPPUNIT_TEST( verify_xml );
  CPPUNIT_TEST_SUITE_END();

protected:
    void verify_xml();
};


#endif // TC_DIGITALSIGN_H