#include "stdafx.h"

#include "TC_ForbidLayer.h"
#include "UtHemHelper.h"
#include "../Hem/ForbidLayer.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ForbidLayer );


// Проверка повторного заполнения всего слоя
// - генерация изменений
// - сериализация всего слоя
void TC_ForbidLayer::Reload()
{
    wstring header =
        L"<Test>"
        L"  <ForbidLayer>"
        ;
    wstring body1 =
        L"<ForbidEvent Id='R488ff2ec-712e-4e03-8f77-72c2707a1646' refCode='55965766-fc03-4c6a-b0e5-d6819638515f' esr='09220:09230' FullSpan='Y' StartTime='20190710T061500Z' StopTime='20190710T100000Z' />"
        L"<ForbidEvent Id='i1001' esr='09230' waynum='3' parknum='1' StartTime='20181231T093000Z' StopTime='20191204T110300Z' Fact_Start='20181231T093000Z' Fact_Stop='20190116T020300Z' Reason='1601' inner_name='srv4' moveModeInRight='autoblock' RightAB='Y' moveModeInWrong='phone' />"
        L"<ForbidEvent Id='i2002' esr='11052' waynum='3' parknum='1' StartTime='20181209T035600Z' StopTime='20191202T045600Z' Reason='1601' inner_name='srv4' moveModeInRight='autoblock' RightAB='Y' />"
        ;
    wstring body2 =
        L"<ForbidEvent Id='R488ff2ec-712e-4e03-8f77-72c2707a1646' refCode='00000000-0000-0000-0000-000000000000' esr='09220:09230' FullSpan='Y' StartTime='20190710T061500Z' StopTime='20190710T100000Z' />"
        L"<ForbidEvent Id='i1001' esr='09230' waynum='3' parknum='1' StartTime='20181231T093000Z' StopTime='20191204T110300Z' Fact_Start='20181231T093000Z' Fact_Stop='20190116T020300Z' Reason='1601' inner_name='srv4' moveModeInRight='autoblock' RightAB='Y' moveModeInWrong='phone' />"
        // i2002 удален
        L"<ForbidEvent Id='i3003' esr='77777' waynum='3' parknum='1' StartTime='20181209T035600Z' StopTime='20191202T045600Z' Reason='1601' inner_name='srv4' moveModeInRight='autoblock' RightAB='Y' />"
        ;
    wstring footer =
        L"  </ForbidLayer>"
        L"</Test>"
        ;

    UtLayer<Forbid::Layer> forbid_layer;
    // загружаем из файла вариант 1
    attic::a_document doc;
    CPPUNIT_ASSERT( doc.load_wide( header+body1+footer ) );
    doc.document_element() >> forbid_layer;
    forbid_layer.TakeChanges( attic::a_node() );
    // проверяем вариант 1
    attic::a_document res1("Test");
    attic::a_node fullNode1 = res1.document_element().append_child("Full");
    fullNode1 << forbid_layer;
    std::wstring plain1 = res1.pretty_wstr();
    CPPUNIT_ASSERT( !plain1.empty() );
    // - полное содержимое
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//ForbidEvent[@Id='R488ff2ec-712e-4e03-8f77-72c2707a1646']").empty() );
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//ForbidEvent[@Id='i1001']").empty() );
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//ForbidEvent[@Id='i2002']").empty() );

    // загружаем вариант 2    
    CPPUNIT_ASSERT( doc.load_wide( header+body2+footer ) );
    doc.document_element() >> forbid_layer;
    // проверяем вариант 2
    attic::a_document res2("Test");
    attic::a_node chNode = res2.document_element().append_child("Changes");
    forbid_layer.TakeChanges( chNode );
    attic::a_node fullNode2 = res2.document_element().append_child("Full");
    fullNode2 << forbid_layer;
    std::wstring plain2 = res2.pretty_wstr();
    CPPUNIT_ASSERT( !plain2.empty() );
    // - различия
    CPPUNIT_ASSERT_EQUAL( size_t(3), chNode.select_nodes("//Trio[@layer='forbid']").size() );
    // - полное содержимое
    CPPUNIT_ASSERT_EQUAL( size_t(3), fullNode2.select_nodes("//ForbidEvent[@Id]").size() );
    CPPUNIT_ASSERT( !fullNode1.select_nodes("//ForbidEvent[@Id='R488ff2ec-712e-4e03-8f77-72c2707a1646']").empty() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//ForbidEvent[@Id='i1001']").empty() );
    CPPUNIT_ASSERT( !fullNode2.select_nodes("//ForbidEvent[@Id='i3003']").empty() );
}
