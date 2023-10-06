#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class GManipulationEnabler;
class TC_GManipulationEnabler :
	public TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_GManipulationEnabler );
	CPPUNIT_TEST( OnlySwitchesSelected );
	CPPUNIT_TEST( OnlySemaforsSelected );
	CPPUNIT_TEST( SwitchesNStripsSelected );
	CPPUNIT_TEST( FiguresNSemaforsSelected );
	CPPUNIT_TEST_SUITE_END();

	void OnlySwitchesSelected();
	void OnlySemaforsSelected();
	void SwitchesNStripsSelected();
	void FiguresNSemaforsSelected();

	boost::shared_ptr <GManipulationEnabler> GetEnabler() const;
};