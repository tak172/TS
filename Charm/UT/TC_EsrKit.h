#ifndef TC_ESRKIT_H
#define TC_ESRKIT_H

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса EsrKit
 */

class TC_EsrKit : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_EsrKit );
  CPPUNIT_TEST( Numbers );
  CPPUNIT_TEST( Strings );
  CPPUNIT_TEST( Strings6 );
  CPPUNIT_TEST( Baddies );
  CPPUNIT_TEST( compare );
  CPPUNIT_TEST( replace );
  CPPUNIT_TEST( intersect );
  CPPUNIT_TEST( complement );
  CPPUNIT_TEST( bogus );
  CPPUNIT_TEST( term6 );
  CPPUNIT_TEST( statEsr );
  CPPUNIT_TEST_SUITE_END();

protected:
	void Numbers();
    void Strings();
	void Strings6();
    void Baddies();
    void compare();
    void replace();
    void intersect();
    void complement();
    void bogus();
    void term6();
    void statEsr();
};


#endif // TC_ESRKIT_H