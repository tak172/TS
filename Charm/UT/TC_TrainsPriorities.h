#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/TrainCharacteristics.h"

class TrainDescr;
class TC_TrainsPriorities : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_TrainsPriorities );
	CPPUNIT_TEST( DefaultSorting );
	CPPUNIT_TEST( DefaultSorting2 );
	CPPUNIT_TEST( OffsetsSerialize );
	CPPUNIT_TEST( OffsetsDeserialize );
	CPPUNIT_TEST( OffsetsManipulations );
	CPPUNIT_TEST( OffsetsSorting1 );
	CPPUNIT_TEST( OffsetsSorting2 );
	CPPUNIT_TEST( OffsetsSorting3 );
	CPPUNIT_TEST( OffsetsSorting4 );
	CPPUNIT_TEST( OffsetsSorting5 );
	CPPUNIT_TEST( OffsetsSorting6 );
	CPPUNIT_TEST( OffsetsSorting7 );
	CPPUNIT_TEST( OffsetsSorting8 );
	CPPUNIT_TEST( OffsetsSorting9 );
	CPPUNIT_TEST( OffsetsSorting10 );
	CPPUNIT_TEST_SUITE_END();

private:
	void DefaultSorting(); //умолчательна€ сортировка (без доп.смещений)
	void DefaultSorting2();
	void OffsetsSerialize(); //сериализаци€ объекта TrainsPriorityOffsets
	void OffsetsDeserialize(); //десереализаци€ объекта TrainsPriorityOffsets
	void OffsetsManipulations(); //пользовательские манипул€ции с объектом TrainsPriorityOffsets
	void OffsetsSorting1(); //сортировка с доп.смещени€ми
	void OffsetsSorting2();
	void OffsetsSorting3();
	void OffsetsSorting4();
	void OffsetsSorting5();
	void OffsetsSorting6();
	void OffsetsSorting7();
	void OffsetsSorting8();
	void OffsetsSorting9();
	void OffsetsSorting10();

	std::shared_ptr<const TrainDescr> MakeAnyDescr( const TrainCharacteristics::TrainFeature & purpose ) const;
};