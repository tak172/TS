#include "stdafx.h"

#include "TC_EsrGuide.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/ParkWayKit.h"
#include "../helpful/rwCoord.h"
#include "../StatBuilder/basic_defs.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_EsrGuide );

/*
проверить класс EsrGuide
*/

void TC_EsrGuide::GetSet()
{
    EsrGuide eg;

    CPPUNIT_ASSERT( !eg.isKnown(k1) );
    CPPUNIT_ASSERT( !eg.isKnown(k12) );
    CPPUNIT_ASSERT( !eg.isKnown(k2) );
    CPPUNIT_ASSERT( !eg.isKnown(k23) );
    CPPUNIT_ASSERT( !eg.isKnown(k3) );

    eg.setNameAndAxis( k1, L"Бубново", p1 );
    eg.setTermWay( k1, ParkWayKit(1,1), EsrGuide::Passenger );

    eg.setNameAndAxis( k2, L"Трефная", p2 );
    eg.setTermWay( k2, ParkWayKit(2), EsrGuide::Major );
    eg.setTermWay( k2, ParkWayKit(3,82), EsrGuide::Major|EsrGuide::Passenger );
    eg.setTermWay( k2, ParkWayKit(3,37), EsrGuide::Passenger|EsrGuide::Embarkation );

    eg.setNameAndAxis( k3, L"Куличики", p3 );

    PicketingInfo left12, right12;
    left12.SetCoord(rwRuledCoord(L"xxx",rwCoord(35,350)));
    right12.SetCoord(rwRuledCoord(L"xxx",rwCoord(47,470)));
    eg.setSpan( k12, Oddness::EVEN, false );
    eg.setSpanWay( k12, 2, left12, right12 );
    eg.setSpanWay( k12, 1, right12, left12 );

    CPPUNIT_ASSERT( eg.isKnown(k1) );
    CPPUNIT_ASSERT( eg.getName(k1)==L"Бубново" );
    CPPUNIT_ASSERT( eg.getAxis(k1)==p1 );
    CPPUNIT_ASSERT( eg.getTermWays(k1).size()==1 );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(1,1),EsrGuide::Passenger) );

    CPPUNIT_ASSERT( eg.isKnown(k2) );
    CPPUNIT_ASSERT( eg.getName(k2)==L"Трефная" );
    CPPUNIT_ASSERT( eg.getAxis(k2)==p2 );
    CPPUNIT_ASSERT( eg.getTermWays(k2).size()==3 );
    CPPUNIT_ASSERT( eg.isSpecialized(k2,ParkWayKit(0,2),EsrGuide::Major ) );
    CPPUNIT_ASSERT( eg.isSpecialized(k2,ParkWayKit(3,37),EsrGuide::Passenger) );
    CPPUNIT_ASSERT( eg.isSpecialized(k2,ParkWayKit(3,37),EsrGuide::Embarkation) );
    CPPUNIT_ASSERT( !eg.isSomehowSpecialized(k2,ParkWayKit(6,66)) );

    CPPUNIT_ASSERT( eg.isKnown(k12) );
    CPPUNIT_ASSERT( eg.getName(k12)==L"Бубново - Трефная" );
    CPPUNIT_ASSERT( eg.getSpanWays(k12).size() == 2 );
    CPPUNIT_ASSERT( eg.getSpanWays(k12).front()== 1 );
    CPPUNIT_ASSERT( eg.getSpanWays(k12).back() == 2  );
    CPPUNIT_ASSERT( eg.getSpanWayOddness(k12,1)==Oddness::ODD  );
    CPPUNIT_ASSERT( eg.getSpanWayOddness(k12,2)==Oddness::EVEN  );
    CPPUNIT_ASSERT( eg.getSpanWayOddness(k12,666)==Oddness::UNDEF  );
    CPPUNIT_ASSERT( eg.getSpanWayBound(k12,1) == std::make_pair( right12, left12 ) );
    CPPUNIT_ASSERT( eg.getSpanWayBound(k12,2) == std::make_pair( left12, right12 ) );
    CPPUNIT_ASSERT( eg.getSpanWayBound(k12,666) == std::make_pair( PicketingInfo(), PicketingInfo() ) );

    CPPUNIT_ASSERT( eg.isKnown(k3) );
    CPPUNIT_ASSERT( eg.getName(k3)==L"Куличики" );
    CPPUNIT_ASSERT( eg.getAxis(k3)==p3 );

    CPPUNIT_ASSERT( !eg.isKnown(k23) );
    CPPUNIT_ASSERT( eg.getName(k23)==L"Трефная - Куличики" );
}

void TC_EsrGuide::LoadSaveCompatible()
{
    EsrGuide eg;
    std::wstring source;
    {
        const char * eg_text1251 = 
            "<EsrGuide>"
            "  <EsrGuide kit='01111' name='Бубново' picketing1_val='101~77' picketing1_comm='Буй'  picketing2_val='710~52' picketing2_comm='Кадуй' >"
            "    <Way waynum='7' parknum='23' />"
            "  </EsrGuide>"
            "  <EsrGuide kit='02222' name='Трефная' picketing1_val='707~11' picketing1_comm='Кадуй' >"
            "    <Way waynum='2' detail='1'/>"
            "    <Way waynum='1' parknum='5' />"
            "  </EsrGuide>"
            "  <!-- перегон Бубново - Трефная -->"
            "  <SpanGuide kit='01111:02222' orientation='odd'>"
            "    <Way wayNum='1' >"
            "        <rwcoord picketing1_val='222~22' picketing1_comm='yyy' />"
            "        <rwcoord picketing1_val='111~11' picketing1_comm='xxx' />"
            "    </Way>"
            "    <Way wayNum='2' >"
            "        <rwcoord picketing1_val='111~11' picketing1_comm='xxx' />"
            "        <rwcoord picketing1_val='222~22' picketing1_comm='yyy' />"
            "    </Way>"
            "    <Way wayNum='4' />"
            "  </SpanGuide>"
            "  <!-- перегон ? [09990] - ? [09997] -->"
            "  <SpanGuide kit='09990:09997' orientation='even' >"
            "    <Way wayNum='1' />"
            "    <Way wayNum='4' />"
            "  </SpanGuide>"
            "</EsrGuide>";
        attic::a_document x_doc;
        x_doc.load_wide( From1251( eg_text1251 ) );
        eg.load_from(x_doc.document_element());

        attic::a_node corr = x_doc.document_element().child("EsrGuide").next_sibling().child("Way");
        corr.remove_attribute("detail");
        corr.brief_attribute("major",true);
        source = x_doc.pretty_wstr();
    }

    CPPUNIT_ASSERT( eg.isKnown(k1) );
    CPPUNIT_ASSERT( eg.getName(k1)==L"Бубново" );
    CPPUNIT_ASSERT( eg.getAxis(k1)==p1 );
    CPPUNIT_ASSERT( eg.getTermWays(k1).size()==1 );
    CPPUNIT_ASSERT( !eg.isSomehowSpecialized(k1,ParkWayKit(23,7)) );

    CPPUNIT_ASSERT( eg.isKnown(k2) );
    CPPUNIT_ASSERT( eg.getName(k2)==L"Трефная" );
    CPPUNIT_ASSERT( eg.getAxis(k2)==p2 );
    CPPUNIT_ASSERT( eg.getTermWays(k2).size()==2 );
    CPPUNIT_ASSERT( eg.isSpecialized(k2,ParkWayKit(0,2), EsrGuide::Major) );
    CPPUNIT_ASSERT( !eg.isSomehowSpecialized(k2,ParkWayKit(5,1)) );

    auto waynums = eg.getSpanWays(k12);
    CPPUNIT_ASSERT( waynums.size() == 3 );
    CPPUNIT_ASSERT( count(waynums.begin(), waynums.end(),4) );
    CPPUNIT_ASSERT( count(waynums.begin(), waynums.end(),2) );
    CPPUNIT_ASSERT( count(waynums.begin(), waynums.end(),1) );
    CPPUNIT_ASSERT( eg.getSpanWayOddness(k12,2) == Oddness::EVEN );
    CPPUNIT_ASSERT( eg.getSpanWayOddness(k12,1) == Oddness::ODD );
    auto bound = eg.getSpanWayBound(k12,1);
    rwRuledCoord pick;
    CPPUNIT_ASSERT( bound.first.GetFirstCoord(pick) );
    CPPUNIT_ASSERT( pick.ruler() == L"yyy" );
    CPPUNIT_ASSERT( pick.coord() == rwCoord(222,22) );
    CPPUNIT_ASSERT( bound.second.GetFirstCoord(pick) );
    CPPUNIT_ASSERT( pick.ruler() == L"xxx" );
    CPPUNIT_ASSERT( pick.coord() == rwCoord(111,11) );

    attic::a_document y_doc;
    y_doc.set_document_element( EsrGuide::tag() );
    eg.save_to( y_doc.document_element() );
    std::wstring renew = y_doc.pretty_wstr();
    auto mism = std::mismatch(source.begin(), source.end(), renew.begin(), renew.end() );
    CPPUNIT_ASSERT( source == renew );
}

void TC_EsrGuide::LoadSaveSpecialization()
{
    EsrGuide eg;
    std::wstring source;
    {
        const char * eg_text1251 = 
            "<EsrGuide>"
            "  <EsrGuide kit='01111' name='Бубново' picketing1_val='101~77' picketing1_comm='Буй'  picketing2_val='710~52' picketing2_comm='Кадуй' >"
            "    <Way waynum='1' parknum='7' major='Y' />"
            "    <Way waynum='2' parknum='7' major='Y' passenger='Y' />"
            "    <Way waynum='3' parknum='7' major='Y' passenger='Y' embarkation='Y' />"
            "    <Way waynum='4' parknum='7'           passenger='Y' embarkation='Y' />"
            "    <Way waynum='5' parknum='7'                         embarkation='Y' />"
            "    <Way waynum='6' parknum='7' />"
            "  </EsrGuide>"
            "  <EsrGuide kit='02222' name='Трефная' picketing1_val='707~11' picketing1_comm='Кадуй' >"
            "    <Way waynum='1' parknum='5' />"
            "  </EsrGuide>"
            "  <!-- перегон Бубново - Трефная -->"
            "  <SpanGuide kit='01111:02222' orientation='odd'>"
            "    <Way wayNum='1' >"
            "        <rwcoord picketing1_val='222~22' picketing1_comm='yyy' />"
            "        <rwcoord picketing1_val='111~11' picketing1_comm='xxx' />"
            "    </Way>"
            "  </SpanGuide>"
            "</EsrGuide>";
        attic::a_document x_doc;
        x_doc.load_wide( From1251( eg_text1251 ) );
        source = x_doc.pretty_wstr();
        eg.load_from(x_doc.document_element());
    }

    CPPUNIT_ASSERT( eg.isKnown(k1) );
    CPPUNIT_ASSERT( eg.getName(k1)==L"Бубново" );
    CPPUNIT_ASSERT( eg.getAxis(k1)==p1 );
    CPPUNIT_ASSERT( eg.getTermWays(k1).size()==6 );
    CPPUNIT_ASSERT( eg.isSomehowSpecialized(k1,ParkWayKit(7,1)) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,1),EsrGuide::Major) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,2),EsrGuide::Major) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,2),EsrGuide::Passenger) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,3),EsrGuide::Major) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,3),EsrGuide::Passenger) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,3),EsrGuide::Embarkation) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,4),EsrGuide::Passenger) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,4),EsrGuide::Embarkation) );
    CPPUNIT_ASSERT( eg.isSpecialized(k1,ParkWayKit(7,5),EsrGuide::Embarkation) );
    CPPUNIT_ASSERT( !eg.isSomehowSpecialized(k1,ParkWayKit(7,6)) );

    CPPUNIT_ASSERT( eg.isKnown(k2) );
    CPPUNIT_ASSERT( eg.getName(k2)==L"Трефная" );
    CPPUNIT_ASSERT( eg.getAxis(k2)==p2 );
    CPPUNIT_ASSERT( eg.getTermWays(k2).size()==1 );
    CPPUNIT_ASSERT( !eg.isSomehowSpecialized(k2,ParkWayKit(5,1)) );

    auto waynums = eg.getSpanWays(k12);
    CPPUNIT_ASSERT( waynums.size() == 1 );
    CPPUNIT_ASSERT( count(waynums.begin(), waynums.end(),1) );
    CPPUNIT_ASSERT( eg.getSpanWayOddness(k12,1) == Oddness::UNDEF );

    attic::a_document y_doc;
    y_doc.set_document_element( EsrGuide::tag() );
    eg.save_to( y_doc.document_element() );
    std::wstring renew = y_doc.pretty_wstr();
    auto mism = std::mismatch(source.begin(), source.end(), renew.begin(), renew.end() );
    CPPUNIT_ASSERT( source == renew );
}


void TC_EsrGuide::GetSpanForTerminals()
{
    EsrGuide eg;

    eg.setNameAndAxis( k1, L"Бубново", p1 );
    eg.setNameAndAxis( k2, L"Трефная", p2 );
    eg.setNameAndAxis( k3, L"Куличики", p3 );
    eg.setSpan( k12, Oddness::EVEN, false );
    eg.setSpan( k23, Oddness::ODD, true );

    // запрос на перегоны для станций
    std::vector<EsrKit> myTerms;
    myTerms.push_back( k3 );
    myTerms.push_back( k23 );// не станция
    myTerms.push_back( k2 );
    myTerms.push_back( k2 ); // дубль
    myTerms.push_back( k12 );// не станция
    myTerms.push_back( k1 );
    myTerms.push_back( EsrKit() ); // вообще пустышка
    EsrKit k0( 1000 );
    CPPUNIT_ASSERT( k0 < k1 );
    EsrKit k1m( 1212 );
    CPPUNIT_ASSERT( k1 < k1m && k1m < k2 );
    EsrKit k4( 4000 );
    CPPUNIT_ASSERT( k3 < k4 );
    myTerms.push_back( k0 ); // неизвестные станции
    myTerms.push_back( k1m );
    myTerms.push_back( k4 );
    // 
    auto spans = eg.getSpanForTerms( myTerms );
    CPPUNIT_ASSERT( 2 == spans.size() );
    CPPUNIT_ASSERT( k12 == spans.front() );
    CPPUNIT_ASSERT( k23 == spans.back() );
}

void TC_EsrGuide::setUp()
{
    k1  = EsrKit(1111);
    k2  = EsrKit(2222);
    k12 = EsrKit( 1111, 2222 );
    k3  = EsrKit(3333);
    k23 = EsrKit( 2222, 3333 );
    p2.SetCoord(rwRuledCoord(L"Кадуй", rwCoord(707,11)));
    p1.SetCoord(rwRuledCoord(L"Буй",   rwCoord(101,77)));
    p1.SetCoord(rwRuledCoord(L"Кадуй", rwCoord(710,52)));
    p3.SetCoord(rwRuledCoord(L"Отрыв", rwCoord(987,345)));
}