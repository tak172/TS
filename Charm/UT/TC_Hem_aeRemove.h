#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Attic.h"
#include "../Hem/GuessTransciever.h"
#include "UtHemHelper.h"

class HappenLayer;

class TC_Hem_aeRemove : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeRemove );
    CPPUNIT_TEST( events13 );
    CPPUNIT_TEST( events12 );
    CPPUNIT_TEST( events11 );
    CPPUNIT_TEST( events10 );
    CPPUNIT_TEST( events9 );
    CPPUNIT_TEST( events8 );
    CPPUNIT_TEST( events7 );
    CPPUNIT_TEST( events6 );
    CPPUNIT_TEST( events5 );
    CPPUNIT_TEST( events4 );
    CPPUNIT_TEST( events3 );
    CPPUNIT_TEST( events2 );
    CPPUNIT_TEST( events1 );
    CPPUNIT_TEST( DisformEnded );
    CPPUNIT_TEST( UncontrolledZoneEnded );
    CPPUNIT_TEST( GuessUnknownedPath );

    CPPUNIT_TEST( RemoveFutureEvent );
    CPPUNIT_TEST( RemoveFutureEventOnBoardStation );
	CPPUNIT_TEST_SUITE_END();
public:
    static void collectUndo( attic::a_document& undoer, HappenLayer& happen_layer );
    static bool implementUndo( attic::a_document& undoer, HappenLayer& hl );

private:
    void events13();
    void events12();
    void events11();
    void events10();
    void events9();
    void events8();
    void events7();
    void events6();
    void events5();
    void events4();
    void events3();
    void events2();
    void events1();
    void DisformEnded(); // оканчивающаяся на Disform
    void UncontrolledZoneEnded(); // оканчивающаяся в неконтролируемой зоне
    void GuessUnknownedPath(); // нить не ведется отслеживанием
    void RemoveFutureEvent();   //удаление прогнозного события
    void RemoveFutureEventOnBoardStation(); //удаление прогнозного события на граничной станции (примыкает к контролируемому перегону)

    void makeStageData( unsigned count_of_event );
    void runStage( const std::string& cutted );

    Hem::GuessTransciever guessTransciever;
    UtIdentifyCategoryList identifyCategory;

    attic::a_document doc;
};
