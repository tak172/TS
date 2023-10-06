#ifndef TC_LOCOMOTIVE_H
#define TC_LOCOMOTIVE_H

#include <cppunit/extensions/HelperMacros.h>

class TC_Locomotive: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Locomotive );
  CPPUNIT_TEST( read_write );
  CPPUNIT_TEST( compatibility );
  CPPUNIT_TEST( compat1_3_71 );
  CPPUNIT_TEST( changeKIG_572 );
  CPPUNIT_TEST( mixRusLat );
  CPPUNIT_TEST( saveUnknown );
  CPPUNIT_TEST_SUITE_END();

protected:
    void tearDown() override;
    void read_write();
    void compatibility();
    void compat1_3_71();
    void changeKIG_572();
    void mixRusLat();
    void saveUnknown();
private:
};

#endif // TC_LOCOMOTIVE_H
