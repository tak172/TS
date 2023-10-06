#include "stdafx.h"
#include "TC_GManipulationEnabler.h"
#include "../StatBuilder/MouseHandler.h"
#include "../StatBuilder/GraphicView.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/SwitchMemento.h"
#include "../StatBuilder/GManipulationEnabler.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GManipulationEnabler );

GManipulationEnablerPtr TC_GManipulationEnabler::GetEnabler() const
{
	return gdoc->GetEditor()->GetManipulationEnabler( gdoc.get() );
}

void TC_GManipulationEnabler::OnlySwitchesSelected()
{
	BuildSwitchKit( CGridPoint( 10, 10 ) );
	BuildSwitchKit( CGridPoint( 20, 20 ) );
	Select( SWITCH );

	GManipulationEnablerPtr gmanenaPtr = GetEnabler();
	CPPUNIT_ASSERT( !gmanenaPtr->ObjectsZChangable() );
	CPPUNIT_ASSERT( !gmanenaPtr->ObjectsAlignable() );
	CPPUNIT_ASSERT( !gmanenaPtr->ObjectsDeletable() );
	CPPUNIT_ASSERT( !gmanenaPtr->ObjectsMoveable() );
}

void TC_GManipulationEnabler::OnlySemaforsSelected()
{
	//светофорные мачты
	BuildSemalegKit( CGridPoint( 10, 10 ), 2 );
	BuildSemalegKit( CGridPoint( 10, 20 ), 2 );
	Select( ALL_CLASSES );

	GManipulationEnablerPtr gmanenaPtr = GetEnabler();
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsZChangable() );
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsAlignable() );
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsDeletable() );
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsMoveable() );
}

void TC_GManipulationEnabler::SwitchesNStripsSelected()
{
	BuildSwitchKit( CGridPoint( 10, 10 ) );
	BuildSwitchKit( CGridPoint( 20, 20 ) );
	Select( ALL_CLASSES );

	//ObjectMenuRegulator omenuRegul( gdoc.get(), gdoc->GetEditor()->CanStartManipulation( gdoc.get() ), STRIP );
	GManipulationEnablerPtr gmanenaPtr = GetEnabler();
	CPPUNIT_ASSERT( !gmanenaPtr->ObjectsZChangable() );
	CPPUNIT_ASSERT( !gmanenaPtr->ObjectsAlignable() );
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsDeletable() );
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsMoveable() );
}

void TC_GManipulationEnabler::FiguresNSemaforsSelected()
{
	//светофорные мачты
	BuildSemalegKit( CGridPoint( 10, 10 ), 1 );
	BuildSemalegKit( CGridPoint( 20, 20 ), 1 );

	//фигуры
	GraphElemPtr figure_sample = FastCreate( FIGURE );
	pgom->AddObject( figure_sample->Clone(), gdoc.get(), SELECTED );
	pgom->AddObject( figure_sample->Clone(), gdoc.get(), SELECTED );

	Select( ALL_CLASSES );

	GManipulationEnablerPtr gmanenaPtr = GetEnabler();
	//ObjectMenuRegulator omenuRegul( gdoc.get(), gdoc->GetEditor()->CanStartManipulation( gdoc.get() ), STRIP );
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsZChangable() );
	CPPUNIT_ASSERT( !gmanenaPtr->ObjectsAlignable() );
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsDeletable() );
	CPPUNIT_ASSERT( gmanenaPtr->ObjectsMoveable() );
}