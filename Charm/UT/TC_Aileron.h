#ifndef TC_AILERON_H
#define TC_AILERON_H

#include <cppunit/extensions/HelperMacros.h>
#include "Parody.h"

/// ����� ��� ������ \ref Ailerons
class TC_Aileron : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Aileron );
  CPPUNIT_TEST( street );
  CPPUNIT_TEST( opposite );
  CPPUNIT_TEST( dropper );
  CPPUNIT_TEST( tick1646 );
  CPPUNIT_TEST( tick1646_comment3 );
  CPPUNIT_TEST( independent_with_TS );
  CPPUNIT_TEST( unknown_1631 );
  CPPUNIT_TEST( in_route );
  CPPUNIT_TEST( receives_lts );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

protected:
    void street(); // ���������� �����
    void opposite(); // ��������� ������� � ����� ������� 
    void dropper(); // ��������������� ������
    void tick1646(); // ����� 1646
    void tick1646_comment3();
    void independent_with_TS(); // ������������� �������� � �����
    void unknown_1631(); // ����������� ��������� �������� ���������������� �������
    void in_route();    // �������� ������� � ��������
    void receives_lts(); // ��������� ����� ����� ��� ��������

private:
};


#endif // TC_AILERON_H