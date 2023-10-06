#include "stdafx.h"

#include "TC_Hem_aeRectify.h"
#include "UtHemHelper.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Hem/Appliers/aeRectify.h"
#include "../helpful/LocoCharacteristics.h"

using namespace std;
using namespace HemHelpful;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeRectify );

void TC_Hem_aeRectify::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_Hem_aeRectify::RectifyWithMiss()
{
    const EsrKit K(1100,1200);
    const time_t T=3600*(240+23);

    UtLayer<HappenLayer> happen_layer;

    // заполнение графика одной нитью 1w2w3w4w, которая и будет сглаживаться
    SpotEventPtr pat[] = {
        SpotEventPtr( new SpotEvent( HCode::FORM, BadgeE(L"1w",K), T+1 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"1w",K), T+1 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"2w",K), T+2 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"3w",K), T+3 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"4w",K), T+4 ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,     BadgeE(L"4w",K), T+4 ) ),
    };
    TrioSpot trio;
    trio.body( begin(pat), end(pat) );
    Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
    pusher.Action();
    auto sz = pusher.AppliedEvents().size();
    CPPUNIT_ASSERT( trio.body().size() == sz );

    // выполняем сглаживание с произвольными событиями
    SpotEventPtr rectEvent[] = {
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"не было",K), T+9 ) ), // несуществовавшее событие
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"4w",K), T+4 ) ), // по этому событию будет сглажено 1w2w3w4w --> 1w4w
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"2w",K), T+1 ) ), // это событие уже исчезло к моменту обработки
        SpotEventPtr(), // вообще нулевой указатель
    };
    // проверим, что исключений не будет
    for( auto& spotEv : rectEvent )
    {
        Hem::aeRectify rectifyEngine( happen_layer.GetWriteAccess(), spotEv );
        rectifyEngine.Action();
    }
    // ок, исключений не было

    // проверим, что осталась одна нить из трех событий
    CPPUNIT_ASSERT( happen_layer.exist_path_size( 3 ) );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
}

void TC_Hem_aeRectify::RectifyDeathForm()
{
    const EsrKit K1(100);
    const EsrKit K12(100,200);
    const EsrKit K2(200);
    const time_t T=3600*(240+23);

    UtLayer<HappenLayer> happen_layer;

    // заполнение графика одной нитью 1w2w3w4w, которая и будет сглаживаться
    SpotEventPtr pat[] = {
        SpotEventPtr( new SpotEvent( HCode::FORM,      BadgeE(L"1w", K1),  T+1 ) ),
        SpotEventPtr( new SpotEvent( HCode::DEPARTURE, BadgeE(L"1w", K1),  T+2 ) ),
        SpotEventPtr( new SpotEvent( HCode::STATION_EXIT, BadgeE(L"nap", K1),  T+2 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"12w",K12), T+3 ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,     BadgeE(L"12w",K12), T+4 ) ),
        SpotEventPtr( new SpotEvent( HCode::FORM,      BadgeE(L"12w",K12), T+5 ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_MOVE, BadgeE(L"w12",K12), T+6 ) ),
        SpotEventPtr( new SpotEvent( HCode::STATION_ENTRY, BadgeE(L"chap", K2),  T+7 ) ),
        SpotEventPtr( new SpotEvent( HCode::ARRIVAL,   BadgeE(L"7p", K2),  T+7 ) ),
        SpotEventPtr( new SpotEvent( HCode::DEATH,     BadgeE(L"7p", K2),  T+8 ) ),
    };
    TrioSpot trio;
    trio.body( begin(pat), end(pat) );
    Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
    pusher.Action();
    auto sz = pusher.AppliedEvents().size();
    CPPUNIT_ASSERT( trio.body().size() == sz );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_path_size(10) );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Departure Station_exit Span_move Death Form Span_move Station_entry Arrival Death" ) );
    // выполняем сглаживание этой нитки
    {
        Hem::aeRectify rectifyEngine( happen_layer.GetWriteAccess(), pat[0] );
        rectifyEngine.Action();
    }

    // проверим, что осталась одна нить из двух событий
    happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_path_size(4) );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Departure Arrival Death" ) );
}

void TC_Hem_aeRectify::RectifyTransitionBetweenSpanMove()
{
    const time_t T=3600*(240+23);

    UtLayer<HappenLayer> happen_layer;

    // заполнение графика одной нитью, которая и будет исправлена
    SpotEvent pth[] = {
        SpotEvent( HCode::FORM,      BadgeE(L"a", EsrKit(11111,22222) ),  T+10 ),
        SpotEvent( HCode::SPAN_MOVE, BadgeE(L"a", EsrKit(11111,22222) ),  T+20 ),
        SpotEvent( HCode::SPAN_MOVE, BadgeE(L"b", EsrKit(33333,22222) ),  T+40 ),
        SpotEvent( HCode::DEATH,     BadgeE(L"b", EsrKit(33333,22222) ),  T+40 ),
    };
    happen_layer.createPath( pth );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Span_move Span_move Death" ) );

    // выполняем сглаживание этой нитки
    Hem::aeRectify rectifyEngine( happen_layer.GetWriteAccess(), make_shared<const SpotEvent>(pth[0]) );
    rectifyEngine.Action();

    // проверим, что осталась одна нить из четырех событий
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form          Transition Span_move      Death" ) );
    CPPUNIT_ASSERT( happen_layer.exist_series( "Bdg", "a[11111:22222] -[22222]   b[22222:33333] b[22222:33333]" ) );
}

void TC_Hem_aeRectify::Death_Form_Departure()
{
    UtLayer<HappenLayer> happen_layer;
   // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='P8P[11010:11363]' create_time='20151013T064507Z' waynum='1' />"
        L"    <SpotEvent name='Transition' Bdg='3SP:3+[11363]' create_time='20151013T064845Z' />"
        L"    <SpotEvent name='Death' Bdg='2-4SP:2-[11360]' create_time='20151013T065553Z' />"
        L"    <SpotEvent name='Form' Bdg='1C[11360]' create_time='20151013T065553Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Departure' Bdg='1C[11360]' create_time='20151013T065646Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='2-4SP:2-,4+[11343]' create_time='20151013T070602Z' />"
        L"    <SpotEvent name='Form' Bdg='1C[11343]' create_time='20151013T070602Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Departure' Bdg='1C[11343]' create_time='20151013T070642Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Transition' Bdg='1C[11342]' create_time='20151013T071336Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Transition' Bdg='2C[11340]' create_time='20151013T072209Z' waynum='2' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='2AC[11340]' create_time='20151013T072253Z' waynum='2' parknum='2' />"
        L"</HemPath>"
        );

    happen_layer.createPath( d.pretty_wstr() );
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Transition Arrival Departure Arrival Departure Transition Transition Death" ) );
}

void TC_Hem_aeRectify::Death_Form_atEnd()
{
    UtLayer<HappenLayer> happen_layer;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='2C[11310]' create_time='20151007T213014Z' waynum='2' parknum='1' />"
        L"    <SpotEvent name='Departure' Bdg='2C[11310]' create_time='20151007T213014Z' waynum='2' parknum='1' index='0001-052-1100' num='4509' resloc='Y' />"
        L"    <SpotEvent name='Death' Bdg='PKP[11310]' create_time='20151007T213441Z' />"
        L"    <SpotEvent name='Form' Bdg='2SP:2-[11271]' create_time='20151007T213615Z' />"
        L"    <SpotEvent name='Transition' Bdg='1SP[11271]' create_time='20151007T213639Z' />"
        L"    <SpotEvent name='Span_move' Bdg='N1P_BUR[11270:11271]' create_time='20151007T213639Z' waynum='1'>"
        L"        <rwcoord picketing1_val='438~300' picketing1_comm='Россия' />"
        L"        <rwcoord picketing1_val='440~300' picketing1_comm='Россия' />"
        L"    </SpotEvent>"
        L"</HemPath>"
        );

    happen_layer.createPath( d.pretty_wstr() );
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Departure Transition Span_move" ) );
}

void TC_Hem_aeRectify::SpanMove_Entrastopping()
{
    UtLayer<HappenLayer> happen_layer;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='CDP[11270]' create_time='20151007T214508Z' index='' num='2974' fretran='Y' />"
        L"    <SpotEvent name='Station_exit' Bdg='1C[11270]' create_time='20151007T214807Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Span_move' Bdg='N7P[11267:11270]' create_time='20151007T215228Z' waynum='1'>"
        L"        <rwcoord picketing1_val='428~400' picketing1_comm='Россия' />"
        L"        <rwcoord picketing1_val='429~900' picketing1_comm='Россия' />"
        L"    </SpotEvent>"
        L"    <SpotEvent name='Entra_stopping' Bdg='N7P[11267:11270]' create_time='20151007T215404Z' waynum='1'>"
        L"        <rwcoord picketing1_val='428~400' picketing1_comm='Россия' />"
        L"        <rwcoord picketing1_val='429~900' picketing1_comm='Россия' />"
        L"    </SpotEvent>"
        L"</HemPath>"
        );

    happen_layer.createPath( d.pretty_wstr() );
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Departure Entra_stopping " ) );
}

void TC_Hem_aeRectify::SpanMove_Death_Form_StationExit()
{
    UtLayer<HappenLayer> happen_layer;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='7-23SP:7+[11310]' create_time='20151007T220800Z' />"
        L"    <SpotEvent name='Station_exit' Bdg='41SP[11310]' create_time='20151007T220800Z' />"
        L"    <SpotEvent name='Span_move' Bdg='39/41P[11310:11312]' create_time='20151007T220800Z' waynum='2' />"
        L"    <SpotEvent name='Death' Bdg='39SP[11312]' create_time='20151007T221011Z' />"
        L"    <SpotEvent name='Form' Bdg='39SP:39-[11312]' create_time='20151007T223017Z' />"
        L"    <SpotEvent name='Station_exit' Bdg='39SP:39-[11312]' create_time='20151007T223043Z' />"
        L"    <SpotEvent name='Span_move' Bdg='39/41P[11310:11312]' create_time='20151007T223043Z' waynum='2' />"
        L"    <SpotEvent name='Death' Bdg='15SP:15+[11310]' create_time='20151007T223204Z' />"
        L"</HemPath>"
        );

    happen_layer.createPath( d.pretty_wstr() );
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Departure Transition Span_move Death " ) );
}

void TC_Hem_aeRectify::Arrival_Death_Form_Departure()
{
    UtLayer<HappenLayer> happen_layer;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"    <SpotEvent name='Form' Bdg='N3P_SER[11431:11432]' create_time='20150907T184247Z' waynum='1' />"
        L"    <SpotEvent name='Transition' Bdg='1C[11432]' create_time='20150907T185232Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Arrival' Bdg='1C[11432]' create_time='20150907T185332Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Death' Bdg='1C[11432]' create_time='20150907T185332Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Form' Bdg='1C[11432]' create_time='20150907T191314Z' waynum='1' parknum='1' />"
        L"    <SpotEvent name='Departure' Bdg='1C[11432]' create_time='20150907T191420Z' waynum='1' parknum='1' />"
        L"</HemPath>"
       );

    happen_layer.createPath( d.pretty_wstr() );
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Transition Arrival Departure" ) );
}

void TC_Hem_aeRectify::TermDeath_FormSpan()
{
    UtLayer<HappenLayer> hl;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Transition' Bdg='4SP[11273]' create_time='20160215T030305Z' />"
        L"  <SpotEvent name='Death'   Bdg='2SP:2-[11273]' create_time='20160215T030332Z' />"
        L"  <SpotEvent name='Form'       Bdg='P2P[11273:11330]' create_time='20160215T030337Z' waynum='1' />"
        L"  <SpotEvent name='Arrival'           Bdg='1C[11330]' create_time='20160215T031016Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure'         Bdg='1C[11330]' create_time='20160215T031101Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    hl.createPath( d.pretty_wstr() );
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Arrival Departure ") );
}

void TC_Hem_aeRectify::MultyEntraStopping()
{
    UtLayer<HappenLayer> hl;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='N11P_RE1[11272:11273]' create_time='20160308T071657Z' waynum='1'>"
        L"    <rwcoord picketing1_val='446~500' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='448~' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Entra_stopping' Bdg='N13P_RE1[11272:11273]' create_time='20160308T071801Z' waynum='1'>"
        L"    <rwcoord picketing1_val='445~895' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='446~500' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Span_move' Bdg='N13P_RE1[11272:11273]' create_time='20160308T072100Z' waynum='1'>"
        L"    <rwcoord picketing1_val='445~895' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='446~500' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        // последнее событие только что добавлено
        L"  <SpotEvent name='Entra_stopping' Bdg='N13P_RE1[11272:11273]' create_time='20160308T072100Z' waynum='1'>"
        L"    <rwcoord picketing1_val='445~895' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='446~500' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    hl.createPath( d.pretty_wstr() );
    // проверка загрузки исходных данных
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Entra_stopping Span_move Entra_stopping ") );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Entra_stopping ") ); // остался только последний сигнал Entra_Stopping
}

void TC_Hem_aeRectify::WipeDeathForm()
{
    UtLayer<HappenLayer> hl;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Form'           Bdg='NAP[11360]' create_time='20160404T135936Z' />"
        L"  <SpotEvent name='Death'          Bdg='NAP[11360]' create_time='20160404T140000Z' />"
        L"  <SpotEvent name='Form'    Bdg='N1P_VSK[11343:11360]' create_time='20160404T140005Z' waynum='1' />"
        L"  <SpotEvent name='Transition'      Bdg='1C[11343]' create_time='20160404T141025Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Death'  Bdg='1-3SP:1+,3+[11343]' create_time='20160404T141034Z' />"
        L"  <SpotEvent name='Form'           Bdg='NAP[11343]' create_time='20160404T141034Z' />"
        L"  <SpotEvent name='Death'          Bdg='NAP[11343]' create_time='20160404T141058Z' />"
        L"  <SpotEvent name='Form'   Bdg='N1P_VIG[11342:11343]' create_time='20160404T141103Z' waynum='1' />"
        L"  <SpotEvent name='Arrival'         Bdg='1C[11342]' create_time='20160404T141604Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure'       Bdg='1C[11342]' create_time='20160404T141649Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Death'  Bdg='1-3SP:1+,3+[11342]' create_time='20160404T141652Z' />"
        L"  <SpotEvent name='Form'           Bdg='NAP[11342]' create_time='20160404T141653Z' />"
        L"  <SpotEvent name='Death'          Bdg='NAP[11342]' create_time='20160404T141725Z' />"
        L"  <SpotEvent name='Form'    Bdg='N1P_AGL[11340:11342]' create_time='20160404T141730Z' waynum='1' />"
        L"  <SpotEvent name='Arrival'         Bdg='2C[11340]' create_time='20160404T142348Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Departure'       Bdg='2C[11340]' create_time='20160404T142513Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Death'     Bdg='19SP:19+[11340]' create_time='20160404T142520Z' />"
        L"  <SpotEvent name='Form'      Bdg='11SP:11+[11340]' create_time='20160404T142521Z' />"
        L"  <SpotEvent name='Death'     Bdg='11SP:11+[11340]' create_time='20160404T142529Z' />"
        L"  <SpotEvent name='Form'           Bdg='2AC[11340]' create_time='20160404T142529Z' waynum='2' parknum='2' />"
        L"  <SpotEvent name='Death'          Bdg='2AC[11340]' create_time='20160404T142553Z' waynum='2' parknum='2' />"
        L"</HemPath>"
        );
    hl.createPath( d.pretty_wstr() );
    // проверка загрузки исходных данных
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(21) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(9) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form       Departure  Transition Arrival   Departure Arrival   Departure Arrival    Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "NAP[11360] NAP[11360] 1C[11343]  1C[11342] 1C[11342] 2C[11340] 2C[11340] 2AC[11340] 2AC[11340] " ));
}

void TC_Hem_aeRectify::MisplacedDeath()
{
    UtLayer<HappenLayer> hl;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='SV9C[09764:09772]' create_time='20160419T064355Z' waynum='1' index='' num='' noinfo='Y'>"
        L"    <rwcoord picketing1_val='133~399' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='132~748' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Station_entry' Bdg='NDP[09772]' create_time='20160419T065453Z' />"
        L"  <SpotEvent name='Death' Bdg='SV1C[09764:09772]' create_time='20160419T065532Z' waynum='1'>"
        L"    <rwcoord picketing1_val='127~310' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='126~571' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Form' Bdg='1C[09772]' create_time='20160419T065533Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Death' Bdg='1C[09772]' create_time='20160419T065556Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Form' Bdg='1C[09772]' create_time='20160419T065627Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='1C[09772]' create_time='20160419T065737Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Station_exit' Bdg='PDP[09772]' create_time='20160419T065818Z' />"
        L"  <SpotEvent name='Transition' Bdg='3C[09750]' create_time='20160419T072452Z' waynum='3' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[09751]' create_time='20160419T074406Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='ZK2C[09751:09780]' create_time='20160419T075949Z' waynum='1'>"
        L"    <rwcoord picketing1_val='86~22' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='86~747' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    hl.createPath( d.pretty_wstr() );
    // проверка загрузки исходных данных
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(11) );
    //CPPUNIT_ASSERT( hl.exist_series( "name", "Form Entra_stopping Span_move Entra_stopping ") );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Arrival Departure Transition Transition Span_move ") );
}

void TC_Hem_aeRectify::DeathForm_HereAndThere()
{
    UtLayer<HappenLayer> hl;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Transition' Bdg='1C[09211]' create_time='20160419T194637Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='VL2C[09211:09220]' create_time='20160419T195828Z' waynum='1' />"
        L"    <SpotEvent name='Death' Bdg='VL3C[09211:09220]' create_time='20160419T195952Z' waynum='1' />"
        L"    <SpotEvent name='Form' Bdg='NDP[09220]' create_time='20160419T200051Z' />"
        L"    <SpotEvent name='Station_exit' Bdg='NDP[09220]' create_time='20160419T200051Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='NDP[09220]' create_time='20160419T200052Z' />"
        L"  <SpotEvent name='Transition' Bdg='1C[09220]' create_time='20160419T200121Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='NDP[09230]' create_time='20160419T200955Z' />"
        L"</HemPath>"
        );
    hl.createPath( d.pretty_wstr() );
    // проверка загрузки исходных данных
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(8) );
    //CPPUNIT_ASSERT( hl.exist_series( "name", "Form Entra_stopping Span_move Entra_stopping ") );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Transition Transition Transition " ) );
}

void TC_Hem_aeRectify::DeathForm_Instant()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Span_move' Bdg='KNUU1_D[09676:09715]' create_time='20160609T205959Z' waynum='1' />"
        L"  <SpotEvent name='Span_move'  Bdg='KNUU2_D[09676:09715]' create_time='20160609T210035Z' waynum='1' />"
        L"  <SpotEvent name='Death'      Bdg='KNUU2_D[09676:09715]' create_time='20160609T210035Z' waynum='1' />"
        L"  <SpotEvent name='Form'       Bdg='KNUU2_D[09676:09715]' create_time='20160609T210035Z' waynum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='KNU15_D[09676:09715]' create_time='20160609T210243Z' waynum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='KNUU2_D[09676:09715]' create_time='20160609T210245Z' waynum='1' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move Span_move" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "KNUU1_D[09676:09715] KNUU2_D[09676:09715]" ) );
    CPPUNIT_ASSERT( hl.exist_path_size(2) );
}

void TC_Hem_aeRectify::DeathForm_case9()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Form'          Bdg='2SP+[09010]' create_time='20160615T151405Z' index='' num='' noinfo='Y' />"
        L"  <SpotEvent name='Station_exit'  Bdg='2SP[09010]' create_time='20160615T151405Z' />"
        L"  <SpotEvent name='Transition'    Bdg='2p[09100]' create_time='20160615T151817Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Station_entry' Bdg='KPU13A/6A[09104]' create_time='20160615T152439Z' />"
        L"  <SpotEvent name='Death'         Bdg='KPU13A/6A[09104]' create_time='20160615T152706Z' />"
        L"  <SpotEvent name='Form'          Bdg='KPU11A_O[09104:09150]' create_time='20160615T152707Z' waynum='1' />"
        L"  <SpotEvent name='Span_move'     Bdg='KPU9A_O[09104:09150]' create_time='20160615T152814Z' waynum='1' />"
        L"  <SpotEvent name='Death'         Bdg='KPU9A_O[09104:09150]' create_time='20160615T152957Z' waynum='1' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(8) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form         Departure  Transition     Transition       Span_move              Death         " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "2SP+[09010] 2SP+[09010] 2p[09100]  KPU13A/6A[09104] KPU9A_O[09104:09150] KPU9A_O[09104:09150]" ) );
}

void TC_Hem_aeRectify::DeathForm_caseA()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Span_move'     Bdg='m[01777:01999]'  create_time='20160622T000000Z' waynum='1' />"
        L"  <SpotEvent name='Arrival'       Bdg='alfa[01777]'     create_time='20160622T011111Z' />"
        L"  <SpotEvent name='Death'         Bdg='beta[01777]'     create_time='20160622T022222Z' />"
        L"  <SpotEvent name='Form'          Bdg='mm[01555:01777]' create_time='20160622T033333Z' waynum='2' />"
        L"  <SpotEvent name='Station_entry' Bdg='gamm[01777]'     create_time='20160622T044444Z' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move      Arrival     Departure   Station_entry " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "m[01777:01999] alfa[01777] beta[01777] gamm[01777] " ) );
}

void TC_Hem_aeRectify::DeathForm_Span()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170712T050633Z' name='Form'       Bdg='5p[09100]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20170712T050633Z' name='Departure'  Bdg='5p[09100]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20170712T053524Z' name='Transition' Bdg='1p[09715]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170712T053914Z' name='Span_move'  Bdg='KNU17_S[09715:09730]' waynum='1' />"
        L"  <SpotEvent create_time='20170712T054017Z' name='Death'      Bdg='KNU17_S[09715:09730]' waynum='1' />"
        L"  <SpotEvent create_time='20170712T054018Z' name='Form'       Bdg='KNU15_S[09715:09730]' waynum='1' />"
        L"  <SpotEvent create_time='20170712T054024Z' name='Death'      Bdg='KNU15_S[09715:09730]' waynum='1' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form      Departure Transition Span_move            Death                Form                 Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "5p[09100] 5p[09100] 1p[09715]  KNU17_S[09715:09730] KNU17_S[09715:09730] KNU15_S[09715:09730] KNU15_S[09715:09730] " ) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form      Departure Transition Span_move            Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "5p[09100] 5p[09100] 1p[09715]  KNU15_S[09715:09730] KNU15_S[09715:09730] " ) );
}

void TC_Hem_aeRectify::DeathForm_inside_SpanStopping()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Form'       Bdg='2SP+[09010]' create_time='20160627T043822Z' index='' num='6503' subur='Y' />"
        L"  <SpotEvent name='Departure'  Bdg='2SP[09010]' create_time='20160627T043822Z' />"
        L"  <SpotEvent name='Transition' Bdg='4p[09100]' create_time='20160627T044310Z' waynum='4' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='2p[09730]' create_time='20160627T052815Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Span_stopping_begin' Bdg='CKPU2_K[09730:09732]' create_time='20160627T052951Z' waynum='1' />"
        L"  <SpotEvent name='Death' Bdg='CKPU2_K[09730:09732]' create_time='20160627T053033Z' waynum='1' />"
        L"  <SpotEvent name='Form'  Bdg='KU6/7_K[09730:09732]' create_time='20160627T053033Z' waynum='1' />"
        L"  <SpotEvent name='Span_stopping_end' Bdg='CKPU2_K[09730:09732]' create_time='20160627T053033Z' waynum='1' />"
        L"  <SpotEvent name='Arrival'   Bdg='1p[09732]' create_time='20160627T053601Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='1p[09732]' create_time='20160627T053845Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(10) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Transition Transition Span_stopping_begin Span_stopping_end Arrival Departure " ) );
}

void TC_Hem_aeRectify::DeathForm_caseB()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Transition' Bdg='1C[11777]' create_time='20160627T171310Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Span_move'  Bdg='N9P[11555:11777]' create_time='20160627T171409Z' waynum='1' />"
        L"  <SpotEvent name='Death'      Bdg='N9P[11555:11777]' create_time='20160627T171409Z' waynum='1' />"
        L"  <SpotEvent name='Form'       Bdg='2-4SP:2-[11555]'  create_time='20160627T171413Z' />"
        L"  <SpotEvent name='Arrival'    Bdg='1C[11555]' create_time='20160627T171413Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Departure'  Bdg='1C[11555]' create_time='20160627T171413Z' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Arrival Departure " ) );
}

void TC_Hem_aeRectify::RoundTime()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Arrival' Bdg='3AC[09670]' create_time='20160628T131629Z' waynum='3' parknum='3' />"
        L"  <SpotEvent name='Departure' Bdg='3AC[09670]' create_time='20160628T131629Z' waynum='3' parknum='3' />"
        L"  <SpotEvent name='Station_entry' Bdg='2P[11010]' create_time='20160628T131629Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Station_exit' Bdg='2P[11010]' create_time='20160628T131629Z' waynum='2' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='NpP2[09670]' create_time='20160628T131644Z' />"
        L"  <SpotEvent name='Span_move' Bdg='P6P_Z[09100:09670]' create_time='20160628T131834Z' waynum='2' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Arrival Departure Transition Transition Span_move" ) );
}

// чистка Death/Form - станции встык
void TC_Hem_aeRectify::DeathForm_TermTerm()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='32C[09180]' create_time='20160705T154847Z' waynum='32' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='32C[09180]' create_time='20160705T154847Z' waynum='32' parknum='1' />"
        L"  <SpotEvent name='Station_exit' Bdg='NBP[09180]' create_time='20160705T155154Z' />"
        L"  <SpotEvent name='Station_entry' Bdg='BGP[09162]' create_time='20160705T155154Z' waynum='2' parknum='1'>"
        L"    <rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Station_exit' Bdg='BGP[09162]' create_time='20160705T155202Z' waynum='2' parknum='1'>"
        L"    <rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Station_entry' Bdg='NBP[09180]' create_time='20160705T155202Z' />"
        L"  <SpotEvent name='Death' Bdg='NBP[09180]' create_time='20160705T155204Z' />"
        L"  <SpotEvent name='Form' Bdg='BGP[09162]' create_time='20160705T155206Z' waynum='2' parknum='1'>"
        L"    <rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='BGP[09162]' create_time='20160705T155421Z' waynum='2' parknum='1'>"
        L"    <rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(9) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Transition Transition Arrival Death" ) );
}


// чистка Death/Form - смежные перегоны
void TC_Hem_aeRectify::DeathForm_SpanSpan()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170706T031622Z' name='Transition' Bdg='2C[09700]' waynum='2' parknum='1' index='0001-025-0900' num='4820' resloc='Y' />"
        L"  <SpotEvent create_time='20170706T031826Z' name='Span_move'  Bdg='PGP[09700:09701]' waynum='2' />"
        L"  <SpotEvent create_time='20170706T032321Z' name='Death'      Bdg='PGP[09700:09701]' waynum='2' />"

        L"  <SpotEvent create_time='20170706T032322Z' name='Form'       Bdg='NGP[09680:09701]' />"
        L"  <SpotEvent create_time='20170706T032432Z' name='Span_move'  Bdg='NGP[09680:09701]' waynum='3' />"
        L"  <SpotEvent create_time='20170706T033053Z' name='Transition' Bdg='1C[09680]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );

    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Transition Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "2C[09700]  -[09701]   1C[09680]  " ) );
}

// чистка
void TC_Hem_aeRectify::DeathForm_caseC()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20160811T083959Z' name='Form' Bdg='PAP[11330]' index='' num='' noinfo='Y'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T083959Z' name='Departure' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T084009Z' name='Transition' Bdg='2-4SP[11330]' />"
        L"  <SpotEvent create_time='20160811T085657Z' name='Span_stopping_begin' Bdg='N15P[11330:11332]' waynum='1' intervalSec='1006'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092155Z' name='Span_stopping_end' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092222Z' name='Transition' Bdg='PAP[11330]' />"
        L"  <SpotEvent create_time='20160811T092253Z' name='Span_move' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092253Z' name='Death' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092256Z' name='Form' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092257Z' name='Departure' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092305Z' name='Span_move' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092305Z' name='Death' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092308Z' name='Form' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092309Z' name='Departure' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092353Z' name='Transition' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092402Z' name='Span_move' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092402Z' name='Death' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092405Z' name='Form' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092406Z' name='Departure' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092414Z' name='Span_move' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092414Z' name='Death' Bdg='N15P[11330:11332]' waynum='1'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"    <rwcoord picketing1_val='465~975' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092417Z' name='Form' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092418Z' name='Departure' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092437Z' name='Arrival' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092444Z' name='Departure' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092505Z' name='Death' Bdg='PAP[11330]'>"
        L"    <rwcoord picketing1_val='464~300' picketing1_comm='Россия' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160811T092509Z' name='Form' Bdg='2-4SP:2+[11330]' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(27) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(10) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form       Departure  Transition   Span_stopping_begin Span_stopping_end Transition Arrival    Departure  Death      Form " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "PAP[11330] PAP[11330] 2-4SP[11330] N15P[11330:11332]   N15P[11330:11332] PAP[11330] PAP[11330] PAP[11330] PAP[11330] 2-4SP:2+[11330] " ) );
}

// чистка Move/Death/Form причем Move на том же месте и времени что и Death
void TC_Hem_aeRectify::DeathForm_caseD()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20161112T143213Z' name='Form' Bdg='A[09150]' waynum='11' parknum='1' index='0900-778-0918' num='3243' length='57' weight='1667' fretran='Y'>"
        L"    <Locomotive Series='2М62У' NumLoc='268' Dor='0' Depo='0' Kod='0'>"
        L"      <Crew EngineDriver='PANKEVICH' Tim_Beg='2016-11-12 09:35' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161112T143213Z' name='Departure' Bdg='A[09150]' waynum='11' parknum='1' />"
        L"  <SpotEvent create_time='20161112T151614Z' name='Span_move' Bdg='CAKPU1_C[09150:09160]' waynum='1'>"
        L"    <rwcoord picketing1_val='22~900' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='24~400' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161112T151614Z' name='Death' Bdg='CAKPU1_C[09150:09160]' waynum='1'>"
        L"    <rwcoord picketing1_val='22~900' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='24~400' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161112T151620Z' name='Form' Bdg='CAP:6/8+,10+[09150]' />"
        L"  <SpotEvent create_time='20161112T151620Z' name='Station_exit' Bdg='CAP:2/4+[09150]'>"
        L"    <rwcoord picketing1_val='22~900' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161112T151620Z' name='Span_move' Bdg='CAKPU1_C[09150:09160]' waynum='1'>"
        L"    <rwcoord picketing1_val='22~900' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='24~400' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161112T151620Z' name='Span_move' Bdg='CAKPU2_C[09150:09160]' waynum='1'>"
        L"    <rwcoord picketing1_val='24~400' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(8) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form     Departure Transition          Span_move             " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "A[09150] A[09150]  CAP:6/8+,10+[09150] CAKPU2_C[09150:09160] " ) );
}

// чистка множественных Death-Form при входе на станцию
void TC_Hem_aeRectify::DeathForm_caseE()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20161226T134341Z' name='Form' Bdg='SS7[11082:11090]' waynum='1'>"
        L"    <rwcoord picketing1_val='144~500' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='147~500' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T134632Z' name='Span_move' Bdg='SS1[11082:11090]' waynum='1'>"
        L"    <rwcoord picketing1_val='142~500' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='141~600' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T134747Z' name='Death' Bdg='SS1[11082:11090]' waynum='1'>"
        L"    <rwcoord picketing1_val='142~500' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='141~600' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T134748Z' name='Form' Bdg='NDP[11090]'>"
        L"    <rwcoord picketing1_val='141~600' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T134753Z' name='Death' Bdg='NDP[11090]'>"
        L"    <rwcoord picketing1_val='141~600' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T134804Z' name='Form' Bdg='1SP:1+[11090]' />"
        L"  <SpotEvent create_time='20161226T134814Z' name='Arrival' Bdg='1C[11090]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161226T134940Z' name='Death' Bdg='PDP[11090]'>"
        L"    <rwcoord picketing1_val='140~600' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T134945Z' name='Form' Bdg='BS9[11090:11091]' waynum='1'>"
        L"    <rwcoord picketing1_val='138~600' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='136~600' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T135802Z' name='Span_move' Bdg='BS1[11090:11091]' waynum='1'>"
        L"    <rwcoord picketing1_val='131~600' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='130~' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T135923Z' name='Station_entry' Bdg='NDP[11091]'>"
        L"    <rwcoord picketing1_val='130~' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161226T140000Z' name='Arrival' Bdg='1C[11091]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161226T140132Z' name='Death' Bdg='PDP[11091]'>"
        L"    <rwcoord picketing1_val='129~' picketing1_comm='Земитани-Валга' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(13) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form             Arrival   Departure  Arrival   Death " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "SS7[11082:11090] 1C[11090] PDP[11090] 1C[11091] PDP[11091] " ) );
}

void TC_Hem_aeRectify::Move_Around_SpanStopping()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent name='Transition'          Bdg='1A[09730]'       create_time='20160627T000100Z' waynum='1' />"
        L"  <SpotEvent name='Span_move'           Bdg='U1[09730:09732]' create_time='20160627T000200Z' waynum='1' />"
        L"  <SpotEvent name='Span_stopping_begin' Bdg='U1[09730:09732]' create_time='20160627T000300Z' waynum='1' />"
        L"  <SpotEvent name='Span_stopping_end'   Bdg='U1[09730:09732]' create_time='20160627T000400Z' waynum='1' />"
        L"  <SpotEvent name='Span_move'           Bdg='U2[09730:09732]' create_time='20160627T000500Z' waynum='1' />"
        L"  <SpotEvent name='Span_stopping_begin' Bdg='U2[09730:09732]' create_time='20160627T000600Z' waynum='1' />"
        L"  <SpotEvent name='Span_stopping_end'   Bdg='U2[09730:09732]' create_time='20160627T000700Z' waynum='1' />"
        L"  <SpotEvent name='Span_move'           Bdg='U3[09730:09732]' create_time='20160627T000800Z' waynum='1' />"
        L"  <SpotEvent name='Transition'                Bdg='2B[09732]' create_time='20160627T000900Z' waynum='1' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(9) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Span_stopping_begin Span_stopping_end Span_stopping_begin Span_stopping_end Transition" ) );
}

void TC_Hem_aeRectify::WipePocketEntryExit()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20161129T101356Z' name='Form' Bdg='B2IP[09160:09162]' waynum='2'>"
        L"    <rwcoord picketing1_val='37~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T102117Z' name='Transition' Bdg='3p[09160]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20161129T102144Z' name='Span_move' Bdg='NBKPU1_C[09150:09160]' waynum='2'>"
        L"    <rwcoord picketing1_val='30~900' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='32~169' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T102239Z' name='Pocket_entry' Bdg='NBKPU1_X[09150:09160]' waynum='2'>"
        L"    <rwcoord picketing1_val='30~900' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T102516Z' name='Pocket_exit' Bdg='NBKPU1_X[09150:09160]' waynum='2'>"
        L"    <rwcoord picketing1_val='28~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T102806Z' name='Span_move' Bdg='CBKPU1_C[09150:09160]' waynum='2'>"
        L"    <rwcoord picketing1_val='22~900' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='24~400' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T102924Z' name='Arrival' Bdg='5Ap[09150]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20161129T103102Z' name='Departure' Bdg='5Ap[09150]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20161129T103102Z' name='Transition' Bdg='5p[09150]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20161129T103129Z' name='Span_move' Bdg='NBKPU1_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='21~' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T103244Z' name='Pocket_entry' Bdg='NBKPU1_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T103515Z' name='Pocket_exit' Bdg='KPU10V/5V_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T104210Z' name='Span_move' Bdg='CBKPU1_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='7~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='8~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161129T104409Z' name='Transition' Bdg='NBP:2/4+[09104]'>"
        L"    <rwcoord picketing1_val='7~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(14) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Transition Arrival Departure Transition Transition " ) );
}

void TC_Hem_aeRectify::WipePocket_with_Stopping()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20161218T130630Z' name='Transition' Bdg='5p[09150]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20161218T130657Z' name='Span_move' Bdg='NBKPU1_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='21~' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161218T130809Z' name='Pocket_entry' Bdg='NBKPU1_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161218T133210Z' name='Span_stopping_begin' Bdg='NBKPU2_O[09104:09150]' waynum='2' intervalSec='1441'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161218T150716Z' name='Span_stopping_end' Bdg='NBKPU2_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161218T150716Z' name='Pocket_exit' Bdg='KPU10V/5V_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161218T151651Z' name='Span_move' Bdg='CBKPU1_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='7~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='8~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161218T151834Z' name='Transition' Bdg='NBP[09104]'>"
        L"    <rwcoord picketing1_val='6~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(8) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Span_move Pocket_entry Span_stopping_begin Span_stopping_end Pocket_exit Span_move Transition " ) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Transition Span_stopping_begin Span_stopping_end Transition" ) );
}

void TC_Hem_aeRectify::WipePocketEntryDeath()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20161219T091705Z' name='Span_move' Bdg='KPU10V/5V_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='15~100' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161219T092027Z' name='Pocket_entry' Bdg='KPU10V/5V_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161219T094428Z' name='Span_stopping_begin' Bdg='NBKPU2_O[09104:09150]' waynum='2' intervalSec='1441'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161219T102945Z' name='Death' Bdg='NBKPU2_O[09104:09150]' waynum='2'>"
        L"    <rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move Pocket_entry Span_stopping_begin Death " ) );
    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move Span_stopping_begin Death " ) );
}

static SpotEventPtr selectByCode( HappenLayer &hl, HCode need )
{
    auto beforeAll = hl.GetPaths_OnlyEvents( hl.GetLatestTime()-10*3600, hl.GetLatestTime()+1 );
    for( auto& blist : beforeAll )
    {
        for( auto& bevent : blist )
        {
            if ( need == bevent->GetCode() )
            {
                return bevent;
            }
        }
    }
    return SpotEventPtr();
}

void TC_Hem_aeRectify::RefineSpanstopping()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170504T101416Z' name='Form' Bdg='1N2IP[11041:11052]' waynum='1' index='' num='D2831' fretran='Y'>"
        L"    <rwcoord picketing1_val='461~750' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170504T110827Z' name='Transition' Bdg='1C[11052]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170504T112402Z' name='Transition' Bdg='1C[11050]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170504T113215Z' name='Span_stopping_begin' Bdg='SK8C[11050:11051]' waynum='1' intervalSec='337'>"
        L"    <rwcoord picketing1_val='430~400' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='430~800' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170504T113215Z' name='Span_stopping_end' Bdg='SK8C[11050:11051]' waynum='1'>"
        L"    <rwcoord picketing1_val='430~400' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='430~800' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170504T113800Z' name='Transition' Bdg='1C[11051]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170504T121213Z' name='Span_move' Bdg='N1P[11051:11060]' waynum='1'>"
        L"    <rwcoord picketing1_val='14~665' picketing1_comm='524км' />"
        L"    <rwcoord picketing1_val='14~352' picketing1_comm='524км' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );

    time_t stopper = 0;
    {
        attic::a_attribute temp = d.document_element().append_attribute("time_conv");
        temp.set_value("20170504T113215Z");
        stopper = temp.as_time();
        d.document_element().remove_attribute(temp);
    }

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );

    SpotEventPtr tempEv;
    tempEv = selectByCode(hl, HCode::SPAN_STOPPING_BEGIN);
    CPPUNIT_ASSERT( stopper == tempEv->GetTime() );
    CPPUNIT_ASSERT( 337 == tempEv->GetOffsetInterval() );
    tempEv = selectByCode(hl, HCode::SPAN_STOPPING_END);
    CPPUNIT_ASSERT( stopper == tempEv->GetTime() );
    CPPUNIT_ASSERT( 0 == tempEv->GetOffsetInterval() );

    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
    tempEv = selectByCode(hl, HCode::SPAN_STOPPING_BEGIN);
    CPPUNIT_ASSERT( stopper-337 == tempEv->GetTime() );
    CPPUNIT_ASSERT( 0 == tempEv->GetOffsetInterval() );
    tempEv = selectByCode(hl, HCode::SPAN_STOPPING_END);
    CPPUNIT_ASSERT( stopper == tempEv->GetTime() );
    CPPUNIT_ASSERT( 0 == tempEv->GetOffsetInterval() );
}

void TC_Hem_aeRectify::DeathForm_TermTransition()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170706T031622Z' name='Form' Bdg='2C[09700]' waynum='2' parknum='1' index='0001-025-0900' num='4820' resloc='Y'>"
        L"    <Locomotive Series='2М62' NumLoc='1194' Depo='0' Consec='1'>"
        L"      <Crew EngineDriver='RABINS' Tim_Beg='2017-07-05 20:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T031622Z' name='Departure' Bdg='2C[09700]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20170706T031714Z' name='Station_exit' Bdg='NP[09700]'>"
        L"    <rwcoord picketing1_val='8~' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T031826Z' name='Span_move' Bdg='PGP[09700:09701]' waynum='2'>"
        L"    <rwcoord picketing1_val='7~' picketing1_comm='Засулаукс' />"
        L"    <rwcoord picketing1_val='4~500' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T032010Z' name='Station_entry' Bdg='PP[09701]'>"
        L"    <rwcoord picketing1_val='4~500' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T032321Z' name='Death' Bdg='13C[09701]' waynum='13' parknum='1' />"
        L"  <SpotEvent create_time='20170706T032322Z' name='Form' Bdg='23-27SP[09701]' />"
        L"  <SpotEvent create_time='20170706T032432Z' name='Station_exit' Bdg='NP[09701]'>"
        L"    <rwcoord picketing1_val='6~500' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T032432Z' name='Span_move' Bdg='NGP[09680:09701]' waynum='3'>"
        L"    <rwcoord picketing1_val='4~400' picketing1_comm='Засулаукс' />"
        L"    <rwcoord picketing1_val='6~500' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T032750Z' name='Station_entry' Bdg='PP[09680]'>"
        L"    <rwcoord picketing1_val='3~900' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T033053Z' name='Transition' Bdg='1C[09680]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170706T033145Z' name='Station_exit' Bdg='NP[09680]'>"
        L"    <rwcoord picketing1_val='1~990' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T033145Z' name='Span_move' Bdg='NGP[09670:09680]' waynum='3'>"
        L"    <rwcoord picketing1_val='1~515' picketing1_comm='Засулаукс' />"
        L"    <rwcoord picketing1_val='1~990' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T033404Z' name='Station_entry' Bdg='PBP[09670]'>"
        L"    <rwcoord picketing1_val='0~938' picketing1_comm='Засулаукс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170706T033728Z' name='Transition' Bdg='1BC[09670]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20170706T033854Z' name='Transition' Bdg='1C[09670]' waynum='1' parknum='2' />"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(16) );

    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form      Departure Transition Transition Transition Transition " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "2C[09700] 2C[09700] 13C[09701] 1C[09680]  1BC[09670] 1C[09670]  " ) );
}

// чистка Death/Form - станция, перегон+движение по неправильному пути
void TC_Hem_aeRectify::DeathForm_TermSpanWrongWay()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"  <SpotEvent create_time='20170905T082913Z' name='Form' Bdg='5p[09100]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20170905T082913Z' name='Departure' Bdg='5p[09100]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20170905T083129Z' name='Death' Bdg='ww[09100]'>"
        L"    <rwcoord picketing1_val='3~200' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170905T083131Z' name='Form' Bdg='ww[09100:09104]' index='' num='' noinfo='Y' waynum='2' >"
        L"    <rwcoord picketing1_val='3~200' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='5~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20170905T083131Z' name='Wrong_way' Bdg='ww[09100:09104]' waynum='2' >"
        L"    <rwcoord picketing1_val='3~200' picketing1_comm='Торнянкалнс-Елгава' />"
        L"    <rwcoord picketing1_val='5~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );

    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form      Departure Wrong_way       " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "5p[09100] 5p[09100] ww[09100:09104] " ) );
}

// чистка Death/Form - совпадение со следующим движением по перегону
void TC_Hem_aeRectify::DeathForm_IdentSpanMove()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"<SpotEvent create_time='20190219T173135Z' name='Form' Bdg='R5P[09006:09010]' waynum='1' index='' num='6735' suburbreg='Y'>"
        L"<rwcoord picketing1_val='3~265' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='2~865' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190219T173621Z' name='Transition' Bdg='7C[09010]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20190219T181323Z' name='Arrival' Bdg='2p[09160]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190219T181451Z' name='Departure' Bdg='2p[09160]' waynum='2' parknum='1' optCode='09160:09162' />"
        L"<SpotEvent create_time='20190219T181522Z' name='Death' Bdg='CAP[09160]'>"
        L"<rwcoord picketing1_val='33~600' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<Event create_time='20190219T181523Z' name='Form' Bdg='CAKPU1[09160:09162]' index='' num='6735' suburbreg='Y' waynum='1' id='11' infsrc='guess'>"
        L"<rwcoord picketing1_val='33~600' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='34~600' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</Event>"
        L"<Event create_time='20190219T181523Z' name='Span_move' Bdg='CAKPU1[09160:09162]' waynum='1' id='11' infsrc='guess'>"
        L"<rwcoord picketing1_val='33~600' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='34~600' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</Event>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );

    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form             Transition Arrival   Departure Span_move " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "R5P[09006:09010] 7C[09010]  2p[09160] 2p[09160] CAKPU1[09160:09162] " ) );
}

// чистка Death/Form - совпадение со следующим движением по перегону
void TC_Hem_aeRectify::DeathForm_IdentSpanMove2()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"<SpotEvent create_time='20200208T110713Z' name='Form' Bdg='3C[11090]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200208T110713Z' name='Departure' Bdg='3C[11090]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200208T111651Z' name='Station_entry' Bdg='PDP[11082]'>"
        L"    <rwcoord picketing1_val='154~500' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200208T111806Z' name='Death' Bdg='NDP[11082]'>"
        L"    <rwcoord picketing1_val='155~500' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200208T111811Z' name='Form' Bdg='SL1[11081:11082]' index='' num='' noinfo='Y' waynum='1' id='95' infsrc='guess'>"
        L"    <rwcoord picketing1_val='156~500' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='155~500' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200208T111811Z' name='Span_move' Bdg='SL1[11081:11082]' waynum='1' id='95' infsrc='guess'>"
        L"    <rwcoord picketing1_val='156~500' picketing1_comm='Земитани-Валга' />"
        L"    <rwcoord picketing1_val='155~500' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"</HemPath>"

        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form      Departure Station_entry Death      Form             Span_move " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "3C[11090] 3C[11090] PDP[11082]    NDP[11082] SL1[11081:11082] SL1[11081:11082] " ) );

    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form      Departure Transition Span_move " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "3C[11090] 3C[11090] PDP[11082] SL1[11081:11082] " ) );
}

// чистка Death/Form - совпадение со следующим движением по перегону
void TC_Hem_aeRectify::DeathForm_IdentSpanMove3()
{
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"<SpotEvent create_time='20200208T110612Z' name='Form' Bdg='P26PC[11310:11380]' waynum='1'>"
        L"    <rwcoord picketing1_val='206~270' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='205~990' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200208T112222Z' name='Arrival' Bdg='1AC[11310]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200208T112632Z' name='Departure' Bdg='1C[11310]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200208T112743Z' name='Arrival' Bdg='1PG[11320]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200208T112915Z' name='Death' Bdg='NP[11320]'>"
        L"    <rwcoord picketing1_val='227~718' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"

        L"<SpotEvent create_time='20200208T112917Z' name='Form' Bdg='BPR[11302:11320]' index='' num='' noinfo='Y' waynum='1' id='114' infsrc='guess'>"
        L"    <rwcoord picketing1_val='229~150' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='227~718' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200208T112917Z' name='Span_move' Bdg='BPR[11302:11320]' waynum='1' id='114' infsrc='guess'>"
        L"    <rwcoord picketing1_val='229~150' picketing1_comm='Рига-Зилупе' />"
        L"    <rwcoord picketing1_val='227~718' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );
    UtLayer<HappenLayer> hl;
    hl.createPath( d.pretty_wstr() );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form               Arrival    Departure Arrival    Death     Form             Span_move " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "P26PC[11310:11380] 1AC[11310] 1C[11310] 1PG[11320] NP[11320] BPR[11302:11320] BPR[11302:11320] " ) );

    // чистка
    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );
    Hem::aeRectify rectify( hl.GetWriteAccess(), event );
    rectify.Action();
    // проверка результата
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form               Arrival    Departure Arrival    Departure Span_move " ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "P26PC[11310:11380] 1AC[11310] 1C[11310] 1PG[11320] NP[11320] BPR[11302:11320] " ) );
}

void TC_Hem_aeRectify::RemoveSpanStoppingOnTheSamePlace()
{
    UtLayer<HappenLayer> happen_layer;

    BadgeE bdg(L"8P_L", EsrKit(9380,9400));
    time_t t = 3600*(240+23);

    SpotEventPtr pat[] = {
        SpotEventPtr( new SpotEvent( HCode::FORM,      bdg,  t ) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_STOPPING_END, bdg, t) ),
        SpotEventPtr( new SpotEvent( HCode::SPAN_STOPPING_BEGIN,   bdg, t ) ),
    };
    TrioSpot trio;
    trio.body( begin(pat), end(pat) );
    Hem::aePusherAutoHappen pusher( happen_layer.GetWriteAccess(), trio, identifyCategory );
    pusher.Action();
    auto sz = pusher.AppliedEvents().size();
    CPPUNIT_ASSERT( trio.body().size() == sz );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_path_size(3) );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Span_stopping_end Span_stopping_begin" ) );
    // выполняем сглаживание этой нитки
    {
        Hem::aeRectify rectifyEngine( happen_layer.GetWriteAccess(), pat[0] );
        rectifyEngine.Action();
    }

    // проверим, что осталась одна нить из двух событий
    happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_path_size(1) );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form" ) );
}

void TC_Hem_aeRectify::SpanStoppingBeginAndDeathForm()
{
    UtLayer<HappenLayer> happen_layer;

    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"<SpotEvent create_time='20200806T053417Z' name='Form' Bdg='3C[11270]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200806T053417Z' name='Departure' Bdg='3C[11270]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200806T055621Z' name='Span_stopping_begin' Bdg='N1P_BUR[11270:11271]' waynum='1' intervalSec='1009'>"
        L"<rwcoord picketing1_val='439~100' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='440~780' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200806T065552Z' name='Death' Bdg='N1P_BUR[11270:11271]' waynum='1'>"
        L"<rwcoord picketing1_val='439~100' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='440~780' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200806T065556Z' name='Form' Bdg='N1P_BUR[11270:11271]' waynum='1'>"
        L"<rwcoord picketing1_val='439~100' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='440~780' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200806T085511Z' name='Death' Bdg='N1P_BUR[11270:11271]' waynum='1' >"
        L"<rwcoord picketing1_val='439~100' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='440~780' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    happen_layer.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Departure Span_stopping_begin Death Form Death" ) );

    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Form Departure Span_stopping_begin Death" ) );
}

void TC_Hem_aeRectify::Check_Path()
{
    UtLayer<HappenLayer> happen_layer;
    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"<SpotEvent create_time='20191126T210400Z' name='ExplicitForm' Bdg='ASOUP 1042[09180]' waynum='8' parknum='1' index='0918-030-0982' num='V3441' length='7' weight='598' net_weight='421' modular='Y'>"
        L"<Locomotive Series='М62' NumLoc='1598' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GERASEMCUKS' Tim_Beg='2019-11-27 05:46' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20191127T050927Z' name='Departure' Bdg='15C[09180]' waynum='15' parknum='1' />"
        L"<SpotEvent create_time='20191127T063147Z' name='Station_exit' Bdg='PDP[09180]' />"
        L"<SpotEvent create_time='20191127T064607Z' name='Station_entry' Bdg='NDP[09764]' />"
        L"<SpotEvent create_time='20191127T070438Z' name='Departure' Bdg='5BC[09764]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );

    happen_layer.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "ExplicitForm Departure Station_exit Station_entry Departure" ) );

    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "ExplicitForm Departure Arrival Departure" ) );
}

void TC_Hem_aeRectify::Arrival_Departure_Death()
{
    UtLayer<HappenLayer> happen_layer;

    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"<SpotEvent create_time='20200727T073400Z' name='Departure' Bdg='ASOUP 1042[09640]' waynum='9' parknum='2' />"
        L"<SpotEvent create_time='20200727T073700Z' name='Arrival' Bdg='ASOUP 1042[09640]' waynum='3' parknum='3' />"
        L"<SpotEvent create_time='20200806T055621Z' name='Death' Bdg='ASOUP 1042[09640]' waynum='3' parknum='3'>"
        L"</HemPath>"
        );

    happen_layer.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Departure Arrival Death" ) );

    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Departure Arrival Death" ) );
}

void TC_Hem_aeRectify::WrongReplace()
{
    UtLayer<HappenLayer> happen_layer;

    // первая нить
    attic::a_document d;
    d.load_wide(
        L"<HemPath>"
        L"<SpotEvent create_time='20200918T053551Z' name='Transition' Bdg='1C[11342]' waynum='1' parknum='1' intervalSec='49' optCode='11340:11342' />"
        L"<SpotEvent create_time='20200918T054644Z' name='Arrival' Bdg='2C[11340]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200918T055725Z' name='Death' Bdg='11SP[11340]' />"
        L"<SpotEvent create_time='20200918T055726Z' name='Form' Bdg='2AC[11340]' waynum='2' parknum='2' />"
        L"<SpotEvent create_time='20200918T055850Z' name='Station_exit' Bdg='NAP[11340]'>"
        L"<rwcoord picketing1_val='489~800' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200918T055850Z' name='Wrong_way' Bdg='N1P_KAC[11332:11340]' waynum='1'>"
        L"<rwcoord picketing1_val='487~800' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='489~800' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200918T055850Z' name='Span_move' Bdg='N1P_KAC[11332:11340]' waynum='1'>"
        L"<rwcoord picketing1_val='487~800' picketing1_comm='Россия' />"
        L"<rwcoord picketing1_val='489~800' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    happen_layer.createPath( d.pretty_wstr() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_path_size( 7 ) );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Transition Arrival Death Form Station_exit Wrong_way Span_move" ) );

    SpotEventPtr event = make_shared<SpotEvent>( d.document_element().first_child() );

    Hem::aeRectify rectify( happen_layer.GetWriteAccess(), event );
    rectify.Action();
    happen_layer.TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT( happen_layer.path_count() == 1 );
    CPPUNIT_ASSERT( happen_layer.exist_path_size( 7 ) );
    CPPUNIT_ASSERT( happen_layer.exist_series( "name", "Transition Arrival Departure Arrival Departure Wrong_way Span_move" ) );
}
