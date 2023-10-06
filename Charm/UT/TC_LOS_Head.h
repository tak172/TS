#ifndef TC_LOS_HEAD_H
#define TC_LOS_HEAD_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса LOS_Head
 */
class Latch_LOS_Head;
class TC_LOS_Head : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_Head );
  CPPUNIT_TEST( immediate );
  CPPUNIT_TEST( open_priority );
  CPPUNIT_TEST( latching );
  CPPUNIT_TEST( latching_reopen );
  CPPUNIT_TEST( latching_select );
  CPPUNIT_TEST_SUITE_END();

protected:
    void setUp() override;
    void tearDown() override;

    void immediate(); // непосредственное срабатывание (без удержания)
    void open_priority(); // приоритет открытия вместе с другими сигналами
    void latching(); // простое удержание одного светофора
    void latching_reopen(); // удержание после повторного открытия
    void latching_select(); // удержание нескольких светофоров одновременно, но не всех
private:
    static const time_t T = 12345;
    static const unsigned INTERV = 10;
    Latch_LOS_Head* latch;
};


#endif // TC_LOS_HEAD_H