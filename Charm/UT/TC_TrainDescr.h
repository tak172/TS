#ifndef TC_TRAINDESCR_H
#define TC_TRAINDESCR_H

#include <cppunit/extensions/HelperMacros.h>

class TC_TrainDescr: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_TrainDescr );
  CPPUNIT_TEST( getNord );
  CPPUNIT_TEST( getNiias );
  CPPUNIT_TEST( getNovosibSupplimentary );
  CPPUNIT_TEST( numberAndIndexFit );
  CPPUNIT_TEST( indexEqualOrNumberFit );
  CPPUNIT_TEST( deserialization );
  CPPUNIT_TEST( serialization );
  CPPUNIT_TEST( areNumbersPaired );
  CPPUNIT_TEST( supplement );
  CPPUNIT_TEST( equalExceptLoco );
  CPPUNIT_TEST_SUITE_END();

protected:
    void tearDown() override;
    void getNord();
    void getNiias();
    void getNovosibSupplimentary();
    void numberAndIndexFit();
    void indexEqualOrNumberFit();
    void deserialization();
    void serialization();
    void areNumbersPaired();
    void supplement();
    void equalExceptLoco();
private:
};


#endif // TC_TRAINDESCR_H