#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"


class TC_LogicSpanWay : public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_LogicSpanWay );
	CPPUNIT_TEST( NonullLengthWay );
	CPPUNIT_TEST( UndefLengthWay );
	CPPUNIT_TEST( LengthMiniWayS );
	CPPUNIT_TEST( LengthMiniWayJ );
	CPPUNIT_TEST( LengthMiniWaySJ );
	CPPUNIT_TEST( LengthMiniWaySJS );
	CPPUNIT_TEST( LengthMiniWayJSJ );
	CPPUNIT_TEST_SUITE_END();


protected:
	void NonullLengthWay(); //путь перегона с ненулевой длиной
	void UndefLengthWay(); //путь перегона с неопределенной длиной
	//маленькие перегонные пути:
	void LengthMiniWayS(); //участок
	void LengthMiniWayJ(); //стык
	void LengthMiniWaySJ(); //участок - стык
	void LengthMiniWaySJS(); //и т.д.
	void LengthMiniWayJSJ();
};