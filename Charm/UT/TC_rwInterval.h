#pragma once

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса KmPk
 */
class PicketingInfo;

class TC_rwInterval : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_rwInterval );
  CPPUNIT_TEST( create );
  CPPUNIT_TEST( operate );
  CPPUNIT_TEST( convert_ordinal );
  CPPUNIT_TEST( interval_from_coord );
  CPPUNIT_TEST_SUITE_END();

protected:
    void create();
    void operate();
    void convert_ordinal();
    void interval_from_coord();
private:
    PicketingInfo PicketingInfoFromStr(const std::wstring& str);
};
