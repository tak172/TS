#ifndef TC_THECHARTS_H
#define TC_THECHARTS_H

#include <cppunit/extensions/HelperMacros.h>

class TC_TheCharts : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_TheCharts );
    CPPUNIT_TEST( SelectLive );
    CPPUNIT_TEST( SelectBy );
    CPPUNIT_TEST( SliceBoundaries );
    CPPUNIT_TEST( MoveCurrentTime );
    CPPUNIT_TEST( ChangeSummerWinter );
//     CPPUNIT_TEST( ReadDb );
//     CPPUNIT_TEST( ReadFile );
	CPPUNIT_TEST_SUITE_END();

protected:
    void setUp() override;
    void tearDown() override;
    void SelectLive(); // тест выбора живого куска
    void SelectBy();   // тест выбора куска по интервалу
    void SliceBoundaries();  // границы куска - смена, видимая смена, интервал прогноза
    void MoveCurrentTime(); // переход текущего времени через границу смены
    void ChangeSummerWinter(); // переход смены (летнее-зимнее время)
    void ReadDb(); // чтение из базы
    void ReadFile(); // чтение из файла
};

#endif // TC_THECHARTS_H