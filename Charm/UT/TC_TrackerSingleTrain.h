#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Элементарные изменения одиночной ПЕ

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerSingleTrain : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerSingleTrain );
	CPPUNIT_TEST( Appearance );
	CPPUNIT_TEST( AppearanceOnSwitch );
	CPPUNIT_TEST( AtomicMultiSwitchMultiAppearance );
	CPPUNIT_TEST( NamedMoving );
	CPPUNIT_TEST( NamedDoubling );
	CPPUNIT_TEST( AtomicMoving );
	CPPUNIT_TEST( AtomicMoving2 );
	CPPUNIT_TEST( MultiSwitchMoving );
	CPPUNIT_TEST( MultiSwitchMoving2 );
	CPPUNIT_TEST( RefSameTrain );
	CPPUNIT_TEST( RefDiffTrain );
	CPPUNIT_TEST( WrongSortingSideEffect );
	CPPUNIT_TEST( BidirectionalMovingForbid );
	CPPUNIT_TEST( MultiADWaysDiffStations );
	CPPUNIT_TEST( SimultaneousBusyAndFree );
	CPPUNIT_TEST( MicrostationOut );
	CPPUNIT_TEST( SaveTrainWithinComplexADWay );
	CPPUNIT_TEST( NoSaveTrainWithinComplexADWay );
	CPPUNIT_TEST_SUITE_END();

	void Appearance(); //случай возникновения ПЕ
	void AppearanceOnSwitch(); //случай возникновения на стрелке одиночной ПЕ
	void AtomicMultiSwitchMultiAppearance(); //атомарное появление нескольких ПЕ на нескольких стрелках
	void NamedMoving(); //случай элементарного перемещения известного ПЕ (на один участок)
	void NamedDoubling(); //дублирование сообщений о занятости для существующего ПЕ
	void AtomicMoving(); //атомарное (описанное одной инкрементной посылкой) смещение ПЕ
	void AtomicMoving2(); //атомарное смещение ПЕ (в другой последовательности событий)
	void MultiSwitchMoving(); //случай смещения ПЕ через несколько стрелок
	void MultiSwitchMoving2(); //случай смещения ПЕ через несколько стрелок (вариант 2: длиннее)
	void RefSameTrain(); //дублирование появления ПЕ reference-посылками
	void RefDiffTrain(); //смещение ПЕ по результатам reference-посылок
	void WrongSortingSideEffect(); //побочный эффект сортировки имен участков ЦЗ (проверка на смежность производилась не по топологическому фактору)
	void BidirectionalMovingForbid(); //запрет на движение в обе стороны
	void MultiADWaysDiffStations(); //занятие поездом нескольких п/о путей на разных станциях
	void SimultaneousBusyAndFree(); //одновременное занятие и освобождение нескольких участков
	void MicrostationOut(); //выход с микростанции (станция, состоящая из единственного ПО-участка)
	void SaveTrainWithinComplexADWay(); //сохранение поезда при движении внутри составного ПО-пути
	void NoSaveTrainWithinComplexADWay(); //нет сохранения поезда при движении внутри составного ПО-пути
};