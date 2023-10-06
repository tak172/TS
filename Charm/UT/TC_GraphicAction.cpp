#include "stdafx.h"
#include "TC_GraphicAction.h"
#include "GraphicDocumentTest.h"
#include "GraphicViewTest.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/Action.h"
#include "../StatBuilder/GraphicAction.h"
#include "../StatBuilder/MacroAction.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GraphicAction );

void TC_GraphicAction::FigureMoving()
{
	GraphElemPtr gfigure = FastCreate( FIGURE );
	CGridPoint inithp( 10, 10 );
	gfigure->SetHotPoint( inithp );
	pgom->AddObject( gfigure->Clone(), gdoc.get() );
	GraphElemPtr ge_from( gfigure->Clone() );
	GraphElemPtr ge_to( gfigure->Clone() );
	ge_to->Move( CGridSize( 5, 5 ), false );
	MacroAction maction;
	boost::scoped_ptr <Action> pact( new GraphicAction( GWiden( ge_to.get() ), GWiden( ge_from.get() ), gdoc.get(), maction ) );
	pact->Execute( true ); //двигаем фигуру
	set <const CGraphicElement *> resset;
	pgom->GetObjects( FIGURE, ALL_STATUSES, gdoc.get(), resset );
	CPPUNIT_ASSERT( resset.size() == 1 );
	CPPUNIT_ASSERT( ( *resset.begin() )->GetHotPoint() == CGridPoint( inithp.x + 5, inithp.y + 5 ) );
}