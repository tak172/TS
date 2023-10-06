#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Logic.h"

class TC_LogicDocument :
	public TC_Logic,
	public CPPUNIT_NS::TestFixture
{
    void setUp() override;
    void tearDown() override;
private:
	CPPUNIT_TEST_SUITE( TC_LogicDocument );
	CPPUNIT_TEST( Creating );
	CPPUNIT_TEST( OnesidedJointTesting );
	CPPUNIT_TEST( SingleJointTesting );
	CPPUNIT_TEST( CorrectJointTesting );
	CPPUNIT_TEST( WaysNumsTesting );
	CPPUNIT_TEST_SUITE_END();
	void Creating();
	void CorrectJointTesting();
	void OnesidedJointTesting();
	void SingleJointTesting();
	void WaysNumsTesting();
};