#ifndef TC_LOS_LANDSCAPE_H
#define TC_LOS_LANDSCAPE_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * ѕроверка класса LOS_landscape
 */
class LOS_base;
class rwCoord;
namespace Landscape
{
    class Path;
    class PeMark;
};

class TC_LOS_landscape : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_landscape );
  CPPUNIT_TEST( test_path );
  CPPUNIT_TEST( test_pe );
  CPPUNIT_TEST( test_bounds );
  CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void test_path();
    void test_pe();
    void test_bounds();

public:
    // генераци€ отладочных данных
    static std::vector<Landscape::Path> generate_Path();
    static std::vector<Landscape::PeMark> generate_PeMark();
    static void generate_Bounds( std::wstring* ruler, rwCoord* l, rwCoord* r );
};


#endif // TC_LOS_LANDSCAPE_H