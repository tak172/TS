#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"
#include "../Hem/HemHelpful.h"

class TopologyTest;
class HappenLayer;
class EsrGuide;
namespace Hem
{
    class GuessTransciever;
}

class TC_Hem_aeChangeListSpanWork : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeChangeListSpanWork );
    CPPUNIT_TEST( WorkWithReturn );
    CPPUNIT_TEST_SUITE_END();

private:
    void WorkWithReturn();

    void setUp() override;
    void tearDown() override;

    std::shared_ptr< UtLayer<HappenLayer> > happen_layer;
    std::shared_ptr< UtLayer<AsoupLayer> > asoup_layer;
    std::shared_ptr<EsrGuide> esrGuide;
    std::shared_ptr<TopologyTest> topology;
    std::shared_ptr<Hem::GuessTransciever> guessTransciever;
};
