#ifndef TC_RWCOORD_H
#define TC_RWCOORD_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * �������� ������ rwCoord
 */

class TC_rwCoord : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_rwCoord );
  CPPUNIT_TEST( simple_empty );
  CPPUNIT_TEST( from_text );
  CPPUNIT_TEST( exactKm );
  CPPUNIT_TEST( Km_and_tiny );
  CPPUNIT_TEST( exactPiquet );
  CPPUNIT_TEST( piquet_and_tiny );
  CPPUNIT_TEST( using_flat_meter );
  CPPUNIT_TEST( equals );
  CPPUNIT_TEST( compare_normal );
  CPPUNIT_TEST( compare_strange );
  CPPUNIT_TEST( set_irregular );
  CPPUNIT_TEST( calc_irregular );
  CPPUNIT_TEST( offset_w_irregular );
  CPPUNIT_TEST( ceil_floor );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;
protected:
    void simple_empty(); // ��������
    void from_text();    // ���������� �� ������
    void exactKm();      // ������ ������������ �������
    void Km_and_tiny();  // �������� � ���������
    void exactPiquet();  // ������ �������� �������
    void piquet_and_tiny(); // ����� � ���������
    void using_flat_meter(); // ���������� ������� � ������
    void equals();          // ���������
    void compare_normal();  // ��������� �������� (����������)
    void compare_strange(); // ��������� �������� (������������ ������)
    void set_irregular();   // ��������� ������������ ����������
    void calc_irregular();  // ���������� � ������������� �����������
    void offset_w_irregular(); // ����� ���������� �� ��������� ������
    void ceil_floor(); // ����� �� �������� ������������ �������
private:
};


#endif // TC_RWCOORD_H