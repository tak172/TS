#include "stdafx.h"

#include "TC_TrainDescr.h"
#include "../helpful/Attic.h"
#include "../helpful/StrToTime.h"
#include "../helpful/TrainDescr.h"
#include "../helpful/LocoCharacteristics.h"
#include "../helpful/Crew.h"
#include "../helpful/Locomotive.h"
#include "UtHelpfulDateTime.h"

using namespace std;
using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrainDescr );

void TC_TrainDescr::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_TrainDescr::getNord()
{
    const wstring p=
        L"<Info>"
        L"    <Train Num='1111' Idx='0000-000-0000' />"
        L"    <Train Num='2222' Idx='7979-127-0172' Ulen='61' Brut='5884' />"
        L"    <Train Num='3333' Idx='0300-883-7800' OH='НД' Ulen='82' Brut='3888' />"
        L"    <Train Num='4444' Idx='0300-760-2300' OH='Н2508' Ulen='101' Brut='2993' />"
        L"    <Train Num='5555' Idx='0300-762-2300' OH='ТНД' Ulen='71' Brut='5302' />"
        L"    <Train Num='6666' Idx='2700-015-0764' OH='ТН2508ВМ' Ulen='89' Brut='8000' />"
        L"    <Train Num='7777' Idx='0000-015-0765' OH='Т2508Д' />"
        L"    <Train Num='8888' Idx='1100-015-0004' OH='ВМН25ПД' />"
        L"    <Train Num='9999' Idx='1100-015-0004' OH='Н' />"
        L"</Info>";

    attic::a_document x_doc;
    attic::a_parse_result res = x_doc.load_wide( p );
    CPPUNIT_ASSERT( res );

    std::vector<TrainDescr> vTD;
    for( auto tn : x_doc.child("Info").children("Train") )
    {
        TrainDescr td = TrainDescr::getNord( tn );
        if ( !td.empty() )
            vTD.push_back( td );
    }
    // данные для проверки
    const size_t SZ = 9;
    wstring nums[] = { L"1111", L"2222", L"3333", L"4444", L"5555", L"6666", L"7777", L"8888", L"9999" };
    wstring notes[] = { L"", 
                        L"",
                        L"ДН",      //from "НД" - стандартный порядок 
                        L"Н2508",   //from "Н2508"
                        L"ДТН",     //from "ТНД" - стандартный порядок
                        L"ТВМН2508",  //from "ТН2508ВМ" 
                        L"ДТ",      //from "Т2508Д" - стандартный порядок
                        L"ПДВМН25", //from "ВМН25ПД" - стандартный порядок
                        L"Н"       //from "Н"
                        };
    CPPUNIT_ASSERT( SZ == size_array(nums) && SZ == size_array(notes) );
    // проверка результата
    CPPUNIT_ASSERT( SZ == vTD.size() );
    for( unsigned i=0; i<SZ; ++i )
        {
        const auto& tdr = vTD[i];
        CPPUNIT_ASSERT( tdr.GetNumber().getString() == nums[i] );
        CPPUNIT_ASSERT(tdr.wstr_notes() == notes[i] );
        }
}

void TC_TrainDescr::getNiias()
{
    attic::a_document x_doc;
    attic::a_parse_result res = x_doc.load_wide(
        L"<Info>                                                                               "
        L"    <Train Num='111' Idx='1212-121-1212' Ulen='21' Brut='2211' Delay='+2:11' />      "
        L"    <Train Num='3333' Idx='3331-332-3333' OH='Т' Ulen='60' Brut='373' Delay='+0:35'/>"
        L"    <Train Num='4444' Idx='4441-443-4444' />"
        L"</Info>                                                                              "
        );
    CPPUNIT_ASSERT( res );

    std::vector<TrainDescr> vTD;
    for( auto tn : x_doc.child("Info").children("Train") )
    {
        TrainDescr td = TrainDescr::getNord( tn );
        if ( !td.empty() )
            vTD.push_back( td );
    }
    CPPUNIT_ASSERT( vTD.size()==3 );
}

void TC_TrainDescr::getNovosibSupplimentary()
{
    const wstring p=
        L"<Ext.Train Time='2015-05-30 16:29'>"
        L"  <Strip ID='80000 0П'>"
        L"    <Train Num='7777' Idx='8626-094-8142' Ulen='66' Brut='6038' Local_Cargo='1' WarnSpeed='?' OH='ПB'>"
        L"      <Locomotive Series='ВЛ10' NumLoc='1177' Dor='77' Depo='7' Type='2' Kod='27' />"
        L"      <Locomotive Series='ВЛ23' NumLoc='5500' Dor='76' Depo='17' Type='3' Kod='25' />"
        L"    </Train>"
        L"  </Strip>"
        L"  <Strip ID='76000 4П'>"
        L"    <Train Num='5555' Idx='0763-697-7985' Ulen='69' Brut='2063' Local_Cargo='1' WarnSpeed='?' OH='HПC' Delay='07:06'>"
        L"      <Locomotive Series='ВЛ11' NumLoc='5500' Dor='76' Depo='17' Type='3' Kod='25' />"
        L"      <Crew Dor='76' Depo='17' Tim_Beg='2015-05-30 10:54' />"
        L"    </Train>"
        L"  </Strip>"
        L"</Ext.Train>";

    attic::a_document x_doc;
    attic::a_parse_result res = x_doc.load_wide( p );
    CPPUNIT_ASSERT( res );

    time_t moment = StrDataTime_To_UTC( L"2015-05-30 21:00");
    std::vector<TrainDescr> vTD;
    for( auto tn : x_doc.child("Ext.Train").children("Strip") )
    {
        TrainDescr td = TrainDescr::getNovosibSupplimentary( tn.child("Train"), moment );
        if ( !td.empty() )
            vTD.push_back( td );
    }
    // проверка результата
    CPPUNIT_ASSERT( 2 == vTD.size() );
    CPPUNIT_ASSERT( vTD[0].GetNumber().getNumber() == 7777 );
    CPPUNIT_ASSERT( vTD[0].Locomotives().size() == 2 );
    CPPUNIT_ASSERT( vTD[1].GetNumber().getNumber() == 5555 );
    CPPUNIT_ASSERT( vTD[1].Locomotives().size() == 1 );
    std::set <TrainCharacteristics::TrainFeature> featset;
    vTD[1].GetFeatures( featset );
    CPPUNIT_ASSERT( 1 == featset.count( TrainCharacteristics::TrainFeature::BrigGrt10 ) );
    CPPUNIT_ASSERT( 0 == featset.count( TrainCharacteristics::TrainFeature::BrigGrt11 ) );
}

void TC_TrainDescr::numberAndIndexFit()
{
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"1212-121-1212").NumberAndIndexFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(TrainDescr(L"", L"1212-121-1212").NumberAndIndexFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"1212-121-1212").NumberAndIndexFit(TrainDescr(L"", L"1212-121-1212")));
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"").NumberAndIndexFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"1212-121-1212").NumberAndIndexFit(TrainDescr(L"1000", L"")));
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"").NumberAndIndexFit(TrainDescr(L"1000", L"")));
    CPPUNIT_ASSERT(TrainDescr(L"", L"1212-121-1212").NumberAndIndexFit(TrainDescr(L"", L"1212-121-1212")));
	CPPUNIT_ASSERT(TrainDescr(L"1001", L"1212-121-1212").NumberAndIndexFit(TrainDescr(L"1000", L"1212-121-1212")));

    CPPUNIT_ASSERT(!TrainDescr(L"", L"").NumberAndIndexFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(!TrainDescr(L"1000", L"1212-121-1212").NumberAndIndexFit(TrainDescr(L"", L"")));
    CPPUNIT_ASSERT(!TrainDescr(L"1000", L"1212-121-1212").NumberAndIndexFit(TrainDescr(L"1000", L"1212-121-1213")));
	//����� ��������!! ������ ����� ������� ���������� �� �������
    CPPUNIT_ASSERT(!TrainDescr(L"1000", L"").NumberAndIndexFit(TrainDescr(L"", L"1212-121-1212")));

    CPPUNIT_ASSERT(TrainDescr(L"1001", L"").NumberAndIndexFit(TrainDescr(L"1002", L"")));
    CPPUNIT_ASSERT(!TrainDescr(L"1003", L"").NumberAndIndexFit(TrainDescr(L"1002", L"")));
}

void TC_TrainDescr::indexEqualOrNumberFit()
{
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"1212-121-1212").IndexEqualOrNumberFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(TrainDescr(L"", L"1212-121-1212").IndexEqualOrNumberFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"1212-121-1212").IndexEqualOrNumberFit(TrainDescr(L"", L"1212-121-1212")));
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"").IndexEqualOrNumberFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"1212-121-1212").IndexEqualOrNumberFit(TrainDescr(L"1000", L"")));
    CPPUNIT_ASSERT(TrainDescr(L"1000", L"").IndexEqualOrNumberFit(TrainDescr(L"1000", L"")));
    CPPUNIT_ASSERT(TrainDescr(L"", L"1212-121-1212").IndexEqualOrNumberFit(TrainDescr(L"", L"1212-121-1212")));

    CPPUNIT_ASSERT(!TrainDescr(L"", L"").IndexEqualOrNumberFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(!TrainDescr(L"1000", L"1212-121-1212").IndexEqualOrNumberFit(TrainDescr(L"", L"")));
    CPPUNIT_ASSERT(TrainDescr(L"1001", L"1212-121-1212").IndexEqualOrNumberFit(TrainDescr(L"1000", L"1212-121-1212")));
    CPPUNIT_ASSERT(!TrainDescr(L"1000", L"1212-121-1212").IndexEqualOrNumberFit(TrainDescr(L"1000", L"1212-121-1213")));
    CPPUNIT_ASSERT(!TrainDescr(L"1000", L"").IndexEqualOrNumberFit(TrainDescr(L"", L"1212-121-1212")));

    CPPUNIT_ASSERT(TrainDescr(L"1001", L"").IndexEqualOrNumberFit(TrainDescr(L"1002", L"")));
    CPPUNIT_ASSERT(!TrainDescr(L"1003", L"").IndexEqualOrNumberFit(TrainDescr(L"1002", L"")));
}

void TC_TrainDescr::deserialization()
{
    std::vector<TrainDescr> vTD;
    {
        const wstring p=
            L"<Ext.Train Time='2015-05-30 16:29'>"
            L"  <Strip ID='80000 0П'>"
            L"    <Train Num='7777' Idx='8626-094-8142' Ulen='66' Brut='6038' Local_Cargo='1' WarnSpeed='?' OH='ПB'>"
            L"      <Locomotive Series='ВЛ10' NumLoc='1177' Dor='77' Depo='7' Type='2' Kod='27' />"
            L"      <Locomotive Series='ВЛ26' NumLoc='5500' Dor='76' Depo='17' Type='3' Kod='25' />"
            L"    </Train>"
            L"  </Strip>"
            L"  <Strip ID='76000 4П'>"
            L"    <Train Num='5555' Idx='0763-697-7985' Ulen='69' Brut='2063' Local_Cargo='1' WarnSpeed='?' OH='HПC' Delay='07:06'>"
            L"      <Locomotive Series='ВЛ11' NumLoc='5500' Dor='76' Depo='17' Type='3' Kod='25' />"
            L"      <Crew Dor='76' Depo='17' Tim_Beg='2015-05-30 10:54' />"
            L"    </Train>"
            L"  </Strip>"
            L"</Ext.Train>";

        attic::a_document x_doc;
        attic::a_parse_result res = x_doc.load_wide( p );
        CPPUNIT_ASSERT( res );

        time_t moment = StrDataTime_To_UTC( L"2015-05-30 21:00");
        for( auto tn : x_doc.child("Ext.Train").children("Strip") )
        {
            TrainDescr td = TrainDescr::getNovosibSupplimentary( tn.child("Train"), moment );
            if ( !td.empty() )
            {
                attic::a_document tmpDoc("Tmp");
                tmpDoc.document_element() << td;

                TrainDescr newTd(tmpDoc.document_element());
                CPPUNIT_ASSERT(td == newTd);
                vTD.push_back(newTd);
            }
        }
    }
    // проверка результата
    CPPUNIT_ASSERT( 2 == vTD.size() );
    CPPUNIT_ASSERT( vTD[0].GetNumber().getNumber() == 7777 );
    CPPUNIT_ASSERT( vTD[0].Locomotives().size() == 2 );
    CPPUNIT_ASSERT( vTD[1].GetNumber().getNumber() == 5555 );
    CPPUNIT_ASSERT( vTD[1].Locomotives().size() == 1 );
    const Crew& crew = vTD[1].Locomotives()[0].get_crew();
    CPPUNIT_ASSERT( !crew.empty() );
}

void TC_TrainDescr::serialization()
{
    TrainDescr td(L"11111", L"1111-111-1111");
    td.SetRelLength(100);
    td.SetBrutto(200);

    // Локомотивы и бригады
    std::vector<Locomotive> locomotives;
    auto TEP80 = LocoCharacteristics::instance()->GetSeriesCode(L"ТЭП80");
    locomotives.push_back(Locomotive( LocoId(470,TEP80), 3/*депо*/, 4/*код следования*/));
    locomotives.back().set_crew( Crew(L"DRIVER_0", 100u/*номер машиниста*/) );
    locomotives.push_back(Locomotive( LocoId(471,TEP80), 3/*депо*/, 4/*код следования*/));
    locomotives.back().set_crew( Crew(L"DRIVER_1", 101u/*номер машиниста*/) );
    td.SetLocomotives(locomotives);
    td.SetEmptyCarsCount(3);
    td.SetFullCarsCount(12);
    td.SetScotchBlockCount(5);

    attic::a_document outDoc("TDTest");
    outDoc.document_element() << td;

    std::wstring text = outDoc.pretty_wstr();
    CPPUNIT_ASSERT(!text.empty());

    const attic::a_node tdNode = outDoc.document_element();
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("/TDTest[@index='1111-111-1111']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("/TDTest[@num='11111']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("/TDTest[@length='100']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("/TDTest[@weight='200']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("/TDTest[@emptyCars='3']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("/TDTest[@fullCars='12']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("/TDTest[@scotchBlock='5']").size());
    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)tdNode.select_nodes("/TDTest/Locomotive").size());
    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)tdNode.select_nodes("//Locomotive").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("//Locomotive[@NumLoc='470']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("//Locomotive[@NumLoc='471']").size());
    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)tdNode.select_nodes("//Locomotive[@Depo='3']").size());
    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)tdNode.select_nodes("//Locomotive[@Consec='4']").size());
    CPPUNIT_ASSERT_EQUAL(2u, (unsigned)tdNode.select_nodes("/TDTest/Locomotive/Crew").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("//Crew[@EngineDriver='DRIVER_0']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("//Crew[@EngineDriver='DRIVER_1']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("//Crew[@TabNum='100']").size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)tdNode.select_nodes("//Crew[@TabNum='101']").size());

    TrainDescr readTd(outDoc.document_element());
    CPPUNIT_ASSERT(readTd == td);
}

void TC_TrainDescr::areNumbersPaired()
{
    CPPUNIT_ASSERT(TrainNumber(1).isPairedWith(TrainNumber(2)));
    CPPUNIT_ASSERT(TrainNumber(2).isPairedWith(TrainNumber(1)));
    CPPUNIT_ASSERT(!TrainNumber(3).isPairedWith(TrainNumber(2)));
    CPPUNIT_ASSERT(!TrainNumber(2).isPairedWith(TrainNumber(3)));
    CPPUNIT_ASSERT(!TrainNumber(0).isPairedWith(TrainNumber(1)));
    CPPUNIT_ASSERT(!TrainNumber(0).isPairedWith(TrainNumber(0)));
    CPPUNIT_ASSERT(!TrainNumber(1).isPairedWith(TrainNumber(1)));
    CPPUNIT_ASSERT(!TrainNumber(2).isPairedWith(TrainNumber(2)));
    CPPUNIT_ASSERT(!TrainNumber(10).isPairedWith(TrainNumber(11)));
    CPPUNIT_ASSERT(!TrainNumber(11).isPairedWith(TrainNumber(10)));
}

void TC_TrainDescr::supplement()
{
    std::wstring const c_index = L"1001-111-1111";

    {
        TrainDescr td1001(L"1001", c_index);
        TrainDescr td1002(L"1002", L"");
    
        CPPUNIT_ASSERT(td1002.SupplementWith(td1001));
        CPPUNIT_ASSERT_EQUAL(c_index, td1001.GetIndex().str());
        CPPUNIT_ASSERT_EQUAL(c_index, td1002.GetIndex().str());
        CPPUNIT_ASSERT_EQUAL(1001u, td1001.GetNumber().getNumber());
        CPPUNIT_ASSERT_EQUAL(1002u, td1002.GetNumber().getNumber());
    }

    {
        TrainDescr td1001(L"1003", c_index);
        TrainDescr td1002(L"1002", L"");

        CPPUNIT_ASSERT(!td1002.SupplementWith(td1001));
        CPPUNIT_ASSERT_EQUAL(c_index, td1001.GetIndex().str());
        CPPUNIT_ASSERT_EQUAL(std::wstring(), td1002.GetIndex().str());
        CPPUNIT_ASSERT_EQUAL(1003u, td1001.GetNumber().getNumber());
        CPPUNIT_ASSERT_EQUAL(1002u, td1002.GetNumber().getNumber());
    }

    {
        const TrainDescr tdJ(L"J2235", c_index);
        TrainDescr tdV(L"V2235", L"");

        CPPUNIT_ASSERT(tdV.SupplementWith(tdJ));
        CPPUNIT_ASSERT_EQUAL(c_index, tdV.GetIndex().str());
        CPPUNIT_ASSERT(L"V2235"==tdV.GetNumber().getString());
    }
}

// сравнение дескрипторов без учета локомотивов
void TC_TrainDescr::equalExceptLoco()
{
    TrainDescr tdA(L"1234", L"9876-543-2109");
    {
        std::vector<Locomotive> locomotives;
        auto TEP80 = LocoCharacteristics::instance()->GetSeriesCode(L"ТЭП80");
        locomotives.push_back( Locomotive( LocoId(470,TEP80), 3/*депо*/, 4/*код следования*/) );
        tdA.SetLocomotives(locomotives);
    }
    TrainDescr tdB = tdA;
    CPPUNIT_ASSERT( tdA == tdB );
    // изменить только код перевозчика в локомотиве B
    {
        std::vector<Locomotive> locomotives = tdB.Locomotives();
        unsigned cr = locomotives.back().getCarrier();
        locomotives.back().setCarrier( cr + 17 );
        tdB.SetLocomotives(locomotives);
    }
    CPPUNIT_ASSERT( tdA != tdB );
    CPPUNIT_ASSERT( TrainDescr::equalExceptLocomotives(tdA, tdB) );
}
