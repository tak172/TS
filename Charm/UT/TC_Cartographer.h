#ifndef TC_CARTOGRAPHER_H
#define TC_CARTOGRAPHER_H

#include <cppunit/extensions/HelperMacros.h>
#include "../StatBuilder/Geo.h"
/* 
* Класс для проверки работы класса Cartographer
*/

class TC_Cartographer : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Cartographer );
	CPPUNIT_TEST( AddNRemSingle );
	CPPUNIT_TEST( AddNRemMultipleNOvp );
	CPPUNIT_TEST( AddNRemMultipleInJnt );
	CPPUNIT_TEST( AddNRemMultipleOvp );
	CPPUNIT_TEST( AddNRemSingleDistrNOvp );
	CPPUNIT_TEST( AddNRemSingleDistrOvp );
	CPPUNIT_TEST( searchInner );
    CPPUNIT_TEST( Dimension );

	CPPUNIT_TEST_SUITE_END();

public:

protected:
	void AddNRemSingle(); // простое одиночное добавление
	void AddNRemMultipleNOvp(); // множественное добавление (БЕЗ ПЕРЕКРЫТИЯ)
	void AddNRemMultipleInJnt(); // множественное добавление (В СТЫК)
	void AddNRemMultipleOvp(); // множественное добавление (С ПЕРЕКРЫТИЕМ)
	void AddNRemSingleDistrNOvp(); // одиночное добавление объекта с распределенной площадью (БЕЗ ПЕРЕКРЫТИЯ)
	void AddNRemSingleDistrOvp(); // одиночное добавление объекта с распределенной площадью (С ПЕРЕКРЫТИЕМ)
    void searchInner(); // проверка поиска перекрытия
    void Dimension(); // проверка получения общих размеров

private:
	//равномерное добавление/удаление (с регулируемым числом элементов и коэффициентом перекрытия( >1 - перекрытие есть ) )
	void AddNRemCommon( unsigned int x_num ,unsigned int y_num,
		double horiz_gap, double vert_gap);
};


#endif // TC_CARTOGRAPHER_H