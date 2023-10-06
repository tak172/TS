#pragma once
#ifndef TC_STRIPSHOT_H
#define TC_STRIPSHOT_H

#include <cppunit/extensions/HelperMacros.h>

class TC_StripShot: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_StripShot );
  CPPUNIT_TEST( BadData );
  CPPUNIT_TEST( parsing_Train );
  CPPUNIT_TEST( parsing_StripShot );
  CPPUNIT_TEST_SUITE_END();

public:
    //virtual void setUp();
    //virtual void tearDown();
protected:
    void BadData();
    void parsing_Train();
    void parsing_StripShot();
private:
};


#endif // TC_STRIPSHOT_H