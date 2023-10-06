#include "stdafx.h"

#include "TC_Platforms.h"
#include "../helpful/Attic.h"
#include "../Hem/Platforms.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Platforms );

using namespace std;

void TC_Platforms::test()
{
    std::wstring text = 
        L"<Platforms>"
        L"  <Span from='123456' to='112233'>"
        L"    <Platform esr='022222' name='Two'/>"
        L"    <Platform esr='011111' name='One'/>"
        L"  </Span>"
        L"  <Span from='765432' to='554433'>"
        L"    <Platform esr='033333' name='Three'/>"
        L"    <Platform esr='044444' name='Four'/>"
        L"  </Span>"
        L"<Platforms>";
    attic::a_document doc;
    doc.load_wide( text );

    Platforms platforms;
    platforms.deserialize( doc.document_element() );
    CPPUNIT_ASSERT( platforms.contain( EsrKit(1111) ) );
    CPPUNIT_ASSERT( platforms.contain( EsrKit(2222) ) );
    CPPUNIT_ASSERT( platforms.contain( EsrKit(3333) ) );
    CPPUNIT_ASSERT( !platforms.contain( EsrKit(5555) ) );

    // ввод-вывод
    attic::a_document redoc("test");
    attic::a_node n = redoc.document_element().append_child("re");
    platforms.serialize( n );
    Platforms replat;
    replat.deserialize( n );

    CPPUNIT_ASSERT( replat.contain( EsrKit(1111) ) );
    CPPUNIT_ASSERT( replat.contain( EsrKit(2222) ) );
    CPPUNIT_ASSERT( replat.contain( EsrKit(3333) ) );
    CPPUNIT_ASSERT( !replat.contain( EsrKit(5555) ) );
}

