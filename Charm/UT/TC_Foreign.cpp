#include "stdafx.h"
#include <boost/filesystem.hpp>
#include "TC_Foreign.h"
#include "UtReadResource.h"
#include "../helpful/FilesByMask.h"
#include "../helpful/FotoFinish.h"
#include "../helpful/StrToTime.h"
#include "../helpful/PicketingInfo.h"
#include "../Fund/UralParser/StripShot.h"
#include "../Fund/ExcerptWarning.h"
#include "../Fund/ExcerptWindow.h"
#include "../Fund/ExcerptTricolor.h"
#include "../Fund/SupplyTrainInfo.h"
#include "../Fund/Foreign_Utils.h"
#include "../Fund/BisK.h"
#include "../Fund/RegulatoryRecode/RegRecode_Schedule.h"
#include "../Fund/RegulatoryRecode/RegRecode_Gathering.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Foreign );

void TC_Foreign::setUp()
{
    attic::a_document doc;
    doc.load_wide(
        L"<EsrGuide>"
        L"  <EsrGuide kit='09400' name='Lielvarde' picketing1_val='51~400' picketing1_comm='Рига-Зилупе'>"
        L"    <Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='3' parknum='1' />"
        L"    <Way waynum='5' parknum='1' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09410' name='Ogre' picketing1_val='34~400' picketing1_comm='Рига-Зилупе'>"
        L"    <Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='2' parknum='1' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='3' parknum='1' major='Y' />"
        L"    <Way waynum='4' parknum='1' />"
        L"    <Way waynum='5' parknum='1' />"
        L"    <Way waynum='12' parknum='1' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09600' name='Saulkrasti' picketing1_val='48~400' picketing1_comm='Рига-Скулте'>"
        L"    <Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='3' parknum='1' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='4' parknum='1' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09604' name='Inčupe' picketing1_val='43~400' picketing1_comm='Рига-Скулте' />"
        L"  <EsrGuide kit='09603' name='Lilaste' picketing1_val='37~128' picketing1_comm='Рига-Скулте'>"
        L"    <Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='3' parknum='1' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09750' name='Tukums-II' picketing1_val='108~300' picketing1_comm='Вентспилс' picketing2_val='64~400' picketing2_comm='Рига-Тукумс'>"
        L"    <Way waynum='1' parknum='1' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='3' parknum='1' major='Y' />"
        L"    <Way waynum='4' parknum='1' />"
        L"    <Way waynum='5' parknum='1' />"
        L"    <Way waynum='6' parknum='1' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09772' name='Slampe' picketing1_val='125~700' picketing1_comm='Вентспилс'>"
        L"    <Way waynum='1' parknum='1' major='Y' />"
        L"    <Way waynum='2' parknum='1' />"
        L"    <Way waynum='3' parknum='1' passenger='Y' embarkation='Y' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='11000' name='Гадюкино' picketing1_val='415~200' picketing1_comm='Москва-Посинь' />"
        L"  <EsrGuide kit='11401' name='Mezare' picketing1_val='154~400' picketing1_comm='Рига-Зилупе'>"
        L"    <Way waynum='1' parknum='1' major='Y' />"
        L"    <Way waynum='3' parknum='1' passenger='Y' embarkation='Y' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='11412' name='Kukas' picketing1_val='142~400' picketing1_comm='Рига-Зилупе'>"
        L"    <Way waynum='1' parknum='1' major='Y' />"
        L"    <Way waynum='3' parknum='1' />"
        L"    <Way waynum='5' parknum='1' passenger='Y' embarkation='Y' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09100' name='Torņakalns' picketing1_val='2~700' picketing1_comm='Рига' picketing2_val='2~700' picketing2_comm='Рига-Тукумс' picketing3_val='0~1' picketing3_comm='Торнякалнс' picketing4_val='2~700' picketing4_comm='Торнянкалнс-Елгава'>"
        L"    <Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='3' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='4' parknum='1' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='5' parknum='1' />"
        L"    <Way waynum='6' parknum='1' />"
        L"    <Way waynum='22' parknum='1' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09104' name='8 km' picketing1_val='7~200' picketing1_comm='Рига' picketing2_val='7~200' picketing2_comm='Торнянкалнс-Елгава'>"
        L"    <Way waynum='1' parknum='1' />"
        L"    <Way waynum='2' parknum='1' major='Y' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09150' name='Olaine' picketing1_val='22~' picketing1_comm='Рига' picketing2_val='22~' picketing2_comm='Торнянкалнс-Елгава'>"
        L"    <Way waynum='1' parknum='1' />"
        L"    <Way waynum='2' parknum='1' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='3' parknum='1' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='4' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='5' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='8' parknum='1' />"
        L"    <Way waynum='9' parknum='1' />"
        L"  </EsrGuide>"
        L"  <!-- перегон Torņakalns - 8 km -->"
        L"  <SpanGuide kit='09100:09104' orientation='odd'>"
        L"    <Way wayNum='1'>"
        L"      <rwcoord picketing1_val='3~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"      <rwcoord picketing1_val='5~750' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    </Way>"
        L"    <Way wayNum='2'>"
        L"      <rwcoord picketing1_val='3~200' picketing1_comm='Торнянкалнс-Елгава' />"
        L"      <rwcoord picketing1_val='6~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    </Way>"
        L"  </SpanGuide>"
        L"  <!-- перегон 8 km - Olaine -->"
        L"  <SpanGuide kit='09104:09150' orientation='odd'>"
        L"    <Way wayNum='1'>"
        L"      <rwcoord picketing1_val='8~800' picketing1_comm='Торнянкалнс-Елгава' />"
        L"      <rwcoord picketing1_val='21~' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    </Way>"
        L"    <Way wayNum='2'>"
        L"      <rwcoord picketing1_val='7~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"      <rwcoord picketing1_val='21~' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    </Way>"
        L"  </SpanGuide>"
        L"  <!-- перегон Saulkrasti - Inčupe -->"
        L"  <SpanGuide kit='09600:09604' orientation='odd'>"
        L"    <Way wayNum='1'>"
        L"      <rwcoord picketing1_val='47~642' picketing1_comm='Рига-Скулте' />"
        L"      <rwcoord picketing1_val='43~900' picketing1_comm='Рига-Скулте' />"
        L"    </Way>"
        L"    <Way wayNum='2'>"
        L"      <rwcoord picketing1_val='47~642' picketing1_comm='Рига-Скулте' />"
        L"      <rwcoord picketing1_val='43~900' picketing1_comm='Рига-Скулте' />"
        L"    </Way>"
        L"  </SpanGuide>"
        L"  <!-- перегон Inčupe - Lilaste -->"
        L"  <SpanGuide kit='09604:09603' orientation='odd'>"
        L"    <Way wayNum='1'>"
        L"      <rwcoord picketing1_val='42~145' picketing1_comm='Рига-Скулте' />"
        L"      <rwcoord picketing1_val='37~737' picketing1_comm='Рига-Скулте' />"
        L"    </Way>"
        L"  </SpanGuide>"
        L"  <!-- перегон Tukums-II - Slampe -->"
        L"  <SpanGuide kit='09750:09772' orientation='even'>"
        L"    <Way wayNum='1'>"
        L"      <rwcoord picketing1_val='109~188' picketing1_comm='Вентспилс' />"
        L"      <rwcoord picketing1_val='124~975' picketing1_comm='Вентспилс' />"
        L"    </Way>"
        L"  </SpanGuide>"
        L"  <!-- перегон Mezare - Kukas -->"
        L"  <SpanGuide kit='11401:11412' orientation='odd'>"
        L"    <Way wayNum='1'>"
        L"      <rwcoord picketing1_val='153~716' picketing1_comm='Рига-Зилупе' />"
        L"      <rwcoord picketing1_val='143~228' picketing1_comm='Рига-Зилупе' />"
        L"    </Way>"
        L"  </SpanGuide>"
        L"  <!-- перегон Lielvarde - Ogre -->"
        L"  <SpanGuide kit='09400:09410' orientation='odd'>"
        L"    <Way wayNum='1'>"
        L"      <rwcoord picketing1_val='51~' picketing1_comm='Рига-Зилупе' />"
        L"      <rwcoord picketing1_val='35~' picketing1_comm='Рига-Зилупе' />"
        L"    </Way>"
        L"    <Way wayNum='2'>"
        L"      <rwcoord picketing1_val='51~' picketing1_comm='Рига-Зилупе' />"
        L"      <rwcoord picketing1_val='35~' picketing1_comm='Рига-Зилупе' />"
        L"    </Way>"
        L"  </SpanGuide>"
        L"  <EsrGuide kit='09290' name='Dobele' picketing1_val='72~96' picketing1_comm='Лиепая'>"
        L"    <Way waynum='1' parknum='1' passenger='Y' />"
        L"    <Way waynum='2' parknum='1' major='Y' />"
        L"    <Way waynum='3' parknum='1' />"
        L"    <Way waynum='4' parknum='1' />"
        L"    <Way waynum='6' parknum='1' />"
        L"  </EsrGuide>"
        L"  <EsrGuide kit='09303' name='Biksti' picketing1_val='93~152' picketing1_comm='Лиепая'>"
        L"    <Way waynum='1' parknum='1' major='Y' passenger='Y' />"
        L"    <Way waynum='2' parknum='1' passenger='Y' embarkation='Y' />"
        L"    <Way waynum='3' parknum='1' />"
        L"  </EsrGuide>"
        L"  <!-- перегон Dobele - Biksti -->"
        L"  <SpanGuide kit='09290:09303' orientation='odd'>"
        L"      <Way wayNum='1'>"
        L"          <rwcoord picketing1_val='73~366' picketing1_comm='Лиепая' />"
        L"          <rwcoord picketing1_val='92~349' picketing1_comm='Лиепая' />"
        L"      </Way>"
        L"  </SpanGuide>"
        L"</EsrGuide>"
        );
    esrGuide.load_from( doc.document_element() );

//     esrGuide.setNameAndAxis( EsrKit(11000), L"Гадюкино", PicketingInfo() );

}

void TC_Foreign::tearDown()
{}

void TC_Foreign::detectBisk()
{
    const char * bisks[] =
    {
        "<?xml version='1.0' encoding='Windows-1251'?>"
        "<zka>"
        "  <To>0</To>"
        "  <Iz>2509201609251657298280</Iz>"
        "  <Nz>27505</Nz>"
        "  <Ds>201710021631</Ds>"
        "  <OtStanz>110709</OtStanz>"
        "  <Dop>чётная и; нечётная горловина</Dop>"
        "  <Har>Обеспечить особую бдительность и частую подачу оповестительных сигналов. Путевая бригада с электроинструментами.</Har>"
        "  <SDT>201609260800</SDT>"
        "  <DoDT>201609261700</DoDT>"
        "  <TMP>4</TMP>"
        "</zka>"
    };

    for( auto text : bisks )
    {
        attic::a_document doc;
        doc.load_wide( From1251(text) );

        CPPUNIT_ASSERT( Foreign::AsyncBisk::detect(doc) );
    }
}

void TC_Foreign::detectNordRW_GU()
{
    const char * sample =
        "<?xml version='1.0' encoding='windows-1251'?>"
        "<NormTrains>"
        "  <NTrain ID='1027 [31002-31000]' MT='08.12.2021 11:24:53'>"
        "    <Thread K='6' S='5' T='ДЕПО(31002) - ЯРОСЛАВЛЬ-ГЛ(31000); elbrus-eid:492494244' MT='08.12.2021 11:24:53'>"
        "      <K MT='09.12.2021 04:53:00' />"
        "      <K y='2021' m='12' d='00000000100000000000000000000000' />"
        "      <Body>"
        "        <Rp Esr='31002' Nm='ДЕПО' N='1027' Pr='8888' Ot='0753' PW='1/1' />"
        "        <Rp Esr='31000' Nm='ЯРОСЛАВЛЬ-ГЛ' Pr='0758' Ot='8888' PW='11/1' />"
        "      </Body>"
        "    </Thread>"
        "  </NTrain>"
        "  <NTrain ID='1027 [31002-31000]' MT='08.12.2021 11:24:53'>"
        "    <Thread K='6' S='5' T='ДЕПО(31002) - ЯРОСЛАВЛЬ-ГЛ(31000); elbrus-eid:492520998' MT='08.12.2021 11:24:53'>"
        "      <K MT='09.12.2021 04:53:00' />"
        "      <K y='2021' m='12' d='00000000100000000000000000000000' />"
        "      <Body>"
        "        <Rp Esr='31002' Nm='ДЕПО' N='1027' Pr='8888' Ot='0753' PW='1/1' />"
        "        <Rp Esr='31000' Nm='ЯРОСЛАВЛЬ-ГЛ' Pr='0758' Ot='8888' PW='11/1' />"
        "      </Body>"
        "    </Thread>"
        "  </NTrain>"
        "</NormTrains>"
        ;

    attic::a_document doc;
    CPPUNIT_ASSERT( doc.load_wide( From1251(sample) ) );
    CPPUNIT_ASSERT( Foreign::AsyncNordRW_GU::detect(doc) );

    auto interest = []( const StationEsr /*esr*/ ){
        return true;
    };
    RegulatoryRecode::Schedule schedule;
    CPPUNIT_ASSERT( schedule.empty() );
    schedule.read_elbrus( doc, interest );
    CPPUNIT_ASSERT( !schedule.empty() );
    attic::a_document charm_doc;
    schedule.write_charm( charm_doc, time(nullptr) );
}

void TC_Foreign::detectSamara()
{
    const char * samaras[] =
    {
        "<Message Id='1504' CaclDate='26.06.2012 16:33:15' TypeMessage='6305' >"
        "    <Record  TypeMessage='13' UchKod='65940' ObjectId='Н2У' ObjectState='2' Text1='' Text2='Постоянное ограничение скорости 80' BegDateTime='25.06.2012 10:00:00' EndDateTime='' EventId='237074463' Text3='80' />"
        "    <Record  TypeMessage='13' UchKod='65150' ObjectId='пер10' ObjectState='1' Text1='' Text2='Ограничение скорости 70' BegDateTime='26.06.2012 10:00:00' EndDateTime='26.06.2012 18:00:00' EventId='237037963' Text3='70' />"
        "</Message>"
    };

    for( auto text : samaras )
    {
        attic::a_document doc;
        doc.load_wide( From1251(text) );
        CPPUNIT_ASSERT( Foreign::AsyncSamara::detect(doc) );
    }
}

// if ( attic::a_node info = xdoc.child('Info') )
// {
//     // Новосибирск по наличию атрибутов и/или тегов
//         info.child('gidural..svetofor') ||
// }
void TC_Foreign::detectNovosib()
{
    const char * novosibs[] =
    {
        //
        "<AnWeightLen xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xmlns:xsd='http://www.w3.org/2001/XMLSchema'>"
        "    <Period tBeg='16.03.2017 04:01:00' tEnd='16.03.2017 13:00:00' />"
        "    <RU_Result>"
        "        <RU num='83' name='Дорога'>"
        "            <Weight Norm='4330' Fact='4289' Color='2' />"
        "            <Vuch Norm='58' Fact='56,7' Color='3' />"
        "            <V_route Norm='6840' Fact='1017,1' Color='3' />"
        "            <Proc_graf_gr_otpr Norm='100' Fact='95' Color='2' />"
        "            <Proc_graf_gr_prosl Norm='100' Fact='62' Color='1' />"
        "        </RU>"
        "    </RU_Result>"
        "    <DU_Result>"
        "        <DU num='101' name='Исилькуль-Московка'>"
        "            <Weight Norm='4450' Fact='4393' Color='2' />"
        "            <Vuch Norm='58,5' Fact='62,1' Color='3' />"
        "            <V_route Norm='696' Fact='961,4' Color='3' />"
        "            <Proc_graf_gr_prosl Norm='100' Fact='69' Color='1' />"
        "            <Proc_graf_gr_otpr Norm='100' Fact='100' Color='3' />"
        "        </DU>"
        "    </DU_Result>"
        "</AnWeightLen>"
        ,
        // station performance
        "<Info xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xmlns:xsd='http://www.w3.org/2001/XMLSchema'>"
        "    <station_performance station='830003'>"
        "        <unloading plan='0' fact='0' />"
        "        <loading plan='0' fact='1' />"
        "        <working_park plan='1329' fact='1411' />"
        "        <formed plan='17' fact='6' />"
        "        <formed1 plan='0' fact='6' />"
        "        <formed2 plan='0' fact='0' />"
        "        <disbanded plan='0' fact='7' />"
        "        <disbanded1 plan='1192' fact='358' />"
        "        <disbanded2 plan='0' fact='0' />"
        "        <downtime plan='0' fact='992' />"
        "    </station_performance>"
        "</Info>"
        ,
        // trains
        "<Ext.Train Time='2015-05-29 12:00'>"
        "  <Strip ID='85440 0П' />"
        "</Ext.Train>"
        ,
        // failure
        "<Ext.Failure Time='2015-05-15 03:40'>"
        "  <Failure ID='233163708300050' Place='83000'>"
        "  <Strip ID='83000 2П' />"
        "</Failure>"
        ,
        // passing trains
        "<Info Time='1900-01-01 00:00' >"
        "  <passing_trains Dor='83'>"
        "    <station ESR='83200' Fact='15' Plan='23' Plan_TP='21' Paln_CD='23' Plan_otch='20' Plan_TP_otch='21' Paln_CD_otch='24' Fact_otch='20'>"
        "      <passing TrainNum='3141' Idx='3100+001+0500' time='16:45' />"
        "    </station>"
        "  </passing_trains>"
        "</Info>"
        ,
        // received trains
        "<Info Time='1900-01-01 00:00'>"
        "    <receive_trains Dor='83'>"
        "        <station ESR='83200' Fact='14' Plan='23' Plan_TP='21' Paln_CD='23' Plan_otch='20' Plan_TP_otch='21' Paln_CD_otch='24' Fact_otch='19'>"
        "            <receive TrainNum='3811' Idx='0300+477+2300' time='15:43'/>"
        "            <receive TrainNum='3815' Idx='0820+041+7973' time='15:43'/>"
        "        </station>"
        "    </receive_trains>"
        "</Info>"
        ,
        // local work
        "<Lc.work Time='2015-06-03 08:07'>"
        "    <local_work ESR='830107' long_wait_load='0' long_wait_unload='82'/>"
        "    <local_work ESR='830709' long_wait_load='0' long_wait_unload='11'/>"
        "    <local_work ESR='831504' long_wait_load='83' long_wait_unload='519'/>"
        "    <local_work ESR='876007' long_wait_load='3' long_wait_unload='0'/>"
        "</Lc.work>"
        ,
        // сообщение о показателях работы участка
        "<Info Time='2015-03-25 10:01'>"
        "    <performance_dispatching_area Dor='83' area='02'>"
        "        <service_speed plan='43' quest='44' fulfilment='27' quality='2'/>"
        "        <weight_train plan='5200' quest='5250' fulfilment='5211' quality='2'/>"
        "        <timeline_passenger_trains plan='99' quest='100' fulfilment='100' quality='2'/>"
        "        <timeline_freight_trains plan='80' quest='83' fulfilment='73' quality='2'/>"
        "    </performance_dispatching_area>"
        "</Info>"
    };

    for( auto text : novosibs )
    {
        attic::a_document doc;
        doc.load_wide( From1251(text) );
        CPPUNIT_ASSERT( Foreign::AsyncNovosib::detect(doc) );
    }
}

void TC_Foreign::detectNordRW()
{
    const char * nords[] =
    {
        // warning
        "<Info>"
        "    <Warn Id='1180340' Stan='282002' KmPk_Beg='1266' KmPk_End='1266' Tim_Beg='2007-02-25 00:00' >"
        "        <Object Napr='0' />"
        "        <Speed />"
        "        <Comment>-1264,OДH/УЧ</Comment>"
        "    </Warn>"
        "</Info>"
        ,
        // window
        "<Info>"
        "    <Wind Id='1518640' Tim_Beg='2016-12-04 11:00' Tim_End='2016-12-04 15:00' Fact_Beg='2016-12-04 11:00' Fact_End='2016-12-04 15:00' >"
        "        <Comment>шч выключить 5К из централизации    МРК</Comment>"
        "        <Object Place='310005'  />"
        "        <Object Place='310005'  />"
        "        <Sl_Name>Служба ПЧ-3   Работа ПРСМ</Sl_Name>"
        "    </Wind>"
        "</Info>"
        ,
        // indicators
        "<Info>"
        "    <gidural..svetophor dor='28' stan='31000' stan_napr='0' pr_lok='0' pr_brig='0' pr_put='3'/>"
        "    <gidural..svetophor dor='28' stan='31000' stan_napr='23700' pr_lok='3' pr_brig='2' pr_put='0'/>"
        "    <gidural..svetophor dor='28' stan='31000' stan_napr='31490' pr_lok='0' pr_brig='1' pr_put='0'/>"
        "</Info>"
        ,
        // trains
        "<Info>"
        "    <Strip Id='24360 К1ЧП      ' />"
        "    <Strip Id='04610 01/02     '>"
        "        <Train Num='2528' Idx='0300-279-7600' Ulen='70' Brut='2238' />"
        "    </Strip>"
        "</Info>"
    };

    for( auto text : nords )
    {
        attic::a_document doc;
        doc.load_wide( From1251(text) );
        CPPUNIT_ASSERT( Foreign::AsyncNordRW::detect(doc) );
    }
}

void TC_Foreign::biskWarn()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka>"
        L"  <To>0</To>"
        L"  <Iz>2509201609261811283010</Iz>"
        L"  <Nz>27505</Nz>"
        L"  <Ds>201710021631</Ds>"
        L"  <OtStanz>110003</OtStanz>"
        L"  <OtKm>390</OtKm>"
        L"  <OtPk>6</OtPk>"
        L"  <DoKm>390</DoKm>"
        L"  <DoPk>7</DoPk>"
        L"  <Dop>Съезд Nr. 4-6; </Dop>"
        L"  <Har>Работает PALLAS (наплавка рельсов).</Har>"
        L"  <SDT>201609270740</SDT>"
        L"  <DoDT/>"
        L"  <TMP>1</TMP>"
        L"  <Sk>25</Sk>"
        L"</zka>"
        );

    EsrKit ESR("110003",0,true);
    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    const LimitDetail& detail = lim.detail;

    CPPUNIT_ASSERT( !lim.empty() );
    CPPUNIT_ASSERT( lim.getIdentifer().get_value()   == ExtId::IdCode("R27505.201710021631", "") );
    CPPUNIT_ASSERT( lim.getIdentifer().get_refcode() == "2509201609261811283010" );
    auto sparse = lot.get<Excerpt::SparseLot>();
    CPPUNIT_ASSERT( sparse != nullptr );
    CPPUNIT_ASSERT( lot.getEsr() == ESR );
    CPPUNIT_ASSERT( sparse->getObject().num() == ESR );

    CPPUNIT_ASSERT( detail.start() == UTC_from_Str("201609270740", "%Y%m%d%H%M") );
    CPPUNIT_ASSERT( detail.untilStop() ); 
    CPPUNIT_ASSERT( detail.getComment() == L"Работает PALLAS (наплавка рельсов)." ); 
    CPPUNIT_ASSERT( detail.getReason() == 0 ); 
    CPPUNIT_ASSERT( detail.isSudden() == false ); 
    RestrictSpeed rs;
    rs[RestrictSpeed::ALL] = 25;
    CPPUNIT_ASSERT( detail.getRestrict() == rs );
    // check response
    CPPUNIT_ASSERT( !bunch.response.empty() );
    attic::a_document xd;
    xd.load_utf8( bunch.response );
    CPPUNIT_ASSERT( BisK::TO::CONFIRM == xd.child("zka").child("To").text_as_uint(99) );
    CPPUNIT_ASSERT( BisK::KP::SUCCESS == xd.child("zka").child("Kp").text_as_uint(99) );
}

void TC_Foreign::biskExpireWarn()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka>"
        L"  <To>1</To>"
        L"  <Ii>2509201609261033287120</Ii>"
        L"  <Ioz>2509201609251519296100</Ioz>"
        L"  <DoDT>201609261035</DoDT>"
        L"  <Nz>27306</Nz>"
        L"  <Ds>201709291144</Ds>"
        L"</zka>"
        );

    EsrKit ESR("110003",0,true);
    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( nullptr != bunch.LimitExpire );
    const Limit::Expire& lt = *bunch.LimitExpire;

    CPPUNIT_ASSERT( !lt.empty() );
    CPPUNIT_ASSERT( lt.getIdentifer().get_value() == ExtId::IdCode("R27306.201709291144", "") );
    CPPUNIT_ASSERT( lt.getIdentifer().get_refcode() == "2509201609251519296100" );
    CPPUNIT_ASSERT( lt.getStop() == UTC_from_Str("201609261035", "%Y%m%d%H%M") );
}

void TC_Foreign::biskWarn_SingleWaySpan()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file='2509201705251119565750_13878.xml'>"
        L"  <To>0</To>"
        L"  <Iz>2509201705251119565750</Iz>"
        L"  <Nz>27505</Nz>"
        L"  <Ds>201710021631</Ds>"
        L"  <OtStanz>114127</OtStanz>"
        L"  <DoStanz>114019</DoStanz>"
        L"  <Put>главный путь</Put>"
        L"  <OtKm>151</OtKm>"
        L"  <OtPk>6</OtPk>"
        L"  <DoKm>151</DoKm>"
        L"  <DoPk>8</DoPk>"
        L"  <Dop>; главный путь</Dop>"
        L"  <Har>Ремонт  пути.</Har>"
        L"  <SDT>201705251150</SDT>"
        L"  <DoDT />"
        L"  <TMP>3</TMP>"
        L"  <Sk>60</Sk>"
        L"</zka>"
        );

    EsrKit ESR(11412,11401);
    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( spanlot != nullptr );
    CPPUNIT_ASSERT( spanlot->getEsr() == ESR );
    CPPUNIT_ASSERT( spanlot->getParkWay() == ParkWayKit(1) );
    rwInterval ibeg = rwInterval::from_ordinal(151,6);
    rwInterval iend = rwInterval::from_ordinal(151,8);
    CPPUNIT_ASSERT( spanlot->getRInterval().interval() == aggregate(ibeg,iend) );
}

void TC_Foreign::biskWarn_SingleWayFullSpan()
{
    attic::a_document doc;
    doc.load_wide(
        L" <zka file='2509202308072032193620_17383.xml'>"
        L"   <To>0</To>"
        L"   <Iz>2509202308072032193620</Iz>"
        L"   <Nz>17383</Nz>"
        L"   <Ds>202308072032</Ds>"
        L"   <OtStanz>092905</OtStanz>"
        L"   <DoStanz>093039</DoStanz>"
        L"   <Put>однопутный</Put>"
        L"   <Dop>однопутный</Dop>"
        L"   <Har>Приближаться и проезжать переезд с особой бдительностью и готовностью остановится, в случае препятствия для дальнейшего движения. Отключено энергоснабжение переезда.</Har>"
        L"   <SDT>202308072032</SDT>"
        L"   <DoDT />"
        L"   <TMP>3</TMP>"
        L"   <Sk>40</Sk>"
        L" </zka>"
    );

    EsrKit ESR( 9290, 9303 );
    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( spanlot != nullptr );
    CPPUNIT_ASSERT( spanlot->getEsr() == ESR );
    CPPUNIT_ASSERT( spanlot->getParkWay() == ParkWayKit( 1 ) );
    rwRuledInterval ri( rwRuler(L"Лиепая"), rwCoord(73,366), rwCoord(92,349) );
    CPPUNIT_ASSERT( spanlot->getRInterval() == ri );
}

void TC_Foreign::biskWarn_VerbalOddSpanWay()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file='2509201705251021304960_13853.xml'>"
        L"  <To>0</To>"
        L"  <Iz>2509201705251021304960</Iz>"
        L"  <Nz>27505</Nz>"
        L"  <Ds>201710021631</Ds>"
        L"  <OtStanz>094101</OtStanz>"
        L"  <DoStanz>094008</DoStanz>"
        L"  <Put>нечетный путь</Put>"
        L"  <OtKm>35</OtKm>"
        L"  <OtPk>10</OtPk>"
        L"  <DoKm>40</DoKm>"
        L"  <DoPk>6</DoPk>"
        L"  <Dop>; нечетный путь</Dop>"
        L"  <Har>Кап.ремонт пути.</Har>"
        L"  <SDT>201705260800</SDT>"
        L"  <DoDT />"
        L"  <TMP>3</TMP>"
        L"  <Sk>60</Sk>"
        L"</zka>"
        );

    EsrKit ESR(9410,9400);
    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( spanlot != nullptr );
    CPPUNIT_ASSERT( spanlot->getEsr() == ESR );
    CPPUNIT_ASSERT( spanlot->getParkWay() == ParkWayKit(1) );
    rwInterval ibeg = rwInterval::from_ordinal(35,10);
    rwInterval iend = rwInterval::from_ordinal(40,6);
    CPPUNIT_ASSERT( spanlot->getRInterval().interval() == aggregate(ibeg,iend) );
}

void TC_Foreign::biskWarn_VerbalEvenSpanWay()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file='2509201705251025345170_13856.xml'>"
        L"  <To>0</To>"
        L"  <Iz>2509201705251025345170</Iz>"
        L"  <Nz>27505</Nz>"
        L"  <Ds>201710021631</Ds>"
        L"  <OtStanz>094101</OtStanz>"
        L"  <DoStanz>094008</DoStanz>"
        L"  <Put>четный путь</Put>"
        L"  <OtKm>40</OtKm>"
        L"  <OtPk>7</OtPk>"
        L"  <DoKm>41</DoKm>"
        L"  <DoPk>7</DoPk>"
        L"  <Dop>; четный путь</Dop>"
        L"  <Har>На соседнем пути работает путевая машина.</Har>"
        L"  <SDT>201705260940</SDT>"
        L"  <DoDT />"
        L"  <TMP>1</TMP>"
        L"  <Sk>40</Sk>"
        L"</zka>"
        );

    EsrKit ESR(9410,9400);
    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( spanlot != nullptr );
    CPPUNIT_ASSERT( spanlot->getEsr() == ESR );
    CPPUNIT_ASSERT( spanlot->getParkWay() == ParkWayKit(2) );
    rwInterval ibeg = rwInterval::from_ordinal(41,7);
    rwInterval iend = rwInterval::from_ordinal(40,7);
    CPPUNIT_ASSERT( spanlot->getRInterval().interval() == aggregate(ibeg,iend) );
}

void TC_Foreign::biskWarn_checkTmpSk()
{
    std::string common =
        "<zka file='2509201708111351317790_22067.xml'>"
        "  <To>0</To>"
        "  <Iz>2509201708111351317790</Iz>"
        "  <Nz>27505</Nz>"
        "  <Ds>201710021631</Ds>"
        "  <OtStanz>097504</OtStanz>"
        "  <DoStanz>097720</DoStanz>"
        "  <Put />"
        "  <OtKm>110</OtKm>"
        "  <OtPk>3</OtPk>"
        "  <DoKm>112</DoKm>"
        "  <DoPk>10</DoPk>"
        "  <SDT>201708121100</SDT>"
        "  <DoDT>201708122000</DoDT>"
//         "  <TMP>7</TMP>"
//         "  <Sk>40</Sk>"
//         "  <Sk1>60</Sk1>"
//         "  <Sk2>80</Sk2>"
        "</zka>";


    enum { SK  = 1, SK1 = 2, SK2 = 4 };
    struct TYPE7_2
    {
        int tmp_value;
        int required;
        int illegal;
    };

    TYPE7_2 table_7_2[] = // таблица из ТЗ "Увязка BisK-KIGAS"
    { //    TMP, обязательно, запрещено  
        {   1,      SK,           SK1+SK2 },
        {   2,      0,         SK+SK1+SK2 },
        {   3,      SK,           SK1+SK2 },
        {   4,      0,         SK+SK1+SK2 },
        {   5,      SK1,       SK+    SK2 },
        {   6,      SK2,       SK+SK1     },
        {   7,      SK1+SK2,   SK         },
        {   8,      SK1+SK2,   SK         },
        {   9,      SK1,       SK+    SK2 },
        {  10,      SK2,       SK+SK1     },
    };

    const int DATA_SK = 40;
    const int DATA_SK1 = 50;
    const int DATA_SK2 = 60;

    EsrKit ESR(9750,9772);
    Foreign::GlobalData ngd( gUraler, esrGuide );

    for( auto t72 : table_7_2 )
    {
        for( int i=1; i<=SK+SK1+SK2; ++i )
        {
            attic::a_document doc;
            doc.load_wide( From1251( common ) );
            attic::a_node zka = doc.document_element();
            zka.append_child("TMP").append_text( t72.tmp_value );
            if ( i & SK )
                zka.append_child("Sk").append_text( DATA_SK );
            if ( i & SK1 )
                zka.append_child("Sk1").append_text( DATA_SK1 );
            if ( i & SK2 )
                zka.append_child("Sk2").append_text( DATA_SK2 );

            Foreign::AsyncBisk aBisk( ngd );
            Foreign::SyncBisk  sBisk( aBisk, wallClock );
            aBisk.process( doc, wallClock );
            Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

            bool correctly =
                !(t72.required & ~i) && // в документе есть все необходимые теги
                !(t72.illegal & i);     // в документе НЕТ запрещенных тегов

//             if ( correctly == bunch.Limits.empty() )
//                 OutputDebugString(L"ll");
            if ( correctly )
            {
                CPPUNIT_ASSERT( !bunch.Limits.empty() );
                const Limit::Core& lim = bunch.Limits.back();
                const Excerpt::Lot& lot = lim.lot;
                // проверка разбора пути перегона
                CPPUNIT_ASSERT( !lot.empty() );
                const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
                CPPUNIT_ASSERT( spanlot != nullptr );
                CPPUNIT_ASSERT( spanlot->getEsr() == ESR );
            }
            else
            {
                CPPUNIT_ASSERT( bunch.Limits.empty() );
            }
        }
    }
}

void TC_Foreign::biskWarn_Tornakalns_Olaine_nepara()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file='2509201705251021304977_13877.xml'>"
        L"  <To>0</To>"
        L"  <Iz>2509201705251021304977</Iz>"
        L"  <Nz>27511</Nz>"
        L"  <Ds>201710021631</Ds>"
        L"  <OtStanz>091001</OtStanz>"
        L"  <DoStanz>091508</DoStanz>"
        L"  <Put>нечетный путь</Put>"
        L"  <OtKm>21</OtKm>"
        L"  <OtPk>7</OtPk>"
        L"  <DoKm>21</DoKm>"
        L"  <DoPk>10</DoPk>"
        L"  <Dop>; нечетный путь</Dop>"
        L"  <Har>Кап.ремонт пути.</Har>"
        L"  <SDT>201705260800</SDT>"
        L"  <DoDT />"
        L"  <TMP>3</TMP>"
        L"  <Sk>60</Sk>"
        L"</zka>"
        );

    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( spanlot != nullptr );
    EsrKit Olaine8km(9150,9104); // именно: несуществующий Торнякалнс-Олайне заменен на Олайне-8км
    CPPUNIT_ASSERT( spanlot->getEsr() == Olaine8km );
    CPPUNIT_ASSERT( spanlot->getParkWay() == ParkWayKit(1) );
    rwInterval ibeg = rwInterval::from_ordinal(21,7);
    rwInterval iend = rwInterval::from_ordinal(21,10);
    CPPUNIT_ASSERT( spanlot->getRInterval().interval() == aggregate(ibeg,iend) );
}

void TC_Foreign::biskWarn_Tornakalns_Olaine_para()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file='2509201705251021304977_13877.xml'>"
        L"  <To>0</To>"
        L"  <Iz>2509201705251021304977</Iz>"
        L"  <Nz>27511</Nz>"
        L"  <Ds>201710021631</Ds>"
        L"  <OtStanz>091001</OtStanz>"
        L"  <DoStanz>091508</DoStanz>"
        L"  <Put>четный путь</Put>"
        L"  <OtKm>6</OtKm>"
        L"  <OtPk>7</OtPk>"
        L"  <DoKm>6</DoKm>"
        L"  <DoPk>7</DoPk>"
        L"  <Dop>; четный путь</Dop>"
        L"  <Har>Кап.ремонт пути.</Har>"
        L"  <SDT>201705260800</SDT>"
        L"  <DoDT />"
        L"  <TMP>3</TMP>"
        L"  <Sk>25</Sk>"
        L"</zka>"
        );

    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( spanlot != nullptr );
    EsrKit Torn_8km(9100,9104); // именно: несуществующий Торнякалнс-Олайне заменен на Торнякалнс-8км
    CPPUNIT_ASSERT( spanlot->getEsr() == Torn_8km );
    CPPUNIT_ASSERT( spanlot->getParkWay() == ParkWayKit(2) );
    rwInterval iPk = rwInterval::from_ordinal(6,7);
    CPPUNIT_ASSERT( spanlot->getRInterval().interval() == iPk );
}

void TC_Foreign::biskWarn_Saulkrasti_Lilaste()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file='2509201810171005437700_30124.xml'>"
        L"  <To>0</To>"
        L"  <Iz>2509201810171005437700</Iz>"
        L"  <Nz>30124</Nz>"
        L"  <Ds>201810170938</Ds>"
        L"  <OtStanz>096037</OtStanz>"
        L"  <DoStanz>096003</DoStanz>"
        L"  <Put>однопутный</Put>"
        L"  <OtKm>39</OtKm>"
        L"  <OtPk>9</OtPk>"
        L"  <DoKm>39</DoKm>"
        L"  <DoPk>10</DoPk>"
        L"  <Dop>однопутный</Dop>"
        L"  <SDT>201810170940</SDT>"
        L"  <DoDT />"
        L"  <TMP>1</TMP>"
        L"  <Sk>40</Sk>"
        L"</zka>"
        );

    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( spanlot != nullptr );
    EsrKit Incupe_Lilaste(9604,9603); // именно: несуществующий Saulkrasti-Lilaste заменен на Incupe-Lilaste
    CPPUNIT_ASSERT( spanlot->getEsr() == Incupe_Lilaste );
    CPPUNIT_ASSERT( spanlot->getParkWay() == ParkWayKit(1) );
    rwInterval iPk = aggregate( rwInterval::from_ordinal(39,9), rwInterval::from_ordinal(39,10) );
    CPPUNIT_ASSERT( spanlot->getRInterval().interval() == iPk );
}

void TC_Foreign::biskWarn_Tornakalns_Olaine_putB()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file='2509201705251021304977_13877.xml'>"
        L"  <To>0</To>"
        L"  <Iz>2509201705251021304977</Iz>"
        L"  <Nz>27511</Nz>"
        L"  <Ds>201710021631</Ds>"
        L"  <OtStanz>091001</OtStanz>"
        L"  <DoStanz>091508</DoStanz>"
        L"  <Put>путь \"Б\"</Put>"
        L"  <OtKm>6</OtKm>"
        L"  <OtPk>7</OtPk>"
        L"  <DoKm>6</DoKm>"
        L"  <DoPk>7</DoPk>"
        L"  <Dop>. путь \"Б\"</Dop>"
        L"  <Har>Кап.ремонт пути.</Har>"
        L"  <SDT>201705260800</SDT>"
        L"  <DoDT />"
        L"  <TMP>3</TMP>"
        L"  <Sk>25</Sk>"
        L"</zka>"
        );

    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( spanlot != nullptr );
    EsrKit Torn_8km(9100,9104); // именно: несуществующий Торнякалнс-Олайне заменен на Торнякалнс-8км
    CPPUNIT_ASSERT( spanlot->getEsr() == Torn_8km );
    CPPUNIT_ASSERT( spanlot->getParkWay() == ParkWayKit(2) );
    rwInterval iPk = rwInterval::from_ordinal(6,7);
    CPPUNIT_ASSERT( spanlot->getRInterval().interval() == iPk );
}

void TC_Foreign::biskWarn_Tornakalns_Olaine_ticket7185()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file=\"2509202303270408123740_5829.xml\">"
        L"  <To>0</To>"
        L"  <Iz>2509202303270408123740</Iz>"
        L"  <Nz>5829</Nz>"
        L"  <Ds>202303211340</Ds>"
        L"  <OtStanz>091008</OtStanz>"
        L"  <DoStanz>091508</DoStanz>"
        L"  <Put>четный путь</Put>"
        L"  <OtKm>8</OtKm>"
        L"  <OtPk>1</OtPk>"
        L"  <Dop>\"B\" четный путь</Dop>"
        L"  <Har>Дефектность рельсов.</Har>"
        L"  <SDT>202303211400</SDT>"
        L"  <DoDT />"
        L"  <TMP>3</TMP>"
        L"  <Sk>40</Sk>"
        L"</zka>"
    );

    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора
    CPPUNIT_ASSERT( !lot.empty() );
    {
        const Excerpt::SpanLot* spanlot = lot.get<Excerpt::SpanLot>();
        CPPUNIT_ASSERT( spanlot == nullptr ); // предупреждение оказалось на блок-посту а не на перегоне
    }
    {
        const Excerpt::TermWayLot* termlot = lot.get<Excerpt::TermWayLot>();
        EsrKit bp8km( 9104 ); // именно: предупреждение оказалось именно на промежуточном блок-посту 8км
        CPPUNIT_ASSERT( termlot->getEsr() == bp8km );
        CPPUNIT_ASSERT( termlot->getParkWay() == ParkWayKit( 2 ) );
    }
}

void TC_Foreign::biskWarn_Tornakalns_switch()
{
    attic::a_document doc;
    doc.load_wide(
        L"<zka file='2509202002201526313880_4779_conf.xml'>"
        L"  <To>0</To>"
        L"  <Iz>2509201705251021304977</Iz>"
        L"  <Nz>27511</Nz>"
        L"  <Ds>201710021631</Ds>"
        L"  <OtStanz>091001</OtStanz>"
        L"  <Dop>Parmijas Nr. 67.,9.,1\n2. главный путь</Dop>"
        L"  <SDT>201705260800</SDT>"
        L"  <DoDT />"
        L"  <TMP>3</TMP>"
        L"  <Sk>25</Sk>"
        L"</zka>"
        );

    Foreign::GlobalData ngd( gUraler, esrGuide );
    Foreign::AsyncBisk aBisk( ngd );
    Foreign::SyncBisk  sBisk( aBisk, wallClock );
    aBisk.process( doc, wallClock );
    Foreign::Bunch bunch = sBisk.complete( lex, SupplyTrainInfo() );

    CPPUNIT_ASSERT( !bunch.Limits.empty() );
    const Limit::Core& lim = bunch.Limits.back();
    const Excerpt::Lot& lot = lim.lot;
    // проверка разбора пути перегона
    CPPUNIT_ASSERT( !lot.empty() );
    const Excerpt::SparseLot* sparseLot = lot.get<Excerpt::SparseLot>();
    CPPUNIT_ASSERT( sparseLot != nullptr );
    CPPUNIT_ASSERT( sparseLot->getObject() == BadgeE(L"67", EsrKit(9100)) );
    CPPUNIT_ASSERT( sparseLot->getClassObject() == ELEMENT_CLASS::SWITCH );
}

void TC_Foreign::readNordRW_GU_text()
{
    // пример из файла svn::MNS/АРМ/Ярославль-Сонково/Данные/BASHILOV/INF/INF_28/NORM_PS.288
    std::string data =
"; Расписания пасс. поездов из базы АРМ ГДП (LST).28.05.2014  10:22:50\r\n"
";\r\n"
"; \"Ковров\" - \"Москва Яр.\"\r\n"
";              Ковров          Федулово        Пост 243 км        Новки2        Новки-2 САИПС     Большаково         Савино          Шорыгино         Ладыгино            Шуя           Ворожино           Кохма           Горино        Иваново-Сорт.       Иваново        Текстильный      Оболсуново         Тейково          Сахтыш          Якшинский          Нерль         Петровская     Гаврилов Посад   Юрьев-Польский       Бавлены        Кольчугино         Кипрево      Бельково САИПС      Бельково       Александров 2     Александров      Москва Яр.    \r\n"
"@1              26310            26304            29602            26290            26289            31931            31930            31921            31901            31900            31891            31890            31880            31860            31850            31840            31820            31810            31800            31790            31781            31770            31760            31730            31720            31710            31690            23732            23730            23701            23700            19550      \r\n"
"# 0987>     0/0 8888 1649    0/0 1653 1653    1/1 1658 1658    1/1 1704 1704    0/0 1706 1706    1/1 1723^1724    1/1 1736^1737    1/1 1746 1746    1/1 1753 1753    1/1 1809^1818    1/1 1831 1831    1/1 1838 1838    1/1 1842 1842    3/1 1847 1847    1/1 1856 1934    1/1 1944 1944    1/1 2004^2040    1/1 2049 2049    1/1 2055 2055    1/1 2103 2103    1/1 2113^2114    1/1 2124^2125    1/1 2141^2142    1/1 2217 2217    1/1 2232 2232    1/1 2248 2248    1/1 2317^2318    0/0 2338 2338    1/1 2340 2340    1/1 2354 2354    1/1 0004 0131    0/0 0322 8888 \r\n"
"# 0985>     0/0 8888 1649    0/0 1653 1653    1/1 1658 1658    1/1 1704 1704    0/0 1706 1706    1/1 1723^1724    1/1 1736^1737    1/1 1746 1746    1/1 1753 1753    1/1 1809^1818    1/1 1831 1831    1/1 1838 1838    1/1 1842 1842    3/1 1847 1847    1/1 1856 1934    1/1 1944 1944    1/1 2004^2040    1/1 2049 2049    1/1 2055 2055    1/1 2103 2103    1/1 2113^2114    1/1 2124^2125    1/1 2141^2142    1/1 2217 2217    1/1 2232 2232    1/1 2248 2248    1/1 2317^2318    0/0 2338 2338    1/1 2340 2340    1/1 2354 2354    1/1 0004 0131    0/0 0322 8888 \r\n"
"# 0983>     0/0 8888 1649    0/0 1653 1653    1/1 1658 1658    1/1 1704 1704    0/0 1706 1706    1/1 1723^1724    1/1 1736^1737    1/1 1746 1746    1/1 1753 1753    1/1 1809^1818    1/1 1831 1831    1/1 1838 1838    1/1 1842 1842    3/1 1847 1847    1/1 1856 1934    1/1 1944 1944    1/1 2004^2040    1/1 2049 2049    1/1 2055 2055    1/1 2103 2103    1/1 2113^2114    1/1 2124^2125    1/1 2141^2142    1/1 2217 2217    1/1 2232 2232    1/1 2248 2248    1/1 2317^2318    0/0 2338 2338    1/1 2340 2340    1/1 2354 2354    1/1 0004 0131    0/0 0322 8888 \r\n"
"# 0981>     0/0 8888 1649    0/0 1653 1653    1/1 1658 1658    1/1 1704 1704    0/0 1706 1706    1/1 1723^1724    1/1 1736^1737    1/1 1746 1746    1/1 1753 1753    1/1 1809^1818    1/1 1831 1831    1/1 1838 1838    1/1 1842 1842    3/1 1847 1847    1/1 1856 1934    1/1 1944 1944    1/1 2004^2040    1/1 2049 2049    1/1 2055 2055    1/1 2103 2103    1/1 2113^2114    1/1 2124^2125    1/1 2141^2142    1/1 2217 2217    1/1 2232 2232    1/1 2248 2248    1/1 2317^2318    0/0 2338 2338    1/1 2340 2340    1/1 2354 2354    1/1 0004 0131    0/0 0322 8888 \r\n"
"# 0979>     0/0 8888 1649    0/0 1653 1653    1/1 1658 1658    1/1 1704 1704    0/0 1706 1706    1/1 1723^1724    1/1 1736^1737    1/1 1746 1746    1/1 1753 1753    1/1 1809^1818    1/1 1831 1831    1/1 1838 1838    1/1 1842 1842    3/1 1847 1847    1/1 1856 1934    1/1 1944 1944    1/1 2004^2040    1/1 2049 2049    1/1 2055 2055    1/1 2103 2103    1/1 2113^2114    1/1 2124^2125    1/1 2141^2142    1/1 2217 2217    1/1 2232 2232    1/1 2248 2248    1/1 2317^2318    0/0 2338 2338    1/1 2340 2340    1/1 2354 2354    1/1 0004 0131    0/0 0322 8888 \r\n"
";\r\n"
"; \"Котлас-Южный\" - \"Вологда-1\"\r\n"
";           Котлас-Южный   Котлас Узел Парк1   Котлас-Узел       Пл1078км          Ядриха            Реваж            Удима            Ерга           Ломоватка         Сенгос           Кизема            Лойга           Сулонда           Илеза           Шангалы         Костылево        Кокшеньга          Кулой            Вага            Вельск            Пежма         Рзд. 78 км     Келарева Горка      Пл 62 км          Можуга          Валдеево         Вересово         Коноша-2      Коноша-1 парк Б     Коноша-1          Ерцево           Явенга           Вожега        Кадниковский        Пундуга         Харовская      Семигородная       Морженга         Печаткино         Сухона           Рыбкино        Б/п. 499 км       Вологда-1    \r\n"
"@2              28010            28013            28020            29945            29940            29931            29920            29907            29900            29898            29870            29860            29841            29830            29811            29810            29792            29790            29780            29770            29712            29710            29711            29702            29700            29691            29690            29303            29299            29300            29290            29280            30140            30130            30112            30110            30100            30082            30070            30060            30050            30022            30010      \r\n"
"# 0937>     1/1 0121 0201   1/11 0210 0210   10/1 0217 0251    1/1 0257 0257    1/1 0302 0302    1/1 0313 0313    1/1 0339 0339    1/1 0350 0350    1/1 0402 0402    1/1 0414 0414    1/1 0442 0442    1/1 0457 0457    1/1 0518 0518    1/1 0529 0529    1/1 0555 0555    1/1 0608 0608    1/1 0628 0628    1/1 0654 0709    1/1 0732 0732    1/3 0739 0739    1/1 0758 0758    1/1 0803 0803    1/1 0809 0809    1/1 0815 0815    1/1 0836 0836    1/1 0847 0847    1/1 0857 0857    1/1 0904 0904    2/1 0913 0913    1/1 0917 0945    1/1 1007 1007    1/1 1022 1022    1/1 1037 1037    1/1 1049 1049    1/1 1103 1103    1/1 1120 1120    1/1 1130 1130    1/1 1143 1143    1/1 1153 1153    1/1 1159 1159    1/1 1215 1215    1/1 1217 1217    1/1 1225 8888 \r\n"
"# 0935>     1/1 2223 2315   1/11 2324 2324   10/1 2330 0002    1/1 0008 0008    1/1 0014 0017    1/1 0029 0029    1/1 0053 0053    1/1 0103 0103    1/1 0112 0112    1/1 0123 0123    1/1 0149 0149    1/1 0203 0203    1/1 0223 0223    1/1 0233 0233    1/1 0254 0254    1/1 0305 0308    1/1 0327 0327    1/1 0348 0358    1/1 0417 0417    1/3 0423 0427    1/1 0449^0455    1/1 0502 0502    1/1 0510 0512    1/1 0520 0520    1/1 0541 0541    1/1 0552 0552    1/1 0602 0602    1/1 0609 0609    2/1 0618 0618    1/1 0623 0700    1/1 0723 0723    1/1 0743 0743    1/1 0758 0800    1/1 0816 0816    1/1 0831 0831    1/1 0849 0851    1/1 0906 0906    1/1 0921 0921    1/1 0931 0931    1/1 0937 0937    1/1 0954 0954    1/1 0958 0958    1/1 1007 1042 \r\n"
";\r\n"
"; \"Вологда-1\" - \"Котлас-Южный\"\r\n"
";             Вологда-1       Б/п. 499 км        Рыбкино          Сухона          Печаткино        Морженга       Семигородная       Харовская         Пундуга       Кадниковский        Вожега           Явенга           Ерцево          Коноша-1         Коноша-2         Вересово         Валдеево          Подюга             Юра          Усть-Шоноша    Верхняя Синега       Вельск            Вага             Кулой          Кокшеньга        Костылево         Шангалы           Илеза           Сулонда           Лойга           Кизема           Сенгос          Ломоватка          Ерга             Удима            Реваж           Ядриха          Пл1078км        Котлас-Узел   Котлас Узел Парк1  Котлас-Южный   \r\n"
"@3              30010            30022            30050            30060            30070            30082            30100            30110            30112            30130            30140            29280            29290            29300            29303            29690            29691            29730            29742            29749            29758            29770            29780            29790            29792            29810            29811            29830            29841            29860            29870            29898            29900            29907            29920            29931            29940            29945            28020            28013            28010      \r\n"
"# 0938>     2/4 8888 1815    1/2 1823 1823    1/2 1830 1830    1/2 1849 1849    1/2 1855 1855    1/2 1904 1904    1/2 1919 1919    1/2 1931 1931    1/2 1951 1951    1/2 2006 2006    1/2 2022 2022    1/2 2039 2039    1/2 2056 2056    1/2 2119 2141    1/2 2155 2155    1/2 2204 2204    1/2 2215 2215    1/2 2234 2234    1/2 2259 2259    1/2 2310 2310    1/2 2335 2335    1/2 2357 2357    1/2 0006 0006    1/2 0024 0036    1/2 0058 0058    1/2 0117 0117    1/2 0130 0130    1/2 0149 0149    1/2 0158 0158    1/2 0216 0216    1/2 0226 0226    1/2 0250 0250    1/2 0300 0300    1/2 0310 0310    1/2 0318 0318    1/2 0336 0336    1/2 0343 0343    1/2 0346 0346   10/2 0354 0426   1/12 0432 0432    1/2 0440 0515 \r\n"
"# 0936>     2/4 8888 1710    1/2 1718 1718    1/2 1725 1725    1/2 1744 1744    1/2 1750 1750    1/2 1759 1759    1/2 1813 1813    1/2 1823 1823    1/2 1840 1840    1/2 1854 1854    1/2 1908 1908    1/2 1923 1923    1/2 1938 1938    1/2 2000 2025    1/2 2038 2038    1/2 2046 2046    1/2 2056 2056    1/2 2111 2111    1/2 2132 2132    1/2 2144 2144    1/2 2209 2209    1/2 2231 2231    1/2 2240 2240    1/2 2258 2313    1/2 2335 2335    1/2 2354 2354    1/2 0007 0007    1/2 0027 0027    1/2 0038 0038    1/2 0057 0057    1/2 0107 0107    1/2 0131 0131    1/2 0141 0141    1/2 0151 0151    1/2 0158 0158    1/2 0216 0216    1/2 0224 0224    1/2 0228 0228   10/2 0236 0308   1/12 0313 0313    1/2 0320 0355 \r\n"
";\r\n"
"; \"Котлас-Южный\" - \"Великий Устюг\"\r\n"
";           Котлас-Южный   Котлас Узел Парк1   Котлас-Узел       Пл1078км          Ядриха          Приводино        Красавино      Великий Устюг  \r\n"
"@4              28010            28013            28020            29945            29940            29950            29960            29970      \r\n"
"# 0937>     1/1 0442 0517   1/11 0526 0526   10/1 0532 0604    1/1 0611 0611    1/1 0616 0616    1/1 0634 0634    1/1 0656 0656    1/1 0730 8888 \r\n"
"# 0935>     1/1 0326 0400   1/11 0409 0409   10/1 0415 0447    1/1 0453 0453    1/1 0500 0500    1/1 0519 0519    1/1 0540 0540    1/1 0615 8888 \r\n"
;

    RegulatoryRecode::Schedule schedule;
    schedule.read_text_gu( data );
    attic::a_document charm_doc;
    schedule.write_charm( charm_doc, wallClock );
}

void TC_Foreign::readNordRW_Zip()
{
    std::string bindata[] = {
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA1 ),
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA2 ),
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA3 )
    };
    // обработка
    RegulatoryRecode::Gathering gather;
    CPPUNIT_ASSERT( !gather.ready() );
    for( const auto& data : bindata ) {
        CPPUNIT_ASSERT( !gather.ready() );
        CPPUNIT_ASSERT( gather.put( data ) );
    }
    CPPUNIT_ASSERT( gather.ready() );
    std::string unpacked = gather.get();
    CPPUNIT_ASSERT( unpacked.size() > 100*1024L );

    RegulatoryRecode::Schedule schedule;
    {
        auto ods = []( const std::wstring& ws ){
            OutputDebugString( ws.c_str() );
        };
        FotoFinish ff(L"read_text2_gu", ods, boost::posix_time::milliseconds(50));
        schedule.read_text2_gu( unpacked );
    }
    attic::a_document charm_doc;
    schedule.write_charm( charm_doc, wallClock );

    /* наличие парков, путей, четности, календаря

    @I  ID="0013 27580 04000 0013Н" N="0013"
    @N K="0" S="1" D1="17.04.2022" D2="19.04.2022" T="НОВОКУЗНЕЦК 'Новокузнецк - С-Петербург Лад.'" ID_ET="910589394" ID_ET="911317013"
    @R ...     30343       30331ч      30000       30010н      30030     ...
    @T ...   2117 2117   2126 2126   2138 2138   2148 2214   2221 2221   ...
    @W ...      1/1         2/1         1/2         2/2         1/1      ...
    @K Y="2022" M="04" D="00000000000000001010000000000000"
    */
    // наличие конкретного поезда
    attic::a_node thNode = charm_doc.select_node("//Thread[@train='13']").node();
    CPPUNIT_ASSERT( thNode );
    // календаря для него
    attic::a_node daysNode = thNode.child("OnDays");
    CPPUNIT_ASSERT( daysNode );
    // конкретного события
    attic::a_node evNode = thNode.select_node("//Thread[@train='13']/Event[@esr='30331']").node();
    CPPUNIT_ASSERT( evNode );
    CPPUNIT_ASSERT_EQUAL( 1,  evNode.attribute("waynum" ).as_int() );
    CPPUNIT_ASSERT_EQUAL( 2, evNode.attribute("parknum").as_int() );
    CPPUNIT_ASSERT_EQUAL( 212600L, evNode.attribute("time").as_long() );
    CPPUNIT_ASSERT_EQUAL( 30331L,  evNode.attribute("esr").as_long() );
    CPPUNIT_ASSERT_EQUAL( 30331L,  evNode.attribute("esr").as_long() );
    CPPUNIT_ASSERT( "even" == evNode.attribute("parity").as_string_ref() );
}

void TC_Foreign::orderNordRW_Zip()
{
    std::string bindata[] = {
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA2 ), // проверка некорректной последовательности
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA1 ),
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA1 ), // дубль для проверки восстановления
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA2 ),
        "(:1042 909/000+11310 ...:)", // проверка неподходящих данных
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA3 ), // полное сообщение
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA1 ),
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA2 ),
        UtReadResource( L"BINDATA", ID_FOREIGN_DATA3 ), // второе полное сообщение
    };
    // обработка
    size_t success_receive = 0;
    RegulatoryRecode::Gathering gather;
    CPPUNIT_ASSERT( !gather.ready() );
    for( int i=0; i<size_array(bindata); ++i ) {
        const auto& data = bindata[i];
        CPPUNIT_ASSERT( !gather.ready() );
        // обработка очередной порции данных
        if ( gather.put( data ) ) 
            ; // данные добавлены успешно
        else if ( !gather.is002( data ) )
            ;// это неприемлемые данные - игнорировать
        else if ( gather.out_of_sequence( data ) ) {
            // нарушена последовательность
            gather.clear();     // сброс имеющихся
            gather.put( data ); // попытка добавить ещё раз (успешно или неуспешно - не важно)
        }
        // извлечение готового сообщения
        if ( gather.ready() ) {
            std::string unpacked = gather.get();
            CPPUNIT_ASSERT( unpacked.size() > 100*1024L );
            gather.clear();
            ++success_receive;
        }
    }
    CPPUNIT_ASSERT( 2 == success_receive );
}
