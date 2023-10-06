#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../GUI/GuiColor.h"

class TC_GidUralColor : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TC_GidUralColor);

    CPPUNIT_TEST(Load);     // разбор

    CPPUNIT_TEST_SUITE_END();

public:

protected:
    void Load();

private:
};
