#include "stdafx.h"
#include "TC_LObjectManager.h"
#include "LogicDocumentTest.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/LinkManager.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/LogicElement.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LObjectManager );

void TC_LObjectManager::GenerateRouteAlienLastJoint()
{
	//начальное состояние
	CLObjectManager * plom = projdoc->GetLObjectManager();
	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.empty() );

	//включение перегонного документа
	CLogicDocumentTest spandoc;
	CStationManager * pstam = projdoc->GetStationManager();
	projdoc->Include( &spandoc );
	EsrKit spanEsr( 1, 2 );
	SetSpanEsr( &spandoc, spanEsr );

	//построение маршрута со стыком на перегоне
	CLogicElement * start_leg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> leg_strips;
	BuildJoint( leg_strips, start_leg );
	pair <CLogicElement *, CLogicElement *> last_joint_strips( leg_strips.second, 0 );
	CLogicElement * final_leg = BuildLeg( TRAIN );
	CLogicElement * last_joint = BuildJoint( last_joint_strips, final_leg, &spandoc );
	plom->GenerateRoutes( logdoc.get() );
	
	//проверка
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 1 );
	const CLogicElement * lroute = *lroutes.begin();
	const vector <CLink> & routeLinks = lroute->GetLinks();
	const CLink & lastJointLink = routeLinks.back();
	CPPUNIT_ASSERT( lastJointLink.obj_badge == BadgeE( last_joint->GetName(), spanEsr ) );
}

void TC_LObjectManager::JointNameChanging()
{
	pair <CLogicElement *, CLogicElement *> lstrips;
	CLogicElement * ljoint = BuildJoint( lstrips );
	plom->ChangeObjectName( lstrips.first, L"ПП", logdoc.get(), EsrCode() );
	plom->ChangeObjectName( lstrips.second, L"4П", logdoc.get(), EsrCode() );
	CPPUNIT_ASSERT( ljoint->GetName() == L"4П~ПП" );
}

void TC_LObjectManager::JointConflictNameChanging()
{
	CLogicDocumentTest ldoc1, ldoc2;
	projdoc->Include( &ldoc1 );
	projdoc->Include( &ldoc2 );
	CStationManager * pstam = projdoc->GetStationManager();
	EsrKit ecode1( 1 ), ecode2( 2 );
	SetStationEsr( &ldoc1, ecode1 );
	SetStationEsr( &ldoc2, ecode2 );
	CLogicElement * lstrip1 = BuildObject( STRIP, true, &ldoc1 );
	CLogicElement * midstrip = BuildObject( STRIP );
	CLogicElement * lstrip2 = BuildObject( STRIP, true, &ldoc2 );
	CLogicElement * ljoint1 = BuildJoint( make_pair( lstrip1, midstrip ) );
	CLogicElement * ljoint2 = BuildJoint( make_pair( midstrip, lstrip2 ) );
	CLObjectManager * plom = projdoc->GetLObjectManager();
	plom->ChangeObjectName( lstrip1, L"1П", &ldoc1, EsrKit( 1 ) );
	plom->ChangeObjectName( midstrip, L"2П", logdoc.get(), EsrCode() );
	plom->ChangeObjectName( lstrip2, L"1П", &ldoc2, EsrKit( 2 ) );
	CPPUNIT_ASSERT( ljoint1->GetName() == L"1П~2П" );
	CPPUNIT_ASSERT( ljoint2->GetName() == L"1П~2П#2" );
}

void TC_LObjectManager::ChangeStripNameToSame()
{
	pair <CLogicElement *, CLogicElement *> jstrips;
	const CLogicElement * ljoint = BuildJoint( jstrips );
	wstring init_jname = ljoint->GetName();
	plom->ChangeObjectName( jstrips.first, jstrips.first->GetName(), logdoc.get(), EsrCode() );
	CPPUNIT_ASSERT( init_jname == ljoint->GetName() );
}

void TC_LObjectManager::RouteUndependencyToCommutatorRemove()
{
	projdoc->OpenAllPools();
	CLogicElement * lroute = CreateTestRoute();
	CLogicElement * lcommutator = BuildObject( COMMUTATOR, true, logdoc.get() );
	const auto & esrCode = pstam->GetESR( lcommutator );
	const auto & commutLink = lcommutator->BuildLink( esrCode );
	CPPUNIT_ASSERT( plim->Join( lroute, commutLink ) );
	plom->RemoveObject( lcommutator, logdoc.get() );
	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 1 );
}

void TC_LObjectManager::RouteDependencyToAlienStrip()
{
	CLObjectManager * plom = projdoc->GetLObjectManager();
	CLogicDocumentTest alienDoc;
	projdoc->Include<CLogicDocumentTest>( &alienDoc );
	projdoc->OpenAllPools();
	auto leditor = logdoc->GetEditor();
	EsrKit spanCode( EsrCode().getTerm(), EsrCode().getTerm() + 1 );
	SetSpanEsr( &alienDoc, spanCode );

	CLogicElement * lleg = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> semaStrips;
	CLogicElement * ljoint1 = BuildJoint( semaStrips, lleg );
	pair <CLogicElement *, CLogicElement *> finalStrips( semaStrips.second, nullptr );
	CLogicElement * ljoint2 = BuildJoint( finalStrips, nullptr, &alienDoc );
	plom->GenerateRoutes( logdoc.get() );
	leditor->StoreManipulations();
	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	plom->RemoveObject( finalStrips.second );
	leditor->StoreManipulations();

	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	set <const CLogicElement *> ljoints;
	plom->GetObjects( JOINT, logdoc.get(), ljoints );
	CPPUNIT_ASSERT( lroutes.empty() );
}

void TC_LObjectManager::RenameHeadIntoRoute()
{
	CLObjectManager * plom = projdoc->GetLObjectManager();
	projdoc->OpenAllPools();
	CLogicElement * lroute = CreateTestRoute();
	wstring initRouteName = lroute->GetName();

	//изменение имени головы, входящей в маршрут
	const auto & contentLinks = lroute->GetLinks();
	const auto & headLinks = lroute->GetLinks( HEAD );
	CPPUNIT_ASSERT( headLinks.size() == 1 );
	CLogicElement * lhead = plom->GetObjectByBadge<CLogicElement *>( headLinks[0].obj_badge );
	const auto oldHeadName = lhead->GetName();
	wstring newHeadName = oldHeadName + L"New";
	plom->ChangeObjectName( lhead, newHeadName, logdoc.get(), EsrCode() );
	CPPUNIT_ASSERT( lhead->GetName() == newHeadName );

	//должно измениться как имя маршрута, так и ссылки на него во всех элементах, входящих в него
	auto hnamePos = initRouteName.find( oldHeadName );
	CPPUNIT_ASSERT( hnamePos == 0 ); //имя маршрута начинается с имени головы
	wstring newRouteName = initRouteName;
	newRouteName.replace( hnamePos, oldHeadName.size(), newHeadName );
	CPPUNIT_ASSERT( lroute->GetName() == newRouteName );
	for ( const CLink & rlink : contentLinks )
	{
		const CLogicElement * linkle = plom->GetObjectByBadge<const CLogicElement *>( rlink.obj_badge );
		CPPUNIT_ASSERT( linkle );
		const auto & routeLinks = linkle->GetLinks( ROUTE );
		CPPUNIT_ASSERT( routeLinks.size() == 1 );
		const CLink & routeLink = routeLinks[0];
		CPPUNIT_ASSERT( routeLink.obj_badge.str() == newRouteName );
	}
}

void TC_LObjectManager::RenameRoutesCommonFinalStrip()
{
	CLObjectManager * plom = projdoc->GetLObjectManager();
	projdoc->OpenAllPools();
	vector <HEAD_TYPE> htypes;
	htypes.push_back( SHUNTING );
	auto lleg = BuildLeg( htypes );
	pair <CLogicElement *, CLogicElement *> strips;
	BuildJoint( strips, lleg );
	CLogicElement * midstrip1 = nullptr, * midstrip2 = nullptr;
	TC_Logic::BuildSwitchKit( strips.second, midstrip1, midstrip2 );
	CLogicElement * finalstrip = nullptr, * midstrip3 = nullptr;
	TC_Logic::BuildSwitchKit( finalstrip, midstrip1, midstrip3 );
	pair <CLogicElement *, CLogicElement *> midstrips( midstrip2, midstrip3 );
	BuildJoint( midstrips );
	pair <CLogicElement *, CLogicElement *> finalstrips( finalstrip, nullptr );
	BuildJoint( finalstrips );

	plom->GenerateRoutes( logdoc.get() );
	set <CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 2 );
	plom->ChangeObjectName( finalstrips.second, L"new_name", logdoc.get(), EsrCode() );
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 2 );
	CPPUNIT_ASSERT( ( *lroutes.cbegin() )->GetName() != ( *lroutes.crbegin() )->GetName() );
}

void TC_LObjectManager::InterstationRouteAndCommutator()
{
	CLogicDocumentTest aliendoc;
	projdoc->Include( &aliendoc );
	projdoc->OpenAllPools();
	const EsrKit & native_code = EsrCode();
	EsrKit alienCode( native_code.getTerm() + 1 );
	SetStationEsr( &aliendoc, alienCode );

	vector <HEAD_TYPE> htypes;
	htypes.push_back( TRAIN );
	auto srcleg = BuildLeg( htypes );
	pair <CLogicElement *, CLogicElement *> strips;
	BuildJoint( strips, srcleg );
	pair <CLogicElement *, CLogicElement *> midstrips( strips.second, nullptr );
	auto midjoint = BuildJoint( midstrips );
	auto destleg = BuildLeg( htypes );
	pair <CLogicElement *, CLogicElement *> strips2( midstrips.second, nullptr );
	auto destjoint = BuildJoint( strips2, destleg );
	auto lcomm = BuildObject( COMMUTATOR, true, &aliendoc );
	plim->Join( lcomm, midjoint->BuildLink( EsrCode(), CLink::BLUE ) );
	plim->Join( lcomm, destjoint->BuildLink( EsrCode(), CLink::RED ) );

	CLObjectManager * plom = projdoc->GetLObjectManager();
	projdoc->OpenAllPools();
	plom->GenerateRoutes( logdoc.get() );
	plim->CommutRouteLink( &aliendoc );
	set <CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 1 );
	auto lroute = *lroutes.cbegin();
	const auto & commlinks = lroute->GetLinks( COMMUTATOR );
	CPPUNIT_ASSERT( commlinks.size() == 1 );
}