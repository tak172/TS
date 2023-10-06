#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"
#include "../Guess/TrackerEvent.h"

class TC_TrackerWrongWay : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerWrongWay );
	CPPUNIT_TEST( UnorientedNoNumber );
	CPPUNIT_TEST( UnorientedEvenNumber );
	CPPUNIT_TEST( UnorientedOddNumber );
	CPPUNIT_TEST( UnorientedEvenNumberWithSema );
	CPPUNIT_TEST( SpanOrientedNoNumber );
	CPPUNIT_TEST( SpanOrientedEvenNumber );
	CPPUNIT_TEST( SpanOrientedOddNumber );
	CPPUNIT_TEST( StatOrientedNoNumber );
	CPPUNIT_TEST( StatOrientedEvenNumber );
	CPPUNIT_TEST( StatOrientedOddNumber );
	CPPUNIT_TEST( CheckOrder );
	CPPUNIT_TEST( OneWaySpan );
	CPPUNIT_TEST( NoWrongWayWithFictiveStrip );
	CPPUNIT_TEST( DeathThenRecoveryOnWrongSpanWithEvent );
	CPPUNIT_TEST( DeathThenRecoveryOnWrongSpanWOEvent );
	CPPUNIT_TEST( DeathThenRecoveryOnWrongSpanUniqueEvent );
	CPPUNIT_TEST( BadHeadOrientationWhileWrongWayWithRecovery );
	CPPUNIT_TEST( BadHeadOrientationWhileWrongWayWithRecovery2 );
	CPPUNIT_TEST_SUITE_END();

	void UnorientedNoNumber(); //стык не ориентирован, путь перегона без номера
	void UnorientedEvenNumber(); //стык не ориентирован, путь перегона четный
	void UnorientedOddNumber(); //стык не ориентирован, путь перегона нечетный
	void UnorientedEvenNumberWithSema(); //стык не ориентирован, путь перегона четный, на выезде расположен четный светофор
	void SpanOrientedNoNumber(); //стык ориентирован на перегон, путь перегона без номера
	void SpanOrientedEvenNumber(); //стык ориентирован на перегон, путь перегона четный
	void SpanOrientedOddNumber(); //стык ориентирован на перегон, путь перегона нечетный
	void StatOrientedNoNumber(); //стык ориентирован на станцию, путь перегона без номера
	void StatOrientedEvenNumber(); //стык ориентирован на станцию, путь перегона четный
	void StatOrientedOddNumber(); //стык ориентирован на станцию, путь перегона нечетный
	void CheckOrder(); //порядок следования событий: сначала выход на неправильный путь перегона, потом движение по перегону
	void OneWaySpan(); //движение по однопутному перегону (правильно в любом случае)
	void NoWrongWayWithFictiveStrip();
	void DeathThenRecoveryOnWrongSpanWithEvent(); //генерация события выхода на неправильный путь перегона при восстановлении на границе станция-перегон
	void DeathThenRecoveryOnWrongSpanWOEvent(); //нет генерации события выхода на неправильный путь перегона при восстановлении на границе станция-перегон
	void DeathThenRecoveryOnWrongSpanUniqueEvent(); //событие выхода на неправильный путь перегона при восстановлении на границе станция-перегон должно быть единственным
	bool GoodMove( const std::wstring & strip1, const std::wstring & strip2, Oddness trainOddness ); //результат - было ли событие выхода на неправильный путь
	void BadHeadOrientationWhileWrongWayWithRecovery(); //неудачная ориентация поезда во время восстановления с одновременным выходом на неправильный путь перегона
	void BadHeadOrientationWhileWrongWayWithRecovery2();
};