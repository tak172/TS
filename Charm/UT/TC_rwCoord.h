#ifndef TC_RWCOORD_H
#define TC_RWCOORD_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка класса rwCoord
 */

class TC_rwCoord : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_rwCoord );
  CPPUNIT_TEST( simple_empty );
  CPPUNIT_TEST( from_text );
  CPPUNIT_TEST( exactKm );
  CPPUNIT_TEST( Km_and_tiny );
  CPPUNIT_TEST( exactPiquet );
  CPPUNIT_TEST( piquet_and_tiny );
  CPPUNIT_TEST( using_flat_meter );
  CPPUNIT_TEST( equals );
  CPPUNIT_TEST( compare_normal );
  CPPUNIT_TEST( compare_strange );
  CPPUNIT_TEST( set_irregular );
  CPPUNIT_TEST( calc_irregular );
  CPPUNIT_TEST( offset_w_irregular );
  CPPUNIT_TEST( ceil_floor );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;
protected:
    void simple_empty(); // пустышка
    void from_text();    // построение из строки
    void exactKm();      // ровная километровая отметка
    void Km_and_tiny();  // километр с небольшим
    void exactPiquet();  // ровный пикетный столбик
    void piquet_and_tiny(); // пикет с небольшим
    void using_flat_meter(); // устаревшее задание в метрах
    void equals();          // сравнения
    void compare_normal();  // сравнения логичные (нормальные)
    void compare_strange(); // сравнения странные (неправильные пикеты)
    void set_irregular();   // установка неправильных километров
    void calc_irregular();  // вычисления с неправильными километрами
    void offset_w_irregular(); // сдвиг координаты на несколько метров
    void ceil_floor(); // сдвиг до соседних километровых отметок
private:
};


#endif // TC_RWCOORD_H