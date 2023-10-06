#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

// проверка класса HappenLayer
class TC_HappenLayer : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_HappenLayer );
    CPPUNIT_TEST( AutoCreate );
    CPPUNIT_TEST( UserCreate );
    CPPUNIT_TEST( AutoAppend );
    CPPUNIT_TEST( UserAppend );
    CPPUNIT_TEST( AutoChange );
    CPPUNIT_TEST( UserChange );
    CPPUNIT_TEST( AutoDelete );
    CPPUNIT_TEST( UserDelete );
    CPPUNIT_TEST( AutoTimeError );
    CPPUNIT_TEST( UserTimeError );
    CPPUNIT_TEST( AutoEventCollision );
    CPPUNIT_TEST( UserEventCollision );
    CPPUNIT_TEST( UserBadPreposition );
    CPPUNIT_TEST( AutoSerialization );
    CPPUNIT_TEST( UserSerialization );
    CPPUNIT_TEST( ConflictCleaning );
    CPPUNIT_TEST_SUITE_END();

private:
    void AutoCreate();         // Hem::PusherAutoHappen создание нити
    void UserCreate();         // Hem::PusherUserHappen создание нити
    void AutoAppend();         // Hem::PusherAutoHappen дополнение нити
    void UserAppend();         // Hem::PusherUserHappen дополнение нити
    void AutoChange();         // Hem::PusherAutoHappen изменение содержимого
    void UserChange();         // Hem::PusherUserHappen изменение содержимого
    void AutoDelete();         // Hem::PusherAutoHappen удаление нити
    void UserDelete();         // Hem::PusherUserHappen удаление нити
    void AutoTimeError();      // Hem::PusherAutoHappen ошибка времени
    void UserTimeError();      // Hem::PusherUserHappen ошибка времени
    void AutoEventCollision(); // Hem::PusherAutoHappen совпадение событий
    void UserEventCollision(); // Hem::PusherUserHappen совпадение событий
    void UserBadPreposition(); // Hem::PusherUserHappen при неверной препозиции начинать новую нить
    void AutoSerialization(); 
    void UserSerialization();
    void ConflictCleaning(); // Hem::PusherAutoHappen чистка конфликтов (см. #3723)

    template< class Pusher > void AnyCreate();
    template< class Pusher > void AnyAppend();
    template< class Pusher > void AnyChange();
    template< class Pusher > void AnyDelete();
    template< class Pusher > void AnySerialize();

    static const time_t T=3600*(240+23);
    UtIdentifyCategoryList identifyCategory;
};
