#ifndef TC_DATETIME_H
#define TC_DATETIME_H

#include <cppunit/extensions/HelperMacros.h>

class TC_DateTime: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_DateTime );
  CPPUNIT_TEST( fromYMDhms );
  CPPUNIT_TEST( fromYMDhms_bad );
  CPPUNIT_TEST( fromDMYhms );
  CPPUNIT_TEST( fromDMYhms_bad );
  CPPUNIT_TEST( check_dst );
  CPPUNIT_TEST( to_string );
  CPPUNIT_TEST( check_moscow_local );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void fromYMDhms();
    void fromYMDhms_bad();
    void fromDMYhms();
    void fromDMYhms_bad();
    void check_dst();
    void to_string();
    void check_moscow_local();
private:
    int winter_hours;
    int summer_hours;
};


#endif // TC_DATETIME_H