#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/HemHelpful.h"
#include "UtHemHelper.h"

class HappenLayer;
namespace Regulatory
{
    class Layer;
};

class TC_Hem_aeCompliance : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeCompliance );
    CPPUNIT_TEST( DisformByRegular );
    CPPUNIT_TEST_SUITE_END();

private:
    void DisformByRegular();


    std::shared_ptr< UtLayer<HappenLayer> > m_happenLayer;
    std::shared_ptr<Regulatory::Layer> m_regulatoryLayer;
};
