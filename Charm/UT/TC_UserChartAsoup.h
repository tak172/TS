#pragma once

#include <memory>
#include <cppunit/extensions/HelperMacros.h>

class UserChart;
class TC_UserChartAsoup : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_UserChartAsoup );
	CPPUNIT_TEST( AsoupMessage );
    CPPUNIT_TEST( AsoupUsed );
	CPPUNIT_TEST( AsoupDir );
	CPPUNIT_TEST_SUITE_END();

protected:
	void AsoupMessage(); // Simple test
    void AsoupUsed(); 

	virtual void setUp();
	virtual void tearDown();
	
	
	void AsoupDir();

private:
	std::unique_ptr<UserChart> m_userChart;
};