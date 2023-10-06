#pragma once
// #include <string>
// #include <memory>
#include <cppunit/extensions/HelperMacros.h>
#include "../Pluk/PlukDataBase.h"

class TC_PlukAttendance :
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TC_PlukAttendance);
 	CPPUNIT_TEST(Write);
    CPPUNIT_TEST(Rewrite);
	CPPUNIT_TEST_SUITE_END();

public:
	void Write();
    void Rewrite();

	std::shared_ptr<PlukDataBase> GetPlukDataBase(const std::string& stringConnect);

	void setUp() override;
	void tearDown() override;
};
