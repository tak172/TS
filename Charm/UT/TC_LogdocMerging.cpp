#include "stdafx.h"
#include "TC_LogdocMerging.h"
#include "LogicDocumentTest.h"
#include "ProjectDocumentTest.h"
#include "../Statbuilder/LosesErsatz.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/StationManager.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LogdocMerging );

void TC_LogdocMerging::Mergability()
{
	CLogicDocumentTest docarr[3];
	wstring strip_names[] = { L"strip_name", L"strip_name", L"original_strip_name" };
	for ( unsigned int k = 0; k < 3; ++k )
	{
		CStationManager * pstam = docarr[k].GetStationManager();
		EsrKit ecode( k + 1 );
		SetStationEsr( &docarr[k], ecode );
		CLObjectManager * plom = docarr[k].GetLObjectManager();
		CLogicElement * lstrip = BuildObject( STRIP, false );
		lstrip->SetName( strip_names[k] );
		plom->AddObject( lstrip, &docarr[k] );
	}
	
	wstring commentary;
	CPPUNIT_ASSERT( !pstam->Mergable( &docarr[0], &docarr[1], commentary ) );
	CPPUNIT_ASSERT( pstam->Mergable( &docarr[1], &docarr[2], commentary ) );
	CPPUNIT_ASSERT( pstam->Mergable( &docarr[0], &docarr[2], commentary ) );
}

void TC_LogdocMerging::Merging()
{
	CLogicDocumentTest docarr[3];
	CStationManager * pstam = projdoc->GetStationManager();
	for ( unsigned int k = 0; k < 3; ++k )
	{
		projdoc->Include( &docarr[k] );
		EsrKit ecode( k + 1 );
		SetStationEsr( &docarr[k], ecode );
		BuildObject( STRIP, true, &docarr[k] );
	}

	pstam->Merge( &docarr[0], &docarr[1] );
	pstam->Merge( &docarr[0], &docarr[2] );

	for ( unsigned int k = 0; k < 3; ++k )
	{
		CLObjectManager * plom = docarr[k].GetLObjectManager();
		set <const CLogicElement *> all_objects;
		plom->GetObjects( ALL_CLASSES, &docarr[k], all_objects );
		if ( k == 0 )
			CPPUNIT_ASSERT( all_objects.size() == 3 );
		else
			CPPUNIT_ASSERT( all_objects.size() == 1 );
	}
}