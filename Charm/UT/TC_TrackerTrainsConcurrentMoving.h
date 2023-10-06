#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Тесты на одновременное движение множественных ПЕ

class TC_TrackerTrainsConcurrentMoving : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainsConcurrentMoving );
	CPPUNIT_TEST( UnknownMoving );
	CPPUNIT_TEST( RacingMovingSinglePlaces );
	CPPUNIT_TEST( RacingMovingMultiPlaces );
	CPPUNIT_TEST( CloseMoving );
	CPPUNIT_TEST( CloseMoving2 );
	CPPUNIT_TEST( CloseMoving3 );
	CPPUNIT_TEST( CloseMoving4 );
	CPPUNIT_TEST( ChainShortening );
	CPPUNIT_TEST( ChainShortening2 );
	CPPUNIT_TEST( ChainShortening3 );
	CPPUNIT_TEST( ChainShortening4 );
	CPPUNIT_TEST( ChainShortening5 );
	CPPUNIT_TEST( ChainShortening6 );
	CPPUNIT_TEST( ChainShortening7 );
	CPPUNIT_TEST( SpanChainShortening );
	CPPUNIT_TEST( ChainBreaking );
	CPPUNIT_TEST( ParallelMovingNearWrongSema );
	CPPUNIT_TEST( TrainChaseNoTrainThroughWrongSema );
	CPPUNIT_TEST( TrainChaseNoTrainThroughGoodSema );
	CPPUNIT_TEST( TrainChaseTwoNoTrainsThroughWrongSema );
	CPPUNIT_TEST( TrainChaseTwoNoTrainsThroughGoodSema );
	CPPUNIT_TEST( TrainChaseTwoNoTrainsThroughGoodSema2 );
	CPPUNIT_TEST( TrainAbsorbsNoTrainThroughGoodSema );
	CPPUNIT_TEST( PushByDyingThroughStripFast );
	CPPUNIT_TEST( PushByDyingThroughStripSlow );
	CPPUNIT_TEST( ApproachingSpanTrainVsExitingStatTrain );
	CPPUNIT_TEST( LeavingSpanTrainVsExitingStatTrain );
	CPPUNIT_TEST( LeavingSpanTrainVsExitingStatTrain2 );
	CPPUNIT_TEST_SUITE_END();

	void UnknownMoving(); //перемещение двух неизвестных ПЕ (со одним свободным участком между собой) в одну сторону на один участок
	void RacingMovingSinglePlaces(); //одновременное движение двух поездов с наездом заднего поезда на место переднего. каждый из поездов занимает одно место
	//одновременное движение двух поездов с наездом заднего поезда на место переднего. целевой поезд занимает более одного места
	void RacingMovingMultiPlaces();

	void CloseMoving(); //движение вплотную
	void CloseMoving2(); //движение вплотную (ситуация 2)
	void CloseMoving3(); //движение вплотную (ситуация 3)
	void CloseMoving4(); //движение вплотную (ситуация 4, длинная маневровая и короткий поезд)
	void ChainShortening(); //укорачивание ЦЗ (ситуация 1)
	void ChainShortening2(); //укорачивание ЦЗ (ситуация 2)
	void ChainShortening3(); //укорачивание ЦЗ (ситуация 3)
	void ChainShortening4(); //укорачивание ЦЗ (ситуация 4)
	void ChainShortening5(); //укорачивание ЦЗ (ситуация 5)
	void ChainShortening6(); //укорачивание ЦЗ (ситуация 6)
	void ChainShortening7();
	void SpanChainShortening(); //укорачивание ЦЗ на перегоне
	void ChainBreaking(); //гашение середины ЦЗ в случае двух поездов, стоящих вплотную
	void ParallelMovingNearWrongSema(); //одновременное смещение поездов около светофора чужой четности
	void TrainChaseNoTrainThroughWrongSema(); //поезд догоняет маневровую, проезжая через светофор чужой четности
	void TrainChaseNoTrainThroughGoodSema(); //поезд догоняет маневровую, проезжая через светофор своей четности
	void TrainChaseTwoNoTrainsThroughWrongSema(); //поезд догоняет две маневровые, проезжая через светофор чужой четности
	void TrainChaseTwoNoTrainsThroughGoodSema(); //поезд догоняет две маневровые, проезжая через светофор своей четности
	void TrainChaseTwoNoTrainsThroughGoodSema2();
	void TrainAbsorbsNoTrainThroughGoodSema(); //слияние маневровой с поездом после его наезда на маневровую через светофор своей четности
	void PushByDyingThroughStripFast(); //проталкивание умирающим поездом соседа через участок на место маневровой
	void PushByDyingThroughStripSlow();
	void PushByDyingThroughStrip( bool slowly );
	void ApproachingSpanTrainVsExitingStatTrain(); //при прочих равных предпочтение отдается подъезжающему перегонному поезду перед выходящим со станции
	void LeavingSpanTrainVsExitingStatTrain(); //при прочих равных предпочтение отдается поезду, выходящему со станции, перед отъезжающим перегонным
	void LeavingSpanTrainVsExitingStatTrain2();
};