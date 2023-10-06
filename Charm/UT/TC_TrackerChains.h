#ifndef TC_TRACKERCHAINS_H
#define TC_TRACKERCHAINS_H

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка верности построения цепочек занятости на стороне Guess (Tracker) при их появлении, слиянии и разделении

class TestTracker;
class TrainField;
class TC_TrackerChains : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_TrackerChains );
	CPPUNIT_TEST( AppearanceScenario );
	CPPUNIT_TEST( MergingScenario );
	CPPUNIT_TEST( LongMergingScenario );
	CPPUNIT_TEST( TripleSwitchScenario );
	CPPUNIT_TEST( SpanToStatScenario );
	CPPUNIT_TEST( SplittingScenario );
	CPPUNIT_TEST( SwitchToggling );
	CPPUNIT_TEST( IncorrectChainsMergingOnForcedLocation );
	CPPUNIT_TEST_SUITE_END();

public:
	TC_TrackerChains();
	virtual void setUp();
	virtual void tearDown();

protected:
	void AppearanceScenario(); //проверка появления цепочек (активация лосов по одному)
	void MergingScenario(); //проверка слияния цепочек
	void LongMergingScenario(); //проверка слияния длинных (более одного участка) цепочек
	void TripleSwitchScenario(); //проверки построения цепочки через три стрелки
	void SpanToStatScenario(); //проверка ведения цепочки от перегона к станции
	void SplittingScenario(); //проверка разбиения цепочек
	void SwitchToggling(); //перевод стрелки в середине цепочки
	void IncorrectChainsMergingOnForcedLocation(); //ошибочное слияние ЦЗ после форсированной установки ПЕ

private:
	//генерируем два "поля" станций (TFReversed создается в обратном порядке ( изменяется структура и нумерация вершин и ребер графа )
	TrainFieldPtr TFReverted;
	void CheckUniqueExistence( BadgeE bdg_array[], unsigned int array_size ) const;
};
#endif // TC_TRACKERCHAINS_H