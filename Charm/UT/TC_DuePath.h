#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_DuePath: public CPPUNIT_NS::TestFixture
{

	CPPUNIT_TEST_SUITE(TC_DuePath);
    CPPUNIT_TEST(actor_64_noext);
    CPPUNIT_TEST(hem_64_dll);
    CPPUNIT_TEST(guess_32_dll);
	CPPUNIT_TEST_SUITE_END();

public: 
	void actor_64_noext();
    void hem_64_dll();
    void guess_32_dll();
};

