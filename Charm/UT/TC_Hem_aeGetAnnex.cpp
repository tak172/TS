#include "stdafx.h"
#include "TC_Hem_aeGetAnnex.h"
#include "UtHemHelper.h"
#include "../Hem/Appliers/aeGetAnnex.h"
#include "../Hem/DncDspAttendance.h"
#include "../Hem/Demand.h"
#include "../Hem/UserChart.h"

namespace bt = boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeGetAnnex );

void TC_Hem_aeGetAnnex::setUp()
{}

static DistrictGuide buildDictrictGuide()
{
    attic::a_document d;
    d.load_wide(
        L"<DistrictList>"
        L"<District code='09010-09180' name='Rīga-Jelgava'>"
        L"<Involve esr='09100:09104' />"
        L"<Involve esr='09104' />"
        L"<Involve esr='09150' />"
        L"<Involve esr='09160' />"
        L"<Involve esr='09160:09162' />"
        L"<Involve esr='09162' />"
        L"<Involve esr='09162:09180' />"
        L"<Involve esr='09670:09676' />"
        L"<Involve esr='09676' />"
        L"<Involve esr='09715' />"
        L"<Involve esr='09730' />"
        L"<Involve esr='09732' />"
        L"<Involve esr='09736' />"
        L"<Involve esr='09736:09750' />"
        L"</District>"
        L"</DistrictList>"
        );

    DistrictGuide dg;
    dg.deserialize( d.document_element() );
    return dg;
}

void TC_Hem_aeGetAnnex::exoticPath()
{
    attic::a_document docDemand;
    docDemand.load_wide(
        L"<SoxPostDemand mark='valet#1' issue_moment='20200302T014938Z'>"
        L"<A2F_HINT Hint='cmd_demand' version='1' district_code='09010-09180' issue_moment='20200302T014938Z'>"
        L"<Action code='GetAnnex' District='09010-09180' />"
        L"<Shift beginning='20200301T160000Z' end='20200302T040000Z' />"
        L"</A2F_HINT>"
        L"</SoxPostDemand>"
        );
    Hem::Demand demand( docDemand.document_element().first_child() );

    DncDsp::Attendance attendance;
    UtChart<UserChart> utUC;
    utUC.getHappenLayer().createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200301T210516Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' intervalSec='18' optCode='09812:09813' />"
        L"<SpotEvent create_time='20200301T211332Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' intervalSec='3' optCode='09810:09812' />"
        L"<SpotEvent create_time='20200301T212949Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' intervalSec='8' optCode='09803:09810' />"
        L"<SpotEvent create_time='20200301T214057Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' intervalSec='2' optCode='09802:09803' />"
        L"<SpotEvent create_time='20200301T215044Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' intervalSec='4' optCode='09801:09802' />"
        L"<SpotEvent create_time='20200301T220227Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' intervalSec='6' optCode='09800:09801' />"
        L"<SpotEvent create_time='20200301T221454Z' name='Transition' Bdg='2C[09800]' waynum='2' parknum='1' intervalSec='4' optCode='09790:09800' />"
        L"<SpotEvent create_time='20200301T222357Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' intervalSec='7' optCode='09780:09790' />"
        L"<SpotEvent create_time='20200301T223513Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' intervalSec='2' optCode='09751:09780' />"
        L"<SpotEvent create_time='20200301T225016Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' intervalSec='3' optCode='09750:09751' />"
        L"<SpotEvent create_time='20200301T230155Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' intervalSec='3' optCode='09750:09772' />"
        L"<SpotEvent create_time='20200301T232515Z' name='Arrival' Bdg='2C[09772]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200302T000513Z' name='Departure' Bdg='2C[09772]' waynum='2' parknum='1' optCode='09764:09772' />"
        L"<SpotEvent create_time='20200302T002551Z' name='Transition' Bdg='1C[09764]' waynum='1' parknum='1' intervalSec='2' optCode='09180:09764' />"
        L"<SpotEvent create_time='20200302T005111Z' name='Arrival' Bdg='8C[09180]' waynum='8' parknum='1' index='0982-011-1800' num='V2448' length='57' weight='1729' through='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='90' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='ABAKUMOVS' Tim_Beg='2020-03-01 21:32' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200302T014354Z' name='Departure' Bdg='8C[09180]' waynum='8' parknum='1' index='0982-011-1800' num='V2448' length='57' weight='1729' through='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='90' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-BONDARS' Tim_Beg='2020-03-02 02:28' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200302T014938Z' name='Death' Bdg='8C[09180]' waynum='8' parknum='1' />"
        L"</HemPath>"
        );

    DistrictGuide districtGuide=buildDictrictGuide();
    utUC.cachingDistrictGuide(districtGuide);

    Hem::aeGetAnnex engine(utUC, attendance, demand);
    CPPUNIT_ASSERT_NO_THROW( engine.Action() );
    auto response = engine.GetResponse();
    CPPUNIT_ASSERT( nullptr != response );
}
