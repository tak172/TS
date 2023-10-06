#pragma once
#include <cppunit/extensions/HelperMacros.h>
// #include "../Pluk/PlukDataBase.h"

class TC_HemTabInfo :
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TC_HemTabInfo);
 	CPPUNIT_TEST(InOut);
	CPPUNIT_TEST_SUITE_END();

public:
	void InOut();
};
