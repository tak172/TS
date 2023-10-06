#include "stdafx.h"

#include <sstream>
#include "TC_StateLTS.h"
#include "../helpful/StateLTS.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_StateLTS );

StateLTS a,p,u,e;

void TC_StateLTS::setUp()
{
    a.set_any(StateLTS::ACTIVE);
    p.set_any(StateLTS::PASSIVE);
    u.set_any(StateLTS::UNDEF);
    e.set_any(StateLTS::ELIM);
}

void TC_StateLTS::construct()
{
    //ctr
    StateLTS v;

    CPPUNIT_ASSERT( StateLTS::UNDEF  ==v );
    CPPUNIT_ASSERT( StateLTS::ACTIVE ==a );
    CPPUNIT_ASSERT( StateLTS::PASSIVE==p );
    CPPUNIT_ASSERT( StateLTS::UNDEF  ==u );
    CPPUNIT_ASSERT( StateLTS::ELIM   ==e );

    CPPUNIT_ASSERT( StateLTS::ACTIVE ==StateLTS('A') );
    CPPUNIT_ASSERT( StateLTS::PASSIVE==StateLTS('p') );
    CPPUNIT_ASSERT( StateLTS::UNDEF  ==StateLTS('?') );
    CPPUNIT_ASSERT( StateLTS::UNDEF  ==StateLTS('w') ); // любое неизвестное
    CPPUNIT_ASSERT( StateLTS::ELIM   ==StateLTS('e') );

    CPPUNIT_ASSERT( a.is(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( p.is(StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( u.is(StateLTS::UNDEF) );
    CPPUNIT_ASSERT( e.is(StateLTS::ELIM) );
}

void TC_StateLTS::assignment()
{
    CPPUNIT_ASSERT( !a.asgn(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( p.asgn(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( u.asgn(StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( !e.asgn(StateLTS::ACTIVE) );

    CPPUNIT_ASSERT( StateLTS::PASSIVE == (a=StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( StateLTS::ACTIVE == (p=StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( StateLTS::ACTIVE == (u=StateLTS::ACTIVE) );

    CPPUNIT_ASSERT( StateLTS::ELIM == (e=StateLTS::ACTIVE) );
    CPPUNIT_ASSERT( StateLTS::ELIM == (e=StateLTS::PASSIVE) );
    CPPUNIT_ASSERT( StateLTS::ELIM == (e=StateLTS::UNDEF) );
}

void TC_StateLTS::operate()
{
    CPPUNIT_ASSERT( StateLTS::PASSIVE == a.invert() );
    CPPUNIT_ASSERT( StateLTS::ACTIVE  == p.invert() );
    CPPUNIT_ASSERT( StateLTS::UNDEF   == u.invert() );
    CPPUNIT_ASSERT( StateLTS::ELIM    == e.invert() );

    CPPUNIT_ASSERT( StateLTS::ACTIVE == (a|p) );
    CPPUNIT_ASSERT( StateLTS::ACTIVE == (a|u) );
    CPPUNIT_ASSERT( StateLTS::UNDEF  == (p|u) );
    CPPUNIT_ASSERT( StateLTS::ACTIVE == (a|e) );
    CPPUNIT_ASSERT( StateLTS::UNDEF  == (p|e) );

    CPPUNIT_ASSERT( StateLTS::PASSIVE == (a&p) );
    CPPUNIT_ASSERT( StateLTS::UNDEF   == (a&u) );
    CPPUNIT_ASSERT( StateLTS::PASSIVE == (p&u) );
    CPPUNIT_ASSERT( StateLTS::UNDEF   == (a&e) );
    CPPUNIT_ASSERT( StateLTS::PASSIVE == (p&e) );

    CPPUNIT_ASSERT( StateLTS::ACTIVE == (a^p) );
    CPPUNIT_ASSERT( StateLTS::UNDEF  == (a^u) );
    CPPUNIT_ASSERT( StateLTS::UNDEF  == (p^u) );
    CPPUNIT_ASSERT( StateLTS::UNDEF  == (a^e) );
    CPPUNIT_ASSERT( StateLTS::UNDEF  == (p^e) );
}
