#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_SpansRegistry : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_SpansRegistry );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST_SUITE_END();

	void Serialize();
	void Deserialize();
};