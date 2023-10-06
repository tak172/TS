#pragma once

//проверка механизма восстановления поездов

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerRecovery : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerRecovery );
	CPPUNIT_TEST( DiscordantMoveRecovery ); //несогласованное смещение поезда с участка на участок (когда следующий участок загорается после гашения всего). случай восстановления
	CPPUNIT_TEST( DiscordantJumpRecovery ); //скачок поезда после гашения исходного участка (гашение исходного, загорание через один и загорание соседнего)
	CPPUNIT_TEST( BoundDissapearanceUnrecovery ); //запрет восстановления при исчезновении поезда на граничном участке
	CPPUNIT_TEST( RecoveryCleanAfterSetInfo ); //очистка информации к восстановлению поезда при принудительной установке такой же информации на действующий поезд
	CPPUNIT_TEST( SamePlaceRecovery ); //восстановление при гашении и зажигании того же участка
	CPPUNIT_TEST( StripJump ); //переход поезда через участок
	CPPUNIT_TEST( RecoveryForbidness ); //запрет на восстановление для определенных участков
	CPPUNIT_TEST( RecoveringInsteadMoving ); //восстановление поезда вместо движения ближайшего
	CPPUNIT_TEST( RecoveringInsteadMoving2 );
	CPPUNIT_TEST( RecoveringInsteadMoving3 );
	CPPUNIT_TEST( RecoveringAndCountraryMovingOnSingleStrip ); //восстановление и встречное движение на единственном занятом участке
	CPPUNIT_TEST( SameTrainFarPlacesRecovering ); //восстановление поезда на группе не смежных удаленных участков
	CPPUNIT_TEST( RecoveryOnThreeStrips ); //восстановление одновременно на трех участках
	CPPUNIT_TEST( RecoveryOverBlockedStrips ); //восстановление через составной блок-участок
	CPPUNIT_TEST( InterferringOfEmptyInfoRecovering ); //случай параллельного восстановления поездов без информации на несвязанных перегонах
	CPPUNIT_TEST( RecoveryThroughLive ); //недопустимость восстановления поезда с перескоком через живой из-за нарушения их порядка следования
	CPPUNIT_TEST( TopologicallyUnreachableRecover ); //топологически невозможное восстановление через стрелку
	CPPUNIT_TEST( RecoveringOnSinglePlaceUnity ); //восстановление поверх ПЕ, занимающего единственное место
	CPPUNIT_TEST( RecoveryVersusOccupation ); //восстановление поезда и потенциальное занятие тех же участков другой ПЕ
	CPPUNIT_TEST( RecoveryVersusOccupation2 );
	CPPUNIT_TEST( PermittedRecoveryOverSwitch ); //возможное восстановление поезда на участке через стрелку
	CPPUNIT_TEST( ForbiddenRecoveryOverSwitch ); //запрет восстановления поезда на участке через стрелку
	CPPUNIT_TEST( DeathVersusDisform ); //исчезновение поезда вместо расформирования и последующее его восстановление
	CPPUNIT_TEST( MultiRecovery ); //множественное восстановление
	CPPUNIT_TEST( MultiRecoveryWithinOneChain ); //множественное восстановление внтури одной ЦЗ
	CPPUNIT_TEST( MultiRecoveryWithinOneChain2 );
	CPPUNIT_TEST( MultiRecoveryWithinOneChain3 );
	CPPUNIT_TEST( MultiRecoveryWithinOneChain4 );
	CPPUNIT_TEST( RecoveryWithTaking ); //восстановлние с отъемом у ближайшего поезда одного из участков
	CPPUNIT_TEST( RecoveryWithTaking2 );
	CPPUNIT_TEST( RecoveryWithTaking3 );
	CPPUNIT_TEST( ForbidMultiPlacesRecovery ); //восстановление на нескольких несмежных местах запрещено
	CPPUNIT_TEST( HeadPositioningAfterRecovery ); //голова поезда должна по возможности находиться ближе к месту восстановления и не занимать бывших в истории поезда мест
	CPPUNIT_TEST( OddnessRecoveryForNoident ); //восстановление четности неидентифицированного поезда
	CPPUNIT_TEST( DeathPlaceShouldBeRecordedInHistory ); //при смерти поезда его место должно быть отмечено в истории
	CPPUNIT_TEST( AppearanceVsRecovery ); //приоритет восстановления перед созданием
	CPPUNIT_TEST( NoTrainRecovery ); //восстановление маневровой
	CPPUNIT_TEST( NoTrainRecovery2 );
	CPPUNIT_TEST( NoTrainRecoveryNearADWay ); //восстановление маневровой около ПО-пути
	CPPUNIT_TEST( NoRememberNoTrainOnSpan ); //при хотя бы частичном нахождении маневровой на перегоне - она не восстанавливается (для исключения последующих паразитных восстановлений)
	CPPUNIT_TEST( NoRememberSuddenSpanActivity ); //внезапная активность на перегоне не приводит к запоминанию после удаления поезда
	CPPUNIT_TEST( ForbidRecoveryAfterRouteSet ); //удаление информации о восстановлении после задания маршрута
	CPPUNIT_TEST( FullStationExitAfterRecoveryOnADWay ); //покидание станции после восстановления на ПО-пути
	CPPUNIT_TEST_SUITE_END();

	void DiscordantMoveRecovery();
	void DiscordantJumpRecovery();
	void BoundDissapearanceUnrecovery();
	void RecoveryCleanAfterSetInfo();
	void SamePlaceRecovery();
	void StripJump();
	void RecoveryForbidness();
	void RecoveringInsteadMoving();
	void RecoveringInsteadMoving2();
	void RecoveringInsteadMoving3();
	void RecoveringAndCountraryMovingOnSingleStrip();
	void SameTrainFarPlacesRecovering();
	void RecoveryOnThreeStrips();
	void RecoveryOverBlockedStrips();
	void InterferringOfEmptyInfoRecovering();
	void RecoveryThroughLive();
	void TopologicallyUnreachableRecover();
	void RecoveringOnSinglePlaceUnity();
	void RecoveryVersusOccupation();
	void RecoveryVersusOccupation2();
	void PermittedRecoveryOverSwitch();
	void ForbiddenRecoveryOverSwitch();
	void DeathVersusDisform();
	void MultiRecovery();
	void MultiRecoveryWithinOneChain();
	void MultiRecoveryWithinOneChain2();
	void MultiRecoveryWithinOneChain3();
	void MultiRecoveryWithinOneChain4();
	void RecoveryWithTaking();
	void RecoveryWithTaking2();
	void RecoveryWithTaking3();
	void ForbidMultiPlacesRecovery();
	void HeadPositioningAfterRecovery();
	void OddnessRecoveryForNoident();
	void DeathPlaceShouldBeRecordedInHistory();
	void AppearanceVsRecovery();
	void NoTrainRecovery();
	void NoTrainRecovery2();
	void NoTrainRecoveryNearADWay();
	void NoRememberNoTrainOnSpan();
	void NoRememberSuddenSpanActivity();
	void ForbidRecoveryAfterRouteSet();
	void FullStationExitAfterRecoveryOnADWay();
};