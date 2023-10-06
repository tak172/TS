#include "stdafx.h"
#include "TC_LogicView.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LogicView.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LogicView );

using namespace std;

void TC_LogicView::setUp()
{
    TC_Logic::setUp_helper();
}

void TC_LogicView::tearDown()
{
    TC_Logic::tearDown_helper();
}

void TC_LogicView::AlphanumComparing()
{
	CLogicElement * le1 = BuildObject( STRIP, false );
	CLogicElement * le2 = BuildObject( STRIP, false );
	le1->SetName( L"abcd" );
	le2->SetName( L"ghjk200" );
	CPPUNIT_ASSERT( CLogicView::AlphaNumCompare( le1, le2 ) == -1 );
	le1->SetName( L"10ï" );
	le2->SetName( L"4ï" );
	CPPUNIT_ASSERT( CLogicView::AlphaNumCompare( le1, le2 ) == 1 );
	le1->SetName( L"×bcd4fÂ" );
	le2->SetName( L"×bcd28fÂ" );
	CPPUNIT_ASSERT( CLogicView::AlphaNumCompare( le1, le2 ) == -1 );
	le1->SetName( L"xcdf28" );
	le2->SetName( L"xcdg4" );
	CPPUNIT_ASSERT( CLogicView::AlphaNumCompare( le1, le2 ) == -1 );
	le1->SetName( L"pkq56" );
	le2->SetName( L"pkq9" );
	CPPUNIT_ASSERT( CLogicView::AlphaNumCompare( le1, le2 ) == 1 );
    delete le1;
    delete le2;
}