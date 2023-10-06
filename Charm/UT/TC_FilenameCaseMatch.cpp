#include "stdafx.h"
// #include <io.h>
// #include <thread>
#include <boost/filesystem.hpp>
// #include <boost/date_time.hpp>
// #include "../helpful/LogStream.h"
// #include "../helpful/LogExtensions.h"
#include "../helpful/FilesByMask.h"
#include "../helpful/FilenameCaseMatch.h"
// #include "../helpful/Time_Iso.h"
// #include "../Guess/GLog.h"
#include "TC_FilenameCaseMatch.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_FilenameCaseMatch );

namespace bfs = boost::filesystem;

static void clear_all()
{
    bfs::path p = bfs::temp_directory_path();
    std::wstring mask = ( p / L"*.?c_tmp" ).c_str();
    auto remover = [p]( const FilesByMask_Data& res ) {
        boost::system::error_code ec;
        CPPUNIT_ASSERT( bfs::remove( p / res.fname.to_string(), ec ) && !ec );
        return true;
    };
    FilesByMask( mask, remover );
}

void TC_FilenameCaseMatch::setUp()
{
    clear_all();
}

void TC_FilenameCaseMatch::tearDown()
{
    clear_all();
}

void TC_FilenameCaseMatch::equal()
{
    bfs::path p = bfs::temp_directory_path()/L"Alfa.tc_tmp";
    std::ofstream f;
    f.open( p.c_str(), std::ios::trunc | std::ios::out );
    f << "text";
    CPPUNIT_ASSERT( f );
    f.close();

    auto found = FilenameDifferentCase( p.generic_wstring() );
    CPPUNIT_ASSERT( found.empty() );
}

void TC_FilenameCaseMatch::diff()
{
    bfs::path fact = bfs::temp_directory_path() / L"beTa.tc_tmp";
    bfs::path used = bfs::temp_directory_path() / L"beta.tc_tmp";
    const std::wstring SHOW =                     L"--^";
    std::ofstream f;
    f.open( fact.c_str(), std::ios::trunc | std::ios::out );
    f << "text";
    CPPUNIT_ASSERT( f );
    f.close();

    auto found = FilenameDifferentCase( used.generic_wstring() );
    CPPUNIT_ASSERT( found == fact.filename().wstring() );
    auto diff = FilenameDifferences( used.generic_wstring(), found );
    CPPUNIT_ASSERT( diff == SHOW );
}

void TC_FilenameCaseMatch::diffN()
{
    bfs::path fact = bfs::temp_directory_path() / L"GamMa-Delta.Tc_tmp";
    bfs::path used = bfs::temp_directory_path() / L"gamma-delta.tc_tmp";
    const std::wstring SHOW =                     L"^--^--^-----^";
    std::ofstream f;
    f.open( fact.c_str(), std::ios::trunc | std::ios::out );
    f << "text";
    CPPUNIT_ASSERT( f );
    f.close();

    auto found = FilenameDifferentCase( used.generic_wstring() );
    CPPUNIT_ASSERT( found == fact.filename().wstring() );
    auto diff = FilenameDifferences( used.generic_wstring(), found );
    CPPUNIT_ASSERT( diff == SHOW );
}

void TC_FilenameCaseMatch::none()
{
    bfs::path fact = bfs::temp_directory_path() / L"xxx.tc_tmp";
    bfs::path used = bfs::temp_directory_path() / L"MIX.tc_tmp";
    std::ofstream f;
    f.open( fact.c_str(), std::ios::trunc | std::ios::out );
    f << "text";
    CPPUNIT_ASSERT( f );
    f.close();

    auto found = FilenameDifferentCase( used.generic_wstring() );
    CPPUNIT_ASSERT( found.empty() );
}
