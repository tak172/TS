#ifndef TC_POSTERSHOP_H
#define TC_POSTERSHOP_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка stream_badge<*>
 */

class TC_PosterShop : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_PosterShop );
  CPPUNIT_TEST( direct );
  CPPUNIT_TEST( compound );
  CPPUNIT_TEST( subs_for_compound );
  CPPUNIT_TEST_SUITE_END();

public:
    void tearDown();
protected:
    void direct();
    void compound();;
    void subs_for_compound();
private:
};


#endif // TC_POSTERSHOP_H