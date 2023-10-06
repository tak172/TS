#ifndef TC_LOGCLEANER_H
#define TC_LOGCLEANER_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса ActorPermit
 */
class TC_LogCleaner : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_LogCleaner );
  CPPUNIT_TEST( test_size );
  CPPUNIT_TEST( test_count );
  CPPUNIT_TEST( test_DebugLog_else );
  CPPUNIT_TEST( test_DebugLog_evaluate );
  CPPUNIT_TEST_SUITE_END();

public:
protected:
    void test_size();
	void test_count();
    void test_DebugLog_else();
    void test_DebugLog_evaluate();
};

#endif // TC_LOGCLEANER_H