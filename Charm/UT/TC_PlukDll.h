#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <string>

class TC_PlukDll :
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TC_PlukDll);
	//CPPUNIT_TEST(InitDll);
	//CPPUNIT_TEST(WriteChange);
	CPPUNIT_TEST_SUITE_END();

public:

	void InitDll();
	void WriteChange();
	
	virtual void setUp();
	virtual void tearDown();

};