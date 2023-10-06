#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace Limit { class Event; };
// проверка класса
class TC_LimitLayer : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_LimitLayer );
    CPPUNIT_TEST( Create );
    CPPUNIT_TEST( Change );
    CPPUNIT_TEST( Delete );
    CPPUNIT_TEST( GetByTimeAndEsr );
    CPPUNIT_TEST( Reload );
    CPPUNIT_TEST_SUITE_END();

private:
    void Create();
    void Change();
    void Delete();
    void GetByTimeAndEsr();
    void Reload();
};
