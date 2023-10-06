#include "stdafx.h"

#include "TC_LimitLayer.h"
#include "UtHemHelper.h"
#include "../Hem/LimitLayer.h"
#include "../helpful/Interval_Time_t.h"
#include "../helpful/Time_Iso.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LimitLayer );

static const time_t T=3600*(240+23);

void TC_LimitLayer::Create()
{
    Limit::Event ev= createEvent<Limit::Event>(
        L"<Data Id='i78' waynum='2' StartTime='20150909T213456Z' esr='11050:11051' Reason='1601' UntilStop='Y' SpeedPass='63' SpeedGoods='64' SpeedSuburban='65'>"
        L"  <picketing_start picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"</Data>"
        );
    UtLayer<Limit::Layer> limit_layer;
    limit_layer.Push(ev);
    limit_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i78'") );

    Limit::Event badev= createEvent<Limit::Event>(
        L"<Data Id='G15' StartTime='20150909T213456Z' Reason='1601' UntilStop='Y' SpeedAll='25' SpeedPass='65537' SpeedGoods='65537' SpeedSuburban='65537' Comment='xxx' />"
        );
    limit_layer.Push(badev);
    limit_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( !limit_layer.exist_event("@Id='G15'") );
}

void TC_LimitLayer::Change()
{
    Limit::Event ev1= createEvent<Limit::Event>(
        L"<Data Id='i78' waynum='11' StartTime='20150909T213456Z' esr='11050:11051' Reason='1601' UntilStop='Y' SpeedPass='60' SpeedGoods='60' SpeedSuburban='60'>"
        L"  <picketing_start picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"</Data>"
        );
    Limit::Event ev2= createEvent<Limit::Event>(
        L"<Data Id='i78' waynum='22' StartTime='20150909T213456Z' esr='11050:11051' Reason='1601' UntilStop='Y' SpeedPass='60' SpeedGoods='60' SpeedSuburban='60'>"
        L"  <picketing_start picketing1_val='428~110' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='428~110' picketing1_comm='Вентспилс' />"
        L"</Data>"
        );
    UtLayer<Limit::Layer> limit_layer;
    limit_layer.Push(ev1);
    limit_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i78' and @waynum=11") );
    CPPUNIT_ASSERT( !limit_layer.exist_event("@Id='i78' and @waynum=22") );

    limit_layer.Push(ev2);
    limit_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( !limit_layer.exist_event("@Id='i78' and @waynum=11") );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i78' and @waynum=22") );
}

void TC_LimitLayer::Delete()
{
    Limit::Event ev= createEvent<Limit::Event>(
        L"<Data Id='i78' waynum='45' StartTime='20150909T213456Z' esr='11050:11051' Reason='1601' UntilStop='Y' SpeedPass='60' SpeedGoods='60' SpeedSuburban='60'>"
        L"  <picketing_start picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"</Data>"
        );
    UtLayer<Limit::Layer> limit_layer;
    limit_layer.Push(ev);
    limit_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( limit_layer.path_count() == 1 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i78'") );

    limit_layer.Remove( ExtId::make_local(78) );
    limit_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( limit_layer.path_count() == 0 );
    CPPUNIT_ASSERT( !limit_layer.exist_event("@Id='i78'") );
}

void TC_LimitLayer::GetByTimeAndEsr()
{
    Limit::Event ev1= createEvent<Limit::Event>(
        L"<Data Id='i55' waynum='11' esr='11111:22222' StartTime='20150707T000000Z' UntilStop='Y' Reason='1601' SpeedPass='60' SpeedGoods='60' SpeedSuburban='60'>"
        L"  <picketing_start picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"</Data>"
        );
    Limit::Event ev2= createEvent<Limit::Event>(
        L"<Data Id='i66' waynum='11' esr='11111:22222' StartTime='20150909T000000Z' StartTime='20150930T000000Z' Reason='1601' SpeedPass='60' SpeedGoods='60' SpeedSuburban='60'>"
        L"  <picketing_start picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"</Data>"
        );
    Limit::Event ev3= createEvent<Limit::Event>(
        L"<Data Id='i77' waynum='11' esr='22222:33333' StartTime='20150909T000000Z' StartTime='20150930T000000Z' Reason='1601' SpeedPass='60' SpeedGoods='60' SpeedSuburban='60'>"
        L"  <picketing_start picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='428~100' picketing1_comm='Вентспилс' />"
        L"</Data>"
        );
    UtLayer<Limit::Layer> limit_layer;
    limit_layer.Push(ev1);
    limit_layer.Push(ev2);
    limit_layer.Push(ev3);
    limit_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( limit_layer.path_count() == 3 );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i55'") );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i66'") );
    CPPUNIT_ASSERT( limit_layer.exist_event("@Id='i77'") );

    interval_time_t tt2011(    time_from_iso("20110101T000000Z"), time_from_iso("20111231T000000Z") );
    interval_time_t tt2015(    time_from_iso("20150101T000000Z"), time_from_iso("20151231T000000Z") );
    interval_time_t tt2015_8( time_from_iso("20150801T000000Z"), time_from_iso("20150825T000000Z") );
    EsrKit e12( 11111, 22222 );
    EsrKit e23( 22222, 33333 );

    auto have = [](const std::vector<ExtId>& vec, const std::string& s) {
        auto pred = [s](const ExtId& id)->bool{
            return id.get_value() == ExtId::IdCode(s, "");
        };
        return std::any_of( vec.begin(), vec.end(), pred );
    };

    auto res = limit_layer.GetByTimeAndEsr( tt2011, e12 );
    CPPUNIT_ASSERT( 0 == res.size() );

    res = limit_layer.GetByTimeAndEsr( tt2015, e12 );
    CPPUNIT_ASSERT( 2 == res.size() );
    CPPUNIT_ASSERT( have( res, "i55" ) );
    CPPUNIT_ASSERT( have( res, "i66" ) );

    res = limit_layer.GetByTimeAndEsr( tt2015_8, e12 );
    CPPUNIT_ASSERT( 1 == res.size() );
    CPPUNIT_ASSERT( have( res, "i55" ) );

    res = limit_layer.GetByTimeAndEsr( tt2015, e23 );
    CPPUNIT_ASSERT( 1 == res.size() );
    CPPUNIT_ASSERT( have( res, "i77" ) );
}

// Проверка повторного заполнения всего слоя предупреждений
// - генерация изменений
// - сериализация всего слоя
void TC_LimitLayer::Reload()
{
    wstring header =
        L"<Test>"
        L"  <LimitLayer>"
        ;
    wstring body1 =
        L"<LimitEvent Id='R11111.201905230804' refCode='2509201907051205587100' esr='11090' JustObject='unknown[11090]' class='16' StartTime='20190524T070000Z' UntilStop='Y' SpeedAll='25' />"
        L"<LimitEvent Id='R22222.201906250732' refCode='2509201907051207171320' esr='09241' waynum='1' parknum='1' StartTime='20190625T080000Z' UntilStop='Y' SpeedAll='70' />"
        L"<LimitEvent Id='R33333.201907011347' refCode='2509201907011348085870' esr='11062' JustObject='unknown[11062]' class='16' StartTime='20190701T110000Z' StopTime='20190705T023000Z' Comment='Рельсовые стыки закреплены типовыми струбцинами Robel' SpeedPass='60' SpeedGoods='60' SpeedSuburban='60' />"
        ;
    wstring body2 =
        L"<LimitEvent Id='R11111.201905230804' refCode='xxxxxxxxxxxxxxxxxxxxxx' esr='11090' JustObject='unknown[11090]' class='16' StartTime='20190524T070000Z' UntilStop='Y' SpeedAll='25' />"
        L"<LimitEvent Id='R22222.201906250732' refCode='2509201907051207171320' esr='09241' waynum='1' parknum='1' StartTime='20190625T080000Z' UntilStop='Y' SpeedAll='70' />"
        L"<LimitEvent Id='R44444.201907031343' refCode='2509201907031344283160' esr='09420' JustObject='unknown[09420]' class='16' StartTime='20190704T200000Z' StopTime='20190704T235000Z' SpeedPass='25' SpeedGoods='25' SpeedSuburban='25' />"
        L"<LimitEvent Id='R55555.201907011347' refCode='yyyyyyyyyyyyyyyyyyyyyy' esr='11062' JustObject='unknown[11062]' class='16' StartTime='20190701T110000Z' StopTime='20190705T023000Z' Comment='Рельсовые стыки закреплены типовыми струбцинами Robel' SpeedPass='60' SpeedGoods='60' SpeedSuburban='60' />"
        ;
    wstring footer =
        L"  </LimitLayer>"
        L"</Test>"
        ;

    UtLayer<Limit::Layer> limit_layer;
    // загружаем из файла вариант 1
    attic::a_document doc;
    CPPUNIT_ASSERT( doc.load_wide( header+body1+footer ) );
    doc.document_element() >> limit_layer;
    limit_layer.TakeChanges( attic::a_node() );
    // проверяем вариант 1
    attic::a_document res1("Test");
    attic::a_node fullNode1 = res1.document_element().append_child("Full");
    fullNode1 << limit_layer;
    std::wstring plain1 = res1.pretty_wstr();
    CPPUNIT_ASSERT( !plain1.empty() );
    // - полное содержимое
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//LimitEvent[@Id='R11111.201905230804']").empty() );
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//LimitEvent[@Id='R22222.201906250732']").empty() );
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//LimitEvent[@Id='R33333.201907011347']").empty() );

    // загружаем вариант 2    
    CPPUNIT_ASSERT( doc.load_wide( header+body2+footer ) );
    doc.document_element() >> limit_layer;
    // проверяем вариант 2
    attic::a_document res2("Test");
    attic::a_node chNode = res2.document_element().append_child("Changes");
    limit_layer.TakeChanges( chNode );
    attic::a_node fullNode2 = res2.document_element().append_child("Full");
    fullNode2 << limit_layer;
    std::wstring plain2 = res2.pretty_wstr();
    CPPUNIT_ASSERT( !plain2.empty() );
    // - различия
    CPPUNIT_ASSERT_EQUAL( size_t(4), chNode.select_nodes("//Trio[@layer='limit']").size() );
    // - полное содержимое
    CPPUNIT_ASSERT_EQUAL( size_t(4), fullNode2.select_nodes("//LimitEvent[@Id]").size() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//LimitEvent[@Id='R11111.201905230804']").empty() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//LimitEvent[@Id='R22222.201906250732']").empty() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//LimitEvent[@Id='R44444.201907031343']").empty() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//LimitEvent[@Id='R55555.201907011347']").empty() );
}
