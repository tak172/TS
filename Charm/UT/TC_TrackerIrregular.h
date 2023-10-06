#pragma once

//проверка обработки нестандартных ситуаций и некорректных сигналов

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerIrregular : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerIrregular );
	CPPUNIT_TEST( UnlinkedSpanStrip ); //перегонный участок, не связанный ни с кем, не отслеживается
	CPPUNIT_TEST( DiscordantMoveMerging ); //слияние поезда и ПЕ при несогласованном движении по перегону
	CPPUNIT_TEST( AbnormalLightningOnSwitch ); //некорректное поведение на стрелке
	CPPUNIT_TEST( AbnormalLightningOnSwitch2 ); //некорректное поведение на стрелке
	CPPUNIT_TEST( TrainVsNotrainDisappearance ); //занятие общего участка двумя ПЕ (выбор между поездом и не поездом)
	CPPUNIT_TEST( ChangesAfterCrush ); //фиксация изменения после сброса (вылета) модуля отслеживания
	CPPUNIT_TEST( TrainContentVsDirectionDisappearance ); //занятие общего участка двумя ПЕ (приоритет перед направлением движения имеет полнота информации о поезде)
    CPPUNIT_TEST( DoubleArrivingByReturn ); //дублирование события прибытия при "возвращении" ПЕ на п/о путь (за счет мигания п/о пути)
	CPPUNIT_TEST( WreckingADPassing ); //ненормальный проезд через п/о путь (въезд, проезд на закрытый светофор, осаживание и нормальное отправление)
	CPPUNIT_TEST( DeathRecoverAndErrorStopping ); //восстановление поезда после исчезновения и ошибочная генерация стоянки перед входным
	CPPUNIT_TEST( AsyncEraseDisappeared ); //асинхронная зачистка исчезнувшего поезда по истечении отведенного лимита времени
	CPPUNIT_TEST( TransitionReturnExit ); //проследование п/о пути, возвращение и выход на перегон
	CPPUNIT_TEST( BlinkSeveralEntranceAfterArrival ); //мигание нескольких въездных участков после прибытия поезда на п/о путь
	CPPUNIT_TEST( InternalUnlightWithNeighbour ); //гашение середины поезда при наличии по соседству еще одной ПЕ
	CPPUNIT_TEST( TrainPassThroughFakeBusy ); //проезд поезда через участок с ложной занятостью
	CPPUNIT_TEST( ReversiveMoveOnSpan ); //обратное движение поезда на перегоне
	CPPUNIT_TEST( ReversiveMoveOnSema ); //обратное движение поезда через светофор
	CPPUNIT_TEST( MultiSectionFaultBusy ); //ложная занятость из двух раздельных секций (две маневровые)
	CPPUNIT_TEST( TrainThroughFaultFreeOnSpan ); //проезд одного поезда через ложную свободность (на перегоне)
	CPPUNIT_TEST( TrainThroughFaultFreeOnADOut ); //проезд одного поезда через ложную свободность (выход с ПО пути)
	CPPUNIT_TEST( TrainThroughFaultFreeOnShuntSema ); //проезд одного поезда через ложную свободность (проезд через маневровый светофор)
	CPPUNIT_TEST( TrainThroughFaultFreeOnShuntSema2 ); //проезд одного поезда через ложную свободность (проезд через маневровый светофор)
	CPPUNIT_TEST( TwoTrainsThroughFaultFree ); //проезд двух поездов через ложную свободность
	CPPUNIT_TEST( InsensitivityToInsignificantFree ); //ничего не меняющая свободность не должна приводить к появлению новой ПЕ
	CPPUNIT_TEST( BlinkingOnActiveRoute ); //моргание занятости на части заданного маршрута (сразу после светофора)
	CPPUNIT_TEST_SUITE_END();

	void UnlinkedSpanStrip();
	void DiscordantMoveMerging();
	void AbnormalLightningOnSwitch();
	void AbnormalLightningOnSwitch2();
	void TrainVsNotrainDisappearance();
	void ChangesAfterCrush();
	void TrainContentVsDirectionDisappearance();
    void DoubleArrivingByReturn();
	void WreckingADPassing();
	void DeathRecoverAndErrorStopping();
	void AsyncEraseDisappeared();
	void TransitionReturnExit();
	void BlinkSeveralEntranceAfterArrival();
	void InternalUnlightWithNeighbour();
	void TrainPassThroughFakeBusy();
	void ReversiveMoveOnSpan();
	void ReversiveMoveOnSema();
	void MultiSectionFaultBusy();
	void TrainThroughFaultFreeOnSpan();
	void TrainThroughFaultFreeOnADOut();
	void TrainThroughFaultFreeOnShuntSema();
	void TrainThroughFaultFreeOnShuntSema2();
	void TwoTrainsThroughFaultFree();
	void InsensitivityToInsignificantFree();
	void BlinkingOnActiveRoute();
};