#ifndef TC_GLOG_H
#define TC_GLOG_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * ����� ��� �������� ������ glog
 */
class TC_GLog : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_GLog );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp()    override;
    //void tearDown() override;
protected:
    void test();
};


#endif // TC_HLOG_H