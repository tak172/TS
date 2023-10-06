#include "stdafx.h"
#include "TC_LRUCache.h"
#include "../helpful/LRUCache.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LRUCache );

//класс-вычислитель (умножает на два)
class By2Multiplier
{
public:
	unsigned int mulitply( unsigned int key ) const
	{
		++calculationsNum;
		return key * 2;
	}
	unsigned int calcCount() const 
	{
		unsigned int retval = calculationsNum;
		calculationsNum = 0;
		return retval;
	}

private:
	static unsigned int calculationsNum; //число обращений к функции вычисления
};

unsigned int By2Multiplier::calculationsNum = 0;

typedef LRUCache<unsigned int, unsigned int, By2Multiplier> LRUCachedMultiplier;

unsigned int LRUCachedMultiplier::calculate( const unsigned int & key ) const
{
	return containerPtr->mulitply( key );
}

void TC_LRUCache::Caching()
{
	shared_ptr<By2Multiplier> multiplierPtr( new By2Multiplier() );
	LRUCachedMultiplier cachingMultiplier( multiplierPtr, 5 );
	cachingMultiplier.get( 2 );
	cachingMultiplier.get( 3 );
	cachingMultiplier.get( 4 );
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 3 ); //все три запроса вычислялись. в кэше 3 элемента
	cachingMultiplier.get( 4 );
	cachingMultiplier.get( 3 );
	cachingMultiplier.get( 1 );
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 1 ); //только один запрос вычислялся. в кэше 4 элемента
	cachingMultiplier.get( 5 );
	cachingMultiplier.get( 2 );
	cachingMultiplier.get( 4 );
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 1 ); //только один запрос вычислялся. в кэше 5 элементов
	auto res1 = cachingMultiplier.get( 1 );
	auto res2 = cachingMultiplier.get( 2 );
	auto res3 = cachingMultiplier.get( 3 );
	auto res4 = cachingMultiplier.get( 4 );
	auto res5 = cachingMultiplier.get( 5 );
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 0 ); //ни один запрос вычислялся. в кэше 5 элементов
	CPPUNIT_ASSERT( res1 == 2 && res2 == 4 && res3 == 6 && res4 == 8 && res5 == 10 );
}

void TC_LRUCache::Overflow()
{
	shared_ptr<By2Multiplier> multiplierPtr( new By2Multiplier() );
	LRUCachedMultiplier cachingMultiplier( multiplierPtr, 2 );
	cachingMultiplier.get( 2 );
	cachingMultiplier.get( 3 );
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 2 ); //два запроса вычислялись. в кэше 2 элемента
	cachingMultiplier.get( 4 );
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 1 ); //переполнение. запрос вычисляется. в кэше 2 элемента
	cachingMultiplier.get( 2 ); 
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 1 ); //запрос вычисляется, т.к. элемент уже вытеснен. в кэше 2 элемента
}

void TC_LRUCache::NullCapacity()
{
	shared_ptr<By2Multiplier> multiplierPtr( new By2Multiplier() );
	LRUCachedMultiplier cachingMultiplier( multiplierPtr, 0 );
	cachingMultiplier.get( 2 );
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 1 );
	cachingMultiplier.get( 2 );
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 1 ); //кэширование не работает
}

void TC_LRUCache::Updating()
{
	shared_ptr<By2Multiplier> multiplierPtr( new By2Multiplier() );
	LRUCachedMultiplier cachingMultiplier( multiplierPtr, 5 );
	cachingMultiplier.get( 2 );
	cachingMultiplier.get( 3 );
	cachingMultiplier.get( 4 );
	cachingMultiplier.get( 3 );
	cachingMultiplier.get( 2 );
	cachingMultiplier.get( 1 );
	cachingMultiplier.get( 3 );
	cachingMultiplier.get( 5 );
	cachingMultiplier.get( 6 );
	cachingMultiplier.get( 7 );
	multiplierPtr->calcCount(); //сброс
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 0 );
	cachingMultiplier.get( 2 ); //элемент вытеснен, поэтому вычисляется
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 1 );
	cachingMultiplier.get( 4 ); //элемент вытеснен, поэтому вычисляется
	CPPUNIT_ASSERT( multiplierPtr->calcCount() == 1 );
}