#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Signal.h"

class TC_SXMLWrapper : 
	private TC_Signal,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_SXMLWrapper );
	CPPUNIT_TEST( BasicSave );
	CPPUNIT_TEST( SaveLts );
	CPPUNIT_TEST( SaveVirtual );
	CPPUNIT_TEST( SaveFormula );
	CPPUNIT_TEST( BasicLoad );
	CPPUNIT_TEST( LoadLts );
	CPPUNIT_TEST( LoadVirtual );
	CPPUNIT_TEST( LoadFormula );

	CPPUNIT_TEST_SUITE_END();

protected:
	void BasicSave(); //сохранение общей части (станция, абонент, формат)
	void SaveLts(); //сохранение Lts-сигнала
	void SaveVirtual(); //сохранение виртуального сигнала (Ebiloc)
	void SaveFormula(); //сохранение формулы
	void BasicLoad();
	void LoadLts();
	void LoadVirtual();
	void LoadFormula();
};