#include "stdafx.h"

#include "TC_Aileron.h"
#include "Parody.h"
#include "../helpful/Log.h"
#include "../Fund/Aileron.h"
#include "../Fund/Lexicon.h"

using namespace std;
using namespace Parody;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Aileron );

void TC_Aileron::setUp()
{
    InterlockedLog::instance();
    Etui::instance();
}
void TC_Aileron::tearDown()
{
    Etui::Shutdowner();
}

void TC_Aileron::street()
{
    // ������� ������� � �� �����
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2 3" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+" ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+" ).minus( "2-" );
    Switch( "3" ).base( "2+" ).plus( "3+" ).minus( "3-" );
    // 
    // ������� 1 2 3 � �����
    //
    Elements( "1 2 3" ).set_ts("+1-0");
    // ����������� ���������
    Elements( "1!" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "1!  1+" ).check("UaG") );
    CPPUNIT_ASSERT( Elements( "1-  2+  2-  3+  3-" ).check("uag") );
    // ����������� ���������
    Elements( "1!" ).set_ts("a1u0");
    CPPUNIT_ASSERT( Elements( "1!  1+" ).check("AuG") );
    CPPUNIT_ASSERT( Elements( "1-  2+    2-  3+  3-" ).check("uag") );
    // 
    // ������� 1 � ������, 2&3 � �����
    //
    Elements( "1" ).set_ts("+0-1");
    Elements( "2 3" ).set_ts("+1-0");
    // ��������� �������� �� 1- 2+ 3+
    Elements( "1!" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "1! 1- 2+ 3+" ).check("UaG") );
    CPPUNIT_ASSERT( Elements( "1+ 2- 3-" ).check("uag") );
    // ��������� �������� �� 1- 2+ 3+
    Elements( "1!" ).set_ts("u0a1");
    CPPUNIT_ASSERT( Elements( "1! 1- 2+ 3+" ).check("uAG") );
    CPPUNIT_ASSERT( Elements( "1+ 2- 3-   " ).check("uag") );

    // 
    // ������� 1 �/�, 2&3 � �����
    //
    Elements( "1" ).set_ts("+0-0");
    Elements( "2 3" ).set_ts("+1-0");
    // ��������� �������� �� 1+ 1- 2+ 3+
    Elements( "1!" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "1! 1+ 1- 2+ 3+" ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "2- 3-" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "1!" ).check("G") );
    CPPUNIT_ASSERT( Elements( "2- 3- 1+ 1- 2+ 3+" ).check("g") );
    // ��������� �� ��������
    Elements( "1!" ).set_ts("u0a1");
    CPPUNIT_ASSERT( Elements( "1!" ).check("uAG") );
    CPPUNIT_ASSERT( Elements( "1- 1+ 2+ 2- 3+ 3-" ).check("uag") );
}

void TC_Aileron::opposite()
{
    // ������� ������� � �� �����
    Elements( STRIP, "1! 4!" ).make_ts("UL");
    Elements( SWITCH, "1 4" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+4+" ).minus( "1-"   );
    Switch( "4" ).base( "4!" ).plus( "1+4+" ).minus( "4-" );
    //
    // ������ ������ �� 1! - ���������� �� 1+4+ ����� ���� ������� 1
    //
    Elements( "1!" ).set_ts("u1");
    Elements( "4!" ).set_ts("u?");

    Elements( "1 4" ).set_ts("+1-0");
    CPPUNIT_ASSERT( Elements( "1! 1+4+   " ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "4! 1-   4-" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "1! 4! 1+4+" ).check("G") );
    CPPUNIT_ASSERT( Elements( "   1-   4-" ).check("g") );
    Elements( "1" ).set_ts("+0-0");
    CPPUNIT_ASSERT( Elements( "1! 1+4+ 1-" ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "4!      4-" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "1! 4! 1+4+" ).check("G") );
    CPPUNIT_ASSERT( Elements( "     1- 4-" ).check("g") );
    Elements( "1" ).set_ts("+0-1");
    CPPUNIT_ASSERT( Elements( "1! 1-     " ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "4! 1+4+ 4-" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "1! 1+4+ 1- 4!" ).check("G") );
    CPPUNIT_ASSERT( Elements( "4-           " ).check("g") );
    //
    // ������ ������ �� 4! - ���������� �� 1+4+ ����� ���� ������� 4
    //
    Elements( "1!" ).set_ts("u0");
    Elements( "4!" ).set_ts("u1");

    Elements( "1 4" ).set_ts("+1-0");
    CPPUNIT_ASSERT( Elements( "4! 1+4+   " ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "1! 1-   4-" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "1! 4! 1+4+" ).check("G") );
    CPPUNIT_ASSERT( Elements( "1-      4-" ).check("g") );
    Elements( "4" ).set_ts("+0-0");
    CPPUNIT_ASSERT( Elements( "4! 1+4+ 4-" ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "1!      1-" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "1! 4! 1+4+" ).check("G") );
    CPPUNIT_ASSERT( Elements( "     1- 4-" ).check("g") );
    Elements( "4" ).set_ts("+0-1");
    CPPUNIT_ASSERT( Elements( "4! 4-     " ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "1! 1+4+ 1-" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "1! 1+4+ 4! 4- " ).check("G") );
    CPPUNIT_ASSERT( Elements( "1-            " ).check("g") );
    //
    // ������� �� 1! � 4! - ���������� �� 1+4+ ����� ����� ������� 1 ��� 4
    //
    Elements( "1!" ).set_ts("U1a0");
    Elements( "4!" ).set_ts("u0A1");

    Elements( "1" ).set_ts("+1-0");
    {
        Elements( "4" ).set_ts("+1-0");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("UaG") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("uag") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("uag") );
        }
        Elements( "4" ).set_ts("+0-0");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("UaG") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("uag") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("uag") );
        }
        Elements( "4" ).set_ts("+0-1");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("UaG") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("uag") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("uAG") );
        }
    }
    Elements( "1" ).set_ts("+0-0");
    {
        Elements( "4" ).set_ts("+1-0");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("UaG") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("Uag") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("uag") );
        }
        Elements( "4" ).set_ts("+0-0");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("Ua") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("Ua") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("ua") );
            CPPUNIT_ASSERT( Elements( "1+4+ 1- 4-" ).check("g") );
        }
        Elements( "4" ).set_ts("+0-1");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("Ua") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("Ua") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("uAG") );
            CPPUNIT_ASSERT( Elements( "1+4+ 1- " ).check("g") );
        }
    }
    Elements( "1" ).set_ts("+0-1");
    {
        Elements( "4" ).set_ts("+1-0");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("uAG") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("UaG") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("uag") );
        }
        Elements( "4" ).set_ts("+0-0");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("uag") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("UaG") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("uag") );
        }
        Elements( "4" ).set_ts("+0-1");
        {
            CPPUNIT_ASSERT( Elements( "1+4+" ).check("uag") );
            CPPUNIT_ASSERT( Elements( "1-" ).check("UaG") );
            CPPUNIT_ASSERT( Elements( "4-" ).check("uAG") );
        }
    }
}

void TC_Aileron::dropper()
{
    // ������� ������� � �� �����
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 4" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+4+" ).minus( "1-" );
    Switch( "4" ).base( "4!" ).plus( "1+4+" ).minus( "4-" );
    // ������ ��������� � ��������� ������
    Elements( "1" ).set_ts("+1-0");
    Elements( "4" ).set_ts("+1-0");
    {
        Elements( "1!" ).set_ts("U1a0");
        CPPUNIT_ASSERT( Elements( "1+4+ 4!" ).check("UaG") );
        CPPUNIT_ASSERT( Elements( "1-" ).check("uag") );
        CPPUNIT_ASSERT( Elements( "4-" ).check("uag") );
        Elements( "1!" ).set_ts("u0A1");
        CPPUNIT_ASSERT( Elements( "1+4+ 4!" ).check("uA") );
        CPPUNIT_ASSERT( Elements( "1-" ).check("ua") );
        CPPUNIT_ASSERT( Elements( "4-" ).check("ua") );
        CPPUNIT_ASSERT( Elements( "1- 4-" ).check("g") );
        CPPUNIT_ASSERT( Elements( "1+4+ 4!" ).check("G") );
    }
    Elements( "4" ).set_ts("+0-0");
    {
        Elements( "1!" ).set_ts("U1a0");
        CPPUNIT_ASSERT( Elements( "1+4+" ).check("UaG") );
        CPPUNIT_ASSERT( Elements( "4!" ).check("uaG") );
        CPPUNIT_ASSERT( Elements( "1-" ).check("uag") );
        CPPUNIT_ASSERT( Elements( "4-" ).check("uag") );
        Elements( "1!" ).set_ts("u0A1");
        CPPUNIT_ASSERT( Elements( "1+4+" ).check("uAG") );
        CPPUNIT_ASSERT( Elements( "4!" ).check("uaG") );
        CPPUNIT_ASSERT( Elements( "1-" ).check("uag") );
        CPPUNIT_ASSERT( Elements( "4-" ).check("uag") );
        CPPUNIT_ASSERT( Elements( "4 " ).check( "G" ) );
    }
    Elements( "4" ).set_ts("+0-1");
    {
        Elements( "1!" ).set_ts("U1a0");
        CPPUNIT_ASSERT( Elements( "1+4+" ).check("UaG") );
        CPPUNIT_ASSERT( Elements( "4!" ).check("uaG") );
        CPPUNIT_ASSERT( Elements( "1-" ).check("uag") );
        CPPUNIT_ASSERT( Elements( "4-" ).check("uaG") );
        Elements( "1!" ).set_ts("u0A1");
        CPPUNIT_ASSERT( Elements( "1+4+" ).check("uAG") );
        CPPUNIT_ASSERT( Elements( "4!" ).check("uaG") );
        CPPUNIT_ASSERT( Elements( "1-" ).check("uag") );
        CPPUNIT_ASSERT( Elements( "4-" ).check("uaG") );
    }
}

void TC_Aileron::tick1646()
{
    // ������� ������� � �� �����
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2 3 4 5 7 8" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+4+" ).minus( "1-"   );
    Switch( "2" ).base( "1-" ).plus( "2+"   ).minus( "2-"   );
    Switch( "3" ).base( "2+" ).plus( "3+"   ).minus( "3-4-" );
    Switch( "4" ).base( "8+" ).plus( "1+4+" ).minus( "3-4-" );
    Switch( "5" ).base( "3+" ).plus( "5+"   ).minus( "5-"   );
    Switch( "7" ).base( "5+" ).plus( "7+"   ).minus( "7-8-" );
    Switch( "8" ).base( "8!" ).plus( "8+"   ).minus( "7-8-" );
    // �������� ��������� (��. ����� 1646)
    Elements( "1!" ).set_ts("u1");
    Elements( "1" ).set_ts("+0-1");
    Elements( "2" ).set_ts("+1-0");
    Elements( "3 4" ).set_ts("+0-1");
    Elements( "5 7 8" ).set_ts("+1-0");
    CPPUNIT_ASSERT( Elements( "1! 1-   2+ 3-4-       8+  " ).check("U") );
    CPPUNIT_ASSERT( Elements( "   1+4+ 2- 3+   5+ 7+ 7-8-" ).check("u") );

    // �������� (��. ����� 1646)
    Elements( "3" ).set_ts("+1-0");
    CPPUNIT_ASSERT( Elements( "1! 1-   2+ 3+   5+ 7+        " ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "           3-4-       8+   8!" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "   1+4+ 2-      5-    7-8-   " ).check("ua") );
}

void TC_Aileron::tick1646_comment3()
{
    // ������� ������� � �� �����
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2 3 4 5 6 7 8 9 10 11 12 13" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+"  ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+"  ).minus( "2-" );
    Switch( "3" ).base( "2+" ).plus( "3+"  ).minus( "3-" );
    Switch( "4" ).base( "4!" ).plus( "1+"  ).minus( "3-" );
    Switch( "5" ).base( "3+" ).plus( "5+"  ).minus( "5-" );
    Switch( "6" ).base( "6!" ).plus( "12+" ).minus( "5-" );
    Switch( "7" ).base( "5+" ).plus( "7+"  ).minus( "7-" );
    Switch( "8" ).base( "8!" ).plus( "4!"  ).minus( "7-" );
    Switch( "9" ).base( "9!" ).plus( "9+"  ).minus( "2-" );
    Switch( "10").base( "10!").plus( "9+"  ).minus( "10-");
    Switch( "11").base( "11!").plus( "11+" ).minus( "10-" );
    Switch( "12").base( "9!" ).plus( "12+" ).minus( "12-");
    Switch( "13").base( "13!").plus( "11!" ).minus( "12-" );
    Switch( "14").base( "6!" ).plus( "14+" ).minus( "14-" );
    Switch( "15").base( "15!").plus( "13!" ).minus( "14-" );

    // ��������� ��������� (��. ����� 1646 ����������� 3)
    Elements( "1!" ).set_ts("u1");
    Elements( "1 2 9" ).set_ts("+0-1");
    CPPUNIT_ASSERT( Elements( "1! 1- 2- 9!" ).check("U") );
    CPPUNIT_ASSERT( Elements( "   1+ 2+ 9+" ).check("u") );

    // ����������� �������� � �������
    Elements( "12 6" ).set_ts("+1-0");
    Elements( "14 15" ).set_ts("+0-1");
    CPPUNIT_ASSERT( Elements( "12+ 6! 14- 15!" ).check("U") );
    CPPUNIT_ASSERT( Elements( "12-    14+ 13!" ).check("u") );

    Elements( "12" ).set_ts("+0-1");
    CPPUNIT_ASSERT( Elements( "12-           " ).check("U") );
    CPPUNIT_ASSERT( Elements( "12+ 6! 14- 15!" ).check("u") );
}

// ��������, ��� ������� � ����� ����� �� ������ ���� �� ����� ������ �������
// ������, ���������������� ������ ������� ��������� ���������
void TC_Aileron::independent_with_TS()
{
    // ������� ������� � �� �����
    Elements( STRIP, "A B C" ).make_ts("UL");
    Elements( SWITCH, "1"    ).make_ts("+-");
    Switch( "1" ).base( "A" ).plus( "B" ).minus( "C" );
    // �������� ��������� - ��� ������� passive
    Elements( "1" ).set_ts("+0-0");
    Elements( "A B C" ).set_ts("u0a0");
    // 
    // ������� � �����
    //
    Elements( "1" ).set_ts("+1-0");
    // ������������� ��������� �� ����
    Elements( "A" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "A"   ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "B C" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "A B" ).check("G") );
    CPPUNIT_ASSERT( Elements( "C"   ).check("g") );

    // ������������� ��������� �� ����
    Elements( "A" ).set_ts("u0a1");
    CPPUNIT_ASSERT( Elements( "A"   ).check("uA") );
    CPPUNIT_ASSERT( Elements( "B C" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "A B" ).check("G") );
    CPPUNIT_ASSERT( Elements( "C"   ).check("g") );

    // ������������� ��������� �� ��������� �������
    Elements( "A" ).set_ts("u0a0");
    Elements( "B" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "B"   ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "A C" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "A B" ).check("G") );
    CPPUNIT_ASSERT( Elements( "C"   ).check("g") );

    // ������������� ��������� �� ��������� �������
    Elements( "B" ).set_ts("u0a1");
    CPPUNIT_ASSERT( Elements( "B"   ).check("uA") );
    CPPUNIT_ASSERT( Elements( "A C" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "A B" ).check("G") );
    CPPUNIT_ASSERT( Elements( "C"   ).check("g") );

    // ��� ��������� ������� �������� �� ��������
}

// �������� ������� "����������" ����� �������
void TC_Aileron::unknown_1631()
{
    // ������� ������� � �� �����
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+" ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+" ).minus( "2-" );

    // ��������� ������� ����������
    Elements( "1!" ).set_ts("u?a?");
    CPPUNIT_ASSERT( Elements( "1!" ).check("k") );

    const char * _PLUS  = "+1-0";
    const char * _MINUS = "+0-1";
    const char * _OOC   = "+0-0";
    const char * ANY[] = {_PLUS, _MINUS, _OOC};

    // ������� �� ���� ���������� - ������� ��� ����� ����� ����������
    for(auto & i1 : ANY)
    {
        Elements( "1" ).set_ts(i1);
        for(auto & i2 : ANY)
        {
            Elements( "2" ).set_ts(i2);
            CPPUNIT_ASSERT( Elements( " 1+ 1- 2+ 2- " ).check("k") );
        }
    }
}

void TC_Aileron::in_route()
{
    // ������� ������� � �� �����
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2 3" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+" ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+" ).minus( "2-" );
    Switch( "3" ).base( "2+" ).plus( "3+" ).minus( "3-" );
    // ������� 1 � ������, 2&3 � �����
    Elements( "1  " ).set_ts("+0-1");
    Elements( "2 3" ).set_ts("+1-0");
    // �������� ��� ����� ��������
    Elements base( "1!" );
    Elements routed( "1- 2+ 3+" );
    Elements other( "1+ 2- 3-" );
    // ��������� ���������� �� ������� �������
    base.set_ts("u0a1");
    // ��������� ����������� ����� �������
    CPPUNIT_ASSERT( routed.check("uAG") ); 
    // �� ����������� ���� �������
    CPPUNIT_ASSERT( other .check("uag") );
    // ������� � �������� ����� �� ���������
    CPPUNIT_ASSERT( base  .chk_los(&LOS_base::lockedInTrainRoute,false) );
    CPPUNIT_ASSERT( routed.chk_los(&LOS_base::lockedInTrainRoute,false) );
    CPPUNIT_ASSERT( other .chk_los(&LOS_base::lockedInTrainRoute,false) );
    // ������� ������� �������� � �������� �������
    base.call_los( &LOS_base::bindRoute, true );
    base.call_los( &LOS_base::setRouteType, true );
    // ���������
    CPPUNIT_ASSERT( base  .chk_los(&LOS_base::lockedInTrainRoute,true) );  // ������� � ��������
    CPPUNIT_ASSERT( routed.chk_los(&LOS_base::lockedInTrainRoute,true) );  // ��������� ��������� ���� � ��������
    CPPUNIT_ASSERT( other .chk_los(&LOS_base::lockedInTrainRoute,false) ); // ������ - �� � ��������
}

void TC_Aileron::receives_lts()
{
    // ���������� ������� - ������� ������� � �����
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+" ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+" ).minus( "2-" );
    // ������ ���������� ������� - ����� ��� ����
    Elements( SWITCH, "7 9" ).make_ts("+-");
    Switch( "7" ).base( "7!" ).plus( "7+" ).minus( "7-" );
    Switch( "9" ).base( "7-" ).plus( "9+" ).minus( "9-" );
    //
    Etui::instance()->complete();
    //
    auto elem = [](const std::wstring& name) -> const CLogicElement* {
        BadgeE bdg( name, Elements::commonEsr() );
        Lexicon& lex = Etui::instance()->lex();
        return lex.LogElBy( bdg );
    };
    //
    Ailerons& ailerons = Etui::instance()->ailerons();

    CPPUNIT_ASSERT( ailerons.receives_lts( elem(L"1!") ) );
    CPPUNIT_ASSERT( ailerons.receives_lts( elem(L"1+") ) );
    CPPUNIT_ASSERT( ailerons.receives_lts( elem(L"1-") ) );
    CPPUNIT_ASSERT( ailerons.receives_lts( elem(L"2+") ) );
    CPPUNIT_ASSERT( ailerons.receives_lts( elem(L"2-") ) );

    CPPUNIT_ASSERT( !ailerons.receives_lts( elem(L"7!") ) );
    CPPUNIT_ASSERT( !ailerons.receives_lts( elem(L"7+") ) );
    CPPUNIT_ASSERT( !ailerons.receives_lts( elem(L"7-") ) );
    CPPUNIT_ASSERT( !ailerons.receives_lts( elem(L"9+") ) );
    CPPUNIT_ASSERT( !ailerons.receives_lts( elem(L"9-") ) );
}
