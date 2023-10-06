#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Hem_aeReduceBusy : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeReduceBusy );
    CPPUNIT_TEST( onlyDeleteClosed );
	CPPUNIT_TEST_SUITE_END();

private:
	void onlyDeleteClosed();
};
