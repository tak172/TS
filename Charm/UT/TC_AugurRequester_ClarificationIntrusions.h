#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

class FutureLayer;
class RouteIntrusionKeeper;

// Тестирование корректировки интрузий после расчета прогноза
class TC_AdjustIntrusions : public CPPUNIT_NS::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TC_AdjustIntrusions );
    CPPUNIT_TEST( SelfVisit );
    CPPUNIT_TEST( OtherVisit );
    CPPUNIT_TEST( SameTrainNumber );
	CPPUNIT_TEST_SUITE_END();

    void SelfVisit();    // проверка на собственное посещение (как без фиксации так и с фиксацией)
    void OtherVisit();    // проверка на посещение другим поездом
    void SameTrainNumber(); // обработка поездов с одним номером и разными индексами

    void setUp() override;
    void tearDown() override;

private:
    std::shared_ptr< UtLayer<FutureLayer> > future_layer;
    std::shared_ptr<RouteIntrusionKeeper>   rIntrusionKeeper;
    time_t liveMoment;

    // установка текущего времени
    void setLiveMoment( std::string _currentMoment );
    // генератор заголовка события
    std::wstring HEAD( unsigned HHMMss ) const;
};
