#include "stdafx.h"

#include "TC_Hem_aeDisformAfterIdentify.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/Appliers/aeCut.h"
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Hem/Appliers/aeRectify.h"
#include "../Hem/Bill.h"
#include "../helpful/LocoCharacteristics.h"
#include "../Hem/UserEditInventory.h"

using namespace std;
using namespace HemHelpful;
using namespace Hem;
using namespace Hem::ApplierCarryOut;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeDisformAfterIdentify );

void TC_Hem_aeDisformAfterIdentify::CutPathEndsOnArrival()
{
    UtLayer<HappenLayer> hl;
    hl.createPath( 
        L"<HemPath>"
        L"<SpotEvent create_time='20190916T164813Z' name='Form' Bdg='3C[09670]' waynum='3' parknum='2' intervalSec='98' index='' num='6432' length='4' weight='238' suburbreg='Y' />"
        L"<SpotEvent create_time='20190916T164851Z' name='Departure' Bdg='3AC[09670]' waynum='3' parknum='3' intervalSec='21' />"
        L"<SpotEvent create_time='20190916T165248Z' name='Transition' Bdg='3p[09100]' waynum='3' parknum='1' intervalSec='103' />"
        L"<SpotEvent create_time='20190916T165521Z' name='Arrival' Bdg='6C[09010]' waynum='6' parknum='1' index='' num='6240' suburbreg='Y' />"
        L"</HemPath>"
        );

    UtLayer<AsoupLayer> al;
    al.createPath(
        L"<AsoupEvent create_time='20190916T165500Z' name='Arrival' Bdg='24.121025,56.946324[09010]' index='' num='6432' length='4' weight='238' suburbreg='Y' linkBadge='6C[09010]' linkTime='20190916T165521Z' linkCode='Arrival'>"
        L"<![CDATA[<TrainAssignment operation='6' date='16-09-2019 19:55' esr='09010' location='24.121025,56.946324' number='6432' issue_moment='20190916T170009Z'>"
        L"<Unit number='1342-03--ER2' driver='Guzenko' driverTime='16-09-2019 19:03' weight='238' length='4' specialConditions='M' carrier='24'>"
        L"<Wagon number='1342-03' axisCount='4' weight='53' length='1' type='1' owner='24' />"
        L"<Wagon number='1342-04' axisCount='4' weight='66' length='1' type='2' owner='24' />"
        L"<Wagon number='1341-06' axisCount='4' weight='66' length='1' type='2' owner='24' />"
        L"<Wagon number='1342-07' axisCount='4' weight='53' length='1' type='1' owner='24' />"
        L"</Unit>"
        L"</TrainAssignment>"
        L"]]><Locomotive Series='на2' NumLoc='1342'>"
        L"<Crew EngineDriver='Guzenko' Tim_Beg='2019-09-16 19:03' PersonMode='1' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );
    CPPUNIT_ASSERT( al.path_count()==1 );

    auto asoupEvents = al.GetEventsBySuburbanNumber(6432);

    CPPUNIT_ASSERT( asoupEvents.size()==1 ); 
    auto asoup = *asoupEvents.cbegin();

    CPPUNIT_ASSERT( asoup );

    aeDisformAfterIdentify cut( hl.GetWriteAccess(), al.GetReadAccess(), asoup );
    auto result = cut.PerformAction();

    CPPUNIT_ASSERT( hl.path_count()==1 );
    CPPUNIT_ASSERT( result.first && result.second );
}

