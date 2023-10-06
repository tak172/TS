#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Проверка генерации событий стоянки на перегоне

class TC_TrackerSpanStopping : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerSpanStopping );
	CPPUNIT_TEST( CasualCaseWithLengthWithPicketage );
	CPPUNIT_TEST( CasualCaseWithLengthWOPicketage );
	CPPUNIT_TEST( CasualCaseWOLengthWithPicketage );
	CPPUNIT_TEST( CasualCaseWOLengthWOPicketage );
	CPPUNIT_TEST( SpanStopAfterReborn );
	CPPUNIT_TEST( SpanStopAfterReborn2 );
	CPPUNIT_TEST( SpanStopAfterWideReborn );
	CPPUNIT_TEST( NonSpanStopAfterWideReborn );
	CPPUNIT_TEST( NonSpanStopAfterRebornNear );
	CPPUNIT_TEST( SpanStopAfterRebornAndWaiting );
	CPPUNIT_TEST( NonSpanStopAfterRelight );
	CPPUNIT_TEST( NonSpanStopAfterRelightAndWaiting );
	CPPUNIT_TEST( ParallelActivity );
	CPPUNIT_TEST( SpanStoppingAndPocketEntry );
	CPPUNIT_TEST( SpanStoppingIntoPocket );
	CPPUNIT_TEST( SeveralStops );
	CPPUNIT_TEST( StopBeforeStation );
	CPPUNIT_TEST( SpanStoppingEarlierThanEntraStopping );
	CPPUNIT_TEST( EntraStoppingEarlierThanSpanStopping );
	CPPUNIT_TEST( EntraStoppingEarlierThanSpanStopping2 );
	CPPUNIT_TEST( NoTrainSpanStopping );
	CPPUNIT_TEST_SUITE_END();

	void CasualCaseWithLengthWithPicketage(); //стоянка на перегонном участке с указанной длиной и пикетажами стыков
	void CasualCaseWithLengthWOPicketage(); //стоянка на перегонном участке с указанной длиной, но без пикетажей стыков
	void CasualCaseWOLengthWithPicketage(); //стоянка на перегонном участке с неуказанной длиной, но с пикетажами стыков
	void CasualCaseWOLengthWOPicketage(); //стоянка на перегонном участке с неуказанной длиной и не без пикетажей стыков
	void SpanStopAfterReborn();	//стоянка на перегоне по моменту пересоздания поезда в том же месте после пропадания
	void SpanStopAfterReborn2();
	void SpanStopAfterWideReborn(); //стоянка при исчезновении на одном и восстановлении на нескольких (на двух) участках
	void NonSpanStopAfterWideReborn(); //нет стоянки при исчезновении на одном и восстановлении на нескольких (на двух) участках
	void NonSpanStopAfterRebornNear(); //нет стоянки на перегоне по моменту пересоздания поезда на соседенем месте после пропадания
	void SpanStopAfterRebornAndWaiting(); //стоянка на перегоне по истечению небольшого времения после пересоздания
	void NonSpanStopAfterRelight(); //отсутствие стоянки на перегоне после перезагорания участка из-за невосстановления поезда, а появления нового
	void NonSpanStopAfterRelightAndWaiting(); //отсутствие стоянки на перегоне после перезагорания участка и ожидания из-за невосстановления поезда, а появления нового
	void ParallelActivity(); //параллельная активность на разных перегонах
	void SpanStoppingAndPocketEntry(); //стоянка на перегоне и уход в карман
	void SpanStoppingIntoPocket(); //уход в карман и стоянка на перегоне внутри кармана
	void SeveralStops(); //несколько стоянок на перегоне
	void StopBeforeStation(); //стоянка на перегонном участке перед станцией при движении в сторону станции
	void SpanStoppingEarlierThanEntraStopping(); //наличие стоянки на перегоне отменяет генерацию стоянки перед входным
	void EntraStoppingEarlierThanSpanStopping(); //наличие стоянки перед входным отменяет генерацию стоянки на перегоне
	void EntraStoppingEarlierThanSpanStopping2();
	void NoTrainSpanStopping(); //случай стоянки на перегоне маневровой (еще не поезд). события быть не должно
};