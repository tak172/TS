#include "stdafx.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "TC_Hem_aeChangeListSpanWork.h"
#include "TC_Hem_ae_helper.h"
#include "../Hem/Bill.h"
#include "../Hem/UserChart.h"
#include "../Hem/Appliers/aeChangeListSpanWork.h"
#include "../Hem/SpanWorkInformator.h"
#include "../helpful/EsrGuide.h"
#include "TopologyTest.h"
#include "../Hem/Appliers/Context.h"
#include "../Hem/GuessTransciever.h"
#include "../Hem/LimitTrainKeeper.h"
#include "../Hem/UncontrolledStations.h"
#include "../helpful/NsiBasis.h"


using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeChangeListSpanWork );

void TC_Hem_aeChangeListSpanWork::WorkWithReturn()
{
    auto& hl = *happen_layer;
    hl.createPath(
        L"<HemPath>"
        L"<SpotEvent create_time='20200910T124650Z' name='Form' Bdg='1C[09736]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200910T124650Z' name='Departure' Bdg='1C[09736]' waynum='1' parknum='1' optCode='09736:09750' />"
        L"<SpotEvent create_time='20200910T124721Z' name='Span_move' Bdg='NR1IP[09736:09750]' waynum='1'>"
        L"<rwcoord picketing1_val='61~800' picketing1_comm='Рига-Тукумс' />"
        L"<rwcoord picketing1_val='63~800' picketing1_comm='Рига-Тукумс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200910T124729Z' name='Disform' Bdg='NR1IP[09736:09750]' waynum='1'>"
        L"<rwcoord picketing1_val='61~800' picketing1_comm='Рига-Тукумс' />"
        L"<rwcoord picketing1_val='63~800' picketing1_comm='Рига-Тукумс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(4) );

    Hem::Bill bill = createBill(
        L"<A2F_HINT Hint='cmd_edit' issue_moment='20200910T133105Z'>"
        L"<PrePoint>"
        L"<EventPoint name='Form' Bdg='1C[09736]' create_time='20200910T124650Z' waynum='1' parknum='1' />"
        L"<EventPoint name='Departure' Bdg='1C[09736]' create_time='20200910T124650Z' waynum='1' parknum='1' />"
        L"<EventPoint name='Span_move' Bdg='NR1IP[09736:09750]' create_time='20200910T124721Z' waynum='1'>"
        L"<rwcoord picketing1_val='62~800' picketing1_comm='Рига-Тукумс' />"
        L"</EventPoint>"
        L"<EventPoint name='Disform' Bdg='NR1IP[09736:09750]' create_time='20200910T124729Z' waynum='1'>"
        L"<rwcoord picketing1_val='62~800' picketing1_comm='Рига-Тукумс' />"
        L"</EventPoint>"
        L"</PrePoint>"
        L"<Action code='SpanWork' District='09010-09180' DistrictRight='Y' />"
        L"<PostPoint esr='09736:09750' waynum='1' start='20200910T124745Z' stop='20200910T132145Z' returnFlag='Y' num='8242' index='' remove='' />"
        L"</A2F_HINT>"
        );

    DistrictGuide dg;
    {
        attic::a_document doc;
        doc.load_wide(
            L"<DistrictList>"
            L"<District code='09010-09180' name='Riga-Jelgava'>"   L"<!-- рига- елгава  - вкладка Тукумс-Рига-Елгава-->"
            L"<Involve esr='09100:09104'/>"    L"<!--торнянкалнс  -8км -->"
            L"<Involve esr='09104'/>"    L"<!--8км -->"
            L"<Involve esr='09150'/>"    L"<!--олайне -->"
            L"<Involve esr='09160'/>"    L"<!--цена -->"
            L"<Involve esr='09160:09162'/>"    L"<!--цена-цукурфабрика -->"
            L"<Involve esr='09162'/>"     L"<!-- цукурфабрика -->"
            L"<Involve esr='09162:09180'/>"    L"<!-- цукурфабрика -->"
            L"<Involve esr='09676:09670'/>"       L"<!--  приедайне-засулаукс -->"
            L"<Involve esr='09676'/>"     L"<!-- приедайне -->"
            L"<Involve esr='09715'/>"   L"<!-- дубулти -->"
            L"<Involve esr='09730'/>"    L"<!--слока  -->"
            L"<Involve esr='09732'/>"   L"<!-- кемери -->"
            L"<Involve esr='09736'/>"    L"<!--тукумс1 -->"
            L"<Involve esr='09750:09736'/>"    L"<!-- тукумс2- тукумс1 -->"
            L"</District>"
            L"</DistrictList>" );

        dg.deserialize( doc.document_element() );
    }

    SpanWorkInformatorPtr spanworkInformator( new SpanWorkInformator() );
    std::shared_ptr<Note::Layer> noteLayer(new Note::Layer() );
    UserEditInventoryPtr userEditInventory( new UserEditInventory(nullptr, spanworkInformator, nullptr));
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;
    Context context(bill.getChance(), *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, userEditInventory, 0);
    Hem::ApplierCarryOut::aeChangeListSpanWork apl(  Hem::AccessHappenAsoup(hl.base(), asoup_layer->base()), noteLayer, context, dg.get_district(bill.getChance().getDCode()) , bill.getHeadAllSpotsThrow(), bill.getTailSpanWorkThrows(), bill.removeTail() );
    apl.Action();
    CPPUNIT_ASSERT( hl.path_count() == 1 );
    CPPUNIT_ASSERT( hl.exist_path_size(5) );
    CPPUNIT_ASSERT( hl.exist_series("name", "Form Departure Span_stopping_begin Span_stopping_end Disform" ) );
}

void TC_Hem_aeChangeListSpanWork::setUp()
{
    attic::a_document doc;
    EsrGuide eg; 
     doc.load_wide(
         L"<EsrGuide>"
         L"<EsrGuide kit='09736' name='Tukums-I' picketing1_val='61~300' picketing1_comm='Рига-Тукумс'>"
         L"<Way waynum='1' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='2' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' major='Y' passenger='Y' />"
         L"</EsrGuide>"
         L"<EsrGuide kit='09750' name='Tukums-II' picketing1_val='108~300' picketing1_comm='Вентспилс' picketing2_val='64~400' picketing2_comm='Рига-Тукумс'>"
         L"<Way waynum='1' parknum='1' passenger='Y' embarkation='Y' />"
         L"<Way waynum='2' parknum='1' major='Y' passenger='Y' embarkation='Y' />"
         L"<Way waynum='3' parknum='1' major='Y' passenger='Y' />"
         L"<Way waynum='4' parknum='1' />"
         L"<Way waynum='5' parknum='1' />"
         L"<Way waynum='6' parknum='1' />"
         L"</EsrGuide>"
         L"<SpanGuide kit='09736:09750' orientation='odd'>"
         L"<Way wayNum='1'>"
         L"<rwcoord picketing1_val='61~800' picketing1_comm='Рига-Тукумс' />"
         L"<rwcoord picketing1_val='63~800' picketing1_comm='Рига-Тукумс' />"
         L"</Way>"
         L"</SpanGuide>"
         L"</EsrGuide>");
     eg.load_from( doc.document_element() );
     happen_layer = make_shared< UtLayer<HappenLayer> >();
     asoup_layer = make_shared< UtLayer<AsoupLayer> >();
     topology.reset( new TopologyTest );
     esrGuide.reset( new EsrGuide );
     guessTransciever.reset( new Hem::GuessTransciever );
}

void TC_Hem_aeChangeListSpanWork::tearDown()
{
}

