#ifndef TC_CLONE_LOS_H
#define TC_CLONE_LOS_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса LOS_Strip
 */
class TC_Clone_LOS : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Clone_LOS );
  CPPUNIT_TEST( equivalence );
  CPPUNIT_TEST_SUITE_END();

protected:
    void equivalence();
};


#endif // TC_CLONE_LOS_H