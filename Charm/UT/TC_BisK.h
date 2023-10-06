#ifndef TC_BISK_H_
#define TC_BISK_H_

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы функций пространства BisK
 */
class TC_BisK : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_BisK );
  CPPUNIT_TEST( isId );
  CPPUNIT_TEST( makeId );
  CPPUNIT_TEST( makeId_0_microsec );
  CPPUNIT_TEST( makeAckSucc );
  CPPUNIT_TEST( makeAckFail );
  CPPUNIT_TEST( makeRequest );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp()    {}
    virtual void tearDown() {}
protected:
    void isId();
    void makeId();
    void makeId_0_microsec();
    void makeAckSucc();
    void makeAckFail();
    void makeRequest();
};


#endif //TC_BISK_H_
