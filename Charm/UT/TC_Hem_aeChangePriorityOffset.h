#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/UserChart.h"

class UtUserChart : public UserChart
{
public:
    bool existOffset( const TrainNumber& trn );
};

class TC_Hem_aeChangePriorityOffset : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeChangePriorityOffset );
    CPPUNIT_TEST( Create );
	CPPUNIT_TEST_SUITE_END();

private:
	void Create();  // создание предупреждения
    static griffin::Sender GrifFunc();
};
