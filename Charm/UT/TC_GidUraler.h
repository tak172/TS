#ifndef TC_GIDURALER_H
#define TC_GIDURALER_H
#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Attic.h"
#include "../Fund/GidUraler.h"
#include "../StatBuilder/basic_defs.h"

/* 
 * Класс для проверки работы класса KmPk
 */

class TC_GidUraler : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_GidUraler );
  CPPUNIT_TEST( search_single );
  CPPUNIT_TEST( search_multi );
  CPPUNIT_TEST( busy );
  CPPUNIT_TEST( force_on_esr );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

protected:
    void search_single();
    void search_multi();
    void busy();
    void force_on_esr();
};


#endif // TC_GIDURALER_H