#ifndef TC_DICTUM_H
#define TC_DICTUM_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка кодирования класса DICTUM_LTS
 */

class TC_Dictum : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Dictum );
  CPPUNIT_TEST( compatible_output );
  CPPUNIT_TEST( compatible_input );
  CPPUNIT_TEST( read_from_rich );
  CPPUNIT_TEST( write_to_rich );
  CPPUNIT_TEST( rich_and_legacy );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
protected:
    void compatible_output(); // совместимость вывода на бинарном уровне
    void compatible_input();  // совместимость  ввода на бинарном уровне
    void read_from_rich();
    void write_to_rich();
    void rich_and_legacy();
private:
    std::vector<char> etalon1;
    std::vector<char> etalon2;
};

#endif // TC_DICTUM_H