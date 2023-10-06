#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//Обработка смены поездной информации

class TC_TrackerInfoSetting : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerInfoSetting );
	CPPUNIT_TEST( PlaceChangings );
	CPPUNIT_TEST( NoTrainInfoSetting );
	CPPUNIT_TEST( ArrivedNoDepartured );
	CPPUNIT_TEST( ArrivedThenDepartured );
	CPPUNIT_TEST( InfoSettingBeforeArriving );
	CPPUNIT_TEST( FormedNoDepartured );
	CPPUNIT_TEST( FormedThenDepartured );
	CPPUNIT_TEST( FullyInSpan );
	CPPUNIT_TEST( HouseHold );
	CPPUNIT_TEST( ArrivalFactExpiration );
	CPPUNIT_TEST( ArrivalExpirationWhileLPFDelay );
	CPPUNIT_TEST( NotrainToTrain );
	CPPUNIT_TEST( DuplicateInfoMustBeErased );
	CPPUNIT_TEST( DuplicateInfoForDifferentUnities );
	CPPUNIT_TEST( DuplicateInfoForDifferentUnities2 );
	CPPUNIT_TEST( AddingIncompleteInfo );
	CPPUNIT_TEST( UndefOddnessAlsoApplyable );
	CPPUNIT_TEST( NearNoTrainInfoSettingMakesMoving );
	CPPUNIT_TEST( FormAfterDelayedForm );
	CPPUNIT_TEST( SetInfoOnTechnodeWithOddnessIgnore );
	CPPUNIT_TEST( SetInfoOnTechnodeWOOddnessIgnore );
	CPPUNIT_TEST( TransitionShouldCleanDelayedInfo );
	CPPUNIT_TEST( HittedExtDisformSuccess );
	CPPUNIT_TEST( HittedExtDisformReject );
	CPPUNIT_TEST( HittedExtDisformThruMoving );
	CPPUNIT_TEST( HittedExtDisformThruMoving2 );
	CPPUNIT_TEST( HittedExtDisformThruMoving3 );
	CPPUNIT_TEST( MissedExtDisformUnsuccess );
	CPPUNIT_TEST( MissedExtDisformExpired );
	CPPUNIT_TEST( MissedExtDisformAlienNumber );
	CPPUNIT_TEST( MissedExtDisformArrivedWithDisform );
	CPPUNIT_TEST( MissedExtDisformArrivedWODisform );
	CPPUNIT_TEST( OneMomentDisformAndForm );
	//CPPUNIT_TEST( NoNumberDisformForbidTransition );
	//CPPUNIT_TEST( NoNumberOverlappedDisformForbidTransition );
	CPPUNIT_TEST( HittedNoNumberDisform );
	CPPUNIT_TEST( MissedNoNumberDisform );
	CPPUNIT_TEST( DelayedInfoReplacement );
	CPPUNIT_TEST( TrainsMigrationAfterTrainInfoDuplication );
	CPPUNIT_TEST( TrainsMigrationAfterTrainInfoDuplication2 );
	CPPUNIT_TEST( BunchOfIdenticalNumbers );
	CPPUNIT_TEST_SUITE_END();

	void PlaceChangings(); //проверка на генерацию изменений мест, отправляемых на Fund

	//приложение информации для маневровой
	void NoTrainInfoSetting();
	//поезд прибыл на ПО-путь, но не отправился
	void ArrivedNoDepartured();
	//поезд прибыл на ПО-путь и отправился
	void ArrivedThenDepartured();
	//приложение информации до прибытия поезда
	void InfoSettingBeforeArriving();
	//поезд сформировался на ПО-пути, не прибывая, но не отправился
	void FormedNoDepartured();
	//поезд сформировался на ПО-пути и отправился
	void FormedThenDepartured();
	//ПЕ полностью на перегоне
	void FullyInSpan();
	//отдельный тест для хозяйственных поездов
	void HouseHold();
	//случай, когда поезд так и не успел прибыть на ПО-путь, поэтому инфопакет был выброшен
	void ArrivalFactExpiration();
	//задержка инфопакета под прибытие в условиях задержки входного сигнала
	void ArrivalExpirationWhileLPFDelay();
	//создание поезда из маневровой
	void NotrainToTrain();
	//дублирующий дексриптор не должен буферизироваться
	void DuplicateInfoMustBeErased();
	//накладывание одного и того же номера на разные ПЕ
	void DuplicateInfoForDifferentUnities();
	void DuplicateInfoForDifferentUnities2();
	//наложение не полного инфопакета не должно стирать уже существующую информацию
	void AddingIncompleteInfo();
	//неидентифицированный поезд с неизвестной четностью по умолчанию считается совпадающим по четности
	void UndefOddnessAlsoApplyable();
	//приложение информации на маневровую рядом с поездом, уже ее имеющим, приводит не пересозданию нового поезда, а к движению существующего
	void NearNoTrainInfoSettingMakesMoving();
	//случай пересоздания поезда, предварительно созданного по задержанному инфопакету
	void FormAfterDelayedForm();
	//установка номера на поезд на тех.узле с признаком игнорирования четности
	void SetInfoOnTechnodeWithOddnessIgnore();
	//установка номера на поезд на тех.узле без признака игнорирования четности
	void SetInfoOnTechnodeWOOddnessIgnore();
	//проследование поезда через путь, на котором оставлен инфопакет под прибытие должно удалять данный инфопакет из очереди ожидания безвозвратно
	void TransitionShouldCleanDelayedInfo();
	//одновременное расформирование и формирование нового поезда (от внешней системы)
	void OneMomentDisformAndForm();

	void HittedExtDisformSuccess(); //попадающий инфопакет расформирования от внешней системы (успешно)
	void HittedExtDisformReject(); //попадающий инфопакет расформирования от внешней системы (отказ)
	void HittedExtDisformThruMoving(); //попадающий инфопакет расформирования от внешний системы на поезд, который фактически ходом проследует ПО-путь
	void HittedExtDisformThruMoving2(); //проверка на дублирующее событие отправления
	void HittedExtDisformThruMoving3(); //случай длинного поезда
	void MissedExtDisformUnsuccess(); //промах инфопакета расформирования от внешней системы (неуспешное применение)
	void MissedExtDisformExpired(); //промах инфопакета расформирования от внешней системы (сообщение удаляется по исчерпанию времени)
	void MissedExtDisformAlienNumber(); //промах инфопакета расформирования от внешней системы (сообщение не применяется из-за расхождения номеров)
	void MissedExtDisformArrivedWithDisform(); //промах инфопакета расформирования от внешней системы (ожидание прибытия поезда с последующим расформированием)
	void MissedExtDisformArrivedWODisform(); //промах инфопакета расформирования от внешней системы (прибытие поезда без расформирования)
	void NoNumberDisformForbidTransition(); //инфопакет расформирования без номера запрещает проследование
	void NoNumberOverlappedDisformForbidTransition(); //инфопакет расформирования без номера запрещает проследование (случай наложения другого инфопакета)
	void HittedNoNumberDisform(); //инфопакет расформирования без номера попадает в поезд (применяется)
	void MissedNoNumberDisform(); //инфопакет расформирования без номера не попадает в поезда на ПЕ (не применяется)
	void DelayedInfoReplacement(); //в случае прихода нескольких инфопакетов на один и тот же путь до срабатывания первого из них следует сохранять только последний
	void TrainsMigrationAfterTrainInfoDuplication(); //при применении инфопакета с существующим номер/индексом аналогичный поезд при достаточной близости переносится на новое место
	void TrainsMigrationAfterTrainInfoDuplication2();
	void BunchOfIdenticalNumbers(); //одномоментное применение разных инфопакетов с идентичными номерами

	//вспомогательные функции
	bool CheckPermitted( unsigned int /*prevIden*/, const TrainDescr & /*newDescr*/ );
	bool CheckForbidden( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
	bool CheckRecreated( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
	bool CheckForbiddenThenPermittedAfterArrive( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
	bool CheckForbiddenThenRecreationAfterArrive( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
	bool CheckHouseHold( unsigned int /*prevIden*/, ConstTrainDescrPtr /*prevDescrPtr*/, const TrainDescr & /*newDescr*/ );
};