#pragma once

#include <cppunit/extensions/HelperMacros.h>

/* 
* Проверка Scoreboard
 */

namespace RollingStock { class Model; };
namespace Scoreboard   { class Blank; };
class ChainInfo;
class Config;
class ExplainManager;

class TC_Scoreboard : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Scoreboard );
  //CPPUNIT_TEST( promotion );
  CPPUNIT_TEST_SUITE_END();
public:
  ~TC_Scoreboard();

protected:
    void promotion();

    void apply_all( Config &cfg );
    void setUp() override;
    void tearDown() override;

    boost::scoped_ptr< RollingStock::Model > m_model;
    boost::scoped_ptr< ChainInfo > m_chaininfo;
    boost::scoped_ptr< Scoreboard::Blank > m_blank;
    boost::scoped_ptr< ExplainManager > m_explainMgr;
    BadgeE pre;
    BadgeE top;
};
