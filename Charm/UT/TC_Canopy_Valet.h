#ifndef TC_CANOPY_VALET_H_
#define TC_CANOPY_VALET_H_

#include <cppunit/extensions/HelperMacros.h>

class TC_CanopyValet: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_CanopyValet );
  CPPUNIT_TEST( test_request );
  CPPUNIT_TEST( test_answer );
  CPPUNIT_TEST( test_undo );
  CPPUNIT_TEST_SUITE_END();

public:
    // virtual void setUp();
    // virtual void tearDown();
protected:
    void test_request();
    void test_answer();
    void test_undo();
private:
};


#endif // TC_CANOPY_VALET_H_