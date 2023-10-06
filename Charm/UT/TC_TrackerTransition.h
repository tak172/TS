#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка генерации событий отправления

class TC_TrackerTransition : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTransition );
	CPPUNIT_TEST( UncontainedTransition );
	CPPUNIT_TEST( AsyncTransitionCargo );
	CPPUNIT_TEST( AsyncTransitionPassengerSpec );
	CPPUNIT_TEST( AsyncTransitionPassengerNoSpec );
	CPPUNIT_TEST( StandardTransition );
	CPPUNIT_TEST( WOWaysTransition );
	CPPUNIT_TEST( WOWaysPassing );
	CPPUNIT_TEST( OpenedBlockpostTransition );
	CPPUNIT_TEST( ClosedBlockpostTransition );
	CPPUNIT_TEST( DeparturingFormAndBPTransition );
	CPPUNIT_TEST( TransitionWhileMultiWaysLiberation );
	CPPUNIT_TEST( LongTransitionThroughEmbarkWay );
	CPPUNIT_TEST( FastTransitionThroughEmbarkWay );
	CPPUNIT_TEST( MoveThroughSeveralBlockposts );
	CPPUNIT_TEST( HistoryLimitationOnTransitionGeneration );
	CPPUNIT_TEST_SUITE_END();

	void UncontainedTransition(); //проследование протяженного поезда, который не помещается на п/о путь целиком
	void AsyncTransitionCargo(); //проследование грузового поезда
	void AsyncTransitionPassengerSpec(); //проследование пассажирского поезда (по пути, специализированному для посадки/высадки пассажиров)
	void AsyncTransitionPassengerNoSpec(); //проследование пассажирского поезда (по пути, НЕ специализированному для посадки/высадки пассажиров)
	void StandardTransition(); //формирование при въезде на станцию и последующее стандартное проследование
	void WOWaysTransition(); //проследование станции, минуя п/о пути (событие проследования есть)
	void WOWaysPassing(); //проезд станции, минуя п/о пути (не проследование)
	void OpenedBlockpostTransition(); //проследование через блок-пост (светофор открыт)
	void ClosedBlockpostTransition(); //проследование через блок-пост (светофор закрыт)
	void BlockpostTransition(); //проследование через блок-пост
	void DeparturingFormAndBPTransition(); //создание поезда после покидания п/о пути и последующее проследование блок-поста
	void TransitionWhileMultiWaysLiberation(); //проследование при освобождении нескольких участков (проверка на правильное выстраивание освобождаемых участков)
	//дополнение при необходимости события проследования интервалом между полным въездом на путь и моментов события проследования
	void LongTransitionThroughEmbarkWay();
	void FastTransitionThroughEmbarkWay();
	void MoveThroughSeveralBlockposts(); //ошибка "застревания" информации о выходе с ПО-пути давно минувшей станции
	void HistoryLimitationOnTransitionGeneration(); //ограничение глубины просмотра истории при генерации проследования
};