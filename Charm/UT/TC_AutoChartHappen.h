#ifndef TC_AUTOCHARTHAPPEN_H
#define TC_AUTOCHARTHAPPEN_H

#include <cppunit/extensions/HelperMacros.h>
#include "TrackerInfrastructure.h"
#include "TC_ChartImpl.h"

//Тестирование AutoChart.HappenLayer

class TestTracker;
class HemEvent;
class Chart;
class AutoChartTest;
class TC_AutoChartHappen : 
	private TC_ChartImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	TC_AutoChartHappen();
	~TC_AutoChartHappen();

private:
	TrackerInfrastructure & TI;
	boost::scoped_ptr<AutoChartTest> autoChart;
	attic::a_document xml_doc;
	CPPUNIT_TEST_SUITE( TC_AutoChartHappen );
	CPPUNIT_TEST( StationEntry );
	CPPUNIT_TEST( StationExit );
	CPPUNIT_TEST( SpanMove );
	CPPUNIT_TEST( StartValidation );
	CPPUNIT_TEST( PocketTransition );
	CPPUNIT_TEST( PocketExit );
	CPPUNIT_TEST( InvalidForm );
	CPPUNIT_TEST( Recovery );
    //CPPUNIT_TEST( InfoChanging );
    CPPUNIT_TEST( EntranceStopping );
    CPPUNIT_TEST( SpanToSpanTransit );
    CPPUNIT_TEST( ReduceAmount );
	CPPUNIT_TEST( StatToStatWOArrdep );
	CPPUNIT_TEST( TwoEventsValidness );
    CPPUNIT_TEST( SafelyDiscard );
	//CPPUNIT_TEST( MultiInfoChanges );
	CPPUNIT_TEST_SUITE_END();

protected:
	void StationEntry(); //имитация входа на станцию
	void StationExit(); //имитация выхода со станции
	void SpanMove(); //движение по перегону
	void StartValidation(); //проверка старта нити с валидных событий
	void PocketTransition(); //проход "кармана"
	void PocketExit(); //выход из "кармана" нового поезда
	void InvalidForm(); //некорректное формирование поезда
	void Recovery(); //восстановление поезда после пропадания
    void InfoChanging(); //изменение информации по нити
    void EntranceStopping(); //стоянка перед входным светофором
    void SpanToSpanTransit(); //проследование от перегона до перегона
    void ReduceAmount(); // стирание старых нитей
	void StatToStatWOArrdep(); //проследование от станции до станции без занятия п/о путей
	void TwoEventsValidness(); //валидность последовательности двух событий
	void MultiInfoChanges(); //множественные изменения поездной информации в одной точке в одно время
    void SafelyDiscard();// пропуск несущественных трио
	//разрешены ли события для следования друг за другом:
	bool PermittedAsSequence( const std::pair <HCode, BadgeE> &,
		const std::pair <HCode, BadgeE> &,
		time_t momtime );
};
#endif // TC_AUTOCHARTHAPPEN_H