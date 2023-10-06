#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка правильности генерации событий отслеживания для отправки на Hem

class TC_TrackerEvents : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerEvents );
	CPPUNIT_TEST( SpanToSpanByOrdInc );
	CPPUNIT_TEST( TwoTrainsByInc );
	CPPUNIT_TEST( OneTrainByUnordInc );
	CPPUNIT_TEST( OccupationOwnStrip );
	CPPUNIT_TEST( StationToStation );
	CPPUNIT_TEST( PocketMove );
	CPPUNIT_TEST( JointCoordinates );
	CPPUNIT_TEST( ParkWayNumbers );
	CPPUNIT_TEST( NoTrainDeath );
    CPPUNIT_TEST( BanCoordinatesAtStaion );
	CPPUNIT_TEST( YieldNumber );
    CPPUNIT_TEST( RepeatInfoChanging );
	CPPUNIT_TEST( LightFadeArrdep );
	CPPUNIT_TEST( EntryInfoChangeExit );
	CPPUNIT_TEST( UTurnAfterStationPartialEntryVar1 );
	CPPUNIT_TEST( UTurnAfterStationPartialEntryVar2 );
	CPPUNIT_TEST( HeadPlaceInDeath );
	CPPUNIT_TEST( FlashLeaving );
	CPPUNIT_TEST( PicketageOnForm );
	CPPUNIT_TEST( FaultStickingTrainKnowledge );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST( DisformTimeOnReachingEndOfStation );
	CPPUNIT_TEST( SameTimeEvents );
	CPPUNIT_TEST_SUITE_END();

	void SpanToSpanByOrdInc(); //продвижение ПЕ от перегона к перегону по упорядоченным инкрементным посылкам
	void TwoTrainsByInc(); //возникновение двух ПЕ по инкрементным посылкам
	void OneTrainByUnordInc(); //возникновение одной ПЕ по неупорядоченным инкрементным посылкам
	void OccupationOwnStrip(); //занятие ПЕ участков, уже входящих в ее состав
	void StationToStation(); //переезд со станции на станцию
	void PocketMove(); //вход/выход из "кармана"
	void JointCoordinates(); //наличие координат в событиях при проезде через стыки с координатами
	void ParkWayNumbers(); //наличие номеров путей и парков для п/о путей
	void NoTrainDeath(); //при исчезновении "не поезда" событие DEATH не генерируется
    void BanCoordinatesAtStaion(); //для станционных событий не передаются координаты (даже если они есть)
	void YieldNumber(); //корректная генерация события при уступке номера (назначение существующего номера другому поезду)
    void RepeatInfoChanging(); //корректная генерация события при повторной установке того же номера (изменилась лишь усл. длина)
	void LightFadeArrdep(); //загорание и гашение п/о пути
	void EntryInfoChangeExit(); //смена информации по поезду между входом и выходом со станции
	void UTurnAfterStationPartialEntryVar1(); //разворот поезда после частичного въезда на станцию (вариант 1)
	void UTurnAfterStationPartialEntryVar2(); //разворот поезда после частичного въезда на станцию (вариант 2)
	void HeadPlaceInDeath(); //в событии смерти поезда должно указываться место головы поезда
	void FlashLeaving(); //покидание поезда п/о пути с одновременным гашением всех занимаемых участков
	void CheckUTurnEvents();
	void PicketageOnForm(); //пикетаж в событии формирования на неком обычном пути
	void FaultStickingTrainKnowledge(); //информация о наличии поезда на станции ошибочно остается при пересоздании поезда на перегоне через границу со станцией
	void Serialize();
	void Deserialize();
	void DisformTimeOnReachingEndOfStation(); //корректное определение времения расформирования при достижении границы станции
	void SameTimeEvents(); //из-за длительных задержек посылок не должно генерироваться событий с одинаковыми моментами
};