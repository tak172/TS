#include "stdafx.h"
#include "TC_LogicElement.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LinkManager.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LogicElement );

void TC_LogicElement::setUp()
{
    TC_Logic::setUp_helper();
}

void TC_LogicElement::tearDown()
{
    TC_Logic::tearDown_helper();
}

void TC_LogicElement::LinkSorting()
{
	CLogicElement * lstrip = BuildObject( STRIP );
	CLogicElement * ljoint = BuildObject( JOINT );
	CLogicElement * lroute = BuildObject( ROUTE );
	CLinkManager * plim = logdoc->GetLinkManager();
	const CLink & routeLink = lroute->BuildLink( EsrCode() );
	const CLink & jointLink = ljoint->BuildLink( EsrCode() );
	plim->Join( lstrip, routeLink );
	plim->Join( lstrip, jointLink );
	const auto & stripLinks = lstrip->GetLinks();
	CPPUNIT_ASSERT( !stripLinks.empty() );

	//линки с маршрутами должны отсортировываться в конец вектора
	CPPUNIT_ASSERT( stripLinks[0] == jointLink );
	CPPUNIT_ASSERT( stripLinks[1] == routeLink );
}