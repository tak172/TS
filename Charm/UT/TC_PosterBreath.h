#ifndef TC_POSTERBREATH_H
#define TC_POSTERBREATH_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Fund/PosterShop.h"

/// Тесты для класса \ref PosterBreath
class TC_PosterBreath : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_PosterBreath );
  CPPUNIT_TEST( wait_for_start );
  CPPUNIT_TEST( insert_multy );
  CPPUNIT_TEST( run_twice );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
protected:
    void wait_for_start(); ///< выдержка времени до срабатывания объекта
    void insert_multy();   ///< повторная вставка (повторное срабатывание) объекта
    void run_twice();      ///< неоднократное последовательное срабатывание объекта
private:
    PosterShop ps;
    static const time_t t0 = 1234567; // произвольное нечто 
    EsrKit kit;      // станция
    BadgeE b1,b2,b3; // объекты 
    size_t d1,d2,d3; // задержки
};


#endif // TC_POSTERBREATH_H