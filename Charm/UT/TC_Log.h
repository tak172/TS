#pragma once

#include <cppunit/extensions/HelperMacros.h>

/* 
 * Проверка Log
 */

class TC_Log : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Log );
  CPPUNIT_TEST( flush );
  CPPUNIT_TEST( daily );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() override;
  void tearDown() override;
protected:
    void flush(); // тест сброса буферов на диск
	void daily(); // проверка разбиения по дням
};
