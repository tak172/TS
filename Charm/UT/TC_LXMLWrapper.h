#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Logic.h"

class TC_LXMLWrapper : 
	private TC_Logic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_LXMLWrapper );
	CPPUNIT_TEST( SaveStationCommon );
	CPPUNIT_TEST( SaveSpanCommon );
	CPPUNIT_TEST( LoadStationCommon );
	CPPUNIT_TEST( LoadSpanCommon );
	CPPUNIT_TEST( SaveLoadObjects );
	CPPUNIT_TEST( SaveStrip );
	CPPUNIT_TEST( LoadStrip );
	CPPUNIT_TEST( SaveJoint );
	CPPUNIT_TEST( LoadJoint );
	CPPUNIT_TEST( LoadJointWithInvalidPicketage );
	CPPUNIT_TEST( SaveLandscape );
	CPPUNIT_TEST( LoadLandscape );
	CPPUNIT_TEST( LoadStationWithInvalidAxis );
	CPPUNIT_TEST( SaveWaysLocks );
	CPPUNIT_TEST( LoadWaysLocks );
	CPPUNIT_TEST_SUITE_END();

    void setUp() override;
    void tearDown() override;
protected:
	void SaveStationCommon();
	void SaveSpanCommon();
	void LoadStationCommon();
	void LoadSpanCommon();
	void SaveLoadObjects();
	void SaveStrip();
	void LoadStrip();
	void SaveJoint();
	void LoadJoint();
	void LoadJointWithInvalidPicketage();
	void SaveLandscape();
	void LoadLandscape();
	void LoadStationWithInvalidAxis();
	void SaveWaysLocks();
	void LoadWaysLocks();
	CLogicElement * CreateUncommonObject( ELEMENT_CLASS ) const;
};
