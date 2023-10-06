#ifndef TC_EVENTTRIO_H
#define TC_EVENTTRIO_H

#include <cppunit/extensions/HelperMacros.h>

class TC_EventTrio : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_EventTrio );
  CPPUNIT_TEST( simple );
  CPPUNIT_TEST( with_cover );
  CPPUNIT_TEST_SUITE_END();

protected:
    void simple();
    void with_cover();
private:
};


#endif // TC_EVENTTRIO_H