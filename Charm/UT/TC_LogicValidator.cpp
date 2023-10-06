#include "stdafx.h"
#include "TC_LogicValidator.h"
#include "LogicDocumentTest.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LLoadResult.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LObjectManager.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LogicValidator );

void TC_LogicValidator::OneSidedLink()
{
	CLogicElement * lstrip = BuildObject( STRIP );
	lstrip->PushLink( CLink( SWITCH, BadgeE( L"switch", EsrCode() ), CLink::BASE ) );
	boost::shared_ptr <LLoadResult> lresPtr( new LLoadResult( L"filename ") );
	LogicValidator lvalidator( logdoc.get(), lresPtr );
	lvalidator.Validate();
	wstring loadinfo = lresPtr->GetDetailedInfo();
	CPPUNIT_ASSERT( !loadinfo.empty() );
}

void TC_LogicValidator::JointTransferring()
{
	CLogicDocumentTest spandoc;
	EsrKit spanCode( EsrCode().getTerm(), EsrCode().getTerm() + 1 );
	auto pstam = spandoc.GetStationManager();
	SetSpanEsr( &spandoc, spanCode );
	projdoc->Include( &spandoc );

	auto lstrip1 = BuildObject( STRIP, true, &spandoc );
	auto lstrip2 = BuildObject( STRIP, true, &spandoc );
	auto lstrip3 = BuildObject( STRIP, true, &spandoc );
	auto stripPair12 = make_pair( lstrip1, lstrip2 );
	auto stripPair23 = make_pair( lstrip2, lstrip3 );
	auto lleg1 = BuildLeg( TRAIN, &spandoc );
	auto lleg2 = BuildLeg( TRAIN, &spandoc );
	auto ljoint12 = BuildJoint( stripPair12, lleg1 );
	const auto & ljoint12Link = ljoint12->BuildLink( EsrCode() );
	auto ljoint23 = BuildJoint( stripPair23, lleg2 );
	const auto & ljoint23Link = ljoint23->BuildLink( EsrCode() );
	plim->Join( lstrip1, ljoint12Link );
	plim->Join( lstrip2, ljoint12Link );
	plim->Join( lstrip2, ljoint23Link );
	plim->Join( lstrip3, ljoint23Link );

	//до коррекция стыки и участки разнесены по документам
	auto plom = projdoc->GetLObjectManager();
	set <const CLogicElement *> statJoints, spanStrips;
	set <const CLogicElement *> spanLegs;
	plom->GetObjects<const CLogicElement *>( JOINT, logdoc.get(), statJoints );
	plom->GetObjects<const CLogicElement *>( STRIP, &spandoc, spanStrips );
	plom->GetObjects<const CLogicElement *>( SEMALEG, &spandoc, spanLegs );
	CPPUNIT_ASSERT( statJoints.size() == 2 && spanStrips.size() == 3 && spanLegs.size() == 2 );
	map <const CLogicElement *, vector <CLink> > initLegLinks;
	for ( const CLogicElement * lsema : spanLegs )
		initLegLinks[lsema] = lsema->GetLinks();

	//коррекция
	auto lresPtr = logdoc->GetLoadResult();
	LogicValidator lvalidator( logdoc.get(), lresPtr );
	lvalidator.Validate();

	//участки и стыки
	//после коррекции стыки, связанные с участками, расположенными на перегоне, должны быть перенесены на перегон
	set <const CLogicElement *> statObjects, spanJoints;
	plom->GetObjects<const CLogicElement *>( ALL_CLASSES, logdoc.get(), statObjects );
	plom->GetObjects<const CLogicElement *>( STRIP, &spandoc, spanStrips );
	plom->GetObjects<const CLogicElement *>( JOINT, &spandoc, spanJoints );
	CPPUNIT_ASSERT( statObjects.empty() && spanStrips.size() == 3 && spanJoints.size() == 2 );
	for ( auto ljoint : spanJoints )
		CPPUNIT_ASSERT( ljoint->GetLinks( STRIP ).size() == 2 );
	for ( auto lstrip : spanStrips )
	{
		auto stripLinks = lstrip->GetLinks( JOINT );
		if ( lstrip == lstrip1 || lstrip == lstrip3 )
			CPPUNIT_ASSERT( stripLinks.size() == 1 );
		else
		{
			CPPUNIT_ASSERT( lstrip == lstrip2 );
			CPPUNIT_ASSERT( stripLinks.size() == 2 );
		}
	}

	//светфоры
	plom->GetObjects<const CLogicElement *>( SEMALEG, &spandoc, spanLegs );
	CPPUNIT_ASSERT( spanLegs.size() == 2 );
	for ( const CLogicElement * lsema : spanLegs )
	{
		auto illIt = initLegLinks.find( lsema );
		CPPUNIT_ASSERT( illIt != initLegLinks.cend() );
		CPPUNIT_ASSERT( lsema->GetLinks() == illIt->second );
	}

	CPPUNIT_ASSERT( logdoc->GetLoadResult()->ChangedByLoading() );
	CPPUNIT_ASSERT( spandoc.GetLoadResult()->ChangedByLoading() );
}

void TC_LogicValidator::RoutedJointTransferring()
{
	//еще одна станция и перегон между ними
	CLogicDocumentTest spandoc, statdoc2;
	EsrKit statCode2( EsrCode().getTerm() + 1 ) ;
	EsrKit spanCode( EsrCode().getTerm(), statCode2.getTerm() );
	auto pstam = spandoc.GetStationManager();
	SetSpanEsr( &spandoc, spanCode );
	pstam = statdoc2.GetStationManager();
	SetStationEsr( &statdoc2, statCode2 );
	projdoc->Include( &statdoc2 );
	projdoc->Include( &spandoc );
	pstam = projdoc->GetStationManager();
	auto statDescrPtr = pstam->GetDescription<StationDescription>( logdoc.get() );
	StationDescription statDescr( *statDescrPtr );
	statDescr.options.transroutesAble = true;
	pstam->SetDescription( statDescr, logdoc.get() );

	CLogicElement * lleg = BuildLeg( SHUNTING );
	pair <CLogicElement *, CLogicElement *> stripsNear;
	stripsNear.first = BuildObject( STRIP, true, &spandoc );
	stripsNear.second = BuildObject( STRIP, true, &statdoc2 );
	CLogicElement * ljoint = BuildJoint( stripsNear, lleg );

	//создаем маршрут (вручную, т.к. пока не реализована функциональность трансстанционных маршрутов)
	CLogicElement * lroute = BuildObject( ROUTE, true, logdoc.get() );
	set <const CLogicElement *> lheads;
	plom->GetObjects( HEAD, logdoc.get(), lheads );
	CPPUNIT_ASSERT( lheads.size() == 1 );
	auto lhead = *lheads.cbegin();
	const auto & rlink1 = lhead->BuildLink( EsrCode() );
	const auto & rlink2 = stripsNear.second->BuildLink( statCode2 );
	plim->Join( lroute, rlink1 );
	plim->Join( lroute, rlink2 );

	set <const CLogicElement *> lroutes;
	plom->GetObjects( ROUTE, logdoc.get(), lroutes );
	CPPUNIT_ASSERT( lroutes.size() == 1 );

	//коррекция
	auto lresPtr = logdoc->GetLoadResult();
	LogicValidator lvalidator( logdoc.get(), lresPtr );
	lvalidator.Validate();

	//маршруты должны сохранять состав
	const auto & rlinks = ( *lroutes.cbegin() )->GetLinks();
	CPPUNIT_ASSERT( rlinks.size() == 2 );
	CPPUNIT_ASSERT( rlinks[0] == rlink1 );
	CPPUNIT_ASSERT( rlinks[1] == rlink2 );
}