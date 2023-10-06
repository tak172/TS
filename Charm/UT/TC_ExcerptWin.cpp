#include "stdafx.h"

#include "TC_ExcerptWin.h"
#include "../Fund/ExcerptWindow.h"
#include "../Fund/RackWindow.h"
#include "../helpful/StrToTime.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ExcerptWin );

bool check(const WindowExcerpt& w, time_t mom, 
                 WINDOW_PHASE wph_before, 
                 WINDOW_PHASE wph_after 
                 )
{
    return (w.getState(mom-1) == wph_before)
        && (w.getState(mom+0) == wph_after)
        && (w.getState(mom+1) == wph_after);
}
bool checkEv_nsvfc(const WindowExcerpt& w, time_t n, time_t s, time_t v, time_t f, time_t c)
{
    vector<time_t> etalon;
    etalon.push_back(n);
    etalon.push_back(s);
    etalon.push_back(v);
    etalon.push_back(f);
    etalon.push_back(c);
    etalon.push_back(c+WindowExcerpt::COMPLETE_DELAY); // спец событие для удаления
    sort(etalon.begin(),etalon.end());

    vector<time_t> accepted=w.getEvents();
    sort(accepted.begin(),accepted.end());

    for( ; !etalon.empty(); etalon.pop_back() )
    {
        if ( !std::binary_search( accepted.begin(),accepted.end(),etalon.back() ) )
            return false;
    }
    return true;
}

WindowExcerpt TC_ExcerptWin::makeWin( time_t mom_mentioned,
                                      int Id,
                                      const char * plan_start,
                                      const char * plan_finish,
                                      const char * fact_start,
                                      const char * fact_finish
                                    )
{
        std::string s =
            "<Info>"
            "   <Wind>"
            "      <Comment> вывоз людей и инструмента; рук. ПДБ Калинин </Comment>"
            "      <Object Place='289529 289406' Put='1' KmPk_Beg='1.1' KmPk_End='25.6'  />"
            "      <Sl_Name>Служба ПЧ-36   Работа СДПМ</Sl_Name>"
            "   </Wind>"
            "</Info>";

        attic::a_document x_doc;
        x_doc.load_wide(From1251(s));
        attic::a_node winNode = x_doc.document_element().child("Wind");
        CPPUNIT_ASSERT( winNode );
        winNode.brief_attribute( "Id", Id );
        winNode.brief_attribute( "Tim_Beg",  plan_start);
        winNode.brief_attribute( "Tim_End",  plan_finish);
        winNode.brief_attribute( "Fact_Beg", fact_start);
        winNode.brief_attribute( "Fact_End", fact_finish);

        WindowExcerpt w = WindowExcerpt::getNord( winNode, mom_mentioned );
        return w;
}

void TC_ExcerptWin::planning()
{
    const char * PLAN__START     = "2011-06-23 08:00";
    const char * PLAN_FINISH     = "2011-06-23 09:00";
    time_t m = StrDataTime_To_UTC("2011-06-23 11:00");

    time_t s = StrDataTime_To_UTC(PLAN__START);
    time_t f = StrDataTime_To_UTC(PLAN_FINISH);
    time_t c = f+WindowExcerpt::COMPLETE_DELAY;

    WindowExcerpt w = makeWin(m, 11, PLAN__START, PLAN_FINISH );

    CPPUNIT_ASSERT( !w.empty() );

    CPPUNIT_ASSERT( check(w,s,WPH_FUTURE,WPH_FUTURE) );
    CPPUNIT_ASSERT( check(w,f,WPH_FUTURE,WPH_FUTURE) );
    CPPUNIT_ASSERT( check(w,c,WPH_FUTURE,WPH_FUTURE) );

    CPPUNIT_ASSERT( check(w,m,WPH_FUTURE, WPH_NOTHING ) );
}

void TC_ExcerptWin::factBE()
{
    const char * PLAN__START =     "2011-06-23 08:00";
    const char * PLAN_FINISH =     "2011-06-23 09:00";
    const char * FACT__START =     "2011-06-23 20:00";
    time_t v = StrDataTime_To_UTC("2011-06-23 21:00");
    const char * FACT_FINISH =     "2011-06-23 22:00";
    time_t m = StrDataTime_To_UTC("2011-06-23 22:30");

    time_t n = StrDataTime_To_UTC(FACT__START)-WindowExcerpt::NEAR_FUTURE;
    time_t s  = StrDataTime_To_UTC(FACT__START);
    //     v
    time_t f  = StrDataTime_To_UTC(FACT_FINISH);
    time_t c  = f+WindowExcerpt::COMPLETE_DELAY;

    WindowExcerpt w = makeWin(m, 22, PLAN__START, PLAN_FINISH, FACT__START, FACT_FINISH );
    CPPUNIT_ASSERT( !w.empty() );
    CPPUNIT_ASSERT( checkEv_nsvfc(w, n,s,v,f,c) );
    CPPUNIT_ASSERT( check( w, n, WPH_NOTHING,  WPH_FUTURE ) );
    CPPUNIT_ASSERT( check( w, s, WPH_FUTURE,   WPH_NOW ) );
    CPPUNIT_ASSERT( check( w, v, WPH_NOW,      WPH_OVERRUN ) );
    CPPUNIT_ASSERT( check( w, f, WPH_OVERRUN,  WPH_COMPLETE ) );
    CPPUNIT_ASSERT( check( w, c, WPH_COMPLETE, WPH_NOTHING ) );
}

void TC_ExcerptWin::factBm()
{
    const char * PLAN__START =     "2011-06-23 08:00";
    const char * FACT__START =     "2011-06-23 08:15";
    const char * PLAN_FINISH =     "2011-06-23 09:00";
    time_t v = StrDataTime_To_UTC("2011-06-23 09:15");
    time_t m = StrDataTime_To_UTC("2011-06-23 09:30");

    time_t n = StrDataTime_To_UTC(FACT__START)-WindowExcerpt::NEAR_FUTURE;
    time_t s = StrDataTime_To_UTC(FACT__START);
    //     v
    time_t f  = m;
    time_t c  = f+WindowExcerpt::COMPLETE_DELAY;

    WindowExcerpt w = makeWin(m, 33, PLAN__START, PLAN_FINISH, FACT__START, NULL );
    CPPUNIT_ASSERT( !w.empty() );
    CPPUNIT_ASSERT( checkEv_nsvfc(w, n,s,v,f,c) );
    CPPUNIT_ASSERT( check( w, n, WPH_NOTHING,  WPH_FUTURE ) );
    CPPUNIT_ASSERT( check( w, s, WPH_FUTURE,   WPH_NOW ) );
    CPPUNIT_ASSERT( check( w, v, WPH_NOW,      WPH_OVERRUN ) );
    CPPUNIT_ASSERT( check( w, f, WPH_OVERRUN,  WPH_COMPLETE ) );
    CPPUNIT_ASSERT( check( w, c, WPH_COMPLETE, WPH_NOTHING ) );
}

void TC_ExcerptWin::fact_E()
{
    const char * PLAN__START =     "2011-06-23 08:00";
    const char * PLAN_FINISH =     "2011-06-23 09:00";
    const char * FACT_FINISH =     "2011-06-23 09:15";
    time_t v = StrDataTime_To_UTC("2011-06-23 09:15");
    time_t m = StrDataTime_To_UTC("2011-06-23 09:25");

    time_t n  = StrDataTime_To_UTC(PLAN__START)-WindowExcerpt::NEAR_FUTURE;
    time_t s  = StrDataTime_To_UTC(PLAN__START);
    //     v
    time_t f  = StrDataTime_To_UTC(FACT_FINISH);
    //     m
    time_t c  = f+WindowExcerpt::COMPLETE_DELAY;

    WindowExcerpt w = makeWin(m, 44, PLAN__START, PLAN_FINISH, NULL, FACT_FINISH );
    CPPUNIT_ASSERT( !w.empty() );
    CPPUNIT_ASSERT( checkEv_nsvfc(w, n,s,v,f,c) );
    CPPUNIT_ASSERT( check( w, n, WPH_NOTHING,  WPH_FUTURE ) );
    CPPUNIT_ASSERT( check( w, s, WPH_FUTURE,   WPH_NOW ) );
    CPPUNIT_ASSERT( check( w, f, WPH_NOW,      WPH_COMPLETE ) );
    CPPUNIT_ASSERT( check(w,m,WPH_COMPLETE, WPH_NOTHING ) );
    CPPUNIT_ASSERT( check( w, c, WPH_COMPLETE, WPH_NOTHING ) );
}

void TC_ExcerptWin::BadData()
{
    attic::a_document x_doc;
    attic::a_parse_result res = x_doc.load_utf8( "..." );
    attic::a_node info = x_doc.child("Info");

    std::vector<WindowExcerpt> v = WindowExcerpt::readTags( info, time_t(0), "Wind" );
    CPPUNIT_ASSERT( v.empty()==true );
}

void TC_ExcerptWin::Wind()
{
    const char* s1251= 
        "<Info>"
        " <Wind Id=\"548301\" Tim_Beg=\"2011-06-23 08:00\" Tim_End=\"2011-06-23 16:00\" >"
        "  <Comment>23.06.2011 г. ТЭ+СДПМ  3407 отправление с Мульды в 8час. 30мин., пробег Мульда- Хановей- вывоз людей и инструмента; рук. ПДБ Калинин, инструктаж и.о.ПЧ Ерошевич.   Мульда-Хановей  </Comment>"
        "  <Object Place=\"289529 289406\" Put=\"1\" KmPk_Beg=\"1.1\" KmPk_End=\"25.6\"  />"
        "  <Sl_Name>Служба ПЧ-36   Работа СДПМ</Sl_Name>"
        " </Wind>"
        "</Info>";
    attic::a_document x_doc;
    x_doc.load_wide(From1251(s1251));
    WindowExcerpt w = WindowExcerpt::getNord( x_doc.child("Info").child("Wind"), time_t(12345) );
    CPPUNIT_ASSERT( !w.empty() );
}

void TC_ExcerptWin::RackWindow_work()
{
    RackWindow rackWin;
    const char * p08     = "2011-06-23 08:00";
    const char * p09     = "2011-06-23 09:00";
    const char * men     = "2011-06-23 09:33";
    const char * p10     = "2011-06-23 10:00";
    const char * p11     = "2011-06-23 11:00";
    const char * p12     = "2011-06-23 12:00";
    const char * late    = "2011-06-23 12:20";

    rackWin.add( makeWin( StrDataTime_To_UTC(men), 11, p08, p09, p08, p09 ) );
    rackWin.add( makeWin( StrDataTime_To_UTC(men), 22, p09, p10 ) );
    rackWin.add( makeWin( StrDataTime_To_UTC(men), 33, p10, p11, p10, p11 ) );
    CPPUNIT_ASSERT( rackWin.size() == 3 );
    // есть необработанные окна на момент men
    time_t m = StrDataTime_To_UTC(men);
    CPPUNIT_ASSERT( rackWin.existBefore( m ) );
    // обработка окон (неопределено: 11 или 22)
    while( true )
    {
        const WindowExcerpt& win = rackWin.get_next( m );
        if ( win.getId().empty() )
            break;;
        // сообщить о завершении обработки
        rackWin.processed( win, m );
    }
    // теперь обрабатывать нечего (ничего нового не случилось) и окно 11 должно исчезнуть
    CPPUNIT_ASSERT( rackWin.get_next( m ).getId().empty() );
    CPPUNIT_ASSERT( rackWin.size() == 2 );

    {
        // планируемое окно помнят, но так и не выполняют - лишь дополнительное упоминание до 12:00
        rackWin.add( makeWin( StrDataTime_To_UTC(p12), 22, p09, p10 ) );
        {
            // к моменту (late-1) - дообработаны все, остается только окно 22 (его упоминание ещё не истекло)
            time_t la1 = StrDataTime_To_UTC(late)-1;
            CPPUNIT_ASSERT( rackWin.existBefore( la1 ) );
            // обработка окон
            while( !rackWin.get_next( la1 ).getId().empty() )
                rackWin.processed( rackWin.get_next( la1 ), la1 ); // сообщить о завершении обработки
            // теперь обрабатывать нечего и все окна должны исчезнуть
            CPPUNIT_ASSERT( rackWin.get_next( la1 ).getId().empty() );
            CPPUNIT_ASSERT( rackWin.size() == 1 );
        }

        {
            // к моменту late - упоминание уже истекло и более ничего нет
            time_t la0 = StrDataTime_To_UTC(late);
            CPPUNIT_ASSERT( rackWin.existBefore( la0 ) );
            // обработка окон
            while( !rackWin.get_next( la0 ).getId().empty() )
                rackWin.processed( rackWin.get_next( la0 ), la0 ); // сообщить о завершении обработки
            // теперь обрабатывать нечего и все окна должны исчезнуть
            CPPUNIT_ASSERT( rackWin.get_next( la0 ).getId().empty() );
            CPPUNIT_ASSERT( rackWin.size() == 0 );
        }
    }
}
