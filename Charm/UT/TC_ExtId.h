#ifndef TC_EXTID_H
#define TC_EXTID_H

#include <cppunit/extensions/HelperMacros.h>

class TC_ExtId: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ExtId );
  CPPUNIT_TEST( none );
  CPPUNIT_TEST( local );
  CPPUNIT_TEST( exter );
  CPPUNIT_TEST( compatibility );
  CPPUNIT_TEST( serializing );
  CPPUNIT_TEST( hashing );
  CPPUNIT_TEST( compare_id );
  CPPUNIT_TEST_SUITE_END();

public:
    //virtual void setUp();
    //virtual void tearDown();
protected:
    void none();
    void local();
    void exter();
    void compatibility();
    void serializing();
    void hashing();
	void compare_id();
};


#endif // TC_EXTID_H