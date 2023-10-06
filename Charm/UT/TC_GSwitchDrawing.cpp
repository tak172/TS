#include "stdafx.h"
#include "TC_GSwitchDrawing.h"
#include "../StatBuilder/GraphicContext.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GraphicView.h"
#include "../Actor/LOSSwitch.h"
#include "../StatBuilder/GraphicElement.h"
#include "GraphicViewTest.h"
#include "../StatBuilder/DrawElemInfo.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GSwitchDrawing );

void TC_GSwitchDrawing::PlusMarkDraw()
{
	CGraphicElement * gswitch = BuildSwitchKit( CGridPoint( 10, 10 ), CGridSize( -1, 0 ), CGridSize( 1, 0 ), CGridSize( 1, -1 ) );
	const CGridPoint & hp = gswitch->GetHotPoint();
	TestContext * ptc = gdoc->GetViewTest()->GetContextTest();
	LOS_base * lb = new LOS_switch();
	auto gviewSkin = gdoc->GetSkin();
	gswitch->DrawElement( ptc, DrawElemInfo( LosBasePtr( lb ), false, UNSELECTED, false, gviewSkin.get(), nullptr ) );
	const list <TestContext::Line> & painted_lines = ptc->GetPaintedLines();
	CPPUNIT_ASSERT( painted_lines.size() == 1 );
	const TestContext::Line & line = painted_lines.front();
	CPPUNIT_ASSERT( line.first.X > hp.x && line.second.X > hp.x ); //правее точки стрелки
	CPPUNIT_ASSERT( line.first.Y < hp.y && line.second.Y < hp.y ); //выше точки стрелки
	CPPUNIT_ASSERT( line.first.Y == line.second.Y && line.first.X < line.second.X ); //ориентирован вправо
    delete ptc;
}

void TC_GSwitchDrawing::EdgeCase()
{
	CGraphicElement * gswitch = BuildSwitchKit( CGridPoint( 1, 4 ), CGridSize( 0, -3 ), CGridSize( 0, 3 ), CGridSize( -3, -3 ) );
	const CGridPoint & hp = gswitch->GetHotPoint();
	TestContext * ptc = gdoc->GetViewTest()->GetContextTest();
	LOS_base * lb = new LOS_switch();
	auto gviewSkin = gdoc->GetSkin();
	gswitch->DrawElement( ptc, DrawElemInfo( LosBasePtr( lb ), false, UNSELECTED, false, gviewSkin.get(), nullptr ) );
	const list <TestContext::Line> & painted_lines = ptc->GetPaintedLines();
	CPPUNIT_ASSERT( painted_lines.size() == 1 );
	const TestContext::Line & line = painted_lines.front();
	CPPUNIT_ASSERT( line.first.X < hp.x && line.second.X < hp.x ); //левее точки стрелки
	CPPUNIT_ASSERT( line.first.Y > hp.y && line.second.Y > hp.y ); //ниже точки стрелки
	CPPUNIT_ASSERT( line.first.X == line.second.X && line.first.Y < line.second.Y ); //ориентирован вниз
    delete ptc;
}