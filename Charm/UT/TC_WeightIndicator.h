#ifndef TC_WEIGHTINDICATOR_H
#define TC_WEIGHTINDICATOR_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Fund/RackWarning.h"
#include "../Fund/WeightIndicator.h"


/* 
 * Класс для проверки работы класса WeightIndicator
 */

class TC_WeightIndicator : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_WeightIndicator );
  CPPUNIT_TEST( test1 );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();

    virtual void tearDown();
protected:
    void test1();

private:
    void makeWarn( time_t* mom_beg, time_t* mom_end, const std::wstring& warn_text );
    void makeWI( const std::wstring& warn_text );
    std::wstring check_state( const CLogicElement* le, time_t just_moment );

    RibMap<int>             ribSpeed;
    RackWarning             rackWar;
    WeightIndicator::Rack   weIdxs;
    time_t w1b,w1e;
    time_t w2b,w2e;
};


#endif // TC_WEIGHTINDICATOR_H