#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка генерации событий отправления

class TC_TrackerDeparture : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerDeparture );
	CPPUNIT_TEST( Departure );
	CPPUNIT_TEST( OpenSemaDepartureWithRoute );
	CPPUNIT_TEST( OpenSemaDepartureWORoute );
	CPPUNIT_TEST( OpenMultiWayDeparture );
	CPPUNIT_TEST( CloseWayExit );
	CPPUNIT_TEST( CloseWayExitAndStationExit );
	CPPUNIT_TEST( WOSemaWayExit );
	CPPUNIT_TEST( WOSemaWayExitAndStationExit );
	CPPUNIT_TEST( BanTransitionAfterStart );
	CPPUNIT_TEST( FormDepartureAndSpanMove );
	CPPUNIT_TEST( FormSpanMoveAndLeaveAD );
	CPPUNIT_TEST( CompositeRouteDeparture );
	CPPUNIT_TEST( DuplicateEventOnReborn );
	CPPUNIT_TEST( DelayedDepartureForExitingNoTrain );
	CPPUNIT_TEST( DelayedDepartureForExitingNoTrain2 );
	CPPUNIT_TEST( DelayedDepartureForExitingNoTrain3 );
	CPPUNIT_TEST( DelayedDepartureForExitingNoTrain4 );
	CPPUNIT_TEST( DepartureWithRecoveryForExitingNoTrain );
	CPPUNIT_TEST( DepartureWithRecoveryForExitingNoTrain2 );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain2 );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain3 );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain4 );
	CPPUNIT_TEST( DelayedDepartureForFormingTrain5 );
	CPPUNIT_TEST( DelayedDepartureADOutUpdate );
	CPPUNIT_TEST( DelayedDepartureADOutUpdate2 );
	CPPUNIT_TEST( ADOutThenReturnThenUpgradeThenExit );
	CPPUNIT_TEST( DelayedDepartureForLongTrain );
	CPPUNIT_TEST( MixingUpDepartureFactOnRecovering );
	CPPUNIT_TEST_SUITE_END();

	void Departure(); //отправление с п/о пути
	void OpenSemaDepartureWithRoute(); //отправление с п/о пути на разрешающий сигнал светофора с заданным поездным маршрутом
	void OpenSemaDepartureWORoute(); //отправление с п/о пути на разрешающий сигнал светофора без заданного поездного маршрута
	void OpenMultiWayDeparture(); //отправление с п/о пути (из нескольких участков) на разрешающий сигнал светофора
	void CloseWayExit(); //выезд с п/о пути на запрещающий сигнал светофора
	void CloseWayExitAndStationExit(); //выезд с п/о пути на запрещающий сигнал светофора с последующим выездом со станции
	void WOSemaWayExit(); //выезд с п/о пути при отсутствии светофора
	void WOSemaWayExitAndStationExit(); //выезд с п/о пути при отсутствии светофора с последующим выездом со станции
	void BanTransitionAfterStart(); //первым событием после начала работы отслеживания не может быть проследование
	void FormDepartureAndSpanMove(); //формирование поезда после отправления и выезд на перегон
	void FormSpanMoveAndLeaveAD(); //формирование поезда, выход на перегон и выезд с п/о пути
	void CompositeRouteDeparture(); //отправление со станции по двум заданным маршрутам
	void DuplicateEventOnReborn(); //дублирование события после пересоздания поезда
	void DelayedDepartureForExitingNoTrain(); //при превращении маневровой в поезд по выходу со станции генерируется также и отправление с последнего ПО-пути
	void DelayedDepartureForExitingNoTrain2();
	void DelayedDepartureForExitingNoTrain3();
	void DelayedDepartureForExitingNoTrain4();
	void DelayedDepartureForFormingTrain(); //отложенное отправление с последнего ПО-пути при формировании поезда из маневровой
	void DelayedDepartureForFormingTrain2();
	void DelayedDepartureForFormingTrain3();
	void DelayedDepartureForFormingTrain4();
	void DelayedDepartureForFormingTrain5();
	void DelayedDepartureADOutUpdate(); //обновление информации о выходе с ПО-пути для маневровой при ее переезде на другой ПО-путь
	void DelayedDepartureADOutUpdate2();
	void ADOutThenReturnThenUpgradeThenExit(); //выезд маневровой с ПО-пути с последующим возвращением, апгрейдом и выходом со станции уже поезда
	void DelayedDepartureForLongTrain(); //отправление длинного поезда по закрытому светфору
	void DepartureWithRecoveryForExitingNoTrain(); //отправление, совмещенное с исчезновением и восстановлением маневровой
	void DepartureWithRecoveryForExitingNoTrain2();
	void MixingUpDepartureFactOnRecovering(); //перепутывание фактов отправления с разных ПО-путей из-за восстановления
};