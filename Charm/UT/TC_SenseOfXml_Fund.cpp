#include "stdafx.h"

#include "../helpful/Attic.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/AutoChart.h"
#include "../Hem/Filer.h"
#include "../SenseOfXml/SenseOfXml.h"
#include "../SenseOfXml/SenseOfXml_Fund.h"
#include "../SenseOfXml/Outlet.h"
#include "TC_SenseOfXml_Fund.h"
#include "MockFiler.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SenseOfXml_Fund );

void TC_SenseOfXml_Fund::check_PostSlice_toOutlet()
{
    Outlet outlet;
    SenseOfXml::HandlerChain chainOutlet;
    OutletConcrete concrete( outlet, Goal::Canopy );
    chainOutlet.attach( new PostSlice("4client"), new FundHandler_ToClient( concrete ) );
    // пустой документ не обрабатывается
    attic::a_document doc;
    CPPUNIT_ASSERT( !chainOutlet.executable(doc) );
    CPPUNIT_ASSERT( outlet.empty() );
    // а команда обрабатывается
    PostSlice pa("4client");
    CPPUNIT_ASSERT( chainOutlet.executable( pa.doc() ) );
    chainOutlet.execute( pa.doc() );
    CPPUNIT_ASSERT( !outlet.empty() );
    CPPUNIT_ASSERT( outlet.pop_front() >= 0 );
    CPPUNIT_ASSERT( outlet.empty() );
    CPPUNIT_ASSERT( outlet.top_size() == std::string::npos );
}

void TC_SenseOfXml_Fund::compatible_Backup()
{
    attic::a_document doc;
    doc.load_wide(
        L"<?xml version='1.0'?>"
        L"<HxBackup>"
        L"  <AutoChart>"
        L"    <HappenLayer>"
        L"      <HemPath>"
        L"        <SpotEvent name='Appearance' Bdg='2ЧП1[31340:31363]' create_time='20141114T045044Z' />"
        L"       <SpotEvent name='Disappearance' Bdg='2ЧП1[31340:31363]' create_time='20141114T045347Z' />"
        L"      </HemPath>"
        L"    </HappenLayer>"
        L"  </AutoChart>"
        L"</HxBackup>"
        );

    // Надо doc отправить в hem, запросить из hem, получить из hem и сравнить с исходным
}
