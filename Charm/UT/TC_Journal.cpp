#include "stdafx.h"

#include "TC_Journal.h"
#include "../helpful/Journal.h"
#include "../helpful/JournalGuide.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Journal );

void TC_Journal::test()
{
    // проверка ввода-вывода 
    attic::a_document doc;
    doc.load_wide(
        L"<JournalList>"
        L"  <!--ветвление и до и после-->"
        L"  <Journal code='jrn01' title='Все ВологдЫ'>"
        L"    <!--подход от Молочная на Вологда-2 -->"
        L"    <Approach esr='30162' />"
        L"    <!-- подход от БП 499км на Вологда-2 -->"
        L"    <Approach esr='30048' />"
        L"    <!-- целевая: Вологда2 name специально указан, иначе название берется из OEC -->"
        L"    <TargetStation esr='30030' name='Во-о-о-о-логда-2'/>"
        L"    <!-- целевая: Вологда1 по которой сортировка -->"
        L"    <TargetStation esr='30010' sort='Y' />"
        L"    <!-- подход от Лоcта к Вологда1 -->"
        L"    <Approach esr='30000' />"
        L"    <!-- подход от Паприха (через Лоста?) к Вологда1 -->"
        L"    <Approach esr='30325' />"
        //         L"    <!-- подход от внутреннего депо Вологда1 -->"
        //         L"    <Approach internal='Y' name='депо'/>"
        L"  </Journal>"
        L"  <!--линейный-->"
        L"  <Journal code='jrn02' title='Вологда1-Лоста-Паприха'>"
        L"    <!-- подход от Вологда-2 -->"
        L"    <Approach esr='30030' />"
        L"    <!-- целевая: Вологда1 по которой сортировка -->"
        L"    <TargetStation esr='30010' sort='Y' />"
        L"    <!-- целевая: Лоста -->"
        L"    <TargetStation esr='30000' />"
        L"    <!-- целевая: Паприха -->"
        L"    <TargetStation esr='30325' />"
        L"    <!-- подход от Грязовец -->"
        L"    <Approach esr='30048' />"
        L"  </Journal>"
        L"  <!--линейный-->"
        L"  <Journal code='jrnRyb' title='Рыбинск (тов и пасс)'>"
        L"    <!-- подход от Торопово -->"
        L"    <Approach esr='31231' />"
        L"    <!-- целевая: Рыбинск тов. -->"
        L"    <TargetStation  esr='31230' />"
        L"    <!-- целевая: Рыбинск пасс. -->"
        L"    <TargetStation  esr='31210' sort='Y' />"
        L"    <!-- подход от Юринский -->"
        L"    <Approach esr='31202' />"
        L"  </Journal>"
        L"</JournalList>"
    );

    JournalGuide jg;
    CPPUNIT_ASSERT( jg.empty() );
    jg.deserialize( doc.document_element() );

    auto codes = jg.get_codes();
    CPPUNIT_ASSERT( 3 == codes.size() );
    CPPUNIT_ASSERT( 1 == std::count( codes.begin(), codes.end(), L"jrn01" ) );
    CPPUNIT_ASSERT( 1 == std::count( codes.begin(), codes.end(), L"jrn02" ) );
    CPPUNIT_ASSERT( 1 == std::count( codes.begin(), codes.end(), L"jrnRyb" ) );

    const Journal& jrn = jg.get( L"jrn01" );
    CPPUNIT_ASSERT( 2 == jrn.left_approach_size() );
    CPPUNIT_ASSERT( jrn.in_left_approach( StationEsr(30162) ) );
    CPPUNIT_ASSERT( jrn.in_left_approach( StationEsr(30048) ) );
    CPPUNIT_ASSERT( 0 == jrn.target_position( StationEsr(30030) ) );
    CPPUNIT_ASSERT( 1 == jrn.target_position( StationEsr(30010) ) );
    CPPUNIT_ASSERT( jrn.sortBy() == EsrKit(30010) );
    CPPUNIT_ASSERT( 2 == jrn.right_approach_size() );
    CPPUNIT_ASSERT( jrn.in_right_approach( StationEsr(30000) ) );
    CPPUNIT_ASSERT( jrn.in_right_approach( StationEsr(30325) ) );
}
