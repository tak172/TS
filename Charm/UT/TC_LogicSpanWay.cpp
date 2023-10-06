#include "stdafx.h"
#include "TC_LogicSpanWay.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/LogicSpanWay.h"
#include "../StatBuilder/LogicJoint.h"
#include "ProjectDocumentTest.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/LosesErsatz.h"
#include "LogicViewTest.h"
#include "../StatBuilder/LinkManager.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LogicSpanWay );

using namespace std;

typedef LogicSpanWay::JointsStrips JointsStrips;

void TC_LogicSpanWay::NonullLengthWay()
{
	CLogicDocumentTest spandoc;
	EsrKit statEsr = EsrCode();
	EsrKit spanEsr( statEsr.getTerm(), statEsr.getTerm() + 1 );
	SetSpanEsr( &spandoc, spanEsr );
	projdoc->Include( &spandoc );

	list <const CLogicElement *> lstrips;
	pair<CLogicElement *, CLogicElement *> lstrips12;
	CLogicElement * ljoint12 = BuildJoint( lstrips12, nullptr, &spandoc );

	LogicSpanWay::JointStrips jsSet;
	jsSet[lstrips12.first].insert( ljoint12 );
	jsSet[ljoint12].insert( lstrips12.first );
	jsSet[lstrips12.second].insert( ljoint12 );
	jsSet[ljoint12].insert( lstrips12.second );

	pair<CLogicElement *, CLogicElement *> lstrips23( lstrips12.second, nullptr );
	CLogicElement * ljoint23 = BuildJoint( lstrips23, nullptr, &spandoc );

	jsSet[lstrips23.first].insert( ljoint23 );
	jsSet[ljoint23].insert( lstrips23.first );
	jsSet[lstrips23.second].insert( ljoint23 );
	jsSet[ljoint23].insert( lstrips23.second );

	pair <CLogicElement *, CLogicElement *> lstrips34( lstrips23.second, nullptr );
	CLogicElement * ljoint34 = BuildJoint( lstrips34, nullptr, &spandoc );

	jsSet[lstrips34.first].insert( ljoint34 );
	jsSet[ljoint34].insert( lstrips34.first );
	jsSet[lstrips34.second].insert( ljoint34 );
	jsSet[ljoint34].insert( lstrips34.second );

	lstrips.push_back( lstrips12.second );
	lstrips.push_back( lstrips23.second );

	PicketingInfo pckInfo1, pckInfo2;
	pckInfo1.SetCoord( rwRuledCoord( L"axis", rwCoord( 5, 200 )) );
	pckInfo2.SetCoord( rwRuledCoord( L"axis", rwCoord( 11, 720 )) );
	static_cast<CLogicJoint *>( ljoint12 )->SetPicketingInfo( pckInfo1 );
	static_cast<CLogicJoint *>( ljoint34 )->SetPicketingInfo( pckInfo2 );
	
	LogicSpanWay lspanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips ), map<const CLogicElement *, LogicSpanWay::StripsLegs>(), move( jsSet ) );
	const auto & length = lspanWay.Length();
	CPPUNIT_ASSERT( length == 6520. );
}

void TC_LogicSpanWay::UndefLengthWay()
{
	CLogicDocumentTest spandoc;
	EsrKit statEsr = EsrCode();
	EsrKit spanEsr( statEsr.getTerm(), statEsr.getTerm() + 1 );
	SetSpanEsr( &spandoc, spanEsr );
	projdoc->Include( &spandoc );

	LogicSpanWay::JointStrips jsSet;
	list<const CLogicElement *> lstrips;

	pair <CLogicElement *, CLogicElement *> lstrips12;
	CLogicElement * ljoint12 = BuildJoint( lstrips12, nullptr, &spandoc );
	jsSet[lstrips12.first].insert( ljoint12 );
	jsSet[ljoint12].insert( lstrips12.first );
	jsSet[lstrips12.second].insert( ljoint12 );
	jsSet[ljoint12].insert( lstrips12.second );

	pair <CLogicElement *, CLogicElement *> lstrips23( lstrips12.second, nullptr );
	CLogicElement * ljoint23 = BuildJoint( lstrips23, nullptr, &spandoc );
	jsSet[lstrips23.first].insert( ljoint23 );
	jsSet[ljoint23].insert( lstrips23.first );
	jsSet[lstrips23.second].insert( ljoint23 );
	jsSet[ljoint23].insert( lstrips23.second );

	pair <CLogicElement *, CLogicElement *> lstrips34( lstrips23.second, nullptr );
	CLogicElement * ljoint34 = BuildJoint( lstrips34, nullptr, &spandoc );
	jsSet[lstrips34.first].insert( ljoint34 );
	jsSet[ljoint34].insert( lstrips34.first );
	jsSet[lstrips34.second].insert( ljoint34 );
	jsSet[ljoint34].insert( lstrips34.second );

	lstrips.push_back( lstrips12.second );
	lstrips.push_back( lstrips23.second );

	PicketingInfo pckInfo1, pckInfo2;
	pckInfo1.SetCoord( rwRuledCoord( L"axis", rwCoord( 5, 200 ) ) );
	
	static_cast<CLogicJoint *>( ljoint12 )->SetPicketingInfo( pckInfo1 );
	static_cast<CLogicJoint *>( ljoint34 )->SetPicketingInfo( pckInfo2 );

	LogicSpanWay lspanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips ), map<const CLogicElement *, LogicSpanWay::StripsLegs>(), move( jsSet ) );
	const auto & length = lspanWay.Length();
	CPPUNIT_ASSERT( length == 0. );
}

void TC_LogicSpanWay::LengthMiniWayS()
{
	CLogicDocumentTest spandoc;
	EsrKit statEsr = EsrCode();
	EsrKit spanEsr( statEsr.getTerm(), statEsr.getTerm() + 1 );
	SetSpanEsr( &spandoc, spanEsr );
	projdoc->Include( &spandoc );

	LogicSpanWay::JointStrips jsSet;
	list<const CLogicElement *> lstrips;

	const CLogicElement * lstrip = BuildObject( STRIP, nullptr, &spandoc );
	lstrips.push_back( lstrip );
	jsSet.insert( make_pair( lstrip, set<const CLogicElement *>() ) );
	LogicSpanWay lspanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips ), map<const CLogicElement *, LogicSpanWay::StripsLegs>(), move( jsSet ) );
	const auto & length = lspanWay.Length();
	CPPUNIT_ASSERT( length == 0 );
    delete lstrip;
}

void TC_LogicSpanWay::LengthMiniWayJ()
{
	CLogicDocumentTest spandoc;
	EsrKit statEsr = EsrCode();
	EsrKit spanEsr( statEsr.getTerm(), statEsr.getTerm() + 1 );
	SetSpanEsr( &spandoc, spanEsr );
	projdoc->Include( &spandoc );

	LogicSpanWay::JointStrips jsSet;
	list<const CLogicElement *> lstrips;

	const CLogicElement * ljoint = BuildObject( JOINT, nullptr, &spandoc );
	jsSet.insert( make_pair( ljoint, set<const CLogicElement *>() ) );
	LogicSpanWay lspanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips ), map<const CLogicElement *, LogicSpanWay::StripsLegs>(), move( jsSet ) );
	const auto & length = lspanWay.Length();
	CPPUNIT_ASSERT( length == 0 );
    delete ljoint;
}

void TC_LogicSpanWay::LengthMiniWaySJ()
{
	CLogicDocumentTest spandoc;
	EsrKit statEsr = EsrCode();
	EsrKit spanEsr( statEsr.getTerm(), statEsr.getTerm() + 1 );
	SetSpanEsr( &spandoc, spanEsr );
	projdoc->Include( &spandoc );

	LogicSpanWay::JointStrips jsSet;
	list<const CLogicElement *> lstrips;

	CLogicElement * lstrip = BuildObject( STRIP, nullptr, &spandoc );
	lstrips.push_back( lstrip );
	CLogicElement * ljoint = BuildObject( JOINT, nullptr, &spandoc );
	plim->Join( lstrip, ljoint->BuildLink( spanEsr ) );
	plim->Join( ljoint, lstrip->BuildLink( spanEsr ) );

	set<const CLogicElement *> ljointsSet;
	ljointsSet.insert( ljoint );
	jsSet.insert( make_pair( lstrip, ljointsSet ) );

	set<const CLogicElement *> lstripsSet;
	lstripsSet.insert( lstrip );
	jsSet.insert( make_pair( ljoint, lstripsSet ) );

	LogicSpanWay lspanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips ), map<const CLogicElement *, LogicSpanWay::StripsLegs>(), move( jsSet ) );
	const auto & length = lspanWay.Length();
	CPPUNIT_ASSERT( length == 0 );
    delete ljoint;
    delete lstrip;
}

void TC_LogicSpanWay::LengthMiniWaySJS()
{
	CLogicDocumentTest spandoc;
	EsrKit statEsr = EsrCode();
	EsrKit spanEsr( statEsr.getTerm(), statEsr.getTerm() + 1 );
	SetSpanEsr( &spandoc, spanEsr );
	projdoc->Include( &spandoc );

	LogicSpanWay::JointStrips jsSet;
	list<const CLogicElement *> lstrips;

	pair <CLogicElement *, CLogicElement *> stripPair;
	CLogicElement * ljoint = BuildJoint( stripPair );

	set<const CLogicElement *> lstripsSet, ljointsSet;
	lstripsSet.insert( stripPair.first );
	lstripsSet.insert( stripPair.second );
	ljointsSet.insert( ljoint );
	jsSet.insert( make_pair( ljoint, lstripsSet ) );
	jsSet.insert( make_pair( stripPair.first, ljointsSet ) );
	jsSet.insert( make_pair( stripPair.second, ljointsSet ) );

	LogicSpanWay lspanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips ), map<const CLogicElement *, LogicSpanWay::StripsLegs>(), move( jsSet ) );
	const auto & length = lspanWay.Length();
	CPPUNIT_ASSERT( length == 0 );
}

void TC_LogicSpanWay::LengthMiniWayJSJ()
{
	CLogicDocumentTest spandoc;
	EsrKit statEsr = EsrCode();
	EsrKit spanEsr( statEsr.getTerm(), statEsr.getTerm() + 1 );
	SetSpanEsr( &spandoc, spanEsr );
	projdoc->Include( &spandoc );

	LogicSpanWay::JointStrips jsSet;
	list<const CLogicElement *> lstrips;

	CLogicElement * lstrip = BuildObject( STRIP, nullptr, &spandoc );
	lstrips.push_back( lstrip );
	CLogicElement * ljoint1 = BuildObject( JOINT, nullptr, &spandoc );
	CLogicElement * ljoint2 = BuildObject( JOINT, nullptr, &spandoc );
	plim->Join( lstrip, ljoint1->BuildLink( spanEsr ) );
	plim->Join( ljoint1, lstrip->BuildLink( spanEsr ) );
	plim->Join( lstrip, ljoint2->BuildLink( spanEsr ) );
	plim->Join( ljoint2, lstrip->BuildLink( spanEsr ) );

	PicketingInfo pckInfo1, pckInfo2;
	pckInfo1.SetCoord( rwRuledCoord( L"axis", rwCoord( 4, 200 )) );
	pckInfo2.SetCoord( rwRuledCoord( L"axis", rwCoord( 11, 720 )) );
	static_cast<CLogicJoint *>( ljoint1 )->SetPicketingInfo( pckInfo1 );
	static_cast<CLogicJoint *>( ljoint2 )->SetPicketingInfo( pckInfo2 );

	set<const CLogicElement *> ljointsSet;
	ljointsSet.insert( ljoint1 );
	ljointsSet.insert( ljoint2 );
	jsSet.insert( make_pair( lstrip, ljointsSet ) );

	set<const CLogicElement *> lstripsSet;
	lstripsSet.insert( lstrip );
	jsSet.insert( make_pair( ljoint1, lstripsSet ) );
	jsSet.insert( make_pair( ljoint2, lstripsSet ) );

	LogicSpanWay lspanWay( SpanLockType::BIDIR_AUTOBLOCK, move( lstrips ), map<const CLogicElement *, LogicSpanWay::StripsLegs>(), move( jsSet ) );
	const auto & length = lspanWay.Length();
	CPPUNIT_ASSERT( length == 7520 );
    delete ljoint2;
    delete ljoint1;
    delete lstrip;
}