#include "stdafx.h"
#include "TC_TechnicalTime.h"
#include "../helpful/TechnicalTime.h"
#include "../helpful/RoadCode.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TechnicalTime );

static RoadKit belorussiaRoad( RoadCode::belorussia );
static RoadKit latvianRoad( RoadCode::latvianNationalCode );
static RoadKit anyRoad;

void TC_TechnicalTime::Including1()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification(); //спецификация охватывает все возможные варианты
	specifiedTime.duration = TechnicalTime::Minutes( 10 );
	techTime.AddTime( specifiedTime );

	TechnicalTime::MinutesPtr minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN ) );
	CPPUNIT_ASSERT( minutesPtr && minutesPtr->count() == 10 );
	minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Empty ) );
	CPPUNIT_ASSERT( minutesPtr && minutesPtr->count() == 10 );
	minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Any, latvianRoad, belorussiaRoad ) );
	CPPUNIT_ASSERT( minutesPtr && minutesPtr->count() == 10 );
	minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Any, anyRoad, anyRoad ) );
	CPPUNIT_ASSERT( minutesPtr && minutesPtr->count() == 10 );
	minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Any, anyRoad, anyRoad, 
		TrainCharacteristics::TrainFeature::Refrigerator ) );
	CPPUNIT_ASSERT( !minutesPtr ); //из-за типа, который должен совпадать строго
}

void TC_TechnicalTime::Including2()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Any, 
		latvianRoad, belorussiaRoad, TrainCharacteristics::TrainFeature::TechnicalMotorcar ); //спецификация охватывает не все возможные варианты
	specifiedTime.duration = TechnicalTime::Minutes( 55 );
	techTime.AddTime( specifiedTime );

	TechnicalTime::MinutesPtr minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Any,
		latvianRoad, belorussiaRoad, TrainCharacteristics::TrainFeature::TechnicalMotorcar ) );
	CPPUNIT_ASSERT( minutesPtr && minutesPtr->count() == 55 );
	minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Empty,
		latvianRoad, belorussiaRoad, TrainCharacteristics::TrainFeature::TechnicalMotorcar ) );
	CPPUNIT_ASSERT( minutesPtr && minutesPtr->count() == 55 );
	minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Full,
		latvianRoad, belorussiaRoad, TrainCharacteristics::TrainFeature::TechnicalMotorcar ) );
	CPPUNIT_ASSERT( minutesPtr && minutesPtr->count() == 55 );
	minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Empty,
		latvianRoad, belorussiaRoad, TrainCharacteristics::TrainFeature::NoInfo ) );
	CPPUNIT_ASSERT( !minutesPtr ); //из-за типа, который должен совпадать строго
}

void TC_TechnicalTime::IntersectingAndNoIncluding()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Full, 
		belorussiaRoad, latvianRoad, TrainCharacteristics::TrainFeature::NoInfo ); //спецификация охватывает не все возможные варианты
	specifiedTime.duration = TechnicalTime::Minutes( 150 );
	techTime.AddTime( specifiedTime );

	TechnicalTime::MinutesPtr minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Any,
		belorussiaRoad, latvianRoad, TrainCharacteristics::TrainFeature::FastAllYear ) );
	CPPUNIT_ASSERT( !minutesPtr );
	minutesPtr = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Full,
		anyRoad, anyRoad, TrainCharacteristics::TrainFeature::NoInfo ) );
	CPPUNIT_ASSERT( !minutesPtr );
}

void TC_TechnicalTime::NonIntersecting()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Full, 
		anyRoad, anyRoad, TrainCharacteristics::TrainFeature::FastAllYear );
	specifiedTime.duration = TechnicalTime::Minutes( 150 );
	techTime.AddTime( specifiedTime );
	specifiedTime.specification = TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Full, 
		anyRoad, anyRoad, TrainCharacteristics::TrainFeature::Rapid );
	CPPUNIT_ASSERT( techTime.AddTime( specifiedTime ) );
	specifiedTime.specification = TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Full, 
		anyRoad, anyRoad, TrainCharacteristics::TrainFeature::Rapid );
	CPPUNIT_ASSERT( !techTime.Addable( specifiedTime.specification ) );
	const auto & timeVals = techTime.GetAll();
	CPPUNIT_ASSERT( timeVals.size() == 2 );
}

void TC_TechnicalTime::GettingWithUndefOddness()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Full, 
		belorussiaRoad, latvianRoad, TrainCharacteristics::TrainFeature::FastAllYear );
	specifiedTime.duration = TechnicalTime::Minutes( 150 );
	CPPUNIT_ASSERT( techTime.AddTime( specifiedTime ) );

	auto duratPtr1 = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Full, belorussiaRoad, latvianRoad,
		TrainCharacteristics::TrainFeature::FastAllYear ) );
	CPPUNIT_ASSERT( duratPtr1 && duratPtr1->count() == 150 );
	auto duratPtr2 = techTime.GetTime( TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Full, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::FastAllYear ) );
	CPPUNIT_ASSERT( !duratPtr2 );
	auto duratPtr3 = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Full, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::NoInfo ) );
	CPPUNIT_ASSERT( !duratPtr3 );
}

void TC_TechnicalTime::GettingWithUndefNetto()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Any, latvianRoad, belorussiaRoad, 
		TrainCharacteristics::TrainFeature::SnowRemoval );
	specifiedTime.duration = TechnicalTime::Minutes( 240 );
	CPPUNIT_ASSERT( techTime.AddTime( specifiedTime ) );

	auto duratPtr1 = techTime.GetTime( TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Empty, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::SnowRemoval ) );
	CPPUNIT_ASSERT( duratPtr1 && duratPtr1->count() == 240 );
	auto duratPtr2 = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Full, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::SnowRemoval ) );
	CPPUNIT_ASSERT( !duratPtr2 );
	auto duratPtr3 = techTime.GetTime( TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Empty, anyRoad, anyRoad,
		TrainCharacteristics::TrainFeature::SnowRemoval ) );
	CPPUNIT_ASSERT( !duratPtr3 );
}

void TC_TechnicalTime::GettingWithUndefAssignment()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Empty, anyRoad, anyRoad, 
		TrainCharacteristics::TrainFeature::Rapid );
	specifiedTime.duration = TechnicalTime::Minutes( 321 );
	CPPUNIT_ASSERT( techTime.AddTime( specifiedTime ) );

	auto duratPtr1 = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Empty, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::Rapid ) );
	CPPUNIT_ASSERT( duratPtr1 && duratPtr1->count() == 321 );
	auto duratPtr2 = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Full, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::Rapid ) );
	CPPUNIT_ASSERT( !duratPtr2 );
	auto duratPtr3 = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Empty, anyRoad, anyRoad,
		TrainCharacteristics::TrainFeature::Rapid ) );
	CPPUNIT_ASSERT( !duratPtr3 );
}

void TC_TechnicalTime::GettingWithUndefType()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Empty, belorussiaRoad, latvianRoad, 
		TrainCharacteristics::TrainFeature::NoInfo );
	specifiedTime.duration = TechnicalTime::Minutes( 455 );
	CPPUNIT_ASSERT( techTime.AddTime( specifiedTime ) );

	auto duratPtr1 = techTime.GetTime( TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Empty, belorussiaRoad, latvianRoad,
		TrainCharacteristics::TrainFeature::NoInfo ) );
	CPPUNIT_ASSERT( duratPtr1 && duratPtr1->count() == 455 );
	auto duratPtr2 = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Empty, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::NoInfo ) );
	CPPUNIT_ASSERT( !duratPtr2 );
	auto duratPtr3 = techTime.GetTime( TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Any, belorussiaRoad, latvianRoad,
		TrainCharacteristics::TrainFeature::NoInfo ) );
	CPPUNIT_ASSERT( !duratPtr3 );
	auto duratPtr4 = techTime.GetTime( TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Empty, belorussiaRoad, latvianRoad,
		TrainCharacteristics::TrainFeature::Refrigerator ) );
	CPPUNIT_ASSERT( !duratPtr4 );
}

void TC_TechnicalTime::GettingWithFullyDefined()
{
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specifiedTime;
	specifiedTime.specification = TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Empty, latvianRoad, belorussiaRoad, 
		TrainCharacteristics::TrainFeature::Selfmoving );
	specifiedTime.duration = TechnicalTime::Minutes( 570 );
	CPPUNIT_ASSERT( techTime.AddTime( specifiedTime ) );

	auto duratPtr1 = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Empty, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::Selfmoving ) );
	CPPUNIT_ASSERT( duratPtr1 && duratPtr1->count() == 570 );
	auto duratPtr2 = techTime.GetTime( TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Empty, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::Selfmoving ) );
	CPPUNIT_ASSERT( !duratPtr2 );
	auto duratPtr3 = techTime.GetTime( TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Empty, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::Rapid ) );
	CPPUNIT_ASSERT( !duratPtr3 );
}