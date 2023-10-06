#pragma once

#include <cppunit/extensions/HelperMacros.h>

// тесты для класса HemThemes
class TC_HemThemes : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_HemThemes );
	CPPUNIT_TEST( LoadSave );
	CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp();
	virtual void tearDown();

protected:
	void LoadSave();

private:
};

