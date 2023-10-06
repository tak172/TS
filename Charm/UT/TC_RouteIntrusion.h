#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_RouteIntrusion : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_RouteIntrusion );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST_SUITE_END();

	void Serialize();
	void Deserialize();
};