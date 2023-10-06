#include "stdafx.h"
#include "TC_IndirectLoses.h"
#include "../Fund/Lexicon.h"
#include "../Fund/IndirectLosesNotifier.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LinkManager.h"
#include "BuildTrainDescr.h"
#include "../Fund/TrainPlacesReview.h"
#include "../Actor/InfoNumber.h"

using namespace std;


CPPUNIT_TEST_SUITE_REGISTRATION( TC_IndirectLoses );

void TC_IndirectLoses::setUp()
{
    TC_Project::setUp();
	lexiconPtr.reset( new Lexicon() );
	auto pstam = spanDoc.GetStationManager();
	const auto & spanCode = EsrKit( 100, 200 );
	SetSpanEsr( &spanDoc, spanCode );
	MakeObjectsBasis();
	tplcReviewer.reset( new TrainPlacesReview() );
	indirLosesNotifier.reset( new IndirectLosesNotifier( L"", logdoc.get(), *lexiconPtr, *tplcReviewer, list <CLogicDocument *>() ) );
}

void TC_IndirectLoses::tearDown()
{
    TC_Project::tearDown();
}

void TC_IndirectLoses::MakeObjectsBasis()
{
	projdoc->Include( &spanDoc );
	lexiconPtr->reserve( 8 );
	MakeStationObjects();
	MakeSpanObjects();
}

void TC_IndirectLoses::TrainsPlacingNotify( attic::a_node trainsNode, const time_t * timPtr )
{
	const auto & changedStrips = tplcReviewer->PlacesChangeNotify( trainsNode, timPtr );
	for ( const auto & stripBdg : changedStrips )
	{
		auto lstrip = lexiconPtr->LogElBy( stripBdg );
		indirLosesNotifier->ObjectStateNotify( lstrip );
	}
}

void TC_IndirectLoses::MakeStationObjects()
{
	//генераци€ (вместе с лосами)
	statStrip1 = BuildObject( STRIP );
    CPPUNIT_ASSERT( lexiconPtr->append( statStrip1, EsrCode() ) );
	statStrip2 = BuildObject( STRIP );
    CPPUNIT_ASSERT( lexiconPtr->append( statStrip2, EsrCode() ) );
	statBoard1 = BuildObject( LOGIC_BOARD, true, logdoc.get() );
    CPPUNIT_ASSERT( lexiconPtr->append( statBoard1, EsrCode() ) );
	statBoard2 = BuildObject( LOGIC_BOARD, true, logdoc.get() );
    CPPUNIT_ASSERT( lexiconPtr->append( statBoard2, EsrCode() ) );

	//св€зывание
	CLinkManager * plim = logdoc->GetLinkManager();
	CLink statBoardLink1 = statBoard1->BuildLink( EsrCode() );
	CLink statBoardLink2 = statBoard2->BuildLink( EsrCode() );
	plim->Join( statStrip1, statBoardLink1 );
	plim->Join( statStrip2, statBoardLink2 );
}

void TC_IndirectLoses::MakeSpanObjects()
{
	//генераци€ (вместе с лосами)
	for ( unsigned int k = 0; k < 3; ++k )
	{
		spanStrips.push_back( BuildObject( STRIP, true, &spanDoc ) );
        CPPUNIT_ASSERT( lexiconPtr->append( spanStrips.back(), EsrCode( &spanDoc ) ) );
	}
	spanBoard = BuildObject( LOGIC_BOARD, true, &spanDoc );
    CPPUNIT_ASSERT( lexiconPtr->append( spanBoard, EsrCode( &spanDoc ) ) );

	//св€зывание
	CLinkManager * plim = logdoc->GetLinkManager();
	CLink spanBoardLink = spanBoard->BuildLink( EsrCode( &spanDoc ) );
	for ( auto ss : spanStrips )	
		plim->Join( ss, spanBoardLink );
}

void TC_IndirectLoses::SingleEntryOnStation()
{
	attic::a_document numdoc;
	TrainDescr tdescr = BuildTrain( 25, L"11-22-33", L"", 65, 99, 9, L"2:10" );
	auto nodesPair = CreateInfoDoc( numdoc, tdescr, 1 );

	MakeTrainNode( nodesPair.second, statStrip1, true );
	TrainsPlacingNotify( nodesPair.first );

	auto statBoardLos = lexiconPtr->getLos( statBoard1 );
	TestPresence( statBoardLos, 0 );
	statBoardLos = lexiconPtr->getLos( statBoard2 );
	TestAbsence( statBoardLos, 0 );
}

void TC_IndirectLoses::DoubleEntryOnStation()
{
	attic::a_document numdoc;
	TrainDescr tdescr = BuildTrain( 25, L"11-22-33", L"", 65, 99, 9, L"2:10" );
	auto nodesPair = CreateInfoDoc( numdoc, tdescr, 1 );
	MakeTrainNode( nodesPair.second, statStrip1, true );
	MakeTrainNode( nodesPair.second, statStrip2, true );
	TrainsPlacingNotify( nodesPair.first );

	auto statBoardLos = lexiconPtr->getLos( statBoard1 );
	TestPresence( statBoardLos, 0 );
	statBoardLos = lexiconPtr->getLos( statBoard2 );
	TestPresence( statBoardLos, 0 );
}

void TC_IndirectLoses::MultiEntryOnSpan()
{
	attic::a_document numdoc;
	TrainDescr tdescr = BuildTrain( 25, L"11-22-33", L"", 65, 99, 9, L"2:10" );
	auto nodesPair = CreateInfoDoc( numdoc, tdescr, 1 );
	for ( auto ss : spanStrips )
		MakeTrainNode( nodesPair.second, ss, true );
	TrainsPlacingNotify( nodesPair.first );

	auto spanBoardLos = lexiconPtr->getLos( spanBoard );
	TestAbsence( spanBoardLos, 0 );
	TestAbsence( spanBoardLos, 1 );
	TestPresence( spanBoardLos, 2 );
}

void TC_IndirectLoses::MultiEntryEverywhere()
{
	attic::a_document numdoc;
	TrainDescr tdescr = BuildTrain( 25, L"11-22-33", L"", 65, 99, 9, L"2:10" );
	auto nodesPair = CreateInfoDoc( numdoc, tdescr, 1 );
	for ( auto ss : spanStrips )
		MakeTrainNode( nodesPair.second, ss, true );
	MakeTrainNode( nodesPair.second, statStrip1, true );
	MakeTrainNode( nodesPair.second, statStrip2, true );
	TrainsPlacingNotify( nodesPair.first );

	auto statBoardLos = lexiconPtr->getLos( statBoard1 );
	TestPresence( statBoardLos, 0 );
	statBoardLos = lexiconPtr->getLos( statBoard2 );
	TestPresence( statBoardLos, 0 );
	auto spanBoardLos = lexiconPtr->getLos( spanBoard );
	TestAbsence( spanBoardLos, 0 );
	TestAbsence( spanBoardLos, 1 );
	TestPresence( spanBoardLos, 2 );
}

void TC_IndirectLoses::ReEntryDoesNotChange()
{
    attic::a_document numdoc;
	TrainDescr tdescr = BuildTrain( 25, L"11-22-33", L"", 65, 99, 9, L"2:10" );
    auto nodesPair = CreateInfoDoc( numdoc, tdescr, 1 );
    for ( auto ss : spanStrips )
        MakeTrainNode( nodesPair.second, ss, true );
    MakeTrainNode( nodesPair.second, statStrip1, true );
    MakeTrainNode( nodesPair.second, statStrip2, true );

    // сначала раскладка поездов об€зательно изменит объекты
    lexiconPtr->reset_all_changed();
	time_t trainAppMoment = time( nullptr );
    TrainsPlacingNotify( nodesPair.first, &trainAppMoment );
    CPPUNIT_ASSERT( !lexiconPtr->noneChanges() );

    // повторна€ раскладка уже не приводит к изменени€м
    lexiconPtr->reset_all_changed();
    TrainsPlacingNotify( nodesPair.first, &trainAppMoment );
    CPPUNIT_ASSERT( lexiconPtr->noneChanges() );
}

void TC_IndirectLoses::TestPresence( LOS_base * lboard, unsigned int ind ) const
{
	auto brdItems = lboard->get_Subitems<Info_Number>();
	CPPUNIT_ASSERT( !brdItems.empty() );
	CPPUNIT_ASSERT( ind < brdItems.size() );
	CPPUNIT_ASSERT( brdItems[ind].traitsPE != TrainDescr() );
}

void TC_IndirectLoses::TestAbsence( LOS_base * lboard, unsigned int ind ) const
{
	auto brdItems = lboard->get_Subitems<Info_Number>();
	if( ind < brdItems.size() )
		CPPUNIT_ASSERT( brdItems[ind].traitsPE == TrainDescr() );
}