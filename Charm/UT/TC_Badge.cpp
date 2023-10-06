#include "stdafx.h"

#include "TC_Badge.h"
#include "../helpful/Badge.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Badge );

void TC_Badge::compare()
{
    BadgeE e1( L"", EsrKit(100) );
    BadgeE a1( L"AAA", EsrKit(100) );
    BadgeE b1( L"BBB", EsrKit(100) );
    BadgeE a2( L"AAA", EsrKit(200) );

    CPPUNIT_ASSERT( e1 < a1 );
    CPPUNIT_ASSERT( e1 < b1 );
    CPPUNIT_ASSERT( a1 < b1 );
    CPPUNIT_ASSERT( b1 < a2 );
}
