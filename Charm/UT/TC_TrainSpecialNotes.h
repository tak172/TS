#ifndef TC_TRAINSPECIALNOTES_H
#define TC_TRAINSPECIALNOTES_H

#include <cppunit/extensions/HelperMacros.h>

class TC_TrainSpecialNotes: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_TrainSpecialNotes );
  CPPUNIT_TEST( createSeparated );
  CPPUNIT_TEST( createFromString );
  CPPUNIT_TEST( custom );
  CPPUNIT_TEST_SUITE_END();

protected:
    void createSeparated();
    void createFromString();
    void custom();
private:
};


#endif // TC_TRAINSPECIALNOTES_H