#ifndef TC_EXCEPTIONTRACER_H
#define TC_EXCEPTIONTRACER_H

#include <cppunit/extensions/HelperMacros.h>

/// \brief тест ExceptionTracer на перехват исключений в любых нитях
class TC_ExceptionTracer : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_ExceptionTracer );
  CPPUNIT_TEST( fix );
  CPPUNIT_TEST( fix_without_detailed );
  CPPUNIT_TEST( skip );
  CPPUNIT_TEST( nesting );
  CPPUNIT_TEST( parallel );
  //CPPUNIT_TEST( manual_dump );
  //CPPUNIT_TEST( terminate_by_thread );
  //CPPUNIT_TEST( terminate_by_exit );
  //CPPUNIT_TEST( terminate_by_signal );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp() override;
	virtual void tearDown() override;

protected:
    void fix();         // фиксация исключений стандартной библиотеки
    void fix_without_detailed(); // игнорирование недетализированных дампов
	void skip();        // пропуск некоторых исключений
    void nesting();     // обработка вложенных исключений (повторный throw) 
    void parallel();    // одновременный выброс дампов в параллельных нитях
    void manual_dump();
    void terminate_by_thread();
    void terminate_by_exit();
    void terminate_by_signal();
};

#endif // TC_EXCEPTIONTRACER_H
