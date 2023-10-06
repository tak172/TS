#include "stdafx.h"
#include "TC_GraphicDocument.h"
#include "../StatBuilder/MultiEditor.h"
#include "GraphicDocumentTest.h"
#include "GraphicViewTest.h"
#include "../StatBuilder/GObjectManager.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GraphicDocument );

void TC_GraphicDocument::Creating()
{
	CGraphicDocumentTest gdoc;
	CGObjectManager * pgom = gdoc.GetGObjectManager();
	CPPUNIT_ASSERT( pgom );
	bool has_selected = pgom->HasSelected( &gdoc ); //косвенная проверка на доступ к документу gdoc (GetKeeper())
	CPPUNIT_ASSERT( !has_selected );
}