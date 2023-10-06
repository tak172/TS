#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Pilgrim : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Pilgrim );
    CPPUNIT_TEST( TestSoundPath );
	CPPUNIT_TEST_SUITE_END();
public:
    void setUp() override;
    void tearDown() override;
protected:
    void TestSoundPath();
    boost::filesystem::path d1;
    boost::filesystem::path d1d2;
};
