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

    void Serialization();      // Тест сериализации и десериализации слоя
    void Generation();         // Тест генерации ниток без учета срезки по времени
    void GenerationCutting();  // Тест генерации ниток с учетом срезки по времени
    void GenerationMerging();  // Тест генерации ниток из двух расписаний
    void GenerationAdding();   // Тест генерации ниток из двух расписаний
    void SerializationAfterGeneration(); // Тест сериализации слоя после получения заказа на генерацию ниток
    void Serialization2();     // Тест сериализации сгенерированных ниток наравне с расписаниями
    void TrimPlanned();        // Тест удаления старых дневных расписаний (вариантные не проверяются)
    void EliminatePlatforms(); // Тест удаления платформ из сгенерированных ниток
    void ParkWaySerialization();  // парки пути - сериализация
    void ParkWayGeneration();     // парки пути - генерация
    void CalendarSerialization(); // применение календаря
    void GenerationByCalendar();// применение календаря
};

