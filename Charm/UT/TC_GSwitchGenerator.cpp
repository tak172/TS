#include "stdafx.h"
#include "TC_GSwitchGenerator.h"
#include "../StatBuilder/GSwitchGenerator.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/GraphicSwitch.h"
#include "../StatBuilder/GraphicStrip.h"
#include "../StatBuilder/GObjectManager.h"
#include "../StatBuilder/SwitchMemento.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GSwitchGenerator );

void TC_GSwitchGenerator::SwitchCreating()
{
	//пересекаем два участка
	GraphElemPtr strip_sample = FastCreate( STRIP );
	CGraphicElement * horstrip = strip_sample->Clone();
	horstrip->SetHotPoint( CGridPoint( 0, 1 ) );
	horstrip->SetSizes( 2, 0 );
	pgom->AddObject( horstrip, gdoc.get() );
	CGraphicElement * diagstrip = strip_sample->Clone();
	diagstrip->SetHotPoint( CGridPoint( 0, 0 ) );
	diagstrip->SetSizes( 1, 1 );
	pgom->AddObject( diagstrip, gdoc.get() );
	pgom->SetStatus( diagstrip, SELECTED, gdoc.get() );
	SwitchMemento switch_memento( gdoc.get() );

	//генерация стрелок
	CGSwitchGenerator swigen( gdoc.get() );
	swigen.MakeSwitches( NULL, ALL_STATUSES, &switch_memento );

	//проверка
	set <const CGraphicElement *> allstrips, allswitches;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), allswitches );
	CPPUNIT_ASSERT( allstrips.size() == 3 );
	CPPUNIT_ASSERT( allswitches.size() == 1 );
	CPPUNIT_ASSERT( (*allswitches.begin())->GetHotPoint() == CGridPoint( 1, 1 ) );
}

void TC_GSwitchGenerator::SwitchRecovering()
{
	//имеем стрелку в исходном состоянии
	CGraphicElement * init_switch = BuildSwitchKit( CGridPoint( 5, 5 ), CGridSize( -2, 0 ), 
		CGridSize( 2, 0 ), CGridSize( 1, 1 ) );
	init_switch->SetName( L"Switch name" );
	CGraphicSwitch * gswitch = static_cast <CGraphicSwitch *>( init_switch );
	CGraphicStrip * minus_strip = gswitch->GetSpecialStrip( CLink::MINUS );
	pgom->SetStatus( minus_strip, SELECTED, gdoc.get() );
	SwitchMemento switch_memento( gdoc.get() );

	//смещаем участок
	minus_strip->UnlinkFromAll();
	minus_strip->Move( CGridSize( -1, 0 ), false );

	//перегенерируем
	CGSwitchGenerator swigen( gdoc.get() );
	swigen.MakeSwitches( NULL, ALL_STATUSES, &switch_memento );
	swigen.Distribute();

	//проверка
	set <const CGraphicElement *> allstrips, allswitches;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), allswitches );
	CPPUNIT_ASSERT( allstrips.size() == 3 );
	CPPUNIT_ASSERT( allswitches.size() == 1 );
	const CGraphicElement * new_swtich = *allswitches.begin();
	CPPUNIT_ASSERT( new_swtich->GetHotPoint() == CGridPoint( 4, 5 ) );
	CPPUNIT_ASSERT( new_swtich->GetName() == L"Switch name" );
}

void TC_GSwitchGenerator::SwitchNotRecovering()
{
	//имеем стрелку в исходном состоянии
	CGraphicElement * init_switch = BuildSwitchKit( CGridPoint( 5, 5 ), CGridSize( -2, 0 ), 
		CGridSize( 2, 0 ), CGridSize( 1, 1 ) );
	init_switch->SetName( L"Switch name" );
	CGraphicSwitch * gswitch = static_cast <CGraphicSwitch *>( init_switch );
	CGraphicStrip * minus_strip = gswitch->GetSpecialStrip( CLink::MINUS );
	pgom->SetStatus( minus_strip, SELECTED, gdoc.get() );
	SwitchMemento switch_memento( gdoc.get() );

	//смещаем участок с разрушением стрелки и возвращаем его на место
	minus_strip->UnlinkFromAll();
	minus_strip->Move( CGridSize( 0, 1 ), false );
	CGSwitchGenerator swigen( gdoc.get() );
	swigen.MakeSwitches( NULL, ALL_STATUSES, &switch_memento );
	swigen.Distribute();

	//возврат
	SwitchMemento switch_memento2( gdoc.get() );
	minus_strip->Move( CGridSize( 0, -1 ), false );
	swigen.MakeSwitches( NULL, ALL_STATUSES, &switch_memento2 );
	swigen.Distribute();

	//проверка
	set <const CGraphicElement *> allstrips, allswitches;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), allswitches );
	CPPUNIT_ASSERT( allstrips.size() == 3 );
	CPPUNIT_ASSERT( allswitches.size() == 1 );
	const CGraphicElement * new_swtich = *allswitches.begin();
	CPPUNIT_ASSERT( new_swtich->GetHotPoint() == CGridPoint( 5, 5 ) );
	CPPUNIT_ASSERT( new_swtich->GetName().empty() );
}

void TC_GSwitchGenerator::OrthoSwitchNotRecreating()
{
	//имеем стрелку в исходном состоянии
	CGraphicElement * init_switch = BuildSwitchKit( CGridPoint( 5, 5 ), CGridSize( -2, 0 ), 
		CGridSize( 2, 0 ), CGridSize( 0, 2 ) );
	init_switch->SetName( L"Switch name" );
	CGraphicSwitch * gswitch = static_cast <CGraphicSwitch *>( init_switch );
	CGraphicStrip * minus_strip = gswitch->GetSpecialStrip( CLink::MINUS );
	pgom->SetStatus( minus_strip, SELECTED, gdoc.get() );
	SwitchMemento switch_memento( gdoc.get() );

	//смещаем участок с разрушением стрелки и возвращаем его на место
	CGSwitchGenerator swigen( gdoc.get() );
	minus_strip->UnlinkFromAll();
	minus_strip->Move( CGridSize( 0, 1 ), false );
	swigen.MakeSwitches( NULL, ALL_STATUSES, &switch_memento );
	swigen.Distribute();

	//возврат
	SwitchMemento switch_memento2( gdoc.get() );
	minus_strip->Move( CGridSize( 0, -1 ), false );
	swigen.MakeSwitches( NULL, ALL_STATUSES, &switch_memento2 );
	swigen.Distribute();

	//проверка
	set <const CGraphicElement *> allstrips, allswitches;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), allswitches );
	CPPUNIT_ASSERT( allstrips.size() == 2 );
	CPPUNIT_ASSERT( allswitches.empty() );
}

void TC_GSwitchGenerator::OrthoSwitchRecreating()
{
	//имеем стрелку в исходном состоянии
	CGraphicElement * init_switch = BuildSwitchKit( CGridPoint( 5, 5 ), CGridSize( -2, 0 ), 
		CGridSize( 2, 0 ), CGridSize( 0, 2 ) );
	init_switch->SetName( L"Switch name" );
	CGraphicSwitch * gswitch = static_cast <CGraphicSwitch *>( init_switch );
	CGraphicStrip * minus_strip = gswitch->GetSpecialStrip( CLink::MINUS );
	pgom->SetStatus( minus_strip, SELECTED, gdoc.get() );
	SwitchMemento switch_memento( gdoc.get() );

	//смещаем участок
	minus_strip->UnlinkFromAll();
	minus_strip->Move( CGridSize( -1, 0 ), false );

	//перегенерируем
	CGSwitchGenerator swigen( gdoc.get() );
	swigen.MakeSwitches( NULL, ALL_STATUSES, &switch_memento );
	swigen.Distribute();

	//проверка
	set <const CGraphicElement *> allstrips, allswitches;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), allswitches );
	CPPUNIT_ASSERT( allstrips.size() == 3 );
	CPPUNIT_ASSERT( allswitches.size() == 1 );
	const CGraphicElement * new_swtich = *allswitches.begin();
	CPPUNIT_ASSERT( new_swtich->GetHotPoint() == CGridPoint( 4, 5 ) );
	CPPUNIT_ASSERT( new_swtich->GetName() == L"Switch name" );
}

void TC_GSwitchGenerator::TriangleJunction()
{
	//имеем две стрелки, с двумя пересекающимися примыкающими участками
	CGraphicElement * switch1 = BuildSwitchKit( CGridPoint( 10, 12 ), CGridSize( -2, 0 ), 
		CGridSize( 1, 0 ), CGridSize( 2, -2 ) );
	/*CGraphicElement * switch2 =*/ BuildSwitchKit( CGridPoint( 13, 12 ), CGridSize( 2, 0 ), 
		CGridSize( -1, 0 ), CGridSize( -2, -2 ) );

	//смещаем один из участков для образования треугольного соединения
	CGraphicSwitch * gswitch = static_cast <CGraphicSwitch *>( switch1 );
	CGraphicStrip * minus_strip = gswitch->GetSpecialStrip( CLink::MINUS );
	pgom->SetStatus( minus_strip, SELECTED, gdoc.get() );
	SwitchMemento switch_memento( gdoc.get() );
	minus_strip->UnlinkFromAll();
	minus_strip->Move( CGridSize( -1, 0 ), false );

	//перегенерируем
	CGSwitchGenerator swigen( gdoc.get() );
	swigen.MakeSwitches( NULL, ALL_STATUSES, &switch_memento );
	swigen.Distribute();

	//проверка
	set <const CGraphicElement *> allstrips, allswitches;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), allstrips );
	pgom->GetObjects( SWITCH, ALL_STATUSES, gdoc.get(), allswitches );
	CPPUNIT_ASSERT( allstrips.size() == 6 );
	CPPUNIT_ASSERT( allswitches.size() == 2 );
}