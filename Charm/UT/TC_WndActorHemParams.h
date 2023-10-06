#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"
#include "../Actor/HemDraw/WndActorHemParams.h"

struct EsrRoute;

// тесты для класса CategoryTree

class TC_WndActorHemParams : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_WndActorHemParams );
    CPPUNIT_TEST( Load );
    CPPUNIT_TEST( CorrectMap );
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();

protected:
    void Load(); //имитация чтения из файла
    void CorrectMap();  //имитация динамической корректировки раскладки графика

private:
    EsrGuide esrGuide;
    FxA_TOPOLOGY hem_topology;
};
