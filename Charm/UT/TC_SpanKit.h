#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"

class rwCoord;
class TC_SpanKit : public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_SpanKit );
	CPPUNIT_TEST( OnlyStripsSpan );
	CPPUNIT_TEST( PicketageIncreasingStripStarting );
	CPPUNIT_TEST( PicketageDecreasingStripStarting );
	CPPUNIT_TEST( PicketageIncreasingJointStarting );
	CPPUNIT_TEST( LegDirection );
	CPPUNIT_TEST( OneLinkedManyUnlinked );
	CPPUNIT_TEST( OneUnlinked );
	CPPUNIT_TEST( OneLinked );
	CPPUNIT_TEST( ManyUnlinked );
	CPPUNIT_TEST( ManyLinked );
	CPPUNIT_TEST( NoneLinked );
	CPPUNIT_TEST( CheckLockType );
	CPPUNIT_TEST( AutoOddness );
	CPPUNIT_TEST( FillForGuess );
	CPPUNIT_TEST_SUITE_END();

public:
	TC_SpanKit();

protected:
	void OnlyStripsSpan(); //перегон только с участками (без светофоров)
	void PicketageIncreasingStripStarting(); //определения возрастания пикетажа (путь начинается с участка)
	void PicketageDecreasingStripStarting(); //определения убывания пикетажа
	void PicketageIncreasingJointStarting(); //определения возрастания пикетажа (путь начинается со стыка)
	void LegDirection(); //определение направления мачты
	void OneLinkedManyUnlinked(); //один путь связан со смежной станцией связан, другие нет
	void OneUnlinked(); //один путь, не связанный со смежной станцией
	void OneLinked(); //один путь, связанный со смежной станцией
	void ManyUnlinked(); //несколько путей, не связанных со смежной станцией
	void ManyLinked(); //несколько путей, связанных со смежной станцией
	void NoneLinked(); //пути отсутствуют
	void CheckLockType(); //назначение путям перегона корректных типов блокировок
	void AutoOddness(); //автоопределение четности перегона
	void FillForGuess(); //заполнение xml-нода для Guess

	//создание трехстыкового перегонного пути, начинающегося с участка (возвращаемое значение - стартовый путь)
	CLogicElement * MakeSpanStripStarting( const rwCoord & first_joint_coord, const rwCoord & third_joint_coord );

private:
	std::wstring pckname;
};