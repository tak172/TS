#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка генерации событий при проезде множественных п/о путей

class TC_TrackerADMultiways : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerADMultiways );
	CPPUNIT_TEST( ArriveTransit );
	CPPUNIT_TEST( TransitTransit );
	CPPUNIT_TEST( TransitArrive );
	CPPUNIT_TEST( PassArrive );
	CPPUNIT_TEST( PassTransit );
	CPPUNIT_TEST( ArrivePass );
	CPPUNIT_TEST( TransitPass );
	CPPUNIT_TEST( ReversiveTransitTransit );
	CPPUNIT_TEST( ReversiveDepartTransit );
	CPPUNIT_TEST( MultiArrdepEventsOnUnstableSignal );
	CPPUNIT_TEST( MultiWayBornAndGoSpan );
	CPPUNIT_TEST( UpgradeOnTwoADWaysAndLeave );
	CPPUNIT_TEST( ComplexWayBornAndGoOut );
	CPPUNIT_TEST( OutFromADWayWithMidsema );
	CPPUNIT_TEST( GoingThroughADWayWithMidsema1 );
	CPPUNIT_TEST( GoingThroughADWayWithMidsema2 );
	CPPUNIT_TEST( ExcessiveDeparture );
	CPPUNIT_TEST_SUITE_END();

	void ArriveTransit(); //прибытие на первый путь. проследование следующего
	void TransitTransit(); //проследование обоих путей
	void ReversiveTransitTransit(); //проследование обоих путей (в обратном направлении)
	void ReversiveDepartTransit(); //отправление и проследование (в обратном направлении)
	void TransitArrive(); //проследование первого пути. прибытие на следующий
	void PassArrive(); //проезд на закрытый светофор первого пути. прибытие на следующий
	void PassTransit(); //проезд на закрытый светофор первого пути. проследование следующего
	void ArrivePass(); //прибытие на первый путь. проезд на закрытый светофор второго пути
	void TransitPass(); //проследование первого пути. проезд на закрытый светофор второго пути
	void MultiArrdepEventsOnUnstableSignal(); //множественные события прибытия/отправления на нестабильном (дрожащем) сигнале
	void MultiWayBornAndGoSpan(); //возникновение на составных п/о путях и выход на перегон
	void UpgradeOnTwoADWaysAndLeave(); //создание поезда из ПЕ, расположенной на двух п/о путях, и выезд с одного из п/о путей
	void ComplexWayBornAndGoOut(); //создание поезда на составном п/о пути и выезд с него
	void OutFromADWayWithMidsema(); //выход с составного п/о пути со светофором посередение
	void GoingThroughADWayWithMidsema1(); //проследование составного п/о пути со светофором посередение (вариант 1)
	void GoingThroughADWayWithMidsema2(); //проследование составного п/о пути со светофором посередение (вариант 2)
	void ExcessiveDeparture(); //избыточное отправление на плохой работе ЭЦ

	typedef TrackerInfrastructure::TIActivity TIActivity;
	//проезд через два пути (в правильной конфигурации)
	std::vector <TIActivity> goodWaysPassingActs;
	void MakeActivities();
	void SetPreMoveActivity( TIActivity preMoveActivity ) { goodWaysPassingActs[0] = preMoveActivity; }
	void SetSemaActivities( TIActivity firstSemaActivity, TIActivity secondSemaActivity )
	{
		goodWaysPassingActs[2] = firstSemaActivity;
		goodWaysPassingActs[4] = secondSemaActivity;
	}
};