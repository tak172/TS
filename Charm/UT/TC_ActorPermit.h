#ifndef TC_ACTORPERMIT_H
#define TC_ACTORPERMIT_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Permission/ActorPermit.h"

/* 
 * Класс для проверки работы класса ActorPermit
 */
class TC_ActorPermit : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ActorPermit );
  CPPUNIT_TEST( check_none );
  CPPUNIT_TEST( check_include );
  CPPUNIT_TEST( check_exclude );
  CPPUNIT_TEST( check_include_exclude );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void check_none();
    void check_include();
    void check_exclude();
    void check_include_exclude();

private:
    ActorPermit ap;

    static unsigned long ip4(const char * str);
};


#endif
