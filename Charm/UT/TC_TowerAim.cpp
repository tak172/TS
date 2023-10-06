#include "stdafx.h"
#include "TC_TowerAim.h"
#include "../Tower/Aim.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TowerAim );

namespace bt = boost::posix_time;
namespace bg = boost::gregorian;

const EsrKit esr(12345);
const BadgeE iu( L"iu", esr );
const BadgeE dn( L"dn", esr );
const BadgeE up( L"up", esr );
const BadgeE curr( L"curr", esr );
const std::wstring ret(L"nord");

// хранение проверочных данных 
typedef std::map< BadgeE, std::pair<std::wstring, Aim::WaterMark> > SAMPLE_TYPE;

// проверка наличия всех
static bool TakeAndCheck( Aim* aim, const SAMPLE_TYPE& sample )
{
    SAMPLE_TYPE my_sample= sample;

    BadgeE bdg;
    std::wstring text;
    Aim::WaterMark wm;
    while( aim->take_state( bdg, text, wm ) ) {
        auto it = my_sample.find( bdg );
        if ( my_sample.end() == it )
            return false;
        if ( it->second.first != text )
            return false;
        if ( it->second.second != wm )
            return false;
        my_sample.erase( it );
    }
    return my_sample.empty();
}

void TC_TowerAim::test_static()
{
    Aim aim;
    aim.append( ret, iu, dn, curr, up );

    // исходное состояние - все пусто
    SAMPLE_TYPE sample;
    sample[ dn ]   = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    sample[ curr ] = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    sample[ up ]   = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    CPPUNIT_ASSERT_MESSAGE( "start: all empty",  TakeAndCheck( &aim, sample ) );

    // некий поезд на этом участке
    RollingStock::Train tr;
    tr.dba( DB_ADDR(0x7654) );
    tr.head( iu );
    tr.speed( 77 );
    // вносим границы и ставим поезд
    aim.set_bound( ret, tr.dba(), 11.1, 22.2 );
    aim.apply( tr );

    //
    // все поля работают
    sample[ dn ]   = std::make_pair( L"11.1", Aim::WaterMark::TOO_LOW );
    sample[ curr ] = std::make_pair( L"77.0", Aim::WaterMark::TOO_HIGH );
    sample[ up ]   = std::make_pair( L"22.2", Aim::WaterMark::TOO_HIGH );
    CPPUNIT_ASSERT_MESSAGE( "set overspeed",  TakeAndCheck( &aim, sample ) );

    // границы меняются
    aim.set_bound( ret, tr.dba(), 0, 99 );

    sample[ dn ]   = std::make_pair(  L"0.0", Aim::WaterMark::TOO_LOW );
    sample[ curr ] = std::make_pair( L"77.0", Aim::WaterMark::NORMAL );
    sample[ up ]   = std::make_pair( L"99.0", Aim::WaterMark::TOO_HIGH );
    CPPUNIT_ASSERT_MESSAGE( "bound changed",  TakeAndCheck( &aim, sample ) );

    // все гаснет 
    aim.set_empty(iu);

    sample[ dn ]   = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    sample[ curr ] = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    sample[ up ]   = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    CPPUNIT_ASSERT_MESSAGE( "iu empty",  TakeAndCheck( &aim, sample ) );
}

void TC_TowerAim::test_dynamic()
{
    bt::ptime wall_clock( bg::date( 2022, 12, 30 ), bt::time_duration(0,0,0) );
    bt::time_duration STEP = bt::seconds(1);

    auto my_clock = [&wall_clock](){
        return wall_clock;
    };

    Aim aim;
    aim.append( ret, iu, dn, curr, up );
    aim.set_clock( my_clock, STEP );
    aim.set_game_time( 1 );

    SAMPLE_TYPE sample;

    // некий поезд на этом участке
    RollingStock::Train tr;
    tr.dba( DB_ADDR(0x7654) );
    tr.head( iu );
    tr.speed( 23 );
    tr.speedup( -3600*8.0 ); //тормозит на 8 км/ч за секунду
    
    aim.set_bound( ret, tr.dba(), 11.1, 22.2 );
    aim.apply( tr );

    // все поля работают (время не менялось)
    sample[ dn ]   = std::make_pair( L"11.1", Aim::WaterMark::TOO_LOW );
    sample[ curr ] = std::make_pair( L"23.0", Aim::WaterMark::TOO_HIGH );
    sample[ up ]   = std::make_pair( L"22.2", Aim::WaterMark::TOO_HIGH );
    CPPUNIT_ASSERT_MESSAGE( "speed over",  TakeAndCheck( &aim, sample ) );

    // на следующем шаге
    wall_clock += STEP;
    sample.clear();
    sample[ curr ] = std::make_pair( L"15.0", Aim::WaterMark::NORMAL );
    CPPUNIT_ASSERT_MESSAGE( "step1-normal",  TakeAndCheck( &aim, sample ) );

    // на втором шаге
    wall_clock += STEP;
    sample.clear();
    sample[ curr ] = std::make_pair(  L"7.0", Aim::WaterMark::TOO_LOW );
    CPPUNIT_ASSERT_MESSAGE( "step2-too low",  TakeAndCheck( &aim, sample ) );

    // проверка на полную остановку
    wall_clock += STEP;
    sample.clear();
    sample[ curr ] = std::make_pair(  L"0.0", Aim::WaterMark::TOO_LOW );
    CPPUNIT_ASSERT_MESSAGE( "step3-stopped",  TakeAndCheck( &aim, sample ) );


    // все гаснет 
    aim.set_empty(iu);
    sample.clear();
    sample[ dn ]   = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    sample[ curr ] = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    sample[ up ]   = std::make_pair( L" ", Aim::WaterMark::NORMAL );
    CPPUNIT_ASSERT_MESSAGE( "iu empty",  TakeAndCheck( &aim, sample ) );
}
