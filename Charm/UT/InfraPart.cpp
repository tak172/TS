#include "stdafx.h"
#include "InfraPart.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicElement.h"
#include "../Fund/RouteElementState.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
TestElem::TestElem( ELEMENT_CLASS elclass, const BadgeE & elbadge, bool have_ts, unsigned int objtype, bool _intoOneWaySpan ) : 
	bdg( elbadge ), intoOneWaySpan( _intoOneWaySpan )
{
	lePtr.reset( CLogicElementFactory().CreateObject( elclass ) );
	if ( lePtr )
	{
		lePtr->SetName( elbadge.str() );
		lePtr->SetType( objtype );
		if ( have_ts )
			lePtr->setLts( "used", BadgeU( L"ts_name", 1 ) );
		losPtr = CreateLos( StateLTS::PASSIVE, StateLTS::PASSIVE, elclass );
	}
}

shared_ptr<LOS_base> TestElem::Los() const 
{ 
	return losPtr ? losPtr : shared_ptr<LOS_base>( LOS_base::fabric_from_object( NONE ) );
}

shared_ptr<LOS_base> TestElem::CreateLos( StateLTS firstLts, StateLTS secondLts, ELEMENT_CLASS elclass )
{
	shared_ptr<LOS_base> losPtr( LOS_base::fabric_from_object( elclass ) );
	switch( elclass )
	{
	case STRIP:
		losPtr->setUsed( firstLts );
		break;
	case SWITCH:
		losPtr->setPlus( firstLts );
		losPtr->setMinus( secondLts );
		break;
	case HEAD:
		losPtr->setOpenSignal( firstLts );
		losPtr->setCloseSignal( secondLts );
		break;
	}
	return losPtr;
}

void TestElem::Link( const CLink & link )
{
	vector <CLink> links = lePtr->GetLinks();
	size_t initsize = links.size();
	if ( find( links.begin(), links.end(), link ) == links.end() )
		links.push_back( link );
	if ( links.size() > initsize )
		lePtr->SetLinks( links );
}

//////////////////////////////////////////////////////////////////////////
void InfraPart::Insert( ELEMENT_CLASS elclass, BadgeE bdgarr[], unsigned int arr_size, bool have_ts, unsigned int objtype, bool one_way_span )
{
	for ( unsigned int k = 0; k < arr_size; ++k )
		tstobjects.insert( make_pair( bdgarr[k].str(), TestElem( elclass, bdgarr[k], have_ts, objtype, one_way_span ) ) );
}

void InfraPart::InsertRoute( const BadgeE & route, const vector<CLink> & routeLinks )
{
	ASSERT( !routeLinks.empty() );
	const auto & headBdg = routeLinks.front().obj_badge;
	auto headElemPtr = Get( headBdg.str() );
	ASSERT( headElemPtr );
	if ( headElemPtr )
	{
		auto routeType = headElemPtr->lePtr->GetType(); //тип маршрута совпадает с типом головы
		TestElem routeElem( ROUTE, route, false, routeType );
		tstobjects.insert( make_pair( route.str(), routeElem ) );
		Link( route.str(), routeLinks );
		const auto & lelinks = routeElem.lePtr->GetLinks();
		for ( unsigned int i = 0; i < lelinks.size(); ++i )
			routeElem.Los()->addRouteElementState( lelinks[i].obj_badge, RouteElementState( lelinks[i].object_class ), i );
	}
}

CLink InfraPart::FormLink( wstring elname, unsigned int link_type ) const
{
	CLink link;
	const auto & telemPtr = Get( elname );
	ASSERT( telemPtr );
	if ( telemPtr )
		link = CLink( telemPtr->lePtr->GetClass(), telemPtr->bdg, link_type );
	return link;
}

void InfraPart::Link( wstring objname, const CLink & link )
{
	auto telemPtr = Get( objname );
	ASSERT( telemPtr );
	if ( telemPtr )
		telemPtr->Link( link );

	//обратная линковка
	auto oppositeElemPtr = Get( link.obj_badge.str() );
	ASSERT( oppositeElemPtr );
	if ( oppositeElemPtr )
		oppositeElemPtr->Link( FormLink( objname, link.link_type ) );
}

void InfraPart::Link( wstring objname, const CLink & link1, const CLink & link2 )
{
	Link( objname, link1 );
	Link( objname, link2 );
}

void InfraPart::Link( wstring objname, const vector <CLink> & vlinks )
{
	for( const auto & link : vlinks )
		Link( objname, link );
}

void InfraPart::LinkSema( wstring legBdg, wstring fromStrip, wstring toStrip, const vector <wstring> & heads_of_leg )
{
	vector <CLink> links_of_leg;
	links_of_leg.push_back( FormLink( fromStrip, CLink::FROM ) );
	links_of_leg.push_back( FormLink( toStrip, CLink::TO ) );
	const CLink & legLink = FormLink( legBdg );
	for( const wstring & headname : heads_of_leg )
	{
		links_of_leg.push_back( FormLink( headname ) );
		Link( headname, legLink );
	}
	for( const CLink & link_of_leg : links_of_leg )
		Link( legBdg, link_of_leg );
}

void InfraPart::Reset()
{
	for ( const auto & tobjpair : tstobjects )
	{
		const auto & tstobj = tobjpair.second;
		tstobj.Los()->reset_signals();
	}
}