#pragma once

#include <cppunit/extensions/HelperMacros.h>

class SpotEvent;

/* 
 * Класс для проверки работы CGeneratorMissedEvent
 */
class TC_GeneratorMissedEvent : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_GeneratorMissedEvent );
  CPPUNIT_TEST( testDeparture );
  CPPUNIT_TEST( testArrival );
  CPPUNIT_TEST( testNoGenerationForCorrect );
  CPPUNIT_TEST( testNoGenerationForError );
  CPPUNIT_TEST( test_7081_6 );
  CPPUNIT_TEST_SUITE_END();

protected:
    void testDeparture();
    void testArrival();
    void testNoGenerationForCorrect();
	void testNoGenerationForError();
	void test_7081_6();

	void CheckAllCorrectEventOnPrevStation(time_t timePrev, BadgeE bdgPrev, std::shared_ptr<SpotEvent> spotNext);
	void CheckAllCorrectEventOnPrevSpan(time_t timePrev, BadgeE bdgPrev, std::shared_ptr<SpotEvent> spotNext);

	void CheckNoGeneration(HCode codeEvent, BadgeE bdgPrev, time_t timePrev, std::shared_ptr<SpotEvent> spotNext);

};

