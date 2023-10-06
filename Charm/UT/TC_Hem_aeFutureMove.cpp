#include "stdafx.h"

#include "TC_Hem_aeFutureMove.h"
#include "../Hem/Appliers/aeFutureMoveDefault.h"
#include "../Hem/Appliers/aeFutureMoveOrder.h"
#include "../Hem/Appliers/aeFutureStationRules.h"
#include "../Hem/RouteIntrusionKeeper.h"
#include "../Hem/RouteIntrusion.h"
#include "../Hem/GriffinUtility.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeFutureMove );

void TC_Hem_aeFutureMove::duplicate()
{
    using namespace Hem::ApplierCarryOut;

    const TrainDescr tdLeader( L"88" );
    const TrainDescr tdSelf( L"K6022" );

    RouteIntrusionKeeper intrKeeper;

    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 0 );
    {
        aeFutureMoveOrder order( tdSelf, intrKeeper, EsrKit(12345), tdLeader );
        order.Action();
    }
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 1 );
//     {
// 		RouteIntrusion::StationIntrusion stationIntrusion;
//         aeFutureStationRules statRules( tdSelf, intrKeeper, EsrKit(12345), stationIntrusion, false );
//         statRules.Action();
//     }
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 1 );
    {
        aeFutureMoveDefault defmove( tdSelf, intrKeeper, EsrKit(12345) );
        defmove.Action();
    }
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 0 );
}

void TC_Hem_aeFutureMove::reverse()
{
    using namespace Hem::ApplierCarryOut;

    const TrainDescr tdA( L"88" );
    const TrainDescr tdB( L"K6022" );

    RouteIntrusionKeeper intrKeeper;

    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 0 );
    {
        aeFutureMoveOrder order( tdA, intrKeeper, EsrKit(12345), tdB );
        order.Action();
    }
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 1 );
    {
        aeFutureMoveOrder order( tdB, intrKeeper, EsrKit(44444), tdA );
        order.Action();
    }
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 1 );
    intrKeeper.erase( tdB );
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 0 );
}

void TC_Hem_aeFutureMove::rename()
{
    using namespace Hem::ApplierCarryOut;

    RouteIntrusionKeeper intrKeeper;

    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 0 );
    {
        aeFutureMoveOrder order( TrainDescr( L"A88" ), intrKeeper, EsrKit(12345), TrainDescr( L"B6022" ) );
        order.Action();
    }
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 1 );
    {
        aeFutureMoveOrder order( TrainDescr( L"D6022" ), intrKeeper, EsrKit(44444), TrainDescr( L"E88" ) );
        order.Action();
    }
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 1 );
    intrKeeper.erase( TrainDescr( L"G6022" ) );
    CPPUNIT_ASSERT( intrKeeper.getIntrusions().size() == 0 );
}
