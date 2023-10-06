#include "stdafx.h"
#include "TC_StationManager.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/LObjectManager.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/MultiEditor.h"
#include "ProjectDocumentTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_StationManager );

using namespace std;

void TC_StationManager::StationByLe()
{
	CLogicElement * lstrip = CLogicElementFactory().CreateObject( STRIP );
	lstrip->SetName( L"Strip_name" );
	plom->AddObject( lstrip, logdoc.get() );
	const EsrKit & strip_esr = pstam->GetESR( lstrip );
	CPPUNIT_ASSERT( EsrCode() == strip_esr );
}

void TC_StationManager::EsrChanging()
{
	//имеем две станции и перегон между ними
	const EsrKit second_esr( EsrCode().getTerm() + 1 );
	LogDocTestPtr second_station = AddStation( second_esr );
	const EsrKit span_esr( EsrCode().getTerm(), second_esr.getTerm() );
	LogDocTestPtr span_doc = AddStation( span_esr );

	//меняем ЕСР-код одной из станций
	EsrKit newEsr( EsrCode().getTerm() + 5 );
	pstam->ReplaceESR( EsrCode(), newEsr );
	CPPUNIT_ASSERT( pstam->GetESR( logdoc.get() ) == newEsr );
	CPPUNIT_ASSERT( pstam->GetESR( second_station.get() ) == second_esr );
	const EsrKit & new_span_esr = pstam->GetESR( span_doc.get() );
	CPPUNIT_ASSERT( EsrKit::intersect( new_span_esr, newEsr ) == newEsr );
	CPPUNIT_ASSERT( EsrKit::intersect( new_span_esr, second_esr ) == second_esr );
}

TC_StationManager::LogDocTestPtr TC_StationManager::AddStation( const EsrKit & esrCode )
{
	LogDocTestPtr secdoc( new CLogicDocumentTest );
	projdoc->Include( secdoc.get() );
	if ( esrCode.terminal() )
		SetStationEsr( secdoc.get(), esrCode );
	else
		SetSpanEsr( secdoc.get(), esrCode );
	return secdoc;
}