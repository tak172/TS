#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Monitor : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Monitor );
        CPPUNIT_TEST( KeyValueCollector );
        CPPUNIT_TEST( Reporter );
        CPPUNIT_TEST( NetworkFails );
        CPPUNIT_TEST( ReportsCount );
        // CPPUNIT_TEST( ActorReportingIn );  // Тест подключения к работающему Актёру
    CPPUNIT_TEST_SUITE_END();

private:
    void tearDown();
    void KeyValueCollector();
	void Reporter();
    void NetworkFails();
    void ReportsCount();
    void ActorReportingIn();
};
