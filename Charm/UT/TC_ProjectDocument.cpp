#include "stdafx.h"
#include "TC_ProjectDocument.h"
#include "ProjectDocumentTest.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/LogicView.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ProjectDocument );

void TC_ProjectDocument::Unjoin()
{
	Editor <LTraits> * leditor = logdoc->GetEditor();
	leditor->OpenPool( logdoc.get() );
	CLogicDocument * ldoc = logdoc.get();
	projdoc->UnjoinChildDocument<LTraits>( ldoc );
	leditor = logdoc->GetEditor();
	CPPUNIT_ASSERT( leditor->PoolOpened( ldoc ) );
}