#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_TechnicalTime : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_TechnicalTime );
  CPPUNIT_TEST( Including1 );
  CPPUNIT_TEST( Including2 );
  CPPUNIT_TEST( IntersectingAndNoIncluding );
  CPPUNIT_TEST( NonIntersecting );
  CPPUNIT_TEST( GettingWithUndefOddness );
  CPPUNIT_TEST( GettingWithUndefNetto );
  CPPUNIT_TEST( GettingWithUndefAssignment );
  CPPUNIT_TEST( GettingWithUndefType );
  CPPUNIT_TEST( GettingWithFullyDefined );
  CPPUNIT_TEST_SUITE_END();

private:
	void Including1();
	void Including2();
	void IntersectingAndNoIncluding();
	void NonIntersecting();
	void GettingWithUndefOddness(); //не конкретизирована четность
	void GettingWithUndefNetto(); //не конкретизировано нетто
	void GettingWithUndefAssignment(); //не конкретизировано назначение
	void GettingWithUndefType(); //не конкретизирован тип поезда
	void GettingWithFullyDefined(); //все конкретизировано
};