#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_RoadKit : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_RoadKit );
	CPPUNIT_TEST( SerializeAny );
	CPPUNIT_TEST( DeserializeAny );
	CPPUNIT_TEST( SerializeRichRoadFully );
	CPPUNIT_TEST( DeserializeRichRoadFully );
	CPPUNIT_TEST( SerializePoorRoad );
	CPPUNIT_TEST( DeserializePoorRoad );
	CPPUNIT_TEST( SerializeRichRoadOneLocal );
	CPPUNIT_TEST( DeserializeRichRoadOneLocal );
	CPPUNIT_TEST( SerializeRichRoadSeveralLocals );
	CPPUNIT_TEST( DeserializeRichRoadSeveralLocals );
	CPPUNIT_TEST( SerializeRichRoadSeveralLocalsInverse );
	CPPUNIT_TEST( DeserializeRichRoadSeveralLocalsInverse );
	CPPUNIT_TEST( SerializeInverseRichRoadFully );
	CPPUNIT_TEST( DeserializeInverseRichRoadFully );
	CPPUNIT_TEST( SerializeInverseSeveralRichRoadFully );
	CPPUNIT_TEST( DeserializeInverseSeveralRichRoadFully );
	CPPUNIT_TEST( SerializeInverseRichRoadOneLocal );
	CPPUNIT_TEST( DeserializeInverseRichRoadOneLocal );
	CPPUNIT_TEST( SerializeInverseSeveralRichRoadSeveralLocals );
	CPPUNIT_TEST( DeserializeInverseSeveralRichRoadSeveralLocals );
	CPPUNIT_TEST( SerializeInverseSeveralRichRoadSeveralLocalsInverse );
	CPPUNIT_TEST( DeserializeInverseSeveralRichRoadSeveralLocalsInverse );
	CPPUNIT_TEST( SerializeInverseSeveralRichRoadMixedLocals );
	CPPUNIT_TEST( DeserializeInverseSeveralRichRoadMixedLocals );
	CPPUNIT_TEST_SUITE_END();

	void SerializeAny(); //все возможные дороги
	void DeserializeAny();
	void SerializeRichRoadFully(); //одна составная дорога целиком
	void DeserializeRichRoadFully();
	void SerializePoorRoad(); //одна простая дорога
	void DeserializePoorRoad();
	void SerializeRichRoadOneLocal(); //одна поддорога составной дороги
	void DeserializeRichRoadOneLocal();
	void SerializeRichRoadSeveralLocals(); //несколько поддорог составной дороги
	void DeserializeRichRoadSeveralLocals();
	void SerializeRichRoadSeveralLocalsInverse(); //вся составная дорога за исключением нескольких
	void DeserializeRichRoadSeveralLocalsInverse();
	void SerializeInverseRichRoadFully(); //все возможные дороги за исключением одной составной
	void DeserializeInverseRichRoadFully();
	void SerializeInverseSeveralRichRoadFully(); //все возможные дороги за исключением нескольких составных
	void DeserializeInverseSeveralRichRoadFully();
	void SerializeInverseRichRoadOneLocal(); //все возможные дороги за исключением одной поддороги определенной составной дороги
	void DeserializeInverseRichRoadOneLocal();
	void SerializeInverseSeveralRichRoadSeveralLocals(); //все возможные дороги за исключением нескольких поддорог разных составных дорог
	void DeserializeInverseSeveralRichRoadSeveralLocals();
	void SerializeInverseSeveralRichRoadSeveralLocalsInverse(); //все возможные дороги и несколько поддорог разных составных дорог (проверка на двойное отрицание)
	void DeserializeInverseSeveralRichRoadSeveralLocalsInverse();
	void SerializeInverseSeveralRichRoadMixedLocals(); //все возможные дороги за исключением нескольких дорог, описанных смешанных образом
	void DeserializeInverseSeveralRichRoadMixedLocals();
};