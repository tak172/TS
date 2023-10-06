#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_DistrictGuide.h"
#include "../helpful/ControlMode.h"
#include "../helpful/DistrictGuide.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_DistrictGuide );

void TC_DistrictGuide::LoadSave()
{
    const std::wstring district_str = 
        L"<DistrictList>"
        L"<District code='11111-44444' name='Участок 1'>"
        L"<Involve esr='11111'/>"
        L"<Involve esr='22222'/>"
        L"<Involve esr='33333'/>"
        L"</District>"
        L"<District code='55555-77777' name='Участок 2'>"
        L"<Involve esr='55555'/>"
        L"<Involve esr='66666'/>"
        L"<Involve esr='66666:77777'/>"
        L"</District>"
        L"</DistrictList>";

    attic::a_document x_doc;
    CPPUNIT_ASSERT( x_doc.load_wide( district_str ) );
    std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

    DistrictGuide dg;
    dg.deserialize( x_doc.document_element() );

    attic::a_document y_doc;
    dg.serialize(y_doc.set_document_element("DistrictList"));
    std::wstring str_dst = FromUtf8(y_doc.document_element().to_str());

    auto mismres = std::mismatch( str_src.cbegin(), str_src.cend(), str_dst.cbegin(), str_dst.cend());

    CPPUNIT_ASSERT( str_src == str_dst );
}

void TC_DistrictGuide::testInvolve()
{
    const std::wstring district_str = 
        L"<DistrictList>"
        L"<District code='11111-44444' name='Участок 1'>"
        L"<Involve esr='11111'/>"
        L"<Involve esr='22222'/>"
        L"<Involve esr='33333'/>"
        L"</District>"
        L"<District code='55555-77777' name='Участок 2'>"
        L"<Involve esr='55555'/>"
        L"<Involve esr='66666'/>"
        L"<Involve esr='66666:77777'/>"
        L"</District>"
        L"</DistrictList>";

    attic::a_document x_doc;
    CPPUNIT_ASSERT( x_doc.load_wide( district_str ) );

    DistrictGuide dg;
    dg.deserialize( x_doc.document_element() );

    CPPUNIT_ASSERT ( dg.get_district(L"11111-44444") );
    CDistrict district = *dg.get_district( L"11111-44444" );
    CPPUNIT_ASSERT ( district.get_control(EsrKit(11111))==CONTROL_ALL );
    CPPUNIT_ASSERT ( district.get_control( EsrKit(22222) )==CONTROL_ALL);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(33333) )==CONTROL_ALL);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(44444) )==CONTROL_NONE);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(11111, 22222) )==CONTROL_ALL);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(22222, 33333) )==CONTROL_ALL);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(33333, 44444) )==CONTROL_THREAD);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(44444, 55555) )==CONTROL_NONE);

    CPPUNIT_ASSERT ( dg.get_district(L"55555-77777") );
    district = *dg.get_district(L"55555-77777");
    CPPUNIT_ASSERT ( district.get_control( EsrKit(55555) )==CONTROL_ALL);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(66666) )==CONTROL_ALL);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(55555, 66666) )==CONTROL_ALL);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(66666,77777) )==CONTROL_ALL);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(77777) )==CONTROL_THREAD);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(77777,88888) )==CONTROL_NONE);
    CPPUNIT_ASSERT ( district.get_control( EsrKit(44444,55555) )==CONTROL_THREAD);
}

void TC_DistrictGuide::testGetSpan()
{
    const std::wstring district_str = 
    L"<DistrictList>"
    L"<District code='09180-09860' name='Jelgava-Liepaja'>"  L"<!-- елгава-лиепая - вкладка лиепая-елгава -->"
    L"<Involve esr='09280:09180'/>"   L"<!--глуда-елгава -->"
    L"<Involve esr='09280'/>"    L"<!-- глуда -->"
    L"<Involve esr='09290'/>"    L"<!-- добеле -->"
    L"<Involve esr='09303'/>"    L"<!-- биксти -->"
    L"<Involve esr='09320'/>"    L"<!-- бросени -->"
    L"<Involve esr='09330'/>"    L"<!-- салдус  -->"
    L"<Involve esr='09340'/>"    L"<!-- скрунда -->"
    L"<Involve esr='09350'/>"    L"<!-- калвене -->"
    L"<Involve esr='09351'/>"    L"<!-- илмая -->"
    L"<Involve esr='09355'/>"    L"<!-- торе -->"
    L"<Involve esr='09860'  border='Y' target='Y'/>"    L"<!-- лиепая -->"
    L"<Involve esr='09170:09180'/>"   L"<!--мейтене-елгава -->"
    L"<Involve esr='09170' border='Y'/>"    L"<!-- мейтене -->"
    L"<Involve esr='09170:12600'/>"    L"<!-- мейтене -джониски-->"
    L"<Involve esr='09940'/>"    L"<!--бене -->"
    L"<Involve esr='09960' border='Y'/>"  L"<!--реньге -->"
    L"<Involve esr='09960:12640'  />"    L"<!--мазейки -->"
    L"</District>";

    attic::a_document x_doc;
    CPPUNIT_ASSERT( x_doc.load_wide( district_str ) );

    DistrictGuide dg;
    dg.deserialize( x_doc.document_element() );

    auto pDistrict = dg.get_district(L"09180-09860");
    
    std::set<EsrKit> vEsrLegal;
    vEsrLegal.insert(EsrKit(9180,9280));
    vEsrLegal.insert(EsrKit(9280,9290));
    vEsrLegal.insert(EsrKit(9290,9303));
    vEsrLegal.insert(EsrKit(9303,9320));
    vEsrLegal.insert(EsrKit(9320,9330));
    vEsrLegal.insert(EsrKit(9330,9340));
    vEsrLegal.insert(EsrKit(9340,9350));
    vEsrLegal.insert(EsrKit(9350,9351));
    vEsrLegal.insert(EsrKit(9351,9355));
    vEsrLegal.insert(EsrKit(9355,9860));
    vEsrLegal.insert(EsrKit(9170,9180));
    vEsrLegal.insert(EsrKit(9170,12600));
    vEsrLegal.insert(EsrKit(9280,9940));
    vEsrLegal.insert(EsrKit(9940,9960));
    vEsrLegal.insert(EsrKit(9960,12640));
    
    auto vSpan = pDistrict->get_esr(vEsrLegal);
    auto pGludaBene = std::find(vSpan.cbegin(), vSpan.cend(), EsrKit(9280, 9940));
    CPPUNIT_ASSERT( pGludaBene!=vSpan.begin());
}

void TC_DistrictGuide::testStationList()
{
    const std::wstring district_str = 
        L"<DistrictList>"
        L"<District code='11000-11042' name='Bigosovas iecirknis'>"      L"<!--Даугавпилс-Индра - вкладка даугавпилс-бигосова -->"
        L"<Involve esr='16169' border='Y' target='Y'/>"        L"<!--  Бигосово стык  -->"
        L"<Involve esr='16169:16170'/>"        L"<!--  Бигосово-Бигосово стык  -->"
        L"<Involve esr='11042'/>"       L"<!-- Индра   -->"
        L"<Involve esr='11041'/>"        L"<!--Ниедрица   -->"
        L"<Involve esr='11052'/>"       L"<!-- Скайста   -->"
        L"<Involve esr='11050'/>"        L"<!-- Краслава  -->"
        L"<Involve esr='11051'/>"        L"<!-- Силава  -->"
        L"<Involve esr='11060'/>"        L"<!--  Извалда -->"
        L"<Involve esr='11062'/>"         L"<!-- Науджени -->"
        L"<Involve esr='11063:11062'/>"       L"<!--науджени-401км -->"
        L"<Involve esr='09923'/>"        L"<!-- Aluksne   -->"
        L"<Involve esr='11154'/>"        L"<!--  Kalniena  -->"
        L"<Involve esr='11160'/>"        L"<!--  Гулбене  -->"
        L"<Involve esr='11710:11160'/>"        L"<!-- Гулбене  -Мадона   -->"
        L"<Involve esr='11710'/>"        L"<!-- Мадона   -->"
        L"<Involve esr='11750'/>"         L"<!-- Яункалснава  -->"
        L"<Involve esr='11760:11750'/>"    L"<!-- Плявинас-Яункалснава -->"
        L"<Involve esr='11780'/>"         L"<!-- Весета  -->"
        L"</District>"
        L"</DistrictList>";

    attic::a_document x_doc;
    CPPUNIT_ASSERT( x_doc.load_wide( district_str ) );

    DistrictGuide dg;
    dg.deserialize( x_doc.document_element() );

    auto pDistrict = dg.get_district(L"11000-11042");
    CPPUNIT_ASSERT( pDistrict );

    //не включая пограничные станции
    auto vStations = pDistrict->get_stations(false);
    CPPUNIT_ASSERT( vStations.size()== 14 );

    auto pBigosovo = std::find(vStations.cbegin(), vStations.cend(), EsrKit(16170));

    CPPUNIT_ASSERT( pBigosovo==vStations.cend());
    
    //включая пограничные станции
    vStations = pDistrict->get_stations(true);
    CPPUNIT_ASSERT( vStations.size()== 17 );

    pBigosovo = std::find(vStations.cbegin(), vStations.cend(), EsrKit(16170));
    CPPUNIT_ASSERT( pBigosovo!=vStations.cend());
}

void TC_DistrictGuide::directCreate()
{
    EsrKit invol[] = { EsrKit(100), EsrKit(200), EsrKit(300), EsrKit(300,400) };
    EsrKit bord[]  = { EsrKit(100),              EsrKit(300),                EsrKit(400) };
    EsrKit targ[]  = {              EsrKit(200),                                 EsrKit(500) };

    std::vector<EsrKit> vInv( std::begin(invol), std::end(invol) );
    std::vector<EsrKit> vBrd( std::begin(bord), std::end(bord) );
    std::vector<EsrKit> vTrg( std::begin(targ), std::end(targ) );
    std::vector<EsrKit> vAln;

    CDistrict distr(L"DiName", L"DiCode", false, vInv, vBrd, vTrg, vAln );

    CPPUNIT_ASSERT( distr.get_name() == L"DiName" );
    CPPUNIT_ASSERT( distr.get_code() == L"DiCode" );
    CPPUNIT_ASSERT( distr.get_esrs() == vInv );

    CPPUNIT_ASSERT( distr.is_obligatory_border( EsrKit(100) ) );
    CPPUNIT_ASSERT( distr.is_obligatory_border( EsrKit(300) ) );
    CPPUNIT_ASSERT( !distr.is_obligatory_border( EsrKit(400) ) );

    CPPUNIT_ASSERT( distr.getTargetStations().size() == 1 );
    CPPUNIT_ASSERT( distr.is_target( EsrKit(200) ) );
}
