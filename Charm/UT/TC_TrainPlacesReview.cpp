#include "stdafx.h"
#include "Parody.h"
#include "TC_TrainPlacesReview.h"
#include "../Fund/TrainPlacesReview.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrainPlacesReview );

void TC_TrainPlacesReview::PlaceTrain()
{
	TrainPlacesReview tpreview;
	BadgeE placeBadge( L"PlcName", EsrKit( 10 ) );
	unsigned int trainIden = 250;
	TrainDescrPtr tdescrPtr( new TrainDescr( L"2454", L"300-400-500" ) );
	time_t placeTime( 300 );
	tpreview.DoAppear( placeBadge, trainIden, tdescrPtr, &placeTime );

	auto retdescrPtr = tpreview.GetDescrPtr( trainIden );
	CPPUNIT_ASSERT( retdescrPtr && *retdescrPtr == *tdescrPtr );
	const auto & retPlace = tpreview.GetPlace( trainIden );
	CPPUNIT_ASSERT( retPlace.size() == 1 && retPlace.front() == placeBadge );
	const auto & retTime = tpreview.GetTime( trainIden );
	CPPUNIT_ASSERT( placeTime == retTime );
	const auto & retUnityInfo = tpreview.GetUnityInfo( placeBadge );
	CPPUNIT_ASSERT( retUnityInfo.first && *retUnityInfo.first == *tdescrPtr && retUnityInfo.second == trainIden );
}

void TC_TrainPlacesReview::EraseTrain()
{
	TrainPlacesReview tpreview;
	BadgeE placeBadge( L"PlcName", EsrKit( 10 ) );
	unsigned int trainIden = 250;
	TrainDescrPtr tdescrPtr( new TrainDescr( L"2454", L"300-400-500" ) );
	time_t placeTime( 300 );
	tpreview.DoAppear( placeBadge, trainIden, tdescrPtr, &placeTime );
	tpreview.DoDisappear( placeBadge, trainIden );

	auto retdescrPtr = tpreview.GetDescrPtr( trainIden );
	CPPUNIT_ASSERT( !retdescrPtr );
	const auto & retPlace = tpreview.GetPlace( trainIden );
	CPPUNIT_ASSERT( retPlace.empty() );
	const auto & retTime = tpreview.GetTime( trainIden );
	CPPUNIT_ASSERT( retTime == 0 );
	const auto & retUnityInfo = tpreview.GetUnityInfo( placeBadge );
	CPPUNIT_ASSERT( !retUnityInfo.first && retUnityInfo.second == 0 );
}


void TC_TrainPlacesReview::PlaceShunting()
{
	TrainPlacesReview tpreview;
	BadgeE placeBadge( L"PlcName", EsrKit( 10 ) );
	unsigned int trainIden = 250;
	time_t placeTime( 300 );
	tpreview.DoAppear( placeBadge, trainIden, nullptr, &placeTime );

	auto retdescrPtr = tpreview.GetDescrPtr( trainIden );
	CPPUNIT_ASSERT( !retdescrPtr );
	const auto & retPlace = tpreview.GetPlace( trainIden );
	CPPUNIT_ASSERT( retPlace.size() == 1 && retPlace.front() == placeBadge );
	const auto & retTime = tpreview.GetTime( trainIden );
	CPPUNIT_ASSERT( placeTime == retTime );
	const auto & retUnityInfo = tpreview.GetUnityInfo( placeBadge );
	CPPUNIT_ASSERT( !retUnityInfo.first && retUnityInfo.second == trainIden );
}

void TC_TrainPlacesReview::EraseShunting()
{
	TrainPlacesReview tpreview;
	BadgeE placeBadge( L"PlcName", EsrKit( 10 ) );
	unsigned int shuntIden = 250;
	time_t placeTime( 300 );
	tpreview.DoAppear( placeBadge, shuntIden, nullptr, &placeTime );
	tpreview.DoDisappear( placeBadge, shuntIden );

	auto retdescrPtr = tpreview.GetDescrPtr( shuntIden );
	CPPUNIT_ASSERT( !retdescrPtr );
	const auto & retPlace = tpreview.GetPlace( shuntIden );
	CPPUNIT_ASSERT( retPlace.empty() );
	const auto & retTime = tpreview.GetTime( shuntIden );
	CPPUNIT_ASSERT( retTime == 0 );
	const auto & retUnityInfo = tpreview.GetUnityInfo( placeBadge );
	CPPUNIT_ASSERT( !retUnityInfo.first && retUnityInfo.second == 0 );
}

void TC_TrainPlacesReview::PlaceTrainOnShunting()
{
	TrainPlacesReview tpreview;
	BadgeE placeBadge( L"PlcName", EsrKit( 10 ) );
	unsigned int shuntIden = 250;
	time_t shuntPlaceTime( 300 );
	tpreview.DoAppear( placeBadge, shuntIden, nullptr, &shuntPlaceTime );

	unsigned int trainIden = 310;
	time_t trainPlaceTime( 500 );
	TrainDescrPtr tdescrPtr( new TrainDescr( L"2454", L"300-400-500" ) );
	tpreview.DoAppear( placeBadge, trainIden, tdescrPtr, &trainPlaceTime );

	auto rettraindescrPtr = tpreview.GetDescrPtr( trainIden );
	CPPUNIT_ASSERT( rettraindescrPtr && *rettraindescrPtr == *tdescrPtr );
	const auto & rettrainPlace = tpreview.GetPlace( trainIden );
	CPPUNIT_ASSERT( rettrainPlace.size() == 1 && rettrainPlace.front() == placeBadge );
	const auto & rettrainTime = tpreview.GetTime( trainIden );
	CPPUNIT_ASSERT( trainPlaceTime == rettrainTime );
	const auto & retUnityInfo = tpreview.GetUnityInfo( placeBadge );
	CPPUNIT_ASSERT( retUnityInfo.first && *retUnityInfo.first == *tdescrPtr && retUnityInfo.second == trainIden );

	auto retshuntdescrPtr = tpreview.GetDescrPtr( shuntIden );
	CPPUNIT_ASSERT( !retshuntdescrPtr );
	const auto & retshuntPlace = tpreview.GetPlace( shuntIden );
	CPPUNIT_ASSERT( retshuntPlace.empty() );
	const auto & retshuntTime = tpreview.GetTime( shuntIden );
	CPPUNIT_ASSERT( retshuntTime == 0 );
}

void TC_TrainPlacesReview::NonrelevantEraseAfterReplace()
{
	TrainPlacesReview tpreview;
	BadgeE placeBadge( L"PlcName", EsrKit( 10 ) );
	unsigned int trainIden = 250;
	TrainDescrPtr tdescrPtr( new TrainDescr( L"2454", L"300-400-500" ) );
	time_t placeTime( 300 );
	tpreview.DoAppear( placeBadge, trainIden, tdescrPtr, &placeTime );

	unsigned int prevShuntIden = 17;
	tpreview.DoDisappear( placeBadge, prevShuntIden );
	auto retdescrPtr = tpreview.GetDescrPtr( trainIden );
	CPPUNIT_ASSERT( retdescrPtr && *retdescrPtr == *tdescrPtr );
	const auto & retPlace = tpreview.GetPlace( trainIden );
	CPPUNIT_ASSERT( retPlace.size() == 1 && retPlace.front() == placeBadge );
	const auto & retTime = tpreview.GetTime( trainIden );
	CPPUNIT_ASSERT( placeTime == retTime );
	const auto & retUnityInfo = tpreview.GetUnityInfo( placeBadge );
	CPPUNIT_ASSERT( retUnityInfo.first && *retUnityInfo.first == *tdescrPtr && retUnityInfo.second == trainIden );
}

void TC_TrainPlacesReview::MultiPlaceIntoSinglePacket()
{
	TrainPlacesReview tpreview;
	unsigned int trainIden = 410;
	BadgeE plc1( L"place1", EsrKit( 1 ) ), plc2( L"place2", EsrKit( 1 ) ), plc3( L"place3", EsrKit( 1 ) );
	time_t mom1( 450 );
	tpreview.DoAppear( plc1, trainIden, make_shared<TrainDescr>(), &mom1 );
	tpreview.DoAppear( plc2, trainIden, make_shared<TrainDescr>(), &mom1 );
	tpreview.DoAppear( plc3, trainIden, make_shared<TrainDescr>(), &mom1 );

	string msgStr = "<info id=\"410\">"\
					"<train num=\"2255\"/>"\
					"<train_place presence=\"Y\" Bdg=\"place1[1]\"/>"\
					"<train_place presence=\"Y\" Bdg=\"place2[1]\"/>"\
					"<train_place presence=\"Y\" Bdg=\"place3[1]\"/>"\
				"</info>"\
				;
	attic::a_document xdoc( "plcchanges" );
	xdoc.document_element().append_buffer( msgStr.c_str(), msgStr.size() );
	time_t mom2( 750 );
	auto changedStrips = tpreview.PlacesChangeNotify( xdoc.document_element(), &mom2 );
	CPPUNIT_ASSERT( changedStrips.size() == 3 );
	CPPUNIT_ASSERT( any_of( changedStrips.cbegin(), changedStrips.cend(), [&plc1]( const BadgeE & bdg ){ return bdg == plc1; } ) );
	CPPUNIT_ASSERT( any_of( changedStrips.cbegin(), changedStrips.cend(), [&plc2]( const BadgeE & bdg ){ return bdg == plc2; } ) );
	CPPUNIT_ASSERT( any_of( changedStrips.cbegin(), changedStrips.cend(), [&plc3]( const BadgeE & bdg ){ return bdg == plc3; } ) );
}