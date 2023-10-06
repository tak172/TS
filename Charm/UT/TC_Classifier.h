#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Classifier : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Classifier );
    CPPUNIT_TEST( Loading );
    CPPUNIT_TEST( ShouldKnowNames );
    CPPUNIT_TEST( ShouldKnowPositions );
    CPPUNIT_TEST( RailroadClassifierData );
    CPPUNIT_TEST( CarClassifierData );
    CPPUNIT_TEST_SUITE_END();

protected:
    void Loading();
    void ShouldKnowNames();
    void ShouldKnowPositions();
    void RailroadClassifierData();
    void CarClassifierData();
};