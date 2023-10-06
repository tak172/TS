#include "stdafx.h"

#include "TC_Hem_aeChangePriorityOffset.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "../helpful/TrainsPriorityOffsets.h"
#include "../Hem/Bill.h"
#include "../Hem/UserChart.h"
#include "../Hem/Appliers/aeChangePriorityOffset.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Hem/GriffinUtility.h"

using namespace boost::posix_time;

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeChangePriorityOffset );

void TC_Hem_aeChangePriorityOffset::Create()
{
    UtUserChart userChart;

    TrainDescr tdr(L"V2426");
    CPPUNIT_ASSERT ( !userChart.existOffset(tdr.GetNumber()));

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<Action code='PredictPriorityChange' index='' num='V2426' through='Y' inner='Y' shift='1' District='09180-09820' DistrictRight='Y' />"
        L"</A2F_HINT>"
        );
    time_t t = (from_iso_string("20180727T040000Z") - from_time_t(0)).total_seconds();

    userChart.carryOut(bill, t, GrifFunc());
    CPPUNIT_ASSERT( userChart.existOffset(tdr.GetNumber()) );
}

griffin::Sender TC_Hem_aeChangePriorityOffset::GrifFunc()
{
    griffin::Sender gf = []( const griffin::Utility* ){};
    return gf;
}

bool UtUserChart::existOffset( const TrainNumber& trn )
{
return getTrainPriorityOffsets().GetOffset(trn)!=nullptr ;
}
