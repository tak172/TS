#include "stdafx.h"
#include "TC_Project.h"
#include "../StatBuilder/MultiEditor.h"
#include "ProjectDocumentTest.h"
#include "LogicDocumentTest.h"
#include "GraphicDocumentTest.h"

void TC_Project::setUp()
{
    TC_Graphic::setUp_helper();
    TC_Logic::setUp_helper();
    projdoc.reset( new CProjectDocumentTest() );
    projdoc->Include( logdoc.get() );
    projdoc->Include( gdoc.get() );
    plim = projdoc->GetLinkManager();
    pstam = projdoc->GetStationManager();
    plom = projdoc->GetLObjectManager();
	projdoc->SetThisProject();
}

void TC_Project::tearDown()
{
    projdoc.reset();
    TC_Logic::tearDown_helper();
    TC_Graphic::tearDown_helper();
}
