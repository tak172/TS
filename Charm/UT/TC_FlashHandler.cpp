#include "stdafx.h"
#include "TC_FlashHandler.h"
#include "../StatBuilder/FlashHandler.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/SwitchMemento.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/GraphicSwitch.h"
#include "../StatBuilder/GraphicStrip.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_FlashHandler );

void TC_FlashHandler::FigureMoving()
{
	//готовим фигуру
	GraphElemPtr figure_sample = FastCreate( FIGURE );
	CGraphicElement * gfigure = figure_sample->Clone();
	pgom->AddObject( gfigure, gdoc.get(), SELECTED );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	CGridPoint initHP = gfigure->GetHotPoint();

	//смещаем
	CGridSize shift( 15, 20 );
	FlashMoveHandler fmoveHandler( gdoc->GetView(), gdoc->GetEditor(), false );
	fmoveHandler.Shift( shift.w, shift.h );
	set <const CGraphicElement *> allfigures;
	pgom->GetObjects( FIGURE, SELECTED, gdoc.get(), allfigures );
	CPPUNIT_ASSERT( allfigures.size() == 1 );
	CPPUNIT_ASSERT( ( *allfigures.begin() )->GetHotPoint() == CGridPoint( initHP.x + shift.w, initHP.y + shift.h ) );
	wstring problemTxt;
	fmoveHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );
}

void TC_FlashHandler::SemaMoving()
{
	//готовим светофор
	CGridPoint leg_inithp( 100, 100 );
	CGraphicElement * gleg = BuildSemalegKit( leg_inithp, 1 );
	Select( ALL_CLASSES );
	const vector <CGraphicElement *> & subobjects = gleg->GetSubObjects();
	CGraphicElement * ghead = subobjects.back();
	const CGridPoint & head_inithp = ghead->GetHotPoint();
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );

	//смещаем
	CGridSize shift( 17.25, 9.68 );
	FlashMoveHandler fmoveHandler( gdoc->GetView(), gdoc->GetEditor(), false );
	fmoveHandler.Shift( shift.w, shift.h );
	wstring problemTxt;
	fmoveHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );
	set <const CGraphicElement *> allobjects;
	pgom->GetObjects( ALL_CLASSES, SELECTED, gdoc.get(), allobjects );
	CPPUNIT_ASSERT( allobjects.size() == 2 );
	for( const CGraphicElement * ge : allobjects )
	{
		CPPUNIT_ASSERT( ge == gleg || ge == ghead );
		if ( ge == gleg )
			CPPUNIT_ASSERT( ge->GetHotPoint() == CGridPoint( leg_inithp.x + shift.w, leg_inithp.y + shift.h ) );
		else
			CPPUNIT_ASSERT( ge->GetHotPoint() == CGridPoint( head_inithp.x + shift.w, head_inithp.y + shift.h ) );
	}
}

void TC_FlashHandler::SemaRotate()
{
	//готовим светофор с двум€ головами
	CGridPoint initHP( 100, 100 );
	BuildSemalegKit( initHP, 2 );
	Select( ALL_CLASSES );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );

	//вращаем
	double rotangle = M_PI_4;
	FlashMoveHandler fmoveHandler( gdoc->GetView(), gdoc->GetEditor(), false );
	fmoveHandler.Rotate( rotangle );
	wstring problemTxt;
	fmoveHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );
	set <const CGraphicElement *> allobjects;
	pgom->GetObjects( ALL_CLASSES, SELECTED, gdoc.get(), allobjects );
	CPPUNIT_ASSERT( allobjects.size() == 3 );
	for( const CGraphicElement * ge : allobjects )
		CPPUNIT_ASSERT( ge->IsValid() );
}

void TC_FlashHandler::StripsCrossing()
{
	GraphElemPtr gptr = FastCreate( STRIP );
	CGraphicElement * ge1 = gptr->Clone();
	ge1->SetHotPoint( CGridPoint( 1, 1 ) );
	ge1->SetSizes( 6, 0 );
	pgom->AddObject( ge1, gdoc.get() );
	CGraphicElement * ge2 = gptr->Clone();
	ge2->SetHotPoint( CGridPoint( 3, 1 ) );
	ge2->SetSizes( 3, 3 );
	//ge2 - без добавлени€ в документ
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );

	list <CGraphicElement *> inserted_elems;
	inserted_elems.push_back( ge2 );
	FlashAddHandler faddHandler( gdoc->GetView(), gdoc->GetEditor(), inserted_elems );
	wstring problemTxt;
	faddHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );

	CGridRect lookedRect( CGridPoint( 3, 1 ), CGridPoint( 6, 4 ) );
	set <const CGraphicElement *> allstrips, allswitches;
	pgom->GetObjects( lookedRect, STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	pgom->GetObjects( lookedRect, SWITCH, ALL_STATUSES, gdoc.get(), allswitches );
	CPPUNIT_ASSERT( allstrips.size() == 3 );
	CPPUNIT_ASSERT( allswitches.size() == 1 );
}

void TC_FlashHandler::TryingSwitchMoving()
{
	CGridPoint init_hp( 5, 0 );
	CGraphicElement * gswitch = BuildSwitchKit( init_hp );
	Select( SWITCH );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );

	FlashMoveHandler fmoveHandler( gdoc->GetView(), gdoc->GetEditor(), false );
	CGridSize shift( 20, 40 );
	fmoveHandler.Shift( shift.w, shift.h );
	wstring problemTxt;
	fmoveHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );

	set <const CGraphicElement *> switches;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), switches );
	CPPUNIT_ASSERT( switches.size() == 1 );
	CPPUNIT_ASSERT( gswitch == (*switches.begin()) );
	CPPUNIT_ASSERT( gswitch->GetHotPoint() == init_hp );
}

void TC_FlashHandler::SwitchedStripMoving()
{
	CGridPoint init_hp( 5, 0 );
	BuildSwitchKit( init_hp );
	set <CGraphicElement *> allstrips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	//выдел€ем диагональный участок
	for( CGraphicElement * gstrip : allstrips )
	{
		double w, h;
		gstrip->GetSizes( w, h );
		if ( w != 0 && h != 0 )
		{
			pgom->SetStatus( gstrip, SELECTED, gdoc.get() );
			break;
		}
	}
	
	//сдвиг
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	FlashMoveHandler fmoveHandler( gdoc->GetView(), gdoc->GetEditor(), false );
	CGridSize shift( 1, 0 );
	fmoveHandler.Shift( shift.w, shift.h );
	wstring problemTxt;
	fmoveHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );

	//проверка
	set <const CGraphicElement *> switches, strips;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), switches );
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), strips );
	CPPUNIT_ASSERT( switches.size() == 1 );
	CPPUNIT_ASSERT( (*switches.begin())->GetHotPoint() == CGridPoint( init_hp.x + shift.w, init_hp.y ) );
	CPPUNIT_ASSERT( strips.size() == 3 );
}

void TC_FlashHandler::SwitchedOrthoStripMoving()
{
	CGridPoint init_hp( 5, 0 );
	BuildSwitchKit( init_hp, CGridSize( -5, 0 ), CGridSize( 5, 0 ), CGridSize( 0, 5 ) );
	set <const CGraphicElement *> allstrips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	//выдел€ем вертикальный участок
	for( auto gstrip : allstrips )
	{
		double w, h;
		gstrip->GetSizes( w, h );
		if ( w == 0 && h != 0 )
		{
			pgom->SetStatus( gstrip, SELECTED, gdoc.get() );
			break;
		}
	}

	//сдвиг
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	FlashMoveHandler fmoveHandler( gdoc->GetView(), gdoc->GetEditor(), false );
	CGridSize shift( 1, 0 );
	fmoveHandler.Shift( shift.w, shift.h );
	wstring problemTxt;
	fmoveHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );

	//проверка
	set <const CGraphicElement *> switches, strips;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), switches );
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), strips );
	CPPUNIT_ASSERT( switches.size() == 1 );
	CPPUNIT_ASSERT( (*switches.begin())->GetHotPoint() == CGridPoint( init_hp.x + shift.w, init_hp.y ) );
	CPPUNIT_ASSERT( strips.size() == 3 );
}

void TC_FlashHandler::OrthoStripMoving()
{
	//формируем три перпендикул€рных участка
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * horiz1 = gstrip_sample->Clone();
	horiz1->SetHotPoint( CGridPoint( 0, 0 ) );
	horiz1->SetSizes( 5, 0 );
	pgom->AddObject( horiz1, gdoc.get() );

	CGraphicElement * horiz2 = gstrip_sample->Clone();
	horiz2->SetHotPoint( CGridPoint( 5, 0 ) );
	horiz2->SetSizes( 5, 0 );
	pgom->AddObject( horiz2, gdoc.get() );

	CGraphicElement * vert = gstrip_sample->Clone();
	vert->SetHotPoint( CGridPoint( 5, 0 ) );
	vert->SetSizes( 0, 5 );
	pgom->AddObject( vert, gdoc.get() );

	//выдел€ем вертикальный участок
	pgom->SetStatus( vert, SELECTED, gdoc.get() );

	//сдвиг
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	FlashMoveHandler fmoveHandler( gdoc->GetView(), gdoc->GetEditor(), false );
	CGridSize shift( 1, 0 );
	fmoveHandler.Shift( shift.w, shift.h );
	wstring problemTxt;
	fmoveHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );

	//проверка
	set <const CGraphicElement *> switches, strips;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), switches );
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), strips );
	CPPUNIT_ASSERT( switches.empty() );
	CPPUNIT_ASSERT( strips.size() == 3 );
}

void TC_FlashHandler::StripsFragmentationBid()
{
	CGraphicElement * gelem = BuildSwitchKit( CGridPoint( 10, 10 ) );
	CGraphicSwitch * gswitch = static_cast <CGraphicSwitch *>( gelem );
	CGraphicStrip * minus_strip = gswitch->GetSpecialStrip( CLink::MINUS );
	CPPUNIT_ASSERT( minus_strip );

	//выдел€ем минусовой участок
	pgom->SetStatus( minus_strip, SELECTED, gdoc.get() );

	//поворот
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	FlashMoveHandler fhandler( gdoc->GetView(), gdoc->GetEditor(), false );
	fhandler.Rotate( -M_PI_4 );
	wstring problemTxt;
	fhandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );

	//проверка
	set <const CGraphicElement *> switches, strips;
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), switches );
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), strips );
	CPPUNIT_ASSERT( switches.empty() );
	CPPUNIT_ASSERT( strips.size() == 2 );
}

void TC_FlashHandler::NoMovingOfTextsQuasiNames()
{
	//готовим текстовый объект
	GraphElemPtr gtext_sample = FastCreate( GRAPHIC_TEXT );
	CGraphicElement * gtext = gtext_sample->Clone();
	pgom->AddObject( gtext, gdoc.get(), SELECTED );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	const auto & nameShift = gtext->GetNameShift(); 
	CPPUNIT_ASSERT( nameShift.h == 0 && nameShift.w == 0 );

	//смещаем
	CGridSize shift( 15, 20 );
	FlashMoveHandler fmoveHandler( gdoc->GetView(), gdoc->GetEditor(), true );
	fmoveHandler.Shift( shift.w, shift.h );
	CPPUNIT_ASSERT( !pgom->StateChanged( gdoc.get() ) );
	set <const CGraphicElement *> allgtexts;
	pgom->GetObjects( GRAPHIC_TEXT, SELECTED, gdoc.get(), allgtexts );
	CPPUNIT_ASSERT( allgtexts.size() == 1 );
	auto modgtext = *allgtexts.cbegin();
	const auto & modNameShift = modgtext->GetNameShift();
	CPPUNIT_ASSERT( modNameShift.h == 0 && modNameShift.w == 0 );
	wstring problemTxt;
	fmoveHandler.Finish( problemTxt );
	CPPUNIT_ASSERT( problemTxt.empty() );
}