#ifndef TC_AILERON_H
#define TC_AILERON_H

#include <cppunit/extensions/HelperMacros.h>
#include "Parody.h"

/// Тесты для класса \ref Ailerons
class TC_Aileron : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Aileron );
  CPPUNIT_TEST( street );
  CPPUNIT_TEST( opposite );
  CPPUNIT_TEST( dropper );
  CPPUNIT_TEST( tick1646 );
  CPPUNIT_TEST( tick1646_comment3 );
  CPPUNIT_TEST( independent_with_TS );
  CPPUNIT_TEST( unknown_1631 );
  CPPUNIT_TEST( in_route );
  CPPUNIT_TEST( receives_lts );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

protected:
    void street(); // стрелочная улица
    void opposite(); // встречные стрелки в ОДНОМ участке 
    void dropper(); // противошерстный остряк
    void tick1646(); // тикет 1646
    void tick1646_comment3();
    void independent_with_TS(); // Независимость участков с ТСами
    void unknown_1631(); // неизвестное состояние участков распространяется повсюду
    void in_route();    // передача участия в маршруте
    void receives_lts(); // получение ЛТСов прямо или косвенно

private:
};


#endif // TC_AILERON_H