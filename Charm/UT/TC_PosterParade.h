#ifndef TC_POSTERPARADE_H
#define TC_POSTERPARADE_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"

/* 
 * Проверка stream_badge<*>
 */

class TC_PosterParade : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_PosterParade );
  CPPUNIT_TEST( append_direct );
  CPPUNIT_TEST( append_compound );
  CPPUNIT_TEST( text_direct );
  CPPUNIT_TEST( text_compound );
  CPPUNIT_TEST( sound_direct );
  CPPUNIT_TEST( sound_compound );
  CPPUNIT_TEST( mix_direct_compound );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
protected:
    void append_direct();
    void append_compound();
    void text_direct();
    void text_compound();
    void sound_direct();
    void sound_compound();
    void mix_direct_compound();
private:
    EsrGuide myEsrGuide;
    static const time_t MOMENT = 12345;

};


#endif // TC_POSTERPARADE_H