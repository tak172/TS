#pragma once

#include <cppunit/extensions/HelperMacros.h>

// тесты для класса CategoryTree
class TC_ShowCategory : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_ShowCategory );
    CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST( GetCategory );
    CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp();
	virtual void tearDown();

protected:
    void LoadSave();
    void GetCategory();

private:
};

