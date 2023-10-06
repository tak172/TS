#include "stdafx.h"
#include "TC_GElemXMLPlug.h"
#include "../StatBuilder/GraphicMultiBoardXMLPlug.h"
#include "../StatBuilder/GraphicMultiBoard.h"
#include "GraphicXMLPlugTest.h"
#include "../StatBuilder/IndividualNameFont.h"
#include "../StatBuilder/GraphicStrip.h"
#include "../StatBuilder/GraphicStripXMLPlug.h"
#include "../StatBuilder/GraphicText.h"
#include "../StatBuilder/GraphicTextXMLPlug.h"
#include "../StatBuilder/ColorGamut.h"
#include "../StatBuilder/GraphicLandscape.h"
#include "../StatBuilder/GraphicLandscapeXMLPlug.h"
#include "../StatBuilder/GraphicTableBoard.h"
#include "../StatBuilder/GraphicTableBoardXMLPlug.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GElemXMLPlug );

using namespace std;

void TC_GElemXMLPlug::SaveElement()
{
	GraphElemPtr gePtr = FastCreate( FIGURE );
	gePtr->SetName( L"gobject_name" );
	gePtr->IndividualFontUsing( true );
	Gdiplus::Font gefont( L"times new roman", 2.5, Gdiplus::FontStyleBold );
	boost::scoped_ptr <IndividualNameFont> ifontPtr( new IndividualNameFont( &gefont ) );
	gePtr->SetIndividualFont( ifontPtr.get() );
	gePtr->SetHotPoint( CGridPoint( 20.22, 250.79 ) );
	gePtr->SetNameShift( CGridSize( -0.3, 4.1 ) );
	gePtr->SetGroupName( L"group_name" );
    gePtr->SetSofReference( L"sof_reference" );
	gePtr->SetLogicLink( CLink( FIGURE, BadgeE( L"link_name", EsrKit( 10100 ) ) ) );

	GraphicXMLPlugTest xplug( *gePtr );
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	xplug.saveTo( root_node );

	CPPUNIT_ASSERT( root_node.attribute( "name" ).as_string() == string( "gobject_name" ) );
	CPPUNIT_ASSERT( root_node.attribute( "font" ).as_string() == string( "Times New Roman" ) );
	CPPUNIT_ASSERT( root_node.attribute( "fontstyle" ).as_def_int( 0 ) == Gdiplus::FontStyleBold );
	CPPUNIT_ASSERT( root_node.attribute( "fontsize" ).as_double() == 2.5 );
	CPPUNIT_ASSERT( root_node.attribute( "hp_x" ).as_double() == 20.22 );
	CPPUNIT_ASSERT( root_node.attribute( "hp_y" ).as_double() == 250.79 );
	CPPUNIT_ASSERT( root_node.attribute( "nameshift_x" ).as_double() == -0.3 );
	CPPUNIT_ASSERT( root_node.attribute( "nameshift_y" ).as_double() == 4.1 );
	CPPUNIT_ASSERT( root_node.attribute( "group" ).as_string() == string( "group_name" ) );
    CPPUNIT_ASSERT( root_node.attribute( "sof_ref" ).as_string() == string( "sof_reference" ) );
	attic::a_node link_child = root_node.child( "link" );
	CPPUNIT_ASSERT( link_child.attribute( "name" ).as_string() == string( "link_name" ) );
	CPPUNIT_ASSERT( link_child.attribute( "ESR_code" ).as_string() == string( "10100" ) );
	CPPUNIT_ASSERT( link_child.attribute( "class" ).as_int() == FIGURE );
}

void TC_GElemXMLPlug::LoadElement()
{
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	root_node.ensure_attribute( "name" ).set_value( "gobject_name" );
	root_node.ensure_attribute( "font" ).set_value( "Times New Roman" );
	root_node.ensure_attribute( "fontstyle" ).set_value( Gdiplus::FontStyleBold );
	root_node.ensure_attribute( "fontsize" ).set_value( 2.5 );
	root_node.ensure_attribute( "hp_x" ).set_value( 20.22 );
	root_node.ensure_attribute( "hp_y" ).set_value( 250.79 );
	root_node.ensure_attribute( "nameshift_x" ).set_value( -0.3 );
	root_node.ensure_attribute( "nameshift_y" ).set_value( 4.1 );
	root_node.ensure_attribute( "group" ).set_value( "group_name" );
    root_node.ensure_attribute( "sof_ref" ).set_value( "My_sof_reference" );
	attic::a_node link_child = root_node.append_child( "link" );
	link_child.ensure_attribute( "name" ).set_value( "link_name" );
	link_child.ensure_attribute( "ESR_code" ).set_value( "10100" );
	link_child.ensure_attribute( "class" ).set_value( FIGURE );

	GraphElemPtr gePtr = FastCreate( FIGURE );
	GraphicXMLPlugTest xplug( *gePtr );
	xplug.loadFrom( root_node );

	CPPUNIT_ASSERT( gePtr->GetName() == L"gobject_name" );
	const IndividualNameFont * infont = gePtr->IndividualFont();
	CPPUNIT_ASSERT( infont->GetName() == L"Times New Roman" );
	CPPUNIT_ASSERT( infont->GetSize() == 2.5 );
	CPPUNIT_ASSERT( gePtr->GetHotPoint() == CGridPoint( 20.22, 250.79 ) );
	CGridSize nameshift = gePtr->GetNameShift();
	CPPUNIT_ASSERT( nameshift == CGridSize( -0.3, 4.1 ) );
	CPPUNIT_ASSERT( gePtr->GetGroupName() == L"group_name" );
    CPPUNIT_ASSERT( gePtr->GetSofReference() == L"My_sof_reference" );
 	const CLink & gelink = gePtr->GetLogicLink();
	CPPUNIT_ASSERT( gePtr->GetLogicLink().obj_badge == BadgeE( L"link_name", EsrKit( 10100 ) ) );
}

void TC_GElemXMLPlug::SaveBoard()
{
	GraphElemPtr gePtr = FastCreate( TABLE_BOARD );
	CGraphicBoard & gboard = static_cast <CGraphicBoard &>( *gePtr );
	double width, height;
	gboard.GetSizes( width, height );
	double min_size = min( width, height );
	vector <double> shifts;
	shifts.push_back( 0.4 * min_size );
	shifts.push_back( 0.6 * min_size );
	shifts.push_back( 0.8 * min_size );
	gboard.SetShifts( shifts );
	gboard.SetDirection( RIGHT_TO_LEFT );

	GraphicBoardXMLPlug mb_plug( gboard );
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	mb_plug.saveTo( root_node );

	vector <double> node_shifts;
	attic::a_node point_node = root_node.child( "point" );
	while( point_node )
	{
		node_shifts.push_back( point_node.attribute( "value" ).as_double() );
		point_node = point_node.next_sibling();
	}
	CPPUNIT_ASSERT( shifts == node_shifts );
	CPPUNIT_ASSERT( root_node.attribute( "direction" ).as_string() == string( "rtol" ) );
}

void TC_GElemXMLPlug::LoadBoard()
{
	GraphElemPtr gePtr = FastCreate( TABLE_BOARD );
	gePtr->SetSizes( 5, 20 );
	CGraphicBoard & gboard = static_cast <CGraphicBoard &>( *gePtr );

	attic::a_document tst_doc, tst_doc2;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	root_node.ensure_attribute( "direction" ).set_value( "rtol" );
	attic::a_node root_node2 = tst_doc2.set_document_element( "root_node" );
	root_node2.ensure_attribute( "direction" ).set_value( "ltor" );
	attic::a_node point_node1 = root_node2.append_child( "point" );
	point_node1.ensure_attribute( "value" ).set_value( 3.5 );
	attic::a_node point_node2 = root_node2.append_child( "point" );
	point_node2.ensure_attribute( "value" ).set_value( 8.72 );

	GraphicBoardXMLPlug bplug( gboard );
	bplug.loadFrom( root_node );
	CPPUNIT_ASSERT( gboard.GetDirection() == RIGHT_TO_LEFT );
	CPPUNIT_ASSERT( gboard.GetShifts().empty() );
	bplug.loadFrom( root_node2 );
	CPPUNIT_ASSERT( gboard.GetDirection() == LEFT_TO_RIGHT );
	CPPUNIT_ASSERT( gboard.GetShifts().size() == 2 );
	auto gshifts = gboard.GetShifts();
	CPPUNIT_ASSERT( gshifts.size() == 2 );
	CPPUNIT_ASSERT( GRID_COORD( gshifts[0] ) == GRID_COORD( 3.5 ) );
	CPPUNIT_ASSERT( GRID_COORD( gshifts[1] ) == GRID_COORD( 8.72 ) );
}


static bool checkDirty( CGraphicBoard &gboard, double * a, double * b, size_t sz )
{
    const bool CORRECTED = true;
    const bool UNTOUCHED = false;

    vector<double> src;
    src.assign( a, a+sz );
    if ( CORRECTED != gboard.SetShifts( src ) ) // проверяем что корректировка БЫЛА
        return false;
    vector<double> out = gboard.GetShifts();
    if ( out.size() != sz ) // количество точек сохраняется
        return false;
    for( unsigned i=0; i<sz; ++i ) // проверка по эталону
    {
        if ( fabs(out[i] - b[i]) > REAL_EPSILON )
            return false;
    }
    if ( UNTOUCHED != gboard.SetShifts( out ) ) // повторно корректировки уже НЕ будет
        return false;
    // значит всё хорошо
    return true;
}

void TC_GElemXMLPlug::CorrectDirtyBoard()
{
    GraphElemPtr gePtr = FastCreate( TABLE_BOARD );
    CGraphicBoard & gboard = static_cast <CGraphicBoard &>( *gePtr );
    const double SOME = 200;
    {
        // точки ниже дна, нормальные и выше крыши
        gboard.SetSizes( SOME, 10.0 );
        double a[] = { -99.0, -77.0, 3.0, 7.0, +77.0, +99.0 };
        double b[] = {   1.0,   2.0, 3.0, 7.0,   8.0,   9.0 };
        CPPUNIT_ASSERT( size_array(a) ==  size_array(b) );
        CPPUNIT_ASSERT( checkDirty(gboard, a, b, size_array(a) ) );
    }
    {
        // точки все ниже дна
        gboard.SetSizes( SOME, 3.0 );
        double a[] = { -99.0, -77.0,  };
        double b[] = {   1.0,   2.0,  };
        CPPUNIT_ASSERT( size_array(a) ==  size_array(b) );
        CPPUNIT_ASSERT( checkDirty(gboard, a, b, size_array(a) ) );
    }
    {
        // точки все выше крыши
        gboard.SetSizes( SOME, 3.0 );
        double a[] = { +99.0, +77.0,  };
        double b[] = {   1.0,   2.0,  };
        CPPUNIT_ASSERT( size_array(a) ==  size_array(b) );
        CPPUNIT_ASSERT( checkDirty(gboard, a, b, size_array(a) ) );
    }
    {
        // точки ниже дна или выше крыши (нет нормальных)
        gboard.SetSizes( SOME, 3.0 );
        double a[] = { -99.0,  +7.0,  };
        double b[] = {   1.0,   2.0,  };
        CPPUNIT_ASSERT( size_array(a) ==  size_array(b) );
        CPPUNIT_ASSERT( checkDirty(gboard, a, b, size_array(a) ) );
    }
}

void TC_GElemXMLPlug::SaveMultiBoard()
{
	GraphElemPtr gePtr = FastCreate( MULTI_BOARD );
	CGraphicMultiBoard & gmboard = static_cast <CGraphicMultiBoard &>( *gePtr );
	gmboard.SetParity( ODD_PARITY );
	gmboard.SetBindsHidden( true );
	GraphicMultiBoardXMLPlug mb_plug( gmboard );
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	mb_plug.saveTo( root_node );
	CPPUNIT_ASSERT( root_node.attribute( "parity" ).as_string() == string( "odd" ) );
	CPPUNIT_ASSERT( root_node.attribute( "hidebinds" ).as_def_bool( false ) == true );
}

void TC_GElemXMLPlug::LoadMultiBoard()
{
	GraphElemPtr gePtr = FastCreate( MULTI_BOARD );
	CGraphicMultiBoard & gmboard = static_cast <CGraphicMultiBoard &>( *gePtr );
	double width, height;
	gmboard.GetSizes( width, height );
	gmboard.SetSizes( width, 100 );

	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	root_node.ensure_attribute( "direction" ).set_value( "rtol" ); 
	root_node.ensure_attribute( "parity" ).set_value( "any" );
	root_node.ensure_attribute( "hidebinds" ).set_value( false );
	root_node.append_child( "point" ).ensure_attribute( "value" ).set_value( 0.5 * width );
	root_node.append_child( "point" ).ensure_attribute( "value" ).set_value( 0.75 * width );
	root_node.append_child( "point" ).ensure_attribute( "value" ).set_value( width * 1.2 );
	root_node.append_child( "point" ).ensure_attribute( "value" ).set_value( width * 1.45 );
	root_node.ensure_attribute( "direction" ).set_value( "rtol" );

	GraphicMultiBoardXMLPlug mb_plug( gmboard );
	mb_plug.loadFrom( root_node );

	const vector <double> & shifts = gmboard.GetShifts();
	CPPUNIT_ASSERT( shifts.size() == 4 );
	CPPUNIT_ASSERT( shifts[0] == 0.5 * width );
	CPPUNIT_ASSERT( shifts[1] == 0.75 * width );
	CPPUNIT_ASSERT( shifts[2] == width );
	CPPUNIT_ASSERT( shifts[3] == width );
	CPPUNIT_ASSERT( gmboard.GetDirection() == RIGHT_TO_LEFT );
	CPPUNIT_ASSERT( gmboard.GetParity() == ANY_PARITY );
	CPPUNIT_ASSERT( !gmboard.BindsHidden() );
}

void TC_GElemXMLPlug::SaveTableBoard()
{
	GraphElemPtr gePtr = FastCreate( TABLE_BOARD );
	CGraphicTableBoard & gtboard = static_cast <CGraphicTableBoard &>( *gePtr );
	gtboard.SetTiling( CGraphicTableBoard::TILING::BOTTOM_TO_TOP );
	GraphicTableBoardXMLPlug tb_plug( gtboard );
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	tb_plug.saveTo( root_node );
	CPPUNIT_ASSERT( root_node.attribute( "tiling" ).as_string() == string( "btot" ) );
}

void TC_GElemXMLPlug::LoadTableBoard()
{
	GraphElemPtr gePtr = FastCreate( TABLE_BOARD );
	CGraphicTableBoard & gtboard = static_cast <CGraphicTableBoard &>( *gePtr );
	double width, height;
	gtboard.GetSizes( width, height );
	gtboard.SetSizes( 100, height );

	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	root_node.ensure_attribute( "tiling" ).set_value( "btot" );
	root_node.append_child( "point" ).ensure_attribute( "value" ).set_value( 0.5 * height );
	root_node.append_child( "point" ).ensure_attribute( "value" ).set_value( 0.75 * height );
	root_node.append_child( "point" ).ensure_attribute( "value" ).set_value( height * 1.2 );
	root_node.append_child( "point" ).ensure_attribute( "value" ).set_value( height * 1.45 );
	root_node.ensure_attribute( "direction" ).set_value( "rtol" );

	GraphicTableBoardXMLPlug tb_plug( gtboard );
	tb_plug.loadFrom( root_node );

	const vector <double> & shifts = gtboard.GetShifts();
	CPPUNIT_ASSERT( shifts.size() == 4 );
	CPPUNIT_ASSERT( shifts[0] == 0.5 * height );
	CPPUNIT_ASSERT( shifts[1] == 0.75 * height );
	CPPUNIT_ASSERT( (shifts[1] < shifts[2]) && (shifts[2] < height) );
	CPPUNIT_ASSERT( (shifts[2] < shifts[3]) && (shifts[3] < height) );
	CPPUNIT_ASSERT( gtboard.GetTiling() == CGraphicTableBoard::TILING::BOTTOM_TO_TOP );
}

void TC_GElemXMLPlug::Save2dElement()
{
	GraphElemPtr gePtr = FastCreate( FIGURE );
	CGraphic2dElement & g2delem = static_cast <CGraphic2dElement &>( *gePtr );
	g2delem.SetSizes( 4.5, 16.9 );
	g2delem.SetAngle( -M_PI / 6 );
	double elangle = g2delem.GetAngle();

	Graphic2dXMLPlug xplug( g2delem );
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	xplug.saveTo( root_node );

	CPPUNIT_ASSERT( root_node.attribute( "dx" ).as_double() == 4.5 );
	CPPUNIT_ASSERT( root_node.attribute( "dy" ).as_double() == 16.9 );
	double node_angle = root_node.attribute( "angle" ).as_double();
	Geo::NormalizeAngle( node_angle );
	CPPUNIT_ASSERT( elangle == node_angle );
}

void TC_GElemXMLPlug::Load2dElement()
{
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	root_node.ensure_attribute( "dx" ).set_value( 4.5 );
	root_node.ensure_attribute( "dy" ).set_value( 16.9 );
	double node_angle = -M_PI / 6;
	Geo::NormalizeAngle( node_angle );
	root_node.ensure_attribute( "angle" ).set_value( node_angle );

	GraphElemPtr gePtr = FastCreate( FIGURE );
	CGraphic2dElement & g2delem = static_cast <CGraphic2dElement &>( *gePtr );
	Graphic2dXMLPlug xplug( g2delem );
	g2delem.loadFrom( root_node );

	double gwidth, gheight;
	g2delem.GetSizes( gwidth, gheight );
	CPPUNIT_ASSERT( gwidth == 4.5 && gheight == 16.9 );
	CPPUNIT_ASSERT( g2delem.GetAngle() == node_angle );
}

void TC_GElemXMLPlug::SaveStrip()
{
	GraphElemPtr gePtr = FastCreate( STRIP );
	CGraphicStrip & gstrip = static_cast <CGraphicStrip &>( *gePtr );
	gstrip.SetJoints( OUTSIZED, SIZED );

	GraphicStripXMLPlug xplug( gstrip );
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	xplug.saveTo( root_node );

	CPPUNIT_ASSERT( root_node.attribute( "joint1" ).as_int() == OUTSIZED );
	CPPUNIT_ASSERT( root_node.attribute( "joint2" ).as_int() == SIZED );
}

void TC_GElemXMLPlug::LoadStrip()
{
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	root_node.ensure_attribute( "joint1" ).set_value( SIZED );
	root_node.ensure_attribute( "joint2" ).set_value( OUTSIZED );

	GraphElemPtr gePtr = FastCreate( STRIP );
	CGraphicStrip & gstrip = static_cast <CGraphicStrip &>( *gePtr );
	GraphicStripXMLPlug xplug( gstrip );
	gstrip.loadFrom( root_node );

	JOINT_TYPE jtype1, jtype2;
	gstrip.GetJoints( jtype1, jtype2 );
	CPPUNIT_ASSERT( jtype1 == SIZED && jtype2 == OUTSIZED );
}

void TC_GElemXMLPlug::SaveGText()
{
	GraphElemPtr gePtr = FastCreate( GRAPHIC_TEXT );
	CGraphicText & gtext = static_cast <CGraphicText &>( *gePtr );
	gtext.SetText( L"some text" );
	ColorDescr cdescr( ColorDescr::SIMPLE, Gdiplus::Color::Red ), 
		cdescr2( ColorDescr::SIMPLE, Gdiplus::Color::Blue ), 
		cdescr3( ColorDescr::SIMPLE, Gdiplus::Color::Green );
	gtext.SetColor( NOT_LIGHT, false, cdescr );
	gtext.SetColor( LIGHT1, true, cdescr2 );
	gtext.SetColor( LIGHT2, false, cdescr3 );
	gtext.SetAutoPlace( true );
	gtext.SetFont( L"arial", Gdiplus::FontStyleUnderline, Gdiplus::REAL( 5.7 ) );

	GraphicTextXMLPlug xplug( gtext );
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	xplug.saveTo( root_node );

	CPPUNIT_ASSERT( root_node.attribute( "text" ).as_string() == string( "some text" ) );
	CPPUNIT_ASSERT( root_node.attribute( "unlight_bkgrcol" ).as_wstring() == ColorGamut::ColdescrToStr( cdescr ) );
	CPPUNIT_ASSERT( root_node.attribute( "light1_txtcol" ).as_wstring() == ColorGamut::ColdescrToStr( cdescr2 ) );
	CPPUNIT_ASSERT( root_node.attribute( "light2_bkgrcol" ).as_wstring() == ColorGamut::ColdescrToStr( cdescr3 ) );
	CPPUNIT_ASSERT( root_node.attribute( "autoplace" ).as_bool() == true );
	CPPUNIT_ASSERT( root_node.attribute( "font" ).as_string() == string( "Arial" ) );
	CPPUNIT_ASSERT( root_node.attribute( "fontstyle" ).as_int() == Gdiplus::FontStyleUnderline );
	CPPUNIT_ASSERT( root_node.attribute( "fontsize" ).as_double() == 5.7 );
}

void TC_GElemXMLPlug::LoadGText()
{
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	ColorDescr cdescr( ColorDescr::SIMPLE, Gdiplus::Color::Red ), 
		cdescr2( ColorDescr::SIMPLE, Gdiplus::Color::Blue ), 
		cdescr3( ColorDescr::SIMPLE, Gdiplus::Color::Green );
	root_node.ensure_attribute( "text" ).set_value( "some_text" );
	root_node.ensure_attribute( "unlight_txtcol" ).set_value( ColorGamut::ColdescrToStr( cdescr ) );
	root_node.ensure_attribute( "light1_txtcol" ).set_value( ColorGamut::ColdescrToStr( cdescr2 ) );
	root_node.ensure_attribute( "light2_bkgrcol" ).set_value( ColorGamut::ColdescrToStr( cdescr3 ) );
	root_node.ensure_attribute( "autoplace" ).set_value( false );
	root_node.ensure_attribute( "font" ).set_value( "times new roman" );
	root_node.ensure_attribute( "fontstyle" ).set_value( Gdiplus::FontStyleItalic );
	Gdiplus::REAL fsize = ( Gdiplus::REAL ) 4.1;
	root_node.ensure_attribute( "fontsize" ).set_value( fsize );

	GraphElemPtr gePtr = FastCreate( GRAPHIC_TEXT );
	CGraphicText & gtext = static_cast <CGraphicText &>( *gePtr );
	GraphicTextXMLPlug xplug( gtext );
	gtext.loadFrom( root_node );

	CPPUNIT_ASSERT( gtext.GetText() == L"some_text" );
	CPPUNIT_ASSERT( gtext.GetColor( NOT_LIGHT, true ) == cdescr );
	CPPUNIT_ASSERT( gtext.GetColor( LIGHT1, true ) == cdescr2 );
	CPPUNIT_ASSERT( gtext.GetColor( LIGHT2, false ) == cdescr3 );
	CPPUNIT_ASSERT( gtext.GetAutoPlace() == false );
	Gdiplus::Font * txtfont = gtext.GetFont();
	Gdiplus::FontFamily fonfam;
	txtfont->GetFamily( &fonfam );
	wchar_t fname[64] = L"";
	fonfam.GetFamilyName( fname );
	CPPUNIT_ASSERT( wstring( fname ) == L"Times New Roman" );
	CPPUNIT_ASSERT( txtfont->GetStyle() == Gdiplus::FontStyleItalic );
	CPPUNIT_ASSERT( txtfont->GetSize() == fsize );
}

void TC_GElemXMLPlug::SaveLandscape()
{
	GraphElemPtr gePtr = FastCreate( LANDSCAPE );
	CGraphicLandscape & gland = static_cast <CGraphicLandscape &>( *gePtr );
	gland.SetXDirection( RIGHT_TO_LEFT );
	gland.SetKmPlacementToVertical( true );
	gland.SetPickHeight( 3.8 );
	CGraphicLandscape::WaysInfo wInfo = gland.GetWaysInfo();
	wInfo.ways_count = 7;
	wInfo.ways_visibility = true;
	wInfo.edge_gap = 10.2;
	wInfo.mid_gap = 5.26;
	wInfo.way_width = 2.92;
	gland.SetWaysInfo( wInfo );
	gland.SetPickDistance( 14 );
	gland.SetScaledPickview( true );

	GraphicLandscapeXMLPlug xplug( gland );
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	xplug.saveTo( root_node );

	PERDIRECTION direction = ( root_node.attribute( "direction" ).as_bool() ? LEFT_TO_RIGHT : RIGHT_TO_LEFT );
	CPPUNIT_ASSERT( direction == RIGHT_TO_LEFT );
	CPPUNIT_ASSERT( root_node.attribute( "kmvertical" ).as_def_bool( false ) == true );
	CPPUNIT_ASSERT( root_node.attribute( "pickheight" ).as_double() == 3.8 );
	CPPUNIT_ASSERT( root_node.attribute( "wayscount" ).as_int( 0 ) == 7 );
	CPPUNIT_ASSERT( root_node.attribute( "waysvisib" ).as_def_bool( false ) == true );
	CPPUNIT_ASSERT( root_node.attribute( "edgegap" ).as_double() == 10.2 );
	CPPUNIT_ASSERT( root_node.attribute( "midgap" ).as_double() == 5.26 );
	CPPUNIT_ASSERT( root_node.attribute( "waywidth" ).as_double() == 2.92 );
	CPPUNIT_ASSERT( root_node.attribute( "pickdist" ).as_uint() == 14 );
	CPPUNIT_ASSERT( root_node.attribute( "scaledpickview" ).as_def_bool( false ) == true );
}

void TC_GElemXMLPlug::LoadLandscape()
{
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	root_node.ensure_attribute( "direction" ).set_value( true );
	root_node.ensure_attribute( "kmvertical" ).set_value( false );
	root_node.ensure_attribute( "pickheight" ).set_value( 3.8 );
	root_node.ensure_attribute( "wayscount" ).set_value( 7 );
	root_node.ensure_attribute( "waysvisib" ).set_value( false );
	root_node.ensure_attribute( "edgegap" ).set_value( 10.2 );
	root_node.ensure_attribute( "midgap" ).set_value( 5.26 );
	root_node.ensure_attribute( "waywidth" ).set_value( 2.92 );
	root_node.ensure_attribute( "pickdist" ).set_value( 88 );
	root_node.ensure_attribute( "scaledpickview" ).set_value( true );

	GraphElemPtr gePtr = FastCreate( LANDSCAPE );
	CGraphicLandscape & gland = static_cast <CGraphicLandscape &>( *gePtr );
	GraphicLandscapeXMLPlug xplug( gland );
	gland.loadFrom( root_node );

	CPPUNIT_ASSERT( gland.GetXDirection() == LEFT_TO_RIGHT );
	CPPUNIT_ASSERT( !gland.IsVerticalKmPlacement() );
	CPPUNIT_ASSERT( gland.GetPickHeight() == 3.8 );
	CPPUNIT_ASSERT( gland.GetPickDistance() == 88 );
	CPPUNIT_ASSERT( gland.IsScaledPickview() );
	const CGraphicLandscape::WaysInfo & wainfo = gland.GetWaysInfo();
	CPPUNIT_ASSERT( wainfo.ways_count == 7 );
	CPPUNIT_ASSERT( !wainfo.ways_visibility );
	CPPUNIT_ASSERT( wainfo.edge_gap == 10.2 );
	CPPUNIT_ASSERT( wainfo.mid_gap == 5.26 );
	CPPUNIT_ASSERT( wainfo.way_width == 2.92 );
}

void TC_GElemXMLPlug::ObsoleteLoad()
{
	attic::a_document tst_doc;
	attic::a_node root_node = tst_doc.set_document_element( "root_node" );
	root_node.ensure_attribute( "evenness" ).set_value( "rtol" ); //поддержка устаревшего атрибута "evenness"

	GraphElemPtr gePtr = FastCreate( MULTI_BOARD );
	CGraphicMultiBoard & gmboard = static_cast <CGraphicMultiBoard &>( *gePtr );
	GraphicMultiBoardXMLPlug mb_plug( gmboard );
	mb_plug.loadFrom( root_node );

	CPPUNIT_ASSERT( gmboard.GetDirection() == RIGHT_TO_LEFT );
}
