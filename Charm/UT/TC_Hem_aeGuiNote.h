#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Hem_aeGuiNote : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Hem_aeGuiNote );
    CPPUNIT_TEST( Create );
	CPPUNIT_TEST( CreateDelay );
//     CPPUNIT_TEST( Replace );
//     CPPUNIT_TEST( Remove );
    CPPUNIT_TEST_SUITE_END();

private:
    void Create();  
	void CreateDelay();
//     void Replace(); 
//     void Remove();  
};
