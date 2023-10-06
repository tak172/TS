#ifndef TC_EXCERPT_LOT_H
#define TC_EXCERPT_LOT_H

#include <cppunit/extensions/HelperMacros.h>

namespace attic { class a_node; };

class TC_Excerpt_Lot: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Excerpt_Lot );
  CPPUNIT_TEST( Span_Full );
  CPPUNIT_TEST( Span_SingleWay_Segment );
  CPPUNIT_TEST( Terminal_IoWay );
  CPPUNIT_TEST( Terminal_SeparateObject );
  CPPUNIT_TEST_SUITE_END();

public:
    //virtual void setUp();
    //virtual void tearDown();
protected:
    void Span_Full();            // все пути перегона
    void Span_SingleWay_Segment();  // часть одного пути перегона (в т.ч. и полностью этот путь перегона)
    void Terminal_IoWay();          // приемоотправочный путь на станции
    void Terminal_SeparateObject(); // отдельный объект на станции
private:
    void rewrite_lot( attic::a_node parent );
};


#endif // TC_EXCERPT_LOT_H