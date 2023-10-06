#include "stdafx.h"

#include "TC_Carrier.h"
#include "../Hem/Carrier.h"
#include "../Hem/TheCharts.h"
#include "../helpful/Locomotive.h"
#include "../helpful/Crew.h"
#include "../helpful/LocoCharacteristics.h"
#include "../helpful/StrToTime.h"
#include "../SenseOfXml/Outlet.h"
#include "../SenseOfXml/SenseOfXml.h"
#include "../SenseOfXml/SenseOfXml_Hem.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Carrier );

using namespace std;

void TC_Carrier::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_Carrier::test()
{
    time_t moment = 1234567;
    
    LocoId id1( 185, LocoCharacteristics::instance()->GetSeriesCode(L"2ТЭ10У") );
    LocoId id2( 4848,LocoCharacteristics::instance()->GetSeriesCode(L"ЧМЭ3") );

    Carrier  carrier;
    carrier.set_title( 3, L"БАЛТИЯС ТРАНЗИТА СЕРВИСС АО", L"BALTIJAS TRANZĪTA SERVISS AS" );
    carrier.set_title( 4, L"ООО ЛДЗ КАРГО", L"SIA LDZ CARGO" );
    carrier.set_leasing( id1, 3, moment-1000, 0 );
    carrier.set_leasing( id1, 4, moment-100, moment+100 );
    carrier.set_leasing( id2, 3, moment-1000, 0 );
    carrier.set_leasing( id2, 5, moment-100, moment-50 );

    CPPUNIT_ASSERT( carrier.get_company( 4 ).first == L"ООО ЛДЗ КАРГО" );
    CPPUNIT_ASSERT( carrier.get_company( 3 ).first == L"БАЛТИЯС ТРАНЗИТА СЕРВИСС АО" );

    CPPUNIT_ASSERT( carrier.get_code( id1, moment ) == 4 );
    CPPUNIT_ASSERT( carrier.get_code( id1, moment-120 ) == 3 );
    CPPUNIT_ASSERT( carrier.get_code( id1, moment-130 ) == 3 );
    CPPUNIT_ASSERT( carrier.get_code( id2, moment ) == 3 );
    CPPUNIT_ASSERT( carrier.get_code( id2, moment-77 ) == 5 );

    // проверка ввода-вывода 
    attic::a_document doc("test");
    attic::a_node temp = doc.document_element().append_child("xx");
    carrier.serialize( temp );
    Carrier car2;
    car2.deserialize( temp );

    CPPUNIT_ASSERT( carrier == car2 );
    carrier.set_title( 5, L"mismatch", L"lvMismatch" );
    CPPUNIT_ASSERT( !(carrier == car2) );
}

void TC_Carrier::request()
{
    time_t moment = 123456789;

    LocoId id1( 185,  LocoCharacteristics::instance()->GetSeriesCode(L"2ТЭ10У") );
    LocoId id2( 4848, LocoCharacteristics::instance()->GetSeriesCode(L"ЧМЭ3")   );

    Carrier  carrier;
    carrier.set_title( 3, L"БАЛТИЯС ТРАНЗИТА СЕРВИСС АО", L"BALTIJAS TRANZĪTA SERVISS AS" );
    carrier.set_title( 4, L"ООО ЛДЗ КАРГО", L"SIA LDZ CARGO" );
    carrier.set_leasing( id1, 3, moment-1000, 0 );
    carrier.set_leasing( id1, 4, moment-100, moment+100 );
    carrier.set_leasing( id2, 3, moment-1000, 0 );
    carrier.set_leasing( id2, 5, moment-100, moment-50 );

    carrier.request_add( LocoId( 1142, LocoCharacteristics::instance()->GetSeriesCode(L"2ТЭ116") ) );
    carrier.request_add( LocoId( 3423, LocoCharacteristics::instance()->GetSeriesCode(L"2ТЭ10М") ) );

    attic::a_document doc;
    doc.load_wide(
        L"<Carrier time='19731129T213309Z'>"
        L"  <Request locNum='1142' locSer='530'/>"
        L"  <Request locNum='3423' locSer='575'/>"
        L"</Carrier>"
        );
    const std::string etalon = doc.to_str();

    CPPUNIT_ASSERT( carrier.request_exist() );
    std::string ss = carrier.request_string( moment );
    CPPUNIT_ASSERT( etalon == ss );
    carrier.request_clear();
    CPPUNIT_ASSERT( !carrier.request_exist() );
    CPPUNIT_ASSERT( std::string() == carrier.request_string( moment ) );
}

void TC_Carrier::answer()
{
    attic::a_document doc;
    doc.load_wide(
        L"<Carrier>"
        L"  <Answer locNum='0110' locSer='583' locRu='2M62УM' locLv='2M62UM' carrierCode='04' titleRu='КАРГО'   titleLc='CARGO'   time='2017-12-22 00:00:00.000000' timeEnd='' />"
        L"  <Answer locNum='0328' locSer='757' locRu='ДКГУ'   locLv='DGKU'   carrierCode='05' titleRu='ГАО ЛДЗ' titleLc='VAS LDZ' time='2018-03-08 00:00:00.000000' timeEnd='2018-03-29 23:59:00.000000' />"
        L"  <Answer locNum='0328' locSer='757' locRu='ДКГУ'   locLv='DGKU'   carrierCode='77' titleRu='777 ЛДЗ' titleLc='777 LDZ' time='2018-01-01 00:00:00.000000' timeEnd='' />"
        L"</Carrier>"
        );
    SenseOfXml::Command cmd( doc );

    Carrier carrier;
    TheCharts theCharts;
    Outlet outlet;
    OutletConcrete concrete(outlet,Goal::Canopy);
    HemHandler_CarrierAnswer handler( theCharts, carrier, concrete );

    handler.execute(cmd);
    handler.execute(cmd);
    handler.execute(cmd);

    CPPUNIT_ASSERT( L"КАРГО"    == carrier.get_company(4).first );
    CPPUNIT_ASSERT( L"ГАО ЛДЗ" == carrier.get_company(5).first );
    CPPUNIT_ASSERT( L"777 ЛДЗ" == carrier.get_company(77).first );
    time_t momLo  = UTC_from_Str( "2018-03-08 00:00:00.000000",  "%Y-%m-%d %H:%M:%s" );
    time_t momUp  = UTC_from_Str( "2018-03-29 23:59:00.000000",  "%Y-%m-%d %H:%M:%s" );

    LocoId id1( 110, 583 );
    CPPUNIT_ASSERT( 4  == carrier.get_code( id1, momLo-1 ) );
    LocoId id2( 328, 757 );
    CPPUNIT_ASSERT( 77 == carrier.get_code( id2, momLo-1 ) );
    CPPUNIT_ASSERT( 5  == carrier.get_code( id2, momLo ) );
    CPPUNIT_ASSERT( 5  == carrier.get_code( id2, momUp-1 ) );
    CPPUNIT_ASSERT( 77 == carrier.get_code( id2, momUp ) );
}