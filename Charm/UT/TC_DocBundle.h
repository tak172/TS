#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_DocBundle :
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_DocBundle );
	CPPUNIT_TEST( Creating );
	CPPUNIT_TEST( Adding );
	CPPUNIT_TEST( Removing );
	CPPUNIT_TEST_SUITE_END();
	void Creating();
	void Adding();
	void Removing();
};