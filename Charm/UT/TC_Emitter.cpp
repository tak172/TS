#include "stdafx.h"

#include <boost/filesystem.hpp>
#include "TC_Emitter.h"
#include "UtPlayerCreator.h"
#include "../helpful/Attic.h"
#include "../helpful/FilesByMask.h"
#include "../helpful/Time_Iso.h"
#include "../helpful/Serialization.h"
#include "../helpful/LocalQpidPlukDB.h"
#include "../helpful/Amqp/Factory.h"
#include "../helpful/Amqp/Customer.h"
#include "../helpful/Amqp/Message.h"
#include "../BlackBox/bbx_BlackBox.h"
#include "../BlackBox/bbx_FileChain.h"
#include "../Fund/InBbxEmitter.h"
#include "../Fund/QueueWithMarker.h"
#include "../Hem/Filer.h"
#include "../Pluk/PlukCommandFactory.h"
#include "../Pluk/PlukConst.h"
#include "../Pluk/PlukMessage.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Emitter );

// ����� ����� - �� ������� �� hef ��� bbx
void TC_Emitter::testLiveEmitter()
{
    const time_t origin = time_from_iso("20200304T050607Z") - time_from_iso("20200304T050607Z") % 1000;
    time_t mark = origin;
    auto marker = [&mark](){
        return mark;
    };

    QueueWithMarker< LOS_AREA_TYPE, time_t > Harons_data(marker);  // ������� ������� � ��������� �������
    m_emitter.reset( new LiveEmitter( Harons_data ) );

    // �� ������ origin ������������ �����
    mark = origin;
    LOS_AREA_TYPE a;
    a.push_back('a');
    Harons_data.push( a );
    // �� ������ origin+3 ��� ������
    mark = origin + 3;
    LOS_AREA_TYPE b1;
    LOS_AREA_TYPE b2;
    b1.push_back('b');
    b1.push_back('1');
    Harons_data.push( b1 );
    b2.push_back('2');
    b2.push_back('b');
    Harons_data.push( b2 );

    // ������� ����������
    Bbx::Identifier bbxId;
    LOS_AREA_TYPE inLat;
    //
    // - ���������� ���� ������ ������� origin
    //
    CPPUNIT_ASSERT( origin == m_emitter->get_current() );   // ������� ����� ���� ������ ��� ��������
    CPPUNIT_ASSERT( !m_emitter->current_finished() );           // ������ origin �� �������� ���������
    CPPUNIT_ASSERT( m_emitter->retrieve_for_current( bbxId, inLat ) ); // ����� a �����������
    CPPUNIT_ASSERT( a == inLat );
    CPPUNIT_ASSERT( !m_emitter->retrieve_for_current( bbxId, inLat ) ); // ������ ������ �� �������
    CPPUNIT_ASSERT( m_emitter->current_finished() );      // ��������� ������� origin ���������

    //
    // - ���������� ���� ������ ������� origin+3
    //
    mark = origin + 9;

    CPPUNIT_ASSERT( m_emitter->move_current() );
    CPPUNIT_ASSERT_EQUAL( origin+1, m_emitter->get_current() );
    CPPUNIT_ASSERT( m_emitter->current_finished() );      

    CPPUNIT_ASSERT( m_emitter->move_current() );
    CPPUNIT_ASSERT_EQUAL( origin+2, m_emitter->get_current() );
    CPPUNIT_ASSERT( m_emitter->current_finished() );      

    CPPUNIT_ASSERT( m_emitter->move_current() );
    CPPUNIT_ASSERT_EQUAL( origin+3, m_emitter->get_current() );
    CPPUNIT_ASSERT( ! m_emitter->current_finished() );      

    CPPUNIT_ASSERT( origin+3 == m_emitter->get_current() );   // ������� ����� ���� ������ ��� ��������
    CPPUNIT_ASSERT( m_emitter->retrieve_for_current( bbxId, inLat ) ); // ����� a �����������
    CPPUNIT_ASSERT( b1 == inLat );
    CPPUNIT_ASSERT( !m_emitter->current_finished() );      // ��������� ������� origin ���������

    CPPUNIT_ASSERT( m_emitter->retrieve_for_current( bbxId, inLat ) ); // ����� a �����������
    CPPUNIT_ASSERT( b2 == inLat );
    CPPUNIT_ASSERT( m_emitter->current_finished() );      // ��������� ������� origin ���������

    //
    // - ��������� �� �������� ������� origin+9 ��� ��� ������ ������
    //
    for( int i=3; i<9; ++i )
    {
        CPPUNIT_ASSERT( m_emitter->current_finished() );      // ��������� ������� origin ���������
        CPPUNIT_ASSERT( m_emitter->move_current() );
    }
    CPPUNIT_ASSERT( !m_emitter->current_finished() );      // ��������� ������� origin ���������
}

// ��������� bbx � ������� ������� - ���� ��� ������� bbx
void TC_Emitter::testTimeMachineEmitter()
{
    writeFileBbx( time_from_iso( "20180101T010101Z" ), time_from_iso( "20180202T020202Z" ) );
    m_emitter.reset( new TimeMachineEmitter( *m_bbxLocation, UtPlayerPanel::construct ) );
    CPPUNIT_ASSERT( m_emitter->get_current() == time_from_iso( "20180101T010101Z" ) );
}

// ��������� bbx � ������� ������� - ���� ��� ���������� bbx
void TC_Emitter::testTimeMachineEmitter_noBbx()
{
    CPPUNIT_ASSERT_THROW( m_emitter.reset( new TimeMachineEmitter( *m_bbxLocation, UtPlayerPanel::construct ) ),
        std::invalid_argument );
}

// ��������� bbx �� ������������ �������� - ���� ��� ������� bbx
void TC_Emitter::testPlainEmitter()
{
    writeFileBbx( time_from_iso( "20180101T111111Z" ), time_from_iso( "20180202T222222Z" ) );
    m_emitter.reset( new PlainEmitter( *m_bbxLocation ) );
    std::string ss = time_to_iso( m_emitter->get_current() );
    CPPUNIT_ASSERT( ss == "20180101T111111Z" );
}

// ��������� bbx �� ������������ �������� - ���� ��� bbx
void TC_Emitter::testPlainEmitter_noBbx()
{
    CPPUNIT_ASSERT_THROW( m_emitter.reset( new PlainEmitter( *m_bbxLocation ) ),
        std::invalid_argument );
}

// ������������� ������ - ���� � ��������; ������� bbx �� �����������
void TC_Emitter::testFreezeHefEmitter()
{
    auto orig = "20180102T152030Z";
    auto myTZ = "NNN+09DDD+08,M3.5.0/03:00,M10.5.0/04:00";
    writeFileHef( time_from_iso( orig ), myTZ );
    m_emitter.reset( new Freeze_Emitter( m_folder ) );
    std::string ss = time_to_iso( m_emitter->get_current() );
    CPPUNIT_ASSERT( ss == orig );
    std::string tz = m_emitter->getTimeZone();
    CPPUNIT_ASSERT( tz == myTZ );
}

// ������������� ������ - ���� ��� �������; ������� bbx �� �����������
void TC_Emitter::testFreezeHefEmitter_noData()
{
    CPPUNIT_ASSERT( 0 == m_filer->count() );
    CPPUNIT_ASSERT_THROW( m_emitter.reset( new Freeze_Emitter( m_folder ) ), std::invalid_argument );
}

// ������������� ������ - ���� � ����� ������; ������� bbx �� �����������
void TC_Emitter::testFreezePlukEmitter()
{
    LocalQpidPlukDB::Start();

    auto orig = "20180102T152030Z";
    auto myTZ = "NNN+09DDD+08,M3.5.0/03:00,M10.5.0/04:00";
    writePluk( time_from_iso( orig ), myTZ );
    m_emitter.reset( new Freeze_Emitter( LocalQpidPlukDB::GetQpidConnection(), myTZ ) );
    std::string ss = time_to_iso( m_emitter->get_current() );
    CPPUNIT_ASSERT( ss == orig );
    std::string tz = m_emitter->getTimeZone();
    CPPUNIT_ASSERT( tz == myTZ );

    LocalQpidPlukDB::Stop();
}

// ������������� ������ - ���� ��� ���������� ���� ������; ������� bbx �� �����������
void TC_Emitter::testFreezePlukEmitter_noData()
{
    std::string pluk_host_and_port = "";
    CPPUNIT_ASSERT_THROW( m_emitter.reset( new Freeze_Emitter( pluk_host_and_port, "" ) ), std::invalid_argument );
}

void TC_Emitter::testRdcSyncEmitter()
{
    //<BlackBox Path = ".\bbx" daysHold = "30" RdcSync = "20230313T050000Z+03" / >

    QueueWithMarker< LOS_AREA_TYPE, time_t > Harons_data( []() { return time_from_iso( "20200101T010101Z" ); } );

    m_emitter.reset( new RdcSyncEmitter( "20230313T050000Z+03", Harons_data ) );
    std::string ss = time_to_iso( m_emitter->get_current() );
    CPPUNIT_ASSERT( ss == "20230313T050000Z" );    // ����� �� ������������� (�� �� ������� ������� ������)
    CPPUNIT_ASSERT( !m_emitter->from_file() );     // �������� �� ����, � ����� ������
    CPPUNIT_ASSERT( m_emitter->rewrite_outbbx() ); // ��� ����� ���� ��������������
    CPPUNIT_ASSERT( m_emitter->rewrite_outhef() );
}

void TC_Emitter::testRdcSyncEmitter_noData()
{
    QueueWithMarker< LOS_AREA_TYPE, time_t > Harons_data( []() { return time_from_iso( "20200101T010101Z" ); } );
    CPPUNIT_ASSERT_THROW( m_emitter.reset( new RdcSyncEmitter( "", Harons_data ) ),
        std::invalid_argument );
}

// ��������������� �������
void TC_Emitter::writePluk( time_t moment, std::string tzone  )
{
    const std::string& text = time_to_iso( moment );

    attic::a_document doc;
    doc.load_utf8(
        "<SoxPostUpdChange increment='Y' presently='" + text + "'>"
        "  <AutoChanges>"
        "    <HappenLayer>"
        "      <Trio>"
        "        <Body create_time='" + text + "' name='Form' Bdg='xyz[09000:09420]' waynum='1'/>"
        "        <Body create_time='" + text + "' name='Span_move' Bdg='qwe[09000:09420]' waynum='1'/>"
        "      </Trio>"
        "    </HappenLayer>"
        "  </AutoChanges>"
        "  <UserChanges>"
        "    <HappenLayer>"
        "      <Trio>"
        "        <Body create_time='" + text + "' name='Form' Bdg='xyz[09000:09420]' waynum='1'/>"
        "        <Body create_time='" + text + "' name='Span_move' Bdg='qwe[09000:09420]' waynum='1'/>"
        "      </Trio>"
        "    </HappenLayer>"
        "  </UserChanges>"
        "</SoxPostUpdChange>"
        );

    Network::Factory netFactory;
    auto plukConnection = netFactory.createConnection( LocalQpidPlukDB::GetQpidConnection() );
    auto plukWriter = std::make_shared<Network::Customer>( plukConnection, Network::Seconds( 7 ) );

    PlukCommandFactory cmdF;
    std::string request = serialize<PlukMessage>( cmdF.WriteChange( doc.to_str() ) );

    if ( std::shared_ptr<Network::Message> response = plukWriter->request( PlukConst::PlukAddress, request ) )
    {
        auto msg = deserialize<PlukMessage>( response->getContent() );
        const std::string& body = msg->getBody();
        CPPUNIT_ASSERT( !body.empty() );
    }
}

void TC_Emitter::writeFileHef( time_t moment, std::string tzone  )
{
    const std::string& text = time_to_iso( moment );
    attic::a_document doc;
    doc.load_utf8(
        "<Backup producer='test' tz='" + tzone + "'>"
        "  <AutoChart> <HappenLayer>"
        "      <HemPath>"
        "        <SpotEvent create_time='" + text + "' name='Form'      Bdg='abc[09400:09410]' waynum='1' />"
        "        <SpotEvent create_time='" + text + "' name='Span_move' Bdg='def[09400:09410]' waynum='1' />"
        "      </HemPath>"
        "  </HappenLayer> </AutoChart>"
        "</Backup>"
        );
    m_filer->initCurrentShift( moment );
    m_filer->write( moment, doc );
}

void TC_Emitter::writeFileBbx( time_t momentFrom, time_t momentTo )
{
    auto bOut = Bbx::Writer::create( *m_bbxLocation );
    auto defaultId = Bbx::Identifier(Bbx::Identifier::Source::HaronOutput);

    CPPUNIT_ASSERT( bOut->pushReference( std::string("1"), std::string( "from record" ), momentFrom, defaultId) );
    CPPUNIT_ASSERT( bOut->pushReference( std::string("2"), std::string( "to record" ),   momentTo,   defaultId) );
}

void TC_Emitter::setUp()
{
    m_folder = (boost::filesystem::temp_directory_path() / L"x").wstring();
    m_folder.pop_back(); // �� ����� ������ ���������� �����������
    m_filer.reset( new NsCanopy::Filer( m_folder, 10*365 ) ); // ������������ �������� (10 ���)
    m_bbxLocation.reset( new Bbx::Location( m_folder, L"emitPfx-", L".emitSfx" ) );
    Bbx::FileChain fc = *m_bbxLocation->getCPtrChain();
    for( auto my_f = fc.takeEarliestFile();
        !my_f.empty();
        my_f = fc.takeEarliestFile()
        )
    {
        if ( DeleteFile( my_f.c_str() ) )
            ;//OutputDebugString( L" deleted\n" );
        else
            OutputDebugString( (std::wstring(L" NOT deleted error=")+std::to_wstring(GetLastError())+L"\n" ).c_str() );
    }
}

void TC_Emitter::tearDown()
{
    auto remove = [this]( const FilesByMask_Data& fd ){
        CPPUNIT_ASSERT( DeleteFile( (m_folder+fd.fname.to_string()).c_str() ) );
        return true;
    };
    FilesByMask( m_filer->filemask(), remove );
    m_emitter.reset();
    m_bbxLocation.reset();
    m_filer.reset();
    m_folder.clear();
}
