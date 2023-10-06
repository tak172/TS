#include "stdafx.h"

#include "TC_AutoChart.h"

#include "../UT/AutoChartTest.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/SpotEvent.h"
#include "../helpful/TrainDescr.h"
#include "../Hem/AsoupEvent.h"
#include "../Guess/Msg.h"
#include "../helpful/FotoFinish.h"
#include "../helpful/LocoCharacteristics.h"

using namespace HemHelpful;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AutoChart );

TC_AutoChart::TC_AutoChart()
{
}

TC_AutoChart::~TC_AutoChart()
{
}

void TC_AutoChart::setUp()
{
    autoChart.reset( new AutoChartTest() );
}

void TC_AutoChart::tearDown()
{
    autoChart.reset();
    HemEventRegistry::Shutdowner();
    LocoCharacteristics::Shutdowner();
}

void TC_AutoChart::SaveAndLoad()
{
    const string header =
        "<hem2act>"
        "<AutoChart>"
        "<HappenLayer>";
    const string example = 
        "<HemPath>"
        "  <SpotEvent create_time='20131003T121316Z' name='Form'      Bdg='2УП[11111:22222]' />"
        "  <SpotEvent create_time='20131003T121336Z' name='Arrival'   Bdg='2п[22222]'        />"
        "  <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[22222]'        />"
        "  <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        "</HemPath>";
    const string etalon = 
        "<HemPath>"
        "  <SpotEvent create_time='20131003T121316Z' name='Form'      Bdg='2УП[11111:22222]' />"
        "  <SpotEvent create_time='20131003T121336Z' name='Arrival'   Bdg='2п[22222]'        />"
        "  <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[22222]'        />"
        "  <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        "  <SpotEvent create_time='20131003T121406Z' name='Death'     Bdg='ЧУ1[22222:33333]' />"
        "</HemPath>";
    const string footer = 
    "<HemPath>"
    "  <SpotEvent create_time='20131003T121316Z' name='Form'      Bdg='2УП[33333:44444]' />"
    "  <SpotEvent create_time='20131003T121336Z' name='Arrival'   Bdg='2п[33333]'        />"
    "  <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[33333]'        />"
    "  <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[11111:33333]' />"
    "  <SpotEvent create_time='20131003T121406Z' name='Death'     Bdg='ЧУ1[11111:33333]' />"
    "</HemPath>"
    "</HappenLayer>"
    "<AsoupLayer>"
    "<AsoupEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' index='7777-888-9999' num='9999' length='88' weight='7' noinfo = \"Y\"/>"
    "</AsoupLayer>"
    "<GuiLayer/>"
    "<ScheduledLayer/>"
    "<AsoupQueue />"
    "</AutoChart>"
    "</hem2act>";

    { // загружаем из файла
        attic::a_document doc;
        doc.load_wide( From1251(header+example+footer) );
        autoChart->deserialize(doc.document_element());
        autoChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
        auto trimMom = autoChart->trimMoment();
        autoChart->trimLoadedLayer( trimMom );
        autoChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
    }
    std::string	str;
    { // сохраняем в файл
        attic::a_document doc;
        autoChart->serialize(doc.set_document_element( "hem2act" ));
        str = doc.document_element().to_str();
    }
    std::string str_etalon;
    { // делаем эталон
        attic::a_document doc;
        doc.load_wide( From1251(header+etalon+footer) );
        str_etalon = doc.document_element().to_str();
    }
    auto mismresult = mismatch( str.cbegin(), str.cend(), str_etalon.cbegin(), str_etalon.cend() );

    CPPUNIT_ASSERT(str_etalon == str);
}

void TC_AutoChart::DirtyDataLoad()
{
    // данные с ошибками
    const char * dirty1251 = 
        "<test>"
        "  <AutoChart>"
        "    <HappenLayer>"
        "      <HemPath>"
        "        <SpotEvent create_time='20131003T121316Z' name='Form' Bdg='2УП[44444]' />"
        "        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[44444]' />"
        "        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[44444]' />"
        "        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[44444]' />"
        "        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[44444]' />"
        "        <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        "      </HemPath>"
        "      <HemPath>"
        "        <SpotEvent create_time='20131003T121316Z' name='Form' Bdg='2УП[11111:22222]' />"
        "        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' />"
        "        <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[22222]' />"
        "        <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        "        <SpotEvent create_time='20131003T121406Z' name='Death' Bdg='ЧУ1[22222:33333]' />"
        "      </HemPath>"
        "    </HappenLayer>"
        "    <AsoupLayer>"
        "      <AsoupEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' index='7777-888-9999' num='9999' length='88' weight='7' noinfo = \"Y\"/>"
        "      <AsoupEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' index='7777-888-9999' num='9999' length='88' weight='7' noinfo = \"Y\"/>"
        "    </AsoupLayer>"
        "    <ScheduledLayer>"
        "    </ScheduledLayer>"
        "  </AutoChart>"
        "</test>";

    // данные, где ошибки уже удалены
    const char * fine1251 = 
        "<test>"
        "  <AutoChart>"
        "    <HappenLayer>"
        "      <HemPath>"
        "        <SpotEvent create_time='20131003T121316Z' name='Form' Bdg='2УП[11111:22222]' />"
        "        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' />"
        "        <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[22222]' />"
        "        <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        "        <SpotEvent create_time='20131003T121406Z' name='Death' Bdg='ЧУ1[22222:33333]' />"
        "      </HemPath>"
        "    </HappenLayer>"
        "    <AsoupLayer>"
        "      <AsoupEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' index='7777-888-9999' num='9999' length='88' weight='7' noinfo = \"Y\"/>"
        "    </AsoupLayer>"
        "    <GuiLayer />"
        "    <ScheduledLayer />"
        "    <AsoupQueue />"
        "  </AutoChart>"
        "</test>";

    // загружаем из файла с ошибками
    attic::a_document doc_dirty;
    doc_dirty.load_wide( From1251(dirty1251) );
    autoChart->deserialize(doc_dirty.document_element());
    autoChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
    string root_name = doc_dirty.document_element().name();

    // сохраняем в файл то, что загрузили
    attic::a_document doc_dst;
    autoChart->serialize(doc_dst.set_document_element( root_name ));

    // формируем ожидаемый результат
    attic::a_document doc_fine;
    doc_fine.load_wide( From1251(fine1251) );
    doc_fine.set_document_element( root_name );

    // сравниваем результаты
    std::string str_fin = doc_fine.document_element().to_str();
    std::string	str_dst = doc_dst.document_element().to_str();
    auto mismresult = mismatch( str_fin.cbegin(), str_fin.cend(), str_dst.cbegin(), str_dst.cend() );

    CPPUNIT_ASSERT(str_fin == str_dst);
}

void TC_AutoChart::ProfileLoad()
{
    auto ods = []( const std::wstring& ws ){
        OutputDebugString( ws.c_str() );
    };

    attic::a_document doc;
    doc.load_file( L"E:/WorkCopy.all/Sample/logics/#hef/18062118.hef" );
    FotoFinish ff( L"AutoChart total", ods, boost::posix_time::milliseconds(100) );
    for( int i=0; i<3; ++i )
    {
        {
            FotoFinish ff( L"deserialize", ods, boost::posix_time::milliseconds(50) );
            autoChart->deserialize(doc.document_element());
        }
        {
            FotoFinish ff( L"changes", ods, boost::posix_time::milliseconds(50) );
            autoChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
        }
    }
    doc.reset();
    autoChart->serialize(doc.set_document_element( "hem2act" ));
    doc.save_file( L"E:/WorkCopy.all/svn_MNS/RIGA-GID/_Bbx/profile_out.hef" );
    CPPUNIT_ASSERT(true);
}
