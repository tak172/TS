#include "stdafx.h"

#include "TC_Canopy_Valet.h"
#include "../Fund/Valet.h"
#include "../SenseOfXml/SenseOfXml.h"

using namespace std;
using namespace NsCanopy;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_CanopyValet );

void TC_CanopyValet::test_request()
{
    Valet valet(42);
    Valet::DocumentPtr req;
    req = valet.pop_request();
    CPPUNIT_ASSERT( !req );
    valet.push_request( GetAll( valet.getIdStr() ) );
    valet.push_request( GetAll( valet.getIdStr() ) );
    req = valet.pop_request();
    CPPUNIT_ASSERT( req );
    req = valet.pop_request();
    CPPUNIT_ASSERT( req );
    req = valet.pop_request();
    CPPUNIT_ASSERT( !req );
}

void TC_CanopyValet::test_answer()
{
    Valet::DocumentPtr a1( new attic::a_document );
    Valet::DocumentPtr a2( new attic::a_document );
    a1->load_utf8("<One/>");
    a2->load_utf8("<Two/>");

    Valet valet(23);
    Valet::DocumentPtr req;
    req = valet.pop_answer();
    CPPUNIT_ASSERT( !req );

    valet.push_answer(a1);
    valet.push_answer(a2);

    req = valet.pop_answer();
    CPPUNIT_ASSERT( req == a1 );
    req = valet.pop_answer();
    CPPUNIT_ASSERT( req == a2 );
    req = valet.pop_answer();
    CPPUNIT_ASSERT( !req );
}

void TC_CanopyValet::test_undo()
{
    Valet::DocumentPtr u1( new attic::a_document );
    Valet::DocumentPtr u2( new attic::a_document );
    u1->load_utf8("<One/>");
    u2->load_utf8("<Two/>");

    Valet valet(38);
    Valet::DocumentPtr req;
    req = valet.pop_undo();
    CPPUNIT_ASSERT( !req );
    valet.push_undo(u1);
    valet.push_undo(u2);
    req = valet.pop_undo();
    CPPUNIT_ASSERT( req == u2 );
    req = valet.pop_undo();
    CPPUNIT_ASSERT( req == u1 );
    req = valet.pop_undo();
    CPPUNIT_ASSERT( !req );
}
