#include "stdafx.h"
#include "TC_Graphic.h"
#include "../StatBuilder/GraphicEditableFactory.h"
#include "../Actor/Blinker.h"
#include "../StatBuilder/GraphicElement.h"
#include "GraphicDocumentTest.h"
#include "../StatBuilder/GObjectManager.h"
#include "../helpful/Pilgrim.h"
#include "../helpful/TrainCharacteristics.h"

using namespace std;

TC_Graphic::TC_Graphic()
{
    setUp_helper();
}

TC_Graphic::~TC_Graphic()
{
    tearDown_helper();
}

void TC_Graphic::setUp_helper()
{	
	//инициализация GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL );
	gdoc.reset( new CGraphicDocumentTest );
	pgom = gdoc->GetGObjectManager();
}

void TC_Graphic::tearDown_helper()
{
	//все, что зависит от контекста, нужно уничтожать строго до его закрытия
    pgom = nullptr;
	gdoc.reset();
	Gdiplus::GdiplusShutdown( gdiplusToken );
	Blinker::Shutdowner();
	Pilgrim::Shutdowner(); 
    TrainCharacteristics::Shutdowner();
}

GraphElemPtr TC_Graphic::FastCreate( ELEMENT_CLASS elem_class ) const
{
	CGraphicEditableFactory GEF;
	return GraphElemPtr( GEF.CreateObject( elem_class, gdoc->GetSkin() ) );
}

CGraphicElement * TC_Graphic::BuildSemalegKit( const CGridPoint & hotpoint, unsigned int heads_num ) const
{
	GraphElemPtr gleg_prototype = FastCreate( SEMALEG );
	CGraphicElement * gleg = gleg_prototype->Clone();
	GraphElemPtr ghead_prototype = FastCreate( HEAD );
	for ( unsigned int k = 0; k < heads_num; ++k )
	{
		CGraphicElement * ghead = ghead_prototype->Clone();
		gleg->Join( ghead );
		pgom->AddObject( ghead, gdoc.get() );
	}
	gleg->SetHotPoint( hotpoint );
	pgom->AddObject( gleg, gdoc.get() );
	return gleg;
}

CGraphicElement * TC_Graphic::BuildSwitchKit( const CGridPoint & switch_hp, const CGridSize & base_size,
						const CGridSize & plus_size, const CGridSize & minus_size ) const
{
	GraphElemPtr gswitch_prototype = FastCreate( SWITCH );
	GraphElemPtr gstrip_prototype = FastCreate( STRIP );
	CGraphicElement * gswitch = gswitch_prototype->Clone();

	CGraphicElement * base_strip = gstrip_prototype->Clone();
	base_strip->SetHotPoint( switch_hp );
	base_strip->SetSizes( base_size.w, base_size.h );
	gswitch->Join( base_strip, CLink::BASE );
	CGraphicElement * plus_strip = gstrip_prototype->Clone();
	plus_strip->SetHotPoint( switch_hp );
	plus_strip->SetSizes( plus_size.w, plus_size.h );
	gswitch->Join( plus_strip, CLink::PLUS );
	gswitch->SetAngle( plus_strip->GetAngle() );
	CGraphicElement * minus_strip = gstrip_prototype->Clone();
	minus_strip->SetHotPoint( switch_hp );
	minus_strip->SetSizes( minus_size.w, minus_size.h );
	gswitch->Join( minus_strip, CLink::MINUS );

	gswitch->SetHotPoint( switch_hp );

	pgom->AddObject( gswitch, gdoc.get() );
	pgom->AddObject( base_strip, gdoc.get() );
	pgom->AddObject( plus_strip, gdoc.get() );
	pgom->AddObject( minus_strip, gdoc.get() );

	return gswitch;
}

void TC_Graphic::Select( ELEMENT_CLASS elclass ) const
{
	set <const CGraphicElement *> allelems;
	pgom->GetObjects( elclass, ALL_STATUSES, gdoc.get(), allelems );
	pgom->SetStatus( allelems, SELECTED, gdoc.get() );
}

void TC_Graphic::SelectAll() const
{
	pgom->SelectAll( gdoc.get() );
}