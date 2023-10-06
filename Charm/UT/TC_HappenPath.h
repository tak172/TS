#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

class TC_HappenPath: public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_HappenPath );
    CPPUNIT_TEST( Create );
    CPPUNIT_TEST( Create2 );
    CPPUNIT_TEST( BasicOperations );
    CPPUNIT_TEST( SetInfoToSpot );
    CPPUNIT_TEST( GetFragments );
    CPPUNIT_TEST( RemoveInfoFromSpot );
    CPPUNIT_TEST( TrainDescriptions );
    CPPUNIT_TEST( Duplicates );
    CPPUNIT_TEST( CanReplace );
    CPPUNIT_TEST( ReplaceEvents );
    CPPUNIT_TEST( ReplaceEventsFragments );
    CPPUNIT_TEST( Concatenate );
    CPPUNIT_TEST( Concatenate2 );
    CPPUNIT_TEST( ConcatenateEmptyTailFragment );
    CPPUNIT_TEST( ReplaceLastEventFragment );
    CPPUNIT_TEST( InsertSpots );
    CPPUNIT_TEST( SelectStationEvent );
    CPPUNIT_TEST( Directions );
    CPPUNIT_TEST( SpanDirections );
    CPPUNIT_TEST( SeparateBeginning );
    CPPUNIT_TEST( IdentifyWith );
    CPPUNIT_TEST( IdentifyDublicates );
    CPPUNIT_TEST( RemoveFragmentsIf );
    CPPUNIT_TEST( FindDistrictBorders );
    CPPUNIT_TEST( FindDistrictBordersWithoutSpan );
    CPPUNIT_TEST( FindDistrictBordersLeavingDistrict );
    CPPUNIT_TEST( FindDistrictBordersEntrySpot );
    CPPUNIT_TEST( FindDistrictBorders_3997 );
    CPPUNIT_TEST( IdentifyWithOtherLiteral );
    CPPUNIT_TEST( IdentifyThroughTwoLiterals );
    CPPUNIT_TEST( EmptyTrainDescriptions );
    CPPUNIT_TEST( SetInfoToSpotsRange );
    CPPUNIT_TEST_SUITE_END();

private:
    void tearDown() override;
    void Create();
    void Create2();
    void BasicOperations();
    void SetInfoToSpot();
    void GetFragments();
    void RemoveInfoFromSpot();
    void TrainDescriptions();
    void Duplicates();
    void CanReplace();
    void ReplaceEvents();
    void ReplaceEventsFragments();
    void Concatenate();
    void Concatenate2();
    void ConcatenateEmptyTailFragment();
    void ReplaceLastEventFragment();
    void InsertSpots();
    void SelectStationEvent();
    void Directions();
    void SpanDirections();
    void SeparateBeginning();
    void IdentifyWith();
    void IdentifyDublicates();
    void RemoveFragmentsIf();
    void FindDistrictBorders();
    void FindDistrictBordersWithoutSpan();
    void FindDistrictBordersLeavingDistrict();
    void FindDistrictBordersEntrySpot();
    void FindDistrictBorders_3997();
    void IdentifyWithOtherLiteral();
    void IdentifyThroughTwoLiterals();
    void EmptyTrainDescriptions();
    void SetInfoToSpotsRange();

    UtIdentifyCategoryList identifyCategory;
};

