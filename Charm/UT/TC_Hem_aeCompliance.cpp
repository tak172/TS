#include "stdafx.h"

#include "TC_Hem_aeCompliance.h"
#include "../Hem/UserChart.h"
#include "../Hem/Appliers/aeCompliance.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/RegulatoryLayer.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../helpful/Badge.h"
#include "../Hem/Analytics.h"
#include "../Guess/SpotDetails.h"

using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeCompliance );

void TC_Hem_aeCompliance::DisformByRegular()
{
    const std::wstring layerData = L"<export>\n"
        L"<ScheduledLayer timePeriod='[20190624T030000Z/20190626T025959Z)'>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20190624T191430Z' name='Info_changing' Bdg='¹6523[09670]' index='' num='6523' suburbreg='Y' />"
        L"<SpotEvent create_time='20190624T191430Z' name='Departure' Bdg='¹6523[09670]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190624T192630Z' name='Arrival' Bdg='¹6523[09676]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190624T192700Z' name='Departure' Bdg='¹6523[09676]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190624T193830Z' name='Arrival' Bdg='¹6523[09715]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190624T193930Z' name='Departure' Bdg='¹6523[09715]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190624T195400Z' name='Arrival' Bdg='¹6523[09730]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190624T195500Z' name='Departure' Bdg='¹6523[09730]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190624T200300Z' name='Arrival' Bdg='¹6523[09732]' waynum='1' parknum='1' />"
        L"</ScheduledPath>"
        L"<ScheduledPath>"
        L"<SpotEvent create_time='20190625T191430Z' name='Info_changing' Bdg='¹6523[09670]' index='' num='6523' suburbreg='Y' />"
        L"<SpotEvent create_time='20190625T191430Z' name='Departure' Bdg='¹6523[09670]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190625T192630Z' name='Arrival' Bdg='¹6523[09676]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190625T192700Z' name='Departure' Bdg='¹6523[09676]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190625T193830Z' name='Arrival' Bdg='¹6523[09715]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190625T193930Z' name='Departure' Bdg='¹6523[09715]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190625T195400Z' name='Arrival' Bdg='¹6523[09730]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190625T195500Z' name='Departure' Bdg='¹6523[09730]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190625T200300Z' name='Arrival' Bdg='¹6523[09732]' waynum='1' parknum='1' />"
        L"</ScheduledPath>"
        L"</ScheduledLayer>\n"
        L"</export>\n";

    m_regulatoryLayer = std::make_shared<Regulatory::Layer>();

    {
        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(layerData));
        m_regulatoryLayer->deserialize(doc.document_element());
        m_regulatoryLayer->takeChanges(attic::a_node());
    }


    m_happenLayer = std::make_shared< UtLayer<HappenLayer> >();
    auto& hl = *m_happenLayer;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20190624T181513Z' name='Form' Bdg='PpP[09670]' index='' num='6523' suburbreg='Y'/>"
        L"<SpotEvent create_time='20190624T182735Z' name='Transition' Bdg='1p[09676]' waynum='1' parknum='1' intervalSec='81' optCode='09676:09715' />"
        L"<SpotEvent create_time='20190624T183945Z' name='Transition' Bdg='1p[09715]' waynum='1' parknum='1' intervalSec='95' optCode='09715:09730' />"
        L"<SpotEvent create_time='20190624T185500Z' name='Transition' Bdg='2p[09730]' waynum='2' parknum='1' intervalSec='139' />"
        L"<SpotEvent create_time='20190624T190245Z' name='Arrival' Bdg='1p[09732]' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( hl.path_count()==1 );

    time_t tArrival = (from_iso_string("20190624T190245Z") - from_time_t(0)).total_seconds();
    HemHelpful::SpotEventPtr spotPtr = std::make_shared<SpotEvent>( HCode::ARRIVAL, BadgeE( L"1p", EsrKit(9732) ), tArrival );
    Hem::aeCompliance complianceEngine(m_happenLayer->GetWriteAccess(), m_regulatoryLayer->getReadAccess(), spotPtr);

    CPPUNIT_ASSERT( complianceEngine.performAction()==Hem::Analytics::Type::DisformTrain) ;

    CPPUNIT_ASSERT( hl.path_count() == 1 );
}

