#include "stdafx.h"
#include "TC_GObjectManager.h"
#include "GraphicDocumentTest.h"
#include "GraphicViewTest.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/MultiEditor.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GObjectManager );

void TC_GObjectManager::AddRemoveSingleStrip()
{
	set <const CGraphicElement *> docObjects;
	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), docObjects );
	CPPUNIT_ASSERT( docObjects.empty() );

	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * gstrip = gstrip_sample->Clone();
	pgom->AddObject( gstrip, gdoc.get() );

	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), docObjects );
	CPPUNIT_ASSERT( docObjects.size() == 1 );
	pgom->RemoveObject( gstrip, gdoc.get() );

	pgom->GetObjects( ALL_CLASSES, ALL_STATUSES, gdoc.get(), docObjects );
	CPPUNIT_ASSERT( docObjects.empty() );
}

void TC_GObjectManager::GettingWhileAdding()
{
	//имитация ситуации, когда один объект (gstrip1) уже находится на картинке, 
	//а второй (gstrip2) только добавляется
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * gstrip1 = gstrip_sample->Clone();
	gstrip1->SetHotPoint( CGridPoint( 1, 1 ) );
	gstrip1->SetSizes( 6, 0 );
	pgom->AddObject( gstrip1, gdoc.get() );
	CGraphicElement * gstrip2 = gstrip_sample->Clone();
	gstrip2->SetHotPoint( CGridPoint( 3, 1 ) );
	gstrip2->SetSizes( 3, 3 );
	Editor <GTraits> * geditor = gdoc->GetEditor();
	list <CGraphicElement *> ghost_elems;
	ghost_elems.push_back( gstrip2 );
	pgom->AddGhostObjects( ghost_elems, gdoc.get() );
	CGridRect lookedRect( CGridPoint( 3, 1 ), CGridPoint( 6, 4 ) );
	set <CGraphicElement *> allelems;
	pgom->GetObjects( lookedRect, ALL_CLASSES, ALL_STATUSES, gdoc.get(), allelems );
	CPPUNIT_ASSERT( allelems.size() == 2 );
	CPPUNIT_ASSERT( allelems.find( gstrip1 ) != allelems.end() );
	CPPUNIT_ASSERT( allelems.find( gstrip2 ) != allelems.end() );
}