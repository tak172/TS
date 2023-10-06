#ifndef TC_TERRACHART_H
#define TC_TERRACHART_H

#include <cppunit/extensions/HelperMacros.h>

//“ест TerraChart сохранение и загрузка XML-файла
class TC_TerraChart : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_TerraChart );
	CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST( ReduceAmountDncDspAttendance );
	CPPUNIT_TEST_SUITE_END();

protected:
	void LoadSave(); // тест сохранени€ и загрузки из xml
    void ReduceAmountDncDspAttendance(); // тест стирани€ старых записей DncDspAttendance
};

#endif // TC_TERRACHART_H