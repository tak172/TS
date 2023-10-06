#include "stdafx.h"
#include "TC_GXMLWrapper.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/GObjectManager.h"
#include "GraphicDocumentTest.h"
#include "GXMLWrapperTest.h"
#include "../StatBuilder/GLoadResult.h"
#include "../helpful/Informator.h"
#include "../StatBuilder/GViewSkin.h"
#include "../StatBuilder/Measures.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GXMLWrapper );

using namespace std;

void TC_GXMLWrapper::SaveLoadOrthoSwitches()
{
	BuildSwitchKit( CGridPoint( 10, 10 ), CGridSize( -5, 0 ), CGridSize( 5, 0 ), CGridSize( 5, 5 ) );
	GXMLWrapperTest gwrapper( gdoc.get() );
	gwrapper.Save( L"" ); //сохранение в xml-документе

	//удаление всех объектов с проверкой
	CleanAll();

	//загрузка с проверкой
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	set <const CGraphicElement *> all_strips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), all_strips );
	CPPUNIT_ASSERT( all_strips.size() == 3 );
	set <const CGraphicElement *> all_switches;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), all_switches );
	CPPUNIT_ASSERT( all_switches.size() == 1 );
}

void TC_GXMLWrapper::SaveLoadOrthoTwoStrips()
{
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * vert_strip = gstrip_sample->Clone();
	CGraphicElement * horiz_strip = gstrip_sample->Clone();
	vert_strip->SetHotPoint( CGridPoint( 5, 5 ) );
	vert_strip->SetSizes( 0, 5 );
	horiz_strip->SetHotPoint( CGridPoint( 0, 5 ) );
	horiz_strip->SetSizes( 10, 0 );
	pgom->AddObject( vert_strip, gdoc.get() );
	pgom->AddObject( horiz_strip, gdoc.get() );

	GXMLWrapperTest gwrapper( gdoc.get() );
	gwrapper.Save( L"" ); //сохранение в xml-документе
	CleanAll();

	//загрузка с проверкой
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	set <const CGraphicElement *> all_strips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), all_strips );
	CPPUNIT_ASSERT( all_strips.size() == 2 );
	for( const CGraphicElement * gstrip : all_strips )
	{
		const vector <GLinkInfo> & glinks = gstrip->GetLinks();
		CPPUNIT_ASSERT( glinks.empty() ); //не связаны
	}
	set <const CGraphicElement *> all_switches;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), all_switches );
	CPPUNIT_ASSERT( all_switches.empty() );
}

void TC_GXMLWrapper::SaveLoadOrthoThreeStrips()
{
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * vert_strip = gstrip_sample->Clone();
	CGraphicElement * horiz_strip1 = gstrip_sample->Clone();
	CGraphicElement * horiz_strip2 = gstrip_sample->Clone();
	vert_strip->SetHotPoint( CGridPoint( 5, 5 ) );
	vert_strip->SetSizes( 0, 5 );
	horiz_strip1->SetHotPoint( CGridPoint( 0, 5 ) );
	horiz_strip1->SetSizes( 5, 0 );
	horiz_strip2->SetHotPoint( CGridPoint( 5, 5 ) );
	horiz_strip2->SetSizes( 5, 0 );
	pgom->AddObject( vert_strip, gdoc.get() );
	pgom->AddObject( horiz_strip1, gdoc.get() );
	pgom->AddObject( horiz_strip2, gdoc.get() );

	GXMLWrapperTest gwrapper( gdoc.get() );
	gwrapper.Save( L"" ); //сохранение в xml-документе
	CleanAll();

	//загрузка с проверкой
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	set <const CGraphicElement *> all_strips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), all_strips );
	CPPUNIT_ASSERT( all_strips.size() == 3 );
	for( const CGraphicElement * gstrip : all_strips )
	{
		const vector <GLinkInfo> & glinks = gstrip->GetLinks();
		CPPUNIT_ASSERT( glinks.size() == 2 ); //связаны друг с другом
	}
	set <const CGraphicElement *> all_switches;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), all_switches );
	CPPUNIT_ASSERT( all_switches.empty() );
}

void TC_GXMLWrapper::CleanAll()
{
	set <CGraphicElement *> all_switches;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), all_switches );
	for( CGraphicElement * gswitch : all_switches )
		pgom->RemoveObject( gswitch, gdoc.get() );
	set <CGraphicElement *> all_objects;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), all_objects );
	for( CGraphicElement * gelem : all_objects )
		pgom->RemoveObject( gelem, gdoc.get() );
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), all_objects );
	CPPUNIT_ASSERT( all_objects.empty() );
}

void TC_GXMLWrapper::NullLengthStripLoad()
{
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * nulllen_strip = gstrip_sample->Clone();
	nulllen_strip->SetHotPoint( CGridPoint( 5, 5 ) );
	nulllen_strip->SetSizes( 0, 0 );
	pgom->AddObject( nulllen_strip, gdoc.get() );

	GXMLWrapperTest gwrapper( gdoc.get() );
	gwrapper.Save( L"" ); //сохранение в xml-документе
	CleanAll();

	//загрузка с проверкой
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	set <const CGraphicElement *> all_strips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), all_strips );
	CPPUNIT_ASSERT( all_strips.empty() );
}

void TC_GXMLWrapper::SaveLoadSofAttributes()
{
	CInformator * pI = CInformator::Instance();
	CMeasures * msrs = gdoc->GetSkin()->GetMeasures();
    const std::wstring MenuNameExmpl = L"Menu name";
	msrs->SetChiefPicture( true );
    msrs->SetMenuName( MenuNameExmpl );
    
    std::vector<EsrKit> vEsr;
    const std::string EsrListExmpl = "11111,33333";
    msrs->SetEsrByStr( EsrListExmpl );

	GXMLWrapperTest gwrapper( gdoc.get() );
	gwrapper.Save( L"" ); //сохранение в xml-документе
	attic::a_document * xdoc = gdoc->GetXMLDoc();
	attic::a_node rnode = xdoc->child( "GRAPHIC_SCHEMATA" );
	CPPUNIT_ASSERT( rnode );
	attic::a_attribute chief_pict_attr = rnode.attribute( pI->xml_chiefpicture_str );
	CPPUNIT_ASSERT( chief_pict_attr );
	CPPUNIT_ASSERT( chief_pict_attr.as_bool() == true );
    attic::a_attribute menu_name_attr = rnode.attribute( pI->xml_menuname_str );
    CPPUNIT_ASSERT( menu_name_attr );
    CPPUNIT_ASSERT( menu_name_attr.as_wstring() == MenuNameExmpl );

    attic::a_attribute esr_list_attr = rnode.attribute( pI->xml_matchesr_str );
    CPPUNIT_ASSERT( esr_list_attr );
    CPPUNIT_ASSERT( esr_list_attr.as_string() == EsrListExmpl );

	msrs->SetChiefPicture( false );
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	CPPUNIT_ASSERT( msrs->IsChiefPicture() == true );
}

void TC_GXMLWrapper::CleanRootAttributeDuplicates()
{
	//xml-документ с дублирующими атрибутами
	GXMLWrapperTest gwrapper( gdoc.get() );
	attic::a_document * xdoc = gdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "GRAPHIC_SCHEMATA" );
	root_node.append_attribute( "unknown_attribute" );
	root_node.append_attribute( "unknown_attribute" );
	auto attrNum = count_if( root_node.attributes_begin(), root_node.attributes_end(), []( pugi::xml_attribute attr ){
		return string( attr.name() ) == "unknown_attribute";
	} );
	CPPUNIT_ASSERT( attrNum == 2 );

	//загружаем
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );

	//сохраняем (дублирующие атрибуты должны удалиться)
	gwrapper.Save( L"" );

	attic::a_document * xdocSav = gdoc->GetXMLDoc();
	attic::a_node root_node_sav = xdocSav->document_element();
	CPPUNIT_ASSERT( root_node_sav );
	auto attrNumSav = count_if( root_node_sav.attributes_begin(), root_node_sav.attributes_end(), []( pugi::xml_attribute attr ){
		return string( attr.name() ) == "unknown_attribute";
	} );
	CPPUNIT_ASSERT( attrNumSav == 1 );
}