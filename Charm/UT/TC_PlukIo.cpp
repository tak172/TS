#include "stdafx.h"
#include "TC_PlukIo.h"
#include "TC_PortablePgSQL.h"
#include "../Pluk/parse_change.h"
#include "../Pluk/pg_query.h"
#include "../helpful/TrainDescr.h"
#include "../helpful/SpanTravelTime.h"
#include "../helpful/Serialization.h"
#include "../Hem/XmlStrConst.h"
#include "../Hem/AsoupEvent.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/DncDspAttendance.h"
#include "../Guess/SpotDetails.h"
#include "../helpful/Locomotive.h"
#include "../helpful/Log.h"
#include "../Hem/NoteEvent.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukIo );

// тестовый разборщик изменений
class UT_parse_change : public parse_change
{
public:
    UT_parse_change(db_query_ptr _db_query, cache_ptr _cache)
        : parse_change(_db_query,_cache)
    {}
    // специальная реакция на ошибку (для обнаружения в тестах)
    void handle_error(const std::string& ) override
    {
        CPPUNIT_FAIL("UT_parse_change: error occured in DB");
    }
};

// тестовая БД
class UT_PlukDataBase : public PlukDataBase
{
public:
    UT_PlukDataBase(std::string stringConnect)
        : PlukDataBase(stringConnect)
    {}
    // создать тестовый разборщик изменений
    std::shared_ptr<parse_change> buildParseChange(std::shared_ptr<db::pg_query> db_query) override
    {
        return std::make_shared<UT_parse_change>(db_query, cache);
    }
};


void TC_PlukIo::InOutEvent()
{
    std::wstring srcText =
        L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Body create_time='20200324T220557Z' name='Transition' Bdg='mock[09340]' waynum='2' parknum='1' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y'>"
        L"          <rwcoord picketing1_val='24~400' picketing1_comm='Торнянкалнс-Елгава' />"
        L"          <rwcoord picketing1_val='26~500' picketing1_comm='Торнянкалнс-Елгава' />"
//        L"          <feat_texts finaldest='09860' typeinfo='Т' />"
        L"          <Locomotive Series='2ТЭ116' NumLoc='667' Depo='904' Consec='1' CarrierCode='2'>"
        L"            <Crew EngineDriver='M-Iļjins' TabNum='7163' Tim_Beg='2020-03-24 18:30' PersonMode='1' />"
        L"          </Locomotive>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='959' Depo='904' Consec='2' CarrierCode='2'>"
        L"            <Crew EngineDriver='22M-Iļjins' TabNum='7122' Tim_Beg='2020-03-24 18:32' />"
        L"          </Locomotive>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='1654' Depo='904' Consec='3' CarrierCode='2'>"
        L"            <Crew EngineDriver='3M-Iļjins' TabNum='7133' Tim_Beg='2020-03-24 18:23' />"
        L"          </Locomotive>"
        L"        </Body>"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;
    // записать событие в БД
    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );
    attic::a_xpath_node_set srcQuery = srcDoc.select_nodes("//HappenLayer/Trio/Body");
    CPPUNIT_ASSERT( srcQuery.begin() != srcQuery.end() );
    const SpotEvent srcEvent( srcQuery.begin()->node() );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );

    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string trgText = plukDB->getUserChart("20200324T000000Z", "20200325T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
    );
    attic::a_document trgDoc;
    CPPUNIT_ASSERT( trgDoc.load_utf8( trgText ) );
    attic::a_xpath_node_set trgQuery = trgDoc.select_nodes("//SpotEvent");
    CPPUNIT_ASSERT( trgQuery.begin() != trgQuery.end() );
    const SpotEvent trgEvent( trgQuery.begin()->node() );

    // проверить совпадение
    CPPUNIT_ASSERT( srcEvent == trgEvent );
    auto srcDet = srcEvent.GetDetails();
    auto trgDet = trgEvent.GetDetails();
    CPPUNIT_ASSERT( srcDet &&  trgDet );
    CPPUNIT_ASSERT( srcDet->tdptr  && trgDet->tdptr );
    /*
    if ( *srcDet->tdptr != *trgDet->tdptr )
    {
        attic::a_document dd("xx");
        attic::a_node srcNode = dd.document_element().append_child("td");
        srcNode << *srcDet->tdptr;
        attic::a_node trgNode = dd.document_element().append_child("td");
        trgNode << *trgDet->tdptr;
        std::wstring srcS = srcNode.pretty_wstr();
        std::wstring trgS = trgNode.pretty_wstr();
        if ( srcS != trgS )
        {
            auto mismres = std::mismatch( srcS.cbegin(), srcS.cend(), trgS.cbegin() );
            OutputDebugString(L"***");
        }
    }
    */
    CPPUNIT_ASSERT( *srcDet->tdptr == *trgDet->tdptr );
}


void TC_PlukIo::ChangeLoco()
{
    std::wstring srcText =
        L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Body create_time='20200324T220557Z' name='Form' Bdg='mock[09340]' waynum='2' parknum='1' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y'>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='667' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Body>"
        L"        <Body create_time='20200324T230557Z' name='Departure' Bdg='mock[09340]' waynum='2' parknum='1' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y'>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='224' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Body>"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;
    // записать событие в БД
    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );


    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart("20200324T000000Z", "20200325T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
    );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//SpotEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const SpotEvent srcEvent1( it->node() );

    auto srcDet = srcEvent1.GetDetails();
    CPPUNIT_ASSERT( srcDet &&  srcDet->tdptr && srcDet->tdptr->get_loks_number(',')==L"667" );
    it++;
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const SpotEvent srcEvent2( it->node() );
    srcDet = srcEvent2.GetDetails();
    CPPUNIT_ASSERT( srcDet &&  srcDet->tdptr && srcDet->tdptr->get_loks_number(',')==L"224" );


    std::wstring chgLocoText = L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Pre create_time='20200324T220557Z' name='Form' Bdg='mock[09340]' waynum='2' parknum='1' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y' cover='Y'>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='667' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Pre>"
        L"        <Post create_time='20200324T220557Z' name='Form' Bdg='mock[09340]' waynum='2' parknum='1' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y' cover='Y'>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='667' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Post>"
        L"        <Body create_time='20200324T220557Z' name='Form' Bdg='mock[09340]' waynum='2' parknum='1' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y' >"
        L"          <Locomotive Series='2ТЭ116' NumLoc='532' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Body>"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;

    attic::a_document chgDoc;
    CPPUNIT_ASSERT( chgDoc.load_wide(chgLocoText) );
    CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );


    std::string chgText = plukDB->getUserChart("20200324T000000Z", "20200325T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );

    attic::a_document chgdbDoc;
    CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgText ) );
    attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//SpotEvent");
    it = chgQuery.begin();
    CPPUNIT_ASSERT( it != chgQuery.end() );
    const SpotEvent chgEvent1( it->node() );

    auto chgDet = chgEvent1.GetDetails();
    CPPUNIT_ASSERT( chgDet &&  chgDet->tdptr && chgDet->tdptr->get_loks_number(',')==L"532" );
    //Проверяем сохранность локомотива на втором событии!
    it++;
    CPPUNIT_ASSERT( it != chgQuery.end() );
    const SpotEvent chgEvent2( it->node() );

    chgDet = chgEvent2.GetDetails();
    CPPUNIT_ASSERT( chgDet &&  chgDet->tdptr && chgDet->tdptr->get_loks_number(',')==L"224" );
}

void TC_PlukIo::InOutPvh()
{
    SpanTravelTime bodyPre, bodyAft;

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    for( unsigned pass=1; pass<=3; ++pass )
    {
        unsigned off = (pass%100);
        STT_Val val;
        val.set_gruz( (off+11)*60, (off+12)*60, (off+13)*60, (off+14)*60 );
        val.set_pass( (off+21)*60, (off+22)*60, (off+23)*60, (off+24)*60 );
        val.set_dies( (off+31)*60, (off+32)*60, (off+33)*60, (off+34)*60 );
        val.set_elek( (off+41)*60, (off+42)*60, (off+43)*60, (off+44)*60 );


        const unsigned esrFrom = 100;
        const unsigned esrTo  = 105;
        for( unsigned esr = esrFrom; esr < esrTo; ++esr )
        {
            SpanTravelTimeInfo spanInfo( EsrKit(esr), EsrKit(esr+1) );
                               
            bodyAft.SetSpanTime( spanInfo, val );
            SpanTravelTime diffPvh = bodyAft.subtractUnchanged( bodyPre );
            std::string strDiffPvh = serialize<SpanTravelTime>( diffPvh );
            CPPUNIT_ASSERT( plukDB->editPvh( strDiffPvh ) );

            bodyPre.SetSpanTime( spanInfo, val );
        }
        std::string backStr = plukDB->getEditedPvh();
        auto pSpanTravelTimePtr = deserialize<SpanTravelTime>(backStr);
        CPPUNIT_ASSERT( pSpanTravelTimePtr );
        CPPUNIT_ASSERT( !pSpanTravelTimePtr->empty() );
        std::string strA = serialize<SpanTravelTime>( bodyAft );
        std::string strB = serialize<SpanTravelTime>( *pSpanTravelTimePtr );
        CPPUNIT_ASSERT ( strA == strB );
    }
}


std::shared_ptr<PlukDataBase> TC_PlukIo::GetPlukDataBase(const std::string& stringConnect)
{
	auto plukDB = std::make_shared<UT_PlukDataBase>(stringConnect);
	const int max_wait = 5;
	int wait_count = 0;
	while (!plukDB->isConnect() && wait_count < max_wait) {
		++wait_count;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	if (wait_count == max_wait)
		throw std::runtime_error("Error connect to: " + stringConnect);
	else 
		return plukDB;
}

void TC_PlukIo::setUp()
{
	LocalPostgresInstance::Start();
}

void TC_PlukIo::DelEventProperty()
{
    std::wstring srcText =
        L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Body create_time='20200324T220557Z' name='Transition' Bdg='mock[09330]' waynum='2' parknum='1' optCode='09320:09330' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y'>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='667' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Body>"
//        L"        <Body create_time='20200324T230557Z' name='Transition' Bdg='mock[09340]' waynum='2' parknum='1' />"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;
    // записать событие в БД
    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );


    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart("20200324T000000Z", "20200325T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//SpotEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const SpotEvent srcEvent1( it->node() );

    auto srcDet = srcEvent1.GetDetails();
    CPPUNIT_ASSERT( srcDet &&  srcDet->tdptr && srcDet->tdptr->GetNumber().getNumber()==1957 && srcDet->optCode == EsrKit(9320, 9330));

    std::wstring delTrainDescrText = L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Pre create_time='20200324T220557Z' name='Transition' Bdg='mock[09330]' waynum='2' parknum='1' optCode='09320:09330' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y' cover='Y'>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='667' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Pre>"
        L"        <Post create_time='20200324T220557Z' name='Transition' Bdg='mock[09330]' waynum='2' parknum='1' optCode='09320:09330' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y' cover='Y'>"
        L"          <Locomotive Series='2ТЭ116' NumLoc='667' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Post>"
        L"        <Body create_time='20200324T220557Z' name='Transition' Bdg='mock[09330]' waynum='2' parknum='1' />"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;

    attic::a_document chgDoc;
    CPPUNIT_ASSERT( chgDoc.load_wide(delTrainDescrText) );
    CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );


    std::string chgText = plukDB->getUserChart("20200324T000000Z", "20200325T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );

    attic::a_document chgdbDoc;
    CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgText ) );
    attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//SpotEvent");
    it = chgQuery.begin();
    CPPUNIT_ASSERT( it != chgQuery.end() );
    const SpotEvent chgEvent1( it->node() );

    auto chgDet = chgEvent1.GetDetails();
    CPPUNIT_ASSERT( chgDet && chgDet->optCode.empty() && chgDet->tdptr==nullptr );
}

void TC_PlukIo::CheckLocomotiveForEventEqualTime_6192()
{
    std::wstring srcText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Body create_time='20210329T040500Z' name='Form' Bdg='№6107[09600]' waynum='1' parknum='1' index='' num='6107' length='6' weight='242' suburbreg='Y' >"
        L"          <Locomotive Series='ЭР2Т' NumLoc='7116' CarrierCode='24'>"
        L"            <Crew EngineDriver='Drozdovs' Tim_Beg='2021-03-29 06:57' />"
        L"          </Locomotive>"
        L"        </Body>"
        L"        <Body create_time='20210329T040500Z' name='Departure' Bdg='№6107[09600]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T040500Z' name='Transition' Bdg='№6107[09600]' waynum='1' parknum='2' />"
        L"        <Body create_time='20210329T041200Z' name='Arrival' Bdg='№6107[09604]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041230Z' name='Departure' Bdg='№6107[09604]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041730Z' name='Arrival' Bdg='№6107[09603]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041800Z' name='Death' Bdg='№6107[09603]' waynum='1' parknum='1' />"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;
    // записать событие в БД
    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );


    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//SpotEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const SpotEvent srcEvent1( it->node() );

    auto srcDet = srcEvent1.GetDetails();
    CPPUNIT_ASSERT( srcDet &&  srcDet->tdptr && srcDet->tdptr->GetNumber().getNumber()==6107 && srcDet->tdptr->Locomotives().size()==1 );

    {
        std::wstring chgEventText = L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
            L"  <UserChanges>"
            L"    <HappenLayer>"
            L"      <Trio>"
            L"        <Pre create_time='20210329T040500Z' name='Departure' Bdg='№6107[09600]' waynum='1' parknum='1' cover='Y'/>"
            L"        <Body create_time='20210329T040500Z' name='Departure' Bdg='№6107[09600]' waynum='1' parknum='1' cover='Y'/>"
            L"        <Body create_time='20210329T040500Z' name='Transition' Bdg='№6107[09600]' waynum='1' parknum='1' />"
            L"      </Trio>"
            L"    </HappenLayer>"
            L"  </UserChanges>"
            L"</SoxPostUpdChange>"
            ;

        attic::a_document chgDoc;
        CPPUNIT_ASSERT( chgDoc.load_wide(chgEventText) );
        CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );


        std::string chgText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
            std::vector<std::string>( begin(userLayers), end(userLayers) )
            );

        attic::a_document chgdbDoc;
        CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgText ) );
        attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//SpotEvent");
        it = chgQuery.begin();
        CPPUNIT_ASSERT( it != chgQuery.end() );
        const SpotEvent chgEvent1( it->node() );

        auto chgDet = chgEvent1.GetDetails();
        CPPUNIT_ASSERT( chgDet &&  chgDet->tdptr && chgDet->tdptr->GetNumber().getNumber()==6107 && chgDet->tdptr->Locomotives().size()==1 );
    }

    {
        std::wstring chgEventText = L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
            L"  <UserChanges>"
            L"    <HappenLayer>"
            L"      <Trio>"
            L"        <Pre create_time='20210329T040500Z' name='Transition' Bdg='№6107[09600]' waynum='1' parknum='2' cover='Y'/>"
            L"        <Post create_time='20210329T040500Z' name='Transition' Bdg='№6107[09600]' waynum='1' parknum='2' cover='Y'/>"
            L"        <Body create_time='20210329T040500Z' name='Transition' Bdg='№6107[09600]' waynum='1' parknum='3' />"
            L"      </Trio>"
            L"    </HappenLayer>"
            L"  </UserChanges>"
            L"</SoxPostUpdChange>"
            ;

        attic::a_document chgDoc;
        CPPUNIT_ASSERT( chgDoc.load_wide(chgEventText) );
        CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );


        std::string chgText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
            std::vector<std::string>( begin(userLayers), end(userLayers) )
            );

        attic::a_document chgdbDoc;
        CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgText ) );
        attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//SpotEvent");
        it = chgQuery.begin();
        CPPUNIT_ASSERT( it != chgQuery.end() );
        const SpotEvent chgEvent1( it->node() );

        auto chgDet = chgEvent1.GetDetails();
        CPPUNIT_ASSERT( chgDet &&  chgDet->tdptr && chgDet->tdptr->GetNumber().getNumber()==6107 && chgDet->tdptr->Locomotives().size()==1 );
    }

    {
        std::wstring chgEventText = L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
            L"  <UserChanges>"
            L"    <HappenLayer>"
            L"      <Trio>"
            L"        <Pre create_time='20210329T040500Z' name='Form' Bdg='№6107[09600]' waynum='1' parknum='1' index='' num='6107' length='6' weight='242' suburbreg='Y' cover='Y'>"
            L"          <Locomotive Series='ЭР2Т' NumLoc='7116' CarrierCode='24'>"
            L"            <Crew EngineDriver='Drozdovs' Tim_Beg='2021-03-29 06:57' />"
            L"          </Locomotive>"
            L"        </Pre>"
            L"        <Post create_time='20210329T040500Z' name='Form' Bdg='№6107[09600]' waynum='1' parknum='1' index='' num='6107' length='6' weight='242' suburbreg='Y' cover='Y'>"
            L"          <Locomotive Series='ЭР2Т' NumLoc='7116' CarrierCode='24'>"
            L"            <Crew EngineDriver='Drozdovs' Tim_Beg='2021-03-29 06:57' />"
            L"          </Locomotive>"
            L"        </Post>"
            L"        <Body create_time='20210329T040500Z' name='Form' Bdg='№6107[09600]' waynum='1' parknum='1' />"
            L"      </Trio>"
            L"    </HappenLayer>"
            L"  </UserChanges>"
            L"</SoxPostUpdChange>"
            ;

        attic::a_document chgDoc;
        CPPUNIT_ASSERT( chgDoc.load_wide(chgEventText) );
        CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );


        std::string chgText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
            std::vector<std::string>( begin(userLayers), end(userLayers) )
            );

        attic::a_document chgdbDoc;
        CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgText ) );
        attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//SpotEvent");
        it = chgQuery.begin();
        CPPUNIT_ASSERT( it != chgQuery.end() );
        const SpotEvent chgEvent1( it->node() );

        auto chgDet = chgEvent1.GetDetails();
#ifdef _CHECK_REMOVE_TRAIN
        CPPUNIT_ASSERT( chgDet &&  chgDet->tdptr==nullptr);
#else
        CPPUNIT_ASSERT( chgDet &&  (chgDet->tdptr==nullptr || chgDet->tdptr->Locomotives().empty()) );
#endif
    }
}

void TC_PlukIo::CheckLocomotiveForFirstEventEqualTime_6192()
{
    std::wstring srcText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Body create_time='20210329T040500Z' name='Form' Bdg='№6107[09600]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T040500Z' name='Departure' Bdg='№6107[09600]' waynum='1' parknum='1'  index='' num='6107' length='6' weight='242' suburbreg='Y' >"
        L"          <Locomotive Series='ЭР2Т' NumLoc='7116' CarrierCode='24'>"
        L"            <Crew EngineDriver='Drozdovs' Tim_Beg='2021-03-29 06:57' />"
        L"          </Locomotive>"
        L"        </Body>"
        L"        <Body create_time='20210329T040500Z' name='Transition' Bdg='№6107[09600]' waynum='1' parknum='2' />"
        L"        <Body create_time='20210329T041200Z' name='Arrival' Bdg='№6107[09604]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041230Z' name='Departure' Bdg='№6107[09604]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041730Z' name='Arrival' Bdg='№6107[09603]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041800Z' name='Death' Bdg='№6107[09603]' waynum='1' parknum='1' />"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;
    // записать событие в БД
    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );


    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//SpotEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const SpotEvent srcEvent1( it->node() );

    auto srcDet = srcEvent1.GetDetails();
    CPPUNIT_ASSERT( srcDet &&  srcDet->tdptr && srcDet->tdptr->GetNumber().getNumber()==6107 && srcDet->tdptr->Locomotives().size()==1 );

    {
        std::wstring chgEventText = L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
            L"  <UserChanges>"
            L"    <HappenLayer>"
            L"      <Trio>"
            L"        <Pre create_time='20210329T040500Z' name='Departure' Bdg='№6107[09600]' waynum='1' parknum='1'  index='' num='6107' length='6' weight='242' suburbreg='Y' cover='Y'>"
            L"          <Locomotive Series='ЭР2Т' NumLoc='7116' CarrierCode='24'>"
            L"            <Crew EngineDriver='Drozdovs' Tim_Beg='2021-03-29 06:57' />"
            L"          </Locomotive>"
            L"        </Pre>"
            L"        <Post create_time='20210329T040500Z' name='Departure' Bdg='№6107[09600]' waynum='1' parknum='1'  index='' num='6107' length='6' weight='242' suburbreg='Y' cover='Y'>"
            L"          <Locomotive Series='ЭР2Т' NumLoc='7116' CarrierCode='24'>"
            L"            <Crew EngineDriver='Drozdovs' Tim_Beg='2021-03-29 06:57' />"
            L"          </Locomotive>"
            L"        </Post>"
            L"        <Body create_time='20210329T040500Z' name='Departure' Bdg='№6107[09600]' waynum='1' parknum='1'  index='' num='6107' length='6' weight='242' suburbreg='Y' >"
            L"          <Locomotive Series='ЭР2Т' NumLoc='7122' CarrierCode='24'>"
            L"            <Crew EngineDriver='Drozdovs' Tim_Beg='2021-03-29 06:57' />"
            L"          </Locomotive>"
            L"        </Body>"
            L"      </Trio>"
            L"    </HappenLayer>"
            L"  </UserChanges>"
            L"</SoxPostUpdChange>"
            ;

        attic::a_document chgDoc;
        CPPUNIT_ASSERT( chgDoc.load_wide(chgEventText) );
        CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );


        std::string chgText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
            std::vector<std::string>( begin(userLayers), end(userLayers) )
            );

        attic::a_document chgdbDoc;
        CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgText ) );
        attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//SpotEvent");
        it = chgQuery.begin();
        CPPUNIT_ASSERT( it != chgQuery.end() );
        const SpotEvent chgEvent1( it->node() );

        auto chgDet = chgEvent1.GetDetails();
        CPPUNIT_ASSERT( chgDet &&  chgDet->tdptr && chgDet->tdptr->Locomotives().size()==1);
    }
}

void TC_PlukIo::CheckWriteAsoupFeatAndLoco_6194()
{
    std::wstring srcText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"<AutoChanges>"
        L"<AsoupLayer>"
        L"<Trio layer='asoup'>"
        L"<Body create_time='20210329T021200Z' name='Departure' Bdg='ASOUP 1042_1p7w[09180]' index='1255-505-0900' num='3166' length='57' weight='1526' net_weight='181' divisional='Y' waynum='7' parknum='1' dirTo='09000' adjTo='09162'><![CDATA[(:1042 909/000+09180 3166 1255 505 0900 03 09000 29 03 05 12 01/07 0 0/00 00 0\n"
        L"Ю2 0 00  Д 00 00 0000 0 0 057 01526 00181 224 29006848 64603145 000 006 00 003 047.20 000 002 000 60 003 045 006\n"
        L"Ю3 530 00009971 1 22 00 0000 00000 MELENCUKS   \n"
        L"Ю3 530 00009972 9\n"
        L"Ю4 00000 0 00 000 004.60 000 004 \n"
        L"Ю4 08620 2 08 000 001.20 000 001 \n"
        L"Ю4 00000 1 01 003 042.20 000 001 60 003 041 \n"
        L"Ю4 09480 1 01 000 005.20 000 001 60 000 004 \n"
        L"Ю4 09740 1 01 000 037.60 000 037 \n"
        L"Ю4 09640 1 01 003 000.60 003 000 \n"
        L"Ю4 00000 3 00 002 000.60 002 000 \n"
        L"Ю12 00 29006848 1 000 08620 43619 2428 000 00 00 00 00 00 0253 12 20 0211 04 134 09000 08620 08620 08 12550 17 00000 0000 026 1 1080 E      128 000 00000000\n"
        L"Ю12 00 64707144 195225069135 097404210383273000235136006001000970000000014302000000126707\n"
        L"Ю12 00 64628357 255239255255 0241\n"
        L"Ю12 00 64569452 219231223247 0900090002424009000ПEPECЛ\n"
        L"Ю12 00 64542616 219247223247 097403001309700126707\n"
        L"Ю12 00 64538739 251255255255 500\n"
        L"Ю12 00 62128723 255255255255 \n"
        L"Ю12 00 63291124 255239255255 0240\n"
        L"Ю12 00 64361256 255255255255 \n"
        L"Ю12 00 64290059 131247216115 0690964031607802500030096400900013120007961LAT   132\n"
        L"Ю12 00 57952376 131229216115 000097404210385173000241135600097001255043000000000E     128\n"
        L"Ю12 00 55139208 255255255255 \n"
        L"Ю12 00 61066908 255237255255 02330600\n"
        L"Ю12 00 56685191 251237255255 00002375600\n"
        L"Ю12 00 62383815 255237255247 02440600COBETC\n"
        L"Ю12 00 60490083 251239255247 4000240E     \n"
        L"Ю12 00 60653482 255255255255 \n"
        L"Ю12 00 61194338 255255255255 \n"
        L"Ю12 00 55318810 255237255255 02425600\n"
        L"Ю12 00 61892840 251237255255 30002400600\n"
        L"Ю12 00 60488368 251255255255 400\n"
        L"Ю12 00 61436036 251239255255 3000237\n"
        L"Ю12 00 62507207 251239255247 0000245COBETC\n"
        L"Ю12 00 63394175 255239255251 0247144\n"
        L"Ю12 00 59529107 219229223243 09000900023540560009000ПEPECЛ128\n"
        L"Ю12 00 54365341 219231223243 09740000024013097000     144\n"
        L"Ю12 00 61373544 255237255255 02450600\n"
        L"Ю12 00 53570784 255237255255 02435600\n"
        L"Ю12 00 58596891 255239255255 0231\n"
        L"Ю12 00 64450562 255237255255 02480600\n"
        L"Ю12 00 64002165 135231216051 06309640316078025022730096400900013125808269024E12588148\n"
        L"Ю12 00 67629444 191239255255 0490240\n"
        L"Ю12 00 61782157 131231216051 00009000421039999900024840090001255043000000000020ПEPECЛ128\n"
        L"Ю12 00 52190998 231237255255 42103851702365600\n"
        L"Ю12 00 61506176 195245213179 0000023402751400011060000600001902109247 144\n"
        L"Ю12 00 61232948 255239255255 0230\n"
        L"Ю12 00 64420177 255239255255 0240\n"
        L"Ю12 00 61241907 255239255255 0233\n"
        L"Ю12 00 57605669 199229213183 097404210385170231135600097000143020COBETC\n"
        L"Ю12 00 63438378 219229223243 09000900024640060009000ПEPECЛ128\n"
        L"Ю12 00 64393549 255239255255 0247\n"
        L"Ю12 00 52383668 195225091243 094804210379680000257132052061220949009000E     144\n"
        L"Ю12 00 59958280 199233091255 0974042103871502416056001000970012550\n"
        L"Ю12 00 54790662 255239255255 0240\n"
        L"Ю12 00 53391736 195255219255 094804210385654000949009000\n"
        L"Ю12 00 53391769 255255255255 \n"
        L"Ю12 00 55669766 255255255255 \n"
        L"Ю12 00 56555691 255239255255 0236\n"
        L"Ю12 00 64495757 195237219247 09740421038327300024206000970012550126707\n"
        L"Ю12 00 64249279 255239255255 0240\n"
        L"Ю12 00 62230115 255255255255 \n"
        L"Ю12 00 63281950 255239255255 0241\n"
        L"Ю12 00 55570709 255237255255 02365600\n"
        L"Ю12 00 52997780 255239255255 0232\n"
        L"Ю12 00 64235500 255237255255 02410600\n"
        L"Ю12 00 64603145 255255255255 :)]]>"
        L"<feat_texts typeinfo='Д' />"
        L"<Locomotive Series='2ТЭ116' NumLoc='997' Consec='1'>"
        L"<Crew EngineDriver='MELENCUKS' Tim_Beg='2021-03-28 22:00' />"
        L"</Locomotive>"
        L"</Body>"
        L"</Trio>"
        L"</AsoupLayer>"
        L"</AutoChanges>"
        L"</SoxPostUpdChange>"
      ;
    // записать событие в БД
    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );


    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getAutoChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//AsoupEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const AsoupEvent srcEvent1( it->node() );

    auto tdr = srcEvent1.GetDescr();
    CPPUNIT_ASSERT( tdr &&  tdr->GetNumber().getNumber()==3166 && tdr->Locomotives().size()==1 );
}

void TC_PlukIo::CheckWriteAsoupSomeLocomotives_6194()
{
    std::wstring srcText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
L"<AutoChanges>"
L"<AsoupLayer>"
L"<Trio layer='asoup'>"
L"<Body create_time='20210329T020000Z' name='Arrival' Bdg='26.052916,57.774532[08620]' index='' num='7832' length='12' weight='370' temot='Y'><![CDATA[test text]]>"
L"<Locomotive Series='ДР1АЦ' NumLoc='185' CarrierCode='24'>"
L"<Crew EngineDriver='Vilmanis' Tim_Beg='2021-03-29 04:57' />"
L"</Locomotive><Locomotive Series='ДР1АЦ' NumLoc='219' CarrierCode='24'><Crew EngineDriver='Vilmanis' Tim_Beg='2021-03-29 04:57' />"
L"</Locomotive>"
L"</Body>"
L"</Trio>"
L"</AsoupLayer>"
L"</AutoChanges>"
        L"</SoxPostUpdChange>"
        ;

    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );


    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getAutoChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//AsoupEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const AsoupEvent srcEvent1( it->node() );

    auto tdr = srcEvent1.GetDescr();
    CPPUNIT_ASSERT( tdr &&  tdr->GetNumber().getNumber()==7832 && tdr->Locomotives().size()==2 );
}

void TC_PlukIo::CheckMarkWriteRead_6210()
{
    std::wstring srcText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"<UserChanges>"
        L"<MarkLayer>"
        L"<Trio layer='mark'>"
        L"<Body Id='i1' esr='11062:11063' create_time='20210329T021821Z' print='Y' Comment='Поезд D3579 на перегоне Naujene - C.p. 401km. Превышение ПВХ.' inner_name='srv10' auto='Y'>"
        L"<picketing picketing1_val='404~250' picketing1_comm='Вентспилс' />"
//         L"<Comment Text='Поезд %s на перегоне %s. Превышение ПВХ.' />"
//         L"<Comment Text='D3579' />"
//         L"<Comment Text='Naujene - C.p. 401km' />"
L"<Comment Text='train %s on span %s. Upper PVH.' />"
L"<Comment Text='D3579' />"
L"<Comment Text='Naujene - C.p. 401km' />"
        L"</Body>"
        L"</Trio>"
        L"</MarkLayer>"
        L"</UserChanges>"
        L"</SoxPostUpdChange>"
        ;

    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );

    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//MarkEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const Note::Event srcEvent1( it->node() );
    CPPUNIT_ASSERT(srcEvent1.GetDescription().size()==3);
}

void TC_PlukIo::CheckEditAsoup()
{
    std::wstring srcText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20210329T053000Z'>"
        L"<UserChanges>"
        L"<AsoupLayer>"
        L"<Trio layer='asoup'>"
    L"<Body create_time='20210329T053000Z' name='Departure' Bdg='ASOUP 1042_1p3w[09180]' index='0001-060-1263' num='4471' reslocfast='Y' waynum='3' parknum='1' dirTo='09960' adjTo='09280'>"
        L"<![CDATA[(:1042 909/000+09180 4471 0001 060 1263 03 09960 29 03 08 30 01/03 0 0/00 00 0\n"
        L"Ю3 530 00009281 1 07 29 0000 00000 KURCANOVS   \n"
        L"Ю3 530 00009282 9:)]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1'>"
        L"<Crew EngineDriver='KURCANOVS' Tim_Beg='2021-03-29 07:29' />"
        L"</Locomotive>"
        L"</Body>"
        L"</Trio>"
        L"</AsoupLayer>"
        L"</UserChanges>"
        L"</SoxPostUpdChange>";

        attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );

    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//AsoupEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const AsoupEvent srcEvent1( it->node() );
    CPPUNIT_ASSERT( srcEvent1.GetDescr()->GetNumber().getNumber()==4471 );
    CPPUNIT_ASSERT( !it->node().attribute("linkBadge") );

    std::wstring chgText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20210329T053000Z'>"
        L"<UserChanges>"
        L"<AsoupLayer>"
        L"<Trio layer='asoup'>"
        L"<Pre create_time='20210329T053000Z' name='Departure' Bdg='ASOUP 1042_1p3w[09180]' index='0001-060-1263' num='4471' reslocfast='Y' waynum='3' parknum='1' dirTo='09960' adjTo='09280' cover='Y'>"
        L"<![CDATA[(:1042 909/000+09180 4471 0001 060 1263 03 09960 29 03 08 30 01/03 0 0/00 00 0\n"
        L"Ю3 530 00009281 1 07 29 0000 00000 KURCANOVS   \n"
        L"Ю3 530 00009282 9:)]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='KURCANOVS' Tim_Beg='2021-03-29 07:29' />"
        L"</Locomotive>"
        L"</Pre>"
        L"<Body create_time='20210329T053000Z' name='Departure' Bdg='ASOUP 1042_1p3w[09180]' index='0001-060-1263' num='4471' reslocfast='Y' waynum='3' parknum='1' dirTo='09960' adjTo='09280' linkBadge='3C[09180]' linkTime='20210329T052955Z' linkCode='Departure'>"
        L"<![CDATA[(:1042 909/000+09180 4471 0001 060 1263 03 09960 29 03 08 30 01/03 0 0/00 00 0\n"
        L"Ю3 530 00009281 1 07 29 0000 00000 KURCANOVS   \n"
        L"Ю3 530 00009282 9:)]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='KURCANOVS' Tim_Beg='2021-03-29 07:29' />"
        L"</Locomotive>"
        L"</Body>"
        L"</Trio>"
        L"</AsoupLayer>"
        L"</UserChanges>"
        L"</SoxPostUpdChange>";

    attic::a_document chgDoc;
    CPPUNIT_ASSERT( chgDoc.load_wide(chgText) );

    CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );

    std::string chgdbText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document chgdbDoc;
    CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgdbText ) );
    attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//AsoupEvent");
    it = chgQuery.begin();
    CPPUNIT_ASSERT( it != chgQuery.end() );
    const AsoupEvent chgEvent1( it->node() );
    CPPUNIT_ASSERT( chgEvent1.GetDescr()->GetNumber().getNumber()==4471 );
    CPPUNIT_ASSERT( it->node().attribute("linkBadge") );
}

void TC_PlukIo::CheckDeleteAsoup()
{
    std::wstring srcText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20210329T053000Z'>"
        L"<UserChanges>"
        L"<AsoupLayer>"
        L"<Trio layer='asoup'>"
        L"<Body create_time='20210329T053000Z' name='Departure' Bdg='ASOUP 1042_1p3w[09180]' index='0001-060-1263' num='4471' reslocfast='Y' waynum='3' parknum='1' dirTo='09960' adjTo='09280'>"
        L"<![CDATA[(:1042 909/000+09180 4471 0001 060 1263 03 09960 29 03 08 30 01/03 0 0/00 00 0\n"
        L"Ю3 530 00009281 1 07 29 0000 00000 KURCANOVS   \n"
        L"Ю3 530 00009282 9:)]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1'>"
        L"<Crew EngineDriver='KURCANOVS' Tim_Beg='2021-03-29 07:29' />"
        L"</Locomotive>"
        L"</Body>"
        L"</Trio>"
        L"</AsoupLayer>"
        L"</UserChanges>"
        L"</SoxPostUpdChange>";

    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );

    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//AsoupEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const AsoupEvent srcEvent1( it->node() );
    CPPUNIT_ASSERT( srcEvent1.GetDescr()->GetNumber().getNumber()==4471 );
    CPPUNIT_ASSERT( !it->node().attribute("linkBadge") );

    std::wstring chgText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20210329T053000Z'>"
        L"<UserChanges>"
        L"<AsoupLayer>"
        L"<Trio layer='asoup'>"
        L"<Pre create_time='20210329T053000Z' name='Departure' Bdg='ASOUP 1042_1p3w[09180]' index='0001-060-1263' num='4471' reslocfast='Y' waynum='3' parknum='1' dirTo='09960' adjTo='09280' cover='Y'>"
        L"<![CDATA[(:1042 909/000+09180 4471 0001 060 1263 03 09960 29 03 08 30 01/03 0 0/00 00 0\n"
        L"Ю3 530 00009281 1 07 29 0000 00000 KURCANOVS   \n"
        L"Ю3 530 00009282 9:)]]>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='KURCANOVS' Tim_Beg='2021-03-29 07:29' />"
        L"</Locomotive>"
        L"</Pre>"
        L"</Trio>"
        L"</AsoupLayer>"
        L"</UserChanges>"
        L"</SoxPostUpdChange>";

    attic::a_document chgDoc;
    CPPUNIT_ASSERT( chgDoc.load_wide(chgText) );

    CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );

    std::string chgdbText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document chgdbDoc;
    CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgdbText ) );
    attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//AsoupEvent");
    it = chgQuery.begin();
    CPPUNIT_ASSERT( it == chgQuery.end() );
}


void TC_PlukIo::CheckDeleteLiter_6192()
{
    std::wstring srcText =  L"<?xml version='1.0'?>"
        L"<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Body create_time='20210329T040500Z' name='Form' Bdg='#6107[09600]' waynum='1' parknum='1' index='' num='6107' length='6' weight='242' suburbreg='Y' >"
        L"              <feat_texts typeinfo='М' />"
        L"        </Body>"
        L"        <Body create_time='20210329T040500Z' name='Departure' Bdg='№6107[09600]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T040500Z' name='Transition' Bdg='№6107[09600]' waynum='1' parknum='2' />"
        L"        <Body create_time='20210329T041200Z' name='Arrival' Bdg='№6107[09604]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041230Z' name='Departure' Bdg='№6107[09604]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041730Z' name='Arrival' Bdg='№6107[09603]' waynum='1' parknum='1' />"
        L"        <Body create_time='20210329T041800Z' name='Death' Bdg='№6107[09603]' waynum='1' parknum='1' />"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;
    // записать событие в БД
    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide(srcText) );

    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );


    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
        std::vector<std::string>( begin(userLayers), end(userLayers) )
        );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes("//SpotEvent");
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const SpotEvent srcEvent1( it->node() );

    auto srcDet = srcEvent1.GetDetails();
    CPPUNIT_ASSERT( srcDet &&  srcDet->tdptr && srcDet->tdptr->GetNumber().getNumber()==6107 && srcDet->tdptr->isOnePersonMode() );

    {
        std::wstring chgEventText = L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
            L"  <UserChanges>"
            L"    <HappenLayer>"
            L"      <Trio>"
            L"        <Pre create_time='20210329T040500Z' name='Form' Bdg='#6107[09600]' waynum='1' parknum='1'  index='' num='6107' length='6' weight='242' suburbreg='Y' cover='Y'>"
            L"              <feat_texts typeinfo='М' />"
            L"        </Pre>"
            L"        <Post create_time='20210329T040500Z' name='Form' Bdg='#6107[09600]' waynum='1' parknum='1'  index='' num='6107' length='6' weight='242' suburbreg='Y' cover='Y'>"
            L"              <feat_texts typeinfo='М' />"
            L"        </Post>"
            L"        <Body create_time='20210329T040500Z' name='Form' Bdg='#6107[09600]' waynum='1' parknum='1'  index='' num='6107' length='6' weight='242' suburbreg='Y' >"
            L"        </Body>"
            L"      </Trio>"
            L"    </HappenLayer>"
            L"  </UserChanges>"
            L"</SoxPostUpdChange>"
            ;

        attic::a_document chgDoc;
        CPPUNIT_ASSERT( chgDoc.load_wide(chgEventText) );
        CPPUNIT_ASSERT( plukDB->writeChanges( chgDoc.to_str() ) );


        std::string chgText = plukDB->getUserChart("20210329T000000Z", "20210330T000000Z",
            std::vector<std::string>( begin(userLayers), end(userLayers) )
            );

        attic::a_document chgdbDoc;
        CPPUNIT_ASSERT( chgdbDoc.load_utf8( chgText ) );
        attic::a_xpath_node_set chgQuery = chgdbDoc.select_nodes("//SpotEvent");
        it = chgQuery.begin();
        CPPUNIT_ASSERT( it != chgQuery.end() );
        const SpotEvent chgEvent1( it->node() );

        auto chgDet = chgEvent1.GetDetails();
        CPPUNIT_ASSERT( chgDet &&  chgDet->tdptr && !chgDet->tdptr->isOnePersonMode() );
    }
}

// проверка, что все элементы эталона присутствуют в тесте
static bool exist_all( attic::a_node etalon, attic::a_node test, size_t& processed )
{
    for( attic::a_node n : etalon.children() )
    {
        processed++;
        std::string view_n = n.to_str();
        std::string request;
        request = "";
        request += n.name();
        bool start = true;
        for( attic::a_attribute a : n.attributes() )
        {
            if( start )
            {
                start = false;
                request += "[";
            }
            else
            {
                request += " and ";
            }
            request += "@";
            request += a.name();
            request += "='";
            request += a.value();
            request += "'";
        }
        if( !start )
            request += "]";

        attic::a_node tn = test.select_node( request.c_str() ).node();
        if( !tn )
            return false;
        std::string view_tn = tn.to_str();
        if( view_n.empty() || view_tn.empty() )
            return false;
        // проверить всех потомков
        if ( !exist_all( n, tn, processed ) )
            return false;
    }
    return true;
}

void TC_PlukIo::Check_DD_Attendance()
{
    std::wstring attText =
        L"<HemChanges>"
        L"  <TerraChanges>"
        L"    <DncDspAttendance>"
        // district
        L"      <District code='09180-09820' >"
        L"        <Handover time='20180227T110000Z' />"
        L"        <Handover time='20180227T110111Z' revert='20190430T092222Z' unlocker='' />"
        L"        <Handover time='20180227T110333Z' />"
        L"        <Dnc start='20230328T170000Z' persona='G.Grantiņš' />"
        L"        <Dnc start='20230329T050000Z' persona='R.Konstantinovs' />"
        L"        <List>"
        L"          <Dnc persona='A.Lavžele' />"
        L"          <Dnc persona='D.Joņica' />"
        L"          <Dnc persona='G.Grantiņš' />"
        L"          <Dnc persona='R.Konstantinovs' />"
        L"        </List>"
        L"      </District>"
        L"      <District code='09180-09860'>"
        L"        <Handover time='20180622T152600Z' />"
        L"        <Handover time='20190213T172200Z' />"
        L"        <Dnc start='20210106T060000Z' persona='J.Joņica' />"
        L"        <List>"
        L"          <Dnc persona='A.Lavžele' />"
        L"          <Dnc persona='A.Pakuļņevica' />"
        L"          <Dnc persona='J.Joņica' />"
        L"        </List>"
        L"      </District>"
        // station
        L"      <Station ESR_code='09000'>"
        L"          <Dsp start='20230328T170000Z' persona='Vasiljeva'/>"
        L"          <Dsp start='20230329T050000Z' persona='Fokina'/>"
        L"          <Operator start='20230328T170000Z' persona='Jermolajeva'/>"
        L"          <Operator start='20230329T050000Z' persona='Gavrilova'/>"
        L"          <List>"
        L"              <Dsp persona='Akilans'/>"
        L"              <Dsp persona='Fokina'/>"
        L"              <Dsp persona='Fokina Kristīne'/>"
        L"              <Dsp persona='Garasjuka'/>"
        L"              <Dsp persona='Vasiljeva'/>"
        L"              <Operator persona='Akilans'/>"
        L"              <Operator persona='Gavrilova'/>"
        L"              <Operator persona='Jermolajeva'/>"
        L"          </List>"
        L"      </Station>"
        L"      <Station ESR_code='09008'>"
        L"          <Dsp start='20230328T170000Z' persona='Rudzinska'/>"
        L"          <Dsp start='20230329T050000Z' persona='Matvejeva'/>"
        L"          <List>"
        L"              <Dsp persona='Jegorova'/>"
        L"              <Dsp persona='Matvejeva'/>"
        L"              <Dsp persona='Rudzinska'/>"
        L"              <Dsp persona='nav'/>"
        L"          </List>"
        L"      </Station>"
        L"      <Station ESR_code='09010'>"
        L"          <Dsp1 start='20230328T170000Z' persona='Zelepukins'/>"
        L"          <Dsp1 start='20230329T050000Z' persona='Kaļinina'/>"
        L"          <Dsp1 start='20230330T050000Z'/>"
        L"          <Dsp2 start='20230327T170000Z' persona='Kimstača'/>"
        L"          <Dsp2 start='20230330T050000Z' persona='Kimstača'/>"
        L"          <Operator start='20191205T073510Z'/>"
        L"          <List>"
        L"              <Dsp persona='Alabjevs'/>"
        L"              <Dsp persona='Antipovs'/>"
        L"              <Dsp persona='Kaļinina' />"
        L"              <Dsp persona='Kimstača' />"
        L"              <Dsp persona='Zelepukins' />"
        L"              <Operator persona='Masans'/>"
        L"              <Operator persona='Nazarenko 3'/>"
        L"          </List>"
        L"      </Station>"
        L"    </DncDspAttendance>"
        L"  </TerraChanges>"
        L"</HemChanges>";

    // Сформировать список
    attic::a_document attDoc;
    DncDsp::Attendance tempAttendanceIn;
    {
        attic::a_document middleDoc;
        CPPUNIT_ASSERT( middleDoc.load_wide( attText ) );
        middleDoc.document_element().first_child() >> tempAttendanceIn;
        attDoc.set_document_element( "HemChanges" ).append_child( "TerraChanges" ) << tempAttendanceIn;
    }
    
    // записать событие в БД
    auto plukDB = GetPlukDataBase( LocalPostgresInstance::GetStringConnect() );
    CPPUNIT_ASSERT( plukDB->writeChanges( attDoc.to_str() ) );

    // прочитать из базы обратно
    std::vector<std::string> backLayers;
    backLayers.push_back( DncDspAttendance_xAttr );
    std::string trgText = plukDB->getTerraChart( "20180101T000000Z", "20250202T000000Z", backLayers );

    DncDsp::Attendance tempAttendanceOut;
    attic::a_document trgDoc( "test" );
    {
        attic::a_document middleDoc;
        CPPUNIT_ASSERT( middleDoc.load_utf8( trgText ) );
        middleDoc.document_element() >> tempAttendanceOut;
        trgDoc.set_document_element( "HemChanges" ).append_child( "TerraChanges" ) << tempAttendanceOut;
    }
    // проверить наличие всех исходных данных в результате
    // (хотя там может быть что-то лишнее)
    size_t processed_nodes = 0;
    bool compatible = exist_all( attDoc.child( "HemChanges" ), trgDoc.child( "HemChanges" ), processed_nodes );
    CPPUNIT_ASSERT( processed_nodes != 0 );
    CPPUNIT_ASSERT( compatible );
}

void TC_PlukIo::CheckTrainWarning()
{
    std::wstring srcText =
        L"<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        L"  <UserChanges>"
        L"    <HappenLayer>"
        L"      <Trio>"
        L"        <Body create_time='20200324T220557Z' name='Form' Bdg='mock[09340]' waynum='2' parknum='1' "
        L"              index='1100-060-0987' num='J1957' length='57' weight='4693' net_weight='3355' liqrr='Y'>"
        L"          <feat_texts limitspeed='22/11' typeinfo='ДТ' />"
        L"          <Locomotive Series='2ТЭ116' NumLoc='667' Depo='904' Consec='1' CarrierCode='2' />"
        L"        </Body>"
        L"        <Body create_time='20200324T230557Z' name='Departure' Bdg='mock[09340]' waynum='2' parknum='1' />"
        L"      </Trio>"
        L"    </HappenLayer>"
        L"  </UserChanges>"
        L"</SoxPostUpdChange>"
        ;
    // записать событие в БД
    attic::a_document srcDoc;
    CPPUNIT_ASSERT( srcDoc.load_wide( srcText ) );

    auto plukDB = GetPlukDataBase( LocalPostgresInstance::GetStringConnect() );
    CPPUNIT_ASSERT( plukDB->writeChanges( srcDoc.to_str() ) );


    // прочитать из базы обратно
    std::string userLayers[] = {
        "HappenLayer", "AsoupLayer", ScheduledLayer_xAttr, "AsoupQueue",
        "LimitLayer", "MarkLayer", "ForbidLayer", "FutureLayer" };
    std::string srcdbText = plukDB->getUserChart( "20200324T000000Z", "20200325T000000Z",
                                                  std::vector<std::string>( begin( userLayers ), end( userLayers ) )
    );
    attic::a_document srcdbDoc;
    CPPUNIT_ASSERT( srcdbDoc.load_utf8( srcdbText ) );
    attic::a_xpath_node_set srcQuery = srcdbDoc.select_nodes( "//SpotEvent" );
    auto it = srcQuery.begin();
    CPPUNIT_ASSERT( it != srcQuery.end() );
    const SpotEvent srcEvent1( it->node() );

    auto srcDet = srcEvent1.GetDetails();
    CPPUNIT_ASSERT( srcDet && srcDet->tdptr );
    CPPUNIT_ASSERT( srcDet->tdptr->get_loks_number( ',' ) == L"667" );
    CPPUNIT_ASSERT( srcDet->tdptr->ExistFeature( TrainCharacteristics::TrainFeature::LimitSpeed ) );
    CPPUNIT_ASSERT( L"22/11" == srcDet->tdptr->GetFeatureText(TrainCharacteristics::TrainFeature::LimitSpeed) );
}
