#include "stdafx.h"

#include <sstream>
#include "TC_AramisNotify.h"
#include "../helpful/Attic.h"
#include "../helpful/Badge.h"
#include "../Fund/AramisNotify.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AramisNotify );

using namespace std;
using namespace boost::posix_time;


void TC_AramisNotify::test()
{
    const BadgeE bf( L"111", EsrKit(11) );
    const BadgeE bd( L"222", EsrKit(22) );
    const BadgeE bx( L"333", EsrKit(33) );

    std::wstring wf = L"formedPE";
    unsigned id = 765;

    attic::a_document doc;
    {
        AramisNotify aram;
        CPPUNIT_ASSERT( aram.empty() );
        aram.putForm(    bf, wf );
        aram.putForm(    bx, L"" ); // пустышка не будет записана
        aram.putDisform( bd, id );
        aram.serialize( doc );
        // проверить порядок форм и дисформ
        CPPUNIT_ASSERT( aram.firstIsForm() );
        aram.getForm();
        CPPUNIT_ASSERT( !aram.firstIsForm() );
        CPPUNIT_ASSERT( aram.firstIsDisform() );
        auto d = aram.getDisform();
        CPPUNIT_ASSERT( !aram.firstIsDisform() );
        CPPUNIT_ASSERT( aram.empty() );
    }
    {
        AramisNotify aram;
        CPPUNIT_ASSERT( aram.deserialize( doc ) );
        CPPUNIT_ASSERT( aram.firstIsForm() );
        auto f = aram.getForm();
        CPPUNIT_ASSERT( f.first == bf );
        CPPUNIT_ASSERT( f.second == wf );
        CPPUNIT_ASSERT( !aram.firstIsForm() );

        CPPUNIT_ASSERT( aram.firstIsDisform() );
        auto d = aram.getDisform();
        CPPUNIT_ASSERT( d.first == bd );
        CPPUNIT_ASSERT( d.second == id );
        CPPUNIT_ASSERT( !aram.firstIsDisform() );
        CPPUNIT_ASSERT( aram.empty() );
    }
}



