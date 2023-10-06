#include "stdafx.h"
#include "TC_LogicAction.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/Action.h"
#include "../StatBuilder/LogicAction.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LObjectManager.h"
#include "../StatBuilder/LinkManager.h"
#include "../StatBuilder/DocBundle.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/GObjectManager.h"
#include "ProjectDocumentTest.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LogicAction );

void TC_LogicAction::JointedStripChanging()
{
	//заводим два участка и стык между ними
	pair <CLogicElement *, CLogicElement *> lstrips;
	/*CLogicElement * ljoint =*/ BuildJoint( lstrips );
	lstrips.first->SetType( ISOLATED );
	lstrips.second->SetType( ISOLATED );

	//участок меняет тип с "изолированный" на "приближения"
	CLogicElement * lstrip = lstrips.first;
	wstring fname = lstrip->GetName();
	LogElemPtr lstrip_before( lstrip->Clone() );
	LogElemPtr lstrip_after( lstrip->Clone() );
	lstrip_after->SetType( APPROACHING );
	set <const CLogicElement *> all_joints;
	plom->GetObjects( JOINT, logdoc.get(), all_joints );
	CPPUNIT_ASSERT( all_joints.size() == 1 );
	boost::scoped_ptr <Action> lact( new LogicAction( lstrip_after.get(), lstrip_before.get(), logdoc.get() ) );
	lact->Execute( true ); //изменение стрипа не должно отражаться на связанных стыках
	plom->GetObjects( JOINT, logdoc.get(), all_joints );
	CPPUNIT_ASSERT( all_joints.size() == 1 );

	//проверим наличие логического стрипа
	BadgeE strip_a_bdg( fname, EsrCode() );
	const CLogicElement * curstrip_a = plom->GetObjectByBadge <const CLogicElement *>( strip_a_bdg );
	CPPUNIT_ASSERT( curstrip_a != 0 );
	CPPUNIT_ASSERT( curstrip_a->GetType() == APPROACHING );
}

void TC_LogicAction::GLinkedStripChanging()
{
	//заводим один участок
	CLogicElement * lstrip = BuildObject( STRIP );
	lstrip->SetType( ISOLATED );

	//привязываем логический участок к графическому
	GraphElemPtr gstrip_sample = FastCreate( STRIP );
	CGraphicElement * gstrip = gstrip_sample->Clone();
	const CLink & strip_a_link = lstrip->BuildLink( EsrCode() );
	gstrip->SetLogicLink( strip_a_link );
	pgom->AddObject( gstrip, gdoc.get() );

	//участок меняет тип с "изолированный" на "приближения"
	LogElemPtr lstrip_before( lstrip->Clone() );
	LogElemPtr lstrip_after( lstrip->Clone() );
	lstrip_after->SetType( APPROACHING );
	boost::scoped_ptr <Action> lact( new LogicAction( lstrip_after.get(), lstrip_before.get(), logdoc.get() ) );
	lact->Execute( true );

	//не пропала ли привязка
	CPPUNIT_ASSERT( gstrip->GetLogicLink() == strip_a_link );
}

void TC_LogicAction::JointedStripRemoving()
{
	CLogicElement * lstrip1 = BuildObject( STRIP );
	CLogicElement * lstrip2 = BuildObject( STRIP );
	CLogicElement * ljoint = BuildObject( JOINT );
	const CLink & joint_link = ljoint->BuildLink( EsrCode() );
	plim->Join( lstrip1, joint_link );
	plim->Join( lstrip2, joint_link );
	CPPUNIT_ASSERT( plim->IsLinked( lstrip1, joint_link ) );
	CPPUNIT_ASSERT( plim->IsLinked( lstrip2, joint_link ) );

	set <const CLogicElement *> all_joints;
	boost::scoped_ptr <Action> lact( new LogicAction( 0, lstrip1, logdoc.get() ) );
	lact->Execute( true ); //удаление стрипа не должно удалять связанные стыки
	plom->GetObjects( JOINT, logdoc.get(), all_joints );
	CPPUNIT_ASSERT( all_joints.size() == 1 );
}

void TC_LogicAction::ViewChangesOnHeadModify()
{
	projdoc->OpenAllPools();
	const CLogicElement * lleg = BuildLeg( SHUNTING, logdoc.get() );
	set <CLogicElement *> lheads;
	plom->GetObjects<CLogicElement *>( HEAD, logdoc.get(), lheads );
	CPPUNIT_ASSERT( lheads.size() == 1 );
	CLogicElement * lhead = *lheads.cbegin();
	LogElemPtr lheadClone( lhead->Clone() );
	lheadClone->SetName( lhead->GetName() + L"addString" );

	Action * laction = new LogicAction( lhead, lheadClone.get(), logdoc.get() );
	MacroAction mact;
	mact.Add( laction );
	auto vchanges = mact.GetViewChanges();
	CPPUNIT_ASSERT( vchanges->changed_lobjects.size() == 1 );
	auto changesSet = vchanges->changed_lobjects.cbegin()->second;
	CPPUNIT_ASSERT( changesSet.size() == 1 );
	const CLogicElement * changedLe = *changesSet.cbegin();
	CPPUNIT_ASSERT( changedLe->GetClass() == SEMALEG );
	auto leSubobjects = changedLe->GetSubObjects();
	auto headIt = find_if( leSubobjects.cbegin(), leSubobjects.cend(), [this, lhead]( const BadgeE & subBdg ){
		const CLogicElement * lelem = plom->GetObjectByBadge<const CLogicElement *>( subBdg );
		return lelem == lhead;
	} );
	CPPUNIT_ASSERT( headIt != leSubobjects.cend() );
}