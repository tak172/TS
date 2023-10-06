#include "stdafx.h"
#include "ProjectDocumentTest.h"
#include "GraphicDocumentTest.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/MultiEditor.h"

using namespace std;

CProjectDocumentTest::CProjectDocumentTest()
{
	projview.reset( new CProjectViewTest( this ) );
}

CProjectView * CProjectDocumentTest::GetView() const
{ 
	return projview.get(); 
}

void CProjectDocumentTest::OpenAllPools() const
{
	vector <CLogicDocument *> logdocs;
	vector <CGraphicDocument *> graphdocs;
	GetDocuments( logdocs );
	GetDocuments( graphdocs );
	Editor <LTraits> * leditor = GetEditor();
	for( CLogicDocument * ldoc : logdocs )
		leditor->OpenPool( ldoc );
	Editor <GTraits> * geditor = GetEditor();
	for( CGraphicDocument * gdoc : graphdocs )
		geditor->OpenPool( gdoc );
}