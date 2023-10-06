#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//вопросы временнџх задержек и синхронизации

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerTimeSynchro : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();
	virtual void tearDown();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTimeSynchro );
	CPPUNIT_TEST( BoundaryStripBlinking );
	CPPUNIT_TEST( TrainIdentification );
	CPPUNIT_TEST( DislokTimeoutBusy );
	CPPUNIT_TEST( DislokBusyTimeout );
	CPPUNIT_TEST( DislokBusy );
	CPPUNIT_TEST( BusyDislok );
	CPPUNIT_TEST( BusyDislok2 );
	CPPUNIT_TEST( BusyNoextDislok );
	CPPUNIT_TEST( BusyDislokFreeBusyTimeout );
	CPPUNIT_TEST( ExtDislokBusy );
    CPPUNIT_TEST( TestRepeatValue );
	CPPUNIT_TEST( HemDisformDelay );

	CPPUNIT_TEST( GlobalDelayVersusParkWayDelay );
	CPPUNIT_TEST( GlobalDelayVersusOnlyWayDelay );
	CPPUNIT_TEST( GlobalDelayVersusOnlyParkDelay );
	CPPUNIT_TEST( GlobalDelayVersusStationDelay );
	CPPUNIT_TEST( ParkWayDelayVersusOnlyParkDelay );
	CPPUNIT_TEST( ParkWayDelayVersusOnlyWayDelay );

	CPPUNIT_TEST_SUITE_END();

	void BoundaryStripBlinking(); //дребезг участка на границе станции и перегона
	void TrainIdentification(); //идентификаци€ поезда с учетом задержки после фильтрации сигналов

	//нижеуказанные тесты провер€ет различный пор€док комбинаций получени€ событий: прихода идентификации (Dislok),
	//окончание хранени€ идентификации по истечению времени (Timeout), прихода зан€тости (Busy) и прихода свободности (Free)
	void DislokTimeoutBusy();
	void DislokBusyTimeout();
	void DislokBusy();
	void BusyDislok();
	void BusyDislok2();
	void BusyNoextDislok();
	void BusyDislokFreeBusyTimeout();
	void ExtDislokBusy();
	void TestRepeatValue();     // фильтраци€ повторных данных не должна задерживать остальные
	void HemDisformDelay(); //задержка расформировани€ на период фильтрации
	void GlobalDelayVersusParkWayDelay(); //выбор между глобальной задержкой и задержкой на определенный путь и парк
	void GlobalDelayVersusOnlyWayDelay(); //выбор между глобальной задержкой и задержкой на определенный путь (без указани€ парка)
	void GlobalDelayVersusOnlyParkDelay(); //выбор между глобальной задержкой и задержкой на определенный парк (без указани€ пути)
	void GlobalDelayVersusStationDelay(); //выбор между глобальной задержкой и задержкой на определенной станции
	void ParkWayDelayVersusOnlyParkDelay(); //выбор между задержкой на определенный путь и парк и задержкой на определенный парк (без указани€ пути)
	void ParkWayDelayVersusOnlyWayDelay(); //выбор между задержкой на определенный путь и парк и задержкой на определенный путь (без указани€ парка)

	bool IdentifiedAsTrain( const BadgeE &, const TrainDescr & ) const;
};