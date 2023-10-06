#include "stdafx.h"

#include "TC_Hem_aeFixFuture.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "../Hem/Bill.h"
#include "../Hem/UserChart.h"
#include "../Hem/Appliers/aeFixFuture.h"
#include "../helpful/DistrictGuide.h"

using namespace std;
using namespace Hem::ApplierCarryOut;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeFixFuture );

void TC_Hem_aeFixFuture::FixFutureFragment()
{
    UtLayer<FutureLayer> future;
    {
        attic::a_document doc;
    doc.load_wide(
        L"<Test>"
        L"<FutureLayer>"
        L"<HemPath>"
        L"  <SpotEvent create_time='20180515T113734Z' name='Departure' Bdg='¹2582[11720]' index='' num='2582' through='Y' />"
        L"  <SpotEvent create_time='20180515T113734Z' name='Transition' Bdg='¹2582[11760]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180515T115932Z' name='Transition' Bdg='¹2582[09360]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180515T121352Z' name='Transition' Bdg='¹2582[09370]' waynum='1' parknum='1' />"
        L"  <SpotEvent create_time='20180515T122707Z' name='Arrival' Bdg='¹2582[09380]' waynum='3' parknum='1' />"
        L"  <SpotEvent create_time='20180515T124000Z' name='Disform' Bdg='¹2582[09380]' waynum='3' parknum='1' />"
        L"</HemPath>"
        L"</FutureLayer>"
        L"</Test>"
        );
    future.Deserialize( doc.document_element() );
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180516T025959Z'>"
        L"<PrePoint layer='future' name='Transition' Bdg='¹2582[11760]' create_time='20180515T113734Z' waynum='1' parknum='1' />"
        L"<Action code='Fix' District='09000-11760' DistrictRight='Y' />"
        L"<PostPoint layer='future' name='Transition' Bdg='¹2582[09370]' create_time='20180515T121352Z' waynum='1' parknum='1' />"
        L"</A2F_HINT>"
        );

    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
    L"<DistrictList>"
        L"<District code='09000-11760' name='xxx'>"
        L"<Involve esr='11720'/>"
        L"<Involve esr='11760'/>"
        L"<Involve esr='09360'/>"
        L"<Involve esr='09370'/>"
        L"<Involve esr='09380'/>"
        L"<Involve esr='09400'/>"
        L"<Involve esr='09410'/>"
        L"<Involve esr='09420'/>"
        L"<Involve esr='09000:09420'/>"
        L"</District>"
    L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }

    CPPUNIT_ASSERT( future.path_count() == 1 );
    CPPUNIT_ASSERT( future.exist_series( "name", "         Departure    Transition        Transition      Transition         Arrival          Disform" ) );
    CPPUNIT_ASSERT( future.exist_series( "create_time", "20180515T113734Z 20180515T113734Z 20180515T115932Z  20180515T121352Z 20180515T122707Z  20180515T124000Z"   ) );
    CPPUNIT_ASSERT( future.exist_series( "sourcetype", "       -               -                  -               -                -                 -"   ) );
    CPPUNIT_ASSERT( future.exist_series( "num", "             2582             -                  -               -                -                 -"   ) );
    aeFixFuture fix( future.GetWriteAccess(), dg.get_district(bill.getChance().getDCode()), bill.getHeadFixableThrows(), bill.getTailFixableThrows(), !bill.getHeadFixableThrows().IsFixated() );
    fix.Action();
    CPPUNIT_ASSERT( future.path_count() == 1 );
    CPPUNIT_ASSERT( future.exist_series( "name", "         Departure    Transition        Transition          Transition         Arrival          Disform" ) );
    CPPUNIT_ASSERT( future.exist_series( "create_time", "20180515T113734Z 20180515T113734Z 20180515T115932Z  20180515T121352Z 20180515T122707Z  20180515T124000Z"   ) );
    CPPUNIT_ASSERT( future.exist_series( "sourcetype", "        -             fix                fix             fix               -                 -"   ) );
    CPPUNIT_ASSERT( future.exist_series( "num", "             2582             -                  -               -                -                 -"   ) );
}

void TC_Hem_aeFixFuture::UnfixFutureFragment()
{
    UtLayer<FutureLayer> future;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<Test>"
            L"<FutureLayer>"
            L"<HemPath>"
            L"  <SpotEvent create_time='20180515T113734Z' name='Departure' Bdg='¹2582[11720]' index='' num='2582' through='Y' />"
            L"  <SpotEvent create_time='20180515T113734Z' name='Transition' Bdg='¹2582[11760]' waynum='1' parknum='1' sourcetype='fix'/>"
            L"  <SpotEvent create_time='20180515T115932Z' name='Transition' Bdg='¹2582[09360]' waynum='1' parknum='1' sourcetype='fix'/>"
            L"  <SpotEvent create_time='20180515T121352Z' name='Transition' Bdg='¹2582[09370]' waynum='1' parknum='1' sourcetype='fix'/>"
            L"  <SpotEvent create_time='20180515T122707Z' name='Arrival' Bdg='¹2582[09380]' waynum='3' parknum='1' />"
            L"  <SpotEvent create_time='20180515T124000Z' name='Disform' Bdg='¹2582[09380]' waynum='3' parknum='1' />"
            L"</HemPath>"
            L"</FutureLayer>"
            L"</Test>"
            );
        future.Deserialize( doc.document_element() );
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180516T025959Z'>"
        L"<PrePoint layer='future' name='Transition' Bdg='¹2582[11760]' create_time='20180515T113734Z' waynum='1' parknum='1' sourcetype='fix'/>"
        L"<Action code='Fix' District='09000-11760' DistrictRight='Y'/>"
        L"<PostPoint layer='future' name='Transition' Bdg='¹2582[09370]' create_time='20180515T121352Z' waynum='1' parknum='1' sourcetype='fix'/>"
        L"</A2F_HINT>"
        );

    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<DistrictList>"
            L"<District code='09000-11760' name='xxx'>"
            L"<Involve esr='11720'/>"
            L"<Involve esr='11760'/>"
            L"<Involve esr='09360'/>"
            L"<Involve esr='09370'/>"
            L"<Involve esr='09380'/>"
            L"<Involve esr='09400'/>"
            L"<Involve esr='09410'/>"
            L"<Involve esr='09420'/>"
            L"<Involve esr='09000:09420'/>"
            L"</District>"
            L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }

    CPPUNIT_ASSERT( future.path_count() == 1 );
    CPPUNIT_ASSERT( future.exist_series( "name", "         Departure    Transition        Transition      Transition         Arrival          Disform" ) );
    CPPUNIT_ASSERT( future.exist_series( "create_time", "20180515T113734Z 20180515T113734Z 20180515T115932Z  20180515T121352Z 20180515T122707Z  20180515T124000Z"   ) );
    CPPUNIT_ASSERT( future.exist_series( "sourcetype", "        -             fix                fix             fix               -                 -"   ) );
    CPPUNIT_ASSERT( future.exist_series( "num", "             2582             -                  -               -                -                 -"   ) );

    aeFixFuture fix( future.GetWriteAccess(), dg.get_district(bill.getChance().getDCode()), bill.getHeadFixableThrows(), bill.getTailFixableThrows(), !bill.getHeadFixableThrows().IsFixated() );
    fix.Action();
    CPPUNIT_ASSERT( future.path_count() == 1 );
    CPPUNIT_ASSERT( future.exist_series( "name", "         Departure    Transition        Transition      Transition         Arrival          Disform" ) );
    CPPUNIT_ASSERT( future.exist_series( "create_time", "20180515T113734Z 20180515T113734Z 20180515T115932Z  20180515T121352Z 20180515T122707Z  20180515T124000Z"   ) );
    CPPUNIT_ASSERT( future.exist_series( "sourcetype", "       -               -                  -              -               -                 -"   ) );
    CPPUNIT_ASSERT( future.exist_series( "num", "             2582             -                  -              -               -                 -"   ) );
}

void TC_Hem_aeFixFuture::FixFuturePathInDistrict()
{
    UtLayer<FutureLayer> future;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<Test>"
            L"<FutureLayer>"
            L"<HemPath>"
            L"  <SpotEvent create_time='20180515T113000Z' name='Arrival' Bdg='starter_2AC[11720]' index='' num='2582' through='Y' />"
            L"  <SpotEvent create_time='20180515T113730Z' name='Departure' Bdg='2AC[11720]' />"
            L"  <SpotEvent create_time='20180515T113730Z' name='Transition' Bdg='mock[11760]' waynum='1' parknum='1' />"
            L"  <SpotEvent create_time='20180515T115900Z' name='Transition' Bdg='mock[09360]' waynum='1' parknum='1' />"
            L"  <SpotEvent create_time='20180515T121352Z' name='Transition' Bdg='mock[09370]' waynum='1' parknum='1' />"
            L"  <SpotEvent create_time='20180515T122707Z' name='Arrival' Bdg='mock[09380]' waynum='3' parknum='1' />"
            L"  <SpotEvent create_time='20180515T124000Z' name='Disform' Bdg='mock[09380]' waynum='3' parknum='1' />"
            L"</HemPath>"
            L"</FutureLayer>"
            L"</Test>"
            );
        future.Deserialize( doc.document_element() );
    }

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20180516T025959Z'>"
        L"<PrePoint layer='future' name='Transition' Bdg='mock[11760]' create_time='20180515T113730Z' waynum='1' parknum='1' />"
        L"<Action code='FixPath' District='09000-11760' DistrictRight='Y' >"
        L"<EventPoint layer='future' name='Transition' sourcetype='fix' Bdg='mock[11760]' create_time='20180515T113730Z' waynum='1' parknum='1' />"
        L"</Action>"
        L"</A2F_HINT>"
        );

    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<DistrictList>"
            L"<District code='09000-11760' name='xxx'>"
            L"<Involve esr='11720'/>"
            L"<Involve esr='11760'/>"
            L"<Involve esr='09360'/>"
            L"</District>"
            L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }

    CPPUNIT_ASSERT( future.path_count() == 1 );
    CPPUNIT_ASSERT( future.exist_series( "name", "         Arrival             Departure    Transition        Transition      Transition         Arrival          Disform" ) );
    CPPUNIT_ASSERT( future.exist_series( "sourcetype", "       -               -                  -               -                -                 -              -"   ) );
    aeFixFuture fix( future.GetWriteAccess(), dg.get_district(bill.getChance().getDCode()), bill.getHeadFixableThrows(), bill.getReplaceFixableEvents().front().IsFixated() );
    fix.Action();
    CPPUNIT_ASSERT( future.path_count() == 1 );
    CPPUNIT_ASSERT( future.exist_series( "name", "           Arrival        Departure        Transition        Transition      Transition         Arrival          Disform" ) );
    CPPUNIT_ASSERT( future.exist_series( "sourcetype", "        -             fix                fix              fix                -                 -             -"   ) );
}

void TC_Hem_aeFixFuture::UnfixFuturePathInDistrict()
{

}