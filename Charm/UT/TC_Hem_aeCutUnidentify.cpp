#include "stdafx.h"

#include "TC_Hem_aeCutUnidentify.h"
#include "TC_Hem_ae_helper.h"
#include "TC_Hem_aeRemove.h"
#include "UtHemHelper.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/Appliers/aeCutUnidentify.h"
#include "../Hem/Bill.h"
#include "../helpful/Time_Iso.h"
#include "../Hem/UserChart.h"
#include "../Guess/SpotDetails.h"

using namespace std;
using namespace attic;
using namespace HemHelpful;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeCutUnidentify );


void TC_Hem_aeCutUnidentify::SpanStation()
{
    UtLayer<HappenLayer> hl;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200212T081409Z' name='Form' Bdg='LI5[09510:11130]' waynum='1'>"
        L"<rwcoord picketing1_val='67~300' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='69~300' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200212T081457Z' name='Arrival' Bdg='5SP[09510]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200212T081656Z' name='Death' Bdg='5SP[09510]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200212T081701Z' name='Form' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200212T081752Z' name='Death' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"</HemPath>"
        );
     hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200212T081639Z' name='Form' Bdg='6-8SP:6-[09510]' />"
        L"<SpotEvent create_time='20200212T081701Z' name='Arrival' Bdg='14SP:14+[09510]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200212T081728Z' name='Departure' Bdg='14SP:14+[09510]' waynum='3' parknum='1' optCode='09510:09511' />"
        L"<SpotEvent create_time='20200212T081827Z' name='Span_move' Bdg='SL7[09510:09511]' waynum='1'>"
        L"<rwcoord picketing1_val='59~600' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='61~600' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200212T081832Z' name='Death' Bdg='SL7[09510:09511]' waynum='1'>"
        L"<rwcoord picketing1_val='59~600' picketing1_comm='Земитани-Валга' />"
        L"<rwcoord picketing1_val='61~600' picketing1_comm='Земитани-Валга' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 2 );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<Action code='CutUnidentify' District='09580-08620' DistrictRight='Y' FixBound='20190818T170000Z' SaveLog='Y' beginTime='20200212T080800Z' endTime='20200212T082100Z' />"
        L"<EsrList>"
        L"<Station esr='09510:11130' />"
        L"<Station esr='09510' />"
        L"<Station esr='09510:09511' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Hem::Bill::CutUnidentify area = bill.build< Hem::Bill::CutUnidentify >();
    Hem::ApplierCarryOut::aeCutUnidentify cut( hl.GetWriteAccess(), bill.getChance(), area.getTime(), area.getEsr() );
    cut.Action();

    // данные отката
    attic::a_document undoer;
    TC_Hem_aeRemove::collectUndo( undoer, hl );
    CPPUNIT_ASSERT( hl.path_count()==0 );

    // и отмена с проверкой 
    CPPUNIT_ASSERT( TC_Hem_aeRemove::implementUndo(undoer,hl) );
    CPPUNIT_ASSERT( hl.path_count() == 2 );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Arrival Death     Form      Death" ) );
    CPPUNIT_ASSERT( hl.exist_series( "name", "Form Arrival Departure Span_move Death" ) );
}

class UtUserChart : public UserChart
{
public:
    UserChart::getHappenLayer;
};

void TC_Hem_aeCutUnidentify::CheckOrderPathesUndoCommand()
{
    UtUserChart userChart;
    // 1. Ранняя нить с меньшим адресом
    auto undoer_1 = CutOperationsAndGetUndoString(userChart, false);

    //2. Ранняя нить с большим адресом
    auto undoer_2 = CutOperationsAndGetUndoString(userChart, true);

    CPPUNIT_ASSERT( undoer_1==undoer_2 );
}

std::wstring TC_Hem_aeCutUnidentify::CutOperationsAndGetUndoString(UtUserChart &userChart, bool bDescending)
{
    time_t curtime = time_from_iso("20200722T180000Z");
    HemHelpful::SpotEventPtr tmpSpot_1 = std::make_shared<SpotEvent>(SpotEvent( HCode::FORM,  BadgeE(L"1p",EsrKit(400)), curtime));
    HemHelpful::SpotEventPtr tmpSpot_2 = std::make_shared<SpotEvent>(SpotEvent( HCode::FORM,  BadgeE(L"2p",EsrKit(400)), curtime));
    std::vector<SpotEvent> vOne;
    vOne.push_back(SpotEvent( HCode::SPAN_MOVE,       BadgeE(L"1p",EsrKit(100,200)), curtime+10 ));
    vOne.push_back(SpotEvent( HCode::ARRIVAL,       BadgeE(L"1p",EsrKit(200)), curtime+15 ));
    vOne.push_back(SpotEvent( HCode::DEATH,       BadgeE(L"1p",EsrKit(200)), curtime+20 ));
    vOne.push_back(SpotEvent( HCode::FORM,  BadgeE(L"2p",EsrKit(200)), curtime+30 ));
    vOne.push_back(SpotEvent( HCode::DEATH, BadgeE(L"2p",EsrKit(200)), curtime+40 ));

    std::vector<SpotEvent> vTwo;
    vTwo.push_back(SpotEvent( HCode::FORM, BadgeE(L"1sp",EsrKit(200)), curtime+10 ));
    vTwo.push_back(SpotEvent( HCode::ARRIVAL,       BadgeE(L"2p",EsrKit(200)), curtime+30 ));
    vTwo.push_back(SpotEvent( HCode::DEPARTURE,       BadgeE(L"2p",EsrKit(200)), curtime+50 ));
    vTwo.push_back(SpotEvent( HCode::SPAN_MOVE,  BadgeE(L"2p",EsrKit(200,300)), curtime+55 ));
    vTwo.push_back(SpotEvent( HCode::DEATH, BadgeE(L"2p",EsrKit(200, 300)), curtime+60 ));

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit'>"
        L"<Action code='CutUnidentify' District='100-400' DistrictRight='Y' SaveLog='Y' beginTime='20200722T175000Z' endTime='20200722T180500' />"
        L"<EsrList>"
        L"<Station esr='100:200' />"
        L"<Station esr='200' />"
        L"<Station esr='200:300' />"
        L"</EsrList>"
        L"</A2F_HINT>"
        );

    Hem::Bill::CutUnidentify area = bill.build< Hem::Bill::CutUnidentify >();
    std::wstring errmsg;
    CPPUNIT_ASSERT (userChart.getHappenLayer().GetReadAccess()->getAllPathes().empty());

    CPPUNIT_ASSERT( userChart.acceptTrackerEvent(tmpSpot_1, 1, TrainCharacteristics::Source::Guess) );
    CPPUNIT_ASSERT( userChart.acceptTrackerEvent(tmpSpot_2, 2, TrainCharacteristics::Source::Guess) );

    auto& hl = userChart.getHappenLayer();
    attic::a_document dd("do");
    hl.TakeChanges( dd.document_element() );
    {
        auto& container = hl.GetWriteAccess();
        auto path1 = container->getPath( tmpSpot_1 );
        auto path2 = container->getPath( tmpSpot_2 );

        CPPUNIT_ASSERT(path1 && path2);
        if ( bDescending&&path2>path1 || !bDescending&&path2<path1 )
        {
            std::swap(tmpSpot_1, tmpSpot_2);
            std::swap(path2, path1);
        }

        CPPUNIT_ASSERT( container->replaceEvents(tmpSpot_1, tmpSpot_1, vOne, errmsg) );
        CPPUNIT_ASSERT( container->replaceEvents(tmpSpot_2, tmpSpot_2, vTwo, errmsg) );
        container.release();
    }

    hl.TakeChanges( dd.document_element() );

    Hem::ApplierCarryOut::aeCutUnidentify cut( hl.GetWriteAccess(), bill.getChance(), area.getTime(), area.getEsr() );
    cut.Action();

    attic::a_document undoer;
    TC_Hem_aeRemove::collectUndo( undoer, hl );
    hl.TakeChanges( dd.document_element() );
    CPPUNIT_ASSERT( hl.GetReadAccess()->getAllPathes().empty() );
    return undoer.pretty_wstr();
}
