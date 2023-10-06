#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "TC_NiiasXml.h"
#include "../Fund/UralParser/StripShot.h"
#include "../Fund/ExcerptWarning.h"
#include "../Fund/ExcerptWindow.h"
#include "../Fund/Foreign_Utils.h"
#include "../ut/Parody.h"
#include "../helpful/Log.h"
#include "../helpful/EsrGuide.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_NiiasXml );

using namespace Parody;

void TC_NiiasXml::getStrip()
{
    EsrGuide eGuide;
    Foreign::GlobalData ngd(giduraler,eGuide);
    Foreign::AsyncSamara niAs( ngd );
    Foreign::SyncSamara niSy( niAs, time_t(0) );
    niSy.setLexicon( Etui::instance()->lex() );

    niAs.process62Train( doc1 );
    CPPUNIT_ASSERT( 90==niSy.process62Train().size() );

    niAs.process62Train( doc2 );
    CPPUNIT_ASSERT( 91==niSy.process62Train().size() );
}

void TC_NiiasXml::test12Ktsm()
{
    /*
    <Record
    TypeMessage='12'
    UchKod='80480'
    ObjectId='пер6'
    ObjectState='1'
    />
    */

    const std::wstring example =
        L"<Message>"
        L"<Record  TypeMessage='12' UchKod='5' ObjectId='xx' Text3='1' />"
        L"<Record  TypeMessage='12' UchKod='5' ObjectId='yy' Text3='2' />"
        L"<Record  TypeMessage='12' UchKod='5' ObjectId='zz' Text3='3' />"
        L"<Record  TypeMessage='12' UchKod='5' ObjectId='ww' Text3='777' />"
        L"</Message>";

    attic::a_document doc;
    doc.load_wide( example );
    EsrKit e(345,678);
    Elements temp( e, FIGURE, "  %KTSM_X   %KTSM_Y   %KTSM_Z   %KTSM_W  ", PAINTED );
    Etui::instance()->complete();

    giduraler.insert( L"5 xx", BadgeE(L"%KTSM_X",e), 1 );
    giduraler.insert( L"5 yy", BadgeE(L"%KTSM_Y",e), 1 );
    giduraler.insert( L"5 zz", BadgeE(L"%KTSM_Z",e), 1 );
    giduraler.insert( L"5 ww", BadgeE(L"%KTSM_W",e), 1 );
  
    EsrGuide eGuide;
    Foreign::GlobalData ngd(giduraler, eGuide);
    Foreign::AsyncSamara niAs( ngd );
    Foreign::SyncSamara niSy( niAs, time_t(0) );
    niSy.setLexicon( Etui::instance()->lex() );

    niAs.process12Ktsm(doc);
    niSy.complete_Once( moment );

    CPPUNIT_ASSERT( checkObjState(L"%KTSM_X", BACKGROUND) ); // тревога 1
    CPPUNIT_ASSERT( checkObjState(L"%KTSM_Y", LIGHT1) );     // тревога 2 
    CPPUNIT_ASSERT( checkObjState(L"%KTSM_Z", LIGHT2) );     // тревога 0
    CPPUNIT_ASSERT( checkObjState(L"%KTSM_W", NONE) );
}

void TC_NiiasXml::test13warning()
{
    /*
    "UchKod"
    "ObjectId"
    "EventId"
    "Text3"
    "Text2"
    act_beg = UTC_from_DMYhms(x_warn.attribute( "BegDateTime" ).value(), 0 );
    act_end = UTC_from_DMYhms(x_warn.attribute( "EndDateTime" ).value(), time_t(0xFF00FFFFFF00) );
    */
    const std::wstring example =
        L"<Message>"
        L"<Record  TypeMessage='13' UchKod='1' ObjectId='xx' EventId='777' Text3='60' Text2='вот' />"
        L"<Record  TypeMessage='13' UchKod='1' ObjectId='yy' EventId='777' Text3='60' Text2='вот' />"
        L"<Record  TypeMessage='13' UchKod='1' ObjectId='zz' EventId='777' Text3='60' Text2='вот' />"
        L"<Record  TypeMessage='13' UchKod='1' ObjectId='xx' EventId='222' Text3='88' Text2='ах ' />"
        L"<Record  TypeMessage='13' UchKod='1' ObjectId='yy' EventId='333' Text3='10' Text2='как' />"
        L"<Record  TypeMessage='13' UchKod='1' ObjectId='zz' EventId='444' Text3='99' Text2='же ' />"
        L"</Message>";

    attic::a_document doc;
    doc.load_wide( example );
    EsrKit e(345,678);
    Elements creation_strips( e, STRIP, " theX theY theZ   theZero " );
    Etui::instance()->complete();

    giduraler.insert( L"1 xx", BadgeE(L"theX",e), 1 );
    giduraler.insert( L"1 yy", BadgeE(L"theY",e), 1 );
    giduraler.insert( L"1 zz", BadgeE(L"theZ",e), 1 );

    Lexicon& lexi = Etui::instance()->lex();
    EsrGuide eGuide;
    Foreign::GlobalData ngd( giduraler, eGuide );
    Foreign::AsyncSamara niAs( ngd );
    Foreign::SyncSamara niSy( niAs, time_t(0) );
    niSy.setLexicon( lexi );
    niAs.process_once(doc);
    niSy.complete_Once( moment );

    int temp;
    CPPUNIT_ASSERT( lexi.getBoth(BadgeE(L"theX",e)).second->getMaxSpeed(&temp) );
    CPPUNIT_ASSERT_EQUAL( 60, temp );
    CPPUNIT_ASSERT( lexi.getBoth(BadgeE(L"theY",e)).second->getMaxSpeed(&temp) );
    CPPUNIT_ASSERT_EQUAL( 10, temp );
    CPPUNIT_ASSERT( lexi.getBoth(BadgeE(L"theZ",e)).second->getMaxSpeed(&temp) );
    CPPUNIT_ASSERT_EQUAL( 60, temp );
    CPPUNIT_ASSERT( !lexi.getBoth(BadgeE(L"theZero", e)).second->getMaxSpeed(&temp) );
    CPPUNIT_ASSERT_EQUAL(  0, temp );
}

void TC_NiiasXml::test08Window()
{
    const std::wstring full_wide =
        L"<Message>"
        L"<Record TypeMessage='8' UchKod='qq' ObjectId='ru' ObjectState='1' BegDateTime='29.06.2012 12:00:00' EventId='111' />"
        L"<Record TypeMessage='8' UchKod='qq' ObjectId='pl' ObjectState='2' BegDateTime='29.06.2012 12:00:00' EventId='222' />"
        L"<Record TypeMessage='8' UchKod='qq' ObjectId='ov' ObjectState='3' BegDateTime='29.06.2012 12:00:00' EventId='333' />"
        L"</Message>";
    const std::wstring emptywide =
        L"<Message>"
        L"</Message>";

    attic::a_document doc__full;
    attic::a_document doc_empty;
    doc__full.load_wide( full_wide );
    doc_empty.load_wide( emptywide );

    EsrKit e(345,678);
    Elements temp( e, FIGURE, "  %WND_plan   %WND__run   %WND_over ", WIN_TEXT );
    Etui::instance()->complete();

    giduraler.insert( L"qq pl", BadgeE(L"%WND_plan",e), 1 );
    giduraler.insert( L"qq ru", BadgeE(L"%WND__run",e), 1 );
    giduraler.insert( L"qq ov", BadgeE(L"%WND_over",e), 1 );

    EsrGuide eGuide;
    Foreign::GlobalData ngd( giduraler, eGuide );
    Foreign::AsyncSamara niAs( ngd );
    Foreign::SyncSamara niSy( niAs, time_t(0) );
    niSy.setLexicon( Etui::instance()->lex() );

    using namespace boost::posix_time;
    using namespace boost::gregorian;

    time_duration loc_delta;
    {
        ptime val = second_clock::universal_time();
        loc_delta = boost::date_time::c_local_adjustor<ptime>::utc_to_local(val) - val;
    }
    date  mom_date(from_simple_string("2012/06/29"));
    ptime start_in_utc( from_simple_string("2012/06/29"), duration_from_string("12:00:00")-loc_delta );
    TRACE( "mom__start=%s\n", to_iso_extended_string(start_in_utc).c_str() );
    ptime check_moment;

    niAs.process08Window( doc__full );
    niSy.complete_Once( start_in_utc-hours(14) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_plan", WPH_NOTHING ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND__run", WPH_NOW ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_over", WPH_OVERRUN ) );

    niAs.process08Window(doc__full);
    niSy.complete_Once( start_in_utc-hours(8) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_plan", WPH_FUTURE ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND__run", WPH_NOW ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_over", WPH_OVERRUN ) );

    niAs.process08Window(doc__full);
    niSy.complete_Once( start_in_utc );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_plan", WPH_FUTURE ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND__run", WPH_NOW ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_over", WPH_OVERRUN ) );

    niAs.process08Window(doc_empty);
    niSy.complete_Once( start_in_utc+hours(1) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_plan", WPH_NOTHING ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND__run", WPH_NORM_PAST ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_over", WPH_OVER_PAST ) );

    niAs.process08Window(doc_empty);
    niSy.complete_Once( start_in_utc+hours(1)+hours(2)+seconds(1) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_plan", WPH_NOTHING ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND__run", WPH_NOTHING ) );
    CPPUNIT_ASSERT( checkWinPhase(L"%WND_over", WPH_NOTHING ) );
}

void TC_NiiasXml::getHtml()
{
}

void TC_NiiasXml::setUp()
{
    const wchar_t * example =
        L"<?xml version='1.0' encoding='utf-8' ?>"
        L"<Message Id='10309' CaclDate='30.06.2012 18:07:59' TypeMessage='6305'>"
        L"<Record  TypeMessage='32' UchKod='80024' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2770' Text3='58001238000' />"
        L"<Record  TypeMessage='37' UchKod='80480' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2920' Text3='65009348000' />"
        L"<Record  TypeMessage='37' UchKod='80480' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2920' Text3='65009348000' />"
        L"<Record  TypeMessage='35' UchKod='80510' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2462' Text3='63000348300' />"
        L"<Record  TypeMessage='32' UchKod='80510' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2462' Text3='63000348300' />"
        L"<Record  TypeMessage='35' UchKod='80510' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2462' Text3='63000348300' />"
        L"<Record  TypeMessage='32' UchKod='80510' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2462' Text3='63000348300' />"
        L"<Record  TypeMessage='32' UchKod='80506' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2944' Text3='80663288302' />"
        L"<Record  TypeMessage='32' UchKod='80506' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2944' Text3='80663288302' />"
        L"<Record  TypeMessage='34' UchKod='80506' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2389' Text3='86240045878' />"
        L"<Record  TypeMessage='34' UchKod='80506' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2389' Text3='86240045878' />"
        L"<Record  TypeMessage='35' UchKod='80480' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2385' Text3='87015332119' />"
        L"<Record  TypeMessage='35' UchKod='80510' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2961' Text3='84147665922' />"
        L"<Record  TypeMessage='32' UchKod='80510' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2961' Text3='84147665922' />"
        L"<Record  TypeMessage='35' UchKod='80480' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2385' Text3='87015332119' />"
        L"<Record  TypeMessage='33' UchKod='80610' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2328' Text3='63002778000' />"
        L"<Record  TypeMessage='33' UchKod='80610' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2328' Text3='63002778000' />"
        L"<Record  TypeMessage='37' UchKod='80620' ObjectId='' ObjectState='1' Text1='' Text2='' BegDateTime='' EndDateTime='' EventId='2920' Text3='65009348000' />"
        L"<Record  TypeMessage='62' UchKod='65940' ObjectId='Ч2П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='3454' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65950' ObjectId='16сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='1963' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65950' ObjectId='12сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='1963' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65950' ObjectId='2сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='1963' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65950' ObjectId='7/11сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='1963' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65960' ObjectId='Ч1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='5002' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65960' ObjectId='2/30п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='5002' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65930' ObjectId='Ч1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2260' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65930' ObjectId='40/72п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2112' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65930' ObjectId='6п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2304' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65930' ObjectId='9п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2184' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65930' ObjectId='Чп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2260' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65910' ObjectId='Ч1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='7106' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65910' ObjectId='2Ап' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='7106' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65910' ObjectId='6п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2340' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65910' ObjectId='Н1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2391' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65910' ObjectId='Н2П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2391' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65924' ObjectId='Н1У' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='3681' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65924' ObjectId='ЧДП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='3681' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65924' ObjectId='1п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='3681' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65924' ObjectId='Чп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='6520' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65924' ObjectId='Ч1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='6520' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65840' ObjectId='5/19п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2389' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65840' ObjectId='НП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2389' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65840' ObjectId='21/35п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2389' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65840' ObjectId='4п' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='4404' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65840' ObjectId='3/5сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2389' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65840' ObjectId='35сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2389' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65840' ObjectId='Н1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2389' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80024' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='4582' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80024' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='4582' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80024' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2387' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80024' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2770' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80480' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2920' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80480' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2920' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80530' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2367' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80530' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='6033' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80530' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='6033' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80521' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='6033' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80510' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2462' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80510' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2462' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80506' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2944' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80506' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2944' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80506' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='3003' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80521' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2730' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80506' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2389' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80506' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2389' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80480' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2385' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80520' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='6033' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80520' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2730' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80510' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2730' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80510' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2961' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80480' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2385' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80610' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2357' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80610' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2328' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80610' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2328' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80590' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2940' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80590' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2940' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80610' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2357' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80530' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='4316' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80530' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='4316' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80530' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='7023' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80620' ObjectId='' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2920' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65960' ObjectId='Чп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='5002' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65960' ObjectId='2сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='5002' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65975' ObjectId='Ч1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2308' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65990' ObjectId='16сп_' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2108' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65993' ObjectId='Ч2У' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2108' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65676' ObjectId='1АП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2147' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65675' ObjectId='4П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2410' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65675' ObjectId='1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2147' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65700' ObjectId='6ЧП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2232' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65700' ObjectId='2НП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2393' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65700' ObjectId='1СП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2232' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65710' ObjectId='4П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2232' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65725' ObjectId='3П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='4301' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65150' ObjectId='2АП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='4406' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65150' ObjectId='8АП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2232' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65150' ObjectId='38П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2232' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65164' ObjectId='25СП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2758' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65600' ObjectId='2АП' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2716' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='' ObjectId='ЧП1' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2782' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='' ObjectId='1П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2313' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='65840' ObjectId='Ч2П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='2172' Text3='' />"
        L"<Record  TypeMessage='62' UchKod='80620' ObjectId='' ObjectState='6' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='480' Text3='0' />"
        L"<Record  TypeMessage='62' UchKod='80506' ObjectId='' ObjectState='6' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='391' Text3='0' />"
        L"<Record  TypeMessage='62' UchKod='80020' ObjectId='' ObjectState='6' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='134' Text3='0' />"
        L"<Record  TypeMessage='62' UchKod='80024' ObjectId='' ObjectState='6' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='134' Text3='0' />"
        L"<Record  TypeMessage='62' UchKod='65900' ObjectId='10/16сп' ObjectState='6' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='72' Text3='0' />"
        L"<Record  TypeMessage='62' UchKod='65900' ObjectId='Чп' ObjectState='6' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='72' Text3='0' />"
        L"<Record  TypeMessage='62' UchKod='65676' ObjectId='2АП' ObjectState='6' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='258' Text3='0' />"
        L"<Record  TypeMessage='62' UchKod='65586' ObjectId='НП' ObjectState='6' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='715' Text3='0' />"
        L"</Message>";

    doc1.load_wide( example );
    fill_giduraler(doc1);


    using namespace std;
    using namespace boost::algorithm;

    wstring s2=example;
    const wchar_t * del_1pe = L"<Record  TypeMessage='62' UchKod='65940' ObjectId='Ч2П' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='3454' Text3='' />";
    const wchar_t * del_2pe = L"<Record  TypeMessage='62' UchKod='65950' ObjectId='16сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='1963' Text3='' />";
    const wchar_t * chg_bef = L"<Record  TypeMessage='62' UchKod='65950' ObjectId='12сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='1963' Text3='' />";
    const wchar_t * chg_aft = L"<Record  TypeMessage='62' UchKod='65950' ObjectId='12сп' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='7777' Text3='' />";
    const wchar_t * add__pe = L"<Record  TypeMessage='62' UchKod='65950' ObjectId='2сп+' ObjectState='7' Text1='' Text2='0' BegDateTime='' EndDateTime='' EventId='1963' Text3='' />";
    CPPUNIT_ASSERT( find_first(s2,del_1pe) );
    erase_first(s2, del_1pe );
    CPPUNIT_ASSERT( !find_first(s2,del_1pe) );
    CPPUNIT_ASSERT( find_first(s2,del_2pe) );
    erase_first(s2, del_2pe );
    CPPUNIT_ASSERT( !find_first(s2,del_2pe) );
    replace_first(s2,chg_bef,chg_aft);
    if ( s2.find(chg_aft)==wstring::npos )
        CPPUNIT_FAIL("error on subst");
    else
        s2.insert( s2.find(chg_aft), add__pe );
    doc2.load_wide( s2 );
    fill_giduraler(doc2);

    Etui::instance();
    // место для результатов
    out_SuppInfo = SupplyTrainInfo();
}

void TC_NiiasXml::tearDown()
{
    Etui::Shutdowner();
    doc1.reset();
    doc2.reset();
    giduraler.clear();
}

void TC_NiiasXml::fill_giduraler(const attic::a_document& xdoc)
{
    typedef attic::a_xpath_node_set XNS;
    XNS query = xdoc.select_nodes("/Message/Record");
    for( XNS::const_iterator it=query.begin(); it!=query.end(); ++it )
    {
        std::wstring ws=Foreign::getGuId(it->node());
        if ( !ws.empty() )
            giduraler.insert( ws, BadgeE(L"заглушка",EsrKit(345,678)),1 );
    }
}

bool TC_NiiasXml::checkObjState( const wchar_t * obj_name, int checked_state )
{
    const LOS_base* los = Etui::instance()->lex().getLos( BadgeE(obj_name,Elements::commonEsr()) );
    switch( checked_state )
    {
    case NONE:
        return !los->isKnown() && !los->isEnabled() && !los->isEnabled2();
    case BACKGROUND:
        return los->isKnown() && !los->isEnabled() && !los->isEnabled2();
    case LIGHT1:
        return los->isKnown() && los->isEnabled() && !los->isEnabled2();
    case LIGHT2:
        return los->isKnown() && !los->isEnabled() && los->isEnabled2();
    default:
        CPPUNIT_FAIL( "неизвестная проверка" );
        return false;
    }
}

bool TC_NiiasXml::checkWinPhase( const wchar_t * obj_name, WINDOW_PHASE checked_phase )
{
    const LOS_base* los = Etui::instance()->lex().getLos( BadgeE(obj_name,Elements::commonEsr()) );
    return (los->getPhase() == checked_phase);
}
