#include "stdafx.h"
#include "TC_LinkManager.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/LObjectManager.h"
#include "ProjectDocumentTest.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/LosesErsatz.h"
#include "LogicViewTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LinkManager );

using namespace std;
void TC_LinkManager::StripJointLinking()
{
	//связывание
	CLogicElement * lstrip1 = BuildObject( STRIP );
	CLink strip1Link = lstrip1->BuildLink( EsrCode() );

	CLogicElement * lstrip2 = BuildObject( STRIP );
	CLink strip2Link = lstrip2->BuildLink( EsrCode() );
	
	CLogicElement * ljoint = BuildObject( JOINT );
	CLink jointLink = ljoint->BuildLink( EsrCode() );

	vector <CLink> strip1Links = lstrip1->GetLinks();
	vector <CLink> strip2Links = lstrip2->GetLinks();
	vector <CLink> jointLinks = ljoint->GetLinks();
	CPPUNIT_ASSERT( strip1Links.empty() && strip2Links.empty() && jointLinks.empty() );
	CPPUNIT_ASSERT( plim->Join( ljoint, strip1Link ) );
	CPPUNIT_ASSERT( plim->Join( ljoint, strip2Link ) );
	strip1Links = lstrip1->GetLinks();
	strip2Links = lstrip2->GetLinks();
	jointLinks = ljoint->GetLinks();
	CPPUNIT_ASSERT( jointLinks.size() == 2 && strip1Links.size() == 1 && strip2Links.size() == 1 );
	CPPUNIT_ASSERT( jointLinks[0] == strip1Link && jointLinks[1] == strip2Link ); 
	CPPUNIT_ASSERT( strip1Links[0] == jointLink && strip1Links[0] == jointLink );

	//отвязывание
	plim->UnlinkFromAll( lstrip1 );
	strip1Links = lstrip1->GetLinks();
	strip2Links = lstrip2->GetLinks();
	CPPUNIT_ASSERT( strip1Links.empty() );
	CPPUNIT_ASSERT( strip2Links.size() == 1 );
	set <const CLogicElement *> all_joints;
	plom->GetObjects( JOINT, logdoc.get(), all_joints );
	CPPUNIT_ASSERT( all_joints.size() == 1 );
	jointLinks = (*all_joints.begin())->GetLinks();
	CPPUNIT_ASSERT( jointLinks.size() == 1 );
}

void TC_LinkManager::LegHeadLinking()
{
	const CLogicElement * lleg = BuildLeg( TRAIN );
	const CLink & legLink = lleg->BuildLink( EsrCode() );
	set <const CLogicElement *> lheads, llegs;
	plom->GetObjects( HEAD, logdoc.get(), lheads );
	plom->GetObjects( SEMALEG, logdoc.get(), llegs );

	CPPUNIT_ASSERT( lheads.size() == 1 );
	CPPUNIT_ASSERT( llegs.size() == 1 && ( *llegs.begin() ) == lleg );
	const CLogicElement * lhead = ( *lheads.begin() );
	const CLink & headLink = lhead->BuildLink( EsrCode() );
	const vector <CLink> & links_of_head = lhead->GetLinks();
	CPPUNIT_ASSERT( links_of_head.size() == 1 );
	CPPUNIT_ASSERT( links_of_head[0] == legLink );
	const vector <CLink> & links_of_leg = lleg->GetLinks();
	CPPUNIT_ASSERT( links_of_leg.size() == 1 );
	CPPUNIT_ASSERT( links_of_leg[0] == headLink );
}

void TC_LinkManager::DepthSearching()
{
	CLogicDocumentTest spandoc;
	projdoc->Include( &spandoc );
	EsrKit statEsr = EsrCode();
	EsrKit spanEsr( statEsr.getTerm(), statEsr.getTerm() + 1 );
	SetSpanEsr( &spandoc, spanEsr );

	//создаем два стыка с участками на перегоне
	pair <CLogicElement *, CLogicElement *> fstrips;
	BuildJoint( fstrips, nullptr, &spandoc );
	pair <CLogicElement *, CLogicElement *> sstrips( fstrips.second, 0 );
	BuildJoint( sstrips, nullptr, &spandoc );

	//на перегоне создаем стык на границе между перегоном и станцией
	CLogicElement * statStrip = BuildObject( STRIP, false );
	plom->AddObject( statStrip, logdoc.get() );
	CLogicElement * statJoint = BuildObject( JOINT, false );
	plom->AddObject( statJoint, logdoc.get() );
	const CLink & jointLink = statJoint->BuildLink( statEsr );
	plim->Join( statStrip, jointLink );
	plim->Join( sstrips.second, jointLink );

	const set <CLogicElement *> & depth_strips = plim->GetDepthLinks( fstrips.first, 
		( ELEMENT_CLASS )( STRIP | JOINT ), STRIP, -1, spanEsr );
	CPPUNIT_ASSERT( depth_strips.size() ==  2 );
}

void TC_LinkManager::StripRouteUnlinking()
{
	Editor <LTraits> * leditor = logdoc->GetEditor();
	leditor->OpenPool( logdoc.get() );
	CreateTestRoute();
	set <CLogicElement *> lstrips;
	plom->GetObjects( STRIP, logdoc.get(), lstrips );
	CLogicElement * unlinked_strip = 0;
	for( CLogicElement * lstrip : lstrips )
	{
		const vector <CLink> & route_links = lstrip->GetLinks( ROUTE );
		if ( !route_links.empty() )
		{
			unlinked_strip = lstrip;
			break;
		}
	}
	CPPUNIT_ASSERT( unlinked_strip );
	plim->UnlinkFromAll( unlinked_strip );
	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 1 );
	const vector <CLink> & route_links = (*lroutes.begin())->GetLinks();
	CPPUNIT_ASSERT( route_links.empty() );
}

void TC_LinkManager::StripSemaUnlinking()
{
	Editor <LTraits> * leditor = logdoc->GetEditor();
	leditor->OpenPool( logdoc.get() );
	pair <CLogicElement *, CLogicElement *> strips;
	CLogicElement * lleg = BuildLeg( TRAIN );
	BuildJoint( strips, lleg );
	const vector <CLink> & init_semalinks = lleg->GetLinks();
	CPPUNIT_ASSERT( init_semalinks.size() == 3 ); //голова + два участка
	CPPUNIT_ASSERT( strips.first && strips.second );
	plim->UnlinkFromAll( strips.first );
	const vector <CLink> & semalinks = lleg->GetLinks();
	CPPUNIT_ASSERT( semalinks.size() == 1 );
}

void TC_LinkManager::CommutatorRouteUnlinking()
{
	Editor <LTraits> * leditor = logdoc->GetEditor();
	leditor->OpenPool( logdoc.get() );
	CLogicElement * lleg = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> strips1;
	CLogicElement * ljoint1 = BuildJoint( strips1, lleg );
	pair <CLogicElement *, CLogicElement *> strips2( strips1.second, 0 );
	CLogicElement * ljoint2 = BuildJoint( strips1, lleg );

	//генерация маршрута
	CPPUNIT_ASSERT( plom->GenerateRoutes( logdoc.get() ) );
	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 1 );
	const EsrKit & esrCode = logdoc->GetStationManager()->GetESR( logdoc.get() );
	const CLogicElement * lroute = (*lroutes.begin());
	const CLink & routeLink = lroute->BuildLink( esrCode );

	//связывание с коммутатором
	CLogicElement * lcomm = BuildObject( COMMUTATOR );
	const CLink & jlink1 = ljoint1->BuildLink( esrCode, CLink::RED );
	const CLink & jlink2 = ljoint2->BuildLink( esrCode, CLink::BLUE );
	plim->Join( lcomm, jlink1 );
	plim->Join( lcomm, jlink2 );
	plim->Join( lcomm, routeLink );

	vector <CLink> commlinks = lcomm->GetLinks();
	CPPUNIT_ASSERT( commlinks.size() == 3 );

	plim->UnlinkFromAll( lcomm, ROUTE );
	commlinks = lcomm->GetLinks();
	CPPUNIT_ASSERT( commlinks.size() == 2 );
}

void TC_LinkManager::SwitchStripUndefLinking()
{
	Editor <LTraits> * leditor = logdoc->GetEditor();
	leditor->OpenPool( logdoc.get() );
	CLogicElement * base_strip = 0, * plus_strip = 0, * minus_strip = 0;
	CLogicElement * lswitch = TC_Logic::BuildSwitchKit( base_strip, plus_strip, minus_strip );
	CLink tstLink( STRIP, BadgeE( L"tst", EsrKit( 1 ) ), CLink::UNDEF );
	CPPUNIT_ASSERT( !plim->CanBeLinkedWith( lswitch, tstLink ) );
}

void TC_LinkManager::LinksDeficitForHeads()
{
	//создание светофора с множеством голов различных типов:
	CLogicElement * lleg = CLogicElementFactory().CreateObject( SEMALEG );
	lleg->SetName( GenerateName( L"semaleg" ).c_str() );
	plom->AddObject( lleg, logdoc.get() );
	for ( HEAD_TYPE htype = TRAIN; htype != HEAD_TYPE::INVALID_HEAD_TYPE; htype = ( HEAD_TYPE ) ( htype + 1 ) )
	{
		CLogicElement * lhead = CLogicElementFactory().CreateObject( HEAD );
		lhead->SetType( htype );
		lhead->SetName( GenerateName( L"head" ).c_str() );
		plom->AddObject( lhead, logdoc.get() );
		plim->Join( lleg, lhead->BuildLink( EsrCode( logdoc.get() ) ) );
	}

	//проверка голов на "дефицитность" линков
	set <const CLogicElement *> lheads;
	plom->GetObjects( HEAD, logdoc.get(), lheads );
	CPPUNIT_ASSERT( !lheads.empty() );
	for ( auto loghead : lheads )
	{
		auto deflinksCount = plim->LinksDeficit( loghead );
		if ( loghead->GetType() == HEAD_TYPE::TRAIN || loghead->GetType() == HEAD_TYPE::SHUNTING )
			CPPUNIT_ASSERT( deflinksCount == 1 );
		else
			CPPUNIT_ASSERT( deflinksCount == 0 );
	}
}

void TC_LinkManager::StripRouteLinking()
{
	CLogicElement * lstrip = BuildObject( STRIP );
	CLogicElement * lroute = BuildObject( ROUTE );
	CLinkManager * plim = logdoc->GetLinkManager();
	const CLink & routeLink = lroute->BuildLink( EsrCode() );
	plim->Join( lstrip, routeLink );

	auto stripLinks = lstrip->GetLinks();
	CPPUNIT_ASSERT( stripLinks.size() == 1 );
	CPPUNIT_ASSERT( stripLinks[0] == routeLink );

	plim->UnlinkFromAll( lstrip );
	auto stripLinks2 = lstrip->GetLinks();
	CPPUNIT_ASSERT( stripLinks2.empty() );
}

void TC_LinkManager::StripWithStripPossibility()
{
	CLogicElement * lstrip1 = BuildObject( STRIP );
	CLogicElement * lstrip2 = BuildObject( STRIP );
	const auto & stripLink2 = lstrip2->BuildLink( EsrCode() );
	CPPUNIT_ASSERT( plim->CanBeLinkedWith( lstrip1, stripLink2 ) );
}