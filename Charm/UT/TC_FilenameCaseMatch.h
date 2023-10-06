#ifndef TC_FILENAMECASEMATCH_H
#define TC_FILENAMECASEMATCH_H

#include <cppunit/extensions/HelperMacros.h>

class TC_FilenameCaseMatch : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_FilenameCaseMatch );
  CPPUNIT_TEST( equal );
  CPPUNIT_TEST( diff );
  CPPUNIT_TEST( diffN );
  CPPUNIT_TEST( none );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp()    override;
    void tearDown() override;
protected:
    void equal();
    void diff();
    void diffN();
    void none();
};

#endif
