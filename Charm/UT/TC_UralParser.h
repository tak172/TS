#ifndef TC_URALPARSER_H
#define TC_URALPARSER_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса EsrKit
 */

class TC_UralParser : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_UralParser );
  CPPUNIT_TEST( ConvTime );
  CPPUNIT_TEST_SUITE_END();

protected:
    void ConvTime();
private:
};


#endif // TC_URALPARSER_H