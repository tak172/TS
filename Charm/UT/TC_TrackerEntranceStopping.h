#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка правильности генерации событий отслеживания для отправки на Hem

class TC_TrackerEntranceStopping : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerEntranceStopping );
    CPPUNIT_TEST( DelayAndGenerate );
    CPPUNIT_TEST( ShortDelay );
    CPPUNIT_TEST( FormWOGeneration );
    CPPUNIT_TEST( ShortDelayAndMove );
    CPPUNIT_TEST( JointPicketage );
    CPPUNIT_TEST( SpanExitAndStop );
	CPPUNIT_TEST( BlockpostStopping );
	CPPUNIT_TEST( OpenSemaStopping );
	//CPPUNIT_TEST( StopEventOnSemaClose ); //закомментирован из-за удаления п.5.3 по генерации стоянки перед входным по факту его перекрытия
	CPPUNIT_TEST( NoStopEventOnSemaClose );
	CPPUNIT_TEST( ShortBlockpostFastPassing );
	CPPUNIT_TEST( DeathAfterStop );
	CPPUNIT_TEST( RecloseSemaAfterStop );
	CPPUNIT_TEST( LittleStopOnShortSpan );
	CPPUNIT_TEST( NoEntStopOnSingleStripSABWay );
	CPPUNIT_TEST( EntStopOnMultiStripSABWay );
	CPPUNIT_TEST( EntStopOnSingleStripFABWay );
	CPPUNIT_TEST_SUITE_END();

    void DelayAndGenerate(); //стоянка перед входным и генерация события
    void ShortDelay(); //проезд светофора без задержки
    void FormWOGeneration(); //формирование поезда перед светофором (событие не генерируется)
    void ShortDelayAndMove(); //короткая стоянка перед входным с последующим въездом на станцию
    void JointPicketage(); //проверка передачи только пикетажа стыка мачты входного светофора
    void SpanExitAndStop(); //выезд на перегон и остановка за светофором
	void BlockpostStopping(); //стоянки перед блок-постом (событие стоянки перед входным не генерируется)
	void OpenSemaStopping(); //стоянка перед открытым светофором
	void StopEventOnSemaClose(); //генерация события стоянки перед входным по закрытию светофора
	void NoStopEventOnSemaClose(); //отсутствие события стоянки перед входным по закрытию станционного светофора
	void ShortBlockpostFastPassing(); //быстрое проследование короткого блок-поста со стоянкой перед входным на следующей станции
	void DeathAfterStop(); //смерть поезда после остановки перед входным
	void RecloseSemaAfterStop(); //стоянка перед входным с последующим открытием и закрытием светофора
	void LittleStopOnShortSpan(); //небольшая остановка на коротком перегона
	void NoEntStopOnSingleStripSABWay(); //отсутствие стоянки перед входным в случае пути перегона с единственным участком и ПАБ
	void EntStopOnMultiStripSABWay(); //стоянка перед входным в случае пути перегона с несколькими участками и ПАБ
	void EntStopOnSingleStripFABWay(); //стоянка перед входным в случае пути перегона с единственным участком и АБ
};