#ifndef TC_FORBIDDETAIL_H
#define TC_FORBIDDETAIL_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка ForbidDetail
 */
class ForbidDetail;

class TC_ForbidDetail : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ForbidDetail );
  CPPUNIT_TEST( serialize );
  CPPUNIT_TEST_SUITE_END();

protected:
    void serialize();

private:
    bool check_and_reassign( const ForbidDetail &a, ForbidDetail &t );
    static bool isEqual( const ForbidDetail& a, const ForbidDetail& b );
};


#endif // TC_FORBIDDETAIL_H