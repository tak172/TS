#include "stdafx.h"
#include "TC_DocBundle.h"
#include "../StatBuilder/DocBundle.h"
#include "LogicDocumentTest.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/LObjectKeeper.h"
#include "../StatBuilder/GObjectKeeper.h"
#include "DocBundleTest.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_DocBundle );

void TC_DocBundle::Creating()
{
	IGraphic * pGraphic = 0;
	ILogic * pLogic = 0;
	DocBundle docbundle( pGraphic, pLogic );
	CPPUNIT_ASSERT( docbundle.AssertValidness() );
}

void TC_DocBundle::Adding()
{
	IGraphic * pGraphic = 0;
	ILogic * pLogic = 0;
	DocBundleTest docbundle( pGraphic, pLogic );
	boost::scoped_ptr <CGraphicDocument> graphdoc1( new CGraphicDocumentTest() );
	CGObjectKeeper gkeeper1( graphdoc1.get() );
	boost::scoped_ptr <CGraphicDocument> graphdoc2( new CGraphicDocumentTest() );
	CGObjectKeeper gkeeper2( graphdoc2.get() );
	docbundle.Add( graphdoc1.get(), &gkeeper1 );
	docbundle.Add( graphdoc2.get(), &gkeeper2 );
	CPPUNIT_ASSERT( docbundle.Find( graphdoc1.get() ) );
	CPPUNIT_ASSERT( docbundle.Find( graphdoc2.get() ) );
	CPPUNIT_ASSERT( docbundle.KeeperExist( graphdoc1.get() ) );
	CPPUNIT_ASSERT( docbundle.KeeperExist( graphdoc2.get() ) );
}

void TC_DocBundle::Removing()
{
	IGraphic * pGraphic = 0;
	ILogic * pLogic = 0;
	DocBundleTest docbundle( pGraphic, pLogic );
	boost::scoped_ptr <CLogicDocument> logdoc1( new CLogicDocumentTest() );
	CLObjectKeeper lkeeper1( logdoc1.get() );
	boost::scoped_ptr <CLogicDocument> logdoc2( new CLogicDocumentTest() );
	CLObjectKeeper lkeeper2( logdoc2.get() );
	docbundle.Add( logdoc1.get(), &lkeeper1 );
	docbundle.Add( logdoc2.get(), &lkeeper2 );
	docbundle.Remove( logdoc1.get() );
	CPPUNIT_ASSERT( docbundle.KeeperExist( logdoc2.get() ) );
	docbundle.Remove( logdoc2.get() );
	const list <CLogicDocument *> & logdocs = docbundle.Documents<CLogicDocument *>();
	CPPUNIT_ASSERT( logdocs.empty() );
	CPPUNIT_ASSERT( !docbundle.KeeperExist( logdoc1.get() ) );
	CPPUNIT_ASSERT( !docbundle.KeeperExist( logdoc2.get() ) );
}