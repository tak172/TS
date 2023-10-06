#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка отслеживания ПЕ с учетом динамики движения (истории и временнЫх характеристик)

class TC_TrackerDynamics : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerDynamics );
	CPPUNIT_TEST( StationEntry );
	CPPUNIT_TEST( HistoryDepth );
	CPPUNIT_TEST( Declutching );
	CPPUNIT_TEST( OpenWayDeclutching );
	CPPUNIT_TEST( OpenWayDeclutchingWithDelayedSema );
	CPPUNIT_TEST( OpenWayExitAndDeclutchingCloseSema );
	CPPUNIT_TEST( CloseWayDeclutching );
	CPPUNIT_TEST( OpenShuntingDeclutching );
	CPPUNIT_TEST( WOSemaWayDeclutching );
	CPPUNIT_TEST( StationExitMissingArrdep );
	CPPUNIT_TEST( SpanDeclutching );
	CPPUNIT_TEST( SpanDeclutching2 );
	CPPUNIT_TEST( AnotherOddnessCoSemaTransit );
	CPPUNIT_TEST( AnotherOddnessCoSemaFastTransit );
	CPPUNIT_TEST( AnotherOddnessOppSemaTransit );
	CPPUNIT_TEST( ReversiveMoveAfterPartialArrive );
	CPPUNIT_TEST( LongTrainAtypicalStationExit );
	CPPUNIT_TEST( MovingPriority );
	CPPUNIT_TEST( DeclutchingAfterTurn );
	//CPPUNIT_TEST( InstantDoubleTrainsSpanMove );
	CPPUNIT_TEST( InstantMixedUnitiesTightMove );
	CPPUNIT_TEST( TrainJumpToNoTrain );
	CPPUNIT_TEST( TrainJumpToNoTrainOverBlockedPart );
	CPPUNIT_TEST( TrainJumpBidToNoTrainOverBlockedPart );
	//CPPUNIT_TEST( TrainJumpToNoTrainOverSwitchSection );
	//CPPUNIT_TEST( TrainJumpBidToNoTrainOverSwitchSection );
	CPPUNIT_TEST( SpanMotionVsArrdep );
	CPPUNIT_TEST( StatMotionVsPresence );
	CPPUNIT_TEST( StatMotionVsArrdep );
	//CPPUNIT_TEST( HeadMovingsOnSpanOnlyForward );
	CPPUNIT_TEST( SpanMotionVsKeepingLastPlace );
	CPPUNIT_TEST( ChoosingTrainByMoveDirection );
	CPPUNIT_TEST( TwoSideTrainObstacles );
	CPPUNIT_TEST( FormAndDepartureFromObscurity );
	CPPUNIT_TEST( TailBusyOnSpan );
	CPPUNIT_TEST( TailBusyOnSpan2 );
	//CPPUNIT_TEST( TailBusyOnSpan3 );
	CPPUNIT_TEST( GoOutFromStationAfterRecover );
	CPPUNIT_TEST_SUITE_END();

	void StationEntry(); //въезд на станцию
	void HistoryDepth(); //проверка на ведение истории движения ПЕ достаточной глубины
	void Declutching(); //отцепка маневровой ПЕ
	void OpenWayDeclutching(); //разделение поезда после отъезда с п/о пути на открытый светофор
	void OpenWayDeclutchingWithDelayedSema(); //разделение поезда после отъезда с п/о пути на открытый светофор с задержкой сигнала на его последующее закрытие
	void OpenWayExitAndDeclutchingCloseSema(); //выход поезда с ПО пути на открытый светофор, закрытие светофора и отделение поезда
	void CloseWayDeclutching(); //разделение поезда после отъезда с п/о пути на закрытый светофор
	void OpenShuntingDeclutching(); //отъезд локомотива от поезда на ПО пути на открытый маневровый светофор
	void WOSemaWayDeclutching(); //разделение поезда после отъезда с п/о пути при отсутствии светофора
	void StationExitMissingArrdep(); //выезд со станции (без занятия п/о путей)
	void SpanDeclutching(); //выезд поезда на перегон и последующий возврат локомотива на станцию
	void SpanDeclutching2();
	void AnotherOddnessCoSemaTransit(); //проезд поезда через попутный светофор не подходящей четности
	void AnotherOddnessCoSemaFastTransit(); //быстрый проезд (с занятием сразу двух участков) через попутный светофор не подходящей четности
	void AnotherOddnessOppSemaTransit(); //проезд поезда через встречный светофор не подходящей четности
	void ReversiveMoveAfterPartialArrive(); //обратное движение после частичного занятия п/о пути
	void LongTrainAtypicalStationExit(); //продолжительный выход длинного поезда с п/о пути на перегон (с оставлением занятости на п/о пути)
	void MovingPriority(); //приоритет размещения поезда по критерию движения
	void DeclutchingAfterTurn(); //разделение поезда после его разворота (при нахождении на п/о пути и наличии закрытого светофора)
	void InstantDoubleTrainsSpanMove(); //запрет проезда через один участок перегона двух поездов в один момент времени
	void InstantMixedUnitiesTightMove(); //проезд через один участок поезда и не поезда в один момент времени (поезд надо сохранить). остается один занятый участок
	void TrainJumpToNoTrain(); //прыжок поезда через участок к маневровой (маневровая не сохраняется) 
	void TrainJumpToNoTrainOverBlockedPart(); //прыжок поезда через участок к маневровой (маневровая не сохраняется) через блок-участок
	void TrainJumpBidToNoTrainOverBlockedPart(); //запрет прыжка поезда через участок к маневровой через блок-участок
	void TrainJumpToNoTrainOverSwitchSection(); //прыжок поезда через участок к маневровой через стрелочную секцию
	void TrainJumpBidToNoTrainOverSwitchSection(); //запрте прыжка поезда через участок к маневровой через стрелочную секцию
	void SpanMotionVsArrdep(); //выбор места для поезда с точки зрения факторов движения на перегон и присутствия на ПО-путях (приоритет - движению)
	void StatMotionVsPresence(); //выбор места для поезда с точки зрения факторов движения по станции и текущего присутствия (приоритет - движению)
	void StatMotionVsArrdep(); //выбор места для поезда с точки зрения факторов движения по станции и присутствия на ПО-путях (приоритет - ПО-путям)
	void HeadMovingsOnSpanOnlyForward(); //голова поезда при движении по перегону может двигаться только вперед
	void SpanMotionVsKeepingLastPlace(); //выбор места для поезда с точки зрения факторов движения по перегону и наезда на место, единственное для другого поезда (приоритет - сохранение обоих поездов)
	void ChoosingTrainByMoveDirection(); //корректный выбор поезда для занятия с точки зрения направления движения
	void TwoSideTrainObstacles(); //двустороннее ограничение для поезда (чужой светофор и встречное движение перегонного поезда)
	void FormAndDepartureFromObscurity(); //формирование и отправление поезда из неконтролируемой зоны и выход на перегон

	void TailBusyOnSpan(); //занятость на перегоне в хвосте становится маневровой (к проблеме некорректных возвратов поездов на перегон)
	void TailBusyOnSpan2(); //если занятость в хвосте стала поездом, она не может потеснить поезд на станции (к проблеме некорректных возвратов поездов на перегон)
	void TailBusyOnSpan3(); //случай занятости в области хвоста, отстоящей на некоторое расстояние (к проблеме некорректных возвратов поездов на перегон)
	void GoOutFromStationAfterRecover(); //выход на перегон после восстановления
};