#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerChangings : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();
	virtual void tearDown();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerChangings );
	CPPUNIT_TEST( StandardPassing );
	CPPUNIT_TEST( ElementaryMove );
	CPPUNIT_TEST( OneEndBlinking );
	CPPUNIT_TEST( UTurn );
	CPPUNIT_TEST( LengthyLiberationWithFiltering );
	CPPUNIT_TEST( UnknownUnityADWayExitNGoSpan );
	CPPUNIT_TEST( UnknownUnityADWayExitNGoThroughSema );
	CPPUNIT_TEST( HeadMotionsOnSetTrainInfo );
	CPPUNIT_TEST( HeadMotionsOnUnsetTrainInfo );
	CPPUNIT_TEST( HeadMotionsOnChangeInfoForUnmovingTrain );
	CPPUNIT_TEST( DisappearWithRecovery );
	CPPUNIT_TEST( DisappearWithoutRecovery );
	CPPUNIT_TEST( ForceDisappearAfterRouteSet );
	CPPUNIT_TEST( InfoExtensionNoCausesExternalTrainChanges );
	CPPUNIT_TEST_SUITE_END();

	void StandardPassing(); //стандартный проезд поезда через п/о путь станции
	std::wstring StandardPassingInput(); //возврат - номер поезда
	void StandardPassingInputCheck( std::wstring trainNum );
	void StandardPassingOutput();
	void StandardPassingOutputCheck( std::wstring trainNum );
	void ElementaryMove(); //появление поезда и продвижение на один участок
	void OneEndBlinking(); //мигание участка на одном конце поезда
	void UTurn(); //разворот поезда
	void LengthyLiberationWithFiltering(); //освобождение нескольких участков (при наличии режима фильтрации)
	void UnknownUnityADWayExitNGoSpan(); //выезд с п/о пути на запрещащий сигнал поездного светофора и выезд на перегон
	void UnknownUnityADWayExitNGoThroughSema(); //выезд с п/о пути на запрещащий сигнал поездного светофора и последующий проезд через открытый поездной светофор
	void HeadMotionsOnSetTrainInfo(); //уведомления об изменениях головы при установке иноформации
	void HeadMotionsOnUnsetTrainInfo(); //уведомления об изменениях головы при сбросе иноформации
	void HeadMotionsOnChangeInfoForUnmovingTrain(); //уведомления об изменениях головы при смене иноформации (поезд не двигается)
	void DisappearWithRecovery(); //исчезновение поезда с возможным восстановлением
	void DisappearWithoutRecovery(); //исчезновение поезда без возможного восстановления
	void ForceDisappearAfterRouteSet(); //уведомление об исчезновении после задания маршрута поверх сохраненного поезда
	void InfoExtensionNoCausesExternalTrainChanges(); //изменение информации не должно приводить к дополнительным уведомлениям во внешние системы
};