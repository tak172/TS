#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//ѕроверка определени€ четности ѕ≈

class TC_TrackerTrainOddness : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainOddness );
	CPPUNIT_TEST( WoOddnessToEvenByTrainSema );
	CPPUNIT_TEST( WoOddnessToEvenByShuntSema );
	CPPUNIT_TEST( WoOddnessToOddByTrainSema );
	CPPUNIT_TEST( OddToEvenByTrainSema );
	CPPUNIT_TEST( EvenToOddByTrainSema );
	CPPUNIT_TEST( EvenToEvenByTrainSema );
	CPPUNIT_TEST( OddToOddByTrainSema );
	CPPUNIT_TEST( WoOddnessToEvenByOppTrainSema );
	CPPUNIT_TEST( WoOddnessToEvenByOppShuntSema );
	CPPUNIT_TEST( WoOddnessToOddByOppTrainSema );
	CPPUNIT_TEST( ChangeOddnessByStandardSpan );
	CPPUNIT_TEST( ChangeOddnessByNonStandardSpan );
	CPPUNIT_TEST( ChangeOddnessForHouseholdOnSpanToStation );
	CPPUNIT_TEST( UnidentifyForNoHouseholdOnSpanToStation );
	CPPUNIT_TEST_SUITE_END();

	void WoOddnessToEvenByTrainSema(); //смена "без четности->четный" по поездному светофору
	void WoOddnessToEvenByShuntSema(); //смена "без четности->четный" по маневровому светофору
	void WoOddnessToOddByTrainSema(); //смена "без четности->нечетный" по поездному светофору
	void OddToEvenByTrainSema(); //смена "нечетный->четный" по поездному светофору
	void EvenToOddByTrainSema(); //смена "четный->нечетный" по поездному светофору
	void EvenToEvenByTrainSema(); //без смены четности "четный" по поездному светофору
	void OddToOddByTrainSema(); //без смены четности "нечетный" по поездному светофору
	void WoOddnessToEvenByOppTrainSema(); //смена "без четности->четный" по встречному поездному светофору
	void WoOddnessToEvenByOppShuntSema(); //смена "без четности->четный" по встречному маневровому светофору
	void WoOddnessToOddByOppTrainSema(); //смена "без четности->нечетный" по встречному поездному светофору
	void ChangeOddnessByStandardSpan(); //смена четности при выходе на обычный перегон
	void SpanChangingOddnessCheck( std::wstring fromPlace, const TrainDescr & initDescr, Oddness initOddness, std::wstring toPlace, const TrainDescr & resDescr, 
		Oddness resOddness );
	void ChangeOddnessByNonStandardSpan(); //смена четности при выходе на перегон со светофорами с противоречивой четностью
	void ThroughEvenTrainSema( Oddness initOddnes, Oddness newOddness, bool withNumber ); //проезд через четный светофор
	void ThroughOddTrainSema( Oddness initOddness, Oddness newOddness, bool withNumber ); //проезд через нечетный светофор
	void ChangeOddnessForHouseholdOnSpanToStation(); //смена четности хоз€йственного поезда при проезде через чужой светофор с перегона на станцию
	void UnidentifyForNoHouseholdOnSpanToStation(); //утрачивание идентификации не хоз€йственного поезда при проезде через чужой светофор с перегона на станцию
};