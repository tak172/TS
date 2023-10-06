#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_TopologyHelper : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TC_TopologyHelper);
    CPPUNIT_TEST(Correct);
    CPPUNIT_TEST(Mistakes);
    CPPUNIT_TEST(Multiple);
    CPPUNIT_TEST(DirectStnExample);
    CPPUNIT_TEST_SUITE_END();

public:
    TC_TopologyHelper(void);
    ~TC_TopologyHelper(void);

protected:
    void Correct();
    void Mistakes();
    void Multiple();
    void DirectStnExample();
};

