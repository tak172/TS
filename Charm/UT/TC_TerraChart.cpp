#include "stdafx.h"

#include "TC_TerraChart.h"
#include "../Hem/TerraChart.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TerraChart );

void TC_TerraChart::LoadSave()
{
    const wstring example = 
        L"<Core>"
        L"    <TerraChart>"
        L"        <BusyLayer>"
        L"            <BusyPath>"
        L"                <BusyEvent create_time='20131003T120100Z' name='WayOccupy' Bdg='1Ap[33333]' />"
        L"                <BusyEvent create_time='20131003T120300Z' name='WayEmpty'  Bdg='1Ap[33333]' />"
        L"            </BusyPath>"
        L"        </BusyLayer>"
        L"        <SilenceLayer>"
        L"            <SilencePath>"
        L"                <SilenceEvent create_time='20131003T112233Z' name='Cutoff' Bdg='?[11111]' />"
        L"                <SilenceEvent create_time='20131003T113355Z' name='Cutin'  Bdg='?[11111]' />"
        L"            </SilencePath>"
        L"        </SilenceLayer>"
        L"        <ScheduledLayer>"
        L"            <RegulatorySchedule startTime='20140101T000000Z' />"
        L"        </ScheduledLayer>"
        L"        <DncDspAttendance />"
        L"    </TerraChart>"
        L"</Core>";

    // подготовка
    attic::a_document doc_src;
    attic::a_document doc_dst;
    auto parse_res = doc_src.load_wide( example );
    CPPUNIT_ASSERT( parse_res );
    string core = doc_src.document_element().name();
    doc_dst.set_document_element( core );

    // переносим файл -> Terra -> файл
    TerraChart terra;
    terra.deserialize( doc_src.document_element() );
    terra.TakeChanges( attic::a_node() );
    terra.serialize( doc_dst.document_element() );

    // сравниваем результаты
    auto s = doc_src.to_str();
    auto d = doc_dst.to_str();
    auto mismresult = mismatch( s.cbegin(), s.cend(), d.cbegin(), d.cend() );
    CPPUNIT_ASSERT( s == d );
}


void TC_TerraChart::ReduceAmountDncDspAttendance()
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    auto F = []( int hh, int mm ) {
        ptime mom = ptime(date(2018,12,11)) + hours(hh) + minutes(mm);
        ptime epoch( date( 1970, 1, 1 ) );
        return ( mom - epoch ).total_seconds();
    };


    attic::a_document srcDoc;
    srcDoc.load_wide(
        L"<test>"
        L"<TerraChart>"
        L"<DncDspAttendance>"
        L"<District code='09000-09640'>"
        L"<Dnc start='20181002T174000Z' persona='Pakulneviсa' />"
        L"<Dnc start='20181003T060000Z' persona='Сereps' />"
        L"<Dnc start='20181003T174011Z' persona='Korotkijs' />"
        L"<Dnc start='20181004T055500Z' persona='Sapovalovs' />"
        L"<Dnc start='20181004T180000Z' persona='Cereps' />"
        L"<Dnc start='20181005T055535Z' persona='Korotkijs' />"
        L"<Dnc start='20181005T173000Z' persona='Sapovalovs' />"
        L"<Dnc start='20181206T060000Z' persona='Korotkijs' />"
        L"<Dnc start='20181206T173000Z' persona='Pakulnevica' />"
        L"<Dnc start='20181207T060000Z' persona='Cereps' />"
        L"<Dnc start='20181207T174545Z' persona='Korotkijs' />"
        L"<Dnc start='20181208T054000Z' persona='Pakulnevica' />"
        L"<Dnc start='20181208T180000Z' persona='Cereps' />"
        L"<Dnc start='20181209T054000Z' persona='Pakulnevica' />"
        L"<Dnc start='20181209T180000Z' persona='Tihomorskis' />"
        L"<Dnc start='20181210T060000Z' persona='Korotkijs' />"
        L"<Dnc start='20181210T174500Z' persona='Pakulnevica' />"
        L"<List>"
        L"<Dnc persona='Bisarovs' />"
        L"<Dnc persona='Jancenko' />"
        L"<Dnc persona='Korotkijs' />"
        L"<Dnc persona='Sokols' />"
        L"<Dnc persona='Springis' />"
        L"<Dnc persona='Tihomorskis' />"
        L"<Dnc persona='Cereps' />"
        L"<Dnc persona='Sakaluks' />"
        L"<Dnc persona='Sapovalovs' />"
        L"</List>"
        L"</District>"
        L"<Station ESR_code='09000'>"
        L"<Dsp start='20180213T060000Z' persona='Leja' />"
        L"<Dsp start='20180214T060000Z' persona='Fokina' />"
        L"<Dsp start='20180214T180000Z' persona='Leja' />"
        L"<Dsp start='20180215T060000Z' persona='Petrova' />"
        L"<Dsp start='20180215T180000Z' persona='Fokina' />"
        L"<Dsp start='20180217T060000Z' persona='Vasiljeva' />"
        L"<Dsp start='20180217T180000Z' persona='Rumjanceva' />"
        L"<Dsp start='20180218T180000Z' persona='Vasiljeva' />"
        L"<Dsp start='20181208T060000Z' persona='Petrova' />"
        L"<Dsp start='20181208T180000Z' persona='Fokina' />"
        L"<Dsp start='20181209T060000Z' persona='Orsers' />"
        L"<Dsp start='20181210T060000Z' persona='Vasiljeva' />"
        L"<Dsp start='20181210T180000Z' persona='Orsers' />"
        L"<Operator start='20180311T060000Z' persona='Miglane' />"
        L"<Operator start='20180314T060000Z' persona='Petrova' />"
        L"<Operator start='20180315T060000Z' persona='Miglane' />"
        L"<Operator start='20180317T180000Z' persona='Akilans' />"
        L"<Operator start='20180318T183500Z' />"
        L"<Operator start='20181207T060000Z' persona='Akilans' />"
        L"<Operator start='20181208T060000Z' persona='Kozakara' />"
        L"<Operator start='20181209T060000Z' persona='Leja' />"
        L"<Operator start='20181210T060000Z' persona='Kozakara' />"
        L"<List>"
        L"<Dsp persona='Fokina' />"
        L"<Dsp persona='Garasjuka' />"
        L"<Dsp persona='Leja' />"
        L"<Dsp persona='Orsers' />"
        L"<Dsp persona='Petrova' />"
        L"<Dsp persona='Rumjanceva' />"
        L"<Dsp persona='Sidorovics' />"
        L"<Dsp persona='Vasiljeva' />"
        L"<Operator persona='Akilans' />"
        L"<Operator persona='Kozakara' />"
        L"<Operator persona='Miglane' />"
        L"</List>"
        L"</Station>"
        L"<Station ESR_code='09006'>"
        L"<Dsp start='20180214T060000Z' persona='Grebenuka' />"
        L"<Dsp start='20180214T180000Z' persona='Halutins' />"
        L"<Dsp start='20180215T060000Z' persona='Jegorova' />"
        L"<Dsp start='20180215T180000Z' persona='Grebenuka' />"
        L"<Dsp start='20180217T060000Z' persona='Halutins' />"
        L"<Dsp start='20180217T180000Z' persona='Dobrilko' />"
        L"<Dsp start='20181210T060000Z' persona='Dobrilko' />"
        L"<Dsp start='20181210T180000Z' persona='Halutins' />"
        L"<Operator start='20180308T060000Z' persona='Minova' />"
        L"<Operator start='20180309T180000Z' />"
        L"<Operator start='20180311T060000Z' persona='Akilans' />"
        L"<Operator start='20180312T060000Z' persona='Minova' />"
        L"<Operator start='20180312T180000Z' persona='Akilans' />"
        L"<Operator start='20180313T180000Z' persona='Minova' />"
        L"<Operator start='20180314T060000Z' persona='Jermolajeva' />"
        L"<Operator start='20180315T060000Z' persona='Akilans' />"
        L"<Operator start='20180315T180000Z' persona='Jermolajeva' />"
        L"<Operator start='20180316T060000Z' persona='Minova' />"
        L"<Operator start='20180317T180000Z' />"
        L"<Operator start='20181208T060000Z' persona='Gavrilova' />"
        L"<Operator start='20181209T060000Z' persona='Grebenuka' />"
        L"<Operator start='20181210T060000Z' persona='Jermolajeva' />"
        L"<List>"
        L"<Dsp persona='Dobrilko' />"
        L"<Dsp persona='Fokina' />"
        L"<Dsp persona='Grebenuka' />"
        L"<Dsp persona='Halutins' />"
        L"<Dsp persona='Jegorova' />"
        L"<Dsp persona='Leja' />"
        L"<Dsp persona='Orsers' />"
        L"<Dsp persona='Podgajska' />"
        L"<Dsp persona='Sidorovics' />"
        L"<Operator persona='Akilans' />"
        L"<Operator persona='Gavrilova' />"
        L"<Operator persona='Jermolajeva' />"
        L"<Operator persona='Minova' />"
        L"<Operator persona='nav operatora' />"
        L"</List>"
        L"</Station>"
        L"<Station ESR_code='09008'>"
        L"<List>"
        L"<Dsp persona='Jegorova' />"
        L"<Dsp persona='nav' />"
        L"</List>"
        L"</Station>"
        L"</DncDspAttendance>"
        L"</TerraChart>"
        L"</test>"
        );

        TerraChart terra;
        terra.deserialize( srcDoc.document_element() );
        // до сжатия графика - все записи есть
        {
            attic::a_document out_doc("test");
            terra.serialize(out_doc.document_element());
            CPPUNIT_ASSERT( !out_doc.select_nodes("//District[count(Dnc)=17]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Dsp)=13]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Operator)=9]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Dsp)=8]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Operator)=14]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Dsp)=0]").empty() );
        }
        // само сжатие
        terra.ReduceTheAmount( F(6, 0) );
        // после сжатия - только записи не старше чем неделю назад
        {
            attic::a_document out_doc("test");
            terra.serialize(out_doc.document_element());
            CPPUNIT_ASSERT( !out_doc.select_nodes("//District[count(Dnc)=11]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Dsp)=6]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Operator)=5]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Dsp)=3]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Operator)=4]").empty() );
            CPPUNIT_ASSERT( !out_doc.select_nodes("//Station[count(Dsp)=0]").empty() );
        }

}

