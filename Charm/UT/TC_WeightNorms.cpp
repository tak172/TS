#include "stdafx.h"
#include "TC_WeightNorms.h"
#include "../helpful/WeightNorms.h"
#include "../helpful/TrainDescr.h"
#include "../helpful/Locomotive.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_WeightNorms );

void TC_WeightNorms::WeightBid()
{
	WeightNorms wnorms;
	wnorms.AddNorm( WeightNorms::NormValue( 202, StationEsr(23560), 6500 ) );
	TrainDescr tdescr;
	vector<Locomotive> locos;
	locos.push_back( Locomotive( LocoId( 17, 202 ), 0, 0 ) );
	tdescr.SetLocomotives( locos );
	tdescr.SetBrutto( 6501 );
	CPPUNIT_ASSERT( wnorms.StopForbidden( tdescr, StationEsr(23560) ) );
	tdescr.SetBrutto( 6500 );
	CPPUNIT_ASSERT( !wnorms.StopForbidden( tdescr, StationEsr(23560) ) );
}

void TC_WeightNorms::TargetBid()
{
	WeightNorms wnorms;
	wnorms.AddNorm( WeightNorms::NormValue( 271, StationEsr(23560), 3200 ) );
	TrainDescr tdescr;
	vector<Locomotive> locos;
	locos.push_back( Locomotive( LocoId( 17, 271 ), 0, 0 ) );
	tdescr.SetLocomotives( locos );
	tdescr.SetBrutto( 8100 );
	CPPUNIT_ASSERT( wnorms.StopForbidden( tdescr, StationEsr(23560) ) );
	CPPUNIT_ASSERT( !wnorms.StopForbidden( tdescr, StationEsr(23561) ) );
}

void TC_WeightNorms::LocoSeriesBid()
{
	WeightNorms wnorms;
	wnorms.AddNorm( WeightNorms::NormValue( 324, StationEsr(23560), 4700 ) );
	TrainDescr tdescr;
	tdescr.SetBrutto( 9400 );
	vector<Locomotive> locos;
	locos.push_back( Locomotive( LocoId( 17, 324 ), 0, 0 ) );
	tdescr.SetLocomotives( locos );
	CPPUNIT_ASSERT( wnorms.StopForbidden( tdescr, StationEsr(23560) ) );
	locos.clear();
	locos.push_back( Locomotive( LocoId( 17, 325 ), 0, 0 ) );
	tdescr.SetLocomotives( locos );
	CPPUNIT_ASSERT( !wnorms.StopForbidden( tdescr, StationEsr(23560) ) );
}