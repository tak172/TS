#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Actor/HemDraw/HemType.h"

// тесты для класса HemThemes
class TC_SourseEventMgr : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_SourseEventMgr );
    CPPUNIT_TEST( acceptChanges );
    CPPUNIT_TEST( addChain );
	CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp();
	virtual void tearDown();

protected:
    void acceptChanges();
    void addChain();
private:
    void checkAddDelEvents();
    void checkChangeEvents();
    void checkAddDelChain();
    void checkCreateChain();

private:
    std::vector <HemdrawType::BASEPOINT_VECT> vChains;
};

