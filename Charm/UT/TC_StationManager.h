#pragma once

#include "TC_Project.h"

class CLogicDocumentTest;
class TC_StationManager : public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_StationManager );
	CPPUNIT_TEST( StationByLe );
	CPPUNIT_TEST( EsrChanging );
	CPPUNIT_TEST_SUITE_END();

protected:
	void StationByLe();
	void EsrChanging();

	typedef boost::shared_ptr <CLogicDocumentTest> LogDocTestPtr;
	LogDocTestPtr AddStation( const EsrKit & );
	LogDocTestPtr AddSpan( const EsrKit & esrcode1, const EsrKit & esrcode2 );
};
