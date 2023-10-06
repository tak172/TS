#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Translate: public CPPUNIT_NS::TestFixture
{

	CPPUNIT_TEST_SUITE(TC_Translate);
	CPPUNIT_TEST(Translate);
	CPPUNIT_TEST_SUITE_END();

public: 
	void Translate();
};

