#include "stdafx.h"
#include "LogicDocumentTest.h"
#include "LogicViewTest.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LLoadResult.h"

using namespace std;

CLogicDocumentTest::CLogicDocumentTest()
{
	lview.reset( new CLogicViewTest() );
	loadResult.reset( new LLoadResult( wstring() ) );
}

void CLogicDocumentTest::FinalizeLoading()
{
	LogicValidator lvalidator( this, loadResult );
	lvalidator.Validate();
}
