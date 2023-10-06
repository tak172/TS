#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_ScheduledLayer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_ScheduledLayer );
    CPPUNIT_TEST( Serialization );
    CPPUNIT_TEST( Generation );
    CPPUNIT_TEST( GenerationCutting );
    CPPUNIT_TEST( GenerationMerging );
    CPPUNIT_TEST( GenerationAdding );
    CPPUNIT_TEST( SerializationAfterGeneration );
    CPPUNIT_TEST( Serialization2 );
    CPPUNIT_TEST( TrimPlanned );
    CPPUNIT_TEST( EliminatePlatforms );
    CPPUNIT_TEST( ParkWaySerialization );
    CPPUNIT_TEST( ParkWayGeneration );
    CPPUNIT_TEST( CalendarSerialization );
    CPPUNIT_TEST( GenerationByCalendar );
    CPPUNIT_TEST_SUITE_END();

    void Serialization();      // ���� ������������ � �������������� ����
    void Generation();         // ���� ��������� ����� ��� ����� ������ �� �������
    void GenerationCutting();  // ���� ��������� ����� � ������ ������ �� �������
    void GenerationMerging();  // ���� ��������� ����� �� ���� ����������
    void GenerationAdding();   // ���� ��������� ����� �� ���� ����������
    void SerializationAfterGeneration(); // ���� ������������ ���� ����� ��������� ������ �� ��������� �����
    void Serialization2();     // ���� ������������ ��������������� ����� ������� � ������������
    void TrimPlanned();        // ���� �������� ������ ������� ���������� (���������� �� �����������)
    void EliminatePlatforms(); // ���� �������� �������� �� ��������������� �����
    void ParkWaySerialization();  // ����� ���� - ������������
    void ParkWayGeneration();     // ����� ���� - ���������
    void CalendarSerialization(); // ���������� ���������
    void GenerationByCalendar();// ���������� ���������
};

