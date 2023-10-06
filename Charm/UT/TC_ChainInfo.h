#pragma once

#include <cppunit/extensions/HelperMacros.h>

/* 
* �������� ChainInfo
 */

namespace RollingStock { class Model; };
class ChainInfo;

class TC_ChainInfo : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ChainInfo );
  CPPUNIT_TEST( simple );
  CPPUNIT_TEST( mix_trains );
  CPPUNIT_TEST( overwrite );
  CPPUNIT_TEST( summary );
  CPPUNIT_TEST_SUITE_END();
public:
  ~TC_ChainInfo();

protected:
    void setUp() override;
    void tearDown() override;
    void simple(); // ���� ����� �� ���� �������
    void mix_trains(); // ����� ������� ������ �������
    void overwrite(); // ���������� ������� (������ �������)
    void summary(); // ����� �������� �� �������

    boost::scoped_ptr< RollingStock::Model > m_model;
    boost::scoped_ptr< ChainInfo > m_chaininfo;
};
