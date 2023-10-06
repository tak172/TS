#include "stdafx.h"

#include "TC_GidUraler.h"
#include "../ut/Parody.h"
#include "../helpful/Log.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GidUraler );

using namespace Parody;

void TC_GidUraler::search_single()
{
    // создаем пару объектов
    EsrKit e(54321);
    Elements temp( e, STRIP, " aa bb " );
    Etui::instance()->complete();
    // 
    BadgeE bdgA(L"aa",e);
    BadgeE bdgB(L"bb",e);
    size_t posA = 2;
    size_t posB = 3;
    bool busyA = false;
    bool busyB = false;

    // назначаем ural_name для этих объектов
    GIDURALER giduraler;
    giduraler.insert( L"1 alfa", bdgA, posA );
    giduraler.insert( L"2 beta", bdgB, posB );

    // проверяем поиск по ural_name
    GIDURALER::VEC_BnSB info;
    info = giduraler.conform( L"1 alfa" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( info.front() == std::make_tuple(bdgA,posA,busyA) );

    info = giduraler.conform( L"2 beta" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( info.front() == std::make_tuple(bdgB,posB,busyB) );

    info = giduraler.conform( L"3 gamma" );
    CPPUNIT_ASSERT( info.empty() );
}

void TC_GidUraler::search_multi()
{
    // создаем объекты
    EsrKit e(22000);
    Elements temp( e, STRIP, " xx yy zz " );
    Etui::instance()->complete();
    // 
    BadgeE bdgX(L"xx",e);
    BadgeE bdgY(L"yy",e);
    BadgeE bdgZ(L"zz",e);

    // назначаем ural_name для этих объектов
    GIDURALER giduraler;
    giduraler.insert_separated_list( L"10 НПУ1,33 везде",         bdgX );
    giduraler.insert_separated_list( L"22 1пА, 33 везде, 22 1пБ", bdgY );
    giduraler.insert(                        L"33 везде",         bdgZ, 77 );
    giduraler.sort_holded_data();

    // проверяем поиск по ural_name
    GIDURALER::VEC_BnSB info;
    // одиночные результаты
    info = giduraler.conform( L"10 НПУ1" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<0>(info.front()) == bdgX );

    info = giduraler.conform( L"22 1пА" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<0>(info.front()) == bdgY );

    info = giduraler.conform( L"22 1пБ" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<0>(info.front()) == bdgY );

    // групповой результат
    info = giduraler.conform( L"33 везде" );
    CPPUNIT_ASSERT( info.size() == 3 );
    CPPUNIT_ASSERT( get<0>(info[0]) == bdgX );
    CPPUNIT_ASSERT( get<0>(info[1]) == bdgY );
    CPPUNIT_ASSERT( get<0>(info[2]) == bdgZ );
}

void TC_GidUraler::busy()
{
    // создаем объекты
    EsrKit e1(1000);
    EsrKit e2(2000);
    Elements temp1( e1, STRIP, " aa " );
    Elements temp2( e2, STRIP, " bb " );
    Etui::instance()->complete();
    // 
    BadgeE bdgA(L"aa",e1);
    BadgeE bdgB(L"bb",e2);

    // назначаем ural_name для этих объектов
    GIDURALER giduraler;
    giduraler.insert( L"1 alfa", bdgA, 0 );
    giduraler.insert( L"2 beta", bdgB, 13 );
    giduraler.insert( L"3 mix",  bdgA, 11 );
    giduraler.insert( L"3 mix",  bdgB, 22 );
    giduraler.sort_holded_data();

    GIDURALER::VEC_BnSB info;
    attic::a_document doc;
    // исходное состояние - занятость не генерируется
    doc.load_utf8(
        "<Option />"
        );
    giduraler.control_occupation( doc.document_element() );
    info = giduraler.conform( L"1 alfa" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<2>(info.front()) == false );
    info = giduraler.conform( L"2 beta" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<2>(info.front()) == false );
    info = giduraler.conform( L"3 mix" );
    CPPUNIT_ASSERT( info.size()  == 2 );
    CPPUNIT_ASSERT( get<2>(info[0]) == false );
    CPPUNIT_ASSERT( get<2>(info[1]) == false );

    // занятость генерировать всегда и везде
    doc.load_utf8(
        "<Option BusyFromGUral='Y' />"
        );
    giduraler.control_occupation( doc.document_element() );
    info = giduraler.conform( L"1 alfa" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<2>(info.front()) == true );
    info = giduraler.conform( L"2 beta" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<2>(info.front()) == true );
    info = giduraler.conform( L"3 mix" );
    CPPUNIT_ASSERT( info.size()  == 2 );
    CPPUNIT_ASSERT( get<2>(info[0]) == true );
    CPPUNIT_ASSERT( get<2>(info[1]) == true );

    // занятость генерировать только для 1000
    doc.load_wide( 
        L"<Test>"
        L"  <Option BusyFromGUral='Y' >"
        L"    <Include ESR_code='1000'/>" 
        L"  </Option>"
        L"</Test>"
        );
    giduraler.control_occupation( doc.document_element().child("Option") );
    info = giduraler.conform( L"1 alfa" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<2>(info.front()) == true );
    info = giduraler.conform( L"2 beta" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<2>(info.front()) == false );
    info = giduraler.conform( L"3 mix" );
    CPPUNIT_ASSERT( info.size()  == 2 );
    CPPUNIT_ASSERT( get<2>(info[0]) == true );
    CPPUNIT_ASSERT( get<2>(info[1]) == false );

    // занятость генерировать везде, кроме 1000
    doc.load_wide( 
        L"<Test>"
        L"  <SubTest>"
        L"    <Option BusyFromGUral='Y' >"
        L"      <Exclude ESR_code='1000'/>" 
        L"    </Option>"
        L"  </SubTest>"
        L"</Test>"
        );
    giduraler.control_occupation( doc.document_element().child("SubTest").child("Option") );
    info = giduraler.conform( L"1 alfa" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<2>(info.front()) == false );
    info = giduraler.conform( L"2 beta" );
    CPPUNIT_ASSERT( info.size()  == 1 );
    CPPUNIT_ASSERT( get<2>(info.front()) == true );
    info = giduraler.conform( L"3 mix" );
    CPPUNIT_ASSERT( info.size()  == 2 );
    CPPUNIT_ASSERT( get<2>(info[0]) == false );
    CPPUNIT_ASSERT( get<2>(info[1]) == true );
}

void TC_GidUraler::force_on_esr()
{
    // заполнить уралер (данные не важны)
    GIDURALER giduraler;
    giduraler.insert( L"1 alfa", BadgeE(L"xyz", EsrKit(76543) ), 0 );
    giduraler.sort_holded_data();

    attic::a_document doc;
    // исходное состояние - занятость не генерируется
    doc.load_utf8(
        "<Option />"
        );
    giduraler.control_occupation( doc.document_element() );
    CPPUNIT_ASSERT( !giduraler.force_occupation( EsrKit(12345) ) );

    // занятость генерировать всегда и везде
    doc.load_utf8(
        "<Option BusyFromGUral='Y' />"
        );
    giduraler.control_occupation( doc.document_element() );
    CPPUNIT_ASSERT( giduraler.force_occupation( EsrKit(12345) ) );

    // занятость генерировать только для 1000
    doc.load_wide( 
        L"<Test>"
        L"  <Option BusyFromGUral='Y' >"
        L"    <Include ESR_code='1000'/>" 
        L"  </Option>"
        L"</Test>"
        );
    giduraler.control_occupation( doc.document_element().child("Option") );
    CPPUNIT_ASSERT( giduraler.force_occupation( EsrKit(1000) ) );
    CPPUNIT_ASSERT( !giduraler.force_occupation( EsrKit(1001) ) );
    CPPUNIT_ASSERT( !giduraler.force_occupation( EsrKit(1000,2000) ) );

    // занятость генерировать везде, кроме 1000
    doc.load_wide( 
        L"<Test>"
        L"  <SubTest>"
        L"    <Option BusyFromGUral='Y' >"
        L"      <Exclude ESR_code='1000'/>" 
        L"    </Option>"
        L"  </SubTest>"
        L"</Test>"
        );
    giduraler.control_occupation( doc.document_element().child("SubTest").child("Option") );
    CPPUNIT_ASSERT( !giduraler.force_occupation( EsrKit(1000) ) );
    CPPUNIT_ASSERT( giduraler.force_occupation( EsrKit(1001) ) );
    CPPUNIT_ASSERT( giduraler.force_occupation( EsrKit(1000,2000) ) );
}

void TC_GidUraler::setUp()
{
}

void TC_GidUraler::tearDown()
{
    Etui::Shutdowner();
}
