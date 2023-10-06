#include "stdafx.h"
#include "GraphicDocumentTest.h"
#include "GraphicViewTest.h"
#include "../Actor/Blinker.h"
#include "../StatBuilder/GViewSkin.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/GLoadResult.h"

using namespace std;

CGraphicDocumentTest::CGraphicDocumentTest()
{
	gview.reset( new CGraphicViewTest( this ) );
	Blinker * blinker = Blinker::instance();
	view_skin.reset( new GViewSkin( *blinker ) );
	GEF.reset( new CGraphicEditableFactory() );
	loadResult.reset( new GLoadResult( wstring() ) );
}

CGraphicDocumentTest::~CGraphicDocumentTest()
{
	Blinker::Shutdowner();
}

CGraphicView * CGraphicDocumentTest::GetView() const
{ 
	return static_cast <CGraphicView *> ( gview.get() ); 
}

CGraphicViewTest * CGraphicDocumentTest::GetViewTest() const
{
	return gview.get();
}