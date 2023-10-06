#include "stdafx.h"

#include "../helpful/Attic.h"
#include "../Hem/Bill.h"
#include "TC_Hem_Bill.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_HemBill );

void TC_HemBill::LoadRemove()
{
    const wchar_t * text = 
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Departure' Bdg='3C[11290]' create_time='20151019T220012Z' index='0900-119-0660' num='2632' fretran='Y' waynum='3' parknum='1' />"
        L"  <Action code='Remove' />"
        L"</A2F_HINT>";

    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    CPPUNIT_ASSERT( bill.getAction() == Hem::Bill::Action::BA_REMOVE );
    auto head = bill.getHeadSpotThrows();
    CPPUNIT_ASSERT( head.GetCode() == HCode::DEPARTURE );
    CPPUNIT_ASSERT( head.GetBadge() == BadgeE( L"3C", EsrKit(11290) ) );
    
    CPPUNIT_ASSERT_THROW(bill.getTailSpotThrows(), HemHelpful::HemException);
}

void TC_HemBill::LoadAttach()
{
    const wchar_t * text = 
         L"<A2F_HINT Hint='cmd_edit'>"
         L"  <PrePoint name='Span_move' esr='09780:09790' Bdg='KS3C[09780:09790]' create_time='20150909T211237Z' waynum='1'>"
         L"    <rwcoord picketing1_val='75~538' picketing1_comm='Вентспилс' />"
         L"  </PrePoint>"
         L"  <Action code='Attach' />"
         L"  <PostPoint name='Span_move' esr='09764:09772' Bdg='SV1C[09764:09772]' create_time='20150909T211247Z' waynum='1'>"
         L"    <rwcoord picketing1_val='126~940' picketing1_comm='Вентспилс' />"
         L"  </PostPoint>"
         L"</A2F_HINT>";

    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    CPPUNIT_ASSERT( bill.getAction() == Hem::Bill::Action::BA_ATTACH );
    CPPUNIT_ASSERT( !bill.isAsoupHead() );
    CPPUNIT_ASSERT( !bill.isAsoupTail() );
    auto head = bill.getHeadSpotThrows();
    CPPUNIT_ASSERT( head.GetCode() == HCode::SPAN_MOVE );
    CPPUNIT_ASSERT( head.GetBadge() == BadgeE( L"KS3C", EsrKit(9780,9790) ) );
    auto tail = bill.getTailSpotThrows();
    CPPUNIT_ASSERT( tail.GetCode() == HCode::SPAN_MOVE );
    CPPUNIT_ASSERT( tail.GetBadge() == BadgeE( L"SV1C", EsrKit(9764,9772) ) );
}

void TC_HemBill::LoadAttach_AsoupHappen()
{
    const wchar_t * text = 
    L"<A2F_HINT Hint='cmd_edit'>\n"
    L"  <PrePoint layer='asoup' Bdg='ASOUP 1042[11310]' name='Departure' create_time='20151019T213000Z' index='0900-119-0705' num='2974' fretran='Y'><![CDATA[(:1042 909/000+11310 2974 0900 119 0705 03 11260 20 10 00 30 00/00 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 057 01337 00000 224 54821160 52816964 000 004 00 000 052.40 000 001 000 60 000 051 003 90 000 000 001 96 000 000 001\n"
    L"Ю3 579 00002691 1 23 14 0000 00000 БPИГA       \n"
    L"Ю3 579 00000000 9:)]]></PrePoint>\n"
    L"  <Action code='Attach' />\n"
    L"  <PostPoint name='Form' Bdg='1SP:1+[11271]' create_time='20151019T213628Z' />\n"
    L"</A2F_HINT>";

    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    CPPUNIT_ASSERT( bill.getAction() == Hem::Bill::Action::BA_ATTACH );
    CPPUNIT_ASSERT( bill.isAsoupHead() && !bill.isAsoupTail() );
    CPPUNIT_ASSERT( bill.getHeadSpotThrows().GetCode() == HCode::DEPARTURE );
    CPPUNIT_ASSERT( bill.getTailSpotThrows().GetCode() == HCode::FORM );
}

void TC_HemBill::LoadAttach_HappenAsoup()
{
    const wchar_t * text = 
        L"<A2F_HINT Hint='cmd_edit'>\n"
        L"  <PrePoint name='Span_move' Bdg='P3P[11000:11451]' create_time='20151019T224110Z' waynum='3'>\n"
        L"    <rwcoord picketing1_val='389~2' picketing1_comm='Вентспилс' />\n"
        L"  </PrePoint>\n"
        L"  <Action code='Attach' />\n"
        L"  <PostPoint layer='asoup' Bdg='ASOUP 1042[11000]' name='Arrival' create_time='20151019T225000Z' index='1655-069-1100' num='2861' fretran='Y'><![CDATA[(:1042 909/000+11000 2861 1655 069 1100 01 16169 20 10 01 50 00/00 1 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 2 056 05309 03637 256 73041956 73926834 000 000 00 064 000.70 064 000 000 73 064 000 000:)]]></PostPoint>\n"
        L"</A2F_HINT>";

    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    CPPUNIT_ASSERT( bill.getAction() == Hem::Bill::Action::BA_ATTACH );
    CPPUNIT_ASSERT( !bill.isAsoupHead() && bill.isAsoupTail() );
    CPPUNIT_ASSERT( bill.getHeadSpotThrows().GetCode() == HCode::SPAN_MOVE );
    CPPUNIT_ASSERT( bill.getTailSpotThrows().GetCode() == HCode::ARRIVAL );
}

void TC_HemBill::LoadAttach_AsoupAsoup()
{
    const wchar_t * text = 
        L"<A2F_HINT Hint='cmd_edit'>\n"
        L"  <PrePoint layer='asoup' Bdg='ASOUP 1042[11420]' name='Departure' create_time='20151019T223600Z' index='2231-254-0983' num='2255' fretran='Y'><![CDATA[(:1042 909/000+11420 2255 2231 254 0983 01 11310 20 10 01 36 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04906 03194 264 54663125 51600724 000 000 00 066 000.70 066 000 000 73 006 000 000\n"
        L"Ю3 530 00012591 1 22 06 0000 00000 SKRABA      \n"
        L"Ю3 530 00012592 9:)]]></PrePoint>\n"
        L"  <Action code='Attach' />\n"
        L"  <PostPoint layer='asoup' Bdg='ASOUP 1042[11000]' name='Arrival' create_time='20151020T001200Z' index='1655-070-0983' num='2867' fretran='Y'><![CDATA[(:1042 909/000+11000 2867 1655 070 0983 01 16169 20 10 03 12 00/00 1 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 056 05244 03583 256 74722653 92624865 000 000 00 064 000.70 063 000 000 73 063 000 000 90 001 000 000:)]]></PostPoint>\n"
        L"</A2F_HINT>";

    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    CPPUNIT_ASSERT( bill.getAction() == Hem::Bill::Action::BA_ATTACH );
    CPPUNIT_ASSERT( bill.isAsoupHead() && bill.isAsoupTail() );
    CPPUNIT_ASSERT( bill.getHeadSpotThrows().GetCode() == HCode::DEPARTURE );
    CPPUNIT_ASSERT( bill.getTailSpotThrows().GetCode() == HCode::ARRIVAL );
}

void TC_HemBill::LoadCreate()
{
    const wchar_t * text = 
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Span_move' Bdg='?[09210:09211]' create_time='20151019T220032Z'>"
        L"    <rwcoord picketing1_val='236~384' picketing1_comm='Вентспилс' />"
        L"  </PrePoint>"
        L"  <Action code='Create' />"
        L"  <PostPoint name='Span_move' Bdg='?[09211:09220]' create_time='20151019T222152Z'>"
        L"    <rwcoord picketing1_val='222~369' picketing1_comm='Вентспилс' />"
        L"  </PostPoint>"
        L"</A2F_HINT>";

    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    CPPUNIT_ASSERT( bill.getAction() == Hem::Bill::Action::BA_CREATE );
    CPPUNIT_ASSERT( !bill.isAsoupHead() && !bill.isAsoupTail() );
    CPPUNIT_ASSERT( bill.getHeadSpotThrows().GetCode() == HCode::SPAN_MOVE );
    CPPUNIT_ASSERT( bill.getTailSpotThrows().GetCode() == HCode::SPAN_MOVE );
}

void TC_HemBill::CheckTailAndHead()
{
    Hem::Bill bill;
    CPPUNIT_ASSERT(!bill.containsHead());
    CPPUNIT_ASSERT(!bill.containsTail());
    CPPUNIT_ASSERT_THROW(bill.getHeadSpotThrows(), HemHelpful::HemException);
    CPPUNIT_ASSERT_THROW(bill.getTailSpotThrows(), HemHelpful::HemException);
}

void TC_HemBill::LoadIdentifyRequests()
{
    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PrePoint name='Transition' Bdg='2p[200]' create_time='19700101T010120Z' />\n"
            L"    <Action code='Identify'>\n"
            L"        <EventInfo layer='asoup' Bdg='2p[200]' num='1379' index='' name='Transition' waynum='1' parknum='5' create_time='19700101T010022Z' />\n"
            L"    </Action>\n"
            L"    <PostPoint name='Transition' Bdg='4p[400]' waynum='1' parknum='5' create_time='19700101T010140Z' />\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());
        CPPUNIT_ASSERT(bill.containsHead());
        CPPUNIT_ASSERT(bill.containsTail());
        CPPUNIT_ASSERT(bill.getAdditionalInfo().m_asoupIdentify);
        CPPUNIT_ASSERT(bill.getAction() == Hem::Bill::Action::BA_IDENTIFY);
    }

    {
        const std::wstring request = 
            L"<Changing>\n"
            L"    <PrePoint name='Transition' Bdg='2p[200]' create_time='19700101T010020Z'/>\n"
            L"    <Action code='Identify'>\n"
            L"        <TrainInfo num='1379' index='' long='Y' heavy='Y' explosive='N' up='5' side='2' down='' super='Y' machinist='Иванов'>\n"
            L"            <lok num='625' />\n"
            L"            <lok num='121' />\n"
            L"        </TrainInfo>\n"
            L"    </Action>\n"
            L"    <PostPoint name='Transition' Bdg='5p[500]' waynum='1' parknum='5' create_time='19700101T010050Z' />\n"
            L"</Changing>";

        attic::a_document doc;
        CPPUNIT_ASSERT(doc.load_wide(request));

        Hem::Bill bill;
        bill.read(doc.document_element());
        CPPUNIT_ASSERT(bill.containsHead());
        CPPUNIT_ASSERT(bill.containsTail());
        CPPUNIT_ASSERT(bill.getAdditionalInfo().m_userDescr);
        CPPUNIT_ASSERT(bill.getAction() == Hem::Bill::Action::BA_IDENTIFY);
    }
}

void TC_HemBill::LoadCut()
{
    const wchar_t * text = 
        L"<A2F_HINT Hint='cmd_edit'>"
        L"  <PrePoint name='Transition' Bdg='1C[11301]' create_time='20151019T220137Z' index='' num='2611' fretran='Y' waynum='1' parknum='1' />"
        L"  <Action code='Cut' />"
        L"  <PostPoint name='Arrival' Bdg='6AC[11312]' create_time='20151019T221532Z' index='' num='2611' fretran='Y' waynum='6' parknum='1' />"
        L"</A2F_HINT>";

    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    CPPUNIT_ASSERT( bill.getAction() == Hem::Bill::Action::BA_CUT );
    CPPUNIT_ASSERT( !bill.isAsoupHead() && !bill.isAsoupTail() );
    CPPUNIT_ASSERT( bill.getHeadSpotThrows().GetCode() == HCode::TRANSITION );
    CPPUNIT_ASSERT( bill.getTailSpotThrows().GetCode() == HCode::ARRIVAL );
}

void TC_HemBill::FutureMoveOrder()
{
    const wchar_t * text = 
        L"<A2F_HINT Hint='cmd_predict'>"
        L"  <Action code='PredictOrderMoving' index='0982-038-1800' num='V2220' length='57' weight='1334' through='Y' District='09180-09820' DistrictRight='Y' esr='09750' remove='Y'>"
        L"    <Locomotive Series='2ТЭ116' NumLoc='970' Consec='1' CarrierCode='2'>"
        L"      <Crew EngineDriver='KONOVALOVS' Tim_Beg='2018-11-20 11:20' />"
        L"    </Locomotive>"
        L"    <TrainLeader index='' num='3273' divisional='Y'>"
        L"      <feat_texts finaldest='09813' />"
        L"    </TrainLeader>"
        L"  </Action>"
        L"</A2F_HINT>";
    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    using namespace Hem;

    CPPUNIT_ASSERT( bill.getAction() == Bill::Action::BA_FUTURE_MOVE_ORDER );
    Bill::FutureOrdering ordering = bill.build< Bill::FutureOrdering >();
    CPPUNIT_ASSERT( ordering.getEsr() == EsrKit(9750) );
    CPPUNIT_ASSERT( TrainDescr::equalNumberIndex( ordering.getLeader(), TrainDescr(L"V3273") ) );
    CPPUNIT_ASSERT( TrainDescr::equalNumberIndex( ordering.getSelf(),   TrainDescr(L"V2220", L"0982-038-1800") ) );
}

/*
void TC_HemBill::FutureMoveNonstop()
{
	const wchar_t * text = 
		L"<A2F_HINT Hint='cmd_predict'>"
		L"  <Action code='PredictTransition' index='0982-055-1100' num='J2220' length='57' weight='1561' through='Y' District='09180-11420' DistrictRight='Y' esr='09200'>"
		L"    <Locomotive Series='2ТЭ116' NumLoc='875' Consec='1' CarrierCode='2'>"
		L"      <Crew EngineDriver='PATRIJEVS' Tim_Beg='2018-06-19 13:59' />"
		L"    </Locomotive>"
		L"  </Action>"
		L"</A2F_HINT>";
	// подготовка
	attic::a_document doc;
	doc.load_wide( text );
	Hem::Bill bill;
	doc.document_element() >> bill;

	// сравниваем результаты
	using namespace Hem;

	CPPUNIT_ASSERT( bill.getAction() == Bill::Action::BA_FUTURE_MOVE_NONSTOP );
	Bill::FutureOrdering ordering = bill.build< Bill::FutureOrdering >();
	CPPUNIT_ASSERT( ordering.getEsr() == EsrKit(9200) );
	CPPUNIT_ASSERT( TrainDescr::equalNumberIndex( ordering.getLeader(), TrainDescr() ) );
	CPPUNIT_ASSERT( TrainDescr::equalNumberIndex( ordering.getSelf(),   TrainDescr(L"J2220", L"0982-055-1100") ) );
}
*/
void TC_HemBill::FutureMoveDefault()
{
    const wchar_t * text = 
        L"<A2F_HINT Hint='cmd_predict'>"
        L"  <Action code='PredictMoveDefault' index='0982-159-1800' num='K2344' length='37' weight='151' through='Y' District='09180-11420' DistrictRight='Y' esr='01800'>"
        L"    <Locomotive Series='2ТЭ116' NumLoc='875' Consec='1' CarrierCode='2' />"
        L"  </Action>"
        L"</A2F_HINT>";
    // подготовка
    attic::a_document doc;
    doc.load_wide( text );
    Hem::Bill bill;
    doc.document_element() >> bill;

    // сравниваем результаты
    using namespace Hem;

    CPPUNIT_ASSERT( bill.getAction() == Bill::Action::BA_FUTURE_MOVE_DEFAULT );
    Bill::FutureOrdering ordering = bill.build< Bill::FutureOrdering >();
    CPPUNIT_ASSERT( ordering.getEsr() == EsrKit(1800) );
    CPPUNIT_ASSERT( TrainDescr::equalNumberIndex( ordering.getLeader(), TrainDescr() ) );
    CPPUNIT_ASSERT( TrainDescr::equalNumberIndex( ordering.getSelf(),   TrainDescr(L"K2344", L"0982-159-1800") ) );
}
