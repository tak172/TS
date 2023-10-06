#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_RouteIntrusionKeeper : 
	public CPPUNIT_NS::TestFixture
{

private:
	CPPUNIT_TEST_SUITE( TC_RouteIntrusionKeeper );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST_SUITE_END();

protected:
	void Serialize();
	void Deserialize();
};