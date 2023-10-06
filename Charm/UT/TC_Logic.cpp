#include "stdafx.h"
#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/TrainDescr.h"
#include "TC_Logic.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/LinkManager.h"
#include "../Guess/Msg.h"
#include "../StatBuilder/RegionDescription.h"

using namespace std;

void TC_Logic::setUp_helper()
{
	logdoc.reset( new CLogicDocumentTest );
	plim = logdoc->GetLinkManager();
	pstam = logdoc->GetStationManager();
	SetStationEsr( logdoc.get(), EsrKit( 20100 ) );
	plom = logdoc->GetLObjectManager();
}

void TC_Logic::tearDown_helper()
{
    used_names.clear();
    plom = nullptr;
    pstam = nullptr;
    plim = nullptr;
    logdoc.reset();
}

CLogicElement * TC_Logic::BuildObject( ELEMENT_CLASS  elclass, bool add_to_doc, CLogicDocument * ldoc )
{
	CLogicDocument * aimdoc = ( ldoc == 0 ) ? logdoc.get() : ldoc;
	CLogicElement * lobject = CLogicElementFactory().CreateObject( elclass );
	lobject->SetName( GenerateName( L"object" ).c_str() );
	if ( add_to_doc )
		plom->AddObject( lobject, aimdoc );
	return lobject;
}

CLogicElement * TC_Logic::BuildLeg( HEAD_TYPE headType, CLogicDocument * ldoc )
{
	vector <HEAD_TYPE> htypes;
	htypes.push_back( headType );
	return BuildLeg( htypes, ldoc );
}

CLogicElement * TC_Logic::BuildLeg( const vector <HEAD_TYPE> & htypes, CLogicDocument * ldoc )
{
	ASSERT( !htypes.empty() );
	CLogicDocument * aimdoc = ( ldoc == 0 ) ? logdoc.get() : ldoc;
	CLinkManager * plim = aimdoc->GetLinkManager();

	//мачта
	CLogicElement * lleg = CLogicElementFactory().CreateObject( SEMALEG );
	lleg->SetName( GenerateName( L"semaleg" ).c_str() );
	CLObjectManager * doclm = aimdoc->GetLObjectManager();
	doclm->AddObject( lleg, aimdoc );

	//головы
	for ( auto htype : htypes )
	{
		CLogicElement * lhead = CLogicElementFactory().CreateObject( HEAD );
		lhead->SetType( htype );
		lhead->SetName( GenerateName( L"head" ).c_str() );
		doclm->AddObject( lhead, aimdoc );
		plim->Join( lleg, lhead->BuildLink( EsrCode( aimdoc ) ) );
	}
	return lleg;
}

CLogicElement * TC_Logic::BuildJoint( pair <CLogicElement *, CLogicElement *> & strips_near, CLogicElement * lleg, CLogicDocument * ldoc )
{
	CLogicDocument * aimdoc = ( ldoc == 0 ) ? logdoc.get() : ldoc;
	CStationManager * aimpstam = ( ldoc == 0 ) ? pstam : ldoc->GetStationManager();
	//при необходимости создаются участки около стыка. но не мачта светофора.
	CLogicElement *& from_strip = strips_near.first;
	if ( !from_strip )
	{
		from_strip = CLogicElementFactory().CreateObject( STRIP );
		from_strip->SetName( GenerateName( L"strip" ).c_str() );
		plom->AddObject( from_strip, aimdoc );
	}
	CLogicElement *& to_strip = strips_near.second;
	if ( !to_strip )
	{
		to_strip = CLogicElementFactory().CreateObject( STRIP );
		to_strip->SetName( GenerateName( L"strip" ).c_str() );
		plom->AddObject( to_strip, aimdoc );
	}
	CLogicElement * ljoint = CLogicElementFactory().CreateObject( JOINT );
	ljoint->SetName( GenerateName( L"joint" ).c_str() );
	plom->AddObject( ljoint, aimdoc );
	const CLink & strip_link_from = from_strip->BuildLink( aimpstam->GetESR( from_strip ), CLink::FROM );
	const CLink & strip_link_to = to_strip->BuildLink( aimpstam->GetESR( to_strip ), CLink::TO );
	plim->Join( ljoint, strip_link_from );
	plim->Join( ljoint, strip_link_to );
	LinkLeg( lleg, strips_near );
	return ljoint;
}

void TC_Logic::LinkLeg( CLogicElement * lleg, const pair <CLogicElement *, CLogicElement *> & strips_near ) const
{
	if ( lleg )
	{
		const CLink & strip_link_from = strips_near.first->BuildLink( pstam->GetESR( strips_near.first ), CLink::FROM );
		const CLink & strip_link_to = strips_near.second->BuildLink( pstam->GetESR( strips_near.second ), CLink::TO );
		plim->Join( lleg, strip_link_from );
		plim->Join( lleg, strip_link_to );
	}
}

wstring TC_Logic::GenerateName( wstring initial_name )
{
	wstring retname = initial_name;
	unsigned int num = 1;
	while( used_names.find( retname ) != used_names.end() )
	{
		retname = initial_name;
		wostringstream wostream;
		wostream << retname << ++num;
		retname = wostream.str();
	}
	used_names.insert( retname );
	return retname;
}

EsrKit TC_Logic::EsrCode( CLogicDocument * ldoc ) const
{
	CLogicDocument * aimdoc = ( ldoc == 0 ? logdoc.get() : ldoc );
	CStationManager * pstam = aimdoc->GetStationManager();
	return pstam->GetESR( aimdoc );
}

CLogicElement * TC_Logic::BuildSwitchKit( CLogicElement *& base_strip, CLogicElement *& plus_strip, 
		CLogicElement *& minus_strip, CLogicDocument * ldoc )
{
	CLogicDocument * aimdoc = ( ldoc == 0 ) ? logdoc.get() : ldoc;
	CLogicElement * lswitch = BuildObject( SWITCH, true, aimdoc );
	if ( !base_strip )
		base_strip = BuildObject( STRIP, true, aimdoc );
	if ( !plus_strip )
		plus_strip = BuildObject( STRIP, true, aimdoc );
	if ( !minus_strip )
		minus_strip = BuildObject( STRIP, true, aimdoc );
	plim->Join( lswitch, base_strip->BuildLink( EsrCode(), CLink::BASE ) );
	plim->Join( lswitch, plus_strip->BuildLink( EsrCode(), CLink::PLUS ) );
	plim->Join( lswitch, minus_strip->BuildLink( EsrCode(), CLink::MINUS ) );
	return lswitch;
}

CLogicElement * TC_Logic::CreateTestRoute()
{
	CLogicElement * base_strip = 0, * plus_strip = 0, * minus_strip = 0;
	CLogicElement * lswitch = TC_Logic::BuildSwitchKit( base_strip, plus_strip, minus_strip );
	CPPUNIT_ASSERT( lswitch );
	vector <HEAD_TYPE> htypes;
	htypes.push_back( SHUNTING );
	CLogicElement * lleg = BuildLeg( htypes );
	pair <CLogicElement *, CLogicElement *> strips1( 0, base_strip );
	BuildJoint( strips1, lleg );
	pair <CLogicElement *, CLogicElement *> strips2( plus_strip, 0 );
	BuildJoint( strips2 );
	set <CLogicElement  *> initlroutes;
	plom->GetObjects( ROUTE, logdoc.get(), initlroutes );
	plom->GenerateRoutes( logdoc.get() );
	set <CLogicElement *> genlroutes;
	plom->GetObjects( ROUTE, logdoc.get(), genlroutes );
	set <CLogicElement *> newroutes;
	set_difference( genlroutes.cbegin(), genlroutes.cend(), initlroutes.cbegin(), initlroutes.cend(),
		inserter( newroutes, newroutes.cend() ) );
	CPPUNIT_ASSERT( newroutes.size() == 1 );
	return *newroutes.cbegin();
}

void TC_Logic::MakeTrainNode( attic::a_node infoNode, CLogicElement * lstrip, bool presence )
{
	auto tplaceNode = infoNode.append_child( train_place_xAttr );
	const EsrKit & ecode = pstam->GetESR( lstrip );
	const CLink & stripLink = lstrip->BuildLink( ecode );
	const BadgeE & stripBdg = stripLink.obj_badge;
	tplaceNode.append_attribute( bdg_xAttr ).set_value( stripBdg.to_wstring() );
	tplaceNode.append_attribute( presence_xAttr ).set_value( presence ); //признак присутствия поезда
}

pair <attic::a_node, attic::a_node> TC_Logic::CreateInfoDoc( attic::a_document & numdoc, const TrainDescr & tdescr, unsigned int trainId ) const
{
	auto rootNode = numdoc.set_document_element( "numbers" );
	auto infoNode = rootNode.append_child( info_xAttr );
	infoNode.append_attribute( id_xAttr ).set_value( trainId );
	auto trainNode = infoNode.append_child( train_xAttr );
	trainNode << tdescr;
	return make_pair (rootNode, infoNode );
}

void TC_Logic::SetStationEsr( CLogicDocument * statdoc, const EsrKit & statCode )
{
	ASSERT( statCode.terminal() );
	auto pstam = statdoc->GetStationManager();
	pstam->SetDescription<StationDescription>( StationDescription( L"StatName", statCode, PicketingInfo(), StationDescription::Options() ), statdoc );
}

void TC_Logic::SetSpanEsr( CLogicDocument * spandoc, const EsrKit & spanCode )
{
	ASSERT( spanCode.span() );
	auto pstam = spandoc->GetStationManager();
	pstam->SetDescription<SpanDescription>( SpanDescription( L"SpanName", spanCode, PicketingInfo(), WaysLocks( map<unsigned int, SpanLockType>() ), 
		OddnessPtr( new Oddness( Oddness::UNDEF ) ), 0 ), spandoc );
}