#include "stdafx.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "TC_Hem_aeSpanWork.h"
#include "TC_Hem_ae_helper.h"
#include "../Hem/Bill.h"
#include "../Hem/UserChart.h"
#include "../Hem/Appliers/aeSpanWork.h"
#include "../Hem/SpanWorkInformator.h"
#include "../helpful/EsrGuide.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/HappenPath.h"
#include "../Hem/Appliers/aeRectify.h"
#include "../helpful/Serialization.h"
#include "../helpful/LocoCharacteristics.h"
#include "TopologyTest.h"

using namespace std;
using namespace boost::posix_time;
///\todo Написать тест для редактирования неправильного задания (с удалением пометки и склейкой нитей)
CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeSpanWork );

void TC_Hem_aeSpanWork::WorkWithReturn()
{
    createPathAndSpanWorkWithReturnOnStation();

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==6);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);

    {
        CPPUNIT_ASSERT(AddEventToPath( CreateEvent( 
            L"<SpotEvent create_time='20180726T220101Z' name='Wrong_way' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
            L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
            L"<rwcoord picketing1_val='27~100' picketing1_comm='Торнянкалнс-Елгава' />"
            L"</SpotEvent>"
            )));
    CPPUNIT_ASSERT(AddEventToPath( CreateEvent( 
            L"<SpotEvent create_time='20180726T220101Z' name='Span_move' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
            L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
            L"<rwcoord picketing1_val='27~100' picketing1_comm='Торнянкалнс-Елгава' />"
            L"</SpotEvent>"
            )));

    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==8);
    CPPUNIT_ASSERT(pathPtr->GetLastEvent()->GetCode()==HCode::SPAN_STOPPING_BEGIN);
    }

    {
    time_t t = (from_iso_string("20180727T011540Z") - from_time_t(0)).total_seconds();
    PicketingInfo pckInfo1, pckInfo2;
    pckInfo1.SetCoord( rwRuledCoord( L"Торнянкалнс-Елгава", rwCoord( 27, 100 )) );
    pckInfo2.SetCoord( rwRuledCoord( L"Торнянкалнс-Елгава", rwCoord( 27, 500 )) );
    SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
        ParkWayKit(1, 1), std::make_pair(pckInfo1, pckInfo2));

    auto spotPtr = std::make_shared<SpotEvent>(HCode::SPAN_MOVE, BadgeE(L"CAKPU1_C",EsrKit(9150, 9160)), t,  spotDetails);
    CPPUNIT_ASSERT(AddEventToPath(spotPtr));

    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==10);
    CPPUNIT_ASSERT( pathPtr->GetLastEvent()->GetCode()==HCode::SPAN_MOVE );
    auto prevEvent = pathPtr->GetPreviousEvent(*pathPtr->GetLastEvent());
    CPPUNIT_ASSERT(prevEvent->GetCode()==HCode::SPAN_STOPPING_END);
    CPPUNIT_ASSERT(prevEvent->GetTime()==t);
    }

    {
    //Разрыв нити
        time_t t = (from_iso_string("20180727T015030Z") - from_time_t(0)).total_seconds();
        PicketingInfo pckInfo1, pckInfo2;
        pckInfo1.SetCoord( rwRuledCoord( L"Торнянкалнс-Елгава", rwCoord( 26, 500 )) );
        pckInfo2.SetCoord( rwRuledCoord( L"Торнянкалнс-Елгава", rwCoord( 27, 100 )) );
        SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
            ParkWayKit(1, 1), std::make_pair(pckInfo1, pckInfo2));

        auto spotPtr = std::make_shared<SpotEvent>(HCode::SPAN_MOVE, BadgeE(L"KPU6A_C",EsrKit(9150, 9160)), t,  spotDetails);
        CPPUNIT_ASSERT(AddEventToPath(spotPtr));

        CPPUNIT_ASSERT( pathPtr && pathPtr->GetEventsCount()==3 );

        auto createdPathPtr = userChart.getHappenLayer().GetWriteAccess()->getPath(firstEventPtr);
        CPPUNIT_ASSERT(createdPathPtr && createdPathPtr->GetEventsCount()==10);
        CPPUNIT_ASSERT(createdPathPtr && createdPathPtr->GetLastEvent()->GetCode()==HCode::DEATH);
        
        ConstTrainDescrPtr pTrain = pathPtr->GetFragment(pathPtr->GetFirstEvent());
        CPPUNIT_ASSERT( pTrain && pTrain->GetNumber().getNumber() == 8634 && pTrain->GetIndex().str()==L"1111-111-1111" );
    }

//         L"<SpotEvent create_time='20180726T224213Z' name='Span_stopping_begin' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='28~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>"
//         L"<SpotEvent create_time='20180726T231614Z' name='Span_stopping_end' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='28~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>"
//         L"<SpotEvent create_time='20180727T001411Z' name='Span_stopping_begin' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='28~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>"
//         L"<SpotEvent create_time='20180727T005739Z' name='Span_stopping_end' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='28~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>"
//         L"<SpotEvent create_time='20180727T011540Z' name='Span_move' Bdg='CAKPU1_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='22~900' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='24~400' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>"

//     std::string strEventBefore = ToUtf8(L"<SpotEvent create_time='20180726T215802Z' name='Span_move' Bdg='CAKPU2_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='24~400' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>");
// 
//     std::string strEventSpanMove = ToUtf8(L"<SpotEvent create_time='20180726T220101Z' name='Span_move' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='28~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>");
// 
//     std::string strEventSpanStoppingBegin = ToUtf8(L"<SpotEvent create_time='20180726T220300Z' name='Span_stopping_begin' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='28~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>");
// 
//     std::string strEventSpanStoppingEnd = ToUtf8(L"<SpotEvent create_time='20180726T223000Z' name='Span_stopping_end' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
//         L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"<rwcoord picketing1_val='28~500' picketing1_comm='Торнянкалнс-Елгава' />"
//         L"</SpotEvent>");


    
}

void TC_Hem_aeSpanWork::WorkWithReturnError()
{
    createPathAndSpanWorkWithReturnOnStation();

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==6);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);

    {
        CPPUNIT_ASSERT(AddEventToPath( CreateEvent( 
            L"<SpotEvent create_time='20180726T220101Z' name='Span_move' Bdg='KPU6A_C[09150:09160]' waynum='1'>"
            L"<rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
            L"<rwcoord picketing1_val='27~100' picketing1_comm='Торнянкалнс-Елгава' />"
            L"</SpotEvent>"
            )));

        CPPUNIT_ASSERT(pathPtr->GetEventsCount()==7);
        CPPUNIT_ASSERT(pathPtr->GetLastEvent()->GetCode()==HCode::SPAN_STOPPING_BEGIN);
    }

    {
        time_t t = (from_iso_string("20180727T011540Z") - from_time_t(0)).total_seconds();
        PicketingInfo pckInfo1, pckInfo2;
        pckInfo1.SetCoord( rwRuledCoord( L"Торнянкалнс-Елгава", rwCoord( 27, 100 )) );
        pckInfo2.SetCoord( rwRuledCoord( L"Торнянкалнс-Елгава", rwCoord( 27, 500 )) );
        SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
            ParkWayKit(1, 1), std::make_pair(pckInfo1, pckInfo2));

        auto spotPtr = std::make_shared<SpotEvent>(HCode::SPAN_MOVE, BadgeE(L"CAKPU1_C",EsrKit(9150, 9160)), t,  spotDetails);
        CPPUNIT_ASSERT(AddEventToPath(spotPtr));

        CPPUNIT_ASSERT(pathPtr->GetEventsCount()==9);
        CPPUNIT_ASSERT( pathPtr->GetLastEvent()->GetCode()==HCode::SPAN_MOVE );
        auto prevEvent = pathPtr->GetPreviousEvent(*pathPtr->GetLastEvent());
        CPPUNIT_ASSERT(prevEvent->GetCode()==HCode::SPAN_STOPPING_END);
        CPPUNIT_ASSERT(prevEvent->GetTime()==t);
    }

    {
        //Прибытие на "неправильную" станцию
        time_t t = (from_iso_string("20180727T015030Z") - from_time_t(0)).total_seconds();
        SpotDetailsCPtr spotDetails = std::make_shared<SpotDetails>(
            ParkWayKit(1, 1), std::make_pair(PicketingInfo(), PicketingInfo()));

        auto spotPtr = std::make_shared<SpotEvent>(HCode::ARRIVAL, BadgeE(L"1p",EsrKit(9160)), t,  spotDetails);
        CPPUNIT_ASSERT(AddEventToPath(spotPtr));

        CPPUNIT_ASSERT( pathPtr && pathPtr->GetEventsCount()==9 );
        auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, spanWorkPtr->GetESR());
        CPPUNIT_ASSERT( pSpanWork && !pSpanWork->GetMarkId().empty() );
    }

}

std::shared_ptr<SpotEvent> TC_Hem_aeSpanWork::CreateEvent( const std::wstring& str)
{
return deserialize<SpotEvent>(ToUtf8(str));
}

void TC_Hem_aeSpanWork::createPathAndSpanWorkWithReturnOnStation()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180726T194139Z' name='Form' Bdg='5p[09100]' waynum='5' parknum='1' index='0001-224-0915' num='8633' motortroll='Y'>"
        L"<Locomotive Series='АДМ' NumLoc='1506' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='LJVOVSKIS' Tim_Beg='2018-07-26 20:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        );
    AddEventToPath( firstEventPtr );
    AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180726T194301Z' name='Departure' Bdg='5p[09100]' waynum='5' parknum='1' optCode='09100:09104' />"
        ));
    AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180726T194901Z' name='Arrival' Bdg='KPU13A/6A[09104]' waynum='1' parknum='1' />"
        ));
    AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180726T195100Z' name='Departure' Bdg='KPU13A/6A[09104]' waynum='1' parknum='1' />"
        ));
    AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180726T200451Z' name='Arrival' Bdg='3p[09150]' waynum='3' parknum='1' />"
        ));
    AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180726T210428Z' name='Departure' Bdg='3p[09150]' waynum='3' parknum='1' optCode='09150:09160' />"
        ));

    std::wstring text = L"<SpanWork esr='09150:09160' waynum='1' duration='180' returnFlag='Y' num='8634' index='1111-111-1111'>"
        L"<picketing_start picketing1_val='27~' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<picketing_stop picketing1_val='27~4' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpanWork>";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    time_t t = (from_iso_string("20180726T210428Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::DEPARTURE, BadgeE(L"3p",EsrKit(9150)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( spotPtr );
}

void TC_Hem_aeSpanWork::setUp()
{
    attic::a_document doc;
    EsrGuide eg; 
     doc.load_wide(
         L"<EsrGuide>"
         L"<EsrGuide kit='09150' name='Olaine' picketing1_val='22~' picketing1_comm='Рига' picketing2_val='22~' picketing2_comm='Торнянкалнс-Елгава'>"
         L"<Way waynum='1' parknum='1' />"
         L"<Way waynum='2' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='4' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
         L"<Way waynum='5' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
         L"<Way waynum='8' parknum='1' />"
         L"<Way waynum='9' parknum='1' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='09160' name='Cena' picketing1_val='33~100' picketing1_comm='Рига' picketing2_val='33~100' picketing2_comm='Торнянкалнс-Елгава'>"
         L"<Way waynum='1' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
         L"<Way waynum='4' parknum='1' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='11430' name='Livani' picketing1_val='330~800' picketing1_comm='Вентспилс'>"
         L"<Way waynum='1' parknum='1' major='Y' passenger='Y' />"
         L"<Way waynum='2' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' />"
         L"<Way waynum='4' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='5' parknum='1' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='11431' name='Jersika' picketing1_val='341~800' picketing1_comm='Вентспилс'>"
         L"<Way waynum='1' parknum='1' major='Y' passenger='Y' />"
         L"<Way waynum='2' parknum='1' />"
         L"<Way waynum='3' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='1' parknum='2' major='Y' passenger='Y' embarkation='Y' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='11443' name='Liksna' picketing1_val='375~500' picketing1_comm='Вентспилс'>"
         L"<Way waynum='1' parknum='1' major='Y' passenger='Y' />"
         L"<Way waynum='2' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='11445' name='C.p.383km' picketing1_val='10~100' picketing1_comm='1км' picketing2_val='382~891' picketing2_comm='Вентспилс' />"
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
         L"<EsrGuide kit='09370' name='Aizkraukle' picketing1_val='82~500' picketing1_comm='Рига-Зилупе'>"
         L"<Way waynum='1' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
         L"<Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='4' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='6' parknum='1' passenger='Y' embarkation='Y' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='09380' name='Skriveri' picketing1_val='72~700' picketing1_comm='Рига-Зилупе'>"
         L"<Way waynum='1' parknum='1' major='Y' passenger='Y' />"
         L"<Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' passenger='Y' />"
         L"<Way waynum='5' parknum='1' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='09211' name='Lacplesis' picketing1_val='231~700' picketing1_comm='Вентспилс'>"
         L"<Way waynum='1' parknum='1' major='Y' />"
         L"<Way waynum='2' parknum='1' />"
         L"<Way waynum='3' parknum='1' passenger='Y' embarkation='Y' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='09220' name='Vecumnieki' picketing1_val='215~600' picketing1_comm='Вентспилс'>"
         L"<Way waynum='1' parknum='1' major='Y' passenger='Y' />"
         L"<Way waynum='2' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' />"
         L"</EsrGuide>"
         L"<SpanGuide kit='09150:09160' orientation='odd'>"
         L"<Way wayNum='1'>"
         L"<rwcoord picketing1_val='22~900' picketing1_comm='Торнянкалнс-Елгава' />"
         L"<rwcoord picketing1_val='32~152' picketing1_comm='Торнянкалнс-Елгава' />"
         L"</Way>"
         L"<Way wayNum='2'>"
         L"<rwcoord picketing1_val='22~900' picketing1_comm='Торнянкалнс-Елгава' />"
         L"<rwcoord picketing1_val='32~169' picketing1_comm='Торнянкалнс-Елгава' />"
         L"</Way>"
         L"</SpanGuide>"
         L"<SpanGuide kit='11443:11445' orientation='even'>"
         L"<Way wayNum='1'>"
         L"<rwcoord picketing1_val='376~540' picketing1_comm='Вентспилс' />"
         L"<rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
         L"</Way>"
         L"</SpanGuide>"
         L"<SpanGuide kit='09750:09751' orientation='odd'>"
         L"<Way wayNum='1'>"
         L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
         L"<rwcoord picketing1_val='98~858' picketing1_comm='Вентспилс' />"
         L"</Way>"
         L"</SpanGuide>"
         L"<SpanGuide kit='09370:09380' orientation='odd'>"
         L"<Way wayNum='1'>"
         L"<rwcoord picketing1_val='82~' picketing1_comm='Рига-Зилупе' />"
         L"<rwcoord picketing1_val='73~500' picketing1_comm='Рига-Зилупе' />"
         L"</Way>"
         L"<Way wayNum='2'>"
         L"<rwcoord picketing1_val='82~' picketing1_comm='Рига-Зилупе' />"
         L"<rwcoord picketing1_val='73~500' picketing1_comm='Рига-Зилупе' />"
         L"</Way>"
         L"</SpanGuide>"
         L"<SpanGuide kit='09211:09220' orientation='odd'>"
         L"<Way wayNum='1'>"
         L"<rwcoord picketing1_val='230~727' picketing1_comm='Вентспилс' />"
         L"<rwcoord picketing1_val='216~510' picketing1_comm='Вентспилс' />"
         L"</Way>"
         L"</SpanGuide>"
         L"<SpanGuide kit='11430:11431' orientation='even'>"
         L"<Way wayNum='1'>"
         L"<rwcoord picketing1_val='331~816' picketing1_comm='Вентспилс' />"
         L"<rwcoord picketing1_val='340~650' picketing1_comm='Вентспилс' />"
         L"</Way>"
         L"</SpanGuide>"
         L"</EsrGuide>");
     eg.load_from( doc.document_element() );
     userChart.setupEsrGuide(eg);
     std::shared_ptr<TopologyTest> tt( new TopologyTest );
     userChart.setupTopology(tt);
}

void TC_Hem_aeSpanWork::tearDown()
{
    spanWorkPtr.reset();
    pathPtr.reset();
    firstEventPtr.reset();
    LocoCharacteristics::Shutdowner();
}

bool TC_Hem_aeSpanWork::AddEventToPath(std::shared_ptr<SpotEvent> spotPtr, unsigned int tid)
{
    return userChart.acceptTrackerEvent( spotPtr, tid, TrainCharacteristics::Source::Guess, 0);
}

void TC_Hem_aeSpanWork::createPathAndSpanWorkWithoutReturnOnStation()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent name='ExplicitForm' Bdg='ASOUP 1042[11443]' create_time='20180806T083000Z' index='1144-125-1100' num='8502' length='15' weight='455' restaggregates='Y' waynum='1' parknum='1'>"
        L"<Locomotive Series='ТЭМ2' NumLoc='1202' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='KUKO' Tim_Beg='2018-08-06 07:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        );
    AddEventToPath( firstEventPtr );
    AddEventToPath( CreateEvent( 
        L"<SpotEvent name='Departure' Bdg='1C[11443]' create_time='20180806T083857Z' waynum='1' parknum='1' />"
        ));
    AddEventToPath( CreateEvent( 
        L"<SpotEvent name='Span_move' Bdg='P14P[11443:11445]' create_time='20180806T084327Z' waynum='1'>"
        L"<rwcoord picketing1_val='379~618' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='379~200' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));



    std::wstring text = L"<SpanWork esr='11443:11445' waynum='1' duration='0' returnFlag='Y' num='8501' index='1111-111-1111'>"
        L"<picketing_start picketing1_val='376~500' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='382~600' picketing1_comm='Вентспилс' />"
        L"</SpanWork>";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    time_t t = (from_iso_string("20180806T083000Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::EXPLICIT_FORM, BadgeE(L"ASOUP 1042",EsrKit(11443)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( spotPtr );

}

void TC_Hem_aeSpanWork::WorkWithoutReturnError()
{
      createPathAndSpanWorkWithoutReturnOnStation();
      CPPUNIT_ASSERT(pathPtr);
      CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);
      auto& spanWorkInformator = *userChart.getSpanWorkInformator();
      CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
      spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
      CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);

      {
          CPPUNIT_ASSERT(AddEventToPath( CreateEvent( 
              L"<SpotEvent create_time='20180806T084738Z' name='Span_stopping_begin' Bdg='P14P[11443:11445]' waynum='1' intervalSec='251'>"
                 L"<rwcoord picketing1_val='379~618' picketing1_comm='Вентспилс' />"
                 L"<rwcoord picketing1_val='379~200' picketing1_comm='Вентспилс' />"
              L"</SpotEvent>"
              )));

          CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);
          CPPUNIT_ASSERT(pathPtr->GetLastEvent()->GetCode()==HCode::SPAN_STOPPING_BEGIN);
      }

      {
          //Разрыв нити
      CPPUNIT_ASSERT(AddEventToPath( CreateEvent( 
      L"<SpotEvent create_time='20180806T091407Z' name='Span_move' Bdg='P16P[11443:11445]' waynum='1'>"
      L"<rwcoord picketing1_val='378~900' picketing1_comm='Вентспилс' />"
      L"<rwcoord picketing1_val='379~200' picketing1_comm='Вентспилс' />"
      L"</SpotEvent>"
      )));
      
      CPPUNIT_ASSERT(pathPtr && pathPtr->GetEventsCount()==4);

      auto createdPathPtr = userChart.getHappenLayer().GetWriteAccess()->getPath(firstEventPtr);
      CPPUNIT_ASSERT(createdPathPtr && createdPathPtr->GetEventsCount()==5);
      CPPUNIT_ASSERT(createdPathPtr && createdPathPtr->GetLastEvent()->GetCode()==HCode::DEATH);

      ConstTrainDescrPtr pTrain = pathPtr->GetFragment(pathPtr->GetFirstEvent());
      CPPUNIT_ASSERT( pTrain && pTrain->GetNumber().getNumber() == 8501 && pTrain->GetIndex().str()==L"1111-111-1111" );
      CPPUNIT_ASSERT(!spanWorkInformator.getByPath(pathPtr).empty());
      CPPUNIT_ASSERT(!spanWorkInformator.getByPath(createdPathPtr).empty());
      }
}

void TC_Hem_aeSpanWork::TestShortZoneInsideOneStrip()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180912T102055Z' name='Form' Bdg='3C[11420]' waynum='1' parknum='1' optCode='11420:11760' />"
        );
    AddEventToPath( firstEventPtr );
    AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180912T104319Z' name='Transition' Bdg='1C[11420]' waynum='1' parknum='1' intervalSec='121' optCode='11420:11760' />"
        ));
    AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180912T105042Z' name='Span_move' Bdg='P25P[11420:11760]' waynum='1'>"
        L"<rwcoord picketing1_val='123~558' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='123~230' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        ));

    std::wstring text = L"<SpanWork esr='11420:11760' waynum='1' duration='0' workId='i1'>"
        L"<picketing_start picketing1_val='123~' picketing1_comm='Рига-Зилупе' />"
        L"<picketing_stop picketing1_val='123~100' picketing1_comm='Рига-Зилупе' />"
        L"</SpanWork>";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    time_t t = (from_iso_string("20180912T102055Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::FORM, BadgeE(L"3C",EsrKit(11420)), t);
        HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( spotPtr );

        CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);

    {
        CPPUNIT_ASSERT(AddEventToPath( CreateEvent( 
            L"<SpotEvent create_time='20180912T105104Z' name='Span_move' Bdg='P27P[11420:11760]' waynum='1'>"
            L"<rwcoord picketing1_val='123~230' picketing1_comm='Рига-Зилупе' />"
            L"<rwcoord picketing1_val='122~872' picketing1_comm='Рига-Зилупе' />"
            L"</SpotEvent>"
            )));

        CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);
        CPPUNIT_ASSERT(pathPtr->GetLastEvent()->GetCode()==HCode::SPAN_STOPPING_BEGIN);
            auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(11420,11760));
        CPPUNIT_ASSERT( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START);
    }

    {
        CPPUNIT_ASSERT(AddEventToPath( CreateEvent( 
            L"<SpotEvent create_time='20180912T105133Z' name='Span_move' Bdg='P29P[11420:11760]' waynum='1'>"
            L"<rwcoord picketing1_val='122~872' picketing1_comm='Рига-Зилупе' />"
            L"<rwcoord picketing1_val='122~514' picketing1_comm='Рига-Зилупе' />"
            L"</SpotEvent>"
            )));

        CPPUNIT_ASSERT(pathPtr->GetEventsCount()==5);
        CPPUNIT_ASSERT(pathPtr->GetLastEvent()->GetCode()==HCode::SPAN_MOVE);
        auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(11420,11760));
        CPPUNIT_ASSERT( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::WAIT_ARRIVAL);
    }


//         <SpotEvent create_time="20180912T105205Z" name="Span_move" Bdg="P31P[11420:11760]" waynum="1">"
//         <rwcoord picketing1_val="122~514" picketing1_comm="Рига-Зилупе" />"
//         <rwcoord picketing1_val="122~156" picketing1_comm="Рига-Зилупе" />"
//         </SpotEvent>"

}

void TC_Hem_aeSpanWork::TestReturnFromSpanWithStopWithoutSpanwork()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180914T054701Z' name='Form' Bdg='6C[09750]' index='0001-037-0978' num='V8601' motortroll='Y'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    CPPUNIT_ASSERT ( AddEventToPath( CreateEvent( L"<SpotEvent create_time='20180914T054854Z' name='Departure' Bdg='6C[09750]' waynum='6' parknum='1' />" )) );
    CPPUNIT_ASSERT ( AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180914T060359Z' name='Span_stopping_begin' Bdg='TZ4C[09750:09751]' waynum='1'>"
        L"<rwcoord picketing1_val='101~437' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='102~274' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        )));
    CPPUNIT_ASSERT ( AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180914T083012Z' name='Span_stopping_end' Bdg='TZ4C[09750:09751]' waynum='1'>"
        L"<rwcoord picketing1_val='101~437' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='102~274' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        )));

    CPPUNIT_ASSERT ( AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180914T083548Z' name='Span_move' Bdg='TZ11C[09750:09751]' waynum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        )));

    CPPUNIT_ASSERT ( AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180914T083645Z' name='Death' Bdg='TZ11C[09750:09751]' waynum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        )));

    std::wstring text = L"<SpanWork esr='09750:09751' waynum='1' duration='0' returnFlag='Y' num='8602' index='0001-001-0975' status='wait_departure' markId='i7' workId='i1'>"
        L"<picketing_start picketing1_val='98~800' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='98~900' picketing1_comm='Вентспилс' />"
        L"</SpanWork>";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    time_t t = (from_iso_string("20180914T054701Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::FORM, BadgeE(L"6C",EsrKit(9750)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( spotPtr );

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==6);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);

    //вторая нить - первую завершаем, вторую начинаем с Form, сообщая Hem предыдущий ID
    auto pEvent = CreateEvent(
        L"<SpotEvent create_time='20180914T083645Z' name='Form' Bdg='PVDP[09750]' index='' num='V8602' motortroll='Y'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        );
    userChart.acceptTrackerEvent( pEvent, 32, TrainCharacteristics::Source::Guess, 64);
    auto threePath = spanWorkInformator.getPathBySpanWork(pathPtr, *spanWorkPtr );
    CPPUNIT_ASSERT(std::get<0>(threePath) && std::get<1>(threePath));
    auto secondPath = std::get<0>(threePath)==pathPtr ? std::get<1>(threePath) : std::get<0>(threePath);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==6);
    CPPUNIT_ASSERT(secondPath->GetEventsCount()==5);
//     pEvent = CreateEvent( 
//             L"<SpotEvent create_time='20180914T083802Z' name='Arrival' Bdg='5C[09750]' waynum='5' parknum='1' />"
//             );
//     userChart.processTrackerEvent( pEvent, 32, TrainCharacteristics::Source::Guess, 0);

}

void TC_Hem_aeSpanWork::TestReturnFromSpanWithoutSpanwork()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180914T054701Z' name='Form' Bdg='6C[09750]' waynum='6' parknum='1' index='0001-037-0978' num='V8601' motortroll='Y'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    CPPUNIT_ASSERT ( AddEventToPath( CreateEvent( L"<SpotEvent create_time='20180914T054854Z' name='Departure' Bdg='6C[09750]' waynum='6' parknum='1' />" )) );

    CPPUNIT_ASSERT ( AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180914T083548Z' name='Span_move' Bdg='TZ11C[09750:09751]' waynum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        )));

    CPPUNIT_ASSERT ( AddEventToPath( CreateEvent( 
        L"<SpotEvent create_time='20180914T083645Z' name='Death' Bdg='TZ11C[09750:09751]' waynum='1'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        )));

    std::wstring text = L"<SpanWork esr='09750:09751' waynum='1' duration='0' returnFlag='Y' num='8602' index='0001-001-0975' status='wait_departure' markId='i7' workId='i1'>"
        L"<picketing_start picketing1_val='98~800' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='98~900' picketing1_comm='Вентспилс' />"
        L"</SpanWork>";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    time_t t = (from_iso_string("20180914T054701Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::FORM, BadgeE(L"6C",EsrKit(9750)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( spotPtr );

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==4);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);

    //вторая нить - первую завершаем, вторую начинаем с Form, сообщая Hem предыдущий ID
    auto pEvent = CreateEvent(
        L"<SpotEvent create_time='20180914T083645Z' name='Form' Bdg='PVDP[09750]' index='' num='V8602' motortroll='Y'>"
        L"<rwcoord picketing1_val='107~336' picketing1_comm='Вентспилс' />"
        L"<Locomotive Series='ПPCM-4' NumLoc='21' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='SOKOVENINS' Tim_Beg='2018-09-14 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        );
    userChart.acceptTrackerEvent( pEvent, 32, TrainCharacteristics::Source::Guess, 64);
    auto threePath = spanWorkInformator.getPathBySpanWork(pathPtr, *spanWorkPtr );
    CPPUNIT_ASSERT(std::get<0>(threePath) && std::get<1>(threePath));
    auto secondPath = std::get<0>(threePath)==pathPtr ? std::get<1>(threePath) : std::get<0>(threePath);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==4);
    CPPUNIT_ASSERT(secondPath->GetEventsCount()==5);
    //     pEvent = CreateEvent( 
    //             L"<SpotEvent create_time='20180914T083802Z' name='Arrival' Bdg='5C[09750]' waynum='5' parknum='1' />"
    //             );
    //     userChart.processTrackerEvent( pEvent, 32, TrainCharacteristics::Source::Guess, 0);

}

void TC_Hem_aeSpanWork::TestStatusSpanworkAfterDisform()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180914T053941Z' name='Form' Bdg='6C[09750]' waynum='6' parknum='1' index='' num='V8601' motortroll='Y' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );


    std::wstring text = L"<SpanWork esr='09750:09751' waynum='1' duration='0' returnFlag='Y' num='8602' index='0001-001-0975' status='wait_departure' workId='i1'>"
                            L"<picketing_start picketing1_val='98~800' picketing1_comm='Вентспилс' />"
                            L"<picketing_stop picketing1_val='98~900' picketing1_comm='Вентспилс' />"
                        L"</SpanWork>";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    time_t t = (from_iso_string("20180914T053941Z") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::FORM, BadgeE(L"6C",EsrKit(9750)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( spotPtr );

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==1);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, spanWorkPtr->GetESR());
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::WAIT_DEPARTURE );

    CPPUNIT_ASSERT ( AddEventToPath( 
        CreateEvent(L"<SpotEvent create_time='20180914T054654Z' name='Disform' Bdg='6C[09750]' waynum='6' parknum='1' />")
        ));
    CPPUNIT_ASSERT ( pSpanWork->GetStatus()==SpanWork::ActivityStatus::WAIT_DEPARTURE );

}

void TC_Hem_aeSpanWork::InputSpanworkLaterAndTearPath()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180912T085751Z' name='Form' Bdg='5BC[09380]' waynum='5' parknum='1' index='0001-522-0937' num='8612' motortroll='Y'>"
        L"<Locomotive Series='АДМ' NumLoc='640' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='LUCKOVS' Tim_Beg='2018-09-12 08:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T085751Z' name='Departure' Bdg='5BC[09380]' waynum='5' parknum='1' optCode='09370:09380' />" 
        ));

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T090514Z' name='Span_stopping_begin' Bdg='P14P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='78~400' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='78~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        ));

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T093742Z' name='Span_stopping_end' Bdg='P14P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='78~400' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='78~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T093925Z' name='Span_stopping_begin' Bdg='P8P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='79~200' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='79~600' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T101618Z' name='Span_stopping_end' Bdg='P8P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='79~200' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='79~600' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T101618Z' name='Span_stopping_begin' Bdg='P6P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='80~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='79~600' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T105255Z' name='Span_stopping_end' Bdg='P6P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='80~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='79~600' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        ));
    std::wstring text = L"<SpanWork esr='09370:09380' waynum='2' duration='0' returnFlag='Y' num='8613' index='0001-001-0975' status='start' workId='i1' />";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    time_t t = (from_iso_string("20180912T085751") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::FORM, BadgeE(L"5BC",EsrKit(9380)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( spotPtr );

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==8);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, spanWorkPtr->GetESR());
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START );

    CPPUNIT_ASSERT ( AddEventToPath( 
        CreateEvent(
        L"<SpotEvent create_time='20180912T105545Z' name='Span_move' Bdg='P16P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='78~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='77~600' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        )));
    CPPUNIT_ASSERT ( pSpanWork->GetStatus()==SpanWork::ActivityStatus::INRETURN );
    auto threePath = spanWorkInformator.getPathBySpanWork(pathPtr, *spanWorkPtr );
    CPPUNIT_ASSERT(std::get<0>(threePath) && std::get<1>(threePath));
}


void TC_Hem_aeSpanWork::ErrorWaynumInSpanwork()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180912T085751Z' name='Form' Bdg='5BC[09380]' waynum='5' parknum='1' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T085751Z' name='Departure' Bdg='5BC[09380]' waynum='5' parknum='1' optCode='09370:09380' />"
        ));

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180912T090053Z' name='Span_move' Bdg='N8P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='75~200' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='74~700' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        ));

    std::wstring text = 
        L"<SpanWork esr='09370:09380' waynum='1' duration='0' returnFlag='Y' num='8613' index='0001-001-0875' workId='i1'>"
        L"<picketing_start picketing1_val='75~400' picketing1_comm='Рига-Зилупе' />"
        L"<picketing_stop picketing1_val='75~500' picketing1_comm='Рига-Зилупе' />"
        L"</SpanWork>";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    time_t t = (from_iso_string("20180912T085751") - from_time_t(0)).total_seconds();
    auto spotPtr = std::make_shared<SpotEvent>(HCode::FORM, BadgeE(L"5BC",EsrKit(9380)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( spotPtr );

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, spanWorkPtr->GetESR());
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::GO_TO_WORK );

    CPPUNIT_ASSERT ( AddEventToPath( 
        CreateEvent(
        L"<SpotEvent create_time='20180912T090111Z' name='Span_move' Bdg='N10P_SK[09370:09380]' waynum='2'>"
        L"<rwcoord picketing1_val='75~600' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='75~200' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        )));
    CPPUNIT_ASSERT ( pSpanWork->GetStatus()==SpanWork::ActivityStatus::GO_TO_WORK );
    CPPUNIT_ASSERT ( pathPtr->GetEventsCount()==3 && pathPtr->GetLastEvent()->GetTime()==(from_iso_string("20180912T090111") - from_time_t(0)).total_seconds() );
}


void TC_Hem_aeSpanWork::EnterSpanWorkAlivePathOnWorkSpan()
{
   firstEventPtr = CreateEvent(
       L"<SpotEvent create_time='20180914T054701Z' name='Form' Bdg='PVDP[09750]' index='' num='V8601' motortroll='Y' />"
       );
   CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

   AddEventToPath( CreateEvent(
       L"<SpotEvent create_time='20180914T054836Z' name='Span_move' Bdg='TZ10C[09750:09751]' waynum='1'>"
       L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
       L"<rwcoord picketing1_val='105~977' picketing1_comm='Вентспилс' />"
       L"</SpotEvent>"
       ));

   HappenLayer& hl = userChart.getHappenLayer();
   pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );

   CPPUNIT_ASSERT(pathPtr);
   CPPUNIT_ASSERT(pathPtr->GetEventsCount()==2);

   auto& spanWorkInformator = *userChart.getSpanWorkInformator();
   CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());

   Hem::Bill bill = createBill(
       L"<A2F_HINT Hint='cmd_edit'>"
       L"<PrePoint>"
       L"<EventPoint name='Form' Bdg='PVDP[09750]' create_time='20180914T054701Z' index='' num='V8601' motortroll='Y' />"
       L"<EventPoint name='Span_move' Bdg='TZ10C[09750:09751]' create_time='20180914T054836Z' waynum='1'>"
       L"<rwcoord picketing1_val='106~303' picketing1_comm='Вентспилс' />"
       L"</EventPoint>"
       L"</PrePoint>"
       L"<Action code='SpanWork' District='09180-09820' DistrictRight='Y' />"
       L"<PostPoint esr='09750:09751' waynum='1' start='20181109T161630Z' stop='20181109T161630Z' remove=''>"
       L"<picketing_start picketing1_val='105~' picketing1_comm='Вентспилс' />"
       L"<picketing_stop picketing1_val='107~100' picketing1_comm='Вентспилс' />"
       L"</PostPoint>"
       L"</A2F_HINT>"
       );

   time_t t = (from_iso_string("20180914T054837Z") - from_time_t(0)).total_seconds();
   userChart.carryOut( bill, t, GrifFunc() );
   userChart.getHappenLayer().TakeChanges( attic::a_node() );

   CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);
   auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(9750,9751));
   CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START );
   CPPUNIT_ASSERT(pathPtr->GetEventsCount()>=2);
   CPPUNIT_ASSERT(pathPtr->GetFirstEvent()->GetCode() == HCode::FORM );
   CPPUNIT_ASSERT(pathPtr->GetLastEvent()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
}

void TC_Hem_aeSpanWork::EnterSpanWorkDeathPathOnWorkSpan()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180914T054701Z' name='Form' Bdg='PVDP[09750]' index='' num='V8601' motortroll='Y' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180914T054836Z' name='Span_move' Bdg='TZ10C[09750:09751]' waynum='1'>"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='105~977' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180914T054836Z' name='Death' Bdg='TZ10C[09750:09751]' waynum='1'>"
        L"<rwcoord picketing1_val='106~630' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='105~977' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));

    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);

    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint>"
        L"<EventPoint name='Form' Bdg='PVDP[09750]' create_time='20180914T054701Z' index='' num='V8601' motortroll='Y' />"
        L"<EventPoint name='Span_move' Bdg='TZ10C[09750:09751]' create_time='20180914T054836Z' waynum='1'>"
        L"<rwcoord picketing1_val='106~303' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='09180-09820' DistrictRight='Y' />"
        L"<PostPoint esr='09750:09751' waynum='1' start='20181109T161630Z' stop='20181109T161630Z' remove=''>"
        L"<picketing_start picketing1_val='106~' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='106~100' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    time_t t = (from_iso_string("20180914T054837Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(9750, 9751));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START );
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==4 && pathPtr->GetPreviousEvent(*pathPtr->GetLastEvent())->GetCode()==HCode::SPAN_STOPPING_END);
}

void TC_Hem_aeSpanWork::EnterSpanWorkPostFactumStatStat()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20180914T051701Z' name='Departure' Bdg='PVDP[09751]' waynum='1' index='' num='V8601' motortroll='Y' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180914T054701Z' name='Arrival' Bdg='PVDP[09750]' waynum='1' index='' num='V8601' motortroll='Y' />"
        ));

    AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20180914T054701Z' name='Death' Bdg='PVDP[09750]' waynum='1' index='' num='V8601' motortroll='Y' />"
        ));

    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);

    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint>"
        L"<EventPoint name='Departure' Bdg='PVDP[09751]' create_time='20180914T051701Z' waynum='1' index='' num='V8601' motortroll='Y' />"
        L"<EventPoint name='Arrival' Bdg='PDVP[09751]' create_time='20180914T054701Z' waynum='1'/>"
        L"<EventPoint name='Death' Bdg='PDVP[09751]' create_time='20180914T054701Z' waynum='1'/>"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='09180-09820' DistrictRight='Y' />"
        L"<PostPoint esr='09750:09751' waynum='1' start='20181109T161630Z' stop='20181109T161630Z' remove=''>"
        L"<picketing_start picketing1_val='102~' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='102~100' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    time_t t = (from_iso_string("20180914T054701Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(9750, 9751));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::STOP );
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==5 && pathPtr->GetNextEvent(*firstEventPtr)->GetCode()==HCode::SPAN_STOPPING_BEGIN);
}

void TC_Hem_aeSpanWork::CreatePathAndSpanWorkOnTheSameSpan()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent name='Form' Bdg='?p1w1[09220]' create_time='20181112T081800Z' waynum='1' parknum='1' index='0920-123-0922' num='8790' workselftransp='Y' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr, 32 ) );

    AddEventToPath( CreateEvent(L"<SpotEvent name='Departure' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"), 32);

    AddEventToPath( CreateEvent(L"<SpotEvent name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"), 32);

    HappenLayer& hl = userChart.getHappenLayer();
    auto pPath = hl.GetWriteAccess()->getPath( firstEventPtr );

    CPPUNIT_ASSERT(pPath);
    CPPUNIT_ASSERT(pPath->GetEventsCount()==3);

    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pPath).empty());

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint>"
        L"<EventPoint name='Form' Bdg='?p1w1[09220]' create_time='20181112T081800Z' index='0920-123-0922' num='8790' workselftransp='Y' waynum='1' parknum='1' />"
        L"<EventPoint name='Departure' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"<EventPoint name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint esr='09211:09220' waynum='1' start='20181112T094102Z' stop='20181112T094102Z' returnFlag='Y' num='8791' index='0922-012-0920' remove=''>"
        L"<picketing_start picketing1_val='219~' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='219~100' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    time_t t = (from_iso_string("20181112T101810Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pPath).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pPath, EsrKit(9211, 9220));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::WAIT_DEPARTURE );

    bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"<Action code='PushBack' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint name='Span_move' Bdg='?[09211:09220]' create_time='20181112T102900Z'>"
        L"<rwcoord picketing1_val='221~371' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09220' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    t = (from_iso_string("20181112T101900Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    pPath = hl.GetWriteAccess()->getPath( firstEventPtr );
    CPPUNIT_ASSERT(pPath->GetEventsCount()==5 && pPath->GetPreviousEvent(*pPath->GetLastEvent())->GetCode()==HCode::SPAN_STOPPING_END);
    pSpanWork = spanWorkInformator.getByPathEsr(pPath, EsrKit(9211, 9220));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START );
    //TRACE ( pPath->GetString().c_str() );

    bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='spanwork[09211:09220]' create_time='20181112T103900Z' waynum='1'>"
             L"<rwcoord picketing1_val='219~50' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='PushBack' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint name='Arrival' Bdg='?p1w1[09220]' create_time='20181112T112900Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    t = (from_iso_string("20181112T113000Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );
    pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(9211, 9220));

    auto threePath = spanWorkInformator.getPathBySpanWork(pathPtr, *pSpanWork );
    CPPUNIT_ASSERT(std::get<0>(threePath) && std::get<1>(threePath));
}

void TC_Hem_aeSpanWork::TwoSpanWorkWithReturnOnTheSamePlace()
{
    CreatePathAndSpanWorkOnTheSameSpan();
    firstEventPtr = CreateEvent(
        L"<SpotEvent name='Form' Bdg='?p1w3[09220]' create_time='20181112T094900Z' waynum='3' parknum='1' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Departure' Bdg='?p1w3[09220]' create_time='20181112T101800Z' waynum='3' parknum='1' />"
        ));

    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Death' Bdg='?p1w3[09220]' create_time='20181112T101800Z' waynum='3' parknum='1' />"
        ));

    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );

    CPPUNIT_ASSERT(pathPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==3);

    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).empty());

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint>"
        L"<EventPoint name='Form' Bdg='?p1w3[09220]' create_time='20181112T094900Z' index='0982-123-0920' num='8792' workselftransp='Y' waynum='3' parknum='1' />"
        L"<EventPoint name='Departure' Bdg='?p1w3[09220]' create_time='20181112T101800Z' waynum='3' parknum='1' />"
        L"<EventPoint name='Death' Bdg='?p1w3[09220]' create_time='20181112T101800Z' waynum='3' parknum='1' />"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint esr='09211:09220' waynum='1' start='20181112T094523Z' stop='20181112T094523Z' returnFlag='Y' num='8793' index='0922-097-0920' remove=''>"
        L"<picketing_start picketing1_val='221~' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='221~100' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    time_t t = (from_iso_string("20181112T101810Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(9211, 9220));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::WAIT_DEPARTURE );

    bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='?p1w3[09220]' create_time='20181112T101800Z' waynum='3' parknum='1' />"
        L"<Action code='PushBack' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint name='Span_move' Bdg='?[09211:09220]' create_time='20181112T102900Z'>"
        L"<rwcoord picketing1_val='226~535' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09220' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );
    
    t = (from_iso_string("20181112T101900Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    
    pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==5 && pathPtr->GetPreviousEvent(*pathPtr->GetLastEvent())->GetCode()==HCode::SPAN_STOPPING_END);

    pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(9211, 9220));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START );

    bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='spanwork[09211:09220]' create_time='20181112T103859Z'>"
                L"<rwcoord picketing1_val='226~535' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='PushBack' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint name='Arrival' Bdg='?p1w2[09220]' create_time='20181112T112900Z' waynum='2' parknum='1' />"
        L"<EsrList>"
        L"<Station esr='09220' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    t = (from_iso_string("20181112T113000Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );
    pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(9211, 9220));

    auto threePath = spanWorkInformator.getPathBySpanWork(pathPtr, *pSpanWork );
    CPPUNIT_ASSERT(std::get<0>(threePath) && std::get<1>(threePath));
}

void TC_Hem_aeSpanWork::PaintingSpanMoveAndDepartureTheSameTime()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent name='Form' Bdg='?p1w1[09220]' create_time='20181112T081800Z' waynum='1' parknum='1' index='0920-123-0922' num='8790' workselftransp='Y' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    AddEventToPath( CreateEvent(L"<SpotEvent name='Departure' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"));

    AddEventToPath( CreateEvent(L"<SpotEvent name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"));

    HappenLayer& hl = userChart.getHappenLayer();
    auto pPath = hl.GetWriteAccess()->getPath( firstEventPtr );

    CPPUNIT_ASSERT(pPath);
    CPPUNIT_ASSERT(pPath->GetEventsCount()==3);

    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pPath).empty());

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint>"
        L"<EventPoint name='Form' Bdg='?p1w1[09220]' create_time='20181112T081800Z' index='0920-123-0922' num='8790' workselftransp='Y' waynum='1' parknum='1' />"
        L"<EventPoint name='Departure' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"<EventPoint name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint esr='09211:09220' waynum='1' start='20181112T094102Z' stop='20181112T094102Z' returnFlag='Y' num='8791' index='0922-012-0920' remove=''>"
        L"<picketing_start picketing1_val='219~' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='219~100' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    time_t t = (from_iso_string("20181112T101810Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pPath).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pPath, EsrKit(9211, 9220));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::WAIT_DEPARTURE );

    bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"<Action code='PushBack' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint name='Span_move' Bdg='?[09211:09220]' create_time='20181112T101500Z'>"
        L"<rwcoord picketing1_val='221~371' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09220' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    t = (from_iso_string("20181112T101900Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    pPath = hl.GetWriteAccess()->getPath( firstEventPtr );
    CPPUNIT_ASSERT(pPath->GetEventsCount()==5 && pPath->GetPreviousEvent(*pPath->GetLastEvent())->GetCode()==HCode::SPAN_STOPPING_END);
    pSpanWork = spanWorkInformator.getByPathEsr(pPath, EsrKit(9211, 9220));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START );
    TRACE ( pPath->GetString().c_str() );

}

void TC_Hem_aeSpanWork::TearPathPaintingOnSpanWithReturn()
{
    firstEventPtr = CreateEvent(
        L"<SpotEvent name='Form' Bdg='?p1w1[09220]' create_time='20181112T081800Z' waynum='1' parknum='1' index='0920-123-0922' num='8790' workselftransp='Y' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr, 32 ) );

    AddEventToPath( CreateEvent(L"<SpotEvent name='Departure' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"), 32);

    AddEventToPath( CreateEvent(L"<SpotEvent name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"), 32);

    HappenLayer& hl = userChart.getHappenLayer();
    auto pPath = hl.GetWriteAccess()->getPath( firstEventPtr );

    CPPUNIT_ASSERT(pPath);
    CPPUNIT_ASSERT(pPath->GetEventsCount()==3);

    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pPath).empty());

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint>"
        L"<EventPoint name='Form' Bdg='?p1w1[09220]' create_time='20181112T081800Z' index='0920-123-0922' num='8790' workselftransp='Y' waynum='1' parknum='1' />"
        L"<EventPoint name='Departure' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"<EventPoint name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint esr='09211:09220' waynum='1' start='20181112T094102Z' stop='20181112T094102Z' returnFlag='' remove=''>"
        L"<picketing_start picketing1_val='219~' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='219~100' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    time_t t = (from_iso_string("20181112T101810Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pPath).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pPath, EsrKit(9211, 9220));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::WAIT_DEPARTURE );

    bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='?p1w1[09220]' create_time='20181112T101500Z' waynum='1' parknum='1' />"
        L"<Action code='PushBack' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint name='Span_move' Bdg='?[09211:09220]' create_time='20181112T102900Z'>"
        L"<rwcoord picketing1_val='221~371' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='09220' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    t = (from_iso_string("20181112T101900Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    pPath = hl.GetWriteAccess()->getPath( firstEventPtr );
    CPPUNIT_ASSERT(pPath->GetEventsCount()==5 && pPath->GetPreviousEvent(*pPath->GetLastEvent())->GetCode()==HCode::SPAN_STOPPING_END);
    pSpanWork = spanWorkInformator.getByPathEsr(pPath, EsrKit(9211, 9220));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START );
    TRACE ( pPath->GetString().c_str() );

    bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='spanwork[09211:09220]' create_time='20181112T103900Z' waynum='1'>"
        L"<rwcoord picketing1_val='219~50' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='PushBack' District='09180-11420' DistrictRight='Y' />"
        L"<PostPoint name='Arrival' Bdg='?p1w1[09220]' create_time='20181112T112900Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    t = (from_iso_string("20181112T113000Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    pathPtr = hl.GetWriteAccess()->getPath( firstEventPtr );
    pSpanWork = spanWorkInformator.getByPathEsr(pathPtr, EsrKit(9211, 9220));

    auto threePath = spanWorkInformator.getPathBySpanWork(pathPtr, *pSpanWork );
    CPPUNIT_ASSERT(std::get<0>(threePath) && std::get<1>(threePath));
}

void TC_Hem_aeSpanWork::TearPathWithReturnAfterAttach()
{
    //Первая нить
    firstEventPtr = CreateEvent(
        L"<SpotEvent name='Departure' Bdg='?p1w1[11431]' create_time='20181115T091052Z' waynum='1' parknum='1' index='0920-123-0922' num='8790' workselftransp='Y' />"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr ) );

    AddEventToPath( CreateEvent(
                L"<SpotEvent name='Span_stopping_begin' Bdg='P18P[11430:11431]' create_time='20181115T091451Z' waynum='1' >"
                L"<rwcoord picketing1_val='337~697' picketing1_comm='Вентспилс' />"
                L"</SpotEvent>"
                ));
    AddEventToPath( CreateEvent(
            L"<SpotEvent name='Span_stopping_end' Bdg='P18P[11430:11431]' create_time='20181115T092914Z' waynum='1' >"
            L"<rwcoord picketing1_val='337~697' picketing1_comm='Вентспилс' />"
            L"</SpotEvent>"
            ));

    AddEventToPath( CreateEvent(
            L"<SpotEvent name='Span_stopping_begin' Bdg='P14P[11430:11431]' create_time='20181115T092914Z' waynum='1'>"
            L"<rwcoord picketing1_val='338~209' picketing1_comm='Вентспилс' />"
            L"</SpotEvent>"
            ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Span_stopping_end' Bdg='P14P[11430:11431]' create_time='20181115T095604Z' waynum='1'>"
        L"<rwcoord picketing1_val='338~209' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Span_stopping_begin' Bdg='P10P[11430:11431]' create_time='20181115T095809Z' waynum='1'>"
        L"<rwcoord picketing1_val='338~713' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Span_stopping_end' Bdg='P10P[11430:11431]' create_time='20181115T101624Z' waynum='1'>"
        L"<rwcoord picketing1_val='338~713' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Span_stopping_begin' Bdg='P4P[11430:11431]' create_time='20181115T101804Z' waynum='1'>"
        L"<rwcoord picketing1_val='339~743' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Span_stopping_end' Bdg='P4P[11430:11431]' create_time='20181115T104539Z' waynum='1'>"
        L"<rwcoord picketing1_val='339~743' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Span_move' Bdg='P2P[11430:11431]' create_time='20181115T104539Z' waynum='1'>"
        L"<rwcoord picketing1_val='340~347' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));
    AddEventToPath( CreateEvent(
        L"<SpotEvent name='Death' Bdg='P2P[11430:11431]' create_time='20181115T104708Z' waynum='1'>"
        L"<rwcoord picketing1_val='338~209' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        ));


    //Вторая нить
    auto pSecondPathEvent = CreateEvent(
        L"<SpotEvent  name='Form' Bdg='PAP[11431]' create_time='20181115T104708Z' index='1143-221-1100' num='8252' length='3' weight='102' flat='Y'>"
        L"<Locomotive Series='DU08-32' NumLoc='3262' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='BURCEVS' Tim_Beg='2018-11-15 05:30' />"
        L"</Locomotive>"
        L"</SpotEvent>" );

    AddEventToPath( pSecondPathEvent, 32 );

    AddEventToPath( CreateEvent( L"<SpotEvent  name='Arrival' Bdg='3C[11431]' create_time='20181115T104752Z' waynum='3' parknum='1' />" ), 32 );

    AddEventToPath( CreateEvent(L"<SpotEvent name='Departure' Bdg='3C[11431]' create_time='20181115T111222Z' waynum='3' parknum='1' />"), 32 );

    AddEventToPath( CreateEvent( L"<SpotEvent  name='Transition' Bdg='1C[11432]' create_time='20181115T112141Z' waynum='1' parknum='1' intervalSec='55' />"), 32 );

    AddEventToPath( CreateEvent( L"<SpotEvent  name='Transition' Bdg='1C[11446]' create_time='20181115T112849Z' waynum='1' parknum='1' intervalSec='58'/>"), 32 );

    AddEventToPath( CreateEvent( L"<SpotEvent  name='Death' Bdg='1C[11446]' create_time='20181115T112849Z' waynum='1' parknum='1' intervalSec='58'/>"), 32 );

    HappenLayer& hl = userChart.getHappenLayer();
    auto pFirstPath = hl.GetWriteAccess()->getPath( firstEventPtr );

    CPPUNIT_ASSERT(pFirstPath);
    CPPUNIT_ASSERT(pFirstPath->GetEventsCount()==11);

    auto pSecondPath = hl.GetWriteAccess()->getPath( pSecondPathEvent );

    CPPUNIT_ASSERT(pSecondPath);
    CPPUNIT_ASSERT(pSecondPath->GetEventsCount()==6);

    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pFirstPath).empty());

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint>"
        L"<EventPoint name='Form' Bdg='PAP[11431]' create_time='20181115T091052Z' />"
        L"<EventPoint name='Span_stopping_begin' Bdg='P18P[11430:11431]' create_time='20181115T091451Z' waynum='1'>"
        L"<rwcoord picketing1_val='337~697' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Span_stopping_end' Bdg='P18P[11430:11431]' create_time='20181115T092914Z' waynum='1'>"
        L"<rwcoord picketing1_val='337~697' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Span_stopping_begin' Bdg='P14P[11430:11431]' create_time='20181115T092914Z' waynum='1'>"
        L"<rwcoord picketing1_val='338~209' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Span_stopping_end' Bdg='P14P[11430:11431]' create_time='20181115T095604Z' waynum='1'>"
        L"<rwcoord picketing1_val='338~209' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Span_stopping_begin' Bdg='P10P[11430:11431]' create_time='20181115T095809Z' waynum='1'>"
        L"<rwcoord picketing1_val='338~713' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Span_stopping_end' Bdg='P10P[11430:11431]' create_time='20181115T101624Z' waynum='1'>"
        L"<rwcoord picketing1_val='338~713' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Span_stopping_begin' Bdg='P4P[11430:11431]' create_time='20181115T101804Z' waynum='1'>"
        L"<rwcoord picketing1_val='339~743' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Span_stopping_end' Bdg='P4P[11430:11431]' create_time='20181115T104539Z' waynum='1'>"
        L"<rwcoord picketing1_val='339~743' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Span_move' Bdg='P2P[11430:11431]' create_time='20181115T104539Z' waynum='1'>"
        L"<rwcoord picketing1_val='340~347' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"<EventPoint name='Death' Bdg='P2P[11430:11431]' create_time='20181115T104708Z' waynum='1'>"
        L"<rwcoord picketing1_val='340~347' picketing1_comm='Вентспилс' />"
        L"</EventPoint>"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='11420-11000-11310' DistrictRight='Y' />"
        L"<PostPoint esr='11430:11431' waynum='1' start='20181116T095541Z' stop='20181116T095541Z' returnFlag='Y' num='8252' index='1143-221-1100' remove='' />"
        L"</A2F_HINT>"
        );

    time_t t = (from_iso_string("20181115T120000Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pFirstPath).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pFirstPath, EsrKit(11430, 11431));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::START );

    //Склейка
    bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Death' Bdg='P2P[11430:11431]' create_time='20181115T104708Z' waynum='1'>"
        L"<rwcoord picketing1_val='340~347' picketing1_comm='Вентспилс' />"
        L"</PrePoint>"
        L"<Action code='Attach' District='11420-11000-11310' DistrictRight='Y' />"
        L"<PostPoint name='Form' Bdg='PAP[11431]' create_time='20181115T104708Z' index='1143-221-1100' num='8252' length='3' weight='102' flat='Y'>"
        L"<Locomotive Series='DU08-32' NumLoc='3262' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='BURCEVS' Tim_Beg='2018-11-15 05:30' />"
        L"</Locomotive>"
        L"</PostPoint>"
        L"<EsrList>"
        L"<Station esr='11431' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    pFirstPath = hl.GetWriteAccess()->getPath( firstEventPtr );
    auto pCheckEvent = pFirstPath->GetPreviousEvent(*pFirstPath->GetLastEvent());
    CPPUNIT_ASSERT(pFirstPath->GetEventsCount()==4 && 
                pCheckEvent->GetCode()==HCode::SPAN_STOPPING_END &&
                pCheckEvent->GetBadge()==BadgeE(L"P18P", EsrKit(11430,11431)) );
    pSpanWork = spanWorkInformator.getByPathEsr(pFirstPath, EsrKit(11430, 11431));
    CPPUNIT_ASSERT ( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::STOP );

    auto threePath = spanWorkInformator.getPathBySpanWork(pathPtr, *pSpanWork );
    CPPUNIT_ASSERT(std::get<0>(threePath) && std::get<1>(threePath));
}


void TC_Hem_aeSpanWork::EditSpanWorkWithWrongKmPk()
{

        firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20190319T091902Z' name='Form' Bdg='2p[09150]' waynum='2' parknum='1' index='0001-306-0916' num='8217' stone='Y'/>"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr, 32 ) );

    AddEventToPath( CreateEvent(L"<SpotEvent create_time='20190319T092015Z' name='Departure' Bdg='2p[09150]' waynum='2' parknum='1' />"), 32);

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint>"
        L"<EventPoint create_time='20190319T091902Z' name='Form' Bdg='2p[09150]' waynum='2' parknum='1' index='0001-306-0916' num='8217' stone='Y' />"
        L"<EventPoint create_time='20190319T092015Z' name='Departure' Bdg='2p[09150]' waynum='2' parknum='1' />"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='09160-91262' DistrictRight='Y' />"
        L"<PostPoint esr='09160:09162' waynum='1' returnFlag=''>"
        L"<picketing_start picketing1_val='37~800' picketing1_comm='Вентспилс' />"
        L"<picketing_stop picketing1_val='37~900' picketing1_comm='Вентспилс' />"
        L"</PostPoint>"
        L"</A2F_HINT>"
        );

    time_t t = (from_iso_string("201910319T151810Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );

    AddEventToPath( CreateEvent(L"<SpotEvent create_time='20190319T093832Z' name='Arrival' Bdg='1p[09160]' waynum='1' parknum='1' />"), 32);
    AddEventToPath( CreateEvent(L"<SpotEvent create_time='20190319T095310Z' name='Departure' Bdg='1p[09160]' waynum='1' parknum='1' optCode='09160:09162' />"), 32);
    AddEventToPath( CreateEvent(L"<SpotEvent create_time='20190319T101716Z' name='Entra_stopping' Bdg='CAKPU1[09160:09162]' waynum='1' intervalSec='121'>"
                        L"<rwcoord picketing1_val='33~600' picketing1_comm='Торнянкалнс-Елгава' />"
                        L"<rwcoord picketing1_val='34~600' picketing1_comm='Торнянкалнс-Елгава' />"
                    L"</SpotEvent>"), 32);
    AddEventToPath( CreateEvent(L"<SpotEvent create_time='20190319T103732Z' name='Arrival' Bdg='4p[09160]' waynum='1' parknum='1' />"), 32);


    HappenLayer& hl = userChart.getHappenLayer();
    auto pPath = hl.GetWriteAccess()->getPath( firstEventPtr );
    CPPUNIT_ASSERT(pPath);
    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pPath).size()==1);
    auto pSpanWork = spanWorkInformator.getByPathEsr(pPath, EsrKit(9160,9162));
    CPPUNIT_ASSERT( pSpanWork && pSpanWork->GetStatus()==SpanWork::ActivityStatus::STOP);
    auto threePath = spanWorkInformator.getPathBySpanWork(pathPtr, *pSpanWork );
    CPPUNIT_ASSERT(std::get<0>(threePath) && std::get<1>(threePath));
}

griffin::Sender TC_Hem_aeSpanWork::GrifFunc()
{
    griffin::Sender gf = []( const griffin::Utility* ){};
    return gf;
}


void TC_Hem_aeSpanWork::WorkWithReturnOnWrongWay()
{
        
        firstEventPtr = CreateEvent(
        L"<SpotEvent create_time='20191209T222205Z' name='Form' Bdg='3p[09150]' waynum='3' parknum='1' index='0001-128-0910' num='8244' stone='Y'>"
        L"<Locomotive Series='USP2005' NumLoc='825' Consec='1' CarrierCode='5'>"
        L"<Crew EngineDriver='CELEJS' Tim_Beg='2019-12-10 00:20' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        );
    CPPUNIT_ASSERT( AddEventToPath( firstEventPtr, 32 ) );

    CPPUNIT_ASSERT(AddEventToPath( CreateEvent(L"<SpotEvent create_time='20191209T222205Z' name='Departure' Bdg='3p[09150]' waynum='3' parknum='1' />"), 32));

    CPPUNIT_ASSERT(AddEventToPath( CreateEvent(
        L"<SpotEvent create_time='20191209T222348Z' name='Wrong_way' Bdg='NAKPU1_O[09104:09150]' waynum='1'>"
        L"<rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='21~' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"), 32));


    CPPUNIT_ASSERT(AddEventToPath( CreateEvent(        
        L"<SpotEvent create_time='20191209T224758Z' name='Span_stopping_begin' Bdg='NAKPU2_O[09104:09150]' waynum='1' intervalSec='1441'>"
        L"<rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"), 32));

    pathPtr = userChart.getHappenLayer().GetWriteAccess()->getPath(firstEventPtr);
    CPPUNIT_ASSERT(pathPtr);

    std::wstring text = L"<SpanWork esr='09104:09150' waynum='1' start='20191209T222400Z' stop='20191209T222400Z' returnFlag='Y' num='8245' index='' status='wait_departure' workId='i58'>"
        L"<picketing_start picketing1_val='20~' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<picketing_stop picketing1_val='20~100' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpanWork>";

    attic::a_document doc;
    doc.load_utf8( ToUtf8(text) );

    SpanWork spanWork;
    spanWork.deserialize(doc.document_element());
    spanWorkPtr.reset( new SpanWork(spanWork) );

    auto& spanWorkInformator = *userChart.getSpanWorkInformator();
    spanWorkInformator.insert(pathPtr, *spanWorkPtr, nullptr);

    CPPUNIT_ASSERT(spanWorkInformator.getByPath(pathPtr).size()==1);


    CPPUNIT_ASSERT(AddEventToPath( CreateEvent(        
        L"<SpotEvent create_time='20191209T231757Z' name='Span_stopping_end' Bdg='NAKPU2_O[09104:09150]' waynum='1'>"
        L"<rwcoord picketing1_val='17~300' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='19~700' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"), 32));

    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==5);

    {
        //Разрыв нити
        time_t t = (from_iso_string("20191209T231757Z") - from_time_t(0)).total_seconds();

        auto spotPtr = std::make_shared<SpotEvent>(HCode::TRANSITION, BadgeE(L"3-15SP:5/7+,9/11+,15/17+",EsrKit(9150)), t);
        CPPUNIT_ASSERT(AddEventToPath(spotPtr, 32));

        auto createdPathPtr = userChart.getHappenLayer().GetWriteAccess()->getPath(firstEventPtr);
        CPPUNIT_ASSERT(createdPathPtr && createdPathPtr->GetEventsCount()==6);
        CPPUNIT_ASSERT(createdPathPtr && createdPathPtr->GetLastEvent()->GetCode()==HCode::DEATH);

        ConstTrainDescrPtr pTrain = pathPtr->GetFragment(pathPtr->GetFirstEvent());
        CPPUNIT_ASSERT( pTrain && pTrain->GetNumber().getNumber() == 8245 && pTrain->GetIndex().empty() );
    }
}
