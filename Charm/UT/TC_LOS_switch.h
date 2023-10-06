#ifndef TC_LOS_SWITCH_H
#define TC_LOS_SWITCH_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса EsrKit
 */

class TC_LOS_switch : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_switch );
  CPPUNIT_TEST( signalTS );
  CPPUNIT_TEST( signalTSext );
  CPPUNIT_TEST_SUITE_END();

protected:
    void signalTS();
    void signalTSext();
private:
};


#endif // TC_LOS_SWITCH_H