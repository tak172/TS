#pragma once

//тестирование LRU-кэширующей обертки

#include <cppunit/extensions/HelperMacros.h>

class TC_LRUCache : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_LRUCache );
	CPPUNIT_TEST( Caching );
	CPPUNIT_TEST( Overflow );
	CPPUNIT_TEST( NullCapacity );
	CPPUNIT_TEST( Updating );
	CPPUNIT_TEST_SUITE_END();

	void Caching();
	void Overflow();
	void NullCapacity();
	void Updating();
};