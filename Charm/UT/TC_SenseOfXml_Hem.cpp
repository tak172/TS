#include "stdafx.h"

#include <boost/filesystem.hpp>
#include "../helpful/Attic.h"
#include "../helpful/DistrictGuide.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/HemTabInfo.h"
#include "../helpful/StrToTime.h"
#include "../helpful/TopologyDecl.h"
#include "../Permission/PermissionDesignated.h"
#include "../Hem/Carrier.h"
#include "../Hem/TheCharts.h"
#include "../Hem/TheChartsIo.h"
#include "../Hem/UncontrolledStations.h"
#include "../Hem/Platforms.h"
#include "../Hem/Filer.h"
#include "../Hem/Appliers/aeLibrary.h"
#include "../SenseOfXml/SenseOfXml.h"
#include "../SenseOfXml/SenseOfXml_Hem.h"
#include "MockFiler.h"
#include "TC_SenseOfXml_Hem.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SenseOfXml_Hem );

void TC_SenseOfXml_Hem::check_GetAll()
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

    TheCharts thecharts( fileStore, tempPusher, tempPostback );
    thecharts.initializeCurrentTime( time_from_iso( "20170929T130000Z" ) );
    Outlet outlet;
    SenseOfXml::HandlerChain chain;
    OutletConcrete outconcrete(outlet,Goal::Canopy);
    chain.attach( new GetAll("iam"), new HemHandler_GetAll( thecharts, outconcrete ) );

    attic::a_document doc;
    std::string before = doc.to_str();
    // изначально пусто
    CPPUNIT_ASSERT( !chain.executable(doc) );
    CPPUNIT_ASSERT( outlet.empty() );
    // впервые запрашиваем кусок графика для клиента
    GetAll ga("iam");
    ga.setBorder( interval_time_t(time_from_iso( "20170929T110000Z" ), time_from_iso( "20170929T130000Z" )) );
    ga.setUserIp( "192.168.160.186" );
    CPPUNIT_ASSERT( chain.executable( ga.doc() ) );
    chain.execute( ga.doc() );
    // при первом подключении клиента выдается три сообщения
    // - очистка графика
    // - весь график PostSlice 
    // - PostDemandResponse (uncontrolled stations)
    CPPUNIT_ASSERT( outlet.pop_front() > 0 );
    CPPUNIT_ASSERT( outlet.pop_front() > 0 );
    CPPUNIT_ASSERT( outlet.pop_front() > 0 );
    CPPUNIT_ASSERT( outlet.empty() );
    // запрашиваем другой кусок графика для того же клиента
    GetAll ga2("iam");
    ga2.setBorder( interval_time_t(time_from_iso( "20170808T110000Z" ), time_from_iso( "20170808T130000Z" )) );
    ga.setUserIp( "192.168.160.186" );
    CPPUNIT_ASSERT( chain.executable( ga2.doc() ) );
    chain.execute( ga2.doc() );
    // при переключении клиента выдается два сообщения
    // - очистка графика
    // - весь график PostSlice 
    CPPUNIT_ASSERT( outlet.pop_front() > 0 );
    CPPUNIT_ASSERT( outlet.pop_front() > 0 );
    CPPUNIT_ASSERT( outlet.empty() );
}

void TC_SenseOfXml_Hem::check_PostInitialInfo()
{
    EsrGuide esrGuideIn;
    DistrictGuide districtGuideIn;
    UncontrolledStations uncontrolledIn;
    Platforms platformsIn;
    Permission::Designated designatedPermission;
    HemTabInfo hemtab;
    {
        std::wstring temp =
            L"<HemTab filename='ГИД и график АСОУП' zorder='1' cx='113' cy='0' dx='1258' dy='896' bframe='2'>"        
            L"    <HemWin filename='111'  district_code='01111-01111' tabname='111'>"
            L"        <Station ESR='09180' name='Елгава' />"
            L"    </HemWin>"
            L"    <HemWin filename='Jelgava-Ventspils'  district_code='09180-09820' tabname='Jelgava-Ventspils'>"
            L"        <Station ESR='09180' name='Елгава' />"
            L"    </HemWin>"
            L"</HemTab>"
            ;
        hemtab.deserialize( ToUtf8(temp) );
    }
    
    const bool PLAIN_HEM = true;

    PostInitialInfo pii;
    esrGuideIn.save_to( pii.nodeEsrGuide() );
    districtGuideIn.serialize( pii.nodeDistrictGuide() );
    uncontrolledIn.serialize( pii.nodeUncontrolled() );
    platformsIn.serialize( pii.nodePlatforms() );
    pii.nodeHemTab().append_buffer( hemtab.serialize() );

    std::wstring topologyFilePath;
    std::wstring topologyHelperFilePath;
    std::wstring topologyBlacklistFilePath;
    std::wstring regulatoryScheduleFilePath;

    const std::wstring WHERE = boost::filesystem::temp_directory_path().wstring();
    makeFiles( WHERE,
        topologyFilePath,
        topologyHelperFilePath,
        topologyBlacklistFilePath,
        regulatoryScheduleFilePath
        );
    {
        std::wostringstream logString;
        std::string serialization = Nsi::serializeTopology(topologyFilePath, topologyHelperFilePath, topologyBlacklistFilePath, logString );
        attic::a_document tempDoc;
        if (attic::a_parse_result parse = tempDoc.load_utf8(serialization))
        {
            attic::a_node ntopo = pii.nodeTopology();
            for( attic::a_node ch : tempDoc.document_element().children() )
                ntopo.append_copy( ch );
        }
    }
    {
        attic::a_node nreg = pii.nodeRegulatory();
        CPPUNIT_ASSERT( !regulatoryScheduleFilePath.empty() );
        // WriteLog( L"Загрузка графика нормативного расписания из '%s'", config->regulatoryScheduleFilePath );
        attic::a_document doc;
        CPPUNIT_ASSERT( doc.load_file( regulatoryScheduleFilePath ) );
        for( attic::a_node r : doc.document_element().children() )
            nreg.append_copy( r );
    }

    {
        SenseOfXml::Command cmd ( pii.doc() );
        CPPUNIT_ASSERT( !PostInitialInfo::getHemRdonly(cmd) );
    }
    pii.setHemRdonly( PLAIN_HEM );
    {
        SenseOfXml::Command cmd ( pii.doc() );
        CPPUNIT_ASSERT( PLAIN_HEM == PostInitialInfo::getHemRdonly(cmd) );
    }
    pii.issued( time_from_iso("20180501T123456Z") );

    // проверка исполнения
    TheCharts thecharts;
    MockFiler filer;
    bool plainHefMode = false;
    // создать chain ПОСЛЕ filer т.к. chain сохраняет ссылку на filer
    SenseOfXml::HandlerChain chain;
    chain.attach( new PostInitialInfo(), new HemHandler_InitInfo( thecharts, filer, &plainHefMode ) );
    CPPUNIT_ASSERT( chain.executable( pii.doc() ) );
    chain.execute( pii.doc() );
    CPPUNIT_ASSERT( plainHefMode );
}

void TC_SenseOfXml_Hem::makeFiles( const std::wstring& WHERE,
                                  std::wstring& topologyFilePath,
                                  std::wstring& topologyHelperFilePath,
                                  std::wstring& topologyBlacklistFilePath,
                                  std::wstring& regulatoryScheduleFilePath
                                  )
{
    auto makeFile = []( const std::wstring fname, const std::string fdata ){
        std::ofstream of( fname.c_str(), std::ios::trunc | std::ios::out);
        CPPUNIT_ASSERT( of.is_open() );
        of << fdata;
        of.close();
        CPPUNIT_ASSERT( !of.fail() );
    };

    topologyFilePath           = WHERE + L"utData_Topology.stn";
    topologyHelperFilePath     = WHERE + L"utData_Direct_ASOUP.stn";
    topologyBlacklistFilePath  = WHERE + L"utData_TopologyBlacklist.stn";
    regulatoryScheduleFilePath = WHERE + L"utData_schedule.xml";

    makeFile( topologyFilePath,
        "=09500,09041,09050 \n"
        "=09041,09050,00000 \n"
        "*09041,09050 {09041,09051,09050} \n"
        );
    makeFile( topologyHelperFilePath, 
        "11760-11420\n"
        "\n"
        "09000-09420..09360-11760\n"
        "09000-09420..09370-09360\n"
        "09000-09420..09380-09370\n"
        "09000-09420..09400-09380\n"
        "09000-09420..09410-09400\n"
        "09000-09420-09410\n"
        "09000-09420\n"
        );
    makeFile( topologyBlacklistFilePath, 
        "09670..09750..09790\n"
        "09790..09750..09670\n"
        );
    makeFile( regulatoryScheduleFilePath, 
        "<LoadRegulatorySchedule>"
        " <RegulatorySchedule startTime='20180325T010032Z'>"
        "  <Thread train='1'>"
        "   <Event esr='06610' type='departure' time='041500' />"
        "   <Event esr='06612' type='transition' time='043000' />"
        "   <Event esr='11290' type='arrival' time='044200' />"
        "  </Thread>"
        "  <Thread train='2'>"
        "   <Event esr='09010' type='departure' time='173000' />"
        "   <Event esr='09006' type='transition' time='173900' />"
        "   <Event esr='06610' type='arrival' time='230500' />"
        "  </Thread>"
        " </RegulatorySchedule>"
        "</LoadRegulatorySchedule>"
        );
}
