#ifndef TC_RESTRICTIP_H_
#define TC_RESTRICTIP_H_

#include <cppunit/extensions/HelperMacros.h>

class TC_RestrictIp: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_RestrictIp );
  CPPUNIT_TEST( single );
  CPPUNIT_TEST( group );
  CPPUNIT_TEST( getters );
  CPPUNIT_TEST( from_template );
  CPPUNIT_TEST_SUITE_END();

public:
    //virtual void setUp();
    //virtual void tearDown();
protected:
    void single();
    void group();
    void getters();
    void from_template();
private:
};


#endif // TC_RESTRICTIP_H_