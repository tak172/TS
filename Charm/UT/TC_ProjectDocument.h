#pragma once

#include "TC_Project.h"

class TC_ProjectDocument : public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_ProjectDocument );
	CPPUNIT_TEST( Unjoin );
	CPPUNIT_TEST_SUITE_END();

	void Unjoin();
};