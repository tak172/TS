#ifndef TC_LOS_HEAD_H
#define TC_LOS_HEAD_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * ����� ��� �������� ������ ������ LOS_Head
 */
class Latch_LOS_Head;
class TC_LOS_Head : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LOS_Head );
  CPPUNIT_TEST( immediate );
  CPPUNIT_TEST( open_priority );
  CPPUNIT_TEST( latching );
  CPPUNIT_TEST( latching_reopen );
  CPPUNIT_TEST( latching_select );
  CPPUNIT_TEST_SUITE_END();

protected:
    void setUp() override;
    void tearDown() override;

    void immediate(); // ���������������� ������������ (��� ���������)
    void open_priority(); // ��������� �������� ������ � ������� ���������
    void latching(); // ������� ��������� ������ ���������
    void latching_reopen(); // ��������� ����� ���������� ��������
    void latching_select(); // ��������� ���������� ���������� ������������, �� �� ����
private:
    static const time_t T = 12345;
    static const unsigned INTERV = 10;
    Latch_LOS_Head* latch;
};


#endif // TC_LOS_HEAD_H