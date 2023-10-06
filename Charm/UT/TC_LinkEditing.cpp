#include "stdafx.h"
#include "TC_LinkEditing.h"
#include "LogicDocumentTest.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/Router.h"
#include "../StatBuilder/MultiEditor.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LinkEditing );

void TC_LinkEditing::LStripUnlinking()
{
	projdoc->OpenAllPools();
	CreateTestRoute();
	logdoc->GetEditor()->StoreManipulations();

	set <const CLogicElement *> initllegs;
	plom->GetObjects( SEMALEG, logdoc.get(), initllegs );
	CPPUNIT_ASSERT( initllegs.size() == 1 );
	const vector <CLink> & leglinks = (*initllegs.begin())->GetLinks( STRIP, CLink::TO );
	CPPUNIT_ASSERT( leglinks.size() == 1 );
	CLogicElement * start_strip = plom->GetObjectByBadge<CLogicElement *>( leglinks[0].obj_badge );
	plim->UnlinkFromAll( start_strip );
	projdoc->GetEditor()->StoreManipulations();

	set <const CLogicElement *> lroutes, newllegs, ljoints;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	plom->GetObjects( SEMALEG, logdoc.get(), newllegs );
	plom->GetObjects( JOINT, logdoc.get(), ljoints );
	CPPUNIT_ASSERT( lroutes.empty() );
	CPPUNIT_ASSERT( newllegs.size() == 1 );
	const CLogicElement * lleg = *newllegs.begin();
	const vector <CLink> & semalinks = lleg->GetLinks();
	CPPUNIT_ASSERT( semalinks.size() == 1 );
	CPPUNIT_ASSERT( ljoints.size() == 1 );
}

void TC_LinkEditing::LHeadUnlinking()
{
	projdoc->OpenAllPools();
	CreateTestRoute();
	logdoc->GetEditor()->StoreManipulations();

	set <CLogicElement *> initlheads;
	plom->GetObjects( HEAD, logdoc.get(), initlheads );
	CPPUNIT_ASSERT( initlheads.size() == 1 );
	plim->UnlinkFromAll( *initlheads.begin() );
	projdoc->GetEditor()->StoreManipulations();

	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.empty() );
}

void TC_LinkEditing::LCommutatorUnlinking()
{
	//отсоединение коммутатора не должно влиять на существование маршрута
	projdoc->OpenAllPools();
	CreateTestRoute();
	logdoc->GetEditor()->StoreManipulations();

	set <CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CLogicElement * lroute = *lroutes.begin();
	CLogicElement * lcommutator = BuildObject( COMMUTATOR );
	const CLink & commLink = lcommutator->BuildLink( EsrCode() );
	plim->Join( *lroutes.begin(), commLink );
	logdoc->GetEditor()->StoreManipulations();

	vector <CLink> commlinks = lcommutator->GetLinks();
	CPPUNIT_ASSERT( commlinks.size() == 1 );
	vector <CLink> routelinks = lroute->GetLinks( COMMUTATOR );
	CPPUNIT_ASSERT( routelinks.size() == 1 );
	plim->UnlinkFromAll( lcommutator );
	logdoc->GetEditor()->StoreManipulations();

	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 1 );
	lroute = *lroutes.begin();
	commlinks = lcommutator->GetLinks();
	routelinks = lroute->GetLinks( COMMUTATOR );
	CPPUNIT_ASSERT( commlinks.empty() );
	CPPUNIT_ASSERT( routelinks.empty() );
}

void TC_LinkEditing::LJointUnlinking()
{
	projdoc->OpenAllPools();
	CreateTestRoute();
	logdoc->GetEditor()->StoreManipulations();

	set <const CLogicElement *> llegs;
	plom->GetObjects( SEMALEG, logdoc.get(), llegs );
	CPPUNIT_ASSERT( llegs.size() == 1 );
	const vector <CLink> & leglinks = (*llegs.begin())->GetLinks( STRIP );
	CPPUNIT_ASSERT( leglinks.size() == 2 );
	CLogicElement * strip0 = plom->GetObjectByBadge<CLogicElement *>( leglinks[0].obj_badge );
	CLogicElement * strip1 = plom->GetObjectByBadge<CLogicElement *>( leglinks[1].obj_badge );
	CLink interlink;
	const CLogicElement * lelem = plim->GetAnyCommonLink( strip0, strip1, interlink, ALL_CLASSES );
	CLogicElement * ljoint = plom->Release( lelem, logdoc.get() );
	plim->UnlinkFromAll( ljoint );
	logdoc->GetEditor()->StoreManipulations();

	set <const CLogicElement *> lroutes, ljoints;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	plom->GetObjects( JOINT, logdoc.get(), ljoints );
	CPPUNIT_ASSERT( lroutes.empty() );
	CPPUNIT_ASSERT( ljoints.size() == 1 );
}

void TC_LinkEditing::LStripLSwitchBreaking()
{
	projdoc->OpenAllPools();
	CreateTestRoute();
	logdoc->GetEditor()->StoreManipulations();

	set <CLogicElement *> lswitches;
	plom->GetObjects( SWITCH, logdoc.get(), lswitches );
	CLogicElement * lswitch = *lswitches.begin();
	CPPUNIT_ASSERT( lswitches.size() == 1 );
	const vector <CLink> & switch_links = lswitch->GetLinks( STRIP );
	CPPUNIT_ASSERT( switch_links.size() == 3 );
	const CLogicElement * strip_near = plom->GetObjectByBadge<const CLogicElement *>( switch_links[0].obj_badge );
	const CLink & strip_link = strip_near->BuildLink( EsrCode() );
	plim->Unjoin( lswitch, strip_link );
	logdoc->GetEditor()->StoreManipulations();

	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.empty() );
}

void TC_LinkEditing::tearDown()
{
    logdoc->GetEditor()->ResetManipulations();
    TC_Project::tearDown();
}
