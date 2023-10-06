#ifndef TC_POSTERBREATH_H
#define TC_POSTERBREATH_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Fund/PosterShop.h"

/// ����� ��� ������ \ref PosterBreath
class TC_PosterBreath : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_PosterBreath );
  CPPUNIT_TEST( wait_for_start );
  CPPUNIT_TEST( insert_multy );
  CPPUNIT_TEST( run_twice );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
protected:
    void wait_for_start(); ///< �������� ������� �� ������������ �������
    void insert_multy();   ///< ��������� ������� (��������� ������������) �������
    void run_twice();      ///< ������������� ���������������� ������������ �������
private:
    PosterShop ps;
    static const time_t t0 = 1234567; // ������������ ����� 
    EsrKit kit;      // �������
    BadgeE b1,b2,b3; // ������� 
    size_t d1,d2,d3; // ��������
};


#endif // TC_POSTERBREATH_H