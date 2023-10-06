#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_PlukBroker: public CPPUNIT_NS::TestFixture
{
		CPPUNIT_TEST_SUITE(TC_PlukBroker);
		//CPPUNIT_TEST(GetStatutorySchedule);
		//CPPUNIT_TEST(GetVariantRequestList);
		//CPPUNIT_TEST(GetVariantGraphic);
		CPPUNIT_TEST_SUITE_END();

	public:
		void GetStatutorySchedule();
};