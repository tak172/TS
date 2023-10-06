#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace Note { class Event; };
// проверка класса
class TC_NoteLayer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_NoteLayer );
    CPPUNIT_TEST( Reload );
    CPPUNIT_TEST_SUITE_END();

private:
    void Reload();
};
