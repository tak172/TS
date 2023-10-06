#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

class TC_HemLayer : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_HemLayer );
		CPPUNIT_TEST( BasicSetup );
		//CPPUNIT_TEST( AsyncAddRemove );
		CPPUNIT_TEST( ForEach );
		CPPUNIT_TEST( Timings );
		CPPUNIT_TEST( BadgeAndTime );
		CPPUNIT_TEST( EsrAndTime );
		CPPUNIT_TEST( GetAllPathes );
        CPPUNIT_TEST( RemoveEvents );
        CPPUNIT_TEST( ReplaceEvents );
        CPPUNIT_TEST( ReplaceEventsRefuse );
		CPPUNIT_TEST( ConcatenatePathes );
        CPPUNIT_TEST( PushBackCopy );
        CPPUNIT_TEST( PushBack );
        CPPUNIT_TEST( SeparatePathBeginning );
	CPPUNIT_TEST_SUITE_END();

private:
	void BasicSetup();
	void AsyncAddRemove();
	void ForEach();
	void Timings();
	void BadgeAndTime();
	void EsrAndTime();
	void GetAllPathes();
    void RemoveEvents();
	void ReplaceEvents();
    void ReplaceEventsRefuse();
	void ConcatenatePathes();
    void PushBackCopy();
    void PushBack();
    void SeparatePathBeginning();

    UtIdentifyCategoryList identifyCategory;
};

