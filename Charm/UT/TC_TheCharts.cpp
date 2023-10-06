#include "stdafx.h"

#include <boost/filesystem.hpp>
#include "MockFiler.h"
#include "TC_TheCharts.h"
#include "UtHemHelper.h"
#include "../_version_.h"
#include "../Hem/TheCharts.h"
#include "../Hem/TheChartsIo.h"
#include "../helpful/StrToTime.h"
#include "../helpful/WorkHours.h"
#include "../helpful/Amqp/Factory.h"
#include "../helpful/Amqp/Customer.h"
#include "../Hem/Filer.h"
#include "../Hem/AutoChart.h"
#include "../Hem/UserChart.h"
#include "../Hem/TerraChart.h"
#include "../Hem/UncontrolledStations.h"
#include "../Guess/SpotDetails.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TheCharts );

static time_t M( const std::string& str )
{
    time_t xx = StrDataTime_To_UTC( str, time_t(0) );
    CPPUNIT_ASSERT( time_t(0) != xx );
    return xx;
}


void TC_TheCharts::setUp()
{
    // Для примера использовано Рижское летнее время, поскольку в России с 2014 года часы не переводятся
    // "EET+02EEST+01,M3.5.0/03:00,M10.5.0/04:00" - [ посл. вск марта; посл. вск октября]
    set_TimeZone("EET+02EEST+01,M3.5.0/03:00,M10.5.0/04:00");
                   
}
void TC_TheCharts::tearDown()
{
    set_TimeZone(""); // сброс временнОй зоны
}

void TC_TheCharts::SelectLive()
{
    const time_t moment = M("2017-09-11 23:05");
    TheCharts theCharts;
    theCharts.initializeCurrentTime( moment );
    interval_time_t shift = WorkHours::HemBounds( moment );

    Hem::Slice& live0 = theCharts.selectLive();
    CPPUNIT_ASSERT( live0.shiftBoundaries() == shift );
    CPPUNIT_ASSERT( theCharts.getAllSlices().size() == 1 );
    CPPUNIT_ASSERT( theCharts.getClients( shift ).size() == 0 );

    theCharts.attach( "test", interval_time_t(moment, moment+1) );
    theCharts.attach( "testF", shift.offset(+3600*24*10) );
    theCharts.attach( "testP", shift.offset(-3600*24*10) );
    Hem::Slice& live1 = theCharts.selectLive();
    CPPUNIT_ASSERT( live1.shiftBoundaries() == shift );
    CPPUNIT_ASSERT( theCharts.getAllSlices().size() == 3 );
    CPPUNIT_ASSERT( theCharts.getClients( shift ).size() == 1 );
    CPPUNIT_ASSERT( theCharts.getClients( shift ).front() == "test" );
}

void TC_TheCharts::SelectBy()
{
    const interval_time_t a( M("2017-09-20 08:08"), M("2017-09-20 11:11") );
    const interval_time_t b( M("2017-09-20 17:17"), M("2017-09-20 23:23") );
    const interval_time_t c( M("2017-09-20 21:21"), M("2017-09-21 03:03") );
    const interval_time_t d( M("2017-09-21 03:03"), M("2017-09-21 15:15") );

    const interval_time_t shift_day(     M("2017-09-20 06:00"), M("2017-09-20 18:00") );
    const interval_time_t shift_night(   M("2017-09-20 18:00"), M("2017-09-21 06:00") );
    const interval_time_t shift_tomorrow(M("2017-09-21 06:00"), M("2017-09-21 18:00") );

    TheCharts theCharts;

    //
    // смотрим в сегодня и завтра
    //
    theCharts.initializeCurrentTime( a.upper() );
    // сначала все подключения, потом получение кусков т.к. куски могут сдвигаться
    theCharts.attach( "clientA", a );
    theCharts.attach( "clientB", b );
    theCharts.attach( "clientC", c );
    theCharts.attach( "clientD", d );
    {
        // 
        //  Рабочие смены: [<20>6..день...18][18.ночь.0....6][<21>6..день...18]
        //  Запросы:           aaaaaa   bbbbbbbb   cccccc ddddddd    
        //  Текущее время:  -----*
        //  
        Hem::Slice& sliceA = theCharts.selectBy( "clientA" );
        Hem::Slice& sliceB = theCharts.selectBy( "clientB" );
        Hem::Slice& sliceC = theCharts.selectBy( "clientC" );
        Hem::Slice& sliceD = theCharts.selectBy( "clientD" );
        // правильный выбор куска графика
        CPPUNIT_ASSERT( sliceA.shiftBoundaries() == shift_day );
        CPPUNIT_ASSERT( sliceB.shiftBoundaries() == shift_day ); // текущая смена показывает и следующую ночь
        CPPUNIT_ASSERT( sliceC.shiftBoundaries() == shift_day ); // текущая смена показывает и следующую ночь
        CPPUNIT_ASSERT( sliceD.shiftBoundaries() == shift_tomorrow ); // завтрашний день уже отдельно
        auto sum_day_night = hull( shift_day, shift_night );
        CPPUNIT_ASSERT( sliceA.normativeBoundaries().contains( sum_day_night ) );
        CPPUNIT_ASSERT( sliceB.normativeBoundaries().contains( sum_day_night ) );
        CPPUNIT_ASSERT( sliceC.normativeBoundaries().contains( sum_day_night ) );
        // общее состояние графика
        CPPUNIT_ASSERT( theCharts.getAllSlices().size() == 2 );
        CPPUNIT_ASSERT( theCharts.getClients(shift_day).size() == 1 );
        CPPUNIT_ASSERT( theCharts.getClients(shift_day).front() == "clientA" );
        CPPUNIT_ASSERT( theCharts.getClients(shift_night).size() == 2 );
        CPPUNIT_ASSERT( theCharts.getClients(shift_tomorrow).size() == 1 );
        CPPUNIT_ASSERT( theCharts.getClients(shift_tomorrow).front() == "clientD" );
    }

    //
    // сдвигаем время в ночь и смотрим в ночь и следующую смену
    // Важно: клиенты НЕ переподключаются, хотя куски извлекаются заново.
    //
    theCharts.processMovementOfClock( b.upper(), nullptr );
    {
        // 
        //  Рабочие смены: [<20>6..день...18][18.ночь.0....6][<21>6..день...18]
        //  Запросы:           aaaaaa   bbbbbbbb   cccccc ddddddd    
        //  Текущее время:  -------------------*
        //  
        Hem::Slice& sliceA = theCharts.selectBy( "clientA" );
        Hem::Slice& sliceB = theCharts.selectBy( "clientB" );
        Hem::Slice& sliceC = theCharts.selectBy( "clientC" );
        Hem::Slice& sliceD = theCharts.selectBy( "clientD" );
        // правильный выбор куска графика
        CPPUNIT_ASSERT( sliceA.shiftBoundaries() == shift_day );
        CPPUNIT_ASSERT( sliceB.shiftBoundaries() == shift_night ); // текущая смена уже ночная
        CPPUNIT_ASSERT( sliceC.shiftBoundaries() == shift_night ); // текущая смена уже ночная
        CPPUNIT_ASSERT( sliceD.shiftBoundaries() == shift_night ); // завтрашний день слит с ночью
        CPPUNIT_ASSERT( sliceA.normativeBoundaries().contains( shift_day ) );
        CPPUNIT_ASSERT( sliceB.normativeBoundaries().contains( shift_night ) );
        CPPUNIT_ASSERT( sliceB.normativeBoundaries().contains( shift_tomorrow ) );
        CPPUNIT_ASSERT( sliceC.normativeBoundaries().contains( hull( shift_night, shift_tomorrow ) ) );
        // общее состояние графика
        CPPUNIT_ASSERT( theCharts.getAllSlices().size() == 2 );
        CPPUNIT_ASSERT( theCharts.getClients(shift_day).size() == 1 );
        CPPUNIT_ASSERT( theCharts.getClients(shift_day).front() == "clientA" );
        CPPUNIT_ASSERT( theCharts.getClients(shift_night).size() == 2 );
        CPPUNIT_ASSERT( theCharts.getClients(shift_tomorrow).size() == 1 );
        CPPUNIT_ASSERT( theCharts.getClients(shift_tomorrow).front() == "clientD" );
    }
}

void TC_TheCharts::SliceBoundaries()
{
    // 
    //  Рабочие смены: [6..день...18][18.ночь.0..6]
    //  
    const time_t curr = M("2017-09-29 08:08");

    const interval_time_t prev_night( M("2017-09-28 18:00"), M("2017-09-29 06:00") );
    const interval_time_t curr_day(   M("2017-09-29 06:00"), M("2017-09-29 18:00") );
    const interval_time_t next_night( M("2017-09-29 18:00"), M("2017-09-30 06:00") );
    const interval_time_t next_day(   M("2017-09-30 06:00"), M("2017-09-30 18:00") );

    TheCharts theCharts;
    theCharts.initializeCurrentTime( curr );
    theCharts.attach( "test", interval_time_t(curr,curr+1) );

    Hem::Slice& slice = theCharts.selectBy( "test" );
    CPPUNIT_ASSERT( slice.shiftBoundaries().contains( curr ) );
    CPPUNIT_ASSERT( slice.shiftBoundaries() == curr_day );

    CPPUNIT_ASSERT( slice.showBoundaries().contains( slice.shiftBoundaries() ) );
    CPPUNIT_ASSERT( slice.showBoundaries().contains( curr_day ) );
    CPPUNIT_ASSERT( slice.showBoundaries().contains( prev_night ) );

    CPPUNIT_ASSERT( slice.normativeBoundaries().contains( curr_day ) );
}

static void AcceptIncrementChanges( TheCharts& theCharts )
{
    std::vector<Hem::Slice*> slices = theCharts.getAllSlices();
    for( auto pslice : slices )
    {
        attic::a_document analysDoc(hem2g_xAttr);
        attic::a_document chDoc("HemChanges");
        attic::a_node rootNode = chDoc.document_element();
        pslice->AutoCh().takeChanges( rootNode.append_child( AutoChanges_xAttr ), attic::a_node(), 0 );
        pslice->UserCh().takeChanges( rootNode.append_child( UserChanges_xAttr ), analysDoc.document_element(), 0);
        pslice->TerraCh().TakeChanges( rootNode.append_child( TerraChanges_xAttr ));
    }
}

void TC_TheCharts::MoveCurrentTime()
{
    using namespace HemHelpful;
    namespace HCIO = Hem::TheChartsIo;
    MockFiler filer;
    HCIO::FileStore fileStore(filer, HCIO::IOMODE::READWRITE, HCIO::SYNCMODE::ASYNC);

    auto tempPusher = []( std::function<void()> toRun, std::string /*markerGA*/ ){
        OutputDebugString(L"pusherFunc called\n");
        toRun();
    };
    auto tempPostback = []( std::string /*xx*/ ){
        OutputDebugString(L"postbackFunc called\n");
    };

    // генератор событий только для сокращения записи 
    auto gen = []( int esr, time_t moment ){
        BadgeE bdg( std::to_wstring(esr%10) + L"put", EsrKit( esr ) );
        SpotEventPtr res = std::make_shared<SpotEvent>( HCode::TRANSITION, bdg, moment );
        return res;
    };
    // генератор времени в фиксированном дне
    auto MT = []( const std::string& str )
    {
        time_t xx = StrDataTime_To_UTC( "2017-09-29 "+str, time_t(0) );
        CPPUNIT_ASSERT( time_t(0) != xx );
        return xx;
    };

    TheCharts theCharts( fileStore, tempPusher, tempPostback );
    //  Рабочие смены: [6..день...18][18.ночь.0..6]

    // создаем дневную смену         
    filer.initCurrentShift( MT("15:00") );
    theCharts.initializeCurrentTime( MT("15:00") );
    theCharts.attach( "current", MT("15:00") );
    {
        // нить через дневную и следующую ночную смены
        std::vector<SpotEventPtr> pat;
        pat.push_back( gen( 111, MT("11:00") ) );
        pat.push_back( gen( 222, MT("17:00") ) );
        pat.push_back( gen( 333, MT("18:30") ) );
        pat.push_back( gen( 444, MT("21:00") ) );

        TrioSpot trio;
        trio.body( pat.begin(), pat.end() );
        const std::string clA = "clientA";
        theCharts.attach( clA, MT("17:00") );
        Hem::Slice& slice = theCharts.selectBy( clA );
        std::vector<HemHelpful::SpotEventPtr> acceptedEvents;
        slice.UserCh().processSpotTrio(trio, acceptedEvents);
        UtChart<UserChart>& utUC = UtChart<UserChart>::upgrade(slice.UserCh());
        UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( utUC.getHappenLayer() );
        CPPUNIT_ASSERT( hl.exist_path_size( 4 ) );
        CPPUNIT_ASSERT( hl.exist_series( "Bdg", "1put[00111] 2put[00222] 3put[00333] 4put[00444] "));
        theCharts.detach( clA );
        AcceptIncrementChanges( theCharts ); // применить все изменения
    }

    // переходим в ночную (следующую) смену
//     OutputDebugString( L"\nПеред созданием ночной смены\n" );
//     OutputDebugString( FromUtf8(theCharts.logString_OverviewAll()+"\n").c_str() );
    theCharts.processMovementOfClock( MT("15:00"), nullptr );
    AcceptIncrementChanges( theCharts ); // применить все изменения
    theCharts.processMovementOfClock( MT("21:00"), nullptr );
    AcceptIncrementChanges( theCharts ); // применить все изменения
    theCharts.attach( "current", MT("21:00") );
    {
        // редактируем нить для проверки ее существования в текущей (ночной) смене
        TrioSpot trio;
        trio.pre(  gen( 333, MT("18:30") ) );
        trio.body( gen( 3003, MT("18:31") ) );
        trio.post( gen( 333, MT("18:30") ) );
        trio.coverPre(true);
        trio.coverPost(true);

        const std::string clB = "clientB";
        theCharts.attach( clB, MT("21:02") );
        Hem::Slice& slice = theCharts.selectBy( clB );
        std::vector<HemHelpful::SpotEventPtr> acceptedEvents;
        slice.UserCh().processSpotTrio(trio,acceptedEvents);

        UtChart<UserChart>& utUC = UtChart<UserChart>::upgrade(slice.UserCh());
        UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( utUC.getHappenLayer() );
        // при движении часов нить была закрыта как оторвавшаяся от отслеживания (+Death по посл. событию)
        CPPUNIT_ASSERT( hl.exist_path_size( 4+1 ) );
        CPPUNIT_ASSERT( hl.exist_series( "Bdg", "1put[00111] 2put[00222] 3put[03003] 4put[00444]   4put[00444] "));
        theCharts.detach( clB );
        AcceptIncrementChanges( theCharts ); // применить все изменения
    }

    // подключаемся к предыдущей дневной смене для проверки наличия там нити
//     OutputDebugString( L"\nПеред подключением к предыдущей дневной смене\n" );
//     OutputDebugString( FromUtf8(theCharts.logString_OverviewAll()+"\n").c_str() );
    {
        const std::string clX = "cliX";
        theCharts.attach( clX, interval_time_t( MT("16:00"), MT("17:05") ) );
        Hem::Slice& slice = theCharts.selectBy( clX );
//         OutputDebugString( L"\nПосле подключения к предыдущей дневной смене\n" );
//         OutputDebugString( FromUtf8(theCharts.logString_OverviewAll()+"\n").c_str() );
        UtChart<UserChart>& utUC = UtChart<UserChart>::upgrade(slice.UserCh());
        UtLayer<HappenLayer>& hl = UtLayer<HappenLayer>::upgrade( utUC.getHappenLayer() );
        bool prev = hl.exist_series( "Bdg", "1put[00111] 2put[00222] 3put[03003] 4put[00444] ");
        bool next = hl.exist_series( "Bdg", "1put[00111] 2put[00222] 3put[00333] 4put[00444] ");
        CPPUNIT_ASSERT( prev ^ next );
        CPPUNIT_ASSERT( hl.exist_path_size( 4 ) );
        theCharts.detach( clX );
        AcceptIncrementChanges( theCharts ); // применить все изменения
    }
}

void TC_TheCharts::ChangeSummerWinter()
{
    const time_t H = 3600;

    TheCharts theCharts;
    {
        const time_t    mom = M("2019-10-26 13:13");
        const std::string eta( "[2019-10-26 06 - 18)" );
        // последняя летняя смена 2019
        const interval_time_t sh = WorkHours::HemBounds( mom );
        CPPUNIT_ASSERT_EQUAL( eta,  sh.to_view() );
        theCharts.initializeCurrentTime( mom );
        Hem::Slice& live = theCharts.selectLive();
        CPPUNIT_ASSERT_EQUAL( eta, live.shiftBoundaries().to_view() );
    }
    {
        const time_t    mom = M("2019-10-27 01:01");
        const std::string eta( "[2019-10-26 18 - 2019-10-27 06)" );
        // переход в зимнее время внутри смены 2019
        const interval_time_t sh = WorkHours::HemBounds( mom );
        CPPUNIT_ASSERT_EQUAL( 13*H, sh.width() );
        CPPUNIT_ASSERT_EQUAL( eta, sh.to_view() );
        theCharts.processMovementOfClock( mom, nullptr );
        Hem::Slice& live = theCharts.selectLive();
        CPPUNIT_ASSERT_EQUAL( eta, live.shiftBoundaries().to_view() );
    }
    {
        const time_t    target = M("2020-03-28 11:11");
        // последовательная обработка т.к. часы не могут перескакивать через смены
        while( theCharts.getCurrentTime()+10*H < target )
            theCharts.processMovementOfClock( theCharts.getCurrentTime()+10*H, nullptr );
    }
    {
        const time_t    mom = M("2020-03-28 15:15");
        const std::string eta( "[2020-03-28 06 - 18)" );
        // последняя зимняя смена 2019/2020
        const interval_time_t sh = WorkHours::HemBounds( mom );
        CPPUNIT_ASSERT_EQUAL( eta, sh.to_view() );
        theCharts.processMovementOfClock( mom, nullptr );
        Hem::Slice& live = theCharts.selectLive();
        CPPUNIT_ASSERT_EQUAL( eta, live.shiftBoundaries().to_view() );
    }
    {
        const time_t    mom = M("2020-03-29 02:02");
        const std::string eta( "[2020-03-28 18 - 2020-03-29 06)" );
        // переход в летнее время внутри смены 2020
        const interval_time_t sh = WorkHours::HemBounds( mom );
        CPPUNIT_ASSERT_EQUAL( 11*H, sh.width() );
        CPPUNIT_ASSERT_EQUAL( eta, sh.to_view() );
        theCharts.processMovementOfClock( mom, nullptr );
        Hem::Slice& live = theCharts.selectLive();
        CPPUNIT_ASSERT_EQUAL( eta, live.shiftBoundaries().to_view() );
    }
}

void TC_TheCharts::ReadDb()
{
    namespace HCIO = Hem::TheChartsIo;
    const time_t curr = M("2018-06-20 15:00");

    Network::Factory factory;
    auto connect = factory.createConnection( "pluk.nipc:8672" );
    Hem::TheChartsIo::PlukStore plukStore( connect, HCIO::IOMODE::READONLY, HCIO::SYNCMODE::ASYNC);
    auto tempPusher = []( std::function<void()> /*toRun*/, std::string /*markerGA*/ ){
        OutputDebugString(L"pusherFunc called\n");
    };
    auto tempPostback = []( std::string /*msg*/ ){
        OutputDebugString(L"postbackFunc called\n");
    };

    TheCharts theCharts( plukStore, tempPusher, tempPostback );
    theCharts.initializeCurrentTime( curr );
    theCharts.attach( "test", curr );

    Hem::Slice& slice = theCharts.selectLive();
    CPPUNIT_ASSERT( slice.shiftBoundaries().contains( curr ) );
}

namespace {

    UncontrolledStations getUncontrolled()
    {
        UncontrolledStations uncontrolledEsr;
        attic::a_document uncDoc;
        uncDoc.load_wide(
            L"<Some>"
            L"  <UncontrolledSCB ESR='06551' />  <!--Русаново -->"
            L"  <UncontrolledSCB ESR='06600' />  <!--Великие Луки -->"
            L"  <UncontrolledSCB ESR='06610' />  <!--Себеж -->"
            L"  <UncontrolledSCB ESR='06612' />  <!--Посинь -->"
            L"  <UncontrolledSCB ESR='06660' />  <!--Novosokoļņikiem -->"
            L"  <UncontrolledSCB ESR='07000' />  <!--Березки -->"
            L"  <UncontrolledSCB ESR='07050' />  <!--Псков -->"
            L"  <UncontrolledSCB ESR='07200' />  <!--Луга1 -->"
            L"  <UncontrolledSCB ESR='07790' />  <!--Пыталово -->"
            L"  <UncontrolledSCB ESR='07792' />  <!--Скангали -->"
            L"  <UncontrolledSCB ESR='08000' />  <!--Тара -->"
            L"  <UncontrolledSCB ESR='08480' />  <!--Тарту -->"
            L"  <UncontrolledSCB ESR='09040' />  <!--Ganibu parks -->"
            L"  <UncontrolledSCB ESR='09050' />  <!--Rīga-Krasta -->"
            L"  <UncontrolledSCB ESR='09051' />  <!--3 km        -->"
            L"  <UncontrolledSCB ESR='09070' />  <!--Skulte -->"
            L"  <UncontrolledSCB ESR='09280' />  <!--Глуда -->"
            L"  <UncontrolledSCB ESR='09290' />  <!--Добеле -->"
            L"  <UncontrolledSCB ESR='09303' />  <!--Биксти -->"
            L"  <UncontrolledSCB ESR='09320' />  <!--Броцени -->"
            L"  <UncontrolledSCB ESR='09330' />  <!--Салдус -->"
            L"  <UncontrolledSCB ESR='09340' />  <!--Скрунда -->"
            L"  <UncontrolledSCB ESR='09350' />  <!--Калвене -->"
            L"  <UncontrolledSCB ESR='09351' />  <!--Илмая -->"
            L"  <UncontrolledSCB ESR='09355' />  <!--Торе -->"
            L"  <UncontrolledSCB ESR='09430' />  <!--Riga Precu -->"
            L"  <UncontrolledSCB ESR='09440' />  <!--Рига-пр-эксп. -->"
            L"  <UncontrolledSCB ESR='09484' />  <!--Эргли -->"
            L"  <UncontrolledSCB ESR='09600' />  <!--Saulkrasti -->"
            L"  <UncontrolledSCB ESR='09602' />  <!--Inčupe -->"
            L"  <UncontrolledSCB ESR='09603' />  <!--Lilaste -->"
            L"  <UncontrolledSCB ESR='09611' />  <!--Гауя -->"
            L"  <UncontrolledSCB ESR='09612' />  <!--Царникава -->"
            L"  <UncontrolledSCB ESR='09616' />  <!--Вецаки -->"
            L"  <UncontrolledSCB ESR='09630' />  <!--Ziemeļblāzma -->"
            L"  <UncontrolledSCB ESR='09640' />  <!--Mangali -->"
            L"  <UncontrolledSCB ESR='09650' />  <!--Саркандаугава -->"
            L"  <UncontrolledSCB ESR='09690' />  <!--ilguciems -->"
            L"  <UncontrolledSCB ESR='09820' />  <!--Вентпилс -->"
            L"  <UncontrolledSCB ESR='09840' />  <!--Вентсп-а -->"
            L"  <UncontrolledSCB ESR='09850' />  <!--Вентсп-н -->"
            L"  <UncontrolledSCB ESR='09851' />  <!--парм 223 -->"
            L"  <UncontrolledSCB ESR='09880' />  <!--Вентсп-жи -->"
            L"  <UncontrolledSCB ESR='09940' />  <!--Бене -->"
            L"  <UncontrolledSCB ESR='09960' />  <!--Реньге -->"
            L"  <UncontrolledSCB ESR='09990' />  <!--Криевусала -->"
            L"  <UncontrolledSCB ESR='11000' />  <!--Даугавпилс -->"
            L"  <UncontrolledSCB ESR='11001' />  <!--Дау-Д -->"
            L"  <UncontrolledSCB ESR='11002' />  <!--пара -->"
            L"  <UncontrolledSCB ESR='11003' />  <!--непара -->"
            L"  <UncontrolledSCB ESR='11010' />  <!--Дау-ск -->"
            L"  <UncontrolledSCB ESR='11072' />  <!--7 км -->"
            L"  <UncontrolledSCB ESR='11084' />  <!--Valka -->"
            L"  <UncontrolledSCB ESR='11160' />  <!--Gulbene -->"
            L"  <UncontrolledSCB ESR='11173' />  <!--Зигури -->"
            L"  <UncontrolledSCB ESR='11272' />  <!--Резекне-1 -->"
            L"  <UncontrolledSCB ESR='11710' />  <!--Madona -->"
            L"  <UncontrolledSCB ESR='11750' />  <!--Jaunkalsnava -->"
            L"  <UncontrolledSCB ESR='11773' />  <!--8 km     -->"
            L"  <UncontrolledSCB ESR='11780' />  <!--Veseta -->"
            L"  <UncontrolledSCB ESR='12520' />  <!--Šiļajņaja -->"
            L"  <UncontrolledSCB ESR='12550' />  <!--Radviliškis -->"
            L"  <UncontrolledSCB ESR='12562' />  <!--Zoknai -->"
            L"  <UncontrolledSCB ESR='12570' />  <!--Šauļi -->"
            L"  <UncontrolledSCB ESR='12571' />  <!--Toļačaja -->"
            L"  <UncontrolledSCB ESR='12580' />  <!--Gubernija -->"
            L"  <UncontrolledSCB ESR='12581' />  <!--Meškujčaja -->"
            L"  <UncontrolledSCB ESR='12600' />  <!--Jonišķis -->"
            L"  <UncontrolledSCB ESR='12601' />  <!--Nurajčis -->"
            L"  <UncontrolledSCB ESR='12630' />  <!--Bugjaņaja -->"
            L"  <UncontrolledSCB ESR='12631' />  <!--Venta -->"
            L"  <UncontrolledSCB ESR='12640' />  <!--Мазейки -->"
            L"  <UncontrolledSCB ESR='12780' />  <!--Rokiski -->"
            L"  <UncontrolledSCB ESR='12781' />  <!--Obaļaja -->"
            L"  <UncontrolledSCB ESR='12910' />  <!--Turmantas -->"
            L"  <UncontrolledSCB ESR='16000' />  <!--Vitebsk -->"
            L"  <UncontrolledSCB ESR='16130' />  <!--Polocka -->"
            L"  <UncontrolledSCB ESR='16169' />  <!--Бигосово-ст -->"
            L"  <UncontrolledSCB ESR='16170' />  <!--Бигосово -->"
            L"  <UncontrolledSCB ESR='16180' />  <!--Verhņedvinska -->"
            L"  <UncontrolledSCB ESR='16181' />  <!--Svoļno -->"
            L"  <UncontrolledSCB ESR='16190' />  <!--Borkovići -->"
            L"  <UncontrolledSCB ESR='16200' />  <!--Adamova -->"
            L"  <UncontrolledSCB ESR='16210' />  <!--Baravuha -->"
            L"  <UncontrolledSCB ESR='16220' />  <!--Ropņanskaja -->"
            L"  <UncontrolledSCB ESR='16400' />  <!--Krulevschizna -->"
            L"  <UncontrolledSCB ESR='16550' />  <!--Novopolock -->"
            L"</Some>"
            );
        uncontrolledEsr.deserialize(uncDoc.document_element());
        return uncontrolledEsr;
    }
}; // unnamed ns

void TC_TheCharts::ReadFile()
{
    namespace HCIO = Hem::TheChartsIo;
    std::wstring fold = L"E:/WorkCopy.all/Sample/logics/00000/";

    NsCanopy::Filer filer( fold, 30 );
    Hem::TheChartsIo::FileStore fileStore( filer, HCIO::IOMODE::READONLY, HCIO::SYNCMODE::SYNC);
    auto tempPusher = []( std::function<void()> /*toRun*/, std::string /*markerGA*/ ){
        OutputDebugString(L"pusherFunc called\n");
    };
    auto tempPostback = []( std::string /*msg*/ ){
        OutputDebugString(L"postbackFunc called\n");
    };

    const time_t curr = filer.read_latest_moment();

    TheCharts theCharts( fileStore, tempPusher, tempPostback );
    theCharts.setupUncontrolledStations( getUncontrolled() );
    theCharts.initializeCurrentTime( curr );
    theCharts.attach( "test", curr );

    Hem::Slice& slice = theCharts.selectLive();
    CPPUNIT_ASSERT( slice.shiftBoundaries().contains( curr ) );
}
