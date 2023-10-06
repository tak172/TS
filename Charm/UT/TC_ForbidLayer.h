#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_ForbidLayer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_ForbidLayer );
    CPPUNIT_TEST( Reload );
    CPPUNIT_TEST_SUITE_END();

private:
    void Reload();
};
