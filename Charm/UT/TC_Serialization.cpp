#include "stdafx.h"
#include "TC_Serialization.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/SBSerialization.h"
#include "../StatBuilder/GViewSkin.h"
#include "../Actor/Blinker.h"
#include "../helpful/Informator.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/GObjectManager.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GObjectKeeper.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Serialization );

using namespace std;

TC_Serialization::~TC_Serialization()
{
	CInformator::Shutdowner();
}

void TC_Serialization::GraphicSerialize()
{
	CGridPoint cursor_point( 5, 5 );
	
	//генерация
	set <const CGraphicElement *> inpset = GenerateGraphicSet();

	//сериализация
	HGLOBAL globalHandle = SBSerialization::MoveToHGlobal( inpset, cursor_point );
	
	//десериализация
	Blinker * blinker = Blinker::instance();
	GViewSkinPtr viewSkinPtr( new GViewSkin( *blinker ) );
	const list <CGraphicElement *> & glist = SBSerialization::MoveFromHGlobal( globalHandle, cursor_point, viewSkinPtr );

	//проверка эквивалентности
	typedef set <const CGraphicElement *> CT1;
	typedef list <CGraphicElement *> CT2;
	typedef bool (CGraphicElement::*EqFun)( const CGraphicElement * ) const;
	bool equality = GraphicTestEquality<CT1, CT2, EqFun>( inpset, glist, &CGraphicElement::Equal );
	CPPUNIT_ASSERT( equality );

	for( CGraphicElement * ge : glist )
		delete ge;
}

set <const CGraphicElement *> TC_Serialization::GenerateGraphicSet() const
{
	set <const CGraphicElement *> gset;

	//участок
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * gstrip = gstrip_sample->Clone();
	gstrip->SetHotPoint( CGridPoint( 10, 10 ) );
	gstrip->SetSizes( 15, 92 );
	pgom->AddObject( gstrip, gdoc.get() );
	gset.insert( gstrip );

	//стрелка
	GraphElemPtr gswitch_sample = FastCreate( SWITCH );
	CGraphicElement * gswitch = gswitch_sample->Clone();
	gswitch->SetHotPoint( CGridPoint( 82, 20 ) );
	gswitch->SetName( L"Switch name" );
	pgom->AddObject( gswitch, gdoc.get() );
	gset.insert( gswitch );

	//светофор
	CGraphicElement * gsemaleg = BuildSemalegKit( CGridPoint( 30, 40 ), 3 );
	gsemaleg->SetLogicLink( CLink( SEMALEG, BadgeE( L"semalink", EsrKit( 10 ) ) ) );
	gset.insert( gsemaleg );
	const vector <CGraphicElement *> & gheads = gsemaleg->GetSubObjects();
	CPPUNIT_ASSERT( gheads.size() == 3 );
	for ( unsigned int k = 0; k < gheads.size(); ++k )
	{
		switch( k )
		{
		case 0:
			gheads[k]->SetType( TRAIN );
			break;
		case 1:
			gheads[k]->SetType( REPEAT );
			break;
		case 2:
			gheads[k]->SetType( CHOKE );
			gheads[k]->SetLogicLink( CLink( HEAD, BadgeE( L"headlink", EsrKit( 5 ) ) ) );
			break;
		}
		gset.insert( gheads[k] );
	}
	return gset;
}

void TC_Serialization::LogicSerialize()
{
	//генерация
	list <LogElemPtr> source_list = GenerateLogicList();
	list <const CLogicElement *> input_list;
	for( LogElemPtr lePtr : source_list )
		input_list.push_back( lePtr.get() );
	EsrKit inputEsr( 5, 200 );
	//сериализация
	HGLOBAL globalHandle = SBSerialization::MoveToHGlobal( inputEsr, input_list );
	EsrKit outputEsr;
	//десериализация
	list <LogElemPtr> result_list = SBSerialization::MoveFromHGlobal( globalHandle, outputEsr );
	//проверка эквивалентности
	CPPUNIT_ASSERT( inputEsr == outputEsr );
	typedef list <const CLogicElement *> ListType;
	typedef bool (CLogicElement::*EqFun)( const CLogicElement * ) const;
	bool equality = LogicTestEquality<ListType, ListType, EqFun>( input_list, input_list, &CLogicElement::Equal );
	CPPUNIT_ASSERT( equality );
}

list <LogElemPtr> TC_Serialization::GenerateLogicList() const
{
	list <LogElemPtr> logicList;
	LogElemPtr stripPtr( CLogicElementFactory().CreateObject( STRIP ) );
	stripPtr->SetName( L"String name" );
	stripPtr->SetType( ISOLATED );
	logicList.push_back( stripPtr );
	LogElemPtr switchPtr( CLogicElementFactory().CreateObject( SWITCH ) );
	switchPtr->SetName( L"Switch name" );
	vector <CLink> switch_links;
	switch_links.push_back( CLink( STRIP, BadgeE( L"stripLink", EsrKit( 5 ) ), CLink::PLUS ) );
	switch_links.push_back( CLink( STRIP, BadgeE( L"stripLink2", EsrKit( 5 ) ), CLink::MINUS ) );
	switch_links.push_back( CLink( STRIP, BadgeE( L"stripLink3", EsrKit( 5 ) ), CLink::BASE ) );
	switchPtr->SetLinks( switch_links );
	logicList.push_back( switchPtr );
	LogElemPtr semalegPtr( CLogicElementFactory().CreateObject( SEMALEG ) );
	semalegPtr->SetName( L"Switch name" );
	vector <CLink> semaleg_links;
	semaleg_links.push_back( CLink( HEAD, BadgeE( L"headLink", EsrKit( 5 ) ) ) );
	semaleg_links.push_back( CLink( HEAD, BadgeE( L"headLink2", EsrKit( 5 ) ) ) );
	semalegPtr->SetLinks( semaleg_links );
	logicList.push_back( semalegPtr );
	return logicList;
}