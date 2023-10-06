#ifndef TC_USERCHART_H
#define TC_USERCHART_H

#include <cppunit/extensions/HelperMacros.h>
#include "TC_ChartImpl.h"

//Тестирование UserChart сохранение и загрузка XML-файла

class UserChart;
typedef UserChart UserChartTest;
class TC_UserChart : 
	private TC_ChartImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	TC_UserChart();
	~TC_UserChart();
    void setUp() override;
    void tearDown() override;

private:
	boost::scoped_ptr<UserChartTest> userChart;

	CPPUNIT_TEST_SUITE( TC_UserChart );
    CPPUNIT_TEST( SaveAndLoad );
    CPPUNIT_TEST( DirtyDataLoad );
	CPPUNIT_TEST_SUITE_END();

protected:
	void SaveAndLoad();    //имитация записи в файл
    void DirtyDataLoad();  //загрузка файла с ошибками
};

#endif