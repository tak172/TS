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
    // создать объекты и их связи
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2 3" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+" ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+" ).minus( "2-" );
    Switch( "3" ).base( "2+" ).plus( "3+" ).minus( "3-" );
    // 
    // стрелки 1 2 3 в плюсе
    //
    Elements( "1 2 3" ).set_ts("+1-0");
    // прохождение занятости
    Elements( "1!" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "1!  1+" ).check("UaG") );
    CPPUNIT_ASSERT( Elements( "1-  2+  2-  3+  3-" ).check("uag") );
    // прохождение замыкания
    Elements( "1!" ).set_ts("a1u0");
    CPPUNIT_ASSERT( Elements( "1!  1+" ).check("AuG") );
    CPPUNIT_ASSERT( Elements( "1-  2+    2-  3+  3-" ).check("uag") );
    // 
    // стрелка 1 в минусе, 2&3 в плюсе
    //
    Elements( "1" ).set_ts("+0-1");
    Elements( "2 3" ).set_ts("+1-0");
    // занятость проходит на 1- 2+ 3+
    Elements( "1!" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "1! 1- 2+ 3+" ).check("UaG") );
    CPPUNIT_ASSERT( Elements( "1+ 2- 3-" ).check("uag") );
    // замыкание проходит на 1- 2+ 3+
    Elements( "1!" ).set_ts("u0a1");
    CPPUNIT_ASSERT( Elements( "1! 1- 2+ 3+" ).check("uAG") );
    CPPUNIT_ASSERT( Elements( "1+ 2- 3-   " ).check("uag") );

    // 
    // стрелка 1 в/к, 2&3 в плюсе
    //
    Elements( "1" ).set_ts("+0-0");
    Elements( "2 3" ).set_ts("+1-0");
    // занятость проходит на 1+ 1- 2+ 3+
    Elements( "1!" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "1! 1+ 1- 2+ 3+" ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "2- 3-" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "1!" ).check("G") );
    CPPUNIT_ASSERT( Elements( "2- 3- 1+ 1- 2+ 3+" ).check("g") );
    // замыкание не проходит
    Elements( "1!" ).set_ts("u0a1");
    CPPUNIT_ASSERT( Elements( "1!" ).check("uAG") );
    CPPUNIT_ASSERT( Elements( "1- 1+ 2+ 2- 3+ 3-" ).check("uag") );
}

void TC_Aileron::opposite()
{
    // создать объекты и их связи
    Elements( STRIP, "1! 4!" ).make_ts("UL");
    Elements( SWITCH, "1 4" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+4+" ).minus( "1-"   );
    Switch( "4" ).base( "4!" ).plus( "1+4+" ).minus( "4-" );
    //
    // сигнал только на 1! - передается на 1+4+ через плюс стрелки 1
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
    // сигнал только на 4! - передается на 1+4+ через плюс стрелки 4
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
    // сигналы на 1! и 4! - передается на 1+4+ через плюсы стрелок 1 или 4
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
    // создать объекты и их связи
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 4" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+4+" ).minus( "1-" );
    Switch( "4" ).base( "4!" ).plus( "1+4+" ).minus( "4-" );
    // задать состояния и проверить работу
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
    // создать объекты и их связи
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2 3 4 5 7 8" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+4+" ).minus( "1-"   );
    Switch( "2" ).base( "1-" ).plus( "2+"   ).minus( "2-"   );
    Switch( "3" ).base( "2+" ).plus( "3+"   ).minus( "3-4-" );
    Switch( "4" ).base( "8+" ).plus( "1+4+" ).minus( "3-4-" );
    Switch( "5" ).base( "3+" ).plus( "5+"   ).minus( "5-"   );
    Switch( "7" ).base( "5+" ).plus( "7+"   ).minus( "7-8-" );
    Switch( "8" ).base( "8!" ).plus( "8+"   ).minus( "7-8-" );
    // исходное состояние (см. тикет 1646)
    Elements( "1!" ).set_ts("u1");
    Elements( "1" ).set_ts("+0-1");
    Elements( "2" ).set_ts("+1-0");
    Elements( "3 4" ).set_ts("+0-1");
    Elements( "5 7 8" ).set_ts("+1-0");
    CPPUNIT_ASSERT( Elements( "1! 1-   2+ 3-4-       8+  " ).check("U") );
    CPPUNIT_ASSERT( Elements( "   1+4+ 2- 3+   5+ 7+ 7-8-" ).check("u") );

    // действие (см. тикет 1646)
    Elements( "3" ).set_ts("+1-0");
    CPPUNIT_ASSERT( Elements( "1! 1-   2+ 3+   5+ 7+        " ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "           3-4-       8+   8!" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "   1+4+ 2-      5-    7-8-   " ).check("ua") );
}

void TC_Aileron::tick1646_comment3()
{
    // создать объекты и их связи
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

    // ошибочное состояние (см. тикет 1646 комментарий 3)
    Elements( "1!" ).set_ts("u1");
    Elements( "1 2 9" ).set_ts("+0-1");
    CPPUNIT_ASSERT( Elements( "1! 1- 2- 9!" ).check("U") );
    CPPUNIT_ASSERT( Elements( "   1+ 2+ 9+" ).check("u") );

    // продолжение проверок в глубину
    Elements( "12 6" ).set_ts("+1-0");
    Elements( "14 15" ).set_ts("+0-1");
    CPPUNIT_ASSERT( Elements( "12+ 6! 14- 15!" ).check("U") );
    CPPUNIT_ASSERT( Elements( "12-    14+ 13!" ).check("u") );

    Elements( "12" ).set_ts("+0-1");
    CPPUNIT_ASSERT( Elements( "12-           " ).check("U") );
    CPPUNIT_ASSERT( Elements( "12+ 6! 14- 15!" ).check("u") );
}

// Проверка, что участки с ТСами никак не влияют друг на друга сквозь стрелку
// Точнее, распространяется только признак подсветки горловины
void TC_Aileron::independent_with_TS()
{
    // создать объекты и их связи
    Elements( STRIP, "A B C" ).make_ts("UL");
    Elements( SWITCH, "1"    ).make_ts("+-");
    Switch( "1" ).base( "A" ).plus( "B" ).minus( "C" );
    // Исходное состояние - все сигналы passive
    Elements( "1" ).set_ts("+0-0");
    Elements( "A B C" ).set_ts("u0a0");
    // 
    // стрелка в плюсе
    //
    Elements( "1" ).set_ts("+1-0");
    // НЕпрохождение занятости от базы
    Elements( "A" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "A"   ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "B C" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "A B" ).check("G") );
    CPPUNIT_ASSERT( Elements( "C"   ).check("g") );

    // НЕпрохождение замыкания от базы
    Elements( "A" ).set_ts("u0a1");
    CPPUNIT_ASSERT( Elements( "A"   ).check("uA") );
    CPPUNIT_ASSERT( Elements( "B C" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "A B" ).check("G") );
    CPPUNIT_ASSERT( Elements( "C"   ).check("g") );

    // НЕпрохождение занятости от плюсового элерона
    Elements( "A" ).set_ts("u0a0");
    Elements( "B" ).set_ts("u1a0");
    CPPUNIT_ASSERT( Elements( "B"   ).check("Ua") );
    CPPUNIT_ASSERT( Elements( "A C" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "A B" ).check("G") );
    CPPUNIT_ASSERT( Elements( "C"   ).check("g") );

    // НЕпрохождение замыкания от плюсового элерона
    Elements( "B" ).set_ts("u0a1");
    CPPUNIT_ASSERT( Elements( "B"   ).check("uA") );
    CPPUNIT_ASSERT( Elements( "A C" ).check("ua") );
    CPPUNIT_ASSERT( Elements( "A B" ).check("G") );
    CPPUNIT_ASSERT( Elements( "C"   ).check("g") );

    // Для остальных случаев проверки не написаны
}

// Проверка раздачи "неизвестно" через стрелки
void TC_Aileron::unknown_1631()
{
    // создать объекты и их связи
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+" ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+" ).minus( "2-" );

    // состояние участка неизвестно
    Elements( "1!" ).set_ts("u?a?");
    CPPUNIT_ASSERT( Elements( "1!" ).check("k") );

    const char * _PLUS  = "+1-0";
    const char * _MINUS = "+0-1";
    const char * _OOC   = "+0-0";
    const char * ANY[] = {_PLUS, _MINUS, _OOC};

    // стрелки во всех состояниях - участки все равно будут неизвестны
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
    // создать объекты и их связи
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2 3" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+" ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+" ).minus( "2-" );
    Switch( "3" ).base( "2+" ).plus( "3+" ).minus( "3-" );
    // стрелка 1 в минусе, 2&3 в плюсе
    Elements( "1  " ).set_ts("+0-1");
    Elements( "2 3" ).set_ts("+1-0");
    // названия для групп участков
    Elements base( "1!" );
    Elements routed( "1- 2+ 3+" );
    Elements other( "1+ 2- 3-" );
    // замыкание выставляем на опорный участок
    base.set_ts("u0a1");
    // замыкание растекается через стрелки
    CPPUNIT_ASSERT( routed.check("uAG") ); 
    // НЕ растекается мимо стрелок
    CPPUNIT_ASSERT( other .check("uag") );
    // сначала в маршруте никто не участвует
    CPPUNIT_ASSERT( base  .chk_los(&LOS_base::lockedInTrainRoute,false) );
    CPPUNIT_ASSERT( routed.chk_los(&LOS_base::lockedInTrainRoute,false) );
    CPPUNIT_ASSERT( other .chk_los(&LOS_base::lockedInTrainRoute,false) );
    // опорный участок включаем в поездной маршрут
    base.call_los( &LOS_base::bindRoute, true );
    base.call_los( &LOS_base::setRouteType, true );
    // проверяем
    CPPUNIT_ASSERT( base  .chk_los(&LOS_base::lockedInTrainRoute,true) );  // опорный в маршруте
    CPPUNIT_ASSERT( routed.chk_los(&LOS_base::lockedInTrainRoute,true) );  // остальные замкнутые тоже в маршруте
    CPPUNIT_ASSERT( other .chk_los(&LOS_base::lockedInTrainRoute,false) ); // прочие - не в маршруте
}

void TC_Aileron::receives_lts()
{
    // стрелочный участок - базовый элемент с ТСами
    Elements( STRIP, "1!" ).make_ts("UL");
    Elements( SWITCH, "1 2" ).make_ts("+-");
    Switch( "1" ).base( "1!" ).plus( "1+" ).minus( "1-" );
    Switch( "2" ).base( "1-" ).plus( "2+" ).minus( "2-" );
    // другой стрелочный участок - нигде нет ТСов
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
