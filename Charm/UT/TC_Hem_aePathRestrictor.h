#pragma once

#include <memory>
#include <cppunit/extensions/HelperMacros.h>

namespace Regulatory
{
    class Layer;
};

class TC_Hem_aePathRestrictor : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Hem_aePathRestrictor );
    CPPUNIT_TEST( Creation );
    CPPUNIT_TEST( ClearRun );
    CPPUNIT_TEST( Departure );
    CPPUNIT_TEST( Transition );
    CPPUNIT_TEST_SUITE_END();

private:
    std::unique_ptr<Regulatory::Layer> m_regulatoryLayer;

    void setUp() override;
    void tearDown() override;

    void Creation();
    void ClearRun();
    void Departure();
    void Transition();
};
