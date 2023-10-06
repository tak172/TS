#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

// �������� ������ HappenLayer
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
    void AutoCreate();         // Hem::PusherAutoHappen �������� ����
    void UserCreate();         // Hem::PusherUserHappen �������� ����
    void AutoAppend();         // Hem::PusherAutoHappen ���������� ����
    void UserAppend();         // Hem::PusherUserHappen ���������� ����
    void AutoChange();         // Hem::PusherAutoHappen ��������� �����������
    void UserChange();         // Hem::PusherUserHappen ��������� �����������
    void AutoDelete();         // Hem::PusherAutoHappen �������� ����
    void UserDelete();         // Hem::PusherUserHappen �������� ����
    void AutoTimeError();      // Hem::PusherAutoHappen ������ �������
    void UserTimeError();      // Hem::PusherUserHappen ������ �������
    void AutoEventCollision(); // Hem::PusherAutoHappen ���������� �������
    void UserEventCollision(); // Hem::PusherUserHappen ���������� �������
    void UserBadPreposition(); // Hem::PusherUserHappen ��� �������� ���������� �������� ����� ����
    void AutoSerialization(); 
    void UserSerialization();
    void ConflictCleaning(); // Hem::PusherAutoHappen ������ ���������� (��. #3723)

    template< class Pusher > void AnyCreate();
    template< class Pusher > void AnyAppend();
    template< class Pusher > void AnyChange();
    template< class Pusher > void AnyDelete();
    template< class Pusher > void AnySerialize();

    static const time_t T=3600*(240+23);
    UtIdentifyCategoryList identifyCategory;
};
