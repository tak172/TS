#ifndef TC_CREW_H
#define TC_CREW_H

#include <cppunit/extensions/HelperMacros.h>

class TC_CrewAndLocomotive: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_CrewAndLocomotive );
  CPPUNIT_TEST( read_write );
  CPPUNIT_TEST( crew_only );
  CPPUNIT_TEST_SUITE_END();

public:
protected:
    void tearDown() override;
    void read_write();
    void crew_only();

private:
};


#endif // TC_CREW_H
