#include "stdafx.h"
#include "TC_Aligning.h"
#include "../StatBuilder/MultiEditor.h"
#include "GraphicDocumentTest.h"
#include "ManipulatorTest.h"
#include "../StatBuilder/GraphicElement.h"
#include "../StatBuilder/GObjectManager.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Aligning );

void TC_Aligning::SemaLeftAligning()
{
	CGridPoint left_point( 20, 40 );
	CGridPoint right_point( 150, 90 );
	CGraphicElement * left_leg = BuildSemalegKit( left_point, 2 );
	CGraphicElement * right_leg = BuildSemalegKit( right_point, 2 );
	SelectAll();
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );

	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	const vector <CGraphicElement *> & left_heads = left_leg->GetSubObjects();
	CPPUNIT_ASSERT( !left_heads.empty() );
	manPtr->Align( left_heads.back(), Manipulator::LEFT_BORDER );
	CPPUNIT_ASSERT( left_leg->GetHotPoint().x == right_leg->GetHotPoint().x );
}

void TC_Aligning::SemaTopAligning()
{
	CGridPoint top_point( 20, 40 );
	CGridPoint bottom_point( 150, 90 );
	CGraphicElement * top_leg = BuildSemalegKit( top_point, 2 );
	CGraphicElement * bottom_leg = BuildSemalegKit( bottom_point, 2 );
	SelectAll();
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );

	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	const vector <CGraphicElement *> & top_heads = top_leg->GetSubObjects();
	CPPUNIT_ASSERT( !top_heads.empty() );
	manPtr->Align( top_heads.back(), Manipulator::TOP_BORDER );
	CPPUNIT_ASSERT( top_leg->GetHotPoint().y == bottom_leg->GetHotPoint().y );
}

void TC_Aligning::CreateSetToAlign( vector <const CGraphicElement *> & alignvec )
{
	vector <GraphElemPtr> blanks;
	blanks.reserve( 5 );
	for ( unsigned int i = 0; i < 5; ++i )
	{
		GraphElemPtr gptr = FastCreate( FIGURE );
		gptr->SetSizes( i + 2, 5 - i );
		blanks.push_back( gptr );
	}
	blanks[0]->SetHotPoint( CGridPoint( 10, 20 ) );
	blanks[1]->SetHotPoint( CGridPoint( 20, 10 ) );
	blanks[2]->SetHotPoint( CGridPoint( 100, 100 ) );
	blanks[3]->SetHotPoint( CGridPoint( 110, 30 ) );
	blanks[4]->SetHotPoint( CGridPoint( 50, 60 ) );
	alignvec.clear();
	alignvec.reserve( blanks.size() );
	for ( auto geptr : blanks )
	{
		CGraphicElement * new_ge = geptr->Clone();
		alignvec.push_back( new_ge );
		pgom->AddObject( new_ge, gdoc.get(), SELECTED );
	}
}

void TC_Aligning::LeftAligning()
{
	//подготовка объектов
	vector <const CGraphicElement *> elems_to_align;
	CreateSetToAlign( elems_to_align );
	sort( elems_to_align.begin(), elems_to_align.end(), []( const CGraphicElement * ge1, const CGraphicElement * ge2 )
		{
			CGridRect objrect1, objrect2;
			ge1->GetObjectRect( objrect1 );
			ge2->GetObjectRect( objrect2 );
			return objrect1.topleft.x < objrect2.topleft.x;
	});
	CGridRect objrect;
	elems_to_align[0]->GetObjectRect( objrect );
	GRID_COORD left_border = objrect.topleft.x;

	//манипул€ци€
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->Align( elems_to_align[0], Manipulator::LEFT_BORDER );

	//проверка
	for ( auto ge : elems_to_align )
	{
		CGridRect objrect;
		ge->GetObjectRect( objrect );
		CPPUNIT_ASSERT( objrect.topleft.x == left_border );
	}
}

void TC_Aligning::RightAligning()
{
	//подготовка объектов
	vector <const CGraphicElement *> elems_to_align;
	CreateSetToAlign( elems_to_align );
	sort( elems_to_align.begin(), elems_to_align.end(), []( const CGraphicElement * ge1, const CGraphicElement * ge2 )
	{
		CGridRect objrect1, objrect2;
		ge1->GetObjectRect( objrect1 );
		ge2->GetObjectRect( objrect2 );
		return objrect1.topleft.x > objrect2.topleft.x;
	});
	CGridRect objrect;
	elems_to_align[0]->GetObjectRect( objrect );
	GRID_COORD right_border = objrect.rightbottom.x;

	//манипул€ци€
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->Align( elems_to_align[0], Manipulator::RIGHT_BORDER );

	//проверка
	for ( auto ge : elems_to_align )
	{
		CGridRect objrect;
		ge->GetObjectRect( objrect );
		CPPUNIT_ASSERT( objrect.rightbottom.x == right_border );
	}
}

void TC_Aligning::TopAligning()
{
	//подготовка объектов
	vector <const CGraphicElement *> elems_to_align;
	CreateSetToAlign( elems_to_align );
	sort( elems_to_align.begin(), elems_to_align.end(), []( const CGraphicElement * ge1, const CGraphicElement * ge2 )
	{
		CGridRect objrect1, objrect2;
		ge1->GetObjectRect( objrect1 );
		ge2->GetObjectRect( objrect2 );
		return objrect1.topleft.y < objrect2.topleft.y;
	});
	CGridRect objrect;
	elems_to_align[0]->GetObjectRect( objrect );
	GRID_COORD top_border = objrect.topleft.y;

	//манипул€ци€
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->Align( elems_to_align[0], Manipulator::TOP_BORDER );

	//проверка
	for ( auto ge : elems_to_align )
	{
		CGridRect objrect;
		ge->GetObjectRect( objrect );
		CPPUNIT_ASSERT( objrect.topleft.y == top_border );
	}
}

void TC_Aligning::BottomAligning()
{
	//подготовка объектов
	vector <const CGraphicElement *> elems_to_align;
	CreateSetToAlign( elems_to_align );
	sort( elems_to_align.begin(), elems_to_align.end(), []( const CGraphicElement * ge1, const CGraphicElement * ge2 )
	{
		CGridRect objrect1, objrect2;
		ge1->GetObjectRect( objrect1 );
		ge2->GetObjectRect( objrect2 );
		return objrect1.topleft.y > objrect2.topleft.y;
	});
	CGridRect objrect;
	elems_to_align[0]->GetObjectRect( objrect );
	GRID_COORD bottom_border = objrect.rightbottom.y;

	//манипул€ци€
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->Align( elems_to_align[0], Manipulator::BOTTOM_BORDER );

	//проверка
	for ( auto ge : elems_to_align )
	{
		CGridRect objrect;
		ge->GetObjectRect( objrect );
		CPPUNIT_ASSERT( objrect.rightbottom.y == bottom_border );
	}
}

void TC_Aligning::VerticalCentring()
{
	//подготовка объектов
	vector <const CGraphicElement *> elems_to_align;
	CreateSetToAlign( elems_to_align );

	//манипул€ци€
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->Align( elems_to_align[0], Manipulator::VERT_CENTRE );

	//проверка
	set <GRID_COORD> x_centers;
	for ( auto ge : elems_to_align )
	{
		const CGridPoint & central_pt = ge->GetCentralPoint();
		x_centers.insert( central_pt.x );
	}
	CPPUNIT_ASSERT( x_centers.size() == 1 );
}

void TC_Aligning::HorizontalCentring()
{
	//подготовка объектов
	vector <const CGraphicElement *> elems_to_align;
	CreateSetToAlign( elems_to_align );

	//манипул€ци€
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->Align( elems_to_align[0], Manipulator::HOR_CENTRE );

	//проверка
	set <GRID_COORD> y_centers;
	for ( auto ge : elems_to_align )
	{
		const CGridPoint & central_pt = ge->GetCentralPoint();
		y_centers.insert( central_pt.y );
	}
	CPPUNIT_ASSERT( y_centers.size() == 1 );
}

void TC_Aligning::VerticalDistribution()
{
	//подготовка объектов
	vector <const CGraphicElement *> elems_to_align;
	CreateSetToAlign( elems_to_align );

	//манипул€ци€
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->GapsAlign( Manipulator::VERT_DISTR );

	//проверка
	//сортировка элементов сверху вниз
	sort( elems_to_align.begin(), elems_to_align.end(), []( const CGraphicElement * ge1, const CGraphicElement * ge2 ){
		const CGridPoint & ctrPt1 = ge1->GetCentralPoint();
		const CGridPoint & ctrPt2 = ge2->GetCentralPoint();
		return ctrPt1.y < ctrPt2.y;
	});
	set <GRID_COORD> y_diffs;
	for ( unsigned int i = 0, j = i + 1; i < elems_to_align.size() && j < elems_to_align.size(); ++i, ++j )
	{
		const CGridPoint & central_pt1 = elems_to_align[i]->GetCentralPoint();
		const CGridPoint & central_pt2 = elems_to_align[j]->GetCentralPoint();
		y_diffs.insert( abs( central_pt1.y - central_pt2.y ) );
	}
	CPPUNIT_ASSERT( y_diffs.size() == 1 );
}

void TC_Aligning::HorizontalDistribution()
{
	//подготовка объектов
	vector <const CGraphicElement *> elems_to_align;
	CreateSetToAlign( elems_to_align );

	//манипул€ци€
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->GapsAlign( Manipulator::HOR_DISTR );

	//проверка
	//сортировка элементов слева направо
	sort( elems_to_align.begin(), elems_to_align.end(), []( const CGraphicElement * ge1, const CGraphicElement * ge2 ){
		const CGridPoint & ctrPt1 = ge1->GetCentralPoint();
		const CGridPoint & ctrPt2 = ge2->GetCentralPoint();
		return ctrPt1.x < ctrPt2.x;
	});
	set <GRID_COORD> x_diffs;
	for ( unsigned int i = 0, j = i + 1; i < elems_to_align.size() && j < elems_to_align.size(); ++i, ++j )
	{
		const CGridPoint & central_pt1 = elems_to_align[i]->GetCentralPoint();
		const CGridPoint & central_pt2 = elems_to_align[j]->GetCentralPoint();
		x_diffs.insert( abs( central_pt1.x - central_pt2.x ) );
	}
	CPPUNIT_ASSERT( x_diffs.size() == 1 );
}

void TC_Aligning::OverlappingBan()
{
	//вводим два участка, лежащие на одной горизонтальной линии
	GraphElemPtr gptr = FastCreate( STRIP );
	gptr->SetHotPoint( CGridPoint( 0, 0 ) );
	gptr->SetSizes( 5, 0 );
	CGraphicElement * refGelem = gptr->Clone();
	pgom->AddObject( refGelem, gdoc.get(), SELECTED );
	gptr->SetHotPoint( CGridPoint( 10, 0 ) );
	gptr->SetSizes( 5, 0 );
	pgom->AddObject( gptr->Clone(), gdoc.get(), SELECTED );

	//выравнивание
	Editor <GTraits> * geditor = gdoc->GetEditor();
	geditor->OpenPool( gdoc.get() );
	ManipulatorPtr manPtr = gdoc->GetEditor()->CreateManipulator<ManipulatorTest>( gdoc->GetView() );
	manPtr->Align( refGelem, Manipulator::LEFT_BORDER );

	//манипул€ци€ не должна быть выполнена, т.к. участки накладываютс€ друг на друга
	set <const CGraphicElement *> gstrips;
	pgom->GetObjects( STRIP, ALL_STATUSES, gdoc.get(), gstrips );
	CPPUNIT_ASSERT( gstrips.size() == 2 );
	CPPUNIT_ASSERT( any_of( gstrips.cbegin(), gstrips.cend(), []( const CGraphicElement * ge ){
		return ( ge->GetHotPoint() == CGridPoint( 0, 0 ) );
	} ) );
	CPPUNIT_ASSERT( any_of( gstrips.cbegin(), gstrips.cend(), []( const CGraphicElement * ge ){
		return( ge->GetHotPoint() == CGridPoint( 10, 0 ) );
	} ) );
	CPPUNIT_ASSERT( all_of( gstrips.cbegin(), gstrips.cend(), []( const CGraphicElement * ge ){
		double width = 0, height = 5;
		ge->GetSizes( width, height );
		return width == 5 && height == 0;
	} ) );
}
