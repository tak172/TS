#include "stdafx.h"

#include "TC_GuiLayer.h"
#include "UtHemHelper.h"
#include "MockFiler.h"
#include "../helpful/Time_Iso.h"
#include "../Hem/ChangeProcessing.h"
#include "../Hem/GuiLayer.h"
#include "../Hem/TheCharts.h"
#include "../Hem/TheChartsIo.h"
#include "../Hem/UserChart.h"
#include "../SenseOfXml/Outlet.h"
#include "../Hem/Appliers/aeGidUralNote.h"
#include "../helpful/EsrGuide.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_GuiLayer );

// Проверка повторного заполнения всего слоя пометок
// - генерация изменений
// - сериализация всего слоя
void TC_GuiLayer::Reload()
{
    std::wstring header =
        L"<Test>"
        L"  <GuiLayer>"
        ;
    std::wstring body1 =
		L"<GuiEvent idPart1='RU0000351407' idPart2='0000000001' esr='31342:31363' waynum='1' startTime='20220111T012000Z' enterInfoTime='19700105T013647Z' markType='2' posTop='15' posBottom='93' stopTime='20220111T032000Z' flagsPermission='4' serviceCode='157' reasonCode='947' color='14'>"
		L"<Comment><![CDATA[Со снятием напряжения. 147км5пк-149км5пк|Рук. раб.: Шарапов (ЭЧ-1 СЕВ)|Рем.разъед.,креп.со снят.напр(1шт)|закончен раньше по причине неиспавности поезда 69|http://asapvo.gvc.oao.rzd/apvo2-op/app/cards/wnd/351407]]></Comment>"
		L"</GuiEvent>"
		L"<GuiEvent idPart1='RU0000351467' idPart2='0000000001' esr='31370' waynum='1' parknum='1' startTime='20220110T232000Z' enterInfoTime='19700105T013747Z' markType='2' posTop='15' posBottom='93' stopTime='20220111T031000Z' flagsPermission='4' serviceCode='157' color='14'>"
		L"<Comment><![CDATA[Со снятием напряжения.|Рук. раб.: Бураков (ЭЧ-1 СЕВ)|Перевод к/с на новую опору(4шт)|закончен раньше по причине неиспавности поезда 69|http://asapvo.gvc.oao.rzd/apvo2-op/app/cards/wnd/351467]]></Comment>"
		L"</GuiEvent>"
		L"<GuiEvent idPart1='RU0000351549' idPart2='0000000001' esr='31470:31481' waynum='1' startTime='20220110T230500Z' enterInfoTime='19700105T013909Z' markType='2' posTop='15' posBottom='93' stopTime='20220111T005500Z' flagsPermission='4' serviceCode='157' color='14'>"
		L"<Comment><![CDATA[Со снятием напряжения. 262км10пк-276км3пк|Рук. раб.: Мазин (ЭЧ-1 СЕВ)|Установка эл.соединителей(4шт), Установка струн(10шт)|http://asapvo.gvc.oao.rzd/apvo2-op/app/cards/wnd/351549]]></Comment>"
		L"</GuiEvent>"
		L"<GuiEvent idPart1='RU0000351553' idPart2='0000000001' esr='31481' waynum='1' parknum='1' startTime='20220110T225000Z' enterInfoTime='19700105T013913Z' markType='2' posTop='15' posBottom='93' stopTime='20220111T005000Z' flagsPermission='4' serviceCode='157' color='14'>"
		L"<Comment><![CDATA[Со снятием напряжения. |Рук. раб.: Михайлов (ЭЧ-1 СЕВ)|Установка ПС(3шт), Установка эл.соединителей(3шт)|http://asapvo.gvc.oao.rzd/apvo2-op/app/cards/wnd/351553]]></Comment>"
		L"</GuiEvent>"
        ;
    std::wstring footer =
        L"  </GuiLayer>"
        L"</Test>"
        ;

    UtLayer<Gui::Layer> gui_layer;
    // загружаем из файла вариант 1
    attic::a_document doc;
    CPPUNIT_ASSERT( doc.load_wide( header+body1+footer ) );
    doc.document_element() >> gui_layer;
    gui_layer.TakeChanges( attic::a_node() );
    // проверяем вариант 1
    attic::a_document res1("Test");
    //attic::a_node fullNode1 = res1.document_element().append_child("Full");
	attic::a_node& root = res1.document_element();
    root << gui_layer;
    std::wstring plain1 = res1.pretty_wstr();
    CPPUNIT_ASSERT( !plain1.empty() );
    // - полное содержимое
    CPPUNIT_ASSERT( !root.select_nodes("//GuiEvent[@idPart1='RU0000351407' and @idPart2='0000000001']").empty() );
    CPPUNIT_ASSERT( !root.select_nodes("//GuiEvent[@idPart1='RU0000351467' and @idPart2='0000000001']").empty() );
    CPPUNIT_ASSERT( !root.select_nodes("//GuiEvent[@idPart1='RU0000351549' and @idPart2='0000000001']").empty() );
	CPPUNIT_ASSERT( !root.select_nodes("//GuiEvent[@idPart1='RU0000351553' and @idPart2='0000000001']").empty() );
}

void TC_GuiLayer::MessageToHaron()
{
    namespace HCIO = Hem::TheChartsIo;
    MockFiler filer;
    HCIO::FileStore fileStore(filer, HCIO::IOMODE::READWRITE, HCIO::SYNCMODE::ASYNC);

    auto tempPusher = []( std::function<void()> toRun, std::string /*markerGA*/ ){
        OutputDebugString(L"pusherFunc called\n");
        toRun();
    };
    auto tempPostback = []( std::string /*xx*/ ){
        OutputDebugString(L"postbackFunc called\n");
    };

    std::unique_ptr<TheCharts> theCharts( new TheCharts( fileStore, tempPusher, tempPostback ) );
    theCharts->initializeCurrentTime( time_from_iso( "20170929T082233Z" ) );
    std::unique_ptr<Outlet> outlet( new Outlet );

    Hem::Slice& slice = theCharts->selectLive();
	Gui::MarkStaticInfo gmsi(L"280", L"", L"", L"");
	theCharts->setupGuiMarkStaticInfo( gmsi);
    ChangeProcessing chProc( *theCharts, *outlet );

    attic::a_document doc1;
    auto lres1 = doc1.load_wide(
        L"<GuiLayer>"
        L"  <Body idPart1='o1644720765' idPart2='0000130491' esr='31250:31268' startTime='20220213T023800Z' enterInfoTime='20220213T023800Z' markType='1' posTop='40' posBottom='40' stopTime='20220213T023800Z' color='13'>"
		L"		<Comment><![CDATA[пометка]]></Comment>\n"
        L"  </Body>"
        L"</GuiLayer>"
        );
    CPPUNIT_ASSERT( lres1 );
    Gui::Event event1( doc1.document_element().first_child() );
    attic::a_document doc2;
    auto lres2 = doc2.load_wide(
        L"<GuiLayer>"
        L"  <Body idPart1='o1644720765' idPart2='0000130491' esr='31250:31268' startTime='20220213T023800Z' enterInfoTime='20220213T023800Z' markType='1' posTop='40' posBottom='40' stopTime='20220213T023800Z' color='13'>"
		L"		<Comment><![CDATA[редактирование]]></Comment>\n"
        L"  </Body>"
        L"</GuiLayer>"
        );
    CPPUNIT_ASSERT( lres2 );
    Gui::Event event2( doc2.document_element().first_child() );

    UtChart<UserChart>& utUC = UtChart<UserChart>::upgrade( slice.UserCh() );
    UtLayer<Gui::Layer>& gl = UtLayer<Gui::Layer>::upgrade( utUC.getGuiLayer() );

    {
        gl.Push( event1 );
        // это создание первого события в слое гид-уральских пометок
        if ( chProc.Build( slice, theCharts->getCurrentTime() ) )
        {
            chProc.SendToGidUral();
        }
        CPPUNIT_ASSERT( outlet->size() == 1 );
        size_t sztop = outlet->top_size();
        Goal goal;
        std::string buf;
        CPPUNIT_ASSERT( sztop != std::string::npos );
        buf.resize( sztop, char( 0 ) );
        outlet->take_front( &buf.front(), buf.size(), &goal, nullptr );
        CPPUNIT_ASSERT( goal == Goal::Fund );
        attic::a_document msg_doc1;
        CPPUNIT_ASSERT( msg_doc1.load_utf8( buf ) );
        std::string fn1 = msg_doc1.document_element().attribute("file").as_string();
        std::wstring wstr1 = FromUtf8( msg_doc1.document_element().child_value("Mark") ); 
        std::wstring worg1 =
            L"(:0001 28002:20 :2 13 02 22 1464\n"
            L"К 1644719880 01 01 000 00 00 031250 031268 040 040 0222013778 0222013778 00000 00000 000 000 013 0\n"
            L" пометка\n"
            L"00000 $0\n";
        CPPUNIT_ASSERT( fn1 == "Z1122330.000" );
		auto res = std::mismatch(worg1.cbegin(), worg1.cend(), wstr1.cbegin());
        CPPUNIT_ASSERT( wstr1 == worg1 );
    }

    Carrier* carrier = nullptr;
    theCharts->processMovementOfClock( time_from_iso( "20170929T083344Z" ), carrier );

    {

        gl.Push( event2 );
        // это создание первого события в слое гид-уральских пометок
        if ( chProc.Build( slice, theCharts->getCurrentTime() ) )
        {
            chProc.SendToGidUral();
        }
        CPPUNIT_ASSERT( outlet->size() == 1 );
        size_t sztop = outlet->top_size();
        Goal goal;
        std::string buf;
        CPPUNIT_ASSERT( sztop != std::string::npos );
        buf.resize( sztop, char( 0 ) );
        outlet->take_front( &buf.front(), buf.size(), &goal, nullptr );
        CPPUNIT_ASSERT( goal == Goal::Fund );
        attic::a_document msg_doc2;
        CPPUNIT_ASSERT( msg_doc2.load_utf8( buf ) );
        std::string fn2 = msg_doc2.document_element().attribute("file").as_string();
        std::wstring wstr2 = FromUtf8( msg_doc2.document_element().child_value("Mark") ); 
        std::wstring worg2 =
            L"(:0001 28002:20 :2 13 02 22 1464\n"
            L"К 1644719880 01 01 000 00 00 031250 031268 040 040 0222013778 0222013778 00000 00000 000 000 013 0\n"
            L" редактирование\n"
            L"00000 $0\n";
        CPPUNIT_ASSERT( fn2 == "Z1133440.001" );
		auto res = std::mismatch(worg2.cbegin(), worg2.cend(), wstr2.cbegin());
        CPPUNIT_ASSERT( wstr2 == worg2 );
    }
}

static Gui::Event event_from_wtext( boost::wstring_ref wtext )
{
    attic::a_document doc;
    auto lres1 = doc.load_wide( wtext );
    CPPUNIT_ASSERT( lres1 );
    Gui::Event event1( doc.document_element() );
    return event1;
}


void TC_GuiLayer::UpdateEvent()
{
    namespace HCIO = Hem::TheChartsIo;
    MockFiler filer;
    HCIO::FileStore fileStore(filer, HCIO::IOMODE::READWRITE, HCIO::SYNCMODE::ASYNC);

    auto tempPusher = []( std::function<void()> toRun, std::string /*markerGA*/ ){ toRun(); };
    auto tempPostback = []( std::string /*xx*/ ){ };

    std::unique_ptr<TheCharts> theCharts( new TheCharts( fileStore, tempPusher, tempPostback ) );
    theCharts->initializeCurrentTime( time_from_iso( "20170929T082233Z" ) );
    std::unique_ptr<Outlet> outlet( new Outlet );

    Hem::Slice& slice = theCharts->selectLive();
    ChangeProcessing chProc( *theCharts, *outlet );

    UtChart<UserChart>& utUC = UtChart<UserChart>::upgrade( slice.UserCh() );
    UtLayer<Gui::Layer>& gl = UtLayer<Gui::Layer>::upgrade( utUC.getGuiLayer() );

    const Gui::Event e1 = event_from_wtext(
        L"<Body idPart1='o1644720765' idPart2='0000130491' esr='31250:31268' startTime='20220213T023800Z' enterInfoTime='' markType='1' posTop='40' posBottom='40' stopTime='20220213T023800Z' color='13'>"
        L"<Comment><![CDATA[пометка]]></Comment>\n"
        L"</Body>"
        );
    const Gui::Event e2 = event_from_wtext(
        L"<Body idPart1='o1644720765' idPart2='0000130491' esr='31250:31268' startTime='20220213T023800Z' enterInfoTime='' markType='1' posTop='40' posBottom='40' stopTime='20220213T023800Z' color='13'>"
        L"  <Comment><![CDATA[редактирование]]></Comment>\n"
        L"</Body>"
        );


    // изменения при создании события
    gl.Push( e1 );
    CPPUNIT_ASSERT( chProc.Build( slice, theCharts->getCurrentTime() ) );
    CPPUNIT_ASSERT( !chProc.Build( slice, theCharts->getCurrentTime() ) );
    //
    Carrier* carrier = nullptr;
    theCharts->processMovementOfClock( time_from_iso( "20170929T083344Z" ), carrier );

    // изменения при корректировке события
    gl.Push( e2 );
    CPPUNIT_ASSERT( chProc.Build( slice, theCharts->getCurrentTime() ) );
    CPPUNIT_ASSERT( !chProc.Build( slice, theCharts->getCurrentTime() ) );

    // отсутствие изменений при повторной корректировке
    gl.Push( e2 );
    CPPUNIT_ASSERT( !chProc.Build( slice, theCharts->getCurrentTime() ) );
}


void TC_GuiLayer::CreateDelayMarkAndSoanStopping()
{
	auto happen_layer = std::make_shared< UtLayer<HappenLayer> >();
	auto gui_layer = std::make_shared< UtLayer<Gui::Layer> >();

	attic::a_document doc;
	EsrGuide eg;
	doc.load_wide(
		L"<EsrGuide>"
		L"<EsrGuide kit='31230' name='Рыбинск тов.' picketing1_val='357~500' picketing1_comm='Москва'>"
		L"<Way waynum='6' parknum='1' />"
		L"<Way waynum='7' parknum='1' />"
		L"</EsrGuide>"
		L"<EsrGuide kit='31231' name='Торопово' picketing1_val='346~400' picketing1_comm='Москва'>"
		L"<Way waynum='1' parknum='1' passenger='Y' embarkation='Y' />"
		L"<Way waynum='2' parknum='1' major='Y' passenger='Y' />"
		L"<Way waynum='3' parknum='1' />"
		L"</EsrGuide>"
		L"</EsrGuide>");
	eg.load_from( doc.document_element() );

	auto& hl = *happen_layer;
	// первая нить
	hl.createPath(
		L"<HemPath>"
		L"<SpotEvent create_time='20220425T170100Z' name='Departure' Bdg='ASOUP 1042_1p2w[31210]' waynum='2' parknum='1' optCode='31210:31230' index='0300-641-2300' num='2322' length='53' weight='3793' net_weight='2567' through='Y'>"
		L"<feat_texts typeinfo='Т' />"
		L"<Locomotive Series='2ТЭ116УД' NumLoc='25' Depo='2853' Consec='1'>"
		L"<Crew EngineDriver='БУГPOB' TabNum='28492' Tim_Beg='2022-04-25 18:13' />"
		L"</Locomotive>"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220425T172200Z' name='Transition' Bdg='ASOUP 1042_3p1w[31230]' waynum='1' parknum='3' optCode='31230:31231' />"
		L"<SpotEvent create_time='20220425T174000Z' name='Arrival' Bdg='ASOUP 1042_1p2w[31231]' waynum='2' parknum='1' />"
		L"<SpotEvent create_time='20220425T174000Z' name='Death' Bdg='ASOUP 1042_1p2w[31231]' waynum='2' parknum='1' />"
		L"</HemPath>"
		);

	CPPUNIT_ASSERT( hl.path_count() == 1 );
	CPPUNIT_ASSERT( hl.exist_series( "name", "Departure Transition Arrival  Death " ) );

	attic::a_document adoc;

	std::wstring guiStr = 
		L"<GuiEvent idPart1='' esr='31230:31231' startTime='20220425T173300Z' enterInfoTime='20220425T204245Z' markType='5' posTop='25' posBottom='25' stopTime='20220425T173600Z' serviceCode='146' reasonCode='124' color='3' reversEsr='Y' index='0300-641-2300' num='2322' through='Y'>"
		L"<Comment><![CDATA[111]]></Comment>\n"
		L"</GuiEvent>";
	
	adoc.load_wide(guiStr);

	std::shared_ptr<Gui::Event> eventPtr = std::make_shared<Gui::Event>( adoc.document_element() );
	CPPUNIT_ASSERT ( eventPtr );

	Hem::ApplierCarryOut::aeGuiNoteCreate apl(eg, *gui_layer, hl.GetWriteAccess(), *eventPtr, Gui::MarkStaticInfo() );
	apl.Action();

	CPPUNIT_ASSERT( gui_layer->path_count() == 1 );
	CPPUNIT_ASSERT( hl.path_count() == 1 );
	CPPUNIT_ASSERT( hl.exist_series( "name", "Departure Transition Span_stopping_begin Span_stopping_end Arrival  Death " ) );
}
