#include "stdafx.h"
#include "TC_GuessTransciever.h"
#include "../Guess/Msg.h"
#include "../Guess/TrackerEvent.h"
#include "../UT/TC_ChartImpl.h"
#include "../Hem/EventTrio.h"
#include "../Hem/HappenPath.h"

using namespace std;
using namespace HemHelpful;
typedef std::shared_ptr<Hem::HappenPath> PathPtr;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GuessTransciever );

void TC_GuessTransciever::TestReduce()
{
    unsigned int const id0 = 64;
    unsigned int const id1 = 65;
    unsigned int const id2 = 66;
    time_t const curtime = 1234000;
    // имитируем формирование и расформирование нескольких поездов в разное время
    const BadgeE bdg( L"1путь", EsrKit(11002) );

    SpotEventPtr id0_f( new TrackerEvent( id0, HCode::FORM,    bdg, curtime+0  ) );
    SpotEventPtr id0_d( new TrackerEvent( id0, HCode::DEATH,   bdg, curtime+7   ) );
    PathPtr path0(new Hem::HappenPath(id0_f));
    path0->DoAppend(id0_d);

    SpotEventPtr id1_f( new TrackerEvent( id1, HCode::FORM,    bdg, curtime+200 ) );
    SpotEventPtr id1_d( new TrackerEvent( id1, HCode::DEATH,   bdg, curtime+207 ) );
    PathPtr path1(new Hem::HappenPath(id1_f));
    path1->DoAppend(id1_d);

    SpotEventPtr id2_f( new TrackerEvent( id2, HCode::FORM,    bdg, curtime+100 ) );
    SpotEventPtr id2_d( new TrackerEvent( id2, HCode::DEATH,   bdg, curtime+107 ) );
    PathPtr path2(new Hem::HappenPath(id2_f));
    path2->DoAppend(id2_d);

    CPPUNIT_ASSERT(guessTransciever.registerPath(path0, id0));
    guessTransciever.transformToDisformedPath(path0);
    CPPUNIT_ASSERT(guessTransciever.getLastEvent(id0));

    CPPUNIT_ASSERT(guessTransciever.registerPath(path1, id1));
    guessTransciever.transformToDisformedPath(path1);
    CPPUNIT_ASSERT(guessTransciever.getLastEvent(id1));

    CPPUNIT_ASSERT(guessTransciever.registerPath(path2, id2));
    guessTransciever.transformToDisformedPath(path2);
    CPPUNIT_ASSERT(guessTransciever.getLastEvent(id2));

    guessTransciever.reduceAmount( curtime+150 );
    CPPUNIT_ASSERT(!guessTransciever.getLastEvent(id0));
    CPPUNIT_ASSERT(guessTransciever.getLastEvent(id1));
    CPPUNIT_ASSERT(!guessTransciever.getLastEvent(id2));
}

void TC_GuessTransciever::TestPathChange()
{
    unsigned const id0 = 64;

    const BadgeE bdg(L"1путь", EsrKit(11002));

    SpotEventPtr s0_f( new SpotEvent(HCode::FORM, bdg, 100));
    PathPtr path0(new Hem::HappenPath(s0_f));

    // Регистрация новой нитки с новым номером
    CPPUNIT_ASSERT(guessTransciever.registerPath(path0, id0));
    guessTransciever.transformToDisformedPath(path0);
    CPPUNIT_ASSERT(guessTransciever.isPathTracked(path0));
    CPPUNIT_ASSERT_EQUAL(id0, guessTransciever.getPathId(path0));

    SpotEventPtr s1_f(new SpotEvent(HCode::FORM, bdg, 200));
    PathPtr path1(new Hem::HappenPath(s1_f));
    
    // Регистрация новой нитки с существующим номером
    CPPUNIT_ASSERT(guessTransciever.registerPath(path1, id0));
    guessTransciever.transformToDisformedPath(path1);
    CPPUNIT_ASSERT(guessTransciever.isPathTracked(path1));
    CPPUNIT_ASSERT(!guessTransciever.isPathTracked(path0));
    CPPUNIT_ASSERT_EQUAL(id0, guessTransciever.getPathId(path1));
    CPPUNIT_ASSERT_EQUAL(0u, guessTransciever.getPathId(path0));

    // Регистрация существующей нитки с существующим номером
    CPPUNIT_ASSERT(guessTransciever.registerPath(path1, id0));
    guessTransciever.transformToDisformedPath(path1);
    CPPUNIT_ASSERT(guessTransciever.isPathTracked(path1));
    CPPUNIT_ASSERT_EQUAL(id0, guessTransciever.getPathId(path1));

    unsigned const id1 = 65;
    // Регистрация существующей нитки с новым номером - неудачно, остается на старом идентификаторе
    CPPUNIT_ASSERT(!guessTransciever.registerPath(path1, id1));
    guessTransciever.transformToDisformedPath(path1);
    CPPUNIT_ASSERT(guessTransciever.isPathTracked(path1));
    CPPUNIT_ASSERT(id0 == guessTransciever.getPathId(path1));
    
    // Не реализовано
    // CPPUNIT_ASSERT_EQUAL(id1, guessTransciever.getPathId(path1));
}