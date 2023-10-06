#pragma once
#include <cppunit/extensions/HelperMacros.h>
class TC_PlukConnect :
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TC_PlukConnect);
// 	CPPUNIT_TEST(SingleTestConnect);
	/*
	CPPUNIT_TEST(OpenCloseConnect);
	CPPUNIT_TEST(OpenCloseConnect);
	CPPUNIT_TEST(OverheadPoolConnect);
	*/
	CPPUNIT_TEST_SUITE_END();

public:
	void OpenCloseConnect();
	void OpenClosePoolConnect();
	void OverheadPoolConnect();
	virtual void setUp();
	virtual void tearDown();

	void SingleTestConnect() 
	{
		OpenCloseConnect();
		OpenClosePoolConnect();
		OverheadPoolConnect();
	}
};
