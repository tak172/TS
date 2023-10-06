#ifndef TC_XLATESRPARKWAY_H
#define TC_XLATESRPARKWAY_H

#include <cppunit/extensions/HelperMacros.h>

class TC_XlatEsrParkWay : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_XlatEsrParkWay );
  CPPUNIT_TEST( load_save );
  CPPUNIT_TEST( normal );
  CPPUNIT_TEST( badValue );
  CPPUNIT_TEST( repeatValue );
  CPPUNIT_TEST( empty );
  CPPUNIT_TEST( merge );
  CPPUNIT_TEST_SUITE_END();

protected:
    void load_save();
    void normal();
    void badValue();
    void repeatValue();
    void empty();
    void merge();

private:
};


#endif // TC_XLATESRPARKWAY_H