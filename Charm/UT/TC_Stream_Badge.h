#ifndef TC_STREAM_BADGE_H
#define TC_STREAM_BADGE_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка stream_badge<*>
 */

class TC_Stream_Badge : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Stream_Badge );
  CPPUNIT_TEST( char_stream );
  CPPUNIT_TEST( wide_stream );
  CPPUNIT_TEST_SUITE_END();

protected:
    void char_stream();
    void wide_stream();
private:
};


#endif // TC_STREAM_BADGE_H