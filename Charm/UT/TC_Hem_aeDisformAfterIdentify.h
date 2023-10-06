#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/HappenLayer.h"
#include "UtTearDown.h"

class TC_Hem_aeDisformAfterIdentify : public UtTearDowner
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeDisformAfterIdentify );
    CPPUNIT_TEST( CutPathEndsOnArrival );
	CPPUNIT_TEST_SUITE_END();

public:

private:
    void CutPathEndsOnArrival();          // разрыв хвоста нити от станционного события до SpanMove
};
