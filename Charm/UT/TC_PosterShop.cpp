#include "stdafx.h"

#include "TC_PosterShop.h"
#include "../helpful/Attic.h"
#include "../helpful/Log.h"
#include "../Fund/PosterShop.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PosterShop );

void TC_PosterShop::tearDown()
{
}

static vector<BadgeE>& operator << ( vector<BadgeE>& v, const BadgeE& b )
{
    v.push_back(b);
    return v;
}

static vector<BadgeE>& operator << ( vector<BadgeE>& lhs, const vector<BadgeE>& rhs )
{
    lhs.insert( lhs.end(), rhs.begin(), rhs.end() );
    return lhs;
}

static void init( PosterShop &ps, const std::wstring& data )
{
    attic::a_document doc;
    doc.load_wide( data );
    ps.load(doc);
}

// явное указание объекта
void TC_PosterShop::direct()
{
    PosterShop ps;
    init( ps,
        L"<Poster delay='77'>"
        L"    <Place ESR_code='11011'>"
        L"        <It Badge='one' />"
        L"        <It Badge='ONE' delay='11' />"
        L"    </Place>"
        L"    <Place ESR_code='22022' delay='22' >"
        L"        <It Badge='TWO' delay='2' />"
        L"        <It Badge='two' />"
        L"    </Place>"
        L"</Poster>"
        );

    BadgeE b_1( L"one",EsrKit(11011) );
    BadgeE b11( L"ONE",EsrKit(11011) );
    BadgeE b_X( L"XXX",EsrKit(11011) );
    BadgeE b_2( L"two",EsrKit(22022) );
    BadgeE b22( L"TWO",EsrKit(22022) );
    BadgeE bYY( L"YYY",EsrKit(22022) );
    BadgeE bZZ( L"ZZZ",EsrKit(88888) );

    vector<BadgeE> all;
    all << b_1 << b11 << b_X << b_2 << b22 << bYY << bZZ;
    ps.collectPostable(all);

    vector<BadgeE> post = ps.getPostable();
    vector<BadgeE> need;
    need << b_1 << b11 << b_2 << b22;
    std::sort( post.begin(), post.end() );
    std::sort( need.begin(), need.end() );
    CPPUNIT_ASSERT( post == need );
}

// составное правило
void TC_PosterShop::compound()
{
    PosterShop ps;
    init( ps,
        L"<Poster delay='7'>"
        L"    <Compound  delay='7' >"
        L"        <Part idx='0' str='kk' />"
        L"        <Part idx='2' str='aa' />"
        L"        <Part idx='2' str='Bb' />"
        L"        <Part idx='2' str='zZ' />"
        L"        <Part idx='4' str='1'  />"
        L"        <Part idx='4' str='2'  />"
        L"        <Part idx='4' str=''  />"
        L"    </Compound>"
        L"</Poster>"
        );

    vector<BadgeE> suc; // подходящие объекты
    suc << BadgeE( L"kkaa1",EsrKit(22) )
        << BadgeE( L"kkaa2",EsrKit(22) )
        << BadgeE( L"kkaa", EsrKit(22) )
        << BadgeE( L"kkBb1",EsrKit(22) )
        << BadgeE( L"kkBb2",EsrKit(22) )
        << BadgeE( L"kkBb", EsrKit(22) );

    vector<BadgeE> rep; // повторные имена на других станциях
    rep << BadgeE( L"kkaa", EsrKit(333) )
        << BadgeE( L"kkaa", EsrKit(4444) )
        << BadgeE( L"kkBb1",EsrKit(4444) )
        << BadgeE( L"kkBb1",EsrKit(333) );

    vector<BadgeE> bad; // неподходящие имена
    bad << BadgeE( L"#kaa", EsrKit(333) )
        << BadgeE( L"kka#1",EsrKit(4444) )
        << BadgeE( L"kkaa222222",EsrKit(333) );

    vector<BadgeE> all;
    all << suc << bad << rep;
    ps.collectPostable(all);

    vector<BadgeE> post = ps.getPostable();
    std::sort( post.begin(), post.end() );

    vector<BadgeE> need;
    need << suc << rep;
    std::sort( need.begin(), need.end() );

    CPPUNIT_ASSERT( post == need );
}

// подстановка для составного правила
void TC_PosterShop::subs_for_compound()
{
    PosterShop ps;
    init( ps,
        L"<Poster delay='7'>"
        L"    <Substitution>"
        L"      <It of='xx' to='kkBb1'/>"
        L"      <It of='zz' to='kka#1'/>"
        L"    </Substitution>"
        L"    <Compound  delay='7' >"
        L"        <Part idx='0' str='kk' />"
        L"        <Part idx='2' str='aa' />"
        L"        <Part idx='2' str='Bb' />"
        L"        <Part idx='2' str='zZ' />"
        L"        <Part idx='4' str='1'  />"
        L"        <Part idx='4' str='2'  />"
        L"        <Part idx='4' str=''  />"
        L"    </Compound>"
        L"</Poster>"
        );

    vector<BadgeE> all;
    all << BadgeE( L"kkaa1",EsrKit(22) )
        << BadgeE( L"kkBb", EsrKit(22) )
        << BadgeE( L"xx", EsrKit(333) )
        << BadgeE( L"zz",EsrKit(333) );

    ps.collectPostable(all);

    vector<BadgeE> post = ps.getPostable();
    std::sort( post.begin(), post.end() );

    vector<BadgeE> out;
    out << BadgeE( L"kkaa1",EsrKit(22) )
        << BadgeE( L"kkBb", EsrKit(22) )
        << BadgeE( L"xx", EsrKit(333) );
    std::sort( out.begin(), out.end() );

    CPPUNIT_ASSERT( post == out );
}
