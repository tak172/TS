#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка генерации событий прибытия

class TC_TrackerArrival : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerArrival );
	CPPUNIT_TEST( CoupledSlowArriving );
	CPPUNIT_TEST( OppositeArriving );
    CPPUNIT_TEST( ArrivingToOpenWay );
    CPPUNIT_TEST( ArrivingToClosedWay );
    CPPUNIT_TEST( ArrivingToWayWOSema );
	CPPUNIT_TEST( FastArrivingToWayWOSema );
	CPPUNIT_TEST( CloseWayUncontainedArriving );
	CPPUNIT_TEST( OpenWayAsyncArrivingPassenger );
	CPPUNIT_TEST( OpenWayAsyncArrivingCargo );
	CPPUNIT_TEST( ArrivingToOpenWayAndCloseSema );
	CPPUNIT_TEST( ArrivingToOpenWayAndCloseSema2 );
	CPPUNIT_TEST( ArrivingToClosedWayAndReversiveExit );
	CPPUNIT_TEST( UncontainedArrivingPassAndBack );
	CPPUNIT_TEST( OpenWayAsyncArrivingMomentFullEntry );
	CPPUNIT_TEST( OpenWayAsyncArrivingMomentFullEntry2 );
	CPPUNIT_TEST( OpenWayAsyncArrivingMomentPartialEntry );
	CPPUNIT_TEST( ForbidArrivingAfterFormOnSameADWay );
	CPPUNIT_TEST( PermitArrivingAfterForm );
	CPPUNIT_TEST( ForbidArrivingAfterFormOnSameADWay2 );
	CPPUNIT_TEST( ArrivingToBusyWay );
	CPPUNIT_TEST( NoArrivingOnHeadCloseWithNoTrain );
	CPPUNIT_TEST( NoAsyncArrivalForNoTrain );
	CPPUNIT_TEST_SUITE_END();

	void CoupledSlowArriving(); //постепенное прибытие на два п/о пути
	void OppositeArriving(); //прибытие на п/о путь с разных сторон
    void ArrivingToOpenWay(); //прибытие на п/о путь при открытом попутном светофоре
    void ArrivingToClosedWay(); //прибытие на п/о путь при закрытом попутном светофоре
    void ArrivingToWayWOSema(); //прибытие на п/о путь при отсутствии попутного светофора
	void FastArrivingToWayWOSema(); //быстрое прибытие на п/о путь при отсутствии попутного светофора
	void CloseWayUncontainedArriving(); //прибытие протяженного поезда, который не помещается на п/о путь целиком
	void OpenWayAsyncArrivingPassenger(); //асинхронная генерация события прибытия при проезде п/о пути с открытым светофором пассажирского поезда
	void OpenWayAsyncArrivingCargo(); //асинхронная генерация события прибытия при проезде п/о пути с открытым светофором грузового поезда
	void ArrivingToOpenWayAndCloseSema(); //прибытие на п/о путь при открытом попутном светофоре с последующим его перекрытием
	void ArrivingToOpenWayAndCloseSema2(); //частичное прибытие на п/о путь при открытом попутном светофоре с последующим его перекрытием
	void ArrivingToClosedWayAndReversiveExit(); //асинхронное прибытие на п/о путь с закрытым светофором и возвращение на тот же перегон
	void UncontainedArrivingPassAndBack(); //прибытие протяженного поезда (хвост на перегоне), проезд светофора на красный и осаживание
	void OpenWayAsyncArrivingMomentFullEntry(); //тест на правильный момент события прибытия по факту слишком долгого нахождения на пути (полный въезд)
	void OpenWayAsyncArrivingMomentFullEntry2();
	void OpenWayAsyncArrivingMomentPartialEntry(); //тест на правильный момент события прибытия по факту слишком долгого нахождения на пути (частичный въезд)
	void ForbidArrivingAfterFormOnSameADWay(); //поезд не может одновременно сформироваться и прибыть на тот ПО-путь в тот же момент
	void ForbidArrivingAfterFormOnSameADWay2(); //случай перекрытия головы светофора
	void PermitArrivingAfterForm(); //допустимость прибытия после формирования, если поезд сформировался не на ПО-пути
	void ArrivingToBusyWay(); //прибытие на занятый путь с последующим уничтожением
	void NoArrivingOnHeadCloseWithNoTrain(); //закрытие светофора не создает прибытия для маневровой
	void NoAsyncArrivalForNoTrain(); //при рассмотрении истории для асинхронного прибытия не интересуют инциденты для маневровых
};