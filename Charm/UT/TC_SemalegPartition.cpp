#include "stdafx.h"
#include "TC_SemalegPartition.h"
#include "LogicDocumentTest.h"
#include "ProjectDocumentTest.h"
#include "../StatBuilder/Router.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/SemadirDistribution.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LosesErsatz.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SemalegPartition );

using namespace std;

void TC_SemalegPartition::StationCase()
{
	projdoc->OpenAllPools();
	//светофоры четных и нечетных индексов направл€ем в противоположные стороны
	vector <CLogicElement *> legvec( 5 );
	for ( unsigned int k = 0; k < legvec.size(); ++k )
		legvec[k] = BuildLeg( TRAIN );
	CLogicElement * base_strip1 = 0, * plus_strip1 = 0, * minus_strip1 = 0;
	TC_Logic::BuildSwitchKit( base_strip1, plus_strip1, minus_strip1 );
	CLogicElement * base_strip2 = 0, * plus_strip2 = 0, * minus_strip2 = 0;
	TC_Logic::BuildSwitchKit( base_strip2, plus_strip2, minus_strip2 );

	//светофоры четных индексов:
	pair <CLogicElement *, CLogicElement *> strips0( plus_strip1, 0 );
	BuildJoint( strips0, legvec[0] );
	pair <CLogicElement *, CLogicElement *> strips2( strips0.second, 0 );
	BuildJoint( strips2, legvec[2] );
	pair <CLogicElement *, CLogicElement *> strips4( strips2.second, plus_strip2 );
	BuildJoint( strips4, legvec[4] );
	//светофоры нечетных индексов:
	pair <CLogicElement *, CLogicElement *> strips1( 0, minus_strip1 );
	BuildJoint( strips1, legvec[1] );
	pair <CLogicElement *, CLogicElement *> strips3( minus_strip2, strips1.first );
	BuildJoint( strips3, legvec[3] );
	logdoc->GetEditor()->StoreManipulations();

	//разделение мачт на станции осуществимо только при наличии маршрутов
	plom->GenerateRoutes( logdoc.get() );
	logdoc->GetEditor()->StoreManipulations();

	//раздел€ем
	SemadirDistrPtr semadistrPtr = plom->GetSemadirDistribution( logdoc.get() );
	set <const CLogicElement *> codirlegs = semadistrPtr->Colegs();
	set <const CLogicElement *> oppdirlegs = semadistrPtr->Opplegs();
	set <const CLogicElement *> undeflegs = semadistrPtr->Undeflegs();
	CPPUNIT_ASSERT( undeflegs.empty() );
	CPPUNIT_ASSERT( codirlegs.size() == 3 && oppdirlegs.size() == 2 ||
		codirlegs.size() == 2 && oppdirlegs.size() == 3 );
	CPPUNIT_ASSERT( !semadistrPtr->Undistributable() );
}

void TC_SemalegPartition::SpanCase()
{
	//формируем перегон с двум€ станци€ми
	unique_ptr<CLogicDocumentTest> stat1doc( new CLogicDocumentTest );
	projdoc->Include( stat1doc.get() );
	unique_ptr<CLogicDocumentTest> stat2doc( new CLogicDocumentTest );
	projdoc->Include( stat2doc.get() );
	unique_ptr<CLogicDocumentTest> spandoc( new CLogicDocumentTest );
	projdoc->Include( spandoc.get() );

	projdoc->OpenAllPools(); //вызов тестового класса

	CStationManager * pstam = projdoc->GetStationManager();
	SetStationEsr( stat1doc.get(), EsrKit( 1 ) );
	SetStationEsr( stat2doc.get(), EsrKit( 2 ) );
	SetSpanEsr( spandoc.get(), EsrKit( 1, 2 ) );

	//светофоры 0 и 1 индексов направл€ем в сторону станции 1, остальные - в сторону станции 2
	vector <CLogicElement *> legvec( 6 );
	for ( unsigned int k = 0; k < legvec.size(); ++k )
		legvec[k] = BuildLeg( TRAIN, spandoc.get() );
	
	//светофоры индексов 0 и 1
	pair <CLogicElement *, CLogicElement *> woleg_strips1( BuildObject( STRIP, true, stat2doc.get() ), 
		BuildObject( STRIP, true, spandoc.get() ) );
	BuildJoint( woleg_strips1, 0, stat2doc.get() ); //стык выноситс€ на станцию 2
	pair <CLogicElement *, CLogicElement *> span_strips0( woleg_strips1.second, 
		BuildObject( STRIP, true, spandoc.get() ) );
	BuildJoint( span_strips0, legvec[0], spandoc.get() ); //перегонный стык со светофором
	pair <CLogicElement *, CLogicElement *> span_strips1( span_strips0.second,
		BuildObject( STRIP, true, stat1doc.get() ) ); //участок выноситс€ на станцию 1
	BuildJoint( span_strips1, legvec[1], spandoc.get() ); //перегонный стык со светофором

	//светофоры индексов 2, 3, 4 и 5
	pair <CLogicElement *, CLogicElement *> span_strips2( BuildObject( STRIP, true, stat1doc.get() ),
		BuildObject( STRIP, true, spandoc.get() ) ); //участок выноситс€ на станцию 1
	BuildJoint( span_strips2, legvec[2], spandoc.get() ); //перегонный стык со светофором
	pair <CLogicElement *, CLogicElement *> span_strips3( span_strips2.second,
		BuildObject( STRIP, true, spandoc.get() ) );
	BuildJoint( span_strips3, legvec[3], spandoc.get() ); //перегонный стык со светофором
	pair <CLogicElement *, CLogicElement *> woleg_strips2( span_strips3.second,
		BuildObject( STRIP, true, stat2doc.get() ) );
	BuildJoint( woleg_strips2, 0, stat2doc.get() ); //стык выноситс€ на станцию 2

	pair <CLogicElement *, CLogicElement *> woleg_strips3( BuildObject( STRIP, true, stat1doc.get() ),
		BuildObject( STRIP, true, spandoc.get() ) );
	BuildJoint( woleg_strips3, 0, stat1doc.get() ); //стык выноситс€ на станцию 1
	pair <CLogicElement *, CLogicElement *> span_strips4( woleg_strips3.second, 
		BuildObject( STRIP, true, spandoc.get() ) );
	BuildJoint( span_strips4, legvec[4], spandoc.get() ); //перегонный стык со светофором
	pair <CLogicElement *, CLogicElement *> span_strips5( span_strips4.second,
		BuildObject( STRIP, true, stat2doc.get() ) ); //участок выноситс€ на станцию 2
	BuildJoint( span_strips5, legvec[5], spandoc.get() ); //перегонный стык со светофором

	projdoc->GetEditor()->StoreManipulations();

	//раздел€ем
	SemadirDistrPtr semadistrPtr = plom->GetSemadirDistribution( spandoc.get() );
	set <const CLogicElement *> codirlegs = semadistrPtr->Colegs();
	set <const CLogicElement *> oppdirlegs = semadistrPtr->Opplegs();
	set <const CLogicElement *> undeflegs = semadistrPtr->Undeflegs();
	CPPUNIT_ASSERT( undeflegs.empty() );
	CPPUNIT_ASSERT( codirlegs.size() == 2 && oppdirlegs.size() == 4 ||
		codirlegs.size() == 4 && oppdirlegs.size() == 2 );
	CPPUNIT_ASSERT( !semadistrPtr->Undistributable() );
}

void TC_SemalegPartition::StationUnresolvedConflict()
{
	//создаем конфликтную ситуацию дл€ распределени€ мачт
	projdoc->OpenAllPools();
	vector <CLogicElement *> legvec( 5 );
	for ( unsigned int k = 0; k < legvec.size(); ++k )
		legvec[k] = BuildLeg( TRAIN );

	pair <CLogicElement *, CLogicElement *> strips0;
	BuildJoint( strips0, legvec[0] );
	pair <CLogicElement *, CLogicElement *> strips1;
	BuildJoint( strips1, legvec[1] );
	pair <CLogicElement *, CLogicElement *> strips2( strips1.second, 0 );
	BuildJoint( strips2, legvec[2] );
	pair <CLogicElement *, CLogicElement *> strips3( strips2.second, 0 );
	BuildJoint( strips3, legvec[3] );
	plim->Join( legvec[4], strips0.second->BuildLink( EsrCode(), CLink::FROM ) );
	plim->Join( legvec[4], strips0.first->BuildLink( EsrCode(), CLink::TO ) );
	/*CLogicElement * lswitch =*/ TC_Logic::BuildSwitchKit( strips0.second, strips1.first, strips3.second );

	//разделение мачт на станции осуществимо только при наличии маршрутов
	plom->GenerateRoutes( logdoc.get() );
	logdoc->GetEditor()->StoreManipulations();

	//раздел€ем
	SemadirDistrPtr semadistrPtr = plom->GetSemadirDistribution( logdoc.get() );
	set <const CLogicElement *> codirlegs = semadistrPtr->Colegs();
	set <const CLogicElement *> oppdirlegs = semadistrPtr->Opplegs();
	set <const CLogicElement *> undeflegs = semadistrPtr->Undeflegs();
	CPPUNIT_ASSERT( codirlegs.empty() && oppdirlegs.empty() && undeflegs.size() == 5 );
	CPPUNIT_ASSERT( semadistrPtr->Undistributable() );
}

void TC_SemalegPartition::StationResolvedConflict()
{
	//создаем конфликтную ситуацию дл€ распределени€ мачт
	projdoc->OpenAllPools();
	vector <CLogicElement *> legvec( 5 );
	for ( unsigned int k = 0; k < legvec.size(); ++k )
		legvec[k] = BuildLeg( TRAIN );

	pair <CLogicElement *, CLogicElement *> strips0;
	BuildJoint( strips0, legvec[0] );
	pair <CLogicElement *, CLogicElement *> strips1;
	BuildJoint( strips1, legvec[1] );
	CLogicStrip * lstrip = static_cast <CLogicStrip *>( strips1.first );
	lstrip->SetDirectionConflict( true );
	pair <CLogicElement *, CLogicElement *> strips2( strips1.second, 0 );
	BuildJoint( strips2, legvec[2] );
	pair <CLogicElement *, CLogicElement *> strips3( strips2.second, 0 );
	BuildJoint( strips3, legvec[3] );
	plim->Join( legvec[4], strips0.second->BuildLink( EsrCode(), CLink::FROM ) );
	plim->Join( legvec[4], strips0.first->BuildLink( EsrCode(), CLink::TO ) );
	/*CLogicElement * lswitch =*/ TC_Logic::BuildSwitchKit( strips0.second, strips1.first, strips3.second );

	//разделение мачт на станции осуществимо только при наличии маршрутов
	plom->GenerateRoutes( logdoc.get() );
	logdoc->GetEditor()->StoreManipulations();

	//раздел€ем
	SemadirDistrPtr semadistrPtr = plom->GetSemadirDistribution( logdoc.get() );
	set <const CLogicElement *> codirlegs = semadistrPtr->Colegs();
	set <const CLogicElement *> oppdirlegs = semadistrPtr->Opplegs();
	set <const CLogicElement *> undeflegs = semadistrPtr->Undeflegs();
	CPPUNIT_ASSERT( codirlegs.size() == 4 );
	CPPUNIT_ASSERT( oppdirlegs.size() == 1 );
	CPPUNIT_ASSERT( undeflegs.empty() );
	CPPUNIT_ASSERT( !semadistrPtr->Undistributable() );
}

void TC_SemalegPartition::AlienLegOnRoute()
{
	projdoc->OpenAllPools();
	vector <CLogicElement *> legvec( 2 );
	for ( unsigned int k = 0; k < legvec.size(); ++k )
		legvec[k] = BuildLeg( TRAIN );

	pair <CLogicElement *, CLogicElement *> strips0;
	BuildJoint( strips0, legvec[0] ); //стык в начале
	pair <CLogicElement *, CLogicElement *> strips1;
	BuildJoint( strips1, legvec[1] ); //стык в конце
	pair <CLogicElement *, CLogicElement *> strips2( strips0.second, strips1.first );
	BuildJoint( strips2 ); //стык посередине

	//чужа€ мачта
	boost::scoped_ptr <CLogicDocumentTest> aliendocPtr( new CLogicDocumentTest );
	const EsrKit & native_code = EsrCode();
	EsrKit alienCode( native_code.getTerm() + 1 );
	SetStationEsr( aliendocPtr.get(), alienCode );
	CLogicElement * alienLeg = BuildLeg( TRAIN, aliendocPtr.get() );
	plim->Join( alienLeg, strips1.first->BuildLink( EsrCode(), CLink::FROM ) );
	plim->Join( alienLeg, strips1.second->BuildLink( EsrCode(), CLink::TO ) );

	//разделение мачт на станции осуществимо только при наличии маршрутов
	plom->GenerateRoutes( logdoc.get() );
	logdoc->GetEditor()->StoreManipulations();

	//раздел€ем
	SemadirDistrPtr semadistrPtr = plom->GetSemadirDistribution( logdoc.get() );
	set <const CLogicElement *> codirlegs = semadistrPtr->Colegs();
	set <const CLogicElement *> oppdirlegs = semadistrPtr->Opplegs();
	set <const CLogicElement *> undeflegs = semadistrPtr->Undeflegs();
	CPPUNIT_ASSERT( codirlegs.size() == 2 );
	CPPUNIT_ASSERT( oppdirlegs.empty() );
	CPPUNIT_ASSERT( undeflegs.empty() );
	CPPUNIT_ASSERT( !semadistrPtr->Undistributable() );
}

void TC_SemalegPartition::DistributeAfterSemaUnjoin()
{
	CLogicDocumentTest alidoc;
	projdoc->Include( &alidoc );
    // ToDo: Ћогично ли вызывать .getTerm()?
	SetSpanEsr( &alidoc, EsrKit( EsrCode().getTerm(), EsrCode().getTerm() + 1 ) );
	projdoc->OpenAllPools();
	CLogicElement * spanStrip = BuildObject( STRIP, true, &alidoc );

	//формируем стрелку с п€тью светофорами возле нее:
	CLogicElement * baseStrip = nullptr, * plusStrip = nullptr, * minusStrip = nullptr;
	TC_Logic::BuildSwitchKit( baseStrip, plusStrip, minusStrip );
	vector <HEAD_TYPE> htypes;
	htypes.push_back( TRAIN );
	htypes.push_back( INVITATION );
	vector <HEAD_TYPE> htypes2;
	htypes2.push_back( TRAIN );
	htypes2.push_back( SHUNTING );
	CLogicElement * lleg_e = TC_Logic::BuildLeg( htypes2 );
	CLogicElement * lleg_d = TC_Logic::BuildLeg( htypes2 );
	CLogicElement * lleg_c = TC_Logic::BuildLeg( SHUNTING );
	CLogicElement * lleg_b = TC_Logic::BuildLeg( SHUNTING );
	CLogicElement * lleg_a = TC_Logic::BuildLeg( htypes );
	
	pair <CLogicElement *, CLogicElement *> strips12( spanStrip, nullptr );
	TC_Logic::BuildJoint( strips12, lleg_a );
	pair <CLogicElement *, CLogicElement *> strips23( strips12.second, baseStrip );
	TC_Logic::BuildJoint( strips23, lleg_b );
	pair <CLogicElement *, CLogicElement *> strips45( plusStrip, nullptr );
	TC_Logic::BuildJoint( strips45, lleg_c );
	pair <CLogicElement *, CLogicElement *> strips76( nullptr, minusStrip );
	TC_Logic::BuildJoint( strips76, lleg_d );
	pair <CLogicElement *, CLogicElement *> strips78( strips76.first, nullptr );
	TC_Logic::BuildJoint( strips78, lleg_e );

	//перед распределением должны быть созданы маршруты
	plom->GenerateRoutes( logdoc.get() );
	logdoc->GetEditor()->StoreManipulations();
	set <const CLogicElement *> lroutes;
	plom->GetObjects<const CLogicElement *>( ROUTE, logdoc.get(), lroutes );

	//распредел€ем мачты
	SemadirDistrPtr semadistrPtr = plom->GetSemadirDistribution( logdoc.get() );
	set <const CLogicElement *> codirlegs = semadistrPtr->Colegs();
	set <const CLogicElement *> oppdirlegs = semadistrPtr->Opplegs();
	set <const CLogicElement *> undeflegs = semadistrPtr->Undeflegs();
	CPPUNIT_ASSERT( codirlegs.size() == 4 );
	CPPUNIT_ASSERT( oppdirlegs.size() == 1 );
	CPPUNIT_ASSERT( *oppdirlegs.begin() == lleg_d );
	
	//разъедин€ем мачту
	plim->UnlinkFromAll( lleg_b, STRIP );
	logdoc->GetEditor()->StoreManipulations();
	plom->GetObjects<const CLogicElement *>( ROUTE, logdoc.get(), lroutes );
	semadistrPtr = plom->GetSemadirDistribution( logdoc.get() );
	codirlegs = semadistrPtr->Colegs();
	oppdirlegs = semadistrPtr->Opplegs();
	undeflegs = semadistrPtr->Undeflegs();
	CPPUNIT_ASSERT( codirlegs.size() == 2 );
	CPPUNIT_ASSERT( codirlegs.find( lleg_a ) != codirlegs.cend() );
	CPPUNIT_ASSERT( codirlegs.find( lleg_e ) != codirlegs.cend() );
	CPPUNIT_ASSERT( oppdirlegs.size() == 1 );
	CPPUNIT_ASSERT( oppdirlegs.find( lleg_d ) != oppdirlegs.cend() );
	CPPUNIT_ASSERT( undeflegs.size() == 2 );
	CPPUNIT_ASSERT( undeflegs.find( lleg_b ) != undeflegs.cend() );
	CPPUNIT_ASSERT( undeflegs.find( lleg_c ) != undeflegs.cend() );
}