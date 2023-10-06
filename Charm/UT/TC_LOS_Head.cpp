#include "stdafx.h"

#include <sstream>
#include "TC_LOS_Head.h"
#include "../Actor/LOSBase.h"
#include "../Actor/Latch_LOSHead.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LOS_Head );

void TC_LOS_Head::setUp()
{
    Latch_LOS_Head::Shutdowner();
    // настроить удержатель открытого состояния
    latch = Latch_LOS_Head::instance();
    latch->set_interval( INTERV );
    latch->set_wallclock( T );
}

void TC_LOS_Head::tearDown()
{
    Latch_LOS_Head::Shutdowner();
}

void TC_LOS_Head::immediate()
{
    std::unique_ptr<LOS_base> head( LOS_base::fabric_from_object(HEAD) );

    //
    head->setOpenSignal(StateLTS::PASSIVE);
    head->setOpenRepSignal(StateLTS::PASSIVE);
    head->setCloseSignal(StateLTS::PASSIVE);
    head->setBreakSignal(StateLTS::PASSIVE);
    CPPUNIT_ASSERT(!head->isOpen()
                && !head->isOpenRep()
                && !head->isClosed()
                && !head->isBreak()
                );
    //
    head->setOpenSignal(StateLTS::ACTIVE);
    CPPUNIT_ASSERT( head->isOpen()
                && !head->isOpenRep()
                && !head->isClosed()
                && !head->isBreak()
                );
    head->setOpenSignal(StateLTS::PASSIVE);
    //
    head->setOpenRepSignal(StateLTS::ACTIVE);
    CPPUNIT_ASSERT(!head->isOpen()
        &&  head->isOpenRep()
        && !head->isClosed()
        && !head->isBreak()
        );
    head->setOpenRepSignal(StateLTS::PASSIVE);
    //
    head->setCloseSignal(StateLTS::ACTIVE);
    CPPUNIT_ASSERT(!head->isOpen()
        && !head->isOpenRep()
        &&  head->isClosed()
        && !head->isBreak()
        );
    head->setCloseSignal(StateLTS::PASSIVE);
    //
    head->setBreakSignal(StateLTS::ACTIVE);
    CPPUNIT_ASSERT(!head->isOpen()
        && !head->isOpenRep()
        && !head->isClosed()
        &&  head->isBreak()
        );
    head->setBreakSignal(StateLTS::PASSIVE);
}

void TC_LOS_Head::open_priority()
{
    std::unique_ptr<LOS_base> head( LOS_base::fabric_from_object(HEAD) );
    //
    head->setOpenSignal(   StateLTS::PASSIVE );
    head->setOpenRepSignal(StateLTS::PASSIVE );
    head->setCloseSignal(  StateLTS::PASSIVE );
    head->setBreakSignal(  StateLTS::PASSIVE );
    CPPUNIT_ASSERT( !head->isOpen()    );
    CPPUNIT_ASSERT( !head->isBreak()   );
    //
    head->setOpenSignal( StateLTS::ACTIVE );
    CPPUNIT_ASSERT( head->isOpen()    );
    CPPUNIT_ASSERT( !head->isBreak()   );
    //
    head->setCloseSignal( StateLTS::ACTIVE );
    head->setBreakSignal( StateLTS::ACTIVE );
    CPPUNIT_ASSERT( head->isOpen()    );
    CPPUNIT_ASSERT( head->isBreak()   );
}

void TC_LOS_Head::latching()
{
    //
    const BadgeE bdg( L"Ч1", EsrKit(1000) );
    std::unique_ptr<LOS_base> head( LOS_base::fabric_from_object( HEAD ) ); // удерживаемый
    latch->insert( bdg, head.get() );
    //
    latch->set_wallclock( T );
    CPPUNIT_ASSERT( !head->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // открываем
    latch->set_wallclock( T+1 );
    CPPUNIT_ASSERT( head->setOpenSignal(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( head->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // закрываем (тем же самым сигналом)
    latch->set_wallclock( T+2 );
    CPPUNIT_ASSERT( head->setOpenSignal(StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( head->isOpen() );  // ещё защелкнут
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // в самом конце интервала защелкивания
    latch->set_wallclock( T + INTERV + 1 );
    CPPUNIT_ASSERT( head->isOpen() );  // ещё не закрылся
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // после интервала защелкивания
    latch->set_wallclock( T + INTERV + 2 );
    CPPUNIT_ASSERT( latch->has_expired() );  // есть истекшая защелка
    CPPUNIT_ASSERT( bdg == latch->process_expired() );  // закрыть cветофор и проверить его имя
    CPPUNIT_ASSERT( !head->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // более ничего не менялось
}

void TC_LOS_Head::latching_reopen()
{
    const BadgeE bdg( L"Ч4", EsrKit(4000) );
    std::unique_ptr<LOS_base> head( LOS_base::fabric_from_object( HEAD ) ); // удерживаемый
    latch->insert( bdg, head.get() );
    //
    latch->set_wallclock( T );
    CPPUNIT_ASSERT( !head->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // открываем
    latch->set_wallclock( T+1 );
    head->setOpenSignal(StateLTS::ACTIVE);
    CPPUNIT_ASSERT( head->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // быстро закрываем
    latch->set_wallclock( T+2 );
    CPPUNIT_ASSERT( head->setOpenSignal(StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( head->isOpen() );  // ещё защелкнут
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // открываем повторно
    latch->set_wallclock( T+3 );
    CPPUNIT_ASSERT( head->setOpenSignal(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( head->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // и закрываем после повторного открытия
    latch->set_wallclock( T+4 );
    CPPUNIT_ASSERT( head->setOpenSignal(StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( head->isOpen() );  // ещё защелкнут
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // истек интервал после первого закрытия
    latch->set_wallclock( T + INTERV + 2 );
    CPPUNIT_ASSERT( head->isOpen() );  // ещё не закрылся
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // истек интервал после второго закрытия
    latch->set_wallclock( T + INTERV + 4 );
    CPPUNIT_ASSERT( head->isOpen() );
    CPPUNIT_ASSERT( latch->has_expired() );  // есть истекшая защелка
    CPPUNIT_ASSERT( bdg == latch->process_expired() );  // закрыть cветофор и проверить его имя
    CPPUNIT_ASSERT( !head->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // более ничего не менялось
}

void TC_LOS_Head::latching_select()
{
    std::set<BadgeE> badges;
    std::unique_ptr<LOS_base> x( LOS_base::fabric_from_object( HEAD ) ); // не защелкиваемый
    std::unique_ptr<LOS_base> a( LOS_base::fabric_from_object( HEAD ) ); // защелкиваемый
    std::unique_ptr<LOS_base> b( LOS_base::fabric_from_object( HEAD ) ); // защелкиваемый

    badges.insert( BadgeE( L"AA", EsrKit(4000) ) );
    latch->insert( BadgeE( L"AA", EsrKit(4000) ), a.get() );
    badges.insert( BadgeE( L"BB", EsrKit(4000) ) );
    latch->insert( BadgeE( L"BB", EsrKit(4000) ), b.get() );
    // все закрыты
    latch->set_wallclock( T );
    CPPUNIT_ASSERT( !x->isOpen() );
    CPPUNIT_ASSERT( !a->isOpen() );
    CPPUNIT_ASSERT( !b->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // открываем все
    latch->set_wallclock( T+1 );
    CPPUNIT_ASSERT( x->setOpenSignal(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( a->setOpenSignal(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( b->setOpenSignal(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( x->isOpen() );
    CPPUNIT_ASSERT( a->isOpen() );
    CPPUNIT_ASSERT( b->isOpen() );
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // закрываем все
    latch->set_wallclock( T+2 );
    CPPUNIT_ASSERT( x->setOpenSignal(StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( a->setOpenSignal(StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( b->setOpenSignal(StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( !x->isOpen() ); // не защелкивается
    CPPUNIT_ASSERT( a->isOpen() );  // защелкнут и удерживается
    CPPUNIT_ASSERT( b->isOpen() );  // защелкнут и удерживается
    CPPUNIT_ASSERT( !latch->has_expired() );  // ничего не менялось
    // после интервала защелкивания должны выключиться оба светофора
    latch->set_wallclock( T + INTERV + 2 );
    while( latch->has_expired() )
    {
        auto n = latch->process_expired();
        CPPUNIT_ASSERT( badges.count(n) != 0 );
        badges.erase( n );
    }
    CPPUNIT_ASSERT( badges.empty() );
    CPPUNIT_ASSERT( !x->isOpen() ); // не защелкивается
    CPPUNIT_ASSERT( !a->isOpen() ); // уже не защелкнут
    CPPUNIT_ASSERT( !b->isOpen() ); // уже не защелкнут
    CPPUNIT_ASSERT( !latch->has_expired() );  // более ничего не менялось
}
