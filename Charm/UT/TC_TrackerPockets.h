#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"
#include "../Guess/TrackerEvent.h"
#include "../Guess/TrainUnity.h"

//ѕроверка продвижени€ ѕ≈ по "карманам" перегона
class TC_TrackerPockets : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerPockets );
	CPPUNIT_TEST( BusyInsensibility ); //"карманные" участки не чувствительны к сигналам зан€тости
	CPPUNIT_TEST( FreeInsensibility ); //"карманные" участки не чувствительны к сигналам свободности
	CPPUNIT_TEST( TrainKeeping ); //проведение ѕ≈ по перегону с двум€ "карманами"
	CPPUNIT_TEST( MultiTrains ); //движение через "карманы" нескольких ѕ≈
	CPPUNIT_TEST( ResetInPockets ); //сброс состо€ни€ (в т.ч. удаление всех ѕ≈) в момент нахождени€ ѕ≈ в "кармане"
	CPPUNIT_TEST( LongTrainMoveToPocket ); //заезд в "карман" ѕ≈, занимающего несколько участков
	CPPUNIT_TEST( LongTrainMoveOutPocket ); //выезд из "кармана" ѕ≈, сразу на несколько участков
	CPPUNIT_TEST( SetInfo ); //назначение информации поезду, наход€щемус€ в "кармане"
	CPPUNIT_TEST( BidirectOut ); //"выезд" из кармана сразу в обоих направлени€х
	CPPUNIT_TEST( BecomeTrainOnEntry ); //формирование поезда перед въездом в "карман"
	CPPUNIT_TEST( DeleteTrain ); //удаление поезда, наход€щегос€ в "кармане"
	CPPUNIT_TEST( HeterogeneousMoving ); //карманные и некарманные движени€ одновременно (технический тест)
	CPPUNIT_TEST( DeleteAmongMany ); //удаление из середины скоплени€ поездов в "кармане"

	//тест прикрыт из-за полного сброса кармана после его переполнени€ (функциональность требует уточнени€)
	//CPPUNIT_TEST( MultiTrainsPositioning ); //расположение нескольких ѕ≈ в "кармане"

	CPPUNIT_TEST( LongPocketMoving ); //проход через длинный "карман". эффект вли€ни€ пор€дка возвращаемых смежных ребер
    CPPUNIT_TEST( JointPicketage ); //передача пикетажа только дл€ граничного стыка
	CPPUNIT_TEST( FalsePocket ); //игнорирование псевдо-кармана
	CPPUNIT_TEST( ForbidPocketJumping ); //запрет на "перепрыгивание" через карман
	CPPUNIT_TEST( OverflowShort ); //переполнение кармана из одного участка
	CPPUNIT_TEST( OverflowLong ); //переполнение длинного кармана
	CPPUNIT_TEST( PocketEntryVsDisform ); //выбор между уходом в карман и расформированием
	CPPUNIT_TEST( JumpOutOfPocket ); //"выпрыгивание" из кармана через участок
	CPPUNIT_TEST( JumpOutVsExit ); //выбор между "выпрыгиванием" из кармана через участок и обычным выездом
	CPPUNIT_TEST( PocketExitAsSoonAsPossible ); //выход из кармана по возможности побыстрее
	CPPUNIT_TEST_SUITE_END();

	void BusyInsensibility();
	void FreeInsensibility();
	void TrainKeeping();
	void MultiTrains();
	void VerifyHemPath( const std::list <TrackerEventPtr> & );
	TrainUnityCPtr MoveThrow( std::wstring apprstrip1, std::wstring apprstrip2 ); //по€вление ѕ≈ на 1 уч-ке, сдвиг на 2 уч-к и пропадание
	TrainUnityCPtr MoveThrow( std::wstring apprstrip1, std::wstring apprstrip2, std::wstring apprstrip3 ); //аналогично через три участка
	void ResetInPockets();
	void LongTrainMoveToPocket();
	void LongTrainMoveOutPocket();
	void SetInfo();
	void BidirectOut();
	void BecomeTrainOnEntry();
	void DeleteTrain();
	void HeterogeneousMoving();
	void DeleteAmongMany();
	void MultiTrainsPositioning();
	void LongPocketMoving();
    void JointPicketage();
	void FalsePocket();
	void ForbidPocketJumping();
	void OverflowShort();
	void OverflowLong();
	void PocketEntryVsDisform();
	void JumpOutOfPocket();
	void JumpOutVsExit();
	void PocketExitAsSoonAsPossible();
};