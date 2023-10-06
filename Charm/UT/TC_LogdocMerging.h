#pragma once

#include "TC_Project.h"

class TC_LogdocMerging : public TC_Project
{

private:
	CPPUNIT_TEST_SUITE( TC_LogdocMerging );
	CPPUNIT_TEST( Merging );
	CPPUNIT_TEST( Mergability );
	CPPUNIT_TEST_SUITE_END();

	void Merging();
	void Mergability();
};