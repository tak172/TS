#include "stdafx.h"

#include "TC_Hem_aeCut.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/Appliers/aeCut.h"
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Hem/Appliers/aeRectify.h"
#include "../Hem/Bill.h"
#include "../helpful/LocoCharacteristics.h"
#include "../Hem/UserEditInventory.h"
#include "../Hem/Appliers/Context.h"
#include "../Hem/GuessTransciever.h"
#include "../Hem/UncontrolledStations.h"
#include "UtHemHelper.h"
#include "../helpful/Topology.h"
#include "../helpful/NsiBasis.h"

using namespace std;
using namespace HemHelpful;
using namespace Hem;
using namespace Hem::ApplierCarryOut;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeCut );

void TC_Hem_aeCut::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_Hem_aeCut::Middle()
{
    SpotEvent pat[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;

    hl.createPath( pat );
    CPPUNIT_ASSERT( hl.path_count()==1 );

    SpotEvent a( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 );
    SpotEvent b( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 );
    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, a, b );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
}

void TC_Hem_aeCut::WithStart()
{
    SpotEvent pat[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( pat );
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(6) );

    SpotEvent s( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 );
    SpotEvent b( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 );
    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, s, b );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
}

void TC_Hem_aeCut::WithEnd()
{
    SpotEvent pat[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( pat );
    CPPUNIT_ASSERT( hl.path_count()==1 );

    SpotEvent a( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 );
    SpotEvent z( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 );
    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, a, z );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    CPPUNIT_ASSERT( hl.exist_series("name","Form      Departure Transition Transition Span_move       Death" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg", "1p[00100] 1p[00100] 2p[00200]  3p[00300]  edit[00300:00400] edit[00300:00400]" ) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
}

void TC_Hem_aeCut::Whole()
{
    SpotEvent pat[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( pat );
    CPPUNIT_ASSERT( hl.path_count()==1 );

    SpotEvent s( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 );
    SpotEvent b( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 );
    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, s, b );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    CPPUNIT_ASSERT( hl.exist_series("name","Form      Death" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg", "1p[00100] 1p[00100]" ) );
    CPPUNIT_ASSERT( hl.exist_series("name","Span_move Transition" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg", "edit[00400:00500] 5p[00500]" ) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
}

void TC_Hem_aeCut::TooShort()
{
    SpotEvent pat[] = {
        SpotEvent( HCode::FORM,   BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEATH,  BadgeE(L"1p",EsrKit(100)), T+15 ),
    };
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( pat );
    CPPUNIT_ASSERT( hl.path_count()==1 );

    SpotEvent s( HCode::FORM,  BadgeE(L"1p",EsrKit(100)), T+10 );
    SpotEvent b( HCode::DEATH, BadgeE(L"1p",EsrKit(100)), T+15 );
    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, s, b );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
}

void TC_Hem_aeCut::SaveNumber()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Span_move' Bdg='NB2C[11062:11063]' create_time='20151104T011908Z' waynum='1'>"
        L"    <rwcoord picketing1_val='402~200' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='403~' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Transition' Bdg='1C[11062]' create_time='20151104T013153Z' waynum='1' parknum='1' index='' num='D2814' fretran='Y' />"
        L"  <SpotEvent name='Transition' Bdg='1AC[11041]' create_time='20151104T022456Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Arrival' Bdg='5C[11042]' create_time='20151104T023718Z' waynum='5' parknum='1' />"
        L"  <SpotEvent name='Departure' Bdg='5C[11042]' create_time='20151104T030250Z' waynum='5' parknum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='2N2IP[11042:16170]' create_time='20151104T030707Z' waynum='2'>"
        L"    <rwcoord picketing1_val='461~800' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent name='Death' Bdg='2N2IP[11042:16170]' create_time='20151104T031143Z' waynum='2'>"
        L"    <rwcoord picketing1_val='461~800' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move Transition Transition Arrival Departure Span_move Death" ) );
    CPPUNIT_ASSERT( hl.exist_event( "@name='Transition' and @num='D2814'" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Arrival' Bdg='5C[11042]' create_time='20151104T023718Z' index='' num='D2814' fretran='Y' waynum='5' parknum='1' />"
        L"  <Action code='Cut' />"
        L"  <PostPoint name='Departure' Bdg='5C[11042]' create_time='20151104T030250Z' index='' num='D2814' fretran='Y' waynum='5' parknum='1' />"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows());
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move Transition Transition Arrival Death" ) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "                                Form    Departure Span_move Death" ) );
    CPPUNIT_ASSERT( hl.exist_event( "@name='Transition' and @num='D2814'" ) );
    CPPUNIT_ASSERT( hl.exist_event( "@name='Form'       and @num='D2814'" ) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
}

void TC_Hem_aeCut::Case3674()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent name='Form' Bdg='spmv[10000:11111]' create_time='20160402T231809Z' waynum='1' index='' num='J2201' fretran='Y' />"
        L"  <SpotEvent name='Transition' Bdg='1w[11111]' create_time='20160402T232434Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='ww[22222]' create_time='20160402T234009Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='w3[33333]' create_time='20160403T000023Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Transition' Bdg='1C[44444]' create_time='20160403T000709Z' waynum='1' parknum='1' />"
        L"  <SpotEvent name='Span_move' Bdg='move[44444:55555]' create_time='20160403T001431Z' waynum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form              Transition Transition Transition Transition Span_move" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "spmv[10000:11111] 1w[11111]  ww[22222]  w3[33333]  1C[44444]  move[44444:55555]" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Transition' Bdg='ww[22222]' create_time='20160402T234009Z' index='' num='J2201' fretran='Y' waynum='1' parknum='1' />"
        L"  <Action code='Cut' SaveLog='Y' />"
        L"  <PostPoint name='Transition' Bdg='w3[33333]' create_time='20160403T000023Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows() );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "spmv[10000:11111] 1w[11111]  ww[22222]  edit[22222:33333] edit[22222:33333] " ) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form              Transition Transition Span_move         Death" ) );

    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move          Transition Transition Span_move" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "edit[22222:33333]  w3[33333]  1C[44444]  move[44444:55555]" ) );

    // дополнение живой нити некими событиями
    {
        TrioSpot nextTrio;
        nextTrio.pre( createEventPtr<SpotEvent>(
            L"<SpotEvent name='Span_move' Bdg='move[44444:55555]' create_time='20160403T001431Z' waynum='1' />"
            ) );
        nextTrio.body( createEventPtr<SpotEvent>(
            L"<SpotEvent name='Span_move' Bdg='live[44444:55555]' create_time='20160403T001555Z' waynum='1' />"
            ) );
        UtIdentifyCategoryList identifyCategory;
        Hem::aePusherUserHappen pusher( hl.GetWriteAccess(), nextTrio, identifyCategory );
        pusher.Action();
        Hem::aeRectify rectify( hl.GetWriteAccess(), nextTrio.pre() );
        rectify.Action();
    }
    // откат и его проверка (живое событие не мешает и сохраняется)
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form              Transition Transition Transition Transition Span_move" ) );
    CPPUNIT_ASSERT( hl.exist_series( "Bdg",  "spmv[10000:11111] 1w[11111]  ww[22222]  w3[33333]  1C[44444]  live[44444:55555]" ) );
}

void TC_Hem_aeCut::StoppingBegin_Death_3889()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20160831T132955Z' name='Form' Bdg='6C[11310]' waynum='6' parknum='1' index='1945-154-0983' num='V2211' length='51' weight='4872' fretran='Y'>"
        L"    <feat_texts typeinfo='Т' />"
        L"    <Locomotive Series='2М62' NumLoc='722' Dor='0' Depo='0' Kod='0'>"
        L"      <Crew EngineDriver='GARELOVS' Tim_Beg='2016-08-31 21:52' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160831T132955Z' name='Departure' Bdg='6C[11310]' waynum='6' parknum='1' />"
        L"  <SpotEvent create_time='20160831T220806Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20160831T222517Z' name='Span_stopping_begin' Bdg='LS7C[09801:09802]' waynum='1' intervalSec='662'>"
        L"    <rwcoord picketing1_val='51~240' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='52~137' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20160831T224628Z' name='Death' Bdg='LS7C[09801:09802]' waynum='1'>"
        L"    <rwcoord picketing1_val='51~240' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='52~137' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Transition Span_stopping_begin Death" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_stopping_begin' Bdg='LS7C[09801:09802]' create_time='20160831T222517Z' waynum='1' intervalSec='662'>"
        L"    <rwcoord picketing1_val='51~688' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Cut' />"
        L"  <PostPoint name='Death' Bdg='LS7C[09801:09802]' create_time='20160831T224628Z' waynum='1'>"
        L"    <rwcoord picketing1_val='51~688' picketing1_comm='Вентспилс' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows() );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_series( "name",            "Form            Departure        Transition        Death" ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time", "20160831T132955Z 20160831T132955Z 20160831T220806Z 20160831T221415Z" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",            "6C[11310]         6C[11310]        1C[09801]    LS7C[09801:09802]" ) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 ); 
}

void TC_Hem_aeCut::Ticket3912_split()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20161005T061348Z' name='Form' Bdg='1C[09200]' waynum='1' parknum='1' index='0920-006-0919' num='J8252' util='Y'>"
        L"    <Locomotive Series='ZT-25OS' NumLoc='120' Dor='0' Depo='0' Kod='0'>"
        L"      <Crew EngineDriver='HARCENKO' Tim_Beg='2016-10-05 06:00' />"
        L"    </Locomotive>"
        L"    <Locomotive Series='09-3XDY' NumLoc='3260' Dor='0' Depo='0' Kod='0'>"
        L"      <Crew EngineDriver='PIVRIKS' Tim_Beg='2016-10-05 06:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T061559Z' name='Departure' Bdg='1C[09200]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161005T062501Z' name='Arrival' Bdg='3C[09193]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20161005T064317Z' name='Departure' Bdg='3C[09193]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20161005T083104Z' name='Span_move' Bdg='SS10C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='277~483' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='276~436' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T084133Z' name='Span_stopping_begin' Bdg='SS10C[09191:09193]' waynum='1' intervalSec='629'>"
        L"    <rwcoord picketing1_val='277~483' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='276~436' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T092724Z' name='Span_stopping_end' Bdg='SS10C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='277~483' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='276~436' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T105117Z' name='Span_move' Bdg='SS1C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='270~255' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='269~520' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T105237Z' name='Disform' Bdg='SS1C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='270~255' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='269~520' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(9) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Arrival Departure Span_move Span_stopping_begin Span_stopping_end Span_move Disform" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_stopping_begin' Bdg='SS10C[09191:09193]' create_time='20161005T084133Z' waynum='1' intervalSec='629'>"
        L"    <rwcoord picketing1_val='276~959' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Cut' SaveLog='Y' />"
        L"  <PostPoint name='Span_stopping_end' Bdg='SS10C[09191:09193]' create_time='20161005T092724Z' waynum='1'>"
        L"    <rwcoord picketing1_val='276~959' picketing1_comm='Вентспилс' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows() );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Arrival Departure Span_move Span_stopping_begin Span_stopping_end Death" ) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "                                                                                 Span_stopping_begin Span_stopping_end Span_move Disform" ) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
}

void TC_Hem_aeCut::Ticket3912_toEnd()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20161005T061348Z' name='Form' Bdg='1C[09200]' waynum='1' parknum='1' index='0920-006-0919' num='J8252' util='Y'>"
        L"    <Locomotive Series='ZT-25OS' NumLoc='120' Dor='0' Depo='0' Kod='0'>"
        L"      <Crew EngineDriver='HARCENKO' Tim_Beg='2016-10-05 06:00' />"
        L"    </Locomotive>"
        L"    <Locomotive Series='09-3XDY' NumLoc='3260' Dor='0' Depo='0' Kod='0'>"
        L"      <Crew EngineDriver='PIVRIKS' Tim_Beg='2016-10-05 06:00' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T061559Z' name='Departure' Bdg='1C[09200]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20161005T062501Z' name='Arrival' Bdg='3C[09193]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20161005T064317Z' name='Departure' Bdg='3C[09193]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20161005T070634Z' name='Span_stopping_begin' Bdg='SS12C[09191:09193]' waynum='1' intervalSec='694'>"
        L"    <rwcoord picketing1_val='278~200' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='279~45' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T074409Z' name='Span_stopping_end' Bdg='SS12C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='278~200' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='279~45' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T074409Z' name='Span_move' Bdg='SS11C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='277~483' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='278~200' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T075659Z' name='Span_stopping_begin' Bdg='SS11C[09191:09193]' waynum='1' intervalSec='770'>"
        L"    <rwcoord picketing1_val='277~483' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='278~200' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T080530Z' name='Span_stopping_end' Bdg='SS11C[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='277~841' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20161005T080530Z' name='Death' Bdg='edit[09191:09193]' waynum='1'>"
        L"    <rwcoord picketing1_val='277~841' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(10) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_stopping_begin' Bdg='SS12C[09191:09193]' create_time='20161005T070634Z' waynum='1' intervalSec='694'>"
        L"    <rwcoord picketing1_val='278~622' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Cut' SaveLog='Y' />"
        L"  <PostPoint name='Span_stopping_end' Bdg='SS11C[09191:09193]' create_time='20161005T080530Z' waynum='1'>"
        L"    <rwcoord picketing1_val='277~841' picketing1_comm='Вентспилс' />"
        L"  </PostPoint>"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows() );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Arrival Departure Span_stopping_begin Span_stopping_end Death" ) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "                                                                       Span_stopping_begin Span_stopping_end Death" ) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
}

void TC_Hem_aeCut::implementUndo( HappenLayer& hl )
{
    for( auto trNode : undoer.document_element().child(HappenLayer_xAttr).children() )
    {
        TrioSpot eTrio;
        trNode >> eTrio;
        UtIdentifyCategoryList identifyCategory;
        Hem::aePusherUserHappen pusher( hl.GetWriteAccess(), eTrio, identifyCategory );
        pusher.Action();
    }
}

void TC_Hem_aeCut::collectUndo( HappenLayer& hl )
{
    undoer.reset();
    hl.GetReverseChanges( undoer.set_document_element("undo") );
    wstring ws = undoer.pretty_wstr();
    CPPUNIT_ASSERT( !ws.empty() );
}

std::shared_ptr<const SpotEvent> TC_Hem_aeCut::getSpotPtr(HappenLayer& happenLayer, const SpotEvent& spot)
{
    const EventAddress<SpotEvent> address(spot);
    SpotEventPtr spotPtr = happenLayer.GetWriteAccess()->getByAddress(address);
    CPPUNIT_ASSERT(spotPtr);
    return spotPtr;
}

std::shared_ptr<HappenPath> TC_Hem_aeCut::getPathPtr(HappenLayer& happenLayer, const SpotEvent& spot)
{
    SpotEventPtr spotPtr = getSpotPtr(happenLayer, spot);
    return happenLayer.GetWriteAccess()->getPath(spotPtr);
}

void TC_Hem_aeCut::CutPathBeginningUntilDeparture()
{
    SpotEvent path[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::ARRIVAL,    BadgeE(L"2p",EsrKit(200)), T+20 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"2p",EsrKit(200)), T+25 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    UtLayer<HappenLayer> happenLayer;
    happenLayer.createPath( path);
    CPPUNIT_ASSERT_EQUAL(size_t(1), happenLayer.path_count());

    std::shared_ptr<HappenPath> happenPath = getPathPtr(happenLayer, path[0]);
    CPPUNIT_ASSERT(happenPath);

    CPPUNIT_ASSERT_THROW(aeCut::cutPathBeginningUntilDepartureOrTransition(happenLayer.GetWriteAccess(), *happenPath, nullptr), HemHelpful::HemException);
    CPPUNIT_ASSERT(!aeCut::cutPathBeginningUntilDepartureOrTransition(happenLayer.GetWriteAccess(), *happenPath, happenPath->GetFirstEvent()));

    SpotEventPtr spot = getSpotPtr(happenLayer, path[3]);
    CPPUNIT_ASSERT(spot->GetCode() == HCode::DEPARTURE );
    std::shared_ptr<HappenPath> prevHappenPath = aeCut::cutPathBeginningUntilDepartureOrTransition(happenLayer.GetWriteAccess(), *happenPath, spot);
    CPPUNIT_ASSERT(prevHappenPath);

    CPPUNIT_ASSERT_EQUAL(size_t(4), prevHappenPath->GetEventsCount());
    CPPUNIT_ASSERT_EQUAL(size_t(5), happenPath->GetEventsCount());

    CPPUNIT_ASSERT(HCode::FORM == happenPath->GetFirstEvent()->GetCode());
    CPPUNIT_ASSERT(HCode::DEPARTURE == happenPath->GetNextEvent(*happenPath->GetFirstEvent())->GetCode());
    CPPUNIT_ASSERT(HCode::DEATH == prevHappenPath->GetLastEvent()->GetCode());
    CPPUNIT_ASSERT(HCode::ARRIVAL == prevHappenPath->GetPreviousEvent(*prevHappenPath->GetLastEvent())->GetCode());
}

void TC_Hem_aeCut::WorkaroundCutPathStartingDeparture()
{
    SpotEvent path[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::ARRIVAL,    BadgeE(L"2p",EsrKit(200)), T+20 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"2p",EsrKit(200)), T+25 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    UtLayer<HappenLayer> happenLayer;
    happenLayer.createPath( path);
    CPPUNIT_ASSERT_EQUAL(size_t(1), happenLayer.path_count());

    std::shared_ptr<HappenPath> happenPath = getPathPtr(happenLayer, path[0]);
    CPPUNIT_ASSERT(happenPath);

    CPPUNIT_ASSERT(!aeCut::cutPathBeginningUntilDepartureOrTransition(happenLayer.GetWriteAccess(), *happenPath, happenPath->GetFirstEvent()));

    SpotEventPtr spot = getSpotPtr(happenLayer, path[1]);
    CPPUNIT_ASSERT(spot->GetCode() == HCode::DEPARTURE);
    std::shared_ptr<HappenPath> prevHappenPath = aeCut::cutPathBeginningUntilDepartureOrTransition(happenLayer.GetWriteAccess(), *happenPath, spot);
    CPPUNIT_ASSERT(prevHappenPath);
    // расщепления не произошло
    CPPUNIT_ASSERT_EQUAL(size_t(1), happenLayer.path_count());
    CPPUNIT_ASSERT_EQUAL(size_t(7), prevHappenPath->GetEventsCount());
    CPPUNIT_ASSERT( happenPath == prevHappenPath );
}


void TC_Hem_aeCut::CutPathBeginningUntilTransition()
{
    SpotEvent path[] = {
        SpotEvent( HCode::FORM,       BadgeE(L"1p",EsrKit(100)), T+10 ),
        SpotEvent( HCode::DEPARTURE,  BadgeE(L"1p",EsrKit(100)), T+15 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"2p",EsrKit(200)), T+20 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"3p",EsrKit(300)), T+30 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"4p",EsrKit(400)), T+40 ),
        SpotEvent( HCode::TRANSITION, BadgeE(L"5p",EsrKit(500)), T+50 ),
    };
    UtLayer<HappenLayer> happenLayer;
    happenLayer.createPath( path);
    CPPUNIT_ASSERT_EQUAL(size_t(1), happenLayer.path_count());

    std::shared_ptr<HappenPath> happenPath = getPathPtr(happenLayer, path[0]);
    CPPUNIT_ASSERT(happenPath);

    CPPUNIT_ASSERT_THROW(aeCut::cutPathBeginningUntilDepartureOrTransition(happenLayer.GetWriteAccess(), *happenPath, nullptr), HemHelpful::HemException);
    CPPUNIT_ASSERT(!aeCut::cutPathBeginningUntilDepartureOrTransition(happenLayer.GetWriteAccess(), *happenPath, happenPath->GetFirstEvent()));

    SpotEventPtr spot = getSpotPtr(happenLayer, path[2]);
    CPPUNIT_ASSERT(spot->GetCode() == HCode::TRANSITION);
    std::shared_ptr<HappenPath> prevHappenPath = aeCut::cutPathBeginningUntilDepartureOrTransition(happenLayer.GetWriteAccess(), *happenPath, spot);
    CPPUNIT_ASSERT(prevHappenPath);

    CPPUNIT_ASSERT_EQUAL(size_t(4), prevHappenPath->GetEventsCount());
    CPPUNIT_ASSERT_EQUAL(size_t(5), happenPath->GetEventsCount());

    CPPUNIT_ASSERT(HCode::FORM == happenPath->GetFirstEvent()->GetCode());
    CPPUNIT_ASSERT(HCode::DEPARTURE == happenPath->GetNextEvent(*happenPath->GetFirstEvent())->GetCode());
    CPPUNIT_ASSERT(HCode::DEATH == prevHappenPath->GetLastEvent()->GetCode());
    CPPUNIT_ASSERT(HCode::ARRIVAL == prevHappenPath->GetPreviousEvent(*prevHappenPath->GetLastEvent())->GetCode());
}

void TC_Hem_aeCut::CutPathTailStationSpan()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20181004T004000Z' name='Span_move' Bdg='?[09280:09290]'>"
        L"    <rwcoord picketing1_val='63~473' picketing1_comm='Лиепая' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20181004T004800Z' name='Arrival' Bdg='?p1w3[09280]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20181004T005200Z' name='Departure' Bdg='?p1w3[09280]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20181004T010400Z' name='Span_move' Bdg='?[09180:09280]'>"
        L"    <rwcoord picketing1_val='49~667' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20181004T010400Z' name='Death' Bdg='?[09180:09280]'>"
        L"    <rwcoord picketing1_val='49~667' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move Arrival Departure Span_move Death" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20181004T025954Z'>"
        L"<PrePoint name='Arrival' Bdg='?p1w3[09280]' create_time='20181004T004800Z' waynum='3' parknum='1' />"
        L"<Action code='Cut' District='09180-09860' DistrictRight='Y' FixBound='20180622T152600Z' SaveLog='Y' />"
        L"<PostPoint name='Death' Bdg='?[09180:09280]' create_time='20181004T010400Z'>"
        L"<rwcoord picketing1_val='49~667' picketing1_comm='Реньге' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows() );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(3) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Span_move Arrival Death" ) );

    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
}

void TC_Hem_aeCut::StoppingBegin_End_WrongWay_3889()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20201017T061051Z' name='Form' Bdg='PpGP[09180]' index='0001-013-0928' num='8202' stone='Y'>"
        L"    <rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"    <Locomotive Series='У08-275' NumLoc='1132' Consec='1' CarrierCode='5'>"
        L"      <Crew EngineDriver='LAHTIONOVS' Tim_Beg='2020-10-17 07:30' />"
        L"    </Locomotive>"
        L"    <Locomotive Series='USP2005' NumLoc='825' Consec='2' CarrierCode='5'>"
        L"      <Crew EngineDriver='CELEJS' Tim_Beg='2020-10-17 06:30' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20201017T061051Z' name='Departure' Bdg='PpGP[09180]'>"
        L"    <rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20201017T061141Z' name='Wrong_way' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"    <rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"    <rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20201017T061141Z' name='Span_stopping_begin' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"    <rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"    <rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20201017T071927Z' name='Span_stopping_end' Bdg='NGJR[09180:09280]' waynum='1'>"
        L"    <rwcoord picketing1_val='44~' picketing1_comm='Реньге' />"
        L"    <rwcoord picketing1_val='50~' picketing1_comm='Реньге' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20201017T072332Z' name='Arrival' Bdg='5C[09180]' waynum='5' parknum='1' />"
        L"  <SpotEvent create_time='20201017T072332Z' name='Disform' Bdg='5C[09180]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(7) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Departure Wrong_way Span_stopping_begin Span_stopping_end Arrival Disform" ) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20201017T094540Z'>"
        L"<PrePoint name='Span_stopping_begin' Bdg='NGJR[09180:09280]' create_time='20201017T061141Z' waynum='1'>"
        L"<rwcoord picketing1_val='46~1000' picketing1_comm='Реньге' />"
        L"</PrePoint>"
        L"<Action code='Cut' District='09180-09860' DistrictRight='Y' SaveLog='Y' />"
        L"<PostPoint name='Span_stopping_end' Bdg='NGJR[09180:09280]' create_time='20201017T071927Z' waynum='1'>"
        L"<rwcoord picketing1_val='46~1000' picketing1_comm='Реньге' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows() );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==2 );
    CPPUNIT_ASSERT( hl.exist_series( "name",            "Form            Departure        Wrong_way      Span_stopping_begin  Span_stopping_end     Death" ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time", "20201017T061051Z 20201017T061051Z 20201017T061141Z   20201017T061141Z    20201017T062837Z  20201017T062837Z" ) );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",            "PpGP[09180]      PpGP[09180]     NGJR[09180:09280] NGJR[09180:09280]    NGJR[09180:09280]   edit[09180:09280]" ) );
    CPPUNIT_ASSERT( hl.exist_series( "name",            "Form            Wrong_way       Span_stopping_begin Span_stopping_end      Arrival         Disform" ) );
    CPPUNIT_ASSERT( hl.exist_series( "create_time", "20201017T070231Z 20201017T070231Z    20201017T070231Z    20201017T071927Z 20201017T072332Z   20201017T072332Z") );
    CPPUNIT_ASSERT( hl.exist_series("Bdg",         "NGJR[09180:09280] NGJR[09180:09280]   NGJR[09180:09280]   NGJR[09180:09280]  5C[09180]          5C[09180] " ) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 ); 
}

void TC_Hem_aeCut::RemoveEndOfPath()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20190212T124500Z' name='Departure' Bdg='ASOUP 1042[09820]' waynum='16' parknum='8' index='0982-011-1800' num='V2428' length='57' weight='1377' through='Y'>"
        L"    <Locomotive Series='2ТЭ116' NumLoc='1047' Consec='1' CarrierCode='2'>"
        L"      <Crew EngineDriver='BURKOVSKIJS' Tim_Beg='2019-02-12 13:27' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190212T125610Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' optCode='09812:09813' />"
        L"  <SpotEvent create_time='20190212T130552Z' name='Arrival' Bdg='3C[09812]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20190212T132207Z' name='Departure' Bdg='3C[09812]' waynum='3' parknum='1' optCode='09810:09812' />"
        L"  <SpotEvent create_time='20190212T134510Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' optCode='09803:09810' />"
        L"  <SpotEvent create_time='20190212T135436Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' optCode='09802:09803' />"
        L"  <SpotEvent create_time='20190212T140515Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' optCode='09801:09802' />"
        L"  <SpotEvent create_time='20190212T141714Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' optCode='09800:09801' />"
        L"  <SpotEvent create_time='20190212T143039Z' name='Arrival' Bdg='1AC[09800]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20190212T144618Z' name='Departure' Bdg='1BC[09800]' waynum='1' parknum='1' optCode='09790:09800' />"
        L"  <SpotEvent create_time='20190212T145605Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' optCode='09780:09790' />"
        L"  <SpotEvent create_time='20190212T150521Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' optCode='09751:09780' />"
        L"  <SpotEvent create_time='20190212T151917Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' optCode='09750:09751' />"
        L"  <SpotEvent create_time='20190212T152934Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' optCode='09750:09772' />"
        L"  <SpotEvent create_time='20190212T154730Z' name='Transition' Bdg='1C[09772]' waynum='1' parknum='1' optCode='09764:09772' />"
        L"  <SpotEvent create_time='20190212T155935Z' name='Span_move' Bdg='SV21C[09764:09772]' waynum='1'>"
        L"    <rwcoord picketing1_val='140~887' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='141~750' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20190212T155956Z' name='Death' Bdg='SV21C[09764:09772]' waynum='1'>"
        L"    <rwcoord picketing1_val='140~887' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='141~750' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(17) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20190212T180600Z'>"
        L"<PrePoint name='Transition' Bdg='2C[09790]' create_time='20190212T145605Z' waynum='2' parknum='1' />"
        L"<Action code='Cut' District='09180-09820' DistrictRight='Y' FixBound='20180227T110500Z' />"
        L"<PostPoint name='Death' Bdg='SV21C[09764:09772]' create_time='20190212T155956Z' waynum='1'>"
        L"<rwcoord picketing1_val='141~318' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows() );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(12) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 ); 
    CPPUNIT_ASSERT( hl.exist_path_size(17) );
}

void TC_Hem_aeCut::RemoveBeginOfPath()
{
    UtLayer<HappenLayer> hl;
	UtLayer<AsoupLayer> al;
    hl.createPath( 
        L"<HemPath>"
        L"  <SpotEvent create_time='20201113T134534Z' name='Form' Bdg='6C[09180]' waynum='6' parknum='1'  index='1100-093-0982' num='V2723' length='58' weight='4351' net_weight='2908' through='Y' >"
        L"    <Locomotive Series='2М62УМ' NumLoc='93' Consec='1' CarrierCode='4'>"
        L"      <Crew EngineDriver='M-GARKULS' Tim_Beg='2020-11-13 14:18' />"
        L"    </Locomotive>"
        L"  </SpotEvent>"
        L"  <SpotEvent create_time='20201113T134534Z' name='Departure' Bdg='6C[09180]' waynum='6' parknum='1' />"
        L"  <SpotEvent create_time='20201113T141547Z' name='Transition' Bdg='1C[09764]' waynum='1' parknum='1' optCode='09764:09772' />"
        L"  <SpotEvent create_time='20201113T143934Z' name='Transition' Bdg='1C[09772]' waynum='1' parknum='1' optCode='09750:09772' />"
        L"  <SpotEvent create_time='20201113T150548Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20201113T151919Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' optCode='09751:09780' />"
        L"  <SpotEvent create_time='20201113T153510Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' optCode='09780:09790' />"
        L"  <SpotEvent create_time='20201113T155309Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' />"
        L"  <SpotEvent create_time='20201113T160449Z' name='Transition' Bdg='2C[09800]' waynum='2' parknum='1' optCode='09800:09801' />"
        L"  <SpotEvent create_time='20201113T160702Z' name='Span_move' Bdg='SA8C[09800:09801]' waynum='1'>"
        L"    <rwcoord picketing1_val='62~344' picketing1_comm='Вентспилс' />"
        L"    <rwcoord picketing1_val='63~314' picketing1_comm='Вентспилс' />"
        L"  </SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(10) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20201113T160738Z'>"
        L"<PrePoint name='Departure' Bdg='6C[09180]' create_time='20201113T134534Z' Bdg='6C[09180]' waynum='6' parknum='1' />"
        L"<Action code='Cut' District='09180-09820' DistrictRight='Y' />"
        L"<PostPoint name='Transition' Bdg='1C[09764]' create_time='20201113T141547Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;

    Context context(Hem::Chance::System(), esrGuide, guessTransciever, uncontrolledStations, identifyCategory, nullptr, nullptr, nullptr, 0 );
    aeCut cut( hl.GetWriteAccess(), al.GetReadAccess(), context, bill.getHeadSpotThrows(), bill.getTailSpotThrows() );
    cut.Action();
    collectUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(9) );
    CPPUNIT_ASSERT( hl.exist_series( "num",   "  V2723 -  -  -  -  -  -  -  -" ) );
    CPPUNIT_ASSERT( hl.exist_series( "index", "1100-093-0982  -  -  -  -  -  -  -  -" ) );
    implementUndo(hl);
    CPPUNIT_ASSERT( hl.path_count()==1 ); 
    CPPUNIT_ASSERT( hl.exist_path_size(10) );
    CPPUNIT_ASSERT( hl.exist_series( "num",   "  V2723 -  -  -  -  -  -  -  -  -" ) );
    CPPUNIT_ASSERT( hl.exist_series( "index", "1100-093-0982  -  -  -  -  -  -  -  -  -" ) );
}
