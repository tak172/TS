#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/HemHelpful.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/UncontrolledStations.h"
#include "../Hem/GuessTransciever.h"
#include "../helpful/EsrGuide.h"
#include "UtHemHelper.h"

class Topology;
class GuessTransciever;

class TC_Hem_aeReplace : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeReplace );
    CPPUNIT_TEST( CoveredPreposition );
    CPPUNIT_TEST( AppendBack );
    CPPUNIT_TEST( RemoveInner );
    CPPUNIT_TEST( InsertInner );
    CPPUNIT_TEST( InsertWrongway );
    CPPUNIT_TEST( InsertWrongway_2 );
    CPPUNIT_TEST( InsertWrongway_4310 );
    CPPUNIT_TEST( insertDuplicate_4431 );
    CPPUNIT_TEST( ReplaceBadEvent );
    CPPUNIT_TEST( ReplaceFutureEvent );
    CPPUNIT_TEST( ReplaceFutureChronoBreak );
    CPPUNIT_TEST( UndoCommandWithReplaceByRectify );
    CPPUNIT_TEST( WrongUndoCommandWithDublicat );
    CPPUNIT_TEST( ReplaceArrivalDepartureTheSameTime );

	CPPUNIT_TEST_SUITE_END();

private:
    void tearDown() override;
	void CoveredPreposition();
    void AppendBack();
    void RemoveInner();
    void InsertInner();
    void InsertWrongway();
    void InsertWrongway_2();
    void InsertWrongway_4310();
    void insertDuplicate_4431();
    void ReplaceBadEvent();
    void ReplaceFutureEvent();
    void ReplaceFutureChronoBreak();
    void UndoCommandWithReplaceByRectify();
    void WrongUndoCommandWithDublicat();
    void ReplaceArrivalDepartureTheSameTime();

    EsrGuide eg;
    std::shared_ptr<Topology> topology;
    Hem::GuessTransciever guessTransciever;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;
};
