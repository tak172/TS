#include "stdafx.h"
#include "TC_ScheduleTrainRoutes.h"
#include "../Augur/TrainRoutes.h"
#include "../helpful/Serialization.h"
#include "../Augur/AThread.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../helpful/TrainDescr.h"

using namespace std;
using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ScheduleTrainRoutes );

void TC_ScheduleTrainRoutes::Deserialize()
{
	string tstcontent = "<ScheduledLayer timePeriod=\"[2017-Feb-09 16:00:00/2017-Feb-11 15:59:59.999997]\">\n\
						<ScheduledPath>\
						<SpotEvent create_time=\"20170209T114300Z\" name=\"Info_changing\" Bdg=\"1C[09170]\" index=\"\" num=\"3593\" freloc=\"Y\" />\n\
						<SpotEvent create_time=\"20170209T114300Z\" name=\"Transition\" Bdg=\"1C[09170]\" />\n\
						<SpotEvent create_time=\"20170209T120700Z\" name=\"Arrival\" Bdg=\"1C[09200]\" />\n\
						<SpotEvent create_time=\"20170209T121300Z\" name=\"Departure\" Bdg=\"1C[09200]\" />\n\
						<SpotEvent create_time=\"20170209T163500Z\" name=\"Transition\" Bdg=\"1C[11290]\" />\n\
						<SpotEvent create_time=\"20170209T165300Z\" name=\"Transition\" Bdg=\"1C[11291]\" />\n\
						<SpotEvent create_time=\"20170209T170700Z\" name=\"Arrival\" Bdg=\"1C[11292]\" />\n\
						<SpotEvent create_time=\"20170209T171300Z\" name=\"Departure\" Bdg=\"1C[11292]\" />\n\
						<SpotEvent create_time=\"20170209T180700Z\" name=\"Arrival\" Bdg=\"1C[12500]\" />\n\
						<SpotEvent create_time=\"20170209T181300Z\" name=\"Departure\" Bdg=\"1C[12500]\" />\n\
						<SpotEvent create_time=\"20170209T184000Z\" name=\"Transition\" Bdg=\"1C[11301]\" />\n\
						<SpotEvent create_time=\"20170209T184500Z\" name=\"Transition\" Bdg=\"1C[11302]\" />\n\
						<SpotEvent create_time=\"20170209T185200Z\" name=\"Arrival\" Bdg=\"1C[11312]\" />\n\
						</ScheduledPath>";

	auto scheduleChartPtr = deserialize<TrainRoutes<AEvent>>( tstcontent );
	CPPUNIT_ASSERT( scheduleChartPtr );
	const auto & spotLines = scheduleChartPtr->getSpotLines();
	CPPUNIT_ASSERT( spotLines.size() == 1 );
	const auto & spotLine = spotLines.front();
	const auto & aevents = spotLine.second;
	CPPUNIT_ASSERT( aevents.size() == 9 );

	for ( unsigned int k = 0; k < aevents.size(); ++k )
	{
		const auto & diap = aevents[k].diap;
		if ( k < aevents.size() - 1 )
		{
			if ( diap.getOrig() != diap.getEnd() )
				CPPUNIT_ASSERT( aevents[k].getType() == AEvent::STOP_START );
			else
				CPPUNIT_ASSERT( aevents[k].getType() == AEvent::MOVE );
		}
		else
			CPPUNIT_ASSERT( aevents[k].getType() == AEvent::STOP );
		CPPUNIT_ASSERT( aevents[k].getSource() == AEvent::NONE );
	}

	CPPUNIT_ASSERT( from_time_t( aevents[0].diap.getOrig() ) == from_iso_string("20170209T114300Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[0].diap.getEnd() ) == from_iso_string("20170209T114300Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[1].diap.getOrig() ) == from_iso_string("20170209T120700Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[1].diap.getEnd() ) == from_iso_string("20170209T121300Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[2].diap.getOrig() ) == from_iso_string("20170209T163500Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[2].diap.getEnd() ) == from_iso_string("20170209T163500Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[3].diap.getOrig() ) == from_iso_string("20170209T165300Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[3].diap.getEnd() ) == from_iso_string("20170209T165300Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[4].diap.getOrig() ) == from_iso_string("20170209T170700Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[4].diap.getEnd() ) == from_iso_string("20170209T171300Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[5].diap.getOrig() ) == from_iso_string("20170209T180700Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[5].diap.getEnd() ) == from_iso_string("20170209T181300Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[6].diap.getOrig() ) == from_iso_string("20170209T184000Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[6].diap.getEnd() ) == from_iso_string("20170209T184000Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[7].diap.getOrig() ) == from_iso_string("20170209T184500Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[7].diap.getEnd() ) == from_iso_string("20170209T184500Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[8].diap.getOrig() ) == from_iso_string("20170209T185200Z") );
	CPPUNIT_ASSERT( from_time_t( aevents[8].diap.getEnd() ) == from_iso_string("20170209T185200Z") );
}