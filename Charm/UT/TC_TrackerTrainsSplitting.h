#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//ситуации разделени€ одной или нескольких ѕ≈

class TC_TrackerTrainsSplitting : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainsSplitting );
	CPPUNIT_TEST( SwitchSplittingOfTrain );
	CPPUNIT_TEST( StripSplittingOfTrain );
	CPPUNIT_TEST( StripSplittingOfUnknown );
	CPPUNIT_TEST( LengthyDeclutching );
	CPPUNIT_TEST( MultiSeparation );
	CPPUNIT_TEST( SplittingAndRecovering );
	CPPUNIT_TEST( SplitTrainNearAnotherOne );
	CPPUNIT_TEST( OccupyNoTrainAfterADOut );
	CPPUNIT_TEST( MultiTrainRoutesOnADWayOut );
	CPPUNIT_TEST_SUITE_END();

	void SwitchSplittingOfTrain(); //разделение поезда после перевода стрелки
	void StripSplittingOfTrain(); //разделение поезда после гашени€ участка в середине цепочки
	void StripSplittingOfUnknown(); //разделение Ќ≈ поезда после гашени€ участка в середине цепочки
	void LengthyDeclutching(); //расцепление ѕ≈, занимающей большое количество участков (более глубины обхода графа)
	void MultiSeparation(); //разделение двух ѕ≈ одновременно
	void SplittingAndRecovering(); //одновременное разделение и восстановление поезда
	void SplitTrainNearAnotherOne(); //разделение поезда стрелкой при наличии соседного
	void OccupyNoTrainAfterADOut(); //разделение на выезде с ѕќ-пути и последующий наезд на маневровую
	void MultiTrainRoutesOnADWayOut(); //множественные поездные маршруты могут приводить к ошибочному разрыву поезда
};