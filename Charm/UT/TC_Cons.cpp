#include "stdafx.h"
#include "../helpful/Cons.h"
#include "TC_Cons.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Cons );

/*
проверить класс Cons<Elem>
*/
void TC_Cons::single()
{
    Cons<std::string> x;
    CPPUNIT_ASSERT( x.empty() );
    CPPUNIT_ASSERT( x.size() == 0 );

    std::string a1("aaa");
    std::string a2("bbb");

    Cons<std::string> z;
    z += a1;
    z << a2;
    z += a1;

    CPPUNIT_ASSERT( !z.empty() );
    CPPUNIT_ASSERT( z.size() == 3 );
    CPPUNIT_ASSERT( z[0] == "aaa" );
    CPPUNIT_ASSERT( z[1] == "bbb" );
    CPPUNIT_ASSERT( z[2] == "aaa" );
}

void TC_Cons::multi()
{
    Cons<std::string> x;
    CPPUNIT_ASSERT( x.size() == 0 );

    Cons<std::string> y;
    y += std::string( "alfa" );
    y += std::string( "beta" );
    CPPUNIT_ASSERT( y.size() == 2 );

    x = y;
    CPPUNIT_ASSERT( x.size() == 2 );
    CPPUNIT_ASSERT( x == y );

    x += y;
    CPPUNIT_ASSERT( x.size() == 4 );

    y = Cons<std::string>();
    CPPUNIT_ASSERT( x.size() == 4 );
    CPPUNIT_ASSERT( y.empty() );

    CPPUNIT_ASSERT( x.front() == "alfa" );
    CPPUNIT_ASSERT( x[1] == "beta" );
    CPPUNIT_ASSERT( x[2] == "alfa" );
    CPPUNIT_ASSERT( x[3] == "beta" );
}

void TC_Cons::sharing()
{
    typedef std::string Atom;
    typedef Cons<Atom> ConsAtom;

    ConsAtom x;
    CPPUNIT_ASSERT( x.empty() );

    std::shared_ptr<      std::string> a = std::make_shared<std::string>( "alfa" );
    std::shared_ptr<const std::string> b = std::make_shared<const std::string>( "beta" );
    std::shared_ptr<      std::string> d = std::make_shared<std::string>( "delta" );

    x += a;
    x += b;
    x << d;
    
    CPPUNIT_ASSERT( x[0] == "alfa" );
    CPPUNIT_ASSERT( x[1] == "beta" );
    CPPUNIT_ASSERT( x[2] == "delta" );
}

void TC_Cons::esr()
{
    typedef Cons<EsrKit> ConsEsr;

    ConsEsr x;
    CPPUNIT_ASSERT( x.empty() );

    ConsEsr y;
    y += EsrKit(999);
    CPPUNIT_ASSERT( !y.empty() );
    CPPUNIT_ASSERT( y.front() == EsrKit(999) );

    EsrKit a1(111);
    EsrKit a2(200,300);
    EsrKit a3(333);
    ConsEsr z1;
    z1 += a1;
    z1 += a2;
    z1 += a3;

    ConsEsr z2;
    z2 += z1;
    CPPUNIT_ASSERT( z1 == z2 );

    ConsEsr w = z1;
    CPPUNIT_ASSERT( w.size() == 3 );
    CPPUNIT_ASSERT( w[0] == a1 );
    CPPUNIT_ASSERT( w[1] == a2 );
    CPPUNIT_ASSERT( w[2] == a3 );
}
