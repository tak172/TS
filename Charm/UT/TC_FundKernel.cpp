#include "stdafx.h"
#include <boost/filesystem/operations.hpp>
#include "TC_FundKernel.h"
#include "UtPlayerCreator.h"
#include "../helpful/Attic.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_FundKernel );

namespace bfs = boost::filesystem;
namespace bt = boost::posix_time;

void TC_FundKernel::setUp()
{
    kernel.reset( new FundKernel );

    CPPUNIT_ASSERT( !kernel->loading() );
    CPPUNIT_ASSERT( !kernel->running() );
    CPPUNIT_ASSERT( !kernel->stopping() );

    CPPUNIT_ASSERT( kernel );
}

void TC_FundKernel::tearDown()
{
    CPPUNIT_ASSERT( !kernel->loading() );
    CPPUNIT_ASSERT( !kernel->running() );
    CPPUNIT_ASSERT( !kernel->stopping() );
    kernel.reset();

    remove_fcfg_oec();
    TrainCharacteristics::Shutdowner();
}

void TC_FundKernel::start_stop()
{
    // подготовка данных
    create_fcfg_oec();

    boost::system::error_code ec;
    kernel->start( (bfs::temp_directory_path( ec ) / L"ut_FK.fcfg").c_str(), UtPlayerPanel::construct );

    CPPUNIT_ASSERT( wait_for( &FundKernel::running, true ) );
    kernel->stop();
    CPPUNIT_ASSERT( wait_for( &FundKernel::running, false ) );
    CPPUNIT_ASSERT( wait_for( &FundKernel::stopping, false ) );
    //
}


void TC_FundKernel::get_lts()
{
    // подготовка данных
    create_fcfg_oec();

    boost::system::error_code ec;
    kernel->start( ( bfs::temp_directory_path( ec ) / L"ut_FK.fcfg" ).c_str(), UtPlayerPanel::construct );
    CPPUNIT_ASSERT( wait_for( &FundKernel::running, true ) );
    // общий список
    std::vector<BadgeU> lts = kernel->lts_list();
    std::sort( lts.begin(), lts.end() );
    CPPUNIT_ASSERT( lts.size() == 9 );

    CPPUNIT_ASSERT( lts[ 0 ].to_wstring() == L"2п(30450)бит1<304>" );
    CPPUNIT_ASSERT( lts[ 1 ].to_wstring() == L"2п(30450)бит2<304>" );
    CPPUNIT_ASSERT( lts[ 2 ].to_wstring() == L"6(30450)бит1<304>" );
    CPPUNIT_ASSERT( lts[ 3 ].to_wstring() == L"6(30450)бит2<304>" );
    CPPUNIT_ASSERT( lts[ 4 ].to_wstring() == L"9(30720)бит1<6432>" );
    CPPUNIT_ASSERT( lts[ 5 ].to_wstring() == L"9(30720)бит2<6432>" );
    CPPUNIT_ASSERT( lts[ 6 ].to_wstring() == L"9сп(30720)бит1<6432>" );
    CPPUNIT_ASSERT( lts[ 7 ].to_wstring() == L"9сп(30720)бит2<6432>" );
    CPPUNIT_ASSERT( lts[ 8 ].to_wstring() == L"миг.9сп(30720)бит2<6432>" );

    // текущие значения
    std::map< BadgeU, StateLTS > temp;
    auto bdg_val = kernel->lts_values( lts, true );
    temp.clear();
    for( auto& b_v : bdg_val )
    {
        temp[ b_v.first ] = b_v.second;
        // полученное имя есть в исходном списке
        CPPUNIT_ASSERT( lts.end() != std::find( lts.begin(), lts.end(), b_v.first ) );
        CPPUNIT_ASSERT( b_v.second == StateLTS::UNDEF );
    }
    CPPUNIT_ASSERT( temp.size() == lts.size() );
    // внесение изменений и получение новых значений
    const BadgeU bdg( L"6(30450)бит1", 304 );
    FundKernel_If::DataU direct_lts_data;
    direct_lts_data.emplace_back( bdg, StateLTS::ACTIVE );
    kernel->manual_lts_set( direct_lts_data );
    while( kernel->input_unhandled() )
        boost::this_thread::sleep( bt::milliseconds(50) );
    const auto cng_val = kernel->lts_values( lts, false );
    CPPUNIT_ASSERT( cng_val.size() == 1 );
    CPPUNIT_ASSERT( cng_val[ 0 ].first == bdg );
    CPPUNIT_ASSERT( cng_val[ 0 ].second == StateLTS::ACTIVE );
    // опрос без изменений
    const auto cng_none = kernel->lts_values( lts, false );
    CPPUNIT_ASSERT( cng_none.empty());
    // получение всех
    const auto cng_all = kernel->lts_values( lts, true );
    CPPUNIT_ASSERT( cng_all.size() == lts.size() );
    //
    kernel->stop();
    CPPUNIT_ASSERT( wait_for( &FundKernel::running, false ) );
    CPPUNIT_ASSERT( wait_for( &FundKernel::stopping, false ) );
    //
}

void TC_FundKernel::create_fcfg_oec()
{
    std::wstring text_fcfg =
        L"<?xml version='1.0' encoding='utf-8' standalone='yes'?>"
        L"<FundConfig>"
        L"    <BlackBox Path='.' daysHold='90' />"
        L"    <Log Path='.'/>"
        L"    <Load filename='Унжа.oec'/>"
        L"    <Load filename='Корега.oec'/>"
        L"</FundConfig>";
    boost::system::error_code ec;
    auto tempDir = bfs::temp_directory_path( ec );
    attic::a_document fcfg;
    CPPUNIT_ASSERT( fcfg.load_wide( text_fcfg ) );
    CPPUNIT_ASSERT( fcfg.save_file( ( tempDir / L"ut_FK.fcfg" ).c_str() ) );

    std::wstring text_unja =
        L"<?xml version='1.0' encoding='UTF-8' standalone='yes'?>"
        L"<LOGIC_COLLECTION name='Унжа' ESR_code='30720' picketing1_comm='Москва' picketing1_val='656~' >"
        L"    <STRIPS>"
        L"        <element name='9сп:9+' type='0'>"
        L"            <link name='9' class='8' link_type='1' />"
        L"        </element>"
        L"        <element name='9сп' type='0' even_capacity='2' odd_capacity='2' length='60'>"
        L"            <link name='9' class='8' link_type='0' />"
        L"            <ts used='9сп(30720)бит2' ab='6432' />"
        L"            <ts lock='9сп(30720)бит1' ab='6432' />"
        L"            <ts art_used='миг.9сп(30720)бит2' ab='6432' />"
        L"        </element>"
        L"        <element name='9сп:9-' type='0'>"
        L"            <link name='9' class='8' link_type='2' />"
        L"        </element>"
        L"    </STRIPS>"
        L"    <SWITCHES>"
        L"        <element name='9' type='0'>"
        L"            <link name='9сп:9+' class='1' link_type='1' />"
        L"            <link name='9сп' class='1' link_type='0' />"
        L"            <link name='9сп:9-' class='1' link_type='2' />"
        L"            <ts positiv='9(30720)бит1' ab='6432' />"
        L"            <ts negativ='9(30720)бит2' ab='6432' />"
        L"        </element>"
        L"    </SWITCHES>"
        L"    <DigitalSignature>A4956D4E9084DE82A8C434C19E6AFC5346F8DC18BC3A414A7012FDDA9068DE0F203BCFF62EC762450F2954710D45105CDF5498B165C706E341EDE22885C8E381F9DFE79B6CA3BB7B0356F03216B8B39958EA17CA28BB8434A563F4BB10151FD8A1CE18A8AE9AB8BB68607353AB0A15C75645C14DD5633920C6C24ED9B39E562249087DA817989E958971914D53F120BF1326CF139CDAE56586169C687AD39A9B3475F9A530C99EB12084E000C7B5C12E0119C8D3C548CF76FA7205BA08AF1FF7</DigitalSignature>"
        L"</LOGIC_COLLECTION>"
        ;
    attic::a_document unja;
    CPPUNIT_ASSERT( unja.load_wide( text_unja ) );
    CPPUNIT_ASSERT( unja.save_file( ( tempDir / L"Унжа.oec" ).c_str() ) );
 
    std::wstring text_korega =
        L"<?xml version='1.0' encoding='UTF-8' standalone='yes'?>"
        L"<!--Файл описания логики-->"
        L"<LOGIC_COLLECTION name='Корега' ESR_code='30450' picketing1_comm='Москва' picketing1_val='440~200' >"
        L"    <STRIPS>"
        L"        <element name='2п' type='0' even_capacity='66' odd_capacity='66' length='958'>"
        L"            <link name='6' class='8' link_type='0' />"
        L"            <ts used='2п(30450)бит2' ab='304' />"
        L"            <ts lock='2п(30450)бит1' ab='304' />"
        L"        </element>"
        L"        <element name='2п:6+' type='0'>"
        L"            <link name='6' class='8' link_type='1' />"
        L"        </element>"
        L"        <element name='2п:6-' type='0'>"
        L"            <link name='6' class='8' link_type='2' />"
        L"        </element>"
        L"    </STRIPS>"
        L"    <SWITCHES>"
        L"        <element name='6' type='0'>"
        L"            <link name='2п' class='1' link_type='0' />"
        L"            <link name='2п:6+' class='1' link_type='1' />"
        L"            <link name='2п:6-' class='1' link_type='2' />"
        L"            <ts positiv='6(30450)бит1' ab='304' />"
        L"            <ts negativ='6(30450)бит2' ab='304' />"
        L"        </element>"
        L"    </SWITCHES>"
        L"<DigitalSignature>257F3E95BE874C61B34B9F5C038C396FEC3A1CCEA1312E142DCF3033C8769D5E0C222DC29A6517E7ACC0C73FAC92E2F5D1FF06DCAD75DAF9794045848CECAC418B4FC1C59C0851A1807E9A5733BD99743AF2CE4429741C4D4E59993E670BEE265A845B4DA8AC0D3169FF1D4639071146036A62F2FC3E20AD9977D0990AC678D44D2BDF3AB1F091649A52D96F18D80428E8F9CD573C03A89B8A258F75A743B5F7E45424509B5B910AEB067C80E2C1E60A6CAC42A02ED6E4D75F90E17D10DBB38B</DigitalSignature>"
        L"</LOGIC_COLLECTION>"
        ;

    attic::a_document korega;
    CPPUNIT_ASSERT( korega.load_wide( text_korega ) );
    CPPUNIT_ASSERT( korega.save_file( ( tempDir / L"Корега.oec" ).c_str() ) );
}

void TC_FundKernel::remove_fcfg_oec()
{
    boost::system::error_code ec;
    auto tempDir = bfs::temp_directory_path( ec );
    auto fcfg = tempDir / L"ut_FK.fcfg";
    CPPUNIT_ASSERT( !bfs::exists( fcfg, ec ) || bfs::remove( fcfg, ec ) );
    auto unja = tempDir / L"Унжа.oec";
    CPPUNIT_ASSERT( !bfs::exists( unja, ec ) || bfs::remove( unja, ec ) );
    auto korega = tempDir / L"Корега.oec";
    CPPUNIT_ASSERT( !bfs::exists( korega, ec ) || bfs::remove( korega, ec ) );
}

bool TC_FundKernel::wait_for( MEMFUNC func, bool required )
{
    const bt::time_duration durat = bt::seconds(20);
    const bt::time_duration step = bt::milliseconds( 50 );
    const bt::ptime bound = bt::microsec_clock::universal_time() + durat; // граница ожидания
    FundKernel& Kern = *kernel;
    bool success = false;
    while( !success && bt::microsec_clock::universal_time() < bound )
    {
        bool current = ( Kern.*func )( );
        success = ( current == required );
        if( !success )
            boost::this_thread::sleep( step );
    }
    return success;
}
