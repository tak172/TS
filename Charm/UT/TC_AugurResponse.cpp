#include "stdafx.h"
#include "TC_AugurResponse.h"
#include "../Augur/AugurResponse.h"
#include "../Augur/TrainRoutes.h"
#include "../helpful/Serialization.h"
#include "../Augur/AThread.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Hem/FixableEvent.h"
#include "../helpful/TrainDescr.h"
#include "../Guess/SpotDetails.h"
#include "XmlEqual.h"
#include "../helpful/PrognosisFeedback.h"
#include "UtReadFile.h"

using namespace std;
using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AugurResponse );

void TC_AugurResponse::Serialize()
{
	AugurResponse<AEvent> augurResponse( AugurResponseCode::NoNSI );
	TrainRoutes<AEvent> responseRoutes( TrainRoutes<AEvent>::Type::ForecastOut );

	//нить только с живыми событиями
	vector<AEvent> aevVec1;
	AEvent ssAevent1( 32000 );
	ssAevent1.setType( AEvent::STOP_START );
	ssAevent1.setObj( 7, 9 );
	tm tm1 = to_tm( from_iso_string("20170627T151207") );
	tm tm2 = to_tm( from_iso_string("20170627T151429") );
	ssAevent1.diap = ADiap( _mkgmtime( &tm1 ), _mkgmtime( &tm2 ) );
	aevVec1.emplace_back( ssAevent1 );
	AEvent moveAevent( 32000, 32050 );
	moveAevent.setType( AEvent::MOVE );
	moveAevent.setObj( 2 );
	tm tm3 = to_tm( from_iso_string("20170627T151617") );
	tm tm4 = to_tm( from_iso_string("20170627T151845") );
	moveAevent.diap = ADiap( _mkgmtime( &tm3 ), _mkgmtime( &tm4 ) );
	aevVec1.emplace_back( moveAevent );
	responseRoutes.addEventLine( make_pair( TrainDescr( L"3254" ), aevVec1 ) );

	//нить с живыми и фиксированными событиями
	vector<AEvent> aevVec2;
	AEvent transAevent( 11500 );
	transAevent.setSource( AEvent::REAL );
	transAevent.setType( AEvent::MOVE );
	transAevent.setObj( 2, 4 );
	tm tm5 = to_tm( from_iso_string("20170627T130000") );
	transAevent.diap = ADiap( _mkgmtime( &tm5 ), _mkgmtime( &tm5 ) );
	aevVec2.emplace_back( transAevent );
	AEvent fixedTransAevent2( 9120 );
	fixedTransAevent2.setSource( AEvent::FIXED );
	fixedTransAevent2.setType( AEvent::MOVE );
	fixedTransAevent2.setObj( 1, 3 );
	tm tm6 = to_tm( from_iso_string("20170627T140154") );
	fixedTransAevent2.diap = ADiap( _mkgmtime( &tm6 ), _mkgmtime( &tm6 ) );
	aevVec2.emplace_back( fixedTransAevent2 );
	AEvent fixedSSAevent2( 12020 );
	fixedSSAevent2.setSource( AEvent::FIXED );
	fixedSSAevent2.setType( AEvent::STOP_START );
	fixedSSAevent2.setObj( 2, 1 );
	tm tm7 = to_tm( from_iso_string("20170627T190142") );
	tm tm8 = to_tm( from_iso_string("20170627T194757") );
	fixedSSAevent2.diap = ADiap( _mkgmtime( &tm7 ), _mkgmtime( &tm8 ) );
	aevVec2.emplace_back( fixedSSAevent2 );
	AEvent transAevent2( 21100 );
	transAevent2.setSource( AEvent::REAL );
	transAevent2.setType( AEvent::MOVE );
	transAevent2.setObj( 7, 4 );
	tm tm9 = to_tm( from_iso_string("20170627T202149") );
	transAevent2.diap = ADiap( _mkgmtime( &tm9 ), _mkgmtime( &tm9 ) );
	aevVec2.emplace_back( transAevent2 );
	responseRoutes.addEventLine( make_pair( TrainDescr( L"7100" ), aevVec2 ) );

	augurResponse.futureChartPtr.reset( new TrainRoutes<AEvent>( responseRoutes ) );

	string serialStr = serialize( augurResponse );
	string testStr = "<AugurResponse>"\
		"<FutureLayer>"\
		"<HemPath>"\
			"<SpotEvent create_time=\"20170627T151207\" name=\"Info_changing\">"\
				"<TrainDescr index=\"\" num=\"3254\" divisional=\"Y\" />"\
			"</SpotEvent>"\
			"<SpotEvent create_time=\"20170627T151207\" name=\"Arrival\" waynum=\"7\" parknum=\"9\" Bdg=\"mock[32000]\" />"\
			"<SpotEvent create_time=\"20170627T151429\" name=\"Departure\" waynum=\"7\" parknum=\"9\" Bdg=\"mock[32000]\" />"\
			"<SpotEvent create_time=\"20170627T151617\" name=\"Span_move\" waynum=\"2\" Bdg=\"mock[32000:32050]\" />"\
		"</HemPath>"\
		"<HemPath>"\
			"<SpotEvent create_time=\"20170627T130000\" name=\"Info_changing\">"\
				"<TrainDescr index=\"\" num=\"7100\" suburbfast=\"Y\" />"\
			"</SpotEvent>"\
			"<SpotEvent create_time=\"20170627T130000\" name=\"Transition\" waynum=\"2\" parknum=\"4\" Bdg=\"mock[11500]\" />"\
			"<SpotEvent create_time=\"20170627T140154\" name=\"Transition\" waynum=\"1\" parknum=\"3\" Bdg=\"mock[09120]\" sourcetype=\"fix\" />"\
			"<SpotEvent create_time=\"20170627T190142\" name=\"Arrival\" waynum=\"2\" parknum=\"1\" Bdg=\"mock[12020]\" sourcetype=\"fix\" />"\
			"<SpotEvent create_time=\"20170627T194757\" name=\"Departure\" waynum=\"2\" parknum=\"1\" Bdg=\"mock[12020]\" sourcetype=\"fix\" />"\
			"<SpotEvent create_time=\"20170627T202149\" name=\"Transition\" waynum=\"7\" parknum=\"4\" Bdg=\"mock[21100]\" />"\
		"</HemPath>"\
		"</FutureLayer>"\
		"<AugurResponseCode value=\"nonsi\" />"\
		"</AugurResponse>";
	CPPUNIT_ASSERT( xmlEqual( serialStr, testStr ) );
}

void TC_AugurResponse::Deserialize()
{
	string serialStr = "<AugurResponse>"\
		"<FutureLayer>"\
		"<HemPath>"\
			"<SpotEvent create_time=\"20171105T170147\" name=\"Info_changing\">"\
				"<TrainDescr index=\"\" num=\"9007\" fretran=\"Y\" />"\
			"</SpotEvent>"\
			"<SpotEvent create_time=\"20171105T170614\" name=\"Transition\" waynum=\"3\" parknum=\"1\" Bdg=\"mock[44512]\" />"\
			"<SpotEvent create_time=\"20170627T181617\" name=\"Span_move\" waynum=\"2\" Bdg=\"mock[44512:44510]\" />"\
		"</HemPath>"\
		"<HemPath>"\
			"<SpotEvent create_time=\"20171002T081405\" name=\"Info_changing\">"\
				"<TrainDescr index=\"\" num=\"5511\" tepsng=\"Y\" />"\
			"</SpotEvent>"\
			"<SpotEvent create_time=\"20171002T081405\" name=\"Arrival\" waynum=\"9\" parknum=\"3\" Bdg=\"mock[17980]\" sourcetype=\"fix\" />"\
			"<SpotEvent create_time=\"20171002T081921\" name=\"Departure\" waynum=\"9\" parknum=\"3\" Bdg=\"mock[17980]\" sourcetype=\"fix\" />"\
			"<SpotEvent create_time=\"20171002T093439\" name=\"Arrival\" waynum=\"4\" parknum=\"11\" Bdg=\"mock[23150]\" />"\
			"<SpotEvent create_time=\"20171002T093717\" name=\"Departure\" waynum=\"4\" parknum=\"11\" Bdg=\"mock[23150]\" />"\
		"</HemPath>"\
		"</FutureLayer>";\
		"<AugurResponseCode value=\"unknown_error\" />"\
		"</AugurResponse>";
	auto augurResponsePtr = deserialize<AugurResponse<FixableEvent>>( serialStr );
	CPPUNIT_ASSERT( augurResponsePtr );
	auto spotLines = augurResponsePtr->futureChartPtr->getSpotLines();
	CPPUNIT_ASSERT( spotLines.size() == 2 );
	for ( unsigned int i = 0; i < spotLines.size(); ++i )
	{
		const auto & eventLine = spotLines[i];
		if ( i == 0 )
		{
			CPPUNIT_ASSERT( eventLine.first.GetNumber().getNumber() == 9007 );
			const auto & events = eventLine.second;
			CPPUNIT_ASSERT( events.size() == 2 );

			CPPUNIT_ASSERT( events[0].GetCode() == HCode::TRANSITION );
			auto parkWay = events[0].ParkWay();
			CPPUNIT_ASSERT( parkWay.iWay() == 3 && parkWay.iPark() == 1 );
			tm tmval = to_tm( from_iso_string("20171105T170614") );
			CPPUNIT_ASSERT( events[0].GetTime() == _mkgmtime( &tmval ) );
			CPPUNIT_ASSERT( events[0].GetBadge().num() == EsrKit( 44512 ) );
			CPPUNIT_ASSERT( !events[0].IsFixated() );

			CPPUNIT_ASSERT( events[1].GetCode() == HCode::SPAN_MOVE );
			auto parkWay2 = events[1].ParkWay();
			CPPUNIT_ASSERT( parkWay2.iWay() == 2 );
			tm tmval2 = to_tm( from_iso_string("20170627T181617") );
			CPPUNIT_ASSERT( events[1].GetTime() == _mkgmtime( &tmval2 ) );
			CPPUNIT_ASSERT( events[1].GetBadge().num() == EsrKit( 44512, 44510 ) );
			CPPUNIT_ASSERT( !events[1].IsFixated() );
		}
		else if ( i == 1 )
		{
			CPPUNIT_ASSERT( eventLine.first.GetNumber().getNumber() == 5511 );
			const auto & events = eventLine.second;
			CPPUNIT_ASSERT( events.size() == 4 );

			CPPUNIT_ASSERT( events[0].GetCode() == HCode::ARRIVAL );
			auto parkWay = events[0].ParkWay();
			CPPUNIT_ASSERT( parkWay.iWay() == 9 && parkWay.iPark() == 3 );
			tm tmval = to_tm( from_iso_string("20171002T081405") );
			CPPUNIT_ASSERT( events[0].GetTime() == _mkgmtime( &tmval ) );
			CPPUNIT_ASSERT( events[0].GetBadge().num() == EsrKit( 17980 ) );
			CPPUNIT_ASSERT( events[0].IsFixated() );

			CPPUNIT_ASSERT( events[1].GetCode() == HCode::DEPARTURE );
			parkWay = events[1].ParkWay();
			CPPUNIT_ASSERT( parkWay.iWay() == 9 && parkWay.iPark() == 3 );
			tmval = to_tm( from_iso_string("20171002T081921") );
			CPPUNIT_ASSERT( events[1].GetTime() == _mkgmtime( &tmval ) );
			CPPUNIT_ASSERT( events[1].GetBadge().num() == EsrKit( 17980 ) );
			CPPUNIT_ASSERT( events[1].IsFixated() );

			CPPUNIT_ASSERT( events[2].GetCode() == HCode::ARRIVAL );
			parkWay = events[2].ParkWay();
			CPPUNIT_ASSERT( parkWay.iWay() == 4 && parkWay.iPark() == 11 );
			tmval = to_tm( from_iso_string("20171002T093439") );
			CPPUNIT_ASSERT( events[2].GetTime() == _mkgmtime( &tmval ) );
			CPPUNIT_ASSERT( events[2].GetBadge().num() == EsrKit( 23150 ) );
			CPPUNIT_ASSERT( !events[2].IsFixated() );

			CPPUNIT_ASSERT( events[3].GetCode() == HCode::DEPARTURE );
			parkWay = events[3].ParkWay();
			CPPUNIT_ASSERT( parkWay.iWay() == 4 && parkWay.iPark() == 11 );
			tmval = to_tm( from_iso_string("20171002T093717") );
			CPPUNIT_ASSERT( events[3].GetTime() == _mkgmtime( &tmval ) );
			CPPUNIT_ASSERT( events[3].GetBadge().num() == EsrKit( 23150 ) );
			CPPUNIT_ASSERT( !events[3].IsFixated() );
		}
	}

	CPPUNIT_ASSERT( augurResponsePtr->responseCode == AugurResponseCode::UnknownError );
}

void TC_AugurResponse::SerializeAugurEventFeedback()
{
	AugurResponse<AEvent> augurResponse( AugurResponseCode::Success );
	TrainRoutes<AEvent> responseRoutes( TrainRoutes<AEvent>::Type::ForecastOut );

	//нить только с живыми событиями
	vector<AEvent> aevVec;
	AEvent ssAevent( 32000 );
	ssAevent.setType( AEvent::STOP_START );
	ssAevent.setObj( 7, 9 );
	tm tm1 = to_tm( from_iso_string("20170627T151207") );
	tm tm2 = to_tm( from_iso_string("20170627T151429") );
	ssAevent.diap = ADiap( _mkgmtime( &tm1 ), _mkgmtime( &tm2 ) );
	ssAevent.setFeedback( PrognosisFeedback( PrognosisErrorCode::UNIQUE, L"error_information", L"", TrainDescrPtr( new TrainDescr( L"666", L"44-22-11" ) ) ) );
	aevVec.emplace_back( ssAevent );

	AEvent moveAevent( 32000, 32050 );
	moveAevent.setType( AEvent::MOVE );
	moveAevent.setObj( 2 );
	tm1 = to_tm( from_iso_string("20170627T151617") );
	tm2 = to_tm( from_iso_string("20170627T151617") );
	moveAevent.diap = ADiap( _mkgmtime( &tm1 ), _mkgmtime( &tm2 ) );
	moveAevent.setFeedback( PrognosisFeedback( PrognosisErrorCode::COMMON, L"", L"", TrainDescrPtr( new TrainDescr( L"3001" ) ) ) );
	aevVec.emplace_back( moveAevent );

	AEvent moveAevent2( 32000, 32050 );
	moveAevent2.setType( AEvent::MOVE );
	moveAevent2.setObj( 2 );
	tm1 = to_tm( from_iso_string("20170627T151622") );
	tm2 = to_tm( from_iso_string("20170627T151622") );
	moveAevent2.diap = ADiap( _mkgmtime( &tm1 ), _mkgmtime( &tm2 ) );
	moveAevent2.setFeedback( PrognosisFeedback( PrognosisErrorCode::FIX_UNIQUE, L"" ) );
	aevVec.emplace_back( moveAevent2 );

	AEvent moveAevent3( 32000, 32050 );
	moveAevent3.setType( AEvent::MOVE );
	moveAevent3.setObj( 2 );
	tm1 = to_tm( from_iso_string("20170627T151628") );
	tm2 = to_tm( from_iso_string("20170627T151628") );
	moveAevent3.diap = ADiap( _mkgmtime( &tm1 ), _mkgmtime( &tm2 ) );
	moveAevent3.setFeedback( PrognosisFeedback( PrognosisErrorCode::COMMON, L"информация об ошибке" ) );
	aevVec.emplace_back( moveAevent3 );

	responseRoutes.addEventLine( make_pair( TrainDescr( L"3254" ), aevVec ) );

	augurResponse.futureChartPtr.reset( new TrainRoutes<AEvent>( responseRoutes ) );

	string serialStr = serialize( augurResponse );
	string testStr = 
        "<AugurResponse>"
        "  <FutureLayer>"
        "    <HemPath>"
        "      <SpotEvent create_time='20170627T151207' name='Info_changing'>"
        "        <TrainDescr index='' num='3254' divisional='Y' />"
        "      </SpotEvent>"
        "      <SpotEvent create_time='20170627T151207' name='Arrival' waynum='7' parknum='9' Bdg='mock[32000]'>"
        "        <Feedback>"
        "          <Conflict error_code='2' info='error_information'>"
        "            <TrainDescr index='0044-022-0011' num='666' psngyear='Y' />"
        "          </Conflict>"
        "        </Feedback>"
        "      </SpotEvent>"
        "      <SpotEvent create_time='20170627T151429' name='Departure' waynum='7' parknum='9' Bdg='mock[32000]'/>"
        "      <SpotEvent create_time='20170627T151617' name='Span_move' waynum='2' Bdg='mock[32000:32050]'>"
        "        <Feedback>"
        "          <Conflict>"
        "            <TrainDescr index='' num='3001' divisional='Y'/>"
        "          </Conflict>"
        "        </Feedback>"
        "      </SpotEvent>"
        "      <SpotEvent create_time='20170627T151622' name='Span_move' waynum='2' Bdg='mock[32000:32050]'>"
        "        <Feedback>"
        "          <Conflict error_code='3'/>"
        "        </Feedback>"
        "      </SpotEvent>"
        "      <SpotEvent create_time='20170627T151628' name='Span_move' waynum='2' Bdg='mock[32000:32050]'>"
        "        <Feedback>"
        "          <Conflict info= '" + ToUtf8( L"информация об ошибке" ) + "'/>"
        "        </Feedback>"
        "      </SpotEvent>"
        "    </HemPath>"
        "  </FutureLayer>"
        "  <AugurResponseCode value='success' />"
        "</AugurResponse>";
	CPPUNIT_ASSERT( xmlEqual( serialStr, testStr ) );
}

void TC_AugurResponse::DeserializeAugurEventFeedback()
{
	string serialStr = "<AugurResponse>"\
		"<FutureLayer>"\
			"<HemPath>"\
				"<SpotEvent create_time=\"20171105T170147\" name=\"Info_changing\">"\
					"<TrainDescr index=\"\" num=\"9007\" fretran=\"Y\" />"\
				"</SpotEvent>"\
				"<SpotEvent create_time=\"20171105T170614\" name=\"Transition\" waynum=\"3\" parknum=\"1\" Bdg=\"mock[44512]\">"\
					"<Feedback>"\
						"<Conflict error_code=\"1\" info=\"" + ToUtf8( L"тестовая информация" ) + "\">"\
							"<TrainDescr index=\"044-022-011\" num=\"666\" psngyear=\"Y\" />"\
						"</Conflict>"\
					"</Feedback>"\
				"</SpotEvent>"\
				"<SpotEvent create_time=\"20170627T181617\" name=\"Span_move\" waynum=\"2\" Bdg=\"mock[44512:44510]\">"\
					"<Feedback>"\
						"<Conflict>"\
							"<TrainDescr index=\"\" num=\"1920\" divisional=\"Y\" />"\
						"</Conflict>"\
					"</Feedback>"\
				"</SpotEvent>"\
				"<SpotEvent create_time=\"20170627T181622\" name=\"Span_move\" waynum=\"2\" Bdg=\"mock[44512:44510]\">"\
					"<Feedback>"\
						"<Conflict error_code=\"2\"/>"\
					"</Feedback>"\
				"</SpotEvent>"\
				"<SpotEvent create_time=\"20170627T181628\" name=\"Span_move\" waynum=\"2\" Bdg=\"mock[44512:44510]\">"\
					"<Feedback>"\
						"<Conflict error_code=\"0\" info=\"information about error\"/>"\
					"</Feedback>"\
				"</SpotEvent>"\
			"</HemPath>"\
		"</FutureLayer>";\
		"<AugurResponseCode value=\"unknown_error\" />"\
		"</AugurResponse>";

	auto augurResponsePtr = deserialize<AugurResponse<FixableEvent>>( serialStr );
	CPPUNIT_ASSERT( augurResponsePtr );
	auto spotLines = augurResponsePtr->futureChartPtr->getSpotLines();
	CPPUNIT_ASSERT( spotLines.size() == 1 );
	const auto & eventLine = spotLines.back();

	CPPUNIT_ASSERT( eventLine.first.GetNumber().getNumber() == 9007 );
	const auto & events = eventLine.second;
	CPPUNIT_ASSERT( events.size() == 4 );

	CPPUNIT_ASSERT( events[0].GetCode() == HCode::TRANSITION );
	const auto & prognFeedbackPtr1 = events[0].GetPrognosisFeedback();
	CPPUNIT_ASSERT( prognFeedbackPtr1 );
	auto tdescrPtr1 = prognFeedbackPtr1->conflictedWith();
	CPPUNIT_ASSERT( tdescrPtr1 && *tdescrPtr1 == TrainDescr( L"666", L"44-22-11" ) );
	CPPUNIT_ASSERT( prognFeedbackPtr1->getCode() == PrognosisErrorCode::SINGLE );
	CPPUNIT_ASSERT( prognFeedbackPtr1->getInfo() == L"тестовая информация" );

	CPPUNIT_ASSERT( events[1].GetCode() == HCode::SPAN_MOVE );
	const auto & prognFeedbackPtr2 = events[1].GetPrognosisFeedback();
	CPPUNIT_ASSERT( prognFeedbackPtr2 );
	auto tdescrPtr2 = prognFeedbackPtr2->conflictedWith();
	CPPUNIT_ASSERT( tdescrPtr2 && *tdescrPtr2 == TrainDescr( L"1920" ) );
	CPPUNIT_ASSERT( prognFeedbackPtr2->getCode() == PrognosisErrorCode::COMMON );
	CPPUNIT_ASSERT( prognFeedbackPtr2->getInfo().empty() );

	CPPUNIT_ASSERT( events[2].GetCode() == HCode::SPAN_MOVE );
	const auto & prognFeedbackPtr3 = events[2].GetPrognosisFeedback();
	CPPUNIT_ASSERT( prognFeedbackPtr3 );
	auto tdescrPtr3 = prognFeedbackPtr3->conflictedWith();
	CPPUNIT_ASSERT( !tdescrPtr3 );
	CPPUNIT_ASSERT( prognFeedbackPtr3->getCode() == PrognosisErrorCode::UNIQUE );
	CPPUNIT_ASSERT( prognFeedbackPtr3->getInfo().empty() );

	CPPUNIT_ASSERT( events[3].GetCode() == HCode::SPAN_MOVE );
	const auto & prognFeedbackPtr4 = events[3].GetPrognosisFeedback();
	CPPUNIT_ASSERT( prognFeedbackPtr4 );
	auto tdescrPtr4 = prognFeedbackPtr4->conflictedWith();
	CPPUNIT_ASSERT( !tdescrPtr4 );
	CPPUNIT_ASSERT( prognFeedbackPtr4->getCode() == PrognosisErrorCode::COMMON );
	CPPUNIT_ASSERT( prognFeedbackPtr4->getInfo() == L"information about error" );

	CPPUNIT_ASSERT( augurResponsePtr->responseCode == AugurResponseCode::UnknownError );
}

static boost::string_ref select_marked( boost::string_ref text, boost::string_ref tag )
{
    std::string from = "<" + tag.to_string() + ">";
    std::string to = "</" + tag.to_string() + ">";

    auto pos = text.find( from );
    if( text.npos == pos )
        return boost::string_ref();
    text.remove_prefix( pos );
    pos = text.find( to );
    if( text.npos == pos )
        return boost::string_ref();
    text.remove_suffix( text.size() - pos - to.size() );
    return text;
}

void TC_AugurResponse::ProcessResponse()
{
    // файл с ошибкой приема данных прогноза
    std::wstring hcrh = L"E:/WorkCopy.all/_Kigas 2.0/_1.105/ver 84 db 132/20230720T045805Z_h.hcrh";
    const std::string body = UtFileBody( hcrh );
    if( body.empty() )
        return; // это не ошибка - просто файла нет

    // извлечь компоненты
    boost::string_ref happen_body = select_marked( body, "HappenLayer" );

    // внести компоненты в UserChart и AugurResponse
    // и повторить ошибку
    CPPUNIT_ASSERT( !"Не сделано" );    
//     attic::a_document doc;
//     doc.load_utf8( happen_body );
//     ???
//     attic::a_node ha_node = doc.document_element();
//     ha_node.brief_attribute( "producer", "Hcrash_Emitter" );
//     ha_node.brief_attribute( "tz", get_TimeZone() );
}
