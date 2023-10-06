#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"
#include "../Hem/UncontrolledStations.h"

class HappenLayer;
class FutureLayer;

// Тестирование сбора рассчитываемых нитей для Augur
class TC_InputRoutesComposer : public CPPUNIT_NS::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TC_InputRoutesComposer );
    CPPUNIT_TEST( HappenLive );
    CPPUNIT_TEST( HappenDeath );
    CPPUNIT_TEST( FutureSimple );
    CPPUNIT_TEST( FutureWithHappen );
	CPPUNIT_TEST_SUITE_END();

    void HappenLive();   // сбор отслеживаемых нитей исполненного слоя
    void HappenDeath();  // сбор мертвых нитей исполненного слоя
    void FutureSimple(); // сбор только из прогнозного слоя
    void FutureWithHappen(); // сбор из прогнозного И исполненного слоев

    void setUp() override;
    void tearDown() override;

private:
    std::shared_ptr< UtLayer<HappenLayer> > happen_layer;
    std::shared_ptr< UtLayer<FutureLayer> > future_layer;
    HappenLayer::PathSignMap pathForFutureMap;
    UncontrolledStations uncontrolledStations;
    time_t liveMoment;
  
    // установка текущего времени
    void setLiveMoment( std::string _currentMoment );
    // генератор атрибута времени события
    std::wstring HMS( unsigned val ) const;
    // получить нитку по номеру поезда
    HappenLayer::PathPtr              getHappenPath( std::wstring _numTrain ) const;
    std::shared_ptr<const Hem::FixablePath> getFuturePath( std::wstring _numTrain ) const;
    // запомнить связь прогнозной нити с реальной
    void attachFutu2Happen( std::wstring trainNum );

    // обработка результата
    typedef std::pair< TrainDescr, std::vector<FixableEvent> > ReqItem;
    typedef std::vector< ReqItem > ReqLines;
    // построение результата для проверки
    ReqLines processing();
    // поиск указанного дескриптора среди результата или nullptr
    static const ReqItem* findItem( const ReqLines& reqlines, const TrainDescr& td );
};
