#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Attic.h"
#include "UtTearDown.h"

class HappenLayer;
class UtUserChart;

class TC_Hem_aeCutUnidentify : public UtTearDowner
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeCutUnidentify );
    CPPUNIT_TEST( SpanStation );
    //CPPUNIT_TEST( CheckOrderPathesUndoCommand );
	CPPUNIT_TEST_SUITE_END();

private:
    void SpanStation();
    void CheckOrderPathesUndoCommand();

    std::wstring CutOperationsAndGetUndoString(UtUserChart &userChart, bool bDescending);

};
