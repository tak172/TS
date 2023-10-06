#pragma once

#include <cppunit/extensions/HelperMacros.h>


class TC_PlukCommandFactory: public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TC_PlukCommandFactory);
	CPPUNIT_TEST(CommandCreate);
	CPPUNIT_TEST_SUITE_END();

public:
	void CommandCreate();
};

