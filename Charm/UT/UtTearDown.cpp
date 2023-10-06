#include "stdafx.h"
#include "UtTearDown.h"
#include "../helpful/LocoCharacteristics.h"
#include "../Hem/EventsPath.h"
#include "../Hem/TrackedChart.h"
#include "TC_PortablePgSQL.h"


void UtTearDowner::tearDown()
{
    LocoCharacteristics::Shutdowner();
    Hem::DistrictSection::Shutdowner();
    Hem::TrackedChart::eraseCommonData();
    LocalPostgresInstance::Stop();
}
