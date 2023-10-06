#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerTrainDisappearance : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainDisappearance );
	CPPUNIT_TEST( TwinklingDisappearance );
	CPPUNIT_TEST( RealDisappearance );
	CPPUNIT_TEST( HeterogeneousDisappearance );
	CPPUNIT_TEST( DisapperanceAndTeleport );
	CPPUNIT_TEST( DisappearanceVsSplitting );
	CPPUNIT_TEST( LengthyDisapperance );
	CPPUNIT_TEST( DisappearanceAndRecoverNear );
	CPPUNIT_TEST( DeadEndDisappearance );
	CPPUNIT_TEST( DisappearanceVsTeleportToTrain );
	CPPUNIT_TEST( DisappearanceVsTeleportToNoTrain );
	CPPUNIT_TEST_SUITE_END();

	void TwinklingDisappearance(); //нечувствительность отслеживания (в смысле фиксируемых изменений) к кратковременным миганиям участков
	void RealDisappearance(); //действительное исчезновение поезда
	void HeterogeneousDisappearance(); //разнородные исчезновения поездов
	void DisapperanceAndTeleport(); //скачок поезда (на отдаленное место) после его исчезновения
	void LengthyDisapperance(); //случай исчезновения ПЕ (на протяженности нескольких участков)
	void DisappearanceVsSplitting(); //исчезновение ПЕ, похожее на разбиение
	void DisappearanceAndRecoverNear(); //исчезновение поезда с двух участков с последующим восстановлением
	void DeadEndDisappearance(); //исчезновение перед тупиком
	void DisappearanceVsTeleportToTrain(); //при пропадании поезда рядом с находящимся рядом через участок другим поездом телепорт на место соседнего поезда запрещен
	void DisappearanceVsTeleportToNoTrain(); //при пропадании поезда рядом с находящейся рядом через участок маневровой телепорт на ее место разрешен
};