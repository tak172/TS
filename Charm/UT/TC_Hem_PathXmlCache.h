#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

class TopologyTest;

class TC_Hem_PathXmlCache : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_PathXmlCache );
    CPPUNIT_TEST( ChangesHappenLayer );
    CPPUNIT_TEST( ChangesAllEvents );
    CPPUNIT_TEST( ChangesFutureLayer );
    CPPUNIT_TEST( ReplaceSingleEvent );
    CPPUNIT_TEST( ChangesBySplit );
    CPPUNIT_TEST( ChangesByConcatenate );
    CPPUNIT_TEST( Auto2User );
    CPPUNIT_TEST( Auto2User_Misplaced );
    CPPUNIT_TEST( MultiReplaceSingleEvent );
    CPPUNIT_TEST( Auto2User_IdentifyNonexistent );
    CPPUNIT_TEST( AddManyEventsInHead );
	CPPUNIT_TEST_SUITE_END();

    void setTopologyPath( unsigned from, unsigned to, TopologyTest* topology, unsigned third = 1379 );

private:
    void tearDown() override;
    void ChangesHappenLayer(); // проверка получения изменений из HappenLayer
    void ChangesAllEvents();   // проверка изменений сразу всех событий нити
    void ChangesFutureLayer(); // проверка генерации изменений для слоя FutureLayer
    void ReplaceSingleEvent(); // проверка изменений при подмене единственного события
    void ChangesBySplit(); // изменения вызванные разделением нитей
    void ChangesByConcatenate(); // изменения вызванные склейкой нитей
    void Auto2User(); // передача изменений из AutoChart в UserChart
    void Auto2User_Misplaced(); // передача изменений из AutoChart в UserChart (исчезает препозиция)
    void MultiReplaceSingleEvent(); // несколько точечных изменений попрождают несколько отдельных Trio
    void Auto2User_IdentifyNonexistent(); // идентификация несуществующего события
    void AddManyEventsInHead(); // ошибка добавления нескольких событий в начало нити

    UtIdentifyCategoryList identifyCategory;
};
