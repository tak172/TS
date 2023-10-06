#include "stdafx.h"
#include "TC_GraphicValidator.h"
#include "../StatBuilder/GraphicElement.h"
#include "LogicDocumentTest.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/GLoadResult.h"
#include "ProjectDocumentTest.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GraphicValidator );

void TC_GraphicValidator::CorrectUnmatchedSwitchesBDef()
{
	CorrectUnmatchedSwitches( true );
}

void TC_GraphicValidator::CorrectUnmatchedSwitchesBUndef()
{
	CorrectUnmatchedSwitches( false );
}

void TC_GraphicValidator::CorrectUnmatchedSwitches( bool base_defined )
{
	CGraphicElement * gswitch = TC_Graphic::BuildSwitchKit( CGridPoint( 10, 10 ) );
	const auto & all_glinks = gswitch->GetLinks();
	CLogicElement * base_strip = nullptr, * plus_strip = nullptr, * minus_strip = nullptr;
	CLogicElement * lswitch = TC_Logic::BuildSwitchKit( base_strip, plus_strip, minus_strip );
	gswitch->SetLogicLink( lswitch->BuildLink( TC_Logic::EsrCode() ) );
	for ( const auto & glink : all_glinks )
	{
		if ( glink.glink_type == CLink::PLUS )
			glink.glink_ptr->SetLogicLink( minus_strip->BuildLink( TC_Logic::EsrCode() ) );
		else if ( glink.glink_type == CLink::MINUS )
			glink.glink_ptr->SetLogicLink( plus_strip->BuildLink( TC_Logic::EsrCode() ) );
		else
		{
			if ( base_defined )
				glink.glink_ptr->SetLogicLink( base_strip->BuildLink( TC_Logic::EsrCode() ) );
		}
	}
	gdoc->FinalizeLoading();
	CPPUNIT_ASSERT( gdoc->GetLoadResult()->ChangedByLoading() );
}

void TC_GraphicValidator::NoCorrectLSwitchWOLinks()
{
	CGraphicElement * gswitch = TC_Graphic::BuildSwitchKit( CGridPoint( 10, 10 ) );
	CLogicElement * lswitch = BuildObject( SWITCH );
	gswitch->SetLogicLink( lswitch->BuildLink( TC_Logic::EsrCode() ) );
	gdoc->FinalizeLoading();
	CPPUNIT_ASSERT( !gdoc->GetLoadResult()->ChangedByLoading() );
}

void TC_GraphicValidator::SemaWithLegNAlienHeadsLogic()
{
	CGraphicElement * gleg = TC_Graphic::BuildSemalegKit( CGridPoint( 10, 10 ), 3 );
	const auto & glinks = gleg->GetLinks( HEAD );
	vector <CGraphicElement *> gheads;
	for ( const auto & glink : glinks )
		gheads.push_back( glink.glink_ptr );
	vector <HEAD_TYPE> ltypes( 3, HEAD_TYPE::TRAIN );
	CLogicElement * lleg1 = TC_Logic::BuildLeg( ltypes );
	const auto & headLinks1 = lleg1->GetLinks( HEAD );
	CLogicElement * lleg2 = TC_Logic::BuildLeg( ltypes );
	const auto & headLinks2 = lleg2->GetLinks( HEAD );

	const auto & leg2Link = lleg2->BuildLink( TC_Logic::EsrCode() );
	gleg->SetLogicLink( leg2Link ); //логическая мачта от второго светофора
	gheads[0]->SetLogicLink( headLinks1[0] ); //голова от первого светофора
	gheads[1]->SetLogicLink( headLinks2[1] ); //голова от второго светофора
	gheads[2]->SetLogicLink( headLinks1[2] ); //голова от первого светофора
	gdoc->FinalizeLoading();
	CPPUNIT_ASSERT( gdoc->GetLoadResult()->ChangedByLoading() );
	CPPUNIT_ASSERT( gleg->GetLogicLink() == lleg2->BuildLink( TC_Logic::EsrCode() ) );
	CPPUNIT_ASSERT( gheads[0]->GetLogicLink().obj_badge.empty() );
	CPPUNIT_ASSERT( gheads[1]->GetLogicLink() == headLinks2[1] );
	CPPUNIT_ASSERT( gheads[2]->GetLogicLink().obj_badge.empty() );
}

void TC_GraphicValidator::SemaWOLegOneHeadLogic()
{
	CGraphicElement * gleg = TC_Graphic::BuildSemalegKit( CGridPoint( 10, 10 ), 3 );
	const auto & glinks = gleg->GetLinks( HEAD );
	vector <CGraphicElement *> gheads;
	for ( const auto & glink : glinks )
		gheads.push_back( glink.glink_ptr );
	vector <HEAD_TYPE> ltypes( 3, HEAD_TYPE::TRAIN );
	CLogicElement * lleg = TC_Logic::BuildLeg( ltypes );
	const auto & headLinks = lleg->GetLinks( HEAD );
	gheads[1]->SetLogicLink( headLinks[1] ); //одна голова от второго светофора
	gdoc->FinalizeLoading();
	CPPUNIT_ASSERT( gdoc->GetLoadResult()->ChangedByLoading() );
	CPPUNIT_ASSERT( gleg->GetLogicLink() == lleg->BuildLink( TC_Logic::EsrCode() ) );
	CPPUNIT_ASSERT( gheads[0]->GetLogicLink().obj_badge.empty() );
	CPPUNIT_ASSERT( gheads[1]->GetLogicLink() == headLinks[1] );
	CPPUNIT_ASSERT( gheads[2]->GetLogicLink().obj_badge.empty() );
}

void TC_GraphicValidator::SemaWOLegManyHeadLogic()
{
	CGraphicElement * gleg = TC_Graphic::BuildSemalegKit( CGridPoint( 10, 10 ), 3 );
	const auto & glinks = gleg->GetLinks( HEAD );
	vector <CGraphicElement *> gheads;
	for ( const auto & glink : glinks )
		gheads.push_back( glink.glink_ptr );
	vector <HEAD_TYPE> train_types( 3, HEAD_TYPE::TRAIN );
	vector <HEAD_TYPE> shunt_types( 3, HEAD_TYPE::SHUNTING );
	CLogicElement * lleg1 = TC_Logic::BuildLeg( train_types );
	const auto & trainHeadLinks = lleg1->GetLinks( HEAD );
	CLogicElement * lleg2 = TC_Logic::BuildLeg( shunt_types );
	const auto & shuntHeadLinks = lleg2->GetLinks( HEAD );
	gheads[0]->SetLogicLink( shuntHeadLinks[0] ); //маневровая голова от второго светофора
	gheads[1]->SetLogicLink( shuntHeadLinks[1] ); //маневровая голова от второго светофора
	gheads[2]->SetLogicLink( trainHeadLinks[2] ); //поездная голова от первого светофора
	gdoc->FinalizeLoading();
	CPPUNIT_ASSERT( gdoc->GetLoadResult()->ChangedByLoading() );
	CPPUNIT_ASSERT( gleg->GetLogicLink() == lleg1->BuildLink( TC_Logic::EsrCode() ) );
	CPPUNIT_ASSERT( gheads[0]->GetLogicLink().obj_badge.empty() );
	CPPUNIT_ASSERT( gheads[1]->GetLogicLink().obj_badge.empty() );
	CPPUNIT_ASSERT( gheads[2]->GetLogicLink() == trainHeadLinks[2] );
}

void TC_GraphicValidator::SemaWOLegManyTrainHeadLogic()
{
	CGraphicElement * gleg = TC_Graphic::BuildSemalegKit( CGridPoint( 10, 10 ), 3 );
	const auto & glinks = gleg->GetLinks( HEAD );
	vector <CGraphicElement *> gheads;
	for ( const auto & glink : glinks )
		gheads.push_back( glink.glink_ptr );
	vector <HEAD_TYPE> ltypes( 1, HEAD_TYPE::TRAIN );
	CLogicElement * lleg1 = TC_Logic::BuildLeg( ltypes );
	const auto & headLinks1 = lleg1->GetLinks( HEAD );
	CLogicElement * lleg2 = TC_Logic::BuildLeg( ltypes );
	const auto & headLinks2 = lleg2->GetLinks( HEAD );
	gheads[0]->SetLogicLink( headLinks1[0] ); //голова от первого светофора
	gheads[1]->SetLogicLink( headLinks2[0] ); //голова от второго светофора
	gdoc->FinalizeLoading();
	auto loadresult = gdoc->GetLoadResult();
	CPPUNIT_ASSERT( gdoc->GetLoadResult()->ChangedByLoading() );
	CPPUNIT_ASSERT( gleg->GetLogicLink().obj_badge.empty() );
	CPPUNIT_ASSERT( gheads[0]->GetLogicLink().obj_badge.empty() );
	CPPUNIT_ASSERT( gheads[1]->GetLogicLink().obj_badge.empty() );
}