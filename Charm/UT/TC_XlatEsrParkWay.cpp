#include "stdafx.h"

#include "TC_XlatEsrParkWay.h"
#include "../helpful/Attic.h"
#include "../helpful/ParkWayKit.h"
#include "../Hem/XlatEsrParkWay.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_XlatEsrParkWay );

/// \test Загрузка и выгрузка
void TC_XlatEsrParkWay::load_save()
{
    attic::a_document doc;
    doc.load_wide( 
        L"<?xml version='1.0' encoding='utf-8' standalone='yes'?>"
        L"<!-- Таблица соответствия ЕСР, парков и путей в системе Charm и в сообщениях APOVS -->"
        L"<XlatEsrParkWay>"
        L"  <!-- Шкиротава A -->"
        L"  <Station Charm='09000' Apovs='09000' >"
        L"    <Park Charm='3' Apovs='33'>"
        L"      <Way Charm='1' Apovs='1'/>"
        L"      <Way Charm='5' Apovs='5'/>"
        L"      <Way Charm='4' Apovs='4'/>"
        L"    </Park>"
        L"  </Station>"
        L"  <!-- Шкиротава C -->"
        L"  <Station Charm='09008' Apovs='09000' >"
        L"    <Park Charm='1' Apovs='3'>"
        L"      <Way Charm='12' Apovs='12'/>"
        L"      <Way Charm='4' Apovs='4'/>"
        L"    </Park>"
        L"  </Station>"
        L"  <!-- Шкиротава J -->"
        L"  <Station Charm='09006' Apovs='09000' >"
        L"    <Park Charm='0' Apovs='1'>"
        L"      <Way Charm='0' Apovs='1'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
        );

    XlatEsrParkWay table;
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( table.size() == 3+2+1 );

    attic::a_document doc2("test");
    table.save_to( doc2.document_element() );

    XlatEsrParkWay table2;
    table2.load_from( doc2.document_element() );
    CPPUNIT_ASSERT( table2 == table );
}

/// \test Загрузка корректных данных
void TC_XlatEsrParkWay::normal()
{
    attic::a_document doc;
    doc.load_wide( 
        L"<?xml version='1.0' encoding='utf-8' standalone='yes'?>"
        L"<!-- Таблица соответствия ЕСР, парков и путей в системе Charm и в сообщениях APOVS -->"
        L"<XlatEsrParkWay>"
        L"  <!-- Шкиротава A -->"
        L"  <Station Charm='09000' Apovs='09000' >"
        L"    <Park Charm='1' Apovs='1'>"
        L"      <Way Charm='4' Apovs='4'/>"
        L"      <Way Charm='5' Apovs='5'/>"
        L"      <Way Charm='6' Apovs='6'/>"
        L"      <Way Charm='7' Apovs='7'/>"
        L"      <Way Charm='8' Apovs='8'/>"
        L"      <Way Charm='9' Apovs='9'/>"
        L"      <Way Charm='10' Apovs='10'/>"
        L"      <Way Charm='11' Apovs='11'/>"
        L"      <Way Charm='12' Apovs='12'/>"
        L"    </Park>"
        L"  </Station>"
        L"  <!-- Шкиротава C -->"
        L"  <Station Charm='09008' Apovs='09000' >"
        L"    <Park Charm='1' Apovs='3'>"
        L"      <Way Charm='4' Apovs='4'/>"
        L"      <Way Charm='5' Apovs='5'/>"
        L"      <Way Charm='6' Apovs='6'/>"
        L"      <Way Charm='7' Apovs='7'/>"
        L"      <Way Charm='8' Apovs='8'/>"
        L"      <Way Charm='9' Apovs='9'/>"
        L"      <Way Charm='10' Apovs='10'/>"
        L"      <Way Charm='11' Apovs='11'/>"
        L"      <Way Charm='12' Apovs='12'/>"
        L"      <Way Charm='13' Apovs='13'/>"
        L"    </Park>"
        L"  </Station>"
        L"  <!-- Шкиротава J -->"
        L"  <Station Charm='09006' Apovs='09000' >"
        L"    <Park Charm='1' Apovs='4'>"
        L"      <Way Charm='4' Apovs='4'/>"
        L"      <Way Charm='5' Apovs='5'/>"
        L"      <Way Charm='6' Apovs='6'/>"
        L"      <Way Charm='7' Apovs='7'/>"
        L"      <Way Charm='8' Apovs='8'/>"
        L"      <Way Charm='9' Apovs='9'/>"
        L"      <Way Charm='10' Apovs='10'/>"
        L"      <Way Charm='11' Apovs='11'/>"
        L"      <Way Charm='12' Apovs='12'/>"
        L"      <Way Charm='13' Apovs='13'/>"
        L"      <Way Charm='14' Apovs='14'/>"
        L"      <Way Charm='15' Apovs='15'/>"
        L"      <Way Charm='16' Apovs='16'/>"
        L"      <Way Charm='17' Apovs='17'/>"
        L"      <Way Charm='18' Apovs='18'/>"
        L"      <Way Charm='19' Apovs='19'/>"
        L"    </Park>"
        L"    <Park Charm='1' Apovs='5'>"
        L"      <Way Charm='2' Apovs='1'/>"
        L"      <Way Charm='1' Apovs='2'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
        );

    XlatEsrParkWay table;
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( !table.empty() );
    auto ESR_PW = [](int esr, int p, int w){
        return std::make_pair(EsrKit(esr), ParkWayKit(p,w));
    };

    CPPUNIT_ASSERT( table.convertFromApovs( EsrKit(9000), ParkWayKit(1,7) ) == ESR_PW(9000,1,7) );
    CPPUNIT_ASSERT( table.convertFromApovs( EsrKit(9000), ParkWayKit(3,7) ) == ESR_PW(9008,1,7) );
    CPPUNIT_ASSERT( table.convertFromApovs( EsrKit(9000), ParkWayKit(4,7) ) == ESR_PW(9006,1,7) );
    CPPUNIT_ASSERT( table.convertFromApovs( EsrKit(1234), ParkWayKit(6,8) ) == ESR_PW(1234,6,8) );
}

/// \test неверные данные
void TC_XlatEsrParkWay::badValue()
{
    XlatEsrParkWay table;
    attic::a_document doc;

    doc.load_wide( 
        L"<XlatEsrParkWay>"
        L"  <Station Charm='09001' Apovs='xxx' >"
        L"    <Park Charm='1' Apovs='1'>"
        L"      <Way Charm='2' Apovs='2'/>"
        L"    </Park>"
        L"  </Station>"
        L"  <Station Ch__arm='09001' Apovs='12345' >"
        L"    <Park Charm='3' Apovs='3'>"
        L"      <Way Charm='4' Apovs='4'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
        );
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( table.empty() );
    CPPUNIT_ASSERT( table.getErrors().size() == 2 );

    doc.load_wide( 
        L"<XlatEsrParkWay>"
        L"  <Station Charm='09001' Apovs='12345' >"
        L"    <Park Charm='1' Ap__ovs='1'>"
        L"      <Way Charm='4' Apovs='4'/>"
        L"    </Park>"
        L"    <Park Charm='13-18' Apovs='2'>"
        L"      <Way Charm='7' Apovs='7'/>"
        L"    </Park>"
        L"    <Park Ch__arm='3' Apovs='3'>"
        L"      <Way Charm='7' Apovs='7'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
        );
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( table.empty() );
    CPPUNIT_ASSERT( table.getErrors().size() == 3 );

    doc.load_wide( 
        L"<XlatEsrParkWay>"
        L"  <Station Charm='09001' Apovs='12345' >"
        L"    <Park Charm='1' Apovs='1'>"
        L"      <Way Ch_arm='2' Apovs='2'/>"
        L"      <Way Charm='3' Ap_ovs='3'/>"
        L"      <Way Charm='4+7' Apovs='4'/>"
        L"      <Way Charm='5' Apovs='5-67'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
        );
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( table.empty() );
    CPPUNIT_ASSERT( table.getErrors().size() == 4 );
}

/// \test Загрузка при дублировании данных
void TC_XlatEsrParkWay::repeatValue()
{
    XlatEsrParkWay table;
    attic::a_document doc;

    doc.load_wide( 
        L"<XlatEsrParkWay>"
        L"  <Station Charm='09001' Apovs='09001' >"
        L"    <Park Charm='2' Apovs='2'>"
        L"      <Way Charm='25' Apovs='5'/>"
        L"    </Park>"
        L"  </Station>"
        L"  <Station Charm='09001' Apovs='09001' >"
        L"    <Park Charm='2' Apovs='2'>"
        L"      <Way Charm='75' Apovs='5'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
        );
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( !table.empty() );
    CPPUNIT_ASSERT( table.getErrors().size() == 1 );

    doc.load_wide( 
        L"<XlatEsrParkWay>"
        L"  <Station Charm='09001' Apovs='09001' >"
        L"    <Park Charm='2' Apovs='2'>"
        L"      <Way Charm='0' Apovs='0'/>"
        L"    </Park>"
        L"  </Station>"
        L"  <Station Charm='09001' Apovs='09001' >"
        L"    <Park Charm='2' Apovs='2'>"
        L"      <Way Charm='0' Apovs='0'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
        );
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( !table.empty() );
    CPPUNIT_ASSERT( table.getErrors().size() == 1 );
}

/// \test Отсутствие данных
void TC_XlatEsrParkWay::empty()
{
    XlatEsrParkWay table;
    attic::a_document doc;
    // сначала заполним некими данными
    doc.load_wide( 
        L"<XlatEsrParkWay>"
        L"  <Station Charm='09001' Apovs='09001' >"
        L"    <Park Charm='2' Apovs='2'>"
        L"      <Way Charm='5' Apovs='5'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
        );
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( !table.empty() );
    // теперь загрузим пустышку
    doc.load_wide( 
        L"<XlatEsrParkWay>"
        L"      <Nothing Charm='0' Apovs='0'/>"
        L"</XlatEsrParkWay>"
        );
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( table.empty() );
    CPPUNIT_ASSERT( table.getErrors().empty() );
}

void TC_XlatEsrParkWay::merge()
{
    attic::a_document doc;
    doc.load_wide(
        L"<?xml version='1.0' encoding='utf-8' standalone='yes'?>"
        L"<!-- Таблица соответствия ЕСР, парков и путей в системе Charm и в сообщениях APOVS -->"
        L"<XlatEsrParkWay>"
        L"  <!-- Подмена парка 22 на 11 для всех путей и единственного пути в парке 44 -->"
        L"  <Station Charm='1111' Apovs='2222' >"
        L"    <Park Charm='11' Apovs='22'>"
        L"      <Way Charm='0' Apovs='0'/>"
        L"    </Park>"
        L"    <Park Charm='33' Apovs='44'>"
        L"      <Way Charm='3' Apovs='4'/>"
        L"    </Park>"
        L"  </Station>"
        L"  <!-- подмена кода всей станции  -->"
        L"  <Station Charm='3333' Apovs='4444' >"
        L"    <Park Charm='0' Apovs='0'>"
        L"      <Way Charm='0' Apovs='0'/>"
        L"    </Park>"
        L"  </Station>"
        L"</XlatEsrParkWay>"
    );

    XlatEsrParkWay table;
    table.load_from( doc.document_element() );
    CPPUNIT_ASSERT( !table.empty() );
    auto EPW = []( int esr, int p, int w ) {
        return std::make_pair( EsrKit( esr ), ParkWayKit( p, w ) );
    };

    // весь парк
    CPPUNIT_ASSERT( EPW( 1111, 11, 5 ) == table.convertFromApovs( EsrKit( 2222 ), ParkWayKit( 22, 5 ) ) );
    CPPUNIT_ASSERT( EPW( 1111, 11, 9 ) == table.convertFromApovs( EsrKit( 2222 ), ParkWayKit( 22, 9 ) ) );
    // только один путь в парке
    CPPUNIT_ASSERT( EPW( 1111, 33, 3 ) == table.convertFromApovs( EsrKit( 2222 ), ParkWayKit( 44, 4 ) ) );
    CPPUNIT_ASSERT( EPW( 2222, 44, 7 ) == table.convertFromApovs( EsrKit( 2222 ), ParkWayKit( 44, 7 ) ) );
    // вся станция
    CPPUNIT_ASSERT( EPW( 3333, 5, 7 ) == table.convertFromApovs( EsrKit( 4444 ), ParkWayKit( 5, 7 ) ) );
}
