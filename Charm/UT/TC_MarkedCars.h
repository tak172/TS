#pragma once
#include <cppunit/extensions/HelperMacros.h>

class MarkedCars;
class TC_MarkedCars : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_MarkedCars );
  CPPUNIT_TEST( single );
  CPPUNIT_TEST( repeat );
  CPPUNIT_TEST( select );
  CPPUNIT_TEST( ticket6866 );
  CPPUNIT_TEST_SUITE_END();

protected:
    void single();
    void repeat();
    void select();
    void ticket6866();

    void sampling( MarkedCars& markedCars, std::wstring sample[], size_t count );
};
