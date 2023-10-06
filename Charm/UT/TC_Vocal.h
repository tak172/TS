#ifndef TC_VOCAL_H
#define TC_VOCAL_H

#include <cppunit/extensions/HelperMacros.h>

class TC_Vocal : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Vocal );
  CPPUNIT_TEST( sound_path );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
protected:
    void sound_path();
private:
};


#endif // TC_VOCAL_H