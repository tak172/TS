#include "stdafx.h"
#include "TC_Manipulator.h"
#include "../StatBuilder/Geo.h"
#include "ManipulatorTest.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/MouseHandler.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/MultiEditor.h"
#include "GraphicViewTest.h"
#include "../StatBuilder/GViewSkin.h"
#include "../StatBuilder/GraphicGrid.h"
#include "../StatBuilder/GraphicSwitch.h"
#include "../StatBuilder/GraphicStrip.h"
#include "../StatBuilder/GraphicText.h"
#include "../StatBuilder/Measures.h"
#include "../helpful/Informator.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Manipulator );

void TC_Manipulator::MoveStateWhileManipulating()
{
	GraphElemPtr gfigure = FastCreate( FIGURE );
	gfigure->SetHotPoint( CGridPoint( 10, 10 ) );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	pgom->AddObject( gfigure->Clone(), gdoc.get(), SELECTED );
	MultiEditor * deditor = gdoc->GetEditor();
	deditor->StoreManipulations();
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->StartMouseManipulation( Manipulator::MOVING, CGridPoint( 10.5, 10.5 ) );
	bool moved = deditor->MoveState( false );
	CPPUNIT_ASSERT( !moved );
}

void TC_Manipulator::EvenlenStripRotating()
{
	GraphElemPtr gstrip = FastCreate( STRIP );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	for ( unsigned int k = 2; k < 20; ++k )
	{
		gstrip->SetHotPoint( CGridPoint( k, 1 ) );
		for ( double length = 4; length < 20; length += 2 )
		{
			gstrip->SetSizes( 0, length );
			CGraphicElement * strip_clone = gstrip->Clone();
			const auto & init_hp = strip_clone->GetHotPoint();
			pgom->AddObject( strip_clone, gdoc.get(), SELECTED );

			ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
			manPtr->KeyboardManipulating( 'R' );

			set <const CGraphicElement *> allstrips;
			pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
			CPPUNIT_ASSERT( allstrips.size() == 1 );
			const CGraphicElement * rotated_strip = *allstrips.begin();
			const auto & cur_hp = rotated_strip->GetHotPoint();
			const auto & tst_point = CGridPoint( k - length / 2, 1 );
			string msg = "init hp=(" + to_string( init_hp.x ) + ", " + to_string( init_hp.y ) + ")\n";
			msg += "now hp=(" + to_string( cur_hp.x ) + ", " + to_string( cur_hp.y ) + ")\n";
			msg += "tst_point=(" + to_string( tst_point.x ) + ", " + to_string( tst_point.y ) + ")\n";
			CPPUNIT_ASSERT_MESSAGE( msg, cur_hp == tst_point );
			double width, height;
			rotated_strip->GetSizes( width, height );
			CPPUNIT_ASSERT( abs( width ) == length && abs( height ) == length );

			manPtr->KeyboardManipulating( 'R' );
			CPPUNIT_ASSERT( rotated_strip->GetHotPoint() == CGridPoint( k - length / 2, 1 + length / 2 ) );
			rotated_strip->GetSizes( width, height );
			CPPUNIT_ASSERT( abs( width ) == length && height == 0 );			
			pgom->RemoveObject( strip_clone, gdoc.get() );
		}
	}
}

void TC_Manipulator::OddlenStripRotating()
{
	GraphElemPtr gstrip = FastCreate( STRIP );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	gstrip->SetHotPoint( CGridPoint( 2, 1 ) );
	gstrip->SetSizes( 0, 3 );
	CGraphicElement * strip_clone = gstrip->Clone();
	pgom->AddObject( strip_clone, gdoc.get(), SELECTED );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	for( unsigned int k = 0; k < 8; ++k )
	{
		manPtr->KeyboardManipulating( 'R' );
		set <const CGraphicElement *> allstrips;
		pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
		CPPUNIT_ASSERT( allstrips.size() == 1 );
		const CGraphicElement * rotated_strip = *allstrips.begin();
		//хот-поинт не принципиален, т.к. участок в любом случае будет смещен
		double width, height;
		rotated_strip->GetSizes( width, height );
		CPPUNIT_ASSERT( abs( width ) == 3 || abs( height ) == 3 );
		if ( width == 0 )
			CPPUNIT_ASSERT( abs( height ) == 3 );
		if ( height == 0 )
			CPPUNIT_ASSERT( abs( width ) == 3 );
	}
	//участок должен смещаться вниз и вправо
	CPPUNIT_ASSERT( strip_clone->GetHotPoint().x >= GRID_COORD( 2 ) && 
		strip_clone->GetHotPoint().y >= GRID_COORD( 1 ) );
	pgom->RemoveObject( strip_clone, gdoc.get() );
}

void TC_Manipulator::StripNFigureRotating()
{
	GraphElemPtr gstrip = FastCreate( STRIP );
	GraphElemPtr gfigure = FastCreate( FIGURE );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );

	gstrip->SetHotPoint( CGridPoint( 1, 1 ) );
	gstrip->SetSizes( 2, 0 );
	gfigure->SetHotPoint( CGridPoint( 2, 3 ) );
	CGraphicElement * strip_clone = gstrip->Clone();
	CGraphicElement * figure_clone = gfigure->Clone();
	pgom->AddObject( strip_clone, gdoc.get(), SELECTED );
	pgom->AddObject( figure_clone, gdoc.get(), SELECTED );

	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->KeyboardManipulating( 'R' );
	manPtr.reset();

	set <const CGraphicElement *> allstrips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	CPPUNIT_ASSERT( allstrips.size() == 1 );
	const CGraphicElement * rotated_strip = *allstrips.begin();
	double width, height;
	rotated_strip->GetSizes( width, height );
	CPPUNIT_ASSERT( width == 0 && abs( height ) == 2 );

	set <CGraphicElement *> allobjects;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), allobjects );
	for ( auto gobject : allobjects )
		pgom->RemoveObject( gobject, gdoc.get() );
}

void TC_Manipulator::NullLengthStripCreating()
{
	GraphElemPtr gstrip = FastCreate( STRIP );
	gstrip->SetHotPoint( CGridPoint( 1, 1 ) );
	gstrip->SetSizes( 0, 0 );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	list <CGraphicElement *> new_elems;
	new_elems.push_back( gstrip->Clone() );
	pgom->AddGhostObjects( new_elems, gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );

	manPtr->StartMouseManipulation( Manipulator::EDITING, CGridPoint( 1, 1 ) );
	manPtr.reset();

	set <const CGraphicElement *> allstrips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	CPPUNIT_ASSERT( allstrips.empty() );
}

void TC_Manipulator::NameMoving()
{
	GraphElemPtr gstrip = FastCreate( STRIP );
	gstrip->SetHotPoint( CGridPoint( 10, 10 ) );
	gstrip->SetSizes( 10, 0 );
	gstrip->SetName( L"strip_name" );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	pgom->AddObject( gstrip->Clone(), gdoc.get(), SELECTED );
	gdoc->GetEditor()->StoreManipulations();

	//определяем точку тычка в имя объекта
	ScopedGraphics sgraphics( gdoc->GetView() );
	GraphicContext * gcont = const_cast <GraphicContext *>( sgraphics.Get() );
	CGridRect name_rect;
	gstrip->GetNameRect( name_rect, gcont );
	CGridPoint hit_point = gstrip->GetCentralPoint();
	hit_point.y -= 0.5;
	HitPointInfo hpInfo;
	gdoc->GetViewTest()->HitMousePoint( hit_point, hpInfo );

	//смещаем имя
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->StartMouseManipulation( Manipulator::MOVING, hit_point, hpInfo );
	manPtr->MouseManipulating( CGridPoint( hit_point.x, hit_point.y + 10 ) );
	manPtr.reset();

	set <const CGraphicElement *> allstrips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	const CGraphicElement * modified_strip = *allstrips.begin();
	CPPUNIT_ASSERT( gstrip->GetHotPoint() == modified_strip->GetHotPoint() );
	double prev_width, prev_height;
	gstrip->GetSizes( prev_width, prev_height );
	double new_width, new_height;
	modified_strip->GetSizes( new_width, new_height );
	CPPUNIT_ASSERT( prev_width == new_width && prev_height == new_height );
}

void TC_Manipulator::LinksOrder()
{
	//подготовка требуемых объектов
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	const CGraphicElement * gleg = BuildSemalegKit( CGridPoint( 10, 10 ), 2 );
	gdoc->GetEditor()->StoreManipulations();
	const vector <GLinkInfo> & leglinks_before = gleg->GetLinks( HEAD );
	CPPUNIT_ASSERT( leglinks_before.size() == 2 );
	vector <CGridPoint> head_hpts_before;
	head_hpts_before.push_back( leglinks_before[0].glink_ptr->GetHotPoint() );
	head_hpts_before.push_back( leglinks_before[1].glink_ptr->GetHotPoint() );

	//смещение светофора
	pgom->SelectAll( gdoc.get() );
	CGridPoint hit_point = gleg->GetCentralPoint();
	HitPointInfo hpInfo;
	gdoc->GetViewTest()->HitMousePoint( hit_point, hpInfo );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->StartMouseManipulation( Manipulator::MOVING, gleg->GetHotPoint(), hpInfo );
	manPtr->MouseManipulating( CGridPoint( hit_point.x, hit_point.y + 10 ) );
	gdoc->GetEditor()->StoreManipulations();
	manPtr.reset(); //отпустить захваченный мышью мьютекс

	set <const CGraphicElement *> all_objects;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), all_objects );

	//откат
	gdoc->GetEditor()->MoveState( false );

	//проверка порядка следования линков
	set <const CGraphicElement *> legset;
	pgom->GetObjects( SEMALEG, ALL_STATUSES, gdoc.get(), legset );
	CPPUNIT_ASSERT( legset.size() == 1 );
	gleg = *legset.begin();
	vector <GLinkInfo> leglinks_after = gleg->GetLinks( HEAD );
	CPPUNIT_ASSERT( leglinks_after.size() == 2 );
	vector <CGridPoint> head_hpts_after;
	head_hpts_after.push_back( leglinks_after[0].glink_ptr->GetHotPoint() );
	head_hpts_after.push_back( leglinks_after[1].glink_ptr->GetHotPoint() );

	CPPUNIT_ASSERT( head_hpts_before == head_hpts_after );
};

void TC_Manipulator::ScalingAndRemove()
{
	//подготовка требуемых объектов
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	CGraphicElement * gleg = BuildSemalegKit( CGridPoint( 10, 10 ), 1 );
	gdoc->GetEditor()->StoreManipulations();

	//изменение масштаба
	CGraphicGrid * grid = gdoc->GetSkin()->GetGrid();
	grid->Decrease();

	//удаление
	pgom->SelectAll( gdoc.get() );
	ManipulatorPtr manptr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manptr->DeleteSelectedObjects();
	
	CGridRect look_rect( CGridPoint( 9, 9 ), CGridPoint( 11, 11 ) );
	set <const CGraphicElement *> left_elems;
	pgom->GetObjects( look_rect, gdoc.get(), left_elems );
	CPPUNIT_ASSERT( left_elems.empty() );
}

void TC_Manipulator::SmallSwitchRegenarating()
{
	//Воспроизводится ситуация, при которой радиус стрелки меньше толщины участков,
	//что при регенерации стрелок может приводить к тому, что могут быть возвращены
	//три участка, образующие стрелку, но не сама стрелка. Это приводит к созданию 
	//дополнительной паразитной стрелки на месте исходной.

	//подготовка требуемых объектов:
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	gdoc->GetSkin()->GetMeasures()->SetSwitchRadius( 0.01 );
	TestContext::SetSymHeight( ( Gdiplus::REAL ) 0.55 );
	CGraphicElement * gswitch1 = BuildSwitchKit( CGridPoint( 1, 10 ), CGridSize( 1, 0 ), CGridSize( -1, 0 ), CGridSize( -1, 1 ) );
	CGraphicElement * far_switch = BuildSwitchKit( CGridPoint( 3, 9 ), CGridSize( 1, 0 ), CGridSize( -1, 0 ), CGridSize( -1, 1 ) );
	GraphElemPtr gswitch_prototype = FastCreate( SWITCH );
	CGraphicElement * gswitch2 = gswitch_prototype->Clone();
	pgom->AddObject( gswitch2, gdoc.get() );
	gswitch2->SetHotPoint( CGridPoint( 2, 10 ) );
	GraphElemPtr gstrip_prototype = FastCreate( STRIP );
	CGraphicElement * minus_strip2 = gstrip_prototype->Clone();
	pgom->AddObject( minus_strip2, gdoc.get() );
	minus_strip2->SetHotPoint( CGridPoint( 2, 10 ) );
	minus_strip2->SetSizes( -1, 1 );
	CGraphicSwitch * gswitch = static_cast <CGraphicSwitch *>( gswitch1 );
	CGraphicElement * plus_strip2 = gswitch->GetSpecialStrip( CLink::BASE ); //опорный стрип для 1-й стрелки, минусовой для 2-й
	plus_strip2->SetName( L"Strip" );
	gswitch = static_cast <CGraphicSwitch *>( far_switch );
	CGraphicElement * base_strip2 = gswitch->GetSpecialStrip( CLink::MINUS ); //минусовой стрип для удаленной стрелки, опорный для 2-й
	gswitch2->Join( base_strip2, CLink::BASE );
	gswitch2->Join( minus_strip2, CLink::MINUS );
	gswitch2->Join( plus_strip2, CLink::PLUS );
	gdoc->GetEditor()->StoreManipulations();

	//манипуляции с центральным участком не должны приводить к перегенерации удаленной стрелки
	pgom->SetStatus( plus_strip2, SELECTED, gdoc.get() );
	ManipulatorPtr manptr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manptr->StartMouseManipulation( Manipulator::MOVING, CGridPoint( 1.5, 10 ) ); //тычок в середину участка
	manptr.reset(); //отпустить захваченный мышью мьютекс

	//проверка
	const CGraphicElement * tst_swtich = 0;
	set <const CGraphicElement *> all_switches;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), all_switches );
	CPPUNIT_ASSERT( all_switches.size() == 3 );
	TestContext::SetSymHeight( ( Gdiplus::REAL ) 1 );
}

void TC_Manipulator::RoughMoving()
{
	//подготовка объектов
	GraphElemPtr figPtr = FastCreate( FIGURE );
	CGridPoint hp1( 13.8, 27.4 ), hp2( 57.2, 71.9 );
	CGridSize size1( 5.6, 14.2 ), size2( 6.6, 9.4 );
	figPtr->SetHotPoint( hp1 );
	figPtr->SetSizes( size1.w, size1.h );
	pgom->AddObject( figPtr->Clone(), gdoc.get(), SELECTED );
	figPtr->SetHotPoint( hp2 );
	figPtr->SetSizes( size2.w, size2.h );
	pgom->AddObject( figPtr->Clone(), gdoc.get(), SELECTED );

	//смещение
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manptr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	ManipulatorTest * mantest = static_cast <ManipulatorTest *>( manptr.get() );
	mantest->SetSysKeys( Manipulator::SYSKEYS::CTRL );
	manptr->KeyboardManipulating( VK_RIGHT );
	manptr.reset();

	//проверка
	set <const CGraphicElement *> figures;
	pgom->GetObjects( FIGURE, ALL_STATUSES, gdoc.get(), figures );
	CPPUNIT_ASSERT( figures.size() == 2 );
	for ( auto figptr : figures )
	{
		double w, h;
		figptr->GetSizes( w, h );
		if( figptr->GetHotPoint() == CGridPoint( hp1.x + 1, hp1.y ) )
			CPPUNIT_ASSERT( w == size1.w && h == size1.h );
		else
		{
			CPPUNIT_ASSERT( figptr->GetHotPoint() == CGridPoint( hp2.x + 1, hp2.y ) );
			CPPUNIT_ASSERT( w == size2.w && h == size2.h );
		}
	}
}

void TC_Manipulator::MoveFixedAndUnfixed()
{
	//подготовка объектов
	GraphElemPtr figPtr = FastCreate( FIGURE );
	const auto & hp1 = CGridPoint( 0, 0 );
	figPtr->SetHotPoint( hp1 );
	figPtr->SetSizes( 5, 5 );
	CGraphicElement * gfigure1 = figPtr->Clone();
	pgom->AddObject( gfigure1, gdoc.get(), SELECTED );
	const auto & hp2 = CGridPoint( 10, 10 );
	figPtr->SetHotPoint( hp2 );
	CGraphicElement * gfigure2 = figPtr->Clone();
	pgom->AddObject( gfigure2, gdoc.get(), SELECTED );
	pgom->SetFixed( gfigure1, gdoc.get(), true );

	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );

	//смещение зафиксированного
	pgom->SetStatus( gfigure1, SELECTED, gdoc.get() );
	ManipulatorPtr manptr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	ManipulatorTest * mantest = static_cast <ManipulatorTest *>( manptr.get() );
	manptr->KeyboardManipulating( VK_RIGHT );
	manptr.reset();
	CPPUNIT_ASSERT( gfigure1->GetHotPoint() == hp1 ); //смещение запрещено

	//смещение обоих объектов
	pgom->SetStatus( gfigure2, SELECTED, gdoc.get() );
	manptr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	mantest = static_cast <ManipulatorTest *>( manptr.get() );
	manptr->KeyboardManipulating( VK_RIGHT );
	manptr.reset();
	//смещение запрещено
	CPPUNIT_ASSERT( gfigure1->GetHotPoint() == hp1 );
	CPPUNIT_ASSERT( gfigure2->GetHotPoint() == hp2 );

	//смещение не зафиксированного
	pgom->SetStatus( gfigure1, UNSELECTED, gdoc.get() );
	manptr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	mantest = static_cast <ManipulatorTest *>( manptr.get() );
	manptr->KeyboardManipulating( VK_RIGHT );
	manptr.reset();
	//смещение не зафиксированного разрешено
	CPPUNIT_ASSERT( gfigure2->GetHotPoint().x > hp2.x && gfigure2->GetHotPoint().y == hp2.y );
}

void TC_Manipulator::EmptyPoolAfterHit()
{
	MultiEditor * mulEditor = gdoc->GetEditor();
	Editor <GTraits> * geditor = mulEditor;
	geditor->OpenPool( gdoc.get() );

	//создание двух объектов
	GraphElemPtr figPtr = FastCreate( FIGURE );
	const auto & hp1 = CGridPoint( 0, 0 );
	figPtr->SetHotPoint( hp1 );
	figPtr->SetSizes( 5, 5 );
	CGraphicElement * gfigure1 = figPtr->Clone();
	pgom->AddObject( gfigure1, gdoc.get(), SELECTED );
	mulEditor->StoreManipulations();

	vector <const CGraphicElement *> manipObjects;
	mulEditor->GetManipulatedObjects( gdoc.get(), manipObjects );
	CPPUNIT_ASSERT( manipObjects.empty() );

	vector <const CGraphicElement *> hitted;
	pgom->HitPoint( CGridPoint( 2, 2 ), gdoc.get(), hitted );
	mulEditor->GetManipulatedObjects( gdoc.get(), manipObjects );
	CPPUNIT_ASSERT( manipObjects.empty() );
}

void TC_Manipulator::ClickOnSwitchedStrip()
{
	MultiEditor * mulEditor = gdoc->GetEditor();
	Editor <GTraits> * geditor = mulEditor;
	geditor->OpenPool( gdoc.get() );
	CGraphicElement * gswitch = TC_Graphic::BuildSwitchKit( CGridPoint( 10, 10 ) );
	mulEditor->StoreManipulations();

	set <const CGraphicElement *> allObjects;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), allObjects );
	pgom->SetStatus( allObjects, SELECTED, gdoc.get() );

	ManipulatorPtr manptr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	ManipulatorTest * mantest = static_cast <ManipulatorTest *>( manptr.get() );
	manptr->StartMouseManipulation( Manipulator::MOVING, CGridPoint( 11, 10 ) );
	manptr.reset();

	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), allObjects );
	CPPUNIT_ASSERT( allObjects.size() == 4 );
	mulEditor->MoveState( false );
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), allObjects );
	CPPUNIT_ASSERT( allObjects.empty() );
	mulEditor->MoveState( true );
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), allObjects );
	CPPUNIT_ASSERT( allObjects.size() == 4 );
}

void TC_Manipulator::ZTestAfterGhostSemaforAdding()
{
	MultiEditor * mulEditor = gdoc->GetEditor();
	Editor <GTraits> * geditor = mulEditor;
	geditor->OpenPool( gdoc.get() );
	GraphElemPtr gleg_prototype = FastCreate( SEMALEG );
	CGraphicElement * gleg = gleg_prototype->Clone();
	GraphElemPtr ghead_prototype = FastCreate( HEAD );
	CGraphicElement * ghead = ghead_prototype->Clone();
	gleg->Join( ghead );
	gleg->SetHotPoint( CGridPoint( 10, 10 ) );
	list <CGraphicElement *> ghostObjects;
	ghostObjects.push_back( ghead );
	ghostObjects.push_back( gleg );
	pgom->AddGhostObjects( ghostObjects, gdoc.get() );

	set <const CGraphicElement *> glegs, gheads;
	pgom->GetObjects<const CGraphicElement *>( SEMALEG, ALL_STATUSES, gdoc.get(), glegs );
	pgom->GetObjects<const CGraphicElement *>( HEAD, ALL_STATUSES, gdoc.get(), gheads );
	CPPUNIT_ASSERT( glegs.size() == 1 );
	CPPUNIT_ASSERT( gheads.size() == 1 );
	CPPUNIT_ASSERT_EQUAL( 11, ( *glegs.begin() )->GetZLevel() );
	CPPUNIT_ASSERT_EQUAL( 11, ( *gheads.begin() )->GetZLevel() );
}

void TC_Manipulator::TextMerging()
{
	//готовим тесктовый элемент
	MultiEditor * mulEditor = gdoc->GetEditor();
	Editor <GTraits> * geditor = mulEditor;
	geditor->OpenPool( gdoc.get() );
	GraphElemPtr gelem = FastCreate( GRAPHIC_TEXT );
	CGraphicText * gtext = static_cast<CGraphicText *>( gelem.get() );
	gtext->SetText( L"merged name" );
	CGridPoint txtHP( 10, 10 );
	gtext->SetHotPoint( txtHP );
	gtext->SetSizes( 5, 5 );
	auto gtextClone = gtext->Clone(); 
	pgom->AddObject( gtextClone, gdoc.get() );
	mulEditor->StoreManipulations();
	pgom->SetStatus( gtextClone, SELECTED, gdoc.get() );

	//объединяем его с объектам всех возможных классов
	CInformator * pI = CInformator::Instance();
	list <ELEMENT_CLASS> graphicClasses;
	pI->GetGraphicClasses( graphicClasses );
	for ( ELEMENT_CLASS elclass : graphicClasses )
	{
		//обновляем исходный текстовый объект для слияния
		set <const CGraphicElement *> gtexts;
		pgom->GetObjects( GRAPHIC_TEXT, ALL_STATUSES, gdoc.get(), gtexts );
		CPPUNIT_ASSERT( gtexts.size() == 1 );
		auto gtxtObject = *gtexts.cbegin();
		pgom->SetStatus( gtxtObject, SELECTED, gdoc.get() );

		//готовим тестовый объект, в который будет помещаться текст
		GraphElemPtr gelem = FastCreate( elclass );
		const auto & tstHotPoint = CGridPoint( 50, 50 );
		gelem->SetHotPoint( tstHotPoint );
		CGridSize gsize( 10, 10 );
		gelem->SetSizes( gsize.w, gsize.w );
		auto gclone = gelem->Clone();
		pgom->AddObject( gclone, gdoc.get() );
		if ( elclass == HEAD || elclass == SEMALEG )
		{
			GraphElemPtr neigbour = FastCreate( elclass == HEAD ? SEMALEG : HEAD );
			neigbour->SetHotPoint( gelem->GetHotPoint() );
			neigbour->SetSizes( gsize.w, gsize.w );
			auto neighclone = neigbour->Clone();
			pgom->AddObject( neighclone, gdoc.get() );
			gclone->Join( neighclone );
		}
		mulEditor->StoreManipulations();

		//операция слияния
		ManipulatorPtr manptr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
		ManipulatorTest * mantest = static_cast <ManipulatorTest *>( manptr.get() );
		mantest->SetTxtMerging( true );
		manptr->StartMouseManipulation( Manipulator::MOVING, CGridPoint( 11, 10 ) );
		manptr->MouseManipulating( gelem->GetHotPoint() );
		manptr.reset();

		//в случае успешного слияния текстовый объект исчезает
		set <const CGraphicElement *> tstObjects;
		pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), tstObjects );
		auto toIt = find_if( tstObjects.cbegin(), tstObjects.cend(), [&tstHotPoint]( const CGraphicElement * gelem ){
			return gelem->GetHotPoint() == tstHotPoint && gelem->GetClass() != HEAD;
		} );
		CPPUNIT_ASSERT( toIt != tstObjects.cend() );
		auto tstObject = *toIt;
		wstring gname = tstObject->GetName();
		if ( gname == gtext->GetText() )
		{
			//успешное слияние
			if ( elclass == SEMALEG || elclass == HEAD )
				CPPUNIT_ASSERT( tstObjects.size() == 2 );
			else
				CPPUNIT_ASSERT( tstObjects.size() == 1 );
			CPPUNIT_ASSERT( elclass & TXT_MERGEABLE );
			mulEditor->MoveState( false ); //откат слияния
		}
		else
		{
			//не успешное слияние
			CPPUNIT_ASSERT( gname.empty() || elclass == GRAPHIC_TEXT );
			CPPUNIT_ASSERT( !( elclass & TXT_MERGEABLE ) );
		}
		mulEditor->MoveState( false ); //откат добавления тестового объекта
	}
}

void TC_Manipulator::SemaZChanging()
{
	MultiEditor * mulEditor = gdoc->GetEditor();
	Editor <GTraits> * geditor = mulEditor;
	geditor->OpenPool( gdoc.get() );
	CGraphicElement * ge = BuildSemalegKit( CGridPoint( 10, 10 ), 1 );
	ge->SetZLevel( 10 );
	mulEditor->StoreManipulations();

	int glegZLevel = 0, gheadZLevel = 0;
	set <CGraphicElement *> gelems;
	pgom->GetObjects<CGraphicElement *>( ALL_CLASSES, ALL_STATUSES, gdoc.get(), gelems );
	pgom->SendToBack( gelems, gdoc.get() );
	mulEditor->StoreManipulations();
	for ( const CGraphicElement * ge : gelems )
	{
		if ( ge->GetClass() == SEMALEG )
			glegZLevel = ge->GetZLevel();
		else
			gheadZLevel = ge->GetZLevel();
	}

	mulEditor->MoveState( false );
	mulEditor->MoveState( true );
	set <const CGraphicElement *> gelemsAfter;
	pgom->GetObjects<const CGraphicElement *>( ALL_CLASSES, ALL_STATUSES, gdoc.get(), gelemsAfter );
	CPPUNIT_ASSERT( gelemsAfter.size() == 2 );
	for ( const CGraphicElement * ge : gelemsAfter )
	{
		if ( ge->GetClass() == SEMALEG )
			CPPUNIT_ASSERT( glegZLevel == ge->GetZLevel() );
		else
			CPPUNIT_ASSERT( gheadZLevel == ge->GetZLevel() );
	}
}

void TC_Manipulator::SelectingGraphicTextsQuasiName()
{
	GraphElemPtr gtextPtr = FastCreate( GRAPHIC_TEXT );
	CGraphicText * gtextObj = static_cast<CGraphicText *>( gtextPtr.get() );
	gtextObj->SetHotPoint( CGridPoint( 10, 10 ) );
	ScopedGraphics sgraphics( gdoc->GetView() );
	GraphicContext * gcont = const_cast <GraphicContext *>( sgraphics.Get() );
	CGridRect nameRect, objRect;
	gtextPtr->GetObjectRect( objRect );
	gtextPtr->GetNameRect( nameRect, gcont );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	pgom->AddObject( gtextObj->Clone(), gdoc.get(), UNSELECTED );
	MultiEditor * deditor = gdoc->GetEditor();
	deditor->StoreManipulations();
	{
		ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
		auto selrectTopLeftY = objRect.rightbottom.y + numeric_limits<GRID_COORD>::epsilon();
		CGridRect selrect( CGridPoint( nameRect.topleft.x, selrectTopLeftY ), nameRect.rightbottom );
		manPtr->StartMouseManipulation( Manipulator::SELECTING, selrect.topleft );
		manPtr->MouseManipulating( nameRect.rightbottom );
	}
	set<const CGraphicElement *> gtexts;
	pgom->GetObjects( GRAPHIC_TEXT, SELECTED, gdoc.get(), gtexts );
	CPPUNIT_ASSERT( gtexts.empty() );
}
