#pragma once

//проверка генерации событий формирования/расформирования поездов

//подразумевается, что все светоофоры находятся в запрещающем состоянии, 
//если не указано иное

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerFormDisform : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerFormDisform );
	CPPUNIT_TEST( NoDisformCauseLocoMove );
	CPPUNIT_TEST( NoDisformEmptyNumber );
	CPPUNIT_TEST( FormDisform );
	CPPUNIT_TEST( MultiForm );
	CPPUNIT_TEST( DisformAndDeath );
	CPPUNIT_TEST( NoFormOnOnlyOpenSema );
	CPPUNIT_TEST( FormOnOpenRoute );
	CPPUNIT_TEST( LeaveADWayOnOppositeOpenedRoute );
	CPPUNIT_TEST( LeaveADWayOnOppositeClosedRoute );
	CPPUNIT_TEST( LeaveADWayOnTrainSemaAndDisformOnSema );
	CPPUNIT_TEST( LeaveADWayOnShuntSemaAndDisformOnSema );
	CPPUNIT_TEST( LeaveADWayWOSemaAndDisformOnSema );
	CPPUNIT_TEST( FromADWayToSpanOnClosedTrainSemaAndGoToObscurity );
	CPPUNIT_TEST( FromADWayToSpanOnOpenedTrainSemaAndGoToObscurity );
	CPPUNIT_TEST( LeaveADWayWithSemaAndGoToObscurity );
	CPPUNIT_TEST( LeaveADWayWOSemaAndGoToObscurity );
	CPPUNIT_TEST( StatEntryWOADWayDisformOnSema );
	CPPUNIT_TEST( StatEntryAndDeath );
	CPPUNIT_TEST( StatEntryWOADWayGoToNoTSStrip );
	CPPUNIT_TEST( StatEntryWOADWayGoToObscurity );
	CPPUNIT_TEST( StatEntryWOADWayGoToSpanObscurity );
	CPPUNIT_TEST( MoveADWayToADWayAndSemaDisform );
	CPPUNIT_TEST( NoDisformOnSpan );
	CPPUNIT_TEST( StationMigrationFromADWay );
	CPPUNIT_TEST( PassingTwoAlienSemasWithNumForwarding );
	CPPUNIT_TEST( RecreateOnStationFromSpanEntryThroughAlienSema );
	CPPUNIT_TEST( RecreateOnStationFromSpanEntryThroughAlienSema2 );
	CPPUNIT_TEST( MultiTrainsThroughAlienSema );
	CPPUNIT_TEST( MultiTrainsThroughAlienSema2 );
	CPPUNIT_TEST( CompeteForPlaceWhileRecreating );
	CPPUNIT_TEST( LosingOddnesOnDisform );
	CPPUNIT_TEST( IgnoringUndefSemaFromSpanToStat );
	CPPUNIT_TEST( LongtimeDisformLosing );
	CPPUNIT_TEST( RebornInsteadOutThenDeath );
	CPPUNIT_TEST( ADFormNearObscurity );
	CPPUNIT_TEST( ADFormNearObscurity2 );
	CPPUNIT_TEST( ADNoFormNearObscurity );
	CPPUNIT_TEST( ADNoFormNearObscurity2 );
	CPPUNIT_TEST( EventMomentOnDispositionDisform );
	CPPUNIT_TEST( ClosedWayOutAndGoToObscurity );
	CPPUNIT_TEST( OpenedWayOutAndGoToObscurity );
	CPPUNIT_TEST( HistoryLimitationObserve );
	CPPUNIT_TEST( ExtFormNSemaDisform );
	CPPUNIT_TEST_SUITE_END();

	void NoDisformCauseLocoMove(); //отсутствие расформирования после отъезда локомотива от поезда
	void NoDisformEmptyNumber(); //отсутствие расформирования для поезда без номера
	void FormDisform(); //формирование и расформирование поезда
	void MultiForm(); //формирование поезда несколько раз
	void DisformAndDeath(); //расформирование поезда после проезда светофора неправильной четности и его удаление
	void NoFormOnOnlyOpenSema(); //отсутствие формирования при выезде с ПО пути только на открытый поездной светофор (без маршрута)
	void FormOnOpenRoute(); //формирование при движении при выезде с ПО пути на открытый маршрут

	void LeaveADWayOnOppositeOpenedRoute(); //выезд с п/о пути по открытому маршруту в противоположную сторону
	void LeaveADWayOnOppositeClosedRoute(); //выезд с п/о пути по закрытому маршруту в противоположную сторону
	//выезд с п/о пути на поездной светофор соответствующей четности с последующим расформированием при движении по станции через светофор чужой четности:
	void LeaveADWayOnTrainSemaAndDisformOnSema();
	//выезд с п/о пути на маневровый светофор соответствующей четности с последующим расформированием при движении по станции через светофор чужой четности:
	void LeaveADWayOnShuntSemaAndDisformOnSema();
	//выезд с п/о пути без светофора с последующим расформированием при движении по станции через светофор чужой четности:
	void LeaveADWayWOSemaAndDisformOnSema();
	//выезд с п/о пути на закрытый поездной светофор соответствующей четности (маршрут не задан) с последующим расформированием при выходе из зоны отслеживания:
	void FromADWayToSpanOnClosedTrainSemaAndGoToObscurity();
	//выезд с п/о пути на открытый поездной светофор соответствующей четности (заданный маршрут) с последующим расформированием при выходе из зоны отслеживания:
	void FromADWayToSpanOnOpenedTrainSemaAndGoToObscurity();
	//выезд с п/о пути через светофор соответствующей четности с последующим расформированием при выходе из зоны отслеживания:
	void LeaveADWayWithSemaAndGoToObscurity();
	//выезд с п/о пути без светофора с последующим расформированием при выходе из зоны отслеживания:
	void LeaveADWayWOSemaAndGoToObscurity();
	//въезд на станцию без занятия п/о пути
	//и последующее расформирование при движении по станции через светофор чужой четности:
	void StatEntryWOADWayDisformOnSema();
	//въезд на станцию без занятия п/о пути с последующим уходом в неконтролируемую зону
	void StatEntryWOADWayGoToObscurity();
	//въезд на станцию без занятия п/о пути с последующим выходом на перегон и уходом в неконтролируемую зону
	void StatEntryWOADWayGoToSpanObscurity();
	//въезд на станцию без занятия п/о пути с последующим уходом на участок без ТС:
	void StatEntryWOADWayGoToNoTSStrip();
	//въезд на станцию и последующая смерть поезда (исчезновение без видимых причин):
	void StatEntryAndDeath();
	//переезд с п/о пути на п/о путь с последующим расформированием на светофоре чужой четности
	void MoveADWayToADWayAndSemaDisform();
	//отсутствие расформирования при движении по перегону
	void NoDisformOnSpan();
	//переезд поезда со станции на станцию прямо с ПО пути
	void StationMigrationFromADWay();
	//случай проезда поезда через два светофора чужой четности с одновременной перестановкой номера вперед
	void PassingTwoAlienSemasWithNumForwarding();
	//пересоздание поезда с новым номером при въезде с перегона на станцию через чужой светофор
	void RecreateOnStationFromSpanEntryThroughAlienSema();
	void RecreateOnStationFromSpanEntryThroughAlienSema2();
	//пересоздание поезда с новым номером при полном выезде с перегона на станцию через чужой светофор
	void RecreateOnFullSpanLeaveThroughAlienSema();
	//несколько поездов перед светофором чужой четности и появление занятости за светофором
	void MultiTrainsThroughAlienSema();
	void MultiTrainsThroughAlienSema2();
	//конкуренция между двумя поездами за занятие пути с одновременным пересозданием одного из них на чужом светофоре
	void CompeteForPlaceWhileRecreating();
	void LosingOddnesOnDisform(); //расформирование поезда должно приводить к потере четности
	void IgnoringUndefSemaFromSpanToStat(); //при проезде светофора с неопределенной четностью с перегона на станцию поезд сохраняет четность
	//выезд с п/о пути на светофор правильной четности и последующее ошибочное удаление события расформирования по истечению времени хранения (без отправки на ГИД)
	void LongtimeDisformLosing();
	//пересоздание поезда на границе ПО-пути вместо нормального выезда с него
	void RebornInsteadOutThenDeath();
	void ADFormNearObscurity(); //формирование поезда на ПО-пути рядом с неконтролируемой зоной
	void ADFormNearObscurity2();
	void ADNoFormNearObscurity(); //нет формирования поезда на ПО-пути рядом с неконтролируемой зоной
	void ADNoFormNearObscurity2();
	void EventMomentOnDispositionDisform(); //момент события расформирования должен совпадать со временем прихода уведомления о расформировании в случае внешнего источника
	void ClosedWayOutAndGoToObscurity(); //выход поезда на закрытый поездной светофор и проезд до граничного станционного участка
	void OpenedWayOutAndGoToObscurity(); //выход поезда на открытый поездной светофор и проезд до граничного станционного участка
	void HistoryLimitationObserve(); //обзор истории для определения времени ограничивается текущей станцией
	void ExtFormNSemaDisform(); //формирование поезда по номеру и расформирование по движению через светфор чужой четности
};