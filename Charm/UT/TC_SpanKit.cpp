#include "stdafx.h"
#include "TC_SpanKit.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/SpanKit.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/LogicSpanWay.h"
#include "../StatBuilder/LogicJoint.h"
#include "../StatBuilder/LinkManager.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/LogicSemaleg.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SpanKit );

using namespace std;

TC_SpanKit::TC_SpanKit() : pckname( wstring( L"picketage_name" ) )
{
}

void TC_SpanKit::OnlyStripsSpan()
{
	projdoc->OpenAllPools();
	SetSpanEsr( logdoc.get(), EsrKit( 13, 29 ) );
	logdoc->GetEditor()->StoreManipulations();

	//создаем участки именно в таком порядке, чтобы последний созданный был в середине
	CLogicElement * lstrip = BuildObject( STRIP );
	CLogicElement * lstrip2 = BuildObject( STRIP );
	CLogicElement * lstrip3 = BuildObject( STRIP );
	BuildJoint( make_pair( lstrip, lstrip3 ) );
	BuildJoint( make_pair( lstrip2, lstrip3 ) );
	logdoc->GetEditor()->StoreManipulations();

	SpanKitPtr spanKitPtr = plom->GetSpanKit( logdoc.get() );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 1 );
	const LogicSpanWay & span_way = ways.front();
	const list <const CLogicElement *> & lstrips = span_way.Strips();
	CPPUNIT_ASSERT( lstrips.size() == 3 );

	logdoc->GetEditor()->MoveState( false );
	spanKitPtr = plom->GetSpanKit( logdoc.get() );
	CPPUNIT_ASSERT( spanKitPtr );
	CPPUNIT_ASSERT( spanKitPtr->AllWays().empty() );
}

void TC_SpanKit::PicketageIncreasingStripStarting()
{
	projdoc->OpenAllPools();
	SetSpanEsr( logdoc.get(), EsrKit( 1, 2 ) );
	logdoc->GetEditor()->StoreManipulations();

	CLogicElement * start_strip = MakeSpanStripStarting( rwCoord( 10, 200 ), rwCoord( 20, 800 ) );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( logdoc.get() );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 1 );
	const LogicSpanWay & span_way = ways.front();
	const list <const CLogicElement *> & lstrips = span_way.Strips();
	if ( lstrips.front() == start_strip )
		CPPUNIT_ASSERT( span_way.PicketageIsIncreasing( pckname ) );
	else
		CPPUNIT_ASSERT( !span_way.PicketageIsIncreasing( pckname ) );
}

CLogicElement * TC_SpanKit::MakeSpanStripStarting( const rwCoord & first_joint_coord, const rwCoord & third_joint_coord )
{
	pair <CLogicElement *, CLogicElement *> strips1;
	CLogicElement * lelem1 = BuildJoint( strips1 );
	CLogicJoint * ljoint1 = static_cast <CLogicJoint *>( lelem1 );
	PicketingInfo pckinfo;
	pckinfo.SetCoord( rwRuledCoord(pckname, first_joint_coord) );
	ljoint1->SetPicketingInfo( pckinfo );
	pair <CLogicElement *, CLogicElement *> strips2( strips1.second, 0 );
	BuildJoint( strips2 );
	pair <CLogicElement *, CLogicElement *> strips3( strips2.second, 0 );
    CLogicElement * lelem3 = BuildJoint( strips3 );
    CLogicJoint * ljoint3 = static_cast <CLogicJoint *>( lelem3 );
	pckinfo.SetCoord( rwRuledCoord(pckname, third_joint_coord) );
	ljoint3->SetPicketingInfo( pckinfo );
	return strips1.first;
}

void TC_SpanKit::PicketageDecreasingStripStarting()
{
	projdoc->OpenAllPools();
	SetSpanEsr( logdoc.get(), EsrKit( 1, 2 ) );
	logdoc->GetEditor()->StoreManipulations();

	CLogicElement * start_strip = MakeSpanStripStarting( rwCoord( 35, 200 ), rwCoord( 17, 800 ) );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( logdoc.get() );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 1 );
	const LogicSpanWay & span_way = ways.front();
	const list <const CLogicElement *> & lstrips = span_way.Strips();
	if ( lstrips.front() == start_strip )
		CPPUNIT_ASSERT( !span_way.PicketageIsIncreasing( pckname ) );
	else
		CPPUNIT_ASSERT( span_way.PicketageIsIncreasing( pckname ) );
}

void TC_SpanKit::PicketageIncreasingJointStarting()
{
	projdoc->OpenAllPools();
	SetSpanEsr( logdoc.get(), EsrKit( 1, 2 ) );
	logdoc->GetEditor()->StoreManipulations();

	CLogicElement * start_strip = MakeSpanStripStarting( rwCoord( 10, 200 ), rwCoord( 20, 800 ) );
	CLogicElement * first_joint = BuildObject( JOINT );
	plim->Join( first_joint, start_strip->BuildLink( EsrKit( 1, 2 ) ) );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( logdoc.get() );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 1 );
	const LogicSpanWay & span_way = ways.front();
	const list <const CLogicElement *> & lstrips = span_way.Strips();
	if ( lstrips.front() == start_strip )
		CPPUNIT_ASSERT( span_way.PicketageIsIncreasing( pckname ) );
	else
		CPPUNIT_ASSERT( !span_way.PicketageIsIncreasing( pckname ) );
}

void TC_SpanKit::LegDirection()
{
	projdoc->OpenAllPools();
	SetSpanEsr( logdoc.get(), EsrKit( 1, 2 ) );
	logdoc->GetEditor()->StoreManipulations();

	CLogicElement * lleg1 = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> strips1;
	CLogicElement * lelem1 = BuildJoint( strips1, lleg1 );
	CLogicJoint * ljoint1 = static_cast <CLogicJoint *>( lelem1 );
	PicketingInfo pckinfo;
	wstring pckname = L"picketage_name";
	pckinfo.SetCoord( rwRuledCoord(pckname, rwCoord( 10, 200 )) );
	ljoint1->SetPicketingInfo( pckinfo );

	CLogicElement * lleg2 = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> strips2( strips1.second, 0 );
	CLogicElement * lelem2 = BuildJoint( strips2, lleg2 );
	CLogicJoint * ljoint2 = static_cast <CLogicJoint *>( lelem2 );
	pckinfo.SetCoord( rwRuledCoord(pckname, rwCoord( 15, 100 )) );
	ljoint2->SetPicketingInfo( pckinfo );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( logdoc.get() );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 1 );
	const LogicSpanWay & span_way = ways.front();
	bool leg_forward = false;
	CPPUNIT_ASSERT( span_way.LegDirection( lleg1, leg_forward ) );
	if ( span_way.PicketageIsIncreasing( pckname ) )
		CPPUNIT_ASSERT( leg_forward );
	else
		CPPUNIT_ASSERT( !leg_forward );
}

void TC_SpanKit::OneLinkedManyUnlinked()
{
	CLogicDocumentTest spandoc;
	projdoc->Include( &spandoc );
	auto pstam = projdoc->GetStationManager();
	SetSpanEsr( &spandoc, EsrKit( 13, 29 ) );
	auto plom = spandoc.GetLObjectManager();
	vector <CLogicElement *> spanstrips;
	for ( auto i = 0; i < 6; ++i )
		spanstrips.emplace_back( BuildObject( STRIP, true, &spandoc ) );
	CLogicElement * statstrip = BuildObject( STRIP );

	//формируем пути, связанным из которых, является только один
	BuildJoint( make_pair( statstrip, spanstrips[0] ), nullptr, &spandoc );
	for ( auto i = 0; i < 3; ++i )
		BuildJoint( make_pair( spanstrips[i*2], spanstrips[i*2+1] ), nullptr, &spandoc );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 3 );
	CPPUNIT_ASSERT( !spanKitPtr->RightFormed() );
}

void TC_SpanKit::OneLinked()
{
	CLogicDocumentTest spandoc;
	projdoc->Include( &spandoc );
	auto pstam = projdoc->GetStationManager();
	SetSpanEsr( &spandoc, EsrKit( 13, 29 ) );
	auto plom = spandoc.GetLObjectManager();
	vector <CLogicElement *> spanstrips;
	for ( auto i = 0; i < 6; ++i )
		spanstrips.emplace_back( BuildObject( STRIP, true, &spandoc ) );
	CLogicElement * statstrip = BuildObject( STRIP );

	BuildJoint( make_pair( statstrip, spanstrips[0] ), nullptr, &spandoc );
	for ( auto i = 0; i < 5; ++i )
		BuildJoint( make_pair( spanstrips[i], spanstrips[i+1] ), nullptr, &spandoc );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 1 );
	CPPUNIT_ASSERT( spanKitPtr->RightFormed() );
}

void TC_SpanKit::OneUnlinked()
{
	CLogicDocumentTest spandoc;
	projdoc->Include( &spandoc );
	auto pstam = projdoc->GetStationManager();
	SetSpanEsr( &spandoc, EsrKit( 13, 29 ) );
	auto plom = spandoc.GetLObjectManager();
	vector <CLogicElement *> spanstrips;
	for ( auto i = 0; i < 2; ++i )
		spanstrips.emplace_back( BuildObject( STRIP, true, &spandoc ) );

	BuildJoint( make_pair( spanstrips[0], spanstrips[1] ), nullptr, &spandoc );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 1 );
	CPPUNIT_ASSERT( !spanKitPtr->RightFormed() );
}

void TC_SpanKit::ManyUnlinked()
{
	CLogicDocumentTest spandoc;
	projdoc->Include( &spandoc );
	auto pstam = projdoc->GetStationManager();
	SetSpanEsr( &spandoc, EsrKit( 13, 29 ) );
	auto plom = spandoc.GetLObjectManager();
	vector <CLogicElement *> spanstrips;
	for ( auto i = 0; i < 6; ++i )
		spanstrips.emplace_back( BuildObject( STRIP, true, &spandoc ) );

	for ( auto i = 0; i < 3; ++i )
		BuildJoint( make_pair( spanstrips[i*2], spanstrips[i*2+1] ), nullptr, &spandoc );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 3 );
	CPPUNIT_ASSERT( !spanKitPtr->RightFormed() );
}

void TC_SpanKit::ManyLinked()
{
	CLogicDocumentTest spandoc, statdoc;
	projdoc->Include( &spandoc );
	projdoc->Include( &statdoc );
	auto pstam = projdoc->GetStationManager();
	auto plom = projdoc->GetLObjectManager();
	SetSpanEsr( &spandoc, EsrKit( 13, 29 ) );
	SetStationEsr( &statdoc, EsrKit( 29 ) );
	vector <CLogicElement *> spanstrips;
	for ( auto i = 0; i < 6; ++i )
		spanstrips.emplace_back( BuildObject( STRIP, true, &spandoc ) );
	vector <CLogicElement *> statstrips;
	for ( auto i = 0; i < 3; ++i )
		statstrips.emplace_back( BuildObject( STRIP, true, &statdoc ) );

	for ( auto i = 0; i < 3; ++i )
	{
		BuildJoint( make_pair( statstrips[i], spanstrips[i*2] ), nullptr, &spandoc );
		BuildJoint( make_pair( spanstrips[i*2], spanstrips[i*2+1] ), nullptr, &spandoc );
	}
	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 3 );
	CPPUNIT_ASSERT( spanKitPtr->RightFormed() );
}

void TC_SpanKit::NoneLinked()
{
	CLogicDocumentTest spandoc;
	projdoc->Include( &spandoc );
	auto pstam = projdoc->GetStationManager();
	SetSpanEsr( &spandoc, EsrKit( 13, 29 ) );
	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.empty() );
	CPPUNIT_ASSERT( !spanKitPtr->RightFormed() );
}

void TC_SpanKit::CheckLockType()
{
	//создаем перегон из четырех путей по одному участку
	CLogicDocumentTest spandoc;
	projdoc->Include( &spandoc );
	auto pstam = projdoc->GetStationManager();
	SetSpanEsr( &spandoc, EsrKit( 13, 29 ) );

	//типы блокировок определены только для двух путей
	const SpanDescription * sdescrPtr = pstam->GetDescription<SpanDescription>( &spandoc );
	SpanDescription newDescr( *sdescrPtr );
	newDescr.waysLocks.setLock( 2, SpanLockType::ELECTRIC_TOKEN );
	newDescr.waysLocks.setLock( 5, SpanLockType::SEMI_AUTOBLOCK );
	pstam->SetDescription( newDescr, &spandoc );

	//нумеруем пути от 2 до 5
	for ( auto i = 0; i < 4; ++i )
	{
		CLogicStrip * lstrip = static_cast<CLogicStrip *>( BuildObject( STRIP, true, &spandoc ) );
		lstrip->SetWaynum( i + 2 );
	}
	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	CPPUNIT_ASSERT( spanKitPtr );
	const list <LogicSpanWay> & ways = spanKitPtr->AllWays();
	CPPUNIT_ASSERT( ways.size() == 4 );
	CPPUNIT_ASSERT( find_if( ways.cbegin(), ways.cend(), []( const LogicSpanWay & logspanWay ){ return logspanWay.Waynum() == 2; } ) != ways.cend() );
	CPPUNIT_ASSERT( find_if( ways.cbegin(), ways.cend(), []( const LogicSpanWay & logspanWay ){ return logspanWay.Waynum() == 3; } ) != ways.cend() );
	CPPUNIT_ASSERT( find_if( ways.cbegin(), ways.cend(), []( const LogicSpanWay & logspanWay ){ return logspanWay.Waynum() == 4; } ) != ways.cend() );
	CPPUNIT_ASSERT( find_if( ways.cbegin(), ways.cend(), []( const LogicSpanWay & logspanWay ){ return logspanWay.Waynum() == 5; } ) != ways.cend() );
	for ( const LogicSpanWay & logspanWay : ways )
	{
		auto wnum = logspanWay.Waynum();
		switch( wnum )
		{
		case 2:
			CPPUNIT_ASSERT( logspanWay.LockType() == SpanLockType::ELECTRIC_TOKEN );
			break;
		case 3:
			CPPUNIT_ASSERT( logspanWay.LockType() == SpanLockType::BIDIR_AUTOBLOCK );
			break;
		case 4:
			CPPUNIT_ASSERT( logspanWay.LockType() == SpanLockType::BIDIR_AUTOBLOCK );
			break;
		case 5:
			CPPUNIT_ASSERT( logspanWay.LockType() == SpanLockType::SEMI_AUTOBLOCK );
			break;
		}
	}
}

void TC_SpanKit::AutoOddness()
{
	//создаем перегон из двух путей (один без светфоров, другой со двумя светофором, один из которых с неопределенной четностью)
	//направление четности перегона: нечетное направление от 1452 к 9650
	CLogicDocumentTest spandoc, statdoc;
	projdoc->Include( &spandoc );
	projdoc->Include( &statdoc );
	auto pstam = projdoc->GetStationManager();
	SetSpanEsr( &spandoc, EsrKit( 1452, 9650 ) );
	SetStationEsr( &statdoc, EsrKit( 1452 ) );
	const SpanDescription * sdescrPtr = pstam->GetDescription<SpanDescription>( &spandoc );
	CPPUNIT_ASSERT( sdescrPtr->incOddnessPtr && *sdescrPtr->incOddnessPtr == Oddness::UNDEF );

	auto statStrip1 = BuildObject( STRIP, true, &statdoc );
	auto spanStrip1 = BuildObject( STRIP, true, &spandoc );
	BuildJoint( make_pair( statStrip1, spanStrip1 ), nullptr, &spandoc );

	CLogicElement * legElem = BuildLeg( TRAIN, &spandoc );
	CLogicSemaleg * lleg = static_cast<CLogicSemaleg *>( legElem );
	lleg->SetOddness( Oddness::ODD );
	auto statStrip2 = BuildObject( STRIP, true, &statdoc );
	auto spanStrip2 = BuildObject( STRIP, true, &spandoc );
	BuildJoint( make_pair( statStrip2, spanStrip2 ), lleg, &spandoc );

	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	CPPUNIT_ASSERT( spanKitPtr );
	CPPUNIT_ASSERT( spanKitPtr->GetOddness( StationEsr(1452), StationEsr(9650) ) == Oddness::ODD );
}

void TC_SpanKit::FillForGuess()
{
	CLogicDocumentTest spandoc;
	projdoc->Include( &spandoc );
	auto pstam = projdoc->GetStationManager();
	auto statCode1 = EsrCode().getTerm();
	auto statCode2 = statCode1 + 1;
	const auto & spanCode = EsrKit( statCode1, statCode2 );
	SetSpanEsr( &spandoc, spanCode );

	//типы блокировок
	const SpanDescription * sdescrPtr = pstam->GetDescription<SpanDescription>( &spandoc );
	SpanDescription newDescr( *sdescrPtr );
	newDescr.waysLocks.setLock( 1, SpanLockType::ELECTRIC_TOKEN );
	newDescr.waysLocks.setLock( 2, SpanLockType::SEMI_AUTOBLOCK );
	newDescr.waysLocks.setLock( 4, SpanLockType::BIDIR_AUTOBLOCK );
	pstam->SetDescription( newDescr, &spandoc );

	CLogicElement * legElem = BuildLeg( TRAIN, &spandoc );
	CLogicSemaleg * lleg = static_cast<CLogicSemaleg *>( legElem );
	lleg->SetOddness( Oddness::ODD );

	//нумеруем пути
	for ( auto i = 0; i < 3; ++i )
	{
		auto spanStrip = BuildObject( STRIP, true, &spandoc );
		static_cast<CLogicStrip *>( spanStrip )->SetWaynum( i == 2 ? 4 : i + 1 );
		auto statStrip = BuildObject( STRIP, true, logdoc.get() );
		BuildJoint( make_pair( statStrip, spanStrip ), lleg, &spandoc );
	}
	SpanKitPtr spanKitPtr = plom->GetSpanKit( &spandoc );
	const auto & oddness = spanKitPtr->GetOddness( spanCode.getSpan().first, spanCode.getSpan().second );
	CPPUNIT_ASSERT( spanKitPtr && spanKitPtr->RightFormed() );
	attic::a_document xdoc( "root" );
	spanKitPtr->FillForGuess( xdoc.document_element(), spanCode, nullptr );
	auto span_node = xdoc.document_element().first_child();
	CPPUNIT_ASSERT( span_node.name_is( "span" ) );
	auto esr_attr = span_node.attribute( "esr" );
	CPPUNIT_ASSERT( esr_attr && esr_attr.as_string() == string( "20100:20101" ) );
	auto incoddnAttr = span_node.attribute( "incOddness" );
	CPPUNIT_ASSERT( incoddnAttr && incoddnAttr.as_string() == string( "odd" ) );

	map<string, string> wayattribs;
	for( auto way_node = span_node.child( "way" ); way_node; way_node = way_node.next_sibling( "way" ) )
	{
		auto way_attr = way_node.attribute( "waynum" );
		CPPUNIT_ASSERT( way_attr );
		auto locktype_attr = way_node.attribute( "locktype" );
		CPPUNIT_ASSERT( locktype_attr );
		wayattribs[way_attr.as_string()] = locktype_attr.as_string();
	}
	CPPUNIT_ASSERT( wayattribs.size() == 3 );
	CPPUNIT_ASSERT( wayattribs["1"] == "4" );
	CPPUNIT_ASSERT( wayattribs["2"] == "3" );
	CPPUNIT_ASSERT( wayattribs["4"] == "0" );
}