#ifndef TC_STATIONCONTROL_H
#define TC_STATIONCONTROL_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Attic.h"
#include "../TeleControl/StationControl.h"

/// \brief тесты для Mission::StationControl_Base
class TC_StationControl : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_StationControl );
  CPPUNIT_TEST( Load_Simple );
  CPPUNIT_TEST( Load_Panel );
  CPPUNIT_TEST( Load_Compound );
  CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp();
	virtual void tearDown();

protected:
	void Load_Simple( );
    void Load_Compound();
	void Load_Panel();

private:
    attic::a_document x_doc;
    Mission::StationControl msc;
};


#endif // TC_STATIONCONTROL_H