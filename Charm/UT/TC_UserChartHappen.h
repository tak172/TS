#ifndef TC_USERCHARTHAPPEN_H
#define TC_USERCHARTHAPPEN_H

#include <cppunit/extensions/HelperMacros.h>
#include "TrackerInfrastructure.h"
#include "TC_ChartImpl.h"

//Тестирование AutoChart.HappenLayer

class TestTracker;
class HemEvent;
class Chart;
class UserChart;
class TC_UserChartHappen : 
	private TC_ChartImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	TC_UserChartHappen();
	~TC_UserChartHappen();

private:
    boost::scoped_ptr<UserChart> userChart;
	attic::a_document xml_doc;
	CPPUNIT_TEST_SUITE( TC_UserChartHappen );
    CPPUNIT_TEST( ReduceAmount );
    CPPUNIT_TEST( Rectify );
    CPPUNIT_TEST( SafelyDiscard );
	CPPUNIT_TEST_SUITE_END();

protected:
    void ReduceAmount(); // проверка стирания старых нитей
    void Rectify();      // стирание старых нитей
    void SafelyDiscard();// пропуск несущественных трио
};
#endif // TC_USERCHARTHAPPEN_H