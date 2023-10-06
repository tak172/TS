#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ColorGamut;
class Blinker;
/* 
 * Класс для проверки работы класса Permission::Designated
 */
class TC_PaletteDesigner : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_PaletteDesigner );
  CPPUNIT_TEST( Load );
  CPPUNIT_TEST_SUITE_END();

public:
     virtual void setUp();
     virtual void tearDown();
protected:
    void Load();

private:
    ColorGamut* color_gamut;
};

