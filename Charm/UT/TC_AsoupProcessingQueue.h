#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_AsoupProcessingQueue : public CPPUNIT_NS::TestFixture
{
public:
    CPPUNIT_TEST_SUITE( TC_AsoupProcessingQueue );
    CPPUNIT_TEST( single );
    CPPUNIT_TEST( repeated );
    CPPUNIT_TEST_SUITE_END();

private:
    void single(); // Однократная обработка
    void repeated(); // Многократная обработка
};
