#include "stdafx.h"

#include "../helpful/Attic.h"
#include "../helpful/Dictum.h"
#include "TC_WndActorHemParams.h"
#include "../Actor/HemDraw/HemType.h"
#include "../Actor/HemDraw/WndActorHemParams.h"
#include "../Actor/HemDraw/UserHemMap.h"
#include "../Actor/HemDraw/HemMap.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_WndActorHemParams );

time_t ActorCurrentTime()
{return 0;}

int GetEsrAccess( const EsrKit& /*esr*/ )
{return CONTROL_ALL;}

void TC_WndActorHemParams::Load()
{
    const std::wstring hemwin_acfg_text = 
        L"<HemWin filename='Некий график' tabname='Длинное название для элемента' tabview='Y' districtname='Название участка' cx='0' cy='0' dx='2340' dy='980' bframe='1' >"
        L"<Station ESR='11111' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"<Way Id='5' />" L"</Park>"
        L"</Station>"
        L"<Station ESR='22222' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"<Way Id='6' />" L"</Park>"
        L"</Station>"
        L"<Station ESR='55555' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"<Way Id='6' />" L"</Park>"
        L"</Station>"
        L"<Break/>"
        L"<Station ESR='77777' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"<Way Id='6' />" L"</Park>"
        L"</Station>"
        L"<Station ESR='99999' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"</Park>"
        L"</Station>"
        L"</HemWin>";

    attic::a_document x_doc;
    x_doc.load_wide( hemwin_acfg_text );

    WndActorHemParams params( x_doc.document_element(), esrGuide, HTO_LEFT_RIGHT );

    CPPUNIT_ASSERT( params.vHemItem.size()==8 );
    CPPUNIT_ASSERT( params.vHemItem.at(4)->get_esr()==EsrKit(55555,0) );
    HemdrawType::HemItemPosPtr pIt = params.vHemItem.at(1);
    CPPUNIT_ASSERT( pIt->span(EsrKit(11111,22222)));
    CPPUNIT_ASSERT ( pIt->Data==1000);
    pIt = params.vHemItem.at(3);
    CPPUNIT_ASSERT( pIt->span(EsrKit(22222,55555)));
    CPPUNIT_ASSERT ( pIt->Data==9000);
    pIt = params.vHemItem.at(5);
    CPPUNIT_ASSERT( pIt->isBreak());
    CPPUNIT_ASSERT(pIt->get_parks().size()==1 );
    const StationPark& park = *pIt->get_parks().front();
    CPPUNIT_ASSERT(park.get_ways().size()==5);
    CPPUNIT_ASSERT(park.get_ways().back()->get_id()==6);
    CPPUNIT_ASSERT( params.vHemItem.back()->get_esr()==EsrKit(99999,0) );
}

void TC_WndActorHemParams::CorrectMap()
{
    const std::wstring hemwin_acfg_text = 
        L"<HemWin filename='Некий график' tabname='Длинное название для элемента' tabview='Y' districtname='Название участка' cx='0' cy='0' dx='2340' dy='980' bframe='1' >"
        L"<Station ESR='11111' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"<Way Id='5' />" L"</Park>"
        L"</Station>"
        L"<Station ESR='22222' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"<Way Id='6' />" L"</Park>"
        L"</Station>"
        L"<Station ESR='55555' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"<Way Id='6' />" L"</Park>"
        L"</Station>"
        L"<Break/>"
        L"<Station ESR='77777' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"<Way Id='6' />" L"</Park>"
        L"</Station>"
        L"<Station ESR='99999' open='0' coord='0' >"
        L"<Park Id='1' >" L"<Way Id='3' />" L"<Way Id='1' />" L"<Way Id='2' />" L"<Way Id='4' />" L"</Park>"
        L"</Station>"
        L"</HemWin>";

    attic::a_document x_doc;
    x_doc.load_wide( hemwin_acfg_text );

    WndActorHemParams params( x_doc.document_element(), esrGuide, HTO_LEFT_RIGHT );
    params.set_id(1);

    CPPUNIT_ASSERT( params.vHemItem.size()==8 );
    CPPUNIT_ASSERT( params.vHemItem.at(4)->get_esr()==EsrKit(55555,0) );
    HemdrawType::HemItemPosPtr pIt = params.vHemItem.at(3);
    CPPUNIT_ASSERT( pIt->span(EsrKit(22222,55555)));
    CPPUNIT_ASSERT ( pIt->Data==9000);
    CPPUNIT_ASSERT( params.vHemItem.at(5)->isBreak());
    CPPUNIT_ASSERT( params.vHemItem.back()->get_esr()==EsrKit(99999,0) );

    params.correct_map( hem_topology, esrGuide, HTO_LEFT_RIGHT );
    CPPUNIT_ASSERT( params.vHemItem.size()==14 );
    pIt = params.vHemItem.at(3);
    CPPUNIT_ASSERT( pIt->span(EsrKit(22222,33333)));
    CPPUNIT_ASSERT ( pIt->Data==2000);
    CPPUNIT_ASSERT( params.vHemItem.at(4)->station(EsrKit(33333)));
    pIt = params.vHemItem.at(5);
    CPPUNIT_ASSERT( pIt->span(EsrKit(33333,44444)));
    CPPUNIT_ASSERT ( pIt->Data==3000);
    CPPUNIT_ASSERT( params.vHemItem.at(6)->station(EsrKit(44444)));
    CPPUNIT_ASSERT( params.vHemItem.at(7)->span(EsrKit(44444,55555)));
    CPPUNIT_ASSERT( params.vHemItem.at(8)->station(EsrKit(55555)));
    CPPUNIT_ASSERT( params.vHemItem.at(9)->isBreak());
    CPPUNIT_ASSERT( params.vHemItem.at(10)->span(EsrKit(77777,88888)));
    CPPUNIT_ASSERT( params.vHemItem.at(11)->station(EsrKit(88888)));
    CPPUNIT_ASSERT( params.vHemItem.at(12)->span(EsrKit(88888,99999)));
    CPPUNIT_ASSERT( params.vHemItem.back()->station(EsrKit(99999)));
}

void TC_WndActorHemParams::setUp()
{
    PicketingInfo pi;
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(1,100)));
    esrGuide.setNameAndAxis(EsrKit(11111), L"11111", pi);
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(2,100)));
    esrGuide.setNameAndAxis(EsrKit(22222), L"22222", pi);
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(4,100)));
    esrGuide.setNameAndAxis(EsrKit(33333), L"33333", pi);
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(7,100)));
    esrGuide.setNameAndAxis(EsrKit(44444), L"44444", pi);
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(11,100)));
    esrGuide.setNameAndAxis(EsrKit(55555), L"55555", pi);
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(16,100)));
    esrGuide.setNameAndAxis(EsrKit(66666), L"66666", pi);
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(17,100)));
    esrGuide.setNameAndAxis(EsrKit(77777), L"77777", pi);
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(24,100)));
    esrGuide.setNameAndAxis(EsrKit(88888), L"88888", pi);
    pi.SetCoord(rwRuledCoord(L"xxx", rwCoord(29,100)));
    esrGuide.setNameAndAxis(EsrKit(99999), L"99999", pi);

    std::vector<EsrKit> vEsr;
    vEsr.push_back( EsrKit(11111) );
    vEsr.push_back( EsrKit(22222) );

    hem_topology.vEsrRoutes.push_back( EsrRoute(EsrKit(11111), EsrKit(22222), 1, vEsr) );    

    vEsr.clear();
    vEsr.push_back( EsrKit(22222) );
    vEsr.push_back( EsrKit(33333) );
    vEsr.push_back( EsrKit(44444) );
    vEsr.push_back( EsrKit(55555) );
    hem_topology.vEsrRoutes.push_back( EsrRoute(EsrKit(22222), EsrKit(55555), 1, vEsr) );    
    
    vEsr.clear();
    vEsr.push_back( EsrKit(77777) );
    vEsr.push_back( EsrKit(88888) );
    vEsr.push_back( EsrKit(99999) );
    hem_topology.vEsrRoutes.push_back( EsrRoute(EsrKit(77777), EsrKit(99999), 1, vEsr) );    
}

void TC_WndActorHemParams::tearDown()
{
   
}
