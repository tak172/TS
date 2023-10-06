#include "stdafx.h"
#include "TC_Superselected.h"
#include "../StatBuilder/MultiEditor.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GObjectManager.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Superselected );

void TC_Superselected::ResetSelection()
{
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	BuildSwitchKit( CGridPoint( 10, 10 ) );

	SelectAll();
	const CGraphicElement * supersel_ge = pgom->GetSuperselected( gdoc.get() );
	CPPUNIT_ASSERT( supersel_ge );
	set <const CGraphicElement *> allobjects;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), allobjects );
	pgom->SetStatus( allobjects, UNSELECTED, gdoc.get() );
	const CGraphicElement * new_supersel_ge = pgom->GetSuperselected( gdoc.get() );
	CPPUNIT_ASSERT( new_supersel_ge == nullptr );
}

void TC_Superselected::Initialization()
{
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	const CGraphicElement * supersel_ge = pgom->GetSuperselected( gdoc.get() );
	CPPUNIT_ASSERT( supersel_ge == nullptr );
}