#include "stdafx.h"

#include <sstream>
#include "TC_clone_LOS.h"
#include "../Actor/LOSBase.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Clone_LOS );

using namespace std;


void TC_Clone_LOS::equivalence()
{
    boost::ptr_vector<LOS_base> v;
    v.push_back( LOS_base::fabric_from_object(HEAD) );
    v.push_back( LOS_base::fabric_from_object(SWITCH) );
    v.push_back( LOS_base::fabric_from_object(LOGIC_BOARD) );
    v.push_back( LOS_base::fabric_from_object(COMMUTATOR) );
    v.push_back( LOS_base::fabric_from_object(ROUTE) );
    v.push_back( LOS_base::fabric_from_object(LANDSCAPE) );
    v.push_back( LOS_base::fabric_from_object(STRIP) );
    v.push_back( LOS_base::fabric_from_object(FIGURE,FIGURE) );
	v.push_back( LOS_base::fabric_from_object(FIGURE,GRAPHIC_TEXT) );
	v.push_back( LOS_base::fabric_from_object(FIGURE,HTML_AREA) );
    v.push_back( LOS_base::fabric_from_object(JOINT) );

    for( unsigned i=0; i<v.size(); ++i )
        for( unsigned j=0; j<v.size(); ++j )
        {
            if ( i==j )
                CPPUNIT_ASSERT( v[i].isEqual(&v[j]) );
            else
                CPPUNIT_ASSERT( !v[i].isEqual(&v[j]) );
        }
}
