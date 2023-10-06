#include "stdafx.h"
#include "TC_GraphicGrid.h"
#include "../helpful/Informator.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/GObjectManager.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GViewSkin.h"
#include "../StatBuilder/GraphicGrid.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GraphicGrid );

using namespace std;

void TC_GraphicGrid::ObjectsValidness()
{
	BuildSwitchKit( CGridPoint( 10, 10 ) );
	BuildSemalegKit( CGridPoint( 20, 10 ), 3 );
	CInformator * I = CInformator::Instance();
	list <ELEMENT_CLASS> graphic_classes;
	I->GetGraphicClasses( graphic_classes );
	for( ELEMENT_CLASS elclass : graphic_classes )
	{
		if ( elclass != SWITCH && elclass != STRIP && elclass != SEMALEG && elclass != HEAD )
		{
			GraphElemPtr gelem_ptr = FastCreate( elclass );
			pgom->AddObject( gelem_ptr->Clone(), gdoc.get() );
		}
	}
	CGraphicGrid * grid = gdoc->GetSkin()->GetGrid();
	grid->Increase();
	set <const CGraphicElement *> allobjects;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), allobjects );
	for( const CGraphicElement * ge : allobjects )
		CPPUNIT_ASSERT( ge->IsValid() );
}

void TC_GraphicGrid::PixelRound()
{
	CGraphicGrid * grid = gdoc->GetSkin()->GetGrid();
	grid->SetSize( 7.5 );
	CPPUNIT_ASSERT( round( grid->PixelRound( 2.75 ), 1 ) == 2.8 );
	grid->SetSize( 32 );
	CPPUNIT_ASSERT( round( grid->PixelRound( 5.2 ), 4 ) == 5.1875 );
	grid->SetSize( 1 );
	CPPUNIT_ASSERT( round( grid->PixelRound( 0.5 ), 0 ) == 1 );
}