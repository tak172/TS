#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_WeightNorms : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_WeightNorms );
	CPPUNIT_TEST( WeightBid );
	CPPUNIT_TEST( TargetBid );
	CPPUNIT_TEST( LocoSeriesBid );
	CPPUNIT_TEST_SUITE_END();

	void WeightBid();
	void TargetBid();
	void LocoSeriesBid();
};