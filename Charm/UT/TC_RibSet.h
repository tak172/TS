#ifndef TC_RIBSET_H
#define TC_RIBSET_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * ����� ��� �������� ������ ������ KmPk
 */

class TC_RibSet : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_RibSet );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

protected:
    void test();
private:
};


#endif // TC_LOS_BOARD_H