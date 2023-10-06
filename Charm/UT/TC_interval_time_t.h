#ifndef TC_INTERVAL_TIME_T_H
#define TC_INTERVAL_TIME_T_H

#include <cppunit/extensions/HelperMacros.h>

class TC_interval_time_t : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_interval_time_t );
  CPPUNIT_TEST( Dirt );
  CPPUNIT_TEST( Compare );
  CPPUNIT_TEST( Overlap );
  CPPUNIT_TEST( ioString );
  CPPUNIT_TEST( View );
  CPPUNIT_TEST( Insert );
  CPPUNIT_TEST( semiOpenClose );
  CPPUNIT_TEST( semiOverlap );
  CPPUNIT_TEST( semiContains );
  CPPUNIT_TEST( semiReduce );
  CPPUNIT_TEST( semiToString );
  CPPUNIT_TEST( semiReadWrite );
  CPPUNIT_TEST_SUITE_END();

private:
	void Dirt();
    void Compare();
    void Overlap();
    void ioString();
    void View();
    void Insert();
    void semiOpenClose();
    void semiOverlap();
    void semiContains();
    void semiReduce();
    void semiToString();
    void semiReadWrite();
};


#endif // TC_INTERVAL_TIME_T_H