#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_SpanTravelTime.h"
#include "../helpful/Serialization.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SpanTravelTime );

/*
проверка класс SpanTravelTime
*/

void TC_SpanTravelTime::GetSet()
{
	SpanTravelTime SttGuide;

	CPPUNIT_ASSERT( !SttGuide.existData(span12) );
	CPPUNIT_ASSERT( !SttGuide.existData(span34) );
	SttGuide.SetSpanTime( span12, span12_time );
	SttGuide.SetSpanTime( span34, span34_time );
	CPPUNIT_ASSERT( SttGuide.existData(span12) );
	CPPUNIT_ASSERT( !SttGuide.existData(span12.invert()) );
	CPPUNIT_ASSERT( SttGuide.existData(span34) );
    const auto& SttSpan12 = SttGuide.GetSpanTime(span12);
	CPPUNIT_ASSERT( SttSpan12==span12_time );
	CPPUNIT_ASSERT( SttGuide.GetSpanTime(span34)==span34_time );
}

void TC_SpanTravelTime::LoadSave()
{
	std::shared_ptr <SpanTravelTime> sttGuidePtr;
	std::string src_str;
	{
		const std::wstring stt_text = 
			L"<PvxList>"
			L"<Pvx Span='(11111,22222)'>"
			L"<Gruz start='4.5' full='4' stop='1.2' />"
			L"<Pass start='1' full='2' reserv='1' stop='1' />"
			L"</Pvx>"
			L"<Pvx Span='(22222,11111)'>"
			L"<Gruz start='3' full='3' stop='2' />"
			L"<Pass start='2' full='1' reserv='2' stop='2' />"
			L"</Pvx>"
			L"<Pvx Span='(33333,44444)'>"
			L"<Gruz start='4' full='4' stop='1' />"
			L"<Pass start='2' full='2' stop='1' />"
			L"</Pvx>"
			L"</PvxList>";
		attic::a_document x_doc;
		x_doc.load_wide( stt_text );
		auto pvhListNode = x_doc.document_element();
		src_str = pvhListNode.to_str();
		sttGuidePtr = deserialize<SpanTravelTime>( src_str );
		CPPUNIT_ASSERT( sttGuidePtr );
	}

	CPPUNIT_ASSERT( sttGuidePtr->existData(span12) );
	CPPUNIT_ASSERT( sttGuidePtr->existData(span12.invert()) );
	CPPUNIT_ASSERT( sttGuidePtr->existData(span34) );
	
	auto SttVal =  sttGuidePtr->GetSpanTime(span12);
	CPPUNIT_ASSERT( SttVal.pGruz && *SttVal.pGruz==STT_Cat_Val(270,240,0,72) );
	CPPUNIT_ASSERT( SttVal.pPass && *SttVal.pPass==STT_Cat_Val(60,120,60,60) );
	
	SttVal = sttGuidePtr->GetSpanTime(span34);
	CPPUNIT_ASSERT( SttVal.pGruz && *SttVal.pGruz==STT_Cat_Val(240,240,0,60) );
	CPPUNIT_ASSERT( SttVal.pPass && *SttVal.pPass==STT_Cat_Val(120,120,0,60) );
	CPPUNIT_ASSERT( !SttVal.pDies );
	CPPUNIT_ASSERT( !SttVal.pElek );

	std::pair< STT_Val, STT_Val > stt = sttGuidePtr->GetSpanTimeBoth( EsrKit(11111), EsrKit(22222) );
	CPPUNIT_ASSERT( stt.first.pGruz && *stt.first.pGruz==STT_Cat_Val(270,240,0,72) );
	CPPUNIT_ASSERT( stt.first.pPass && *stt.first.pPass==STT_Cat_Val(60,120,60,60) );
	CPPUNIT_ASSERT( stt.second.pGruz && *stt.second.pGruz==STT_Cat_Val(180,180,0,120) );
	CPPUNIT_ASSERT( stt.second.pPass && *stt.second.pPass==STT_Cat_Val(120,60,120,120) );

	auto serialStr = serialize<SpanTravelTime>( *sttGuidePtr );
	CPPUNIT_ASSERT( src_str == serialStr );
}

void TC_SpanTravelTime::LoadErrorCheck()
{
    std::vector<std::wstring> msgs;

    const std::string duplicate_span = 
        "<PvxList>"
        "  <Pvx Span='(11111,22222)'>"
        "    <Gruz start='4.5' full='4' stop='1.2' />" // только пассажирские
        "  </Pvx>"
        "  <Pvx Span='(22222,11111)'>"
        "    <Gruz start='3' full='3' stop='2' />"
        "    <Pass start='2' full='1' reserv='2' stop='2' />"
        "  </Pvx>"
        "  <Pvx Span='(11111,22222)'>"
        "    <Pass start='1' full='2' reserv='1' stop='1' />" // только грузовые
        "  </Pvx>"
        "</PvxList>";
    std::shared_ptr<SpanTravelTime> one = SpanTravelTime::deserializeLogWarnings( duplicate_span, msgs );
    CPPUNIT_ASSERT( one );
    CPPUNIT_ASSERT( one->existData( SpanTravelTimeInfo(EsrKit(11111),EsrKit(22222)) ) );
    CPPUNIT_ASSERT( one->existData( SpanTravelTimeInfo(EsrKit(22222),EsrKit(11111)) ) );
    CPPUNIT_ASSERT( 1 <= msgs.size() );

    const std::string bad_span = 
        "<PvxList>"
        "  <Pvx Span='(11111,22222)'>"
        "    <Gruz start='4.5' full='4' stop='1.2' />"
        "    <Pass start='1' full='2' reserv='1' stop='1' />"
        "  </Pvx>"
        "  <Pvx Span='(33333)'>"
        "    <Gruz start='3' full='3' stop='2' />"
        "    <Pass start='2' full='221' reserv='332' stop='2' />"
        "  </Pvx>"
        "</PvxList>";

    std::shared_ptr<SpanTravelTime> two = SpanTravelTime::deserializeLogWarnings( duplicate_span, msgs );
    CPPUNIT_ASSERT( two );
    CPPUNIT_ASSERT( one->existData( SpanTravelTimeInfo(EsrKit(11111),EsrKit(22222)) ) );
    CPPUNIT_ASSERT( 1 <= msgs.size() );
}

void TC_SpanTravelTime::SubtractUnchanged()
{
    // добавка новых данных
    SpanTravelTimeInfo span23  = SpanTravelTimeInfo(EsrKit(22222),EsrKit(33333));
    SpanTravelTimeInfo span78  = SpanTravelTimeInfo(EsrKit(77777),EsrKit(88888));
    STT_Val upd23; 
    STT_Val upd78; 
    upd23.set_pass( 1230, 2230, 3230, 4230 );
    upd78.set_elek( 7810, 7820, 7830, 7840 );
    // изменения существующих
    STT_Val upd34 = span34_time;
    upd34.set_dies( 1340, 2340, 3340, 4340 );
    // начальное состояние
    SpanTravelTime prev;
    prev.SetSpanTime( span12, span12_time );
    prev.SetSpanTime( span34, span34_time );

    // измененное состояние
    SpanTravelTime next = prev;
    next.SetSpanTime( span23, upd23 );
    next.SetSpanTime( span34, upd34 );
    next.SetSpanTime( span78, upd78 );
    // получение различий
    SpanTravelTime diff = next.subtractUnchanged( prev );
    
    CPPUNIT_ASSERT_EQUAL( size_t(3), diff.GetSpans().size() );
    CPPUNIT_ASSERT( diff.existData( span23 ) );
    CPPUNIT_ASSERT( diff.existData( span34 ) );
    CPPUNIT_ASSERT( diff.existData( span78 ) );
    CPPUNIT_ASSERT( upd23 == diff.GetSpanTime( span23 ) );
    CPPUNIT_ASSERT( upd34 == diff.GetSpanTime( span34 ) );
    CPPUNIT_ASSERT( upd78 == diff.GetSpanTime( span78 ) );
}

void TC_SpanTravelTime::Invert()
{
    auto one = SpanTravelTimeInfo(EsrKit(77777),EsrKit(88888));
    auto two = one.invert();
    auto one_dup = two.invert();
    CPPUNIT_ASSERT( !(one == two) );
    CPPUNIT_ASSERT( one == one_dup );
}

void TC_SpanTravelTime::BadEsr()
{
    auto a = SpanTravelTimeInfo( EsrKit(55555), EsrKit(55555) );
    CPPUNIT_ASSERT( SpanTravelTimeInfo() == a );

    auto b = SpanTravelTimeInfo( EsrKit(55555,66666), EsrKit(77777,88888) );
    CPPUNIT_ASSERT( SpanTravelTimeInfo() == b );
}

void TC_SpanTravelTime::setUp()
{
	span12  = SpanTravelTimeInfo(EsrKit(11111),EsrKit(22222));
	span34  = SpanTravelTimeInfo(EsrKit(33333),EsrKit(44444));

	span12_time.set_gruz( 240, 240, 0, 60 );
	span12_time.set_pass( 60, 120, 60, 60 );

	span34_time.set_gruz( 60, 120, 180, 240 );
    span34_time.set_pass( 60, 120, 180, 240 );
    span34_time.set_dies( 60, 120, 180, 240 );
    span34_time.set_elek( 60, 120, 180, 240 );
}
