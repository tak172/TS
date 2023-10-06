#include "stdafx.h"
#include "TC_DocEditor.h"
#include "../StatBuilder/MultiEditor.h"
#include "GraphicDocumentTest.h"
#include "LogicDocumentTest.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/GraphicElement.h"
#include "ManipulatorTest.h"
#include "../StatBuilder/GraphicSwitch.h"
#include "../StatBuilder/GraphicStrip.h"
#include "../StatBuilder/GViewSkin.h"
#include "../StatBuilder/Measures.h"
#include "../StatBuilder/LinkManager.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_DocEditor );

void TC_DocEditor::PseudoManipulation()
{
	GraphElemPtr gelem = FastCreate( STRIP );
	LogElemPtr lelem( CLogicElementFactory().CreateObject( STRIP ) );
	lelem->SetName( L"strip" );
	Editor <LTraits> * leditor = gdoc->GetEditor();
	leditor->OpenPool( logdoc.get() );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	projdoc->GetLObjectManager()->AddObject( lelem->Clone(), logdoc.get() );
	projdoc->GetGObjectManager()->AddObject( gelem->Clone(), gdoc.get() );
	projdoc->GetEditor()->StoreManipulations();

	set <CGraphicElement *> all_gelems;
	projdoc->GetGObjectManager()->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), all_gelems );
	set <CLogicElement *> all_lelems;
	projdoc->GetLObjectManager()->GetObjects( ALL_CLASSES, logdoc.get(), all_lelems );
	projdoc->GetEditor()->StoreManipulations(); //не должно срабатывать, т.к. реальных изменений не было
	projdoc->GetEditor()->MoveState( false );
	CPPUNIT_ASSERT( projdoc->GetEditor()->CurrentStateIsFirst() );
}

void TC_DocEditor::LegGetting()
{
	projdoc->OpenAllPools();
	BuildSemalegKit( CGridPoint( 10, 10 ), 1 );
	projdoc->GetEditor()->StoreManipulations();
	Select( SEMALEG );
	set <CGraphicElement *> glegs;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	CPPUNIT_ASSERT( glegs.size() == 1 );
	vector <const CGraphicElement *> manobjects;
	projdoc->GetEditor()->GetManipulatedObjects( gdoc.get(), manobjects );
	CPPUNIT_ASSERT( manobjects.size() == 2 );
	projdoc->GetEditor()->StoreManipulations();
}

void TC_DocEditor::TriangleJunctionUndo()
{
	projdoc->OpenAllPools();
	TC_Graphic::BuildSwitchKit( CGridPoint( 13, 12 ), CGridSize( 2, 0 ), CGridSize( -4, 0 ), 
		CGridSize( -2, -2 ) );
	gdoc->GetEditor()->StoreManipulations();
	set <const CGraphicElement *> all_gelems;
	gdoc->GetGObjectManager()->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), all_gelems );
	GraphElemPtr gelem = FastCreate( STRIP );
	gelem->SetHotPoint( CGridPoint( 12, 10 ) );
	gelem->SetSizes( 0, 0 );
	gdoc->GetGObjectManager()->AddObject( gelem->Clone(), gdoc.get(), SELECTED );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->StartMouseManipulation( Manipulator::EDITING, CGridPoint( 12, 10 ) );
	manPtr->MouseManipulating( CGridPoint( 10, 12 ) );
	manPtr.reset();
	gdoc->GetGObjectManager()->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), all_gelems );
	gdoc->GetEditor()->MoveState( false );
	set <const CGraphicElement *> all_gswitches;
	gdoc->GetGObjectManager()->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), all_gswitches );
	CPPUNIT_ASSERT( all_gswitches.size() == 1 );
}

void TC_DocEditor::SwitchedStripLogicSetUndo()
{
	projdoc->OpenAllPools();
	TC_Graphic::BuildSwitchKit( CGridPoint( 10, 10 ), CGridSize( 2, 0 ), CGridSize( -4, 0 ), 
		CGridSize( -2, -2 ) );
	gdoc->GetEditor()->StoreManipulations();

	set <CGraphicElement *> nconst_strips;
	gdoc->GetGObjectManager()->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), nconst_strips );
	CPPUNIT_ASSERT( !nconst_strips.empty() );
	CGraphicElement * some_strip = *nconst_strips.begin();
	some_strip->SetLogicLink( CLink( STRIP, BadgeE( L"strip_name", EsrKit( 1, 2 ) ) ) );
	gdoc->GetEditor()->StoreManipulations();
	gdoc->GetEditor()->MoveState( false );

	set <const CGraphicElement *> const_strips;
	gdoc->GetGObjectManager()->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), const_strips );
	for( const CGraphicElement * ge : const_strips )
		CPPUNIT_ASSERT( ge->GetLogicLink().obj_badge.empty() );
	set <const CGraphicElement *> all_switches;
	gdoc->GetGObjectManager()->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), all_switches );
	CPPUNIT_ASSERT( all_switches.size() == 1 );
	const CGraphicElement * gswitch = *all_switches.begin();
	CPPUNIT_ASSERT( gswitch->GetLinks().size() == 3 );
}

void TC_DocEditor::UndoRemakingNamedSwitch()
{
	gdoc->GetSkin()->GetMeasures()->SetSwitchRadius( 0.1 );
	projdoc->OpenAllPools();
	//стрелочный комплект
	const CGraphicElement * const_gelem = TC_Graphic::BuildSwitchKit( CGridPoint( 10, 10 ), CGridSize( 1, 0 ), 
		CGridSize( -1, 0 ), CGridSize( -1, -1 ) );
	gdoc->GetEditor()->StoreManipulations();

	CGraphicElement * gelem = pgom->Release( const_gelem, gdoc.get() );
	gelem->SetName( L"1" );

	gdoc->GetEditor()->StoreManipulations();
	//участок ниже стрелочного комплекта на 1 клетку
	GraphElemPtr stripPtr = FastCreate( STRIP );
	stripPtr->SetHotPoint( CGridPoint( 9, 11 ) );
	stripPtr->SetSizes( 2, 0 );
	pgom->AddObject( stripPtr->Clone(), gdoc.get() );
	gdoc->GetEditor()->StoreManipulations();

	//выделяем базовый участок
	const CGraphicSwitch * gswitch = static_cast <CGraphicSwitch *>( gelem );
	CGraphicStrip * base_strip = gswitch->GetSpecialStrip( CLink::BASE );
	pgom->SetStatus( base_strip, SELECTED, gdoc.get() );

	//относим его конец к участку под стрелочным комплектом
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->StartMouseManipulation( Manipulator::EDITING, CGridPoint( 10, 10 ) );
	manPtr->MouseManipulating( CGridPoint( 10, 11 ) );
	manPtr.reset();

	//откат
	gdoc->GetEditor()->MoveState( false );

	//просмотр области участка, где в течение отката должна была уничтожиться стрелка
	set <const CGraphicElement *> all_gelems;
	gdoc->GetGObjectManager()->GetObjects( CGridRect( CGridPoint( 9, 10.5 ), CGridPoint( 11, 11 ) ), gdoc.get(), 
		all_gelems );
	CPPUNIT_ASSERT( all_gelems.size() == 1 );
}

void TC_DocEditor::RedoUndoRemovingLObjectWithGLink()
{
	projdoc->OpenAllPools();
	//заводим и связываем логический и графический участки (состояние 1)
	const CLogicElement * lstrip = TC_Logic::BuildObject( STRIP, true, logdoc.get() );
	GraphElemPtr gstrip = TC_Graphic::FastCreate( STRIP );
	const auto & initLink = lstrip->BuildLink( EsrCode() );
	gstrip->SetLogicLink( initLink );
	pgom->AddObject( gstrip->Clone(), gdoc.get() );
	projdoc->GetEditor()->StoreManipulations();

	//удаляем логический участок (состояние 2)
	plom->RemoveObject( lstrip );
	projdoc->GetEditor()->StoreManipulations();

	//графический участок должен потерять привязку
	set <const CGraphicElement *> gstrips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), gstrips );
	CPPUNIT_ASSERT( gstrips.size() == 1 );
	auto llinkNow = ( *gstrips.cbegin() )->GetLogicLink();
	CPPUNIT_ASSERT( llinkNow.obj_badge.empty() );

	//возвращаемся в состояние 1
	projdoc->GetEditor()->MoveState( false );

	//графический участок должен получить привязку
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), gstrips );
	CPPUNIT_ASSERT( gstrips.size() == 1 );
	auto llinkBefore = ( *gstrips.cbegin() )->GetLogicLink();
	CPPUNIT_ASSERT( llinkBefore == initLink );

	//переход в состояние 2
	projdoc->GetEditor()->MoveState( true );

	//графический участок должен иметь привязку
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), gstrips );
	CPPUNIT_ASSERT( gstrips.size() == 1 );
	auto llinkNow2 = ( *gstrips.cbegin() )->GetLogicLink();
	CPPUNIT_ASSERT( llinkNow2.obj_badge.empty() );
}

void TC_DocEditor::UndoRouteDoubleGenerating()
{
	projdoc->OpenAllPools();
	CLogicElement * lroute = CreateTestRoute();
	projdoc->GetEditor()->StoreManipulations();

	//перегенерация маршрутов
	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	for ( auto lroute : lroutes )
		plom->RemoveObject( lroute );
	plom->GenerateRoutes( logdoc.get() );
	projdoc->GetEditor()->StoreManipulations();

	//откат
	projdoc->GetEditor()->MoveState( false );
	set <const CLogicElement *> lelems;
	plom->GetObjects( ALL_CLASSES, logdoc.get(), lelems );

	//откат приводит к состоянию до создания маршрута
	CPPUNIT_ASSERT( lelems.empty() );
}

void TC_DocEditor::ResetRemovingLObject()
{
	projdoc->OpenAllPools();
	CLogicElement * lstrip = BuildObject( STRIP );
	CLogicElement * lclone = lstrip->Clone();
	plom->AddObject( lstrip, logdoc.get() );
	projdoc->GetEditor()->StoreManipulations();
	set <const CLogicElement *> lelems;
	plom->GetObjects( ALL_CLASSES, logdoc.get(), lelems );
	CPPUNIT_ASSERT( lelems.size() == 1 );
	CPPUNIT_ASSERT( *lelems.cbegin() == lstrip );

	plom->RemoveObject( lstrip );
	projdoc->GetEditor()->ResetManipulations();

	plom->GetObjects( ALL_CLASSES, logdoc.get(), lelems );
	CPPUNIT_ASSERT( lelems.size() == 1 );
	CPPUNIT_ASSERT( ( *lelems.cbegin() )->Equal( lclone ) );
	delete lclone;
}

void TC_DocEditor::BlankLogicAction()
{
	projdoc->OpenAllPools();
	CLogicElement * lstrip = BuildObject( STRIP );
	CLogicElement * lclone = lstrip->Clone();
	plom->AddObject( lstrip, logdoc.get() );
	projdoc->GetEditor()->StoreManipulations();

	set <const CLogicElement *> lelems;
	plom->GetObjects( ALL_CLASSES, logdoc.get(), lelems );
	CPPUNIT_ASSERT( lelems.size() == 1 );
	CPPUNIT_ASSERT( *lelems.cbegin() == lstrip );

	plom->RemoveObject( lstrip );
	plom->AddObject( lclone, logdoc.get() );
	projdoc->GetEditor()->StoreManipulations(); //действие не несет никаких изменений и не должно попадать в историю

	plom->GetObjects( ALL_CLASSES, logdoc.get(), lelems );
	CPPUNIT_ASSERT( lelems.size() == 1 );
	CPPUNIT_ASSERT( ( *lelems.cbegin() )->Equal( lclone ) );
}

void TC_DocEditor::ZChanging()
{
	GraphElemPtr gfigure = TC_Graphic::FastCreate( FIGURE );
	gfigure->SetZLevel( 10 );
	pgom->AddObject( gfigure->Clone(), gdoc.get() );
	pgom->AddObject( gfigure->Clone(), gdoc.get() );
	projdoc->OpenAllPools();

	set <CGraphicElement *> gfigures;
	pgom->GetObjects( FIGURE, ALL_STATUSES, gdoc.get(), gfigures );
	CPPUNIT_ASSERT( gfigures.size() == 2 );
	auto gfig1 = *gfigures.cbegin();
	auto gfig2 = *gfigures.crbegin();
	auto topfigure = gfig1->GetZLevel() > gfig2->GetZLevel() ? gfig1 : gfig2;
	set <CGraphicElement *> manobjects;
	manobjects.insert( topfigure );
	pgom->SendToBack( manobjects, gdoc.get() );
	projdoc->GetEditor()->StoreManipulations();

	CPPUNIT_ASSERT( !projdoc->GetEditor()->CurrentStateIsFirst() );
}