#include "stdafx.h"

#include "TC_ChainInfo.h"
#include "../Tower/CarChain.h"
#include "../Tower/CarNumber.h"
#include "../Tower/ChainInfo.h"
#include "../Tower/RollingStock.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ChainInfo );

//using namespace RollingStock;

TC_ChainInfo::~TC_ChainInfo()
{}

void TC_ChainInfo::setUp()
{
    m_model.reset( new RollingStock::Model() );
    m_chaininfo.reset( new ChainInfo(*m_model) );
}

void TC_ChainInfo::tearDown()
{
    m_chaininfo.reset();
    m_model.reset();
}

void TC_ChainInfo::simple()
{
    CPPUNIT_ASSERT( 0 == m_chaininfo->car_count() );
    m_chaininfo->write( 0, CarChain( CarNumber(33330011), 0 ) );
    m_chaininfo->write( 1, CarChain( CarNumber(33330012), 0 ) );
    m_chaininfo->write( 2, CarChain( CarNumber(33330013), 0 ) );
    CPPUNIT_ASSERT( 3 == m_chaininfo->car_count() );
    CPPUNIT_ASSERT( 1 == m_chaininfo->uncoup_count() );
    CPPUNIT_ASSERT( 1 == m_chaininfo->train_count() );
}

void TC_ChainInfo::mix_trains()
{
    m_chaininfo->write( 0, CarChain( CarNumber(33330001), 1 ) );
    m_chaininfo->write( 1, CarChain( CarNumber(33330002), 1 ) );
    m_chaininfo->write( 2, CarChain( CarNumber(33330003), 2 ) );
    m_chaininfo->write( 3, CarChain( CarNumber(33330004), 2 ) );
    m_chaininfo->write( 0, CarChain( CarNumber(33330005), 1 ) );
    m_chaininfo->write( 1, CarChain( CarNumber(33330006), 1 ) );
    CPPUNIT_ASSERT( 6 == m_chaininfo->car_count() );
    CPPUNIT_ASSERT( 3 == m_chaininfo->uncoup_count() );
    CPPUNIT_ASSERT( 2 == m_chaininfo->train_count() );
}

void TC_ChainInfo::overwrite()
{
    m_chaininfo->write( 0, CarChain( CarNumber(33330001), 1 ) );
    m_chaininfo->write( 1, CarChain( CarNumber(33330002), 1 ) );
    m_chaininfo->write( 2, CarChain( CarNumber(33330003), 2 ) );
    m_chaininfo->write( 3, CarChain( CarNumber(33330004), 2 ) );
    m_chaininfo->write( 0, CarChain( CarNumber(33330001), 1 ) );
    m_chaininfo->write( 1, CarChain( CarNumber(33330002), 1 ) );
    m_chaininfo->write( 2, CarChain( CarNumber(33330003), 2 ) );
    m_chaininfo->write( 3, CarChain( CarNumber(33330004), 2 ) );
    CPPUNIT_ASSERT( 4 == m_chaininfo->car_count() );
    CPPUNIT_ASSERT( 2 == m_chaininfo->uncoup_count() );
    CPPUNIT_ASSERT( 1 == m_chaininfo->train_count() );
}

void TC_ChainInfo::summary()
{
    m_chaininfo->write( 0, CarChain( CarNumber(30000001), 1 ) );
    m_chaininfo->write( 1, CarChain( CarNumber(30000002), 2 ) );
    m_chaininfo->write( 2, CarChain( CarNumber(30000003), 2 ) );
    m_chaininfo->write( 3, CarChain( CarNumber(30000004), 2 ) );
    RollingStock::Car car;
    car.dba( DB_ADDR(123001) );
    car.number( CarNumber(30000001) );
    car.length( 25 );
    car.gross_weight( 100 );
    car.axis( 4 );
    car.destination( L"77" );
    car.special_marks( L"BLM" );
    CPPUNIT_ASSERT( m_model->put( car ) );
    car.dba( DB_ADDR(123002) );
    car.number( CarNumber(30000002) );
    CPPUNIT_ASSERT( m_model->put( car ) );
    car.dba( DB_ADDR(123003) );
    car.number( CarNumber(30000003) );
    CPPUNIT_ASSERT( m_model->put( car ) );
    car.dba( DB_ADDR(123004) );
    car.number( CarNumber(30000004) );
    CPPUNIT_ASSERT( m_model->put( car ) );

    auto unc_info = m_chaininfo->get_summary( CarNumber(30000003) );
    CPPUNIT_ASSERT( 3 == unc_info.ch_size );
    CPPUNIT_ASSERT( 300 == unc_info.ch_weight );
}

