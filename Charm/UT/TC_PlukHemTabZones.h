#pragma once
#include <cppunit/extensions/HelperMacros.h>
#include "../Pluk/PlukDataBase.h"

class TC_PlukHemTabZones :
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TC_PlukHemTabZones);
#if _WIN64
    CPPUNIT_TEST( WriteReadRemove );
#endif
	CPPUNIT_TEST_SUITE_END();

public:
	void WriteReadRemove();

	std::shared_ptr<PlukDataBase> GetPlukDataBase(const std::string& stringConnect);

	void setUp() override;
	void tearDown() override;
};
