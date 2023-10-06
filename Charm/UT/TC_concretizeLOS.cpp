#include "stdafx.h"

#include "../StatBuilder/LogicFigure.h"
#include "../StatBuilder/LogicHead.h"
#include "../StatBuilder/LogicRoute.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/LogicSwitch.h"
#include "../StatBuilder/LogicLandscape.h"
#include "../Actor/LOSHead.h"
#include "../Actor/LOSLandscape.h"
#include "TC_concretizeLOS.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../helpful/ADProperties.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_concretizeLOS );

void TC_concretizeLOS::setUp()
{
    elem.clear();
    los.clear();
}
void TC_concretizeLOS::tearDown()
{
    while( !elem.empty() )
    {
        delete elem.back();
        elem.pop_back();
    }
    while( !los.empty() )
    {
        delete los.back();
        los.pop_back();
    }
}

void TC_concretizeLOS::fab_head()
{
	CLogicElementFactory LEF;
    elem.push_back( LEF.CreateObject( HEAD ) );
    elem.back()->SetType(TRAIN);

    elem.push_back( LEF.CreateObject( HEAD ) );
    elem.back()->SetType(SHUNTING);

    make_concrete_los();

    for( size_t i=0; i<elem.size(); ++i )
    {
        CLogicElement* e = elem[i];
        LOS_base * l = los[i];
        CPPUNIT_ASSERT ( e );
        CPPUNIT_ASSERT ( l );
//         CPPUNIT_ASSERT( e->GetType()==(unsigned)l->getTypeOfHead() );
    }
}

// void TC_concretizeLOS::fab_route()
// {
//     недоделка
// }

void TC_concretizeLOS::fab_strip()
{
	CLogicElementFactory LEF;
    elem.push_back( LEF.CreateObject( STRIP ) );
    elem.push_back( LEF.CreateObject( STRIP ) );
    elem.push_back( LEF.CreateObject( STRIP ) );
    static_cast<CLogicStrip*>(elem[0])->SetCapacity(Oddness::ODD, L"cap67");
	static_cast<CLogicStrip*>(elem[0])->SetCapacity(Oddness::EVEN, L"cap70");
	ADProperties::AttrSet adAttributes;
	adAttributes.set( ADProperties::Attrubute::PASSENGER );
	static_cast<CLogicStrip*>(elem[1])->SetADProperties( ADProperties( adAttributes, Oddness::UNDEF, WeightNorms() ) );

    make_concrete_los();

    for( size_t i=0; i<elem.size(); ++i )
    {
        CLogicStrip* e = static_cast<CLogicStrip*>(elem[i]);
        LOS_base * l = los[i];
        CPPUNIT_ASSERT ( e );
        CPPUNIT_ASSERT ( l );
        CPPUNIT_ASSERT( e->GetCapacity( Oddness::ODD )     == l->getCapacity( Oddness::ODD ) );
		CPPUNIT_ASSERT( e->GetCapacity( Oddness::EVEN )     == l->getCapacity( Oddness::EVEN ) );
		const auto & adProps = e->GetADProperties();
		CPPUNIT_ASSERT( adProps.HaveAttribute( ADProperties::Attrubute::PASSENGER ) == l->isWayPassenger() );
    }
}

void TC_concretizeLOS::fab_landscape()
{
	CLogicElementFactory LEF;
    elem.push_back( LEF.CreateObject( LANDSCAPE ) );
    elem.push_back( LEF.CreateObject( LANDSCAPE ) );

    {
        auto& one = *static_cast<CLogicLandscape*>(elem[0]);
        one.SetBound( rwCoord( 75, 300 ), rwCoord( 22, 197 ) );
        one.SetAxisName( L"Учкудук" );
        auto& two = *static_cast<CLogicLandscape*>(elem[1]);
        two.SetBound( rwCoord( 88, 200 ), rwCoord( 99,1200 ) );
        two.SetAxisName( L"Москва" );
    }

    make_concrete_los();
    for( size_t i=0; i<elem.size(); ++i )
    {
        CLogicLandscape* e = static_cast<CLogicLandscape*>(elem[i]);
        LOS_landscape * l = static_cast<LOS_landscape*>(los[i]);
        CPPUNIT_ASSERT ( e );
        CPPUNIT_ASSERT ( l );
        // вычитать логический объект
		rwCoord e_lcoord = e->GetBound( true );
		rwCoord e_rcoord = e->GetBound( false );
		wstring e_axisName = e->GetAxisName();
        // вычитать лос
        std::wstring ruler;
        rwCoord left;
        rwCoord right;
        l->getBound( &ruler, &left, &right );

        CPPUNIT_ASSERT( ruler== e_axisName );
        CPPUNIT_ASSERT( left == e_lcoord );
        CPPUNIT_ASSERT( right == e_rcoord );
    }
}

void TC_concretizeLOS::make_concrete_los()
{
    for( size_t k=0; k<elem.size(); ++k )
    {
        CLogicElement* e = elem[k];
        LOS_base* l = LOS_base::fabric_from_object( e );
        los.push_back( l );
    }
}