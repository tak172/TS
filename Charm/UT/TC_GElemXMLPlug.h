#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_GElemXMLPlug : 
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_GElemXMLPlug );
	CPPUNIT_TEST( SaveElement );
	CPPUNIT_TEST( LoadElement );
	CPPUNIT_TEST( Save2dElement );
	CPPUNIT_TEST( Load2dElement );
	CPPUNIT_TEST( SaveBoard );
    CPPUNIT_TEST( LoadBoard );
    CPPUNIT_TEST( CorrectDirtyBoard );
	CPPUNIT_TEST( SaveMultiBoard );
	CPPUNIT_TEST( LoadMultiBoard );
	CPPUNIT_TEST( SaveTableBoard );
	CPPUNIT_TEST( LoadTableBoard );
	CPPUNIT_TEST( SaveStrip );
	CPPUNIT_TEST( LoadStrip );
	CPPUNIT_TEST( SaveGText );
	CPPUNIT_TEST( LoadGText );
	CPPUNIT_TEST( SaveLandscape );
	CPPUNIT_TEST( LoadLandscape );
	CPPUNIT_TEST( ObsoleteLoad );
	CPPUNIT_TEST_SUITE_END();

protected:
	void SaveElement();
	void LoadElement();
	void Save2dElement();
	void Load2dElement();
	void SaveBoard();
	void LoadBoard();
    void CorrectDirtyBoard();
	void SaveMultiBoard();
	void LoadMultiBoard();
	void SaveTableBoard();
	void LoadTableBoard();
	void SaveStrip();
	void LoadStrip();
	void SaveGText();
	void LoadGText();
	void SaveLandscape();
	void LoadLandscape();
	void ObsoleteLoad();
};
