#include "stdafx.h"
#include "TC_conversion_LOS.h"
#include "../Actor/LOSBase.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Conversion_LOS );

const ELEMENT_CLASS ec_array[]={ HEAD, SWITCH, LOGIC_BOARD, COMMUTATOR, ROUTE, LANDSCAPE, STRIP, FIGURE, JOINT };

void TC_Conversion_LOS::board()
{
    for( const auto& ce : ec_array )
    {
        const LOS_base* cb = LOS_base::fabric_from_object(ce);
        const LOS_board* cs = cb->to_LOS_board();
        CPPUNIT_ASSERT( ( LOGIC_BOARD==ce ) == (nullptr != cs) );
        delete cb;
    }
}

void TC_Conversion_LOS::commutator()
{
    ELEMENT_CLASS     MY_ELEMENT = COMMUTATOR;
    typedef LOS_commutator MY_TYPE;

    for( const auto& ve : ec_array )
    {
        LOS_base* vb = LOS_base::fabric_from_object(ve);
        MY_TYPE* vs = vb->to_LOS_commutator();
        CPPUNIT_ASSERT( ( MY_ELEMENT==ve ) == (nullptr != vs) );
        delete vb;
    }
    for( const auto& ce : ec_array )
    {
        const LOS_base* cb = LOS_base::fabric_from_object(ce);
        const MY_TYPE* cs = cb->to_LOS_commutator();
        CPPUNIT_ASSERT( ( MY_ELEMENT==ce ) == (nullptr != cs) );
        delete cb;
    }
}

void TC_Conversion_LOS::landscape()
{
    ELEMENT_CLASS     MY_ELEMENT = LANDSCAPE;
    typedef LOS_landscape MY_TYPE;

    for( const auto& ve : ec_array )
    {
        LOS_base* vb = LOS_base::fabric_from_object(ve);
        MY_TYPE* vs = vb->to_LOS_landscape();
        CPPUNIT_ASSERT( ( MY_ELEMENT==ve ) == (nullptr != vs) );
        delete vb;
    }
    for( const auto& ce : ec_array )
    {
        const LOS_base* cb = LOS_base::fabric_from_object(ce);
        const MY_TYPE* cs = cb->to_LOS_landscape();
        CPPUNIT_ASSERT( ( MY_ELEMENT==ce ) == (nullptr != cs) );
        delete cb;
    }
}

void TC_Conversion_LOS::strip()
{
    ELEMENT_CLASS     MY_ELEMENT = STRIP;
    typedef LOS_strip MY_TYPE;

    for( const auto& ve : ec_array )
    {
        LOS_base* vb = LOS_base::fabric_from_object(ve);
        MY_TYPE* vs = vb->to_LOS_strip();
        CPPUNIT_ASSERT( ( MY_ELEMENT==ve ) == (nullptr != vs) );
        delete vb;
    }
    for( const auto& ce : ec_array )
    {
        const LOS_base* cb = LOS_base::fabric_from_object(ce);
        const MY_TYPE* cs = cb->to_LOS_strip();
        CPPUNIT_ASSERT( ( MY_ELEMENT==ce ) == (nullptr != cs) );
        delete cb;
    }
}
