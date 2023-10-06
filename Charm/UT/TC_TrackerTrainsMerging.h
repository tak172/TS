#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//ситуации слияния одной или нескольких ПЕ

class TC_TrackerTrainsMerging : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainsMerging );
	CPPUNIT_TEST( MergingTrainWithNearUnity );
	CPPUNIT_TEST( MergeTrainWithUnityOnSpan );
	CPPUNIT_TEST( MergeTrainWithUnityOnSema );
	CPPUNIT_TEST( TrainAbsorptionOfNotrain );
	CPPUNIT_TEST( TrainAbsorptionOfNotrain2 );
	CPPUNIT_TEST( TrainAbsorptionOfNotrain3 );
	CPPUNIT_TEST( NotrainEmbeddingIntoTrain );
	CPPUNIT_TEST( NotrainEmbeddingIntoTrain2 );
	CPPUNIT_TEST( NotrainEmbeddingIntoTrain3 );
	CPPUNIT_TEST( NotrainEmbeddingIntoTrain4 );
	CPPUNIT_TEST( NoMergingWithLeftTrain );
	CPPUNIT_TEST_SUITE_END();

	void MergingTrainWithNearUnity(); //слияние поезда и находящейся рядом ПЕ (не поездом)
	void MergeTrainWithUnityOnSpan(); //слияние поезда с не поездом на перегоне
	void MergeTrainWithUnityOnSema(); //слияние поезда и маневровой около светофора
	void TrainAbsorptionOfNotrain(); //поглощение поездом ПЕ при ее подъезде в голову поезду на ПО-путь (поезд не должен двигаться)
	void TrainAbsorptionOfNotrain2();
	void TrainAbsorptionOfNotrain3();
	void NotrainEmbeddingIntoTrain(); //встраивание маневровой в поезд при ее движении
	void NotrainEmbeddingIntoTrain2(); //"въезд" маневровой ПЕ в поезд (слияние при приближении маневровой к поезду)
	void NotrainEmbeddingIntoTrain3();
	void NotrainEmbeddingIntoTrain4();
	void NoMergingWithLeftTrain(); //слияние с поездом, покинувшим ЦЗ, приводит к ошибке
};