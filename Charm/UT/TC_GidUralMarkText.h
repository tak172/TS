#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../GUI/GuiMarkTextClassificator.h"

class TC_GidUralMarkText : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TC_GidUralMarkText);

    CPPUNIT_TEST(Load);     // разбор

    CPPUNIT_TEST_SUITE_END();

public:

protected:
    void Load();

private:
};
