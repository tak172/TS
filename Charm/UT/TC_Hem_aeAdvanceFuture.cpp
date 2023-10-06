#include "stdafx.h"

#include "TC_Hem_aeAdvanceFuture.h"
#include "UtHemHelper.h"
#include "../Hem/Appliers/aeAdvanceFuture.h"
#include "../helpful/LocoCharacteristics.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeAdvanceFuture );

void TC_Hem_aeAdvanceFuture::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_Hem_aeAdvanceFuture::test()
{
    UtLayer<FutureLayer> fut;
    time_t start=0;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<Test>"
            L"<FutureLayer>"
            L"  <HemPath>"
            L"    <SpotEvent create_time='20171003T000000Z' name='Form' Bdg='NDP[11052]' index='1613-086-1100' num='D2837G' length='58' weight='4970' net_weight='3633' fretran='Y'>"
            L"      <feat_texts typeinfo='ДТ' />"
            L"      <Locomotive Series='2ТЭ10М' NumLoc='2933' Depo='1317' Consec='1'>"
            L"        <Crew EngineDriver='MИШKEBИЧ' TabNum='2767' Tim_Beg='2017-10-03 10:47' />"
            L"      </Locomotive>"
            L"    </SpotEvent>"
            L"    <SpotEvent create_time='20171003T000000Z' name='Arrival' Bdg='NDP[11052]' waynum='3' parknum='1' />"
            L"    <SpotEvent create_time='20171003T001000Z' name='Departure' Bdg='NDP[11052]' waynum='3' parknum='1' />"
            L"    <SpotEvent create_time='20171003T001500Z' name='Transition' Bdg='mock[11050]' waynum='7' parknum='1' />"
            L"    <SpotEvent create_time='20171003T002000Z' name='Transition' Bdg='mock[11051]' waynum='3' parknum='1' />"
            L"    <SpotEvent create_time='20171003T002500Z' name='Transition' Bdg='mock[11363]' />"
            L"    <SpotEvent create_time='20171003T003000Z' name='Transition' Bdg='mock[11010]' waynum='2' parknum='1' />"
            L"    <SpotEvent create_time='20171003T003500Z' name='Arrival' Bdg='mock[11000]' waynum='1' parknum='1' />"
            L"  </HemPath>"
            L"  <HemPath>"
            L"    <SpotEvent create_time='20171003T000700Z' name='Form' Bdg='mock[09715]' index='' num='890' mvps='Y' />"
            L"    <SpotEvent create_time='20171003T000700Z' name='Transition' Bdg='mock[09715]' waynum='2' parknum='1' />"
            L"  </HemPath>"
            L"</FutureLayer>"
            L"</Test>"
            );

        fut.Deserialize( doc.document_element() );
        // настроим начальное время
        doc.document_element().brief_attribute("curr","20171003T000000Z");
        start = doc.document_element().attribute("curr").as_time();
    }
    // краткое задание времени - минутами
    auto moment = [start](int val){
        return start + val*60;
    };

    CPPUNIT_ASSERT( fut.path_count() == 2 );
    CPPUNIT_ASSERT( fut.exist_path_size(8) );
    CPPUNIT_ASSERT( fut.exist_path_size(2) );

    // текущее время дошло до точки 00:05:00
    {
        Hem::aeAdvanceFuture adv( fut.GetWriteAccess(), fut.Bond(), moment(5) );
        adv.Action();
    }
    CPPUNIT_ASSERT( fut.path_count() == 2 );
    CPPUNIT_ASSERT( fut.exist_path_size(6) );
    CPPUNIT_ASSERT( fut.exist_path_size(2) );


    // текущее время дошло до точки 00:09:00
    {
        Hem::aeAdvanceFuture adv( fut.GetWriteAccess(), fut.Bond(), moment(9) );
        adv.Action();
    }
    CPPUNIT_ASSERT( fut.path_count() == 1 );
    CPPUNIT_ASSERT( fut.exist_path_size(6) );
}
