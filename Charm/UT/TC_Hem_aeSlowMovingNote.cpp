#include "stdafx.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "TC_Hem_aeSlowMovingNote.h"
#include "TC_Hem_ae_helper.h"
#include "../Hem/Bill.h"
#include "../Hem/UserChart.h"
#include "../Hem/Appliers/aeSlowMovingNote.h"
#include "../Hem/LimitTrainKeeper.h"
#include "../Hem/HappenPath.h"
#include "../helpful/Serialization.h"
#include "../Guess/SpotDetails.h"
#include "../helpful/SpanTravelTime.h"
#include "../helpful/EsrGuide.h"
#include "../Hem/Appliers/aeLimit.h"
#include "../Hem/Appliers/aeForbid.h"

using namespace std;
using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeSlowMovingNote );

void TC_Hem_aeSlowMovingNote::AddNote()
{
    createPath();

    UtLayer<Note::Layer> noteLayer;
    UtLayer<Limit::Layer> limitLayer;
    UtLayer<Forbid::Layer> forbidLayer;
    UtLayer<Regulatory::Layer> regulatoryLayer;

    HappenLayer& hl = userChart.getHappenLayer();
    
    CPPUNIT_ASSERT( noteLayer.path_count()==0 );
    Hem::aeSlowMovingNote noteCreator(hl.GetReadAccess(), userChart.getEsrGuide(), noteLayer, limitLayer, forbidLayer, regulatoryLayer, spanTravelTime, pathPtr->GetLastEvent());
    noteCreator.Action();
    CPPUNIT_ASSERT( noteLayer.path_count()==1 );
    CPPUNIT_ASSERT( noteLayer.exist_series("esr", "09780:09790") );
    //CPPUNIT_ASSERT( noteLayer.exist_series("Comment",ToUtf8( L"Поезд V2432. Превышение ПВХ.")));
    CPPUNIT_ASSERT( noteLayer.exist_series("inner_name", "srv10"));
}

void TC_Hem_aeSlowMovingNote::AddNoteOnSpanWithLimit()
{
    createPath();

    //событие с превышением ПВХ на перегоне с предупреждением
    AddEventToPath( L"<SpotEvent create_time='20180726T213346Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' intervalSec='3' optCode='09750:09751' />");
    
    UtLayer<Limit::Layer> limitLayer;
    Limit::Event limit = createEvent<Limit::Event>(
        L"<LimitEvent esr='09751:09780' waynum='1' FullSpan='Y' StartTime='20180726T1804346Z' Reason='1601' Sudden='Y' UntilStop='Y' SpeedPass='60' SpeedGoods='60'>"
        L"<picketing_start picketing1_val='85~200' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='97~300' picketing1_comm='Вентспилс' />"
        L"</LimitEvent>" );

    CPPUNIT_ASSERT( limitLayer.path_count()==0 );
    Hem::ApplierCarryOut::aeLimitCreate limitCreator( limitLayer, limit);
    limitCreator.Action();
    CPPUNIT_ASSERT( limitLayer.path_count()==1 );

    HappenLayer& hl = userChart.getHappenLayer();
    UtLayer<Note::Layer> noteLayer;
    UtLayer<Forbid::Layer> forbidLayer;
    UtLayer<Regulatory::Layer> regulatoryLayer;

    CPPUNIT_ASSERT( noteLayer.path_count()==0 );
    Hem::aeSlowMovingNote noteCreator(hl.GetReadAccess(), userChart.getEsrGuide(), noteLayer, limitLayer, forbidLayer, regulatoryLayer, spanTravelTime, pathPtr->GetLastEvent());
    noteCreator.Action();
    CPPUNIT_ASSERT( noteLayer.path_count()==1 );
    CPPUNIT_ASSERT( noteLayer.exist_series("esr", "09751:09780") );
    //CPPUNIT_ASSERT( noteLayer.exist_series("Comment",ToUtf8( L"Поезд V2432. Превышение ПВХ.")));
    CPPUNIT_ASSERT( noteLayer.exist_series("inner_name", "srvWay"));
}

void TC_Hem_aeSlowMovingNote::AddNoteOnSpanWithForbid()
{
    createPath();

    //событие без превышения ПВХ
    AddEventToPath( L"<SpotEvent create_time='20180726T205546Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' intervalSec='3' optCode='09750:09751' />");

    HappenLayer& hl = userChart.getHappenLayer();
    UtLayer<Note::Layer> noteLayer;
    UtLayer<Limit::Layer> limitLayer;
    UtLayer<Forbid::Layer> forbidLayer;
    UtLayer<Regulatory::Layer> regulatoryLayer;

    {
    CPPUNIT_ASSERT( noteLayer.path_count()==0 );
    Hem::aeSlowMovingNote noteCreator(hl.GetReadAccess(), userChart.getEsrGuide(), noteLayer, limitLayer, forbidLayer, regulatoryLayer, spanTravelTime, pathPtr->GetLastEvent());
    noteCreator.Action();
    CPPUNIT_ASSERT( noteLayer.path_count()==0 );
    }

    //событие с превышением ПВХ на перегоне с окном
    AddEventToPath( L"<SpotEvent create_time='20180726T215546Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' intervalSec='3' optCode='09750:09772' />");

    Forbid::Event forbid = createEvent<Forbid::Event>( L"<ForbidEvent Id='i1' esr='09750:09751' waynum='1' FullSpan='Y' StartTime='20180726T205046Z' StopTime='20180726T225546Z' Reason='458752' inner_name='srvWay'>"
        L"<picketing_start picketing1_val='98~58' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"</ForbidEvent>" );
    CPPUNIT_ASSERT( forbidLayer.path_count()==0 );
    Hem::ApplierCarryOut::aeForbidCreate forbidCreator( forbidLayer, forbid);
    forbidCreator.Action();
    CPPUNIT_ASSERT( forbidLayer.path_count()==1 );
    CPPUNIT_ASSERT( limitLayer.path_count()==0 );


    {
    CPPUNIT_ASSERT( noteLayer.path_count()==0 );
    Hem::aeSlowMovingNote noteCreator(hl.GetReadAccess(), userChart.getEsrGuide(), noteLayer, limitLayer, forbidLayer, regulatoryLayer, spanTravelTime, pathPtr->GetLastEvent());
    noteCreator.Action();
    CPPUNIT_ASSERT( noteLayer.path_count()==1 );
    }
    CPPUNIT_ASSERT( noteLayer.exist_series("esr", "09750:09751") );
    //CPPUNIT_ASSERT( noteLayer.exist_series("Comment",ToUtf8( L"Поезд V2432. Превышение ПВХ.")));
    CPPUNIT_ASSERT( noteLayer.exist_series("inner_name", "srvWay"));
}

bool TC_Hem_aeSlowMovingNote::AddEventToPath(const std::wstring& str)
{
    return userChart.acceptTrackerEvent( deserialize<SpotEvent>(ToUtf8(str)), 32, TrainCharacteristics::Source::Guess, 0);
}

void TC_Hem_aeSlowMovingNote::createPath()
{
    AddEventToPath( L"<SpotEvent create_time='20180726T194139Z' name='Form' Bdg='SN1C[09790]' waynum='1' parknum='1' index='' num='V2432' through='Y'/>" );
    AddEventToPath( L"<SpotEvent create_time='20180726T194301Z' name='Departure' Bdg='SN1C[9790]' waynum='1' parknum='1' />");
    AddEventToPath( L"<SpotEvent create_time='20180726T204346Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' intervalSec='3' optCode='09751:09780' />");
    
    time_t t = (from_iso_string("20180726T194139Z") - from_time_t(0)).total_seconds();
    auto firstEventPtr = std::make_shared<const SpotEvent>(HCode::FORM, BadgeE(L"SN1C",EsrKit(9790)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );
    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);
}

void TC_Hem_aeSlowMovingNote::setUp()
{
    {
        attic::a_document doc;
        doc.load_wide(
            L"<PvxList>"
            L"<Pvx Span='(9750,9751)'>"     
            L"<Gruz start='9' full='15' stop='3'/>"
            L"<Dies start='2' full='7.5' stop='1'/>"
            L"</Pvx>"
            L"<Pvx Span='(9751,9750)'>"     
            L"<Gruz start='3' full='9' stop='2'/>"
            L"<Dies start='2' full='7.5' stop='1'/>"
            L"</Pvx>"
            L"<Pvx Span='(9751,9780)'>"     
            L"<Gruz start='6' full='13' stop='2'/>"
            L"<Dies start='3' full='9.5' stop='1.5'/>"
            L"</Pvx>"
            L"<Pvx Span='(9780,9751)'>"    
            L"<Gruz start='3' full='13' stop='2'/>"
            L"<Dies start='2.5' full='9.5' stop='1.5'/>"
            L"</Pvx>"
            L"<Pvx Span='(9780,9790)'>" 
            L"<Gruz start='8' full='19' stop='2'/>"
            L"<Dies start='3.5' full='8' stop='2'/>"
            L"</Pvx>"
            L"<Pvx Span='(9790,9780)'>"
            L"<Gruz start='3' full='9' stop='2'/>"
            L"<Dies start='3' full='8' stop='1.5'/>"
            L"</Pvx>"
            L"</PvxList>"
            );
        std::vector<std::wstring> vMsg;
        if ( auto _spanTravelTime = SpanTravelTime::deserializeLogWarnings(doc.pretty_str(), vMsg))
            spanTravelTime = *_spanTravelTime;
    }
    
    {
    attic::a_document doc;
    EsrGuide eg;
    doc.load_wide(
        L"<EsrGuide>"
        L"<EsrGuide kit='09750' name='Tukums-II' picketing1_val='108~300' picketing1_comm='Вентспилс' picketing2_val='64~400' picketing2_comm='Рига-Тукумс'>"
        L"<Way waynum='1' parknum='1' passenger='Y' embarkation='Y' />"
        L"<Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"<Way waynum='3' parknum='1' major='Y' passenger='Y' />"
        L"<Way waynum='4' parknum='1' />"
        L"<Way waynum='5' parknum='1' />"
        L"<Way waynum='6' parknum='1' />"
        L"</EsrGuide>"
        L"<EsrGuide kit='09751' name='Zvare' picketing1_val='97~900' picketing1_comm='Вентспилс'>"
        L"<Way waynum='1' parknum='1' major='Y' passenger='Y' />"
        L"<Way waynum='3' parknum='1' embarkation='Y' />"
        L"</EsrGuide>"
        L"<EsrGuide kit='09780' name='Kandava' picketing1_val='84~400' picketing1_comm='Вентспилс'>"
        L"<Way waynum='1' parknum='1' major='Y' passenger='Y' />"
        L"<Way waynum='2' parknum='1' />"
        L"<Way waynum='3' parknum='1' />"
        L"<Way waynum='5' parknum='1' passenger='Y' embarkation='Y' />"
        L"</EsrGuide>"
        L"<EsrGuide kit='09790' name='Sabile' picketing1_val='72~700' picketing1_comm='Вентспилс'>"
        L"<Way waynum='1' parknum='1' passenger='Y' embarkation='Y' />"
        L"<Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
        L"<Way waynum='3' parknum='1' />"
        L"<Way waynum='4' parknum='1' />"
        L"</EsrGuide>"
        L"<SpanGuide kit='09750:09751' orientation='odd'>"
        L"<Way wayNum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='98~858' picketing1_comm='Вентспилс' />"
        L"</Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09751:09780' orientation='odd'>"
        L"<Way wayNum='1'>"
        L"<rwcoord picketing1_val='97~226' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='85~298' picketing1_comm='Вентспилс' />"
        L"</Way>"
        L"</SpanGuide>"
        L"<SpanGuide kit='09780:09790' orientation='odd'>"
        L"<Way wayNum='1'>"
        L"<rwcoord picketing1_val='83~492' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='73~986' picketing1_comm='Вентспилс' />"
        L"</Way>"
        L"</SpanGuide>"
        L"</EsrGuide>");
    eg.load_from( doc.document_element() );
    userChart.setupEsrGuide(eg);
    }
}

void TC_Hem_aeSlowMovingNote::tearDown()
{
}

