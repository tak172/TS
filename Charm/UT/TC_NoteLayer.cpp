#include "stdafx.h"

#include "TC_NoteLayer.h"
#include "UtHemHelper.h"
#include "../Hem/NoteLayer.h"
// #include "../helpful/Interval_Time_t.h"
// #include "../helpful/Time_Iso.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_NoteLayer );

// static const time_t T=3600*(240+23);
// 

// Проверка повторного заполнения всего слоя пометок
// - генерация изменений
// - сериализация всего слоя
void TC_NoteLayer::Reload()
{
    wstring header =
        L"<Test>"
        L"  <MarkLayer>"
        ;
    wstring body1 =
        L"<MarkEvent Id='i11111' esr='09000:09006' waynum='29' create_time='20190704T213827Z' print='Y' Comment='Остановка поезда  перед входным светофором.' inner_name='srvMoving' auto='Y' entrastopping='Y'>"
        L"    <picketing picketing1_val='6~899' picketing1_comm='Рига Пассажирская' />"
        L"    <Comment Text='Остановка поезда  перед входным светофором.' />"
        L"</MarkEvent>"
        L"<MarkEvent Id='i22222' esr='11432:11446' create_time='20190704T214711Z' print='Y' Comment='Поезд 2765 на перегоне Sergunta - Nīcgale. Превышение ПВХ.' inner_name='srv10' auto='Y'>"
        L"    <picketing picketing1_val='354~950' picketing1_comm='Вентспилс' />"
        L"    <Comment Text='Поезд 2765 на перегоне Sergunta - Nīcgale. Превышение ПВХ.' />"
        L"</MarkEvent>"
        L"<MarkEvent Id='i33333' esr='09290:09303' create_time='20190704T230600Z' print='Y' Comment='Vilciens Nr.3198  05.07.19 02:06 kustība 83 km 8 p. posmā Dobele - Biksti 82~4 sastāva apskate 28 min' inner_name='srvMany' DelayReason='12103'>"
        L"    <picketing picketing1_val='82~4' picketing1_comm='Лиепая' />"
        L"    <Comment Text='**** без изменений ************' />"
        L"</MarkEvent>"
        ;
    wstring body2 =
        L"<MarkEvent Id='i11111' esr='09000:09006' waynum='29' create_time='20190704T213827Z' print='Y' Comment='Остановка поезда  перед входным светофором.' inner_name='srvMoving' auto='Y' entrastopping='Y'>"
        L"    <picketing picketing1_val='6~899' picketing1_comm='Рига Пассажирская' />"
        L"    <Comment Text='--- изменено ------------------' />"
        L"</MarkEvent>"
        // событие 22222 удалено целиком
        L"<MarkEvent Id='i33333' esr='09290:09303' create_time='20190704T230600Z' print='Y' Comment='Vilciens Nr.3198  05.07.19 02:06 kustība 83 km 8 p. posmā Dobele - Biksti 82~4 sastāva apskate 28 min' inner_name='srvMany' DelayReason='12103'>"
        L"    <picketing picketing1_val='82~4' picketing1_comm='Лиепая' />"
        L"    <Comment Text='**** без изменений ************' />"
        L"</MarkEvent>"
        L"<MarkEvent Id='i44444' esr='11432:11446' create_time='20190704T214711Z' print='Y' Comment='Поезд 2765 на перегоне Sergunta - Nīcgale. Превышение ПВХ.' inner_name='srv10' auto='Y'>"
        L"    <picketing picketing1_val='354~950' picketing1_comm='Вентспилс' />"
        L"    <Comment Text='+++ добавлено +++++++++++++++++' />"
        L"</MarkEvent>"
        ;
    wstring footer =
        L"  </MarkLayer>"
        L"</Test>"
        ;

    UtLayer<Note::Layer> note_layer;
    // загружаем из файла вариант 1
    attic::a_document doc;
    CPPUNIT_ASSERT( doc.load_wide( header+body1+footer ) );
    doc.document_element() >> note_layer;
    note_layer.TakeChanges( attic::a_node() );
    // проверяем вариант 1
    attic::a_document res1("Test");
    attic::a_node fullNode1 = res1.document_element().append_child("Full");
    fullNode1 << note_layer;
    std::wstring plain1 = res1.pretty_wstr();
    CPPUNIT_ASSERT( !plain1.empty() );
    // - полное содержимое
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//MarkEvent[@Id='i11111']").empty() );
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//MarkEvent[@Id='i22222']").empty() );
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//MarkEvent[@Id='i33333']").empty() );

    // загружаем вариант 2    
    CPPUNIT_ASSERT( doc.load_wide( header+body2+footer ) );
    doc.document_element() >> note_layer;
    // проверяем вариант 2
    attic::a_document res2("Test");
    attic::a_node chNode = res2.document_element().append_child("Changes");
    note_layer.TakeChanges( chNode );
    attic::a_node fullNode2 = res2.document_element().append_child("Full");
    fullNode2 << note_layer;
    std::wstring plain2 = res2.pretty_wstr();
    CPPUNIT_ASSERT( !plain2.empty() );
    // - различия
    CPPUNIT_ASSERT_EQUAL( size_t(3), chNode.select_nodes("//Trio[@layer='mark']").size() );
    // - полное содержимое
    CPPUNIT_ASSERT_EQUAL( size_t(3), fullNode2.select_nodes("//MarkEvent[@Id]").size() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//MarkEvent[@Id='i11111']").empty() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//MarkEvent[@Id='i33333']").empty() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//MarkEvent[@Id='i44444']").empty() );
}
