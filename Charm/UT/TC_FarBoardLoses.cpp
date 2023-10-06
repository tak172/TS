#include "stdafx.h"

#include "TC_FarBoardLoses.h"
#include "../Fund/Lexicon.h"
#include "../Fund/IndirectLosesNotifier.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/LObjectManager.h"
#include "../Fund/LogicRelay.h"
#include "../Fund/RouteElementEvent.h"
#include "BuildTrainDescr.h"
#include "../Fund/TrainPlacesReview.h"
#include "../StatBuilder/LogicSemaleg.h"
#include "../Actor/InfoNumber.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_FarBoardLoses );

void TC_FarBoardLoses::setUp()
{
	TC_Project::setUp();
	MakeShortSetObjects();
	MakeLengthySetObjects();
	MakeBPostSetObjects();

	//генерация маршрутов:
	CStationManager * pstam = projdoc->GetStationManager();
	auto statInfoPtr = pstam->GetDescription<StationDescription>( &bpset.bpostDoc );
	ASSERT( statInfoPtr );
	StationDescription bpostDescr( *statInfoPtr );
	bpostDescr.options.transroutesAble = true;
	pstam->SetDescription( bpostDescr, &bpset.bpostDoc ); //для блок-поста разрешены сквозные маршруты
	CLObjectManager * plom = logdoc->GetLObjectManager();
	plom->GenerateRoutes( logdoc.get() );
	plom->GenerateRoutes( &bpset.bpostDoc );

	InitLexicon();
	tplcReviewer.reset( new TrainPlacesReview() );
	indirLosesNotifier.reset( new IndirectLosesNotifier( L"", logdoc.get(), *lexiconPtr, *tplcReviewer, list <CLogicDocument *>() ) );
}

void TC_FarBoardLoses::InitLexicon()
{
	vector<CLogicDocumentTest *> docvec;
	docvec.push_back( logdoc.get() );
	docvec.push_back( &sset.spanDoc );
	docvec.push_back( &lset.lspanDoc );
	docvec.push_back( &lset.rspanDoc );
	docvec.push_back( &bpset.lspanDoc );
	docvec.push_back( &bpset.bpostDoc );
	docvec.push_back( &bpset.rspanDoc );
	set <const CLogicElement *> allObjects;
	for ( auto ldoc : docvec )
	{
		set <const CLogicElement *> docObjects;
		plom->GetObjects( ALL_CLASSES, ldoc, docObjects );
		allObjects.insert( docObjects.cbegin(), docObjects.cend() );
	}
	lexiconPtr.reset( new Lexicon() );
	lexiconPtr->reserve( allObjects.size() );
	auto pstam = logdoc->GetStationManager();
	for ( auto lobject : allObjects )
	{
		auto ecode = pstam->GetESR( lobject );
        CPPUNIT_ASSERT( lexiconPtr->append( lobject, ecode ) );
	}
}

void TC_FarBoardLoses::TrainsPlacingNotify( attic::a_node trainsNode )
{
	const auto & changedStrips = tplcReviewer->PlacesChangeNotify( trainsNode );
	for ( const auto & stripBdg : changedStrips )
	{
		auto lstrip = lexiconPtr->LogElBy( stripBdg );
		indirLosesNotifier->ObjectStateNotify( lstrip );
	}
}

void TC_FarBoardLoses::MakeShortSetObjects()
{
	auto & spanDoc = sset.spanDoc;
	auto & anotherStatDoc = sset.anotherStatDoc;
	auto pstam = spanDoc.GetStationManager();
	auto statCode = EsrCode();
	auto anotherStatCode = EsrKit( statCode.getTerm() + 1 );
	const auto & spanCode = EsrKit( statCode.getTerm(), anotherStatCode.getTerm() );
	SetSpanEsr( &spanDoc, spanCode );
	pstam = anotherStatDoc.GetStationManager();
	SetStationEsr( &anotherStatDoc, anotherStatCode );

	projdoc->Include( &spanDoc );
	projdoc->Include( &anotherStatDoc );
	MakeShortSetStationObjects();
	sset.spanStrips.resize( 5 );
	sset.spanBoard = MakeSpanObjects( &spanDoc, sset.spanStrips );

	//связь между перегоном и станцией
	auto & spanStrips = sset.spanStrips;
	auto & statStrips = sset.statStrips;
	sset.bndLegFromSpanToStat = BuildLeg( TRAIN );
	pair <CLogicElement *, CLogicElement *> bndstrips( spanStrips[0], statStrips[3] );
	BuildJoint( bndstrips, sset.bndLegFromSpanToStat );

	//ограничим перегон другой станцией
	auto farStatStrip = BuildObject( STRIP, true, &anotherStatDoc );
	pair <CLogicElement *, CLogicElement *> bndstrips2( spanStrips.back(), farStatStrip );
	BuildJoint( bndstrips2 );
}

void TC_FarBoardLoses::MakeShortSetStationObjects()
{
	auto & statStrips = sset.statStrips;
	auto & statBoard = sset.statBoard;
	statStrips.resize( 4 );
	MakeStripLine( logdoc.get(), statStrips );
	statBoard = BuildObject( LOGIC_BOARD, true, logdoc.get() );

	//связывание
	CLinkManager * plim = logdoc->GetLinkManager();

	//-участка с табло
	CLink statBoardLink = statBoard->BuildLink( EsrCode( logdoc.get() ) );
	plim->Join( statStrips[1], statBoardLink );
	//- светофоров с участками
	sset.statLegFromSpanToStat = BuildLeg( TRAIN );
	LinkLeg( sset.statLegFromSpanToStat, make_pair( statStrips[1], statStrips[0] ) );
	sset.statLegFromStatToSpan = BuildLeg( TRAIN );
	LinkLeg( sset.statLegFromStatToSpan, make_pair( statStrips[1], statStrips[2] ) );
}

void TC_FarBoardLoses::MakeLengthySetObjects()
{
	auto & lspanDoc = lset.lspanDoc;
	auto & rspanDoc = lset.rspanDoc;
	auto pstam = lspanDoc.GetStationManager();
	auto statCode = EsrCode();
	auto statCode1 = EsrKit( statCode.getTerm() + 17 );
	const auto & lspanCode = EsrKit( statCode.getTerm(), statCode1.getTerm() );
	SetSpanEsr( &lspanDoc, lspanCode );
	auto statCode2 = EsrKit( statCode.getTerm() + 18 );
	pstam = rspanDoc.GetStationManager();
	const auto & rspanCode = EsrKit( statCode.getTerm(), statCode2.getTerm() );
	SetSpanEsr( &rspanDoc, rspanCode );
	projdoc->Include( &lspanDoc );
	projdoc->Include( &rspanDoc );

	lset.lspanStrips.resize( 3 );
	lset.lspanBoard = MakeSpanObjects( &lset.lspanDoc, lset.lspanStrips );
	MakeLengthySetStatObjects();

	//связывание станции и перегонов:
	pair <CLogicElement *, CLogicElement *> bndStrips( lset.lspanStrips.back(), lset.statStrips.front() );
	auto bndLeg = BuildLeg( TRAIN );
	lset.semalegs.insert( lset.semalegs.cbegin(), bndLeg );
	BuildJoint( bndStrips, bndLeg, logdoc.get() );
	lset.rspanStrips.resize( 3 );
	lset.rspanBoard = MakeSpanObjects( &lset.rspanDoc, lset.rspanStrips );
	bndStrips = make_pair( lset.statStrips.back(), lset.rspanStrips.front() );
	BuildJoint( bndStrips, nullptr, logdoc.get() );
}

CLogicElement * TC_FarBoardLoses::MakeSpanObjects( CLogicDocumentTest * spanDoc, vector <CLogicElement *> & lstrips )
{
	MakeStripLine( spanDoc, lstrips );

	//генерация табло
	auto spanBoard = BuildObject( LOGIC_BOARD, true, spanDoc );

	//связывание участков с табло
	CLinkManager * plim = logdoc->GetLinkManager();
	CLink spanBoardLink = spanBoard->BuildLink( EsrCode( spanDoc ) );
	for ( unsigned int k = 0; k < lstrips.size(); ++k )	
		plim->Join( lstrips[k], spanBoardLink );

	return spanBoard;
}

void TC_FarBoardLoses::MakeStripLine( CLogicDocumentTest * ldoc, vector <CLogicElement *> & lstrips )
{
	auto stripsCount = lstrips.size();
	//генерация и связывание участков
	for ( unsigned int k = 0; k < stripsCount; ++k )
		lstrips[k] = BuildObject( STRIP, true, ldoc );
	for ( unsigned int k = 0; k < lstrips.size() - 1; ++k )
		BuildJoint( pair <CLogicElement *, CLogicElement *>( lstrips[k], lstrips[k+1] ), nullptr, ldoc );
}

void TC_FarBoardLoses::MakeLengthySetStatObjects()
{
	CLinkManager * plim = logdoc->GetLinkManager();
	lset.statStrips.resize( 12 );
	MakeStripLine( logdoc.get(), lset.statStrips );

	//генерация поездных табло
	unsigned int stripInd = 1;
	for ( unsigned int k = 0; k < 4; ++k, stripInd += 3 )
	{
		auto statBoard = BuildObject( LOGIC_BOARD, true, logdoc.get() );
		lset.statBoards.push_back( statBoard );
		const CLink & statBoardLink = statBoard->BuildLink( EsrCode() );
		plim->Join( lset.statStrips[stripInd], statBoardLink );
	}

	//генерация светофоров
	stripInd = 2;
	for ( unsigned int k= 0; k < 3; ++k, stripInd += 3 )
	{
		auto lleg = BuildLeg( TRAIN );
		lset.semalegs.push_back( lleg );
		LinkLeg( lleg, make_pair( lset.statStrips[stripInd], lset.statStrips[stripInd+1] ) );
	}
}

void TC_FarBoardLoses::MakeBPostSetObjects()
{
	auto & lspanDoc = bpset.lspanDoc;
	auto & bpostDoc = bpset.bpostDoc;
	auto & rspanDoc = bpset.rspanDoc;
	auto statCode = EsrCode();
	auto statCode1 = EsrKit( statCode.getTerm() + 55 );
	auto bpostCode = EsrKit( statCode.getTerm() + 56 );
	auto statCode2 = EsrKit( statCode.getTerm() + 57 );
	auto pstam = lspanDoc.GetStationManager();
	const auto & lspanCode = EsrKit( statCode1.getTerm(), bpostCode.getTerm() );
	SetSpanEsr( &lspanDoc, lspanCode );
	pstam = bpostDoc.GetStationManager();
	SetStationEsr( &bpostDoc, bpostCode );
	pstam = rspanDoc.GetStationManager();
	const auto & rspanCode = EsrKit( bpostCode.getTerm(), statCode2.getTerm() );
	SetSpanEsr( &rspanDoc, rspanCode );
	projdoc->Include( &lspanDoc );
	projdoc->Include( &bpostDoc );
	projdoc->Include( &rspanDoc );
	auto testpstam = rspanDoc.GetStationManager();

	bpset.statStrips.resize( 2 );
	MakeStripLine( &bpset.bpostDoc, bpset.statStrips );

	bpset.lspanStrips.resize( 2 );
	MakeSpanObjects( &bpset.lspanDoc, bpset.lspanStrips );
	bpset.rspanStrips.resize( 2 );
	bpset.rspanBoard = MakeSpanObjects( &bpset.rspanDoc, bpset.rspanStrips );

	//связывание станции и перегонов:
	pair <CLogicElement *, CLogicElement *> bndStrips( bpset.lspanStrips.back(), bpset.statStrips.front() );
	bpset.semaleg = BuildLeg( TRAIN, &bpset.bpostDoc );
	BuildJoint( bndStrips, bpset.semaleg, &bpset.bpostDoc );
	bndStrips = make_pair( bpset.statStrips.back(), bpset.rspanStrips.front() );
	BuildJoint( bndStrips, nullptr, &bpset.bpostDoc );
}

void TC_FarBoardLoses::CleanTrainOnStrip( bool only_number, CLogicElement * lstrip )
{
	auto stripLos = lexiconPtr->getLos( lstrip );
	stripLos->setUsed( only_number ? StateLTS::ACTIVE : StateLTS::PASSIVE );
	lexiconPtr->markAsChanged( lstrip );
	attic::a_document numdoc;
	TrainDescr tdescr;
	auto nodesPair = CreateInfoDoc( numdoc, tdescr, 1 );
	MakeTrainNode( nodesPair.second, lstrip, false );
	TrainsPlacingNotify( nodesPair.first );
}

TrainDescr TC_FarBoardLoses::SetTrainOnStrip( CLogicElement * lstrip, bool oddNumber )
{
	unsigned int trainNum = ( oddNumber ? 25 : 20 );
	//на самом удаленном участке перегона находится поезд
	auto stripLos = lexiconPtr->getLos( lstrip );
	stripLos->setUsed( StateLTS::ACTIVE );
	lexiconPtr->markAsChanged( lstrip );
	attic::a_document numdoc;
	TrainDescr tdescr = BuildTrain( trainNum, L"11-22-33", L"", 65, 99, 9, L"2:10" );
	auto nodesPair = CreateInfoDoc( numdoc, tdescr, 1 );
	MakeTrainNode( nodesPair.second, lstrip, true );
	TrainsPlacingNotify( nodesPair.first );
	return tdescr;
}

RoutesChangesDetector TC_FarBoardLoses::MakeRouteChangesDetector( const set<const CLogicElement *> & lroutes ) const
{
	RoutesPredefinedContent routesContent;
	for ( auto lroute : lroutes )
	{
		const auto & rlinks = lroute->GetLinks();
		vector<const CLogicElement *> linkelems;
		for ( const auto & rlink : rlinks )
		{
			auto lelem = plom->GetObjectByBadge<const CLogicElement *>( rlink.obj_badge );
			CPPUNIT_ASSERT( lelem );
			linkelems.push_back( lelem );
		}
		routesContent.insert( make_pair( lroute, linkelems ) );
	}
	return RoutesChangesDetector( routesContent );
}

void TC_FarBoardLoses::SetRouteByLeg( CLogicElement * lleg )
{
	//задание маршрута от поездной головы указанной мачты
	CLObjectManager * plom = logdoc->GetLObjectManager();
	const auto & headLinks = lleg->GetLinks( HEAD );
	auto hlIt = find_if( headLinks.cbegin(), headLinks.cend(), [plom]( const CLink & hlink ){
		auto lhead = plom->GetObjectByBadge<const CLogicElement *>( hlink.obj_badge );
		ASSERT( lhead );
		return lhead->GetType() == TRAIN;
	} );
	ASSERT( hlIt != headLinks.cend() );
	wstring headName = hlIt->obj_badge.str();

	set <const CLogicElement *> lroutes;
	vector <CLogicDocument *> logdocs;
	projdoc->GetDocuments( logdocs );
	for ( auto ldoc : logdocs )
	{
		set <const CLogicElement *> docroutes;
		plom->GetObjects( ROUTE, ldoc, docroutes );
		lroutes.insert( docroutes.cbegin(), docroutes.cend() );
	}
	auto rchangeDetector = MakeRouteChangesDetector( lroutes );
	LogicRelay logrelay( *lexiconPtr, rchangeDetector );

	auto routeIt = find_if( lroutes.cbegin(), lroutes.cend(), [&headName]( const CLogicElement * lroute ){
		const auto & rlinks = lroute->GetLinks( HEAD );
		ASSERT( rlinks.size() == 1 );
		return rlinks[0].obj_badge.str() == headName;
	} );
	CPPUNIT_ASSERT( routeIt != lroutes.cend() );
	const CLogicElement * lroute = *routeIt;
	auto losRoute = lexiconPtr->getLos( lroute );
	losRoute->processRouteEvents( set<BadgeE>() );
	lexiconPtr->markAsChanged( lroute );
	const auto & elemLinks = lroute->GetLinks();
	for ( const auto & elemLink : elemLinks )
	{
		const CLogicElement * logEl = nullptr;
		LOS_base * elemLos = nullptr;
		boost::tie(logEl,elemLos) = lexiconPtr->getBoth( elemLink.obj_badge );
		if ( logEl->GetClass() == STRIP || logEl->GetClass() == HEAD )
		{
			ASSERT( elemLos );
			if ( logEl->GetClass() == STRIP )
			{
				elemLos->bindRoute( true );
				elemLos->setLock( StateLTS::ACTIVE );
			}
			else if ( logEl->GetClass() == HEAD )
				elemLos->setOpenSignal( StateLTS::ACTIVE );

			vector<RouteElementEvent> reeVec;
			reeVec.push_back( RouteElementEvent( logEl, elemLos, elemLink.obj_badge ) );
			logrelay.processRouteElementEvent( reeVec );
			lexiconPtr->markAsChanged(logEl);
		}
	}
	indirLosesNotifier->DirectToIndirect();
}

void TC_FarBoardLoses::SHO_SpanToStatByRouteOddNumOddSema()
{
	SHO_SpanToStatByRoute( true, Oddness::ODD, true );
}

void TC_FarBoardLoses::SHO_SpanToStatByRouteOddNumEvenSema()
{
	SHO_SpanToStatByRoute( true, Oddness::EVEN, false );
}

void TC_FarBoardLoses::SHO_SpanToStatByRouteOddNumUndefSema()
{
	SHO_SpanToStatByRoute( true, Oddness::UNDEF, true );
}

void TC_FarBoardLoses::SHO_SpanToStatByRouteEvenNumOddSema()
{
	SHO_SpanToStatByRoute( false, Oddness::ODD, false );
}

void TC_FarBoardLoses::SHO_SpanToStatByRouteEvenNumEvenSema()
{
	SHO_SpanToStatByRoute( false, Oddness::EVEN, true );
}

void TC_FarBoardLoses::SHO_SpanToStatByRouteEvenNumUndefSema()
{
	SHO_SpanToStatByRoute( false, Oddness::UNDEF, true );
}

void TC_FarBoardLoses::SHO_SpanToStatByRoute( bool numIsOdd, Oddness semaOddness, bool permitted )
{
	CLogicSemaleg * lleg = static_cast <CLogicSemaleg *>( sset.bndLegFromSpanToStat );
	auto initOddness = lleg->GetOddness();
	lleg->SetOddness( semaOddness );

	auto & spanStrips = sset.spanStrips;
	auto statBoard = sset.statBoard;
	const auto & tdescr = SetTrainOnStrip( spanStrips.back(), numIsOdd );
	lexiconPtr->reset_all_changed();
	auto statBrdLos = lexiconPtr->getLos( statBoard );
	auto inums = statBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.empty() );
	SetRouteByLeg( sset.bndLegFromSpanToStat );

	//задание маршрута может вызывать реакцию на станционном табло
	inums = statBrdLos->get_Subitems<Info_Number>();
	if ( permitted )
		CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE == tdescr && inums.back().lock_mark && !inums.back().used_mark );
	else
		CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE.empty() && inums.back().lock_mark && !inums.back().used_mark );

	//сброс номера приводит к зачистке станционного табло
	CleanTrainOnStrip( true, spanStrips.back() );
	inums = statBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE.empty() && inums.back().lock_mark && !inums.back().used_mark );

	//установка номера снова
	SetTrainOnStrip( spanStrips.back(), numIsOdd );
	inums = statBrdLos->get_Subitems<Info_Number>();
	if ( permitted )
		CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE == tdescr && inums.back().lock_mark && !inums.back().used_mark );
	else
		CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE.empty() && inums.back().lock_mark && !inums.back().used_mark );

	//заезд поезда
	auto & statStrips = sset.statStrips;
	auto brdStripLos = lexiconPtr->getLos( statStrips[1] );
	brdStripLos->setUsed( StateLTS::ACTIVE );
	CleanTrainOnStrip( false, spanStrips.back() );
	lexiconPtr->markAsChanged( statStrips[1] );
	auto nextStripLos = lexiconPtr->getLos( statStrips[0] );
	nextStripLos->setUsed( StateLTS::ACTIVE );
	lexiconPtr->markAsChanged( statStrips[0] );
	indirLosesNotifier->DirectToIndirect();
	brdStripLos->setUsed( StateLTS::PASSIVE );
	lexiconPtr->markAsChanged( statStrips[1] );
	indirLosesNotifier->DirectToIndirect();
	inums = statBrdLos->get_Subitems<Info_Number>();
	if ( permitted )
		CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE.empty() && inums.back().lock_mark && !inums.back().used_mark );
	else
		CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE.empty() && inums.back().lock_mark && !inums.back().used_mark );

	lleg->SetOddness( initOddness );
}

void TC_FarBoardLoses::SHO_SpanToStatByStripOddNumOddSema()
{
	SHO_SpanToStatByStrip( true, Oddness::ODD, true );
}

void TC_FarBoardLoses::SHO_SpanToStatByStripOddNumEvenSema()
{
	SHO_SpanToStatByStrip( true, Oddness::EVEN, false );
}

void TC_FarBoardLoses::SHO_SpanToStatByStripOddNumUndefSema()
{
	SHO_SpanToStatByStrip( true, Oddness::UNDEF, true );
}

void TC_FarBoardLoses::SHO_SpanToStatByStripEvenNumOddSema()
{
	SHO_SpanToStatByStrip( false, Oddness::ODD, false );
}

void TC_FarBoardLoses::SHO_SpanToStatByStripEvenNumEvenSema()
{
	SHO_SpanToStatByStrip( false, Oddness::EVEN, true );
}

void TC_FarBoardLoses::SHO_SpanToStatByStripEvenNumUndefSema()
{
	SHO_SpanToStatByStrip( false, Oddness::UNDEF, true );
}

void TC_FarBoardLoses::SHO_SpanToStatByStrip( bool numIsOdd, Oddness semaOddness, bool permitted )
{
	CLogicSemaleg * lleg = static_cast <CLogicSemaleg *>( sset.bndLegFromSpanToStat );
	auto initOddness = lleg->GetOddness();
	lleg->SetOddness( semaOddness );

	auto & statBoard = sset.statBoard;
	auto & spanStrips = sset.spanStrips;
	auto statBrdLos = lexiconPtr->getLos( statBoard );
	SetRouteByLeg( sset.bndLegFromSpanToStat );
	lexiconPtr->reset_all_changed();
	auto inums = statBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE.empty() );

	const auto & tdescr = SetTrainOnStrip( spanStrips.back(), numIsOdd );

	//появление должно вызывать реакцию на станционном табло
	inums = statBrdLos->get_Subitems<Info_Number>();
	if ( permitted )
		CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE == tdescr && inums.back().lock_mark && !inums.back().used_mark );
	else
		CPPUNIT_ASSERT( inums.size() == 1 && inums.back().traitsPE.empty() && inums.back().lock_mark && !inums.back().used_mark );

	lleg->SetOddness( initOddness );
}

void TC_FarBoardLoses::SHO_StatToSpanByRoute()
{
	auto & statStrips = sset.statStrips;
	auto & spanBoard = sset.spanBoard;
	const auto & tdescr = SetTrainOnStrip( statStrips[1] );
	lexiconPtr->reset_all_changed();
	auto spanBrdLos = lexiconPtr->getLos( spanBoard );
	auto inums = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.empty() );
	SetRouteByLeg( sset.statLegFromStatToSpan );

	//задание маршрута должно вызывать реакцию на перегонном табло
	inums = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 5 && inums.front().traitsPE == tdescr && !inums.front().lock_mark && !inums.front().used_mark && 
		inums.front().forecast_mark );

	//сброс номера приводит к зачистке станционного табло
	CleanTrainOnStrip( true, statStrips[1] );
	inums = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 5 && inums.front().traitsPE.empty() && !inums.front().lock_mark && !inums.front().used_mark &&
		!inums.front().forecast_mark );

	//установка номера снова
	SetTrainOnStrip( statStrips[1] );
	inums = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 5 && inums.front().traitsPE == tdescr && !inums.front().lock_mark && !inums.front().used_mark && 
		inums.front().forecast_mark );

	//заезд поезда
	auto & spanStrips = sset.spanStrips;
	auto stripLos = lexiconPtr->getLos( statStrips[2] );
	stripLos->setUsed( StateLTS::ACTIVE );
	CleanTrainOnStrip( false, statStrips[1] );
	lexiconPtr->markAsChanged( statStrips[2] );
	stripLos = lexiconPtr->getLos( statStrips[3] );
	stripLos->setUsed( StateLTS::ACTIVE );
	lexiconPtr->markAsChanged( statStrips[3] );
	stripLos = lexiconPtr->getLos( spanStrips[0] );
	stripLos->setUsed( StateLTS::ACTIVE );
	lexiconPtr->markAsChanged( spanStrips[0] );
	indirLosesNotifier->DirectToIndirect();
	inums = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 5 && inums.front().traitsPE.empty() && !inums.front().lock_mark && inums.front().used_mark && 
		!inums.front().forecast_mark );
}

void TC_FarBoardLoses::SHO_StatToSpanByStrip()
{
	auto & spanBoard = sset.spanBoard;
	auto & statStrips = sset.statStrips;
	auto spanBrdLos = lexiconPtr->getLos( spanBoard );
	SetRouteByLeg( sset.statLegFromStatToSpan );
	lexiconPtr->reset_all_changed();
	auto inums = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 5 && inums.front().traitsPE.empty() );
	const auto & tdescr = SetTrainOnStrip( statStrips[1] );

	//появление должно вызывать реакцию на перегонном табло
	inums = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 5 && inums.front().traitsPE == tdescr && !inums.front().lock_mark && !inums.front().used_mark && inums.front().forecast_mark );
}

void TC_FarBoardLoses::SHO_StatToSpanErrorCase()
{
	auto & spanBoard = sset.spanBoard;
	auto & statStrips = sset.statStrips;
	auto & spanStrips = sset.spanStrips;
	auto spanBrdLos = lexiconPtr->getLos( spanBoard );
	SetRouteByLeg( sset.statLegFromStatToSpan );
	lexiconPtr->reset_all_changed();
	const auto & tdescr = SetTrainOnStrip( statStrips[1] );

	//резкий переброс номера со станции на перегон
	SetTrainOnStrip( spanStrips.back() );

	//на перегонном табло не должно быть дублирования
	const auto & inums = spanBrdLos->get_Subitems<Info_Number>();
	for ( unsigned int i = 0; i < inums.size(); ++i )
	{
		if ( i == 4 )
			CPPUNIT_ASSERT( inums[i].traitsPE == tdescr && !inums[i].lock_mark && inums[i].used_mark && !inums[i].forecast_mark );
		else
			CPPUNIT_ASSERT( inums[i].traitsPE.empty() && !inums[i].lock_mark && !inums[i].used_mark && !inums[i].used_mark );
	}
}

void TC_FarBoardLoses::LEN_SpanToStatByRoute()
{
	//установка поезда
	auto & lspanStrips = lset.lspanStrips;
	auto statBoard1 = lset.statBoards[0];
	auto statBoard2 = lset.statBoards[1];
	const auto & tdescr = SetTrainOnStrip( lspanStrips.front() );
	lexiconPtr->reset_all_changed();
	auto statBrdLos1 = lexiconPtr->getLos( statBoard1 );
	auto statBrdLos2 = lexiconPtr->getLos( statBoard2 );
	auto inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.empty() );
	auto inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.empty() );

	//задаем составной маршрут:
	SetRouteByLeg( lset.semalegs[0] );
	SetRouteByLeg( lset.semalegs[1] );

	//задание маршрута должно вызывать реакцию на всех релевантных станционных табло
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );

	//сброс номера приводит к зачистке станционного табло
	CleanTrainOnStrip( true, lspanStrips.front() );
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE.empty() && inums2.back().lock_mark && !inums2.back().used_mark );

	//установка номера снова
	SetTrainOnStrip( lspanStrips.front() );
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );

	//заезд поезда
	auto & statStrips = lset.statStrips;
	auto brdStripLos = lexiconPtr->getLos( statStrips[1] );
	brdStripLos->setUsed( StateLTS::ACTIVE );
	CleanTrainOnStrip( false, lspanStrips.front() );
	lexiconPtr->markAsChanged( statStrips[1] );
	auto nextStripLos = lexiconPtr->getLos( statStrips[2] );
	nextStripLos->setUsed( StateLTS::ACTIVE );
	lexiconPtr->markAsChanged( statStrips[2] );
	SetTrainOnStrip( statStrips[2] );
	indirLosesNotifier->DirectToIndirect();

	brdStripLos->setUsed( StateLTS::PASSIVE );
	lexiconPtr->markAsChanged( statStrips[1] );
	indirLosesNotifier->DirectToIndirect();
	//для первого маршрута остается замыкание:
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() && inums1.back().lock_mark && !inums1.back().used_mark );
	//для второго маршрута номер сохраняется:
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );
}

void TC_FarBoardLoses::LEN_SpanToStatByStrip()
{
	auto & statBoard1 = lset.statBoards[0];
	auto & statBoard2 = lset.statBoards[1];
	auto & spanStrips = lset.lspanStrips;
	auto statBrdLos1 = lexiconPtr->getLos( statBoard1 );
	auto statBrdLos2 = lexiconPtr->getLos( statBoard2 );
	SetRouteByLeg( lset.semalegs[0] );
	SetRouteByLeg( lset.semalegs[1] );
	lexiconPtr->reset_all_changed();
	auto inums1 = statBrdLos1->get_Subitems<Info_Number>();
	auto inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() );
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE.empty() );
	const auto & tdescr = SetTrainOnStrip( spanStrips.front() );

	//появление должно вызывать реакцию на станционном табло
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );
}

void TC_FarBoardLoses::LEN_IntoStatByRoute()
{
	//установка поезда
	auto & statStrips = lset.statStrips;
	auto statBoard1 = lset.statBoards[1];
	auto statBoard2 = lset.statBoards[2];
	const auto & tdescr = SetTrainOnStrip( statStrips[2] ); //ставим поезд перед первым светофором составного маршрута
	lexiconPtr->reset_all_changed();
	auto statBrdLos1 = lexiconPtr->getLos( statBoard1 );
	auto statBrdLos2 = lexiconPtr->getLos( statBoard2 );
	auto inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.empty() );
	auto inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.empty() );

	//задаем составной маршрут:
	SetRouteByLeg( lset.semalegs[1] );
	SetRouteByLeg( lset.semalegs[2] );

	//задание маршрута должно вызывать реакцию на всех релевантных станционных табло
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );

	//сброс номера приводит к зачистке станционного табло
	CleanTrainOnStrip( true, statStrips[2] );
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE.empty() && inums2.back().lock_mark && !inums2.back().used_mark );

	//установка номера снова
	SetTrainOnStrip( statStrips[2] );
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );

	//заезд поезда
	auto brdStripLos = lexiconPtr->getLos( statStrips[4] );
	brdStripLos->setUsed( StateLTS::ACTIVE );
	CleanTrainOnStrip( false, statStrips[2] );
	lexiconPtr->markAsChanged( statStrips[4] );
	auto nextStripLos = lexiconPtr->getLos( statStrips[5] );
	nextStripLos->setUsed( StateLTS::ACTIVE );
	lexiconPtr->markAsChanged( statStrips[5] );
	SetTrainOnStrip( statStrips[5] );
	indirLosesNotifier->DirectToIndirect();

	brdStripLos->setUsed( StateLTS::PASSIVE );
	lexiconPtr->markAsChanged( statStrips[4] );
	indirLosesNotifier->DirectToIndirect();
	//для первого маршрута остается замыкание:
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() && inums1.back().lock_mark && !inums1.back().used_mark );
	//для второго маршрута номер сохраняется:
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );
}

void TC_FarBoardLoses::LEN_IntoStatByStrip()
{
	auto & statBoard1 = lset.statBoards[1];
	auto & statBoard2 = lset.statBoards[2];
	auto statBrdLos1 = lexiconPtr->getLos( statBoard1 );
	auto statBrdLos2 = lexiconPtr->getLos( statBoard2 );
	SetRouteByLeg( lset.semalegs[1] );
	SetRouteByLeg( lset.semalegs[2] );
	lexiconPtr->reset_all_changed();
	auto inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() );
	auto inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE.empty() );
	const auto & tdescr = SetTrainOnStrip( lset.statStrips[2] );

	//появление должно вызывать реакцию на релевантных станционных табло
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );
}

void TC_FarBoardLoses::LEN_StatToSpanByRoute()
{
	//установка поезда
	auto & statStrips = lset.statStrips;
	auto statBoard1 = lset.statBoards[2];
	auto statBoard2 = lset.statBoards[3];
	auto spanBoard = lset.rspanBoard;
	const auto & tdescr = SetTrainOnStrip( statStrips[5] ); //ставим поезд перед первым светофором составного маршрута
	lexiconPtr->reset_all_changed();
	auto statBrdLos1 = lexiconPtr->getLos( statBoard1 );
	auto statBrdLos2 = lexiconPtr->getLos( statBoard2 );
	auto spanBrdLos = lexiconPtr->getLos( spanBoard );
	auto inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.empty() );
	auto inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.empty() );
	auto inums3 = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums3.empty() );

	//задаем составной маршрут:
	SetRouteByLeg( lset.semalegs[2] );
	SetRouteByLeg( lset.semalegs[3] );

	//задание маршрута должно вызывать реакцию на всех релевантных табло
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );
	inums3 = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums3.size() == 3 && inums3.front().traitsPE == tdescr && !inums3.front().lock_mark && !inums3.back().used_mark &&
		inums3.front().forecast_mark );

	//сброс номера приводит к зачистке всех табло
	CleanTrainOnStrip( true, statStrips[5] );
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE.empty() && inums2.back().lock_mark && !inums2.back().used_mark );
	inums3 = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums3.size() == 3 && inums3.front().traitsPE.empty() && !inums3.front().lock_mark && !inums3.front().used_mark &&
		!inums3.front().forecast_mark );

	//установка номера снова
	SetTrainOnStrip( statStrips[5] );
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );
	inums3 = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums3.size() == 3 && inums3.front().traitsPE == tdescr && !inums3.front().lock_mark && !inums3.back().used_mark &&
		inums3.front().forecast_mark );

	//заезд поезда
	auto brdStripLos = lexiconPtr->getLos( statStrips[7] );
	brdStripLos->setUsed( StateLTS::ACTIVE );
	CleanTrainOnStrip( false, statStrips[5] );
	lexiconPtr->markAsChanged( statStrips[7] );
	auto nextStripLos = lexiconPtr->getLos( statStrips[8] );
	nextStripLos->setUsed( StateLTS::ACTIVE );
	lexiconPtr->markAsChanged( statStrips[8] );
	SetTrainOnStrip( statStrips[8] );
	indirLosesNotifier->DirectToIndirect();

	brdStripLos->setUsed( StateLTS::PASSIVE );
	lexiconPtr->markAsChanged( statStrips[7] );
	indirLosesNotifier->DirectToIndirect();
	//для первого маршрута остается замыкание:
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() && inums1.back().lock_mark && !inums1.back().used_mark );
	//для второго маршрута номер сохраняется:
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );
	//для третьего маршрута сохраняется прогнозируемое положене:
	inums3 = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums3.size() == 3 && inums3.front().traitsPE == tdescr && !inums3.front().lock_mark && !inums3.back().used_mark &&
		inums3.front().forecast_mark );
}

void TC_FarBoardLoses::LEN_StatToSpanByStrip()
{
	auto & statBoard1 = lset.statBoards[2];
	auto & statBoard2 = lset.statBoards[3];
	auto statBrdLos1 = lexiconPtr->getLos( statBoard1 );
	auto statBrdLos2 = lexiconPtr->getLos( statBoard2 );
	auto spanBrdLos = lexiconPtr->getLos( lset.rspanBoard );
	SetRouteByLeg( lset.semalegs[2] );
	SetRouteByLeg( lset.semalegs[3] );
	lexiconPtr->reset_all_changed();
	auto inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE.empty() );
	auto inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE.empty() );
	auto inums3 = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums3.size() == 3 && inums3.back().traitsPE.empty() );

	const auto & tdescr = SetTrainOnStrip( lset.statStrips[5] );

	//появление должно вызывать реакцию на релевантных табло
	inums1 = statBrdLos1->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums1.size() == 1 && inums1.back().traitsPE == tdescr && inums1.back().lock_mark && !inums1.back().used_mark );
	inums2 = statBrdLos2->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums2.size() == 1 && inums2.back().traitsPE == tdescr && inums2.back().lock_mark && !inums2.back().used_mark );
	inums3 = spanBrdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums3.size() == 3 && inums3.front().traitsPE == tdescr && !inums3.front().lock_mark && !inums3.front().used_mark && inums3.front().forecast_mark );
}

void TC_FarBoardLoses::BP_AmbivalentByRoute()
{
	auto & spanStrips = bpset.lspanStrips;
	auto rspanBoard = bpset.rspanBoard;
	auto tdescr = SetTrainOnStrip( spanStrips.back(), false );
	lexiconPtr->reset_all_changed();
	auto brdLos = lexiconPtr->getLos( rspanBoard );
	auto inums = brdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.empty() );
	SetRouteByLeg( bpset.semaleg );

	//задание маршрута должно вызывать реакцию на станционном табло
	inums = brdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 2 && inums.front().traitsPE == tdescr && !inums.front().lock_mark && !inums.front().used_mark && inums.front().forecast_mark );

	//сброс номера приводит к зачистке станционного табло
	CleanTrainOnStrip( true, spanStrips.back() );
	inums = brdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 2 && inums.front().traitsPE.empty() );

	//установка номера снова
	SetTrainOnStrip( spanStrips.back(), false );
	inums = brdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 2 && inums.front().traitsPE == tdescr && !inums.front().lock_mark && !inums.front().used_mark && inums.front().forecast_mark );

	//заезд поезда
	auto & statStrips = bpset.statStrips;
	auto statStripLos = lexiconPtr->getLos( statStrips[0] );
	statStripLos->setUsed( StateLTS::ACTIVE );
	CleanTrainOnStrip( false, spanStrips.back() );
	SetTrainOnStrip( statStrips[0], false );
	tdescr = SetTrainOnStrip( statStrips[1], false );
	lexiconPtr->markAsChanged( statStrips[0] );
	auto statStripLos2 = lexiconPtr->getLos( statStrips[1] );
	statStripLos2->setUsed( StateLTS::ACTIVE );
	lexiconPtr->markAsChanged( statStrips[1] );
	indirLosesNotifier->DirectToIndirect();
	inums = brdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 2 && inums.front().traitsPE == tdescr && !inums.front().lock_mark && !inums.front().used_mark && inums.front().forecast_mark );	
}

void TC_FarBoardLoses::BP_AmbivalentByStrip()
{
	auto & rspanBoard = bpset.rspanBoard;
	auto & spanStrips = bpset.lspanStrips;
	auto brdLos = lexiconPtr->getLos( rspanBoard );
	SetRouteByLeg( bpset.semaleg );
	lexiconPtr->reset_all_changed();
	auto inums = brdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 2 && inums.front().traitsPE.empty() );

	//появление должно вызывать реакцию на станционном табло
	const auto & tdescr = SetTrainOnStrip( spanStrips.back(), false );
	inums = brdLos->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( inums.size() == 2 && inums.front().traitsPE == tdescr && !inums.front().lock_mark && !inums.front().used_mark && inums.front().forecast_mark );
}