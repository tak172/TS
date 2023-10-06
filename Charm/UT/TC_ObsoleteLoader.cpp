#include "stdafx.h"
#include "TC_ObsoleteLoader.h"
#include "GXMLWrapperTest.h"
#include "LXMLWrapperTest.h"
#include "GraphicDocumentTest.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/GraphicTableBoard.h"
#include "../StatBuilder/GraphicTableBoardXMLPlug.h"
#include "../StatBuilder/GLoadResult.h"
#include "../StatBuilder/LLoadResult.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/GraphicSemaleg.h"
#include "../StatBuilder/GraphicHead.h"
#include "../StatBuilder/GraphicSemalegXMLPlug.h"
#include "../StatBuilder/GraphicHeadXMLPlug.h"
#include "../StatBuilder/GViewSkin.h"
#include "../StatBuilder/FontRegistry.h"
#include "../StatBuilder/Measures.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/LObjectManager.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ObsoleteLoader );

using namespace std;

void TC_ObsoleteLoader::setUp()
{
    TC_Graphic::setUp_helper();
    TC_Logic::setUp_helper();
}

void TC_ObsoleteLoader::tearDown()
{
    TC_Logic::tearDown_helper();
    TC_Graphic::tearDown_helper();
}

void TC_ObsoleteLoader::ObsoleteSingleBoardBranch()
{
	//готовим документ в старом формате
	GXMLWrapperTest gwrapper( gdoc.get() );
	attic::a_document * xdoc = gdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "GRAPHIC_SCHEMATA" );
	attic::a_node sboards_node = root_node.append_child( "SINGLE_BOARDS" );
	attic::a_node elem_node = sboards_node.append_child( "element" );
	GraphElemPtr tblBoardPtr = FastCreate( TABLE_BOARD );
	CGraphicTableBoard * gtable_board = static_cast <CGraphicTableBoard *>( tblBoardPtr.get() );
	GraphicTableBoardXMLPlug xplug( *gtable_board );
	xplug.saveTo( elem_node );

	//загружаем
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	set <CGraphicElement *> table_boards;
	pgom->GetObjects( TABLE_BOARD, ALL_STATUSES, gdoc.get(), table_boards );
	CPPUNIT_ASSERT( table_boards.size() == 1 );

	//сохраняем (сохраниться должно в новом формате)
	gwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "GRAPHIC_SCHEMATA" );
	CPPUNIT_ASSERT( root_node_sav );
	attic::a_node sboards_node_sav = root_node_sav.child( "SINGLE_BOARDS" ); //это должно исчезнуть
	CPPUNIT_ASSERT( !sboards_node_sav );
	attic::a_node tboards_node_sav = root_node_sav.child( "TABLE_BOARDS" ); //это должно появиться
	CPPUNIT_ASSERT( tboards_node_sav );
	attic::a_node elem_node_sav = tboards_node_sav.child( "element" );
	CPPUNIT_ASSERT( elem_node.to_str() == elem_node_sav.to_str() ); //внутренность та же

	//загружаем в новом формате
	pgom->RemoveObject( *table_boards.begin(), gdoc.get() );
	GXMLWrapperTest( gdoc.get() ).Load( L"", gresult );
	pgom->GetObjects( TABLE_BOARD, ALL_STATUSES, gdoc.get(), table_boards );
	CPPUNIT_ASSERT( table_boards.size() == 1 );
}

void TC_ObsoleteLoader::ObsoleteGBoardBranch()
{
	//готовим документ в старом формате
	GXMLWrapperTest gwrapper( gdoc.get() );
	attic::a_document * xdoc = gdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "GRAPHIC_SCHEMATA" );
	attic::a_node sboards_node = root_node.append_child( "BOARDS" );
	attic::a_node elem_node1 = sboards_node.append_child( "element" );
	elem_node1.ensure_attribute( "name" ).set_value( "tbl_board1" );
	elem_node1.ensure_attribute( "type" ).set_value( 0 );
	attic::a_node elem_node2 = sboards_node.append_child( "element" );
	elem_node2.ensure_attribute( "name" ).set_value( "mlt_board1" );
	elem_node2.ensure_attribute( "type" ).set_value( 1 );
	attic::a_node elem_node3 = sboards_node.append_child( "element" );
	elem_node3.ensure_attribute( "name" ).set_value( "tbl_board2" );
	elem_node3.ensure_attribute( "type" ).set_value( 0 );
	attic::a_node elem_node4 = sboards_node.append_child( "element" );
	elem_node4.ensure_attribute( "name" ).set_value( "mlt_board2" );
	elem_node4.ensure_attribute( "type" ).set_value( 1 );

	//загружаем
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	set <CGraphicElement *> all_objects, tbl_boards, mlt_boards;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), all_objects );
	pgom->GetObjects( TABLE_BOARD, ALL_STATUSES, gdoc.get(), tbl_boards );
	pgom->GetObjects( MULTI_BOARD, ALL_STATUSES, gdoc.get(), mlt_boards );
	CPPUNIT_ASSERT( tbl_boards.size() == 2 );
	CPPUNIT_ASSERT( mlt_boards.size() == 2 );
	CPPUNIT_ASSERT( all_objects.size() == tbl_boards.size() + mlt_boards.size() );

	//сохраняем (сохраниться должно в новом формате)
	gwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "GRAPHIC_SCHEMATA" );
	CPPUNIT_ASSERT( root_node_sav );
	attic::a_node sboards_node_sav = root_node_sav.child( "SINGLE_BOARDS" );
	CPPUNIT_ASSERT( !sboards_node_sav );
	attic::a_node tblboards_node_sav = root_node_sav.child( "TABLE_BOARDS" );
	CPPUNIT_ASSERT( tblboards_node_sav );
	attic::a_node savnode1 = tblboards_node_sav.child( "element" );
	CPPUNIT_ASSERT( savnode1 );
	CPPUNIT_ASSERT( savnode1.attribute( "name" ).as_string() == string( "tbl_board1" ) );
	attic::a_node savnode2 = savnode1.next_sibling();
	CPPUNIT_ASSERT( savnode2 && savnode2.name_is( "element" ) );
	CPPUNIT_ASSERT( savnode2.attribute( "name" ).as_string() == string( "tbl_board2" ) );

	attic::a_node mltboards_node_sav = root_node_sav.child( "MULTI_BOARDS" );
	CPPUNIT_ASSERT( mltboards_node_sav );
	attic::a_node savnode3 = mltboards_node_sav.child( "element" );
	CPPUNIT_ASSERT( savnode3 );
	CPPUNIT_ASSERT( savnode3.attribute( "name" ).as_string() == string( "mlt_board1" ) );
	attic::a_node savnode4 = savnode3.next_sibling();
	CPPUNIT_ASSERT( savnode4 && savnode4.name_is( "element" ) );
	CPPUNIT_ASSERT( savnode4.attribute( "name" ).as_string() == string( "mlt_board2" ) );

	//загружаем в новом формате
	for_each( all_objects.begin(), all_objects.end(), [this]( CGraphicElement * ge ){
		pgom->RemoveObject( ge, gdoc.get() );
	} );
	GXMLWrapperTest( gdoc.get() ).Load( L"", gresult );
	pgom->GetObjects( TABLE_BOARD, ALL_STATUSES, gdoc.get(), tbl_boards );
	CPPUNIT_ASSERT( tbl_boards.size() == 2 );
	pgom->GetObjects( MULTI_BOARD, ALL_STATUSES, gdoc.get(), mlt_boards );
	CPPUNIT_ASSERT( mlt_boards.size() == 2 );
}

void TC_ObsoleteLoader::ObsoleteSemaforsBranch()
{
	//готовим документ в старом формате
	GXMLWrapperTest gwrapper( gdoc.get() );
	attic::a_document * xdoc = gdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "GRAPHIC_SCHEMATA" );
	attic::a_node sboards_node = root_node.append_child( "SEMAFORS" );
	attic::a_node elem_node = sboards_node.append_child( "element" );
	GraphElemPtr semalegPtr = FastCreate( SEMALEG );
	CGraphicSemaleg * gleg = static_cast <CGraphicSemaleg *>( semalegPtr.get() );
	GraphicSemalegXMLPlug xplug( *gleg );
	xplug.saveTo( elem_node );
	attic::a_node leglink_node = elem_node.append_child( "link" );
	leglink_node.append_attribute( "name" ).set_value( "Legname" );
	leglink_node.append_attribute( "ESR_code" ).set_value( 71200 );
	leglink_node.append_attribute( "class" ).set_value( SEMALEG );
	leglink_node.append_attribute( "type" ).set_value( 0 );
	attic::a_node headlink_node = elem_node.append_child( "link" );
	headlink_node.append_attribute( "name" ).set_value( "Headname" );
	headlink_node.append_attribute( "ESR_code" ).set_value( 71200 );
	headlink_node.append_attribute( "class" ).set_value( HEAD );
	headlink_node.append_attribute( "type" ).set_value( 0 );

	//загружаем
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	set <CGraphicElement *> glegs, gheads, all_objects;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	pgom->GetObjects( HEAD, ALL_STATUSES, gdoc.get(), gheads );
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), all_objects );
	CPPUNIT_ASSERT( all_objects.size() == glegs.size() + gheads.size() );
	CPPUNIT_ASSERT( glegs.size() == 1 && gheads.size() == 1 );

	//сохраняем (сохраниться должно в новом формате)
	gwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "GRAPHIC_SCHEMATA" );
	CPPUNIT_ASSERT( root_node_sav );
	attic::a_node legs_node_sav = root_node_sav.child( "SEMALEGS" );
	CPPUNIT_ASSERT( legs_node_sav );
	attic::a_node semafors_node_sav = root_node_sav.child( "SEMAFORS" );
	CPPUNIT_ASSERT( !semafors_node_sav );
	attic::a_node elem_node_sav = legs_node_sav.child( "element" );
	CPPUNIT_ASSERT( elem_node.to_str() == elem_node_sav.to_str() ); //внутренность та же

	//загружаем в новом формате
	for_each( all_objects.begin(), all_objects.end(), [this]( CGraphicElement * ge ){
		pgom->RemoveObject( ge, gdoc.get() );
	} );
	GXMLWrapperTest( gdoc.get() ).Load( L"", gresult );
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	CPPUNIT_ASSERT( glegs.size() == 1 );
	pgom->GetObjects( HEAD, ALL_STATUSES, gdoc.get(), gheads );
	CPPUNIT_ASSERT( gheads.size() == 1 );
}

void TC_ObsoleteLoader::ObsoleteFontsRemoving()
{
	//готовим документ в старом формате
	GXMLWrapperTest gwrapper( gdoc.get() );
	attic::a_document * xdoc = gdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "GRAPHIC_SCHEMATA" );
	root_node.append_attribute( "attribute1" );
	root_node.append_attribute( "board_font_name" ).set_value( "Impact" );
	root_node.append_attribute( "board_font_height" ).set_value( 19 );
	root_node.append_attribute( "attribute2" );

	//загружаем
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	FontRegistry * fntreg = gdoc->GetSkin()->GetFontRegistry();
	FontRegistry::FontIteratorPtr tbfont_ptr = fntreg->CreateIterator( TABLE_BOARD );
	CPPUNIT_ASSERT( tbfont_ptr->GetFontSize() == 19 );
	CPPUNIT_ASSERT( tbfont_ptr->GetFontName() == L"Impact" );
	FontRegistry::FontIteratorPtr mbfont_ptr = fntreg->CreateIterator( MULTI_BOARD );
	CPPUNIT_ASSERT( mbfont_ptr->GetFontSize() == 19 );
	CPPUNIT_ASSERT( mbfont_ptr->GetFontName() == L"Impact" );

	//сохраняем (сохраниться должно в новом формате)
	gwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "GRAPHIC_SCHEMATA" );
	attic::a_attribute attr1 = root_node_sav.first_attribute();
	CPPUNIT_ASSERT( attr1 && attr1.name() == string( "attribute1" ) );

	CPPUNIT_ASSERT( !root_node_sav.attribute( "board_font_name" ) );
	CPPUNIT_ASSERT( !root_node_sav.attribute( "board_font_height" ) );
	attic::a_attribute tb_font_name = root_node_sav.attribute( "table_board_font_name" );
	CPPUNIT_ASSERT( tb_font_name );
	CPPUNIT_ASSERT( tb_font_name.value() == string( "Impact" ) );
	attic::a_attribute tb_font_height = root_node_sav.attribute( "table_board_font_height" );
	CPPUNIT_ASSERT( tb_font_height );
	CPPUNIT_ASSERT( tb_font_height.value() == string( "19" ) );
	attic::a_attribute mb_font_name = root_node_sav.attribute( "multi_board_font_name" );
	CPPUNIT_ASSERT( mb_font_name );
	CPPUNIT_ASSERT( mb_font_name.value() == string( "Impact" ) );
	attic::a_attribute mb_font_height = root_node_sav.attribute( "multi_board_font_height" );
	CPPUNIT_ASSERT( mb_font_height );
	CPPUNIT_ASSERT( mb_font_height.value() == string( "19" ) );

	attic::a_attribute attr2 = mb_font_height.next_attribute();
	CPPUNIT_ASSERT( attr2 && attr2.name() == string( "attribute2" ) );

	//загружаем в новом формате
	GXMLWrapperTest( gdoc.get() ).Load( L"", gresult );
	FontRegistry::FontIteratorPtr tbfont_ptr2 = fntreg->CreateIterator( TABLE_BOARD );
	CPPUNIT_ASSERT( tbfont_ptr2->GetFontSize() == 19 );
	CPPUNIT_ASSERT( tbfont_ptr2->GetFontName() == L"Impact" );
	FontRegistry::FontIteratorPtr mbfont_ptr2 = fntreg->CreateIterator( MULTI_BOARD );
	CPPUNIT_ASSERT( mbfont_ptr2->GetFontSize() == 19 );
	CPPUNIT_ASSERT( mbfont_ptr2->GetFontName() == L"Impact" );
}

void TC_ObsoleteLoader::ObsoleteSwitchRadius()
{
	//готовим документ в старом формате
	GXMLWrapperTest gwrapper( gdoc.get() );
	attic::a_document * xdoc = gdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "GRAPHIC_SCHEMATA" );
	double old_swrad = 10.;
	root_node.append_attribute( "switch_radius" ).set_value( old_swrad );

	//загружаем
	CMeasures * msrs = gdoc->GetSkin()->GetMeasures();
	double old_blen = msrs->GetBladeRelLength();
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	double swrad = msrs->GetSwitchRadius();
	CPPUNIT_ASSERT( swrad == ObsoleteLoader::SWITCH_SCALE_FACTOR * old_swrad );
	double blade_rellen = msrs->GetBladeRelLength();
	CPPUNIT_ASSERT( blade_rellen == abs( round( old_blen / ObsoleteLoader::SWITCH_SCALE_FACTOR, 2 ) ) );

	//сохраняем (сохраниться должно в новом формате)
	gwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "GRAPHIC_SCHEMATA" );
	CPPUNIT_ASSERT( !root_node_sav.attribute( "switch_radius" ) );
	attic::a_attribute swrad_ext_attr = root_node_sav.attribute( "switch_radius_ext" );
	CPPUNIT_ASSERT( swrad_ext_attr );
	CPPUNIT_ASSERT( swrad_ext_attr.as_double() == 18 );
	attic::a_attribute bllen_attr = root_node_sav.attribute( "blade_length" );
	CPPUNIT_ASSERT( bllen_attr );
	CPPUNIT_ASSERT( bllen_attr.as_double() == 0.37 );

	//загружаем в новом формате
	GXMLWrapperTest( gdoc.get() ).Load( L"", gresult );
	CPPUNIT_ASSERT( msrs->GetSwitchRadius() == swrad );
	CPPUNIT_ASSERT( msrs->GetBladeRelLength() == blade_rellen );
}

void TC_ObsoleteLoader::ObsoleteStationAttributes()
{
	//готовим документ в старом формате
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "LOGIC_COLLECTION" );
	root_node.append_attribute( "station" );
	root_node.append_attribute( "ESR_code" ).set_value( 100 );
	root_node.append_attribute( "tech_minutes_time" ).set_value( 135 );

	//загружаем
	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );

	//сохраняем (сохраниться должно в новом формате)
	lwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "LOGIC_COLLECTION" );
	CPPUNIT_ASSERT( !root_node_sav.attribute( "station" ) );
	CPPUNIT_ASSERT( !root_node_sav.attribute( "tech_minutes_time" ) );
}

void TC_ObsoleteLoader::LStripArrdepToWaynumStation()
{
	//готовим документ в старом формате
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "LOGIC_COLLECTION" );
	root_node.append_attribute( "station" );
	root_node.append_attribute( "ESR_code" ).set_value( 31000 );
	attic::a_node strips_node = root_node.append_child( "STRIPS" );
	attic::a_node elem_node = strips_node.append_child( "element" );
	elem_node.append_attribute( "name" ).set_value( L"11п" );
	elem_node.append_attribute( "arrdep" ).set_value( true );
	attic::a_node elem_node2 = strips_node.append_child( "element" );
	elem_node2.append_attribute( "name" ).set_value( L"2/5сп" );
	elem_node2.append_attribute( "arrdep" ).set_value( false );
	attic::a_node elem_node3 = strips_node.append_child( "element" );
	elem_node3.append_attribute( "name" ).set_value( L"206/208" );

	//загружаем
	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );

	//сохраняем (сохраниться должно в новом формате)
	lwrapper.Save( L"" );

	attic::a_node root_node_sav = xdoc->child( "LOGIC_COLLECTION" );
	attic::a_node strips_node_sav = root_node_sav.child( "STRIPS" );
	CPPUNIT_ASSERT( strips_node_sav );
	attic::a_node savnode1 = strips_node_sav.child( "element" );
	CPPUNIT_ASSERT( savnode1 );
	CPPUNIT_ASSERT( savnode1.attribute( "name" ).as_wstring() == L"11п" );
	CPPUNIT_ASSERT( !savnode1.attribute( "arrdep" ) );
	CPPUNIT_ASSERT( savnode1.attribute( "waynum" ).as_wstring() == L"11" );
	attic::a_node savnode2 = savnode1.next_sibling();
	CPPUNIT_ASSERT( savnode2 && savnode2.name_is( "element" ) );
	CPPUNIT_ASSERT( !savnode2.attribute( "arrdep" ) );
	CPPUNIT_ASSERT( savnode2.attribute( "name" ).as_wstring() == L"2/5сп" );
	CPPUNIT_ASSERT( !savnode2.attribute( "waynum" ) );
	attic::a_node savnode3 = savnode2.next_sibling();
	CPPUNIT_ASSERT( savnode3 && savnode3.name_is( "element" ) );
	CPPUNIT_ASSERT( !savnode3.attribute( "arrdep" ) );
	CPPUNIT_ASSERT( !savnode3.attribute( "waynum" ) );
	CPPUNIT_ASSERT( savnode3.attribute( "name" ).as_wstring() == L"206/208" );

	//загружаем в новом формате
	set <const CLogicElement *> all_objects;
	plom->GetObjects( ALL_CLASSES, logdoc.get(), all_objects );
	for_each( all_objects.begin(), all_objects.end(), [this]( const CLogicElement * le ){
		plom->RemoveObject( le, logdoc.get() );
	} );

	LXMLWrapperTest( logdoc.get() ).Load( L"", lresult );
	set <const CLogicElement *> lstrips;
	plom->GetObjects( STRIP, logdoc.get(), lstrips );
	CPPUNIT_ASSERT( lstrips.size() == 3 );
	for ( auto lelem : lstrips )
	{
		const CLogicStrip * lstrip = static_cast <const CLogicStrip *>( lelem );
		const auto & sname = lstrip->GetName();
		string wnumstr = lstrip->GetWaynum();
		if ( sname == L"11п" )
			CPPUNIT_ASSERT( wnumstr == "11" );
		else if ( sname == L"2/5сп" )
			CPPUNIT_ASSERT( wnumstr.empty() );
		else
		{
			CPPUNIT_ASSERT( sname == L"206/208" );
			CPPUNIT_ASSERT( wnumstr.empty() );
		}
	}
}

void TC_ObsoleteLoader::LStripArrdepToWaynumSpan()
{
	//готовим документ в старом формате
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "LOGIC_COLLECTION" );
	root_node.append_attribute( "station" );
	root_node.append_attribute( "ESR_code" ).set_value( "31000:31001" );
	attic::a_node strips_node = root_node.append_child( "STRIPS" );
	attic::a_node elem_node = strips_node.append_child( "element" );
	elem_node.append_attribute( "name" ).set_value( L"2ЧУ" );
	elem_node.append_attribute( "arrdep" ).set_value( true );
	attic::a_node elem_node2 = strips_node.append_child( "element" );
	elem_node2.append_attribute( "name" ).set_value( L"3ЧУ" );
	elem_node2.append_attribute( "arrdep" ).set_value( false );
	attic::a_node elem_node3 = strips_node.append_child( "element" );
	elem_node3.append_attribute( "name" ).set_value( L"4ЧУ" );
	elem_node3.append_attribute( "span_number_way" ).set_value( 2 );

	//загружаем
	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );

	//сохраняем (сохраниться должно в новом формате)
	lwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "LOGIC_COLLECTION" );
	attic::a_node strips_node_sav = root_node_sav.child( "STRIPS" );
	CPPUNIT_ASSERT( strips_node_sav );
	attic::a_node savnode1 = strips_node_sav.child( "element" );
	CPPUNIT_ASSERT( savnode1 );
	CPPUNIT_ASSERT( savnode1.attribute( "name" ).as_wstring() == L"2ЧУ" );
	CPPUNIT_ASSERT( !savnode1.attribute( "arrdep" ) );
	CPPUNIT_ASSERT( !savnode1.attribute( "waynum" ) );
	attic::a_node savnode2 = savnode1.next_sibling();
	CPPUNIT_ASSERT( savnode2 && savnode2.name_is( "element" ) );
	CPPUNIT_ASSERT( savnode2.attribute( "name" ).as_wstring() == L"3ЧУ" );
	CPPUNIT_ASSERT( !savnode2.attribute( "arrdep" ) );
	CPPUNIT_ASSERT( !savnode2.attribute( "waynum" ) );
	attic::a_node savnode3 = savnode2.next_sibling();
	CPPUNIT_ASSERT( savnode3 && savnode3.name_is( "element" ) );
	CPPUNIT_ASSERT( savnode3.attribute( "name" ).as_wstring() == L"4ЧУ" );
	CPPUNIT_ASSERT( !savnode3.attribute( "arrdep" ) );
	CPPUNIT_ASSERT( savnode3.attribute( "waynum" ).as_uint() == 2 );

	//загружаем в новом формате
	set <const CLogicElement *> all_objects;
	plom->GetObjects( ALL_CLASSES, logdoc.get(), all_objects );
	for_each( all_objects.begin(), all_objects.end(), [this]( const CLogicElement * le ){
		plom->RemoveObject( le, logdoc.get() );
	} );
	LXMLWrapperTest( logdoc.get() ).Load( L"", lresult );
	set <const CLogicElement *> lstrips;
	plom->GetObjects( STRIP, logdoc.get(), lstrips );
	CPPUNIT_ASSERT( lstrips.size() == 3 );
	for ( auto lelem : lstrips )
	{
		const CLogicStrip * lstrip = static_cast <const CLogicStrip *>( lelem );
		const auto & sname = lstrip->GetName();
		string wnumstr = lstrip->GetWaynum();
		if ( sname == L"2ЧУ" )
			CPPUNIT_ASSERT( wnumstr.empty() );
		else if ( sname == L"3ЧУ" )
			CPPUNIT_ASSERT( wnumstr.empty() );
		else
		{
			CPPUNIT_ASSERT( sname == L"4ЧУ" );
			CPPUNIT_ASSERT( wnumstr == "2" );
		}
	}
}

void TC_ObsoleteLoader::DeleteOecLogicLegType()
{
	//готовим документ в старом формате
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "LOGIC_COLLECTION" );
	root_node.append_attribute( "station" );
	root_node.append_attribute( "ESR_code" ).set_value( 31000 );
	attic::a_node legs_node = root_node.append_child( "SEMALEGS" );
	attic::a_node elem_node = legs_node.append_child( "element" );
	elem_node.append_attribute( "name" ).set_value( L"Ч" );
	elem_node.append_attribute( "type" ).set_value( 1 );

	//загружаем
	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );

	//сохраняем (сохраниться должно в новом формате)
	lwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "LOGIC_COLLECTION" );
	attic::a_node legs_node_sav = root_node_sav.child( "SEMALEGS" );
	CPPUNIT_ASSERT( legs_node_sav );
	attic::a_node savnode1 = legs_node_sav.child( "element" );
	CPPUNIT_ASSERT( savnode1 );
	CPPUNIT_ASSERT( savnode1.attribute( "name" ).as_wstring() == L"Ч" );
	CPPUNIT_ASSERT( !savnode1.attribute( "type" ) );
}

void TC_ObsoleteLoader::DeleteLogLinkType()
{
	//готовим документ в старом формате
	GXMLWrapperTest gwrapper( gdoc.get() );
	attic::a_document * xdoc = gdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "GRAPHIC_SCHEMATA" );
	attic::a_node slegs_node = root_node.append_child( "SEMALEGS" );

	//первый светофор (мачта и голова)
	attic::a_node elem_node = slegs_node.append_child( "element" );
	GraphElemPtr glegPtr = FastCreate( SEMALEG );
	GraphElemPtr gheadPtr = FastCreate( HEAD );
	CGraphicSemaleg * gleg = static_cast <CGraphicSemaleg *>( glegPtr.get() );
	gleg->SetLogicLink( CLink( SEMALEG, BadgeE( L"LegName", EsrKit( 1 ) ) ) );
	CGraphicHead * ghead = static_cast <CGraphicHead *>( gheadPtr.get() );
	ghead->SetLogicLink( CLink( HEAD, BadgeE( L"HeadName", EsrKit( 1 ) ) ) );
	ghead->SetType( REPEAT );
	CGridPoint hp( 10, 10 );
	gleg->SetHotPoint( hp );
	gleg->SetSizes( 2, 2 );
	GraphicSemalegXMLPlug xplug( *gleg );
	xplug.saveTo( elem_node );
	attic::a_node head_node = elem_node.append_child( "link" );
	GraphicHeadXMLPlug hplug( *ghead );
	hplug.saveTo( head_node );

	//второй светофор (голова без мачты)
	attic::a_node elem_node2 = slegs_node.append_child( "element" );
	GraphElemPtr glegPtr2 = FastCreate( SEMALEG );
	GraphElemPtr gheadPtr2 = FastCreate( HEAD );
	CGraphicSemaleg * gleg2 = static_cast <CGraphicSemaleg *>( glegPtr2.get() );
	CGraphicHead * ghead2 = static_cast <CGraphicHead *>( gheadPtr2.get() );
	ghead2->SetLogicLink( CLink( HEAD, BadgeE( L"HeadName", EsrKit( 1 ) ) ) );
	ghead2->SetType( REPEAT );
	GraphicSemalegXMLPlug xplug2( *gleg2 );
	xplug2.saveTo( elem_node2 );
	attic::a_node head_node2 = elem_node2.append_child( "link" );
	GraphicHeadXMLPlug hplug2( *ghead2 );
	hplug2.saveTo( head_node2 );

	//загружаем
	GLoadResult gresult( L"" );
	gwrapper.Load( L"", gresult );
	set <CGraphicElement *> gelems;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), gelems );
	CPPUNIT_ASSERT( gelems.size() == 4 );

	//сохраняем (сохраниться должно новом формате)
	gwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "GRAPHIC_SCHEMATA" );
	CPPUNIT_ASSERT( root_node_sav );
	attic::a_node legs_node_sav = root_node_sav.child( "SEMALEGS" );
	CPPUNIT_ASSERT( legs_node_sav );
	attic::a_node savnode = legs_node_sav.child( "element" );
	CPPUNIT_ASSERT( savnode );
	attic::a_node leglinknode = savnode.child( "link" );
	CPPUNIT_ASSERT( !leglinknode.attribute( "type" ) ); //тип для основного линка мачты (как и других графических типов) удаляется
	attic::a_node headlinknode = leglinknode.next_sibling( "link" );
	attic::a_attribute headtypeAttr = headlinknode.attribute( "type" );
	CPPUNIT_ASSERT( headtypeAttr ); //тип для линка головы сохраняется (это исключение). он используется в типе графического светофора
	CPPUNIT_ASSERT( headtypeAttr.as_int() == REPEAT ); //тип для линка головы

	attic::a_node savnode2 = savnode.next_sibling( "element" );
	CPPUNIT_ASSERT( savnode2 );
	attic::a_node headlinknode2 = savnode2.child( "link" );
	attic::a_attribute headtypeAttr2 = headlinknode2.attribute( "type" );
	CPPUNIT_ASSERT( headtypeAttr2 ); //тип для линка головы сохраняется (это исключение). он используется в типе графического светофора
	CPPUNIT_ASSERT( headtypeAttr2.as_int() == REPEAT ); //тип для линка головы
}

void TC_ObsoleteLoader::LStripCapacityToTwiceCapacity()
{
	//готовим документ в старом формате
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "LOGIC_COLLECTION" );
	root_node.append_attribute( "station" );
	root_node.append_attribute( "ESR_code" ).set_value( "31000:31001" );
	attic::a_node strips_node = root_node.append_child( "STRIPS" );
	attic::a_node elem_node = strips_node.append_child( "element" );
	elem_node.append_attribute( "name" ).set_value( L"1A" );
	elem_node.append_attribute( "capacity" ).set_value( 83 );

	//загружаем
	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );

	//сохраняем (сохраниться должно в новом формате)
	lwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "LOGIC_COLLECTION" );
	attic::a_node strips_node_sav = root_node_sav.child( "STRIPS" );
	CPPUNIT_ASSERT( strips_node_sav );
	attic::a_node savnode = strips_node_sav.child( "element" );
	CPPUNIT_ASSERT( savnode );
	CPPUNIT_ASSERT( savnode.attribute( "name" ).as_wstring() == L"1A" );
	CPPUNIT_ASSERT( !savnode.attribute( "capacity" ) );
	CPPUNIT_ASSERT( savnode.attribute( "even_capacity" ).as_uint() == 83 );
	CPPUNIT_ASSERT( savnode.attribute( "odd_capacity" ).as_uint() == 83 );
}

void TC_ObsoleteLoader::DeleteJointsParityChanging()
{
	//готовим документ в старом формате
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	attic::a_node root_node = xdoc->set_document_element( "LOGIC_COLLECTION" );
	root_node.append_attribute( "station" );
	root_node.append_attribute( "ESR_code" ).set_value( 31000 );
	attic::a_node joints_node = root_node.append_child( "JOINTS" );
	attic::a_node elem_node = joints_node.append_child( "element" );
	elem_node.append_attribute( "name" ).set_value( L"S1~S2" );
	elem_node.append_attribute( "parity_change" ).set_value( true );
	attic::a_node elem_node2 = joints_node.append_child( "element" );
	elem_node2.append_attribute( "name" ).set_value( L"S2~S3" );
	elem_node2.append_attribute( "parity_change" ).set_value( false );

	//загружаем
	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );

	//сохраняем (сохраниться должно в новом формате)
	lwrapper.Save( L"" );
	attic::a_node root_node_sav = xdoc->child( "LOGIC_COLLECTION" );
	attic::a_node joints_node_sav = root_node_sav.child( "JOINTS" );
	CPPUNIT_ASSERT( joints_node_sav );
	attic::a_node savnode1 = joints_node_sav.child( "element" );
	CPPUNIT_ASSERT( savnode1 );
	CPPUNIT_ASSERT( savnode1.attribute( "name" ).as_wstring() == L"S1~S2" );
	CPPUNIT_ASSERT( !savnode1.attribute( "parity_change" ) );
	attic::a_node savnode2 = savnode1.next_sibling( "element" );
	CPPUNIT_ASSERT( savnode2 );
	CPPUNIT_ASSERT( savnode2.attribute( "name" ).as_wstring() == L"S2~S3" );
	CPPUNIT_ASSERT( !savnode2.attribute( "parity_change" ) );
}