#include "stdafx.h"
#include "TC_AugurRequester_ClarificationIntrusions.h"
#include "../Hem/AugurRequester_ClarificationIntrusions.h"
#include "../Hem/RouteIntrusionKeeper.h"
#include "../Hem/RouteIntrusion.h"
#include "../helpful/Time_Iso.h"
#include "../helpful/Serialization.h"
#include "../helpful/LocoCharacteristics.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AdjustIntrusions );

void TC_AdjustIntrusions::SelfVisit()
{
    // подготовка
    setLiveMoment( "20180505T080000Z" );
    future_layer->createPath( // PE 6001: 11111-22222(FIX)-33333
        L"<HemPath>"
        +HEAD(1011)+L"name='Departure' Bdg='zz[11111]' index='0900-777-1100' num='E6001' waynum='3' parknum='1' />"
        +HEAD(1021)+L"name='Transition' Bdg='mock[22222]' waynum='5' parknum='1' sourcetype='fix' />"
        +HEAD(1031)+L"name='Arrival' Bdg='mock[33333]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath( // PE 6003: 11111-22222-33333
        L"<HemPath>"
        +HEAD(1013)+L"name='Departure' Bdg='zz[11111]' index='0900-777-1100' num='E6003' waynum='3' parknum='1' />"
        +HEAD(1023)+L"name='Transition' Bdg='mock[22222]' waynum='5' parknum='1' />"
        +HEAD(1033)+L"name='Arrival' Bdg='mock[33333]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath( // PE 6005: 11111-44444-33333
        L"<HemPath>"
        +HEAD(1015)+L"name='Departure' Bdg='zz[11111]' index='0900-666-1100' num='E6005' waynum='3' parknum='1' />"
        +HEAD(1025)+L"name='Transition' Bdg='mock[44444]' waynum='5' parknum='1' />"
        +HEAD(1035)+L"name='Arrival' Bdg='mock[33333]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT_EQUAL( size_t(3), future_layer->path_count() );

    rIntrusionKeeper = deserialize<RouteIntrusionKeeper>( 
        "<ArtificalRouteIntrusions>"
        "  <RouteIntrusion>"
        "    <TrainDescr index='0900-740-1800' num='6001' >"
        "      <Locomotive Series='2М62УМ' NumLoc='94' Consec='1' CarrierCode='4'>"
        "        <Crew EngineDriver='LJALIN' Tim_Beg='2018-11-29 12:46' />"
        "      </Locomotive>"
        "    </TrainDescr>"
        "    <Station esrCode='22222'>"
        "      <OrderPriority/>"
        "    </Station>"
        "  </RouteIntrusion>"

        "  <RouteIntrusion>"
        "    <TrainDescr index='1655-029-0983' num='6003' >"
        "      <Locomotive Series='2ТЭ116' NumLoc='875' Consec='1' CarrierCode='2'>"
        "        <Crew EngineDriver='RANCANS' Tim_Beg='2018-11-29 20:11' />"
        "      </Locomotive>"
        "    </TrainDescr>"
        "    <Station esrCode='22222'>"
        "      <OrderPriority/>"
        "    </Station>"
        "  </RouteIntrusion>"

        "  <RouteIntrusion>"
        "    <TrainDescr index='' num='6005' through='Y' />"
        "    <Station esrCode='22222'>"
        "      <OrderPriority />"
        "    </Station>"
        "  </RouteIntrusion>"
        "</ArtificalRouteIntrusions>"
        );
    CPPUNIT_ASSERT_EQUAL( size_t(3), rIntrusionKeeper->getIntrusions().size() );

    // исполнение
    ClarificationIntrusions work( future_layer->base(), *rIntrusionKeeper );
    work.clarify();

    // проверка: все  интрузии по 22222, однако
    //  6001 мешает фиксация,
    //  6003 нет помех
    //  6005 не посетил
    std::vector<RouteIntrusion> vRi =  rIntrusionKeeper->getIntrusions();
    CPPUNIT_ASSERT( vRi.size() == 1 );
    CPPUNIT_ASSERT( vRi.front().getTrainDescr().GetNumber().getNumber() == 6003 );
}

void TC_AdjustIntrusions::OtherVisit()
{
    // подготовка
    setLiveMoment( "20180505T080000Z" );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1011)+L"name='Departure'  Bdg='aa[11111]' num='E6001' index='0900-777-1100' waynum='3' parknum='1' />"
        +HEAD(1021)+L"name='Transition' Bdg='bb[22222]'           waynum='5' parknum='1' />"
        +HEAD(1031)+L"name='Arrival'    Bdg='cc[33333]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1013)+L"name='Departure'  Bdg='zz[11111]' num='E6003' index='0900-777-1100' waynum='3' parknum='1' />"
        +HEAD(1023)+L"name='Transition' Bdg='xx[22222]'           waynum='5' parknum='1' />"
        +HEAD(1033)+L"name='Arrival'    Bdg='qq[33333]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1014)+L"name='Departure'  Bdg='zz[11111]' num='E6004' index='0900-444-1100' waynum='3' parknum='1' />"
        +HEAD(1024)+L"name='Transition' Bdg='xx[44444]'           waynum='5' parknum='1' />"
        +HEAD(1034)+L"name='Arrival'    Bdg='qq[33333]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1015)+L"name='Departure'  Bdg='ff[11111]' num='E6005' index='0900-666-1100' waynum='3' parknum='1' />"
        +HEAD(1025)+L"name='Transition' Bdg='gg[22222]'           waynum='5' parknum='1' />"
        +HEAD(1035)+L"name='Arrival'    Bdg='hh[33333]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1019)+L"name='Departure'  Bdg='ee[11111]' num='E6009' index='0900-236-1100' waynum='3' parknum='1' />"
        +HEAD(1029)+L"name='Transition' Bdg='rr[22222]'           waynum='5' parknum='1' />"
        +HEAD(1039)+L"name='Arrival'    Bdg='tt[33333]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT_EQUAL( size_t(5), future_layer->path_count() );

    rIntrusionKeeper = deserialize<RouteIntrusionKeeper>( 
        "<ArtificalRouteIntrusions>"

        "  <RouteIntrusion>"
        "    <TrainDescr index='0900-740-1800' num='6001' />"
        "    <Station esrCode='22222'>"
        "      <OrderPriority>"
        "        <YieldFor>"
        "          <TrainDescr index=''        num='6007' />"
        "        </YieldFor>"
        "      </OrderPriority>"
        "    </Station>"
        "  </RouteIntrusion>"

        "  <RouteIntrusion>"
        "    <TrainDescr index=''       num='6003' />"
        "    <Station esrCode='22222'>"
        "      <OrderPriority>"
        "        <YieldFor>"
        "          <TrainDescr index='' num='6004' />"
        "        </YieldFor>"
        "      </OrderPriority>"
        "    </Station>"
        "  </RouteIntrusion>"

        "  <RouteIntrusion>"
        "    <TrainDescr index=''       num='6005' />"
        "    <Station esrCode='22222'>"
        "      <OrderPriority>"
        "        <YieldFor>"
        "          <TrainDescr index='' num='6009' />"
        "        </YieldFor>"
        "      </OrderPriority>"
        "    </Station>"
        "  </RouteIntrusion>"

        "</ArtificalRouteIntrusions>"
        );
    CPPUNIT_ASSERT_EQUAL( size_t(3), rIntrusionKeeper->getIntrusions().size() );

    // исполнение
    ClarificationIntrusions work( future_layer->base(), *rIntrusionKeeper );
    work.clarify();

    // проверка: все  интрузии по 22222, однако
    //  6001 ждет 6007, которого нет
    //  6003 ждет 6004, который не посещал 22222
    //  6005 ждет 6009, которые и прошел и посетил
    std::vector<RouteIntrusion> vRi =  rIntrusionKeeper->getIntrusions();
    CPPUNIT_ASSERT_EQUAL( size_t(1), vRi.size() );
    CPPUNIT_ASSERT_EQUAL( unsigned(6005), vRi.front().getTrainDescr().GetNumber().getNumber() );
}

void TC_AdjustIntrusions::SameTrainNumber()
{
    // подготовка
    setLiveMoment( "20180505T080000Z" );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1011)+L"name='Departure'  Bdg='aa[11111]' num='D2001' index='1111-111-1111' waynum='3' parknum='1' />"
        +HEAD(1021)+L"name='Transition' Bdg='bb[22222]'           waynum='5' parknum='1' />"
        +HEAD(1031)+L"name='Arrival'    Bdg='cc[33333]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1012)+L"name='Departure'  Bdg='aa[44444]' num='D2001' index='2222-222-2222' waynum='3' parknum='1' />"
        +HEAD(1022)+L"name='Transition' Bdg='bb[55555]'           waynum='5' parknum='1' />"
        +HEAD(1032)+L"name='Arrival'    Bdg='cc[66666]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1013)+L"name='Departure'  Bdg='aa[11111]' num='X5507' index='3333-333-3333' waynum='3' parknum='1' />"
        +HEAD(1023)+L"name='Transition' Bdg='bb[22222]'           waynum='5' parknum='1' />"
        +HEAD(1033)+L"name='Arrival'    Bdg='cc[33333]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1014)+L"name='Departure'  Bdg='aa[11111]' num='Y5507' index='4444-444-4444' waynum='3' parknum='1' />"
        +HEAD(1024)+L"name='Transition' Bdg='bb[33333]'           waynum='5' parknum='1' />"
        +HEAD(1034)+L"name='Arrival'    Bdg='cc[66666]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    future_layer->createPath(
        L"<HemPath>"
        +HEAD(1015)+L"name='Departure'  Bdg='aa[44444]' num='Z5507' index='5555-555-5555' waynum='3' parknum='1' />"
        +HEAD(1025)+L"name='Transition' Bdg='bb[55555]'           waynum='5' parknum='1' />"
        +HEAD(1035)+L"name='Arrival'    Bdg='cc[66666]'           waynum='3' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT_EQUAL( size_t(5), future_layer->path_count() );

    rIntrusionKeeper = deserialize<RouteIntrusionKeeper>( 
        "<ArtificalRouteIntrusions>"

        "  <RouteIntrusion>"
        "    <TrainDescr index='1111-111-1111' num='D2001' />"
        "    <Station esrCode='22222'>"
        "      <OrderPriority>"
        "        <YieldFor>"
        "          <TrainDescr index='' num='X5507' />"
        "        </YieldFor>"
        "      </OrderPriority>"
        "    </Station>"
        "  </RouteIntrusion>"

        "  <RouteIntrusion>"
        "    <TrainDescr index='2222-222-2222' num='D2001' />"
        "    <Station esrCode='55555'>"
        "      <OrderPriority>"
        "        <YieldFor>"
        "          <TrainDescr index='' num='Z5507' />"
        "        </YieldFor>"
        "      </OrderPriority>"
        "    </Station>"
        "  </RouteIntrusion>"

        "</ArtificalRouteIntrusions>"
        );
    CPPUNIT_ASSERT_EQUAL( size_t(2), rIntrusionKeeper->getIntrusions().size() );

    // исполнение
    ClarificationIntrusions work( future_layer->base(), *rIntrusionKeeper );
    work.clarify();

    // проверка
    std::vector<RouteIntrusion> vRi =  rIntrusionKeeper->getIntrusions();
    CPPUNIT_ASSERT_EQUAL( size_t(2), vRi.size() );
    TrainDescr tdA = vRi.front().getTrainDescr();
    TrainDescr tdB = vRi.back().getTrainDescr();
    if( tdA.GetIndex()>tdB.GetIndex() )
        std::swap( tdA, tdB );
    CPPUNIT_ASSERT_EQUAL( unsigned(2001), tdA.GetNumber().getNumber() );
    CPPUNIT_ASSERT_EQUAL( std::string( "1111-111-1111" ), ToUtf8( tdA.GetIndex().str() ) );
    CPPUNIT_ASSERT_EQUAL( unsigned( 2001 ), tdB.GetNumber().getNumber() );
    CPPUNIT_ASSERT_EQUAL( std::string( "2222-222-2222" ), ToUtf8( tdB.GetIndex().str() ) );
}

void TC_AdjustIntrusions::setUp()
{
    future_layer = std::make_shared< UtLayer<FutureLayer> >();
    rIntrusionKeeper.reset();
    liveMoment = 0;
}

void TC_AdjustIntrusions::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

// установка текущего времени - эта же дата используется для построения данных
void TC_AdjustIntrusions::setLiveMoment( std::string _currentMoment )
{
    liveMoment = time_from_iso(_currentMoment);
    CPPUNIT_ASSERT( liveMoment );
}

// генератор заголовка события
std::wstring TC_AdjustIntrusions::HEAD( unsigned HHMMss ) const
{
    CPPUNIT_ASSERT( liveMoment );

    std::string hms = std::to_string(HHMMss);
    if ( hms.size()==4 )
        hms += "00";
    CPPUNIT_ASSERT( hms.size() == 4 || hms.size() == 6 ); // час_минута или час_минута_секунда

    return FromUtf8("<SpotEvent create_time='" + time_to_iso(liveMoment).substr(0,9) + hms + "Z' ");
}
