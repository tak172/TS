#include "stdafx.h"
#include "TrackerInfrastructure.h"
#include "../StatBuilder/LogicElement.h"
#include "../Fund/RouteElementState.h"
#include "../Fund/RouteElementEvent.h"
#include "../helpful/Serialization.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////

ALLOCATE_SINGLETON(TrackerInfrastructure);

TrackerInfrastructure::TrackerInfrastructure()
    : techStatCode( 66000 ), axis_name( L"reference_system" ), 
    nsiBasisPtr( std::make_shared<TestGuessNsiBasis>() ),
	tfieldPtr( new TrainField( TrackingConfiguration().defSearchPathDepth, nsiBasisPtr ) ), tracker( tfieldPtr, TrackingConfiguration() ), exceptionCatched( false ), 
	fictivePart( axis_name )
{
	curmoment = time( NULL );
	CreateStation( *tfieldPtr, true );
	tracker.Reset();
}

TrackerInfrastructure::~TrackerInfrastructure()
{
	ASSERT( cachedObjects.empty() );
    HemEventRegistry::Shutdowner();
}

TestElemPtr InfraPart::Get( wstring objname ) const
{
	TestElemPtr retElemPtr;
	TstElmap::const_iterator teIt = tstobjects.find( objname );
	if ( teIt != tstobjects.end() )
		retElemPtr.reset( new TestElem( teIt->second ) );
	return retElemPtr;
}

TestElem TrackerInfrastructure::Get( wstring objname ) const
{
	TestElem testElem( NONE, BadgeE(), true, 0 );
	if ( auto testElemPtr = static_cast<const InfraPart &>( fictivePart ).Get( objname ) )
		testElem = *testElemPtr;
	else if ( auto testElemPtr = static_cast<const InfraPart &>( realPart ).Get( objname ) )
		testElem = *testElemPtr;
	return testElem;
}

void TrackerInfrastructure::CreateStation( TrainField & TF, bool directly ) const
{
	//файл картинки станции - \potap\Charm\Рабочая документация\Отслеживание\Тестовый полигон.vsd
	attic::a_document testdoc;
	attic::a_node root_node = testdoc.set_document_element( f2g_xAttr );
	attic::a_node init_node = root_node.append_child( Init_xAttr );

	auto fillPart = [this, &init_node]( const InfraPart & infraPart, bool directly ){
		const auto & tstobjects = infraPart.GetAllObjects();
		vector<TestElem> elemVec;
		elemVec.reserve( tstobjects.size() );
		for( StrElPair elpair : tstobjects )
			elemVec.push_back( elpair.second );
		if ( !directly )
			reverse( elemVec.begin(), elemVec.end() );

		for( const TestElem & telem : elemVec )
		{
			const auto & esr_code = telem.bdg.num();
			auto hold_ts = [&esr_code]( const CLogicElement * lelem ){
				auto ltsUsed = lelem->getLts( "used" );
				return !ltsUsed.empty();
			};
			telem.lePtr->FillForGuess( init_node, telem.bdg.num(), hold_ts, MakeCoInfo( telem ), telem.intoOneWaySpan );
		}

		const auto & spanKits = infraPart.GetAllSpans();
		for ( auto spanKitPair : spanKits )
		{
			const auto & ecode = spanKitPair.first;
			auto spanKitPtr = spanKitPair.second;
			ASSERT( spanKitPtr );
			if ( spanKitPtr )
				spanKitPtr->FillForGuess( init_node, ecode, spanKitPtr->GetUserDefinedIncOddness() );
		}
	};
	fillPart( fictivePart, directly );
	fillPart( realPart, directly );
	TF.Fill( init_node );
}

map <BadgeE, unsigned> TrackerInfrastructure::MakeCoInfo( const TestElem & telem ) const
{
	map <BadgeE, unsigned> linksCoInfo;
	if ( telem.lePtr->GetClass() != ROUTE )
	{
		const auto & links = telem.lePtr->GetLinks();
		for ( const CLink & link : links )
		{
			const TestElem & linkedTElem = Get( link.obj_badge.str() );
			linksCoInfo[linkedTElem.bdg] = linkedTElem.lePtr->GetType();
		}
	}
	else
	{
		const auto & headLinks = telem.lePtr->GetLinks( HEAD );
		ASSERT( headLinks.size() == 1 );
		const auto & headElem = Get( headLinks.front().obj_badge.str() );
		const auto & legLinks = headElem.lePtr->GetLinks( SEMALEG );
		ASSERT( legLinks.size() == 1 );
		const auto & legElem = Get( legLinks.front().obj_badge.str() );
		const auto & stripLinks = legElem.lePtr->GetLinks( STRIP, CLink::FROM );
		ASSERT( stripLinks.size() == 1 );
		linksCoInfo.insert( make_pair( stripLinks.front().obj_badge, 0 ) );
	}
	return linksCoInfo;
}

void TrackerInfrastructure::GenerateXmlDoc( attic::a_document & xml_doc, bool isRef, const vector<TestElem> & testVec ) const
{
	attic::a_node f2g_node = xml_doc.set_document_element( f2g_xAttr );
	attic::a_node record_node = ( isRef ) ? f2g_node.append_child( refsection_xAttr ) : 
		f2g_node.append_child( incsection_xAttr );
	record_node.ensure_attribute( stamp_xAttr ).set_value( curmoment );

	for( const TestElem & telem : testVec )
	{
		attic::a_node los_node = record_node.append_child( los_xAttr );
		los_node.ensure_attribute( bdg_xAttr ).set_value( telem.bdg.to_string() );
		los_node.ensure_attribute( class_xAttr ).set_value( telem.lePtr->GetClass() );
		telem.Los()->write_to_xml( los_node );
	}
	//xml_doc.save_file( L"C:\\genXmlDoc.txt" );
}

time_t TrackerInfrastructure::DetermineStripBusy( wstring strip_name, TestTracker & tracker, bool instantly )
{
	TestElem & telem = Get( strip_name );
	telem.Los()->setUsed( StateLTS::ACTIVE );
	DetermineSingle( strip_name, instantly, tracker );
	return curmoment;
}

time_t TrackerInfrastructure::DetermineStripFree( wstring strip_name, TestTracker & tracker, bool instantly, unsigned int shiftSec )
{
	TestElem & telem = Get( strip_name );
	telem.Los()->setUsed( StateLTS::PASSIVE );
	DetermineSingle( strip_name, instantly, tracker, shiftSec );
	return curmoment;
}

void TrackerInfrastructure::DetermineStripLocked( wstring strip_name, TestTracker & tracker, bool instantly )
{
	TestElem & telem = Get( strip_name );
	telem.Los()->setLock( StateLTS::ACTIVE );
	DetermineSingle( strip_name, instantly, tracker );
}

void TrackerInfrastructure::DetermineStripUnlocked( wstring strip_name, TestTracker & tracker, bool instantly )
{
	TestElem & telem = Get( strip_name );
	telem.Los()->setLock( StateLTS::PASSIVE );
	DetermineSingle( strip_name, instantly, tracker );
}

void TrackerInfrastructure::DetermineSwitchPlus( wstring switch_name, TestTracker & tracker, bool instantly )
{
	TestElem & telem = Get( switch_name );
	telem.Los()->setPlus( StateLTS::ACTIVE );
	telem.Los()->setMinus( StateLTS::PASSIVE );
	DetermineSingle( switch_name, instantly, tracker );
}

void TrackerInfrastructure::DetermineSwitchMinus( wstring switch_name, TestTracker & tracker, bool instantly )
{
	TestElem & telem = Get( switch_name );
	telem.Los()->setPlus( StateLTS::PASSIVE );
	telem.Los()->setMinus( StateLTS::ACTIVE );
	DetermineSingle( switch_name, instantly, tracker );
}

void TrackerInfrastructure::DetermineSwitchOutOfControl( wstring switch_name, TestTracker & tracker, bool instantly )
{
	TestElem & telem = Get( switch_name );
	telem.Los()->setPlus( StateLTS::PASSIVE );
	telem.Los()->setMinus( StateLTS::PASSIVE );
	DetermineSingle( switch_name, instantly, tracker );
}

void TrackerInfrastructure::DetermineHeadOpen( wstring head_name, TestTracker & tracker, bool instantly )
{
	TestElem & telem = Get( head_name );
	telem.Los()->setOpenSignal( StateLTS::ACTIVE );
	telem.Los()->setCloseSignal( StateLTS::PASSIVE );
	DetermineSingle( head_name, instantly, tracker );
}

void TrackerInfrastructure::DetermineHeadClose( wstring head_name, TestTracker & tracker, bool instantly )
{
	TestElem & telem = Get( head_name );
	telem.Los()->setOpenSignal( StateLTS::PASSIVE );
	telem.Los()->setCloseSignal( StateLTS::ACTIVE );
	DetermineSingle( head_name, instantly, tracker );
}

time_t TrackerInfrastructure::DetermineRouteSet( wstring route_name, TestTracker & tracker, bool instantly )
{
	TestElem & routeTelem = Get( route_name );
	const auto & rlinks = routeTelem.lePtr->GetLinks();
	for ( const auto & rlink : rlinks )
	{
		switch( rlink.object_class )
		{
		case HEAD:
			{
				DetermineHeadOpen( rlink.obj_badge.str(), tracker, instantly );
				break;
			}
		case STRIP:
			{
				DetermineStripLocked( rlink.obj_badge.str(), tracker, instantly );
				break;
			}
		}
	}
	DetermineSingle( route_name, instantly, tracker );
	return curmoment;
}

void TrackerInfrastructure::DetermineRouteCutting( wstring route_name, TestTracker & tracker, bool instantly )
{
	TestElem & routeTelem = Get( route_name );
	const auto & rlinks = routeTelem.lePtr->GetLinks();
	for ( const auto & rlink : rlinks )
	{
		switch( rlink.object_class )
		{
		case HEAD:
			{
				DetermineHeadClose( rlink.obj_badge.str(), tracker, instantly );
				break;
			}
		case STRIP:
			{
				DetermineStripLocked( rlink.obj_badge.str(), tracker, instantly );
				break;
			}
		}
	}
	DetermineSingle( route_name, instantly, tracker );
}

void TrackerInfrastructure::DetermineRouteUnset( wstring route_name, TestTracker &, bool instantly )
{
	TestElem & routeTelem = Get( route_name );
	const auto & rlinks = routeTelem.lePtr->GetLinks();
	for ( const auto & rlink : rlinks )
	{
		switch( rlink.object_class )
		{
		case HEAD:
			{
				DetermineHeadClose( rlink.obj_badge.str(), tracker, instantly );
				break;
			}
		case STRIP:
			{
				DetermineStripUnlocked( rlink.obj_badge.str(), tracker, instantly );
				break;
			}
		}
	}
	DetermineSingle( route_name, instantly, tracker );
}

time_t TrackerInfrastructure::DetermineSingle( wstring obj_name, bool instantly, TestTracker & tracker, unsigned int shiftSec )
{
	if ( instantly )
		DetermineBatch( list<wstring>( 1, obj_name ), false, tracker, shiftSec );
	else
		cachedObjects.push_back( obj_name );
	return curmoment;
}

void TrackerInfrastructure::DetermineBatch( const list<wstring> & testVec, bool reference, TestTracker & tracker, unsigned int shiftSec )
{
	//некоторые незапланированные события кидают исключения
	try
	{
		curmoment += shiftSec;

		vector<TestElem> determVector;
		determVector.reserve( testVec.size() );
		for( const wstring & objname : testVec )
		{
			if ( !objname.empty() )
			{
				TestElem tst_elem = Get( objname );
				determVector.push_back( tst_elem );
			}
		}
		LosTupleList loseslist;
		for( const TestElem & telem : determVector )
		{
			LosTuple lostuple;
			get<2>( lostuple ) = telem.lePtr->GetClass();
			get<1>( lostuple ) = telem.bdg;
			get<0>( lostuple ) = telem.Los();
			loseslist.push_back( lostuple );
		}
		UpdateRouteLoses( determVector );
		tracker.Track( reference, curmoment, move( loseslist ) );
	}
	catch( ... )
	{
		exceptionCatched = true;
	}
}

void TrackerInfrastructure::UpdateRouteLoses( const vector<TestElem> & determVector ) const
{
	vector<TestElem> routeElems;
	for_each( determVector.cbegin(), determVector.cend(), [&routeElems]( const TestElem & tstElem ){
		if ( tstElem.lePtr->GetClass() == ROUTE )
			routeElems.push_back( tstElem );
	} );
	for ( const TestElem & routeElem : routeElems )
	{
		const auto & rlinks = routeElem.lePtr->GetLinks();
		vector<RouteElementEvent> reeVec;
		for ( const auto & rlink : rlinks )
		{
			const auto & partelem = Get( rlink.obj_badge.str() );
			RouteElementState elState( rlink.object_class );
			elState.saveLOS( partelem.Los().get() );
			reeVec.push_back( RouteElementEvent( partelem.lePtr.get(), partelem.Los().get(), rlink.obj_badge ) );
		}
		routeElem.Los()->updateRouteStatus( &reeVec[0], rlinks.size() );
		routeElem.Los()->processRouteEvents( set<BadgeE>() );
	}
}

time_t TrackerInfrastructure::FlushData( TestTracker & tracker, bool parcel_type, unsigned int shiftSec )
{
	DetermineBatch( cachedObjects, parcel_type, tracker, shiftSec );
	cachedObjects.clear();
	return curmoment;
}

void TrackerInfrastructure::Reset()
{
	tracker.Reset();
	fictivePart.Reset();
	realPart.Reset();
	exceptionCatched = false;
}

void TrackerInfrastructure::ImitateMotion( vector <wstring> stripsSequence, TestTracker & tracker )
{
	if ( !stripsSequence.empty() )
	{
		DetermineStripBusy( stripsSequence[0], tracker );
		for ( unsigned int seqInd = 1; seqInd < stripsSequence.size(); ++seqInd )
		{
			DetermineStripBusy( stripsSequence[seqInd], tracker ); //выезд на следующий
			DetermineStripFree( stripsSequence[seqInd-1], tracker ); //гашение предыдущего
		}
	}
}

void TrackerInfrastructure::DoActivities( const vector<TIActivity> & activities, TestTracker & trackRef )
{
	for ( auto tiActitvity : activities )
	{
		ASSERT( tiActitvity );
		if ( tiActitvity )
			tiActitvity( *this, trackRef );
	}
}

time_t TrackerInfrastructure::IncreaseTime( unsigned int incdeltaMs )
{
	curmoment += ( incdeltaMs / 1000 );
	DetermineSingle( L"", true, tracker ); //пустая посылка для обновления временнОго штампа
	return curmoment;
}