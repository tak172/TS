#include "stdafx.h"
#include "TC_RibSet.h"
#include "../helpful/rwInterval.h"
#include "../Fund/RibSet.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_RibSet );

void TC_RibSet::test()
{
    RibMap<int> ribSpeed;

    EsrKit esr(7000,8000);
    const int way = 3;
    rwInterval x = rwInterval::from_ordinal("30.2");
    int xx = 22;
    rwInterval y = rwInterval::from_ordinal("30.7");
    int yy = 17;
    rwInterval a = rwInterval::from_ordinal("30");
    int aa = 40;
    rwInterval b = rwInterval::from_ordinal("40");
    int bb = 60;
    rwInterval ab=aggregate(a,b);

    RibTrack<int>& track = ribSpeed.search_with_create( SPANLINE(esr, way) );

    vector<int>::iterator i1;
    vector<int>::iterator i2;

    track.forZone( x, setIntMin(xx) );
    track.forZone( a, setIntMin(aa) );
    track.forZone( y, setIntMin(yy) );
    track.forZone( b, setIntMin(bb) );

    boost::tie(i1,i2) = track.zone(x);
    for( ; i1!=i2; ++i1 )
        CPPUNIT_ASSERT(*i1 <=xx);
    boost::tie(i1,i2) = track.zone(a);
    for( ; i1!=i2; ++i1 )
        CPPUNIT_ASSERT(*i1 <=aa);
    boost::tie(i1,i2) = track.zone(y);
    for( ; i1!=i2; ++i1 )
        CPPUNIT_ASSERT(*i1 <=yy);
    boost::tie(i1,i2) = track.zone(b);
    for( ; i1!=i2; ++i1 )
        CPPUNIT_ASSERT(*i1 <=bb);

    getIntMin getter = track.forZone( a.begin(), b.end(), getIntMin() );
    CPPUNIT_ASSERT(getter.exist());
    CPPUNIT_ASSERT(getter.getValue()==min(min(aa,bb),min(xx,yy)));
}
