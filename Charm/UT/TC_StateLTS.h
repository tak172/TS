#ifndef TC_STATELTS_H
#define TC_STATELTS_H

#include <cppunit/extensions/HelperMacros.h>

class TC_StateLTS : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_StateLTS );
  CPPUNIT_TEST( construct );
  CPPUNIT_TEST( assignment );
  CPPUNIT_TEST( operate );
  CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
protected:
    void construct();
    void assignment();
    void operate();
private:
};

#endif // TC_STATELTS_H