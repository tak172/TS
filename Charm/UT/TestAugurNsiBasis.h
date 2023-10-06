#pragma once

#include "../helpful/NsiBasis.h"
#include "../helpful/SpanTravelTime.h"
#include "../helpful/StatIntervals.h"
#include "../helpful/StationsRegistry.h"
#include "../helpful/SpansRegistry.h"
#include "../helpful/Amqp/RestClient.h"
#include "TopologyTest.h"
#include "../helpful/SpanWayInfo.h"
#include "../helpful/StatWayInfo.h"
#include "../helpful/Oddness.h"
#include "../helpful/TechnicalTime.h"

class RoadKit;
struct TestAugurNsiBasis : public NsiBasis
{
	TestAugurNsiBasis();
	TestAugurNsiBasis( const std::map <EsrKit, SpansRegistry::SpanInfo> &, const std::map<EsrKit, StationsRegistry::StationInfo> & );
	void setDepartureByHemPath(long esr, bool t=true);
};		
