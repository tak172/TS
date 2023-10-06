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

	void SerializeAny(); //��� ��������� ������
	void DeserializeAny();
	void SerializeRichRoadFully(); //���� ��������� ������ �������
	void DeserializeRichRoadFully();
	void SerializePoorRoad(); //���� ������� ������
	void DeserializePoorRoad();
	void SerializeRichRoadOneLocal(); //���� ��������� ��������� ������
	void DeserializeRichRoadOneLocal();
	void SerializeRichRoadSeveralLocals(); //��������� �������� ��������� ������
	void DeserializeRichRoadSeveralLocals();
	void SerializeRichRoadSeveralLocalsInverse(); //��� ��������� ������ �� ����������� ����������
	void DeserializeRichRoadSeveralLocalsInverse();
	void SerializeInverseRichRoadFully(); //��� ��������� ������ �� ����������� ����� ���������
	void DeserializeInverseRichRoadFully();
	void SerializeInverseSeveralRichRoadFully(); //��� ��������� ������ �� ����������� ���������� ���������
	void DeserializeInverseSeveralRichRoadFully();
	void SerializeInverseRichRoadOneLocal(); //��� ��������� ������ �� ����������� ����� ��������� ������������ ��������� ������
	void DeserializeInverseRichRoadOneLocal();
	void SerializeInverseSeveralRichRoadSeveralLocals(); //��� ��������� ������ �� ����������� ���������� �������� ������ ��������� �����
	void DeserializeInverseSeveralRichRoadSeveralLocals();
	void SerializeInverseSeveralRichRoadSeveralLocalsInverse(); //��� ��������� ������ � ��������� �������� ������ ��������� ����� (�������� �� ������� ���������)
	void DeserializeInverseSeveralRichRoadSeveralLocalsInverse();
	void SerializeInverseSeveralRichRoadMixedLocals(); //��� ��������� ������ �� ����������� ���������� �����, ��������� ��������� �������
	void DeserializeInverseSeveralRichRoadMixedLocals();
};