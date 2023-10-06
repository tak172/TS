#include "stdafx.h"
#include <io.h>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include "../helpful/LogStream.h"
#include "../helpful/LogExtensions.h"
#include "../helpful/FilesByMask.h"
#include "../helpful/Time_Iso.h"
#include "../Guess/GLog.h"
#include "TC_GLog.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GLog );

namespace bt = boost::posix_time;
namespace bfs = boost::filesystem;

const std::wstring  moment = L"20160608T225102Z";

void TC_GLog::setUp()
{
    bfs::path p = bfs::temp_directory_path();
    std::wstring mask = (p/(moment+L"*.*")).c_str();
    auto remover = [p]( const FilesByMask_Data& res ){
        boost::system::error_code ec;
        bfs::remove( p/res.fname.to_string(), ec );
        return true;
    };
    FilesByMask( mask, remover );
}

static void normal( const std::wstring& _sect, time_t _mom, const std::wstring& _folder )
{
    // запись лога без крешей
    GLogStream file_streamer( _sect, _mom, _folder, false );
    GLogStream::log() << "normal";
}

static void manual( const std::wstring& _sect, time_t _mom, const std::wstring& _folder )
{
    // запись лога с принудительной установкой флага креша
    GLogStream file_streamer( _sect, _mom, _folder, false );
    GLogStream::log()       << "nor";
    file_streamer.SetCrush();
    GLogStream::log()       <<    "mal";
}

static void catched( const std::wstring& _sect, time_t _mom, const std::wstring& _folder )
{
    // запись лога с перехватом исключения
    GLogStream file_streamer( _sect, _mom, _folder, false );
    try
    {
        GLogStream::log()       << "throw exception";
        throw std::wstring( L"выбросить исключение\n" );
    }
    catch (...)
    {
        file_streamer.SetCrush();
        GLogStream::log() << "перехват";
    }
}

void TC_GLog::test()
{
    bfs::path p = bfs::temp_directory_path();
    std::wstring par_logfolder = (p/L"x").wstring();
    par_logfolder.pop_back(); // уже с разделителем на конце строки

    time_t t = time_from_iso( ToUtf8( moment ) );
    std::wstring pfx = ( p/moment ).c_str();
    auto found = [pfx]( const wchar_t* _sect ){
        std::wstring name = pfx + L"_" + _sect + GLogStream::extensions().crush;
        return ( _waccess( name.c_str(), 0 ) == 0 );
    };

    // проверим создание креша (принудительно)
    std::thread th_n( normal,  L"n", t, par_logfolder );
    std::thread th_xm( manual,  L"x", t, par_logfolder );
    std::thread th_xc( catched, L"x", t, par_logfolder );
    std::thread th_xx( catched, L"x", t, par_logfolder );

    th_n.join();
    th_xm.join();
    th_xc.join();
    th_xx.join();

    // нормальный лог не создает креша
    CPPUNIT_ASSERT( !found(L"n") ); 
    // несколько крешей С ОДНИМ ВРЕМЕНЕМ И СЕКЦИЕЙ дают раздельные файлы
    CPPUNIT_ASSERT( found(L"x") );
    CPPUNIT_ASSERT( found(L"x2") );
    CPPUNIT_ASSERT( found(L"x3") );
    CPPUNIT_ASSERT( !found(L"x4") ); // лишнего не должно быть
}
