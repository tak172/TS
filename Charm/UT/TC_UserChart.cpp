#include "stdafx.h"

#include "TC_UserChart.h"

#include "../Hem/UserChart.h"

using namespace HemHelpful;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_UserChart );

TC_UserChart::TC_UserChart()
{
}

TC_UserChart::~TC_UserChart()
{
}

void TC_UserChart::setUp()
{
    userChart.reset( new UserChartTest() );
}

void TC_UserChart::tearDown()
{
    HemEventRegistry::Shutdowner();
}

void TC_UserChart::SaveAndLoad()
{
    const wstring header =
        L"<hem2act>"
        L"<UserChart>"
        L"<HappenLayer>";
    const wstring example = 
        L"<HemPath>"
        L"  <SpotEvent create_time='20131003T121316Z' name='Form'      Bdg='2УП[11111:22222]' />"
        L"  <SpotEvent create_time='20131003T121336Z' name='Arrival'   Bdg='2п[22222]'        />"
        L"  <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[22222]'        />"
        L"  <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        L"</HemPath>";
    const wstring etalon = 
        L"<HemPath>"
        L"  <SpotEvent create_time='20131003T121316Z' name='Form'      Bdg='2УП[11111:22222]' />"
        L"  <SpotEvent create_time='20131003T121336Z' name='Arrival'   Bdg='2п[22222]'        />"
        L"  <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[22222]'        />"
        L"  <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        L"  <SpotEvent create_time='20131003T121406Z' name='Death'     Bdg='ЧУ1[22222:33333]' />"
        L"</HemPath>";
    const wstring footer = 
    L"<HemPath>"
    L"  <SpotEvent create_time='20131003T121316Z' name='Form'      Bdg='2УП[33333:44444]' />"
    L"  <SpotEvent create_time='20131003T121336Z' name='Arrival'   Bdg='2п[33333]'        />"
    L"  <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[33333]'        />"
    L"  <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[11111:33333]' />"
    L"  <SpotEvent create_time='20131003T121406Z' name='Death'     Bdg='ЧУ1[11111:33333]' />"
    L"</HemPath>"
    L"</HappenLayer>"
    L"<AsoupLayer>"
    L"  <AsoupEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' index='7777-888-9999' num='9999' length='88' weight='7' noinfo = 'Y'/>"
    L"</AsoupLayer>"
    L"<GuiLayer/>"
    L"<ScheduledLayer/>"
    L"<LimitLayer>"
    L"  <LimitEvent Id='i78' esr='11050:11051' FullSpan='Y' StartTime='20151019T223024Z' UntilStop='Y' Reason='1111' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61' >"
    L"    <picketing_start picketing1_val='428~900' picketing1_comm='Вентспилс' />"
    L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
    L"  </LimitEvent>"
    L"</LimitLayer>"
    L"<MarkLayer/>"
    L"<ForbidLayer/>"
	L"<FutureLayer/>"
    L"<SpanWorkInformator/>"
    L"<LimitTrainKeeper />"
	L"<ArtificalRouteIntrusions>"
		L"<RouteIntrusion>"
			L"<TrainDescr index=\"0003-002-0001\" num=\"7005\" suburbfast=\"Y\"/>"
			L"<Station esrCode=\"09326\">"
				L"<OrderPriority>"
					L"<YieldFor>"
						L"<TrainDescr index=\"0044-055-0066\" num=\"D3992\" docwag=\"Y\"/>"
					L"</YieldFor>"
				L"</OrderPriority>"
			L"</Station>"
		L"</RouteIntrusion>"
	L"</ArtificalRouteIntrusions>"
    L"<PriorityOffsets/>"
    L"<AsoupQueue />"
    L"</UserChart>"
    L"</hem2act>";

    { // загружаем из файла
        attic::a_document doc;
        doc.load_wide( header+example+footer );
        userChart->deserialize(doc.document_element());
        userChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
        time_t trim = userChart->trimMoment();
        userChart->trimLoadedLayer( trim );
        userChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
    }
    std::string	str;
    { // сохраняем в файл
        attic::a_document doc;
        userChart->serialize(doc.set_document_element("hem2act"));
        str = doc.document_element().pretty_str(2);
    }
    std::string str_etalon;
    { // делаем эталон
        attic::a_document doc;
        doc.load_wide( header+etalon+footer );
        str_etalon = doc.document_element().pretty_str(2);
    }
    auto mismresult = mismatch( str.cbegin(), str.cend(), str_etalon.cbegin(), str_etalon.cend() );

    CPPUNIT_ASSERT_EQUAL(str_etalon, str);
}

void TC_UserChart::DirtyDataLoad()
{
    // данные с ошибками
    const wstring dirty = 
        L"<test>"
        L"  <UserChart>"
        L"    <HappenLayer>"
        L"      <HemPath>"
        L"        <SpotEvent create_time='20131003T121316Z' name='Form' Bdg='2УП[44444]' />"
        L"        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[44444]' />"
        L"        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[44444]' />"
        L"        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[44444]' />"
        L"        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[44444]' />"
        L"        <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        L"      </HemPath>"
        L"      <HemPath>"
        L"        <SpotEvent create_time='20131003T121316Z' name='Form' Bdg='2УП[11111:22222]' />"
        L"        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' />"
        L"        <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[22222]' />"
        L"        <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        L"        <SpotEvent create_time='20131003T121406Z' name='Death' Bdg='ЧУ1[22222:33333]' />"
        L"      </HemPath>"
        L"    </HappenLayer>"
        L"    <AsoupLayer>"
        L"      <AsoupEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' index='7777-888-9999' num='9999' length='88' weight='7' noinfo = \"Y\"/>"
        L"      <AsoupEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' index='7777-888-9999' num='9999' length='88' weight='7' noinfo = \"Y\"/>"
        L"    </AsoupLayer>"
        L"    <LimitLayer />"
        L"    <ScheduledLayer/>"
		L"	 <FutureLayer/>"
        L"  </UserChart>"
        L"</test>";

    // данные, где ошибки уже удалены
    const wstring fine = 
        L"<test>"
        L"  <UserChart>"
        L"    <HappenLayer>"
        L"      <HemPath>"
        L"        <SpotEvent create_time='20131003T121316Z' name='Form' Bdg='2УП[11111:22222]' />"
        L"        <SpotEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' />"
        L"        <SpotEvent create_time='20131003T121346Z' name='Departure' Bdg='2п[22222]' />"
        L"        <SpotEvent create_time='20131003T121406Z' name='Span_move' Bdg='ЧУ1[22222:33333]' />"
        L"        <SpotEvent create_time='20131003T121406Z' name='Death' Bdg='ЧУ1[22222:33333]' />"
        L"      </HemPath>"
        L"    </HappenLayer>"
        L"    <AsoupLayer>"
        L"      <AsoupEvent create_time='20131003T121336Z' name='Arrival' Bdg='2п[22222]' index='7777-888-9999' num='9999' length='88' weight='7' noinfo = \"Y\"/>"
        L"    </AsoupLayer>"
        L"    <GuiLayer/>"
        L"    <ScheduledLayer />"
        L"    <LimitLayer />"
        L"    <MarkLayer />"
        L"    <ForbidLayer />"
		L"	 <FutureLayer/>"
        L"    <SpanWorkInformator />"
        L"    <LimitTrainKeeper />"
		L"	  <ArtificalRouteIntrusions/>"
        L"    <PriorityOffsets/>"
        L"    <AsoupQueue />"
        L"  </UserChart>"
        L"</test>";

    // загружаем из файла с ошибками
    attic::a_document doc_dirty;
    doc_dirty.load_wide( dirty );
    userChart->deserialize(doc_dirty.document_element());
    userChart->takeChanges( attic::a_node(), attic::a_node(), 0 );
    string root_name = doc_dirty.document_element().name();

    // сохраняем в файл то, что загрузили
    attic::a_document doc_dst;
    userChart->serialize(doc_dst.set_document_element(root_name));

    // формируем ожидаемый результат
    attic::a_document doc_fine;
    doc_fine.load_wide( fine );
    doc_fine.set_document_element( root_name );

    // сравниваем результаты
    std::string str_fin = doc_fine.document_element().pretty_str(2);
    std::string	str_dst = doc_dst.document_element().pretty_str(2);
    auto mismresult = mismatch( str_fin.cbegin(), str_fin.cend(), str_dst.cbegin(), str_dst.cend() );

    CPPUNIT_ASSERT_EQUAL(str_fin, str_dst);
}
