#ifndef TC_EXCERPTWARN_H
#define TC_EXCERPTWARN_H

#include <cppunit/extensions/HelperMacros.h>

class TC_ExcerptWarn: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ExcerptWarn );
  CPPUNIT_TEST( BadData );
  CPPUNIT_TEST( Warn );
  CPPUNIT_TEST_SUITE_END();

public:
    //virtual void setUp();
    //virtual void tearDown();
protected:
    void BadData();
    void Warn();
private:
};


#endif // TC_EXCERPTWARN_H