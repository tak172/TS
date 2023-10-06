#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Junctions : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_Junctions );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( CheckTechnodesAfterDeserialize );
	CPPUNIT_TEST( CheckOddnessAfterDeserialize );
	CPPUNIT_TEST( CheckEqualAreasAfterDeserialize );
	CPPUNIT_TEST_SUITE_END();

	void Serialize();
	void CheckTechnodesAfterDeserialize();
	void CheckOddnessAfterDeserialize();
	void CheckEqualAreasAfterDeserialize();
};