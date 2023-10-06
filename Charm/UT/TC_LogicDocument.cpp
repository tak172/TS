#include "stdafx.h"
#include "TC_LogicDocument.h"
#include "LogicDocumentTest.h"
#include "LogicViewTest.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LLoadResult.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/StationManager.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LogicDocument );

void TC_LogicDocument::setUp()
{
    TC_Logic::setUp_helper();
}

void TC_LogicDocument::tearDown()
{
    TC_Logic::tearDown_helper();
}

void TC_LogicDocument::Creating()
{
	CLogicDocumentTest ldoc;
	CLObjectManager * plom = ldoc.GetLObjectManager();
	CPPUNIT_ASSERT( plom );
	unsigned int objcount = plom->CountObjects( &ldoc ); //косвенная проверка на доступ к документу (GetKeeper())
	CPPUNIT_ASSERT( objcount == 0 );
}

void TC_LogicDocument::CorrectJointTesting()
{
	//стык с двумя участками
	pair <CLogicElement *, CLogicElement *> strips_near;
	BuildJoint( strips_near );
	logdoc->FinalizeLoading();
	wstring load_info = logdoc->GetLoadResult()->GetDetailedInfo();
	CPPUNIT_ASSERT( load_info.empty() );
}

void TC_LogicDocument::OnesidedJointTesting()
{
	//стык с одним участком
	CLogicElement * lstrip = BuildObject( STRIP );
	CLogicElement * ljoint = BuildObject( JOINT );
	plim->Join( lstrip, ljoint->BuildLink( EsrCode() ) );
	logdoc->FinalizeLoading();
	wstring load_info = logdoc->GetLoadResult()->GetDetailedInfo();
	CPPUNIT_ASSERT( !load_info.empty() );
}

void TC_LogicDocument::SingleJointTesting()
{
	//стык без участков
	BuildObject( JOINT );
	logdoc->FinalizeLoading();
	wstring load_info = logdoc->GetLoadResult()->GetDetailedInfo();
	CPPUNIT_ASSERT( !load_info.empty() );
}

void TC_LogicDocument::WaysNumsTesting()
{
	SetSpanEsr( logdoc.get(), EsrKit( 1, 2 ) );
	pair <CLogicElement *, CLogicElement *> lstrips;
	BuildJoint( lstrips );
	CLogicStrip * lstrip1 = static_cast <CLogicStrip *>( lstrips.first );
	CLogicStrip * lstrip2 = static_cast <CLogicStrip *>( lstrips.second );
	lstrip1->SetWaynum( 1 );
	lstrip2->SetWaynum( 2 );
	logdoc->GetStationManager();
	logdoc->FinalizeLoading();
	wstring load_info = logdoc->GetLoadResult()->GetDetailedInfo();
	CPPUNIT_ASSERT( !load_info.empty() );
}