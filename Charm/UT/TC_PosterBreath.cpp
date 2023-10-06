#include "stdafx.h"

#include "TC_PosterBreath.h"
#include "../helpful/Attic.h"
#include "../helpful/Log.h"
#include "../Fund/PosterBreath.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PosterBreath );

void TC_PosterBreath::setUp()
{
    ps.clear();
    attic::a_document doc;
    doc.load_wide(         
        L"<Poster delay='77'>"
        L"    <Place ESR_code='11011'>"
        L"        <It Badge='one'   delay='11' />"
        L"        <It Badge='two'   delay='22' />"
        L"        <It Badge='three' delay='33' />"
        L"    </Place>"
        L"</Poster>"
        );
    ps.load(doc);

    kit = EsrKit(11011);
    b1 = BadgeE( L"one",   kit );
    b2 = BadgeE( L"two",   kit );
    b3 = BadgeE( L"three", kit );
    d1 = 11;
    d2 = 22; 
    d3 = 33;

    vector<BadgeE> v;
    v.push_back( b1 );
    v.push_back( b2 );
    v.push_back( b3 );
    ps.collectPostable(v);
}

void TC_PosterBreath::tearDown()
{
}

void TC_PosterBreath::wait_for_start()
{
    PosterBreath pb(ps);

    // активируем в момент 0
    pb.insert( t0, b3 );
    pb.insert( t0, b2 );
    pb.insert( t0, b1 );

    // изначально нет сработавших
    CPPUNIT_ASSERT( pb.getCollect().empty() );
    // до момента 2 сработает только первый
    pb.update( t0+d2-1 );
    CPPUNIT_ASSERT( pb.getCollect().size()==1 );
    // в момент 2 сработает ещё и второй
    pb.update( t0+d2 );
    CPPUNIT_ASSERT( pb.getCollect().size()==2 );

    // третий удален и поэтому НЕ сработает
    pb.erase( b3 );
    pb.update( t0+d3 );
    CPPUNIT_ASSERT( pb.getCollect().size()==2 );

    // все удалены, факты срабатывания остаются
    pb.erase( b1 );
    pb.erase( b2 );
    pb.erase( b3 );
    pb.update( t0+d1+d2+d3 );
    CPPUNIT_ASSERT( pb.getCollect().size()==2 );
}

void TC_PosterBreath::insert_multy()
{
    PosterBreath pb(ps);

    // активируем несколько раз
    pb.insert( t0,   b2 );
    pb.insert( t0+1, b2 );
    pb.insert( t0+2, b2 );

    // в момент 2 сработает только один раз
    pb.update( t0+d2 );
    CPPUNIT_ASSERT( pb.getCollect().size()==1 );
}

void TC_PosterBreath::run_twice()
{
    PosterBreath pb(ps);

    // активируем и ждем срабатывания
    pb.insert( t0, b1 );
    pb.update( t0+d1 );
    CPPUNIT_ASSERT( pb.getCollect().size()==1 );
    // убираем, повторно активируем и ждем второго срабатывания того же объекта
    pb.erase( b1 );
    pb.insert( t0+100, b1 );
    pb.update( t0+100+d1 );
    CPPUNIT_ASSERT( pb.getCollect().size()==2 );
}
