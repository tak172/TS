#pragma once

#include <cppunit/extensions/HelperMacros.h>

class UtTearDowner : public CPPUNIT_NS::TestFixture
{
private:
    void tearDown() final;
};
