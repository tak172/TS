#include "stdafx.h"
#include "TC_HemTabInfo.h"
// #include "../helpful/DistrictGuide.h"
// #include "../helpful/EsrGuide.h"
#include "../helpful/HemTabInfo.h"
#include "../helpful/Serialization.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_HemTabInfo );

static std::string hemtab_data()
{
    return ToUtf8(
        L"<HemTab file='xxx' trash='yyy' >"
        L"  <HemWin filename='DGR-Vt' district_code='09180-09820' tabname='DGR-Vt'>"
        L"    <!--Daugavpils-Šķ.-->"
        L"    <Station ESR='11010'>"
        L"      <Park Id='1'>"
        L"        <Way Id='1' />"
        L"        <Way Id='2' />"
        L"        <Way Id='3' />"
        L"      </Park>"
        L"    </Station>"
        L"    <OddNum spanlist='09190:11420,09181:09251' />"
        L"  </HemWin>"
        L"  <HemWin filename='other*DGR-Vt' district_code='other*09180-09820' tabname='other*DGR-Vt'>"
        L"    <!--Daugavpils-Šķ.-->"
        L"    <Station ESR='11010'>"
        L"      <Park Id='1'>"
        L"        <Way Id='1' />"
        L"        <Way Id='3' />"
        L"      </Park>"
        L"    </Station>"
        L"    <EvenNum spanlist='22222:33333,44444:55555' />"
        L"  </HemWin>"
        L"</HemTab>"
        );
}

// template<>
// std::shared_ptr<DistrictGuide> deserialize( const std::string & strcontent ) {
//     std::shared_ptr<DistrictGuide> res;
//     attic::a_document xdoc;
//     xdoc.load_utf8( strcontent );
//     auto dgNode = xdoc.document_element();
//     if ( dgNode && dgNode.name_is( DistrictGuide::tag() ) ) {
//         res.reset( new DistrictGuide() );
//         res->deserialize( dgNode );
//     }
//     return res;
// }
// 
// 
// template<>
// std::string serialize( const HemTabInfo& hti )
// {
//     attic::a_document cmpDoc("xx");
//     attic::a_node temp = cmpDoc.document_element().append_child( DistrictGuide::tag() );
//     dg.serialize( temp );
//     return temp.to_str();
// }

void TC_HemTabInfo::InOut()
{
    // Среда передачи
    HemTabInfo hti1;
    hti1.deserialize( hemtab_data() );
    // отправка
    std::string s = hti1.serialize();
    // получение
    HemTabInfo hti2;
    hti2.deserialize( s );

    // сравнение
    std::string s1,s2;
    s1 = hti1.serialize();
    s2 = hti2.serialize();
    CPPUNIT_ASSERT( s1 == s2 );
}
