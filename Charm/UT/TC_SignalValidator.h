#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Signal.h"

class TC_SignalValidator :
	public TC_Signal,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_SignalValidator );
	CPPUNIT_TEST( LADuplicates );
	CPPUNIT_TEST_SUITE_END();

	void LADuplicates();
};