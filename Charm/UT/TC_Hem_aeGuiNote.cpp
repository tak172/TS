#include "stdafx.h"

#include "TC_Hem_aeGuiNote.h"
#include "TC_Hem_ae_helper.h"
#include "UtHemHelper.h"
#include "../Hem/GuiLayer.h"
#include "../Hem/Bill.h"
#include "../Hem/Appliers/aeGidUralNote.h"

using namespace std;
using namespace Hem::ApplierCarryOut;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeGuiNote );

void TC_Hem_aeGuiNote::Create()
{
    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_mww'>"
		L"<Action code='Create' District='31000-05110' DistrictRight='Y' layer='guiMark' />"
		L"<Data>"
		L"<GuiNote esr='31230:31231' enterInfoTime='20211211T051300Z' startTime='20211211T051300Z' markType='3' posTop='31' posBottom='31' stopTime='20211211T053300Z' serviceCode='132' color='12' reversEsr='Y'>"
		L"<Comment />"
		L"</GuiNote>"
        L" </Data>"
        L"</A2F_HINT>"
        );

	UtLayer<Gui::Layer> gui_layer;
	UtLayer<HappenLayer> happen;

	Hem::Bill::GuiNote bGuiNote = bill.build< Hem::Bill::GuiNote >();
	auto markInfo = bGuiNote.GetMarkInfo();

	aeGuiNoteCreate aeGui( gui_layer, happen.GetWriteAccess(), markInfo, Gui::MarkStaticInfo() );

    aeGui.Action();

    CPPUNIT_ASSERT( gui_layer.path_count() == 1 );
    CPPUNIT_ASSERT( gui_layer.exist_event("@esr='31230:31231'") );
}

void TC_Hem_aeGuiNote::CreateDelay()
{
	UtLayer<HappenLayer> happen;
	happen.createPath( 
		L"<HemPath>"
		L"<SpotEvent create_time='20211211T045200Z' name='Arrival' Bdg='?p1w1[05110]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T045200Z' name='Departure' Bdg='?p1w1[05110]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T051605Z' name='Transition' Bdg='?p1w1[31130]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T053117Z' name='Transition' Bdg='?p1w1[31132]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T054511Z' name='Transition' Bdg='?p1w1[31139]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T061023Z' name='Transition' Bdg='?p1w1[31151]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T063642Z' name='Transition' Bdg='?p1w1[31160]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T065654Z' name='Transition' Bdg='?p1w1[31170]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T071600Z' name='Arrival' Bdg='??p1w1[31180]' waynum='1' parknum='1' />"
		L"<SpotEvent create_time='20211211T071600Z' name='Death' Bdg='??p1w1[31180]' waynum='1' parknum='1' />"
		L"</HemPath>"
		);

	CPPUNIT_ASSERT( happen.path_count()==1 );
	CPPUNIT_ASSERT( happen.exist_series( "name", "Arrival Departure Transition Transition Transition Transition Transition Transition Arrival Death" ) );
	CPPUNIT_ASSERT( happen.exist_event( "@name='Transition' and @Bdg='?p1w1[31132]'" ) );
	CPPUNIT_ASSERT( happen.exist_event( "@name='Transition' and @Bdg='?p1w1[31139]'" ) );

	UtLayer<Gui::Layer> gui_layer;
	Gui::Event evSource = createEvent<Gui::Event>(
		L"<GuiEvent idPart1='o1639199220' idPart2='0000130491' esr='31151:31160' startTime='20211211T050700Z' enterInfoTime='20211211T050700Z' markType='3' posTop='28' posBottom='28' stopTime='20211211T054200Z' serviceCode='132' color='12' reversEsr='Y' />"
		);
	gui_layer.Push(evSource);

	CPPUNIT_ASSERT( gui_layer.path_count() == 1 );
	CPPUNIT_ASSERT( gui_layer.exist_event("@esr='31151:31160'") );


	Hem::Bill bill = createBill(
		L"<A2F_HINT Hint='cmd_mww' issue_moment='20211211T080000Z'>"
		L"<Action code='Create' District='31000-05110' DistrictRight='Y' layer='guiMark' />"
		L"<Data MarkReasonId1='o1639199220' MarkReasonId2='0000130491' picketing1_val='78~878' picketing1_comm='Рыбинск'>"
		L"<EventFrom name='Transition' Bdg='?p1w1[31132]' create_time='20211211T053117Z' waynum='1' parknum='1' />"
		L"<EventTo name='Transition' Bdg='?p1w1[31139]' create_time='20211211T054511Z' waynum='1' parknum='1' />"
		L"<GuiNote esr='31132:31139' enterInfoTime='20211211T053200Z' startTime='20211211T053200Z' posTop='34' posBottom='34' stopTime='20211211T054500Z' serviceCode='132' reversEsr='Y'>"
		L"<Comment />"
		L"<TrainDelayInfo reasonId1='o1639199220' reasonId2='0000130491' />"
		L"</GuiNote>"
		L"</Data>"
		L"</A2F_HINT>"
		);


	Hem::Bill::GuiNote bGuiNote = bill.build< Hem::Bill::GuiNote >();
	auto markInfo = bGuiNote.GetMarkInfo();

	aeGuiNoteCreate aeGui( gui_layer, happen.GetWriteAccess(), markInfo, Gui::MarkStaticInfo() );

	aeGui.Action();

	CPPUNIT_ASSERT( gui_layer.path_count() == 2 );
	//проверяем наличие стоянки на перегоне в реальной нити
	CPPUNIT_ASSERT( happen.exist_event("@name='Span_stopping_begin' and @Bdg='guiNote[31132:31139]' and @create_time='20211211T053200Z'") );
	CPPUNIT_ASSERT( happen.exist_event("@name='Span_stopping_end' and @Bdg='guiNote[31132:31139]' and @create_time='20211211T054500Z'") );

	attic::a_document res("Test");
	attic::a_node allEvents = res.document_element().append_child("All");
	allEvents << gui_layer;
	//проверяем наличие пометки "задержка поезда"
	CPPUNIT_ASSERT_EQUAL( size_t(2), allEvents.select_nodes("//GuiEvent[@idPart1]").size() );
	CPPUNIT_ASSERT( !allEvents.select_nodes("//GuiEvent//TrainDelayInfo[@reasonId1='o1639199220' and @reasonId2='0000130491']").empty() );
}

