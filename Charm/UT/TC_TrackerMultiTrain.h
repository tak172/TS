#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Тесты на простые и статические изменения для множественных ПЕ

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerMultiTrain : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerMultiTrain );
	CPPUNIT_TEST( AppearanceOnSwitch );
	CPPUNIT_TEST( NamedMovingToNamed );
	CPPUNIT_TEST( SwitchMergingOfTrainAndNoTrain );
	CPPUNIT_TEST( SwitchNotMergingOfTrains );
	CPPUNIT_TEST( BusinessAtomicOverlay );
	CPPUNIT_TEST( GoingToObscurity );
	CPPUNIT_TEST( HeterogeneousMoving );
	CPPUNIT_TEST( TrainOccupyNoTrain );
	CPPUNIT_TEST( BidirCreate );
	CPPUNIT_TEST( Intersection );
	CPPUNIT_TEST( CloseWrongSemaPassing );
	CPPUNIT_TEST( NoMergeTrainWithUnityWhileDisforming );
	CPPUNIT_TEST( MultiConflict );
	CPPUNIT_TEST( MultiFreeNBusy );
	CPPUNIT_TEST( IndexIgnoringOnTrainPriority );
	CPPUNIT_TEST( TrainExitVsNoTrainMovingOnOpenHead );
	CPPUNIT_TEST( NoTrainsLessIdenOccupyPriority );
	CPPUNIT_TEST( ConflictBetweenNearAndFar );
	CPPUNIT_TEST( TailFadeNearADWayWithTrain );
	CPPUNIT_TEST( TailFadeNearADWayWithNoTrain );
	CPPUNIT_TEST( TailFadeNearTwoShortTrains );
	CPPUNIT_TEST( TailFadeNearTwoShortTrains2 );
	CPPUNIT_TEST( SwitchToggleBreakSequence );
	CPPUNIT_TEST( SwitchToggleBreakSequence2 );
	CPPUNIT_TEST_SUITE_END();

	void AppearanceOnSwitch(); //возникновение на стрелке двух ПЕ
	void NamedMovingToNamed(); //перемещение неизвестного ПЕ к неизвестному ПЕ
	void SwitchMergingOfTrainAndNoTrain(); //слияния поезда и маневровой после перевода стрелки
	void SwitchNotMergingOfTrains(); //запрет слияния поездов после перевода стрелки
	void BusinessAtomicOverlay(); //атомарное покрытие занятостью двух смежных ПЕ одновременно (не должно приводить к изменениям)
	void GoingToObscurity(); //уход одной ПЕ в неизвестность (исчезновение) с одновременным занятием ее места другой ПЕ
	void HeterogeneousMoving();
	void TrainOccupyNoTrain(); //занятие поездом места, предназначенного для непоездной ПЕ (за дефицитом возможных мест и необходимости своего сохранения)
	void BidirCreate(); //занятие двух участков по разные стороны от места, занимаемого поездом (критерий движения определяется историей движения поезда)
	void Intersection(); //пересечение двух поездов на общем участке
	void CloseWrongSemaPassing(); //проезд через светофор чужой четности для двух плотно стоящих поездов
	void NoMergeTrainWithUnityWhileDisforming(); //запрет слияния поезда с ПЕ после переезда через чужой светофор
	void MultiConflict(); //конфликт при занятии единственного участка более, чем двумя ПЕ
	void MultiFreeNBusy(); //одновременное освобождение и занятие нескольких участков для нескольких ПЕ
	void IndexIgnoringOnTrainPriority(); //игнорирование индекса при определении приоритетности поезда
	void TrainExitVsNoTrainMovingOnOpenHead(); //выбор между выездом поезда с ПО-пути на открытый светофор и подъездом к нему маневровой должен отдаваться поезду
	void NoTrainsLessIdenOccupyPriority(); //для не поездов при прочих равных для занятия предпочтение отдается ПЕ с меньшим идентфикатором (для детерминистичной обработки)
	void ConflictBetweenNearAndFar(); //оспаривание занимаемого поездом участка со стороны другого удаленного поезда
	void TailFadeNearADWayWithTrain(); //гашение хвоста соседней ПЕ (не важно какой) рядом с ПО-путем при наличии на нем поезда
	void TailFadeNearADWayWithNoTrain(); //гашение хвоста соседней ПЕ (не важно какой) рядом с ПО-путем при наличии на нем маневровой
	void TailFadeNearTwoShortTrains(); //гашение хвоста поезда около двух одноместных поездов
	void TailFadeNearTwoShortTrains2();
	void SwitchToggleBreakSequence(); //нарушение порядка следования участков после перевода стрелки
	void SwitchToggleBreakSequence2();
};