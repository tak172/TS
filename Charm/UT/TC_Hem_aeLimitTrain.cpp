#include "stdafx.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "TC_Hem_aeLimitTrain.h"
#include "TC_Hem_ae_helper.h"
#include "../Hem/Bill.h"
#include "../Hem/UserChart.h"
#include "../Hem/Appliers/aeLimitTrain.h"
#include "../Hem/LimitTrainKeeper.h"
#include "../Hem/HappenPath.h"
#include "../helpful/Serialization.h"
#include "../Guess/SpotDetails.h"

using namespace std;
using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeLimitTrain );

void TC_Hem_aeLimitTrain::EditLimit()
{
    createPath();

    auto& limitTrainKeeper = *userChart.getLimitTrainKeeper();

    CPPUNIT_ASSERT(limitTrainKeeper.getLimits().empty());

    //Установка предупреждения
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Departure' Bdg='1C[22222]' create_time='20180726T195100Z' waynum='1' parknum='1' />"
        L"<Action code='Create' District='11111-77777' DistrictRight='Y'>"
        L"<LimitTrain esrFrom='22222' mainSpeed='60' comment='text' />"
        L"</Action>"
        L"</A2F_HINT>"
        );

    auto pLimit = deserialize<LimitTrain>("<LimitTrain esrFrom='22222' mainSpeed='60' comment='text' />");
    CPPUNIT_ASSERT(pLimit);

    time_t t = (from_iso_string("20180727T040000Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(limitTrainKeeper.getLimits().size()==1);

    auto fragmentPtr = pathPtr->GetFragment(limitEventPtr);
    CPPUNIT_ASSERT(fragmentPtr);
    const auto checkFeature = TrainCharacteristics::TrainFeature::LimitSpeed;
    std::set<TrainCharacteristics::TrainFeature> vFeatures;
    fragmentPtr->GetFeatures(vFeatures);
    CPPUNIT_ASSERT( size_t(1) == vFeatures.count(checkFeature) &&  
                    fragmentPtr->GetFeatureText(checkFeature)==pLimit->get_speed_wstr());

    //редактирование предупреждения
    bill = createBill(
    L"<A2F_HINT Hint='cmd_edit'>"
    L"  <PrePoint name='Departure' Bdg='1C[22222]' create_time='20180726T195100Z' index='0001-224-0915' num='8633'  motortroll='Y' waynum='1' parknum='1' >"
    L"      <LimitTrain esrFrom='22222' mainSpeed='60' comment='text' />"
    L"  </PrePoint>"
    L"  <Action code='Replace' District='11111-77777' DistrictRight='Y'>"
    L"      <LimitTrain esrFrom='22222' mainSpeed='55' sideSpeed='40' comment='comment' />"
    L"  </Action>"
    L"</A2F_HINT>");

    pLimit = deserialize<LimitTrain>("<LimitTrain esrFrom='22222' mainSpeed='55' sideSpeed='40' comment='comment' />");
    CPPUNIT_ASSERT(pLimit);

    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(limitTrainKeeper.getLimits().size()==1);

    fragmentPtr = pathPtr->GetFragment(limitEventPtr);
    CPPUNIT_ASSERT(fragmentPtr);
    fragmentPtr->GetFeatures(vFeatures);
    CPPUNIT_ASSERT( size_t(1) == vFeatures.count(checkFeature) &&  
        fragmentPtr->GetFeatureText(checkFeature)==pLimit->get_speed_wstr());


    //удаление предупреждения
    bill = createBill(
    L"<A2F_HINT Hint='cmd_edit'>"
    L"<PrePoint name='Departure' Bdg='1C[22222]' create_time='20180726T195100Z' index='0001-224-0915' num='8633'  motortroll='Y' waynum='1' parknum='1'>"
    L"<feat_texts limitspeed='55//40' />"
    L"</PrePoint>"
    L"<Action code='Remove' District='11111-77777' DistrictRight='Y'>"
    L"<LimitTrain esrFrom='22222' mainSpeed='55' sideSpeed='40' comment='comment' />"
    L"</Action>"
    L"</A2F_HINT>");

    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(limitTrainKeeper.getLimits().size()==0);

    fragmentPtr = pathPtr->GetFragment(limitEventPtr);
    CPPUNIT_ASSERT(fragmentPtr);
    fragmentPtr->GetFeatures(vFeatures);
    CPPUNIT_ASSERT( size_t(0) == vFeatures.count(checkFeature) &&  
        fragmentPtr->GetFeatureText(checkFeature)==L"");
}

bool TC_Hem_aeLimitTrain::AddEventToPath(const std::wstring& str)
{
    return userChart.acceptTrackerEvent( deserialize<SpotEvent>(ToUtf8(str)), 32, TrainCharacteristics::Source::Guess, 0);
}

void TC_Hem_aeLimitTrain::createPath()
{
    AddEventToPath( L"<SpotEvent create_time='20180726T194139Z' name='Form' Bdg='1C[11111]' waynum='1' parknum='1' index='0001-224-0915' num='8633' motortroll='Y'/>" );
    AddEventToPath( L"<SpotEvent create_time='20180726T194301Z' name='Departure' Bdg='1C[11111]' waynum='1' parknum='1' />");
    AddEventToPath( L"<SpotEvent create_time='20180726T194901Z' name='Arrival' Bdg='1C[22222]' waynum='1' parknum='1' />");
    AddEventToPath( L"<SpotEvent create_time='20180726T195100Z' name='Departure' Bdg='1C[22222]' waynum='1' parknum='1' />");
    AddEventToPath( L"<SpotEvent create_time='20180726T200451Z' name='Arrival' Bdg='1C[33333]' waynum='1' parknum='1' />");
    AddEventToPath( L"<SpotEvent create_time='20180726T210428Z' name='Departure' Bdg='1C[33333]' waynum='1' parknum='1' />");

    time_t t = (from_iso_string("20180726T195100Z") - from_time_t(0)).total_seconds();
    limitEventPtr = std::make_shared<const SpotEvent>(HCode::DEPARTURE, BadgeE(L"1C",EsrKit(22222)), t);
    HappenLayer& hl = userChart.getHappenLayer();
    pathPtr = hl.GetWriteAccess()->getPath( limitEventPtr );

    CPPUNIT_ASSERT(pathPtr && limitEventPtr);
    CPPUNIT_ASSERT(pathPtr->GetEventsCount()==6);
}

void TC_Hem_aeLimitTrain::setUp()
{
    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<DistrictList>"
            L"<District code='11111-77777' name='xxx'>"
            L"<Involve esr='11111'/>"
            L"<Involve esr='22222'/>"
            L"<Involve esr='33333'/>"
            L"<Involve esr='44444'/>"
            L"<Involve esr='55555'/>"
            L"<Involve esr='66666'/>"
            L"<Involve esr='77777'/>"
            L"</District>"
            L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }
    userChart.cachingDistrictGuide(dg);
}

griffin::Sender TC_Hem_aeLimitTrain::GrifFunc()
{
    griffin::Sender gf = []( const griffin::Utility* ){};
    return gf;
}

void TC_Hem_aeLimitTrain::SaveLimitOnLastEventAfterSpanMove()
{
    createPath();
    limitEventPtr = pathPtr->GetLastEvent();

    auto fragmentPtr = pathPtr->GetFirstFragment();
    CPPUNIT_ASSERT(fragmentPtr);
    const auto checkFeature = TrainCharacteristics::TrainFeature::LimitSpeed;
    std::set<TrainCharacteristics::TrainFeature> vFeatures;
    fragmentPtr->GetFeatures(vFeatures);
    CPPUNIT_ASSERT( size_t(0) == vFeatures.count(checkFeature) && fragmentPtr->GetFeatureText(checkFeature)==L"");

    auto& limitTrainKeeper = *userChart.getLimitTrainKeeper();

    CPPUNIT_ASSERT(limitTrainKeeper.getLimits().empty());

    //Установка предупреждения на последнее событие
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<PrePoint name='Departure' Bdg='1C[33333]' create_time='20180726T210428Z' waynum='1' parknum='1' />"
        L"<Action code='Create' District='11111-77777' DistrictRight='Y'>"
        L"<LimitTrain esrFrom='33333' mainSpeed='60' comment='text' />"
        L"</Action>"
        L"</A2F_HINT>"
        );

    auto pLimit = deserialize<LimitTrain>("<LimitTrain esrFrom='33333' mainSpeed='60' comment='text' />");
    CPPUNIT_ASSERT(pLimit);

    time_t t = (from_iso_string("20180727T040000Z") - from_time_t(0)).total_seconds();
    userChart.carryOut( bill, t, GrifFunc() );
    userChart.getHappenLayer().TakeChanges( attic::a_node() );
    CPPUNIT_ASSERT(limitTrainKeeper.getLimits().size()==1);

    fragmentPtr = pathPtr->GetFragment(limitEventPtr);
    CPPUNIT_ASSERT(fragmentPtr);
    fragmentPtr->GetFeatures(vFeatures);
    CPPUNIT_ASSERT( size_t(1) == vFeatures.count(checkFeature) &&  
        fragmentPtr->GetFeatureText(checkFeature)==pLimit->get_speed_wstr());

    CPPUNIT_ASSERT(AddEventToPath( L"<SpotEvent create_time='20180726T210910Z' name='Span_move' Bdg='?[33333:44444]' waynum='1' />"));
    CPPUNIT_ASSERT( pathPtr->GetEventsCount()==7 );
    CPPUNIT_ASSERT(AddEventToPath( L"<SpotEvent create_time='20180726T211140Z' name='Span_move' Bdg='xx[33333:44444]' waynum='1' />"));
    CPPUNIT_ASSERT( pathPtr->GetEventsCount()==7 );
    fragmentPtr = pathPtr->GetFragment(limitEventPtr);
    CPPUNIT_ASSERT(fragmentPtr);
    fragmentPtr->GetFeatures(vFeatures);
    CPPUNIT_ASSERT( size_t(1) == vFeatures.count(checkFeature) &&  
        fragmentPtr->GetFeatureText(checkFeature)==pLimit->get_speed_wstr());
    
    fragmentPtr = pathPtr->GetFirstFragment();
    CPPUNIT_ASSERT(fragmentPtr);
    fragmentPtr->GetFeatures(vFeatures);
    CPPUNIT_ASSERT( size_t(0) == vFeatures.count(checkFeature) && fragmentPtr->GetFeatureText(checkFeature)==L"");
}


void TC_Hem_aeLimitTrain::ResetFirstLimitInPair()
{
    auto& limitTrainKeeper = *userChart.getLimitTrainKeeper();

       AddEventToPath( L"<SpotEvent create_time='20190117T080902Z' name='Form' Bdg='3C[11111]' waynum='3' parknum='1' index='' num='D2830' through='Y' />" );
       AddEventToPath( L"<SpotEvent create_time='20190117T080936Z' name='Departure' Bdg='3C[11111]' waynum='3' parknum='1' optCode='11060:11062' />" );
       AddEventToPath( L"<SpotEvent create_time='20190117T082432Z' name='Arrival' Bdg='3C[22222]' waynum='3' parknum='1' />" );
       AddEventToPath( L"<SpotEvent create_time='20190117T083043Z' name='Departure' Bdg='3C[22222]' waynum='3' parknum='1' optCode='11051:11060' />" );
       AddEventToPath( L"<SpotEvent create_time='20190117T083733Z' name='Transition' Bdg='1C[33333]' waynum='1' parknum='1' optCode='11050:11051' index='' num='D2830' through='Y'/>" );
       AddEventToPath( L"<SpotEvent create_time='20190117T084730Z' name='Transition' Bdg='1C[44444]' waynum='1' parknum='1' optCode='11050:11052' />" );
       AddEventToPath( L"<SpotEvent create_time='20190117T085935Z' name='Transition' Bdg='1C[55555]' waynum='1' parknum='1' optCode='11041:11052' index='' num='D2830' through='Y'>"
        L"<feat_texts limitspeed='70' />"
        L"</SpotEvent>" );
       AddEventToPath( L"<SpotEvent create_time='20190117T090422Z' name='Transition' Bdg='1AC[66666]' waynum='1' parknum='1' />" );
       AddEventToPath( L"<SpotEvent create_time='20190117T091017Z' name='Transition' Bdg='NI5C[77777]' waynum='1' parknum='1' />" );

       //Установка первого предупреждения
       Hem::Bill bill = createBill(
           L"<A2F_HINT Hint='cmd_edit'>"
           L"<PrePoint name='Transition' Bdg='1C[33333]' create_time='20190117T083733Z' waynum='1' parknum='1' />"
           L"<Action code='Create' District='11111-77777' DistrictRight='Y'>"
           L"<LimitTrain esrFrom='33333' mainSpeed='50' />"
           L"</Action>"
           L"</A2F_HINT>"
           );

       time_t t = (from_iso_string("20190117T091017Z") - from_time_t(0)).total_seconds();
       userChart.carryOut( bill, t, GrifFunc() );
       userChart.getHappenLayer().TakeChanges( attic::a_node() );

       auto pLimit = deserialize<LimitTrain>("<LimitTrain esrFrom='33333' mainSpeed='50' />");
       t = (from_iso_string("20190117T083733Z") - from_time_t(0)).total_seconds();
       limitEventPtr = std::make_shared<const SpotEvent>(HCode::TRANSITION, BadgeE(L"1C",EsrKit(33333)), t);

       //Установка второго предупреждения
       bill = createBill(
           L"<A2F_HINT Hint='cmd_edit'>"
           L"<PrePoint name='Transition' Bdg='1C[55555]' create_time='20190117T085935Z' waynum='1' parknum='1' />"
           L"<Action code='Create' District='11111-77777' DistrictRight='Y'>"
           L"<LimitTrain esrFrom='55555' mainSpeed='70' />"
           L"</Action>"
           L"</A2F_HINT>"
           );

       t = (from_iso_string("20190117T091018Z") - from_time_t(0)).total_seconds();
       userChart.carryOut( bill, t, GrifFunc() );
       userChart.getHappenLayer().TakeChanges( attic::a_node() );
       CPPUNIT_ASSERT(limitTrainKeeper.getLimits().size()==1 && limitTrainKeeper.getLimits().begin()->second.size()==2);


       HappenLayer& hl = userChart.getHappenLayer();
       pathPtr = hl.GetWriteAccess()->getPath( limitEventPtr );

       CPPUNIT_ASSERT( limitEventPtr && pathPtr );

       const auto checkFeature = TrainCharacteristics::TrainFeature::LimitSpeed;
       std::set<TrainCharacteristics::TrainFeature> vFeatures;

       auto fragmentPtr = pathPtr->GetFragment(limitEventPtr);
       CPPUNIT_ASSERT(fragmentPtr);
       fragmentPtr = pathPtr->GetFragment(limitEventPtr);
       CPPUNIT_ASSERT(fragmentPtr);
       fragmentPtr->GetFeatures(vFeatures);
       CPPUNIT_ASSERT( size_t(1) == vFeatures.count(checkFeature) &&  
           fragmentPtr->GetFeatureText(checkFeature)==pLimit->get_speed_wstr());

       t = (from_iso_string("20190117T085935Z") - from_time_t(0)).total_seconds();
       auto eventPtr = std::make_shared<const SpotEvent>(HCode::TRANSITION, BadgeE(L"1C",EsrKit(55555)), t);
       auto pLimitSecond = deserialize<LimitTrain>("<LimitTrain esrFrom='55555' mainSpeed='70' />");
       fragmentPtr = pathPtr->GetFragment(eventPtr);
       CPPUNIT_ASSERT(fragmentPtr);
       fragmentPtr->GetFeatures(vFeatures);
       CPPUNIT_ASSERT( size_t(1) == vFeatures.count(checkFeature) &&  
           fragmentPtr->GetFeatureText(checkFeature)==pLimitSecond->get_speed_wstr());

       //удаление предупреждения
       bill = createBill(
           L"<A2F_HINT Hint='cmd_edit'>"
           L"<PrePoint name='Transition' Bdg='1C[33333]' create_time='20190117T083733ZZ' index='' num='D2830'  through='Y' waynum='1' parknum='1'>"
           L"<feat_texts limitspeed='50' />"
           L"</PrePoint>"
           L"<Action code='Remove' District='11111-77777' DistrictRight='Y'>"
           L"<LimitTrain esrFrom='33333' mainSpeed='50' />"
           L"</Action>"
           L"</A2F_HINT>");

       userChart.carryOut( bill, t, GrifFunc() );
       userChart.getHappenLayer().TakeChanges( attic::a_node() );
       CPPUNIT_ASSERT(limitTrainKeeper.getLimits().size()==1 && limitTrainKeeper.getLimits().begin()->second.size()==1);
       
       int i=0;
       for ( auto evPtr = limitEventPtr; evPtr && i<2; evPtr=pathPtr->GetNextEvent(*evPtr), i++ )
       {
       fragmentPtr = pathPtr->GetFragment(limitEventPtr);
       CPPUNIT_ASSERT(fragmentPtr);
       fragmentPtr->GetFeatures(vFeatures);
       CPPUNIT_ASSERT( size_t(0) == vFeatures.count(checkFeature) &&  
           fragmentPtr->GetFeatureText(checkFeature)==L"");
       }

       fragmentPtr = pathPtr->GetFragment(eventPtr);
       CPPUNIT_ASSERT(fragmentPtr);
       fragmentPtr = pathPtr->GetFragment(eventPtr);
       CPPUNIT_ASSERT(fragmentPtr);
       fragmentPtr->GetFeatures(vFeatures);
       CPPUNIT_ASSERT( size_t(1) == vFeatures.count(checkFeature) &&  
           fragmentPtr->GetFeatureText(checkFeature)==pLimitSecond->get_speed_wstr());
}
