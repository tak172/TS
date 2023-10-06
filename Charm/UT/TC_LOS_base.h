#ifndef TC_LOS_BASE_H
#define TC_LOS_BASE_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Тестирование LOS_base
 */

class TC_LOS_base : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_base );
  CPPUNIT_TEST( uniqSigXcode );
  CPPUNIT_TEST( addSignalCode );
  CPPUNIT_TEST_SUITE_END();

protected:
    void uniqSigXcode();
    void addSignalCode();
private:
};


#endif // TC_LOS_BASE_H