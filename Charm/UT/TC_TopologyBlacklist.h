#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_TopologyBlacklist : public CPPUNIT_NS::TestFixture
{
public:
	TC_TopologyBlacklist();

	CPPUNIT_TEST_SUITE(TC_TopologyBlacklist);
	CPPUNIT_TEST( Infix );
	CPPUNIT_TEST_SUITE_END();

private:
	std::set<StationEsr> nodeCodes;

	void Infix();
};
